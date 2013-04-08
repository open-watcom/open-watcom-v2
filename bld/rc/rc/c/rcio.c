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
* Description:  WRC I/O routines.
*
****************************************************************************/


#include <stdio.h>
#include <stdarg.h>
#include <assert.h>
#include <time.h>
#ifndef __UNIX__
    #include <process.h>
#endif
#include "wio.h"
#include "global.h"
#include "rcmem.h"
#include "errors.h"
#include "exeutil.h"
#include "preproc.h"
#include "reserr.h"
#include "tmpctl.h"
#include "autodep.h"
#include "errprt.h"
#include "util.h"
#include "rcldstr.h"
#include "iortns.h"
#include "semantic.h"


#ifdef __UNIX__
#define PATH_SEP '/'
#define PATH_SPLIT ':'
#else
#define PATH_SEP '\\'
#define PATH_SPLIT ';'
#endif

static void MakeTmpInSameDir( const char * dirfile, char * outfile, char * ext )
/******************************************************************************/
{
    char    drive[ _MAX_DRIVE ];
    char    dir[ _MAX_DIR ];
#ifdef __UNIX__
    char    fname[ 32 ];
#else
    char    *fname = "__TMP__";
#endif

    _splitpath( dirfile, drive, dir, NULL, NULL );
#ifdef __UNIX__
    // Must be able to run several "rc" executables simultaneously
    // in the same directory
    sprintf( fname, "__RCTMP%lu__", (unsigned long)getpid() );
#endif
    _makepath( outfile, drive, dir, fname, ext );
} /* MakeTmpInSameDir */

static int Pass1InitRes( void )
/*****************************/
{
    WResID        null_id;
    ResMemFlags   null_memflags;
    ResLocation   null_loc;

    /* put the temporary file in the same location as the output file */
    CurrResFile.filename = CurrResFile.namebuf;
#ifdef USE_TEMPFILE
    MakeTmpInSameDir( CmdLineParms.OutResFileName, CurrResFile.filename,
                    "res" );
#else
    strcpy( CurrResFile.filename, CmdLineParms.OutResFileName );
#endif

    /* initialize the directory */
    CurrResFile.dir = WResInitDir();
    if( CurrResFile.dir == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        CurrResFile.IsOpen = false;
        return( TRUE );
    }

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ) {
        WResSetTargetOS( CurrResFile.dir, WRES_OS_WIN16 );
    } else if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        WResSetTargetOS( CurrResFile.dir, WRES_OS_WIN32 );
    } else {
        WResSetTargetOS( CurrResFile.dir, WRES_OS_OS2 );
    }

    /* open the temporary file */
    if( CmdLineParms.MSResFormat ) {
        CurrResFile.IsWatcomRes = FALSE;
        CurrResFile.handle = MResOpenNewFile( CurrResFile.filename );

    /* write null header here if it is win32 */
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 &&
                                     CmdLineParms.MSResFormat ) {
            null_loc.start = SemStartResource();
            null_loc.len = SemEndResource( null_loc.start );
            null_id.IsName = FALSE;
            null_id.ID.Num = 0;
            null_memflags = 0;
            SemAddResource( &null_id, &null_id, null_memflags, null_loc );
        }
    } else {
        CurrResFile.IsWatcomRes = TRUE;
        CurrResFile.handle = WResOpenNewFile( CurrResFile.filename );
    }
    if( CurrResFile.handle == -1 ) {
        RcError( ERR_OPENING_TMP, CurrResFile.filename, LastWresErrStr() );
        CurrResFile.IsOpen = false;
        return( TRUE );
    }
    RegisterTmpFile( CurrResFile.filename );

    CurrResFile.IsOpen = true;
    CurrResFile.StringTable = NULL;
    CurrResFile.ErrorTable = NULL;
    CurrResFile.FontDir = NULL;
    CurrResFile.NextCurOrIcon = 1;
    return( FALSE );
} /* Pass1InitRes */

