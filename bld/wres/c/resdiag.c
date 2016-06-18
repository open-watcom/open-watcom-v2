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

static bool ResWriteDialogHeaderCommon32( DialogBoxHeader32 *head, WResFileID handle, bool add_quotes );
static bool ResWriteDialogControlCommon32( ControlClass *class_id, ResNameOrOrdinal *text, uint_16 extra_bytes, WResFileID handle );

bool ResWriteDialogBoxHeader( DialogBoxHeader *head, WResFileID handle )
/**********************************************************************/
{
    bool            error;
    uint_16         tmp16;

    /* write out the fixed size portion of the structure */
    /* the fixed portion is everything up to, but not including, MenuName */
    error = ( WRESWRITE( handle, head, offsetof( DialogBoxHeader, MenuName ) ) != offsetof( DialogBoxHeader, MenuName ) );
    if( error ) {
        WRES_ERROR( WRS_WRITE_FAILED );
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
        tmp16 = head->PointSize;
        error = ResWriteUint16( &tmp16, handle );
        if( !error ) {
            error = ResWriteString( head->FontName, false, handle );
        }
    }
    return( error );
}

bool ResWriteDialogBoxHeader32( DialogBoxHeader32 *head, WResFileID handle )
/**************************************************************************/
{
    bool            error;
    uint_16         tmp16;

    /* write out the fixed size portion of the structure */
    /* the fixed portion is everything up to, but not including, MenuName */
    error = ( WRESWRITE( handle, head, offsetof( DialogBoxHeader32, MenuName ) ) != offsetof( DialogBoxHeader32, MenuName ) );
    if( error ) {
        WRES_ERROR( WRS_WRITE_FAILED );
    }
    if( !error ) {
        error = ResWriteDialogHeaderCommon32( head, handle, false );
    }
    /* if the font was set output the font name and point size */
    if( !error && (head->Style & DS_SETFONT) ) {
        tmp16 = head->PointSize;
        error = ResWriteUint16( &tmp16, handle );
        if( !error ) {
            error = ResWriteString( head->FontName, true, handle );
        }
    }
    return( error );
}


bool ResWriteDialogExHeader32( DialogBoxHeader32 *head, DialogExHeader32 *exhead,
                                                              WResFileID handle )
