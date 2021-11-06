/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Utility to create setup.inf file for Watcom installers.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#include <sys/stat.h>
#ifndef __UNIX__
    #include <direct.h>
    #include <dos.h>
#endif
#ifdef __NT__
    #include <windows.h>
#endif
#include "bool.h"
#include "iopath.h"
#include "encodlng.h"
#include "cvttable.h"

#include "clibext.h"


#define SECTION_BUF_SIZE 8192   // allow long text strings

#define BLOCKSIZE       512

#define RoundUp( size, limit )  ( ( ( size + limit - 1 ) / limit ) * limit )

#define IS_EMPTY(p)     ((p)[0] == '\0' || (p)[0] == '.' && (p)[1] == '\0')

#define IS_WS(c)        ((c) == ' ' || (c) == '\t')
#define SKIP_WS(p)      while(IS_WS(*(p))) (p)++

#define IS_ASCII(c)     (c < 0x80)

typedef int (*comp_fn)(const void *,const void *);

typedef struct path_info {
    struct path_info    *next;
    char                *path;
    int                 target;
    int                 parent;
} PATH_INFO;

typedef struct size_list {
    struct size_list    *next;
    long                size;
    time_t              stamp;
    char                type;
    char                redist;
    char                remove;
    char                *dst_var;
    char                name[1];
} size_list;

typedef struct file_info {
    struct file_info    *next;
//    char                *file;
    char                *pack;
    char                *condition;
    int                 path;
    int                 old_path;
    int                 num_files;
    size_list           *sizes;
} FILE_INFO;


typedef struct list {
    struct list         *next;
    char                *item;
    int                 type;
} LIST;

enum {
    DELETE_DIALOG,
    DELETE_FILE,
    DELETE_DIR
};

enum {
    MKINF_LANG_NULL = 0,
    #define LANG_DEF( id, dbcs )        MKINF_LANG_ ## id ,
    LANG_DEFS
    #undef LANG_DEF
};

static char                 *Product;
static char                 *RelRoot;
static char                 *Setup = NULL;
static FILE_INFO            *FileList = NULL;
static PATH_INFO            *PathList = NULL;
static LIST                 *AppSection = NULL;
static LIST                 *IconList = NULL;
static LIST                 *SupplementList = NULL;
static LIST                 *IniList = NULL;
static LIST                 *AutoList = NULL;
static LIST                 *CfgList = NULL;
static LIST                 *EnvList = NULL;
static LIST                 *DialogList = NULL;
static LIST                 *BootTextList = NULL;
static LIST                 *ExeList = NULL;
static LIST                 *TargetList = NULL;
static LIST                 *LabelList = NULL;
static LIST                 *UpgradeList = NULL;
static LIST                 *AutoSetList = NULL;
static LIST                 *AfterList = NULL;
static LIST                 *BeforeList = NULL;
static LIST                 *EndList = NULL;
static LIST                 *DeleteList = NULL;
static LIST                 *ForceDLLInstallList = NULL;
static LIST                 *AssociationList = NULL;
static LIST                 *ErrMsgList = NULL;
static LIST                 *SetupErrMsgList = NULL;
static int                  Lang = MKINF_LANG_English;
static bool                 Utf8 = false;
static bool                 Upgrade = false;
static bool                 Verbose = false;
static bool                 IgnoreMissingFiles = false;
static bool                 CreateMissingFiles = false;
static LIST                 *Include = NULL;
static const char           MksetupInf[] = "mksetup.inf";

/*
 * Shift-JIS (CP932) lead byte ranges
 * 0x81-0x9F
 * 0xE0-0xFC
 */
static cvt_chr cvt_table_932[] = {
    #define pickb(s,u) {s, u},
    #define picki(s,u)
    #include "cp932uni.h"
    #undef picki
    #undef pickb
};

static int compare_utf8( const cvt_chr *p1, const cvt_chr *p2 )
/*************************************************************/
{
    return( p1->u - p2->u );
}

static size_t utf8_to_cp932( const char *src, size_t src_len, char *dst )
/***********************************************************************/
{
    size_t      i;
    size_t      o;
    cvt_chr     x;
    cvt_chr     *p;

    o = 0;
    for( i = 0; i < src_len && o < SECTION_BUF_SIZE - 6; i++ ) {
        x.u = (unsigned char)src[i];
        if( IS_ASCII( x.u ) ) {
            /*
             * ASCII (0x00-0x7F), no conversion
             */
            dst[o++] = (char)x.u;
        } else {
            /*
             * UTF-8 to UNICODE conversion
             */
            if( (x.u & 0xF0) == 0xE0 ) {
                x.u &= 0x0F;
                x.u = (x.u << 6) | ((unsigned char)src[++i] & 0x3F);
            } else {
                x.u &= 0x1F;
            }
            x.u = (x.u << 6) | ((unsigned char)src[++i] & 0x3F);
            /*
             * UNICODE to CP932 encoding conversion
             */
            p = bsearch( &x, cvt_table_932, sizeof( cvt_table_932 ) / sizeof( cvt_table_932[0] ), sizeof( cvt_table_932[0] ), (comp_fn)compare_utf8 );
            if( p == NULL ) {
                printf( "unknown unicode character: 0x%4.4X\n", x.u );
                x.s = '?';
            } else {
                x.s = p->s;
            }
            if( x.s > 0xFF ) {
                /* write lead byte first */
                dst[o++] = (char)(x.s >> 8);
            }
            dst[o++] = (char)x.s;
        }
    }
    dst[o] = '\0';
    return( o );
}

static void ConcatDirElem( char *dir, const char *elem )
/******************************************************/
{
    size_t      len;

    len = strlen( dir );
    if( len > 0 ) {
        char c = dir[len - 1];
        if( !IS_PATH_SEP( c ) ) {
            dir[len++] = DIR_SEP;
        }
    }
    strcpy( dir + len, elem );
}

