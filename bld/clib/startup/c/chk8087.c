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


#include "variety.h"
#if defined( __OS2__ )
  #define INCL_DOSEXCEPTIONS
  #define INCL_DOSDEVICES
  #include <wos2.h>
#endif
#if defined(__WINDOWS_386__) || defined(__WINDOWS__)
  #include <i86.h>
  #include <windows.h>
#endif

#include "rtdata.h"

unsigned char __init_80x87( unsigned short );

extern  char __test8087( void );

#if defined(__386__)
    #pragma aux __init_80x87 "*" parm routine [eax];

    #pragma aux __test8087 =                                                \
    0x2b 0xc0     /* sub   EAX,EAX        ; start with a preset value */    \
    0x50          /* push  EAX            ; allocate space for ctrl word */ \
    0xdb 0xe3     /* fninit               ; initialize math coprocessor */  \
    0xd9 0x3c 0x24/* fnstcw word ptr [ESP]; store control word in memory */ \
    0x58          /* pop   EAX            ; get control word */             \
    0x8a 0xc4     /* mov   AL,AH          ; get upper byte */               \
    value [al];
#else
    #pragma aux __init_80x87 "*" parm routine [ax];

    #pragma aux __test8087 =                                                \
    0x55          /* push  BP             ; save BP */                      \
    0x8b 0xec     /* mov   BP,SP          ; get access to stack */          \
    0x2b 0xc0     /* sub   AX,AX          ; start with a preset value */    \
    0x50          /* push  AX             ; allocate space for ctrl word */ \
    0xdb 0xe3     /* fninit               ; initialize math coprocessor */  \
    0xd9 0x7e 0xfe/* fnstcw word ptr -2H[bp]; store cntrl word in memory */ \
    0x58          /* pop   AX             ; get control word */             \
    0x8a 0xc4     /* mov   AL,AH          ; get upper byte */               \
    0x5d          /* pop   BP             ; restore BP */                   \
    value [al];
#endif

#if !defined(__PENPOINT__) && !defined(__QNX__) && !defined(__OS2_386__)

struct  _87state {              /* 80x87 save area */
#if defined(__386__)
    char data[108];             /* 32-bit save area size */
#else
    char data[94];              /* 16-bit save area size */
#endif
};

extern void __fsave( struct _87state * );
extern void __frstor( struct _87state * );

#if defined(__386__)

    #pragma aux __fsave =                                           \
    0x9b 0xdd 0x30  /* fsave    [eax]   ; save the 8087 state */    \
    0x9b            /* wait                                   */    \
    parm routine [eax];

    #pragma aux __frstor =                                          \
    0xdd 0x20       /* frstor   [eax]   ; restore the 8087 */       \
    0x9b            /* wait             ; wait             */       \
    parm routine [eax];

#else   /* __286__ */

  #if defined(__BIG_DATA__)
    #pragma aux __fsave =                                           \
    0x53            /* push    bx                           */      \
    0x1e            /* push    ds                           */      \
    0x8e 0xda       /* mov     ds,dx                        */      \
    0x8b 0xd8       /* mov     bx,ax                        */      \
    0x9b 0xdd 0x37  /* fsave   [bx]                         */      \
    0x90 0x9b       /* fwait                                */      \
    0x1f            /* pop     ds                           */      \
    0x5b            /* pop     bx                           */      \
    parm routine [dx ax];
  #else
    #pragma aux __fsave =                                           \
    0x53            /* push    bx                           */      \
    0x8b 0xd8       /* mov     bx,ax                        */      \
    0x9b 0xdd 0x37  /* fsave   [bx]                         */      \
    0x90 0x9b       /* fwait                                */      \
    0x5b            /* pop     bx                           */      \
    parm routine [ax];
  #endif

  #if defined(__BIG_DATA__)
    #pragma aux __frstor =                                          \
    0x53            /* push    bx                           */      \
    0x1e            /* push    ds                           */      \
    0x8e 0xda       /* mov     ds,dx                        */      \
    0x8b 0xd8       /* mov     bx,ax                        */      \
    0x9b 0xdd 0x27  /* frstor  [bx]                         */      \
    0x90 0x9b       /* fwait                                */      \
    0x1f            /* pop     ds                           */      \
    0x5b            /* pop     bx                           */      \
    parm routine [dx ax];
  #else
    #pragma aux __frstor =                                          \
    0x53            /* push    bx                           */      \
    0x8b 0xd8       /* mov     bx,ax                        */      \
    0x9b 0xdd 0x27  /* frstor  [bx]                         */      \
    0x90 0x9b       /* fwait                                */      \
    0x5b            /* pop     bx                           */      \
    parm routine [ax];
  #endif

