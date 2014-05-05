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
* Description:  System specific support routines
*
****************************************************************************/


// on WIN64 long is OK because HANDLE can be hold as 32-bit sign extended value
// even if HANDLE is defined as 64-bit value

#if defined( __WINDOWS__ )
#define NULL_SYSHDL NULL
typedef void (DIGENTRY *mad_sys_handle)( void );
#elif defined( __NT__ )
#define NULL_SYSHDL 0
typedef size_t      mad_sys_handle;
#elif defined( __OS2__ )
#define NULL_SYSHDL 0
#if defined( _M_I86 )
typedef unsigned short mad_sys_handle;
#else
typedef unsigned long mad_sys_handle;
#endif
#elif defined( __RDOS__ )
#define NULL_SYSHDL 0
typedef int         mad_sys_handle;
#else
#define NULL_SYSHDL NULL
typedef void        *mad_sys_handle;
#endif

extern mad_status   MADSysLoad( char *, mad_client_routines *, mad_imp_routines **, mad_sys_handle * );
extern void         MADSysUnload( mad_sys_handle * );
