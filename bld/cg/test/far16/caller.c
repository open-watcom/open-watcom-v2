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
#include <malloc.h>

#define MODIFIER __cdecl

#ifdef __cplusplus
extern "C" {
#endif
extern long __far16 MODIFIER func_16( long __far16 MODIFIER (*p)(char,char), char a, char b );
#ifdef __cplusplus
}
#endif

long C;

long __far16 MODIFIER func_32( char a, char b ) {
    return( a * b + C );
}

int call_16( char a, char b ) {
    return( func_16( func_32, a, b ) );
}

#define STACK_LEFT      0x20

#pragma aux getsp value [ax] = "movzx eax,sp";
extern unsigned short getsp( void );

void main() {
    unsigned short      sp;

    sp = getsp();
    alloca( sp - STACK_LEFT );
    C = 13;
    printf( "%d\n", call_16( 10, 20 ) );
}