static char     lines_buffer[SECTION_BUF_SIZE];

static char *mygets( char *buf, int max_len_buf, FILE *fp )
/*********************************************************/
{
    char        *p,*q,*start;
    char        *d;
    static int  last_lang = MKINF_LANG_NULL;
    size_t      got;
    size_t      len;
    size_t      max_len = SECTION_BUF_SIZE;

    /* unused parameters */ (void)max_len_buf;

    /* syntax is //nstring//mstring//0 */

    /* concatenate multiple lines into lines_buffer if \\ character on the end of line */
    p = lines_buffer;
    for( ;; ) {
        if( fgets( p, (int)max_len, fp ) == NULL ) {
            if( feof( fp ) ) {
                if( p != lines_buffer ) {
                    p[0] = '\0';
                    break;
                }
            }
            return( NULL );
        }
        q = p;
        SKIP_WS( q );
        got = strlen( q );
        if( p != q )
            memmove( p, q, got + 1 );
        /* check '\n' char */
        if( got == 0 || p[got - 1] != '\n' )
            break;
        /* skip '\n' */
        got--;
        /* check continuation char '\\' */
        if( got == 0 || p[got - 1] != '\\' ) {
            /* terminate buffer */
            p[got] = '\0';
            break;
        }
        /* skip '\\' */
        got--;
        /* continuation, append next line to the buffer */
        p += got;
        max_len -= got;
    }
    p = lines_buffer;
    d = buf;
    while( *p != '\0' ) {
        if( p[0] == '/' && p[1] == '/' && isdigit( p[2] ) ) {
            start = p;
            last_lang = p[2] - '0';
            p += 3;
            if( last_lang != MKINF_LANG_NULL ) {
                /* search next end */
                while( p[0] != '/' || p[1] != '/' || !isdigit( p[2] ) ) {
                    ++p;
                }
                if( last_lang == Lang ) {
                    if( !Utf8 && last_lang == MKINF_LANG_Japanese ) {
                        utf8_to_cp932( start + 3, p - ( start + 3 ), d );
                        d += strlen( d );
                    } else {
                        len = p - ( start + 3 );
                        memcpy( d, start + 3, len );
                        d += len;
                    }
                }
            }
        } else {
            *d++ = *p++;
        }
    }
    *d = '\0';
    return( buf );
}


static void AddToList( LIST *new, LIST **list )
/*********************************************/
{
    while( *list != NULL ) {
        list = &((*list)->next);
    }
    *list = new;
}


static long FileSize( const char *file )
/**************************************/
{
    struct stat         stat_buf;

    if( stat( file, &stat_buf ) != 0 ) {
        printf( "Can't find '%s'\n", file );
        return( 0 );
    } else {
        return( RoundUp( stat_buf.st_size, BLOCKSIZE ) );
    }
}


bool CheckParms( int *pargc, char **pargv[] )
/*******************************************/
{
    struct stat         stat_buf;
    char                **argv;
    int                 argc;
    LIST                *new;

    if( *pargc > 1 ) {
        while( ((*pargv)[1] != NULL) && ((*pargv)[1][0] == '-') ) {
            if( tolower( (*pargv)[1][1] ) == 'l' ) {
                Lang = (*pargv)[1][2] - '0';
            } else if( tolower( (*pargv)[1][1] ) == 'd' ) {
                new = malloc( sizeof( LIST ) );
                if( new == NULL ) {
                    printf( "\nOut of memory\n" );
                    exit( 1 );
                }
                new->next = NULL;
                new->item = strdup( &(*pargv)[1][2] );
                AddToList( new, &AppSection );
            } else if( tolower( (*pargv)[1][1] ) == 'i' ) {
                new = malloc( sizeof( LIST ) );
                if( new == NULL ) {
                    printf( "\nOut of memory\n" );
                    exit( 1 );
                }
                new->next = NULL;
                new->item = strdup( &(*pargv)[1][2] );
                AddToList( new, &Include );
            } else if( tolower( (*pargv)[1][1] ) == 'u' && tolower( (*pargv)[1][2] ) == 't' && tolower( (*pargv)[1][3] ) == 'f' && (*pargv)[1][4] == '8') {
                Utf8 = true;
            } else if( tolower( (*pargv)[1][1] ) == 'u' ) {
                Upgrade = true;
            } else if( tolower( (*pargv)[1][1] ) == 'v' ) {
                Verbose = true;
            } else if( tolower( (*pargv)[1][1] ) == 'f' ) {
                IgnoreMissingFiles = true;
            } else if( tolower( (*pargv)[1][1] ) == 'x' ) {
                CreateMissingFiles = true;
            } else {
                printf( "Unrecognized option %s\n", (*pargv)[1] );
            }
            ++*pargv;
            --*pargc;
        }
    }
    argc = *pargc;
    argv = *pargv;
    if( argc != 4 ) {
        printf( "Usage: mkinf [-options] <product> <file_list> <rel_root>\n\n" );
        printf( "Supported options (case insensitive):\n" );
        printf( "-v         verbose operation\n" );
        printf( "-i<path>   include path for setup scripts\n" );
        printf( "-u         create upgrade setup script\n" );
        printf( "-d<string> specify string to add to Application section\n" );
        printf( "-f         force script creation if files missing (testing only)\n" );
        printf( "-x         force creation of missing files (testing only)\n" );
        return( false );
    }
    Product = argv[1];
    RelRoot  = argv[3];
    if( stat( RelRoot, &stat_buf ) != 0 ) {  // exists
        printf( "\nDirectory '%s' does not exist\n", RelRoot );
        return( false );
    }
    if( Lang == MKINF_LANG_Japanese ) {
        if( !Utf8 ) {
            qsort( cvt_table_932, sizeof( cvt_table_932 ) / sizeof( cvt_table_932[0] ), sizeof( cvt_table_932[0] ), (comp_fn)compare_utf8 );
        }
    }
    return( true );
}


