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


#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include "packet.h"
#include "winserv.h"
#include "trperr.h"

extern unsigned NumPrinters( void );
extern void ServMessage( char * );
extern void ServError( char * );

int CurrentPort = -1;    /* ports 0,1,2 */

extern HANDLE CProcHandle;
extern HWND MainWindowHandle;
extern HWND DebugWindow;
extern BOOL RemoteLinkObtained;

HWND PortChildren[MAX_PPORTS];

char _pgmname[]=TRP_Parallel_Name;


void SetAppl( void )
{
    SendMessage( PortChildren[ CurrentPort], BM_SETCHECK, 1, 0L );
} /* SetAppl */


/*
 * CreateSelections
 */
BOOL CreateSelections( short x, short y, short avgx, short avgy )
{
    HWND win;
    char caption[80];
    int i;
    int xp,yp,xs,ys;

    y = y;
    for( i = 0; i < NumPrinters(); i++ ) {
        sprintf( caption, TRP_WIN_parallel_port, i + 1 );
        xp = 15 + 3 * ( x / 4 ) + 10;
        yp = 20 + i * ( avgy + 1 );
        ys = avgy;
        xs = ( strlen( caption ) + 1 ) * avgx;

        win = CreateWindow(
            "BUTTON",               /* class */
            caption,                /* caption */
            WS_CHILD | BS_RADIOBUTTON,
            xp,                     /* init. x pos */
            yp,                     /* init. y pos */
            xs,                     /* init. x size */
            ys,                     /* init. y size */
            MainWindowHandle,       /* parent window */
            RADIO_1 + i,            /* child id */
            CProcHandle,            /* program handle */
            NULL                    /* create parms */
            );

            if( win == NULL )
                return( FALSE );
            PortChildren[i] = win;
            ShowWindow( win, SW_SHOWNORMAL );

      } /* for */
      return( TRUE );

} /* CreateSelections */

/*
 * ProcAppl - receives server specific messages for the main window
 */
void ProcAppl( HWND windhandle, unsigned message, WORD worddata, LONG longdata )
{
    WORD notify;
    char buff[80], *err;

    message = message;
    switch( worddata ) {
    case RADIO_1:
    case RADIO_2:
    case RADIO_3:
        notify = HIWORD( longdata );
        if( notify == BN_CLICKED ) {
            SendMessage( PortChildren[CurrentPort], BM_SETCHECK, 0, 0L );
            CurrentPort = worddata - RADIO_1;
            if( RemoteLinkObtained )
                RemoteUnLink();
            sprintf( buff, "%d", CurrentPort + 1 );
            err = RemoteLink( buff, TRUE );
            if( err != NULL ) {
                ServError( err );
                RemoteLinkObtained = FALSE;
            } else {
                sprintf( buff, TRP_WIN_port_set, CurrentPort + 1 );
                ServMessage( buff );
                SendMessage( PortChildren[CurrentPort], BM_SETCHECK, 1, 0L );
                RemoteLinkObtained = TRUE;
            } /* if */
        } /* if */
        break;
    } /* switch */

} /* ProcAppl */
