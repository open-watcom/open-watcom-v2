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


// Why K&R style function declarations cannot be supported in C++
typedef int INT;
typedef double DOUBLE;

// function 'f1' with unnamed arguments
void f1( INT, DOUBLE )
{
}

// typedef-names can be used as arguments names
#if 0
// function 'f2' with named K&R style arguments (Zortech rejects this)
int f2( INT, DOUBLE )
int INT;
int DOUBLE;
{
    return INT + DOUBLE;
}
#else
int f2( int INT, int DOUBLE )
{
    return INT + DOUBLE;
}
#endif

// declaration of variable 'f3' initialized to 'a' (Zortech rejects this)
int a;
int f3( a );

// declaration of variable 'f3' initialized to comma-expression " a, f3 "
int f4( a, f3 );                        // (Zortech rejects this)
