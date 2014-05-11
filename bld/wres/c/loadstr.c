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
* Description:  Load string resources.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <stdlib.h>
#include "wresall.h"
#include "wresset2.h"
#include "wresdefn.h"
#include "loadstr.h"
#include "wresrtns.h"

#define GET_STR_BUF_LEN 128

WResDir    MainDir;

extern int InitResources2( WResDir *dir, PHANDLE_INFO hInstance );

static int GetString(   WResLangInfo    *res,
                        PHANDLE_INFO    hInstance,
                        UINT            idResource,
                        LPSTR           lpszBuffer,
                        int             nBufferMax )
/*************************************************/
{
    long                prevpos;
    int                 length;
    int                 stringnum;
    int                 stringlen;
    int                 numread;
    int                 ix1, ix2;
    char                stringbuff[GET_STR_BUF_LEN];

    prevpos = WRESSEEK( WRESHANDLE, res->Offset, SEEK_SET );
    if ( prevpos == -1L )
        return( -1 );
    length = res->Length;
    stringnum = idResource & 0x0f;

    numread = 0;
    stringlen = 0;
    ix2 = 0;
    ix1 = 0;
    do {
        if ( numread == 0 ) {
            if ( length > GET_STR_BUF_LEN ) {
                numread = GET_STR_BUF_LEN;
                length -= GET_STR_BUF_LEN;
            } else {
                numread = length;
                length = 0;
            }
            numread = WRESREAD( WRESHANDLE, stringbuff, numread );
            if( numread == 0 ) return( -1 );    // 15-sep-93 AFS
            if( numread == -1 ) return( -1 );
            ix1 = 0;
        }
        if ( ix1 < numread ) {
            if ( stringlen == 0 ) {
                stringlen = ((unsigned char *)stringbuff)[ ix1++ ];
                stringnum--;
            }
            for( ; stringlen > 0; --stringlen ) {
                if ( ix1 >= numread ) break;
                if ( stringnum < 0 ) {
                    if( ix2 < nBufferMax - 1 ) {
                        lpszBuffer[ ix2++ ] = stringbuff[ ix1 ];
                    }
                }
                ix1++;
            }
        } else {
            numread = 0;
        }
    } while( !( (stringlen == 0) && (stringnum < 0) ) );
    lpszBuffer[ ix2 ] = '\0';

    return( 0 );
}

extern int WResLoadString2(  WResDir             dir,
                             PHANDLE_INFO        hInstance,
                             UINT                idResource,
                             LPSTR               lpszBuffer,
                             int                 nBufferMax )
/******************************************************************/
{
    int                 retcode;
    int                 block_num;
    WResID              string_type;
    WResID              block_id;
    WResDirWindow       wind;
    WResLangInfo        *res;
    WResLangType        lang;


    block_num = (idResource >> 4) + 1;
    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    WResInitIDFromNum( block_num, &block_id );
    WResInitIDFromNum( (long)RT_STRING, &string_type );

    if( dir == NULL ) {
        InitResources2( &dir, hInstance );
    }
    wind = WResFindResource( &string_type, &block_id, dir, &lang );

    if ( WResIsEmptyWindow( wind ) ) {
        retcode = -1;
    } else {
        res = WResGetLangInfo( wind );
        retcode = GetString( res, hInstance, idResource, lpszBuffer, nBufferMax );
    }
    return( retcode );
}

extern int WResLoadString(   PHANDLE_INFO        hInstance,
                             UINT                idResource,
                             LPSTR               lpszBuffer,
                             int                 nBufferMax )
/******************************************************************/
{
    return( WResLoadString2( MainDir, hInstance, idResource, lpszBuffer, nBufferMax ) );
}

extern int OpenResFile( PHANDLE_INFO hInstance, const char *filename )
/********************************************************************/
{
    WRESHANDLE = ResOpenFileRO( filename );
    return( WRESHANDLE == NIL_HANDLE );
}

extern int InitResources2( WResDir *dir, PHANDLE_INFO hInstance )
/***************************************************************/
{
    *dir = WResInitDir();
    if( *dir == NULL ) return( -1 );
    return( WResReadDir( WRESHANDLE, *dir, NULL ) );
}

extern int InitResources( PHANDLE_INFO hInstance )
/************************************************/
{
    return( InitResources2( &MainDir, hInstance ) );
}

extern int CloseResFile2( WResDir dir, PHANDLE_INFO hInstance )
/**************************************************************/
{
    WResFreeDir( dir );
    return( ResCloseFile( WRESHANDLE ) );
}

extern int CloseResFile( PHANDLE_INFO hInstance )
/********************************************/
{
    return( CloseResFile2( MainDir, hInstance ) );
}

extern void LoadstrInitStatics( void )
/************************************/
{
    MainDir = NULL;
}
