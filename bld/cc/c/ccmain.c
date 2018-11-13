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
* Description:  C compiler top level driver module and file I/O.
*
****************************************************************************/


#include "cvars.h"
#include "iopath.h"
#include "scan.h"
#include <stdarg.h>
#include <stdio.h>
#include <sys/stat.h>
#ifdef __OSI__
    #include "ostype.h"
#endif
#include "cgdefs.h"
#include "feprotos.h"
#include "swchar.h"
#include "cfeinfo.h"
#include "pathlist.h"

#include "clibext.h"
#include "clibint.h"


#if defined(__UNIX__)
 #define C_PATH         "../c/"
 #define H_PATH         "../h/"
 #define OBJ_EXT        ".o"
#else
 #define C_PATH         "..\\c\\"
 #define H_PATH         "..\\h\\"
 #define OBJ_EXT        ".obj"
#endif
#define DEF_EXT         ".def"
#define ERR_EXT         ".err"
#define MBR_EXT         ".mbr"
#define C_EXT           ".c"
#define CPP_EXT         ".i"
#define DEP_EXT         ".d"

typedef struct path_descr {     // path description
    char buffer[_MAX_PATH2];    // - buffer
    char *drv;                  // - drive
    char *dir;                  // - directory
    char *fnm;                  // - file name
    char *ext;                  // - extension
} path_descr;

bool    PrintWhiteSpace;     // also refered from cmac2.c

static  bool    IsStdIn;
static  char    *FNameBuf = NULL;

void FrontEndInit( bool reuse )
//***************************//
// Do the once only things   //
//***************************//
{
    GlobalCompFlags.cc_reuse = reuse;
    GlobalCompFlags.cc_first_use = true;
    GlobalCompFlags.ignore_environment = false;
    GlobalCompFlags.ignore_current_dir = false;
    GlobalCompFlags.ide_cmd_line_has_files = false;
    GlobalCompFlags.ide_console_output = false;
    GlobalCompFlags.progress_messages = false;
//    GlobalCompFlags.dll_active = false;
}

void FrontEndFini( void )
//********************//
// Fini the once only //
//********************//
{
    GlobalCompFlags.cc_reuse = false;
    GlobalCompFlags.cc_first_use = true;
}

static void ClearGlobals( void )
{
    InitStats();
    IsStdIn = false;
    FNames = NULL;
    RDirNames = NULL;
    IAliasNames = NULL;
    SrcFile = NULL;
    ErrFile = NULL;
    DefFile = NULL;
    CppFile = NULL;
    DepFile = NULL;
    IncPathList = CMemAlloc( 1 );
    SegmentNum = FIRST_PRIVATE_SEGMENT;
    BufSize = BUF_SIZE;
    Buffer = CMemAlloc( BufSize );
    TokenBuf = CMemAlloc( BufSize );
    FNameBuf = CMemAlloc( _MAX_PATH );
    InitIncFile();
    InitErrLoc();
}

static bool ParseCmdLine( char **cmdline )
{
    char        *cmd;

    cmd = (cmdline == NULL) ?  "" : cmdline[0];  // from cpp.c
    if( cmd == NULL ) { // argv == 1
        cmd = "";
    }
    while( *cmd == ' ' )
        ++cmd;
    if( *cmd == '?' || *cmd == '\0' ) {
        CCusage();
        return( false );
    }
    GenCOptions( cmdline );
    return( true );
}

static void OpenDepFile( void )
{
    char        *name;

    if( CompFlags.generate_auto_depend ) {
        name = DepFileName();
        if( name != NULL ) {
            DepFile = fopen( name, "w" );
            if( DepFile != NULL ) {
                setvbuf( DepFile, CPermAlloc( 1024 ), _IOFBF, 1024 );
            }
        }
    }
}

char *ForceSlash( char *name, char slash )
{
    char    *save;

    save = name;
    if( slash != '\0' && name != NULL ) {
        while( name[0] != '\0' ) {
            if( name[0] == '\\' || name[0] == '/' )
                name[0] = slash;
            name++;
        }
    }
    return( save );
}

FNAMEPTR NextDependency( FNAMEPTR curr )
{
    if( curr == NULL ) {
        curr = FNames;
    } else {
        curr = curr->next;
    }
    for( ; curr != NULL; curr = curr->next ) {
        if( curr->rwflag && !SrcFileInRDir( curr ) ) {
            break;
        }
    }
    return( curr );
}


static void DelErrFile(void)
{
    char        *name;

    name = ErrFileName();
    if( name != NULL ) {
        remove( name );
    }
}

static void DelDepFile( void )
{
    char        *name;

    name = DepFileName();
    if( name != NULL ) {
        remove( name );
    }
}

