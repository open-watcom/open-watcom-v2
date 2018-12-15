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


#define BUFFER_SIZE     1024

#define MAX_INCLUDE_DEPTH   16

typedef struct FileStackEntry {
    struct {
        char                *Filename;
        unsigned            LineNo;
        bool                IsCOrHFile;
    }                   loc;
    char                *Filename;
    bool                IsOpen;
    FILE                *fp;
    unsigned long       Offset;     /* offset in file to read from next time if this */
                                    /* is not the current file */
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

static FileStack    InStack;

/****** Text file input routines ******/
/* These routines maintain a stack of input files. Pushing a file onto the */
/* stack opens the file and sets up the buffer. Poping a file closes the file */
/* All other routines (except TextInputInit and TextInputShutdown) operate */
/* on the current file which is the one at the top of the stack */

static void freeCurrentFileNames( void )
/**************************************/
{
    RESFREE( InStack.Current->loc.Filename );
    RESFREE( InStack.Current->Filename );
    InStack.Current->loc.Filename = NULL;
    InStack.Current->Filename = NULL;
}

static void saveCurrentFileOffset( void )
/***************************************/
{
    size_t          charsinbuff;

    if( !IsEmptyFileStack( InStack ) ) {
        charsinbuff = InStack.BufferSize - ( InStack.NextChar - InStack.Buffer );
        InStack.Current->Offset = (unsigned long)( ftell( InStack.Current->fp ) - charsinbuff );
    }
} /* saveCurrentFileOffset */

static void closeFile( FileStackEntry *file )
/*******************************************/
{
    if( file->IsOpen ) {
        fclose( file->fp );
        file->IsOpen = false;
    }
} /* closeFile */

static bool openCurrentFile( void )
/*********************************/
{
    if( !InStack.Current->IsOpen ) {
        InStack.Current->fp = RcIoOpenInput( InStack.Current->Filename, true );
        if( InStack.Current->fp == NULL ) {
            RcError( ERR_CANT_OPEN_FILE, InStack.Current->Filename, strerror( errno ) );
            return( true );
        }
        InStack.Current->IsOpen = true;
        if( fseek( InStack.Current->fp, InStack.Current->Offset, SEEK_SET ) == -1 ) {
            RcError( ERR_READING_FILE, InStack.Current->Filename, strerror( errno ) );
            return( true );
        }
    }

    return( false );
} /* openCurrentFile */

static bool openNewFile( const char *filename )
/*********************************************/
{
    InStack.Current->Filename = RESALLOC( strlen( filename ) + 1 );
    strcpy( InStack.Current->Filename, filename );
    InStack.Current->IsOpen = false;
    InStack.Current->Offset = 0;

    /* set up the logical file info */
    RcIoSetCurrentFileInfo( 1, filename );

    return( openCurrentFile() );
} /* openNewFile */

static bool readCurrentFileBuffer( void )
/***************************************/
{
    size_t          numread;
    bool            error;
    int             inchar;

    if( !InStack.Current->IsOpen ) {
        error = openCurrentFile();
        if( error ) {
            return( true );
        }
    }
    if( CmdLineParms.NoPreprocess ) {
        numread = fread( InStack.Buffer, 1, InStack.BufferSize, InStack.Current->fp );
    } else {
        for( numread = 0; numread < InStack.BufferSize; numread++ ) {
            inchar = PP_Char();
            if( inchar == EOF ) {
                break;
            }
            *( InStack.Buffer + numread ) = (char)inchar;
        }
    }
    InStack.NextChar = InStack.Buffer;
    InStack.EofChar = InStack.Buffer + numread;   /* may be past end of buffer */

    return( false );
} /* readCurrentFileBuffer */

static bool RcIoPopTextInputFile( void )
/**************************************/
{
    closeFile( InStack.Current );
    freeCurrentFileNames();
    InStack.Current--;
    if( IsEmptyFileStack( InStack ) ) {
        return( true );
    } else {
        readCurrentFileBuffer();
        return( false );
    }
} /* RcIoPopTextInputFile */

static void RcIoTextInputInit( void )
/***********************************/
{
    InStack.Buffer = RESALLOC( IO_BUFFER_SIZE );
    InStack.BufferSize = IO_BUFFER_SIZE;
    InStack.Current = InStack.Stack;
} /* RcIoTextInputInit */

static bool RcIoTextInputShutdown( void )
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
        }
    }
    return( true );
} /* RcIoTextInputShutdown */

