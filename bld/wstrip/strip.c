/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Executable strip utility.
*
****************************************************************************/


#include <stdio.h>
#include <stddef.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/types.h>
#if defined(__UNIX__) || defined( __WATCOMC__ )
    #include <utime.h>
#else
    #include <sys/utime.h>
#endif
#include "wio.h"
#include "sopen.h"
#include "machtype.h"
#include "wdbginfo.h"
#include "cv4.h"
#include "tistrail.h"
#include "wstrip.h"
#include "wressetr.h"
#include "pathgrp2.h"

#include "clibext.h"


#define MXFNAME         130
#define BUFSIZE         0x4000

#define WRESMICRO0      0xFF0006FF
#define WRESMICRO1      0x1030
#define WRESMAGIC0      0xC3D4C1D7
#define WRESMAGIC1      0xC3D2CDCF

#define SEEK_POSBACK(p) (-(long)(p))

#define CopyData(i,o)   CopyDataLen( i, o, ~0UL )

#define ARRAYSIZE(a)    (sizeof( a ) / sizeof( a[0] ))

#include "pushpck1.h"
typedef struct WResHeader {
    uint_32     Magic[2];       /* must be WRESMAGIC0 and WRESMAGIC1 */
    uint_32     DirOffset;      /* offset to the start of the directory */
    uint_16     NumResources;   /* number of resourses in the file */
    uint_16     NumTypes;       /* number of different types of resources in file */
    uint_16     WResVer;        /* WRESVERSION */
} WResHeader;
#include "poppck.h"

typedef enum {
    WRAP_NONE,
    WRAP_WATCOM,
    WRAP_CV4,
    WRAP_TIS
} wrapper_type;

typedef struct {
    wrapper_type    type;
    unsigned long   start;
    unsigned long   len;
} info_info;

typedef struct {
    FILE        *fp;
    char        name[_MAX_PATH];
} fdata;

static char *Buffer;

static const char *ExtLst[] = {
#ifdef __UNIX__
    "", ".qnx",
#endif
    ".exe", ".dll",
    ".exp", ".rex",
    ".nlm", ".dsk", ".lan", ".nam",
#ifndef __UNIX__
    ".qnx", "",
#endif
};

static const char SymExt[] = { ".sym" };
static const char ResExt[] = { ".res" };

static fdata   finfo, fin, fout, ftmp;

static bool quiet = false;
static bool nodebug_ok = false;
static bool res = false;
static size_t bufsize;

static void FatalDelTmp( int reason, const char *insert )
{
    Fatal( reason, insert );
}

static void CopyDataLen( fdata *in, fdata *out, unsigned long max )
{
    size_t              size;

    size = bufsize;
    for( ; max != 0; max -= (unsigned long)size ) {
        if( size > max )
            size = (size_t)max;
        size = fread( Buffer, 1, size, in->fp );
        if( ferror( in->fp ) )
            FatalDelTmp( MSG_READ_ERROR, in->name );
        if( size == 0 )
            break;
        if( size != fwrite( Buffer, 1, size, out->fp ) ) {
            FatalDelTmp( MSG_WRITE_ERROR, out->name );
        }
    }
}

static bool TryWATCOM( FILE *fp, info_info *info, bool resfile )
{
    master_dbg_header   header;
    unsigned long       end;

    if( fseek( fp, SEEK_POSBACK( sizeof( header ) ), SEEK_END ) )
        return( false );
    end = ftell( fp );
    for( ;; ) {
        if( fread( (void *)&header, 1, sizeof( header ), fp ) != sizeof( header ) )
            return( false );
        if( header.signature != FOX_SIGNATURE1
          && header.signature != FOX_SIGNATURE2
          && header.signature != (resfile ? WAT_DBG_SIGNATURE : WAT_RES_SIG) )
            break;
        if( header.debug_size > end )
            return( false );
        end -= header.debug_size;
        fseek( fp, end, SEEK_SET );
    }
    if( header.signature != (resfile ? WAT_RES_SIG : WAT_DBG_SIGNATURE) )
        return( false );
    end += sizeof( header );
    if( end <= header.debug_size )
        return( false );
    end -= header.debug_size;
    info->start = end;
    info->len = header.debug_size;
    info->type = WRAP_WATCOM;
    return( true );
}