/*******************************************************************************/
{
    bool            error;
    uint_16         miscbytes[2] = { 0x0001, 0xFFFF };
    uint_16         tmp16;
    uint_32         tmp32;

    /* Write out the miscellaneous two WORDs 01 00 FF FF */

    error = ResWriteUint16( miscbytes, handle );
    if( !error ) {
        error = ResWriteUint16( miscbytes + 1, handle );
    }
    if( !error ) {
        error = ResWriteUint32( &(exhead->HelpId), handle );
    }
    if( !error ) {
        tmp32 = head->ExtendedStyle;
        error = ResWriteUint32( &tmp32, handle );
    }
    if( !error ) {
        tmp32 = head->Style;
        error = ResWriteUint32( &tmp32, handle );
    }
    if( !error ) {
        tmp16 = head->NumOfItems;
        error = ResWriteUint16( &tmp16, handle );
    }
    if( !error ) {
        error = ( WRESWRITE( handle, &(head->Size), sizeof( DialogSizeInfo ) ) != sizeof( DialogSizeInfo ) );
        if( error ) {
            WRES_ERROR( WRS_WRITE_FAILED );
        }
    }
    if( !error ) {
        error = ResWriteDialogHeaderCommon32( head, handle, true );
    }
//    if( !error ) {                    //DRW - commented Sep 25/95
//        error = ResPadDWord( handle );
//    }
    /* If the font was set, write the font information */
    if( !error && (head->Style & DS_SETFONT) ) {
        tmp16 = head->PointSize;
        error = ResWriteUint16( &tmp16, handle );
        if( !error ) {
            error = ResWriteUint16( &(exhead->FontWeight), handle );
        }
        if( !error ) {
            error = ResWriteUint8( &(exhead->FontItalic), handle );
        }
        if( !error ) {
            error = ResWriteUint8( &(exhead->FontExtra), handle );
        }
        if( !error ) {
            error = ResWriteString( head->FontName, true, handle );
        }
        if( !error ) {
            error = ResPadDWord( handle );
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
            newname = WRESALLOC( ( len + 3 ) * sizeof( char ) );
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

void ResFreeDialogBoxHeaderPtrs( DialogBoxHeader * head )
/*******************************************************/
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

void ResFreeDialogBoxHeader32Ptrs( DialogBoxHeader32 * head )
/***********************************************************/
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

bool ResReadDialogBoxHeader32( DialogBoxHeader32 *head, WResFileID handle )
/*************************************************************************/
{
    bool            error;
    WResFileSSize   numread;
    uint_16         tmp16;

    /* read in the fixed size portion of the structure */
    /* the fixed portion is everything up to, but not including, MenuName */
    numread = WRESREAD( handle, head, offsetof( DialogBoxHeader32, MenuName ) );
    error = ( numread != offsetof( DialogBoxHeader32, MenuName ) );
    if( error ) {
        WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
    }

    if( !error ) {
        head->MenuName = ResRead32NameOrOrdinal( handle );
        error = (head->MenuName == NULL);
    }
    if( !error ) {
        head->ClassName = ResRead32NameOrOrdinal( handle );
        error = (head->ClassName == NULL);
    }
    if( !error ) {
        head->Caption = ResRead32String( handle, NULL );
        error = (head->Caption == NULL);
    }

    /* if the font was set input the font name and point size */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResReadUint16( &tmp16, handle );
        head->PointSize = tmp16;
        if( !error ) {
            head->FontName = ResRead32String( handle, NULL );
            error = (head->FontName == NULL);
        }
    } else {
        head->FontName = NULL;
    }

    return( error );
}

bool ResReadDialogBoxHeader( DialogBoxHeader *head, WResFileID handle )
/*********************************************************************/
{
    bool            error;
    WResFileSSize   numread;
    uint_16         tmp16;

    /* read in the fixed size portion of the structure */
    /* the fixed portion is everything up to, but not including, MenuName */
    numread = WRESREAD( handle, head, offsetof( DialogBoxHeader, MenuName ) );
    error = ( numread != offsetof( DialogBoxHeader, MenuName ) );
    if( error ) {
        WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
    }

    if( !error ) {
        head->MenuName = ResReadNameOrOrdinal( handle );
        error = (head->MenuName == NULL);
    }
    if( !error ) {
        head->ClassName = ResReadNameOrOrdinal( handle );
        error = (head->ClassName == NULL);
    }
    if( !error ) {
        head->Caption = ResReadString( handle, NULL );
        error = (head->Caption == NULL);
    }

    /* if the font was set input the font name and point size */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResReadUint16( &tmp16, handle );
        head->PointSize = tmp16;
        if( !error ) {
            head->FontName = ResReadString( handle, NULL );
            error = (head->FontName == NULL);
        }
    } else {
        head->FontName = NULL;
    }

    return( error );
}

bool ResIsDialogEx( WResFileID handle )
/*************************************/
{
    WResFileSSize   numread;
    uint_16         signa[2];

    /* read in the signature part of the header and check it */
    numread = WRESREAD( handle, signa, sizeof( signa ) );
    if( numread != sizeof( signa ) ) {
        WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
    } else {
        if( signa[0] == 0x0001 && signa[1] == 0xFFFF ) {
            return( true );
        }
    }
    return( false );
}

bool ResReadDialogExHeader32( DialogBoxHeader32 *head, DialogExHeader32 *exhead,
                                                             WResFileID handle )
/******************************************************************************/
{
    bool            error;
    WResFileSSize   numread;
    uint_16         miscbytes[2] = { 0x0000, 0x0000 };
    uint_16         tmp16;
    uint_32         tmp32;

    /* Read in the miscellaneous two WORDs 01 00 FF FF */
    error = ResReadUint16( miscbytes, handle );
    if( !error ) {
        error = ResReadUint16( miscbytes + 1, handle );
    }
    if( !error ) {
        error = !(miscbytes[0] == 0x0001 && miscbytes[1] == 0xffff);
    }

    if( !error ) {
        error = ResReadUint32( &(exhead->HelpId), handle );
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, handle );
        head->ExtendedStyle = tmp32;
    }
    if( !error ) {
        error = ResReadUint32( &tmp32, handle );
        head->Style = tmp32;
    }
    if( !error ) {
        error = ResReadUint16( &tmp16, handle );
        head->NumOfItems = tmp16;
    }
    if( !error ) {
        numread = WRESREAD( handle, &(head->Size), sizeof( DialogSizeInfo ) );
        error = ( numread != sizeof( DialogSizeInfo ) );
        if( error ) {
            WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
        }
    }
    if( !error ) {
        head->MenuName = ResRead32NameOrOrdinal( handle );
        error = (head->MenuName == NULL);
    }
    if( !error ) {
        head->ClassName = ResRead32NameOrOrdinal( handle );
        error = (head->ClassName == NULL);
    }
    if( !error ) {
        head->Caption = ResRead32String( handle, NULL );
        error = (head->Caption == NULL);
    }

    /* not needed ???
    if( !error ) {
        error = ResPadDWord( handle );
    }
    */

    /* If the font was set, write the font information */
    if( !error && (head->Style & DS_SETFONT) ) {
        error = ResReadUint16( &tmp16, handle );
        head->PointSize = tmp16;
        if( !error ) {
            error = ResReadUint16( &(exhead->FontWeight), handle );
        }
        if( !error ) {
            error = ResReadUint8( &(exhead->FontItalic), handle );
        }
        if( !error ) {
            error = ResReadUint8( &(exhead->FontExtra), handle );
        }
        if( !error ) {
            head->FontName = ResRead32String( handle, NULL );
            error = (head->FontName == NULL);
        }
        if( !error ) {
            error = ResPadDWord( handle );
        }
    }

    return( error );
}

