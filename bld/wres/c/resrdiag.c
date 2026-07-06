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


static bool ResReadDialogSizeInfo( DialogSizeInfo *sizeinfo, FILE *fp )
/*********************************************************************/
{
    bool        error;

    error = false;
    sizeinfo->x = ResReadUint16( &error, fp );
    if( error )
        return( true );
    sizeinfo->y = ResReadUint16( &error, fp );
    if( error )
        return( true );
    sizeinfo->width = ResReadUint16( &error, fp );
    if( error )
        return( true );
    sizeinfo->height = ResReadUint16( &error, fp );
    return( error );
}

bool ResReadDialogBoxHeader( DialogBoxHeader *head, FILE *fp )
/************************************************************/
{
    bool            error;

    error = false;
    head->Style = ResReadUint32( &error, fp );
    if( error )
        return( true );
    head->NumOfItems = ResReadUint8( &error, fp );
    if( error )
        return( true );
    if( ResReadDialogSizeInfo( &(head->SizeInfo), fp ) )
        return( true );
    head->MenuName = ResReadNameOrOrdinal( fp );
    if( head->MenuName == NULL )
        return( true );
    head->ClassName = ResReadNameOrOrdinal( fp );
    if( head->ClassName == NULL )
        return( true );
    head->Caption = ResReadString( fp, NULL );
    if( head->Caption == NULL )
        return( true );

    /* if the font was set input the font name and point size */
    head->FontName = NULL;
    if( head->Style & DS_SETFONT ) {
        head->PointSize = ResReadUint16( &error, fp );
        if( error )
            return( true );
        head->FontName = ResReadString( fp, NULL );
        if( head->FontName == NULL ) {
            return( true );
        }
    }

    return( false );
}

bool ResIsDialogBoxEx( FILE *fp )
/*******************************/
{
    uint_16         sign0;
    uint_16         sign1;
    bool            error;

    /* read in the signature part of the header and check it */
    error = false;
    sign0 = ResReadUint16( &error, fp );
    if( error )
        return( false );
    sign1 = ResReadUint16( &error, fp );
    if( error )
        return( false );
    return( sign0 == 0x0001 && sign1 == 0xFFFF );
}

static bool ResReadDialogHeaderCommon32( DialogBoxHeader32 *head, FILE *fp )
{
    head->MenuName = ResRead32NameOrOrdinal( fp );
    if( head->MenuName == NULL )
        return( true );
    head->ClassName = ResRead32NameOrOrdinal( fp );
    if( head->ClassName == NULL )
        return( true );
    head->Caption = ResRead32String( fp, NULL );
    return( head->Caption == NULL );
}

bool ResReadDialogBoxHeader32( DialogBoxHeader32 *head, FILE *fp )
/****************************************************************/
{
    bool            error;

    error = false;
    head->Style = ResReadUint32( &error, fp );
    if( error )
        return( true );
    head->ExtendedStyle = ResReadUint32( &error, fp );
    if( error )
        return( true );
    head->NumOfItems = ResReadUint16( &error, fp );
    if( error )
        return( true );
    if( ResReadDialogSizeInfo( &(head->SizeInfo), fp ) )
        return( true );
    if( ResReadDialogHeaderCommon32( head, fp ) )
        return( true );

    /* if the font was set input the font name and point size */
    head->FontName = NULL;
    if( head->Style & DS_SETFONT ) {
        head->PointSize = ResReadUint16( &error, fp );
        if( error )
            return( true );
        head->FontName = ResRead32String( fp, NULL );
        if( head->FontName == NULL ) {
            return( true );
        }
    }

    /* seek to dword boundary if necessary */
    return( ResReadPadDWord( fp ) );
}

bool ResReadDialogBoxExHeader32( DialogBoxHeader32 *head, DialogBoxExHeader32short *exhead, FILE *fp )
/****************************************************************************************************/
{
    bool            error;

    /* Read in the miscellaneous two WORDs 0x0001, 0xFFFF */
    if( !ResIsDialogBoxEx( fp ) )
        return( true );
    error = false;
    exhead->HelpId = ResReadUint32( &error, fp );
    if( error )
        return( true );
    head->ExtendedStyle = ResReadUint32( &error, fp );
    if( error )
        return( true );
    head->Style = ResReadUint32( &error, fp );
    if( error )
        return( true );
    head->NumOfItems = ResReadUint16( &error, fp );
    if( error )
        return( true );
    if( ResReadDialogSizeInfo( &(head->SizeInfo), fp ) )
        return( true );
    if( ResReadDialogHeaderCommon32( head, fp ) )
        return( true );

    /* If the font was set, write the font information */
    if( head->Style & DS_SETFONT ) {
        head->PointSize = ResReadUint16( &error, fp );
        if( error )
            return( true );
        exhead->FontWeight = ResReadUint16( &error, fp );
        if( error )
            return( true );
        exhead->FontItalic = ResReadUint8( &error, fp );
        if( error )
            return( true );
        exhead->FontCharset = ResReadUint8( &error, fp );
        if( error )
            return( true );
        head->FontName = ResRead32String( fp, NULL );
        if( head->FontName == NULL ) {
            return( true );
        }
    }

    /* seek to dword boundary if necessary */
    return( ResReadPadDWord( fp ) );
}

