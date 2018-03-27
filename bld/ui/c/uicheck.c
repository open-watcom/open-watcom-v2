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


#include "uidef.h"


static void within( register SAREA inside, register SAREA outside )
/*****************************************************************/
{
//  if( ( inside.row < 0 ) || ( inside.col < 0 ) ) {
//      uibarf();
//  }
//  if( ( inside.height < 0 ) || ( inside.width < 0 ) ) {
//      uibarf();
//  }
    if( inside.row + inside.height > outside.height ) {
        uibarf();
    }
    if( inside.col + inside.width > outside.width ) {
        uibarf();
    }
}


void intern okarea( SAREA area )
/******************************/
{
    SAREA           scr;

    scr.row = 0;
    scr.col = 0;
    scr.height = UIData->height;
    scr.width = UIData->width;
    within( area, scr );
}


void intern oksubarea( SAREA inside, SAREA outside )
/**************************************************/
{
    okarea( outside );
    within( inside, outside );
}


void intern okline( ORD row, ORD col, uisize len, SAREA area )
/************************************************************/
{
    SAREA       line;

    line.row = row;
    line.col = col;
    line.height = 1;
    line.width = len;
    oksubarea( line, area );
}


void intern okopen( VSCREEN *vptr )
/*********************************/
{
    if( !(vptr->open) ) {
        uibarf();
    }
}