static bool RcIoPushTextInputFile( const char *filename )
/*******************************************************/
{
    bool                error;

    if( InStack.Current == InStack.Stack + MAX_INCLUDE_DEPTH - 1 ) {
        RcError( ERR_RCINCLUDE_TOO_DEEP, MAX_INCLUDE_DEPTH );
        return( true );
    }

    saveCurrentFileOffset();

    InStack.Current++;

    /* open file and set up the file info */
    error = openNewFile( filename );
    if( error ) {
        freeCurrentFileNames();
        InStack.Current--;
    } else {
        error = readCurrentFileBuffer();
    }

    return( error );
} /* RcIoPushTextInputFile */

const char *RcIoGetCurrentFileName( void )
/****************************************/
{
    if( IsEmptyFileStack( InStack ) ) {
        return( NULL );
    } else {
        return( InStack.Current->loc.Filename );
    }
} /* RcIoGetCurrentFileName */

unsigned RcIoGetCurrentFileLineNo( void )
/***************************************/
{
    if( IsEmptyFileStack( InStack ) ) {
        return( 0 );
    } else {
        return( InStack.Current->loc.LineNo );
    }
} /* RcIoGetCurrentFileLineNo */

static bool checkCurrentFileType( void )
/**************************************/
{
    char        ext[_MAX_EXT];
    bool        isCOrH;

    isCOrH = false;
    _splitpath( InStack.Current->loc.Filename, NULL, NULL, NULL, ext );
    /* if this is a c or h file ext will be '.', '[ch]', '\0' */
    if( ext[0] == '.' && ext[1] != '\0' && ext[2] == '\0' ) {
        switch( ext[1] ) {
        case 'c':
        case 'C':
        case 'h':
        case 'H':
            isCOrH = true;
            break;
        }
    }
    return( isCOrH );

} /* checkCurrentFileType */

void RcIoSetCurrentFileInfo( unsigned lineno, const char *filename )
/******************************************************************/
{
    if( !IsEmptyFileStack( InStack ) ) {
        InStack.Current->loc.LineNo = lineno;
        if( filename != NULL ) {
            if( InStack.Current->loc.Filename == NULL ) {
                InStack.Current->loc.Filename = RESALLOC( strlen( filename ) + 1 );
                strcpy( InStack.Current->loc.Filename, filename );
                InStack.Current->loc.IsCOrHFile = checkCurrentFileType();
            } else if( strcmp( InStack.Current->loc.Filename, filename ) != 0 ) {
                RESFREE( InStack.Current->loc.Filename );
                InStack.Current->loc.Filename = RESALLOC( strlen( filename ) + 1 );
                strcpy( InStack.Current->loc.Filename, filename );
                InStack.Current->loc.IsCOrHFile = checkCurrentFileType();
            }
        }
    }
} /* RcIoSetCurrentFileInfo */

bool RcIoIsCOrHFile( void )
/*************************/
/* returns true if the current file is a .c or .h file, false otherwise */
{
    if( IsEmptyFileStack( InStack ) ) {
        return( false );
    } else {
        return( InStack.Current->loc.IsCOrHFile );
    }
} /* RcIoIsCOrHFile */

static int GetLogChar( void )
/***************************/
{
    int     newchar;

    newchar = *(unsigned char *)InStack.NextChar;
    assert( newchar > 0 );
    if( newchar == '\n' ) {
        InStack.Current->loc.LineNo++;
    }

    InStack.NextChar++;
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
            error = readCurrentFileBuffer();
            if( error ) {
                /* this error is reported in readCurrentFileBuffer so just terminate */
                RcFatalError( ERR_NO_MSG );
            }
        }
        if( InStack.NextChar >= InStack.EofChar ) {
            /* this is a real EOF */
            /* unstack one file */
            isempty = RcIoPopTextInputFile();
            if( isempty )
                return( EOF );
            /* if we are still at the EOF char, there has been an error */
            if( InStack.NextChar >= InStack.EofChar ) {
                /* this error is reported in readCurrentFileBuffer so just terminate */
                RcFatalError( ERR_NO_MSG );
            } else {
                /* return \n which will end the current token properly */
                /* if it it is not a string and end it with a runaway */
                /* string error for strings */
                return( '\n' );
            }
        }
    }
    return( GetLogChar() );
} /* RcIoGetChar */

