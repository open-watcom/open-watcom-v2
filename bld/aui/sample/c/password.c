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
* Description:  WHEN YOU FIGURE OUT WHAT THIS MODULE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include "app.h"

#define R0 0
#define R1 2
#define C0 1
#define W 48
#define BW 12
#define B1 BUTTON_POS( 1, 2, W, BW )
#define B2 BUTTON_POS( 2, 2, W, BW )

#define DLG_NEW_ROWS    4
#define DLG_NEW_COLS    W
#define DLG_MAX_COLS    70


static gui_control_info Controls[] = {

    DLG_INVISIBLE_EDIT( "", CTL_NEW_EDIT,               C0, R0, W-1 ),

    DLG_DEFBUTTON( "OK", CTL_NEW_OK,                    B1, R1, B1+BW ),
    DLG_BUTTON( "Cancel", CTL_NEW_CANCEL,               B2, R1, B2+BW ),
};

extern  void    Password( char *text, char *buff, unsigned buff_len )
{
    DlgNewWithCtl( text, buff, buff_len,
                   Controls, ArraySize( Controls ), DlgNewEvent,
                   DLG_NEW_ROWS, DLG_NEW_COLS, DLG_MAX_COLS );
}
