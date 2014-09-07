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
* Description:  DOS Ctrl-Break, critical error and FPE hooking and handling.
*
****************************************************************************/


#include "variety.h"
#include <signal.h>
#include <string.h>
#include <dos.h>
#include "rtdata.h"
#include "stacklow.h"
#include "sigfunc.h"
#include "_int23.h"

/* Ctrl-Break vector (IBM compatible) */
#define CTRL_BRK_VEC    0x1B

typedef void (_WCINTERRUPT _WCFAR *pfun)( void );

#if defined( __386__ )
 #if defined( __WINDOWS_386__ )
  #include "tinyio.h"
 #else
  #include "extender.h"
  #include "dpmi.h"

  extern  int __DPMI_hosted( void );

  extern  void pharlap_setvect( unsigned, pfun );
  #pragma aux  pharlap_setvect =  0x1e   /* push ds    */\
                               0x8e 0xd9 /* mov ds,cx  */\
                               0x88 0xc1 /* mov cl,al  */\
                               0x66 0xb8 0x06 0x25 /* mov ax,2506h */\
                               0xcd 0x21 /* int 21h    */\
                               0x1f      /* pop ds     */\
                        parm caller [al] [cx edx];

  extern  pfun pharlap_rm_getvect( unsigned );
  #pragma aux  pharlap_rm_getvect =     \
                        "mov ax,2503h"  \
                        "int 21h"       \
                        "mov cx,bx"     \
                        "shr ebx,16"    \
                        "mov edx,ebx"   \
                        parm caller [cl] value [cx edx] modify [ax ebx];

  extern  void pharlap_rm_setvect( unsigned, pfun );
  #pragma aux  pharlap_rm_setvect =     \
                        "mov ebx,eax"   \
                        "shl ebx,16"    \
                        "mov bx,dx"     \
                        "mov ax,2505h"  \
                        "int 21h"       \
                        parm caller [cl] [dx eax] modify [ebx];

  extern  pfun pharlap_pm_getvect( unsigned );
  #pragma aux  pharlap_pm_getvect =     \
                        "push es"       \
                        "mov ax,2502h"  \
                        "int 21h"       \
                        "mov cx,es"     \
                        "pop es"        \
                        parm caller [cl] value [cx ebx] modify [ax];

  extern  void pharlap_pm_setvect( unsigned, pfun );
  #pragma aux  pharlap_pm_setvect =     \
                        "push ds"       \
                        "mov ds,dx"     \
                        "mov edx,eax"   \
                        "mov ax,2504h"  \
                        "int 21h"       \
                        "pop ds"        \
                        parm caller [cl] [dx eax];

  extern void _WCFAR *set_stack( void _WCFAR * );
  #pragma aux set_stack = \
                        "mov bx,ss"     \
                        "mov ecx,esp"   \
                        "mov ss,dx"     \
                        "mov esp,eax"   \
                        "mov dx,bx"     \
                        "mov eax,ecx"   \
                        parm [dx eax] value [dx eax] modify [bx ecx];

 #endif
#endif

        void    __restore_int23( void );
        void    __restore_int_ctrl_break( void );
static  void    __restore_int( void );
// __int23_exit is now a multi-state pointer:
// __null_int23_exit        -> implies no vectors are hooked
// __restore_int23          -> implies only int 23 is hooked
// __restore_int_ctrl_break -> implies only ctrl_break is hooked
// __restore_int            -> implies both int 23 and ctrl_break are hooked

static pfun __old_int23 = 0;
static pfun __old_int_ctrl_break = 0;

#if defined( __386__ )

static pfun __old_pm_int23 = 0;
static pfun __old_pm_int_ctrl_break = 0;
#define MY_STACK_SIZE 256
static unsigned int my_stack[256];

#endif

