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
*               for help callback function prototypes
*
****************************************************************************/


#include "wclbproc.h"


#ifdef __cplusplus
extern "C" {
#endif

#if defined( __OS2__ )
typedef void (EXPENTRY *HELPFUNC)(void);

#define MakeProcInstance_HELP(f,i)  (HELPFUNC)_wpi_makeprocinstance((WPI_PROC)f, i)
#define FreeProcInstance_HELP(f)    _wpi_freeprocinstance((WPI_PROC)f)
#else
typedef void (CALLBACK *HELPFUNC)(void);

#if defined( __WINDOWS_386__ )
typedef void        (CALLBACK *HELPFUNCx)( void );
#else
#define HELPFUNCx   HELPFUNC
#endif

#if defined( __WINDOWS__ )
extern HELPFUNCx    MakeProcInstance_HELP( HELPFUNCx fn, HINSTANCE instance );
#pragma aux MakeProcInstance_HELP = MAKEPROCINSTANCE_INLINE
#else
#define MakeProcInstance_HELP(f,i)   ((void)i,f)
#endif

#if defined( __WINDOWS__ ) && defined( _M_I86 )
extern void FreeProcInstance_HELP( HELPFUNC f );
#pragma aux FreeProcInstance_HELP = FREEPROCINSTANCE_INLINE
#else
#define FreeProcInstance_HELP(f)     ((void)f)
#endif

#endif

#ifdef __cplusplus
}
#endif
