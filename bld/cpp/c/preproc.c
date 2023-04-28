/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  A simple macro preprocessor.
*
****************************************************************************/


#include "_preproc.h"
#include <ctype.h>
#include <time.h>
#include "wio.h"
#include "watcom.h"
#include "iopath.h"
#include "pathlist.h"
#include "pathgrp2.h"
#include "ppfmttm.h"

#include "clibext.h"


#define DOS_EOF_CHAR    0x1A

#if defined( __UNIX__ )
#define H_DIR   "../h/"
#else
#define H_DIR   "..\\h\\"
#endif

typedef struct cpp_info {
    struct cpp_info *prev_cpp;
    unsigned char   cpp_type;
    bool            processing;
} CPP_INFO;

enum cpp_types {
    PP_IF,
    PP_ELIF,
    PP_ELSE
};

FILELIST            *PP_File = NULL;
unsigned            PPLineNumber;                   // current line number
pp_flags            PPFlags;                        // pre-processor flags
char                PP__DATE__[14];                 // value for __DATE__ macro
char                PP__TIME__[11];                 // value for __TIME__ macro
const char          *PPNextTokenPtr;                // next character after token end pointer
const char          *PPTokenPtr;                    // pointer to next char in token
MACRO_TOKEN         *PPTokenList;                   // pointer to list of tokens
MACRO_TOKEN         *PPCurToken;                    // pointer to current token
char                PPSavedChar;                    // saved char at end of token

void                (* PPErrorCallback)( const char * ) = NULL;

static pp_callback  *PP_CallBack;                   // mkmk dependency callback function

static CPP_INFO     *PPStack;
static int          NestLevel;
static int          SkipLevel;

static MACRO_ENTRY  *PPHashTable[HASH_SIZE];

static char         PPPreProcChar;                  // preprocessor line intro

static const char   *PPBufPtr;                      // block buffer pointer
static char         *PPLineBuf;                     // line buffer
static size_t       PPLineBufSize;                  // line buffer size
static unsigned     PPErrorCount = 0;

static char         *IncludePath1 = NULL;           // include path from cmdl
static char         *IncludePath2 = NULL;           // include path from env

static char         *macro_buf = NULL;
static size_t       macro_buf_size = 0;

static char *str_dup( const char *str )
{
    size_t  len;
    char    *p;

    len = strlen( str ) + 1;
    p = PP_Malloc( len );
    if( p != NULL )
        memcpy( p, str, len );
    return( p );
}

static FILE *PP_Open( const char *filename )
{
    FILE        *handle;
    FILELIST    *prev_file;

    handle = fopen( filename, "rb" );
    if( handle != NULL ) {
        prev_file = PP_File;
        PP_File = (FILELIST *)PP_Malloc( sizeof( FILELIST ) );
        if( PP_File == NULL ) {
            fclose( handle );
            handle = NULL;
            PP_File = prev_file;
            PP_OutOfMemory();
        } else {
            PP_File->prev_file = prev_file;
            PP_File->handle    = handle;
            PP_File->prev_bufptr = PPBufPtr;
            PP_File->filename  = str_dup( filename );
            PP_File->linenum   = 1;
            PPBufPtr = PP_File->buffer;
            PP_File->buffer[0] = '\0';      // indicate buffer empty
        }
    }
    return( handle );
}

static FILELIST *PP_Close( void )
{
    FILELIST    *this_file;

    this_file = PP_File;
    fclose( this_file->handle );
    PP_File = this_file->prev_file;
    PPBufPtr = this_file->prev_bufptr;
    PP_Free( this_file->filename );
    PP_Free( this_file );
    return( PP_File );
}

static char *AddIncludePath( char *old_list, const char *path_list )
{
    size_t  len;
    size_t  old_len;
    char    *new_list;
    char    *p;

    new_list = old_list;
    if( path_list != NULL && *path_list != '\0' ) {
        len = strlen( path_list );
        if( old_list == NULL ) {
            p = new_list = PP_Malloc( len + 1 );
        } else {
            old_len = strlen( old_list );
            new_list = PP_Malloc( old_len + 1 + len + 1 );
            strcpy( new_list, old_list );
            PP_Free( old_list );
            p = new_list + old_len;
        }
        while( *path_list != '\0' ) {
            if( p != new_list )
                *p++ = PATH_LIST_SEP;
            path_list = GetPathElement( path_list, NULL, &p );
        }
        *p = '\0';
    }
    return( new_list );
}

void PPENTRY PP_IncludePathAdd( const char *path_list )
{
    IncludePath1 = AddIncludePath( IncludePath1, path_list );
}

void PPENTRY PP_IncludePathInit( void )
{
    IncludePath1 = PP_Malloc( 1 );
    *IncludePath1 = '\0';
}

void PPENTRY PP_IncludePathFini( void )
{
    PP_Free( IncludePath1 );
}

static int checkfullpath( const char *filename, char *fullfilename )
{
    _fullpath( fullfilename, filename, _MAX_PATH );
    return( access( fullfilename, R_OK ) );
}

static const char *get_parent_filename( void **cookie )
{
    FILELIST    **last;

    if( cookie != NULL ) {
        last = (FILELIST **)cookie;
        if( *last != NULL ) {
            *last = (*last)->prev_file;
        } else {
            *last = PP_File;
        }
        if( *last != NULL ) {
            return( (*last)->filename );
        }
    }
    return( NULL );
}

