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

static int DefaultConversion( int len, const char *str, char *buf )
{
    int         i;

    if( buf != NULL ) {
        for( i=0; i < len; i++ ) {
            buf[2 * i] = str[i];
            buf[2 * i + 1] = 0;
        }
    }
    return( len * 2 );
}

int (*ConvToUnicode)( int, const char *, char *) = DefaultConversion;

bool ResWriteUint8( const uint_8 *newint, WResFileID handle )
/***********************************************************/
{
    if( WRESWRITE( handle, newint, sizeof( uint_8 ) ) != sizeof( uint_8 ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
}

bool ResWriteUint16( const uint_16 *newint, WResFileID handle )
/*************************************************************/
{
    if( WRESWRITE( handle, newint, sizeof( uint_16 ) ) != sizeof( uint_16 ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
}

bool ResWriteUint32( const uint_32 *newint, WResFileID handle )
/*************************************************************/
{
    if( WRESWRITE( handle, newint, sizeof( uint_32 ) ) != sizeof( uint_32 ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
}

bool WResWriteWResIDNameUni( const WResIDName *name, bool use_unicode, WResFileID handle )
/******************************************************************************************/
{
    bool            error;
    uint_16         numchars;
    uint_8          tmp;
    char            *ptr;
    bool            freebuf;

    freebuf = false;
    error = false;
    if( name == NULL ) {
        /* a NULL name means write 0 length name */
        numchars = 0;
    } else {
        numchars = name->NumChars;
    }
    if( use_unicode ) {
        // for short strings use a static buffer in improve performance
        if( numchars <= CONV_BUF_SIZE / 2 ) {
            ptr = ConvBuffer;
        } else {
            freebuf = true;
            ptr = WRESALLOC( 2 * numchars );
        }
        numchars = (ConvToUnicode)( numchars, name->Name, ptr ) / 2;
        error = ResWriteUint16( &numchars, handle );
        numchars *= 2;
    } else {
        numchars &= 0xFF;       /* in 16-bit the string can be no more than
                                   256 characters*/
        tmp = numchars;
        ptr = (char *)name->Name;
        error = ResWriteUint8( &tmp, handle );
    }
    if( !error && numchars > 0 ) {
        if( WRESWRITE( handle, ptr, numchars ) != numchars ) {
            error = true;
            WRES_ERROR( WRS_WRITE_FAILED );
        }
    }
    if( freebuf ) {
        WRESFREE( ptr );
    }
    return( error );
} /* WResWriteWResIDNameUni */

bool WResWriteWResIDName( const WResIDName *name, WResFileID handle )
/*******************************************************************/
{
    return( WResWriteWResIDNameUni( name, false, handle ) );
}

bool WResWriteWResID( const WResID *name, WResFileID handle )
/***********************************************************/
{
    bool        error;
    uint_16     tmp16;
    uint_8      tmp8;

    tmp8 = name->IsName;
    error = ResWriteUint8( &tmp8, handle );
    if( !error ) {
        if( name->IsName ) {
            error = WResWriteWResIDName( &(name->ID.Name), handle );
        } else {
            tmp16 = name->ID.Num;
            error = ResWriteUint16( &tmp16, handle );
        }
    }

    return( error );
} /* WResWriteWResID */

/*
 * WResWriteTypeRecord - write the type record to the current postion
 *                       in the file identified by fp
 */
bool WResWriteTypeRecord( const WResTypeInfo *type, WResFileID handle )
{
    int             size;

    if( type->TypeName.IsName ) {
        /* -1 because one of the chars in the name is declared in the struct */
        size = sizeof( WResTypeInfo ) + type->TypeName.ID.Name.NumChars - 1;
    } else {
        size = sizeof( WResTypeInfo );
    }
    if( WRESWRITE( handle, type, size ) != size ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
} /* WResWriteTypeRecord */

/*
 * WResWriteResRecord - write the resource record to the current position
 *                      in the file identified by  fp
 */
bool WResWriteResRecord( const WResResInfo *res, WResFileID handle )
{
    int             size;

    if( res->ResName.IsName ) {
        /* -1 because one of the chars in the name is declared in the struct */
        size = sizeof( WResResInfo ) + res->ResName.ID.Name.NumChars - 1;
    } else {
        size = sizeof( WResResInfo );
    }
    if( WRESWRITE( handle, (uint_8 *)res, size ) != size ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
} /* WResWriteResRecord */

/*
 * WResWriteLangRecord - write out a language record at the current file
 *                       position
 */
bool WResWriteLangRecord( const WResLangInfo *info, WResFileID handle )
{
    if( WRESWRITE( handle, info, sizeof( WResLangInfo ) ) != sizeof( WResLangInfo ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
}

bool WResWriteHeaderRecord( const WResHeader *header, WResFileID handle )
/**********************************************************************/
{
    bool            error;

    error = ( WRESSEEK( handle, 0L, SEEK_SET ) == -1 );
    if( error ) {
        WRES_ERROR( WRS_SEEK_FAILED );
    }

    if( !error ) {
        if( WRESWRITE( handle, header, sizeof( WResHeader ) ) != sizeof( WResHeader ) ) {
            error = true;
            WRES_ERROR( WRS_WRITE_FAILED );
        }
    }

    return( error );
} /* WResWriteHeaderRecord */

bool WResWriteExtHeader( const WResExtHeader *ext_head, WResFileID handle )
/*************************************************************************/
{
    if( WRESWRITE( handle, ext_head, sizeof( WResExtHeader ) ) != sizeof( WResExtHeader ) ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        return( true );
    } else {
        return( false );
    }
}

bool ResWriteStringLen( const char *string, bool use_unicode, WResFileID handle, uint_16 len )
/********************************************************************************************/
{
    char            *buf = NULL;
    bool            ret;

    if( use_unicode ) {
        if( len * 2 > CONV_BUF_SIZE ) {
            buf = WRESALLOC( 2 * len );
        } else {
            buf = ConvBuffer;
        }
        len = (ConvToUnicode)( len, string, buf );
        string = buf;
    }
    if( WRESWRITE( handle, string, len ) != len ) {
        WRES_ERROR( WRS_WRITE_FAILED );
        ret = true;
    } else {
        ret = false;
    }
    if( use_unicode ) {
        if( buf != ConvBuffer ) {
            WRESFREE( buf );
        }
    }
    return( ret );
}

bool ResWriteString( const char *string, bool use_unicode, WResFileID handle )
/****************************************************************************/
{
    size_t  stringlen;
    bool    ret;

    /* if string is NULL output the null string */
    if( string == NULL ) {
        string = "";
    }

    /* the +1 is so we will output the '\0' as well */
    stringlen = strlen( string ) + 1;
    ret = ResWriteStringLen( string, use_unicode, handle, stringlen );
    return( ret );
}

bool ResWriteNameOrOrdinal( ResNameOrOrdinal *name, bool use_unicode, WResFileID handle )
/*****************************************************************************************/
{
    bool        error;
    uint_16     flag;
    uint_16     tmp16;
    uint_8      tmp8;

    if( name == NULL ) {
        error = ResWriteString( "", use_unicode, handle );
    } else {
        if( name->ord.fFlag == 0xff ) {
            if( use_unicode ) {
                flag = 0xffff;
                error = ResWriteUint16( &flag, handle );
            } else {
                tmp8 = name->ord.fFlag;
                error = ResWriteUint8( &tmp8, handle );
            }
            if( !error ) {
                tmp16 = name->ord.wOrdinalID;
                error = ResWriteUint16( &tmp16, handle );
            }
        } else {
            error = ResWriteString( name->name, use_unicode, handle );
        }
    }

    return( error );
} /* ResWriteNameOrOrdinal */

static long MResFindNameOrOrdSize( ResNameOrOrdinal *data, bool use_unicode )
/***************************************************************************/
{
    long       size;

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

static int MResFindHeaderSize( MResResourceHeader *header, bool use_unicode )
/***************************************************************************/
{
    int     headersize;
    long    namesize;
    long    typesize;
    long    padding;

    headersize = sizeof( MResResourceHeader ) - 2 * sizeof( ResNameOrOrdinal * ) + sizeof( uint_32 );
    namesize = MResFindNameOrOrdSize( header->Name, use_unicode );
    typesize = MResFindNameOrOrdSize( header->Type, use_unicode );
    headersize += ( namesize + typesize );
    padding = RES_PADDING( typesize + namesize, sizeof( uint_32 ) );

    return( headersize + padding );
}

bool MResWriteResourceHeader( MResResourceHeader *currhead, WResFileID handle, bool iswin32 )
/*******************************************************************************************/
{
    bool        error;
    uint_32     headersize;
    uint_16     tmp16;
    uint_32     tmp32;

    if( !iswin32 ) {
        error = ResWriteNameOrOrdinal( currhead->Type, false, handle );
        if( !error ) {
            error = ResWriteNameOrOrdinal( currhead->Name, false, handle );
        }
        if( !error ) {
            tmp16 = currhead->MemoryFlags;
            error = ResWriteUint16( &tmp16, handle );
        }
        if( !error ) {
            tmp32 = currhead->Size;
            error = ResWriteUint32( &tmp32, handle );
        }
    } else {
        tmp32 = currhead->Size;
        error = ResWriteUint32( &tmp32, handle );
        if( !error ) {
            headersize = MResFindHeaderSize( currhead, true );
            error = ResWriteUint32( &headersize, handle  );
        }
        if( !error ) {
            error = ResWriteNameOrOrdinal( currhead->Type, true, handle );
        }
        if( !error ) {
            error = ResWriteNameOrOrdinal( currhead->Name, true, handle );
        }
        if( !error ) {
            error = ResPadDWord( handle );
        }
        if( !error ) {
            tmp32 = currhead->DataVersion;
            error = ResWriteUint32( &tmp32, handle );
        }
        if( !error ) {
            tmp16 = currhead->MemoryFlags;
            error = ResWriteUint16( &tmp16, handle );
        }
        if( !error ) {
            tmp16 = currhead->LanguageId;
            error = ResWriteUint16( &tmp16, handle );
        }
        if( !error ) {
            tmp32 = currhead->Version;
            error = ResWriteUint32( &tmp32, handle );
        }
        if( !error ) {
            tmp32 = currhead->Characteristics;
            error = ResWriteUint32( &tmp32, handle );
        }
    }

    return( error );
} /* MResWriteResourceHeader */

void WriteInitStatics( void )
/***************************/
{
    memset( ConvBuffer, 0, CONV_BUF_SIZE * sizeof( char ) );
}
