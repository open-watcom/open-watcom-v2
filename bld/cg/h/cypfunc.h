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


#include "hostsys.h"

extern  byte            *CypCopy(byte*,byte*,uint);
extern  char            *CypFill(byte*,uint,byte);
extern  uint            CypLength(char*);
extern  bool            CypEqual(byte*,byte*,uint);

#if !(defined(__386__) || defined(M_I86))
#else
#if defined(__FLAT__) || defined(__SMALL__) || defined(__MEDIUM__)
    #define _SAVES          0x06            /*      push    es */
    #define _RESES          0x07            /*      pop     es */
    #define _SETES          0x1E            /*      push    ds */ \
                            0x07            /*      pop     es */
    #define __ES
    #define __DS
#else
    #define _SAVES
    #define _RESES
    #define _SETES
    #define __ES es
    #define __DS ds
#endif
#if defined(M_I86)
    #define __AX ax
    #define __CX cx
    #define __SI si
    #define __DI di
#elif defined(__386__)
    #define __AX eax
    #define __CX ecx
    #define __SI esi
    #define __DI edi
#endif

#pragma aux CypCopy = \
        _SAVES \
        _SETES \
        0xF3            /*      rep     */ \
        0xA4            /*      movsb   */ \
        _RESES \
        parm routine [ __DS __SI ] [ __ES __DI ] [ __CX ] \
        value [ __ES __DI ] \
        ;

#pragma aux CypFill = \
        _SAVES \
        _SETES \
        0xF3            /*      rep     */ \
        0xAA            /*      stosb   */ \
        _RESES \
        parm routine [ __ES __DI ] [ __CX ] [ al ] \
        value [ __ES __DI ] \
        ;

#pragma aux CypLength = \
        _SAVES \
        _SETES \
        0x31 0xC0       /*      xor     ax,ax */ \
        0x31 0xC9       /*      xor     cx,cx */ \
        0x49            /*      dec     cx */ \
        0xF2            /*      repne   */ \
        0xAE            /*      scasb   */ \
        0xF7 0xD1       /*      not     cx */ \
        0x49            /*      inc     cx */ \
        _RESES \
        parm routine [ __ES __DI ] \
        value        [ __CX ]  \
        modify[ __AX ] \
        ;

#pragma aux CypEqual = \
        _SAVES \
        _SETES \
        0x31 0xC0       /*      xor     ax,ax */ \
        0xF3            /*      repe    */ \
        0xA6            /*      cmpsb   */ \
        0x75 0x01       /*      jne     L1 */ \
        0x48            /*      dec     ax */ \
                        /*L1:   */ \
        _RESES \
        parm routine [ __DS __SI ] [ __ES __DI ] [ __CX ] \
        value [ al ] \
        ;
#endif
