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
* Description:  Stack manipulation for x86 platforms.
*
****************************************************************************/


#include "hostsys.h"


#if defined( __WATCOMC__ ) && defined( _M_IX86 )

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


extern byte near        *bp( void );
#pragma aux bp = 0x89 0xe8 value [ __AX ];

extern byte near        *sp( void );
#pragma aux sp = value [ __SP ];

extern void             setsp( byte near * );
#pragma aux setsp = 0x89 0xc4 parm [ __AX ] modify [ __SP ];

extern void             setbp( byte near * );
#pragma aux setbp = 0x89 0xc5 parm [ __AX ];

#pragma aux SafeRecurse parm caller [ __AX __BX __CX __DX ]; /* just to be sure! */

#else

// nothing special

#endif

extern pointer          SafeRecurse( pointer (*)( pointer ), pointer );
