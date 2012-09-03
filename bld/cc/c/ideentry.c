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
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <setjmp.h>
#include "bool.h"
#include "idedll.h"
#include "cgdefs.h"
#include "cgmisc.h"
#include "cgcli.h"
#include "feprotos.h"

extern  void    SetNoCurrInc( void ); // no curr inc

static   IDECBHdl      Hdl;          // - handle for this instantiation
static   IDECallBacks* Cbs;          // - call backs into IDE
static   IDEInitInfo   Info;

extern void ConsMsg( char const  *line ){
// C compiler call back to do a  console print to stdout

    (*Cbs->PrintMessage)( Hdl, line );
    // we are ignoring return for now
}

extern void ConBlip( void ){
// C compiler do a blip to console
}

extern bool ConTTY( void ){
// C compiler do a blip to console
    return( Info.console_output );
}

extern void ConsErrMsg( cmsg_info  *cinfo ){
// C compiler call back to do a  console print to stderr

    IDEMsgInfo  info;
    IDEMsgSeverity severity;

    switch( cinfo->class ){
    case CMSG_INFO:
        severity = IDEMSGSEV_NOTE;
        break;
    case CMSG_WARN:
        severity = IDEMSGSEV_WARNING;
        break;
    case CMSG_ERRO:
        severity = IDEMSGSEV_ERROR;
        break;
    }
    IdeMsgInit( &info, severity, cinfo->msgtxt );
    IdeMsgSetHelp( &info, "wccerrs.hlp", cinfo->msgnum );
    IdeMsgSetMsgNo( &info, cinfo->msgnum );
    if( cinfo->fname != NULL ){
        IdeMsgSetSrcFile( &info, cinfo->fname );
    }
    if( cinfo->line != 0 ){
        IdeMsgSetSrcLine( &info, cinfo->line );
    }
     (*Cbs->PrintWithInfo)( Hdl, &info );
    // we are ignoring return for now
}

extern void ConsErrMsgVerbatim( char const  *line ){
// C compiler call back to a console print to stderr

    IDEMsgInfo info;
    IdeMsgInit( &info, IDEMSGSEV_ERROR, line );
    (*Cbs->PrintWithInfo)( Hdl, &info );
    // we are ignoring return for now
}

extern void BannerMsg( char const  *line ){
// C compiler call back to print a banner type msg

     IDEMsgInfo info;
      IdeMsgInit( &info, IDEMSGSEV_BANNER, line );
     (*Cbs->PrintWithInfo)( Hdl, &info );
    // we are ignoring return for now
}

extern void DebugMsg( char const  *line ){
// C compiler call back to print a banner type msg

     IDEMsgInfo info;
      IdeMsgInit( &info, IDEMSGSEV_DEBUG, line );
     (*Cbs->PrintWithInfo)( Hdl, &info );
    // we are ignoring return for now
}

extern void NoteMsg( char const  *line ){
// C compiler call back to print a banner type msg

     IDEMsgInfo info;
      IdeMsgInit( &info, IDEMSGSEV_NOTE_MSG, line );
     (*Cbs->PrintWithInfo)( Hdl, &info );
    // we are ignoring return for now
}

extern char *FEGetEnv( char const *name ){
// get enviorment variable
    char *ret;
    ret = NULL;
    if( !Info.ignore_env ){
        if((*Cbs->GetInfo)( Hdl, IDE_GET_ENV_VAR,
             (IDEGetInfoWParam)name, (IDEGetInfoLParam)&ret ) ){
            ret = NULL;
        }
    }
    return( ret );
}

extern void FESetCurInc( void ){
    if( Info.ignore_env ){
        SetNoCurrInc();
    }
}

static   jmp_buf *FatalEnv;
extern void MyExit( int ret ){
    longjmp( *FatalEnv, ret );
}

// IDE INTERFACE


unsigned IDEDLL_EXPORT IDEGetVersion ( void ){
 // GET IDE VERSION
    return IDE_CUR_DLL_VER;
}
#define HEAP_CHK 0
#if HEAP_CHK == 1
static long HeapUsedHi;
static long HeapFreeHi;
#endif
IDEBool IDEDLL_EXPORT IDEInitDLL
    ( IDECBHdl      hdl             // - handle for this instantiation
    , IDECallBacks* cbs             // - call backs into IDE
    , IDEDllHdl*    info ){         // - uninitialized info
//***********************
// DLL INITIALIZATION
//***********************
    Hdl = hdl;
    Cbs = cbs;
    *info = NULL;
    #if HEAP_CHK == 1
        HeapUsedHi = 0;
        HeapFreeHi = 0;
    #endif
    FrontEndInit( TRUE );
    return FALSE;
}

void IDEDLL_EXPORT IDEFreeHeap( void ){
//*************************************
//Cleanup Heap
//*************************************
   _heapmin();
}

