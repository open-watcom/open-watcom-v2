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


#include "cvars.h"
#include "iopath.h"
#include "scan.h"
#include "standard.h"
#include "cg.h"
#include "cgdefs.h"
#include "cgswitch.h"
#include "cgprotos.h"
#include <stdarg.h>
#include <signal.h>
#include <ctype.h>
#if _OS == _CMS
    #include <file.h>
    #include <errno.h>
    #include <setup.h>
    #include <depsets.h>
    #define FEGetEnv EnvVar
    /* plist format to be passed to main */
    int const _parms = UNTOKENIZED;
    int const _staksize = (100*1024);/* stack size */
#elif _OS == _QNX
    #include <stdio.h>
    #include <i86.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <sys/stat.h>
#else
    #include "dos.h"
    #include <io.h>
    #include <fcntl.h>
    #include <process.h>
#endif
#ifdef __OSI__
    #include "ostype.h"
#endif
#include <conio.h>
extern  char    CharSet[];

static char WorkFile[] = "__wrk0__";
static char IsStdIn;
static int IncFileDepth;

void FrontEndInit( bool reuse ) {
//**************************//
// Do the once only things //
//*************************//
    GlobalCompFlags.cc_reuse = reuse;
    GlobalCompFlags.cc_first_use = TRUE;
}

void FrontEndFini( void ) {
//********************//
// Fini the once only //
//********************//
    GlobalCompFlags.cc_reuse = FALSE;
    GlobalCompFlags.cc_first_use = TRUE;
}

void ClearGlobals()
{
    InitStats();
    IsStdIn = 0;
    FNames = NULL;
    RDirNames = NULL;
    SrcFile = NULL;
    ErrFile = NULL;
    DefFile = NULL;
    CppFile = NULL;
    SymLoc  = NULL;
    HFileList = NULL;
    IncFileDepth = 255;
    PageHandle = -1;
    WorkFile[5] = '0';
    SegmentNum = FIRST_PRIVATE_SEGMENT;
}



unsigned char _8087;                                    /* 27-may-91 */
unsigned char _real87;

int FrontEnd( char **cmdline )
{
        _real87 = _8087 = 0;/* set to 0 in case 8087 is present; 27-may-91 */
#if _HOST == 386
        _amblksiz = 16;
#endif
    InitGlobalVars();
    CMemInit();
    InitMsg();
    InitPurge();

    SwitchChar = _dos_switch_char();
    ClearGlobals();
    #if _OS == _CMS
        setxedit( 1 ); /* mjc */
    #endif
    DoCCompile( cmdline );
    PurgeMemory();
    FiniMsg();
    CMemFini();
    GlobalCompFlags.cc_first_use = FALSE;
    return( ErrCount );
}

#if defined(__QNX__)
#define IS_PATH_SEP( ch ) ((ch) == '/')
#else
#define IS_PATH_SEP( ch ) ((ch) == '/' || (ch) == '\\')
#endif

local void MakeTmpName( char *fname )
{
    #if _OS == _CMS
        tmpnam( fname );
    #else
        char    *env;
        int     i;

        #if _OS == _QNX
            env = FEGetEnv( "TMPDIR" );
            if( env == NULL ) env = FEGetEnv( "TMP" );
        #else
            env = FEGetEnv( "TMP" );
        #endif
        if( env == NULL ) env = "";

        #define TMP_EXT ".tmp"
        #define MAX_TMP_PATH (_MAX_PATH - sizeof( WorkFile ) - sizeof( TMP_EXT ) - 2)

        strncpy( fname, env, MAX_TMP_PATH );
        fname[ MAX_TMP_PATH ] = '\0';
        i = strlen( fname );
        if( i > 0 && !IS_PATH_SEP( fname[i-1] )
        #if _OS != _QNX
                && fname[i-1] != ':'
        #endif
            ) {
            fname[i++] = '/';
        }
        strcpy( &fname[i], WorkFile );
        strcpy( &fname[i+sizeof(WorkFile)-1], TMP_EXT );
    #endif
}

#if _OS == _QNX

