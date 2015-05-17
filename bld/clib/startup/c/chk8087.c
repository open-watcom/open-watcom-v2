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
* Description:  Implementation of __chk8087 and other FPU support routines.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#include <float.h>
#if defined( __WINDOWS__ )
  #include <i86.h>
#endif
#if defined( __WINDOWS__ )
  #include <windows.h>
#elif defined( __OS2__ )
  #define INCL_DOSEXCEPTIONS
  #define INCL_DOSDEVICES
  #include <wos2.h>
#endif
#include "rtdata.h"
#include "exitwmsg.h"

#if defined( __WINDOWS_386__ )
extern void __pascal _FloatingPoint( void );
#endif

extern void __GrabFP87( void );

extern unsigned short __8087cw;
#pragma aux __8087cw "*";

#if defined( __DOS_086__ )
extern unsigned char __dos87real;
#pragma aux __dos87real "*";

extern unsigned short __dos87emucall;
#pragma aux __dos87emucall "*";
#endif

extern void __init_80x87( void );
#if defined( __DOS_086__ )
#pragma aux __init_80x87 "*" =      \
        "cmp    __dos87real,0"      \
        "jz     l1"                 \
        "finit"                     \
        "fldcw  __8087cw"           \
"l1:     cmp    __dos87emucall,0"   \
        "jz     l2"                 \
        "mov    ax,1"               \
        "call   __dos87emucall"     \
"l2:" ;
#else
#pragma aux __init_80x87 "*" =      \
        "fninit"                    \
        "fwait"                     \
        "fldcw  __8087cw" ;
#endif

/* 0 => no x87; 1 => 8087; 2 => 80287; 3 => 80387 */
extern unsigned char _WCI86NEAR __x87id( void );
#pragma aux __x87id "*";

#if !defined( __UNIX__ ) && !defined( __OS2_386__ )

extern void __fsave( _87state * );
extern void __frstor( _87state * );

#if defined( __386__ )

#pragma aux __fsave =   \
    "fsave [eax]"       \
    parm routine [eax];

#pragma aux __frstor =  \
    "frstor [eax]"      \
    parm routine [eax];

#else   /* __286__ */

  #if defined( __BIG_DATA__ )
    #pragma aux __fsave =   \
    "push    bx"            \
    "push    ds"            \
    "mov     ds,dx"         \
    "mov     bx,ax"         \
    "fsave   [bx]"          \
    "fwait"                 \
    "pop     ds"            \
    "pop     bx"            \
    parm routine [dx ax];
  #else
    #pragma aux __fsave =   \
    "push    bx"            \
    "mov     bx,ax"         \
    "fsave   [bx]"          \
    "fwait"                 \
    "pop     bx"            \
    parm routine [ax];
  #endif

  #if defined( __BIG_DATA__ )
    #pragma aux __frstor =  \
    "push    bx"            \
    "push    ds"            \
    "mov     ds,dx"         \
    "mov     bx,ax"         \
    "frstor  [bx]"          \
    "fwait"                 \
    "pop     ds"            \
    "pop     bx"            \
    parm routine [dx ax];
  #else
    #pragma aux __frstor =  \
    "push    bx"            \
    "mov     bx,ax"         \
    "frstor  [bx]"          \
    "fwait"                 \
    "pop     bx"            \
    parm routine [ax];
  #endif

#endif

static void __save_8087( _87state * __fs )
{
    __fsave( __fs );
}

static void __rest_8087( _87state * __fs )
{
    __frstor( __fs );
}
#endif  /* !__UNIX__ && && !__OS2__ */

_WCRTLINK void _fpreset( void )
{
    if( _RWD_8087 != 0 ) {
        __init_80x87();
    }
}

void __init_8087( void )
{
#if !defined( __UNIX__ ) && !defined( __OS2_386__ )
    if( _RWD_real87 != 0 ) {            /* if our emulator, don't worry */
        _RWD_Save8087 = __save_8087;    /* point to real save 8087 routine */
        _RWD_Rest8087 = __rest_8087;    /* point to real restore 8087 routine */
    }
#endif
    _fpreset();
}

#if defined( __DOS__ ) || defined( __OS2_286__ )

extern unsigned char _bin_to_ascii_offs( unsigned char c );
#pragma aux _bin_to_ascii_offs = \
    "and  al,0Fh" \
    "cmp  al,9" \
    "jbe short L1" \
    "add  al,7" \
    "L1:" \
    parm routine [al] value [al];

void _WCI86FAR __default_sigfpe_handler( int fpe_sig )
{
    char    msg[] = "Floating point exception (00)";

    msg[sizeof( msg ) - 4] += _bin_to_ascii_offs( fpe_sig >> 4 );
    msg[sizeof( msg ) - 3] += _bin_to_ascii_offs( fpe_sig );
    __fatal_runtime_error( msg, EXIT_FAILURE );
}
#endif

#if defined( __OS2__ )

