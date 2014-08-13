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


#include "wdeglbl.h"
#include "wderesin.h"
#include "wdecsize.h"
#include "wrdll.h"
#include "wresall.h"

static size_t WdeCalcStrlen( char *str, bool is32bit )
{
    size_t      len;

    if( is32bit ) {
        if( str == NULL || !WRmbcs2unicode( str, NULL, &len ) ) {
            len = 2;
        }
    } else {
        len = 1;
        if( str != NULL ) {
            len += strlen( str );
        }
    }

    return( len );
}

static size_t WdeCalcSizeOfControlClass( ControlClass *name, bool is32bit )
{
    size_t size;

    if( name == NULL ) {
        if( is32bit ) {
            size = sizeof( uint_16 );
        } else {
            size = sizeof( uint_8 );
        }
    } else {
        if( is32bit ) {
            if( name->Class & 0x80 ) {
                size = sizeof( uint_16 ) * 2;
            } else {
                size = WdeCalcStrlen( name->ClassName, is32bit );
            }
        } else {
            if( name->Class & 0x80 ) {
                size = 1;
            } else {
                size = WdeCalcStrlen( name->ClassName, is32bit );
            }
        }
    }

    return( size );
}

static size_t WdeCalcSizeOfDialogBoxControl( WdeDialogBoxControl *control,
                                              bool is32bit, bool is32bitEx )
{
    size_t size;

    if( is32bit ) {
        /* JPK - Added for extended dialog support */
        if( is32bitEx ) {
            size = offsetof( DialogBoxExControl32, ClassID );
            size += sizeof( uint_16 );  /* Extra bytes */
        } else {
            size = offsetof( DialogBoxControl32, ClassID );
            size += sizeof( uint_16 );  /* Extra bytes */
        }
    } else {
        size = offsetof( DialogBoxControl, ClassID );
        size += sizeof( uint_8 );       /* Extra bytes */
    }

    size += WdeCalcSizeOfControlClass( GETCTL_CLASSID( control ), is32bit );

    size += WdeCalcSizeOfResNameOrOrdinal( GETCTL_TEXT( control ), is32bit );

    return( size );
}

static size_t WdeCalcSizeOfDialogBoxHeader( WdeDialogBoxHeader *header )
{
    size_t size;

    if( header->is32bit ) {
        size = offsetof( DialogBoxHeader32, MenuName );
    } else {
        size = offsetof( DialogBoxHeader, MenuName );
    }

    size += WdeCalcSizeOfResNameOrOrdinal( GETHDR_MENUNAME( header ), header->is32bit );

    size += WdeCalcSizeOfResNameOrOrdinal( GETHDR_CLASSNAME( header ), header->is32bit );

    size += WdeCalcStrlen( GETHDR_CAPTION( header ), header->is32bit );

    if( GETHDR_STYLE( header ) & DS_SETFONT ) {
        size += sizeof( uint_16 );      /* PointSize */

        if( header->is32bitEx ) {
            size += sizeof( uint_16 );  /* FontWeight */
            size += sizeof( uint_16 );  /* FontItalic */
        }

        size += WdeCalcStrlen( GETHDR_FONTNAME( header ), header->is32bit );
    }

    if( header->is32bitEx ) {
        size += sizeof( uint_32 );      /* HelpID */
        size += 4;                      /* signature bytes */
    }
    return( size );
}

size_t WdeCalcSizeOfWdeDialogBoxInfo( WdeDialogBoxInfo *info )
{
    size_t              size;
    size_t              csize;
    LIST                *clist;
    WdeDialogBoxControl *control;
    bool                is32bit;
    bool                is32bitEx;

    is32bit = info->dialog_header->is32bit;
    is32bitEx = info->dialog_header->is32bitEx;

    if( info == NULL ) {
        return( 0 );
    }

    size = WdeCalcSizeOfDialogBoxHeader( info->dialog_header );

    for( clist = info->control_list; clist; clist = ListNext( clist ) ) {
        control = (WdeDialogBoxControl *)ListElement( clist );
        csize = WdeCalcSizeOfDialogBoxControl( control, is32bit, is32bitEx );
        if( csize == 0 ) {
            return( 0 );
        }
        if( is32bit ) {
            size = size + CALC_PAD( size, sizeof( uint_32 ) );
        }
        size = size + csize;
    }

    if( is32bit ) {
        size = size + CALC_PAD( size, sizeof( uint_32 ) );
    }

    return( size );
}

size_t WdeCalcSizeOfResNameOrOrdinal( ResNameOrOrdinal *name, bool is32bit )
{
    size_t size;

    if( name == NULL ) {
        if( is32bit ) {
            size = sizeof( uint_16 );
        } else {
            size = sizeof( uint_8 );
        }
    } else {
        if( is32bit ) {
            if( name->ord.fFlag == 0xff ) {
                size = sizeof( uint_16 ) + sizeof( uint_16 );
            } else {
                size = WdeCalcStrlen( name->name, is32bit );
            }
        } else {
            if( name->ord.fFlag == 0xff ) {
                size = sizeof( uint_16 ) + sizeof( uint_8 );
            } else {
                size = WdeCalcStrlen( name->name, is32bit );
            }
        }
    }

    return( size );
}