static void DumpDepFile( void )
{
    FNAMEPTR    curr;

    OpenDepFile();
    if( DepFile != NULL ) {
        curr = NextDependency( NULL );
        if( curr != NULL ) {
            fprintf( DepFile, "%s :", ForceSlash( CreateFileName( DependTarget, OBJ_EXT, false ), DependForceSlash ) );
            fprintf( DepFile, " %s", ForceSlash( GetSourceDepName(), DependForceSlash ) );
            for( curr = NextDependency( curr ); curr != NULL; curr = NextDependency( curr ) ) {
                fprintf( DepFile, " %s", ForceSlash( curr->name, DependForceSlash ) );
            }
            fprintf( DepFile, "\n" );
        }
        fclose( DepFile );
        DepFile = NULL;
    } else {
        DelDepFile();
    }
}

static IALIASPTR AddIAlias( const char *alias_name, const char *real_name, bool is_lib )
{
    size_t      alias_len;
    size_t      real_len;
    IALIASPTR   alias, old_alias;
    IALIASPTR   *lnk;

    for( lnk = &IAliasNames; (old_alias = *lnk) != NULL; lnk = &old_alias->next ) {
        if( ( old_alias->is_lib == is_lib ) && !FNAMECMPSTR( alias_name, old_alias->alias_name ) ) {
            break;
        }
    }
    alias_len = strlen( alias_name ) + 1;
    real_len = strlen( real_name ) + 1;
    alias = CMemAlloc( offsetof( ialias_list, alias_name ) + alias_len + real_len );
    alias->next = NULL;
    alias->is_lib = is_lib;
    alias->real_name = alias->alias_name + alias_len;
    memcpy( alias->alias_name, alias_name, alias_len );
    memcpy( alias->real_name, real_name, real_len );
    *lnk = alias;
    if( old_alias != NULL ) {
        /* Replace old alias if it exists */
        alias->next = old_alias->next;
        CMemFree( old_alias );
    }

    return( alias );
}

static void FreeIAlias( void )
{
    IALIASPTR   aliaslist;

    while( (aliaslist = IAliasNames) != NULL ) {
        IAliasNames = aliaslist->next;
        CMemFree( aliaslist );
    }
    IAliasNames = NULL;
}

static const char *IncludeAlias( const char *filename, bool is_lib )
{
    IALIASPTR   alias;

    for( alias = IAliasNames; alias != NULL; alias = alias->next ) {
        if( ( alias->is_lib == is_lib ) && !FNAMECMPSTR( filename, alias->alias_name ) ) {
            return( alias->real_name );
        }
    }
    return( filename );
}

void CppPrtfFilenameErr( const char *filename, src_file_type typ, bool print_error )
{
    bool        save;

    save = CompFlags.cpp_output;
    if( CompFlags.cpp_output ) {
        if( typ == FT_LIBRARY || typ == FT_HEADER_PRE ) {
            CppPrtf( "#include <%s>", filename );
        } else {
            CppPrtf( "#include \"%s\"", filename );
        }
        CompFlags.cpp_output = false;
    }
    if( print_error ) {
        CErr2p( ERR_CANT_OPEN_FILE, filename );
    }
    CompFlags.cpp_output = save;
}

static bool openForcePreInclude( void )
{
    bool    ok;

    ok = OpenSrcFile( ForcePreInclude, FT_HEADER_PRE );
    if( !ok )
        CppPrtfFilenameErr( ForcePreInclude, FT_HEADER_PRE, false );
    return( ok );
}


/* open the primary source file, and return pointer to root file name */

#define STDIN_NAME      "stdin"

static void MakePgmName( void )
{
// Get fname, if file name has no extension whack ".c" on
// if stdin a "." then replace with "stdin" don't whack ".c"
// If no module name make the same as fname
    size_t      len;
    char        buff[_MAX_PATH2];
    char        *fname;
    char        *ext;

    if( WholeFName[0] == '.' && WholeFName[1] == '\0' ) {
        IsStdIn = true;
        CMemFree( WholeFName );
        WholeFName = CMemAlloc( sizeof( STDIN_NAME ) );
        memcpy( WholeFName, STDIN_NAME, sizeof( STDIN_NAME ) );
        fname = WholeFName;
        len = sizeof( STDIN_NAME );
    } else {
        _splitpath2( WholeFName, buff, NULL, NULL, &fname, &ext );
        if( *ext == '\0' ) { // no extension
            char *new;

            len = strlen( WholeFName );
            new = CMemAlloc( len + sizeof( C_EXT ) );
            memcpy( new, WholeFName, len );
            memcpy( new + len, C_EXT, sizeof( C_EXT ) );
            CMemFree( WholeFName );
            WholeFName = new;
        }
        len = strlen( fname ) + 1;
    }
    SrcFName = CMemAlloc( len );
    memcpy( SrcFName, fname, len );
    if( ModuleName == NULL ) {
        ModuleName = SrcFName;
    }
}

