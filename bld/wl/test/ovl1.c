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


#include <stdio.h>
#include <setjmp.h>

extern void root1( void );
extern void ovl2( void );
extern void ovl2a( void );
extern void ovl2b( void );
extern void ovl3a( void );

jmp_buf env1;

void ovl1( void ) {

    puts( "in ovl1a" );
    ovl2();
    puts( "returned to ovl1a" );
}

void (*ptr)(void) = &ovl1;

static void local1( void ) {

    puts( "in local1" );
}

void (*ptr2)(void);

static void local1a( void ) {

    puts( "in local1a" );
}

void ovl1a( void ) {

    ptr2 = &local1a;
    puts( "in ovl1a" );
    local1();
    puts( "returned to ovl1a" );
}

void ovl1b( void ) {

    puts( "in ovl1b" );
    ovl2a();
    puts( "returned to ovl1b" );
}

void ovl1c( void ) {

    puts( "in ovl1c" );
    if( setjmp( env1 ) == 0 ) {
        puts( "first setjmp in ovl1c" );
        ovl2b();
        puts( "bogus! returned to ovl1c after first setjmp!!" );
    } else if( setjmp( env1 ) == 0 ) {
        puts( "second setjmp in ovl1c" );
        root1();
        puts( "bogus! returned to ovl1c after second setjmp!!" );
    }
    puts( "returned to ovl1c after longjmp" );
}
