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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#ifndef _INT33_H_INCLUDED
#define _INT33_H_INCLUDED

#if defined( __DOS__ ) || defined( __WINDOWS__ ) || defined( _M_I86 ) && defined( __OS2__ )

#define STR(...)                    #__VA_ARGS__
#define INSTR(...)                  STR(__VA_ARGS__)

#define VECTOR_MOUSE                0x33

#define _INT_33                     0xcd VECTOR_MOUSE

#define _INT_33_FN(n)               INSTR( mov ax, n ) _INT_33

#define MOUSE_DRIVER_OK             ((unsigned short)-1)

#define MOUSE_SCALE                 8

#define INT_MOUSE_MOVEMENT          0x01
#define INT_LEFT_BUTTON_PRESSED     0x02
#define INT_LEFT_BUTTON_RELEASED    0x04
#define INT_RIGHT_BUTTON_PRESSED    0x08
#define INT_RIGHT_BUTTON_RELEASED   0x10
#define INT_ALL_EVENTS              0x1f
#define INT_ALL_BUTTON_EVENTS       0x1e

#define SOFTWARE_CURSOR             0x00
#define HARDWARE_CURSOR             0x01

enum {
    RESET_MOUSE_AND_GET_STATUS = 0,                 /* 0x00 */
    SHOW_MOUSE_POINTER,                             /* 0x01 */
    HIDE_MOUSE_POINTER,                             /* 0x02 */
    GET_MOUSE_POSITION_AND_BUTTON_STATUS,           /* 0x03 */
    SET_MOUSE_POINTER_POSITION,                     /* 0x04 */
    GET_BUTTON_PRESS_INFORMATION,                   /* 0x05 */
    GET_BUTTON_RELEASE_INFORMATION,                 /* 0x06 */
    SET_HORIZONTAL_LIMITS_FOR_POINTER,              /* 0x07 */
    SET_VERTICAL_LIMITS_FOR_POINTER,                /* 0x08 */
    SET_GRAPHICS_POINTER_SHAPE,                     /* 0x09 */
    SET_TEXT_POINTER_TYPE,                          /* 0x0a */
    READ_MOUSE_MOTION_COUNTERS,                     /* 0x0b */
    SET_USER_DEFINED_MOUSE_EVENT_HANDLER,           /* 0x0c */
    TURN_ON_LIGHT_PEN_EMULATION,                    /* 0x0d */
    TURN_OFF_LIGHT_PEN_EMULATION,                   /* 0x0e */
    SET_MICKEYS_TO_PIXELS_RATIO,                    /* 0x0f */
    SET_MOUSE_POINTER_EXCLUSION_AREA,               /* 0x10 */
    FUNCTION_11,                                    /* 0x11 */
    FUNCTION_12,                                    /* 0x12 */
    SET_DOUBLE_SPEED_THRESHOLD,                     /* 0x13 */
    SWAP_USER_DEFINED_MOUSE_EVENT_HANDLERS,         /* 0x14 */
    GET_MOUSE_SAVE_STATE_BUFFER_SIZE,               /* 0x15 */
    SAVE_MOUSE_DRIVE_STATE,                         /* 0x16 */
    RESTORE_MOUSE_DRIVE_STATE,                      /* 0x17 */
    SET_ALTERNATE_MOUSE_EVENT_HANDLER,              /* 0x18 */
    GET_ADDRESS_OF_ALTERNATE_MOUSE_EVENT_HANDLER,   /* 0x19 */
    SET_MOUSE_SENSITIVITY,                          /* 0x1a */
    GET_MOUSE_SENSITIVITY,                          /* 0x1b */
    SET_MOUSE_INTERRUPT_RATE,                       /* 0x1c */
    SELECT_POINTER_PAGE,                            /* 0x1d */
    GET_POINTER_PAGE,                               /* 0x1e */
    DISABLE_MOUSE_DRIVER,                           /* 0x1f */
    ENABLE_MOUSE_DRIVER,                            /* 0x20 */
    RESET_MOUSE_DRIVER,                             /* 0x21 */
    SET_LANGUAGE_FOR_MOUSE_DRIVER_MESSAGES,         /* 0x22 */
    GET_LANGUAGE_NUMBER,                            /* 0x23 */
    GET_MOUSE_INFORMATION                           /* 0x24 */
};


typedef struct {
    short button_status;
    short x;
    short y;
} mouse_status;

extern unsigned short _BIOSMouseDriverReset( void );
#pragma aux _BIOSMouseDriverReset = \
        _INT_33_FN( 0 )     \
    __parm              [] \
    __value             [__ax] \
    __modify __exact    [__ax __bx]

extern void _BIOSMouseGetPositionAndButtonStatus( mouse_status * );
#ifdef _M_I86
#pragma aux _BIOSMouseGetPositionAndButtonStatus = \
        "push si"   \
        _INT_33_FN( 3 ) \
        "pop  si"   \
        "mov  word ptr es:[si],bx"   \
        "mov  word ptr es:[si+2],cx" \
        "mov  word ptr es:[si+4],dx" \
    __parm              [__es __si] \
    __value             \
    __modify __exact    [__ax __bx __cx __dx]
#else
#pragma aux _BIOSMouseGetPositionAndButtonStatus = \
        "push esi"  \
        _INT_33_FN( 3 ) \
        "pop  esi"  \
        "mov  word ptr es:[esi],bx"   \
        "mov  word ptr es:[esi+2],cx" \
        "mov  word ptr es:[esi+4],dx" \
    __parm              [__es __esi] \
    __value             \
    __modify __exact    [__ax __bx __cx __dx]
