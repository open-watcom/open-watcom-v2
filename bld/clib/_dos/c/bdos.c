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
* Description:  DOS system call (interrupt 21h).
*
****************************************************************************/


#include "variety.h"
#include <dos.h>
#include "tinyio.h"


extern  int     DoBDosCall( unsigned ax, unsigned dx );
#if defined(__386__)
    #pragma aux DoBDosCall =    \
            _INT_21             \
        __parm __caller     [__eax] [__edx] \
        __value             [__eax] \
        __modify __exact    [__eax __ebx __ecx __edx __esi __edi]
#else
    #pragma aux DoBDosCall =    \
            "int 21h"           \
        __parm __caller     [__ax] [__dx] \
        __value             [__ax] \
        __modify __exact    [__ax __bx __cx __dx __si __di]
#endif


_WCRTLINK int bdos( int dosfn, unsigned dx, unsigned int al )
{
    return( DoBDosCall( (dosfn << 8) + (al & 0xff), dx ) );
}
