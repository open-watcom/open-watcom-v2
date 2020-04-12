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
*               for callback function prototypes
*
****************************************************************************/


#include "wclbproc.h"


#define DISPATCHERPROC      DISPATCH_FN *

#if defined( __WINDOWS_386__ )
typedef bool                (CALLBACK *DISPATCHERPROCx)(ACTION_ID, OBJPTR, void *, void *);
#else
#define DISPATCHERPROCx     DISPATCHERPROC
#endif

#if defined( __WINDOWS__ )
extern DISPATCHERPROC       MakeProcInstance_DISPATCHER( DISPATCHERPROCx fn, HINSTANCE instance );
#pragma aux MakeProcInstance_DISPATCHER = MAKEPROCINSTANCE_INLINE
#else
#define MakeProcInstance_DISPATCHER(f,i)    ((void)i,f)
#endif

#if defined( __WINDOWS__ ) && defined( _M_I86 )
void FreeProcInstance_DISPATCHER( DISPATCHERPROC fn );
#pragma aux FreeProcInstance_DISPATCHER = FREEPROCINSTANCE_INLINE
#else
#define FreeProcInstance_DISPATCHER(f)      ((void)f)
#endif
