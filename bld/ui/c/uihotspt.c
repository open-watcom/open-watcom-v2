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



 /************************************************************************
 *       Copyright (C) 1989, by PACE Computing Solutions Inc. All rights *
 *       reserved. No part of this software may be reproduced            *
 *       in any form or by any means - graphic, electronic or            *
 *       mechanical, including photocopying, recording, taping           *
 *       or information storage and retrieval systems - except           *
 *       with the written permission of PACE Computing Solutions Inc.    *
 *************************************************************************/

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "uidef.h"
#include "uivedit.h"
#include "uiledit.h"
#include "uidialog.h"
#include "uigchar.h"

static a_hot_spot_field     *ActiveField = NULL;

static ORD hs_adjust( pos, width )
    int                 pos;
    ORD                 width;
{
    if( pos < 0 ){
        return( width + pos - 1 );
    } else {
        return( pos );
    }
}

#define HOT_BUFFER      80

char uidrawhottext( VSCREEN *vs, char *str, SAREA *parea,
                    ATTR attr, ATTR hotattr,
                    bool hidden, bool no_hotkey, bool push_button )
{
    char        buf[HOT_BUFFER];
    SAREA       area;
    int         offset;
    int         slen;
    char        hotkey;
    int         hotindex;

    hotkey = '\0';
    memset( buf, '\0', HOT_BUFFER );
    slen = 0;
    hotindex = -1;
    area = *parea;
    if( str != NULL ) {
        while( slen < area.width && *str != '\0' ) {
            if( *str == '&' ) {
                if ( !no_hotkey && !hotkey ) {
                    hotkey = tolower( str[1] );
                    hotindex = slen;
                }
            } else {
                buf[ slen++ ] = *str;
            }
            ++str;
        }
    }

    uivfill( vs, area, attr, ' ' );

    if( area.width < 2 ) return( hotkey );

    if( push_button ) {
        offset = ( (int)area.width - slen ) / 2;
        if( offset < 0 ) {
            offset = 0;
        }
        /* allow for shadow */
        area.width -= 1;
    } else {
        offset = 0;
    }

    /* draw text */
    if( !hidden ) {
        uivtextput( vs, area.row, area.col + offset, attr, buf, area.width - offset );
    }

    /* draw hotkey */
    if( hotkey != '\0' ) {
        uivtextput( vs, area.row, area.col + offset + hotindex, hotattr,
                    &buf[hotindex], 1 );
    }

    return( hotkey );
}

char uihotspot( VSCREEN *vs, char *str, SAREA *parea, unsigned short flags )
{
    ATTR        attr, hotattr;
    char        buf[HOT_BUFFER];
    char        hotkey;

    if( ( flags & HOT_HIDDEN ) != 0 ) {
        attr = UIData->attrs[ ATTR_NORMAL ];
        hotattr = attr;
    } else if( ( flags & HOT_CURRENT ) != 0 ) {
        attr = UIData->attrs[ ATTR_CURR_HOTSPOT ];
        hotattr = UIData->attrs[ ATTR_CURR_HOTSPOT_KEY ];
    } else if( ( flags & HOT_DEFAULT ) != 0 ) {
        attr = UIData->attrs[ ATTR_DEFAULT_HOTSPOT ];
        hotattr = attr;
    } else {
        attr = UIData->attrs[ ATTR_HOTSPOT ];
        hotattr = UIData->attrs[ ATTR_DEFAULT_HOTSPOT ];
    }

    hotkey = uidrawhottext( vs, str, parea, attr, hotattr,
                            (bool)(flags & HOT_HIDDEN),
                            (bool)(flags & HOT_NO_KEY), TRUE );

    if( parea->height < 1 ) return( hotkey );   /* no room for shadow */
    memset( buf, '\0', HOT_BUFFER );
    if( ( flags & ( HOT_HIDDEN | HOT_ACTIVE ) ) != 0 ) {
        /* wipe out the shadow */
        uivtextput( vs, parea->row, parea->col + parea->width - 1,
                UIData->attrs[ ATTR_NORMAL ], buf, 1 );
        if( parea->height >= 2 ) {
            uivtextput( vs, parea->row + 1, parea->col,
                        UIData->attrs[ ATTR_NORMAL ], buf, parea->width );
        }
    } else {
        /* draw the shadow */
        buf[0] = UiGChar[ UI_SHADOW_RIGHT ];
        uivtextput( vs, parea->row, parea->col + parea->width - 1,
                UIData->attrs[ ATTR_SHADOW ], buf, 1 );

        if( parea->height >= 2 ) {
            buf[0] = UiGChar[ UI_SHADOW_B_LEFT ];
            memset( &buf[1], UiGChar[ UI_SHADOW_BOTTOM ], parea->width - 2 );
            buf[ parea->width - 1 ] = UiGChar[ UI_SHADOW_B_RIGHT ];
            uivtextput( vs, parea->row + 1, parea->col,
                        UIData->attrs[ ATTR_SHADOW ], buf, parea->width );
        }
    }

    return( hotkey );
}

