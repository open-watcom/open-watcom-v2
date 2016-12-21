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


static bool ResReadDialogSizeInfo( DialogSizeInfo *size, WResFileID fid )
/***********************************************************************/
{
    bool    error;
    uint_16     tmp16;

    error = ResReadUint16( &tmp16, fid );
    size->x = tmp16;
    if( !error ) {
        error = ResReadUint16( &tmp16, fid );
        size->y = tmp16;
    }
    if( !error ) {
        error = ResReadUint16( &tmp16, fid );
        size->width = tmp16;
    }
    if( !error ) {
        error = ResReadUint16( &tmp16, fid );
        size->height = tmp16;
    }
    return( error );
}

bool ResReadDialogBoxHeader( DialogBoxHeader *head, WResFileID fid )
/******************************************************************/
{
    bool            error;
    uint_32         tmp32;
    uint_16         tmp16;
    uint_8          tmp8;

    error = ResReadUint32( &tmp32, fid );
    head->Style = tmp32;
    if( !error ) {
        error = ResReadUint8( &tmp8, fid );
        head->NumOfItems = tmp8;
    }
    if( !error ) {
        error = ResReadDialogSizeInfo( &(head->Size), fid );
    }
    if( !error ) {
        head->MenuName = ResReadNameOrOrdinal( fid );
        error = (head->MenuName == NULL);
    }
    if( !error ) {
        head->ClassName = ResReadNameOrOrdinal( fid );
        error = (head->ClassName == NULL);
    }
    if( !error ) {
        head->Caption = ResReadString( fid, NULL );
        error = (head->Caption == NULL);
    }

    /* if the font was set input the font name and point size */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResReadUint16( &tmp16, fid );
        head->PointSize = tmp16;
        if( !error ) {
            head->FontName = ResReadString( fid, NULL );
            error = (head->FontName == NULL);
        }
    } else {
        head->FontName = NULL;
    }

    return( error );
}

bool ResIsDialogEx( WResFileID fid )
/**********************************/
{
    uint_16         sign0;
    uint_16         sign1;

    /* read in the signature part of the header and check it */
    if( !ResReadUint16( &sign0, fid ) ) {
        if( !ResReadUint16( &sign1, fid ) ) {
            return( sign0 == 0x0001 && sign1 == 0xFFFF );
        }
    }
    return( false );
}

static bool ResReadDialogHeaderCommon32( DialogBoxHeader32 *head, WResFileID fid )
{
    bool    error;

    head->MenuName = ResRead32NameOrOrdinal( fid );
    error = (head->MenuName == NULL);
    if( !error ) {
        head->ClassName = ResRead32NameOrOrdinal( fid );
        error = (head->ClassName == NULL);
    }
    if( !error ) {
        head->Caption = ResRead32String( fid, NULL );
        error = (head->Caption == NULL);
    }
    return( error );
}

bool ResReadDialogBoxHeader32( DialogBoxHeader32 *head, WResFileID fid )
/**********************************************************************/
{
    bool            error;
    uint_32         tmp32;
    uint_16         tmp16;

    error = ResReadUint32( &tmp32, fid );
    head->Style = tmp32;
    if( !error ) {
        error = ResReadUint32( &tmp32, fid );
        head->ExtendedStyle = tmp32;
    }
    if( !error ) {
        error = ResReadUint16( &tmp16, fid );
        head->NumOfItems = tmp16;
    }
    if( !error ) {
        error = ResReadDialogSizeInfo( &(head->Size), fid );
    }
    if( !error ) {
        error = ResReadDialogHeaderCommon32( head, fid );
    }

    /* if the font was set input the font name and point size */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResReadUint16( &tmp16, fid );
        head->PointSize = tmp16;
        if( !error ) {
            head->FontName = ResRead32String( fid, NULL );
            error = (head->FontName == NULL);
        }
    } else {
        head->FontName = NULL;
    }

    if( !error ) {
        /* seek to dword boundary if necessary */
        error = ResReadPadDWord( fid );
    }
    return( error );
}

