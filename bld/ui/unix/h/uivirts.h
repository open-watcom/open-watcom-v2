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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


/*-

 uivirts.h -- interface class for display, mouse, keyboard.
        Note the separation of these is notational convenience
        It is unlikely you will ever want an X keyboard with
        a QNX Console.....

*/

#ifndef _UIVIRTS_H_INCLUDED
#define _UIVIRTS_H_INCLUDED


typedef struct Display {
    bool        (*init)( void );            /* setup */
    bool        (*fini)( void );            /* tear down */
    int         (*update)(SAREA *area);     /* change screen */
    int         (*refresh)(bool noopt);     /* force redraw of screen */
    /*- cursor */
    int         UIHOOK (*getcur)(CURSORORD *crow, CURSORORD *ccol, CURSOR_TYPE *ctype, CATTR *cattr);
    int         UIHOOK (*setcur)(CURSORORD crow, CURSORORD ccol, CURSOR_TYPE ctype, CATTR cattr);
    ui_event    (*event)( void );
} Display;


typedef struct Keyboard {
    bool    (*init)( void );                /* set initial modes, etc... */
    bool    (*fini)( void );                /* restore saved modes, etc... */
    void    (*arm)( void );                 /* arm for next character */
    int     (*save)( void );                /* save current mode, restore original mode */
    int     (*restore)( void );             /* set into raw mode */
    int     (*flush)( void );               /* clear look-ahead */
    int     (*stop)( void );                /* clear look-ahead, disable keyboard events */
    int     (*shift_state)( void );         /* shift status */
    int     (*un_event)(ui_event event);    /* allow modify of next event */
    int     (*wait_keyb)( int, int );       /* wait for keyboard event */
} Keyboard;

typedef struct Mouse {
    bool    (*init)( init_mode install );
    bool    (*fini)( void );
    int     (*set_speed)( unsigned speed );
    int     (*stop)( void );            /* clear input, disable events */
    bool    (*check)( MOUSESTAT *status, MOUSEORD *row, MOUSEORD *col, MOUSETIME *time );
    int     (*wait_mouse)( void );      /* wait for mouse event */
} Mouse;

typedef struct {
    Display     *disp;
    Keyboard    *keyb;
    Mouse       *mouse;
} VirtDisplay;

typedef struct {
    bool        (*check)(void);
    VirtDisplay virt;
} PossibleDisplay;

extern VirtDisplay      UIVirt;

#define UIVIRTS

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
#define _uiwaitkeyb     (*UIVirt.keyb->wait_keyb)

#define _initmouse      (*UIVirt.mouse->init)
#define _finimouse      (*UIVirt.mouse->fini)
#define _checkmouse     (*UIVirt.mouse->check)
#define _stopmouse      (*UIVirt.mouse->stop)
#define _uimousespeed   (*UIVirt.mouse->set_speed)
#define _uiwaitmouse    (*UIVirt.mouse->wait_mouse)

#endif
