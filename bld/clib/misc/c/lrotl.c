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
#include <stdlib.h>
#include <limits.h>

#undef _lrotl

extern  unsigned long __lrotl(unsigned long,unsigned);

#if defined(__AXP__)
#elif defined(__PPC__)
#elif defined(__386__)
#pragma aux __lrotl = "rol eax,cl" parm [eax] [ecx] value [eax] modify [ecx];
#else
#pragma aux __lrotl =   "    and cx,31"\
                        "    je  short L2"\
                        "L1: adc ax,ax"\
                        "    adc dx,dx"\
                        "    adc ax,0"\
                        "    dec cx"\
                        "    jne L1"\
                        "L2:" \
                        parm [ax dx] [cx] value [ax dx];
#endif


_WCRTLINK unsigned long _lrotl( unsigned long value, unsigned int shift )
{
    #if defined(__AXP__) || defined(__PPC__)
        unsigned long tmp;
        tmp = value;
        value = value << shift;
        tmp = tmp >> ((sizeof(tmp)*CHAR_BIT)-shift);
        value = value | tmp;
        return( value );
    #else
        return( __lrotl( value, shift ) );
    #endif
}