static void _WCINTERRUPT _WCFAR __int23_handler( void )
{
#if defined( __386__ )
    unsigned save_stacklow;
    void _WCFAR *save_stack;
    save_stack = set_stack( &(my_stack[MY_STACK_SIZE-1]) );
    save_stacklow = _STACKLOW;
    _STACKLOW = (unsigned)&my_stack;
#endif
    if(( _RWD_child == 0 )
      && ( __int23_exit != __null_int23_exit )
      && ( __int23_exit != __restore_int_ctrl_break )) {
        _enable();
        raise( SIGINT );
    } else {
        _chain_intr( __old_int23 );
    }
#if defined( __386__ )
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
#if defined( __386__ )
    unsigned save_stacklow;
    void _WCFAR *save_stack;
    save_stack = set_stack( &(my_stack[(sizeof(my_stack)-sizeof(my_stack[0]))]) );
    save_stacklow = _STACKLOW;
    _STACKLOW = (unsigned)&my_stack;
#endif
    if(( _RWD_child == 0 )
     && ( __int23_exit != __null_int23_exit )
     && ( __int23_exit != __restore_int23 )) {
        _enable();
        raise( SIGBREAK );
    } else {
        _chain_intr( __old_int_ctrl_break );
    }
#if defined( __386__ )
    set_stack( save_stack );
    _STACKLOW = save_stacklow;
#endif
}

/*
 * NOTE: DO NOT CHANGE MOVE THIS FUNCTION OR INSERT CODE BETWEEN THESE
 *       TWO FUNCTIONS!!!  WE NEED THIS RELATIONSHIP TO CALCULATE THE
 *       SIZE OF __int_ctrl_break_handler()!!!
 */

void __restore_int23( void )
{
    if( __old_int23 == 0 ) {
        return;
    }
    if( __int23_exit == __restore_int ) {
        __int23_exit = __restore_int_ctrl_break;
    } else if( __int23_exit == __restore_int23 ) {
        __int23_exit = __null_int23_exit;
    }
#if defined(__WINDOWS_386__)
    TinySetVect( 0x23, __old_int23 );
#elif defined( __386__ )
    if( _IsPharLap() ) {
        pharlap_rm_setvect( 0x23, __old_int23 );
        pharlap_pm_setvect( 0x23, __old_pm_int23 );
    } else if( __DPMI_hosted() == 1 ) {
        DPMISetRealModeInterruptVector( 0x23, __old_int23 );
        DPMISetPMInterruptVector( 0x23, __old_pm_int23 );
    } else {        /* this is what it used to do */
        _dos_setvect( 0x23, __old_int23 );
    }
#else
    _dos_setvect( 0x23, __old_int23 );
#endif
    __old_int23 = 0;
}

void __restore_int_ctrl_break( void )
{
    if( __old_int_ctrl_break == 0 ) {
        return;
    }
    if( __int23_exit == __restore_int ) {
        __int23_exit = __restore_int23;
    } else if( __int23_exit == __restore_int_ctrl_break ) {
        __int23_exit = __null_int23_exit;
    }
#if defined(__WINDOWS_386__)
    TinySetVect( CTRL_BRK_VEC, __old_int_ctrl_break );
#elif defined( __386__ )
    if( _IsPharLap() ) {
        pharlap_rm_setvect( CTRL_BRK_VEC, __old_int_ctrl_break );
        pharlap_pm_setvect( CTRL_BRK_VEC, __old_pm_int_ctrl_break );
    } else if( __DPMI_hosted() == 1 ) {
        DPMISetRealModeInterruptVector( CTRL_BRK_VEC, __old_int_ctrl_break );
        DPMISetPMInterruptVector( CTRL_BRK_VEC, __old_pm_int_ctrl_break );
    } else {
        _dos_setvect( CTRL_BRK_VEC, __old_int_ctrl_break );
    }
#else
    _dos_setvect( CTRL_BRK_VEC, __old_int_ctrl_break );
#endif
    __old_int_ctrl_break = 0;
}

static void __restore_int( void )
{
    __restore_int23();
    __restore_int_ctrl_break();
}

