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


#include <stdlib.h>
#include <dos.h>
#include <extender.h>
#include "uidef.h"
#include "biosui.h"

#define         IRET                    (char) 0xcf

#ifdef __386__
void far *firstmeg( unsigned segment, unsigned offset )
{
    void far    *meg1;

    if( _IsRational() ) {
        meg1 = MK_FP( FP_SEG( &meg1 ), ( segment << 4 ) + offset );
    } else {
        meg1 = MK_FP( REAL_SEGMENT, ( segment << 4 ) + offset );
    }
    return( meg1 );
}
#endif

void far * GetFarPointer( unsigned segment, unsigned offset )
{
    #ifdef __386__
    return( firstmeg( segment, offset ) );
    #else
    return( MK_FP( segment, offset ) );
    #endif
}


bool intern installed( int num )
{
    unsigned short far          *vector;
    char far                    *intrtn;
    // 91/05/15 DEN - major kludge to fix code gen bug
    int                         zero = 0;


#ifdef __386__
    if( _IsPharLap() ) {
        vector = MK_FP( REAL_SEGMENT, num * 4 );
        intrtn = MK_FP( REAL_SEGMENT, ( ((unsigned) vector[1]) << 4 ) + vector[0] );
    } else if( _IsRational() ) {
        vector = GetFarPointer( 0, num * 4 );
        intrtn = GetFarPointer( 0, (((unsigned) vector[1]) << 4) + vector[0] );
    } else {
#endif
        vector = MK_FP( zero, num * 4 );
        intrtn = MK_FP( vector[1], vector[0] );
#ifdef __386__
    }
#endif
    return( ( intrtn != NULL ) && ( *intrtn != IRET ) );
}