static int findInclude( const char *path, const char *filename, size_t len, char *fullfilename )
{
    char        *p;
    char        c;
    char        fname[_MAX_PATH];

    while( (c = *path) != '\0' ) {
        p = fname;
        do {
            ++path;
            if( IS_PATH_LIST_SEP( c ) ) {
                break;
            }
            *p++ = c;
        } while( (c = *path) != '\0' );
        c = p[-1];
        if( !IS_PATH_SEP( c ) ) {
            *p++ = DIR_SEP;
        }
        memcpy( p, filename, len );
        p[len] = '\0';
        if( checkfullpath( fname, fullfilename ) == 0 ) {
            return( 0 );
        }
    }
    return( -1 );
}

int PPENTRY PP_IncludePathFind( const char *filename, size_t len, char *fullfilename, incl_type incltype, pp_parent_func fn )
/****************************************************************************************************************************
 * Include search order is intended to be compatible with C/C++ compilers and is as follows:
 *
 * 1) For absolute pathnames, try only that pathname and nothing else
 *
 * 2) For includes in double quotes only, search current directory
 *
 * 3) For includes in double quotes only, search the directory of including file
 *
 * 4) Search include directories specified by IncludePath1 (usually command line -I argument(s))
 *
 * 5) Search include directories specified by IncludePath2 (usualy INCLUDE path)
 *
 * 6) Directory 'h' adjacent to current directory (../h)
 *
 * Note that some of these steps will be skipped if PPFLAG_IGNORE_CWD and/or PPFLAG_IGNORE_INCLUDE is set.
 */
{
    int         rc = -1;
    char        fname[_MAX_PATH];

    memcpy( fname, filename, len );
    fname[len] = '\0';
    if( HAS_PATH( fname ) ) {
        /* rule 1 */
        rc = checkfullpath( fname, fullfilename );
    } else {
        /* rule 2 */
        if( rc == -1 && incltype != PPINCLUDE_SYS && (PPFlags & PPFLAG_IGNORE_CWD) == 0 ) {
            rc = checkfullpath( fname, fullfilename );
        }
        /* rule 3 */
        if( rc == -1 && incltype == PPINCLUDE_USR && fn != NULL ) {
            pgroup2     pg;
            size_t      len1;
            void        *cookie = NULL;
            const char  *p;

            while( rc == -1 && (p = fn( &cookie )) != NULL ) {
                _splitpath2( p, pg.buffer, &pg.drive, &pg.dir, NULL, NULL );
                _makepath( fname, pg.drive, pg.dir, NULL, NULL );
                len1 = strlen( fname );
                if( len1 > 0 ) {
                    char c = fname[len1 - 1];
                    if( !IS_PATH_SEP( c ) ) {
                        fname[len1++] = DIR_SEP;
                    }
                }
                memcpy( fname + len1, filename, len );
                fname[len1 + len] = '\0';
                rc = checkfullpath( fname, fullfilename );
            }
        }
        /* rule 4 */
        if( rc == -1 && IncludePath1 != NULL ) {
            rc = findInclude( IncludePath1, filename, len, fullfilename );
        }
        /* rule 5 */
        if( rc == -1 && IncludePath2 != NULL ) {
            rc = findInclude( IncludePath2, filename, len, fullfilename );
        }
        /* rule 6 */
        if( rc == -1 && incltype == PPINCLUDE_USR && (PPFlags & PPFLAG_IGNORE_DEFDIRS) == 0 ) {
            memcpy( fname, H_DIR, sizeof( H_DIR ) - 1 );
            memcpy( fname + sizeof( H_DIR ) - 1, filename, len );
            fname[sizeof( H_DIR ) - 1 + len] = '\0';
            rc = checkfullpath( fname, fullfilename );
        }
    }
    return( rc );
}


static FILE *PP_OpenInclude( const char *filename, size_t len, incl_type incltype )
{
    char        fullfilename[_MAX_PATH];
    int         rc;

    rc = PP_IncludePathFind( filename, len, fullfilename, incltype, get_parent_filename );
    if( PPFlags & PPFLAG_DEPENDENCIES ) {
        (*PP_CallBack)( filename, len, fullfilename, incltype );
    } else if( rc == 0 ) {
        return( PP_Open( fullfilename ) );
    }
    return( NULL );
}

static FILE *PP_OpenIncludeAlias( const char *filename, size_t len, incl_type incltype )
{
    /* unused parameters */ (void)filename; (void)len; (void)incltype;

    /* not yet implemented */

    return( NULL );
}

static FILE *PP_OpenIncludeTruncated( const char *filename, size_t len, incl_type incltype )
{
    pgroup2     pg1;
    pgroup2     pg2;
    bool        truncated;

    truncated = false;
    memcpy( pg1.buffer, filename, len );
    pg1.buffer[len] = '\0';
    _splitpath2( pg1.buffer, pg2.buffer, &pg2.drive, &pg2.dir, &pg2.fname, &pg2.ext );
    if( strlen( pg2.fname ) > 8 ) {
        pg2.fname[8] = '\0';
        truncated = true;
    }
    if( strlen( pg2.ext ) > 4 ) {
        pg2.ext[4] = '\0';
        truncated = true;
    }
    if( truncated ) {
        // try to open truncated name if necessary
        _makepath( pg1.buffer, pg2.drive, pg2.dir, pg2.fname, pg2.ext );
        return( PP_OpenInclude( pg1.buffer, strlen( pg1.buffer ), incltype ) );
    }
    return( NULL );
}

