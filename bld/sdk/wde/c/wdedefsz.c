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


#include "precomp.h"

#include "wdeglbl.h"
#include "wderesiz.h"
#include "wdeobjid.h"
#include "wdeactn.h"
#include "wdefont.h"
#include "wdedefsz.h"
#include "wresall.h"

/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct WdeDefaultSizeStruct {
    OBJ_ID   obj_id;
    POINT    def_size;
} WdeDefaultSizeStruct;

/****************************************************************************/
/* external function prototypes                                             */
/****************************************************************************/

/****************************************************************************/
/* static function prototypes                                               */
/****************************************************************************/

/* these sizes are in dialog units */
static WdeDefaultSizeStruct WdeDefaultSizes[] = {
    { PBUTTON_OBJ,      { 40, 14 } },
    { CBUTTON_OBJ,      { 40, 10 } },
    { RBUTTON_OBJ,      { 40, 10 } },
    { GBUTTON_OBJ,      { 48, 40 } },
    { FRAME_OBJ,        { 20, 16 } },
    { TEXT_OBJ,         { 20,  8 } },
    { EDIT_OBJ,         { 32, 12 } },
    { LISTBOX_OBJ,      { 48, 40 } },
    { COMBOBOX_OBJ,     { 48, 35 } },
    { HSCROLL_OBJ,      { 48, 10 } },
    { VSCROLL_OBJ,      { 10, 40 } },
    { ICON_OBJ,         { 20, 18 } },
    { SIZEBOX_OBJ,      { 10, 10 } },
    { SBAR_OBJ,         { 32, 12 } },
    { LVIEW_OBJ,        { 48, 40 } },
    { TVIEW_OBJ,        { 48, 40 } },
    { TABCNTL_OBJ,      { 48, 40 } },
    { ANIMATE_OBJ,      { 48, 40 } },
    { UPDOWN_OBJ,       { 10, 20 } },
    { TRACKBAR_OBJ,     { 32, 24 } },
    { PROGRESS_OBJ,     { 32, 12 } },
    { HOTKEY_OBJ,       { 32, 12 } },
    { HEADER_OBJ,       { 48, 12 } },
    { 0,                {  0,  0 } }
};

/****************************************************************************/
/* static variables                                                         */
/****************************************************************************/

POINT *WdeGetDefaultSizeFromOBJID( OBJ_ID id )
{
    int i;

    for( i = 0; WdeDefaultSizes[i].obj_id != NULL; i++ ) {
        if( WdeDefaultSizes[i].obj_id == id ) {
            return( &WdeDefaultSizes[i].def_size );
        }
    }

    return( NULL );
}

Bool WdeChangeSizeToDefIfSmallRect( OBJPTR parent, OBJ_ID id, RECT *size )
{
    WdeResizeRatio  r;
    DialogSizeInfo  d;
    RECT            new_size;
    POINT           *dim;

    if( parent == NULL || size == NULL || id == 0 ) {
        return( FALSE );
    }

    /* check if the objects size is greater than min allowed */
    if( size->right - size->left >= MIN_X || size->bottom - size->top >= MIN_Y ) {
        return( TRUE );
    }

    if( (dim = WdeGetDefaultSizeFromOBJID( id )) == NULL ) {
        return( TRUE );
    }

    if( !Forward( parent, GET_RESIZER, &r, NULL ) ) {
        return( FALSE );
    }

    d.x = 0;
    d.y = 0;
    d.width = dim->x;
    d.height = dim->y;

    if( !WdeDialogToScreen( NULL, &r, &d, &new_size ) ) {
        return( FALSE );
    }

    size->right = size->left + (new_size.right - new_size.left);
    size->bottom = size->top + (new_size.bottom - new_size.top);

    return( TRUE );
}