void OpenPageFile()
{
    auto char fname[ _MAX_PATH ];

    for(;;) {
        MakeTmpName( fname );
        PageHandle = open( fname, O_RDWR | O_CREAT | O_EXCL | O_TEMP,
                            S_IRUSR | S_IWUSR );
        if( PageHandle != -1 ) break;
        if( WorkFile[5] == '9' ) WorkFile[5] = 'A' - 1;     /* 11-may-89 */
        WorkFile[5]++;          /* change the digit */
        if( WorkFile[5] == '\0' ) {
            CErr2( ERR_UNABLE_TO_OPEN_WORK_FILE, errno );
            CSuicide();
        }
    }
    /* Under POSIX it's legal to remove a file that's open. The file
       space will be reclaimed when the handle is closed. This makes sure
       that the work file always gets removed. */
    remove( fname );
    PageFile = fdopen( PageHandle, "r+b" );
    if( PageFile == NULL ) {
        CErr2( ERR_UNABLE_TO_OPEN_WORK_FILE, errno );
        CSuicide();
    }
}

#else

#if _OS == _DOS
    #define ERRVAL _doserrno
#else
    #define ERRVAL errno
#endif

void OpenPageFile()
{
    auto char fname[ _MAX_PATH ];

    for(;;) {
        MakeTmpName( fname );
        if( access( fname, 0 ) != 0 ) break;
        if( WorkFile[5] == '9' ) WorkFile[5] = 'A' - 1;     /* 11-may-89 */
        WorkFile[5]++;          /* change the digit */
    }
    PageHandle = -1;
    PageFile = fopen( fname, "w+b" );
    if( PageFile == NULL ) {
        CErr2( ERR_UNABLE_TO_OPEN_WORK_FILE, ERRVAL );
        CSuicide();
    }
    PageHandle = fileno( PageFile );
}

#endif


