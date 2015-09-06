/****************************************************************************
*
*                            Open Watcom Project
*
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


#include "preproc.h"
#include <ctype.h>
#include <time.h>
#include "wio.h"
#include "watcom.h"
#include "iopath.h"
#include "pathlist.h"

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

FILELIST    *PP_File = NULL;
CPP_INFO    *PPStack;
int         NestLevel;
int         SkipLevel;
unsigned    PPLineNumber;                   // current line number
unsigned    PPFlags;                        // pre-processor flags
char        PP__DATE__[] = "\"Dec 31 2005\"";// value for __DATE__ macro
char        PP__TIME__[] = "\"12:00:00\"";  // value for __TIME__ macro
char        *PPBufPtr;                      // block buffer pointer
const char  *PPNextTokenPtr;                // next character after token end pointer
const char  *PPTokenPtr;                    // pointer to next char in token
MACRO_TOKEN *PPTokenList;                   // pointer to list of tokens
MACRO_TOKEN *PPCurToken;                    // pointer to current token
char        PPSavedChar;                    // saved char at end of token
char        PPLineBuf[4096 + 2];            // line buffer
MACRO_ENTRY *PPHashTable[HASH_SIZE];
char        PreProcChar = '#';              // preprocessor line intro

pp_callback *PP_CallBack;                   // mkmk dependency callback function

static char *IncludePath1 = NULL;           // include path from cmdl
static char *IncludePath2 = NULL;           // include path from env

static char *macro_buf = NULL;
static size_t macro_buf_size = 0;

static char *Months[] = {
    "Jan", "Feb", "Mar", "Apr", "May", "Jun",
    "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

static char MBCharLen[256];         // multi-byte character len table

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
            PP_OutOfMemory();
            fclose( handle );
            handle = NULL;
            PP_File = prev_file;
        } else {
            PP_File->prev_file = prev_file;
            PP_File->handle    = handle;
            PP_File->prev_bufptr = PPBufPtr;
            PP_File->filename  = str_dup( filename );
            PP_File->linenum   = 1;
            PPBufPtr = PP_File->buffer;
            *PPBufPtr = '\0';                   // indicate buffer empty
        }
    }
    return( handle );
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
            memcpy( new_list, old_list, old_len );
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

void PP_AddIncludePath( const char *path_list )
{
    IncludePath1 = AddIncludePath( IncludePath1, path_list );
}

void PP_IncludePathInit( void )
{
    IncludePath1 = PP_Malloc( 1 );
    *IncludePath1 = '\0';
}

void PP_IncludePathFini( void )
{
    PP_Free( IncludePath1 );
}

static int findInclude( const char *path, const char *filename, size_t len, char *fullfilename )
{
    char        *p;
    char        c;

    while( (c = *path) != '\0' ) {
        p = fullfilename;
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
        if( access( fullfilename, R_OK ) == 0 ) {
            return( 0 );
        }
    }
    return( -1 );
}

/* Include search order is intended to be compatible with C/C++ compilers
 * and is as follows:
 *
 * 1) For absolute pathnames, try only that pathname and nothing else
 *
 * 2) For includes in double quotes only, search current directory
 *
 * 3) For includes in double quotes only, search the directory
 *    of including file
 *
 * 4) Search include directories specified by IncludePath1 (usually command
 *    line -I argument(s)
 *
 * 5) Search include directories specified by IncludePath2 (usualy INCLUDE path)
 *
 * 6) Directory 'h' adjacent to current directory (../h)
 *
 * Note that some of these steps will be skipped if PPFLAG_IGNORE_CWD and/or
 * PPFLAG_IGNORE_INCLUDE is set.
 */
