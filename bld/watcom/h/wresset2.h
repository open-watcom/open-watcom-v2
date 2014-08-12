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
* Description:  Public interface to wres library.
*
****************************************************************************/


#ifndef WRESSET2_INCLUDED
#define WRESSET2_INCLUDED

#ifdef WIN_GUI
# include <windows.h>
#elif defined( __WATCOMC__ )
# ifndef _COMDEF_H_INCLUDED
#  include <_comdef.h>
# endif
#else
# define _WCI86FAR
#endif

typedef struct handle_info {
    WResFileID  handle;
#ifdef WIN_GUI
    HINSTANCE   inst;
#endif
} HANDLE_INFO, *PHANDLE_INFO;

#ifndef WIN_GUI
#define LoadString      WResLoadString
typedef PHANDLE_INFO    HINSTANCE;
typedef unsigned int    UINT;
typedef char            _WCI86FAR *LPSTR;
#endif

#if defined( __cplusplus )
extern "C" {
#endif

extern bool OpenResFile( PHANDLE_INFO hInstance, const char *filename );
extern bool CloseResFile( PHANDLE_INFO hInstance );
extern bool FindResources( PHANDLE_INFO hInstance );
extern bool InitResources( PHANDLE_INFO hInstance );
extern int WResLoadString( PHANDLE_INFO hInstance, UINT idResource, LPSTR lpszBuffer, int nBufferMax );
extern int WResLoadResource( PHANDLE_INFO hInstance, UINT idType, UINT idResource, LPSTR *lpszBuffer, int *bufferSize );

#if defined( __cplusplus )
}
#endif

#endif
