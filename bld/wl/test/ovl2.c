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

extern void ovl1a( void );
extern void ovl1b( void );
extern void ovl1c( void );
extern void ovl3( void );
extern void ovl3a( void );

static void local2( void ) {

    puts( "in local2" );
    ovl3();
    puts( "returned to local2" );
}

void ovl2( void ) {

    puts( "in ovl2" );
    ovl1b();
    puts( "returned to ovl2" );
    local2();
    puts( "returned to ovl2" );
    ovl1a();
    puts( "returned to ovl2" );
    ovl1c();
    puts( "returned to ovl2" );
}

void ovl2a( void ) {
    puts( "in ovl2a" );
    ovl3();
    puts( "returned to ovl2a" );
}

void ovl2b( void ) {
    puts( "in ovl2b" );
    ovl3a();
    puts( "returned to ovl2b" );
}

int cg22( unsigned short c ) {
switch( c ) {
case 0:
case 3:
case 6:
case 9:
case 12:
case 15:
case 18:
case 21:
case 24:
case 27:
case 30:
case 33:
case 36:
case 39:
case 42:
case 45:
case 48:
case 51:
case 54:
case 57:
case 60:
case 63:
return( 1 );
default:
return( 0 );
}
}
