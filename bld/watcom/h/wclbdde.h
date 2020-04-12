/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2014-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WIN16/WIN386 MakeProcInstance.../FreeProcInstance...
*               for DDE callback function prototypes
*
****************************************************************************/


#include <ddeml.h>
#include "wclbproc.h"


#ifdef __cplusplus
extern "C" {
#endif

#if defined( __WINDOWS_386__ )
typedef HDDEDATA            (CALLBACK *PFNCALLBACKx)( UINT, UINT, HCONV, HSZ, HSZ, HDDEDATA, DWORD, DWORD );
#else
#define PFNCALLBACKx        PFNCALLBACK
#endif

#if defined( __WINDOWS__ )
extern PFNCALLBACK          MakeProcInstance_DDE( PFNCALLBACKx fn, HINSTANCE instance );
#pragma aux MakeProcInstance_DDE = MAKEPROCINSTANCE_INLINE
#else
#define MakeProcInstance_DDE(f,i)   ((void)i,f)
#endif

#if defined( __WINDOWS__ ) && defined( _M_I86 )
extern void FreeProcInstance_DDE( PFNCALLBACK f );
#pragma aux FreeProcInstance_DDE = FREEPROCINSTANCE_INLINE
#else
#define FreeProcInstance_DDE(f)     ((void)f)
#endif

#ifdef __cplusplus
}
#endif