#endif

extern void _BIOSMouseGetPositionAndButtonStatusReset( void );
#ifdef _M_I86
#pragma aux _BIOSMouseGetPositionAndButtonStatusReset = \
        _INT_33_FN( 3 ) \
    __parm              [] \
    __value             \
    __modify __exact    [__ax __bx __cx __dx]
#else
#pragma aux _BIOSMouseGetPositionAndButtonStatusReset = \
        _INT_33_FN( 3 ) \
    __parm              [] \
    __value             \
    __modify __exact    [__ax __bx __cx __dx]
#endif

extern void _BIOSMouseSetPointerPosition( short col, short row );
#pragma aux _BIOSMouseSetPointerPosition = \
        _INT_33_FN( 4 ) \
    __parm              [__cx] [__dx] \
    __value             \
    __modify __exact    [__ax]

extern void _BIOSMouseSetHorizontalLimitsForPointer( short min, short max );
#pragma aux _BIOSMouseSetHorizontalLimitsForPointer = \
        _INT_33_FN( 7 ) \
    __parm              [__cx] [__dx] \
    __value             \
    __modify __exact    [__ax]

extern void _BIOSMouseSetVerticalLimitsForPointer( short min, short max );
#pragma aux _BIOSMouseSetVerticalLimitsForPointer = \
        _INT_33_FN( 8 ) \
    __parm              [__cx] [__dx] \
    __value             \
    __modify __exact    [__ax]

extern void _BIOSMouseSetTextPointerType( short func, short p1, short p2 );
#pragma aux _BIOSMouseSetTextPointerType = \
        _INT_33_FN( 0x0a ) \
    __parm              [__bx] [__cx] [__dx] \
    __value             \
    __modify __exact    [__ax]

extern void _BIOSMouseGetMotionCounters( mouse_status * );
#ifdef _M_I86
#pragma aux _BIOSMouseGetMotionCounters = \
        "push si"   \
        _INT_33_FN( 0x0b ) \
        "pop  si"   \
        "mov  word ptr es:[si+2],cx"   \
        "mov  word ptr es:[si+4],dx" \
    __parm              [__es __si] \
    __value             \
    __modify __exact    [__ax __cx __dx]
#else
#pragma aux _BIOSMouseGetMotionCounters = \
        "push esi"  \
        _INT_33_FN( 0x0b ) \
        "pop  esi"  \
        "mov  word ptr es:[esi+2],cx"   \
        "mov  word ptr es:[esi+4],dx" \
    __parm              [__es __esi] \
    __value             \
    __modify __exact    [__ax __cx __dx]
#endif

extern void _BIOSMouseGetMotionCountersReset( void );
#ifdef _M_I86
#pragma aux _BIOSMouseGetMotionCountersReset = \
        _INT_33_FN( 0x0b ) \
    __parm              [] \
    __value             \
    __modify __exact    [__ax __cx __dx]
#else
#pragma aux _BIOSMouseGetMotionCountersReset = \
        _INT_33_FN( 0x0b ) \
    __parm              [] \
    __value             \
    __modify __exact    [__ax __cx __dx]
#endif

extern void _BIOSMouseSetUserDefinedMouseEventHandler( short mask, void __far *func );
#ifdef _M_I86
#pragma aux _BIOSMouseSetUserDefinedMouseEventHandler = \
        _INT_33_FN( 0x0c ) \
    __parm              [__cx] [__es __dx] \
    __value             \
    __modify __exact    [__ax]
#else
#pragma aux _BIOSMouseSetUserDefinedMouseEventHandler = \
        _INT_33_FN( 0x0c ) \
    __parm              [__cx] [__es __edx] \
    __value             \
    __modify __exact    [__ax]
#endif

extern void _BIOSMouseSetMickeysToPixelsRatio( short x, short y );
#pragma aux _BIOSMouseSetMickeysToPixelsRatio = \
        _INT_33_FN( 0x0f ) \
    __parm              [__cx] [__dx] \
    __value             \
    __modify __exact    [__ax]

extern void _BIOSMouseSetPointerExclusionArea( short, short, short, short );
#pragma aux _BIOSMouseSetPointerExclusionArea = \
        _INT_33_FN( 0x10 ) \
    __parm              [__cx] [__dx] [__si] [__di] \
    __value             \
    __modify __exact    [__ax]

extern void _BIOSMouseSetDoubleSpeedThreshold( short threshold );
#pragma aux _BIOSMouseSetDoubleSpeedThreshold = \
        _INT_33_FN( 0x13 ) \
    __parm              [__dx] \
    __value             \
    __modify __exact    [__ax]

extern void _BIOSMouseSelectPointerPage( short page );
#pragma aux _BIOSMouseSelectPointerPage = \
        _INT_33_FN( 0x1d ) \
    __parm              [__bx] \
    __value             \
    __modify __exact    [__ax]

extern unsigned short _BIOSMouseDriverResetSoft( void );
#pragma aux _BIOSMouseDriverResetSoft = \
        _INT_33_FN( 0x21 )  \
    __parm              [] \
    __value             [__ax] \
    __modify __exact    [__ax __bx]


#endif /* defined( __DOS__ ) || defined( __WINDOWS__ ) || defined( _M_I86 ) && defined( __OS2__ ) */

#endif /* _INT33_H_INCLUDED */