extern void RcFindResource( char *name, char *fullpath ) {

    char        *src;
    char        *dst;
    char        end;
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];

    fullpath[0] = '\0';
    //if the filename has a drive or is an absolute path then ignore
    //the include path and just look at the specified location
    _splitpath( name, drive, dir, NULL, NULL );
    if( drive[0] != '\0' || dir[0] ==PATH_SEP ) {
        if( access( name, F_OK ) == 0 ) {
            strcpy( fullpath, name );
        }
        return;
    }
    if( !CmdLineParms.IgnoreCWD && access( name, F_OK ) == 0 ) {
        strcpy( fullpath, name );
        return;
    }
    if( NewIncludeDirs != NULL ) {
        src = NewIncludeDirs;
        end = *NewIncludeDirs;
        while( end != '\0' ) {
            dst = fullpath;
            while( *src != ';' && *src != PATH_SPLIT && *src != '\0' ) {
                *dst = *src;
                dst ++;
                src ++;
            }
            end = *src;
            src ++;
            if( *( dst - 1 ) != PATH_SEP ) {
                *dst = PATH_SEP;
                dst++;
            }
            strcpy( dst, name );
            if( access( fullpath, F_OK ) == 0 ) return;
        }
    }
    fullpath[0] = '\0';
}

extern void RcTmpFileName( char * tmpfilename )
/*********************************************/
/* uses the TMP env. var. if it is set and puts the result into tmpfilename */
/* which is assumed to be a buffer of at least _MAX_PATH characters */
{
    char *  nextchar;
    char *  tmpdir;

    tmpdir = RcGetEnv( "TMP" );
    if( tmpdir != NULL ) {
        /* leave room for the '\' and the filename */
        strncpy( tmpfilename, tmpdir, _MAX_PATH - L_tmpnam - 1 );
        nextchar = tmpfilename + strlen( tmpfilename ) - 1;
        /* tack a '\' onto the end if it is not there already */
        if( *nextchar != PATH_SEP ) {
            nextchar++;
            *nextchar = PATH_SEP;
        }
        nextchar++;
    } else {
        nextchar = tmpfilename;
    }

    tmpnam( nextchar );
}

static int PreprocessInputFile( void )
/************************************/
{
    unsigned    flags;
    char        rcdefine[13];
    char      **cppargs;
    char       *p;
    int         rc;

    // We have already merged INCLUDE path with /i paths
    flags = PPFLAG_IGNORE_INCLUDE | PPFLAG_EMIT_LINE;
    if( CmdLineParms.IgnoreCWD ) {
        flags |= PPFLAG_IGNORE_CWD;
    }
    rc = PP_Init2( CmdLineParms.InFileName, flags, NewIncludeDirs, CharSetLen );
    if( rc != 0 ) {
        RcError( ERR_CANT_OPEN_FILE, CmdLineParms.InFileName, strerror(errno) );
        return( TRUE );
    }
    strcpy( rcdefine, "RC_INVOKED 1" );
    PP_Define( rcdefine );
    if( !CmdLineParms.NoTargetDefine ) {
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ) {
            strcpy( rcdefine, "__WINDOWS__" );
            PP_Define( rcdefine );
        } else if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            strcpy( rcdefine, "__NT__" );
            PP_Define( rcdefine );
        } else if( CmdLineParms.TargetOS == RC_TARGET_OS_OS2 ) {
            strcpy( rcdefine, "__OS2__" );
            PP_Define( rcdefine );
        }
    }
    cppargs = CmdLineParms.CPPArgs;
    if( cppargs != NULL ) {
        ++cppargs;
        for(;;) {
            p = *cppargs;
            if( p == NULL ) break;
            for(;;) {
                if( *p == '\0' ) break;
                if( *p == '=' ) {
                    *p = ' ';
                    break;
                }
                ++p;
            }
            p = *cppargs;
            PP_Define( p + 2 );         // skip over -d
            ++cppargs;
        }
    }
    return( FALSE );                    // indicate no error
}

extern int RcPass1IoInit( void )
/******************************/
/* Open the two files for input and output. The input stream starts at the */
/* top   infilename   and continues as the directives in the file indicate */
/* Returns false if there is a problem opening one of the files. */
{
    int         error;
    char       *includepath = NULL;

    if( !CmdLineParms.IgnoreINCLUDE ) {
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ) {
            includepath = RcGetEnv( "WINDOWS_INCLUDE" );
        } else if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            includepath = RcGetEnv( "NT_INCLUDE" );
        } else if( CmdLineParms.TargetOS == RC_TARGET_OS_OS2 ) {
            includepath = RcGetEnv( "OS2_INCLUDE" );
        }
    }
    if( includepath != NULL ) {
        AddNewIncludeDirs( includepath );
    }
    if( CmdLineParms.NoPreprocess ) {
        RcIoTextInputInit();
        error = RcIoPushInputFile( CmdLineParms.InFileName );
    } else {
        error = PreprocessInputFile();
        if( error ) return( FALSE );
        RcIoTextInputInit();
        error = RcIoPushInputFile( CmdLineParms.InFileName );
    }
    if( error ) return( FALSE );

    if( !CmdLineParms.PreprocessOnly ) {
        error = Pass1InitRes();
    }
    if( error )  {
        PP_Fini();
        RcIoTextInputShutdown();
        return( FALSE );
    }

    return( TRUE );
}

