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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#if defined(__QNX__)
#include <utime.h>
#else
#include <sys/utime.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <share.h>
#include "machtype.h"
#include "dbginfo.h"
#include "cv4.h"
#include "tistrail.h"

#include "wstrip.h"

#pragma pack (1);

typedef struct WResHeader {
    uint_32     Magic[ 2 ]; /* must be WRESMAGIC0 and WRESMAGIC1 */
    uint_32     DirOffset;  /* offset to the start of the directory */
    uint_16     NumResources; /* number of resourses in the file */
    uint_16     NumTypes;   /* number of different types of resources in file */
    uint_16     WResVer;    /* WRESVERSION */
} WResHeader;

#define MXFNAME         130
#define BUFSIZE         0x4000

#define WRESMICRO0      0xFF0006FF
#define WRESMICRO1      0x1030
#define WRESMAGIC0      0xC3D4C1D7
#define WRESMAGIC1      0xC3D2CDCF


#define NULLCHAR        '\0'
#ifdef __QNX__
    #define PATH_SEP '/'
    #define CASE_PATH_SEP case'/'
    #define PATH_LIST_SEP ':'
#else
    #define PATH_SEP '\\'
    #define CASE_PATH_SEP   case '\\': case '/': case ':'
    #define PATH_LIST_SEP ';'
#endif

typedef enum {
    WRAP_NONE,
    WRAP_WATCOM,
    WRAP_CV4,
    WRAP_TIS
} wrapper_type;

typedef struct {
    wrapper_type        type;
    unsigned long       start;
    unsigned long       len;
} info_info;

static void FindInfoInfo( int, info_info *, int );
static int IsSymResFile( int, int );
static int IsResMagic( int, int );
static void FatalDelTmp( int, char * );
static int Suffix( char *, char * );
static void AddInfo( void );
static void StripInfo( void );

void    Fatal( int, char * );
int     Msg_Get( int resourceid, char *buffer );
int     Msg_Init();
int     Msg_Fini();
void    Usage( void );
void    Banner(void);


static char *Buffer;

static char *ExtLst[] = {
#ifdef __QNX__
        "", ".qnx",
#endif
        ".exe", ".dll",
        ".exp", ".rex",
        ".nlm", ".dsk", ".lan", ".nam",
#ifndef __QNX__
        ".qnx", "",
#endif
        NULL };

static char SymExt[] = { ".sym" };
static char ResExt[] = { ".res" };

typedef struct {
    int         h;
    char        name[_MAX_PATH];
} fdata;

fdata   finfo, fin, fout;

char    fbuff[_MAX_PATH2];

int quiet;
int nodebug_ok;
int res = 0;
unsigned bufsize;

