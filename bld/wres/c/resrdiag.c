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
    if( !error ) {
        sizeinfo->y = ResReadUint16( &error, fp );
    }
    if( !error ) {
        sizeinfo->width = ResReadUint16( &error, fp );
    }
    if( !error ) {
        sizeinfo->height = ResReadUint16( &error, fp );
    }
    return( error );
}

bool ResReadDialogBoxHeader( DialogBoxHeader *head, FILE *fp )
/************************************************************/
{
    bool            error;
    uint_32         tmp32;

    error = ResReadUint32( &tmp32, fp );
    head->Style = tmp32;
    if( !error ) {
        head->NumOfItems = ResReadUint8( &error, fp );
    }
    if( !error ) {
        error = ResReadDialogSizeInfo( &(head->SizeInfo), fp );
    }
    if( !error ) {
        head->MenuName = ResReadNameOrOrdinal( fp );
        error = (head->MenuName == NULL);
    }
    if( !error ) {
        head->ClassName = ResReadNameOrOrdinal( fp );
        error = (head->ClassName == NULL);
    }
    if( !error ) {
        head->Caption = ResReadString( fp, NULL );
        error = (head->Caption == NULL);
    }

    /* if the font was set input the font name and point size */
    if( !error && (head->Style & DS_SETFONT) ) {
        head->PointSize = ResReadUint16( &error, fp );
        if( !error ) {
            head->FontName = ResReadString( fp, NULL );
            error = (head->FontName == NULL);
        }
    } else {
        head->FontName = NULL;
    }

    return( error );
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
    if( !error ) {
        sign1 = ResReadUint16( &error, fp );
        if( !error ) {
            return( sign0 == 0x0001 && sign1 == 0xFFFF );
        }
    }
    return( false );
}

static bool ResReadDialogHeaderCommon32( DialogBoxHeader32 *head, FILE *fp )
{
    bool    error;

    head->MenuName = ResRead32NameOrOrdinal( fp );
    error = (head->MenuName == NULL);
    if( !error ) {
        head->ClassName = ResRead32NameOrOrdinal( fp );
        error = (head->ClassName == NULL);
    }
    if( !error ) {
        head->Caption = ResRead32String( fp, NULL );
        error = (head->Caption == NULL);
    }
    return( error );
}

bool ResReadDialogBoxHeader32( DialogBoxHeader32 *head, FILE *fp )
/****************************************************************/
{
    bool            error;
    uint_32         tmp32;

    error = ResReadUint32( &tmp32, fp );
    head->Style = tmp32;
    if( !error ) {
        error = ResReadUint32( &tmp32, fp );
        head->ExtendedStyle = tmp32;
    }
    if( !error ) {
        head->NumOfItems = ResReadUint16( &error, fp );
    }
    if( !error ) {
        error = ResReadDialogSizeInfo( &(head->SizeInfo), fp );
    }
    if( !error ) {
        error = ResReadDialogHeaderCommon32( head, fp );
    }

    /* if the font was set input the font name and point size */
    if( !error && (head->Style & DS_SETFONT) ) {
        head->PointSize = ResReadUint16( &error, fp );
        if( !error ) {
            head->FontName = ResRead32String( fp, NULL );
            error = (head->FontName == NULL);
        }
    } else {
        head->FontName = NULL;
    }

    if( !error ) {
        /* seek to dword boundary if necessary */
        error = ResReadPadDWord( fp );
    }
    return( error );
}