static bool TryTIS( FILE *fp, info_info *info )
{
    TISTrailer      head;
    long            end;

    if( fseek( fp, SEEK_POSBACK( sizeof( head ) ), SEEK_END ) )
        return( false );
    end = ftell( fp );
    for( ;; ) {
        if( fread( &head, 1, sizeof( head ), fp ) != sizeof( head ) ) {
            return( false );
        }
        if( head.signature != TIS_TRAILER_SIGNATURE ) {
            return( false );
        }
        end -= head.size - sizeof( head );
        if( head.vendor == TIS_TRAILER_VENDOR_TIS && head.type == TIS_TRAILER_TYPE_TIS_DWARF )
            break;
        fseek( fp, end, SEEK_SET );
    }
    info->start = end;
    info->len = head.size;
    info->type = WRAP_TIS;
    return( true );
}

static bool TryCV4( FILE *fp, info_info *info )
{
    cv_trailer          head;
    unsigned long       pos;

    if( fseek( fp, SEEK_POSBACK( sizeof( head ) ), SEEK_END ) )
        return( false );
    pos = ftell( fp );
    if( fread( &head, 1, sizeof( head ), fp ) != sizeof( head ) ) {
        return( false );
    }
    if( memcmp( head.sig, CV4_NB09, sizeof( head.sig ) ) != 0
      && memcmp( head.sig, CV4_NB08, sizeof( head.sig ) ) != 0
      && memcmp( head.sig, CV4_NB07, sizeof( head.sig ) ) != 0
      && memcmp( head.sig, CV4_NB05, sizeof( head.sig ) ) != 0 ) {
        return( false );
    }
    info->len = head.offset;
    info->start = pos + sizeof( head ) - head.offset;
    info->type = WRAP_CV4;
    return( true );
}

static void FindInfoInfo( FILE *fp, info_info *info, bool resfile )
{

    info->type = WRAP_NONE;
    info->start = 0;
    info->len = 0;
    if( TryWATCOM( fp, info, resfile ) )
        return;
    if( resfile )
        return;
    if( TryTIS( fp, info ) )
        return;
    TryCV4( fp, info );
}

static bool IsSymResFile( FILE *fp, bool resfile )
{
    master_dbg_header   header;
    info_info           info;
    unsigned long       end;

    if( fseek( fp, SEEK_POSBACK( sizeof( header ) ), SEEK_END ) )
        return( false );
    end = ftell( fp ) + sizeof( header );
    if( fread( (void *)&header, 1, sizeof( header ), fp ) != sizeof( header ) )
        return( false );
    if( header.signature == (resfile ? WAT_RES_SIG : WAT_DBG_SIGNATURE) && end == header.debug_size )
        return( true );
    if( resfile )
        return( false );
    FindInfoInfo( fp, &info, resfile );
    return( info.type != WRAP_NONE );
}


static bool IsResMagic( FILE *fp, bool resfile )
{
    WResHeader          wheader;

    if( resfile ) {
        fseek( fp, 0, SEEK_SET );
        if( fread( &wheader, 1, sizeof( wheader ), fp ) != sizeof( wheader ) )
            return( false );
        if( (wheader.Magic[0] == WRESMAGIC0) || (wheader.Magic[0] == WRESMICRO0) ) {
            if( (wheader.Magic[1] == WRESMAGIC1) || (wheader.Magic[1] >> 16 == WRESMICRO1) ) {
                return( true );
            }
        }
    }
    return( false );
}