static void CantOpenFile( const char *name )
{
    char    msgtxt[80];
    char    msgbuf[MAX_MSG_LEN];

    CGetMsg( msgtxt, ERR_CANT_OPEN_FILE );
    sprintf( msgbuf, msgtxt, name );
    ConsErrMsgVerbatim( msgbuf );
}

static unsigned CppColumn = 0;
static unsigned CppWidth = 0;
static unsigned CommentChar = '\0';
static bool     CppFirstChar = false;

void SetCppWidth( unsigned width )
{
    CppWidth = width - 1;
}

void CppComment( int ch )
{
    if( CompFlags.keep_comments && CppPrinting() ) {
        if( ch != '\0' ) {
            if( CppColumn + 2 >= CppWidth )
                CppPutc( '\n' );
            CppPutc( '/' );
            CppPutc( ch );
        } else if( CommentChar == '*' ) {
            if( CppColumn + 2 >= CppWidth )
                CppPutc( '\n' );
            CppPutc( '*' );
            CppPutc( '/' );
//        } else if( CommentChar == '/' ) {
//            CppPutc( '\n' );
        }
        CommentChar = ch;
    }
}

void CppPutc( int ch )
{
    int     rc;

    if( CppFirstChar ) {
        CppFirstChar = false;
        if( ch == '\n' ) {
            return;
        }
    }
    if( ch == '\n' ) {
        CppColumn = 0;
        rc = fputc( '\n', CppFile );
    } else if( CppColumn >= CppWidth ) {
        CppColumn = 1;
        if( CommentChar == '\0' ) {
            if( (rc = fputc( '\\', CppFile )) != EOF ) {
                rc = fputc( '\n', CppFile );
            }
        } else if( (rc = fputc( '\n', CppFile )) != EOF ) {
            if( CommentChar == '/' ) {
                CppColumn += 2;
                if( (rc = fputc( '/', CppFile )) != EOF ) {
                    rc = fputc( '/', CppFile );
                }
            }
        }
        if( rc != EOF ) {
            rc = fputc( ch, CppFile );
        }
    } else {
        ++CppColumn;
        rc = fputc( ch, CppFile );
    }
    if( rc == EOF ) {
        CloseFiles();       /* get rid of temp file */
        MyExit( 1 );        /* exit */
    }
}

void CppPrtf( char *fmt, ... )
{
    va_list     arg;
    char        *p;
    char        *buff;

    buff = CMemAlloc( BufSize );
    va_start( arg, fmt );
    vsnprintf( buff, BufSize, fmt, arg );
    for( p = buff; *p != '\0'; ++p ) {
        CppPutc( *p );
    }
    va_end( arg );
    CMemFree( buff );
}

static void OpenCppFile( void )
{
    char  *name = NULL;

    if( CompFlags.cpp_output_to_file ) {
        name = CppFileName();
        CppFile = fopen( name, "w" );
    } else {
        CppFile = stdout;
    }
    CppColumn = 0;
    CppFirstChar = true;
    if( CppFile == NULL ) {
        CantOpenFile( name );
        MyExit( 1 );
    } else {
        if( CppWidth == 0 ) {
            CppWidth = ~0U;
        }
        setvbuf( CppFile, CPermAlloc( 4096 ), _IOFBF, 4096 );
    }
}

char *CreateFileName( const char *template, const char *extension, bool forceext )
{
#if !defined( __CMS__ )
    char        buff[_MAX_PATH2];
    char        *drive;
    char        *dir;
    char        *fname;
    char        *ext;
    const char  *path;

    path = (template == NULL) ? WholeFName : template;
    _splitpath2( path, buff, &drive, &dir, &fname, &ext );
    if( !forceext && template != NULL && ext[0] != '\0' ) {
        extension = ext;
    }
    if( fname[0] == '\0' || fname[0] == '*' ) {
        fname = ModuleName;
    }
    if( template == NULL ) {
        /* default object file goes in current directory */
        drive = "";
        dir = "";
    }
    _makepath( FNameBuf, drive, dir, fname, extension );
#else
    char    *p;

    if( template == NULL )
        template = WholeFName;
    strcpy( FNameBuf, template );
    p = FNameBuf;
    while( *p != '\0' && *p != ' ' )
        ++p;
    strcpy( p, extension );
#endif
    return( FNameBuf );
}

char *GetSourceDepName( void )
{
    char buff[_MAX_PATH2];
    char *ext;

    _splitpath2( WholeFName, buff, NULL, NULL, NULL, &ext );
    return( CreateFileName( SrcDepName, ext, false ) );
}


char *ObjFileName( void )
{
    return( CreateFileName( ObjectFileName, OBJ_EXT, false ) );
}

char *CppFileName( void )
{
    return( CreateFileName( ObjectFileName, CPP_EXT, false ) );
}

char *DepFileName( void )
{
    return( CreateFileName( DependFileName, DEP_EXT, false ) );
}

char *ErrFileName( void )
{
    if( ErrorFileName == NULL )
        return( NULL );
    return( CreateFileName( ErrorFileName, ERR_EXT, false ) );
}

