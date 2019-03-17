/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2015 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  LibMain and _LibMain function declarations.
*
****************************************************************************/


#if defined( __OS2__ )

extern unsigned _LibMain( unsigned hmod, unsigned termination );
#pragma aux _LibMain "_*" __parm __caller []
extern unsigned APIENTRY LibMain( unsigned hmod, unsigned termination );

#elif defined( __NT__ )

extern int APIENTRY _LibMain( HANDLE hdll, DWORD reason, LPVOID reserved );
#pragma aux _LibMain "_*"
extern int APIENTRY LibMain( HANDLE hdll, DWORD reason, LPVOID reserved );

#if defined( _M_ALPHA ) || defined( _M_PPC )
extern int DllMainCRTStartup( HANDLE hdll, DWORD reason, LPVOID reserved );
extern int _DllMainCRTStartup( HANDLE hdll, DWORD reason, LPVOID reserved );
extern int wDllMainCRTStartup( HANDLE hdll, DWORD reason, LPVOID reserved );
#endif

#elif defined( __RDOS__ ) || defined( __RDOSDEV__ )

extern int _LibMain( int hdll, int reason, void *reserved );
#pragma aux _LibMain "_*" __parm [__ebx] [__edx] [__eax] __value [__eax]
extern int __stdcall LibMain( int hdll, int reason, void *reserved );

#elif defined( __WINDOWS__ )

    extern int __far __pascal WEP( int );
#if defined( _M_I86 )
    extern int __far __pascal _WEP( int );
    extern void __clib_WEP( void );
#endif

#elif defined( __DOS__ )

extern unsigned _LibMain( int termination );
/* So that assembly caller doesn't depend on stack/register convention. */
#pragma aux _LibMain "_*" __parm __caller []

#endif
