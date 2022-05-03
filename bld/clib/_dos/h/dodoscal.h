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


#if defined( WIN386 )
#define WIN386FAR   __far
#else
#define WIN386FAR
#endif

extern int  DoDosCall( union REGS WIN386FAR *in, union REGS WIN386FAR *out );
extern int  DoDosxCall( union REGS WIN386FAR *in, union REGS WIN386FAR *out, struct SREGS WIN386FAR *sr );

#if defined( _M_I86 )

#if defined( __BIG_DATA__ ) || defined( WIN386 )
#pragma aux DoDosCall = \
        "push bp"        /* -------. */ \
        "push es"        /* ------.| */ \
        "push ds"        /* -----.|| */ \
        "push cx"        /* ----.||| */ \
        "push bx"        /* ---.|||| */ \
        "push ds"        /* --.||||| */ \
        "mov  ds,dx"     /*   |||||| */ \
        "mov  ax,0[si]"  /*   |||||| */ \
        "mov  bx,2[si]"  /*   |||||| */ \
        "mov  cx,4[si]"  /*   |||||| */ \
        "mov  dx,6[si]"  /*   |||||| */ \
        "mov  di,10[si]" /*   |||||| */ \
        "mov  si,8[si]"  /*   |||||| */ \
        "pop  ds"        /* --'||||| */ \
        "clc"            /*    ||||| */ \
        "int 21h"        /*    ||||| */ \
        "mov  bp,si"     /*    ||||| */ \
        "pop  si"        /* ---'|||| */ \
        "pop  ds"        /* ----'||| */ \
        "mov  0[si],ax"  /*      ||| */ \
        "mov  2[si],bx"  /*      ||| */ \
        "mov  4[si],cx"  /*      ||| */ \
        "mov  6[si],dx"  /*      ||| */ \
        "mov  8[si],bp"  /*      ||| */ \
        "mov  10[si],di" /*      ||| */ \
        "sbb  bp,bp"     /*      ||| */ \
        "and  bp,1"      /*      ||| */ \
        "mov  12[si],bp" /*      ||| */ \
        "pop  ds"        /* -----'|| */ \
        "pop  es"        /* ------'| */ \
        "pop  bp"        /* -------' */ \
    __parm __caller [__dx __si] [__cx __bx] \
    __value         [__ax] \
    __modify        [__bx __cx __dx __di __si]
#else
#pragma aux DoDosCall = \
        "push bp"        /* -----. */ \
        "push es"        /* ----.| */ \
        "push ds"        /* ---.|| */ \
        "push dx"        /* --.||| */ \
        "mov  ax,0[di]"  /*   |||| */ \
        "mov  bx,2[di]"  /*   |||| */ \
        "mov  cx,4[di]"  /*   |||| */ \
        "mov  dx,6[di]"  /*   |||| */ \
        "mov  si,8[di]"  /*   |||| */ \
        "mov  di,10[di]" /*   |||| */ \
        "clc"            /*   |||| */ \
        "int 21h"        /*   |||| */ \
        "mov  bp,di"     /*   |||| */ \
        "pop  di"        /* --'||| */ \
        "pop  ds"        /* ---'|| */ \
        "mov  0[di],ax"  /*     || */ \
        "mov  2[di],bx"  /*     || */ \
        "mov  4[di],cx"  /*     || */ \
        "mov  6[di],dx"  /*     || */ \
        "mov  8[di],si"  /*     || */ \
        "mov  10[di],bp" /*     || */ \
        "sbb  bp,bp"     /*     || */ \
        "and  bp,1"      /*     || */ \
        "mov  12[di],bp" /*     || */ \
        "pop  es"        /* ----'| */ \
        "pop  bp"        /* -----' */ \
    __parm __caller [__di] [__dx] \
    __value         [__ax] \
    __modify        [__bx __cx __dx __di __si]
#endif