void CppPrtChar( int c )
{
    if( CppPrinting() ) {
        CppPutc( c );
    }
}

void OpenDefFile( void )
{
#if !defined( __CMS__ )
    char        buff[_MAX_PATH2];
    char        name[_MAX_PATH];
    char        *fname;

    if( DefFName == NULL ) {
        _splitpath2( SrcFName, buff, NULL, NULL, &fname, NULL );
        _makepath( name, NULL, NULL, fname, DEF_EXT );
        DefFile = fopen( name, "w" );
    } else {
        DefFile = fopen( DefFName, "w" );
    }
    if( DefFile != NULL ) {
        setvbuf( DefFile, CPermAlloc( 1024 ), _IOFBF, 1024 );
    }
#else
    DefFile = fopen( DefFName, "w" );
#endif
}

FILE *OpenBrowseFile( void )
{
    char        buff[_MAX_PATH2];
    char        name[_MAX_PATH];
    char        *fname;
    FILE        *mbr_file;

    if( CompFlags.cpp_output_to_file ) {
        strcpy( name, CreateFileName( ObjectFileName, MBR_EXT, true ) );
    } else {
        _splitpath2( SrcFName, buff, NULL, NULL, &fname, NULL );
        _makepath( name, NULL, NULL, fname, MBR_EXT );
    }
    mbr_file = fopen( name, "wb" );
    if( mbr_file == NULL ) {
        CantOpenFile( name );
    }
    return( mbr_file );
}

void CClose( FILE *fp )
/*********************/
{
    if( fp == NULL ) {
        /* nothing to do */
    } else if( fp != stdin ) {
        fclose( fp );
    }
}

void CloseSrcFile( FCB *srcfcb )
{
    CClose( srcfcb->src_fp );
    CloseFCB( srcfcb );
}

static bool FreeSrcFP( void )
/********************/
{
    FCB     *src_file;
    FCB     *next;
    bool    ret;

    ret = false;
    for( src_file = SrcFile; src_file != NULL; src_file = next ) {
        next = src_file->prev_file;
        if( next == NULL || next->src_fp == NULL ) {
            break;
        }
    }
    if( src_file != NULL && src_file->src_fp != NULL ) {
        src_file->rseekpos = ftell( src_file->src_fp );
        CClose( src_file->src_fp );
        src_file->src_fp = NULL;
        ret = true;
    }
    return( ret );
}


static FNAMEPTR FindFlist( char const *filename )
{ // find a flist
    FNAMEPTR    flist;

    for( flist = FNames; flist != NULL; flist = flist->next ) {
        if( FNAMECMPSTR( filename, flist->name ) == 0 ) {
            break;
        }
    }
    return( flist );
}

static bool IsFNameOnce( char const *filename )
{
    FNAMEPTR    flist;

    flist = FindFlist( filename );
    if( flist == NULL )
        return( false );
    return( flist->once );
}

static bool TryOpen( const char *path, path_descr *ff, src_file_type typ )
{
    FILE        *fp;
    char        filename[_MAX_PATH];
    char        *p;
    path_descr  fd;

    _splitpath2( path, fd.buffer, &fd.drv, &fd.dir, NULL, NULL );
    if( ff->drv[0] != '\0' && fd.drv[0] != '\0' ) {
        return( false );
    }
    // concatenate fd.dir + sep + fp.dir
    p = fd.dir + strlen( fd.dir );
    if( fd.dir[0] != '\0' ) {
        if( !IS_PATH_SEP( p[-1] ) ) {
            *p++ = DIR_SEP;
        }
    }
    strcpy( p, ff->dir );
    _makepath( filename, ( fd.drv[0] == '\0' ) ? ff->drv : fd.drv, fd.dir, ff->fnm, ff->ext );
    if( IsFNameOnce( filename ) ) {
        return( true );
    }
    for( ; (fp = fopen( filename, "rb" )) == NULL; ) {
        if( errno != ENOMEM && errno != ENFILE && errno != EMFILE )
            break;
        if( !FreeSrcFP() ) {        // try closing an include file
            break;
        }
    }
    if( fp == NULL )
        return( false );
/*
    if( CompFlags.use_precompiled_header ) {
        CompFlags.use_precompiled_header = false;
        if( UsePreCompiledHeader( filename ) ) {
            fclose( fp );
            return( true );
        }
    }
*/
    if( OpenFCB( fp, filename, typ ) ) {
        if( CompFlags.cpp_output ) {
            if( CppFile == NULL )
                OpenCppFile();
            EmitPoundLine( 1, filename, true );
            CppFirstChar = true;
        }
        return( true );
    }
    fclose( fp );
    return( false );
}