int AddTarget( const char *target )
/*********************************/
{
    int                 count;
    LIST                *newitem, *curr, **owner;

    count = 1;
    for( owner = &TargetList; (curr = *owner) != NULL; owner = &(curr->next) ) {
        if( stricmp( target, curr->item ) == 0 ) {
            return( count );
        }
        ++count;
    }

    newitem = malloc( sizeof( LIST ) );
    if( newitem != NULL ) {
        newitem->item = strdup( target );
        if( newitem->item != NULL ) {
            newitem->next = NULL;
            *owner = newitem;
            return( count );
        }
        free( newitem );
    }
    printf( "Out of memory\n" );
    return( 0 );
}


static char *GetBracketedString( const char *src, const char **end )
/******************************************************************/
{
    const char      *p1;
    char            *ret;
    size_t          len;

    if( *src++ != '<' ) {
        return( NULL );
    }
    p1 = strchr( src, '>' );
    if( p1 == NULL ) {
        return( NULL );
    }
    len = p1 - src;
    *end = src + len + 1;
    if( (ret = malloc( len + 1 )) == NULL ) {
        return( NULL );
    }
    strncpy( ret, src, len );
    ret[len] = '\0';
    return( ret );
}


int AddPath( const char *path, int target, int parent )
/*****************************************************/
{
    int                 count;
    PATH_INFO           *newitem, *curr, **owner;

    count = 1;
    for( owner = &PathList; (curr = *owner) != NULL; owner = &(curr->next) ) {
        if( stricmp( path, curr->path ) == 0 && target == curr->target ) {
            return( count );
        }
        ++count;
    }

    newitem = malloc( sizeof( PATH_INFO ) );
    if( newitem != NULL ) {
        newitem->path = strdup( path );
        if( newitem->path != NULL ) {
            newitem->target = target;
            newitem->parent = parent;
            newitem->next = NULL;
            *owner = newitem;
            return( count );
        }
        free( newitem );
    }
    printf( "Out of memory\n" );
    return( 0 );
}


int AddPathTree( char *path, int target )
/***************************************/
{
    int         parent;
    char        *p;

    if( path == NULL )
        return( -1 );
    parent = AddPath( ".", target, -1 );
    for( p = path; (p = strchr( p, '\\' )) != NULL; ) {
        *p = '/';
    }
    p = strchr( path, '/' );
    while( p != NULL ) {
        *p = '\0';
        parent = AddPath( path, target, parent );
        if( parent == 0 )
            return( 0 );
        *p = '/';
        p = strchr( p + 1, '/' );
    }
    return( AddPath( path, target, parent ) );
}