bool ResReadDialogExHeader32( DialogBoxHeader32 *head, DialogExHeader32 *exhead, WResFileID fid )
/***********************************************************************************************/
{
    bool            error;
    uint_16         tmp16;
    uint_32         tmp32;

    /* Read in the miscellaneous two WORDs 0x0001, 0xFFFF */
    error = !ResIsDialogEx( fid );
    if( !error ) {
        error = ResReadUint32( &tmp32, fid );
        exhead->HelpId = tmp32;
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, fid );
        head->ExtendedStyle = tmp32;
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, fid );
        head->Style = tmp32;
    }
    if( !error ) {
        error = ResReadUint16( &tmp16, fid );
        head->NumOfItems = tmp16;
    }
    if( !error ) {
        error = ResReadDialogSizeInfo( &(head->Size), fid );
    }
    if( !error ) {
        error = ResReadDialogHeaderCommon32( head, fid );
    }

    /* If the font was set, write the font information */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResReadUint16( &tmp16, fid );
        head->PointSize = tmp16;
        if( !error ) {
            error = ResReadUint16( &(exhead->FontWeight), fid );
        }
        if( !error ) {
            error = ResReadUint8( &(exhead->FontItalic), fid );
        }
        if( !error ) {
            error = ResReadUint8( &(exhead->FontCharset), fid );
        }
        if( !error ) {
            head->FontName = ResRead32String( fid, NULL );
            error = (head->FontName == NULL);
        }
    }

    if( !error ) {
        /* seek to dword boundary if necessary */
        error = ResReadPadDWord( fid );
    }
    return( error );
}

static ControlClass *ReadControlClass( WResFileID fid )
/*****************************************************/
{
    ControlClass    *newclass;
    uint_8          class;
    bool            error;
    size_t          stringlen;
    char            *restofstring;

    restofstring = NULL;
    stringlen = 0;

    /* read in the first byte */
    error = ResReadUint8( &class, fid );
    if( !error ) {
        if( (class & 0x80) == 0 && class != '\0' ) {
            restofstring = ResReadString( fid, &stringlen );
            stringlen++;    /* for the '\0' */
            error = (restofstring == NULL);
    }
    }

    /* allocate memory for the new class */
    if( error ) {
        newclass = NULL;
        } else {
        newclass = WRESALLOC( sizeof( ControlClass ) + stringlen );
        if( newclass == NULL ) {
            error = true;
            WRES_ERROR( WRS_MALLOC_FAILED );
        }
    }

    /* copy the class or string into the correct place */
    if( !error ) {
        newclass->Class = class;
        if( stringlen > 0 ) {
            memcpy( newclass->ClassName + 1, restofstring, stringlen );
        }
    }

    if( restofstring != NULL ) {
        WRESFREE( restofstring );
    }

    return( newclass );
}

static ControlClass *Read32ControlClass( WResFileID fid )
/*******************************************************/
{
    ControlClass    *newclass;
    uint_16         flags;
    uint_16         class;
    bool            error;
    size_t          stringlen;
    char            *restofstring;

    restofstring = NULL;
    stringlen = 0;
    class = 0;

    /* read in the first word */
    error = ResReadUint16( &flags, fid );
        if( !error ) {
        if( flags == 0xffff ) {
            error = ResReadUint16( &class, fid );
        } else {
            class = UNI2ASCII( flags ); /* first 16-bit UNICODE character */
            restofstring = ResRead32String( fid, &stringlen );
            stringlen++;                /* for the '\0' */
            error = (restofstring == NULL);
        }
        }

    /* allocate memory for the new class */
    if( error ) {
        newclass = NULL;
    } else {
        newclass = WRESALLOC( sizeof( ControlClass ) + stringlen );
        if( newclass == NULL ) {
            error = true;
            WRES_ERROR( WRS_MALLOC_FAILED );
        }
    }

    /* copy the class or string into the correct place */
    if( !error ) {
        newclass->Class = (uint_8)class;
        if( flags != 0xffff ) {
            memcpy( newclass->ClassName + 1, restofstring, stringlen );
        }
    }

    if( restofstring != NULL ) {
        WRESFREE( restofstring );
    }

    return( newclass );
}