FNAMEPTR AddFlist( char const *filename )
{
    FNAMEPTR    flist;
    FNAMEPTR    *lnk;
    unsigned    index;
    size_t      len1;
    size_t      len2;
    struct stat statbuf;

    index = 0;
    for( lnk = &FNames; (flist = *lnk) != NULL; lnk = &flist->next ) {
        if( FNAMECMPSTR( filename, flist->name ) == 0 )
            break;
        index++;
    }
    if( flist == NULL ) {
        len1 = strlen( filename ) + 1;
        if( HAS_PATH( filename ) || DependHeaderPath == NULL ) {
            len2 = 0;
            flist = (FNAMEPTR)CMemAlloc( offsetof( fname_list, name ) + len1 );
        } else {
            len2 = strlen( DependHeaderPath );
            flist = (FNAMEPTR)CMemAlloc( offsetof( fname_list, name ) + len2 + len1 );
            memcpy( flist->name, DependHeaderPath, len2 );
        }
        memcpy( flist->name + len2, filename, len1 );
        *lnk = flist;
        flist->next = NULL;
        flist->index = index;
        flist->index_db = DBFILE_INVALID;
        flist->rwflag = true;
        flist->once = false;
        flist->fullpath = NULL;
        flist->mtime = 0;
        if( stat( filename, &statbuf ) == 0 ) {
            flist->mtime = statbuf.st_mtime;
        }
    }
    return( flist );
}

FNAMEPTR FileIndexToFName( unsigned file_index )
{
    FNAMEPTR    flist;

    for( flist = FNames; flist != NULL; flist = flist->next ) {
        if( flist->index == file_index ) {
            break;
        }
    }
    return( flist );
}

char *FNameFullPath( FNAMEPTR flist )
{
    char   fullbuff[2 * _MAX_PATH];
    char   *fullpath;

    if( flist->fullpath == NULL ) {
        fullpath = SrcFullPath( flist->name, fullbuff, sizeof( fullbuff ) );
        if( fullpath != NULL ) {
            fullpath = CStrSave( fullpath );
            flist->fullpath = fullpath;
        } else {
            fullpath = flist->name;
        }
    } else {
        fullpath = flist->fullpath;
    }
    return( fullpath );
}

char *FileIndexToCorrectName( unsigned file_index )
{
    FNAMEPTR    flist;
    char        *name;

    if( NULL == (flist = FileIndexToFName( file_index )) )
        return( NULL );
    if( CompFlags.ef_switch_used ) {
        name = FNameFullPath( flist );
    } else {
        name = flist->name;
    }
    return( name );
}

void FreeFNames( void )
{
    FNAMEPTR    flist;

    while( (flist = FNames) != NULL ) {
        FNames = flist->next;
        if( flist->fullpath != NULL ) {
            CMemFree( flist->fullpath );
        }
        CMemFree( flist );
    }
}

void AddIncFile( INCFILE *ifile )
{
    INCFILE     *ifilep;

    ifile->nextfile = NULL;
    if( IncFileList == NULL ) {
        IncFileList = ifile;
    } else {
        ifilep = IncFileList;
        while( ifilep->nextfile != NULL )
            ifilep = ifilep->nextfile;
        ifilep->nextfile = ifile;
    }
}

void AddIncFileList( const char *filename )
{
    INCFILE     *ifile;
    size_t      len;

    len = strlen( filename ) + 1;
    ifile = (INCFILE *)CMemAlloc( offsetof( INCFILE, filename ) + len );
    memcpy( ifile->filename, filename, len );
    AddIncFile( ifile );
}

void FreeIncFileList( void )
{
    INCFILE *ilist;

    while( (ilist = IncFileList) != NULL ) {
        IncFileList = ilist->nextfile;
        CMemFree( ilist );
    }
}

static RDIRPTR AddRDir( const char *path )
{
    RDIRPTR     dirlist;
    RDIRPTR     *lnk;
    size_t      len;

    for( lnk = &RDirNames; (dirlist = *lnk) != NULL; lnk = &dirlist->next ) {
        if( stricmp( path, dirlist->name ) == 0 ) {
            break;
        }
    }
    if( dirlist == NULL ) {
        len = strlen( path ) + 1;
        dirlist = (RDIRPTR)CMemAlloc( offsetof( rdir_list, name ) + len );
        dirlist->next = NULL;
        memcpy( dirlist->name, path, len );
        *lnk = dirlist;
    }
    return( dirlist );
}

void FreeRDir( void )
{
    RDIRPTR    dirlist;

    while( (dirlist = RDirNames) != NULL ) {
        RDirNames = dirlist->next;
        CMemFree( dirlist );
    }
}

void SrcFileReadOnlyDir( char const *dirs )
{ // add dir to ro set
    char    *full;              // - full path
    char    path[_MAX_PATH];  // - used to extract directory
    char    buff[_MAX_PATH];  // - expanded path for directory

    while( *dirs != '\0' ) {
        char *p = path;
        dirs = GetPathElement( dirs, NULL, &p );
        *p = '\0';
        full = SrcFullPath( path, buff, sizeof( buff ) );
        AddRDir( full );
    }
}