#endif

static void __save_8087( struct _87state * __fs )
{
    __fsave( __fs );
}

static void __rest_8087( struct _87state * __fs )
{
    __frstor( __fs );
}
#endif  /* ! __QNX__ && !__PENPOINT && !__OS2__ */

unsigned char __init_8087()
{
#if !defined(__PENPOINT__) && !defined(__QNX__) && !defined(__OS2_386__)
    if( _RWD_real87 != 0 ) {            /* if our emulator, don't worry */
        _RWD_Save8087 = __save_8087;    /* point to real save 8087 routine */
        _RWD_Rest8087 = __rest_8087;    /* point to real restore 8087 routine */
    }
#endif
    /* 0 => no 8087; 2 => 8087,287; 3=>387 */
    return( __init_80x87( _RWD_8087cw ) );
}

_WCRTLINK void _fpreset( void )
{
    if( _RWD_real87 != 0 ) {
        __init_8087();
    }
}

#if defined( __OS2__ )

void    __chk8087() {
/******************/

    char    devinfo;

#if defined( __386__ )
    DosDevConfig( &devinfo, DEVINFO_COPROCESSOR );
#else
    DosDevConfig( &devinfo, 3, 0 );
#endif
    if( devinfo == 0 ) {
        _RWD_8087 = 0;
    } else {
        _RWD_8087 = __init_80x87( _RWD_8087cw );
    }
    _RWD_real87 = _RWD_8087;
}

#elif defined( __QNX__ )

void __chk8087()
{
    extern      char    __87;
    extern      char    __r87;

    _RWD_real87 = __r87;
    _RWD_8087 = __87;
    if( _RWD_8087 != 0 ) __init_80x87( _RWD_8087cw );
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
{
    if( _RWD_8087 == 0 ) {
        if( __87present() ) {
            _RWD_8087 = 3;      /* 387 */
            _RWD_real87 = 3;
        }
        __init_80x87( _RWD_8087cw );
    }
}

#else

void __chk8087()
{
    unsigned char rc;
    unsigned char real87_rc;

    if( _RWD_8087 != 0 )return;     /* if we already know we have an 80x87 */
                                    /* this prevents real87 from being set */
                                    /* when we have an emulator */
    rc = 0;
    real87_rc = 0;
    _RWD_real87 = real87_rc;
#if defined(__WINDOWS_386__) || defined(__WINDOWS__)
    if( GetWinFlags() & WF_80x87 ) {/* if a coprocessor is present then we */
        #if defined( __WINDOWS_386__ )
            extern void pascal _FloatingPoint( void );
            _FloatingPoint();
        #endif
        rc = __init_8087();         /* initialize even when NO87 is defined */
                                    /* this handles the fpi87 and NO87 case */
        real87_rc = rc;
    } else {
        #if defined( __WINDOWS_386__ )
            // check to see if emulator is loaded
            union REGS regs;
            regs.w.ax = 0xfa00;
            int86( 0x2f, &regs, &regs );
            if( regs.w.ax==1638 ) { /* check for emulator present */
                rc = __init_8087(); /* initialize even when NO87 is defined */
                                    /* this handles the fpi87 and NO87 case */
            }
        #endif
    }
#else
    if( __test8087() == 3 ) {       /* if a coprocessor is present then we */
        rc = __init_8087();         /* initialize even when NO87 is defined */
                                    /* this handles the fpi87 and NO87 case */
        real87_rc = rc;
    }
#endif
    if( _RWD_no87 != 0 )return;     /* if NO87 environment var is defined */
                                    /* then we want to pretend that the */
    _RWD_8087 = rc;                 /* coprocessor doesn't exist */
    _RWD_real87 = real87_rc;
}

#endif