bool ResWriteDialogBoxControl( DialogBoxControl *control, WResFileID handle )
/***************************************************************************/
{
    bool            error;
    uint_8          tmp8;

    /* write the fixed part of the structure */
    /* the structure is fixed up to, but not including, ClassID */
    error = ( WRESWRITE( handle, control, offsetof( DialogBoxControl, ClassID ) ) != offsetof( DialogBoxControl, ClassID ) );
    if( error ) {
        WRES_ERROR( WRS_WRITE_FAILED );
    }

    /* if the ClassID is one of the predefined ones write it out as a byte */
    /* otherwise it is a string */
    if( !error ) {
        if( control->ClassID->Class & 0x80 ) {
            error = ResWriteUint8( &(control->ClassID->Class), handle );
        } else {
            error = ResWriteString( control->ClassID->ClassName, false, handle);
        }
    }

    if( !error ) {
        error = ResWriteNameOrOrdinal( control->Text, false, handle );
    }
    if( !error ) {
        tmp8 = control->ExtraBytes;
        error = ResWriteUint8( &tmp8, handle );
    }

    return( error );
}

bool ResWriteDialogBoxControl32( DialogBoxControl32 *control, WResFileID handle )
/*******************************************************************************/
{
    bool            error;

    /* write the fixed part of the structure */
    /* the structure is fixed up to, but not including, ClassID */
    error = ( WRESWRITE( handle, control, offsetof( DialogBoxControl32, ClassID ) ) != offsetof( DialogBoxControl32, ClassID ) );
    if( error ) {
        WRES_ERROR( WRS_WRITE_FAILED );
    }
    if( !error ) {
        error = ResWriteDialogControlCommon32( control->ClassID,
                                control->Text, control->ExtraBytes, handle );
    }

    return( error );
}

bool ResWriteDialogExControl32( DialogBoxExControl32 *control, WResFileID handle )
/********************************************************************************/
{
    bool            error;

    /* write the fixed part of the structure */
    /* the structure is fixed up to, but not including, ClassID */
    error = ( WRESWRITE( handle, control, offsetof( DialogBoxExControl32, ClassID ) ) != offsetof( DialogBoxExControl32, ClassID ) );
    if( error ) {
        WRES_ERROR( WRS_WRITE_FAILED );
    } else {
        error = ResWriteDialogControlCommon32( control->ClassID,
                                control->Text, control->ExtraBytes, handle );
    }

    return( error );
}

static bool ResWriteDialogControlCommon32( ControlClass *class_id, ResNameOrOrdinal *text,
                                                  uint_16 extra_bytes, WResFileID handle )
/****************************************************************************************/
{
    bool      error;
    uint_16   class_num;

    /* if the ClassID is one of the predefined ones write it out as a byte */
    /* otherwise it is a string */
    if( class_id->Class & 0x80 ) {
        /*the class number is prefixed by 0xFFFF to distinguish it
         * from a string */
        class_num = 0xFFFF;
        error = ResWriteUint16( &class_num, handle );
        if( !error ) {
            class_num =  class_id->Class;
            error = ResWriteUint16( &class_num, handle );
        }
    } else {
        error = ResWriteString( class_id->ClassName, true, handle );
    }
    if( !error ) {
        error = ResWriteNameOrOrdinal( text, true, handle );
    }

    if( !error ) {
        error = ResWriteUint16( &extra_bytes, handle );
    }

    return( error );
}