static int ChangeTmpToOutFile( const char * tmpfile, const char * outfile )
/**************************************************************************/
{
    int     fileerror;      /* error while deleting or renaming */

    /* remove the old copy of the output file */
    fileerror = remove( outfile );
    if( fileerror ) {
        if( errno == ENOENT ) {
            /* ignore the error if it says that the file doesn't exist */
            errno = 0;
        } else {
            RcError( ERR_RENAMEING_TMP_FILE, tmpfile, outfile,
                        strerror( errno ) );
            remove( tmpfile );
            UnregisterTmpFile( tmpfile );
            return( TRUE );
        }
    }
    /* rename the temp file to the output file */
    fileerror = rename( tmpfile, outfile );
    if( fileerror ) {
        RcError( ERR_RENAMEING_TMP_FILE, tmpfile, outfile,
                        strerror( errno ) );
        remove( tmpfile );
        UnregisterTmpFile( tmpfile );
        return( TRUE );
    }

    return( FALSE );
} /* ChangeTmpToOutFile */

static int RemoveCurrResFile( void )
/**********************************/
{
    int     fileerror;

    fileerror = remove( CurrResFile.filename );
    UnregisterTmpFile( CurrResFile.filename );
    if( fileerror ) {
        return( TRUE );
    } else {
        return( FALSE );
    }
}

static void WriteTables( void )
/*****************************/
{
    if( CurrResFile.StringTable != NULL ) {
        SemWriteStringTable( CurrResFile.StringTable,
                    WResIDFromNum( (long)RT_STRING ) );
    }
    if( CurrResFile.ErrorTable != NULL ) {
        SemWriteStringTable( CurrResFile.ErrorTable,
                    WResIDFromNum( (long)RT_ERRTABLE ) );
    }
    if( CurrResFile.FontDir != NULL ) {
        SemWriteFontDir();
    }
}

static void WriteOS2Tables( void )
/********************************/
{
    if( CurrResFile.StringTable != NULL ) {
        SemOS2WriteStringTable( CurrResFile.StringTable,
                    WResIDFromNum( OS2_RT_STRING ) );
    }
    if( CurrResFile.ErrorTable != NULL ) {
        SemOS2WriteStringTable( CurrResFile.ErrorTable,
                    WResIDFromNum( OS2_RT_MESSAGE ) );
    }
    if( CurrResFile.FontDir != NULL ) {
        SemOS2WriteFontDir();
    }
}

static void Pass1ResFileShutdown( void )
/**************************************/
{
    int         error;

    error = FALSE;
    if( CurrResFile.IsOpen ) {
        if( CmdLineParms.TargetOS == RC_TARGET_OS_OS2 )
            WriteOS2Tables();
        else
            WriteTables();
        if( ErrorHasOccured ) {
            ResCloseFile( CurrResFile.handle );
            CurrResFile.IsOpen = false;
            RemoveCurrResFile();
        } else {
            if (CurrResFile.IsWatcomRes) {
                error = WResWriteDir( CurrResFile.handle, CurrResFile.dir );
                if( error ) {
                    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename,
                                LastWresErrStr() );
                }
            }
            if( ResCloseFile( CurrResFile.handle ) == -1 ) {
                RcError( ERR_CLOSING_TMP, CurrResFile.filename,
                         LastWresErrStr() );
                remove( CurrResFile.filename );
                UnregisterTmpFile( CurrResFile.filename );
            } else if( !error ) {
#ifdef USE_TEMPFILE
                ChangeTmpToOutFile( CurrResFile.filename,
                                CmdLineParms.OutResFileName );
#endif
            }
            CurrResFile.IsOpen = false;
        }
        WResFreeDir( CurrResFile.dir );
        CurrResFile.dir = NULL;
    }
} /* Pass1ResFileShutdown */

