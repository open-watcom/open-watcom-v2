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

static bool ResWriteDialogSizeInfo( DialogSizeInfo *sizeinfo, FILE *fp )
/**********************************************************************/
{
    bool    error;

    error = ResWriteUint16( fp, sizeinfo->x );
    if( !error ) {
        error = ResWriteUint16( fp, sizeinfo->y );
    }
    if( !error ) {
        error = ResWriteUint16( fp, sizeinfo->width );
    }
    if( !error ) {
        error = ResWriteUint16( fp, sizeinfo->height );
    }
    return( error );
}

static bool WriteControlClass( FILE *fp, ControlClass *class_id )
{
    /* if the ClassID is one of the predefined ones write it out as a byte */
    /* otherwise it is a string */
    if( class_id->Class & 0x80 )
        return( ResWriteUint8( fp, class_id->Class ) );
    return( ResWriteString( class_id->ClassName, fp ) );
}

static bool WriteControlClass32( FILE *fp, ControlClass *class_id )
{
    bool            error;

    /* if the ClassID is one of the predefined ones write it out as a byte */
    /* otherwise it is a string */
    if( class_id->Class & 0x80 ) {
        /*
         * the class number is prefixed by 0xffff to distinguish it from a string
         */
        error = ResWriteUint16( fp, 0xffff );
        if( !error ) {
            error = ResWriteUint16( fp, class_id->Class );
        }
        return( error );
    }
    return( ResWriteString32( class_id->ClassName, fp ) );
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
        error = WriteControlClass( fp, control->ClassID );
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
        error = WriteControlClass32( fp, control->ClassID );
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
        error = WriteControlClass32( fp, control->ClassID );
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

ControlClass *ResNameOrOrdinalToControlClass( const ResNameOrOrdinal *name )
/**************************************************************************/
{
    size_t          stringlen;
    ControlClass    *class;

    if( name->ord.fFlag == 0xff ) {
        class = ResNumToControlClass( name->ord.wOrdinalID );
    } else {
        if( IS_STR_IGNORE_CASE( name->name, "button" ) ) {
            class = ResNumToControlClass( CLASS_BUTTON );
        } else if( IS_STR_IGNORE_CASE( name->name, "edit" ) ) {
            class = ResNumToControlClass( CLASS_EDIT );
        } else if( IS_STR_IGNORE_CASE( name->name, "static" ) ) {
            class = ResNumToControlClass( CLASS_STATIC );
        } else if( IS_STR_IGNORE_CASE( name->name, "listbox" ) ) {
            class = ResNumToControlClass( CLASS_LISTBOX );
        } else if( IS_STR_IGNORE_CASE( name->name, "scrollbar" ) ) {
            class = ResNumToControlClass( CLASS_SCROLLBAR );
        } else if( IS_STR_IGNORE_CASE( name->name, "combobox" ) ) {
            class = ResNumToControlClass( CLASS_COMBOBOX );
        } else {
            /* space for the '\0' is reserve in the ControlClass structure */
            stringlen = strlen( name->name );
            class = WRESALLOC( sizeof( ControlClass ) + stringlen );
            if( class == NULL ) {
                WRES_ERROR( WRS_MALLOC_FAILED );
            } else {
                /* +1 to copy the '\0' */
                memcpy( class->ClassName, name->name, stringlen + 1 );
            }
        }
    }
    return( class );
}

ControlClass *ResNumToControlClass( uint_16 classnum )
/****************************************************/
{
    ControlClass *  class;

    if( (classnum & 0x80) != 0 || classnum == 0) {
        class = WRESALLOC( sizeof( ControlClass ) );
        if( class == NULL ) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else if( (classnum & 0x80) != 0 ) {
            class->Class = (uint_8)classnum;
        } else {
            class->ClassName[0] = '\0';
        }
    } else {
        class = WRESALLOC( sizeof( ControlClass ) + 1 );
        if( class == NULL ) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            class->ClassName[0] = (char)classnum;
            class->ClassName[1] = '\0';
        }
    }
    return( class );
}
