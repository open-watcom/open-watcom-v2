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


//
// EPMSPY       : DDE spy for WorkFrame/2 and EPM communication
//

#include <process.h>
#include <string.h>
#include <malloc.h>
#include <stdio.h>
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define INCL_WINDDE
#define INCL_WINWINDOWMGR
#include <os2.h>

typedef struct errstr {
    ULONG       errorline;
    ULONG       offset;
    ULONG       length;
    ULONG       firstline;
} errstr;

typedef struct initiate_data {
    ULONG       errorcount;
    errstr      errors[1];
    ULONG       liblength;
    char        libname[1];
} initiate_data;

typedef struct goto_data {
    ULONG       errorline;
    ULONG       offset;
    ULONG       resourceid;
    ULONG       firstline;
    ULONG       textlength;
    char        errortext[17];
} goto_data;

static CONVCONTEXT ConvContext = { sizeof( CONVCONTEXT ), 0 };

static char             *fileName;
static PFNWP            prevClientProc;
static HWND             hwndViper;      // Viper's window handle
static volatile HWND    hwndWF;         // WorkFrame's window handle
static HWND             hwndDDE;        // window handle for DDE session

static  int ddeFile;
static  char    Buffer[512];

#define _Editor         "EPM.EXE"


MRESULT EXPENTRY clientProc( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 ) {
/****************************************************************************/

    switch( msg ) {
    case WM_DDE_INITIATE: {
        DDEINIT *ddei = (PDDEINIT)mp2;
        write( ddeFile, "WM_DDE_INITIATE ", 16 );
        if( (strcmp( "WB Editor", (char *)ddei->pszAppName ) == 0) &&
            (strcmp( fileName, (char *)ddei->pszTopic ) == 0) ) {
            write( ddeFile, fileName, strlen( fileName ) );
            hwndWF = (HWND)mp1;
            WinDdeRespond( hwndWF, hwndDDE, "WB Editor", fileName, &ConvContext );
        }
        write( ddeFile, "\n", 1 );
        DosFreeMem( ddei );
        return( (MRESULT)TRUE );
    } case WM_DDE_INITIATEACK: {
        DDEINIT *ddei = (PDDEINIT)mp2;
        write( ddeFile, "WM_DDE_INITIATEACK\n", 18 );
        DosFreeMem( ddei );
        return( (MRESULT)TRUE );
    } case WM_DDE_DATA: {
        DDEINIT *ddei = (PDDEINIT)mp2;
        write( ddeFile, "WM_DDE_DATA\n", 12 );
        DosFreeMem( ddei );
        return( (MRESULT)TRUE );
    } case WM_DDE_UNADVISE: {
        DDEINIT *ddei = (PDDEINIT)mp2;
        write( ddeFile, "WM_DDE_UNADVISE\n", 16 );
        DosFreeMem( ddei );
        return( (MRESULT)TRUE );
    } case WM_DDE_POKE: {
        DDEINIT *ddei = (PDDEINIT)mp2;
        write( ddeFile, "WM_DDE_POKE\n", 12 );
        DosFreeMem( ddei );
        return( (MRESULT)TRUE );
    } case WM_DDE_ADVISE: {
        DDEINIT *ddei = (PDDEINIT)mp2;
        write( ddeFile, "WM_DDE_ADVISE\n", 14 );
        DosFreeMem( ddei );
        return( (MRESULT)TRUE );
    } case WM_DDE_REQUEST: {
        DDEINIT *ddei = (PDDEINIT)mp2;
        write( ddeFile, "WM_DDE_REQUEST\n", 15 );
        DosFreeMem( ddei );
        return( (MRESULT)TRUE );
    } case WM_DDE_EXECUTE: {
        DDESTRUCT *ddes = (PDDESTRUCT)mp2;
        write( ddeFile, "WM_DDE_EXECUTE ", 15 );
        write( ddeFile, DDES_PSZITEMNAME( ddes ),
               strlen( DDES_PSZITEMNAME( ddes ) ) );
        write( ddeFile, "\n", 1 );
        if( strcmp( DDES_PSZITEMNAME( ddes ), "Initialize" ) == 0 ) {
            initiate_data       *p;
            char                *q;
            ULONG               i;
            p = (initiate_data *)DDES_PABDATA( ddes );
            sprintf( Buffer, "errorcount = %d \n", p->errorcount );
            write( ddeFile, Buffer, strlen( Buffer ) );
            for( i = 0; i < p->errorcount; ++i ) {
                sprintf( Buffer, "line: %d, offset: %d, length: %d, magic: %d\n",
                         p->errors[i].errorline, p->errors[i].offset,
                         p->errors[i].length, p->errors[i].firstline );
                write( ddeFile, Buffer, strlen( Buffer ) );
            }
            q = (char *)&p->errors[p->errorcount];
            i = *(ULONG *)q;
            sprintf( Buffer, "liblength = %d\n", i );
            write( ddeFile, Buffer, strlen( Buffer ) );
            q += sizeof( ULONG );
            write( ddeFile, q, i );
            write( ddeFile, "\n", 1 );
            WinDdePostMsg( hwndWF, hwndDDE, WM_DDE_ACK, ddes, DDEPM_RETRY );
        } else if( strcmp( DDES_PSZITEMNAME( ddes ), "Goto" ) == 0 ) {
            goto_data *p;
            p = (goto_data *)DDES_PABDATA( ddes );
            sprintf( Buffer, "errorline = %d \n", p->errorline );
            write( ddeFile, Buffer, strlen( Buffer ) );
            sprintf( Buffer, "offset = %d \n", p->offset );
            write( ddeFile, Buffer, strlen( Buffer ) );
            sprintf( Buffer, "resourceid = %d \n", p->resourceid );
            write( ddeFile, Buffer, strlen( Buffer ) );
            sprintf( Buffer, "magic = %d \n", p->firstline );
            write( ddeFile, Buffer, strlen( Buffer ) );
            sprintf( Buffer, "textlength = %d \n", p->textlength );
            write( ddeFile, Buffer, strlen( Buffer ) );
            write( ddeFile, p->errortext, p->textlength );
            write( ddeFile, "\n", 1 );
            WinDdePostMsg( hwndWF, hwndDDE, WM_DDE_ACK, ddes, DDEPM_RETRY );
        }
        break;
    } case WM_DDE_ACK: {
        DDESTRUCT *ddes = (PDDESTRUCT)mp2;
        write( ddeFile, "WM_DDE_ACK ", 11 );
        write( ddeFile, "\n", 1 );
        DosFreeMem( ddes );
        break;
    } case WM_DDE_TERMINATE: {
        WinDdePostMsg( hwndWF, hwndDDE, WM_DDE_TERMINATE, NULL, 0 );
        write( ddeFile, "WM_DDE_TERMINATE ", 17 );
        write( ddeFile, "\n", 1 );
        break;
    } default:
        return( prevClientProc( hwnd, msg, mp1, mp2 ) );
    }
    return( 0 );
}