static ControlClass *ReadControlClass( FILE *fp )
/***********************************************/
{
    ControlClass    *newclass;
    uint_8          class;
    bool            error;
    size_t          stringlen;
    char            *restofstring;

    /* read in the first byte */
    error = false;
    class = ResReadUint8( &error, fp );
    if( error )
        return( NULL );

    restofstring = NULL;
    stringlen = 0;
    if( (class & 0x80) != 0 || class == 0 ) {
    } else {
        restofstring = ResReadString( fp, &stringlen );
        stringlen++;    /* for the '\0' */
        if( restofstring == NULL ) {
            return( NULL );
        }
    }

    /* allocate memory for the new class */
    newclass = WRESALLOC( sizeof( ControlClass ) + stringlen );
    if( newclass == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        /* copy the class or string into the correct place */
        if( (class & 0x80) != 0 ) {
            newclass->Class = class;
        } else if( class == 0 ) {
            newclass->ClassName[0] = '\0';
        } else {
            newclass->ClassName[0] = class;
            memcpy( newclass->ClassName + 1, restofstring, stringlen );
        }
    }

    if( restofstring != NULL ) {
        WRESFREE( restofstring );
    }

    return( newclass );
}

static ControlClass *Read32ControlClass( FILE *fp )
/*************************************************/
{
    ControlClass    *newclass;
    uint_16         flags;
    uint_16         class;
    bool            error;
    size_t          stringlen;
    char            *restofstring;

    /* read in the first word */
    error = false;
    flags = ResReadUint16( &error, fp );
    if( error )
        return( NULL );

    class = 0;
    restofstring = NULL;
    stringlen = 0;
    if( flags == 0xffff ) {
        class = ResReadUint16( &error, fp );
        if( error ) {
            return( NULL );
        }
    } else if( flags == 0 ) {
    } else {
        restofstring = ResRead32String( fp, &stringlen );
        stringlen++;                /* for the '\0' */
        if( restofstring == NULL ) {
            return( NULL );
        }
    }

    /* allocate memory for the new class */
    newclass = WRESALLOC( sizeof( ControlClass ) + stringlen );
    if( newclass == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    } else {
        /* copy the class or string into the correct place */
        if( flags == 0xffff ) {
            newclass->Class = (uint_8)class;
        } else if( flags == 0 ) {
            newclass->ClassName[0] = '\0';                  /* NUL character */
        } else {
            newclass->ClassName[0] = UNI2ASCII( flags );    /* first 16-bit UNICODE character */
            memcpy( newclass->ClassName + 1, restofstring, stringlen );
        }
    }

    if( restofstring != NULL ) {
        WRESFREE( restofstring );
    }

    return( newclass );
}

bool ResReadDialogBoxControl( DialogBoxControl *control, FILE *fp )
/*****************************************************************/
{
    bool            error;

    if( ResReadDialogSizeInfo( &(control->SizeInfo), fp ) )
        return( true );
    error = false;
    control->ID = ResReadUint16( &error, fp );
    if( error )
        return( true );
    control->Style = ResReadUint32( &error, fp );
    if( error )
        return( true );
    control->ClassID = ReadControlClass( fp );
    if( control->ClassID == NULL )
        return( true );
    control->Text = ResReadNameOrOrdinal( fp );
    if( control->Text == NULL )
        return( true );
    control->ExtraBytes = ResReadUint8( &error, fp );
    return( error );
}

static bool ResReadDialogControlCommon32( ControlClass **class_id, ResNameOrOrdinal **text, uint_16 *extra_bytes, FILE *fp )
{
    bool            error;

    *class_id = Read32ControlClass( fp );
    if( *class_id == NULL )
        return( true );
    *text = ResRead32NameOrOrdinal( fp );
    if( *text == NULL )
        return( true );
    error = false;
    *extra_bytes = ResReadUint16( &error, fp );
    return( error );
}

bool ResReadDialogBoxControl32( DialogBoxControl32 *control, FILE *fp )
/*********************************************************************/
{
    bool            error;

    error = false;
    control->Style = ResReadUint32( &error, fp );
    if( error )
        return( true );
    control->ExtendedStyle = ResReadUint32( &error, fp );
    if( error )
        return( true );
    if( ResReadDialogSizeInfo( &(control->SizeInfo), fp ) )
        return( true );
    control->ID = ResReadUint16( &error, fp );
    if( error )
        return( true );
    if( ResReadDialogControlCommon32( &(control->ClassID), &(control->Text), &(control->ExtraBytes), fp ) )
        return( true );
    /* seek to dword boundary if necessary */
    return( ResReadPadDWord( fp ) );
}

bool ResReadDialogBoxExControl32( DialogBoxExControl32 *control, FILE *fp )
/*************************************************************************/
{
    bool            error;

    error = false;
    control->HelpId = ResReadUint32( &error, fp );
    if( error )
        return( true );
    control->ExtendedStyle = ResReadUint32( &error, fp );
    if( error )
        return( true );
    control->Style = ResReadUint32( &error, fp );
    if( error )
        return( true );
    if( ResReadDialogSizeInfo( &(control->SizeInfo), fp ) )
        return( true );
    control->ID = ResReadUint32( &error, fp );
    if( error )
        return( true );
    if( ResReadDialogControlCommon32( &(control->ClassID), &(control->Text), &(control->ExtraBytes), fp ) )
        return( true );
    /* seek to dword boundary if necessary */
    return( ResReadPadDWord( fp ) );
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
