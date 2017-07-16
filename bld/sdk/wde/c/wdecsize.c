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

static size_t WdeCalcSizeOfDialogBoxControl( ControlClass *ClassID, ResNameOrOrdinal *Text, bool is32bit, bool is32bitEx )
{
    size_t size;

    if( is32bitEx ) {
        // fix part of DialogBoxExControl32 (up to ClassID)
        size = 4 * sizeof( uint_16 ) + 4 * sizeof( uint_32 );
        size += sizeof( uint_16 );  /* Extra bytes */
    } else if( is32bit ) {
        // fix part of DialogBoxControl32 (up to ClassID)
        size = 5 * sizeof( uint_16 ) + 2 * sizeof( uint_32 );
        size += sizeof( uint_16 );  /* Extra bytes */
    } else {
        // fix part of DialogBoxControl (up to ClassID)
        size = 5 * sizeof( uint_16 ) + 1 * sizeof( uint_32 );
        size += sizeof( uint_8 );   /* Extra bytes */
    }

    size += WdeCalcSizeOfControlClass( ClassID, is32bit );

    size += WdeCalcSizeOfResNameOrOrdinal( Text, is32bit );

    return( size );
}

static size_t WdeCalcSizeOfDialogBoxHeader( ResNameOrOrdinal *MenuName, ResNameOrOrdinal *ClassName, char *Caption, char *FontName, bool is32bit, bool is32bitEx )
{
    size_t size;

    if( is32bitEx ) {
        // fix part of DialogBoxExHeader32 (up to MenuName)
        size = 7 * sizeof( uint_16 ) + 3 * sizeof( uint_32 );
    } else if( is32bit ) {
        // fix part of DialogBoxHeader32 (up to MenuName)
        size = 5 * sizeof( uint_16 ) + 2 * sizeof( uint_32 );
    } else {
        // fix part of DialogBoxHeader (up to MenuName)
        size = 1 * sizeof( uint_8 ) + 4 * sizeof( uint_16 ) + 1 * sizeof( uint_32 );
    }

    size += WdeCalcSizeOfResNameOrOrdinal( MenuName, is32bit );
    size += WdeCalcSizeOfResNameOrOrdinal( ClassName, is32bit );
    size += WdeCalcStrlen( Caption, is32bit );

    if( FontName != NULL ) {
        size += sizeof( WORD );     /* PointSize */
        if( is32bitEx ) {
            size += sizeof( WORD ); /* FontWeight */
            size += sizeof( BYTE ); /* FontItalic */
            size += sizeof( BYTE ); /* FontCharset */
        }
        size += WdeCalcStrlen( FontName, is32bit );
    }

    return( size );
}

size_t WdeCalcSizeOfWdeDialogBoxInfo( WdeDialogBoxInfo *info )
{
    size_t              size;
    size_t              csize;
    LIST                *clist;
    bool                is32bit;
    bool                is32bitEx;

    #define h   (info->dialog_header)
    #define c   ((WdeDialogBoxControl *)ListElement( clist ))

    if( info == NULL ) {
        return( 0 );
    }
    is32bit = h->is32bit;
    is32bitEx = h->is32bitEx;
    size = WdeCalcSizeOfDialogBoxHeader( GETHDR_MENUNAME( h ), GETHDR_CLASSNAME( h ),
                                        GETHDR_CAPTION( h ), GETHDR_FONTFACENAME( h ), is32bit, is32bitEx );
    for( clist = info->control_list; clist != NULL; clist = ListNext( clist ) ) {
        csize = WdeCalcSizeOfDialogBoxControl( GETCTL_CLASSID( c ), GETCTL_TEXT( c ), is32bit, is32bitEx );
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
