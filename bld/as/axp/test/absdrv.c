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


extern unsigned int AbsTest1( void );
extern unsigned int AbsTest2( void );
extern unsigned int AbsTest3( void );
extern unsigned int AbsTest4( void );
extern unsigned int AbsTest5( void );
extern unsigned int AbsTest6( void );
extern unsigned int AbsTest7( void );

#define _Test(expr) ((expr)?printf("Passed.\n"):printf("Failed: %s\n",#expr))

void main( void ) {

    _Test( AbsTest1() == 0x1995 );
    _Test( AbsTest2() == 0x1997 );
    _Test( AbsTest3() == 0x12345678 );
    _Test( AbsTest4() == 0x12 );
    _Test( AbsTest5() == 0x12 );
    _Test( AbsTest6() == 0x3456 );
    _Test( AbsTest7() == 0x19951997 );
    exit( 0 );
}
