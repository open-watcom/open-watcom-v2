/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2013 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  This file tests the access of the C library from C++. This
*               test probably shouldn't be in the OWSTL tests since it doesn't
*               really relate to the STL. However, I'm not clear where else
*               it should go so for now here it is. If other C library
*               related tests are later devised, perhaps a new directory can
*               be made for them.
*
****************************************************************************/

// Some C libraries don't use extern "C" in their header files. For a
// C++ program to use such libraries, it will be necessary for that
// program to enclose the entire header in an extern "C". However, third
// party C library headers will most likely include standard library
// headers, so the example below needs to work. It's a bit tricky, since
// the underlying headers cstdlib and cmath contain some declarations
// (in particular declarations of overloads) that can't be enclosed in
// an extern "C". Those declarations have to be shielded in some cases
// and yet made visible if those headers are "directly" included.

extern "C" {
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <iso646.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wchar.h>
#include <wctype.h>
}

#include <cstdlib>  // Introduces std::abs(long)
#include <cmath>    // Introduces std::abs(double)

#include <stdlib.h> // Neither of these should cause any problems.
#include <cstdlib>

int main()
{
  int    (*p1)(int   ) = std::abs;
  long   (*p2)(long  ) = std::abs;
  double (*p3)(double) = std::abs;

  return 0;
}
