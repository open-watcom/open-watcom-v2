/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Ctrl-Break, critical error and FPE hooking and handling.
*                   (16/32-bit DOS and 16-bit Windows 3.x code)
*
****************************************************************************/


#include "variety.h"
#include <signal.h>
#include <string.h>
#include <dos.h>
#include "tinyio.h"
#include "rtdata.h"
#include "rtstack.h"
#include "stacklow.h"
#include "sigfunc.h"
#include "_int23.h"
#include "_ctrlc.h"
#ifndef _M_I86
    #include "extender.h"
    #include "dpmi.h"
#endif


typedef void (_WCINTERRUPT _WCFAR *pfun)( void );

#ifndef _M_I86

  extern  void pharlap_setvect( unsigned, pfun );
  #pragma aux  pharlap_setvect = \
            "push ds"       \
            "mov  ds,ecx"   \
            "mov  cl,al"    \
            "mov  ax,2506h" \
            __INT_21        \
            "pop  ds"       \
        __parm __caller [__al] [__cx __edx]

  extern  pfun pharlap_rm_getvect( unsigned );
  #pragma aux  pharlap_rm_getvect = \
            "mov ax,2503h"  \
            __INT_21        \
            "mov cx,bx"     \
            "shr ebx,16"    \
            "mov edx,ebx"   \
        __parm __caller [__cl] \
        __value         [__cx __edx] \
        __modify        [__ax __ebx]

  extern  void pharlap_rm_setvect( unsigned, pfun );
  #pragma aux  pharlap_rm_setvect = \
            "mov ebx,eax"   \
            "shl ebx,16"    \
            "mov bx,dx"     \
            "mov ax,2505h"  \
            __INT_21        \
        __parm __caller [__cl] [__dx __eax] \
        __modify        [__ebx]

  extern  pfun pharlap_pm_getvect( unsigned );
  #pragma aux  pharlap_pm_getvect = \
            "push es"       \
            "mov ax,2502h"  \
            __INT_21        \
            "mov ecx,es"    \
            "pop es"        \
        __parm __caller [__cl] \
        __value         [__cx __ebx] \
        __modify        [__ax]

  extern  void pharlap_pm_setvect( unsigned, pfun );
  #pragma aux  pharlap_pm_setvect = \
            "push ds"       \
            "mov ds,edx"    \
            "mov edx,eax"   \
            "mov ax,2504h"  \
            __INT_21        \
            "pop ds"        \
        __parm __caller [__cl] [__dx __eax]

  extern void _WCFAR *set_stack( void _WCFAR * );
  #pragma aux set_stack =       \
            "mov ebx,ss"    \
            "mov ecx,esp"   \
            "mov ss,edx"    \
            "mov esp,eax"   \
            "mov edx,ebx"   \
            "mov eax,ecx"   \
        __parm          [__dx __eax] \
        __value         [__dx __eax] \
        __modify        [__bx __ecx]

#endif

static  void    _WCNEAR __restore_int( void );
// __int23_exit is now a multi-state pointer:
// __null_int23_exit        -> implies no vectors are hooked
// __restore_int23          -> implies only int 23 is hooked
// __restore_int_ctrl_break -> implies only ctrl_break is hooked
// __restore_int            -> implies both int 23 and ctrl_break are hooked

#ifdef _M_I86

static pfun __old_int23 = 0;
static pfun __old_int_ctrl_break = 0;

#else

static pfun __old_rm_int23 = 0;
static pfun __old_rm_int_ctrl_break = 0;
static pfun __old_pm_int23 = 0;
static pfun __old_pm_int_ctrl_break = 0;

#define MY_STACK_SIZE 256
static unsigned int my_stack[MY_STACK_SIZE];

#endif

