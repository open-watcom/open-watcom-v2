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


#include <string.h>
#include "uidef.h"


static void blankarea( SAREA area, ATTR attr )
/********************************************/
{
    ORD         row;
    SAREA       up_area;

    for( row = area.row; row < area.row + area.height; ++row ) {
        bfill( &UIData->screen, row, area.col, attr, ' ', area.width );
    }
    up_area.row = 0;
    up_area.col = 0;
    up_area.height = UIData->height;
    up_area.width = UIData->width;
    physupdate( &up_area );
}


void UIAPI uiblankattr( ATTR attr )
/**********************************/
{
    blankarea( UIData->blank.area, attr );
}


void UIAPI uiblankarea( SAREA area )
/***********************************/
{
    blankarea( area, UIData->attrs[ ATTR_NORMAL ] );
}


void UIAPI uiblankscreen( void )
/*******************************/
{
    blankarea( UIData->blank.area, 0x07 );
}