/*
 * RcIoOpenInput
 * NB when an error occurs this function MUST return without altering errno
 */
FILE *RcIoOpenInput( const char *filename, bool text_mode )
/*********************************************************/
{
    FILE                *fp;
    FileStackEntry      *currfile;
    bool                no_handles_available;

    if( text_mode ) {
        fp = fopen( filename, "rt" );
    } else {
        fp = ResOpenFileRO( filename );
    }
    no_handles_available = ( fp == NULL && errno == EMFILE );
    /* set currfile to be the first (not before first) entry */
    /* close open files except the current input file until able to open */
    /* don't close the current file because Offset isn't set */
    for( currfile = InStack.Stack + 1; no_handles_available && currfile < InStack.Current; ++currfile ) {
        if( currfile->IsOpen ) {
            closeFile( currfile );
            if( text_mode ) {
                fp = fopen( filename, "rt" );
            } else {
                fp = ResOpenFileRO( filename );
            }
            no_handles_available = ( fp == NULL && errno == EMFILE );
        }
    }
    return( fp );

} /* RcIoOpenInput */

/*
 * Pass 1 related functions
 */

static bool Pass1InitRes( void )
/******************************/
{
    WResID        null_id;
    ResMemFlags   null_memflags;
    ResLocation   null_loc;

    memset( &CurrResFile, 0, sizeof( CurrResFile ) );

    /* open the temporary file */
    CurrResFile.filename = "Temporary file 0 (res)";
    CurrResFile.fp = ResOpenFileTmp( NULL );
    if( CurrResFile.fp == NULL ) {
        RcError( ERR_OPENING_TMP, CurrResFile.filename, LastWresErrStr() );
        return( true );
    }

    /* initialize the directory */
    CurrResFile.dir = WResInitDir();
    if( CurrResFile.dir == NULL ) {
        RcError( ERR_OUT_OF_MEMORY );
        ResCloseFile( CurrResFile.fp );
        CurrResFile.fp = NULL;
        return( true );
    }

    if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN16 ) {
        WResSetTargetOS( CurrResFile.dir, WRES_OS_WIN16 );
    } else if( CmdLineParms.TargetOS == RC_TARGET_OS_WIN32 ) {
        WResSetTargetOS( CurrResFile.dir, WRES_OS_WIN32 );
    } else {
        WResSetTargetOS( CurrResFile.dir, WRES_OS_OS2 );
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
        WResFileInit( CurrResFile.fp );
    }
    CurrResFile.NextCurOrIcon = 1;
    return( false );
} /* Pass1InitRes */

int RcFindSourceFile( const char *name, char *fullpath )
/******************************************************/
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

bool RcPass1IoInit( void )
/************************/
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

static bool ChangeTmpToOutFile( FILE *tmpfile, const char *out_name )
/*******************************************************************/
{
    RcStatus    status;      /* error while deleting or renaming */
    FILE        *outfile;
    size_t      numread;
    char        *buffer;

    buffer = RESALLOC( BUFFER_SIZE );

    status = RS_OK;
    RESSEEK( tmpfile, 0, SEEK_SET );
    outfile = ResOpenFileRW( out_name );
    while( (numread = RESREAD( tmpfile, buffer, BUFFER_SIZE )) != 0 ) {
        if( numread != BUFFER_SIZE && RESIOERR( tmpfile, numread ) ) {
            status = RS_READ_ERROR;
            break;
        }
        if( RESWRITE( outfile, buffer, numread ) != numread ) {
            status = RS_WRITE_ERROR;
            break;
        }
    }
    ResCloseFile( outfile );

    RESFREE( buffer );
    return( status == RS_OK );

} /* ChangeTmpToOutFile */

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
    if( CurrResFile.fp != NULL ) {
        if( CmdLineParms.TargetOS == RC_TARGET_OS_OS2 ) {
            WriteOS2Tables();
        } else {
            WriteWINTables();
        }
        if( !ErrorHasOccured ) {
            if( CurrResFile.IsWatcomRes ) {
                error = WResWriteDir( CurrResFile.fp, CurrResFile.dir );
                if( error ) {
                    RcError( ERR_WRITTING_RES_FILE, CurrResFile.filename, LastWresErrStr() );
                }
            }
            if( !error ) {
                ChangeTmpToOutFile( CurrResFile.fp, CmdLineParms.OutResFileName );
            }
        }
        if( CurrResFile.dir != NULL ) {
            WResFreeDir( CurrResFile.dir );
            CurrResFile.dir = NULL;
        }
        if( ResCloseFile( CurrResFile.fp ) ) {
            RcError( ERR_CLOSING_TMP, CurrResFile.filename, LastWresErrStr() );
        }
        CurrResFile.fp = NULL;
    }
} /* Pass1ResFileShutdown */