int main( int argc, char *argv[] )
{
    size_t              size;
    int                 i,j;
    int                 argvlen;
    char                *drive;
    char                *dir;
    char                *name;
    char                *ext;
    int                 add_file;
    struct stat         in_stat;
    struct stat         other_stat;
    struct utimbuf      uptime;
    int                 has_ext;

    if( Msg_Init() != EXIT_SUCCESS ) {
        return( EXIT_FAILURE );
    }
    add_file = 0;
    j = argc-1;
    while( j > 0 ) {
        if( argv[j][0] == '-'
#ifndef __QNX__
            || argv[j][0] == '/'
#endif
          ) {
            argvlen = strlen(argv[j] );
            for( i=1; i<argvlen; i++ ) {
                switch( argv[j][i] ) {
                case 'q': quiet=1; break;
                case 'n': nodebug_ok=1; break;
                case 'a': add_file=1; break;
                case 'r': res=1; break;
                default: Fatal( MSG_INV_OPT, argv[j] );
                }
            }
            for(i=j;i<argc;i++) argv[i]=argv[i+1];
            argc--;
        }
        j--;
    }

    if( !quiet ) {
        Banner();
    }

    if( argc < 2 || argc > 4 )  Usage();
    if( argv[1][0] == '?' && argv[1][1] == '\0' ) Usage();
    bufsize = BUFSIZE;
    while( ( Buffer = malloc( bufsize ) ) == NULL ) {
        size = bufsize & (bufsize - 1);
        bufsize = size ? size : ( (bufsize << 1) | (bufsize << 2) );
        if( bufsize < MXFNAME )  Fatal( MSG_LOW_MEM, NULL );
    }
    finfo.name[0] = '\0';
    for( i = 0;; ++i ) {
        if( ExtLst[ i ] == NULL ) {
            Fatal( MSG_CANT_FIND, argv[1] );
        }
        strcpy( fin.name, argv[ 1 ] );
        has_ext = Suffix( fin.name, ExtLst[ i ] );
        if( stat( fin.name, &in_stat ) == 0 ) break;
        if( has_ext ) break;
    }
    if( argc >= 3 && strcmp( argv[2], "." ) != 0 ) {
        if( stat(argv[2],&other_stat)==0 && S_ISDIR(other_stat.st_mode) ) {
            _splitpath2( fin.name, fbuff, &drive, &dir, &name, &ext );
            _makepath( fout.name, NULL, argv[2], name, ext );
        } else {
            strcpy( fout.name, argv[2] );
            _splitpath2( fin.name, fbuff, &drive, &dir, &name, &ext );
            Suffix( fout.name, ext );
        }
    } else {
        strcpy( fout.name, fin.name );
    }
    if( argc >= 4 ) {
        if( stat(argv[3],&other_stat)==0 && S_ISDIR(other_stat.st_mode) ) {
            _splitpath2( fout.name, fbuff, &drive, &dir, &name, &ext );
            _makepath( finfo.name, NULL, argv[3], name, NULL );
        } else {
            strcpy( finfo.name, argv[3] );
        }
        Suffix( finfo.name, (res ? ResExt : SymExt) );
    }
    finfo.h = -1;

    /* initialize input file */
    fin.h = sopen( fin.name, O_RDONLY | O_BINARY, SH_DENYNO, 0 );
    if( fin.h == -1 ) {
        Fatal( MSG_CANT_OPEN, fin.name );
    }

    /* initialize output file -- note: don't truncate */
    fout.h = sopen( fout.name, O_WRONLY | O_CREAT | O_BINARY,
                SH_DENYNO, S_IRWXU | S_IRWXG | S_IRWXO );
    if( fout.h == -1 ) {
        Fatal( MSG_CANT_CREATE_OUTPUT, fout.name );
    }

    if( add_file ) {
        AddInfo();
    } else {
        StripInfo();
    }
    /* make sure that size of output file is correct */
    chsize( fout.h, lseek( fout.h, 0L, SEEK_CUR ) );

    close( fin.h );
    close( fout.h );
    if( finfo.h != -1 ) {
        close( finfo.h );
    }

    uptime.actime = time( NULL );
    uptime.modtime = in_stat.st_mtime;
    utime( fout.name, &uptime );

    Msg_Fini();
    return( EXIT_SUCCESS );
}

static void CopyData( fdata *in, fdata *out, unsigned long max )
{
    size_t              size;

    for( ;; ) {
        if( max == 0 ) break;
        size = ( max > (unsigned long)bufsize ) ? bufsize : max;
        size = read( in->h, Buffer, size );
        if( size == 0 ) break;
        if( size == -1 ) {
            FatalDelTmp( MSG_READ_ERROR, in->name );
        }
        if( size != write( out->h, Buffer, size ) ) {
            FatalDelTmp( MSG_WRITE_ERROR, out->name );
        }
        max -= (unsigned long)size;
    }
}

