/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include <string.h>
#include "layer0.h"
#include "filefmt.h"
#include "resfmt.h"
#include "mresfmt.h"
#include "write.h"
#include "reserr.h"
#include "read.h"
#include "wresrtns.h"


#define CONV_BUF_SIZE           512

static char     ConvBuffer[CONV_BUF_SIZE];

static size_t DefaultUNIConversion( const char *str, size_t len, char *buf, size_t size )
/***************************************************************************************/
{
    size_t  i;

    if( buf != NULL ) {
        size /= 2;
        if( len > size )
            len = size;
        for( i = 0; i < len; i++ ) {
            *buf++ = *str++;
            *buf++ = 0;
        }
    }
    return( 2 * len );
}

static size_t DefaultMBConversion( const char *str, size_t len, char *buf, size_t size )
/**************************************************************************************/
{
    if( buf != NULL ) {
        if( len > size )
            len = size;
        memcpy( buf, str, len );
    }
    return( len );
}

bool ResWriteUint8( FILE *fp, uint_8 value )
/******************************************/
{
    if( WRESWRITE( fp, &value, sizeof( value ) ) != sizeof( value ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}

bool ResWriteUint16( FILE *fp, uint_16 value )
/********************************************/
{
    CONV_LE_16( value );
    if( WRESWRITE( fp, &value, sizeof( value ) ) != sizeof( value ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}

bool ResWriteUint32( FILE *fp, uint_32 value )
/********************************************/
{
    CONV_LE_32( value );
    if( WRESWRITE( fp, &value, sizeof( value ) ) != sizeof( value ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}

bool ResWritePadDWord( FILE *fp )
/*******************************/
{
    long            curr_pos;
    size_t          padding;
    uint_32         zero = 0;

    curr_pos = WRESTELL( fp );
    if( curr_pos == -1L )
        return( WRES_ERROR( WRS_TELL_FAILED ) );
    padding = RES_PADDING_DWORD( curr_pos );
    if( padding != 0 ) {
        if( WRESWRITE( fp, &zero, padding ) != padding ) {
            return( WRES_ERROR( WRS_WRITE_FAILED ) );
        }
    }
    return( false );
}

bool WResWriteWResIDNameString( const WResIDName *name_id, bool use_unicode, FILE *fp )
/*************************************************************************************/
{
    if( name_id != NULL && name_id->NumChars != 0 ) {
        return( ResWriteStringLen( name_id->Name, name_id->NumChars, use_unicode, true, fp ) );
    } else if( use_unicode ) {
        return( ResWriteUint16( fp, 0 ) );
    } else {
        return( ResWriteUint8( fp, 0 ) );
    }
} /* WResWriteWResIDNameString */

bool WResWriteWResIDName( const WResIDName *name_id, FILE *fp )
/**************************************************************
 * write type and resource IDs names (only ASCII characters)
 */
{
    bool            error;
    unsigned        numchars;

    numchars = name_id->NumChars;
    error = ResWriteUint16( fp, numchars );
    if( numchars > 0 ) {
        if( !error ) {
            if( WRESWRITE( fp, name_id->Name, numchars ) != numchars ) {
                error = WRES_ERROR( WRS_WRITE_FAILED );
            }
        }
    }
    return( error );
}

bool WResWriteWResID( const WResID *id, FILE *fp )
/************************************************/
{
    if( ResWriteUint8( fp, id->IsName ) )
        return( true );
    if( id->IsName ) {
        return( WResWriteWResIDName( &(id->ID.Name), fp ) );
    } else {
        return( ResWriteUint16( fp, id->ID.Num ) );
    }
}

/*
 * WResWriteTypeRecord - write the type record to the current postion
 *                       in the file identified by fp
 */
bool WResWriteTypeRecord( const WResTypeInfo *type, FILE *fp )
/************************************************************/
{
    if( ResWriteUint16( fp, type->NumResources ) )
        return( true );
    return( WResWriteWResID( &type->TypeName, fp ) );
}

/*
 * WResWriteResRecord - write the resource record to the current position
 *                      in the file identified by  fp
 */
bool WResWriteResRecord( const WResResInfo *res, FILE *fp )
/*********************************************************/
{
    if( ResWriteUint16( fp, res->NumResources ) )
        return( true );
    return( WResWriteWResID( &res->ResName, fp ) );
}

/*
 * WResWriteLangRecord - write out a language record at the current file
 *                       position
 */
bool WResWriteLangRecord( const WResLangInfo *info, FILE *fp )
/************************************************************/
{
    if( ResWriteUint16( fp, info->lang.lang ) )
        return( true );
    if( ResWriteUint8( fp, info->lang.sublang ) )
        return( true );
    if( ResWriteUint16( fp, info->MemoryFlags ) )
        return( true );
    if( ResWriteUint32( fp, info->Offset ) )
        return( true );
    return( ResWriteUint32( fp, info->Length ) );
}

bool WResWriteHeader( const WResHeader *header, FILE *fp )
/********************************************************/
{
    if( ResWriteUint32( fp, header->Magic[0] ) )
        return( true );
    if( ResWriteUint32( fp, header->Magic[1] ) )
        return( true );
    if( ResWriteUint32( fp, header->DirOffset ) )
        return( true );
    if( ResWriteUint16( fp, header->NumResources ) )
        return( true );
    if( ResWriteUint16( fp, header->NumTypes ) )
        return( true );
    return( ResWriteUint16( fp, header->WResVer ) );
}

bool WResWriteExtHeader( const WResExtHeader *extheader, FILE *fp )
/*****************************************************************/
{
    if( ResWriteUint16( fp, extheader->TargetOS ) )
        return( true );
    if( ResWriteUint16( fp, extheader->reserved[0] ) )
        return( true );
    if( ResWriteUint16( fp, extheader->reserved[1] ) )
        return( true );
    if( ResWriteUint16( fp, extheader->reserved[2] ) )
        return( true );
    return( ResWriteUint16( fp, extheader->reserved[3] ) );
}

bool ResWriteStringLen( const char *str, size_t numchars, bool use_unicode, bool with_len, FILE *fp )
/***************************************************************************************************/
{
    char            *buf;
    bool            error;
    size_t          size;

    buf = ConvBuffer;
    size = 2 * numchars;
    if( size > CONV_BUF_SIZE ) {
        buf = WRESALLOC( size );
    } else {
        size = CONV_BUF_SIZE;
    }
    if( use_unicode ) {
        size = ConvToUnicode( str, numchars, buf, size );
    } else {
        size = ConvToMultiByte( str, numchars, buf, size );
    }
    error = false;
    if( with_len ) {
        if( use_unicode ) {
            error = ResWriteUint16( fp, numchars );
        } else {
            numchars = size;
            if( numchars > 255 )
                numchars = 255;
            size = numchars;
            error = ResWriteUint8( fp, numchars );
        }
    }
    if( !error ) {
        error = ResWrite( buf, size, fp );
    }
    if( buf != ConvBuffer ) {
        WRESFREE( buf );
    }
    return( error );
}

bool ResWriteString( const char *str, bool use_unicode, FILE *fp )
/****************************************************************/
{
    size_t  len;

    /* if string is NULL output the null string */
    if( str == NULL ) {
        str = "";
    }

    /* the +1 is so we will output the '\0' as well */
    len = strlen( str ) + 1;
    return( ResWriteStringLen( str, len, use_unicode, false, fp ) );
}

bool ResWriteNameOrOrdinal( ResNameOrOrdinal *name, bool use_unicode, FILE *fp )
/******************************************************************************/
{
    bool        error;

    if( name == NULL ) {
        error = ResWriteString( "", use_unicode, fp );
    } else {
        if( name->ord.fFlag == 0xff ) {
            if( use_unicode ) {
                error = ResWriteUint16( fp, 0xffff );
            } else {
                error = ResWriteUint8( fp, 0xff );
            }
            if( !error ) {
                error = ResWriteUint16( fp, name->ord.wOrdinalID );
            }
        } else {
            error = ResWriteString( name->name, use_unicode, fp );
        }
    }

    return( error );
} /* ResWriteNameOrOrdinal */

static size_t MResFindNameOrOrdSize( ResNameOrOrdinal *data, bool use_unicode )
/*****************************************************************************/
{
    size_t  size;

    if( data->ord.fFlag == 0xff ) {
        size = 4;
    } else {
        if( use_unicode ) {
            size = 2 * ( strlen( data->name ) + 1 );
        } else {
            size = strlen( data->name ) + 1;
        }
    }
    return( size );
}

static size_t MResFindHeaderSize( MResResourceHeader *msheader, bool iswin32 )
/****************************************************************************/
{
    size_t  headersize;
    size_t  namesize;
    size_t  typesize;
    size_t  padding;

    namesize = MResFindNameOrOrdSize( msheader->Name, iswin32 );
    typesize = MResFindNameOrOrdSize( msheader->Type, iswin32 );
    if( iswin32 ) {
        headersize = 2 * sizeof( uint_32 ) + namesize + typesize;
        padding = RES_PADDING_DWORD( headersize );
        headersize += padding + 3 * sizeof( uint_32 ) + 2 * sizeof( uint_16 );
    } else {
        headersize = namesize + typesize + sizeof( uint_16 ) + sizeof( uint_32 );
    }
    return( headersize );
}

bool MResWriteResourceHeader( MResResourceHeader *msheader, bool iswin32, FILE *fp )
/**********************************************************************************/
{
    if( iswin32 ) {
        if( ResWriteUint32( fp, msheader->Size ) )
            return( true );
        if( ResWriteUint32( fp, MResFindHeaderSize( msheader, iswin32 ) ) )
            return( true );
        if( ResWriteNameOrOrdinal( msheader->Type, true, fp ) )
            return( true );
        if( ResWriteNameOrOrdinal( msheader->Name, true, fp ) )
            return( true );
        if( ResWritePadDWord( fp ) )
            return( true );
        if( ResWriteUint32( fp, msheader->DataVersion ) )
            return( true );
        if( ResWriteUint16( fp, msheader->MemoryFlags ) )
            return( true );
        if( ResWriteUint16( fp, msheader->LanguageId ) )
            return( true );
        if( ResWriteUint32( fp, msheader->Version ) )
            return( true );
        return( ResWriteUint32( fp, msheader->Characteristics ) );
    } else {
        if( ResWriteNameOrOrdinal( msheader->Type, false, fp ) )
            return( true );
        if( ResWriteNameOrOrdinal( msheader->Name, false, fp ) )
            return( true );
        if( ResWriteUint16( fp, msheader->MemoryFlags ) )
            return( true );
        return( ResWriteUint32( fp, msheader->Size ) );
    }
}

void WriteInitStatics( void )
/***************************/
{
    memset( ConvBuffer, 0, CONV_BUF_SIZE * sizeof( char ) );
}

ConvToUnicode_fn    *ConvToUnicode = DefaultUNIConversion;
ConvToMultiByte_fn  *ConvToMultiByte = DefaultMBConversion;
