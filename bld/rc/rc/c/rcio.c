/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include <errno.h>
#ifdef __UNIX__
    #include <sys/stat.h>
#else
    #include <process.h>
#endif
#include "global.h"
#include "rcerrors.h"
#include "preproc.h"
#include "reserr.h"
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
#include "pathgrp2.h"

#include "clibext.h"


#define BUFFER_SIZE         1024

#define MAX_INCLUDE_DEPTH   16

typedef struct file_loc {
    struct file_loc     *prev;
    char                *Filename;
    unsigned            LineNo;
    bool                IsCOrHFile;
} file_loc;

typedef struct FileStackEntry {
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
    file_loc            *Location;
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

static void freeCurrentFileName( void )
/*************************************/
{
    RESFREE( InStack.Current->Filename );
    InStack.Current->Filename = NULL;
}

static bool checkCurrentFileType( const char *filename )
/******************************************************/
{
    pgroup2     pg;
    bool        isCOrH;

    isCOrH = false;
    _splitpath2( filename, pg.buffer, NULL, NULL, NULL, &pg.ext );
    /* if this is a c or h file ext will be '.', '[ch]', '\0' */
    if( pg.ext[0] == '.' && pg.ext[1] != '\0' && pg.ext[2] == '\0' ) {
        switch( pg.ext[1] ) {
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

static file_loc *addLocation( file_loc *prev, const char *filename )
/******************************************************************/
{
    file_loc    *loc;

    if( prev == NULL || strcmp( prev->Filename, filename ) != 0 ) {
        loc = RESALLOC( sizeof( file_loc ) );
        if( loc != NULL ) {
            loc->prev = prev;
            loc->IsCOrHFile = checkCurrentFileType( filename );
            loc->Filename = RESALLOC( strlen( filename ) + 1 );
            strcpy( loc->Filename, filename );
            return( loc );
        }
    }
    return( prev );
}

static file_loc *removeLocation( file_loc *loc )
/**********************************************/
{
    file_loc    *prev;

    prev = NULL;
    if( loc != NULL ) {
        prev = loc->prev;
        RESFREE( loc->Filename );
        RESFREE( loc );
    }
    return( prev );
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
    freeCurrentFileName();
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
    InStack.Location = NULL;
} /* RcIoTextInputInit */

static bool RcIoTextInputShutdown( void )
/***************************************/
{
    while( InStack.Location != NULL ) {
        InStack.Location = removeLocation( InStack.Location );
    }
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
        freeCurrentFileName();
        InStack.Current--;
    } else {
        error = readCurrentFileBuffer();
    }

    return( error );
} /* RcIoPushTextInputFile */

const char *RcIoGetCurrentFileName( void )
/****************************************/
{
    if( InStack.Location == NULL ) {
        return( NULL );
    } else {
        return( InStack.Location->Filename );
    }
} /* RcIoGetCurrentFileName */

unsigned RcIoGetCurrentFileLineNo( void )
/***************************************/
{
    if( InStack.Location == NULL ) {
        return( 0 );
    } else {
        return( InStack.Location->LineNo );
    }
} /* RcIoGetCurrentFileLineNo */

void RcIoSetCurrentFileInfo( unsigned lineno, const char *filename )
/******************************************************************/
{
    file_loc    *loc;

    loc = InStack.Location;
    if( loc == NULL || lineno == 1 ) {
        loc = addLocation( loc, filename );
    } else if( strcmp( loc->Filename, filename ) != 0 ) {
        loc = removeLocation( loc );
    }
    loc->LineNo = lineno;
    InStack.Location = loc;
} /* RcIoSetCurrentFileInfo */

bool RcIoIsCOrHFile( void )
/*************************/
/* returns true if the current file is a .c or .h file, false otherwise */
{
    if( InStack.Location == NULL ) {
        return( false );
    } else {
        return( InStack.Location->IsCOrHFile );
    }
} /* RcIoIsCOrHFile */

static int GetLogChar( void )
/***************************/
{
    int     newchar;

    newchar = *(unsigned char *)InStack.NextChar;
    assert( newchar > 0 );
    if( newchar == '\n' ) {
        InStack.Location->LineNo++;
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
    ResLocation   null_loc;

    memset( &CurrResFile, 0, sizeof( CurrResFile ) );

    /* open the temporary file */
    CurrResFile.filename = TMPFILE0;
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
            SemAddResource( &null_id, &null_id, 0, null_loc );
        }
    } else {
        CurrResFile.IsWatcomRes = true;
        WResFileInit( CurrResFile.fp );
    }
    CurrResFile.NextCurOrIcon = 1;
    return( false );
} /* Pass1InitRes */

static const char *get_parent_filename( void **cookie )
/*****************************************************/
{
    file_loc  **last;

    if( cookie != NULL ) {
        last = (file_loc **)cookie;
        if( *last == NULL ) {
            *last = InStack.Location;
        } else {
            *last = (*last)->prev;
        }
        if( *last != NULL ) {
            return( (*last)->Filename );
        }
    }
    return( NULL );
}

int RcFindSourceFile( const char *name, char *fullpath )
/******************************************************/
{
    return( PP_IncludePathFind( name, strlen( name ), fullpath, PPINCLUDE_USR, get_parent_filename ) );
}

int PP_MBCharLen( const char *p )
/*******************************/
{
    return( CharSetLen[*(unsigned char *)p] + 1 );
}

static bool PreprocessInputFile( void )
/*************************************/
{
    pp_flags    ppflags;
    char        rcdefine[13];
    char        **cppargs;
    char        *p;
    int         rc;

    ppflags = PPFLAG_EMIT_LINE | PPFLAG_IGNORE_INCLUDE | PPFLAG_TRUNCATE_FILE_NAME;
    if( CmdLineParms.IgnoreCWD ) {
        ppflags |= PPFLAG_IGNORE_CWD;
    }
    rc = PP_FileInit( CmdLineParms.InFileName, ppflags, NULL );
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
    if( CmdLineParms.CPPArgs != NULL ) {
        for( cppargs = CmdLineParms.CPPArgs; (p = *cppargs) != NULL; ++cppargs ) {
            for( ; *p != '\0'; ++p ) {
                if( *p == '=' ) {
                    *p = ' ';
                    break;
                }
            }
            PP_Define( *cppargs + 2 );         // skip over -d
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
                CopyTmpToOutFile( CurrResFile.fp, CmdLineParms.OutResFileName );
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

void RcIoInitStatics( void )
/**************************/
{
    memset( &InStack, 0, sizeof( FileStack ) );
}
