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


#include "state.def"

typedef struct state {
    struct state * next;        /* pointer to next state                    */
    HANDLE      appwnd;         /* window handle                            */
    char        sizeinfo;       /* information about sizing direction       */
    STATE_ID    currstate;      /* current state                            */
    STATE_ID    basestate;      /* base state                               */
    OBJPTR      currobj;        /* current object                           */
    OBJPTR      selecteatom;    /* eatom used for banded select             */
    OBJPTR      prevobject;     /* previous object                          */
    POINT       prevmouse;      /* mouse posn at last significant operation */
    OBJPTR      mainobject;     /* the main object                          */
    OBJ_ID      objtype;        /* type of object to create                 */
    unsigned    gridvinc;       /* grid vertical increment value            */
    unsigned    gridhinc;       /* grid horizontal increment value          */
    void *      objects;        /* pointer to object creation table         */
    POINT       offset;         /* Scrolling offset from (0,0)              */
    HANDLE      hEscAccel;      /* Handle to escape accelerator table       */
    HANDLE      hAccel[ACCELS]; /* Handles to accelerator tables            */
    RECT        scrollrect;     /* The rect than can be scrolled within     */
    SCR_CONFIG  scrollconfig;   /* Scrolling attributes from application    */
    WORD        keystate;       /* Was shift pressed on mouse down?         */
    BOOL        showerror;      /* Show error message when we get it?       */
    char *      error;          /* Pending error string                     */
    FARPROC     mouseaction;    /* Rtn to call on significant mouse actions */
    unsigned    vresizegrid;    /* vertical resize grid increment           */
    unsigned    hresizegrid;    /* horizontal resize grid increment         */
    BOOL        show_eatoms;    /* new eatoms will show their movement rect */
    STATE_HDL   id;             /* state id                                 */
};

#define STATE_INDEX 0

#define DEF_V_GRID 5
#define DEF_H_GRID 5
