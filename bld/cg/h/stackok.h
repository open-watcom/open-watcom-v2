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


#if defined(__386__) || defined(M_I86)

#if defined(__386__)
    #define __AX eax
    #define __BX ebx
    #define __CX ecx
    #define __DX edx
    #define __SP esp
#else
    #define __AX ax
    #define __BX bx
    #define __CX cx
    #define __DX dx
    #define __SP sp
#endif


extern char near        *bp();
#pragma aux bp = 0x89 0xe8 value [ __AX ];

extern char near        *sp();
#pragma aux sp = value [ __SP ];

extern void             setsp(char near *);
#pragma aux setsp = 0x89 0xc4 parm [ __AX ] modify [ __SP ];

extern void             setbp(char near *);
#pragma aux setbp = 0x89 0xc5 parm [ __AX ];

extern char near *__STACKLOW;
#define stacklow() __STACKLOW
#pragma aux __STACKLOW "*";

#pragma aux SafeRecurse parm caller [ __AX __BX __CX __DX ]; /* just to be sure! */

#else

// nothing special

#endif

extern pointer          SafeRecurse( pointer (*)(), pointer );