static void AddInfo()
{
    info_info           info;
    master_dbg_header   header;

    if( finfo.name[0] == '\0' ) {
        Fatal( MSG_NO_SPECIFIED_0 + res, NULL );
    }

    FindInfoInfo( fin.h, &info, res );
    if( info.type != WRAP_NONE ) {
        Fatal( MSG_HAS_INFO_0 + res, fin.name );
    }

    /* initialize symbol or resource file */
    finfo.h = sopen( finfo.name, O_RDONLY | O_BINARY, SH_DENYWR, 0 );
    if( finfo.h == -1 ) {
        FatalDelTmp( MSG_CANT_OPEN, finfo.name );
    }
    if( !IsResMagic( finfo.h, res ) && !IsSymResFile( finfo.h, res ) ) {
        FatalDelTmp( MSG_INV_FILE_0 + res, finfo.name );
    }
    if( strcmp( fin.name, fout.name ) != 0 ) {
        if( lseek( fin.h, 0L, SEEK_SET ) == -1L ) {
            Fatal( MSG_SEEK_ERROR, fin.name );
        }
        CopyData( &fin, &fout, ~0L );
    }

    /* transfer info file to output file */
    lseek( finfo.h, 0L, SEEK_SET );
    lseek( fout.h, 0L, SEEK_END );
    CopyData( &finfo, &fout, ~0L );

    /* add header (trailer), if required */
    if( res ) {
        info.len = lseek( finfo.h, -(int)sizeof( header ), SEEK_END ) + sizeof( header );
        read( finfo.h, (void*)&header, sizeof(header) );
        if( header.signature != WAT_RES_SIG || header.debug_size != info.len ) {
            header.signature = WAT_RES_SIG;
            header.debug_size = info.len + sizeof(header);
            if( write( fout.h, (void*)&header, sizeof(header) ) != sizeof(header) ) {
                FatalDelTmp( MSG_ADD_HEADER_ERROR, NULL );
            }
        }
    }
}

static void StripInfo()
{
    info_info           info;

    FindInfoInfo( fin.h, &info, res );
    if( info.type == WRAP_NONE ) {
        if( !nodebug_ok ) {
            Fatal( MSG_NO_INFO_0 + res, fin.name );
        }
    }

    if( finfo.name[0] != '\0' && info.type != WRAP_NONE ) {
#ifdef __QNX__
        if( strcmp( finfo.name, fout.name ) == 0 ) {
            strcat( finfo.name, (res ? ResExt : SymExt) );
        }
#endif
        finfo.h = sopen( finfo.name, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
              SH_DENYRW, S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP| S_IROTH|S_IWOTH );
        if( finfo.h == -1 ) {
            FatalDelTmp( MSG_CANT_CREATE_0 + res, finfo.name );
        }
    }

    /* transfer executable file upto start of debugging info */
    if( strcmp( fin.name, fout.name ) != 0 ) {
        if( lseek( fin.h, 0L, SEEK_SET ) == -1L ) {
            Fatal( MSG_SEEK_ERROR, fin.name );
        }
        CopyData( &fin, &fout, info.start );
    } else {
        lseek( fin.h, info.start, SEEK_SET );
        lseek( fout.h, info.start, SEEK_SET );
    }

    if( finfo.h != -1 ) {
        /* transfer data to info file */
        CopyData( &fin, &finfo, info.len );
    } else {
        /* else skip it */
        lseek( fin.h, info.len, SEEK_CUR );
    }

    /* transfer remaining data */
    CopyData( &fin, &fout, ~0L );
}


static int Suffix( char *fname, char *suff )
{
    char *scan;
    char *end;

    end = &fname[ strlen( fname ) ];
    scan = end;
    for( ;; ) {
        --scan;
        if( scan <= fname ) break;
        if( *scan == '/' ) break;
#if !defined( __QNX__ )
        if( *scan == '\\' ) break;
#endif
        if( *scan == '.' ) return( 0 ); /* already has an extension */
    }
    strcpy( end, suff );
    return( 1 );
}


