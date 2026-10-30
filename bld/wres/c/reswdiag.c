/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2026 The Open Watcom Contributors. All Rights Reserved.
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
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/

#include <string.h>
#include <limits.h>
#include "layer0.h"
#include "filefmt.h"
#include "write.h"
#include "read.h"
#include "resdiag.h"
#include "reserr.h"
#include "wresdefn.h"
#include "wresrtns.h"
#include "wstrcmp.h"


#define IS_STR_IGNORE_CASE(s,c) ( WresStrnicmp( s, c, sizeof( c ) ) == 0 )

static bool ResWriteDialogSizeInfo( DialogSizeInfo *SizeInfo, FILE *fp )
/**********************************************************************/
{
    bool    error;

    error = ResWriteUint16( fp, SizeInfo->x );
    if( !error ) {
        error = ResWriteUint16( fp, SizeInfo->y );
    }
    if( !error ) {
        error = ResWriteUint16( fp, SizeInfo->width );
    }
    if( !error ) {
        error = ResWriteUint16( fp, SizeInfo->height );
    }
    return( error );
}

static bool writeControlClass( FILE *fp, ResNameOrOrdinal *cclass )
{
    bool    error;

    if( cclass == NULL ) {
        error = ResWriteString( "", fp );
    } else if( cclass->ord.fFlag == 0xff ) {
        error = ResWriteUint8( fp, cclass->ord.wOrdinalID );
    } else {
        error = ResWriteString( cclass->name, fp );
    }
    return( error );
}

bool ResWriteDialogBoxHeader( DialogBoxHeader *head, FILE *fp )
/*************************************************************/
{
    bool            error;

    error = ResWriteUint32( fp, head->Style );
    if( !error ) {
        error = ResWriteUint8( fp, head->NumOfItems );
    }
    if( !error ) {
        error = ResWriteDialogSizeInfo( &(head->SizeInfo), fp );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal( head->MenuName, fp );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal( head->ClassName, fp );
    }
    if( !error ) {
        error = ResWriteString( head->Caption, fp );
    }
    /* if the font was set output the font name and point size */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResWriteUint16( fp, head->PointSize );
        if( !error ) {
            error = ResWriteString( head->FontName, fp );
        }
    }
    return( error );
}

bool ResWriteDialogBoxHeader32( DialogBoxHeader *head, FILE *fp )
/***************************************************************/
{
    bool            error;

    error = ResWriteUint32( fp, head->Style );
    if( !error ) {
        error = ResWriteUint32( fp, head->ExtendedStyle );
    }
    if( !error ) {
        error = ResWriteUint16( fp, head->NumOfItems );
    }
    if( !error ) {
        error = ResWriteDialogSizeInfo( &(head->SizeInfo), fp );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal32( head->MenuName, fp );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal32( head->ClassName, fp );
    }
    if( !error ) {
        error = ResWriteString32( head->Caption, fp );
    }
    /* if the font was set output the font name and point size */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResWriteUint16( fp, head->PointSize );
        if( !error ) {
            error = ResWriteString32( head->FontName, fp );
        }
    }

    if( !error ) {
        /* padding full record to dword boundary if necessary */
        error = ResWritePadDWord( fp );
    }
    return( error );
}


bool ResWriteDialogBoxExHeader( DialogBoxHeader *head, FILE *fp )
/***************************************************************/
{
    bool            error;

    /* Write out the miscellaneous two WORDs 0x0001, 0xffff */

    error = ResWriteUint16( fp, 0x0001 );
    if( !error ) {
        error = ResWriteUint16( fp, 0xffff );
    }
    if( !error ) {
        error = ResWriteUint32( fp, head->HelpId );
    }
    if( !error ) {
        error = ResWriteUint32( fp, head->ExtendedStyle );
    }
    if( !error ) {
        error = ResWriteUint32( fp, head->Style );
    }
    if( !error ) {
        error = ResWriteUint16( fp, head->NumOfItems );
    }
    if( !error ) {
        error = ResWriteDialogSizeInfo( &(head->SizeInfo), fp );
    }
    if( !error ) {
        /*
         * The original Watcom implementation encloses a named DIALOGEX menu
         * in quotes before writing it. This is preserved for compatibility;
         * the reason for this DIALOGEX-specific behavior is currently unknown.
         */
        ResNameOrOrdinal    *res_menu;

        res_menu = head->MenuName;
        if( res_menu != NULL && res_menu->name[0] != '\0' ) {
            size_t     len;
            char       *name;

            len = strlen( res_menu->name );
            name = WRESALLOC( len + 3 );
            name[0] = '"';
            strcpy( name + 1, res_menu->name );
            name[len + 1] = '"';
            name[len + 2] = '\0';
            res_menu = ResStrToNameOrOrdinal( name );
            WRESFREE( name );
        }
        error = ResWriteNameOrOrdinal32( res_menu, fp );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal32( head->ClassName, fp );
    }
    if( !error ) {
        error = ResWriteString32( head->Caption, fp );
    }
    /* If the font was set, write the font information */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResWriteUint16( fp, head->PointSize );
        if( !error ) {
            error = ResWriteUint16( fp, head->FontWeight );
        }
        if( !error ) {
            error = ResWriteUint8( fp, head->FontItalic );
        }
        if( !error ) {
            error = ResWriteUint8( fp, head->FontCharset );
        }
        if( !error ) {
            error = ResWriteString32( head->FontName, fp );
        }
    }

    if( !error ) {
        /* padding full record to dword boundary if necessary */
        error = ResWritePadDWord( fp );
    }
    return( error );
}

