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
#ifdef __WATCOMC__
#include <malloc.h>
#endif
#include <setjmp.h>
#include "bool.h"
#include "idedll.h"
#include "cgdefs.h"
#include "cgmisc.h"
#include "cgcli.h"
#include "feprotos.h"

#include "clibext.h"


#define IDEFN(x)        (*IdeCbs->x)

static   IDECBHdl       IdeHdl;      // - handle for this instantiation
static   IDECallBacks   *IdeCbs;       // - call backs into IDE
//static   IDEInitInfo   Info;

extern void ConsMsg( char const  *line ) {
// C compiler call back to do a  console print to stdout

    IDEFN( PrintMessage )( IdeHdl, line );
    // we are ignoring return for now
}

extern void ConBlip( void ) {
// C compiler do a blip to console
}

extern bool ConTTY( void ) {
// C compiler do a blip to console
    return( GlobalCompFlags.ide_console_output != 0 );
}

extern void ConsErrMsg( cmsg_info  *cinfo ) {
// C compiler call back to do a  console print to stderr

    IDEMsgInfo  info;
    IDEMsgSeverity severity;

    severity = 0;
    switch( cinfo->class ) {
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
    if( cinfo->fname != NULL ) {
        IdeMsgSetSrcFile( &info, cinfo->fname );
    }
    if( cinfo->line != 0 ) {
        IdeMsgSetSrcLine( &info, cinfo->line );
    }
    IDEFN( PrintWithInfo )( IdeHdl, &info );
    // we are ignoring return for now
}

extern void ConsErrMsgVerbatim( char const  *line ) {
// C compiler call back to a console print to stderr

    IDEMsgInfo info;

    IdeMsgInit( &info, IDEMSGSEV_ERROR, line );
    IDEFN( PrintWithInfo )( IdeHdl, &info );
    // we are ignoring return for now
}

extern void BannerMsg( char const  *line ) {
// C compiler call back to print a banner type msg

    IDEMsgInfo info;

    IdeMsgInit( &info, IDEMSGSEV_BANNER, line );
    IDEFN( PrintWithInfo )( IdeHdl, &info );
    // we are ignoring return for now
}

extern void DebugMsg( char const  *line ) {
// C compiler call back to print a banner type msg

    IDEMsgInfo info;

    IdeMsgInit( &info, IDEMSGSEV_DEBUG, line );
    IDEFN( PrintWithInfo )( IdeHdl, &info );
    // we are ignoring return for now
}

extern void NoteMsg( char const  *line ) {
// C compiler call back to print a banner type msg

    IDEMsgInfo info;

    IdeMsgInit( &info, IDEMSGSEV_NOTE_MSG, line );
    IDEFN( PrintWithInfo )( IdeHdl, &info );
    // we are ignoring return for now
}

extern const char *FEGetEnv( char const *name ) {
// get enviorment variable
    const char *ret;

    ret = NULL;
    if( !GlobalCompFlags.ignore_environment ) {
        if( IDEFN( GetInfo )( IdeHdl, IDE_GET_ENV_VAR, (IDEGetInfoWParam)name, (IDEGetInfoLParam)&ret ) ) {
            ret = NULL;
        }
    }
    return( ret );
}

static   jmp_buf *FatalEnv;
extern void MyExit( int ret ) {
    longjmp( *FatalEnv, ret );
}

// IDE INTERFACE


unsigned IDEAPI IDEGetVersion ( void ) {
 // GET IDE VERSION
    return IDE_CUR_DLL_VER;
}

#define HEAP_CHK 0

#if HEAP_CHK == 1
static long HeapUsedHi;
static long HeapFreeHi;
#endif

IDEBool IDEAPI IDEInitDLL
    ( IDECBHdl      idehdl          // - handle for this instantiation
    , IDECallBacks  *idecbs         // - call backs into IDE
    , IDEDllHdl     *dllhdl ) {     // - uninitialized info
//***********************
// DLL INITIALIZATION
//***********************
    IdeHdl = idehdl;
    IdeCbs = idecbs;
    *dllhdl = NULL;
#if HEAP_CHK == 1
    HeapUsedHi = 0;
    HeapFreeHi = 0;
#endif
    FrontEndInit( true );
    return false;
}

void IDEAPI IDEFreeHeap( void ) {
//*************************************
//Cleanup Heap
//*************************************
#ifdef __WATCOMC__
   _heapmin();
#endif
}

void IDEAPI IDEFiniDLL( IDEDllHdl hdl ) {
//*********************************************
//DLL COMPLETION
//********************************************
    hdl = hdl;
    FrontEndFini();
}

#if HEAP_CHK == 1
#include <stdio.h>
#if 0
static void heap_dump( void ) {
    struct _heapinfo h_info;
    int heap_status;
    h_info._pentry = NULL;
    for( ; (heap_status = _heapwalk( &h_info )) == _HEAPOK; ) {
        if( h_info._useflag == _USEDENTRY  ) {
            printf( " %s block at %Fp of size %4.4X\n",
                "USED", h_info._pentry, h_info._size );
        }
    }
    switch( heap_status ) {
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

static int heap_size( struct heap_stat *stat ) {
    struct _heapinfo h_info;
    int heap_status;
    h_info._pentry = NULL;
    stat->free = 0;
    stat->used = 0;
    for( ; (heap_status = _heapwalk( &h_info )) == _HEAPOK; ) {
        if( h_info._useflag == _USEDENTRY ) {
            stat->used += h_info._size;
        } else {
            stat->free += h_info._size;
        }
    }
    return( heap_status );
}
#endif

static void getFrontEndArgv( char **argv, char *infile, char *outfile )
{
    if( !GlobalCompFlags.ide_cmd_line_has_files ) {
        infile[0] = '\0';
        IDEFN( GetInfo )( IdeHdl, IDE_GET_SOURCE_FILE, 0, (IDEGetInfoLParam)infile );
        *argv++ = infile;
        outfile[0] = '\0';
        if( !IDEFN( GetInfo )( IdeHdl, IDE_GET_TARGET_FILE, 0, (IDEGetInfoLParam)(outfile + 4) ) ) {
            outfile[0] = '-';
            outfile[1] = 'f';
            outfile[2] = 'o';
            outfile[3] = '=';
        }
        *argv++ = outfile;
        *argv = NULL;
    }
}


IDEBool IDEAPI IDERunYourSelf   // COMPILE A PROGRAM
    ( IDEDllHdl hdl             // - handle for this instantiation
    , const char* opts          // - options
    , IDEBool* fatal_error ) {  // - addr[fatality indication]
    
    //****************************
    // Do a compile of a file
    //****************************
    jmp_buf     env;
    char        infile[_MAX_PATH];      // - input file name
    char        outfile[4 + _MAX_PATH]; // - output file name (need room for "-fo=")
    char        *argv[4];
    int         ret;

    hdl = hdl;
    TBreak();   // clear any pending IDEStopRunning's
    *fatal_error = false;
    FatalEnv = &env;
    /* initialize argv array */
    argv[0] = (char *)opts;
    argv[1] = NULL;
    getFrontEndArgv( argv + 1, infile, outfile );
    if( (ret = setjmp( env )) != 0 ) {  /* if fatal error has occurred */
        *fatal_error = true;
    } else {
        ret = FrontEnd( argv );
    }
#if HEAP_CHK  == 1
    heap_check();
#endif
#ifdef __OS2__
   _heapmin();
#endif
    return( ret != 0 );
}

#ifdef __UNIX__
char **init_argv( char **args, int argc, char *infile, char *outfile )
{
    char    **argv;
    int     count;

    count = 1 + 2;  // terminating NULL + possible input and output file
    if( argc > 1 ) {
        count += argc;
    }
    argv = malloc( count * sizeof( char * ) );
    count = 0;
    if( argc > 1 ) {
        while( count < argc - 1 ) {
            argv[count++] = *(++args);
        }
    }
    argv[count] = NULL;
    getFrontEndArgv( argv + count, infile, outfile );
    return( argv );
}

IDEBool IDEAPI IDERunYourSelfArgv   // COMPILE A PROGRAM
    ( IDEDllHdl hdl,                // - handle for this instantiation
    int argc,                       // - # of arguments
    char **args,                    // - argument vector
    IDEBool* fatal_error )          // - addr[fatality indication]
{
    //****************************
    // Do a compile of a file
    //****************************
    jmp_buf             env;
    char                infile[_MAX_PATH];      // - input file name
    char                outfile[4 + _MAX_PATH]; // - output file name (need room for "-fo=")
    char                **argv;
    int                 ret;

    TBreak();   // clear any pending IDEStopRunning's
    *fatal_error = false;
    FatalEnv = &env;
    /* allocate and initialize argv array */
    argv = init_argv( args, argc, infile, outfile );
    if( (ret = setjmp( env )) != 0 ) {  /* if fatal error has occurred */
        *fatal_error = true;
    } else {
        ret = FrontEnd( argv );
    }
    free( argv );
#if HEAP_CHK  == 1
    heap_check();
#endif
    return( ret != 0 );
}
#endif

void IDEAPI IDEStopRunning( void )
{
    CauseTBreak();
}

#if 0
// HELP Interface

IDEBool IDEAPI IDEProvideHelp   // PROVIDE HELP INFORMATION
    ( IDEDllHdl hdl             // - handle for this instantiation
    , char const *msg )         // - message
{
    hdl = hdl;
    msg = msg;
    return( true );
}
#endif

IDEBool IDEAPI IDEPassInitInfo( IDEDllHdl hdl, IDEInitInfo *info )
{
    hdl = hdl;

//    DbgVerify( hdl == CompInfo.dll_handle, "PassInitInfo -- handle mismatch" );
    if( info->ver < 2 ) {
        return( true );
    }
    if( info->ignore_env ) {
        GlobalCompFlags.ignore_environment = true;
        GlobalCompFlags.ignore_current_dir = true;
    }
    if( info->ver >= 2 ) {
        if( info->cmd_line_has_files ) {
            GlobalCompFlags.ide_cmd_line_has_files = true;
        }
        if( info->ver >= 3 ) {
            if( info->console_output ) {
                GlobalCompFlags.ide_console_output = true;
            }
            if( info->ver >= 4 ) {
                if( info->progress_messages ) {
                    GlobalCompFlags.progress_messages = true;
                }
            }
        }
    }
#if defined( wcc_dll )
//    GlobalCompFlags.dll_active = true;
#endif
    return( false );
}