static int mkdir_nested( const char *path )
/*****************************************/
{
#ifdef __UNIX__
    struct stat sb;
#elif defined( __NT__ )
    DWORD       attr;
#else
    unsigned    attr;
#endif
    char        pathname[_MAX_PATH + 1];
    char        *p;
    char        *end;
    char        c;

    p = pathname;
    strncpy( pathname, path, _MAX_PATH );
    pathname[_MAX_PATH] = '\0';
    end = pathname + strlen( pathname );

#ifndef __UNIX__
    /* special case for drive letters */
    if( p[0] != '\0' && p[1] == ':' ) {
        p += 2;
    }
#endif
    /* skip initial path separator if present */
    if( p[0] == DIR_SEP )
        ++p;

    /* find the next path component */
    while( p < end ) {
        while( (p < end) && ( *p != DIR_SEP ) )
            ++p;
        c = *p;     // save path separator
        *p = '\0';  // terminate path by NULL character

        /* check if pathname exists */
#ifdef __UNIX__
        if( stat( pathname, &sb ) == -1 ) {
#elif defined( __NT__ )
        attr = GetFileAttributes( pathname );
        if( attr == INVALID_FILE_ATTRIBUTES ) {
#else
        if( _dos_getfileattr( pathname, &attr ) != 0 ) {
#endif
            int rc;

#ifdef __UNIX__
            rc = mkdir( pathname, S_IRWXU | S_IRWXG | S_IRWXO );
#else
            rc = mkdir( pathname );
#endif
            if( rc != 0 ) {
                printf( "Can not create directory '%s': %s\n", pathname, strerror( errno ) );
                return( -1 );
            }
        } else {
            /* make sure it really is a directory */
#ifdef __UNIX__
            if( !S_ISDIR( sb.st_mode ) ) {
#elif defined( __NT__ )
            if( (attr & FILE_ATTRIBUTE_DIRECTORY) == 0 ) {
#else
            if( (attr & _A_SUBDIR) == 0 ) {
#endif
                printf( "Can not create directory '%s': file with the same name already exists\n", pathname );
                return( -1 );
            }
        }
        /* put back the path separator */
        *p++ = c;
    }
    return( 0 );
}

bool AddFile( char *path, char *old_path, char type, char redist, char *file, const char *rel_file, char *dst_var, const char *cond )
/***********************************************************************************************************************************/
{
    int                 path_dir, old_path_dir, target;
    FILE_INFO           *newitem, *curr, **owner;
    long                act_size;
    time_t              time;
    struct stat         stat_buf;
    char                *p;
    char                *root_file;
    char                src[_MAX_PATH];
    size_list           *ns,*sl;
    char                archive_name[16] = "pck00000";
    int                 pack_num = 1;
    char                remove;

    if( !IS_EMPTY( rel_file ) ) {
        if( HAS_PATH( rel_file ) ) {
            strcpy( src, rel_file );
        } else {
            strcpy( src, RelRoot );
            ConcatDirElem( src, rel_file );
        }
    } else if( HAS_PATH( path ) ) {
        // path is absolute. don't use RelRoot
        strcpy( src, path );
        ConcatDirElem( src, file );
    } else {
        strcpy( src, RelRoot );
        if( !IS_EMPTY( path ) ) {
            ConcatDirElem( src, path );
        }
        ConcatDirElem( src, file );
    }
#ifdef __UNIX__
    for( p = src; (p = strchr( p, '\\' )) != NULL; ) {
#else
    for( p = src; (p = strchr( p, '/' )) != NULL; ) {
#endif
        *p = DIR_SEP;
    }
    remove = ( stricmp( cond, "false" ) == 0 );
    if( remove ) {
        act_size = 0;
        time = 0;
    } else if( stat( src, &stat_buf ) != 0 ) {
        printf( "'%s' does not exist\n", src );
        if( IgnoreMissingFiles ) {
            act_size = 1024;
            time = 0;
//            return( true );
        } else if( CreateMissingFiles ) {
            FILE    *fp;
            char    c;

            fp = fopen( src, "w" );
            if( fp == NULL ) {
                for( p = src + strlen( src ); p > src; p-- ) {
                    if( *(p - 1) == DIR_SEP ) {
                        c = *p;
                        *p = '\0';
                        mkdir_nested( src );
                        *p = c;
                        break;
                    }
                }
                fp = fopen( src, "w" );
                if( fp == NULL ) {
                    printf( "Cannot create '%s'\n", src );
                    return( false );
                }
            }
            fclose( fp );
            act_size = 0;
            time = 0;
        } else {
            return( false );
        }
    } else {
        act_size = stat_buf.st_size;
        time = stat_buf.st_mtime;
    }
#if 0
    printf( "\r%s                              \r", file );
    fflush( stdout );
#endif
    act_size = RoundUp( act_size, 512 );

    // strip target off front of path
    if( *path == '%' ) {
        p = strchr( ++path, '%' );
        if( p == NULL ) {
            printf( "Invalid path(%s)\n", path );
            return( false );
        }
        *p = '\0';
        target = AddTarget( path );
        path = p + 1;
        if( *path == '\0' ) {
            path = ".";
        } else if( ( *path == '\\' ) || ( *path == '/' ) ) {
            ++path;
        } else {
            printf( "Invalid path (%s)\n", path );
            return( false );
        }
    } else {
        target = AddTarget( "DstDir" );
    }
    if( target == 0 ) {
        return( false );
    }

    // handle sub-directories in path before full path
    path_dir = AddPathTree( path, target );
    if( path_dir == 0 ) {
        return( false );
    }
    old_path_dir = AddPathTree( old_path, target );
    p = file;
#ifndef __UNIX__
    if( p[0] != '\0' && p[1] == ':' ) {
        p += 2;
    }
#endif
    for( root_file = p; *p != '\0'; p++ ) {
        if( ( *p == '\\' ) || ( *p == '/' ) ) {
            root_file = p + 1;
        }
    }

    // see if the pack_file has been seen already
    for( owner = &FileList; (curr = *owner) != NULL; owner = &(curr->next) ) {
        if( stricmp( curr->pack, archive_name ) != 0 )
            continue;
        // this file is already in the current pack file

        // if the path differs, start a new pack file
        if( curr->path != path_dir ) {
            sprintf( archive_name, "pck%05d", pack_num++ );
            if( Verbose ) {
                printf( "\nPath for archive '%s' changed to '%s'\n", curr->pack, path );
                printf( "Changing archive to '%s'\n", archive_name );
            }
            continue;
        }
        // if the condition differs, start a new pack file
        if( strcmp( curr->condition, cond ) != 0 ) {
            sprintf( archive_name, "pck%05d", pack_num++ );
            if( Verbose ) {
                printf( "\nCondition for archive '%s' changed:\n", curr->pack );
                printf( "Old: <%s>\n", curr->condition );
                printf( "New: <%s>\n", cond );
                printf( "Changing archive to '%s'\n", archive_name );
            }
            continue;
        }
        curr->num_files++;
        ns = malloc( sizeof( size_list ) + strlen( root_file ) );
        if( ns == NULL ) {
            printf( "Out of memory\n" );
            return( false );
        }
        strcpy( ns->name, root_file );
        for( sl = curr->sizes; sl != NULL; sl = sl->next ) {
            if( stricmp( sl->name, ns->name ) == 0 ) {
                printf( "file '%s' included in archive '%s' more than once\n", sl->name, curr->pack );
                free( ns );
                return( false );
            }
        }
        ns->size = act_size;
        ns->stamp = time;
        ns->type = type;
        ns->redist = redist;
        ns->remove = remove;
        ns->dst_var = dst_var;
        ns->next = curr->sizes;
        curr->sizes = ns;
        return( true );
    }

    // add to list
    newitem = malloc( sizeof( FILE_INFO ) );
    if( newitem != NULL ) {
        newitem->pack = strdup( archive_name );
        if( newitem->pack != NULL ) {
            newitem->condition = strdup( cond );
            if( newitem->condition != NULL ) {
                newitem->path = path_dir;
                newitem->old_path = old_path_dir;
                newitem->num_files = 1;
                ns = malloc( sizeof( size_list ) + strlen( root_file ) );
                if( ns != NULL ) {
                    strcpy( ns->name, root_file );
                    ns->size = act_size;
                    ns->stamp = time;
                    ns->next = NULL;
                    ns->type = type;
                    ns->redist = redist;
                    ns->remove = remove;
                    ns->dst_var = dst_var;
                    newitem->sizes = ns;
                    newitem->next = NULL;
                    *owner = newitem;
                    return( true );
                }
                free( newitem->condition );
            }
            free( newitem->pack );
        }
        free( newitem );
    }
    printf( "Out of memory\n" );
    return( false );
}


bool ReadList( FILE *fp )
/***********************/
{
    const char  *p;
    char        *s;
    char        *path;
    char        *old_path;
    char        *file;
    char        *rel_fil;
    char        *condition;
    char        *desc;
    char        *dst_var;
    char        *where;
    char        buf[1024];
    char        redist;
    char        type;
    bool        no_error;

    printf( "Checking files...\n" );
    no_error = true;
    while( fgets( buf, sizeof( buf ), fp ) != NULL ) {
        s = GetBracketedString( buf, &p );
        if( s == NULL ) {
            printf( "Invalid list file format - 'type' not found\n" );
            exit( 2 );
        }
        type = s[0];
        free( s );
        s = GetBracketedString( p, &p );
        if( s == NULL ) {
            printf( "Invalid list file format - 'redist' not found\n" );
            exit( 2 );
        }
        redist = s[0];
        free( s );
        path = GetBracketedString( p, &p );
        if( path == NULL ) {
            printf( "Invalid list file format - 'dir' not found\n" );
            exit( 2 );
        }
        old_path = GetBracketedString( p, &p );
        if( old_path == NULL ) {
            printf( "Invalid list file format - 'old dir' not found\n" );
            exit( 2 );
        }
        if( (stricmp( path, old_path ) == 0) || (*old_path == '\0') ) {
            free( old_path );
            old_path = NULL;
        }
        file = GetBracketedString( p, &p );
        if( file == NULL ) {
            printf( "Invalid list file format - 'file' not found\n" );
            exit( 2 );
        }
        rel_fil = GetBracketedString( p, &p );
        if( rel_fil == NULL ) {
            printf( "Invalid list file format - 'rel file' not found\n" );
            exit( 2 );
        }
        if( *rel_fil == '\0' ) {
            free( rel_fil );
            rel_fil = strdup( "." );
        }
        where = GetBracketedString( p, &p );
        if( where == NULL ) {
            printf( "Invalid list file format - 'product' not found\n" );
            exit( 2 );
        }
        dst_var = GetBracketedString( p, &p );
        if( dst_var == NULL ) {
            printf( "Invalid list file format - 'dst_var' not found\n" );
            exit( 2 );
        }
        if( *dst_var == '\0' ) {
            free( dst_var );
            dst_var = NULL;
        }
        condition = GetBracketedString( p, &p );
        if( condition == NULL ) {
            printf( "Invalid list file format - 'condition' not found\n" );
            exit( 2 );
        }
        desc = GetBracketedString( p, &p );
        if( desc == NULL ) {
            printf( "Invalid list file format - 'description' not found\n" );
            exit( 2 );
        }
        if( ( dst_var != NULL ) && strcmp( dst_var, "." ) == 0 ) {
            free( dst_var );
            dst_var = NULL;
        }
        if( !AddFile( path, old_path, type, redist, file, rel_fil, dst_var, condition ) ) {
            no_error = false;
        }
        free( path );
        free( old_path );
        free( file );
        free( rel_fil );
        free( where );
        free( condition );
        free( desc );
    }
    printf( ".\n" );
    return( no_error );
}


#define STRING_include          "include="
#define STRING_icon             "icon="
#define STRING_supplemental     "supplemental="
#define STRING_ini              "ini="
#define STRING_auto             "auto="
#define STRING_cfg              "cfg="
#define STRING_autoset          "autoset="
#define STRING_spawnafter       "spawnafter="
#define STRING_spawnbefore      "spawnbefore="
#define STRING_spawnend         "spawnend="
#define STRING_env              "env="
#define STRING_dialog           "dialog="
#define STRING_boottext         "boottext="
#define STRING_exe              "exe="
#define STRING_label            "label="
#define STRING_deletedialog     "deletedialog="
#define STRING_deletefile       "deletefile="
#define STRING_deletedir        "deletedir="
#define STRING_language         "language="
#define STRING_upgrade          "upgrade="
#define STRING_forcedll         "forcedll="
#define STRING_assoc            "assoc="
#define STRING_errmsg           "errmsg="
#define STRING_setuperrmsg      "setuperrmsg="
#define STRING_setup            "setup="

#define STRING_IS( buf, new, string ) \
        ( strnicmp( buf, string, sizeof( string ) - 1 ) == 0 && \
          ( new->item = strdup( buf + sizeof( string ) - 1 ) ) != NULL )


static FILE *PathOpen( const char *name )
/***************************************/
{
    FILE        *fp;
    LIST        *p;
    char        buf[_MAX_PATH];

    fp = fopen( name, "r" );
    for( p = Include; p != NULL && fp == NULL; p = p->next ) {
        _makepath( buf, NULL, p->item, name, NULL );
        fp = fopen( buf, "r" );
    }
    if( fp == NULL ) {
        printf( "\nCannot open include file '%s'\n", name );
        exit( 1 );
    }
    return( fp );
}

static char *ReplaceEnv( char *file_name )
/****************************************/
// if file_name is of the form $env_var$\name, replace with
// value of the environment variable
{
    char                *p, *q, *e, *var;
    char                buff[_MAX_PATH];

    // copy and make changes into 'buff'
    q = buff;
    p = file_name;
    for( ;; ) {
        if( *p == '$' ) {
            e = strchr( p + 1, '$' );
            if( e == NULL ) {
                strcpy( q, p );
                break;
            } else {
                *e = '\0';
                var = getenv( p + 1 );
                if( var == NULL ) {
                    printf( "Error: environment variable '%s' not found\n", p + 1 );
                } else {
                    strcpy( q, var );
                    q += strlen( var );
                }
                p = e + 1;
            }
        } else {
            *q = *p;
            if( *p == '\0' )
                break;
            ++p;
            ++q;
        }
    }
    if( strcmp( buff, file_name ) == 0 ) {
        // no environment variables found
        return( file_name );
    }
    free( file_name );
    return( strdup( buff ) );
}

static bool processLine( const char *line, LIST **list )
{
    LIST            *new;

    new = malloc( sizeof( LIST ) );
    if( new == NULL ) {
        return( true );
    }
    new->next = NULL;
    if( STRING_IS( line, new, STRING_icon ) ) {
        AddToList( new, &IconList );
    } else if( STRING_IS( line, new, STRING_supplemental ) ) {
        AddToList( new, &SupplementList );
    } else if( STRING_IS( line, new, STRING_ini ) ) {
        AddToList( new, &IniList );
    } else if( STRING_IS( line, new, STRING_auto ) ) {
        AddToList( new, &AutoList );
    } else if( STRING_IS( line, new, STRING_cfg ) ) {
        AddToList( new, &CfgList );
    } else if( STRING_IS( line, new, STRING_autoset ) ) {
        AddToList( new, &AutoSetList );
    } else if( STRING_IS( line, new, STRING_spawnafter ) ) {
        AddToList( new, &AfterList );
    } else if( STRING_IS( line, new, STRING_spawnbefore ) ) {
        AddToList( new, &BeforeList );
    } else if( STRING_IS( line, new, STRING_spawnend ) ) {
        AddToList( new, &EndList );
    } else if( STRING_IS( line, new, STRING_env ) ) {
        AddToList( new, &EnvList );
    } else if( STRING_IS( line, new, STRING_dialog ) ) {
        AddToList( new, &DialogList );
    } else if( STRING_IS( line, new, STRING_boottext ) ) {
        AddToList( new, &BootTextList );
    } else if( STRING_IS( line, new, STRING_exe ) ) {
        new->item = ReplaceEnv( new->item );
        AddToList( new, &ExeList );
    } else if( STRING_IS( line, new, STRING_label ) ) {
        AddToList( new, &LabelList );
    } else if( STRING_IS( line, new, STRING_upgrade ) ) {
        AddToList( new, &UpgradeList );
    } else if( STRING_IS( line, new, STRING_deletedialog ) ) {
        new->type = DELETE_DIALOG;
        AddToList( new, &DeleteList );
    } else if( STRING_IS( line, new, STRING_deletefile ) ) {
        new->type = DELETE_FILE;
        AddToList( new, &DeleteList );
    } else if( STRING_IS( line, new, STRING_deletedir ) ) {
        new->type = DELETE_DIR;
        AddToList( new, &DeleteList );
    } else if( STRING_IS( line, new, STRING_language ) ) {
        Lang = new->item[0] - '0';
        free( new->item );
        free( new );
    } else if( STRING_IS( line, new, STRING_forcedll ) ) {
        AddToList( new, &ForceDLLInstallList );
    } else if( STRING_IS( line, new, STRING_assoc ) ) {
        AddToList( new, &AssociationList );
    } else if( STRING_IS( line, new, STRING_errmsg ) ) {
        AddToList( new, &ErrMsgList );
    } else if( STRING_IS( line, new, STRING_setuperrmsg ) ) {
        AddToList( new, &SetupErrMsgList );
    } else {
        new->item = strdup( line );
        AddToList( new, list );
    }
    return( false );
}

static char     SectionBuf[SECTION_BUF_SIZE];

void ReadSection( FILE *fp, const char *section, LIST **list )
/************************************************************/
{
    int             file_curr = 0;
    FILE            *file_stack[20];
    bool            found;

    found = false;
    for( ;; ) {
        if( mygets( SectionBuf, sizeof( SectionBuf ), fp ) == NULL ) {
            if( file_curr-- > 0 ) {
                fclose( fp );
                fp = file_stack[file_curr];
                continue;
            }
            break;
        }
        if( SectionBuf[0]== '#' || SectionBuf[0]== '\0' )
            continue;
        if( SectionBuf[0] == '[' && SectionBuf[strlen( SectionBuf ) - 1] == ']' ) {
            if( stricmp( SectionBuf, section ) == 0 ) {
                found = true;
            } else if( found ) {
                while( file_curr-- > 0 ) {
                    fclose( fp );
                    fp = file_stack[file_curr];
                }
                break;
            }
        } else if( found ) {
            if( strnicmp( SectionBuf, STRING_setup, sizeof( STRING_setup ) - 1 ) == 0 ) {
                free( Setup );
                Setup = ReplaceEnv( strdup( SectionBuf + sizeof( STRING_setup ) - 1 ) );
            } else if( strnicmp( SectionBuf, STRING_include, sizeof( STRING_include ) - 1 ) == 0 ) {
                file_stack[file_curr++] = fp;
                fp = PathOpen( SectionBuf + sizeof( STRING_include ) - 1 );
            } else if( processLine( SectionBuf, list ) ) {
                while( file_curr-- > 0 ) {
                    fclose( fp );
                    fp = file_stack[file_curr];
                }
                fclose( fp );
                printf( "\nOut of memory\n" );
                exit( 1 );
            }
        }
    }
    if( !found ) {
        printf( "%s section not found in '%s'\n", section, MksetupInf );
    }
}


void ReadInfFile( void )
/**********************/
{
    FILE                *fp;
    char                ver_buf[80];

    fp = PathOpen( MksetupInf );
    if( fp == NULL ) {
        printf( "Cannot open '%s'\n", MksetupInf );
        return;
    }
    sprintf( ver_buf, "[%s]", Product );
    ReadSection( fp, ver_buf, &AppSection );
    fclose( fp );
}


static void fput36( FILE *fp, long value )
/****************************************/
{
    char        buff[30];

    if( value < 0 ) {
        fprintf( fp, "-" );
        value = -value;
    }
    ltoa( value, buff, 36 );
    fprintf( fp, "%s", buff );
}

void DumpSizes( FILE *fp, FILE_INFO *curr )
/*****************************************/
{
    size_list   *csize;

    fput36( fp, curr->num_files );
    fprintf( fp, "," );
    if( curr->num_files > 1 ) {
        fprintf( fp, "\\\n" );
    }
    for( csize = curr->sizes; csize != NULL; csize = csize->next ) {
        fprintf( fp, "%s!", csize->name );
        fput36( fp, csize->size/512 );
        fprintf( fp, "!" );
        if( csize->redist != '\0' ) {
            fput36( fp, (long)( csize->stamp ) );
        }
        fprintf( fp, "!" );
        if( csize->dst_var != NULL ) {
            fprintf( fp, "%s", csize->dst_var );
        }
        fprintf( fp, "!" );
        if( csize->type != '\0' ) {
            fprintf( fp, "%c", tolower( csize->type ) );
        }
        if( csize->redist != '\0' ) {
            // 'o' for ODBC file
            // 's' for supplemental file (not deleted)
            fprintf( fp, "!%c", tolower( csize->redist ) );
        }
        fprintf( fp, "," );
        if( curr->num_files > 1 ) {
            fprintf( fp, "\\\n" );
        }
    }
}

bool CheckForDuplicateFiles( void )
/*********************************/
{
    FILE_INFO           *file1,*file2;
    size_list           *name1,*name2;
    bool                ok = true;

    if( FileList != NULL ) {
        for( file1 = FileList; file1->next != NULL; file1 = file1->next ) {
            for( file2 = file1->next; file2 != NULL; file2 = file2->next ) {
                if( file1->path != file2->path || file1->condition != file2->condition )
                    continue;
                for( name1 = file1->sizes; name1 != NULL; name1 = name1->next ) {
                    for( name2 = file2->sizes; name2 != NULL; name2 = name2->next ) {
                        if( stricmp( name1->name, name2->name ) == 0 ) {
                            printf( "'%s' is in 2 pack files (%s) (%s)\n",
                                    name1->name,
                                    file1->pack,
                                    file2->pack );
                            ok = false;
                        }
                    }
                }
            }
        }
    }
    return( ok );
}


static void DumpFile( FILE *out, const char *fname )
/**************************************************/
{
    FILE                *in;
    char                *buf;

    in = PathOpen( fname );
    if( in != NULL ) {
        buf = malloc( SECTION_BUF_SIZE );
        if( buf != NULL ) {
            for( ; mygets( buf, SECTION_BUF_SIZE, in ) != NULL; ) {
                if( strnicmp( buf, STRING_include, sizeof( STRING_include ) - 1 ) == 0 ) {
                    DumpFile( out, buf + sizeof( STRING_include ) - 1 );
                } else if( buf[0] != '#' ) {
                    fputs( buf, out );
                    fputc( '\n', out );
                }
            }
            free( buf );
        } else {
            printf( "Out of memory\n" );
        }
        fclose( in );
    } else {
        printf( "Cannot open '%s'\n", fname );
    }
}


static void CreateScript( long init_size, unsigned padding )
/**********************************************************/
{
    FILE                *fp;
    FILE_INFO           *curr;
    PATH_INFO           *path;
    LIST                *list;
    LIST                *list2;

    /* unused parameters */ (void)init_size;

    fp = fopen( "setup.inf", "w" );
    if( fp == NULL ) {
        printf( "Cannot create file 'setup.inf'\n" );
        fp = stdout;
    }
    fprintf( fp, "[Application]\n" );
    for( list = AppSection; list != NULL; list = list->next ) {
        fprintf( fp, "%s\n", list->item );
    }
    if( Upgrade ) {
        fprintf( fp, "IsUpgrade=1\n" );
    }
    fprintf( fp, "\n[Targets]\n" );
    for( list = TargetList; list != NULL; list = list->next ) {
        fprintf( fp, "%s", list->item );
        for( list2 = SupplementList; list2 != NULL; list2 = list2->next ) {
            if( stricmp( list->item, list2->item ) == 0 ) {
                fprintf( fp, ",supplemental" );
            }
        }
        fprintf( fp, "\n" );
    }

    fprintf( fp, "\n[Dirs]\n" );
    for( path = PathList; path != NULL; path = path->next ) {
        fprintf( fp, "%s,%d,%d\n", path->path, path->target, path->parent );
    }

    fprintf( fp, "\n[Files]\n" );
    for( curr = FileList; curr != NULL; curr = curr->next ) {
        fprintf( fp, "%s,", curr->pack );
        DumpSizes( fp, curr );
        fput36( fp, curr->path );
        fprintf( fp, "," );
        fput36( fp, curr->old_path );
        fprintf( fp, ",%s\n", curr->condition );
    }

    if( DeleteList != NULL ) {
        fprintf( fp, "\n[DeleteFiles]\n" );
        for( list = DeleteList; list != NULL; list = list->next ) {
            switch( list->type ) {
            case DELETE_DIALOG:
                fprintf( fp, "dialog=" );
                break;
            case DELETE_FILE:
                fprintf( fp, "file=" );
                break;
            case DELETE_DIR:
                fprintf( fp, "directory=" );
                break;
            }
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( IconList != NULL ) {
        fprintf( fp, "\n[PM Info]\n" );
        for( list = IconList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( IniList != NULL ) {
        fprintf( fp, "\n[Profile]\n" );
        for( list = IniList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( AutoList != NULL ) {
        fprintf( fp, "\n[Autoexec]\n" );
        for( list = AutoList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( CfgList != NULL ) {
        fprintf( fp, "\n[Config]\n" );
        for( list = CfgList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( AutoSetList != NULL ) {
        fprintf( fp, "\n[AutoSet]\n" );
        for( list = AutoSetList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( BeforeList != NULL || AfterList != NULL || EndList != NULL ) {
        fprintf( fp, "\n[Spawn]\n" );
        for( list = BeforeList; list != NULL; list = list->next ) {
            fprintf( fp, "before=%s\n", list->item );
        }
        for( list = AfterList; list != NULL; list = list->next ) {
            fprintf( fp, "after=%s\n", list->item );
        }
        for( list = EndList; list != NULL; list = list->next ) {
            fprintf( fp, "end=%s\n", list->item );
        }
    }

    if( EnvList != NULL ) {
        fprintf( fp, "\n[Environment]\n" );
        for( list = EnvList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( DialogList != NULL ) {
        for( list = DialogList; list != NULL; list = list->next ) {
            fprintf( fp, "\n[Dialog]\n" );
            DumpFile( fp, list->item );
        }
    }

    if( LabelList != NULL ) {
        fprintf( fp, "\n[Labels]\n" );
        for( list = LabelList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( UpgradeList != NULL ) {
        fprintf( fp, "\n[Upgrade]\n" );
        for( list = UpgradeList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( ForceDLLInstallList != NULL ) {
        fprintf( fp, "\n[ForceDLLInstall]\n" );
        for( list = ForceDLLInstallList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( AssociationList != NULL ) {
        fprintf( fp, "\n[Associations]\n" );
        for( list = AssociationList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( ErrMsgList != NULL ) {
        fprintf( fp, "\n[ErrorMessage]\n" );
        for( list = ErrMsgList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    if( SetupErrMsgList != NULL ) {
        fprintf( fp, "\n[SetupErrorMessage]\n" );
        for( list = SetupErrMsgList; list != NULL; list = list->next ) {
            fprintf( fp, "%s\n", list->item );
        }
    }

    fprintf( fp, "\n[End]\n" );

    while( padding != 0 ) {
        /* add some padding to bring the size up to old size */
        fputc( ' ', fp );
        --padding;
    }

    fclose( fp );
}


static void MakeScript( void )
/****************************/
{
    FILE_INFO           *curr;
    size_list           *csize;
    long                act_size;
    long                size, old_size, inf_size;
    LIST                *list;

    act_size = 0;
    for( curr = FileList; curr != NULL; curr = curr->next ) {
        for( csize = curr->sizes; csize != NULL; csize = csize->next ) {
            act_size += csize->size;
        }
    }
    printf( "Installed size = %ld\n", act_size );

//  place SETUP.EXE, *.EXE on the 1st disk
    size = FileSize( Setup );
    for( list = ExeList; list != NULL; list = list->next ) {
        size += FileSize( list->item );
    }
    inf_size = 0;
    old_size = 0;
    for ( ;; ) {
        /* keep creating script until size stabilizes */
        CreateScript( size + inf_size, 0 );
        inf_size = FileSize( "setup.inf" );
        if( old_size > inf_size ) {
            /*
                If the new size of the install script is less than the
                old size, we can create the script with some padding
                to bring it up to the old size. This prevents us from
                going into an oscillation between two sizes.
            */
            CreateScript( size + old_size, old_size - inf_size );
            inf_size = old_size;
        }
        if( old_size == inf_size )
            break;
        old_size = inf_size;
    }
}


static char *getPath( int pos )
/*****************************/
{
    int                 count;
    PATH_INFO           *curr;

    count = 1;
    for( curr = PathList; curr != NULL; curr = curr->next ) {
        if( count == pos )
            return( curr->path );
        ++count;
    }
    return( NULL );
}


void CreateFileList( FILE *fp )
/*****************************/
{
    FILE_INFO           *curr;
    size_list           *list;
    char                *path;

    for( curr = FileList; curr != NULL; curr = curr->next ) {
        path = getPath( curr->path );
        for( list = curr->sizes; list != NULL; list = list->next ) {
            if( list->remove )
                continue;
            if( path != NULL && strcmp( path, "." ) )
                fprintf( fp, "%s/", path );
            fprintf( fp, "%s\n", list->name );
        }
    }
}


void MakeLaundryList( void )
/**************************/
{
    FILE                *fp;

    // create archive.lst
    fp = fopen( "instarch.lst", "w" );
    if( fp == NULL ) {
        printf( "Cannot create file 'instarch.lst'\n" );
        fp = stdout;
    }
    // always add setup.inf to file list
    fprintf( fp, "setup.inf\n" );
    CreateFileList( fp );
    if( fp != stdout ) {
        fclose( fp );
    }
}

static void setupInit( void )
/***************************/
{
    Setup = strdup( "setup.exe" );
}

static void setupFini( void )
/***************************/
{
    free( Setup );
}

int main( int argc, char *argv[] )
/********************************/
{
    bool                ok;
    FILE                *fp;

    if( !CheckParms( &argc, &argv ) ) {
        return( 1 );
    }
    fp = fopen( argv[2], "r" );
    if( fp == NULL ) {
        printf( "Cannot open '%s'\n", argv[2]);
        return( 1 );
    }
    setupInit();
    printf( "Reading Info File...\n" );
    ReadInfFile();
    ok = ReadList( fp );
    fclose( fp );
    if( !ok ) {
        return( 1 );
    }
    printf( "Checking for duplicate files...\n" );
    ok = CheckForDuplicateFiles();
    if( !ok ) {
        return( 1 );
    }
    if( !CreateMissingFiles ) {
        printf( "Making script...\n" );
        MakeScript();
        MakeLaundryList();
    }
    setupFini();
    return( 0 );
}

