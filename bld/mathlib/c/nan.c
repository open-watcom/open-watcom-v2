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
* Description: Performs a fake fused multiply-add in long double in an
*              attempt to improve accuracy
*
****************************************************************************/

#include <math.h>

_WMRTDATA extern const float    __f_posqnan;
_WMRTDATA extern const double   __d_posqnan;

_WMRTLINK float nanf(const char *ignored)
{
    return __f_posqnan;
}

_WMRTLINK double nan(const char *ignored)
{
    return __d_posqnan;
}

#if defined(_LONG_DOUBLE_) && defined(__X86__)
_WMRTDATA extern const float    __ld_posqnan;
#endif

#ifdef _LONG_DOUBLE_
_WMRTLINK long double nanl(const char *ignored)
#else
_WMRTLINK double nanl(const char *ignored)
#endif
{
#ifdef _LONG_DOUBLE_ 
#ifdef __X86__
    return __ld_posqnan;
#else
    return (long double)__d_posqnan;
#endif
#else
    return nan(ignored);
#endif
}