static void uidisplayhotspot( VSCREEN *w, a_hot_spot_field *hs )
{
    hs->ptr->flags |= uihotspot( w, hs->ptr->str, &hs->area, hs->ptr->flags );
}

void uiposnhotspots( VSCREEN *w, void *field )
{
    a_hot_spot_field    *hs = field;

    for( ; hs->typ != FLD_VOID; ++hs ){
        if( hs->typ == FLD_HOT ){
            if( hs->ptr->length == 0 ){
                if( hs->ptr->str != NULL ){
                    hs->area.width = strlen( hs->ptr->str ) + 1;
                }
                if( hs->area.width < 9 ) {
                    /* no tiny buttons */
                    hs->area.width = 9;
                }
            } else {
                hs->area.width = hs->ptr->length;
            }
            hs->area.row = hs_adjust( hs->ptr->row, w->area.height );
            hs->area.col = hs_adjust( hs->ptr->startcol, w->area.width );
            if( hs->ptr->startcol < 0 ) {
                hs->area.col += -hs->area.width + 1;
            }
            hs->area.height = 2;
            if( hs->ptr->event == EV_ENTER ) {
                hs->ptr->flags |= HOT_DEFAULT;
            }
        }
    }
}

void uiprinthotspots( VSCREEN *w, void *field )
{
    a_hot_spot_field    *hs = field;

    for( ; hs->typ != FLD_VOID; ++hs ){
        if( hs->typ == FLD_HOT ){
            uidisplayhotspot( w, hs );
        }
    }
}

void uioffhotspots( VSCREEN *w, void *field )
{
    a_hot_spot_field    *hs = field;

    for( ; hs->typ != FLD_VOID; ++hs ){
        if( hs->typ == FLD_HOT ){
            uihotspot( w, hs->ptr->str, &hs->area, hs->ptr->flags | HOT_ACTIVE );
        }
    }
    uirefresh();
}

EVENT uihotspotfilter( VSCREEN *w, void *field, EVENT ev )
{
    int                 row, col;
    a_hot_spot_field    *hsf;

    for( hsf = field ; hsf->typ != FLD_VOID; ++hsf ){
        if( hsf->typ != FLD_HOT ) continue;
        switch( ev ) {
            case ' ':
            case EV_ENTER:
                if( hsf->ptr->flags & HOT_CURRENT ) {
                    return( hsf->ptr->event );
                }
                break;
        }
    }
    for( hsf = field ; hsf->typ != FLD_VOID; ++hsf ){
        if( hsf->typ != FLD_HOT ) continue;
        if( hsf->ptr->flags & HOT_HIDDEN ) continue;
        switch( ev ) {
            case EV_ENTER:
                if( hsf->ptr->flags & HOT_DEFAULT ) {
                    return( hsf->ptr->event );
                }
                break;
            case EV_MOUSE_RELEASE:
            case EV_MOUSE_PRESS:
            case EV_MOUSE_DCLICK :
            case EV_MOUSE_REPEAT:
            case EV_MOUSE_DRAG:
                uimousepos( w, &row, &col );
                if( hsf->area.row == row  && hsf->ptr->str != NULL  &&
                    hsf->area.col <= col  &&
                    hsf->area.col + hsf->area.width > col ) {
                        if( ev == EV_MOUSE_PRESS || ev == EV_MOUSE_DCLICK ) {
                            ActiveField = hsf;
                            ActiveField->ptr->flags |= HOT_ACTIVE;
                            uidisplayhotspot( w, ActiveField );
                            ev = EV_NO_EVENT;
                        } else if( ev == EV_MOUSE_RELEASE  &&  hsf == ActiveField ) {
                            ActiveField->ptr->flags &= (~HOT_ACTIVE);
                            uidisplayhotspot( w, ActiveField );
                            ActiveField = NULL;
                            ev = hsf->ptr->event;
                        } else if( hsf == ActiveField  &&
                                !( hsf->ptr->flags & HOT_ACTIVE ) ) {
                            hsf->ptr->flags |= HOT_ACTIVE;
                            uidisplayhotspot( w, hsf );
                            ev = EV_NO_EVENT;
                        }
                } else if( hsf->ptr->flags & HOT_ACTIVE ) {
                    hsf->ptr->flags &= (~HOT_ACTIVE);
                    uidisplayhotspot( w, hsf );
                }
                break;
        }
    }
    return( ev );
}

bool uiisdefaulthotspot( void *field, EVENT ev )
{
    a_hot_spot_field    *hsf;

    for( hsf = field ; hsf->typ != FLD_VOID; ++hsf ){
        if( hsf->typ != FLD_HOT ) continue;
        if( hsf->ptr->flags & HOT_DEFAULT ) {
            return( hsf->ptr->event == ev );
        }
    }
    return( FALSE );
}
