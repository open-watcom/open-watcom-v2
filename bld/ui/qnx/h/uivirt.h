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


/*-

 uivirt.h -- interface class for display, mouse, keyboard.
        Note the separation of these is notational convenience
        It is unlikely you will ever want an X keyboard with
        a QNX Console.....

*/

#ifndef qnx_uivirt_h
#define qnx_uivirt_h


typedef struct Display {
        int   (*init)( void );          /* setup */
        int   (*fini)( void );          /* tear down */
        int   (*update)( SAREA *area ); /* change screen */
        int   (*refresh)( int noopt );  /* force redraw of screen */
        /*- cursor */
        int   (*getcur)( ORD *row, ORD *col, int *type, int *attr );
        int   (*setcur)( ORD row, ORD col, int type, int attr );
        EVENT (*event)( void );
} Display;


typedef struct Keyboard {
        int  (*init)( void );       /* set initial modes, etc... */
        int  (*fini)( void );       /* restore saved modes, etc... */
        void (*arm)( void );        /* arm for next character */
        int  (*save)( void );       /* save current mode, restore original mode */
        int  (*restore)( void );    /* set into raw mode */
        int  (*flush)( void );      /* clear look-ahead */
        int  (*stop)( void );       /* clear look-ahead, disable keyboard events */
        int  (*shift_state)( void );/* shift status */
        int  (*un_event)( EVENT event ); /* allow modify of next event */
} Keyboard;

typedef struct Mouse {
        int (*init)( int install );
        int (*fini)( void );
        int (*set_speed)( int speed );
        int (*stop)( void );        /* clear input, disable events */
        int (*check)( unsigned short *status, unsigned short *row,
                         unsigned short *col, unsigned long *time );
} Mouse;

typedef struct {
    Display     *disp;
    Keyboard    *keyb;
    Mouse       *mouse;
} VirtDisplay;

typedef struct {
    bool        (*check)( void );
    VirtDisplay virt;
} PossibleDisplay;

extern VirtDisplay      UIVirt;

/*-
 * convenient naming, and easier changes later...
 */

#define _uibiosinit     (*UIVirt.disp->init)
#define _uibiosfini     (*UIVirt.disp->fini)
#define _physupdate     (*UIVirt.disp->update)
#define _ui_refresh     (*UIVirt.disp->refresh)
#define _uigetcursor    (*UIVirt.disp->getcur)
#define _uisetcursor    (*UIVirt.disp->setcur)
#define _uievent        (*UIVirt.disp->event)

#define _initkeyboard   (*UIVirt.keyb->init)
#define _finikeyboard   (*UIVirt.keyb->fini)
#define _armkeyboard    (*UIVirt.keyb->arm)
#define _savekeyb       (*UIVirt.keyb->save)
#define _restorekeyb    (*UIVirt.keyb->restore)
#define _flushkey       (*UIVirt.keyb->flush)
#define _stopkeyb       (*UIVirt.keyb->stop)
#define _uicheckshift   (*UIVirt.keyb->shift_state)
#define _checkshift     _uicheckshift
#define _uishiftrelease (*UIVirt.keyb->un_event)

#define _initmouse      (*UIVirt.mouse->init)
#define _finimouse      (*UIVirt.mouse->fini)
#define _checkmouse     (*UIVirt.mouse->check)
#define _stopmouse      (*UIVirt.mouse->stop)
#define _uimousespeed   (*UIVirt.mouse->set_speed)

#endif
