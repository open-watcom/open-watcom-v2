/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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


#if defined( _M_I86 )

#if defined( WIN386 )
extern short    DoDosCall( void __far *in, void __far *out );
#else
extern short    DoDosCall( void *in, void *out );
#endif
#if defined( __BIG_DATA__ ) || defined( WIN386 )
#pragma aux DoDosCall = \
        "push ds"           \
        "push es"           \
        "push bp"           \
        "push cx"           \
        "mov  ds,dx"        \
        "mov  bp,bx"        \
        "mov  ax,0[si]"     \
        "mov  bx,2[si]"     \
        "mov  cx,4[si]"     \
        "mov  dx,6[si]"     \
        "mov  di,10[si]"    \
        "mov  si,8[si]"     \
        "clc"               \
        "int 21h"           \
        "xchg si,bp"        \
        "pop  ds"           \
        "mov  0[si],ax"     \
        "mov  2[si],bx"     \
        "mov  4[si],cx"     \
        "mov  6[si],dx"     \
        "mov  8[si],bp"     \
        "mov  10[si],di"    \
        "pop  bp"           \
        "pop  es"           \
        "pop  ds"           \
        "sbb  ax,ax"        \
    __parm __caller [__dx __si] [__cx __bx] \
    __value         [__ax] \
    __modify        [__ax __dx __di]
#else
#pragma aux DoDosCall = \
        "push es"           \
        "push bp"           \
        "push dx"           \
        "mov  ax,0[di]"     \
        "mov  bx,2[di]"     \
        "mov  cx,4[di]"     \
        "mov  dx,6[di]"     \
        "mov  si,8[di]"     \
        "mov  di,10[di]"    \
        "clc"               \
        "int 21h"           \
        "mov  bp,di"        \
        "pop  di"           \
        "mov  0[di],ax"     \
        "mov  2[di],bx"     \
        "mov  4[di],cx"     \
        "mov  6[di],dx"     \
        "mov  8[di],si"     \
        "mov  10[di],bp"    \
        "sbb  ax,ax"        \
        "pop  bp"           \
        "pop  es"           \
    __parm __caller [__di] [__dx] \
    __value         [__ax] \
    __modify        [__bx __cx __dx __di __si]
#endif

#if defined( WIN386 )
extern short    DoDosxCall( void __far *in, void __far *out, void __far *sr );
#else
extern short    DoDosxCall( void *in, void *out, void *sr );
#endif
#if defined( __BIG_DATA__ ) || defined( WIN386 )
#pragma aux DoDosxCall = \
        "push ds"        /* ----------. */ \
        "mov  ds,di"     /*           | */ \
        "push bp"        /* ---------.| */ \
        "mov  es,0[bx]"  /*          || */ \
        "mov  bp,6[bx]"  /*          || */ \
        "push dx"        /* --------.|| */ \
        "push ax"        /* -------.||| */ \
        "push ds"        /* ------.|||| */ \
        "push bx"        /* -----.||||| */ \
        "mov  ds,cx"     /*      |||||| */ \
        "mov  ax,0[si]"  /*      |||||| */ \
        "mov  bx,2[si]"  /*      |||||| */ \
        "mov  cx,4[si]"  /*      |||||| */ \
        "mov  dx,6[si]"  /*      |||||| */ \
        "mov  di,10[si]" /*      |||||| */ \
        "mov  si,8[si]"  /*      |||||| */ \
        "mov  ds,bp"     /*      |||||| */ \
        "clc"            /*      |||||| */ \
        "int 21h"        /*      |||||| */ \
        "push ds"        /* ----.|||||| */ \
        "push si"        /* ---.||||||| */ \
        "mov  bp,sp"     /*    |||||||| */ \
        "mov  si,8[bp]"  /*    |||||||| */ \
        "mov  ds,10[bp]" /*    |||||||| */ \
        "pop  bp"        /* ---'||||||| */ \
        "mov  0[si],ax"  /*     ||||||| */ \
        "mov  2[si],bx"  /*     ||||||| */ \
        "mov  4[si],cx"  /*     ||||||| */ \
        "mov  6[si],dx"  /*     ||||||| */ \
        "mov  8[si],bp"  /*     ||||||| */ \
        "mov  10[si],di" /*     ||||||| */ \
        "sbb  ax,ax"     /*     ||||||| */ \
        "pop  bx"        /*(ds)-'|||||| */ \
        "pop  si"        /* -----'||||| */ \
        "pop  ds"        /* ------'|||| */ \
        "mov  0[si],es"  /*        |||| */ \
        "mov  6[si],bx"  /*        |||| */ \
        "pop  bx"        /* -------'||| */ \
        "pop  bx"        /* --------'|| */ \
        "pop  bp"        /* ---------'| */ \
        "pop  ds"        /* ----------' */ \
    __parm __caller [__si __cx] [__ax __dx] [__bx __di] \
    __value         [__ax] \
    __modify        [__di __es]