bool SrcFileInRDir( FNAMEPTR flist )
{
    RDIRPTR     dirlist;
    bool        read_only;      // - true ==> file is in read-only directory
    char        *fullpath;      // - full path

    read_only = false;
    fullpath = FNameFullPath( flist );
    for( dirlist = RDirNames; dirlist != NULL; dirlist = dirlist->next ) {
        if( strnicmp( dirlist->name, fullpath, strlen( dirlist->name ) ) == 0 ) {
            read_only = true;
            break;
        }
    }
    return( read_only );
}

void SrcFileReadOnlyFile( char const *file )
{
    FNAMEPTR    flist;

    if( file == NULL ) {
        flist = SrcFile->src_flist;
    } else {
        flist= FindFlist( file );
    }
    if( flist  != NULL ) {
        flist->rwflag = false;
    }
}

void SrcFileIncludeAlias( const char *alias_name, const char *real_name, bool is_lib )
{
    AddIAlias( alias_name, real_name, is_lib );
}

void SetSrcFNameOnce( void )
{
    SrcFile->src_flist->once = true;
}

static void ParseInit( void )
{
    const char  *force;

    ScanInit();
    CTypeInit();
    MacroInit();
    SymInit();
    SpcSymInit();
    StringInit();
    InitDataQuads();
    ExprInit();
    StmtInit();
    SegInit();
    force = FEGetEnv( "FORCE" );
    if( force != NULL ) {
        ForceInclude = CStrSave( force );
    }
    ForcePreInclude = CStrSave( "_preincl.h" );
}

static bool OpenPgmFile( void )
{
    if( IsStdIn ) {
        if( OpenFCB( stdin, "stdin", FT_SRC ) ) {
            if( CompFlags.cpp_output ) {
                if( CppFile == NULL )
                    OpenCppFile();
                EmitPoundLine( 1, "stdin", true );
                CppFirstChar = true;
            }
            MainSrcFile = SrcFile;
            return( true );
        }
        return( false );
    }
    if( !OpenSrcFile( WholeFName, FT_SRC ) ) {
        CantOpenFile( WholeFName );
        CSuicide();
        return( false );
    }
#if _CPU == 370
    SrcFile->colum = Column;
    SrcFile->trunc = Trunc;
#endif
    MainSrcFile = SrcFile;
    return( true );
}


static void Parse( void )
{
    EmitInit();
    // The first token in a file should be #include if a user wants to
    // use pre-compiled headers. The following call to NextToken() to
    // get the very first token of the file will load the pre-compiled
    // header if the user requested such and it is a #include directive.
    if( ForceInclude != NULL ) {
        if( !OpenSrcFile( ForceInclude, FT_HEADER_FORCED ) ) {
            CppPrtfFilenameErr( ForceInclude, FT_HEADER_FORCED, true );
        }
    }
    if( ForcePreInclude != NULL ) {
        openForcePreInclude();
    }
    CompFlags.ok_to_use_precompiled_hdr = true;
    NextToken();
    // If we didn't get a #include with the above call to NextToken()
    // it's too late to use pre-compiled header now.
    CompFlags.ok_to_use_precompiled_hdr = false;
    ParsePgm();
    if( DefFile != NULL ) {
        fclose( DefFile );
        DefFile = NULL;
    }
    ChkCallParms();
    EndBlock();     /* end of block 0 */
    MacroFini();
}

static void CPP_Parse( void )
{
    if( ForceInclude != NULL ) {
        CppPrtChar( '\n' );
        if( !OpenSrcFile( ForceInclude, FT_HEADER_FORCED ) ) {
            CppPrtfFilenameErr( ForceInclude, FT_HEADER_FORCED, true );
        }
    }
    CurToken = T_NULL;
    while( CurToken != T_EOF ) {
        GetNextToken();
        CppPrtToken();
    }
    MacroFini();
}

