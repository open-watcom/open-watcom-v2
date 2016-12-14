/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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

#include "clibext.h"


static bool ResWriteDialogSizeInfo( DialogSizeInfo *size, WResFileID handle )
/***************************************************************************/
{
    bool    error;

    error = ResWriteUint16( size->x, handle );
    if( !error ) {
        error = ResWriteUint16( size->y, handle );
    }
    if( !error ) {
        error = ResWriteUint16( size->width, handle );
    }
    if( !error ) {
        error = ResWriteUint16( size->height, handle );
    }
    return( error );
}

bool ResWriteDialogBoxHeader( DialogBoxHeader *head, WResFileID handle )
/**********************************************************************/
{
    bool            error;

    error = ResWriteUint32( head->Style, handle );
    if( !error ) {
        error = ResWriteUint8( head->NumOfItems, handle );
    }
    if( !error ) {
        error = ResWriteDialogSizeInfo( &(head->Size), handle );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal( head->MenuName, false, handle );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal( head->ClassName, false, handle );
    }
    if( !error ) {
        error = ResWriteString( head->Caption, false, handle );
    }
    /* if the font was set output the font name and point size */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResWriteUint16( head->PointSize, handle );
        if( !error ) {
            error = ResWriteString( head->FontName, false, handle );
        }
    }
    return( error );
}

static bool ResWriteDialogHeaderCommon32( DialogBoxHeader32 *head, WResFileID handle, bool add_quotes )
/*****************************************************************************************************/
{
    bool    error;
    size_t  len;
    char    *newname;

    if( add_quotes ) {
        if( head->MenuName != NULL && head->MenuName->name[0] != '\0' ) {
            len = strlen( head->MenuName->name );
            newname = WRESALLOC( len + 3 );
            newname[0] = '"';
            strcpy( newname + 1, head->MenuName->name );
            newname[len + 1] = '"';
            newname[len + 2] = '\0';
            head->MenuName = ResStrToNameOrOrd( newname );
            WRESFREE( newname );
        }
    }

    error = ResWriteNameOrOrdinal( head->MenuName, true, handle );
    if( !error ) {
        error = ResWriteNameOrOrdinal( head->ClassName, true, handle );
    }
    if( !error ) {
        error = ResWriteString( head->Caption, true, handle );
    }

    return( error );
}

bool ResWriteDialogBoxHeader32( DialogBoxHeader32 *head, WResFileID handle )
/**************************************************************************/
{
    bool            error;

    error = ResWriteUint32( head->Style, handle );
    if( !error ) {
        error = ResWriteUint32( head->ExtendedStyle, handle );
    }
    if( !error ) {
        error = ResWriteUint16( head->NumOfItems, handle );
    }
    if( !error ) {
        error = ResWriteDialogSizeInfo( &(head->Size), handle );
    }
    if( !error ) {
        error = ResWriteDialogHeaderCommon32( head, handle, false );
    }
    /* if the font was set output the font name and point size */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResWriteUint16( head->PointSize, handle );
        if( !error ) {
            error = ResWriteString( head->FontName, true, handle );
        }
    }

    if( !error ) {
        /* padding full record to dword boundary if necessary */
        error = ResWritePadDWord( handle );
    }
    return( error );
}


bool ResWriteDialogExHeader32( DialogBoxHeader32 *head, DialogExHeader32 *exhead,
                                                              WResFileID handle )
/*******************************************************************************/
{
    bool            error;

    /* Write out the miscellaneous two WORDs 01 00 FF FF */

    error = ResWriteUint16( 1, handle );
    if( !error ) {
        error = ResWriteUint16( (uint_16)-1, handle );
    }
    if( !error ) {
        error = ResWriteUint32( exhead->HelpId, handle );
    }
    if( !error ) {
        error = ResWriteUint32( head->ExtendedStyle, handle );
    }
    if( !error ) {
        error = ResWriteUint32( head->Style, handle );
    }
    if( !error ) {
        error = ResWriteUint16( head->NumOfItems, handle );
    }
    if( !error ) {
        error = ResWriteDialogSizeInfo( &(head->Size), handle );
    }
    if( !error ) {
        error = ResWriteDialogHeaderCommon32( head, handle, true );
    }
    /* If the font was set, write the font information */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResWriteUint16( head->PointSize, handle );
        if( !error ) {
            error = ResWriteUint16( exhead->FontWeight, handle );
        }
        if( !error ) {
            error = ResWriteUint8( exhead->FontItalic, handle );
        }
        if( !error ) {
            error = ResWriteUint8( exhead->FontCharset, handle );
        }
        if( !error ) {
            error = ResWriteString( head->FontName, true, handle );
        }
    }

    if( !error ) {
        /* padding full record to dword boundary if necessary */
        error = ResWritePadDWord( handle );
    }
    return( error );
}