extern void RcPass1IoShutdown( void )
/***********************************/
{
    RcIoTextInputShutdown();
    if( !CmdLineParms.PreprocessOnly ) {
        Pass1ResFileShutdown();
    }
} /* RcPass1IoShutdown */

static int OpenResFileInfo( ExeType type )
/****************************************/
{
    int             rc;
    ExtraRes        *curfile;


    if( (type == EXE_TYPE_NE_WIN || type == EXE_TYPE_NE_OS2)
        && CmdLineParms.ExtraResFiles != NULL ) {
        RcError( ERR_FR_NOT_VALID_FOR_WIN );
        return( FALSE );
    }
    Pass2Info.AllResFilesOpen = TRUE;
    if( CmdLineParms.NoResFile ) {
        Pass2Info.ResFiles = RcMemMalloc( sizeof( ResFileInfo ) );
        Pass2Info.ResFiles->name = NULL;
        Pass2Info.ResFiles->IsOpen = FALSE;
        Pass2Info.ResFiles->Handle = -1;
        Pass2Info.ResFiles->Dir = NULL;
        return( TRUE );
    }

    curfile = RcMemMalloc( sizeof( ExtraRes ) );
    curfile->next = CmdLineParms.ExtraResFiles;
    CmdLineParms.ExtraResFiles = curfile;

    if( CmdLineParms.Pass2Only ) {
        strcpy( curfile->name, CmdLineParms.InFileName );
    } else {
        strcpy( curfile->name, CmdLineParms.OutResFileName );
    }

    rc = OpenResFiles( CmdLineParms.ExtraResFiles, &Pass2Info.ResFiles,
                  &Pass2Info.AllResFilesOpen, type,
                  CmdLineParms.InExeFileName );

    return( rc );

} /* OpenResFileInfo */


static int openExeFileInfoRO( char *filename, ExeFileInfo *info )
/***************************************************************/
{
    RcStatus        status;
    exe_pe_header   *pehdr;

    info->Handle = RcOpen( filename, O_RDONLY|O_BINARY );
    if( info->Handle == -1 ) {
        RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
        return( FALSE );
    }
    info->IsOpen = TRUE;
    info->Type = FindNEPELXHeader( info->Handle, &info->WinHeadOffset );
    info->name = filename;
    switch( info->Type ) {
    case EXE_TYPE_NE_WIN:
    case EXE_TYPE_NE_OS2:
        status = SeekRead( info->Handle, info->WinHeadOffset, &info->u.NEInfo.WinHead, sizeof( os2_exe_header ) );
        if( status != RS_OK ) {
            RcError( ERR_NOT_VALID_EXE, filename );
            return( FALSE );
        } else {
            info->DebugOffset = info->WinHeadOffset + sizeof( os2_exe_header );
        }
        break;
    case EXE_TYPE_PE:
        pehdr = &info->u.PEInfo.WinHeadData;
        info->u.PEInfo.WinHead = pehdr;
        status = SeekRead( info->Handle, info->WinHeadOffset, &PE32( *pehdr ), sizeof( pe_header ) );
        if( status != RS_OK ) {
            RcError( ERR_NOT_VALID_EXE, filename );
            return( FALSE );
        }
        if( IS_PE64( *pehdr ) ) {
            status = SeekRead( info->Handle, info->WinHeadOffset, &PE64( *pehdr ), sizeof( pe_header64 ) );
            if( status != RS_OK ) {
                RcError( ERR_NOT_VALID_EXE, filename );
                return( FALSE );
            }
            info->DebugOffset = info->WinHeadOffset + sizeof( pe_header64 );
        } else {
            info->DebugOffset = info->WinHeadOffset + sizeof( pe_header );
        }
        break;
    case EXE_TYPE_LX:
        status = SeekRead( info->Handle, info->WinHeadOffset, &info->u.LXInfo.OS2Head, sizeof( os2_flat_header ) );
        if( status != RS_OK ) {
            RcError( ERR_NOT_VALID_EXE, filename );
            return( FALSE );
        } else {
            info->DebugOffset = info->WinHeadOffset + sizeof( os2_flat_header );
        }
        break;
    default:
        RcError( ERR_NOT_VALID_EXE, filename );
        return( FALSE );
        break;
    }

    RcSeek( info->Handle, 0, SEEK_SET );
    return( TRUE );
} /* openExeFileInfoRO */

