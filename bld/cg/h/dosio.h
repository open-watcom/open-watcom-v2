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

typedef unsigned_16     handle;
typedef signed_32       dosret;

#define HStdIn  0
#define HStdOut 1
#define HStdErr 2

#if _HOSTOS & ( _PLDT | _WIN386 )
    #define _SETCF 0xD1 0xD0            /* rcl eax,1 */ \
                   0xD1 0xC8            /* ror eax,1 */
    #define __DX edx
    #define __CX ecx
    #define _INT21
    #define _DS
#elif _HOSTOS & _DOS
    #define _SETCF 0x19 0xD2       /*      sbb     dx,dx */
    #define __DX dx
    #define __CX cx
    #if defined( __SMALL__ ) || defined( __MEDIUM__ )
        #define _DS
    #else
        #define _DS ds
    #endif
    #define _INT21
#elif _HOSTOS & (_OS220 | _OSTWO | _NT)
    #undef _INT21
#endif

extern  dosret          FPut(handle,char*,unsigned int);
#ifdef _INT21
    #pragma aux FPut = \
        0xB4 0x40       /*      mov     ah,40H */ \
        0xCD 0x21       /*      int     21H */ \
        _SETCF \
        parm routine [ bx ] [ _DS __DX ] [ __CX ] \
        ;
#endif

extern  dosret          FGet(handle,char*,unsigned int);
#ifdef _INT21
    #pragma aux FGet = \
        0xB4 0x3F       /*      mov     ah,3FH */ \
        0xCD 0x21       /*      int     21H */ \
        _SETCF \
        parm routine [ bx ] [ _DS __DX ] [ __CX ] \
        ;
#endif

extern  dosret          FOpen(char*,unsigned int);
#ifdef _INT21
    #pragma aux FOpen = \
        0xB4 0x3D       /*      mov     ah,3DH */ \
        0xCD 0x21       /*      int     21H */ \
        _SETCF \
        parm routine [ _DS __DX ] [ al ]       \
        ;
#endif


extern  dosret          FClose(handle);
#ifdef _INT21
  #pragma aux FClose = \
        0xB4 0x3E       /*      mov     ah,3EH */ \
        0xCD 0x21       /*      int     21H */ \
        _SETCF \
        parm routine [ bx ]      \
        ;
#endif

extern  dosret          FCreate(char*,unsigned int);
#ifdef _INT21
    #pragma aux FCreate = \
        0xB4 0x3C       /*      mov     ah,3CH */ \
        0xCD 0x21       /*      int     21H */ \
        _SETCF \
        parm routine [ _DS __DX ] [ cx ]       \
        ;
#endif


extern  dosret          LSeek(handle,char,long);
#ifdef _INT21
  #if _HOSTOS & _DOS
    #pragma aux LSeek = \
        0xB4 0x42       /*      mov     ah,42H */ \
        0x89 0xF2       /*      mov     dx,si */ \
        0xCD 0x21       /*      int     21H */ \
        0x19 0xC9       /*      sbb     cx,cx */ \
        0x09 0xCA       /*      or      dx,cx */ \
        parm routine [ bx ] [ al ] [ cx si ] \
        ;
  #else
    #pragma aux LSeek = \
        0x66 0x8b 0xd1          /* mov     dx,cx */ \
        0xc1 0xe9 0x10          /* shr     ecx,10H */ \
        0xB4 0x42               /* mov     ah,42H */ \
        0xCD 0x21               /* int     21H */ \
        0x66 0x1b 0xc9          /* sbb     cx,cx */ \
        0x66 0x0b 0xd1          /* or      dx,cx */ \
        0xc1 0xe0 0x10          /* shl     eax,10H */ \
        0x66 0x0b 0xc2          /* or      ax,dx */ \
        0xc1 0xc8 0x10          /* ror     eax,10H */ \
        parm routine [ bx ] [ al ] [ ecx ] \
        modify  [ edx ] \
        ;
  #endif
#endif

extern  dosret          FErase(char*);
#ifdef _INT21
    #pragma aux FErase = \
        0xB4 0x41       /*      mov     ah,41H */ \
        0xCD 0x21       /*      int     21H */ \
        _SETCF \
        parm routine [ _DS __DX ] \
        ;
#endif
