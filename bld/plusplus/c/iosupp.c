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
* Description:  I/O support routines.
*
****************************************************************************/


#include "plusplus.h"
#include <stdarg.h>
#if defined(__UNIX__)
 #include <dirent.h>
#else
 #include <direct.h>
#endif
#include "preproc.h"
#include "memmgr.h"
#include "iosupp.h"
#include "cgdata.h"
#include "fname.h"
#include "hfile.h"
#include "initdefs.h"
#include "stats.h"
#include "pcheader.h"
#include "ring.h"
#include "brinfo.h"
#include "autodept.h"
#include "iopath.h"
#include "sysdep.h"
#include "ialias.h"
#include "pathgrp2.h"

#include "clibext.h"


typedef struct buf_alloc BUF_ALLOC;
struct buf_alloc {              // BUF_ALLOC -- allocated buffer
    BUF_ALLOC* next;            // - next buffer allocation
    FILE* file;                 // - file in question
    void* buffer;               // - allocated buffer
};


static carve_t      carve_buf;      // carver: BUF_ALLOC
static BUF_ALLOC    *buffers;       // allocated buffers
static FILE         *tempfileFP;    // file ptr for temp file
static DISK_ADDR    tempBlock;      // next available block in temp file
static unsigned     outFileChecked; // mask for checking output files


#if defined(__OS2__) || defined(__DOS__) || defined(__NT__) || defined(__RDOS__)

static const char* pathSrc[] =        // paths for source file
    {   "..\\cpp"
    ,   "..\\c"
    ,   NULL
    };

static const char* pathHdr[] =        // paths for header files
    {   "..\\h"
    ,   "..\\include"
    ,   NULL
    };

static const char* pathCmd[] =        // paths for command files
    {   "..\\occ"
    ,   NULL
    };

static const char* extsHdr[] =        // extensions for header files
    {   ".hpp"
    ,   ".h"
    ,   NULL
    };

static const char* extsSrc[] =        // extensions for source files
    {   ".cpp"
    ,   ".cc"
    ,   ".c"
    ,   NULL
    };

static const char* extsCmd[] =        // extensions for command files
    {   ".occ"
    ,   NULL
    };

static const char* extsOut[] = {      // extensions for output files
    #define pick(e,u,o)     o,
        OFT_DEFS()
#ifdef OPT_BR
        OFT_BRI_DEFS()
#endif
    #undef pick
};

#elif defined(__UNIX__)

static const char* pathSrc[] =        // paths for source file
    {   "../C"
    ,   "../cpp"
    ,   "../c"
    ,   NULL
    };

static const char* pathHdr[] =        // paths for header files
    {   "../H"
    ,   "../h"
    ,   NULL
    };

static const char* pathCmd[] =        // paths for command files
    {   "../occ"
    ,   NULL
    };

static const char* extsHdr[] =        // extensions for header files
    {   ".H"
    ,   ".hpp"
    ,   ".h"
    ,   NULL
    };

static const char* extsSrc[] =        // extensions for source files
    {   ".C"
    ,   ".cpp"
    ,   ".cc"
    ,   ".c"
    ,   NULL
    };

static const char* extsCmd[] =        // extensions for command files
    {   ".occ"
    ,   NULL
    };

static const char* extsOut[] = {      // extensions for output files
    #define pick(e,u,o)     u,
        OFT_DEFS()
#ifdef OPT_BR
        OFT_BRI_DEFS()
#endif
    #undef pick
};

#else

#error IOSUPP not configured for OS

#endif

static char *FNameBuf = NULL;   // file name buffer for output files