int PP_FindInclude( const char *filename, size_t len, char *fullfilename, int incl_type )
{
    int         rc = -1;
    char        drivebuf[_MAX_DRIVE];
    char        dirbuf[_MAX_DIR];

    memcpy( fullfilename, filename, len );
    fullfilename[len] = '\0';
    if( HAS_PATH( fullfilename ) ) {
        rc = access( fullfilename, R_OK );
    } else {
        if( rc == -1 && incl_type != PPINCLUDE_SYS && (PPFlags & PPFLAG_IGNORE_CWD) == 0 ) {
            rc = access( fullfilename, R_OK );
        }
        if( rc == -1 && incl_type == PPINCLUDE_USR && PP_File != NULL ) {
            size_t  len1;

            _splitpath( PP_File->filename, drivebuf, dirbuf, NULL, NULL );
            _makepath( fullfilename, drivebuf, dirbuf, NULL, NULL );
            len1 = strlen( fullfilename );
            if( len1 > 0 ) {
                char c = fullfilename[len1 - 1];
                if( !IS_PATH_SEP( c ) ) {
                    fullfilename[len1++] = DIR_SEP;
                }
            }
            memcpy( fullfilename + len1, filename, len );
            fullfilename[len1 + len] = '\0';
            rc = access( fullfilename, R_OK );
        }
        if( rc == -1 && IncludePath1 != NULL ) {
            rc = findInclude( IncludePath1, filename, len, fullfilename );
        }
        if( rc == -1 && IncludePath2 != NULL ) {
            rc = findInclude( IncludePath2, filename, len, fullfilename );
        }
        if( rc == -1 && incl_type == PPINCLUDE_USR && (PPFlags & PPFLAG_IGNORE_DEFDIRS) == 0 ) {
            memcpy( fullfilename, H_DIR, sizeof( H_DIR ) - 1 );
            memcpy( fullfilename + sizeof( H_DIR ) - 1, filename, len );
            fullfilename[sizeof( H_DIR ) - 1 + len] = '\0';
            rc = access( fullfilename, R_OK );
        }
    }
    return( rc );
}


static FILE *PP_OpenInclude( const char *filename, size_t len, int incl_type )
{
    char        fullfilename[_MAX_PATH];
    int         rc;

    rc = PP_FindInclude( filename, len, fullfilename, incl_type );
    if( PPFlags & PPFLAG_DEPENDENCIES ) {
        (*PP_CallBack)( filename, len, fullfilename, incl_type );
    } else if( rc == 0 ) {
        return( PP_Open( fullfilename ) );
    }
    return( NULL );
}