BOOL __syscall InitDDE( HWND hwndCaller ) {
/*****************************************/

    ULONG       style = 0;

    hwndViper = hwndCaller;
    hwndWF = NULLHANDLE;
    hwndDDE = WinCreateStdWindow( HWND_DESKTOP, 0, &style, WC_FRAME,
                                       NULL, 0, NULLHANDLE, 0, NULL );
    if( hwndDDE == NULLHANDLE ) {
        return( FALSE );
    }
    prevClientProc = WinSubclassWindow( hwndDDE, (PFNWP)clientProc );
    return( TRUE );
}

static void Connect() {
/*********************/

    WinDdeInitiate( hwndDDE, "WB Editor", fileName, &ConvContext );
}


MRESULT EXPENTRY MainDriver( HWND hwnd, USHORT msg, MPARAM mp1, MPARAM mp2 ) {
//============================================================================

    HPS         ps;
    RECTL       rcl;

    switch( msg ) {
    case WM_CREATE:
        InitDDE( hwnd );
        break;
    case WM_PAINT:
        Connect();
        ps = WinBeginPaint( hwnd, NULL, NULL );
        WinQueryWindowRect( hwnd, &rcl );
        WinFillRect( ps, &rcl, CLR_WHITE );
        WinEndPaint( ps );
        return( 0 );
    }
    return( WinDefWindowProc( hwnd, msg, mp1, mp2 ) );
}


int     main( int argc, char *argv[] ) {
/**************************************/

    ULONG       style;
    QMSG        qmsg;
    HWND        FrameHandle;
    HWND        WinHandle;
    HMQ         hMessageQueue;
    HAB         AnchorBlock;
    int         i;

    AnchorBlock = WinInitialize( 0 );
    if( AnchorBlock == 0 ) return( 0 );
    hMessageQueue = WinCreateMsgQueue( AnchorBlock, 0 );
    if( hMessageQueue == 0 ) return( 0 );
    if( !WinRegisterClass( AnchorBlock, "WATCOM", (PFNWP)MainDriver,
                           CS_SIZEREDRAW, 0 ) ) {
        return( 0 );
    }
    style = FCF_TITLEBAR | FCF_SYSMENU | FCF_SIZEBORDER | FCF_MINMAX |
            FCF_SHELLPOSITION | FCF_TASKLIST;
    FrameHandle = WinCreateStdWindow( HWND_DESKTOP, WS_VISIBLE, &style,
                                      "WATCOM", "", 0, NULL, 0,
                                      &WinHandle );
    if( FrameHandle == 0 ) return( 0 );
    ddeFile = open( "d:\\editdll\\epm\\dde.out", O_WRONLY | O_TEXT | O_CREAT | O_TRUNC );
    fileName = argv[2];
    for( i = 0; i < argc; ++i ) {
        write( ddeFile, argv[i], strlen( argv[i] ) );
        write( ddeFile, "\n", 1 );
    }

    while( WinGetMsg( AnchorBlock, &qmsg, NULL, 0, 0 ) ) {
        WinDispatchMsg( AnchorBlock, &qmsg );
    }
    close( ddeFile );

    WinDestroyWindow( FrameHandle );
    WinDestroyMsgQueue( hMessageQueue );
    WinTerminate( AnchorBlock );

    return( 1 );
}