static void PP_GenLine( void )
{
    char        *p;
    const char  *fname;
    int         len;

    p = PPLineBuf + 1;
    if( PPFlags & PPFLAG_EMIT_LINE ) {
        p += sprintf( p, "%cline %u \"", PPPreProcChar, PP_File->linenum );
        fname = PP_File->filename;
        while( *fname != '\0' ) {
#ifndef __UNIX__
            if( IS_DIR_SEP( *fname ) ) {
                *p++ = DIR_SEP;
                *p++ = DIR_SEP;
                ++fname;
                continue;
            }
#endif
            len = PP_MBCharLen( fname );
            while( len-- > 0 ) {
                *p++ = *fname++;
            }
        }
        *p++ = '\"';
    }
    *p++ = '\n';
    *p = '\0';
    PPNextTokenPtr = PPLineBuf + 1;
}

static void PP_GenError( const char *msg )
{
    sprintf( PPLineBuf + 1, "%cerror %s\n", PPPreProcChar, msg );
    PPNextTokenPtr = PPLineBuf + 1;
}

static void PP_TimeInit( void )
{
    struct tm   *tod;
    time_t      time_of_day;

    time_of_day = time( &time_of_day );
    tod = localtime( &time_of_day );
    FormatTime_tm( PP__TIME__ + 1, tod );
    FormatDate_tm( PP__DATE__ + 1, tod );
}

int PPENTRY PP_FileInit( const char *filename, pp_flags ppflags, const char *include_path )
{
    FILE        *handle;
    int         hash;

    for( hash = 0; hash < HASH_SIZE; hash++ ) {
        PPHashTable[hash] = NULL;
    }
    PPFlags = ppflags;
    NestLevel = 0;
    SkipLevel = 0;
    IncludePath2 = PP_Malloc( 1 );
    *IncludePath2 = '\0';
    IncludePath2 = AddIncludePath( IncludePath2, include_path );
    if( (PPFlags & PPFLAG_IGNORE_INCLUDE) == 0 ) {
        IncludePath2 = AddIncludePath( IncludePath2, PP_GetEnv( "INCLUDE" ) );
    }
    PP_AddMacro( "__LINE__", 8 );
    PP_AddMacro( "__FILE__", 8 );
    PP_AddMacro( "__DATE__", 8 );
    PP_AddMacro( "__TIME__", 8 );
    PP_AddMacro( "__STDC__", 8 );
    PP_TimeInit();

    handle = PP_Open( filename );
    if( handle == NULL )
        return( -1 );
    PP_GenLine();
    PPSavedChar = '\0';
    PPTokenPtr = PPNextTokenPtr;
    return( 0 );
}

void PP_Dependency_List( pp_callback *callback )
{
    PP_CallBack = callback;
    for( ; PP_Char() != EOF; ) {
        PPTokenPtr = "";        // quickly skip over token
    }
}

static void PP_CloseAllFiles( void )
{
    while( PP_File != NULL ) {
        PP_Close();
    }
}

static char *resize_macro_buf( char *buf, size_t new_size )
{
    if( new_size > macro_buf_size ) {
        new_size = ( ( 255 + new_size ) / 256 ) * 256;
        macro_buf = PP_Malloc( new_size );
        if( buf != NULL ) {
            memcpy( macro_buf, buf, macro_buf_size );
            PP_Free( buf );
        }
        macro_buf_size = new_size;
    }
    return( macro_buf );
}

static void free_macro_buf( void )
{
    PP_Free( macro_buf );
    macro_buf = NULL;
    macro_buf_size = 0;
}

void PPENTRY PP_FileFini( void )
{
    int         hash;
    MACRO_ENTRY *me;

    for( hash = 0; hash < HASH_SIZE; hash++ ) {
        for( ; (me = PPHashTable[hash]) != NULL; ) {
            PPHashTable[hash] = me->next;
            if( me->replacement_list != NULL )
                PP_Free( me->replacement_list );
            PP_Free( me );
        }
    }
    free_macro_buf();
    PP_CloseAllFiles();
    PP_Free( IncludePath2 );
}

static size_t PP_ReadBuf( size_t line_len )
{
    size_t      len;
    FILELIST    *this_file;
    char        *p;

    this_file = PP_File;
    len = fread( this_file->buffer, 1, PPBUFSIZE, this_file->handle );
    this_file->buffer[len] = '\0';
    PPBufPtr = this_file->buffer;
    if( PPLineBufSize < line_len + len ) {
        PPLineBufSize *= 2;
        p = PP_Malloc( PPLineBufSize + 2 );
        memcpy( p, PPLineBuf, line_len );
        PP_Free( PPLineBuf );
        PPLineBuf = p;
    }
    return( len );
}

