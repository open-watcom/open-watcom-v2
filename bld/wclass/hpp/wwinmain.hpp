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


#ifndef wwinmain_module
#define wwinmain_module

#include "_windows.hpp"
/*
#pragma library
#pragma library (wclass)
*/
typedef enum {
    COORD_USER,
    COORD_SCREEN,
    COORD_VIRTUAL
} coordinate_system;

extern  void            wclassmain( void );
extern  void            InitWClass( gui_rect * );
extern  void            InitGUI( coordinate_system, gui_ord, gui_ord, gui_rect * );


#define MAINOBJECT( a, c, w, h )        \
                                        \
    static a *AppInstance;              \
                                        \
    void wclassmain() {                 \
    /************/                      \
                                        \
        gui_rect        screen;         \
                                        \
        InitGUI( c, w, h, &screen );    \
        InitWClass( &screen );          \
        AppInstance = new a;            \
    }                                   \
                                        \
    extern "C" int GUIDeath() {         \
    /*************************/         \
                                        \
        delete AppInstance;             \
        return( TRUE );                 \
    }
#endif