#if defined( __BIG_DATA__ ) || defined( WIN386 )
#pragma aux DoDosxCall = \
        "push bp"        /* ----------. */ \
        "push es"        /* ---------.| */ \
        "push ds"        /* --------.|| */ \
        "push dx"        /* -------.||| */ \
        "push ax"        /* ------.|||| */ \
        "push di"        /* -----.||||| */ \
        "push bx"        /* ----.|||||| */ \
        "mov  ds,di"     /*     ||||||| */ \
        "mov  es,0[bx]"  /*     ||||||| */ \
        "mov  bp,6[bx]"  /*     ||||||| */ \
        "mov  ds,cx"     /*     ||||||| */ \
        "mov  ax,0[si]"  /*     ||||||| */ \
        "mov  bx,2[si]"  /*     ||||||| */ \
        "mov  cx,4[si]"  /*     ||||||| */ \
        "mov  dx,6[si]"  /*     ||||||| */ \
        "mov  di,10[si]" /*     ||||||| */ \
        "mov  si,8[si]"  /*     ||||||| */ \
        "mov  ds,bp"     /*     ||||||| */ \
        "clc"            /*     ||||||| */ \
        "int 21h"        /*     ||||||| */ \
        "push ds"        /* ---.||||||| */ \
        "push si"        /* --.|||||||| */ \
        "mov  bp,sp"     /*   ||||||||| */ \
        "lds  si,8[bp]"  /*   ||||||||| */ \
        "pop  bp"        /* --'|||||||| */ \
        "mov  0[si],ax"  /*    |||||||| */ \
        "mov  2[si],bx"  /*    |||||||| */ \
        "mov  4[si],cx"  /*    |||||||| */ \
        "mov  6[si],dx"  /*    |||||||| */ \
        "mov  8[si],bp"  /*    |||||||| */ \
        "mov  10[si],di" /*    |||||||| */ \
        "sbb  bp,bp"     /*    |||||||| */ \
        "and  bp,1"      /*    |||||||| */ \
        "mov  12[si],bp" /*    |||||||| */ \
        "pop  bp"        /* ---'||||||| */ \
        "pop  si"        /* ----'|||||| */ \
        "pop  ds"        /* -----'||||| */ \
        "mov  0[si],es"  /*       ||||| */ \
        "mov  6[si],bp"  /*       ||||| */ \
        "pop  bp"        /* ------'|||| */ \
        "pop  bp"        /* -------'||| */ \
        "pop  ds"        /* --------'|| */ \
        "pop  es"        /* ---------'| */ \
        "pop  bp"        /* ----------' */ \
    __parm __caller [__si __cx] [__ax __dx] [__bx __di] \
    __value         [__ax] \
    __modify        [__bx __cx __dx __di __si]
#else
#pragma aux DoDosxCall = \
        "push bp"        /* --------. */ \
        "push es"        /* -------.| */ \
        "push bx"        /* ------.|| */ \
        "push ds"        /* -----.||| */ \
        "push dx"        /* ----.|||| */ \
        "mov  es,[bx]"   /*     ||||| */ \
        "mov  bp,6[bx]"  /*     ||||| */ \
        "mov  ax,0[di]"  /*     ||||| */ \
        "mov  bx,2[di]"  /*     ||||| */ \
        "mov  cx,4[di]"  /*     ||||| */ \
        "mov  dx,6[di]"  /*     ||||| */ \
        "mov  si,8[di]"  /*     ||||| */ \
        "mov  di,10[di]" /*     ||||| */ \
        "mov  ds,bp"     /*     ||||| */ \
        "clc"            /*     ||||| */ \
        "int 21h"        /*     ||||| */ \
        "push ds"        /* ---.||||| */ \
        "push di"        /* --.|||||| */ \
        "mov  bp,sp"     /*   ||||||| */ \
        "lds  di,4[bp]"  /*   ||||||| */ \
        "mov  0[di],ax"  /*   ||||||| */ \
        "mov  2[di],bx"  /*   ||||||| */ \
        "mov  4[di],cx"  /*   ||||||| */ \
        "mov  6[di],dx"  /*   ||||||| */ \
        "mov  8[di],si"  /*   ||||||| */ \
        "pop  10[di]"    /* --'|||||| */ \
        "sbb  bp,bp"     /*    |||||| */ \
        "and  bp,1"      /*    |||||| */ \
        "mov  12[di],bp" /*    |||||| */ \
        "pop  bp"        /* ---'||||| */ \
        "pop  bx"        /* ----'|||| */ \
        "pop  bx"        /* -----'||| */ \
        "pop  bx"        /* ------'|| */ \
        "mov  6[bx],bp"  /*        || */ \
        "mov  [bx],es"   /*        || */ \
        "pop  es"        /* -------'| */ \
        "pop  bp"        /* --------' */ \
    __parm __caller [__di] [__dx] [__bx] \
    __value         [__ax] \
    __modify        [__bx __cx __dx __di __si]