bool ResReadDialogBoxControl( DialogBoxControl *control, WResFileID fid )
/***********************************************************************/
{
    bool            error;
    uint_32         tmp32;
    uint_16         tmp16;
    uint_8          tmp8;

    error = ResReadDialogSizeInfo( &(control->Size), fid );
    if( !error ) {
        error = ResReadUint16( &tmp16, fid );
        control->ID = tmp16;
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, fid );
        control->Style = tmp32;
    }
    if( !error ) {
        control->ClassID = ReadControlClass( fid );
        error = (control->ClassID == NULL);
    }
    if( !error ) {
        control->Text = ResReadNameOrOrdinal( fid );
    }
    if( !error ) {
        error = ResReadUint8( &tmp8, fid );
        control->ExtraBytes = tmp8;
    }

    return( error );
}

static bool ResReadDialogControlCommon32( ControlClass **class_id, ResNameOrOrdinal **text, uint_16 *extra_bytes, WResFileID fid )
{
    bool            error;
    uint_16         tmp16;

    *class_id = Read32ControlClass( fid );
    error = (*class_id == NULL);
    if( !error ) {
        *text = ResRead32NameOrOrdinal( fid );
    }
    if( !error ) {
        error = ResReadUint16( &tmp16, fid );
        *extra_bytes = tmp16;
    }

    return( error );
}

bool ResReadDialogBoxControl32( DialogBoxControl32 *control, WResFileID fid )
/***************************************************************************/
{
    bool            error;
    uint_32         tmp32;
    uint_16         tmp16;

    error = ResReadUint32( &tmp32, fid );
    control->Style = tmp32;
    if( !error ) {
        error = ResReadUint32( &tmp32, fid );
        control->ExtendedStyle = tmp32;
    }
    if( !error ) {
        error = ResReadDialogSizeInfo( &(control->Size), fid );
    }
    if( !error ) {
        error = ResReadUint16( &tmp16, fid );
        control->ID = tmp16;
    }
    if( !error ) {
        error = ResReadDialogControlCommon32( &(control->ClassID), &(control->Text), &(control->ExtraBytes), fid );
    }

    if( !error ) {
        /* seek to dword boundary if necessary */
        error = ResReadPadDWord( fid );
    }
    return( error );
}

bool ResReadDialogExControl32( DialogBoxExControl32 *control, WResFileID fid )
/****************************************************************************/
{
    bool            error;
    uint_32         tmp32;

    error = ResReadUint32( &tmp32, fid );
    control->HelpId = tmp32;
    if( !error ) {
        error = ResReadUint32( &tmp32, fid );
        control->ExtendedStyle = tmp32;
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, fid );
        control->Style = tmp32;
    }
    if( !error ) {
        error = ResReadDialogSizeInfo( &(control->Size), fid );
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, fid );
        control->ID = tmp32;
            }
    if( !error ) {
        error = ResReadDialogControlCommon32( &(control->ClassID), &(control->Text), &(control->ExtraBytes), fid );
        }

    if( !error ) {
        /* seek to dword boundary if necessary */
        error = ResReadPadDWord( fid );
    }
    return( error );
}

void ResFreeDialogBoxHeaderPtrs( DialogBoxHeader *head )
/******************************************************/
{
    if( head->MenuName != NULL ) {
        WRESFREE( head->MenuName );
    }
    if( head->ClassName != NULL ) {
        WRESFREE( head->ClassName );
    }
    if( head->Caption != NULL ) {
        WRESFREE( head->Caption );
    }
    if( head->FontName != NULL ) {
        WRESFREE( head->FontName );
    }
}

void ResFreeDialogBoxHeader32Ptrs( DialogBoxHeader32 *head )
/**********************************************************/
{
    if( head->MenuName != NULL ) {
        WRESFREE( head->MenuName );
    }
    if( head->ClassName != NULL ) {
        WRESFREE( head->ClassName );
    }
    if( head->Caption != NULL ) {
        WRESFREE( head->Caption );
    }
    if( head->FontName != NULL ) {
        WRESFREE( head->FontName );
    }
}
