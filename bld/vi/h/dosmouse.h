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


#ifndef _DOSMOUSE_INCLUDED
#define _DOSMOUSE_INCLUDED

#define MOUSE_INT                       0x33
#define MOUSE_SCALE                     8

#define INT_MOUSE_MOVEMENT              0x01
#define INT_LEFT_BUTTON_PRESSED         0x02
#define INT_LEFT_BUTTON_RELEASED        0x04
#define INT_RIGHT_BUTTON_PRESSED        0x08
#define INT_RIGHT_BUTTON_RELEASED       0x10
#define INT_ALL_EVENTS                  0x1f
#define INT_ALL_BUTTON_EVENTS           0x1e

#define SOFTWARE_CURSOR                 0x00
#define HARDWARE_CURSOR                 0x01

enum {
    RESET_MOUSE_AND_GET_STATUS = 0,         /* 0x00 */
    SHOW_MOUSE_POINTER,                     /* 0x01 */
    HIDE_MOUSE_POINTER,                     /* 0x02 */
    GET_MOUSE_POSITION_AND_BUTTON_STATUS,   /* 0x03 */
    SET_MOUSE_POINTER_POSITION,             /* 0x04 */
    GET_BUTTON_PRESS_INFORMATION,           /* 0x05 */
    GET_BUTTON_RELEASE_INFORMATION,         /* 0x06 */
    SET_HORIZONTAL_LIMITS_FOR_POINTER,      /* 0x07 */
    SET_VERTICAL_LIMITS_FOR_POINTER,        /* 0x08 */
    SET_GRAPHICS_POINTER_SHAPE,             /* 0x09 */
    SET_TEXT_POINTER_TYPE,                  /* 0x0a */
    READ_MOUSE_MOTION_COUNTERS,             /* 0x0b */
    SET_USER_DEFINED_MOUSE_EVENT_HANDLER,   /* 0x0c */
    TURN_ON_LIGHT_PEN_EMULATION,            /* 0x0d */
    TURN_OFF_LIGHT_PEN_EMULATION,           /* 0x0e */
    SET_MICKEYS_TO_PIXELS_RATIO,            /* 0x0f */
    SET_MOUSE_POINTER_EXCLUSION_AREA,       /* 0x10 */
    SET_DOUBLE_SPEED_THRESHOLD=0x13,        /* 0x13 */
    SWAP_USER_DEFINED_MOUSE_EVENT_HANDLERS, /* 0x14 */
    GET_MOUSE_SAVE_STATE_BUFFER_SIZE,       /* 0x15 */
    SAVE_MOUSE_DRIVE_STATE,                 /* 0x16 */
    RESTORE_MOUSE_DRIVE_STATE,              /* 0x17 */
    SET_ALTERNATE_MOUSE_EVENT_HANDLER,      /* 0x18 */
    GET_ADDRESS_OF_ALTERNATE_MOUSE_EVENT_HANDLER, /* 0x19 */
    SET_MOUSE_SENSITIVITY,                  /* 0x1a */
    GET_MOUSE_SENSITIVITY,                  /* 0x1b */
    SET_MOUSE_INTERRUPT_RATE,               /* 0x1c */
    SELECT_POINTER_PAGE,                    /* 0x1d */
    GET_POINTER_PAGE,                       /* 0x1e */
    DISABLE_MOUSE_DRIVER,                   /* 0x1f */
    ENABLE_MOUSE_DRIVER,                    /* 0x20 */
    RESET_MOUSE_DRIVER,                     /* 0x21 */
    SET_LANGUAGE_FOR_MOUSE_DRIVER_MESSAGES, /* 0x22 */
    GET_LANGUAGE_NUMBER,                    /* 0x23 */
    GET_MOUSE_INFORMATION                   /* 0x24 */
};


extern short MouseFunction( short );
#pragma aux MouseFunction = \
        "int 033h" \
    parm [ax] value [ax] modify [ax bx cx dx];

typedef struct {
    short button_status;
    short x;
    short y;
} mouse_status;

extern void GetMousePositionAndButtonStatus( mouse_status * );
#ifndef __386__
#pragma aux GetMousePositionAndButtonStatus = \
        "mov ax, 03h" \
        "push si" \
        "int 033h" \
        "pop si" \
        "mov word ptr es:[si], bx" \
        "mov word ptr es:[si+2], cx" \
        "mov word ptr es:[si+4], dx" \
    parm [es si] modify [ax bx cx dx];
#else
#pragma aux GetMousePositionAndButtonStatus = \
        "mov ax, 03h" \
        "push esi" \
        "int 033h" \
        "pop esi" \
        "mov word ptr [esi], bx" \
        "mov word ptr 2[esi], cx" \
        "mov word ptr 4[esi], dx" \
    parm [esi] modify [ax bx cx dx];
#endif

extern void SetMousePointerExclusionArea( short, short, short, short );
#pragma aux SetMousePointerExclusionArea = \
        "mov ax, 04h" \
        "int 033h" \
    parm [cx] [dx] [si] [di] modify [ax bx cx dx];

extern void SetMousePointerPosition( short, short );
#pragma aux SetMousePointerPosition = \
        "mov ax, 04h" \
        "int 033h" \
    parm [cx] [dx] modify [ax bx cx dx];

extern void SetHorizontalLimitsForPointer( short, short );
#pragma aux SetHorizontalLimitsForPointer = \
        "mov ax, 07h" \
        "int 33h" \
        parm [cx] [dx] modify [ax bx cx dx];

extern void SetVerticalLimitsForPointer( short, short );
#pragma aux SetVerticalLimitsForPointer = \
        "mov ax, 08h" \
        "int 33h" \
    parm [cx] [dx] modify [ax bx cx dx];

extern void SetTextPointerType( short, short, short );
#pragma aux SetTextPointerType = \
        "mov ax, 0ah" \
        "int 33h" \
    parm [bx] [cx] [dx] modify [ax bx cx dx];

extern void SetUserDefinedMouseEventHandler( short, void far * );
#pragma aux SetUserDefinedMouseEventHandler = \
        "mov ax, 0ch" \
        "int 33h" \
    parm [cx] [es dx] modify [ax bx cx dx];

extern void SetMickeysToPixelsRatio( short, short );
#pragma aux SetMickeysToPixelsRatio = \
        "mov ax, 0fh" \
        "int 33h" \
    parm [cx] [dx] modify [ax bx cx dx];

extern void SetDoubleSpeedThreshold( short );
#pragma aux SetDoubleSpeedThreshold = \
        "mov ax, 13h" \
        "int 33h" \
    parm [dx] modify [ax bx cx dx];

extern void SelectPointerPage( short );
#pragma aux SelectPointerPage = \
        "mov ax, 1dh" \
        "int 33h" \
    parm [bx] modify [ax bx cx dx];

#endif