char *IoSuppOutFileName(        // BUILD AN OUTPUT NAME FROM SOURCE NAME
    out_file_type typ )         // - extension
{
    pgroup2 pg1;
    pgroup2 pg2;
    const char *ext;
    char *path;
    bool use_defaults;
    unsigned mask;
    FILE *try_create;

    path = WholeFName;
    use_defaults = true;
    switch( typ ) {
    case OFT_DEF:
#ifdef OPT_BR
    case OFT_BRI:
#endif
        break;
    case OFT_DEP:
        if( DependFileName != NULL ) {
            path = DependFileName;
            use_defaults = false;
        }
        break;
    case OFT_ERR:
        if( ErrorFileName == NULL )
            return( NULL );
        outFileChecked |= 1 << typ; // don't create a file. it's just a name.
        path = ErrorFileName;
        use_defaults = false;
        break;
    case OFT_SRCDEP:
        outFileChecked |= 1 << typ;
        if( SrcDepFileName != NULL ) {
            path = SrcDepFileName;
        }
        break;
    case OFT_TRG:
        outFileChecked |= 1 << typ; // don't create a file. it's just a name.
        if( TargetFileName != NULL ) {
            path = TargetFileName;
            use_defaults = false;
            break;
        }
        /* fall through */
    case OFT_PPO:
    case OFT_OBJ:
    case OFT_MBR:
        if( ObjectFileName != NULL ) {
            path = ObjectFileName;
            if( typ != OFT_MBR ) {
                use_defaults = false;
            }
        }
        break;
    }
    _splitpath2( path, pg1.buffer, &pg1.drive, &pg1.dir, &pg1.fname, &pg1.ext );
    if( use_defaults ) {
        pg1.drive = "";
        pg1.dir = "";
    }
    if( pg1.fname[0] == '\0' || pg1.fname[0] == '*' ) {
        pg1.fname = ModuleName;
    }
    ext = pg1.ext;
    if( typ == OFT_SRCDEP ) {
        if( pg1.ext[0] == '\0' ) {
            if( SrcDepFileName != NULL ) {
                _splitpath2( WholeFName, pg2.buffer, NULL, NULL, NULL, &pg2.ext );
                ext = pg2.ext;
            }
        }
    } else if( use_defaults || ext[0] == '\0' ) {
        ext = extsOut[typ];
    }
    _makepath( FNameBuf, pg1.drive, pg1.dir, pg1.fname, ext );
    mask = 1 << typ;
    if( (outFileChecked & mask) == 0 ) {
        outFileChecked |= mask;
        try_create = fopen( FNameBuf, "w" );
        if( try_create != NULL ) {
            fclose( try_create );
        } else {
            CErr2p( ERR_CANNOT_CREATE_OUTPUT_FILE, FNameBuf );
        }
    }
    return( FNameBuf );
}


static void set_buffering(      // SET BUFFERING FOR AN OPEN FILE
    FILE *fp,                   // - opened file
    size_t buf_size,            // - buffer size
    int mode )                  // - buffering mode
{
    BUF_ALLOC* ba = RingCarveAlloc( carve_buf, &buffers );
    ba->file = fp;
    ba->buffer = CMemAlloc( buf_size );
    setvbuf( fp, ba->buffer, mode, buf_size );
}


void IoSuppSetBuffering(        // SET FULL BUFFERING FOR AN OPEN FILE
    FILE *fp,                   // - opened file
    size_t buf_size )           // - buffer size
{
    set_buffering( fp, buf_size, _IOFBF );
}


void IoSuppSetLineBuffering(    // SET LINE BUFFERING FOR AN OPEN FILE
    FILE *fp,                   // - opened file
    size_t buf_size )           // - buffer size
{
    set_buffering( fp, buf_size, _IOLBF );
}


static void freeBuffer(         // FREE A BUFFER
    BUF_ALLOC* ba )             // - allocated buffer
{
    CMemFree( ba->buffer );
    RingPrune( &buffers, ba );
    CarveFree( carve_buf, ba );
}


bool IoSuppCloseFile(           // CLOSE FILE IF OPENED
    FILE **file_ptr )           // - addr( file pointer )
{
    bool ok;                    // - return: true ==> was open
    BUF_ALLOC* ba;              // - current allocated buffer

    if( *file_ptr == NULL ) {
        ok = false;
    } else {
        RingIterBegSafe( buffers, ba ) {
            if( *file_ptr == ba->buffer ) {
                freeBuffer( ba );
                break;
            }
        } RingIterEndSafe( ba );
        SrcFileFClose( *file_ptr );
        *file_ptr = NULL;
        ok = true;
    }
    return( ok );
}