static void DoCCompile( char **cmdline )
/**************************************/
{
    jmp_buf     env;

    Environment = &env;
    if( setjmp( env ) ) {           /* if fatal error has occurred */
        EmitAbort();                /* abort code generator */
        CPragmaFini();
        CloseFiles();
        FreeFNames();
        FreeRDir();
        FreeIAlias();
        ErrCount = 1;
        MyExit( 1 );
    }
    ParseInit();
    if( ParseCmdLine( cmdline ) ) {
        if( WholeFName == NULL ) {
            CErr1( ERR_FILENAME_REQUIRED );
            return;
        }
        MakePgmName();
        DelErrFile();               /* delete old error file */
        MergeInclude();             /* merge INCLUDE= with IncPathList */
        CPragmaInit();              /* memory model is known now */
#if _CPU == 370
        ParseAuxFile();
#endif
        if( CompFlags.cpp_output ) {
            PrintWhiteSpace = true;
            if( ForcePreInclude != NULL ) {
                CompFlags.cpp_output = false;
                if( openForcePreInclude() ) {
                    CurToken = T_NULL;
                    while( CurToken != T_EOF ) {
                        GetNextToken();
                    }
                }
                CompFlags.cpp_output = true;
            }
            OpenPgmFile();
            CPP_Parse();
            if( !CompFlags.quiet_mode ) {
                PrintStats();
            }
            if( CompFlags.warnings_cause_bad_exit ) {
                ErrCount += WngCount;
            }
        } else {
            OpenPgmFile();
            MacroAddComp(); // Add any compile time only macros
            Parse();
            if( !CompFlags.quiet_mode ) {
                PrintStats();
            }
            if( CompFlags.warnings_cause_bad_exit ) {
                ErrCount += WngCount;
            }
            if( ( ErrCount == 0 ) && ( !CompFlags.check_syntax ) ) {
                if( CompFlags.emit_browser_info ) {
                    DwarfBrowseEmit();
                }
                FreeMacroSegments();
                DoCompile();
            } else {
                FreeMacroSegments();
            }
        }
        if( ErrCount == 0 ) {
            DumpDepFile();
        } else {
            DelDepFile();
        }

        SymFini();
        CPragmaFini();
    } else {
        ErrCount = 1;
    }
    CloseFiles();
    FreeFNames();
    FreeRDir();
    FreeIAlias();
    FreeIncFileList();
}

static bool try_open_file( const char *path, path_descr *fp, path_descr *fa, src_file_type typ )
{
    bool    ok;
    bool    truncated;
    char    save_chr_name;
    char    save_chr_ext;

    // try to open regular name
    ok = TryOpen( path, fp, typ );
    if( ok ) {
        return( ok );
    }
    if( fa != NULL ) {
        // try to open alias name if defined
        ok = TryOpen( path, fa, typ );
        if( ok ) {
            return( ok );
        }
    }
    if( CompFlags.check_truncated_fnames ) {
        save_chr_name = fp->fnm[8];
        save_chr_ext = fp->ext[4];
        truncated = false;
        if( strlen( fp->fnm ) > 8 ) {
            fp->fnm[8] = '\0';
            truncated = true;
        }
        if( strlen( fp->ext ) > 4 ) {
            fp->ext[4] = '\0';
            truncated = true;
        }
        if( truncated ) {
            // try to open truncated name if enabled
            ok = TryOpen( path, fp, typ );
            if( ok ) {
                return( ok );
            }
            fp->fnm[8] = save_chr_name;
            fp->ext[4] = save_chr_ext;
        }
    }
    return( ok );
}

