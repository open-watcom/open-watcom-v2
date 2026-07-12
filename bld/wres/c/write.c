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

bool ResWriteUint8( uint_8 value, FILE *fp )
/*******************************************/
{
    if( WRESWRITE( fp, &value, sizeof( value ) ) != sizeof( value ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}

bool ResWriteUint16( uint_16 value, FILE *fp )
/*********************************************/
{
    CONV_LE_16( value );
    if( WRESWRITE( fp, &value, sizeof( value ) ) != sizeof( value ) )
        return( WRES_ERROR( WRS_WRITE_FAILED ) );
    return( false );
}

bool ResWriteUint32( uint_32 value, FILE *fp )
/*********************************************/
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
        return( ResWriteStringLen( name_id->Name, use_unicode, fp, name_id->NumChars, true ) );
    } else if( use_unicode ) {
        return( ResWriteUint16( 0, fp ) );
    } else {
        return( ResWriteUint8( 0, fp ) );
    }
} /* WResWriteWResIDNameString */

bool WResWriteWResIDName( const WResIDName *name_id, FILE *fp )
/*************************************************************/
{
    return( WResWriteWResIDNameString( name_id, false, fp ) );
}

bool WResWriteWResID( const WResID *id, FILE *fp )
/************************************************/
{
    if( ResWriteUint8( id->IsName, fp ) )
        return( true );
    if( id->IsName ) {
        return( WResWriteWResIDName( &(id->ID.Name), fp ) );
    } else {
        return( ResWriteUint16( id->ID.Num, fp ) );
    }
}

/*
 * WResWriteTypeRecord - write the type record to the current postion
 *                       in the file identified by fp
 */
bool WResWriteTypeRecord( const WResTypeInfo *type, FILE *fp )
/************************************************************/
{
    if( ResWriteUint16( type->NumResources, fp ) )
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
    if( ResWriteUint16( res->NumResources, fp ) )
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
    if( ResWriteUint16( info->lang.lang, fp ) )
        return( true );
    if( ResWriteUint8( info->lang.sublang, fp ) )
        return( true );
    if( ResWriteUint16( info->MemoryFlags, fp ) )
        return( true );
    if( ResWriteUint32( info->Offset, fp ) )
        return( true );
    return( ResWriteUint32( info->Length, fp ) );
}

bool WResWriteHeader( const WResHeader *header, FILE *fp )
/********************************************************/
{
    if( ResWriteUint32( header->Magic[0], fp ) )
        return( true );
    if( ResWriteUint32( header->Magic[1], fp ) )
        return( true );
    if( ResWriteUint32( header->DirOffset, fp ) )
        return( true );
    if( ResWriteUint16( header->NumResources, fp ) )
        return( true );
    if( ResWriteUint16( header->NumTypes, fp ) )
        return( true );
    return( ResWriteUint16( header->WResVer, fp ) );
}

bool WResWriteExtHeader( const WResExtHeader *extheader, FILE *fp )
/*****************************************************************/
{
    if( ResWriteUint16( extheader->TargetOS, fp ) )
        return( true );
    if( ResWriteUint16( extheader->reserved[0], fp ) )
        return( true );
    if( ResWriteUint16( extheader->reserved[1], fp ) )
        return( true );
    if( ResWriteUint16( extheader->reserved[2], fp ) )
        return( true );
    return( ResWriteUint16( extheader->reserved[3], fp ) );
}

bool ResWriteStringLen( const char *str, bool use_unicode, FILE *fp, size_t numchars, bool with_len )
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
            error = ResWriteUint16( numchars, fp );
        } else {
            numchars = size;
            if( numchars > 255 )
                numchars = 255;
            size = numchars;
            error = ResWriteUint8( numchars, fp );
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
    return( ResWriteStringLen( str, use_unicode, fp, len, false ) );
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
                error = ResWriteUint16( 0xffff, fp );
            } else {
                error = ResWriteUint8( 0xff, fp );
            }
            if( !error ) {
                error = ResWriteUint16( name->ord.wOrdinalID, fp );
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

    headersize = 2 * sizeof( uint_16 ) + 5 * sizeof( uint_32 );
    namesize = MResFindNameOrOrdSize( msheader->Name, iswin32 );
    typesize = MResFindNameOrOrdSize( msheader->Type, iswin32 );
    headersize += ( namesize + typesize );
    padding = RES_PADDING_DWORD( typesize + namesize );

    return( headersize + padding );
}

bool MResWriteResourceHeader( MResResourceHeader *msheader, FILE *fp, bool iswin32 )
/**********************************************************************************/
{
    if( iswin32 ) {
        if( ResWriteUint32( msheader->Size, fp ) )
            return( true );
        if( ResWriteUint32( MResFindHeaderSize( msheader, iswin32 ), fp ) )
            return( true );
        if( ResWriteNameOrOrdinal( msheader->Type, true, fp ) )
            return( true );
        if( ResWriteNameOrOrdinal( msheader->Name, true, fp ) )
            return( true );
        if( ResWritePadDWord( fp ) )
            return( true );
        if( ResWriteUint32( msheader->DataVersion, fp ) )
            return( true );
        if( ResWriteUint16( msheader->MemoryFlags, fp ) )
            return( true );
        if( ResWriteUint16( msheader->LanguageId, fp ) )
            return( true );
        if( ResWriteUint32( msheader->Version, fp ) )
            return( true );
        return( ResWriteUint32( msheader->Characteristics, fp ) );
    } else {
        if( ResWriteNameOrOrdinal( msheader->Type, false, fp ) )
            return( true );
        if( ResWriteNameOrOrdinal( msheader->Name, false, fp ) )
            return( true );
        if( ResWriteUint16( msheader->MemoryFlags, fp ) )
            return( true );
        return( ResWriteUint32( msheader->Size, fp ) );
    }
}

void WriteInitStatics( void )
/***************************/
{
    memset( ConvBuffer, 0, CONV_BUF_SIZE * sizeof( char ) );
}

ConvToUnicode_fn    *ConvToUnicode = DefaultUNIConversion;
ConvToMultiByte_fn  *ConvToMultiByte = DefaultMBConversion;