void RcPass1IoShutdown( void )
/****************************/
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
        Pass2Info.ResFile->fp = NULL;
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

    info->fp = ResOpenFileRO( filename );
    if( info->fp == NULL ) {
        RcError( ERR_CANT_OPEN_FILE, filename, strerror( errno ) );
        return( false );
    }
    info->IsOpen = true;
    info->Type = FindNEPELXHeader( info->fp, &info->WinHeadOffset );
    info->name = filename;
    switch( info->Type ) {
    case EXE_TYPE_NE_WIN:
    case EXE_TYPE_NE_OS2:
        status = SeekRead( info->fp, info->WinHeadOffset, &info->u.NEInfo.WinHead, sizeof( os2_exe_header ) );
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
        status = SeekRead( info->fp, info->WinHeadOffset, &PE32( *pehdr ), sizeof( pe_header ) );
        if( status != RS_OK ) {
            RcError( ERR_NOT_VALID_EXE, filename );
            return( false );
        }
        if( IS_PE64( *pehdr ) ) {
            status = SeekRead( info->fp, info->WinHeadOffset, &PE64( *pehdr ), sizeof( pe_header64 ) );
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
        status = SeekRead( info->fp, info->WinHeadOffset, &info->u.LXInfo.OS2Head, sizeof( os2_flat_header ) );
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

    return( !RESSEEK( info->fp, 0, SEEK_SET ) );
} /* openExeFileInfoRO */

/*
 * Pass 2 related functions
 */

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

static void ClosePass2FilesAndFreeMem( void )
/*******************************************/
{
    ExeFileInfo         *tmp;
    ExeFileInfo         *old;

    tmp = &(Pass2Info.TmpFile);
    old = &(Pass2Info.OldFile);

    if( old->IsOpen ) {
        RESCLOSE( old->fp );
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

bool RcPass2IoInit( void )
/************************/
{
    bool    noerror;
    bool    tmpexe_exists;

    memset( &Pass2Info, 0, sizeof( RcPass2Info ) );
    Pass2Info.IoBuffer = RESALLOC( IO_BUFFER_SIZE );

    noerror = openExeFileInfoRO( CmdLineParms.InExeFileName, &(Pass2Info.OldFile) );
    if( noerror ) {
        Pass2Info.TmpFile.name = "Temporary file 2 (exe)";
        Pass2Info.TmpFile.fp = ResOpenFileTmp( NULL );
        if( Pass2Info.TmpFile.fp == NULL ) {
            RcError( ERR_OPENING_TMP, Pass2Info.TmpFile.name, strerror( errno ) );
            noerror = false;
        }
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
            ResCloseFile( Pass2Info.TmpFile.fp );
            Pass2Info.TmpFile.fp = NULL;
        }
    }

    return( noerror );
} /* RcPass2IoInit */

void RcPass2IoShutdown( bool noerror )
/************************************/
{
    ClosePass2FilesAndFreeMem();
    if( Pass2Info.IoBuffer != NULL ) {
        RESFREE( Pass2Info.IoBuffer );
        Pass2Info.IoBuffer = NULL;
    }
    if( noerror ) {
        ChangeTmpToOutFile( Pass2Info.TmpFile.fp, CmdLineParms.OutExeFileName );
    }
    ResCloseFile( Pass2Info.TmpFile.fp );
    Pass2Info.TmpFile.fp = NULL;

} /* RcPass2IoShutdown */

void RcIoInitStatics( void )
/**************************/
{
    memset( &InStack, 0, sizeof( FileStack ) );
}
