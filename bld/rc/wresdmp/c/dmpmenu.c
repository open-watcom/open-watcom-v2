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
#include "watcom.h"
#include "wresall.h"
#include "global.h"
#include "flag.h"
#include "dmpmenu.h"
#include "rcrtns.h"

static char * MenuOnFlags[16] = {
    "GRAYED",       /* 0x0001 */
    "INACTIVE",     /* 0x0002 */
    "BITMAP",       /* 0x0004 */
    "CHECKED",      /* 0x0008 */
    "POPUP",        /* 0x0010 */
    "MENUBARBREAK", /* 0x0020 */
    "MENUBREAK",    /* 0x0040 */
    "ENDMENU",      /* 0x0080 */
    "OWNERDRAW",    /* 0x0100 */
    NULL,
    NULL,
    NULL,
    NULL,           /* 0x1000 */
    NULL,
    "HELP",         /* 0x4000 */
    NULL
};

/* print nothing if the bit for a menu flag is off */
static char * MenuOffFlags[16] = {
    NULL,           /* 0x0001 */
    NULL,
    NULL,
    NULL,
    NULL,           /* 0x0010 */
    NULL,
    NULL,
    NULL,
    NULL,           /* 0x0100 */
    NULL,
    NULL,
    NULL,
    NULL,           /* 0x1000 */
    NULL,
    NULL,
    NULL
};

static void PrintPopupItem( MenuItemPopup * item )
/************************************************/
{
    printf( "\tPopup item.   Flags: " );
    PrintUint16Flags( item->ItemFlags, MenuOnFlags, MenuOffFlags, 43 );
    printf( "\t    Text: %s\n", item->ItemText );
}

static void PrintNormalItem( MenuItemNormal * item )
/**************************************************/
{
    printf( "\tNormal item.   Id: %5d  Flags: ", item->ItemID );
    PrintUint16Flags( item->ItemFlags, MenuOnFlags, MenuOffFlags, 43 );
    printf( "\t    Text: %s\n", item->ItemText );
}

static void PrintHeader( MenuHeader * head )
/******************************************/
{
    printf( "\tMenu Header.   Version: %d  Header size: %d\n", head->Version,
                head->HeaderSize );
}

bool DumpMenu( uint_32 offset, uint_32 length, WResFileID handle )
/****************************************************************/
{
    bool        error;
    int         prevpos;
    int         depth;  /* number of menu levels deep */
    MenuItem *  item;
    MenuHeader  head;

    length = length;
    prevpos = RCSEEK( handle, offset, SEEK_SET );
    error = (prevpos == -1);

    if( !error ) {
        error = ResReadMenuHeader( &head, handle );
    }
    if( !error ) {
        PrintHeader( &head );
    }

    depth = 1;
    while( depth > 0 && !error ) {
        item = ResNewMenuItem();
        error = ResReadMenuItem( item, handle );
        if( !error ) {
            if( item->IsPopup ) {
                depth++;
                PrintPopupItem( &(item->Item.Popup) );
                if (item->Item.Popup.ItemFlags & MENU_ENDMENU) {
                    depth--;
                }
            } else {
                PrintNormalItem( &(item->Item.Normal) );
                if (item->Item.Normal.ItemFlags & MENU_ENDMENU) {
                    depth--;
                }
            }
        }
        ResFreeMenuItem( item );
    }

    RCSEEK( handle, prevpos, SEEK_SET );

    return( error );
}