static int openNewExeFileInfo( char *filename, ExeFileInfo *info )
/******************************************************************/
{
    info->Handle = RcOpen( filename, O_RDWR|O_CREAT|O_TRUNC|O_BINARY, PMODE_RW );
    if( info->Handle == -1 ) {
        RcError( ERR_OPENING_TMP, filename, strerror( errno ) );
        return( FALSE );
    }
    RegisterTmpFile( filename );
    info->IsOpen = TRUE;
    info->DebugOffset = 0;
    info->name = filename;

    return( TRUE );
} /* openNewExeFileInfo */

static void FreeNEFileInfoPtrs( NEExeInfo * info )
/*************************************************/
{
    if( info->Seg.Segments != NULL ) {
        RcMemFree( info->Seg.Segments );
        info->Seg.Segments = NULL;
    }
    if( info->Res.Str.StringBlock != NULL ) {
        RcMemFree( info->Res.Str.StringBlock );
        info->Res.Str.StringBlock = NULL;
    }
    if( info->Res.Str.StringList != NULL ) {
        RcMemFree( info->Res.Str.StringList );
        info->Res.Str.StringList = NULL;
    }
} /* FreeNEFileInfoPtrs */

static void FreePEFileInfoPtrs( PEExeInfo * info )
/************************************************/
{
    if( info->Objects != NULL ) {
        RcMemFree( info->Objects );
    }
}

static void FreeLXFileInfoPtrs( LXExeInfo *info )
/***********************************************/
{
    if( info->Objects != NULL ) {
        RcMemFree( info->Objects );
    }
    if( info->Pages != NULL ) {
        RcMemFree( info->Pages );
    }
    if( info->Res.resources != NULL ) {
        RcMemFree( info->Res.resources );
    }
}

extern void ClosePass2FilesAndFreeMem( void )
/*******************************************/
{
    ExeFileInfo         *tmp;
    ExeFileInfo         *old;
    char                *tmpfilename;

    tmp = &(Pass2Info.TmpFile);
    old = &(Pass2Info.OldFile);
    tmpfilename = Pass2Info.TmpFileName;

    if( old->IsOpen ) {
        RcClose( old->Handle );
        old->IsOpen = FALSE;
    }
    switch( old->Type ) {
    case EXE_TYPE_NE_WIN:
    case EXE_TYPE_NE_OS2:
        FreeNEFileInfoPtrs( &old->u.NEInfo );
        break;
    case EXE_TYPE_PE:
        FreePEFileInfoPtrs( &old->u.PEInfo );
        break;
    case EXE_TYPE_LX:
        FreeLXFileInfoPtrs( &old->u.LXInfo );
        break;
    default: //EXE_TYPE_UNKNOWN
        break;
    }

    if( tmp->IsOpen ) {
        RcClose( tmp->Handle );
        tmp->IsOpen = FALSE;
    }
    switch( tmp->Type ) {
    case EXE_TYPE_NE_WIN:
    case EXE_TYPE_NE_OS2:
        FreeNEFileInfoPtrs( &tmp->u.NEInfo );
        break;
    case EXE_TYPE_PE:
        FreePEFileInfoPtrs( &tmp->u.PEInfo );
        break;
    case EXE_TYPE_LX:
        FreeLXFileInfoPtrs( &tmp->u.LXInfo );
        break;
    default: //EXE_TYPE_UNKNOWN
        break;
    }
    CloseResFiles( Pass2Info.ResFiles );
} /* ClosePass2FilesAndFreeMem */