static ControlClass *ReadControlClass( WResFileID handle )
/********************************************************/
{
    ControlClass *  newclass;
    uint_8          class;
    bool            error;
    int             stringlen;
    char *          restofstring;

    restofstring = NULL;
    stringlen = 0;

    /* read in the first byte */
    error = ResReadUint8( &class, handle );
    if( !error ) {
        if( (class & 0x80) == 0 && class != '\0' ) {
            restofstring = ResReadString( handle, &stringlen );
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

static ControlClass *Read32ControlClass( WResFileID handle )
/**********************************************************/
{
    ControlClass *  newclass;
    uint_16         flags;
    uint_16         class;
    bool            error;
    int             stringlen;
    char *          restofstring;

    restofstring = NULL;
    stringlen = 0;
    class = 0;

    /* read in the first word */
    error = ResReadUint16( &flags, handle );
    if( !error ) {
        if( flags == 0xffff ) {
            error = ResReadUint16( &class, handle );
        } else {
            class = flags & 0xFF;       /* first 16-bit UNICODE character */
            restofstring = ResRead32String( handle, &stringlen );
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
        if( flags != 0xffff ) {
            memcpy( newclass->ClassName + 1, restofstring, stringlen );
        }
    }

    if( restofstring != NULL ) {
        WRESFREE( restofstring );
    }

    return( newclass );
}

bool ResReadDialogBoxControl( DialogBoxControl *control, WResFileID handle )
/**************************************************************************/
{
    bool            error;
    WResFileSSize   numread;
    uint_8          tmp8;

    /* read the fixed part of the structure */
    /* the structure is fixed up to, but not including, ClassID */
    numread = WRESREAD( handle, control, offsetof( DialogBoxControl, ClassID ) );
    error = ( numread != offsetof( DialogBoxControl, ClassID ) );
    if( error ) {
        WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
    }

    if( !error ) {
        control->ClassID = ReadControlClass( handle );
        error = (control->ClassID == NULL);
    }

    if( !error ) {
        control->Text = ResReadNameOrOrdinal( handle );
    }
    if( !error ) {
        error = ResReadUint8( &tmp8, handle );
        control->ExtraBytes = tmp8;
    }

    return( error );
}

bool ResReadDialogBoxControl32( DialogBoxControl32 *control, WResFileID handle )
/******************************************************************************/
{
    bool            error;
    WResFileSSize   numread;
    uint_16         tmp16;

    error = ResPadDWord( handle );

    /* read the fixed part of the structure */
    /* the structure is fixed up to, but not including, ClassID */
    if( !error ) {
        numread = WRESREAD( handle, control, offsetof( DialogBoxControl32, ClassID ) );
        error = ( numread != offsetof( DialogBoxControl32, ClassID ) );
        if( error ) {
            WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
        }
    }

    if( !error ) {
        control->ClassID = Read32ControlClass( handle );
        error = (control->ClassID == NULL);
    }

    if( !error ) {
        control->Text = ResRead32NameOrOrdinal( handle );
    }
    if( !error ) {
        error = ResReadUint16( &tmp16, handle );
        control->ExtraBytes = tmp16;
    }

    return( error );
}

bool ResReadDialogExControl32( DialogBoxExControl32 *control, WResFileID handle )
/*******************************************************************************/
{
    bool            error;
    WResFileSSize   numread;

    error = ResPadDWord( handle );

    /* read the fixed part of the structure */
    /* the structure is fixed up to, but not including, ClassID */
    if( !error ) {
        numread = WRESREAD( handle, control, offsetof( DialogBoxExControl32, ClassID ) );
        error = ( numread != offsetof( DialogBoxExControl32, ClassID ) );
        if( error ) {
            WRES_ERROR( WRESIOERR( handle, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE );
        }
    }

    if( !error ) {
        control->ClassID = Read32ControlClass( handle );
        error = (control->ClassID == NULL);
    }

    if( !error ) {
        control->Text = ResRead32NameOrOrdinal( handle );
    }
    if( !error ) {
        error = ResReadUint16( &(control->ExtraBytes), handle );
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
            class->Class = classnum;
        }
    } else {
        class = WRESALLOC( sizeof( ControlClass ) + 1 );
        if( class == NULL ) {
            WRES_ERROR( WRS_MALLOC_FAILED );
        } else {
            class->Class = classnum;
            *(class->ClassName + 1) = '\0';
        }
    }
    return( class );
}