static void PP_GenLine( void )
{
    char        *p;
    const char  *fname;
    int         i;

    p = PPLineBuf + 1;
    if( PPFlags & PPFLAG_EMIT_LINE ) {
        p += sprintf( p, "%cline %u \"", PreProcChar, PP_File->linenum );
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
            for( i = MBCharLen[*(unsigned char *)fname] + 1; i > 0; --i ) {
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
    sprintf( PPLineBuf + 1, "%cerror %s\n", PreProcChar, msg );
    PPNextTokenPtr = PPLineBuf + 1;
}

static void PP_TimeInit( void )
{
    struct tm   *tod;
    time_t      time_of_day;

    time_of_day = time( &time_of_day );
    tod = localtime( &time_of_day );
    sprintf( PP__TIME__, "\"%.2d:%.2d:%.2d\"", tod->tm_hour, tod->tm_min, tod->tm_sec );
    sprintf( PP__DATE__, "\"%3s %2d %d\"", Months[tod->tm_mon], tod->tm_mday, tod->tm_year + 1900 );
}

static void SetRange( int low, int high, char data )
{
    int     i;

    for( i = low; i <= high; ++i ) {
        MBCharLen[i] = data;
    }
}

void PP_SetLeadBytes( const char *bytes )
{
    unsigned    i;

    for( i = 0; i < 256; i++ ) {
        MBCharLen[i] = bytes[i];
    }
}

int PP_Init( const char *filename, unsigned flags, const char *include_path )
{
    return( PP_Init2( filename, flags, include_path, NULL ) );
}

int PP_Init2( const char *filename, unsigned flags, const char *include_path, const char *leadbytes )
{
    FILE        *handle;
    int         hash;

    for( hash = 0; hash < HASH_SIZE; hash++ ) {
        PPHashTable[hash] = NULL;
    }
    NestLevel = 0;
    SkipLevel = 0;
    PPFlags = flags;
    memset( MBCharLen, 0, 256 );
    if( leadbytes != NULL ) {
        PP_SetLeadBytes( leadbytes );
    } else if( flags & PPFLAG_DB_KANJI ) {
        SetRange( 0x81, 0x9f, 1 );
        SetRange( 0xe0, 0xfc, 1 );
    } else if( flags & PPFLAG_DB_CHINESE ) {
        SetRange( 0x81, 0xfc, 1 );
    } else if( flags & PPFLAG_DB_KOREAN ) {
        SetRange( 0x81, 0xfd, 1 );
    } else if( flags & PPFLAG_UTF8 ) {
        SetRange( 0xc0, 0xdf, 1 );
        SetRange( 0xe0, 0xef, 2 );
        SetRange( 0xf0, 0xf7, 3 );
        SetRange( 0xf8, 0xfb, 4 );
        SetRange( 0xfc, 0xfd, 5 );
    }
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
    FILELIST    *tmp;

    while( PP_File != NULL ) {
        tmp = PP_File;
        PP_File = PP_File->prev_file;
        fclose( tmp->handle );
        PP_Free( tmp->filename );
        PP_Free( tmp );
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

void PP_Fini( void )
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

static size_t PP_ReadBuf( void )
{
    size_t      len;
    FILELIST    *this_file;

    this_file = PP_File;
    len = fread( this_file->buffer, 1, PPBUFSIZE, this_file->handle );
    this_file->buffer[len] = '\0';
    PPBufPtr = this_file->buffer;
    return( len );
}

static size_t PP_ReadLine( char *line_generated )
{
    FILELIST            *this_file;
    size_t              len;
    unsigned char       c;

    if( PP_File == NULL ) {     // if end of main file
        return( 0 );            // - indicate EOF
    }
    PPLineNumber = PP_File->linenum;
    PPLineBuf[0] = '\0';
    PPNextTokenPtr = PPLineBuf + 1;
    *line_generated = 0;
    len = 1;
    for( ;; ) {
        for( ;; ) {
            for( ;; ) {
                c = *PPBufPtr;
                if( c == DOS_EOF_CHAR ) {               // 17-oct-94
                    c = '\n';
                    if( len != 1 )
                        break;
                    c = DOS_EOF_CHAR;
                } else {
                    ++PPBufPtr;
                    if( c != '\0' ) {
                        break;
                    }
                }
                if( c == DOS_EOF_CHAR || (PP_ReadBuf() == 0) ) {
                    // if the last line of a file does not end with a carriage
                    // return then still return what is on that line
                    if( len > 1 ) {
                        c = '\n';
                        break;
                    }
                    this_file = PP_File;
                    fclose( this_file->handle );
                    PP_File = this_file->prev_file;
                    PPBufPtr = this_file->prev_bufptr;
                    PP_Free( this_file->filename );
                    PP_Free( this_file );
                    if( PP_File == NULL ) {     // if end of main file
                        return( 0 );            // - indicate EOF
                    }
                    PP_GenLine();
                    *line_generated = 1;
                    len = strlen( PPNextTokenPtr );
                    return( len );
                }
            }
            PPLineBuf[len] = c;
            if( c == '\n' )
                break;
            ++len;
        }
        PP_File->linenum++;
        if( PPLineBuf[len - 1] == '\r' )
            --len;
        if( PPLineBuf[len - 1] != '\\' )
            break;
        --len;
    }
    PPLineBuf[len++] = '\n';
    PPLineBuf[len++] = '\0';
    PPNextTokenPtr = PPLineBuf + 1;
    return( len );
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

static void open_include_file( const char *filename, const char *end, int incl_type )
{
    size_t      len;
    char        *buffer;

    len = end - filename;
    if( PP_OpenInclude( filename, len, incl_type ) == NULL ) {
        /* filename is located in preprocessor buffer
         * temporary copy is necessary, because buffer is
         * overwriten by sprintf function
         */
        buffer = str_dup( filename );
        sprintf( PPLineBuf + 1, "%cerror Unable to open '%.*s'\n", PreProcChar, (int)len, buffer );
        PP_Free( buffer );
        PPNextTokenPtr = PPLineBuf + 1;
    } else {
        PP_GenLine();
    }
}

static void PP_Include( const char *ptr )
{
    const char  *filename;
    char        delim;
    int         incl_type;

    while( *ptr == ' ' || *ptr == '\t' )
        ++ptr;
    filename = ptr + 1;
    if( *ptr == '<' ) {
        delim = '>';
        incl_type = PPINCLUDE_SYS;
    } else if( *ptr == '"' ) {
        delim = '"';
        incl_type = PPINCLUDE_USR;
    } else {
        PP_GenError( "Unrecognized INCLUDE directive" );
        return;
    }
    ++ptr;
    while( *ptr != delim && *ptr != '\0' )
        ++ptr;
    open_include_file( filename, ptr, incl_type );
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

void PP_Define( const char *ptr )
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
        if( len == strlen( me->name ) && memcmp( me->name, macro_name, len ) == 0 ) {
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

static void IncLevel( int value )
{
    CPP_INFO    *cpp;

    cpp = (CPP_INFO *)PP_Malloc( sizeof( CPP_INFO ) );
    cpp->prev_cpp = PPStack;
    cpp->cpp_type = PP_IF;
    cpp->processing = FALSE;
    PPStack = cpp;
    if( NestLevel == SkipLevel ) {
        if( value ) {
            ++SkipLevel;
            cpp->processing = TRUE;
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
            PPStack->processing = FALSE;
            PPStack->cpp_type = PP_ELIF;
        } else if( NestLevel == SkipLevel + 1 ) {
            /* only evaluate the expression when required */
            if( PPStack->cpp_type == PP_IF ) {
                value = PPConstExpr( ptr );
                if( value ) {
                    SkipLevel = NestLevel; /* start including else part */
                    PPStack->processing = TRUE;
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
                PPStack->processing = FALSE;
            } else if( NestLevel == SkipLevel + 1 ) {
                /* cpp_type will be PP_ELIF if an elif was true */
                if( PPStack->cpp_type == PP_IF ) {
                    SkipLevel = NestLevel;  /* start including else part */
                    PPStack->processing = TRUE;
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
    char        line_generated;
    bool        white_space;

    for( ;; ) {
        if( PP_ReadLine( &line_generated ) == 0 )
            return( 0 );
        // don't look for preprocessor directives inside multi-line comments
        if( !line_generated && !(PPFlags & PPFLAG_SKIP_COMMENT) ) {
            p = PP_SkipSpace( PPNextTokenPtr, &white_space );
            if( *p == PreProcChar ) {
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
        break;
    }
    return( 1 );
}

static const char *PPScanLiteral( const char *p )
{
    char        quote_char;
    int         i;

    quote_char = *p++;
    for( ;; ) {
        i = MBCharLen[*(unsigned char *)p];
        if( i )  {
            p += i + 1;
            continue;
        }
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
        ++p;
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
        if( *p == PreProcChar )
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
        if( p[0] == PreProcChar ) {
            if( p[1] == PreProcChar ) {
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

int PP_Char( void )
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
                return( *PPTokenPtr++ );
            }
            DeleteNestedMacro();
        }
        for( ;; ) {
            for( ;; ) {
                if( PP_ScanNextToken( &token ) == EOF )
                    return( EOF );
                if( token != PPT_COMMENT )
                    break;
                if( PPFlags & PPFLAG_KEEP_COMMENTS )
                    break;
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
    return( *PPTokenPtr++ );
}

extern void PreprocVarInit( void )
/********************************/
{
    PP_File = NULL;
    PPStack = NULL;
    PPLineNumber = 0;
    strcpy( PP__DATE__, "\"Dec 31 2005\"" );
    strcpy( PP__TIME__, "\"12:00:00\"" );
    PPBufPtr = NULL;
    PPNextTokenPtr = NULL;
    PPTokenList = NULL;
    PPCurToken = NULL;
    memset( PPLineBuf, 0, sizeof( PPLineBuf ) );
    PreProcChar = '#';
}
