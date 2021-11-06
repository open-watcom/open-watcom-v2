/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2021 The Open Watcom Contributors. All Rights Reserved.
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


#define GETSUPPID(x)    GetSuppId( QUOTED( x ) )

extern void InitTrap( const char *parms );
//extern bool ReInitTrap( const char *parms );
extern void FiniTrap( void );
extern void InitSuppServices( void );
extern void FiniSuppServices( void );

extern bool InitCapabilities( void );
extern bool InitFileInfoSupp( void );

extern trap_shandle GetSuppId( char *name );

extern void RemoteSuspend( void );
extern void RemoteResume( void );

extern bool SetCapabilitiesExactBreakpointSupport( bool status, bool set_switch );
extern bool IsExactBreakpointsSupported( void );
extern bool Is8ByteBreakpointsSupported( void );