static bool openSrc(            // ATTEMPT TO OPEN FILE
    char *name,                 // - file name
    src_file_type typ )         // - type of file being opened
{
    pch_absorb pch_OK;          // - pre-compiled header load status
    FILE *fp;                   // - file pointer
    time_t ftime;
#ifdef OPT_BR
    bool might_browse;          // - true ==> might browse, if right file type
#endif

    if( SrcFileProcessOnce( name ) ) {
        SrcFileOpen( NULL, name, 0 );
        return( true );
    }
    ftime = SysFileTime( name );
    fp = SrcFileFOpen( name, SFO_SOURCE_FILE );
    if( fp == NULL ) {
        return( false );
    }
#ifdef OPT_BR
    might_browse = false;
#endif
    if( CompFlags.watch_for_pcheader ) {
        CompFlags.watch_for_pcheader = false;
        pch_OK = PCHeaderAbsorb( name );
        if( pch_OK != PCHA_OK ) {
            SrcFileSetCreatePCHeader();
            SrcFileOpen( fp, name, ftime );
#ifdef OPT_BR
            might_browse = true;
#endif
        } else {
            SrcFileOpen( NULL, name, 0 );
            fclose( fp );
        }
    } else {
        SrcFileOpen( fp, name, ftime );
        if( typ == FT_SRC ) {
            SetSrcFilePrimary();
        }
#ifdef OPT_BR
        might_browse = true;
#endif
    }
#ifdef OPT_BR
    if( might_browse ) {
        switch( typ ) {
        case FT_SRC:
        case FT_LIBRARY:
        case FT_HEADER:
        case FT_HEADER_FORCED:
        case FT_HEADER_PRE:
            BrinfOpenSource( SrcFileCurrent() );
            break;
        }
    }
#endif
    return( true );
}


static const char *openSrcExt(  // ATTEMPT TO OPEN FILE (EXT. TO BE APPENDED)
    const char *ext,            // - extension
    pgroup2 *nd,                // - name descriptor
    src_file_type typ )         // - type of file being opened
{
    char name[_MAX_PATH];       // - buffer for file name

    _makepath( name, nd->drive, nd->dir, nd->fname, ext );
    /* so we can tell if the open worked */
    if( openSrc( name, typ ) )
        return( (ext != NULL) ? ext : "" );
    return( NULL );
}


static const char *openSrcExts( // ATTEMPT TO OPEN FILE (EXT.S TO BE APPENDED)
    const char **exts,          // - extensions
    pgroup2 *nd,                // - name descriptor
    src_file_type typ )         // - type of file being opened
{
    const char *ext;            // - current extension

    ext = openSrcExt( nd->ext, nd, typ );
    if( ext == NULL ) {
        if( nd->ext[0] == '\0' ) {
            switch( typ ) {
            case FT_SRC:
                if( CompFlags.dont_autogen_ext_src )
                    exts = NULL;
                break;
            case FT_HEADER:
            case FT_HEADER_FORCED:
            case FT_HEADER_PRE:
            case FT_LIBRARY:
                if( CompFlags.dont_autogen_ext_inc )
                    exts = NULL;
                break;
            default:
                break;
            }
            if( exts != NULL ) {
                while( (ext = *exts++) != NULL ) {
                    ext = openSrcExt( ext, nd, typ );
                    if( ext != NULL ) {
                        break;
                    }
                }
            }
        }
    }
    return( ext );
}


static bool openSrcPath(        // ATTEMPT TO OPEN FILE (PATH TO BE PREPENDED)
    const char *path,           // - path
    pgroup2 *fd,                // - file descriptor
    const char **exts,          // - file extensions
    src_file_type typ )         // - type of file being opened
{
    bool ok;                    // - return: true ==> opened
    pgroup2 pd;                 // - path descriptor
    char dir[_MAX_PATH * 2];    // - new path
    char *pp;                   // - pointer into path
    const char *ext;            // - extension opened

    ok = false;
    dir[0] = '\0';
    _splitpath2( path, pd.buffer, &pd.drive, &pd.dir, &pd.fname, &pd.ext );
    if( fd->drive[0] == '\0' ) {
        pp = stxpcpy( dir, path );
    } else if( pd.drive[0] == '\0' ) {
        pp = stxpcpy( dir, fd->drive );
        pp = stxpcpy( pp, path );
    } else {
        return( ok );
    }
    if( pp > dir ) {
        if( !IS_PATH_SEP( pp[-1] ) ) {
            *pp++ = DIR_SEP;
        }
    }
    _makepath( pp, NULL, fd->dir, fd->fname, fd->ext );
    _splitpath2( dir, pd.buffer, &pd.drive, &pd.dir, &pd.fname, &pd.ext );
    ext = openSrcExts( exts, &pd, typ );
    if( ext != NULL ) {
        ok = true;
        if( ( typ == FT_SRC ) && ( ext != fd->ext ) ) {
            _makepath( dir, fd->drive, fd->dir, fd->fname, ext );
            WholeFName = FNameAdd( dir );
        }
    }
    return( ok );
}

