/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2014-2014 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  ...PROCx type definition for WIN386 callback far functions
*
****************************************************************************/


#if defined( __WINDOWS_386__ )
typedef BOOL            (CALLBACK *DLGPROCx)(HWND,UINT,WPARAM,LPARAM);
typedef void            (CALLBACK *FARPROCx)(void);
typedef int             (CALLBACK *FONTENUMPROCx)(const LOGFONT *,const TEXTMETRIC *,int,LPARAM);
typedef LRESULT         (CALLBACK *HOOKPROCx)(int,WPARAM,LPARAM);
typedef BOOL            (CALLBACK *WNDENUMPROCx)(HWND,LPARAM);
typedef UINT            (CALLBACK *__CDHOOKPROCx)(HWND,UINT,WPARAM,LPARAM);
typedef __CDHOOKPROCx   LPOFNHOOKPROCx;
typedef LRESULT         (CALLBACK *WNDPROCx)(HWND,UINT,WPARAM,LPARAM);
#else
#define DLGPROCx        DLGPROC
#define FARPROCx        FARPROC
#define FONTENUMPROCx   FONTENUMPROC
#define HOOKPROCx       HOOKPROC
#define LPOFNHOOKPROCx  LPOFNHOOKPROC
#define WNDENUMPROCx    WNDENUMPROC
#define WNDPROCx        WNDPROC
#endif