extern int RcPass2IoInit( void )
/******************************/
{
    int     noerror;
    int     tmpexe_exists;

    memset( &Pass2Info, '\0', sizeof( RcPass2Info ) );
    Pass2Info.IoBuffer = RcMemMalloc( IO_BUFFER_SIZE );
    MakeTmpInSameDir( CmdLineParms.OutExeFileName, Pass2Info.TmpFileName,
                            "tmp" );
    noerror = openExeFileInfoRO( CmdLineParms.InExeFileName,
                    &(Pass2Info.OldFile) );
    if( noerror ) {
        noerror = openNewExeFileInfo( Pass2Info.TmpFileName,
                                      &(Pass2Info.TmpFile) );
    }
    tmpexe_exists = noerror;

    if( noerror ) {
        Pass2Info.TmpFile.Type = Pass2Info.OldFile.Type;
        Pass2Info.TmpFile.WinHeadOffset = Pass2Info.OldFile.WinHeadOffset;
        if( Pass2Info.OldFile.Type == EXE_TYPE_PE ) {
            Pass2Info.TmpFile.u.PEInfo.WinHead = &Pass2Info.TmpFile.u.PEInfo.WinHeadData;
            *Pass2Info.TmpFile.u.PEInfo.WinHead = *Pass2Info.OldFile.u.PEInfo.WinHead;
        }
        if( (Pass2Info.OldFile.Type == EXE_TYPE_NE_WIN || Pass2Info.OldFile.Type == EXE_TYPE_NE_OS2)
            && CmdLineParms.ExtraResFiles != NULL ) {
            RcError( ERR_FR_NOT_VALID_FOR_WIN );
            noerror = FALSE;
        } else {
            noerror = OpenResFileInfo( Pass2Info.OldFile.Type );
        }
    }

    if( !noerror ) {
        RcMemFree( Pass2Info.IoBuffer );
        Pass2Info.IoBuffer = NULL;
        ClosePass2FilesAndFreeMem();
        if( tmpexe_exists ) {
            remove( Pass2Info.TmpFileName );
            UnregisterTmpFile( Pass2Info.TmpFileName );
        }
    }

    return( noerror );
} /* RcPass2IoInit */

extern void RcPass2IoShutdown( int noerror )
/******************************************/
{
    ClosePass2FilesAndFreeMem();
    if( Pass2Info.IoBuffer != NULL ) {
        RcMemFree( Pass2Info.IoBuffer );
        Pass2Info.IoBuffer = NULL;
    }
    if( noerror ) {
        ChangeTmpToOutFile( Pass2Info.TmpFileName,
                            CmdLineParms.OutExeFileName);
    } else {
        UnregisterTmpFile( Pass2Info.TmpFileName );
        remove( Pass2Info.TmpFileName );
    }
} /* RcPass2IoShutdown */

/****** Text file input routines ******/
/* These routines maintain a stack of input files. Pushing a file onto the */
/* stack opens the file and sets up the buffer. Poping a file closes the file */
/* All other routines (except TextInputInit and TextInputShutdown) operate */
/* on the current file which is the one at the top of the stack */

#define MAX_INCLUDE_DEPTH   16

typedef struct PhysFileInfo {
    char    Filename[ _MAX_PATH ];
    int     IsOpen;
    int     Handle;
    long    Offset;     /* offset in file to read from next time if this */
                        /* is not the current file */
} PhysFileInfo;

typedef struct FileStackEntry {
    LogicalFileInfo     Logical;
    PhysFileInfo        Physical;
} FileStackEntry;

typedef struct FileStack {
    unsigned char       *Buffer;
    uint                BufferSize;
    unsigned char       *NextChar;
    unsigned char       *EofChar;       /* DON'T dereference, see below */
    /* + 1 for the before first entry */
    FileStackEntry      Stack[ MAX_INCLUDE_DEPTH + 1 ];
    FileStackEntry      *Current;
} FileStack;
/* EofChar points to the memory location after the last character currently */
/* in the buffer. If the physical EOF has been reached it will point to */
/* within Buffer, otherwise it will point AFTER Buffer. If NextChar == */
/* EofChar then either EOF has been reached or it's time to read in more */
/* of the file */
/* NB: Characters in the buffer must be unsigned for proper MBCS support! */

#define IsEmptyFileStack( stack ) ((stack).Current == (stack).Stack)

FileStack InStack;

extern void RcIoTextInputInit( void )
/***********************************/
{
    InStack.Buffer = RcMemMalloc( IO_BUFFER_SIZE );
    InStack.BufferSize = IO_BUFFER_SIZE;
    InStack.Current = InStack.Stack;
} /* RcIoTextInputInit */

extern int RcIoTextInputShutdown( void )
/**************************************/
{
    if( InStack.Buffer != NULL ) {
        RcMemFree( InStack.Buffer );
        InStack.Buffer = NULL;
        InStack.BufferSize = 0;
        if( IsEmptyFileStack( InStack ) ) {
            return( FALSE );
        } else {
            while( !IsEmptyFileStack( InStack ) ) {
                RcIoPopInputFile();
            }
            // return( TRUE );
        }
    }
    return( TRUE );
} /* RcIoTextInputShutdown */