static void AddInfo( void )
{
    info_info           info;
    master_dbg_header   header;

    if( finfo.name[0] == '\0' ) {
        Fatal( ( res ) ? MSG_NO_SPECIFIED_1 : MSG_NO_SPECIFIED_0, NULL );
    }

    FindInfoInfo( fin.fp, &info, res );
    if( info.type != WRAP_NONE ) {
        Fatal( ( res ) ? MSG_HAS_INFO_1 : MSG_HAS_INFO_0, fin.name );
    }

    /* initialize symbol or resource file */
    finfo.fp = fopen( finfo.name, "rb" );
    if( finfo.fp == NULL )
        FatalDelTmp( MSG_CANT_OPEN, finfo.name );
    if( !IsResMagic( finfo.fp, res ) && !IsSymResFile( finfo.fp, res ) )
        FatalDelTmp( ( res ) ? MSG_INV_FILE_1 : MSG_INV_FILE_0, finfo.name );

    /* transfer input file to output file */
    fseek( fin.fp, 0, SEEK_SET );
    CopyData( &fin, &ftmp );
    /* transfer info file to output file */
    fseek( finfo.fp, 0, SEEK_SET );
    CopyData( &finfo, &ftmp );

    /* add header (trailer), if required */
    if( res ) {
        if( fseek( finfo.fp, SEEK_POSBACK( sizeof( header ) ), SEEK_END ) )
            Fatal( MSG_SEEK_ERROR, finfo.name );
        info.len = ftell( finfo.fp ) + sizeof( header );
        if( fread( (void *)&header, 1, sizeof( header ), finfo.fp ) != sizeof( header ) )
            Fatal( MSG_READ_ERROR, finfo.name );
        if( header.signature != WAT_RES_SIG || header.debug_size != info.len ) {
            header.signature = WAT_RES_SIG;
            header.debug_size = info.len + sizeof( header );
            if( fwrite( (void *)&header, 1, sizeof( header ), ftmp.fp ) != sizeof( header ) ) {
                FatalDelTmp( MSG_ADD_HEADER_ERROR, NULL );
            }
        }
    }
    fclose( finfo.fp );
}

static void StripInfo( void )
{
    info_info           info;

    FindInfoInfo( fin.fp, &info, res );
    if( info.type == WRAP_NONE ) {
        if( !nodebug_ok ) {
            Fatal( ( res ) ? MSG_NO_INFO_1 : MSG_NO_INFO_0, fin.name );
        }
    }
    finfo.fp = NULL;
    if( finfo.name[0] != '\0' && info.type != WRAP_NONE ) {
        finfo.fp = fopen( finfo.name, "wb" );
        if( finfo.fp == NULL ) {
            FatalDelTmp( ( res ) ? MSG_CANT_CREATE_1 : MSG_CANT_CREATE_0, finfo.name );
        }
    }

    /* transfer executable file upto start of debugging info */
    fseek( fin.fp, 0, SEEK_SET );
    CopyDataLen( &fin, &ftmp, info.start );

    if( finfo.fp != NULL ) {
        /* transfer data to info file */
        CopyDataLen( &fin, &finfo, info.len );
        fclose( finfo.fp );
    } else {
        /* else skip it */
        fseek( fin.fp, info.len, SEEK_CUR );
    }

    /* transfer remaining data */
    CopyData( &fin, &ftmp );
}

