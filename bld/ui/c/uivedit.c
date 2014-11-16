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

static int isdelim( char ch )
{
    if( uicharlen( ch ) == 2 ) return( FALSE );
    if( isalnum( ch ) ) return( 0 );
    if( ch == '_' ) return( 0 );
    return( 1 );
}


 static void clear_buffer( VBUFFER *buffer )
/******************************************/
{
    buffer->index = 0;
    memset( buffer->content, ' ', buffer->length );
    buffer->dirty = TRUE;
}


int intern secondbyte( char *buff, char *which )
{
    while( buff < which ) {
        buff += uicharlen( *buff );
    }
    return( buff > which );
}

EVENT UIAPI uieditevent( EVENT ev, VBUFFER *buffer )
/***************************************************/
{
    register    char                    *bptr;
    register    EVENT                   new;
    register    bool                    allblank;
    register    bool                    cleared;    /* buffer auto-cleared */
    register    bool                    right;      /* cursor moved right */
    register    bool                    ignore_uicharlen; /* ignore dbcs lead character when moving right */

    new = EV_NO_EVENT;
    cleared = FALSE;
    right = FALSE;
    ignore_uicharlen = FALSE;
    if( ev == EV_INSERT ) {
        buffer->insert = buffer->insert ? FALSE : TRUE ;
    } else if( ( ev == EV_CURSOR_LEFT ) || ( ev == EV_RUB_OUT ) ) {
        if( buffer->index > 0 ) {
            --(buffer->index);
            if( secondbyte( buffer->content, buffer->content + buffer->index ) ) {
                --(buffer->index);
            }
        } else {
            if( ev == EV_CURSOR_LEFT ) {
                new = EV_BUMP_LEFT;
            } else {
                new = EV_JOIN_LEFT;
            }
            ev = EV_NO_EVENT;           /* this cancels the rub out */
        }
    } else if( iseditchar( ev ) ) {
        if( buffer->index < buffer->length ) {
            if( buffer->index == 0 && buffer->auto_clear ) {
                cleared = TRUE;
                clear_buffer( buffer );
            }
            if( buffer->insert ) {
                bptr = buffer->content + buffer->length - 1;
                if( *bptr == ' ' ) {
                    while( bptr >= buffer->content + buffer->index ) {
                        *bptr = *(bptr-1);
                        --bptr;
                    }
                    *(buffer->content + buffer->index) = (char) ev;
                    right = TRUE;
                    buffer->dirty = TRUE;
                }
            } else {
                bptr = (buffer->content + buffer->index);
                if( uicharlen( *bptr ) == 2 ) {
                    if( uicharlen( (char)ev ) == 1 ) {
                        *bptr = (char) ev;
                        ++bptr;
                        while( bptr < buffer->content + buffer->length - 1 ) {
                            *bptr = *(bptr+1);
                            ++bptr;
                        }
                        *bptr = ' ';
                    } else {
                        *bptr = (char) ev;
                    }
                } else {
                    if( uicharlen( (char)ev ) == 2 ) {
                        bptr = buffer->content + buffer->length - 1;
                        if( *bptr == ' ' ) {
                            while( bptr >= buffer->content + buffer->index ) {
                                *bptr = *(bptr-1);
                                --bptr;
                            }
                            *(buffer->content + buffer->index) = (char) ev;
                            *(buffer->content + buffer->index + 1) = 'a';
                        }
                        ignore_uicharlen = TRUE;
                    } else {
                        *bptr = (char) ev;
                    }
                }
                right = TRUE;
                buffer->dirty = TRUE;
            }
        }
    } else if( ev == EV_HOME ) {
        buffer->index = 0;
    } else if( ev == EV_END ) {
        bptr = buffer->content + buffer->length;
        while( bptr > buffer->content ) {
            if( *(bptr-1) != ' ' ) {
                break;
            }
            --bptr;
        }
        buffer->index = bptr - buffer->content;
    } else if( ev == EV_CTRL_LEFT ) {
        bptr = buffer->content + buffer->index;
        while( bptr > buffer->content ) {
            --bptr;
            if( secondbyte( buffer->content, bptr ) ) --bptr;
            if( !isdelim( *bptr ) )break;
        }
        while( bptr > buffer->content ) {
            --bptr;
            if( secondbyte( buffer->content, bptr ) ) --bptr;
            if( isdelim( *bptr ) ) {
                ++bptr;
                break;
            }
        }
        buffer->index = bptr - buffer->content;
    } else if( ev == EV_CTRL_RIGHT ) {

        char                    *eptr;   /* end pointer */
        char                    *tptr;   /* blank tail */

        bptr = buffer->content + buffer->index;
        eptr = buffer->content + buffer->length;
        for( ;; ) {
            if( bptr >= eptr ) break;
            if( isdelim( *bptr ) ) break;
            bptr += uicharlen( *bptr );
        }
        for( tptr = bptr; tptr < eptr ; ) {
            if( *tptr == ' ' ) {
                /* do nothing */
            } else if( isdelim( *tptr ) ) {
                bptr = tptr + 1;       /* position after last non-blank */
            } else {
                bptr = tptr;           /* position on next word */
                break;
            }
            tptr += uicharlen( *tptr );
        }
        buffer->index = bptr - buffer->content;
    } else if( ev == EV_CTRL_HOME ) {
        clear_buffer( buffer );
    } else if( ev == EV_CTRL_END ) {
        memset( buffer->content + buffer->index,
                        ' ', buffer->length - buffer->index );
        buffer->dirty = TRUE;
    } else if( ev == EV_CURSOR_RIGHT ) {
        allblank = TRUE;
        bptr = buffer->content + buffer->index;
        while( bptr < buffer->content + buffer->length ) {
            if( *bptr != ' ' ) {
                allblank = FALSE;
                break;
            }
            if( ignore_uicharlen ) {
                ++bptr;
            } else {
                bptr += uicharlen( *bptr );
            }
        }
        if( allblank ) {
            new = EV_BUMP_RIGHT;
        } else {
            right = TRUE;
        }
    } else {
        new = ev;
    }
    /* cursor right is handled here so that when characters are typed the
       event can be changed to a cursor right to move the cursor over */
    if( right ) {
        if( buffer->index < buffer->length ) {
            bptr = buffer->content + buffer->index;
            buffer->index += uicharlen( *bptr );
        }
        if( buffer->index == buffer->length ) {
            new = EV_BUMP_RIGHT;      /* 891206 */
        }
    } else if( ( ev == EV_DELETE ) || ( ev == EV_RUB_OUT ) ) {
        int     delsize;

        allblank = TRUE;
        if( buffer->index < buffer->length ) {
            bptr = buffer->content + buffer->index;
            delsize = uicharlen( *bptr );
            while( bptr < buffer->content + buffer->length - delsize ) {
                if( *bptr != ' ' ) {
                    allblank = FALSE;
                }
                *bptr = *(bptr+delsize);
                ++bptr;
            }
            *bptr = ' ';
            if( delsize == 2 ) {
                *++bptr = ' ';
            }
            buffer->dirty = TRUE;
        }
        if( allblank && ( ev == EV_DELETE ) ) {
            new = EV_JOIN_RIGHT;
        } else {
            new = EV_NO_EVENT;
        }
    }
    if( (new == EV_NO_EVENT  ||  new == EV_BUFFER_CLEAR) && ev != EV_INSERT ) {
        buffer->auto_clear = FALSE;
    }
    if( cleared ) {
        if( uiinlist( EV_BUFFER_CLEAR ) ) {
            new = EV_BUFFER_CLEAR;
        }
    }
    return( new );
}