static bool try_open_file( const char *path, pgroup2 *fd, pgroup2 *fa, const char **exts, src_file_type typ )
{
    bool    ok;
    char    save_chr_fname;
    char    save_chr_ext;

    ok = openSrcPath( path, fd, exts, typ );
    if( ok ) {
        return( ok );
    }
    if( fa != NULL ) {
        ok = openSrcPath( path, fa, exts, typ );
        if( ok ) {
            return( ok );
        }
    }
    if( CompFlags.check_truncated_fnames ) {
        save_chr_fname = '\0';
        if( strlen( fd->fname ) > 8 ) {
            save_chr_fname = fd->fname[8];
            fd->fname[8] = '\0';
        }
        save_chr_ext = '\0';
        if( strlen( fd->ext ) > 4 ) {
            save_chr_ext = fd->ext[4];
            fd->ext[4] = '\0';
        }
        if( save_chr_fname != '\0' || save_chr_ext != '\0' ) {
            ok = openSrcPath( path, fd, exts, typ );
            if( !ok ) {
                if( save_chr_fname != '\0' ) {
                    fd->fname[8] = save_chr_fname;
                }
                if( save_chr_ext != '\0' ) {
                    fd->ext[4] = save_chr_ext;
                }
            }
        }
    }
    return( ok );
}

static bool doIoSuppOpenSrc(    // OPEN A SOURCE FILE (PRIMARY,HEADER)
    pgroup2 *fd,                // - descriptor for file name
    pgroup2 *fai,               // - descriptor for alias file name
    src_file_type typ )         // - type of search path to use
{
    const char  **paths;        // - optional paths to prepend
    const char  **exts;         // - optional extensions to append
    bool ok;                    // - return: true ==> opened
    const char  *path;          // - next path
    char bufpth[_MAX_PATH];     // - buffer for next path
    SRCFILE curr;               // - current included file
    SRCFILE stdin_srcfile;      // - srcfile for stdin
    pgroup2 idescr;             // - descriptor for included file
    LINE_NO dummy;              // - dummy line number holder
    char prevpth[_MAX_PATH];    // - buffer for previous path
    bool alias_abs;
    bool alias_check;
    pgroup2 *fa;

    alias_abs = false;
    alias_check = false;
    ok = false;
    fa = NULL;
    switch( typ ) {
    case FT_SRC:
        exts = extsSrc;
        if( fd->fname[0] == '\0' && fd->ext[0] == '.' && fd->ext[1] == '\0' ) {
            if( ErrCount != 0 ) {
                // command line errors may result in "." as the input name
                // so the user thinks that the compiler is hung!
                return( false );
            }
            WholeFName = FNameAdd( "stdin" );
            stdin_srcfile = SrcFileOpen( stdin, WholeFName, 0 );
            SrcFileNotAFile( stdin_srcfile );
            ok = true;
            break;
        }
        ok = openSrcPath( "", fd, exts, typ );
        if( ok )
            break;
        if( !CompFlags.ignore_default_dirs && !IS_DIR_SEP( fd->dir[0] ) ) {
            for( paths = pathSrc; (path = *paths) != NULL; ++paths ) {
                ok = openSrcPath( path, fd, exts, typ );
                if( ok ) {
                    break;
                }
            }
        }
        break;
    case FT_HEADER:
    case FT_HEADER_FORCED:
    case FT_HEADER_PRE:
    case FT_LIBRARY:
        exts = extsHdr;
        alias_abs = ( fai != NULL && ( fai->drive[0] != '\0' || IS_DIR_SEP( fai->dir[0] ) ) );
        // have to look for absolute paths
        if( fd->drive[0] != '\0' || IS_DIR_SEP( fd->dir[0] ) ) {
            if( alias_abs )
                fa = fai;
            alias_abs = false;
            ok = try_open_file( "", fd, fa, exts, typ );
            break;
        }
        /* if alias contains abs path then check it after last check for regular name */
        alias_check = ( fai != NULL && fai->drive[0] == '\0' && !IS_DIR_SEP( fai->dir[0] ) );
        if( alias_check )
            fa = fai;
        if( typ != FT_LIBRARY && !IS_DIR_SEP( fd->dir[0] ) ) {
            if( CompFlags.ignore_default_dirs ) {
                bufpth[0] = '\0';
                curr = SrcFileCurrent();
                if( curr != NULL ) {
                    _splitpath2( SrcFileName( curr ), idescr.buffer, &idescr.drive, &idescr.dir, &idescr.fname, &idescr.ext );
                    _makepath( bufpth, idescr.drive, idescr.dir, NULL, NULL );
                }
                ok = try_open_file( bufpth, fd, fa, exts, typ );
                if( ok ) {
                    break;
                }
            } else {
                if( !CompFlags.ignore_current_dir ) {
                    // check for current directory
                    ok = try_open_file( "", fd, fa, exts, typ );
                    if( ok ) {
                        break;
                    }
                }
                /* check directories of currently included files */
                prevpth[0] = '\xff';  /* to make it not compare with anything else */
                prevpth[1] = '\0';
                curr = SrcFileCurrent();
                for( ; curr != NULL; ) {
                    _splitpath2( SrcFileName( curr ), idescr.buffer, &idescr.drive, &idescr.dir, &idescr.fname, &idescr.ext );
                    _makepath( bufpth, idescr.drive, idescr.dir, NULL, NULL );
                    /*optimization: don't try and open if in previously checked dir*/
                    if( strcmp( bufpth, prevpth ) != 0 ) {
                        ok = try_open_file( bufpth, fd, fa, exts, typ );
                        if( ok ) {
                            break;
                        }
                    }
                    curr = SrcFileIncluded( curr, &dummy );
                    strcpy( prevpth, bufpth );
                }
                if( ok ) {
                    break;
                }
            }
        }
        HFileListStart();
        for( ;; ) {
            HFileListNext( bufpth );
            if( *bufpth == '\0' )
                break;
            ok = try_open_file( bufpth, fd, fa, exts, typ );
            if( ok ) {
                break;
            }
        }
        if( ok ) {
            break;
        }
        if( typ != FT_LIBRARY && !CompFlags.ignore_default_dirs && !IS_DIR_SEP( fd->dir[0] ) ) {
            for( paths = pathHdr; (path = *paths) != NULL; ++paths ) {
                ok = try_open_file( path, fd, fa, exts, typ );
                if( ok ) {
                    break;
                }
            }
        }
        if( alias_abs ) {
            ok = openSrcPath( "", fai, exts, typ );
        }
        break;
    case FT_CMD:
        exts = extsCmd;
        ok = openSrcPath( "", fd, exts, typ );
        if( ok )
            break;
        if( !IS_DIR_SEP( fd->dir[0] ) ) {
            for( paths = pathCmd; (path = *paths) != NULL; ++paths ) {
                ok = openSrcPath( path, fd, exts, typ );
                if( ok ) {
                    break;
                }
            }
        }
        break;
    default:
        exts = NULL;
        break;
    }
    if( ok ) {
        switch( typ ) {
        case FT_CMD:
            SetSrcFileCommand();
            break;
        case FT_LIBRARY:
            SetSrcFileLibrary();
            break;
        }
    }
    return( ok );
}