int main( int argc, char *argv[] )
{
    size_t              size;
    int                 i;
    int                 j;
    size_t              k;
    size_t              argvlen;
    pgroup2             *pg;
    pgroup2             *pg_tmp;
    int                 add_file;
    struct stat         statx;
    struct utimbuf      uptime;
    time_t              mtime;
    const char          *in_ext;

#define IS_DIR(x,st)    (stat(x, &(st)) == 0 && S_ISDIR( (st).st_mode ))

#ifndef __WATCOMC__
    _argv = argv;
    _argc = argc;
#endif

    if( !Msg_Init() ) {
        return( EXIT_FAILURE );
    }
    add_file = 0;
    j = argc - 1;
    while( j > 0 ) {
#ifdef __UNIX__
        if( argv[j][0] == '-' ) {
#else
        if( argv[j][0] == '-' || argv[j][0] == '/' ) {
#endif
            argvlen = strlen( argv[j] );
            for( k = 1; k < argvlen; k++ ) {
                switch( argv[j][k] ) {
                case '?':
                case 'h':
                    Usage();
                    break;
                case 'q': quiet = true; break;
                case 'n': nodebug_ok = true; break;
                case 'a': add_file = true; break;
                case 'r': res = true; break;
                default: Fatal( MSG_INV_OPT, argv[j] );
                }
            }
            for( i = j; i < argc; i++ )
                argv[i] = argv[i + 1];
            argc--;
        }
        j--;
    }

    if( !quiet ) {
        Banner();
    }

    if( argc < 2 || argc > 4 )
        Usage();
    if( argv[1][0] == '?' && argv[1][1] == '\0' )
        Usage();
    bufsize = BUFSIZE;
    while( ( Buffer = malloc( bufsize ) ) == NULL ) {
        size = bufsize & (bufsize - 1);
        bufsize = size ? size : ( (bufsize << 1) | (bufsize << 2) );
        if( bufsize < MXFNAME ) {
            Fatal( MSG_LOW_MEM, NULL );
        }
    }
    pg = malloc( sizeof( pgroup2 ) );
    if( pg == NULL ) {
        Fatal( MSG_LOW_MEM, NULL );
    }
    _splitpath2( argv[1], pg->buffer, &pg->drive, &pg->dir, &pg->fname, &pg->ext );
    in_ext = pg->ext;
    mtime = 0;
    if( in_ext[0] == '\0' ) {
        for( i = 0; i < ARRAYSIZE( ExtLst ); ++i ) {
            in_ext = ExtLst[i];
            _makepath( fin.name, pg->drive, pg->dir, pg->fname, in_ext );
            if( stat( fin.name, &statx ) == 0 ) {
                mtime = statx.st_mtime;
                break;
            }
        }
    } else {
        _makepath( fin.name, pg->drive, pg->dir, pg->fname, in_ext );
        if( stat( fin.name, &statx ) == 0 ) {
            mtime = statx.st_mtime;
        }
    }
    if( mtime == 0 ) {
        free( pg );
        Fatal( MSG_CANT_FIND, argv[1] );
    }
    pg_tmp = malloc( sizeof( pgroup2 ) );
    if( pg_tmp == NULL ) {
        free( pg );
        Fatal( MSG_LOW_MEM, NULL );
    }
    if( argc >= 3 && strcmp( argv[2], "." ) != 0 ) {
        _splitpath2( argv[2], pg_tmp->buffer, &pg->drive, &pg->dir, &pg_tmp->fname, &pg_tmp->ext );
        _makepath( ftmp.name, pg->drive, pg->dir, pg_tmp->fname, pg_tmp->ext );
        if( !IS_DIR( ftmp.name, statx ) ) {
            pg->fname = pg_tmp->fname;
        }
    }
    _makepath( fout.name, pg->drive, pg->dir, pg->fname, in_ext );
    finfo.name[0] = '\0';
    if( argc >= 4 ) {
        _splitpath2( argv[3], pg_tmp->buffer, &pg_tmp->drive, &pg_tmp->dir, &pg_tmp->fname, &pg_tmp->ext );
        _makepath( ftmp.name, pg_tmp->drive, pg_tmp->dir, pg_tmp->fname, pg_tmp->ext );
        if( IS_DIR( ftmp.name, statx ) ) {
            pg_tmp->fname = pg->fname;
        }
        _makepath( finfo.name, pg_tmp->drive, pg_tmp->dir, pg_tmp->fname, (res ? ResExt : SymExt) );
    }
    free( pg_tmp );
    free( pg );

    /* initialize temporary file */
    strcpy( ftmp.name, "temporary file" );
    ftmp.fp = tmpfile();
    if( ftmp.fp == NULL ) {
        Fatal( MSG_CANT_OPEN, ftmp.name );
    }

    /* initialize input file */
    fin.fp = fopen( fin.name, "rb" );
    if( fin.fp == NULL ) {
        Fatal( MSG_CANT_OPEN, fin.name );
    }
    if( add_file ) {
        AddInfo();
    } else {
        StripInfo();
    }
    fclose( fin.fp );
    /* initialize output file, overwrite if exists */
    fout.fp = fopen( fout.name, "wb" );
    if( fout.fp == NULL ) {
        Fatal( MSG_CANT_CREATE_OUTPUT, fout.name );
    }
    /* copy temporary file to output executable file */
    fseek( ftmp.fp, 0, SEEK_SET );
    CopyData( &ftmp, &fout );
    fclose( fout.fp );
    fclose( ftmp.fp );

    uptime.actime = time( NULL );
    uptime.modtime = mtime;
    utime( fout.name, &uptime );

    Msg_Fini();
    return( EXIT_SUCCESS );
}