static int IsSymResFile( int handle, int resfile )
{
    master_dbg_header   header;
    info_info           info;

    lseek( handle, -(int)sizeof( header ), SEEK_END );
    if( read( handle, (void*)&header, sizeof(header) ) != sizeof(header) ) return(0);
    if( header.signature == (resfile ? WAT_RES_SIG : VALID_SIGNATURE) &&
         lseek( handle, 0L, SEEK_END ) == header.debug_size ) return(1);
    if( resfile ) return( 0 );
    FindInfoInfo( handle, &info, resfile );
    return( info.type != WRAP_NONE );
}


static int IsResMagic( int handle, int resfile )
{
    WResHeader          wheader;

    if( resfile ){
        lseek( handle, 0L, SEEK_SET );
        if(read(handle,&wheader,sizeof(wheader)) != sizeof(wheader)) return(0);
        if( (wheader.Magic[0] == WRESMAGIC0)
                                || (wheader.Magic[0] == WRESMICRO0) ) {
            if( (wheader.Magic[1] == WRESMAGIC1) ||
                        (wheader.Magic[1] >> 16 == WRESMICRO1) ) return (1);
        }
    }
    return (0);
}

static int TryWATCOM( int h, info_info *info, int resfile )
{
    master_dbg_header   header;
    unsigned long       end;

    end = lseek( h, -(int)sizeof( header ), SEEK_END );
    for( ;; ) {
        if( read( h, (void*)&header, sizeof(header) ) != sizeof(header) ) return(0);
        if( header.signature != FOX_SIGNATURE1
            && header.signature != FOX_SIGNATURE2
            && header.signature != (resfile ? VALID_SIGNATURE : WAT_RES_SIG) ) break;
        if( header.debug_size > end ) return(0);
        end = lseek( h, end - header.debug_size, SEEK_SET );
    }
    if( header.signature != (resfile ? WAT_RES_SIG : VALID_SIGNATURE) ) return(0);
    end += sizeof( header );
    if( end <= header.debug_size ) return(0);
    end -= header.debug_size;
    info->start = end;
    info->len = header.debug_size;
    info->type = WRAP_WATCOM;
    return( 1 );
}

static int TryTIS( int h, info_info *info )
{
    TISTrailer          head;
    unsigned long       end;

    end = lseek( h, -(int)sizeof( head ), SEEK_END );
    for(;;){
        if( read( h, &head, sizeof(head)) != sizeof(head) ) {
            return( 0 );
        }
        if( head.signature != TIS_TRAILER_SIGNATURE ) {
            return( 0 );
        }
        end -= head.size - sizeof( head );
        if( head.vendor == TIS_TRAILER_VENDOR_TIS
         && head.type == TIS_TRAILER_TYPE_TIS_DWARF ) break;
        lseek( h, end, SEEK_SET );
    }
    info->start = end;
    info->len = head.size;
    info->type = WRAP_TIS;
    return( 1 );
}

static int TryCV4( int h, info_info *info )
{
    cv_trailer          head;
    unsigned long       pos;

    pos = lseek( h, -(long)sizeof( head ), SEEK_END );
    if( read( h, &head, sizeof( head ) ) != sizeof( head ) ) {
        return( 0 );
    }
    if( memcmp( head.sig, CV4_NB09, sizeof( head.sig ) ) != 0
     && memcmp( head.sig, CV4_NB08, sizeof( head.sig ) ) != 0
     && memcmp( head.sig, CV4_NB07, sizeof( head.sig ) ) != 0
     && memcmp( head.sig, CV4_NB05, sizeof( head.sig ) ) != 0 ) {
        return( 0 );
    }
    info->len = head.offset;
    info->start = pos + sizeof( head ) - head.offset;
    info->type = WRAP_CV4;
    return( 1 );
}

static void FindInfoInfo( int h, info_info *info, int resfile )
{

    info->type = WRAP_NONE;
    info->start = 0;
    info->len = 0;
    if( TryWATCOM( h, info, resfile ) ) return;
    if( resfile ) return;
    if( TryTIS( h, info ) ) return;
    TryCV4( h, info );
}


static void FatalDelTmp( int reason, char *insert )
{
    Fatal( reason, insert );
}
