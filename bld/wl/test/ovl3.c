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

extern jmp_buf env1;

extern  void (*_close_ovl_file)(void);

void ovl3a( void ) {
    puts( "in ovl3a" );
    longjmp( env1, 1 );
    puts( "bogus! returned to ovl3a after longjmp!!" );
}

void ovl3( void ) {
    puts( "in ovl3" );
    puts( "closing overlay file" );
    _close_ovl_file();
    puts( "overlay file closed" );
}
int cg24( unsigned short c ) {
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
case 66:
case 69:
return( 1 );
default:
return( 0 );
}
}
int cg26( unsigned short c ) {
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
case 66:
case 69:
case 72:
case 75:
return( 1 );
default:
return( 0 );
}
}
int cg28( unsigned short c ) {
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
case 66:
case 69:
case 72:
case 75:
case 78:
case 81:
return( 1 );
default:
return( 0 );
}
}
int cg30( unsigned short c ) {
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
case 66:
case 69:
case 72:
case 75:
case 78:
case 81:
case 84:
case 87:
return( 1 );
default:
return( 0 );
}
}