static void normalizeSep( char *dir )
{
    char    c;

    while( (c = *dir) != '\0' ) {
#if defined( __UNIX__ )
        if( c == '\\' )
            *dir = '/';
#else
        if( c == '/' )
            *dir = '\\';
#endif
        dir++;
    }
}

bool IoSuppOpenSrc(             // OPEN A SOURCE FILE (PRIMARY,HEADER)
    const char *file_name,      // - supplied file name
    src_file_type typ )         // - type of search path to use
{
    pgroup2     fd;             // - descriptor for file name
    pgroup2     fa;             // - descriptor for alias file name
    pgroup2     *fap;           // - pointer to descriptor for alias file name
    const char  *alias_file_name;

#ifdef OPT_BR
    if( NULL != file_name
     && file_name[0] != '\0' ) {
        TOKEN_LOCN locn;
        switch( typ ) {
        case FT_SRC:
        case FT_HEADER:
        case FT_HEADER_FORCED:
        case FT_HEADER_PRE:
        case FT_LIBRARY:
            SrcFileGetTokenLocn( &locn );
            BrinfIncludeSource( file_name, &locn );
            break;
        }
    }
#endif
    _splitpath2( file_name, fd.buffer, &fd.drive, &fd.dir, &fd.fname, &fd.ext );
    normalizeSep( fd.dir );
    fap = NULL;
    switch( typ ) {
    case FT_HEADER:
    case FT_HEADER_FORCED:
    case FT_HEADER_PRE:
    case FT_LIBRARY:
        // See if there's an alias for this file name
        alias_file_name = IAliasLookup( file_name, typ == FT_LIBRARY );
        if( alias_file_name != file_name ) {
            _splitpath2( alias_file_name, fa.buffer, &fa.drive, &fa.dir, &fa.fname, &fa.ext );
            normalizeSep( fa.dir );
            fap = &fa;
        }
        break;
    }
    return( doIoSuppOpenSrc( &fd, fap, typ ) );
}