bool ResWriteDialogBoxControl( DialogBoxControl *control, WResFileID handle )
/***************************************************************************/
{
    bool            error;

    error = ResWriteDialogSizeInfo( &(control->Size), handle );
    if( !error ) {
        error = ResWriteUint16( control->ID, handle );
    }
    if( !error ) {
        error = ResWriteUint32( control->Style, handle );
    }

    /* if the ClassID is one of the predefined ones write it out as a byte */
    /* otherwise it is a string */
    if( !error ) {
        if( control->ClassID->Class & 0x80 ) {
            error = ResWriteUint8( control->ClassID->Class, handle );
        } else {
            error = ResWriteString( control->ClassID->ClassName, false, handle);
        }
    }

    if( !error ) {
        error = ResWriteNameOrOrdinal( control->Text, false, handle );
    }
    if( !error ) {
        error = ResWriteUint8( control->ExtraBytes, handle );
    }

    return( error );
}

static bool ResWriteDialogControlCommon32( ControlClass *class_id, ResNameOrOrdinal *text,
                                                  uint_16 extra_bytes, WResFileID handle )
/****************************************************************************************/
{
    bool      error;

    /* if the ClassID is one of the predefined ones write it out as a byte */
    /* otherwise it is a string */
    if( class_id->Class & 0x80 ) {
        /*the class number is prefixed by 0xFFFF to distinguish it
         * from a string */
        error = ResWriteUint16( (uint_16)-1, handle );
        if( !error ) {
            error = ResWriteUint16( class_id->Class, handle );
        }
    } else {
        error = ResWriteString( class_id->ClassName, true, handle );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal( text, true, handle );
    }

    if( !error ) {
        error = ResWriteUint16( extra_bytes, handle );
    }

    return( error );
}

bool ResWriteDialogBoxControl32( DialogBoxControl32 *control, WResFileID handle )
/*******************************************************************************/
{
    bool            error;

    error = ResWriteUint32( control->Style, handle );
    if( !error ) {
        error = ResWriteUint32( control->ExtendedStyle, handle );
    }
    if( !error ) {
        error = ResWriteDialogSizeInfo( &(control->Size), handle );
    }
    if( !error ) {
        error = ResWriteUint16( control->ID, handle );
    }
    if( !error ) {
        error = ResWriteDialogControlCommon32( control->ClassID, control->Text, control->ExtraBytes, handle );
    }

    if( !error ) {
        /* padding full record to dword boundary if necessary */
        error = ResWritePadDWord( handle );
    }
    return( error );
}

bool ResWriteDialogExControl32( DialogBoxExControl32 *control, WResFileID handle )
/********************************************************************************/
{
    bool            error;

    error = ResWriteUint32( control->HelpId, handle );
    if( !error ) {
        error = ResWriteUint32( control->ExtendedStyle, handle );
    }
    if( !error ) {
        error = ResWriteUint32( control->Style, handle );
    }
    if( !error ) {
        error = ResWriteDialogSizeInfo( &(control->Size), handle );
    }
    if( !error ) {
        error = ResWriteUint32( control->ID, handle );
    }
    if( !error ) {
        error = ResWriteDialogControlCommon32( control->ClassID, control->Text, control->ExtraBytes, handle );
    }

    if( !error ) {
        /* padding full record to dword boundary if necessary */
        error = ResWritePadDWord( handle );
    }
    return( error );
}

ControlClass *ResNameOrOrdToControlClass( const ResNameOrOrdinal *name )
/**********************************************************************/
{
    size_t          stringlen;
    ControlClass    *class;

    if( name->ord.fFlag == 0xff ) {
        class = ResNumToControlClass( name->ord.wOrdinalID );
    } else {
        if( stricmp( name->name, "button" ) == 0 ) {
            class = ResNumToControlClass( CLASS_BUTTON );
        } else if( stricmp( name->name, "edit" ) == 0 ) {
            class = ResNumToControlClass( CLASS_EDIT );
        } else if( stricmp( name->name, "static" ) == 0 ) {
            class = ResNumToControlClass( CLASS_STATIC );
        } else if( stricmp( name->name, "listbox" ) == 0 ) {
            class = ResNumToControlClass( CLASS_LISTBOX );
        } else if( stricmp( name->name, "scrollbar" ) == 0 ) {
            class = ResNumToControlClass( CLASS_SCROLLBAR );
        } else if( stricmp( name->name, "combobox" ) == 0 ) {
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

    if( classnum & 0x80 ) {
        class = WRESALLOC( sizeof( ControlClass ) );
        if( class == NULL ) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            class->Class = (uint_8)classnum;
        }
    } else {
        class = WRESALLOC( sizeof( ControlClass ) + 1 );
        if( class == NULL ) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            *(class->ClassName + 0) = (char)classnum;
            *(class->ClassName + 1) = '\0';
        }
    }
    return( class );
}