static int OpenPhysicalFile( PhysFileInfo * phys )
/************************************************/
{
    if( !phys->IsOpen ) {
        phys->Handle = RcIoOpenInput( phys->Filename, O_RDONLY | O_TEXT );
        if( phys->Handle == -1 ) {
            RcError( ERR_CANT_OPEN_FILE, phys->Filename, strerror( errno ) );
            return( TRUE );
        }
        phys->IsOpen = TRUE;
        if( RcSeek( phys->Handle, phys->Offset, SEEK_SET ) == -1 ) {
            RcError( ERR_READING_FILE, phys->Filename, strerror( errno ) );
            return( TRUE );
        }
    }

    return( FALSE );
} /* OpenPhysicalFile */

static int OpenNewPhysicalFile( PhysFileInfo * phys, char * filename )
/********************************************************************/
{
    strncpy( phys->Filename, filename, _MAX_PATH );
    phys->IsOpen = FALSE;
    phys->Offset = 0;

    return( OpenPhysicalFile( phys ) );
} /* OpenNewPhysicalFile */

static void SetPhysFileOffset( FileStack * stack )
/************************************************/
{
    PhysFileInfo *  phys;
    uint_16         charsinbuff;

    phys = &(stack->Current->Physical);

    charsinbuff = stack->BufferSize - (stack->NextChar - stack->Buffer);
    phys->Offset = RcTell( phys->Handle ) - charsinbuff;
} /* SetPhysFileOffset */

static int ReadBuffer( FileStack * stack )
/****************************************/
{
    PhysFileInfo    *phys;
    int             numread;
    int             error;
    int             inchar;

    phys = &(stack->Current->Physical);
    if( !phys->IsOpen ) {
        error = OpenPhysicalFile( phys );
        if( error ) return( TRUE );
    }
    if( CmdLineParms.NoPreprocess ) {
        numread = RcRead( phys->Handle, stack->Buffer, stack->BufferSize );
    } else {
        for( numread = 0; numread < stack->BufferSize; numread++ ) {
            inchar = PP_Char();
            if( inchar == EOF ) {
                break;
            }
            *( stack->Buffer + numread ) = (char)inchar;
        }
    }
    stack->NextChar = stack->Buffer;
    stack->EofChar = stack->Buffer + numread;   /* may be past end of buffer */

    return( FALSE );
} /* ReadBuffer */

extern int RcIoPushInputFile( char * filename )
/*********************************************/
{
    int                 error;

    if( InStack.Current == InStack.Stack + MAX_INCLUDE_DEPTH - 1 ) {
        RcError( ERR_RCINCLUDE_TOO_DEEP, MAX_INCLUDE_DEPTH );
        return( TRUE );
    }

    SetPhysFileOffset( &(InStack) );

    InStack.Current++;

    /* set up the logical file info */
    RcIoSetLogicalFileInfo( 1, filename );

    /* set up the physical file info */
    error = OpenNewPhysicalFile( &(InStack.Current->Physical), filename );
    if( error ) {
        InStack.Current--;
    } else {
        error = ReadBuffer( &(InStack) );
    }

    return( error );
} /* RcIoPushInputFile */

static void ClosePhysicalFile( PhysFileInfo * phys )
/**************************************************/
{
    if( phys->IsOpen ) {
        RcClose( phys->Handle );
        phys->IsOpen = FALSE;
    }
} /* ClosePhysicalFile */

extern int RcIoPopInputFile( void )
/*********************************/
{
    PhysFileInfo *  phys;

    phys = &(InStack.Current->Physical);
    ClosePhysicalFile( phys );
    InStack.Current--;
    if( IsEmptyFileStack( InStack ) ) {
        return( TRUE );
    } else {
        ReadBuffer( &(InStack) );
        return( FALSE );
    }
} /* RcIoPopInputFile */

static int GetLogChar( FileStack * stack )
/****************************************/
{
    int     newchar;

    newchar = *(stack->NextChar);
    assert( newchar > 0 );
    if( newchar == '\n' ) {
        stack->Current->Logical.LineNum++;
    }

    stack->NextChar++;
    return( newchar );
} /* GetLogChar */

