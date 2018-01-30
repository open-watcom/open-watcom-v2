/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2018 The Open Watcom Contributors. All Rights Reserved.
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

#if defined( __WATCOMC__ )
  #define cdecl __cdecl
#else
  #define cdecl
#endif

#define P_UNBUFFERED        0
#define P_DIALOGUE          1
#define P_MENU              2
#define P_VSCREEN           3
#define P_BACKGROUND        4

#define _ESC                "\033"
#define _ESC_CHAR           '\033'

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

#define         UIAPI
#define         intern          /* near */

#ifdef __cplusplus
    extern "C" {
#endif

bool            intern          balloc( BUFFER *, ORD, ORD );
void            intern          battrflip( BUFFER *, int, int, int, int );
void            intern          battribute( BUFFER *, int, int, int, int, ATTR );
void            intern          uibcopy( BUFFER *, int, int, BUFFER *, int, int, int );
void            intern          bframe( struct buffer * );
void            intern          bfree( struct buffer * );
void            intern          bfake( BUFFER *, ORD, ORD );
void            intern          bfill( BUFFER *, int, int, ATTR, unsigned char, int );
void            intern          blowup( BUFFER *, SAREA, unsigned char *, ATTR );
void            intern          bpixel( BUFFER *, ORD, ORD, ATTR, unsigned char );
void            intern          braw( BUFFER *, int, int, PIXEL *, int );
void            intern          bstring( BUFFER *, int, int, ATTR, LPC_STRING, int );
void            intern          bunframe( struct buffer * );
int             intern          checkkey( void );
void            intern          checkmouse( MOUSESTAT *, MOUSEORD *, MOUSEORD *, MOUSETIME * );
unsigned char   intern          checkshift( void );
void            intern          closebackground( void );
void            intern          closewindow( UI_WINDOW * );
void            intern          dirtyarea( UI_WINDOW *, SAREA );
void            intern          dirtynext( SAREA, UI_WINDOW * );
void            intern          dividearea( SAREA, SAREA, SAREA * );
void            intern          drawbox( BUFFER *, SAREA, unsigned char *, ATTR, int );
VSCREEN*        intern          findvscreen( ORD, ORD );
void            intern          finibios( void );
void            intern          finikeyboard( void );
void            intern          flushkey( void );
void            intern          frontwindow( UI_WINDOW * );
unsigned int    intern          getkey( void );
bool            intern          initbios( void );
bool            intern          initkeyboard( void );
bool            intern          isdialogue( VSCREEN * );
bool            intern          isscreen( BUFFER * );
bool            intern          issnow( BUFFER * );
void            intern          kbdspawnstart( void );
void            intern          kbdspawnend( void );
ui_event        intern          keyboardevent( void );
ui_event        intern          menuevent( VSCREEN * );
ui_event        intern          mouseevent( void );
void            intern          mouseon( void );
void            intern          mouseoff( void );
void            intern          mousespawnstart( void );
void            intern          mousespawnend( void );
void            intern          okarea( SAREA );
void            intern          okline( ORD, ORD, int, SAREA );
void            intern          okopen( VSCREEN * );
void            intern          oksubarea( SAREA, SAREA );
void            intern          openbackground( void );
bool            intern          openwindow( UI_WINDOW * );
void            intern          movewindow( UI_WINDOW *, ORD, ORD );
void            intern          physupdate( SAREA * );
#if defined( __UNIX__ )
void            intern          forbid_refresh( void );
void            intern          permit_refresh( void );
#else
    #define                     forbid_refresh()
    #define                     permit_refresh()
#endif
ui_event        intern          saveevent( void );
ui_event        intern          getprime( VSCREEN * );
void            intern          setupmouse( void );
void            intern          setvideomode( unsigned );
int             intern          videopage( void );
void            intern          vertretrace( void );
ui_event        intern          getanyevent( void );
void            intern          waitforevent( void );
void            intern          initeventlists( void );

/*
 * below are OS specific internal shared functions
 */

#if defined( __UNIX__ )
void            intern          newcursor( void );
#endif
#if defined( __NETWARE__ )
bool            intern          kbdisblocked( void );
#endif
#if defined( __DOS__ )
bool            intern          mouse_installed( void );
#endif

#ifdef __cplusplus
}
#endif

#endif
