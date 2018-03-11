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
#include <ctype.h>
#include "uidef.h"
#include "uivedit.h"
#include "uiedit.h"


static bool isdelim( int ch )
{
    if( uicharlen( ch ) == 2 )
        return( false );
    if( isalnum( ch ) )
        return( false );
    if( ch == '_' )
        return( false );
    return( true );
}


static void clear_buffer( VBUFFER *buffer )
/*****************************************/
{
    buffer->index = 0;
    memset( buffer->content, ' ', buffer->length );
    buffer->dirty = true;
}


static bool secondbyte( char *buff, char *which )
{
    while( buff < which ) {
        buff += uicharlen( UCHAR_VALUE( *buff ) );
    }
    return( buff > which );
}

ui_event UIAPI uieditevent( ui_event ui_ev, VBUFFER *buffer )
/***********************************************************/
{
    char            *bptr;
    ui_event        new;
    bool            allblank;
    bool            cleared;    /* buffer auto-cleared */
    bool            right;      /* cursor moved right */
    bool            ignore_uicharlen; /* ignore dbcs lead character when moving right */

    new = EV_NO_EVENT;
    cleared = false;
    right = false;
    ignore_uicharlen = false;
    if( ui_ev == EV_INSERT ) {
        buffer->insert = buffer->insert ? false : true ;
    } else if( ( ui_ev == EV_CURSOR_LEFT ) || ( ui_ev == EV_RUB_OUT ) ) {
        if( buffer->index > 0 ) {
            --(buffer->index);
            if( secondbyte( buffer->content, buffer->content + buffer->index ) ) {
                --(buffer->index);
            }
        } else {
            if( ui_ev == EV_CURSOR_LEFT ) {
                new = EV_BUMP_LEFT;
            } else {
                new = EV_JOIN_LEFT;
            }
            ui_ev = EV_NO_EVENT;           /* this cancels the rub out */
        }
    } else if( iseditchar( ui_ev ) ) {
        if( buffer->index < buffer->length ) {
            if( buffer->index == 0 && buffer->auto_clear ) {
                cleared = true;
                clear_buffer( buffer );
            }
            if( buffer->insert ) {
                bptr = buffer->content + buffer->length - 1;
                if( *bptr == ' ' ) {
                    for( ; bptr >= buffer->content + buffer->index; bptr-- ) {
                        *bptr = *(bptr - 1);
                    }
                    buffer->content[buffer->index] = CHAR_VALUE( ui_ev );
                    right = true;
                    buffer->dirty = true;
                }
            } else {
                bptr = (buffer->content + buffer->index);
                if( uicharlen( UCHAR_VALUE( *bptr ) ) == 2 ) {
                    if( uicharlen( UCHAR_VALUE( ui_ev ) ) == 1 ) {
                        *bptr = CHAR_VALUE( ui_ev );
                        for( bptr++; bptr < buffer->content + buffer->length - 1; bptr++ ) {
                            *bptr = *(bptr + 1);
                        }
                        *bptr = ' ';
                    } else {
                        *bptr = CHAR_VALUE( ui_ev );
                    }
                } else {
                    if( uicharlen( UCHAR_VALUE( ui_ev ) ) == 2 ) {
                        bptr = buffer->content + buffer->length - 1;
                        if( *bptr == ' ' ) {
                            for( ; bptr >= buffer->content + buffer->index; bptr-- ) {
                                *bptr = *(bptr - 1);
                            }
                            buffer->content[buffer->index] = CHAR_VALUE( ui_ev );
                            buffer->content[buffer->index + 1] = 'a';
                        }
                        ignore_uicharlen = true;
                    } else {
                        *bptr = CHAR_VALUE( ui_ev );
                    }
                }
                right = true;
                buffer->dirty = true;
            }
        }
    } else if( ui_ev == EV_HOME ) {
        buffer->index = 0;
    } else if( ui_ev == EV_END ) {
        for( bptr = buffer->content + buffer->length; bptr > buffer->content; bptr-- ) {
            if( *(bptr - 1) != ' ' ) {
                break;
            }
        }
        buffer->index = bptr - buffer->content;
    } else if( ui_ev == EV_CTRL_CURSOR_LEFT ) {
        for( bptr = buffer->content + buffer->index; bptr > buffer->content; ) {
            --bptr;
            if( secondbyte( buffer->content, bptr ) )
                --bptr;
            if( !isdelim( UCHAR_VALUE( *bptr ) ) ) {
                break;
            }
        }
        for( ; bptr > buffer->content; ) {
            --bptr;
            if( secondbyte( buffer->content, bptr ) )
                --bptr;
            if( isdelim( UCHAR_VALUE( *bptr ) ) ) {
                ++bptr;
                break;
            }
        }
        buffer->index = bptr - buffer->content;
    } else if( ui_ev == EV_CTRL_CURSOR_RIGHT ) {

        char                    *eptr;   /* end pointer */
        char                    *tptr;   /* blank tail */

        bptr = buffer->content + buffer->index;
        eptr = buffer->content + buffer->length;
        for( ;; ) {
            if( bptr >= eptr )
                break;
            if( isdelim( UCHAR_VALUE( *bptr ) ) )
                break;
            bptr += uicharlen( UCHAR_VALUE( *bptr ) );
        }
        for( tptr = bptr; tptr < eptr; ) {
            if( *tptr == ' ' ) {
                /* do nothing */
            } else if( isdelim( UCHAR_VALUE( *tptr ) ) ) {
                bptr = tptr + 1;       /* position after last non-blank */
            } else {
                bptr = tptr;           /* position on next word */
                break;
            }
            tptr += uicharlen( UCHAR_VALUE( *tptr ) );
        }
        buffer->index = bptr - buffer->content;
    } else if( ui_ev == EV_CTRL_HOME ) {
        clear_buffer( buffer );
    } else if( ui_ev == EV_CTRL_END ) {
        memset( buffer->content + buffer->index, ' ', buffer->length - buffer->index );
        buffer->dirty = true;
    } else if( ui_ev == EV_CURSOR_RIGHT ) {
        allblank = true;
        for( bptr = buffer->content + buffer->index; bptr < buffer->content + buffer->length; ) {
            if( *bptr != ' ' ) {
                allblank = false;
                break;
            }
            if( ignore_uicharlen ) {
                ++bptr;
            } else {
                bptr += uicharlen( UCHAR_VALUE( *bptr ) );
            }
        }
        if( allblank ) {
            new = EV_BUMP_RIGHT;
        } else {
            right = true;
        }
    } else {
        new = ui_ev;
    }
    /* cursor right is handled here so that when characters are typed the
       event can be changed to a cursor right to move the cursor over */
    if( right ) {
        if( buffer->index < buffer->length ) {
            bptr = buffer->content + buffer->index;
            buffer->index += uicharlen( UCHAR_VALUE( *bptr ) );
        }
        if( buffer->index == buffer->length ) {
            new = EV_BUMP_RIGHT;      /* 891206 */
        }
    } else if( ( ui_ev == EV_DELETE ) || ( ui_ev == EV_RUB_OUT ) ) {
        int     delsize;

        allblank = true;
        if( buffer->index < buffer->length ) {
            delsize = uicharlen( UCHAR_VALUE( buffer->content[buffer->index] ) );
            for( bptr = buffer->content + buffer->index; bptr < buffer->content + buffer->length - delsize; bptr++ ) {
                if( *bptr != ' ' ) {
                    allblank = false;
                }
                *bptr = *(bptr + delsize);
            }
            *bptr = ' ';
            if( delsize == 2 ) {
                *++bptr = ' ';
            }
            buffer->dirty = true;
        }
        if( allblank && ( ui_ev == EV_DELETE ) ) {
            new = EV_JOIN_RIGHT;
        } else {
            new = EV_NO_EVENT;
        }
    }
    if( (new == EV_NO_EVENT || new == EV_BUFFER_CLEAR) && ui_ev != EV_INSERT ) {
        buffer->auto_clear = false;
    }
    if( cleared ) {
        if( uiinlists( EV_BUFFER_CLEAR ) ) {
            new = EV_BUFFER_CLEAR;
        }
    }
    return( new );
}