static void _WCINTERRUPT _WCFAR __int23_handler( void )
{
#ifndef _M_I86
    unsigned save_stacklow;
    void _WCFAR *save_stack;

    save_stack = set_stack( &(my_stack[MY_STACK_SIZE-1]) );
    save_stacklow = _STACKLOW;
    _STACKLOW = (unsigned)my_stack;
#endif
    if(( _RWD_child == 0 )
      && ( __int23_exit != __null_int23_exit )
      && ( __int23_exit != __restore_int_ctrl_break )) {
        _enable();
        raise( SIGINT );
    } else {
#ifdef _M_I86
        _chain_intr( __old_int23 );
#else
        _chain_intr( __old_rm_int23 );
#endif
    }
#ifndef _M_I86
    set_stack( save_stack );
    _STACKLOW = save_stacklow;
#endif
}

/*
 * NOTE: DO NOT CHANGE MOVE THIS FUNCTION OR INSERT CODE BETWEEN THESE
 *       TWO FUNCTIONS!!!  WE NEED THIS RELATIONSHIP TO CALCULATE THE
 *       SIZE OF __int23_handler()!!!
 */

static void _WCINTERRUPT _WCFAR __int_ctrl_break_handler( void )
{
#ifndef _M_I86
    unsigned save_stacklow;
    void _WCFAR *save_stack;

    save_stack = set_stack( &(my_stack[(sizeof(my_stack)-sizeof(my_stack[0]))]) );
    save_stacklow = _STACKLOW;
    _STACKLOW = (unsigned)my_stack;
#endif
    if(( _RWD_child == 0 )
     && ( __int23_exit != __null_int23_exit )
     && ( __int23_exit != __restore_int23 )) {
        _enable();
        raise( SIGBREAK );
    } else {
#ifdef _M_I86
        _chain_intr( __old_int_ctrl_break );
#else
        _chain_intr( __old_rm_int_ctrl_break );
#endif
    }
#ifndef _M_I86
    set_stack( save_stack );
    _STACKLOW = save_stacklow;
#endif
}

/*
 * NOTE: DO NOT CHANGE MOVE THIS FUNCTION OR INSERT CODE BETWEEN THESE
 *       TWO FUNCTIONS!!!  WE NEED THIS RELATIONSHIP TO CALCULATE THE
 *       SIZE OF __int_ctrl_break_handler()!!!
 */