static size_t PP_ReadLine( bool *line_generated )
{
    size_t              line_len;
    unsigned char       c;

    if( PP_File == NULL ) {     // if end of main file
        return( 0 );            // - indicate EOF
    }
    PPLineNumber = PP_File->linenum;
    PPLineBuf[0] = '\0';
    PPNextTokenPtr = PPLineBuf + 1;
    *line_generated = false;
    line_len = 1;
    for( ;; ) {
        for( ;; ) {
            for( ;; ) {
                c = *PPBufPtr;
                if( c == DOS_EOF_CHAR ) {               // 17-oct-94
                    c = '\n';
                    if( line_len != 1 )
                        break;
                    c = DOS_EOF_CHAR;
                } else {
                    ++PPBufPtr;
                    if( c != '\0' ) {
                        break;
                    }
                }
                if( c == DOS_EOF_CHAR || ( PP_ReadBuf( line_len ) == 0 ) ) {
                    // if the last line of a file does not end with a carriage
                    // return then still return what is on that line
                    if( line_len > 1 ) {
                        c = '\n';
                        break;
                    }
                    if( PP_Close() == NULL ) {  // if end of main file
                        return( 0 );            // - indicate EOF
                    }
                    PP_GenLine();
                    *line_generated = true;
                    line_len = strlen( PPNextTokenPtr );
                    return( line_len );
                }
            }
            PPLineBuf[line_len] = c;
            if( c == '\n' )
                break;
            ++line_len;
        }
        PP_File->linenum++;
        if( PPLineBuf[line_len - 1] == '\r' )
            --line_len;
        if( PPLineBuf[line_len - 1] != '\\' )
            break;
        --line_len;
    }
    PPLineBuf[line_len++] = '\n';
    PPLineBuf[line_len++] = '\0';
    PPNextTokenPtr = PPLineBuf + 1;
    return( line_len );
}

#define _rotl( a, b )   ( ( a << b ) | ( a >> ( 16 - b ) ) )

static unsigned PP_Hash( const char *name, size_t len )
{
    unsigned    hash;

    hash = 0;
    while( len-- > 0 ) {
        hash = (hash << 4) + *name++;
        hash = (hash ^ _rotl( hash & 0xF000, 4 )) & 0x0FFF;
    }
    return( hash % HASH_SIZE );
}


int PP_Class( char c )
{
    if( c == '_' )
        return( CC_ALPHA );
    if( c >= 'a' && c <= 'z' )
        return( CC_ALPHA );
    if( c >= 'A' && c <= 'Z' )
        return( CC_ALPHA );
    if( c >= '0' && c <= '9' )
        return( CC_DIGIT );
    return( 0 );
}

const char *PP_ScanName( const char *ptr )
{
    if( PP_Class( *ptr ) == CC_ALPHA ) {
        ++ptr;
        while( PP_Class( *ptr ) != 0 ) {
            ++ptr;
        }
    }
    return( ptr );
}

static void open_include_file( const char *filename, const char *end, incl_type incltype )
{
    size_t      len;
    char        *buffer;
    bool        ok;

    len = end - filename;
    ok = ( PP_OpenInclude( filename, len, incltype ) != NULL );
    if( !ok ) {
        /* check open alias file name */
        ok = ( PP_OpenIncludeAlias( filename, len, incltype ) != NULL );
        if( !ok && (PPFlags & PPFLAG_TRUNCATE_FILE_NAME) ) {
            /* check truncated file name */
            ok = ( PP_OpenIncludeTruncated( filename, len, incltype ) != NULL );
        }
    }
    if( ok ) {
        PP_GenLine();
    } else {
        /* filename is located in preprocessor buffer
         * temporary copy is necessary, because buffer is
         * overwriten by sprintf function
         */
        buffer = str_dup( filename );
        if( PPErrorCallback != NULL ) {
            PPErrorCallback( buffer );
        }
        sprintf( PPLineBuf + 1, "%cerror Unable to open '%.*s'\n", PPPreProcChar, (int)len, buffer );
        PP_Free( buffer );
        PPNextTokenPtr = PPLineBuf + 1;
        PPErrorCount++;
    }
}

static void PP_Include( const char *ptr )
{
    const char  *filename;
    char        delim;
    incl_type   incltype;

    while( *ptr == ' ' || *ptr == '\t' )
        ++ptr;
    filename = ptr + 1;
    if( *ptr == '<' ) {
        delim = '>';
        incltype = PPINCLUDE_SYS;
    } else if( *ptr == '"' ) {
        delim = '"';
        incltype = PPINCLUDE_USR;
    } else {
        if( PPErrorCallback != NULL ) {
            PPErrorCallback( "Unrecognized INCLUDE directive" );
        }
        PP_GenError( "Unrecognized INCLUDE directive" );
        PPErrorCount++;
        return;
    }
    ++ptr;
    while( *ptr != delim && *ptr != '\0' )
        ++ptr;
    open_include_file( filename, ptr, incltype );
}

static void PP_RCInclude( const char *ptr )
{
    const char  *filename;
    bool        quoted = false;

    while( *ptr == ' ' || *ptr == '\t' )
        ++ptr;
    if( *ptr == '\"' ) {
        ptr++;
        quoted = true;
    }

    filename = ptr;
    ++ptr;
    if( quoted ) {
        while( *ptr != '\"' ) {
            ptr++;
        }
    } else {
        for( ;; ) {
            if( *ptr == ' ' )
                break;
            if( *ptr == '\t' )
                break;
            if( *ptr == '\r' )
                break;
            if( *ptr == '\n' )
                break;
            if( *ptr == '\0' )
                break;
            if( *ptr == '\"' )
                break;
            ++ptr;
        }
    }
    open_include_file( filename, ptr, PPINCLUDE_USR );
}