void __grab_int23( void )
{
    if( __old_int23 == 0 ) {
#if defined(__WINDOWS_386__)
        __old_int23 = _dos_getvect( 0x23 );
        TinySetVect( 0x23, (void (_WCNEAR *)(void))__int23_handler );
#elif defined( __386__ )
        if( _IsPharLap() ) {
            __old_int23 = pharlap_rm_getvect( 0x23 );
            __old_pm_int23 = pharlap_pm_getvect( 0x23 );
            pharlap_setvect( 0x23, (pfun) (void (_WCNEAR *)(void))__int23_handler );
        } else if( __DPMI_hosted() == 1 ) {
            DPMILockLinearRegion((long)__int23_handler,
                ((long)__int_ctrl_break_handler - (long)__int23_handler));
            __old_int23 = DPMIGetRealModeInterruptVector( 0x23 );
            __old_pm_int23 = DPMIGetPMInterruptVector( 0x23 );
            DPMISetPMInterruptVector( 0x23, __int23_handler );
        } else {        /* what it used to do */
            __old_int23 = _dos_getvect( 0x23 );
            _dos_setvect( 0x23, __int23_handler );
        }
#else
        __old_int23 = _dos_getvect( 0x23 );
        _dos_setvect( 0x23, __int23_handler );
#endif
        if( __int23_exit == __null_int23_exit ) {
            __int23_exit = __restore_int23;
        } else if( __int23_exit == __restore_int_ctrl_break ) {
            __int23_exit = __restore_int;
        }
    }
}

void __grab_int_ctrl_break( void )
{
    if( __old_int_ctrl_break == 0 ) {
#if defined(__WINDOWS_386__)
        __old_int_ctrl_break = _dos_getvect( CTRL_BRK_VEC );
        TinySetVect( CTRL_BRK_VEC, (void (_WCNEAR *)(void))__int_ctrl_break_handler );
#elif defined( __386__ )
        if( _IsPharLap() ) {
            __old_int_ctrl_break = pharlap_rm_getvect( CTRL_BRK_VEC );
            __old_pm_int_ctrl_break = pharlap_pm_getvect( CTRL_BRK_VEC );
            pharlap_setvect( CTRL_BRK_VEC, (pfun) (void (_WCNEAR *)(void))__int_ctrl_break_handler );
        } else if( __DPMI_hosted() == 1 ) {
            DPMILockLinearRegion((long)__int_ctrl_break_handler,
                ((long)__restore_int23 - (long)__int_ctrl_break_handler));
            __old_int_ctrl_break = DPMIGetRealModeInterruptVector( CTRL_BRK_VEC );
            __old_pm_int_ctrl_break = DPMIGetPMInterruptVector( CTRL_BRK_VEC );
            DPMISetPMInterruptVector( CTRL_BRK_VEC, __int_ctrl_break_handler );
        } else {        /* what it used to do */
            __old_int_ctrl_break = _dos_getvect( CTRL_BRK_VEC );
            _dos_setvect( CTRL_BRK_VEC, __int_ctrl_break_handler );
        }
#else
        __old_int_ctrl_break = _dos_getvect( CTRL_BRK_VEC );
        _dos_setvect( CTRL_BRK_VEC, __int_ctrl_break_handler );
#endif
        if( __int23_exit == __null_int23_exit ) {
            __int23_exit = __restore_int_ctrl_break;
        } else if( __int23_exit == __restore_int23 ) {
            __int23_exit = __restore_int;
        }
    }
}

#if defined( __DOS__ )

static FPEhandler   *__old_FPE_handler = NULL;

void __restore_FPE_handler( void )
{
    if( __old_FPE_handler == NULL ) {
        return;
    }
    __FPE_handler = __old_FPE_handler;
    __old_FPE_handler = NULL;
}

void __grab_FPE_handler( void )
{
    if( __old_FPE_handler == NULL ) {
        __old_FPE_handler = __FPE_handler;
        __FPE_handler = __sigfpe_handler;
    }
}
#endif