#else
#pragma aux DoDosxCall = \
        "push bp"        /* ----------. */ \
        "push es"        /* ---------.| */ \
        "push bx"        /* --------.|| */ \
        "push ds"        /* -------.||| */ \
        "push dx"        /* ------.|||| */ \
        "mov  es,[bx]"   /*       ||||| */ \
        "mov  bp,6[bx]"  /*       ||||| */ \
        "mov  ax,0[di]"  /*       ||||| */ \
        "mov  bx,2[di]"  /*       ||||| */ \
        "mov  cx,4[di]"  /*       ||||| */ \
        "mov  dx,6[di]"  /*       ||||| */ \
        "mov  si,8[di]"  /*       ||||| */ \
        "mov  di,10[di]" /*       ||||| */ \
        "mov  ds,bp"     /*       ||||| */ \
        "clc"            /*       ||||| */ \
        "int 21h"        /*       ||||| */ \
        "push ds"        /* -----.||||| */ \
        "push di"        /* ----.|||||| */ \
        "mov  bp,sp"     /*     ||||||| */ \
        "mov  di,4[bp]"  /*     ||||||| */ \
        "mov  ds,6[bp]"  /*     ||||||| */ \
        "mov  0[di],ax"  /*     ||||||| */ \
        "mov  2[di],bx"  /*     ||||||| */ \
        "mov  4[di],cx"  /*     ||||||| */ \
        "mov  6[di],dx"  /*     ||||||| */ \
        "mov  8[di],si"  /*     ||||||| */ \
        "pop  10[di]"    /* ----'|||||| */ \
        "pop  ax"        /*(ds) -'||||| */ \
        "pop  bx"        /* ------'|||| */ \
        "pop  bx"        /* -------'||| */ \
        "pop  bx"        /* --------'|| */ \
        "mov  6[bx],ax"  /*          || */ \
        "mov  [bx],es"   /*          || */ \
        "sbb  ax,ax"     /*          || */ \
        "pop  es"        /* ---------'| */ \
        "pop  bp"        /* ----------' */ \
    __parm __caller [__di] [__dx] [__bx] \
    __value         [__ax] \
    __modify        [__bx __cx __dx __si __di]
#endif

#elif defined( _M_IX86 ) && !defined( __WINDOWS__ )

extern int      DoDosCall( void *in, void *out );
#pragma aux DoDosCall = \
        "push ebp"          \
        "push edx"          \
        "mov  eax,[edi]"    \
        "mov  ebx,4[edi]"   \
        "mov  ecx,8[edi]"   \
        "mov  edx,12[edi]"  \
        "mov  esi,16[edi]"  \
        "mov  edi,20[edi]"  \
        "clc"               \
        "int 21h"           \
        "mov  ebp,edi"      \
        "pop  edi"          \
        "mov  [edi],eax"    \
        "mov  4[edi],ebx"   \
        "mov  8[edi],ecx"   \
        "mov  12[edi],edx"  \
        "mov  16[edi],esi"  \
        "mov  20[edi],ebp"  \
        "sbb  eax,eax"      \
        "pop  ebp"          \
    __parm __caller [__edi] [__edx] \
    __value         [__eax] \
    __modify        [__ebx __ecx __edx __edi __esi]

extern int      DoDosxCall( void *in, void *out, void *sr );
#pragma aux DoDosxCall = \
        "push ebp"         /* -----------. */ \
        "push es"          /* ----------.| */ \
        "push ebx"         /* ---------.|| */ \
        "push ds"          /* --------.||| */ \
        "push edx"         /* -------.|||| */ \
        "mov  es,[ebx]"    /*        ||||| */ \
        "mov  bp,6[ebx]"   /*        ||||| */ \
        "mov  eax,0[edi]"  /*        ||||| */ \
        "mov  ebx,4[edi]"  /*        ||||| */ \
        "mov  ecx,8[edi]"  /*        ||||| */ \
        "mov  edx,12[edi]" /*        ||||| */ \
        "mov  esi,16[edi]" /*        ||||| */ \
        "mov  edi,20[edi]" /*        ||||| */ \
        "mov  ds,ebp"      /*        ||||| */ \
        "clc"              /*        ||||| */ \
        "int 21h"          /*        ||||| */ \
        "push ds"          /* ------.||||| */ \
        "push edi"         /* -----.|||||| */ \
        "mov  ebp,esp"     /*      ||||||| */ \
        "mov  edi,8[ebp]"  /*      ||||||| */ \
        "mov  ds,12[ebp]"  /*      ||||||| */ \
        "mov  0[edi],eax"  /*      ||||||| */ \
        "mov  4[edi],ebx"  /*      ||||||| */ \
        "mov  8[edi],ecx"  /*      ||||||| */ \
        "mov  12[edi],edx" /*      ||||||| */ \
        "mov  16[edi],esi" /*      ||||||| */ \
        "pop  20[edi]"     /* -----'|||||| */ \
        "pop  eax"         /*(ds) --'||||| */ \
        "pop  ebx"         /* -------'|||| */ \
        "pop  ebx"         /* --------'||| */ \
        "pop  ebx"         /* ---------'|| */ \
        "mov  6[ebx],ax"   /*           || */ \
        "mov  0[ebx],es"   /*           || */ \
        "sbb  eax,eax"     /*           || */ \
        "pop  es"          /* ----------'| */ \
        "pop  ebp"         /* -----------' */ \
    __parm __caller [__edi] [__edx] [__ebx] \
    __value         [__eax] \
    __modify        [__ebx __ecx __edx __edi __esi]

#endif