bool ResReadDialogBoxExHeader32( DialogBoxHeader32 *head, DialogBoxExHeader32short *exhead, FILE *fp )
/****************************************************************************************************/
{
    bool            error;
    uint_32         tmp32;

    /* Read in the miscellaneous two WORDs 0x0001, 0xFFFF */
    error = !ResIsDialogBoxEx( fp );
    if( !error ) {
        error = ResReadUint32( &tmp32, fp );
        exhead->HelpId = tmp32;
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, fp );
        head->ExtendedStyle = tmp32;
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, fp );
        head->Style = tmp32;
    }
    if( !error ) {
        head->NumOfItems = ResReadUint16( &error, fp );
    }
    if( !error ) {
        error = ResReadDialogSizeInfo( &(head->SizeInfo), fp );
    }
    if( !error ) {
        error = ResReadDialogHeaderCommon32( head, fp );
    }

    /* If the font was set, write the font information */
    if( !error && (head->Style & DS_SETFONT) ) {
        head->PointSize = ResReadUint16( &error, fp );
        if( !error ) {
            exhead->FontWeight = ResReadUint16( &error, fp );
        }
        if( !error ) {
            exhead->FontItalic = ResReadUint8( &error, fp );
        }
        if( !error ) {
            exhead->FontCharset = ResReadUint8( &error, fp );
        }
        if( !error ) {
            head->FontName = ResRead32String( fp, NULL );
            error = (head->FontName == NULL);
        }
    }

    if( !error ) {
        /* seek to dword boundary if necessary */
        error = ResReadPadDWord( fp );
    }
    return( error );
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
        error = false;
    } else {
        restofstring = ResReadString( fp, &stringlen );
        stringlen++;    /* for the '\0' */
        error = (restofstring == NULL);
    }

    /* allocate memory for the new class */
    newclass = NULL;
    if( !error ) {
        newclass = WRESALLOC( sizeof( ControlClass ) + stringlen );
        if( newclass == NULL ) {
            error = WRES_ERROR( WRS_MALLOC_FAILED );
        }
    }

    /* copy the class or string into the correct place */
    if( !error ) {
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
    } else if( flags == 0 ) {
        error = false;
    } else {
        restofstring = ResRead32String( fp, &stringlen );
        stringlen++;                /* for the '\0' */
        error = (restofstring == NULL);
    }

    /* allocate memory for the new class */
    newclass = NULL;
    if( !error ) {
        newclass = WRESALLOC( sizeof( ControlClass ) + stringlen );
        if( newclass == NULL ) {
            error = WRES_ERROR( WRS_MALLOC_FAILED );
        }
    }

    /* copy the class or string into the correct place */
    if( !error ) {
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
    uint_32         tmp32;

    error = ResReadDialogSizeInfo( &(control->SizeInfo), fp );
    if( !error ) {
        control->ID = ResReadUint16( &error, fp );
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, fp );
        control->Style = tmp32;
    }
    if( !error ) {
        control->ClassID = ReadControlClass( fp );
        error = ( control->ClassID == NULL );
    }
    if( !error ) {
        control->Text = ResReadNameOrOrdinal( fp );
    }
    if( !error ) {
        control->ExtraBytes = ResReadUint8( &error, fp );
    }

    return( error );
}

static bool ResReadDialogControlCommon32( ControlClass **class_id, ResNameOrOrdinal **text, uint_16 *extra_bytes, FILE *fp )
{
    bool            error;

    *class_id = Read32ControlClass( fp );
    error = ( *class_id == NULL );
    if( !error ) {
        *text = ResRead32NameOrOrdinal( fp );
    }
    if( !error ) {
        *extra_bytes = ResReadUint16( &error, fp );
    }

    return( error );
}

bool ResReadDialogBoxControl32( DialogBoxControl32 *control, FILE *fp )
/*********************************************************************/
{
    bool            error;
    uint_32         tmp32;

    error = ResReadUint32( &tmp32, fp );
    control->Style = tmp32;
    if( !error ) {
        error = ResReadUint32( &tmp32, fp );
        control->ExtendedStyle = tmp32;
    }
    if( !error ) {
        error = ResReadDialogSizeInfo( &(control->SizeInfo), fp );
    }
    if( !error ) {
        control->ID = ResReadUint16( &error, fp );
    }
    if( !error ) {
        error = ResReadDialogControlCommon32( &(control->ClassID), &(control->Text), &(control->ExtraBytes), fp );
    }

    if( !error ) {
        /* seek to dword boundary if necessary */
        error = ResReadPadDWord( fp );
    }
    return( error );
}

bool ResReadDialogBoxExControl32( DialogBoxExControl32 *control, FILE *fp )
/*************************************************************************/
{
    bool            error;
    uint_32         tmp32;

    error = ResReadUint32( &tmp32, fp );
    control->HelpId = tmp32;
    if( !error ) {
        error = ResReadUint32( &tmp32, fp );
        control->ExtendedStyle = tmp32;
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, fp );
        control->Style = tmp32;
    }
    if( !error ) {
        error = ResReadDialogSizeInfo( &(control->SizeInfo), fp );
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, fp );
        control->ID = tmp32;
    }
    if( !error ) {
        error = ResReadDialogControlCommon32( &(control->ClassID), &(control->Text), &(control->ExtraBytes), fp );
    }

    if( !error ) {
        /* seek to dword boundary if necessary */
        error = ResReadPadDWord( fp );
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
