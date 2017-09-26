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
#include "rcerrors.h"
#include "preproc.h"
#include "reserr.h"
#include "tmpctl.h"
#include "autodep.h"
#include "errprt.h"
#include "util.h"
#include "rcldstr.h"
#include "semantic.h"
#include "wresdefn.h"
#include "iopath.h"
#include "pathlist.h"
#include "rcrtns.h"
#include "rccore.h"
#include "exeutil.h"

#include "clibext.h"


char *RcMakeTmpInSameDir( const char *dirfile, char id, const char *ext )
/***********************************************************************/
{
    char    drive[_MAX_DRIVE];
    char    dir[_MAX_DIR];
    char    *out;
    char    fname[32];
#if defined( __DOS__ ) || defined( __OSI__ )
    sprintf( fname, "__TMP%c__", id );
#else
    // Must be able to run several "rc" executables simultaneously
    // in the same directory
    sprintf( fname, "__RCTMP%c%lu__", id, (unsigned long)getpid() );
#endif
    out = RESALLOC( strlen( dirfile ) + 1 + strlen( fname ) + strlen( ext ) + 1 );
    _splitpath( dirfile, drive, dir, NULL, NULL );
    _makepath( out, drive, dir, fname, ext );
    return( out );
} /* RcMakeTmpInSameDir */

static bool Pass1InitRes( void )
/******************************/
{
    WResID        null_id;
    ResMemFlags   null_memflags;
    ResLocation   null_loc;

    /* put the temporary file in the same location as the output file */
    CurrResFile.filename = RcMakeTmpInSameDir( CmdLineParms.OutResFileName, '0', "res" );

    /* initialize the directory */
    CurrResFile.dir = WResInitDir();
    if( CurrResFile.dir == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        CurrResFile.IsOpen = false;
        return( true );
    }

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ) {
        WResSetTargetOS( CurrResFile.dir, WRES_OS_WIN16 );
    } else if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        WResSetTargetOS( CurrResFile.dir, WRES_OS_WIN32 );
    } else {
        WResSetTargetOS( CurrResFile.dir, WRES_OS_OS2 );
    }

    /* open the temporary file */
    CurrResFile.fid = ResOpenNewFile( CurrResFile.filename );
    if( CurrResFile.fid == WRES_NIL_HANDLE ) {
        RcError( ERR_OPENING_TMP, CurrResFile.filename, LastWresErrStr() );
        CurrResFile.IsOpen = false;
        return( true );
    }
    if( CmdLineParms.MSResFormat ) {
        CurrResFile.IsWatcomRes = false;
        /* write null header here if it is win32 */
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            null_loc.start = SemStartResource();
            null_loc.len = SemEndResource( null_loc.start );
            null_id.IsName = false;
            null_id.ID.Num = 0;
            null_memflags = 0;
            SemAddResource( &null_id, &null_id, null_memflags, null_loc );
        }
    } else {
        CurrResFile.IsWatcomRes = true;
        WResFileInit( CurrResFile.fid );
    }
    RegisterTmpFile( CurrResFile.filename );

    CurrResFile.IsOpen = true;
    CurrResFile.StringTable = NULL;
    CurrResFile.ErrorTable = NULL;
    CurrResFile.FontDir = NULL;
    CurrResFile.NextCurOrIcon = 1;
    return( false );
} /* Pass1InitRes */

int RcFindResource( const char *name, char *fullpath )
/****************************************************/
{
    return( PP_IncludePathFind( name, strlen( name ), fullpath, PPINCLUDE_SRC ) );
}

static bool PreprocessInputFile( void )
/*************************************/
{
    unsigned    flags;
    char        rcdefine[13];
    char      **cppargs;
    char       *p;
    int         rc;

    flags = PPFLAG_EMIT_LINE | PPFLAG_IGNORE_INCLUDE;
    if( CmdLineParms.IgnoreCWD ) {
        flags |= PPFLAG_IGNORE_CWD;
    }
    rc = PP_FileInit2( CmdLineParms.InFileName, flags, NULL, CharSetLen );
    if( rc != 0 ) {
        RcError( ERR_CANT_OPEN_FILE, CmdLineParms.InFileName, strerror(errno) );
        return( true );
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
        for( ++cppargs; (p = *cppargs) != NULL; ++cppargs ) {
            for( ; *p != '\0'; ++p ) {
                if( *p == '=' ) {
                    *p = ' ';
                    break;
                }
            }
            p = *cppargs;
            PP_Define( p + 2 );         // skip over -d
            RESFREE( p );
        }
    }
    return( false );                    // indicate no error
}

