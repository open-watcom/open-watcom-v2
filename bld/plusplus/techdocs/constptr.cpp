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


// 'int **' cannot be converted to 'const int **'
// this is not allowed because it would allow a constant value to
// be changed which the C/C++ type system is supposed to prevent
// (this example is from the ISO/ANSI C++ draft section 8.2.1)
// see CNSTPTR2.CPP for a simpler example
#include <stdio.h>

const int ci = 10;
const int *pc = &ci;
const int **ppc;
int *p;

void main()
{
    ppc = &p;           // suppose WATCOM C++ allowed this to work...
                        // 'int **' -> 'const int **'
    *ppc = &ci;         // OK
                        // 'const int *' -> 'const int *'
    *p = 5;             // OK (but we just changed 'ci' to 5)
                        // problem is the first assignment in 'main'
                        // is incorrect

    // Borland C++ prints 'ci is now 5 (should be 10)'
    printf( "ci is now %d (should be 10)\n", *pc );
}