static void ioSuppError(        // SIGNAL I/O ERROR AND ABORT
    MSG_NUM error_code )            // - error code
{
    CErr2( error_code, errno );
    CSuicide();
}


static void ioSuppReadError(      // SIGNAL ERROR ON READ
    void )
{
    ioSuppError( ERR_WORK_FILE_READ_ERROR );
}


static void ioSuppWriteError(     // SIGNAL ERROR ON WRITE
    void )
{
    ioSuppError( ERR_WORK_FILE_WRITE_ERROR );
}


static void ioSuppTempOpen(             // OPEN TEMPORARY FILE
    void )
{
    tempfileFP = tmpfile();
    if( tempfileFP == NULL ) {
        ioSuppError( ERR_UNABLE_TO_OPEN_WORK_FILE );
    }
}


char *IoSuppFullPath(           // GET FULL PATH OF FILE NAME (ALWAYS USE RET VALUE)
    char *name,                 // - input file name
    char *buff,                 // - output buffer
    unsigned size )             // - output buffer size
{
    DbgAssert( size >= _MAX_PATH );
#ifndef NDEBUG
    // caller should use return value only!
    // - this code will make sure caller doesn't use buff
    *buff = '.';
    ++buff;
    --size;
#endif
    return( _getFilenameFullPath( buff, name, size ) );
}


DISK_ADDR IoSuppTempNextBlock(  // GET NEXT BLOCK NUMBER
    unsigned num_blocks )       // - number of blocks allocated
{
    DISK_ADDR retn;

    retn = tempBlock + 1;
    tempBlock += num_blocks;
    return( retn );
}


void IoSuppTempWrite(           // WRITE TO TEMPORARY FILE
    DISK_ADDR   block_num,      // - block within temp file
    size_t      block_size,     // - size of blocks
    void        *data )         // - buffer to write
{
    if( tempfileFP == NULL )
        ioSuppTempOpen();
    block_num--;
    if( fseek( tempfileFP, block_size * block_num, SEEK_SET ) ) {
        ioSuppWriteError();
    }
    if( block_size != fwrite( data, 1, block_size, tempfileFP ) ) {
        ioSuppWriteError();
    }
}


void IoSuppTempRead(            // READ FROM TEMPORARY FILE
    DISK_ADDR   block_num,      // - block within temp file
    size_t      block_size,     // - size of blocks
    void        *data )         // - buffer to read
{
    if( tempfileFP == NULL )
        ioSuppTempOpen();
    block_num--;
    if( fseek( tempfileFP, block_size * block_num, SEEK_SET ) ) {
        ioSuppReadError();
    }
    if( block_size != fread( data, 1, block_size, tempfileFP ) ) {
        ioSuppReadError();
    }
}


static bool pathExists(         // TEST IF A PATH EXISTS
    const char *path )          // - path to be tested
{
    DIR *dir;                   // - control for directory
    bool ok;                    // - return: true ==> directory exists

    ok = false;
    dir = opendir( path );
    if( dir != NULL ) {
        closedir( dir );
        ok = true;
    }
    return( ok );
}

static void setPaths(           // SET PATHS (IF THEY EXIST)
    const char **vect )         // - the vector of potential paths
{
    const char  **dest;         // - place to store
    const char  **test;         // - path to test
    const char  *path;          // - current path

    dest = vect;
    test = vect;
    for( ;; ) {
        path = *test;
        if( path == NULL )
            break;
        if( pathExists( path ) ) {
            *dest++ = path;
        }
        ++test;
    }
    *dest = NULL;
}


static void ioSuppInit(         // INITIALIZE IO SUPPORT
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    outFileChecked = 0;
    tempBlock = 0;
    tempfileFP = NULL;
    FNameBuf = CMemAlloc( _MAX_PATH );
    carve_buf = CarveCreate( sizeof( BUF_ALLOC ), 8 );
    setPaths( pathSrc );
    setPaths( pathHdr );
    setPaths( pathCmd );
}


static void ioSuppFini(         // FINALIZE IO SUPPORT
    INITFINI* defn )            // - definition
{
    /* unused parameters */ (void)defn;

    if( tempfileFP != NULL ) {
        fclose( tempfileFP );
    }
    while( NULL != buffers ) {
        freeBuffer( buffers );
    }
    CarveDestroy( carve_buf );
    CMemFree( FNameBuf );
}


INITDEFN( io_support, ioSuppInit, ioSuppFini )
