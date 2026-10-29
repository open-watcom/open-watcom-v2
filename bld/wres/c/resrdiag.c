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


static bool ResReadDialogSizeInfo( DialogSizeInfo *SizeInfo, FILE *fp )
/*********************************************************************/
{
    bool        error;

    error = false;
    SizeInfo->x = ResReadUint16( &error, fp );
    if( error )
        return( true );
    SizeInfo->y = ResReadUint16( &error, fp );
    if( error )
        return( true );
    SizeInfo->width = ResReadUint16( &error, fp );
    if( error )
        return( true );
    SizeInfo->height = ResReadUint16( &error, fp );
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

bool ResReadDialogBoxHeader32( DialogBoxHeader *head, FILE *fp )
/**************************************************************/
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
    head->MenuName = ResReadNameOrOrdinal32( fp );
    if( head->MenuName == NULL )
        return( true );
    head->ClassName = ResReadNameOrOrdinal32( fp );
    if( head->ClassName == NULL )
        return( true );
    head->Caption = ResReadString32( fp, NULL );
    if( head->Caption == NULL )
        return( true );

    /* if the font was set input the font name and point size */
    head->FontName = NULL;
    if( head->Style & DS_SETFONT ) {
        head->PointSize = ResReadUint16( &error, fp );
        if( error )
            return( true );
        head->FontName = ResReadString32( fp, NULL );
        if( head->FontName == NULL ) {
            return( true );
        }
    }
    /* seek to dword boundary if necessary */
    return( ResReadPadDWord( fp ) );
}

bool ResReadDialogBoxExHeader( DialogBoxHeader *head, FILE *fp )
/**************************************************************/
{
    bool            error;

    /* Read in the miscellaneous two WORDs 0x0001, 0xFFFF */
    if( !ResIsDialogBoxEx( fp ) )
        return( true );
    error = false;
    head->HelpId = ResReadUint32( &error, fp );
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
    head->MenuName = ResReadNameOrOrdinal32( fp );
    if( head->MenuName == NULL )
        return( true );
    head->ClassName = ResReadNameOrOrdinal32( fp );
    if( head->ClassName == NULL )
        return( true );
    head->Caption = ResReadString32( fp, NULL );
    if( head->Caption == NULL )
        return( true );

    /* If the font was set, write the font information */
    head->FontName = NULL;
    if( head->Style & DS_SETFONT ) {
        head->PointSize = ResReadUint16( &error, fp );
        if( error )
            return( true );
        head->FontWeight = ResReadUint16( &error, fp );
        if( error )
            return( true );
        head->FontItalic = ResReadUint8( &error, fp );
        if( error )
            return( true );
        head->FontCharset = ResReadUint8( &error, fp );
        if( error )
            return( true );
        head->FontName = ResReadString32( fp, NULL );
        if( head->FontName == NULL ) {
            return( true );
        }
    }
    /* seek to dword boundary if necessary */
    return( ResReadPadDWord( fp ) );
}

static ResNameOrOrdinal *readControlClass( FILE *fp )
/***************************************************/
{
    ResNameOrOrdinal	*cclass;
    uint_8          	flags;
    bool            	error;
    size_t          	stringlen;
    char            	*restofstring;

    /* read in the first byte */
    error = false;
    flags = ResReadUint8( &error, fp );
    if( error )
        return( NULL );

    restofstring = NULL;
    stringlen = 0;
    if( flags & 0x80 ) {
    } else if( flags == 0 ) {
    } else {
        restofstring = ResReadString( fp, &stringlen );
        if( restofstring == NULL ) {
            return( NULL );
        }
        stringlen++;    /* for the '\0' */
    }

    /* allocate memory for the new class */
    cclass = WRESALLOC( sizeof( ResNameOrOrdinal ) + stringlen );
    if( cclass == NULL ) {
        WRES_ERROR( WRS_MALLOC_FAILED );
    /* copy the class or string into the correct place */
    } else if( flags & 0x80 ) {
        cclass->ord.fFlag = 0xff;
        cclass->ord.wOrdinalID = flags;
    } else if( flags == 0 ) {
        cclass->name[0] = '\0';
    } else {
        cclass->name[0] = flags;
        memcpy( cclass->name + 1, restofstring, stringlen );
    }

    if( restofstring != NULL ) {
        WRESFREE( restofstring );
    }
    return( cclass );
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
    control->ClassID = readControlClass( fp );
    if( control->ClassID == NULL )
        return( true );
    control->Text = ResReadNameOrOrdinal( fp );
    if( control->Text == NULL )
        return( true );
    control->ExtraBytes = ResReadUint8( &error, fp );
    return( error );
}

bool ResReadDialogBoxControl32( DialogBoxControl *control, FILE *fp )
/*******************************************************************/
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
    control->ClassID = ResReadNameOrOrdinal32( fp );
    if( control->ClassID == NULL )
        return( true );
    control->Text = ResReadNameOrOrdinal32( fp );
    if( control->Text == NULL )
        return( true );
    control->ExtraBytes = ResReadUint16( &error, fp );
    if( error )
        return( true );

    /* seek to dword boundary if necessary */
    return( ResReadPadDWord( fp ) );
}

bool ResReadDialogBoxExControl( DialogBoxControl *control, FILE *fp )
/*******************************************************************/
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
    control->ClassID = ResReadNameOrOrdinal32( fp );
    if( control->ClassID == NULL )
        return( true );
    control->Text = ResReadNameOrOrdinal32( fp );
    if( control->Text == NULL )
        return( true );
    control->ExtraBytes = ResReadUint16( &error, fp );
    if( error )
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

void ResFreeDialogBoxHeaderPtrs32( DialogBoxHeader *head )
/********************************************************/
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
