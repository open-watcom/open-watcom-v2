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


#ifndef _UIDEF_H_
#define _UIDEF_H_
#include <stddef.h>

#ifndef HP
    #ifndef MAXINT
        #define         MAXINT                  0x7fff
    #endif
#endif
#ifndef TRUE
    #define         TRUE                    1
    #define         FALSE                   0
#endif

#define         P_UNBUFFERED            0
#define         P_DIALOGUE              1
#define         P_MENU                  2
#define         P_VSCREEN               3
#define         P_BACKGROUND            4

#define         bool                    int

#ifndef min
    #define         min( a,b )              ( a < b ? a : b )
#endif
#ifndef max
    #define         max( a,b )              ( a > b ? a : b )
#endif

#ifdef _unused
   #undef _unused
#endif
#ifdef UNIX
    #define _unused(var)        var=var
#endif
#ifdef __WATCOMC__
    #define _unused(var)        var=var
#endif
#ifdef __TURBOC__
    #define _unused(var)        var=var
#endif
#ifdef MICROSOFT
    #define _unused(var)        var=var
#endif
#ifdef LATTICE
    #define _unused(var)        var=var
#endif

#if defined( TSR )

    #define _IND( X )           ind_xfer( X )
    #define _IND_EVENT( X )     ind_xfer_event( X )
    #define _IND_LONG( X )      ind_xfer_long( X )
    #define _IND_INT( X )       ind_xfer_int( X )
    #define _IND_SAREA( X )     ind_xfer_sarea( X )

    void far *                  (*ind_xfer( void (far* rtn)() ) )();
    EVENT                       (*ind_xfer_event( EVENT (far* rtn)() ) )();
    long                        (*ind_xfer_long( long (far* rtn)() ) )();
    int                         (*ind_xfer_int( int (far* rtn)() ) )();
    SAREA                       (*ind_xfer_sarea( SAREA (far* rtn)() ) )();

#else

    #define _IND( X )       (X)
    #define _IND_EVENT(X)   (X)
    #define _IND_LONG( X )  (X)
    #define _IND_INT(X)     (X)
    #define _IND_SAREA(X)   (X)

#endif


#include "stdui.h"

#define         global          /* global */
#define         intern          /* near */

#ifdef __cplusplus
    extern "C" {
#endif

int             intern          balloc(struct buffer *,int ,int );
void            intern          battrflip( BUFFER *, int, int, int, int );
void            intern          battribute( BUFFER *, int, int, int, int, ATTR );
void            intern          uibcopy( BUFFER *, int, int, BUFFER *, int, int, int );
void            intern          bframe(struct buffer *);
void            intern          bfree(struct buffer *);
void            intern          bfake( BUFFER *, int, int );
void            intern          bfill( BUFFER *, int, int, ATTR, char, int );
void            intern          blowup( BUFFER *, SAREA, char *, ATTR );
void            intern          bpixel( BUFFER *, ORD, ORD, ATTR, char );
void            intern          braw( BUFFER *, int, int, PIXEL *, int );
void            intern          bstring( BUFFER *, int, int, ATTR, char __FAR *, int );
void            intern          bunframe(struct buffer *);
int             intern          checkkey( void );
void            intern          checkmouse(unsigned short *,unsigned short *,
                                           unsigned short *,unsigned long *);
unsigned char   intern          checkshift( void );
void            intern          closebackground( void );
void            intern          closewindow( UI_WINDOW * );
void            intern          dirtyarea( UI_WINDOW *, SAREA );
void            intern          dirtynext( SAREA, UI_WINDOW * );
void            intern          dividearea( SAREA, SAREA, SAREA * );
void            intern          drawbox( BUFFER *, SAREA, char *, ATTR, int );
LPPIXEL         intern          faralloc( unsigned int );
void            intern          farfree( LPPIXEL );
VSCREEN*        intern          findvscreen( ORD, ORD );
void            intern          finibios( void );
void            intern          finikeyboard( void );
void            intern          flushkey( void );
void            intern          frontwindow( UI_WINDOW * );
unsigned int    intern          getkey( void );
int             intern          initbios( void );
int             intern          initkeyboard( void );
int             intern          initmonitor( void );
int             intern          installed( int );
int             intern          isdialogue( VSCREEN * );
int             intern          isscreen( BUFFER * );
int             intern          issnow( BUFFER * );
void            intern          kbdspawnstart();
void            intern          kbdspawnend();
EVENT           intern          keyboardevent( void );
void            intern          menuevent( VSCREEN * );
EVENT           intern          mouseevent( void );
void            intern          mouseon( void );
void            intern          mouseoff( void );
void            intern          mousespawnstart();
void            intern          mousespawnend();
void            intern          okarea( SAREA );
void            intern          okline( ORD, ORD, int, SAREA );
void            intern          okopen( VSCREEN * );
void            intern          oksubarea( SAREA, SAREA );
void            intern          openbackground( void );
int             intern          openwindow( UI_WINDOW * );
void            intern          movewindow( UI_WINDOW *, ORD, ORD );
void            intern          physupdate( SAREA * );
#if defined( __QNX__ ) || defined( UNIX )
void            intern          forbid_refresh( void );
void            intern          permit_refresh( void );
#else
    #define                     forbid_refresh()
    #define                     permit_refresh()
#endif
EVENT           intern          saveevent( void );
EVENT           intern          getprime( VSCREEN * );
void            intern          setupmouse( void );
void            intern          setvideomode( unsigned );
int             intern          videopage( void );
void            intern          vertretrace( void );
void            global          uistartevent( void );
void            global          uidoneevent( void );

#ifdef __cplusplus
}
#endif

#endif
