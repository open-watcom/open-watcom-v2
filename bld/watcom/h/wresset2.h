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


#ifndef WRESSET2_INCLUDED
#define WRESSET2_INCLUDED
#ifdef WIN_GUI
#include "windows.h"
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

typedef struct handle_info {
        int     handle;
        char    *filename;
#ifdef WIN_GUI
    HINSTANCE   inst;
#endif
} HANDLE_INFO;
typedef struct handle_info * PHANDLE_INFO;

#ifndef WIN_GUI
#define LoadString2( Dir, hInstance, idResource, lpszBuffer, nBufferMax ) \
            WResLoadString( Dir, hInstance, idResource, lpszBuffer, nBufferMax )
#define LoadString( hInstance, idResource, lpszBuffer, nBufferMax ) \
            WResLoadString( hInstance, idResource, lpszBuffer, nBufferMax )
#ifndef WINAPI
#define WINAPI
#endif
typedef unsigned int UINT;
typedef char _WCI86FAR * LPSTR;
typedef PHANDLE_INFO HINSTANCE;
#endif

#if defined( __cplusplus )
extern "C" {
#endif

struct WResDirHead;
int OpenResFile( PHANDLE_INFO hInstance );
int FindResources( PHANDLE_INFO hInstance );
int InitResources( PHANDLE_INFO hInstance );
int InitResources2( struct WResDirHead **, PHANDLE_INFO hInstance );
int WINAPI WResLoadString( PHANDLE_INFO hInstance,
                           UINT idResource,
                           LPSTR lpszBuffer,
                           int nBufferMax );
int WINAPI WResLoadString2( struct WResDirHead *,
                            PHANDLE_INFO hInstance,
                            UINT idResource,
                            LPSTR lpszBuffer,
                            int nBufferMax );
int WINAPI WResLoadResource( PHANDLE_INFO       hInstance,
                             UINT               idType,
                             UINT               idResource,
                             LPSTR              *lpszBuffer,
                             int                *bufferSize );
int WINAPI WResLoadResource2( struct WResDirHead *,
                              PHANDLE_INFO      hInstance,
                              UINT              idType,
                              UINT              idResource,
                              LPSTR             *lpszBuffer,
                              int               *bufferSize );
int CloseResFile( PHANDLE_INFO hInstance );
int CloseResFile2( struct WResDirHead *, PHANDLE_INFO hInstance );

#if defined( __cplusplus )
}
#endif

#endif
