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


static void backblank( SAREA area, void *dummy )
/**********************************************/
{
    register    ORD                     row;

    dummy=dummy;
    for( row = area.row; row < area.row + area.height; ++row ) {
        bfill( &UIData->screen, row, area.col, UIData->attrs[ATTR_NORMAL], ' ', area.width );
    }
}


static void backfill( SAREA area, void *dummy )
/*********************************************/
{
    register    ORD                     row;

    dummy=dummy;
    for( row = area.row; row < area.row + area.height; ++row ) {
        uibcopy( &UIData->blank.type.buffer, row, area.col,
               &UIData->screen, row, area.col, area.width );
    }
}

void uirestorebackground( void )
/******************************/
{
    ATTR        hold;

    hold = UIData->attrs[ ATTR_NORMAL ];
    UIData->attrs[ ATTR_NORMAL ] = 0x07;
    uidirty( UIData->blank.area );
    uirefresh();
    UIData->attrs[ ATTR_NORMAL ] = hold;
}


void intern openbackground( void )
/********************************/
{
    UIData->blank.area.row = 0;
    UIData->blank.area.col = 0;
    UIData->blank.area.height = UIData->height;
    UIData->blank.area.width = UIData->width;
    UIData->blank.priority = P_BACKGROUND;
    UIData->blank.update = backblank;
    UIData->blank.parm = (void *)0xbb;// NULL is reserved for CGUI screens!
    openwindow( &UIData->blank );
}


void intern closebackground( void )
/*********************************/
{
    closewindow( &UIData->blank );
}


BUFFER * UIAPI uibackgroundbuffer( void )
/****************************************/
{
    register    bool                    ok;

    if( UIData->blank.type.buffer.origin != NULL ){
        ok = TRUE;
    } else {
        ok = balloc( &UIData->blank.type.buffer, UIData->height, UIData->width );
    }
    if( ok ) {
        UIData->blank.update = backfill;
        UIData->blank.parm = (void *)0xbb;// NULL is reserved for CGUI screens!
        //UIData->blank.parm = NULL;       // ... just put in any old value
        return( &UIData->blank.type.buffer );
    }
    return( NULL );
}

bool UIAPI uiremovebackground( void )
/************************************/
{
    if( UIData->blank.type.buffer.origin != NULL ){
        bfree( &UIData->blank.type.buffer );
        UIData->blank.type.buffer.origin = NULL;
    }
    UIData->blank.update = backblank;
    UIData->blank.parm = &UIData->attrs[ ATTR_NORMAL];
    return( TRUE );
}

bool UIAPI uikeepbackground( void )
/**********************************/
{
    register    ORD                     row;
    register    BUFFER                  *buff;

    buff = uibackgroundbuffer();
    if( buff ) {
        for( row = 0; row < UIData->height; ++ row ) {
            uibcopy( &UIData->screen, row, 0,
                   buff, row, 0, UIData->width );
        }
    }
    return( buff != NULL );
}