bool ResWriteDialogBoxControl( DialogBoxControl *control, FILE *fp )
/******************************************************************/
{
    bool            error;

    error = ResWriteDialogSizeInfo( &(control->SizeInfo), fp );
    if( !error ) {
        error = ResWriteUint16( fp, control->ID );
    }
    if( !error ) {
        error = ResWriteUint32( fp, control->Style );
    }
    if( !error ) {
        error = writeControlClass( fp, control->ClassID );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal( control->Text, fp );
    }
    if( !error ) {
        error = ResWriteUint8( fp, control->ExtraBytes );
    }

    return( error );
}

bool ResWriteDialogBoxControl32( DialogBoxControl *control, FILE *fp )
/********************************************************************/
{
    bool            error;

    error = ResWriteUint32( fp, control->Style );
    if( !error ) {
        error = ResWriteUint32( fp, control->ExtendedStyle );
    }
    if( !error ) {
        error = ResWriteDialogSizeInfo( &(control->SizeInfo), fp );
    }
    if( !error ) {
        error = ResWriteUint16( fp, control->ID );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal32( control->ClassID, fp );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal32( control->Text, fp );
    }

    if( !error ) {
        error = ResWriteUint16( fp, control->ExtraBytes );
    }

    if( !error ) {
        /* padding full record to dword boundary if necessary */
        error = ResWritePadDWord( fp );
    }
    return( error );
}

bool ResWriteDialogBoxExControl( DialogBoxControl *control, FILE *fp )
/********************************************************************/
{
    bool            error;

    error = ResWriteUint32( fp, control->HelpId );
    if( !error ) {
        error = ResWriteUint32( fp, control->ExtendedStyle );
    }
    if( !error ) {
        error = ResWriteUint32( fp, control->Style );
    }
    if( !error ) {
        error = ResWriteDialogSizeInfo( &(control->SizeInfo), fp );
    }
    if( !error ) {
        error = ResWriteUint32( fp, control->ID );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal32( control->ClassID, fp );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal32( control->Text, fp );
    }

    if( !error ) {
        error = ResWriteUint16( fp, control->ExtraBytes );
    }

    if( !error ) {
        /* padding full record to dword boundary if necessary */
        error = ResWritePadDWord( fp );
    }
    return( error );
}

ResNameOrOrdinal *ResNormalizeControlClass( const ResNameOrOrdinal *cclass )
/**************************************************************************/
{
    ResNameOrOrdinal    *new_cclass;

    if( cclass->ord.fFlag == 0xff ) {
        new_cclass = ResNumToNameOrOrdinal( cclass->ord.wOrdinalID );
    } else {
        if( IS_STR_IGNORE_CASE( cclass->name, "button" ) ) {
            new_cclass = ResNumToNameOrOrdinal( CLASS_BUTTON );
        } else if( IS_STR_IGNORE_CASE( cclass->name, "edit" ) ) {
            new_cclass = ResNumToNameOrOrdinal( CLASS_EDIT );
        } else if( IS_STR_IGNORE_CASE( cclass->name, "static" ) ) {
            new_cclass = ResNumToNameOrOrdinal( CLASS_STATIC );
        } else if( IS_STR_IGNORE_CASE( cclass->name, "listbox" ) ) {
            new_cclass = ResNumToNameOrOrdinal( CLASS_LISTBOX );
        } else if( IS_STR_IGNORE_CASE( cclass->name, "scrollbar" ) ) {
            new_cclass = ResNumToNameOrOrdinal( CLASS_SCROLLBAR );
        } else if( IS_STR_IGNORE_CASE( cclass->name, "combobox" ) ) {
            new_cclass = ResNumToNameOrOrdinal( CLASS_COMBOBOX );
        } else {
            new_cclass = ResStrToNameOrOrdinal( cclass->name );
        }
    }
    return( new_cclass );
}