extern int RcIoGetChar( void )
/****************************/
{
    int     isempty;
    int     error;

    if( IsEmptyFileStack( InStack ) ) {
        return( RC_EOF );
    }

    if( InStack.NextChar >= InStack.EofChar ) {
        /* if this is a real EOF */
        if( InStack.NextChar < InStack.Buffer + InStack.BufferSize ) {
            /* unstack one file */
            isempty = RcIoPopInputFile();
            if( isempty ) {
                return( RC_EOF );
            } else {
                /* if we are still at the EOF char, there has been an error */
                if( InStack.NextChar >= InStack.EofChar ) {
                    /* this error is reported in ReadBuffer so just terminate */
                    RcFatalError( ERR_NO_MSG );
                } else {
                    /* return \n which will end the current token properly */
                    /* if it it is not a string and end it with a runaway */
                    /* string error for strings */
                    return( '\n' );
                }
            }
        } else {
            /* otherwise we have reached the end of the buffer */
            error = ReadBuffer( &(InStack) );
            if( error ) {
                /* this error is reported in ReadBuffer so just terminate */
                RcFatalError( ERR_NO_MSG );
            }
        }
    }
    return( GetLogChar( &InStack ) );
} /* RcIoGetChar */

extern const LogicalFileInfo * RcIoGetLogicalFileInfo( void )
/***********************************************************/
{
    if( IsEmptyFileStack( InStack ) ) {
        return( NULL );
    } else {
        return( &(InStack.Current->Logical) );
    }
} /* RcIoGetLogicalFileInfo */

extern void RcIoOverrideIsCOrHFlag( void )
/****************************************/
{
    LogicalFileInfo *   log;

    if( !IsEmptyFileStack( InStack ) ) {
        log = &(InStack.Current->Logical);
        log->IsCOrHFile = FALSE;
    }
} /* RcIoOverrideIsCOrHFlag */

extern void RcIoSetIsCOrHFlag( void )
/***********************************/
{
    LogicalFileInfo *   log;
    char                ext[ _MAX_EXT ];

    if( !IsEmptyFileStack( InStack ) ) {
        log = &(InStack.Current->Logical);
        _splitpath( log->Filename, NULL, NULL, NULL, ext );
        /* if this is a c or h file ext will be '.', '[ch]', '\0' */
        if( (ext[1] == 'c' || ext[1] == 'h' || ext[1] == 'C'
            || ext[1] == 'H' ) && ext[2] == '\0' ) {
            /* if the logical file is a c or h file */
            log->IsCOrHFile = TRUE;
        } else {
            log->IsCOrHFile = FALSE;
        }
    }
} /* RcIoSetIsCOrHFlag */

extern void RcIoSetLogicalFileInfo( int linenum, char * filename )
/****************************************************************/
{
    LogicalFileInfo *   log;

    if( !IsEmptyFileStack( InStack ) ) {
        log = &(InStack.Current->Logical);
        log->LineNum = linenum;
        if( filename != NULL ) {
            strncpy( log->Filename, filename, _MAX_PATH );
            RcIoSetIsCOrHFlag();
        }
    }
} /* RcIoSetLogicalFileInfo */

extern int RcIoIsCOrHFile( void )
/*******************************/
/* returns true if the current file is a .c or .h file, false otherwise */
{
    if( IsEmptyFileStack( InStack ) ) {
        return( FALSE );
    } else {
        return( InStack.Current->Logical.IsCOrHFile );
    }
} /* RcIoIsCOrHFile */

/*
 * RcIoOpenInput
 * NB when an error occurs this function MUST return without altering errno
 */
extern int RcIoOpenInput( char * filename, int flags, ... )
/********************************************************/
{
    int                 handle;
    int                 perms;
    va_list             args;
    FileStackEntry      *currfile;

    if( flags & O_CREAT ) {
        va_start( args, flags );
        perms = va_arg( args, int );
        va_end( args );
    } else {
        perms = 0;
    }

    handle = RcOpen( filename, flags, perms );

    if( handle == -1 && errno == EMFILE ) {
        /* set currfile to be the first (not before first) entry */
        currfile = InStack.Stack + 1;
        /* close open files except the current input file until able to open */
        /* don't close the current file because Offset isn't set */
        while( currfile < InStack.Current && handle == -1 && errno == EMFILE ) {
            if( currfile->Physical.IsOpen ) {
                ClosePhysicalFile( &(currfile->Physical) );
                handle = RcOpen( filename, flags, perms );
            }
            currfile++;
       }
    }
    return( handle );

} /* RcIoOpenInput */

extern void RcIoInitStatics( void )
/*********************************/
{
    memset( &InStack, 0, sizeof( FileStack ) );
}
