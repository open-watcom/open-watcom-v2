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
* Description:  I/O support routines.
*
****************************************************************************/


#include "plusplus.h"

#include <stdio.h>
#include <stdarg.h>
#if defined(__UNIX__)
 #include <dirent.h>
#else
 #include <direct.h>
#endif

#include "wio.h"
#include "preproc.h"
#include "errdefns.h"
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

typedef struct buf_alloc BUF_ALLOC;
struct buf_alloc {              // BUF_ALLOC -- allocated buffer
    BUF_ALLOC* next;            // - next buffer allocation
    FILE* file;                 // - file in question
    void* buffer;               // - allocated buffer
};


static carve_t carve_buf;       // carver: BUF_ALLOC
static BUF_ALLOC* buffers;      // allocated buffers
static int  temphandle;         // handle for temp file
static char *tempname;          // name of temp file
static DISK_ADDR tempBlock;     // next available block in temp file
static unsigned outFileChecked; // mask for checking output files

static char  workFile[] =       // template for work file
    "__wrk0__";

#if defined(__OS2__) || defined(__DOS__) || defined(__NT__)

static char* pathSrc[] =        // paths for source file
    {   "..\\cpp"
    ,   "..\\c"
    ,   NULL
    };

static char* pathHdr[] =        // paths for header files
    {   "..\\h"
    ,   "..\\include"
    ,   NULL
    };

static char* pathCmd[] =        // paths for command files
    {   "..\\occ"
    ,   NULL
    };

static char* extsHdr[] =        // extensions for header files
    {   ".hpp"
    ,   ".h"
    ,   NULL
    };

static char* extsSrc[] =        // extensions for source files
    {   ".cpp"
    ,   ".cc"
    ,    ".c"
    ,   NULL
    };

static char* extsCmd[] =        // extensions for command files
    {   ".occ"
    ,   NULL
    };

static char* extsOut[] =        // extensions for output files
    {   ".obj"
    ,   ".i"
    ,   ".err"
    ,   ".mbr"
    ,   ".def"
#ifdef OPT_BR
    ,   ".brm"
#endif
    ,   ".d"
    ,   ".obj"
    };

#define IS_DIR_SEP( c )         ((c)=='/'||(c)=='\\')
#define IS_PATH_SEP( c )        ((c)==':'||IS_DIR_SEP(c))
#define PATH_SEP                '\\'
#define INC_PATH_SEP            ';'

#elif defined(__UNIX__)

static char* pathSrc[] =        // paths for source file
    {   "../C"
    ,   "../cpp"
    ,   "../c"
    ,   NULL
    };

static char* pathHdr[] =        // paths for header files
    {   "../H"
    ,   "../h"
    ,   NULL
    };

static char* pathCmd[] =        // paths for command files
    {   "../occ"
    ,   NULL
    };

static char* extsHdr[] =        // extensions for header files
    {   ".H"
    ,   ".hpp"
    ,   ".h"
    ,   NULL
    };

static char* extsSrc[] =        // extensions for source files
    {   ".C"
    ,   ".cpp"
    ,   ".cc"
    ,   ".c"
    ,   NULL
    };

static char* extsCmd[] =        // extensions for command files
    {   ".occ"
    ,   NULL
    };

static char* extsOut[] =        // extensions for output files
    {   ".o"
    ,   ".i"
    ,   ".err"
    ,   ".mbr"
    ,   ".def"
#ifdef OPT_BR
    ,   ".brm"
#endif
    ,   ".d"
    ,   ".o"
    };

#define IS_DIR_SEP( c )         ((c)=='/')
#define IS_PATH_SEP( c )        IS_DIR_SEP( c )
#define PATH_SEP                '/'
#define INC_PATH_SEP            ':'

#else

#error IOSUPP not configured for OS

#endif

static char *FNameBuf = NULL;   // file name buffer for output files

char *IoSuppOutFileName(        // BUILD AN OUTPUT NAME FROM SOURCE NAME
    enum out_file_type typ )    // - extension
{
    char *drive;
    char *dir;
    char *fname;
    char *ext;
    char *extsrc;
    char *path;
    int use_defaults;
    unsigned mask;
    FILE *try_create;
    auto char buff[ _MAX_PATH2 ];