void _WCNEAR __restore_int23( void )
{
#ifdef _M_I86
    if( __old_int23 == 0 ) {
#else
    if( __old_rm_int23 == 0 ) {
#endif
        return;
    }
    if( __int23_exit == __restore_int ) {
        __int23_exit = __restore_int_ctrl_break;
    } else if( __int23_exit == __restore_int23 ) {
        __int23_exit = __null_int23_exit;
    }
#ifdef _M_I86
    _dos_setvect( 0x23, __old_int23 );
    __old_int23 = 0;
#else
    if( _IsPharLap() ) {
        pharlap_rm_setvect( 0x23, __old_rm_int23 );
        pharlap_pm_setvect( 0x23, __old_pm_int23 );
    } else if( _DPMI ) {
        DPMISetRealModeInterruptVector( 0x23, __old_rm_int23 );
        DPMISetPMInterruptVector( 0x23, __old_pm_int23 );
    } else {        /* this is what it used to do */
        _dos_setvect( 0x23, __old_rm_int23 );
    }
    __old_rm_int23 = 0;
#endif
}

void _WCNEAR __restore_int_ctrl_break( void )
{
#ifdef _M_I86
    if( __old_int_ctrl_break == 0 ) {
#else
    if( __old_rm_int_ctrl_break == 0 ) {
#endif
        return;
    }
    if( __int23_exit == __restore_int ) {
        __int23_exit = __restore_int23;
    } else if( __int23_exit == __restore_int_ctrl_break ) {
        __int23_exit = __null_int23_exit;
    }
#ifdef _M_I86
    _dos_setvect( CTRL_BREAK_INT, __old_int_ctrl_break );
    __old_int_ctrl_break = 0;
#else
    if( _IsPharLap() ) {
        pharlap_rm_setvect( CTRL_BREAK_INT, __old_rm_int_ctrl_break );
        pharlap_pm_setvect( CTRL_BREAK_INT, __old_pm_int_ctrl_break );
    } else if( _DPMI ) {
        DPMISetRealModeInterruptVector( CTRL_BREAK_INT, __old_rm_int_ctrl_break );
        DPMISetPMInterruptVector( CTRL_BREAK_INT, __old_pm_int_ctrl_break );
    } else {
        _dos_setvect( CTRL_BREAK_INT, __old_rm_int_ctrl_break );
    }
    __old_rm_int_ctrl_break = 0;
#endif
}

static void _WCNEAR __restore_int( void )
{
    __restore_int23();
    __restore_int_ctrl_break();
}

void _WCNEAR __grab_int23( void )
{
#ifdef _M_I86
    if( __old_int23 == 0 ) {
        __old_int23 = _dos_getvect( 0x23 );
        _dos_setvect( 0x23, __int23_handler );
#else
    if( __old_rm_int23 == 0 ) {
        if( _IsPharLap() ) {
            __old_rm_int23 = pharlap_rm_getvect( 0x23 );
            __old_pm_int23 = pharlap_pm_getvect( 0x23 );
            pharlap_setvect( 0x23, __int23_handler );
        } else if( _DPMI ) {
            DPMILockLinearRegion((long)__int23_handler,
                ((long)__int_ctrl_break_handler - (long)__int23_handler));
            __old_rm_int23 = DPMIGetRealModeInterruptVector( 0x23 );
            __old_pm_int23 = DPMIGetPMInterruptVector( 0x23 );
            DPMISetPMInterruptVector( 0x23, __int23_handler );
        } else {        /* what it used to do */
            __old_rm_int23 = _dos_getvect( 0x23 );
            _dos_setvect( 0x23, __int23_handler );
        }
#endif
        if( __int23_exit == __null_int23_exit ) {
            __int23_exit = __restore_int23;
        } else if( __int23_exit == __restore_int_ctrl_break ) {
            __int23_exit = __restore_int;
        }
    }
}

void _WCNEAR __grab_int_ctrl_break( void )
{
#ifdef _M_I86
    if( __old_int_ctrl_break == 0 ) {
        __old_int_ctrl_break = _dos_getvect( CTRL_BREAK_INT );
        _dos_setvect( CTRL_BREAK_INT, __int_ctrl_break_handler );
#else
    if( __old_rm_int_ctrl_break == 0 ) {
        if( _IsPharLap() ) {
            __old_rm_int_ctrl_break = pharlap_rm_getvect( CTRL_BREAK_INT );
            __old_pm_int_ctrl_break = pharlap_pm_getvect( CTRL_BREAK_INT );
            pharlap_setvect( CTRL_BREAK_INT, __int_ctrl_break_handler );
        } else if( _DPMI ) {
            DPMILockLinearRegion((long)__int_ctrl_break_handler,
                ((long)__restore_int23 - (long)__int_ctrl_break_handler));
            __old_rm_int_ctrl_break = DPMIGetRealModeInterruptVector( CTRL_BREAK_INT );
            __old_pm_int_ctrl_break = DPMIGetPMInterruptVector( CTRL_BREAK_INT );
            DPMISetPMInterruptVector( CTRL_BREAK_INT, __int_ctrl_break_handler );
        } else {        /* what it used to do */
            __old_rm_int_ctrl_break = _dos_getvect( CTRL_BREAK_INT );
            _dos_setvect( CTRL_BREAK_INT, __int_ctrl_break_handler );
        }
#endif
        if( __int23_exit == __null_int23_exit ) {
            __int23_exit = __restore_int_ctrl_break;
        } else if( __int23_exit == __restore_int23 ) {
            __int23_exit = __restore_int;
        }
    }
}