MACRO_ENTRY *PP_AddMacro( const char *macro_name, size_t len )
{
    MACRO_ENTRY     *me;
    unsigned int    hash;

    me = PP_MacroLookup( macro_name, len );
    if( me == NULL ) {
        me = (MACRO_ENTRY *)PP_Malloc( sizeof( MACRO_ENTRY ) + len );
        if( me != NULL ) {
            hash = PP_Hash( macro_name, len );
            me->next = PPHashTable[hash];
            PPHashTable[hash] = me;
            memcpy( me->name, macro_name, len );
            me->name[len] = '\0';
            me->parmcount = PP_SPECIAL_MACRO;
            me->replacement_list = NULL;
        }
    }
    return( me );
}

static const char *PP_SkipSpace( const char *p, bool *white_space )
{
    const char  *p2;

    p2 = p;
    *white_space = false;       // assume no white space
    while( *p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' )
        ++p;
    if( p != p2 ) {
        *white_space = true;    // indicate white space skipped
    }
    return( p );
}

static const char *PP_SkipComment( const char *p, bool *comment )
{
    *comment = false;           // assume no comment
    if( *p == '/' && (p[1] == '/' || p[1] == '*') ) {
        *comment = true;        // indicate comment skipped
        if( p[1] == '/' ) {
            p += 2;
            while( *p != '\0' ) {
                ++p;
            }
        } else {
            p += 2;
            for( ;; ) {
                if( *p == '\0' ) {
                    PPFlags |= PPFLAG_SKIP_COMMENT; // continued on next line
                    break;
                }
                if( *p == '*' && p[1] == '/' ) {
                    p += 2;
                    break;
                }
                ++p;
            }
        }
    } else {
        if( PPFlags & PPFLAG_ASM_COMMENT ) {
            if( *p == ';' || *p == '#' ) {
                *comment = true;
                do {
                    p++;
                } while( *p != '\0' );
            }
        }
    }
    return( p );
}

const char *PP_SkipWhiteSpace( const char *p, bool *white_space )
{
    const char  *p2;

    p2 = p;
    for( ;; ) {
        while( *p == ' ' || *p == '\t' || *p == '\r' || *p == '\n' )
            ++p;
        p = PP_SkipComment( p, white_space );
        if( !*white_space ) {
            break;
        }
    }
    if( p != p2 ) {
        *white_space = true;    // indicate white space skipped
    }
    return( p );
}

void PPENTRY PP_Define( const char *ptr )
{
    MACRO_ENTRY *me;
    const char  *macro_name;
    char        *p;
    const char  *p2;
    size_t      len;
    size_t      len1;
    bool        white_space;
    ppt_token   token;

    macro_name = PP_SkipWhiteSpace( ptr, &white_space );
    ptr = PP_ScanName( macro_name );
    me = PP_AddMacro( macro_name, ptr - macro_name );
    if( me != NULL ) {
        p = NULL;
        me->parmcount = 0;
        len = 0;
        if( *ptr == '(' ) {
            me->parmcount = 1;
            ptr++;
            for( ;; ) {
                ptr = PP_SkipWhiteSpace( ptr, &white_space );
                if( *ptr == '\0' || *ptr == ')' )
                    break;
                macro_name = ptr;
                ptr = PP_ScanName( macro_name );
                len1 = ptr - macro_name;
                p = resize_macro_buf( p, len + len1 );
                memcpy( p + len, macro_name, len1 );
                len += len1;
                me->parmcount++;
                ptr = PP_SkipWhiteSpace( ptr, &white_space );
                if( *ptr != ',' )
                    break;
                ++ptr;
                p = resize_macro_buf( p, len + 1 );
                p[len++] = '\0';                        // mark end of parm
            }
            if( *ptr == ')' ) {
                ++ptr;
                if( me->parmcount != 1 ) {
                    p = resize_macro_buf( p, len + 1 );
                    p[len++] = '\0';                    // mark end of macro parms
                }
            }
        }
        ptr = PP_SkipWhiteSpace( ptr, &white_space );
        for( ; *ptr != '\0' && *ptr != '\n'; ) {
            p2 = PP_ScanToken( ptr, &token );
            len1 = p2 - ptr;
            p = resize_macro_buf( p, len + len1 );
            memcpy( p + len, ptr, len1 );
            len += len1;
            ptr = PP_SkipWhiteSpace( p2, &white_space );
            if( *ptr == '\0' || *ptr == '\n' )
                break;
            if( white_space ) {
                p = resize_macro_buf( p, len + 1 );
                p[len++] = ' ';
            }
        }
        p = resize_macro_buf( p, len + 1 );
        p[len++] = '\0';
        me->replacement_list = PP_Malloc( len );
        memcpy( me->replacement_list, p, len );
    } else {
        PP_OutOfMemory();
    }
}

MACRO_ENTRY *PP_MacroLookup( const char *macro_name, size_t len )
{
    MACRO_ENTRY *me;
    unsigned    hash;

    hash = PP_Hash( macro_name, len );
    for( me = PPHashTable[hash]; me != NULL; me = me->next ) {
        if( memcmp( me->name, macro_name, len ) == 0 && me->name[len] == '\0' ) {
            break;
        }
    }
    return( me );
}

MACRO_ENTRY *PP_ScanMacroLookup( const char *ptr )
{
    const char  *macro_name;
    MACRO_ENTRY *me;

    while( *ptr == ' ' || *ptr == '\t' )
        ++ptr;
    macro_name = ptr;
    ptr = PP_ScanName( macro_name );
    me = PP_MacroLookup( macro_name, ptr - macro_name );
    PPNextTokenPtr = ptr;
    return( me );
}

void PPENTRY PP_MacrosWalk( pp_walk_func fn, void *cookies )
{
    int             hash;
    const char      *endptr;
    MACRO_ENTRY     *me;
    PREPROC_VALUE   val;

    for( hash = 0; hash < HASH_SIZE; hash++ ) {
        for( me = PPHashTable[hash]; me != NULL; me = me->next ) {
            if( me->parmcount == 0 && me->replacement_list != NULL ) {
                if( PPEvalExpr( me->replacement_list, &endptr, &val ) ) {
                    if( *endptr == '\0' ) {
                        fn( me, &val, cookies );
                    }
                }
            }
        }
    }
}

static void IncLevel( int value )
{
    CPP_INFO    *cpp;

    cpp = (CPP_INFO *)PP_Malloc( sizeof( CPP_INFO ) );
    cpp->prev_cpp = PPStack;
    cpp->cpp_type = PP_IF;
    cpp->processing = false;
    PPStack = cpp;
    if( NestLevel == SkipLevel ) {
        if( value ) {
            ++SkipLevel;
            cpp->processing = true;
        }
    }
    ++NestLevel;
}

static void PP_Undef( const char *ptr )
{
    MACRO_ENTRY *me;

    me = PP_ScanMacroLookup( ptr );
    if( me != NULL ) {
        me->name[0] = '\0';
    }
}

static void PP_Ifdef( const char *ptr )
{
    MACRO_ENTRY *me;

    me = PP_ScanMacroLookup( ptr );
    IncLevel( me != NULL );
}

static void PP_Ifndef( const char *ptr )
{
    MACRO_ENTRY *me;

    me = PP_ScanMacroLookup( ptr );
    IncLevel( me == NULL );
}

static int PPConstExpr( const char *ptr )
{
    PREPROC_VALUE   value;

    PPTokenPtr = ptr;
    PPFlags |= PPFLAG_PREPROCESSING;
    PP_ConstExpr( &value );
    PPFlags &= ~PPFLAG_PREPROCESSING;
    return( value.val.ivalue != 0 );
}

static void PP_If( const char *ptr )
{
    int         value;

    while( *ptr == ' ' || *ptr == '\t' )
        ++ptr;
    if( NestLevel == SkipLevel ) {
        value = PPConstExpr( ptr );
        IncLevel( value );
    } else {                /* no need to evaluate expression */
        IncLevel( 0 );
    }
}

static void PP_Elif( const char *ptr )
{
    int         value;

    while( *ptr == ' ' || *ptr == '\t' )
        ++ptr;
    if( NestLevel == 0 || PPStack->cpp_type == PP_ELSE ) {
        // CErr1( ERR_MISPLACED_ELIF );
    } else {
        if( NestLevel == SkipLevel ) {
            --SkipLevel;                /* start skipping else part */
            PPStack->processing = false;
            PPStack->cpp_type = PP_ELIF;
        } else if( NestLevel == SkipLevel + 1 ) {
            /* only evaluate the expression when required */
            if( PPStack->cpp_type == PP_IF ) {
                value = PPConstExpr( ptr );
                if( value ) {
                    SkipLevel = NestLevel; /* start including else part */
                    PPStack->processing = true;
                    PPStack->cpp_type = PP_ELIF;
                }
            }
        }
    }
}

static void PP_Else( void )
{
    if( PPStack != NULL ) {
        if( PPStack->cpp_type == PP_ELSE ) {
            // error
        } else {
            if( NestLevel == SkipLevel ) {
                --SkipLevel;            /* start skipping else part */
                PPStack->processing = false;
            } else if( NestLevel == SkipLevel + 1 ) {
                /* cpp_type will be PP_ELIF if an elif was true */
                if( PPStack->cpp_type == PP_IF ) {
                    SkipLevel = NestLevel;  /* start including else part */
                    PPStack->processing = true;
                }
            }
            PPStack->cpp_type = PP_ELSE;
        }
    }
}

static void PP_Endif( void )
{
    CPP_INFO    *cpp;

    if( NestLevel != 0 ) {
        --NestLevel;
        cpp = PPStack;
        PPStack = cpp->prev_cpp;
        PP_Free( cpp );
    }
    if( NestLevel < SkipLevel ) {
        SkipLevel = NestLevel;
    }
}

static int PP_Sharp( const char *ptr )
{
    const char  *token;
    size_t      len;

    while( *ptr == ' ' || *ptr == '\t' )
        ++ptr;
    token = ptr;
    while( *ptr >= 'a' && *ptr <= 'z' )
        ++ptr;
    len = ptr - token;
    if( len == 7 && memcmp( token, "include", 7 ) == 0 ) {
        if( NestLevel == SkipLevel ) {
            PP_Include( ptr );
            return( 0 );
        }
    } else if( len == 6 && memcmp( token, "define", 6 ) == 0 ) {
        if( NestLevel == SkipLevel ) {
            PP_Define( ptr );
        }
    } else if( len == 5 && memcmp( token, "undef", 5 ) == 0 ) {
        if( NestLevel == SkipLevel ) {
            PP_Undef( ptr );
        }
    } else if( len == 5 && memcmp( token, "ifdef", 5 ) == 0 ) {
        PP_Ifdef( ptr );
    } else if( len == 6 && memcmp( token, "ifndef", 6 ) == 0 ) {
        PP_Ifndef( ptr );
    } else if( len == 2 && memcmp( token, "if", 2 ) == 0 ) {
        PP_If( ptr );
    } else if( len == 4 && memcmp( token, "elif", 4 ) == 0 ) {
        PP_Elif( ptr );
    } else if( len == 4 && memcmp( token, "else", 4 ) == 0 ) {
        PP_Else();
    } else if( len == 5 && memcmp( token, "endif", 5 ) == 0 ) {
        PP_Endif();
    } else {
        if( PPFlags & PPFLAG_ASM_COMMENT ) {
            // # is also a line-comment char in MS's stupid assembler
            // so we just return 1 to say we recognized it and the rest
            // of this delightful preprocessor will take care of eating
            // the remainder of the line
            return( 1 );
        }
        return( 0 );            // indicate unrecognized
    }
    return( 1 );                // indicate recognized
}

static void RCInclude( const char *ptr )
{
    const char  *token;
    size_t      len;

    token = ptr;
    while( (*ptr >= 'a' && *ptr <= 'z') || (*ptr >= 'A' && *ptr <= 'Z' ) ) {
        ++ptr;
    }
    len = ptr - token;
    if( len == 9 && stricmp( token, "rcinclude" ) == 0 ) {
        if( NestLevel == SkipLevel ) {
            PP_RCInclude( ptr );
        }
    }
}


static int PP_Read( void )
{
    const char  *p;
    bool        line_generated;
    bool        white_space;

    if( PP_ReadLine( &line_generated ) == 0 )
        return( 0 );
    // don't look for preprocessor directives inside multi-line comments
    if( !line_generated && (PPFlags & PPFLAG_SKIP_COMMENT) == 0 ) {
        p = PP_SkipSpace( PPNextTokenPtr, &white_space );
        if( *p == PPPreProcChar ) {
            if( PP_Sharp( p + 1 ) ) {       // if recognized
                PPLineBuf[1 + 0] = '\n';
                PPLineBuf[1 + 1] = '\0';
                PPNextTokenPtr = PPLineBuf + 1;
            }
        } else {
            RCInclude( p );
        }
    }
    if( NestLevel != SkipLevel ) {
        PPLineBuf[1 + 0] = '\n';
        PPLineBuf[1 + 1] = '\0';
        PPNextTokenPtr = PPLineBuf + 1;
    }
    return( 1 );
}

static const char *PPScanLiteral( const char *p )
{
    char        quote_char;
    int         len;

    for( quote_char = *p++; ; p += len ) {
        len = PP_MBCharLen( p );
        if( len == 1 )  {
            if( *p == '\0' )
                break;
            if( *p == quote_char ) {
                ++p;
                break;
            }
            if( *p == '\\' ) {
                ++p;
                if( *p == '\0' ) {
                    break;
                }
            }
        }
    }
    return( p );
}

static const char *PPScanDigits( const char *p )
{
    while( *p >= '0' && *p <= '9' )
        ++p;
    return( p );
}

static const char *PPScanSuffix( const char *p )
{
    while( *p == 'u' || *p == 'U' || *p == 'l' || *p == 'L' ) {
        p++;
    }
    return( p );
}

static const char *PPScanHexNumber( const char *p )
{
    char    c;

    p += 2;                             // skip over the "0x"
    for( ;; ) {
        p = PPScanDigits( p );
        c = (char)tolower( *p );
        if( c < 'a' || c > 'f' )
            break;
        ++p;
    }
    p = PPScanSuffix( p );
    return( p );
}

static const char *PPScanNumber( const char *p )
{
    p = PPScanDigits( p );
    if( *p == '.' ) {
        p = PPScanDigits( p + 1 );
    }
    if( *p == 'e' || *p == 'E' ) {
        p++;
        if( *p == '+' || *p == '-' ) {
            ++p;
        }
        p = PPScanDigits( p );
    }
    p = PPScanSuffix( p );
    return( p );
}

static const char *PPScanOther( const char *p )
{
    for( ;; ) {
        if( *p == '\0' )
            break;
        if( *p == '\'' )
            break;
        if( *p == '\"' )
            break;
        if( *p == '_'  )
            break;
        if( *p == ','  )
            break;
        if( *p == PPPreProcChar )
            break;
        if( *p == '('  )
            break;
        if( *p == ')'  )
            break;
        if( *p == ' '  )
            break;
        if( *p == '\t' )
            break;
        if( *p == '\r' )
            break;
        if( *p == '\n' )
            break;
        if( *p >= '0' && *p <= '9' )
            break;
        if( isalpha( *p ) )
            break;
        ++p;
    }
    return( p );
}

const char *PP_ScanToken( const char *p, ppt_token *token )
{
    const char  *p2;
    bool        white_space;
    ppt_token   ctoken;

    if( PPFlags & PPFLAG_SKIP_COMMENT ) {
        *token = PPT_COMMENT;
        for( ; *p != '\0'; ++p ) {
            if( p[0] == '*' && p[1] == '/' ) {
                p += 2;
                PPFlags &= ~PPFLAG_SKIP_COMMENT;
                break;
            }
        }
        return( p );
    }
    switch( *p ) {
    case '(':
        ++p;
        ctoken = PPT_LEFT_PAREN;
        break;
    case ')':
        ++p;
        ctoken = PPT_RIGHT_PAREN;
        break;
    case ',':
        ++p;
        ctoken = PPT_COMMA;
        break;
    case '\'':
    case '\"':
        p = PPScanLiteral( p );
        ctoken = PPT_LITERAL;
        break;
    case '.':
        if( p[1] >= '0' && p[1] <= '9' ) {
            p = PPScanNumber( p );
            ctoken = PPT_NUMBER;
        } else {
            p = PPScanOther( p );
            ctoken = PPT_OTHER;
        }
        break;
    case ' ':
    case '\t':
    case '\r':
    case '\n':
        p = PP_SkipSpace( p, &white_space );
        ctoken = PPT_WHITE_SPACE;
        break;
    case ';':
    case '/':
        p2 = PP_SkipComment( p, &white_space );
        if( white_space ) {
            p = p2;
            ctoken = PPT_COMMENT;
        } else {
            p = PPScanOther( p );
            ctoken = PPT_OTHER;
        }
        break;
    case '#':
        if( PPFlags & PPFLAG_ASM_COMMENT ) {
            p2 = PP_SkipComment( p, &white_space );
            if( white_space ) {
                p = p2;
                ctoken = PPT_COMMENT;
            } else {
                p = PPScanOther( p );
                ctoken = PPT_OTHER;
            }
            break;
        }
        // Fall through!
    default:
        if( p[0] == PPPreProcChar ) {
            if( p[1] == PPPreProcChar ) {
                p += 2;
                ctoken = PPT_SHARP_SHARP;
            } else {
                ++p;
                ctoken = PPT_SHARP;
            }
        } else if( p[0] >= '0' && p[0] <= '9' ) {
            if( p[1] == 'x' || p[1] == 'X' ) {
                p = PPScanHexNumber( p );
            } else {
                p = PPScanNumber( p );
            }
            ctoken = PPT_NUMBER;
        } else if( p[0] == 'L' && (p[1] == '\'' || p[1] == '\"') ) {
            p = PPScanLiteral( p + 1 );
            ctoken = PPT_LITERAL;
        } else if( isalpha( p[0] ) || p[0] == '_' ) {
            p = PP_ScanName( p );
            ctoken = PPT_ID;
        } else {
            p = PPScanOther( p );
            ctoken = PPT_OTHER;
        }
        break;
    }
    *token = ctoken;
    return( p );
}

int PP_ScanNextToken( ppt_token *token )
{
    *(char *)PPNextTokenPtr = PPSavedChar;
    if( *PPNextTokenPtr == '\0' ) {
        if( PPFlags & PPFLAG_DONT_READ )
            return( EOF );
        if( PP_Read() == 0 ) {
            return( EOF );
        }
    }
    PPTokenPtr = PPNextTokenPtr;
    PPNextTokenPtr = PP_ScanToken( PPNextTokenPtr, token );
    PPSavedChar = *PPNextTokenPtr;
    *(char *)PPNextTokenPtr = '\0';
    return( 0 );
}

int PPENTRY PP_Char( void )
{
    MACRO_TOKEN *mtok;
    MACRO_ENTRY *me;
    ppt_token   token;

    if( *PPTokenPtr == '\0' ) {
        for( ; (mtok = PPTokenList) != NULL; ) {
            PPTokenList = mtok->next;
            PP_Free( mtok );
            mtok = PPTokenList;
            if( mtok == NULL )
                break;
            if( !IS_END_OF_MACRO( mtok ) ) {
                PPTokenPtr = mtok->data;
                return( (unsigned char)*PPTokenPtr++ );
            }
            DeleteNestedMacro();
        }
        for( ;; ) {
            if( PP_ScanNextToken( &token ) == EOF )
                return( EOF );
            if( token == PPT_COMMENT && (PPFlags & PPFLAG_KEEP_COMMENTS) == 0 ) {
                if( PPSavedChar == '\0' ) {
                    PPTokenPtr = PPNextTokenPtr;
                    return( '\n' );
                } else {
                    // replace comment with a space
                    PPTokenPtr = PPNextTokenPtr;
                    return( ' ' );
                }
            }
            if( token != PPT_ID )
                break;
            me = PP_MacroLookup( PPTokenPtr, strlen( PPTokenPtr ) );
            if( me == NULL )
                break;
            if( me->parmcount != PP_SPECIAL_MACRO && me->parmcount != 0 ) {
                if( PPSavedChar != '(' ) {
                    break;
                }
            }
            DoMacroExpansion( me );
            if( PPTokenList != NULL ) {
                PPTokenPtr = PPTokenList->data;
                break;
            }
        }
    }
    return( (unsigned char)*PPTokenPtr++ );
}

void PPENTRY PP_Init( char c )
/****************************/
{
    PP_File = NULL;
    PPStack = NULL;
    PPErrorCount = 0;
    PPLineNumber = 0;
    strcpy( PP__DATE__, "\"Dec 31 2005\"" );
    strcpy( PP__TIME__, "\"12:00:00\"" );
    PPBufPtr = NULL;
    PPNextTokenPtr = NULL;
    PPTokenList = NULL;
    PPCurToken = NULL;
    PPLineBufSize = PPBUFSIZE;
    PPLineBuf = PP_Malloc( PPLineBufSize + 2 );
    PPLineBuf[0] = '\0';
    PPLineBuf[1] = '\0';
    PPPreProcChar = c;
    PPMacroVarInit();
}

int PPENTRY PP_Fini( void )
/*************************/
{
    PPMacroVarFini();
    PP_Free( PPLineBuf );
    PPLineBuf = NULL;
    return( PPErrorCount > 0 );
}