void IDEDLL_EXPORT IDEFiniDLL( IDEDllHdl hdl ){
//*********************************************
//DLL COMPLETION
//********************************************
    hdl = hdl;
    FrontEndFini();
}

#if HEAP_CHK == 1
#include <stdio.h>
#if 0
static void heap_dump( void ){
    struct _heapinfo h_info;
    int heap_status;
    h_info._pentry = NULL;
    for(;;){
        heap_status = _heapwalk( &h_info );
        if( heap_status != _HEAPOK )break;
        if( h_info._useflag == _USEDENTRY  ){
            printf( " %s block at %Fp of size %4.4X\n",
                "USED", h_info._pentry, h_info._size );
        }
    }
    switch( heap_status ){
    case _HEAPEND:
        printf( "end of heap\n" );
        break;
    case _HEAPEMPTY:
        printf( "heap empty\n" );
        break;
    case _HEAPBADBEGIN:
        printf( "heap hurt\n" );
        break;
    case _HEAPBADPTR:
        printf( "bad heap ptr\n" );
        break;
    case _HEAPBADNODE:
        printf( "bad heap node\n" );
        break;
    }
}
#endif
struct heap_stat {
    int free;
    int used;
};

static int heap_size( struct heap_stat *stat ){
    struct _heapinfo h_info;
    int heap_status;
    h_info._pentry = NULL;
    stat->free = 0;
    stat->used = 0;
    for(;;){
        heap_status = _heapwalk( &h_info );
        if( heap_status != _HEAPOK )break;
        if( h_info._useflag == _USEDENTRY ){
            stat->used += h_info._size;
        }else{
            stat->free += h_info._size;
        }
    }
    return( heap_status );
}
#endif
IDEBool IDEDLL_EXPORT IDERunYourSelf // COMPILE A PROGRAM
    ( IDEDllHdl hdl             // - handle for this instantiation
    , const char* opts          // - options
    , IDEBool* fatal_error ){   // - addr[ fatality indication ]
//****************************
// Do a compile of a file
//****************************
    jmp_buf env;
    char    input[_MAX_PATH]; // - input file name
    char    output[4+_MAX_PATH];//- output file name (need room for "-fo=")
    char const *argv[4];
    int     ret;
#if HEAP_CHK == 1
    struct heap_stat    after;
#endif

    TBreak();   // clear any pending IDEStopRunning's
    argv[1] = NULL;
    argv[2] = NULL;
    *fatal_error = FALSE;
    FatalEnv = &env;
    if( ret = setjmp( env ) ) {       /* if fatal error has occurred */
        *fatal_error = TRUE;
        return( ret ); // get out
    }
    if( !Info.cmd_line_has_files ){
        if(!(*Cbs->GetInfo)( Hdl, IDE_GET_SOURCE_FILE, 0, (IDEGetInfoLParam) &input[0] ) ){
            argv[1] = input;
        }
        if(!(*Cbs->GetInfo)( Hdl, IDE_GET_TARGET_FILE, 0, (IDEGetInfoLParam) &output[4] ) ){
            output[0] = '-';
            output[1] = 'f';
            output[2] = 'o';
            output[3] = '=';
            output[4] = '\0';
            argv[2] = output;
        }
    }
    argv[0] = opts;
    argv[3] = NULL;
    ret = FrontEnd( (char **)argv );
#if HEAP_CHK  == 1

    switch( heap_size( &after ) ){
    case _HEAPOK:
        break;
    case _HEAPBADBEGIN:
        DebugMsg( "heap HEAPBADBEGIN" );
        break;
    case _HEAPBADNODE:
        DebugMsg( "heap HEAPBADNODE" );
        break;
    }
    printf( "Used %d + Free %d = %d\n", after.used, after.free, after.used + after.free  );
    if( after.used != HeapUsedHi ){
        printf( "Used different\n" );
        HeapUsedHi = after.used;
 //     heap_dump();
    }
    if( after.free > HeapFreeHi ){
        printf( "Bigger Free\n" );
        HeapFreeHi = after.free;
 //     heap_dump();
    }
    fflush( stdout );
#endif
#ifdef __OS2__
   _heapmin();
#endif
    return( ret );
}

void IDEDLL_EXPORT IDEStopRunning( void )
{
    CauseTBreak();
}

// HELP Interface

IDEBool IDEDLL_EXPORT IDEProvideHelp       // PROVIDE HELP INFORMATION
    ( IDEDllHdl hdl             // - handle for this instantiation
    , char const* msg )         // - message
{
    hdl = hdl;
    msg = msg;
    return( TRUE );
}

IDEBool IDEDLL_EXPORT IDEPassInitInfo( IDEDllHdl hdl, IDEInitInfo *info )
{
    hdl = hdl;
    Info = *info;
    if( info->ver < IDE_CUR_INFO_VER5 ) {
        return( TRUE );
    }
    return( FALSE );
}