extern bool RcPass1IoInit( void )
/*******************************/
/* Open the two files for input and output. The input stream starts at the */
/* top   infilename   and continues as the directives in the file indicate */
/* Returns false if there is a problem opening one of the files. */
{
    bool        error;
    const char  *includepath = NULL;

    if( !CmdLineParms.IgnoreINCLUDE ) {
        if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ) {
            includepath = RcGetEnv( "WINDOWS_INCLUDE" );
        } else if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
            includepath = RcGetEnv( "NT_INCLUDE" );
        } else if( CmdLineParms.TargetOS == RC_TARGET_OS_OS2 ) {
            includepath = RcGetEnv( "OS2_INCLUDE" );
        }
        if( includepath != NULL ) {
            PP_IncludePathAdd( includepath );
        }
        includepath = RcGetEnv( "INCLUDE" );
        if( includepath != NULL ) {
            PP_IncludePathAdd( includepath );
        }
    }
    if( !CmdLineParms.NoPreprocess ) {
        if( PreprocessInputFile() ) {
            return( false );
        }
    }
    RcIoTextInputInit();
    error = RcIoPushTextInputFile( CmdLineParms.InFileName );
    if( error )
        return( false );

    if( !CmdLineParms.PreprocessOnly ) {
        error = Pass1InitRes();
    }
    if( error )  {
        PP_FileFini();
        RcIoTextInputShutdown();
        return( false );
    }

    return( true );
}

static bool ChangeTmpToOutFile( const char *tmpfile, const char *outfile )
/************************************************************************/
{
    int     fileerror;      /* error while deleting or renaming */
    bool    rc;

    /* remove the old copy of the output file */
    fileerror = remove( outfile );
    if( fileerror ) {
        if( errno == ENOENT ) {
            /* ignore the error if it says that the file doesn't exist */
            errno = 0;
        } else {
            RcError( ERR_DELETING_FILE, outfile, strerror( errno ) );
            remove( tmpfile );
            UnregisterTmpFile( tmpfile );
            return( true );
        }
    }
    rc = false;
    /* rename the temp file to the output file */
    fileerror = rename( tmpfile, outfile );
    if( fileerror ) {
        RcError( ERR_RENAMING_TMP_FILE, tmpfile, outfile, strerror( errno ) );
        remove( tmpfile );
        rc = true;
    }
    UnregisterTmpFile( tmpfile );
    return( rc );
} /* ChangeTmpToOutFile */

static bool RemoveCurrResFile( void )
/**********************************/
{
    int     fileerror;

    fileerror = remove( CurrResFile.filename );
    UnregisterTmpFile( CurrResFile.filename );
    if( fileerror ) {
        return( true );
    } else {
        return( false );
    }
}