void CloseFiles()
{
    #if _OS != _QNX
        auto char fname[ _MAX_PATH ];
    #endif

    if( CppFile != NULL ) {
        fflush( CppFile );
        if( ferror( CppFile ) ) {
            char  msgtxt[80];
            char  msgbuf[MAX_MSG_LEN];

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
    if( PageFile != NULL ) {
        fclose( PageFile );
        #if _OS != _QNX
            MakeTmpName( fname );
            remove( fname );
        #endif
        PageHandle = -1;
        PageFile = NULL;
    }
}

static bool ParseCmdLine( char **cmdline )
{
    char        *cmd;

    cmd = (cmdline == NULL) ?  "" : cmdline[0];  // from cpp.c
    if( cmd == NULL ) { // argv == 1
        cmd = "";
    }
    while( *cmd == ' ' ) ++cmd;                     /* 13-mar-90 */
    if( *cmd == '?' || *cmd == '\0' ) {
        CBanner();
        CCusage();
        return( FALSE );
    }
    GenCOptions( cmdline );
    FESetCurInc();
    if( WholeFName != NULL ) {
        MakePgmName( );
        OpenPgmFile();
        MainSrcFile = SrcFile;                  /* 05-jan-94 */
    }
    return( TRUE );
}

void DoCCompile( char **cmdline )
/******************************/
{
    auto jmp_buf env;

    Environment = &env;
    if( setjmp( env ) ) {       /* if fatal error has occurred */
        EmitAbort();                /* abort code generator */
        PragmaFini();
        CloseFiles();
        FreeFNames();
        FreeRDir();
        ErrCount = 1;
        MyExit( 1 );
    }
    ParseInit();
    ForceInclude = FEGetEnv( "FORCE" );
    if( ParseCmdLine( cmdline ) ) {
        if( WholeFName == NULL ) {
            CErr1( ERR_FILENAME_REQUIRED );
            return;
        }
        DelErrFile();               /* delete old error file */
        OpenErrFile();              /* open error file just in case */
        MergeInclude();             /* merge INCLUDE= with HFileList */
        CPragmaInit();              /* memory model is known now */
        #if _CPU == 370
            ParseAuxFile();
        #endif
        if( CompFlags.cpp_output ) {
            CPP_Parse();
            if( ! CompFlags.quiet_mode ) PrintStats();
        } else {
            MacroAddComp(); // Add any compile time only macros
            Parse();
            if( ! CompFlags.quiet_mode ) PrintStats();
            if( ( ErrCount == 0 ) && ( ! CompFlags.check_syntax ) ) {
                if( CompFlags.emit_browser_info ) DwarfBrowseEmit();
                FreeMacroSegments();
                DoCompile();
            } else {
                FreeMacroSegments();
            }
        }
        SymFini();
        PragmaFini();
    } else {
        ErrCount = 1;
    }
    CloseFiles();
    FreeFNames();
    FreeRDir();
}





/* open the primary source file, and return pointer to root file name */

#define STDIN_NAME      "stdin"

extern char FindIt[10] = "cgtype";

static void MakePgmName( void )
{
// Get fname, if file name has no extension wack ".c" on
// if stdin a "." then replace with "stdin" don't wack ".c"
// If no module name make the same as fname
    int         len;
    char *ptr;
    char buff[ _MAX_PATH2 ];
    char *drive;
    char *dir;
    char *fname;
    char *ext;

    ptr = WholeFName;
    if( ptr[0] == '.' && ptr[1] == '\0' ) {
        IsStdIn = 1;
        CMemFree( WholeFName );
        len = strlen( STDIN_NAME );
        WholeFName = CMemAlloc( len + sizeof( char ) );
        strcpy( WholeFName, STDIN_NAME );
        fname = WholeFName;
    } else {
         _splitpath2( ptr, buff, &drive, &dir, &fname, &ext );
        if(  *ext == '\0' ) { // no extension
            char *new;

            len = strlen( WholeFName );
            len += sizeof( C_EXT );      /* for the ".c\0" */
            new = CMemAlloc( len );
            strcpy( new, WholeFName );
            strcat( new, C_EXT );
            CMemFree( WholeFName );
            WholeFName = new;
        }
    }
    len = strlen( fname );
    SrcFName = CMemAlloc( len + sizeof( char ) );
//  if( strcmp( fname, FindIt ) == 0 ) {
//      printf( "here\n" );
//  }
    strcpy( SrcFName, fname );
    if( ModuleName == NULL ) ModuleName = SrcFName;
}

local void CantOpenFile( char *name )
{
    char msgtxt[80];
    char  msgbuf[MAX_MSG_LEN];

    CGetMsg( msgtxt, ERR_CANT_OPEN_FILE );
    sprintf( msgbuf, msgtxt, name );
    BannerMsg( msgbuf );
}

int OpenPgmFile()
{
    if( IsStdIn ) {
        return( OpenFCB( stdin, "stdin" ) );
    }
    if( TryOpen( "", "", WholeFName, "" ) == 0 ) {
        if( TryOpen( C_PATH, PATH_SEP, WholeFName, "" ) == 0 ) {
            CantOpenFile( WholeFName );
            CSuicide();
            return( 0 );
        }
    }
    #if _CPU == 370
        SrcFile->colum = Column;
        SrcFile->trunc = Trunc;
    #endif
    return( 1 );
}


static char *CreateFileName( char *template, char *extension )
{
    #if _OS != _CMS
        char buff[ _MAX_PATH2 ];
        char *drive;
        char *dir;
        char *fname;
        char *ext;
        char    *path;

        path = (template == NULL) ? WholeFName : template;

        _splitpath2( path, buff, &drive, &dir, &fname, &ext );
        if( template == NULL || ext[0] == '\0' ) {
            ext = extension;
        }
        if( fname[0] == '\0' || fname[0] == '*' ) {
            fname = ModuleName;
        }
        if( template == NULL ) {
            /* default object file goes in current directory */
            drive = "";
            dir = "";
        }
        _makepath( Buffer, drive, dir, fname, ext );
    #else
        char    *p;

        if( template == NULL )  template = WholeFName;
        strcpy( Buffer, template  );
        p = Buffer;
        while( *p != '\0' && *p != ' ' ) ++p;
        strcpy( p, extension );
    #endif
    return( Buffer );
}

char *ObjFileName( char *ext )
{
    return( CreateFileName( ObjectFileName, ext ) );
}

char *ErrFileName()
{
    if( ErrorFileName == NULL ) return( NULL );
    return( CreateFileName( ErrorFileName, ERR_EXT ) );
}

void PrtChar( int c )
{
    if( ! CppPrinting() ) return;
    CppPutc( c );
}


static unsigned CppColumn = 0;
static unsigned CppWidth = 0;
static unsigned CommentChar = 0;
static unsigned CppFirstChar = 0;

void SetCppWidth( unsigned width )
{
    CppWidth = width - 1;
}

void CppComment( int ch )
{
    if( !CompFlags.cpp_output ) return;
    if( !CompFlags.keep_comments ) return;
    if( !CppPrinting() ) return;
    if( ch != 0 ) {
        if( CppColumn+2 >= CppWidth ) CppPutc( '\n' );
        CppPutc( '/' );
        CppPutc( ch );
    } else if( CommentChar == '*' ) {
        if( CppColumn+2 >= CppWidth ) CppPutc( '\n' );
        CppPutc( '*' );
        CppPutc( '/' );
//  } else if( CommentChar == '/' ) {
//      CppPutc( '\n' );
    }
    CommentChar = ch;
}


void CppPutc( int ch )
{

    if( CppFirstChar ) {
        CppFirstChar = 0;
        if( ch == '\n' ) {
            return;
        }
    }
    if( ch == '\n' ) {
        CppColumn = 0;
    } else if( CppColumn >= CppWidth ) {
        if( CommentChar == 0 ) {
           if( fputc( '\\', CppFile ) < 0 )goto werror;
        }
        if( fputc( '\n', CppFile ) < 0 )goto werror;
        if( CommentChar == '/' ) {
            if( fputc( '/', CppFile ) < 0 )goto werror;
            if( fputc( '/', CppFile ) < 0 )goto werror;
            CppColumn = 3;
        } else {
            CppColumn = 1;
        }
    } else {
        ++CppColumn;
    }
    if( fputc( ch, CppFile ) < 0 )goto werror;
    return;
werror:
    CloseFiles();   /* get rid of temp file */
    MyExit( 1 );     /* exit to DOS do not pass GO */
    return;
}


void CppPrtf( char *fmt, ... )
{
    va_list     arg;
    char        *p;
    char        buff[BUF_SIZE];

    va_start( arg, fmt );
    vsprintf( buff, fmt, arg );
    for( p = buff; *p != '\0'; ++p ) {
        CppPutc( *p );
    }
    va_end( arg );
}


void OpenCppFile()
{
    char        *name;

    if( CompFlags.cpp_output_to_file ) {                /* 29-sep-90 */
        name = ObjFileName( CPP_EXT );
        CppFile = fopen( name, "w" );
    } else {
        CppFile = stdout;
    }
    CppColumn = 0;
    CppFirstChar = 1;
    if( CppFile == NULL ) {
        CantOpenFile( name );
        MyExit( 1 );
    } else {
        if( CppWidth == 0 ) {
            #if _OS == _CMS
                CppWidth = lrecl( fileno( CppFile ) )-1;
            #else
                CppWidth = ~0;
            #endif
        }
        setvbuf( CppFile, CPermAlloc( 512 ), _IOFBF, 512 );
    }
}


void OpenDefFile()
{
#if _OS != _CMS
    char buff[_MAX_PATH2];
    char name[_MAX_PATH ];
    char *fname;

    if( DefFName == NULL ) {
        _splitpath2( SrcFName, buff, NULL, NULL, &fname, NULL );
        _makepath( name, NULL, NULL, fname, DEF_EXT );
        DefFile = fopen( name, "w" );
    } else {
        DefFile = fopen( DefFName, "w" );
    }
    if( DefFile != NULL ) {
        setvbuf( DefFile, CPermAlloc( 32 ), _IOFBF, 32 );
    }
#else
    DefFile = fopen( DefFName, "w" );
#endif
}

FILE *OpenBrowseFile()
{
    char        buff[_MAX_PATH2];
    char        name[_MAX_PATH];
    char        *fname;
    FILE        *mbr_file;

    if( CompFlags.cpp_output_to_file ) {                /* 29-sep-90 */
        strcpy(   name,  ObjFileName( MBR_EXT ) );
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


void DelErrFile()
{
    char        *name;

    name = ErrFileName();
    if( name != NULL ) remove( name );
}


int OpenSrcFile( char *filename, int delimiter )
{
    int         i;
    char        *p;
    char        buff[_MAX_PATH2];
    char        try[_MAX_PATH];
    char        *drive;
    char        *dir;
    char        *name;
    char        *ext;
    int         save;
    FCB         *curr;

    _splitpath2( filename, buff, &drive, &dir, &name, &ext );
    if( drive[0] != '\0' || IS_PATH_SEP(dir[0]) ) {
        // 14-sep-94 if drive letter given or path from root given
        if( TryOpen( "", "", filename, "" ) != 0 ) return( 1 );
        goto cant_open_file;
    }
    if( delimiter != '<' ) {                                /* 17-mar-91 */
        if( CompFlags.curdir_inc ) {  // try current directory
            if( TryOpen( "", "", filename, "" ) != 0 ) return( 1 );
        }
        if( drive[0] == '\0' && !IS_PATH_SEP( dir[0] ) ) {
            for( curr = SrcFile; curr!= NULL; curr = curr->prev_file ) {
                _splitpath2( curr->src_name, buff, &drive, &dir, &name, &ext );
                _makepath( try, drive, dir, filename, NULL );
                if( TryOpen( "", "", try, "" ) != 0 ) return(1);
            }
        }
    }
    if( HFileList != NULL ) {
        p = HFileList;
        do {
            i = 0;
            while( *p == ' ' ) ++p;                     /* 28-feb-95 */
            for(;;) {
                if( *p == INCLUDE_SEP ) break;
                if( *p == '\0' ) break;
                if( i < sizeof(buff)-2 ) {
                    buff[i++] = *p;
                }
                ++p;
            }
            while( i != 0 ) {                           /* 28-feb-95 */
                if( buff[i-1] != ' ' ) break;
                --i;
            }
#define SEP_LEN (sizeof( PATH_SEP ) - 1)
            buff[i] = '\0';
            if( i>=SEP_LEN && strcmp( &buff[i-SEP_LEN], PATH_SEP )==0 ) {
                buff[i-SEP_LEN] = '\0';
            }
            if( TryOpen( buff, PATH_SEP, filename, "" ) != 0 ) return(1);
            if( *p == INCLUDE_SEP ) ++p;
        } while( *p != '\0' );
    }
    if( delimiter != '<' ) {                        /* 17-mar-91 */
        if( TryOpen( H_PATH, PATH_SEP, filename, "" ) != 0 ) return( 1 );
    }
cant_open_file:
    save = CompFlags.cpp_output;
    if( CompFlags.cpp_output ) {                        /* 18-aug-91 */
        if( delimiter == '<' ) {
            CppPrtf( "#include <%s>", filename );
        } else {
            CppPrtf( "#include \"%s\"", filename );
        }
        CompFlags.cpp_output = 0;
    }
    CErr2p( ERR_CANT_OPEN_FILE, filename );
    CompFlags.cpp_output = save;
    return( 0 );
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
    ++IncFileDepth;
    if( srcfcb->src_fp != NULL ) {          /* not in-memory buffer */
        CClose( srcfcb->src_fp );
    }
    FEfree( srcfcb->src_buf );
    --srcfcb->src_line;
    if( CompFlags.scanning_comment ) {
        CErr1( ERR_INCOMPLETE_COMMENT );
    }
    SrcFile = srcfcb->prev_file;
    CurrChar = srcfcb->prev_currchar;
    if( SrcFile == MainSrcFile ) {
        if( CompFlags.make_precompiled_header ) {
            CompFlags.make_precompiled_header = 0;
            if( ErrCount == 0  ) {
                BuildPreCompiledHeader( PCH_FileName );
            }
        }
    }
    if( SrcFile != NULL ) {
        if(  SrcFile->src_fp == NULL ) {
            SrcFile->src_fp = fopen( SrcFile->src_name, "rb" );
            fseek( SrcFile->src_fp, SrcFile->rseekpos, SEEK_SET );
        }
        ScanCharPtr = SrcFile->src_ptr; // get scan ptr from prev file
        TokenFno = SrcFile->src_fno;
        ErrFName = SrcFile->src_name;
        IncLineCount += srcfcb->src_line;
        SrcFileLineNum = SrcFile->src_line;
        if( CompFlags.cpp_output ) {
            EmitPoundLine( SrcFile->src_line, SrcFile->src_name, 1 );
        }
    } else {
        SrcLineCount = srcfcb->src_line;
        CurrChar = EOF_CHAR;
    }
    CMemFree( srcfcb );
}

static int OpenFCB( FILE *fp, char *filename )
{
    if( FCB_Alloc( fp, filename ) == 0 ) {       /* split apart 19-sep-89 */
        CErr1( ERR_OUT_OF_MEMORY );
        return( FALSE );
    }
    return( TRUE );
}

bool FreeSrcFP( void )
/*************/
{
    FCB *src_file;
    FCB *next;
    bool ret;

    src_file = SrcFile;
    ret = FALSE;
    while( src_file != NULL ) {
        next = src_file->prev_file;
        if( next == NULL || next->src_fp == NULL )break;
        src_file = next;
    }
    if( src_file != NULL && src_file->src_fp != NULL ) {
        src_file->rseekpos = ftell( src_file->src_fp );
        CClose( src_file->src_fp );
        src_file->src_fp = NULL;
        ret = TRUE;
    }
    return( ret );
}

static bool IsFNameOnce( char const *filename );
int TryOpen( char *prefix, char *separator, char *filename, char *suffix )
{
    int         i, j;
    FILE        *fp;
    auto char   buf[2*130];

    if( IncFileDepth == 0 ) {
        CErr2( ERR_INCDEPTH, 255 );
        CSuicide();
        return( 0 );
    }
    i = 0;
    while( buf[i] = *prefix++ )    ++i;
    while( buf[i] = *separator++ ) ++i;
    j = i;
    while( buf[i] = *filename++ )  ++i;
    while( buf[i] = *suffix++ )    ++i;
    filename = &buf[0];                 /* point to the full name */
    if( IsFNameOnce( filename ) ) {
        return( 1 );
    }
    for(;;) {
        fp = fopen( filename, "rb" );
        if( fp != NULL )break;
        if( errno != ENOMEM && errno != ENFILE && errno != EMFILE ) break;
        if( !FreeSrcFP() )break;      // try closing an include file
    }
    if( fp == NULL )  return( 0 );

    if( CompFlags.use_precompiled_header ) {
        CompFlags.use_precompiled_header = 0;
        if( UsePreCompiledHeader( filename ) == 0 ) {
            fclose( fp );
            return( 1 );
        }
    }
    if( OpenFCB( fp, filename ) ) {
        return( 1 );
    }
    fclose( fp );
    return( 0 );
}

int SrcFileTime( char const *filename, time_t *mtime )
{
    struct stat statbuf;

    if( stat( filename, &statbuf ) == 0 ) {
        *mtime = statbuf.st_mtime;
        return( 0 );
    }
    *mtime = 0;
    return( -1 );
}

static FNAMEPTR FindFlist( char const *filename )
{ // find a flist
    FNAMEPTR    flist;

    flist = FNames;
    while( flist  != NULL ) {
        if( strcmp( filename, flist->name ) == 0 ) break;
        flist = flist->next;
    }
    return( flist );
}

FNAMEPTR AddFlist( char const *filename )
{
    FNAMEPTR    flist;
    FNAMEPTR    *lnk;
    int         index;

    index = 0;
    lnk = &FNames;
    while( (flist = *lnk) != NULL ) {
        if( strcmp( filename, flist->name ) == 0 ) break;
        lnk = &flist->next;
        index++;
    }
    if( flist == NULL ) {
        flist = (FNAMEPTR)CMemAlloc( strlen( filename )
                            + sizeof( struct fname_list ) );
        flist->next = NULL;
        flist->index = index;
        flist->rwflag = TRUE;
        flist->once   = FALSE;
        flist->fullpath = NULL;
        strcpy( flist->name, filename );
        *lnk = flist;
        SrcFileTime( filename, &flist->mtime );
    }
    return( flist );
}

FNAMEPTR FileIndexToFName( unsigned file_index )
{
    FNAMEPTR    flist;

    for( flist = FNames; flist; flist = flist->next ) {
        if( flist->index == file_index )break;
    }
    return( flist );
}

char *FNameFullPath( FNAMEPTR flist )
{
    char   fullbuff[2*PATH_MAX];
    char *fullpath;


    if( flist->fullpath == NULL ) {
        fullpath = SrcFullPath( fullbuff, flist->name, sizeof( fullbuff ) );
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

static bool IsFNameOnce( char const *filename )
{
    bool ret;
    FNAMEPTR    flist;

    ret = FALSE;
    flist = FindFlist( filename );
    if( flist != NULL ) {
        ret = flist->once;
    }
    return( ret );
}

void FreeFNames( void )
{
    FNAMEPTR    flist;

    while( flist = FNames ) {
        FNames = flist->next;
        if( flist->fullpath != NULL ) {
            CMemFree( flist->fullpath );
        }
        CMemFree( flist );
    }
}

static void AddIncFileList( char *filename )
{
    INCFILE     *ifile;
    INCFILE     *ifilep;
    int         len;

    len = strlen( filename );
    ifile = (INCFILE *)CMemAlloc( sizeof(INCFILE) + len );
    ifile->len = len;
    strcpy( ifile->filename, filename );
    ifile->nextfile = NULL;
    if( IncFileList == NULL ) {
        IncFileList = ifile;
    } else {
        ifilep = IncFileList;
        while( ifilep->nextfile != NULL )  ifilep = ifilep->nextfile;
        ifilep->nextfile = ifile;
    }
}

RDIRPTR AddRDir( char *path )
{
    RDIRPTR   dirlist;
    RDIRPTR  *lnk;

    lnk = &RDirNames;
    while( (dirlist = *lnk) != NULL ) {
        if( stricmp( path, dirlist->name ) == 0 ) break;
        lnk = &dirlist->next;
    }
    if( dirlist == NULL ) {
        dirlist = (RDIRPTR)CMemAlloc( strlen( path )
                            + sizeof( struct rdir_list ) );
        dirlist->next = NULL;
        strcpy( dirlist->name, path );
        *lnk = dirlist;
    }
    return( dirlist );
}

void FreeRDir( void )
{
    RDIRPTR    dirlist;

    while( dirlist = RDirNames ) {
        RDirNames = dirlist->next;
        CMemFree( dirlist );
    }
}

static char *IncPathElement(     // GET ONE PATH ELEMENT FROM INCLUDE LIST
    const char *path,           // - include list
    char *prefix )              // - buffer to store element
{
    unsigned    length;

    length = 0;
    for( ; ; ) {
        if( *path == '\0' ) break;
        if( *path == INCLUDE_SEP ) {
            ++path;
            if( length != 0 ) {
                break;
            }
        } else {
            ++length;
            *prefix++ = *path++;
        }
    }
    if( ( length > 1 ) && IS_PATH_SEP( *(prefix-1) ) ) --prefix;
    *prefix = '\0';
    return( (char *)path );
}

void SrcFileReadOnlyDir( char const *dir )
{ // add dir to ro set
    char *full;                 // - full path
    auto char path[_MAX_PATH];  // - used to extract directory
    auto char buff[_MAX_PATH];  // - expanded path for directory

    while( *dir != '\0') {
        dir =  IncPathElement( dir, path );
        full = SrcFullPath( buff, path, sizeof( buff ) );
        AddRDir( full );
    }
}

bool SrcFileInRDir( FNAMEPTR flist ) {
    RDIRPTR   dirlist;
    bool read_only;             // - TRUE ==> file is in read-only directory
    char *fullpath;             // - full path

    read_only = FALSE;
    fullpath  = FNameFullPath( flist );
    dirlist = RDirNames;
    while( dirlist != NULL ) {
        if( strnicmp(dirlist->name , fullpath, strlen( dirlist->name ) )== 0 ) {
            read_only = TRUE;
            break;
        }
        dirlist = dirlist->next;
    }
    return( read_only );
}

void SrcFileReadOnlyFile( char const *file )
{
    FNAMEPTR    flist;

    if( file == NULL  ) {
        flist = SrcFile->src_flist;
    } else {
        flist= FindFlist( file );
    }
    if( flist  != NULL ) {
        flist->rwflag = FALSE;
    }
}

int FListSrcQue( void )
{
    FNAMEPTR    flist;
    char       *fullpath;
    int         count;
    int         fno;
    // this is all very kludged in
    count = 0;
    flist = FNames;
    while( flist != NULL ) {
        fullpath = FNameFullPath( flist );
        fno = DBSrcFile( fullpath );
        flist = flist->next;
    }
    return( count );
}

static int FCB_Alloc( FILE *fp, char *filename )
{
    int         i;
    FCB         *srcfcb;
    char        *src_buffer;
    FNAMEPTR    flist;

   --IncFileDepth;
    srcfcb = (FCB *) CMemAlloc( sizeof( FCB ) );
    i = PRODUCTION_BUFFER_SIZE;
    src_buffer = FEmalloc( i + 3 );
    if( srcfcb ) {
        srcfcb->src_buf = src_buffer;
        srcfcb->src_ptr = src_buffer;
        src_buffer[0] = '\0';
        if( SrcFile != NULL ) {         // if already have a file open
            SrcFile->src_ptr = ScanCharPtr;     // - save current scan pointer
        }
        ScanCharPtr = src_buffer;               // set scan ptr for new file
        flist = AddFlist( filename );
        srcfcb->src_line = 1;
        SrcFileLineNum = 1;
        srcfcb->src_name = flist->name;
        srcfcb->src_fno  = flist->index;
        TokenFno = flist->index;
        srcfcb->src_flist= flist;
        ErrFName = flist->name;
        srcfcb->src_fp   = fp;
        srcfcb->prev_file = SrcFile;
        srcfcb->src_cnt = 0;
        srcfcb->prev_currchar = CurrChar;
        srcfcb->src_bufsize = i;
        srcfcb->peeking = 0;
        #if _CPU == 370
            srcfcb->colum = 0;     /* init colum, trunc info */
            srcfcb->trunc = 0;
            srcfcb->prevcount = 0;
        #endif
        if( SrcFile != NULL ) {                 /* 28-jan-94 */
            if( SrcFile == MainSrcFile ) {
                // remember name of included file
                AddIncFileList( filename );
            }
        }
        srcfcb->rseekpos = 0;
        SrcFile = srcfcb;
        CurrChar = '\n';    /* set next character to newline */
        if( CompFlags.cpp_output ) {            /* 10-aug-91 */
            if( CppFile == NULL )  OpenCppFile();
            EmitPoundLine( 1, filename, 1 );
            CppFirstChar = 1;
        }
        return( 1 );
    }
    return( 0 );
}

void SetSrcFNameOnce( void )
{
    SrcFile->src_flist->once = TRUE;
}

static void ParseInit()
{
    ScanInit();
    CTypeInit();
    MacroInit();
    SymInit();
    SpcSymInit();
    StringInit();
#ifndef NEWCFE
        QuadInit();
#endif
    InitDataQuads();
    ExprInit();
#ifdef NEWCFE
    StmtInit();
#endif
    SegInit();                                  /* 02-feb-92 */
}


local void Parse()
{
    EmitInit();
    // The first token in a file should be #include if a user wants to
    // use pre-compiled headers. The following call to NextToken() to
    // get the very first token of the file will load the pre-compiled
    // header if the user requested such and it is a #include directive.
    CompFlags.ok_to_use_precompiled_hdr = 1;    /* 27-jun-94 */
    if( ForceInclude ) {                        /* 17-feb-95 */
        if( PCH_FileName != NULL ) {
            CompFlags.use_precompiled_header = 1;
        }
        // we want to keep in the pre-compiled header
        // any macros that are defined in forced include file
        InitialMacroFlag = 0;                   /* 02-jun-95 */
        OpenSrcFile( ForceInclude, 0 );
        CompFlags.use_precompiled_header = 0;
        CompFlags.ok_to_use_precompiled_hdr = 0;
    }
    NextToken();
    // If we didn't get a #include with the above call to NextToken()
    // it's too late to use pre-compiled header now.
    CompFlags.ok_to_use_precompiled_hdr = 0;    /* 27-jun-94 */
    ParsePgm();
    if( DefFile != NULL ) {
        fclose( DefFile );
        DefFile = NULL;
    }
    ChkCallParms();
    EndBlock();     /* end of block 0 */
    MacroFini();
    if( ! CompFlags.quiet_mode ) PrintStats();
    if( CompFlags.warnings_cause_bad_exit )  ErrCount += WngCount;
}

int PrintWhiteSpace;
static void CPP_Parse()
{
    if( ForceInclude ) {
        PrtChar( '\n' );
        OpenSrcFile( ForceInclude, 0 );
    }
    PrintWhiteSpace = TRUE;
    for(;;) {
        GetNextToken();
        if( CurToken == T_EOF ) break;
        PrtToken();
    }
    MacroFini();
}


void EmitPoundLine( unsigned line_num, char *filename, int newline )
{
    if( CompFlags.cpp_line_wanted ) {
        if( CppPrinting() ) {
            CppPrtf( "#line %u \"", line_num );    /* 04-apr-91 */
            while( *filename ) {
                #if 0                                   /* 15-may-94 */
                    if( *filename == '\\' ) {
                        PrtChar( '\\' );
                    }
                #endif
                PrtChar( *filename );
                ++filename;
            }
            PrtChar( '\"' );
            if( newline )  PrtChar( '\n' );
        }
    }
}

void EmitLine( unsigned line_num, char *filename )
{
    EmitPoundLine( line_num, filename, 0 );
}

int CppPrinting()
{
    if( NestLevel != SkipLevel ) return( 0 );   /* 01-dec-89 */
    return( 1 );
}


void PrtToken(void)
{
    if( ! CppPrinting() ) return;
    switch( CurToken ) {
    case T_BAD_CHAR:                    /* 12-apr-89 */
    case T_BAD_TOKEN:                   /* 12-apr-89 */
    case T_ID:
    case T_CONSTANT:
        CppPrtf( "%s", Buffer );
        break;
    case T_STRING:
        if( CompFlags.wide_char_string ) {              /* 18-feb-90 */
            PrtChar( 'L' );
        }
        CppPrtf( "\"%s\"", Buffer );
        break;
    case T_EOF:
    case T_NULL:
        break;
    case T_WHITE_SPACE:
        if( PrintWhiteSpace ) {
            CppPrtf( "%s", Tokens[ CurToken ] );
        } else {
            PrintWhiteSpace = TRUE; //Toggle
        }
        break;
    default:
        CppPrtf( "%s", Tokens[ CurToken ] );
    }
}


void GetNextToken(void)
{
    CurToken = T_NULL;
    if( MacroPtr != NULL ) {
        GetMacroToken();
        if( CurToken == T_NULL ) {                      /* 29-may-95 */
            // prevents macro expansion from merging with trailing text
            // to form new tokens in pre-processed output
            CurToken = T_WHITE_SPACE;
        }
    } else {
        for(;;) {
            if( CurrChar == EOF_CHAR ) break;
            if( (CharSet[ CurrChar ] & C_WS) == 0 ) break;
            if( CurrChar != '\r' ) PrtChar( CurrChar );
            NextChar();
        }
        CurToken = ScanToken();
    }
}