    use_defaults = FALSE;
    switch( typ ) {
      case OFT_DEF:
#ifdef OPT_BR
      case OFT_BRI:
#endif
        path = WholeFName;
        use_defaults = TRUE;
        break;
      case OFT_DEP:
        path = DependFileName;
        if( path == NULL ) {
            use_defaults = TRUE;
            path = WholeFName;
        }
        break;
      case OFT_ERR:
        if( ErrorFileName == NULL )
            return( NULL );
        outFileChecked |= 1 << typ; // don't create a file. it's just a name.
        path = ErrorFileName;
        break;
      case OFT_SRCDEP:
        outFileChecked |= 1 << typ;
        if( !(path = SrcDepFileName ) ) {
            path = WholeFName;
        }
        else 
        {
            auto char buff[ _MAX_PATH2 ];
            char *drive;
            char *dir;
            char *fname;
            _splitpath2( WholeFName, buff, &drive, &dir, &fname, &extsrc );
        }
        break;
      case OFT_TRG:
        outFileChecked |= 1 << typ; // don't create a file. it's just a name.
        if( path = TargetFileName ) break;
      case OFT_PPO:
      case OFT_OBJ:
      case OFT_MBR:
        path = ObjectFileName;
        if( path == NULL ) {
            use_defaults = TRUE;
            path = WholeFName;
        }
        break;
    }
    _splitpath2( path, buff, &drive, &dir, &fname, &ext );
    switch( typ ) {
      case OFT_MBR:
        ext = "";       // don't override extension
        break;
      case OFT_SRCDEP:
        if( !ext || !ext[0] )
            ext = extsrc;
        break;
    }
    if( use_defaults || ext[0] == '\0' ) {
        ext = extsOut[ typ ];
    }
    if( fname[0] == '\0' || fname[0] == '*' ) {
        fname = ModuleName;
    }
    if( use_defaults ) {
        drive = "";
        dir = "";
    }
    _makepath( FNameBuf, drive, dir, fname, ext );
    mask = 1 << typ;
    if(( outFileChecked & mask ) == 0 ) {
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


static void set_buffering(       // SET BUFFERING FOR AN OPEN FILE
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


boolean IoSuppCloseFile(        // CLOSE FILE IF OPENED
    FILE **file_ptr )           // - addr( file pointer )
{
    boolean retn;               // - return: TRUE ==> was open
    BUF_ALLOC* ba;              // - current allocated buffer

    if( *file_ptr == NULL ) {
        retn = FALSE;
    } else {
        RingIterBegSafe( buffers, ba ) {
            if( *file_ptr == ba->buffer ) {
                freeBuffer( ba );
                break;
            }
        } RingIterEndSafe( ba );
        SrcFileFClose( *file_ptr );
        *file_ptr = NULL;
        retn = TRUE;
    }
    return( retn );
}


struct path_descr               // path description
{   char buffer[ _MAX_PATH2 ];  // - buffer
    char *drv;                  // - drive
    char *dir;                  // - directory
    char *fnm;                  // - file name
    char *ext;                  // - extension
};


static void splitFileName(      // SPLIT APART PATH/FILENAME
    const char *name,           // - name to be split
    struct path_descr *descr )  // - descriptor
{
    _splitpath2( name
               , descr->buffer
               , &descr->drv
               , &descr->dir
               , &descr->fnm
               , &descr->ext );
}


static void makeDirName(        // MAKE FILE NAME (WITHOUT DRIVE)
    char *pp,                   // - target location
    struct path_descr *nd )     // - name descriptor
{
    _makepath( pp, NULL, nd->dir, nd->fnm, nd->ext );
}


static boolean openSrc(         // ATTEMPT TO OPEN FILE
    char *name,                 // - file name
    enum file_type typ )        // - type of file being opened
{
    pch_status pch_OK;          // - pre-compiled header load status
    FILE *fp;                   // - file pointer
    boolean might_browse;       // - true ==> might browse, if right file type

    if( SrcFileProcessOnce( name ) ) {
        SrcFileOpen( NULL, name );
        return( TRUE );
    }
    fp = SrcFileFOpen( name, SFO_SOURCE_FILE );
    if( fp == NULL ) {
        return( FALSE );
    }
    might_browse = FALSE;
    if( CompFlags.watch_for_pcheader ) {
        CompFlags.watch_for_pcheader = FALSE;
        pch_OK = PCHeaderAbsorb( name );
        if( pch_OK != PCHA_OK ) {
            SrcFileSetCreatePCHeader();
            SrcFileOpen( fp, name );
            might_browse = TRUE;
        } else {
            SrcFileOpen( NULL, name );
            fclose( fp );
        }
    } else {
        SrcFileOpen( fp, name );
        if( typ == FT_SRC ) {
            SetSrcFilePrimary();
        }
        might_browse = TRUE;
    }
#ifdef OPT_BR
    if( might_browse ) switch( typ ) {
      case FT_SRC :
      case FT_LIBRARY :
      case FT_HEADER :
        BrinfOpenSource( SrcFileCurrent() );
        break;
    }
#endif
    return( TRUE );
}


static char *openExt(           // ATTEMPT TO OPEN FILE (EXT. TO BE APPENDED)
    char *ext,                  // - extension
    struct path_descr *nd,      // - name descriptor
    enum file_type typ )        // - type of file being opened
{
    char        *ret;           // - ret
    char name[ _MAX_PATH ];     // - buffer for file name

    _makepath( name, nd->drv, nd->dir, nd->fnm, ext );
    /* so we can tell if the open worked */
    ret = (ext != NULL) ? ext : "";
    if( ! openSrc( name, typ ) ) {
        ret = NULL;
    }
    return ret;
}


static char *openSrcExts(       // ATTEMPT TO OPEN FILE (EXT.S TO BE APPENDED)
    char **exts,                // - extensions
    struct path_descr *nd,      // - name descriptor
    enum file_type typ )        // - type of file being opened
{
    char *ext;                  // - current extension

    if( nd->ext[0] == '\0' ) {
        int doSrc = (!(CompFlags.dont_autogen_ext_src) && (FT_SRC == typ));
        int doInc = (!(CompFlags.dont_autogen_ext_inc) && ((FT_HEADER == typ)||(FT_LIBRARY == typ)));
        int doExt = (doSrc || doInc);

        ext = openExt( NULL, nd, typ );

        if(( ext == NULL ) && (doExt)) {
            for( ; ; ) {
                ext = *exts++;
                if( ext == NULL ) break;
                ext = openExt( ext, nd, typ );
                if( ext != NULL ) break;
            }
        }
    } else {
        ext = openExt( nd->ext, nd, typ );
    }
    return ext;
}


static char *concSep(           // CONCATENATE PATH SEPARATOR AS REQUIRED
    char *pp,                   // - pointer into buffer
    char *buffer )              // - buffer
{
    char *pred;                 // - preceding char in buffer

    if( pp > buffer ) {
        pred = pp - 1;
        if( !IS_PATH_SEP( *pred ) ) {
            *pp++ = PATH_SEP;
        }
    }
    return pp;
}


static boolean openSrcPath(     // ATTEMPT TO OPEN FILE (PATH TO BE PREPENDED)
    char *path,                 // - path
    char **exts,                // - file extensions
    struct path_descr *fd,      // - file descriptor
    enum file_type typ )        // - type of file being opened
{
    boolean retn;               // - return: TRUE ==> opened
    struct path_descr pd;       // - path descriptor
    char dir[ _MAX_PATH*2 ];    // - new path
    char *pp;                   // - pointer into path
    char *ext;                  // - extension opened

    splitFileName( path, &pd );
    if( fd->drv[0] == '\0' ) {
        pp = stpcpy( dir, path );
    } else if( pd.drv[0] == '\0' ) {
        pp = stpcpy( dir, fd->drv );
        pp = stpcpy( pp, path );
    } else {
        pp = NULL;
    }
    if( pp == NULL ) {
        retn = FALSE;
    } else {
        pp = concSep( pp, dir );
        makeDirName( pp, fd );
        splitFileName( dir, &pd );
        ext = openSrcExts( exts, &pd, typ );
        if( ext == NULL ) {
            retn = FALSE;
        } else {
            if( ( typ == FT_SRC ) && ( ext != fd->ext ) ) {
                _makepath( dir, fd->drv, fd->dir, fd->fnm, ext );
                WholeFName = FNameAdd( dir );
            }
            retn = TRUE;
        }
    }
    return retn;
}


static boolean doIoSuppOpenSrc(  // OPEN A SOURCE FILE (PRIMARY,HEADER)
    struct path_descr *fd,      // - descriptor for file name
    enum file_type typ )        // - type of search path to use
{
    char **paths;               // - optional paths to prepend
    char **exts;                // - optional extensions to append
    boolean retn;               // - return: TRUE ==> opened
    char *path;                 // - next path
    char bufpth[ _MAX_PATH ];   // - buffer for next path
    SRCFILE curr;               // - current included file
    SRCFILE stdin_srcfile;      // - srcfile for stdin
    struct path_descr idescr;   // - descriptor for included file
    LINE_NO dummy;              // - dummy line number holder
    char prevpth[ _MAX_PATH ];  // - buffer for previous path

    switch( typ ) {
    case FT_SRC:
        if( fd->fnm[0] == '\0' && fd->ext[0] == '.' && fd->ext[1] == '\0' ) {
            if( ErrCount != 0 ) {
                // command line errors may result in "." as the input name
                // so the user thinks that the compiler is hung!
                return( FALSE );
            }
            WholeFName = FNameAdd( "stdin" );
            stdin_srcfile = SrcFileOpen( stdin, WholeFName );
            SrcFileNotAFile( stdin_srcfile );
            goto file_was_found;
        }
        paths = pathSrc;
        exts = extsSrc;
        break;
    case FT_HEADER:
    case FT_LIBRARY:
        if( !CompFlags.ignore_current_dir ) {
            paths = pathHdr;
        } else {
            paths = NULL;
        }
        exts = extsHdr;
        break;
    case FT_CMD:
        paths = pathCmd;
        exts = extsCmd;
        break;
    }
    switch( typ ) {
    case FT_LIBRARY:
        if( fd->drv[0] != '\0' || IS_DIR_SEP( fd->dir[0] ) ) {
            retn = openSrcPath( "", exts, fd, typ );
            if( retn ) goto file_was_found;
        }
        break;
    case FT_HEADER:
        // even if ignoreing current dir, have to look for absolute paths
        if( !CompFlags.ignore_current_dir || fd->drv[0] != '\0' ) {
             // look in current directory
            retn = openSrcPath( "", exts, fd, typ );
            if( retn ) goto file_was_found;
        }
        /* check directories of currently included files */
        if( !IS_PATH_SEP( fd->dir[0] ) ) {
            prevpth[0] = '\xff'; /* to make it not compare with anything else */
            prevpth[1] = '\0';
            curr = SrcFileCurrent();
            for( ;; ) {
                if( curr == NULL ) break;
                splitFileName( SrcFileName( curr ), &idescr );
                _makepath( bufpth, idescr.drv, idescr.dir, NULL, NULL );
                /*optimization: don't try and open if in previously checked dir*/
                if( strcmp( bufpth, prevpth ) != 0 ) {
                    retn = openSrcPath( bufpth, exts, fd, FT_HEADER );
                    if( retn ) goto file_was_found;
                }
                curr = SrcFileIncluded( curr, &dummy );
                strcpy( prevpth, bufpth );
            }
        }
        break;
    case FT_SRC:
    case FT_CMD:
        retn = openSrcPath( "", exts, fd, typ );
        if( retn ) goto file_was_found;
        break;
    }
    switch( typ ) {
    case FT_HEADER:
    case FT_LIBRARY:
        HFileListStart();
        for( ; ; ) {
            HFileListNext( bufpth );
            if( *bufpth == '\0' ) break;
            retn = openSrcPath( bufpth, exts, fd, typ );
            if( retn ) goto file_was_found;
        }
        break;
    }
    switch( typ ) {
    case FT_HEADER:
    case FT_CMD:
    case FT_SRC:
        if( IS_PATH_SEP( fd->dir[0] ) ) {
            // absolute path
            break;
        }
        if( paths != NULL ) {
            for( ; ; ) {
                path = *paths++;
                if( path == NULL ) break;
                retn = openSrcPath( path, exts, fd, typ );
                if( retn ) goto file_was_found;
            }
        }
        break;
    }
    return FALSE;
file_was_found:
    switch( typ ) {
    case FT_CMD:
        SrcFileCommand();
        break;
    case FT_LIBRARY:
        SrcFileLibrary();
        break;
    }
    return TRUE;
}


boolean IoSuppOpenSrc(          // OPEN A SOURCE FILE (PRIMARY,HEADER)
    const char *file_name,      // - supplied file name
    enum file_type typ )        // - type of search path to use
{
    struct path_descr   fd;     // - descriptor for file name

#ifdef OPT_BR
    if( NULL != file_name
     && file_name[0] != '\0' ) {
        TOKEN_LOCN locn;
        switch( typ ) {
          case FT_SRC :
          case FT_HEADER :
          case FT_LIBRARY :
            SrcFileGetTokenLocn( &locn );
            BrinfIncludeSource( file_name, &locn );
            break;
        }
    }
#endif
    splitFileName( file_name, &fd );
    if( doIoSuppOpenSrc( &fd, typ ) ) return TRUE;
    #if !defined(__DOS__)
        if( !CompFlags.check_truncated_fnames ) return FALSE;
        if( strlen( fd.fnm ) <= 8 ) return FALSE;
        fd.fnm[8] = '\0';
        if( doIoSuppOpenSrc( &fd, typ ) ) return TRUE;
    #endif
    return FALSE;
}

static void tempFname( char *fname )
{
    char    *env;
    int     i;

    #if defined(__UNIX__)
        env = CppGetEnv( "TMPDIR" );
        if( env == NULL ) env = CppGetEnv( "TMP" );
    #else
        env = CppGetEnv( "TMP" );
    #endif

    if( env == NULL ) env = "";

    #define TMP_EXT ".tmp"
    #define MAX_TMP_PATH (_MAX_PATH - sizeof( workFile ) - sizeof( TMP_EXT ) - 2)

    strncpy( fname, env, MAX_TMP_PATH );
    fname[ MAX_TMP_PATH ] = '\0';
    i = strlen( fname );
    if( i > 0 && !IS_PATH_SEP( fname[i-1] ) ) {
        fname[i++] = PATH_SEP;
    }
    strcpy( &fname[i], workFile );
    strcpy( &fname[i+sizeof(workFile)-1], TMP_EXT );
}

#if defined(__DOS__)
 #include "tinyio.h"
extern void __SetIOMode( int, unsigned );
#endif


static void ioSuppError(        // SIGNAL I/O ERROR AND ABORT
    int error_code )            // - error code
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
    int         mode;
    auto char   fname[ _MAX_PATH ];

    mode = O_RDWR | O_CREAT | O_EXCL;
#ifdef __UNIX__
  #ifndef O_TEMP
    #define O_TEMP 0    /* Not a standard flag */
  #endif
    // Unix files are always binary
    mode |= O_TEMP;
#else
    mode |= O_BINARY;
#endif
    for(;;) {
        tempFname( fname );
        #if defined(__DOS__)
        {   tiny_ret_t  rc;
            rc = TinyCreateNew( fname, 0 );
            if( TINY_ERROR( rc ) ) {
                temphandle = -1;
            } else {
                temphandle = TINY_INFO( rc );
                __SetIOMode( temphandle, _READ | _WRITE | _BINARY );
            }
        }
        #else
            temphandle = open( fname, mode, PMODE_RW );
        #endif
        if( temphandle != -1 ) break;
        if( workFile[5] == 'Z' ) {
            temphandle = -1;
            break;
        }
        switch( workFile[5] ) {
        case '9':
            workFile[5] = 'A';
            break;
        case 'I':
            workFile[5] = 'J';  /* file-system may be EBCDIC */
            break;
        case 'R':
            workFile[5] = 'S';  /* file-system may be EBCDIC */
            break;
        default:
            ++workFile[5];
            break;
        }
    }
    #if defined(__UNIX__)
        /* Under POSIX it's legal to remove a file that's open. The file
           space will be reclaimed when the handle is closed. This makes
           sure that the work file always gets removed. */
        remove( fname );
        tempname = NULL;
    #else
        tempname = FNameAdd( fname );
    #endif
    if( temphandle == -1 ) {
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
    return _getFilenameFullPath( buff, name, size );
}


DISK_ADDR IoSuppTempNextBlock(  // GET NEXT BLOCK NUMBER
    unsigned num_blocks )       // - number of blocks allocated
{
    DISK_ADDR retn;

    retn = tempBlock + 1;
    tempBlock += num_blocks;
    return retn;
}


void IoSuppTempWrite(           // WRITE TO TEMPORARY FILE
    DISK_ADDR   block_num,      // - block within temp file
    size_t      block_size,     // - size of blocks
    void        *data )         // - buffer to write
{
    if( temphandle == -1 ) ioSuppTempOpen();
    block_num--;
    if( -1 == lseek( temphandle, block_size * block_num, SEEK_SET ) ) {
        ioSuppWriteError();
    }
    if( block_size != write( temphandle, data, block_size ) ) {
        ioSuppWriteError();
    }
}


void IoSuppTempRead(            // READ FROM TEMPORARY FILE
    DISK_ADDR   block_num,      // - block within temp file
    size_t      block_size,     // - size of blocks
    void        *data )         // - buffer to read
{
    if( temphandle == -1 ) ioSuppTempOpen();
    block_num--;
    if( -1 == lseek( temphandle, block_size * block_num, SEEK_SET ) ) {
        ioSuppReadError();
    }
    if( block_size != read( temphandle, data, block_size ) ) {
        ioSuppReadError();
    }
}


char *IoSuppIncPathElement(     // GET ONE PATH ELEMENT FROM INCLUDE LIST
    const char *path,           // - include list
    char *prefix )              // - buffer to store element
{
    unsigned    length;

    length = 0;
    for( ; ; ) {
        if( *path == '\0' ) break;
        if( (*path == INC_PATH_SEP) || (*path == ';') ) {
            ++path;
            if( length != 0 ) {
                break;
            }
        } else {
            ++length;
            *prefix++ = *path++;
        }
    }
    if( ( length > 1 ) && IS_DIR_SEP( *(prefix-1) ) ) --prefix;
    *prefix = '\0';
    return( (char *)path );
}


char *IoSuppAddIncPathSep(      // ADD AN INCLUDE PATH SEPARATOR
    char *path )                // - place to add separator
{
    *path = INC_PATH_SEP;
    return( path + 1 );
}


static boolean pathExists(      // TEST IF A PATH EXISTS
    char *path )                // - path to be tested
{
    DIR *dir;                   // - control for directory
    boolean retn;               // - return: TRUE ==> directory exists

    retn = FALSE;
    dir = opendir( path );
    if( dir != NULL ) {
        closedir( dir );
        retn = TRUE;
    }
    return retn;
}

static void setPaths(           // SET PATHS (IF THEY EXIST)
    char **vect )                // - the vector of potential paths
{
    char **dest;                // - place to store
    char **test;                // - path to test
    char *path;                 // - current path

    dest = vect;
    test = vect;
    for( ;; ) {
        path = *test;
        if( path == NULL ) break;
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
    defn = defn;
    outFileChecked = 0;
    tempBlock = 0;
    tempname = NULL;
    temphandle = -1;
    workFile[5] = '0';
    FNameBuf = CMemAlloc( _MAX_PATH );
    carve_buf = CarveCreate( sizeof( BUF_ALLOC ), 8 );
    setPaths( pathSrc );
    setPaths( pathHdr );
    setPaths( pathCmd );
}


static void ioSuppFini(         // FINALIZE IO SUPPORT
    INITFINI* defn )            // - definition
{
    defn = defn;
    if( temphandle != -1 ) {
        close( temphandle );
        if( tempname != NULL ) {
            remove( tempname );
        }
    }
    while( NULL != buffers ) {
        freeBuffer( buffers );
    }
    CarveDestroy( carve_buf );
    CMemFree( FNameBuf );
}


INITDEFN( io_support, ioSuppInit, ioSuppFini )