#endif

#elif defined( _M_IX86 ) && !defined( __WINDOWS__ )

#pragma aux DoDosCall = \
        "push ebp"         /* -----. */ \
        "push es"          /* ----.| */ \
        "push ds"          /* ---.|| */ \
        "push edx"         /* --.||| */ \
        "mov  eax,[edi]"   /*   |||| */ \
        "mov  ebx,4[edi]"  /*   |||| */ \
        "mov  ecx,8[edi]"  /*   |||| */ \
        "mov  edx,12[edi]" /*   |||| */ \
        "mov  esi,16[edi]" /*   |||| */ \
        "mov  edi,20[edi]" /*   |||| */ \
        "clc"              /*   |||| */ \
        "int 21h"          /*   |||| */ \
        "mov  ebp,edi"     /*   |||| */ \
        "pop  edi"         /* --'||| */ \
        "pop  ds"          /* ---'|| */ \
        "mov  [edi],eax"   /*     || */ \
        "mov  4[edi],ebx"  /*     || */ \
        "mov  8[edi],ecx"  /*     || */ \
        "mov  12[edi],edx" /*     || */ \
        "mov  16[edi],esi" /*     || */ \
        "mov  20[edi],ebp" /*     || */ \
        "sbb  ebp,ebp"     /*     || */ \
        "and  ebp,1"       /*     || */ \
        "mov  24[edi],ebp" /*     || */ \
        "pop  es"          /* ----'| */ \
        "pop  ebp"         /* -----' */ \
    __parm __caller [__edi] [__edx] \
    __value         [__eax] \
    __modify        [__ebx __ecx __edx __edi __esi]

#pragma aux DoDosxCall = \
        "push ebp"         /* --------. */ \
        "push es"          /* -------.| */ \
        "push ebx"         /* ------.|| */ \
        "push ds"          /* -----.||| */ \
        "push edx"         /* ----.|||| */ \
        "mov  es,[ebx]"    /*     ||||| */ \
        "mov  bp,6[ebx]"   /*     ||||| */ \
        "mov  eax,0[edi]"  /*     ||||| */ \
        "mov  ebx,4[edi]"  /*     ||||| */ \
        "mov  ecx,8[edi]"  /*     ||||| */ \
        "mov  edx,12[edi]" /*     ||||| */ \
        "mov  esi,16[edi]" /*     ||||| */ \
        "mov  edi,20[edi]" /*     ||||| */ \
        "mov  ds,ebp"      /*     ||||| */ \
        "clc"              /*     ||||| */ \
        "int 21h"          /*     ||||| */ \
        "push ds"          /* ---.||||| */ \
        "push edi"         /* --.|||||| */ \
        "mov  ebp,esp"     /*   ||||||| */ \
        "lds  edi,8[ebp]"  /*   ||||||| */ \
        "mov  0[edi],eax"  /*   ||||||| */ \
        "mov  4[edi],ebx"  /*   ||||||| */ \
        "mov  8[edi],ecx"  /*   ||||||| */ \
        "mov  12[edi],edx" /*   ||||||| */ \
        "mov  16[edi],esi" /*   ||||||| */ \
        "pop  20[edi]"     /* --'|||||| */ \
        "sbb  ebp,ebp"     /*    |||||| */ \
        "and  ebp,1"       /*    |||||| */ \
        "mov  24[edi],ebp" /*    |||||| */ \
        "pop  ebp"         /* ---'||||| */ \
        "pop  ebx"         /* ----'|||| */ \
        "pop  ebx"         /* -----'||| */ \
        "pop  ebx"         /* ------'|| */ \
        "mov  6[ebx],bp"   /*        || */ \
        "mov  0[ebx],es"   /*        || */ \
        "pop  es"          /* -------'| */ \
        "pop  ebp"         /* --------' */ \
    __parm __caller [__edi] [__edx] [__ebx] \
    __value         [__eax] \
    __modify        [__ebx __ecx __edx __edi __esi]

#endif
