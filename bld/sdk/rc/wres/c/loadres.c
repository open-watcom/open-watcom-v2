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


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <malloc.h>
#ifdef UNIX
    #include <alloca.h>
    #ifdef _AIX
        #define alloca __alloca
    #endif
#endif


/*
   This next set of lines is a temp fix until the 11.0 headers are
   in universal usage.
   The net result is that for 16bit Intel platforms _WCI86FAR will be __far
   for all other compilation targets it will be nothing.
*/
#ifndef _WCI86FAR
    #include <errno.h>
    #ifndef _WCI86FAR
        #ifdef M_I86
            #define _WCI86FAR __far
        #else
            #define _WCI86FAR
        #endif
    #endif
#endif

#ifdef WIN_GUI
#include <windows.h>
#else
#define WINAPI
typedef unsigned int UINT;
typedef char _WCI86FAR *   LPSTR;
#endif

#include <unistd.h>
#include "wresall.h"
#include "loadstr.h"

#include "phandle.h"

extern WResDir    MainDir;

static int GetResource( WResLangInfo    *res,
                        PHANDLE_INFO    hInstance,
                        LPSTR           lpszBuffer )
/**************************************************/
{
    off_t               prevpos;
    unsigned            numread;

    prevpos = WRESSEEK( hInstance->handle, res->Offset, SEEK_SET );
    if ( prevpos == -1L ) return( -1 );
    numread = WRESREAD( hInstance->handle, (void *)lpszBuffer, (int)res->Length );

    return( 0 );
}

extern int WINAPI WResLoadResource2( WResDir            dir,
                                     PHANDLE_INFO       hInstance,
                                     UINT               idType,
                                     UINT               idResource,
                                     LPSTR              *lpszBuffer,
                                     int                *bufferSize )
/******************************************************************/
{
    int                 retcode;
    WResID              resource_type;
    WResID              resource_id;
    WResDirWindow       wind;
    WResLangInfo        *res;
    WResLangType        lang;

    if( ( lpszBuffer == NULL ) || ( bufferSize == NULL ) ) {
        return( -1 );
    }

    lang.lang = DEF_LANG;
    lang.sublang = DEF_SUBLANG;
    WResInitIDFromNum( idResource, &resource_id );
    WResInitIDFromNum( (long)idType, &resource_type );

    wind = WResFindResource( &resource_type, &resource_id, dir, &lang );

    if( WResIsEmptyWindow( wind ) ) {
        retcode = -1;
    } else {
        res = WResGetLangInfo( wind );
        // lets make sure we dont perturb malloc into apoplectic fits
        if( res->Length >= INT_MAX ) {
            return( -1 );
        }
        *lpszBuffer = (LPSTR)WRESALLOC( res->Length );
        if( *lpszBuffer == NULL ) {
            return( -1 );
        }
        *bufferSize = (int)res->Length;
        retcode = GetResource( res, hInstance, *lpszBuffer );
    }

    return( retcode );
}

extern int WINAPI WResLoadResource( PHANDLE_INFO       hInstance,
                                    UINT               idType,
                                    UINT               idResource,
                                    LPSTR              *lpszBuffer,
                                    int                *bufferSize )
/******************************************************************/
{
    return( WResLoadResource2( MainDir, hInstance, idType, idResource, lpszBuffer, bufferSize ) );
}

