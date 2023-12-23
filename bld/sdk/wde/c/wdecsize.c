/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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

static size_t WdeCalcSizeOfControlClass( ControlClass *name, bool is32bit )
{
    size_t size;

    if( name == NULL ) {
        if( is32bit ) {
            size = SIZEU16;
        } else {
            size = SIZEU8;
        }
    } else {
        if( is32bit ) {
            if( name->Class & 0x80 ) {
                size = SIZEU16 + SIZEU16;
            } else {
                size = WRCalcStrlen( name->ClassName, is32bit );
            }
        } else {
            if( name->Class & 0x80 ) {
                size = SIZEU8;
            } else {
                size = WRCalcStrlen( name->ClassName, is32bit );
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
        size = 4 * SIZEU16 + 4 * SIZEU32 + /* Extra bytes */ SIZEU16; 
    } else if( is32bit ) {
        // fix part of DialogBoxControl32 (up to ClassID)
        size = 5 * SIZEU16 + 2 * SIZEU32 + /* Extra bytes */ SIZEU16;
    } else {
        // fix part of DialogBoxControl (up to ClassID)
        size = 5 * SIZEU16 + 1 * SIZEU32 + /* Extra bytes */ SIZEU8;
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
        size = 7 * SIZEU16 + 3 * SIZEU32;
    } else if( is32bit ) {
        // fix part of DialogBoxHeader32 (up to MenuName)
        size = 5 * SIZEU16 + 2 * SIZEU32;
    } else {
        // fix part of DialogBoxHeader (up to MenuName)
        size = 1 * SIZEU8 + 4 * SIZEU16 + 1 * SIZEU32;
    }

    size += WdeCalcSizeOfResNameOrOrdinal( MenuName, is32bit );
    size += WdeCalcSizeOfResNameOrOrdinal( ClassName, is32bit );
    size += WRCalcStrlen( Caption, is32bit );

    if( FontName != NULL ) {
        /* PointSize */
        size += SIZEU16;
        if( is32bitEx ) {
            /* FontWeight + FontItalic + FontCharset */
            size += SIZEU16 + SIZEU8 + SIZEU8;
        }
        size += WRCalcStrlen( FontName, is32bit );
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
            size = size + CALC_PAD( size, SIZEU32 );
        }
        size = size + csize;
    }
    if( is32bit ) {
        size = size + CALC_PAD( size, SIZEU32 );
    }

    return( size );
}

size_t WdeCalcSizeOfResNameOrOrdinal( ResNameOrOrdinal *name, bool is32bit )
{
    size_t size;

    if( name == NULL ) {
        if( is32bit ) {
            size = SIZEU16;
        } else {
            size = SIZEU8;
        }
    } else {
        if( is32bit ) {
            if( name->ord.fFlag == 0xff ) {
                size = SIZEU16 + SIZEU16;
            } else {
                size = WRCalcStrlen( name->name, is32bit );
            }
        } else {
            if( name->ord.fFlag == 0xff ) {
                size = SIZEU8 + SIZEU16;
            } else {
                size = WRCalcStrlen( name->name, is32bit );
            }
        }
    }

    return( size );
}
