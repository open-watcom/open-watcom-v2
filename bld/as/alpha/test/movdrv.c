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


extern unsigned int MovTest1( void );
extern unsigned int MovTest2( void );
extern unsigned int MovTest3( void );
extern unsigned int MovTest4( void );

#define _Test(expr) ((expr)?printf("Passed.\n"):printf("Failed: %s\n",#expr))

void main( void ) {

    _Test( MovTest1() == 0x48 );
    _Test( MovTest2() == 0x1234 );
    _Test( MovTest3() == 0x12345678 );
    _Test( MovTest4() == 0x7fff89ab );
    _Test( LdaTest1() == 0x38 );
    _Test( LdaTest2() == 0x1224 );
    _Test( LdaTest3() == 0x1244 );
    _Test( LdaTest4() == 0x12345688 );
    _Test( LdaTest5() == 0x7fff89ac );
    exit( 0 );
}
