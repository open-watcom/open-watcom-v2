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


#ifndef WINHDR_INCLUDED
#define WINHDR_INCLUDED

#if defined( __NT__ )
    #define _WINSOCKAPI_
#endif

#ifdef __OS2__
    #define INCL_BASE
    #define INCL_PM
    #include <os2.h>
    #include "os2win.h"
#else
    #define HAS_HWND
    #define INCLUDE_COMMDLG_H
    #define INCLUDE_DDEML_H
    #define INCLUDE_SHELLAPI_H
    #define INCLUDE_TOOLHELP_H
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>
    #include <commdlg.h>
    #include <ddeml.h>
    #include <shellapi.h>
    #include "wi163264.h"
#endif

#if 0
    // see display.c, ..\c\dc.c
    #define BITBLT_BUFFER_DISPLAY
#endif

#endif
