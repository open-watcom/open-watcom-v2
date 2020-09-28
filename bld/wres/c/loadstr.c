/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Load string resources.
*
****************************************************************************/


#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "wresall.h"
#include "wresset2.h"
#include "wresdefn.h"
#include "loadstr.h"
#include "wresrtns.h"
#include "layer2.h"


#define READ_STRING_BUFLEN  128

WResDir    MainDir;

static int GetString( WResLangInfo *res, PHANDLE_INFO hinfo, unsigned int idResource,
                                                    lpstr lpszBuffer, int nBufferMax )
/************************************************************************************/
{
    size_t              length;
    int                 stringnum;
    unsigned            stringlen;
    size_t              numread;
    size_t              ix1;
    int                 ix2;
    char                stringbuff[READ_STRING_BUFLEN];

    if( WRESSEEK( hinfo->fp, res->Offset, SEEK_SET ) )
        return( -1 );
    length = res->Length;
    stringnum = idResource & 0x0f;

    numread = 0;
    stringlen = 0;
    ix2 = 0;
    ix1 = 0;
    do {
        if( numread == 0 ) {
            if( length > READ_STRING_BUFLEN ) {
                numread = READ_STRING_BUFLEN;
                length -= READ_STRING_BUFLEN;
            } else {
                numread = length;
                length = 0;
            }
            numread = WRESREAD( hinfo->fp, stringbuff, numread );
            if( WRESIOERR( hinfo->fp, numread ) )
                return( -1 );
            if( numread == 0 )
                return( -1 );
            ix1 = 0;
        }
        if( ix1 < numread ) {
            if( stringlen == 0 ) {
                stringlen = ((unsigned char *)stringbuff)[ix1++];
                stringnum--;
            }
            for( ; stringlen > 0; --stringlen ) {
                if( ix1 >= numread )
                    break;
                if( stringnum < 0 ) {
                    if( ix2 < nBufferMax - 1 ) {
                        lpszBuffer[ix2++] = stringbuff[ix1];
                    }
                }
                ix1++;
            }
        } else {
            numread = 0;
        }
    } while( !( (stringlen == 0) && (stringnum < 0) ) );
    lpszBuffer[ix2] = '\0';

    return( ix2 );
}

int WResLoadString2( WResDir dir, PHANDLE_INFO hinfo, unsigned int idResource,
                                            lpstr lpszBuffer, int nBufferMax )
/****************************************************************************/
{
    int                 retcode;
    unsigned            block_num;
    WResID              string_type;
    WResID              block_id;
    WResDirWindow       wind;
    WResLangInfo        *res;
    WResLangType        lang;

    block_num = (idResource >> 4) + 1;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    WResInitIDFromNum( block_num, &block_id );
    WResInitIDFromNum( RESOURCE2INT( RT_STRING ), &string_type );

    if( dir == NULL ) {
        InitResources2( &dir, hinfo );
    }
    wind = WResFindResource( &string_type, &block_id, dir, &lang );

    if( WResIsEmptyWindow( wind ) ) {
        retcode = -1;
    } else {
        res = WResGetLangInfo( wind );
        retcode = GetString( res, hinfo, idResource, lpszBuffer, nBufferMax );
    }
    return( retcode );
}

int WResLoadString( PHANDLE_INFO hinfo, unsigned int idResource, lpstr lpszBuffer, int nBufferMax )
/*************************************************************************************************/
{
    return( WResLoadString2( MainDir, hinfo, idResource, lpszBuffer, nBufferMax ) );
}

bool InitResources2( WResDir *dir, PHANDLE_INFO hinfo )
/*****************************************************/
/* return true if error */
{
    *dir = WResInitDir();
    if( *dir == NULL )
        return( true );
    return( WResReadDir( hinfo->fp, *dir, NULL ) );
}

bool InitResources( PHANDLE_INFO hinfo )
/**************************************/
/* return true if error */
{
    return( InitResources2( &MainDir, hinfo ) );
}

bool FiniResources2( WResDir dir, PHANDLE_INFO hinfo )
/****************************************************/
/* return true if error */
{
    /* unused parameters */ (void)hinfo;

    WResFreeDir( dir );
    return( false );
}

bool FiniResources( PHANDLE_INFO hinfo )
/**************************************/
/* return true if error */
{
    return( FiniResources2( MainDir, hinfo ) );
}

void LoadstrInitStatics( void )
/*****************************/
{
    MainDir = NULL;
}
