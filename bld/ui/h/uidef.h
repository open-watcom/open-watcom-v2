/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Generic ui library macros and prototypes.
*
****************************************************************************/


#ifndef _UIDEF_H_
#define _UIDEF_H_
#include <stdlib.h>
#include "stdui.h"


#define P_UNBUFFERED        0
#define P_DIALOGUE          1
#define P_MENU              2
#define P_VSCREEN           3
#define P_BACKGROUND        4

#define _ESC                "\033"
#define _ESC_CHAR           '\033'

#define CHAR_VALUE(c)       (char)(unsigned char)(c)
#define UCHAR_VALUE(c)      (unsigned char)(c)

typedef enum {
    UI_MOUSE_PRESS          = 1,
    UI_MOUSE_PRESS_RIGHT    = 2,
    UI_MOUSE_PRESS_MIDDLE   = 4,
    UI_MOUSE_PRESS_ANY      = (UI_MOUSE_PRESS | UI_MOUSE_PRESS_RIGHT | UI_MOUSE_PRESS_MIDDLE)
} MOUSESTAT;

#if defined( TSR )

#define _IND( X )           ind_xfer( X )
#define _IND_EVENT( X )     ind_xfer_event( X )
#define _IND_LONG( X )      ind_xfer_long( X )
#define _IND_INT( X )       ind_xfer_int( X )
#define _IND_SAREA( X )     ind_xfer_sarea( X )

void __far *                (*ind_xfer( void (__far *rtn)() ) )();
ui_event                    (*ind_xfer_event( ui_event (__far *rtn)() ) )();
long                        (*ind_xfer_long( long (__far *rtn)() ) )();
int                         (*ind_xfer_int( int (__far *rtn)() ) )();
SAREA                       (*ind_xfer_sarea( SAREA (__far *rtn)() ) )();

#else

#define _IND( X )       (X)
#define _IND_EVENT(X)   (X)
#define _IND_LONG( X )  (X)
#define _IND_INT(X)     (X)
#define _IND_SAREA(X)   (X)

#endif

#define intern          /* internal API */

extern ui_event         Event;

#ifdef __cplusplus
    extern "C" {
#endif

extern bool             intern balloc( BUFFER *, uisize, uisize );
extern void             intern battrflip( BUFFER *, int, int, uisize, uisize );
extern void             intern battribute( BUFFER *, int, int, uisize, uisize, ATTR );
extern void             intern uibcopy( BUFFER *, int, int, BUFFER *, int, int, uisize );
extern void             intern bframe( BUFFER * );
extern void             intern bfree( BUFFER * );
extern void             intern bfake( BUFFER *, int, int );
extern void             intern bfill( BUFFER *, int, int, ATTR, char, uisize );
extern void             intern bpixel( BUFFER *, int, int, ATTR, char );
extern void             intern braw( BUFFER *, int, int, LP_PIXEL, uisize );
extern void             intern bstring( BUFFER *, int, int, ATTR, LPC_STRING, uisize );
extern void             intern bunframe( BUFFER * );
extern int              intern checkkey( void );
extern void             intern checkmouse( MOUSESTAT *, MOUSEORD *, MOUSEORD *, MOUSETIME * );
extern unsigned char    intern checkshift( void );
extern void             intern closebackground( void );
extern void             intern closewindow( UI_WINDOW * );
extern void             intern dirtyarea( UI_WINDOW *, SAREA );
extern void             intern dirtynext( SAREA, UI_WINDOW * );
extern void             intern dividearea( SAREA, SAREA, SAREA * );
extern void             intern drawbox( BUFFER *, SAREA, const char *, ATTR, bool );
extern VSCREEN*         intern findvscreen( ORD, ORD );
extern void             intern finibios( void );
extern void             intern finikeyboard( void );
extern void             intern flushkey( void );
extern void             intern frontwindow( UI_WINDOW * );
extern unsigned         intern getkey( void );
extern bool             intern initbios( void );
extern bool             intern initkeyboard( void );
extern bool             intern isdialogue( VSCREEN * );
extern bool             intern isscreen( BUFFER * );
extern bool             intern issnow( BUFFER * );
extern void             intern kbdspawnstart( void );
extern void             intern kbdspawnend( void );
extern ui_event         intern keyboardevent( void );
extern ui_event         intern menuevent( VSCREEN * );
extern ui_event         intern mouseevent( void );
extern void             intern mouseon( void );
extern void             intern mouseoff( void );
extern void             intern mousespawnstart( void );
extern void             intern mousespawnend( void );
extern void             intern okarea( SAREA );
extern void             intern okline( ORD, ORD, uisize, SAREA );
extern void             intern okopen( VSCREEN * );
extern void             intern oksubarea( SAREA, SAREA );
extern void             intern openbackground( void );
extern bool             intern openwindow( UI_WINDOW * );
extern void             intern movewindow( UI_WINDOW *, ORD, ORD );
extern void             intern physupdate( SAREA * );
#if defined( __UNIX__ )
extern void             intern forbid_refresh( void );
extern void             intern permit_refresh( void );
#else
#define forbid_refresh()
#define permit_refresh()
#endif
extern ui_event         intern saveevent( void );
extern ui_event         intern getprime( VSCREEN * );
extern void             intern setupmouse( void );
extern int              intern videopage( void );
extern void             intern vertretrace( void );
extern ui_event         intern getanyevent( void );
extern void             intern waitforevent( void );
extern void             intern initeventlists( void );

extern void             intern SetCharacterTables( void );
extern int              intern GetNewPos( int pos, int num );

extern VSCREEN          _FARD * intern uiopen( SAREA *, const char *, screen_flags );
extern void             intern uiclose( VSCREEN _FARD * );

extern void             intern newcursor( void );

/*
 * below are OS specific internal shared functions
 */

#if defined( __NETWARE__ )
extern bool             intern kbdisblocked( void );
#endif
#if defined( __DOS__ )
extern bool             intern mouse_installed( void );
#endif

#ifdef __cplusplus
}
#endif

#endif
