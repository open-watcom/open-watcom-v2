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


#include <stdlib.h>

typedef struct  {
        unsigned long   quot;
        unsigned long   rem;
} uldiv_t;

#ifdef __386__
#pragma aux u_ldiv parm [eax] [ebx] [esi] modify [edx] = \
        "xor edx, edx" \
        "div ebx" \
        "mov [esi], eax" \
        "mov 4[esi], edx";
#else
extern void     __U4D(void);

#pragma aux u_ldiv parm [dx ax] [cx bx] [es si] = \
        "call   __U4D"          \
        "mov    es:0[si], ax"   \
        "mov    es:2[si], dx"   \
        "mov    es:4[si], bx"   \
        "mov    es:6[si], cx"
#endif

extern void u_ldiv( unsigned long a, unsigned long b, uldiv_t *buff );