static void WriteWINTables( void )
/********************************/
{
    if( CurrResFile.StringTable != NULL ) {
        SemWINWriteStringTable( CurrResFile.StringTable,
                    WResIDFromNum( RESOURCE2INT( RT_STRING ) ) );
    }
    if( CurrResFile.ErrorTable != NULL ) {
        SemWINWriteStringTable( CurrResFile.ErrorTable,
                    WResIDFromNum( RESOURCE2INT( RT_ERRTABLE ) ) );
    }
    if( CurrResFile.FontDir != NULL ) {
        SemWINWriteFontDir();
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
    bool        error;

    error = false;
    if( CurrResFile.IsOpen ) {
        if( CmdLineParms.TargetOS == RC_TARGET_OS_OS2 ) {
            WriteOS2Tables();
        } else {
            WriteWINTables();
        }
        if( ErrorHasOccured ) {
            ResCloseFile( CurrResFile.fid );
            CurrResFile.IsOpen = false;
            RemoveCurrResFile();
        } else {
            if( CurrResFile.IsWatcomRes ) {
                error = WResWriteDir( CurrResFile.fid, CurrResFile.dir );
                if( error ) {
                    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, LastWresErrStr() );
                }
            }
            if( ResCloseFile( CurrResFile.fid ) ) {
                RcError( ERR_CLOSING_TMP, CurrResFile.filename, LastWresErrStr() );
                remove( CurrResFile.filename );
                UnregisterTmpFile( CurrResFile.filename );
            } else if( !error ) {
                ChangeTmpToOutFile( CurrResFile.filename, CmdLineParms.OutResFileName );
            }
            CurrResFile.IsOpen = false;
        }
        WResFreeDir( CurrResFile.dir );
        CurrResFile.dir = NULL;
        RESFREE( CurrResFile.filename );
        CurrResFile.filename = NULL;
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

static bool OpenResFileInfo( ExeType type )
/*****************************************/
{
    bool            error;
    ExtraRes        *curfile;
    char            *name;


    if( ( type == EXE_TYPE_NE_WIN || type == EXE_TYPE_NE_OS2 )
        && CmdLineParms.ExtraResFiles != NULL ) {
        RcError( ERR_FR_NOT_VALID_FOR_WIN );
        return( false );
    }
    Pass2Info.AllResFilesOpen = true;
    if( CmdLineParms.NoResFile ) {
        Pass2Info.ResFile = RESALLOC( sizeof( ResFileInfo ) );
        Pass2Info.ResFile->next = NULL;
        Pass2Info.ResFile->name = NULL;
        Pass2Info.ResFile->IsOpen = false;
        Pass2Info.ResFile->fid = WRES_NIL_HANDLE;
        Pass2Info.ResFile->Dir = NULL;
        return( true );
    }

    if( CmdLineParms.Pass2Only ) {
        name = CmdLineParms.InFileName;
    } else {
        name = CmdLineParms.OutResFileName;
    }
    curfile = RESALLOC( sizeof( ExtraRes ) + strlen( name ) );
    curfile->next = CmdLineParms.ExtraResFiles;
    CmdLineParms.ExtraResFiles = curfile;
    strcpy( curfile->name, name );

    error = OpenResFiles( CmdLineParms.ExtraResFiles, &Pass2Info.ResFile,
                  &Pass2Info.AllResFilesOpen, type,
                  CmdLineParms.InExeFileName );

    return( error );

} /* OpenResFileInfo */


static bool openExeFileInfoRO( const char *filename, ExeFileInfo *info )
/**********************************************************************/
{
    RcStatus        status;
    exe_pe_header   *pehdr;

    info->fid = ResOpenFileRO( filename );
    if( info->fid == WRES_NIL_HANDLE ) {
        RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
        return( false );
    }
    info->IsOpen = true;
    info->Type = FindNEPELXHeader( info->fid, &info->WinHeadOffset );
    info->name = filename;
    switch( info->Type ) {
    case EXE_TYPE_NE_WIN:
    case EXE_TYPE_NE_OS2:
        status = SeekRead( info->fid, info->WinHeadOffset, &info->u.NEInfo.WinHead, sizeof( os2_exe_header ) );
        if( status != RS_OK ) {
            RcError( ERR_NOT_VALID_EXE, filename );
            return( false );
        } else {
            info->DebugOffset = info->WinHeadOffset + sizeof( os2_exe_header );
        }
        break;
    case EXE_TYPE_PE:
        pehdr = &info->u.PEInfo.WinHeadData;
        info->u.PEInfo.WinHead = pehdr;
        status = SeekRead( info->fid, info->WinHeadOffset, &PE32( *pehdr ), sizeof( pe_header ) );
        if( status != RS_OK ) {
            RcError( ERR_NOT_VALID_EXE, filename );
            return( false );
        }
        if( IS_PE64( *pehdr ) ) {
            status = SeekRead( info->fid, info->WinHeadOffset, &PE64( *pehdr ), sizeof( pe_header64 ) );
            if( status != RS_OK ) {
                RcError( ERR_NOT_VALID_EXE, filename );
                return( false );
            }
            info->DebugOffset = info->WinHeadOffset + sizeof( pe_header64 );
        } else {
            info->DebugOffset = info->WinHeadOffset + sizeof( pe_header );
        }
        break;
    case EXE_TYPE_LX:
        status = SeekRead( info->fid, info->WinHeadOffset, &info->u.LXInfo.OS2Head, sizeof( os2_flat_header ) );
        if( status != RS_OK ) {
            RcError( ERR_NOT_VALID_EXE, filename );
            return( false );
        } else {
            info->DebugOffset = info->WinHeadOffset + sizeof( os2_flat_header );
        }
        break;
    default:
        RcError( ERR_NOT_VALID_EXE, filename );
        return( false );
        break;
    }

    return( !RESSEEK( info->fid, 0, SEEK_SET ) );
} /* openExeFileInfoRO */

static bool openNewExeFileInfo( char *filename, ExeFileInfo *info )
/******************************************************************/
{
    info->fid = ResOpenNewFile( filename );
    if( info->fid == WRES_NIL_HANDLE ) {
        RcError( ERR_OPENING_TMP, filename, strerror( errno ) );
        return( false );
    }
    RegisterTmpFile( filename );
    info->IsOpen = true;
    info->DebugOffset = 0;
    info->name = filename;

    return( true );
} /* openNewExeFileInfo */

static void FreeNEFileInfoPtrs( NEExeInfo * info )
/*************************************************/
{
    if( info->Seg.Segments != NULL ) {
        RESFREE( info->Seg.Segments );
        info->Seg.Segments = NULL;
    }
    if( info->Res.Str.StringBlock != NULL ) {
        RESFREE( info->Res.Str.StringBlock );
        info->Res.Str.StringBlock = NULL;
    }
    if( info->Res.Str.StringList != NULL ) {
        RESFREE( info->Res.Str.StringList );
        info->Res.Str.StringList = NULL;
    }
} /* FreeNEFileInfoPtrs */

static void FreePEFileInfoPtrs( PEExeInfo * info )
/************************************************/
{
    if( info->Objects != NULL ) {
        RESFREE( info->Objects );
    }
}

static void FreeLXFileInfoPtrs( LXExeInfo *info )
/***********************************************/
{
    if( info->Objects != NULL ) {
        RESFREE( info->Objects );
    }
    if( info->Pages != NULL ) {
        RESFREE( info->Pages );
    }
    if( info->Res.resources != NULL ) {
        RESFREE( info->Res.resources );
    }
}

extern void ClosePass2FilesAndFreeMem( void )
/*******************************************/
{
    ExeFileInfo         *tmp;
    ExeFileInfo         *old;
//    char                *tmpfilename;

    tmp = &(Pass2Info.TmpFile);
    old = &(Pass2Info.OldFile);
//    tmpfilename = Pass2Info.TmpFileName;

    if( old->IsOpen ) {
        RESCLOSE( old->fid );
        old->IsOpen = false;
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
        RESCLOSE( tmp->fid );
        tmp->IsOpen = false;
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
    CloseResFiles( Pass2Info.ResFile );
} /* ClosePass2FilesAndFreeMem */

extern bool RcPass2IoInit( void )
/******************************/
{
    bool    noerror;
    bool    tmpexe_exists;

    memset( &Pass2Info, '\0', sizeof( RcPass2Info ) );
    Pass2Info.IoBuffer = RESALLOC( IO_BUFFER_SIZE );
    /* put the temporary file in the same location as the output file */
    Pass2Info.TmpFileName = RcMakeTmpInSameDir( CmdLineParms.OutExeFileName, '2', "tmp" );
    noerror = openExeFileInfoRO( CmdLineParms.InExeFileName, &(Pass2Info.OldFile) );
    if( noerror ) {
        noerror = openNewExeFileInfo( Pass2Info.TmpFileName, &(Pass2Info.TmpFile) );
    }
    tmpexe_exists = noerror;

    if( noerror ) {
        Pass2Info.TmpFile.Type = Pass2Info.OldFile.Type;
        Pass2Info.TmpFile.WinHeadOffset = Pass2Info.OldFile.WinHeadOffset;
        if( Pass2Info.OldFile.Type == EXE_TYPE_PE ) {
            Pass2Info.TmpFile.u.PEInfo.WinHead = &Pass2Info.TmpFile.u.PEInfo.WinHeadData;
            *Pass2Info.TmpFile.u.PEInfo.WinHead = *Pass2Info.OldFile.u.PEInfo.WinHead;
        }
        if( ( Pass2Info.OldFile.Type == EXE_TYPE_NE_WIN || Pass2Info.OldFile.Type == EXE_TYPE_NE_OS2 )
            && CmdLineParms.ExtraResFiles != NULL ) {
            RcError( ERR_FR_NOT_VALID_FOR_WIN );
            noerror = false;
        } else {
            noerror = OpenResFileInfo( Pass2Info.OldFile.Type );
        }
    }

    if( !noerror ) {
        RESFREE( Pass2Info.IoBuffer );
        Pass2Info.IoBuffer = NULL;
        ClosePass2FilesAndFreeMem();
        if( tmpexe_exists ) {
            remove( Pass2Info.TmpFileName );
            UnregisterTmpFile( Pass2Info.TmpFileName );
            RESFREE( Pass2Info.TmpFileName );
            Pass2Info.TmpFileName = NULL;
        }
    }

    return( noerror );
} /* RcPass2IoInit */

extern void RcPass2IoShutdown( bool noerror )
/******************************************/
{
    ClosePass2FilesAndFreeMem();
    if( Pass2Info.IoBuffer != NULL ) {
        RESFREE( Pass2Info.IoBuffer );
        Pass2Info.IoBuffer = NULL;
    }
    if( noerror ) {
        ChangeTmpToOutFile( Pass2Info.TmpFileName, CmdLineParms.OutExeFileName);
    } else {
        UnregisterTmpFile( Pass2Info.TmpFileName );
        remove( Pass2Info.TmpFileName );
    }
    RESFREE( Pass2Info.TmpFileName );
    Pass2Info.TmpFileName = NULL;
} /* RcPass2IoShutdown */

/****** Text file input routines ******/
/* These routines maintain a stack of input files. Pushing a file onto the */
/* stack opens the file and sets up the buffer. Poping a file closes the file */
/* All other routines (except TextInputInit and TextInputShutdown) operate */
/* on the current file which is the one at the top of the stack */

#define MAX_INCLUDE_DEPTH   16

typedef struct PhysFileInfo {
    char            *Filename;
    bool            IsOpen;
    WResFileID      fid;
    unsigned long   Offset;     /* offset in file to read from next time if this */
                                /* is not the current file */
} PhysFileInfo;

typedef struct FileStackEntry {
    LogicalFileInfo     Logical;
    PhysFileInfo        Physical;
} FileStackEntry;

typedef struct FileStack {
    char                *Buffer;
    size_t              BufferSize;
    char                *NextChar;
    char                *EofChar;       /* DON'T dereference, see below */
    /* + 1 for the before first entry */
    FileStackEntry      Stack[MAX_INCLUDE_DEPTH + 1];
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
    InStack.Buffer = RESALLOC( IO_BUFFER_SIZE );
    InStack.BufferSize = IO_BUFFER_SIZE;
    InStack.Current = InStack.Stack;
} /* RcIoTextInputInit */

extern bool RcIoTextInputShutdown( void )
/***************************************/
{
    if( InStack.Buffer != NULL ) {
        RESFREE( InStack.Buffer );
        InStack.Buffer = NULL;
        InStack.BufferSize = 0;
        if( IsEmptyFileStack( InStack ) ) {
            return( false );
        } else {
            while( !IsEmptyFileStack( InStack ) ) {
                RcIoPopTextInputFile();
            }
            // return( true );
        }
    }
    return( true );
} /* RcIoTextInputShutdown */

static bool OpenPhysicalFile( PhysFileInfo *phys )
/************************************************/
{
    if( !phys->IsOpen ) {
        phys->fid = RcIoOpenInput( phys->Filename, true );
        if( phys->fid == WRES_NIL_HANDLE ) {
            RcError( ERR_CANT_OPEN_FILE, phys->Filename, strerror( errno ) );
            return( true );
        }
        phys->IsOpen = true;
        if( fseek( WRES_FID2FH( phys->fid ), phys->Offset, SEEK_SET ) == -1 ) {
            RcError( ERR_READING_FILE, phys->Filename, strerror( errno ) );
            return( true );
        }
    }

    return( false );
} /* OpenPhysicalFile */

static bool OpenNewPhysicalFile( PhysFileInfo *phys, const char *filename )
/*************************************************************************/
{
    phys->Filename = RESALLOC( strlen( filename ) + 1 );
    strcpy( phys->Filename, filename );
    phys->IsOpen = false;
    phys->Offset = 0;

    return( OpenPhysicalFile( phys ) );
} /* OpenNewPhysicalFile */

static void SetPhysFileOffset( FileStack * stack )
/************************************************/
{
    PhysFileInfo    *phys;
    size_t          charsinbuff;

    if( !IsEmptyFileStack( *stack ) ) {
        phys = &(stack->Current->Physical);
        charsinbuff = stack->BufferSize - ( stack->NextChar - stack->Buffer );
        phys->Offset = ftell( WRES_FID2FH( phys->fid ) ) - charsinbuff;
    }
} /* SetPhysFileOffset */

static bool ReadBuffer( FileStack * stack )
/*****************************************/
{
    PhysFileInfo    *phys;
    size_t          numread;
    bool            error;
    int             inchar;

    phys = &(stack->Current->Physical);
    if( !phys->IsOpen ) {
        error = OpenPhysicalFile( phys );
        if( error ) {
            return( true );
        }
    }
    if( CmdLineParms.NoPreprocess ) {
        numread = fread( stack->Buffer, 1, stack->BufferSize, WRES_FID2FH( phys->fid ) );
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

    return( false );
} /* ReadBuffer */

static void FreeLogicalFilename( void )
{
    LogicalFileInfo     *log;

    log = &(InStack.Current->Logical);
    RESFREE( log->Filename );
    log->Filename = NULL;
}

static void FreePhysicalFilename( void )
{
    PhysFileInfo    *phys;

    phys = &(InStack.Current->Physical);
    RESFREE( phys->Filename );
    phys->Filename = NULL;
}

bool RcIoPushTextInputFile( const char * filename )
/*************************************************/
{
    bool                error;

    if( InStack.Current == InStack.Stack + MAX_INCLUDE_DEPTH - 1 ) {
        RcError( ERR_RCINCLUDE_TOO_DEEP, MAX_INCLUDE_DEPTH );
        return( true );
    }

    SetPhysFileOffset( &(InStack) );

    InStack.Current++;

    /* set up the logical file info */
    RcIoSetLogicalFileInfo( 1, filename );

    /* set up the physical file info */
    error = OpenNewPhysicalFile( &(InStack.Current->Physical), filename );
    if( error ) {
        FreeLogicalFilename();
        FreePhysicalFilename();
        InStack.Current--;
    } else {
        error = ReadBuffer( &(InStack) );
    }

    return( error );
} /* RcIoPushTextInputFile */

static void ClosePhysicalFile( PhysFileInfo * phys )
/**************************************************/
{
    if( phys->IsOpen ) {
        fclose( WRES_FID2FH( phys->fid ) );
        phys->IsOpen = false;
    }
} /* ClosePhysicalFile */

bool RcIoPopTextInputFile( void )
/*******************************/
{
    PhysFileInfo *  phys;

    phys = &(InStack.Current->Physical);
    ClosePhysicalFile( phys );
    FreeLogicalFilename();
    FreePhysicalFilename();
    InStack.Current--;
    if( IsEmptyFileStack( InStack ) ) {
        return( true );
    } else {
        ReadBuffer( &(InStack) );
        return( false );
    }
} /* RcIoPopTextInputFile */

static int GetLogChar( FileStack * stack )
/****************************************/
{
    int     newchar;

    newchar = (unsigned char)*(stack->NextChar);
    assert( newchar > 0 );
    if( newchar == '\n' ) {
        stack->Current->Logical.LineNum++;
    }

    stack->NextChar++;
    return( newchar );
} /* GetLogChar */

int RcIoGetChar( void )
/*********************/
{
    bool    isempty;
    bool    error;

    if( IsEmptyFileStack( InStack ) ) {
        return( EOF );
    }

    if( InStack.NextChar >= InStack.EofChar ) {
        /* we have reached the end of the buffer */
        if( InStack.NextChar >= InStack.Buffer + InStack.BufferSize ) {
            /* try to read next buffer */
            error = ReadBuffer( &(InStack) );
            if( error ) {
                /* this error is reported in ReadBuffer so just terminate */
                RcFatalError( ERR_NO_MSG );
            }
        }
        if( InStack.NextChar >= InStack.EofChar ) {
            /* this is a real EOF */
            /* unstack one file */
            isempty = RcIoPopTextInputFile();
            if( isempty ) {
                return( EOF );
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
        log->IsCOrHFile = false;
    }
} /* RcIoOverrideIsCOrHFlag */

extern void RcIoSetIsCOrHFlag( void )
/***********************************/
{
    LogicalFileInfo *   log;
    char                ext[_MAX_EXT];

    if( !IsEmptyFileStack( InStack ) ) {
        log = &(InStack.Current->Logical);
        _splitpath( log->Filename, NULL, NULL, NULL, ext );
        /* if this is a c or h file ext will be '.', '[ch]', '\0' */
        if( ( ext[1] == 'c' || ext[1] == 'h' || ext[1] == 'C' || ext[1] == 'H' ) && ext[2] == '\0' ) {
            /* if the logical file is a c or h file */
            log->IsCOrHFile = true;
        } else {
            log->IsCOrHFile = false;
        }
    }
} /* RcIoSetIsCOrHFlag */

extern void RcIoSetLogicalFileInfo( int linenum, const char * filename )
/**********************************************************************/
{
    LogicalFileInfo *   log;

    if( !IsEmptyFileStack( InStack ) ) {
        log = &(InStack.Current->Logical);
        log->LineNum = linenum;
        if( filename != NULL ) {
            if( log->Filename == NULL ) {
                log->Filename = RESALLOC( strlen( filename ) + 1 );
                strcpy( log->Filename, filename );
            } else if( strcmp( log->Filename, filename ) != 0 ) {
                RESFREE( log->Filename );
                log->Filename = RESALLOC( strlen( filename ) + 1 );
                strcpy( log->Filename, filename );
            }
            RcIoSetIsCOrHFlag();
        }
    }
} /* RcIoSetLogicalFileInfo */

extern bool RcIoIsCOrHFile( void )
/********************************/
/* returns true if the current file is a .c or .h file, false otherwise */
{
    if( IsEmptyFileStack( InStack ) ) {
        return( false );
    } else {
        return( InStack.Current->Logical.IsCOrHFile );
    }
} /* RcIoIsCOrHFile */

/*
 * RcIoOpenInput
 * NB when an error occurs this function MUST return without altering errno
 */
WResFileID RcIoOpenInput( const char *filename, bool text_mode )
/**************************************************************/
{
    WResFileID          fid;
    FileStackEntry      *currfile;
    bool                no_handles_available;

    if( text_mode ) {
        fid = WRES_FH2FID( fopen( filename, "rt" ) );
    } else {
        fid = ResOpenFileRO( filename );
    }
    no_handles_available = ( fid == WRES_NIL_HANDLE && errno == EMFILE );
    if( no_handles_available ) {
        /* set currfile to be the first (not before first) entry */
        /* close open files except the current input file until able to open */
        /* don't close the current file because Offset isn't set */
        for( currfile = InStack.Stack + 1; currfile < InStack.Current && no_handles_available; ++currfile ) {
            if( currfile->Physical.IsOpen ) {
                ClosePhysicalFile( &(currfile->Physical) );
                if( text_mode ) {
                    fid = WRES_FH2FID( fopen( filename, "rt" ) );
                } else {
                    fid = ResOpenFileRO( filename );
                }
                no_handles_available = ( fid == WRES_NIL_HANDLE && errno == EMFILE );
            }
       }
    }
    return( fid );

} /* RcIoOpenInput */

extern void RcIoInitStatics( void )
/*********************************/
{
    memset( &InStack, 0, sizeof( FileStack ) );
}
