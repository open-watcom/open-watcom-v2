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
*               for helptool.dll callback function prototypes
*
****************************************************************************/


#include "wclbproc.h"


#ifdef __cplusplus
extern "C" {
#endif

#if defined( __WINDOWS__ )
typedef void (CALLBACK *LPFNINTHCALLBACK)( void );
#endif

#if defined( __WINDOWS_386__ )
typedef void                (CALLBACK *LPFNINTHCALLBACKx)( void );
typedef BOOL                (CALLBACK *LPFNNOTIFYCALLBACKx)( WORD, DWORD );
#else
#define LPFNINTHCALLBACKx LPFNINTHCALLBACK
#define LPFNNOTIFYCALLBACKx LPFNNOTIFYCALLBACK
#endif

#if defined( __WINDOWS__ )
extern LPFNINTHCALLBACK   MakeProcInstance_INTH( LPFNINTHCALLBACKx fn, HINSTANCE instance );
extern LPFNNOTIFYCALLBACK   MakeProcInstance_NOTIFY( LPFNNOTIFYCALLBACKx fn, HINSTANCE instance );
#pragma aux MakeProcInstance_INTH = MAKEPROCINSTANCE_INLINE
#pragma aux MakeProcInstance_NOTIFY = MAKEPROCINSTANCE_INLINE
#else
#define MakeProcInstance_INTH(f,i)      ((void)i,f)
#define MakeProcInstance_NOTIFY(f,i)    ((void)i,f)
#endif

#if defined( __WINDOWS__ ) && defined( _M_I86 )
extern void FreeProcInstance_INTH( LPFNINTHCALLBACK f );
extern void FreeProcInstance_NOTIFY( LPFNNOTIFYCALLBACK f );
#pragma aux FreeProcInstance_INTH = FREEPROCINSTANCE_INLINE
#pragma aux FreeProcInstance_NOTIFY = FREEPROCINSTANCE_INLINE
#else
#define FreeProcInstance_INTH(f)        ((void)f)
#define FreeProcInstance_NOTIFY(f)      ((void)f)
#endif

#ifdef __cplusplus
}
#endif
