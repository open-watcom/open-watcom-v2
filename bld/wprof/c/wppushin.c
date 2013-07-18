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

#include "common.h"
#include "aui.h"
#include "wpaui.h"
#include "dip.h"
#include "sampinfo.h"
#include "memutil.h"


extern void SetSampleInfo(sio_data *curr_sio);

/* Forward declarations */
extern int  WPPopEvent( void );


static int      *wpEventStack = 0;
static void     *wpDataStack = 0;
static int      wpStackSize = 0;
static int      wpDataSize = 0;



extern void *WPPopPtr( void )
/***************************/
{
    void        *data_point;

    if( wpDataSize == 0 ) {
        return( NULL );
    }
    wpDataSize -= sizeof( void * );
    data_point = *(char **)wpDataStack;
    return( data_point );
}



extern void WndDoInput( void )
/****************************/
{
    void    *data_point;
    int     event;

    event = WPPopEvent();
    if( event == WP_SETSAMPS_EVENT ) {
        data_point = WPPopPtr();
        SetSampleInfo( (sio_data *)data_point );
    }
}



extern void WPPushEvent( int event )
/**********************************/
{
    wpStackSize++;
    wpEventStack = ProfRealloc( wpEventStack, wpStackSize * sizeof( int ) );
    wpEventStack[wpStackSize-1] = event;
}



extern void WPPushPtrEvent( int event, void *ptr )
/************************************************/
{
    void    *data_point;

    WPPushEvent( event );
    wpDataSize += sizeof( void * );
    wpDataStack = ProfRealloc( wpDataStack, wpDataSize );
    data_point = (char *)wpDataStack + wpDataSize - sizeof(pointer);
    *(char **)data_point = ptr;
}



extern int WPPopEvent( void )
/***************************/
{
    if( wpStackSize == 0 ) {
        return( WP_NO_EVENT );
    }
    wpStackSize--;
    return( wpEventStack[wpStackSize] );
}