void __chk8087( void )
/********************/
{
    char    devinfo;

#if defined( __386__ )
    DosDevConfig( &devinfo, DEVINFO_COPROCESSOR );
    if( devinfo == 0 ) {
        _RWD_real87 = 0;
    } else {
        _RWD_real87 = __x87id();
    }
    _RWD_8087 = _RWD_real87;
#else
    if( _RWD_8087 == 0 ) {
        DosDevConfig( &devinfo, 3, 0 );
        if( devinfo == 0 ) {
            _RWD_real87 = 0;
        } else {
            _RWD_real87 = __x87id();
        }
        _RWD_8087 = _RWD_real87;
    }
    if( _RWD_real87 ) {
        __GrabFP87();
    }
    if( _RWD_8087 ) {
        _RWD_FPE_handler = __default_sigfpe_handler;
    }
#endif
    _fpreset();
}

#elif defined( __QNX__ )

void __chk8087( void )
/********************/
{
    extern      char    __87;
    extern      char    __r87;

    _RWD_real87 = __r87;
    _RWD_8087 = __87;
    _fpreset();
}

#elif defined( __LINUX__ )

void __chk8087( void )
/********************/
{
    // TODO: We really need to call Linux and determine if the machine
    //       has a real FPU or not, so we can properly work with an FPU
    //       emulator.
    _RWD_real87 = __x87id();
    _RWD_8087 = _RWD_real87;
    _fpreset();
}

#elif defined( __NETWARE__ )

extern short __87present( void );
#pragma aux __87present =       \
    "smsw  ax           ",      \
    "test  ax, 4        ",      \
    "jne   no_emu       ",      \
    "xor   ax, ax       ",      \
    "no_emu:            ",      \
    "mov   ax, 1        "       \
value [ ax ];

extern void __chk8087( void )
/*****************************/
{
    if( _RWD_8087 == 0 ) {
        if( __87present() ) {
            _RWD_8087 = 3;      /* 387 */
            _RWD_real87 = 3;
        }
        __init_80x87();
    }
}

#elif defined( __NT__ ) || defined( __RDOS__ )

void __chk8087( void )
/********************/
{
    _RWD_real87 = __x87id();
    _RWD_8087 = _RWD_real87;
    __init_8087();
}

#elif defined( __RDOSDEV__ )

void __chk8087( void )
/********************/
{
    _RWD_real87 = 1;
    _RWD_8087 = _RWD_real87;
}

#elif defined( __DOS__ )

void __chk8087( void )
/********************/
{
    if( _RWD_8087 != 0 ) {          /* if we already know we have an 80x87 */
#if !defined( __386__ )
        if( __dos87real )
            __GrabFP87();
#endif
        _RWD_FPE_handler = __default_sigfpe_handler;
        return;                    /* this prevents real87 from being set */
    }                               /* when we have an emulator */
    _RWD_real87 = __x87id();        /* if a coprocessor is present then we */
    _RWD_8087 = _RWD_real87;        /* initialize even when NO87 is defined */
#if !defined( __386__ )
    __dos87real = _RWD_real87;
#endif
    __init_8087();                  /* this handles the fpi87 and NO87 case */
    if( _RWD_no87 != 0 ) {          /* if NO87 environment var is defined */
        _RWD_8087 = 0;              /* then we want to pretend that the */
        _RWD_real87 = 0;            /* coprocessor doesn't exist */
    }
    if( _RWD_real87 ) {
        __GrabFP87();
    }
    if( _RWD_8087 ) {
        _RWD_FPE_handler = __default_sigfpe_handler;
    }
}

#elif defined( __WINDOWS__ )

void __chk8087( void )
/********************/
{
    if( _RWD_8087 != 0 )             /* if we already know we have an 80x87 */
        return;                      /* this prevents real87 from being set */
                                     /* when we have an emulator */
    if( GetWinFlags() & WF_80x87 ) { /* if a coprocessor is present then we */
  #if defined( __386__ )
        _FloatingPoint();
  #endif
        _RWD_real87 = __x87id();     /* initialize even when NO87 is defined */
        _RWD_8087 = _RWD_real87;     /* this handles the fpi87 and NO87 case */
        __init_8087();
    } else {
  #if defined( __386__ )
        // check to see if emulator is loaded
        union REGS regs;
        regs.w.ax = 0xfa00;
        int86( 0x2f, &regs, &regs );
        if( regs.w.ax == 0x0666 ) {  /* check for emulator present */
            _RWD_real87 = __x87id(); /* initialize even when NO87 is defined */
            _RWD_8087 = _RWD_real87; /* this handles the fpi87 and NO87 case */
            __init_8087();
        }
  #endif
    }
    if( _RWD_no87 != 0 ) {           /* if NO87 environment var is defined */
        _RWD_8087 = 0;               /* then we want to pretend that the */
        _RWD_real87 = 0;             /* coprocessor doesn't exist */
    }
}

#endif