static bool doOpenSrcFile( path_descr *fp, path_descr *fa, src_file_type typ )
{
    char        *s;
    char        *p;
    char        buff[_MAX_PATH2];
    char        try[_MAX_PATH];
    FCB         *curr;
    char        c;
    path_descr  fd;

    if( typ == FT_SRC ) {
        if( try_open_file( "", fp, NULL, typ ) )
            return( true );
        if( !CompFlags.ignore_default_dirs ) {
            if( try_open_file( C_PATH, fp, NULL, typ ) ) {
                return( true );
            }
        }
        return( false );
    }
    if( fp->drv[0] != '\0' || IS_DIR_SEP( fp->dir[0] ) ) {
        // try absolute path
        // if drive letter given or path from root given
        if( try_open_file( "", fp, fa, typ ) ) {
            return( true );
        }
    } else {
        switch( typ ) {
        case FT_HEADER_PRE:
        case FT_LIBRARY:
            break;
        case FT_HEADER:
        case FT_HEADER_FORCED:
            if( CompFlags.ignore_default_dirs ) {
                try[0] = '\0';
                // physical file name must be used, not logical
                _splitpath2( SrcFile->src_flist->name, fd.buffer, &fd.drv, &fd.dir, NULL, NULL );
                _makepath( try, fd.drv, fd.dir, NULL, NULL );
                if( try_open_file( try, fp, fa, typ ) ) {
                    return( true );
                }
            } else {
                // try current directory
                if( !GlobalCompFlags.ignore_current_dir ) {
                    if( try_open_file( "", fp, fa, typ ) ) {
                        return( true );
                    }
                }
                for( curr = SrcFile; curr!= NULL; curr = curr->prev_file ) {
                    // physical file name must be used, not logical
                    _splitpath2( curr->src_flist->name, fd.buffer, &fd.drv, &fd.dir, NULL, NULL );
                    _makepath( try, fd.drv, fd.dir, NULL, NULL );
                    if( try_open_file( try, fp, fa, typ ) ) {
                        return( true );
                    }
                }
            }
            break;
        }
        s = IncPathList;
        while( (c = *s) != '\0' ) {
            p = buff;
            do {
                ++s;
                if( IS_PATH_LIST_SEP( c ) )
                    break;
                *p++ = c;
            } while( (c = *s) != '\0' );
            c = p[-1];
            if( !IS_PATH_SEP( c ) ) {
                *p++ = DIR_SEP;
            }
            *p = '\0';
            if( try_open_file( buff, fp, fa, typ ) ) {
                return( true );
            }
        }
        switch( typ ) {
        case FT_HEADER_PRE:
        case FT_LIBRARY:
            break;
        case FT_HEADER:
        case FT_HEADER_FORCED:
            if( !CompFlags.ignore_default_dirs ) {
                // try current ../h directory
                if( try_open_file( H_PATH, fp, fa, typ ) ) {
                    return( true );
                }
            }
            break;
        }
    }
    return( false );
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

bool OpenSrcFile( const char *filename, src_file_type typ )
{
    const char  *alias_filename;
    path_descr  fp;
    path_descr  fa;
    path_descr  *fap;

    _splitpath2( filename, fp.buffer, &fp.drv, &fp.dir, &fp.fnm, &fp.ext );
    normalizeSep( fp.dir );
    fap = NULL;
    switch( typ ) {
    case FT_SRC:
        break;
    case FT_HEADER:
    case FT_HEADER_FORCED:
    case FT_HEADER_PRE:
    case FT_LIBRARY:
        // See if there's an alias for this filename
        alias_filename = IncludeAlias( filename, ( typ == FT_LIBRARY || typ == FT_HEADER_PRE ) );
        if( alias_filename != NULL ) {
            _splitpath2( alias_filename, fa.buffer, &fa.drv, &fa.dir, &fa.fnm, &fa.ext );
            normalizeSep( fa.dir );
            fap = &fa;
        }
        break;
    }
    return( doOpenSrcFile( &fp, fap, typ ) );
}


void EmitPoundLine( unsigned line_num, const char *filename, bool newline )
{
    if( CompFlags.cpp_line_wanted && CppPrinting() ) {
        CppPrtf( "#line %u \"", line_num );
        while( *filename != '\0' ) {
            CppPutc( *filename );
            ++filename;
        }
        CppPutc( '\"' );
        if( newline ) {
            CppPutc( '\n' );
        }
    }
}

void EmitLine( unsigned line_num, const char *filename )
{
    EmitPoundLine( line_num, filename, false );
}

bool CppPrinting( void )
{
    return( NestLevel == SkipLevel );
}

void CppPrtToken( void )
{
    if( CppPrinting() ) {
        switch( CurToken ) {
        case T_BAD_CHAR:
        case T_BAD_TOKEN:
        case T_ID:
        case T_CONSTANT:
            CppPrtf( "%s", Buffer );
            break;
        case T_STRING:
            if( CompFlags.wide_char_string )
                CppPutc( 'L' );
            CppPrtf( "\"%s\"", Buffer );
            break;
        case T_EOF:
        case T_NULL:
            break;
        case T_WHITE_SPACE:
            if( PrintWhiteSpace || CompFlags.in_pragma ) {
                CppPrtf( "%s", Tokens[CurToken] );
            } else {
                PrintWhiteSpace = true; //Toggle
            }
            break;
        default:
            CppPrtf( "%s", Tokens[CurToken] );
        }
    }
}


void GetNextToken( void )
{
    CurToken = T_NULL;
    if( MacroPtr != NULL ) {
        GetMacroToken();
    } else {
        while( CurrChar != EOF_CHAR ) {
            if( (CharSet[CurrChar] & C_WS) == 0 )
                break;
            if( CurrChar != '\r' )
                CppPrtChar( CurrChar );
            NextChar();
        }
        CurToken = ScanToken();
    }
}

void CloseFiles( void )
{
    if( CppFile != NULL ) {
        fflush( CppFile );
        if( ferror( CppFile ) ) {
            char    msgtxt[80];
            char    msgbuf[MAX_MSG_LEN];

            /* issue message */
            CGetMsg( msgtxt, ERR_FATAL_ERROR );
            sprintf( msgbuf, msgtxt, strerror( errno ) );
            NoteMsg( msgbuf );
        }
        fclose( CppFile );
        CppFile = NULL;
    }
    if( ErrFile != NULL ) {
        fclose( ErrFile );
        ErrFile = NULL;
        if( ! CompFlags.errfile_written ) {
            DelErrFile();
        }
    }
    if( DefFile != NULL ) {
        fclose( DefFile );
        DefFile = NULL;
    }
}


int FrontEnd( char **cmdline )
{
#if defined(__WATCOMC__) && defined( _M_IX86 )
    /* set to 0 in case 8087 is present */
    _real87 = 0;
    _8087 = 0;
#endif

    InitGlobalVars();
    CMemInit();
    InitMsg();
    InitPurge();

    SwitchChar = _dos_switch_char();
    ClearGlobals();
    DoCCompile( cmdline );
    PurgeMemory();
    FiniMsg();
    CMemFini();
    GlobalCompFlags.cc_first_use = false;
    return( ErrCount );
}
