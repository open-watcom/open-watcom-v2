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

#include <stdio.h>
#include <stdlib.h>
#include "stdnt.h"
#undef GetThreadContext
#undef SetThreadContext

#if defined( MD_x86 )
#define VDMCONTEXT_TO_USE (VDMCONTEXT_CONTROL | VDMCONTEXT_INTEGER | \
                    VDMCONTEXT_SEGMENTS | VDMCONTEXT_DEBUG_REGISTERS | \
                    VDMCONTEXT_FLOATING_POINT | VDMCONTEXT_EXTENDED_REGISTERS)
#elif defined( MD_x64 )
#define VDMCONTEXT_TO_USE
#elif defined( MD_axp ) | defined( MD_ppc )
#define VDMCONTEXT_TO_USE (VDMCONTEXT_CONTROL | VDMCONTEXT_INTEGER | \
                    VDMCONTEXT_SEGMENTS | VDMCONTEXT_DEBUG_REGISTERS | \
                    VDMCONTEXT_FLOATING_POINT)
#else
    #error VDMCONTEXT_TO_USE not configured
#endif

#if defined( MD_x64 )
#define WOWCONTEXT_TO_USE
#elif defined( MD_x86 ) || defined( MD_axp ) | defined( MD_ppc )
#define WOWCONTEXT_TO_USE
#else
    #error WOWCONTEXT_TO_USE not configured
#endif

/*
 * MyGetThreadContext - get the context for a specific thread
 */
BOOL MyGetThreadContext( thread_info *ti, MYCONTEXT *pc )
{
#ifdef WOW
    BOOL    rc;

    if( ( ti->is_wow || ti->is_dos ) && UseVDMStuff ) {
#if defined( MD_x86 )
        VDMCONTEXT      vc;

        vc.ContextFlags = VDMCONTEXT_TO_USE;
        rc = pVDMGetThreadContext( &DebugEvent, &vc );
        /*
         * VDMCONTEXT and CONTEXT are the same on an x86 machine.
         * If we were ever to try to port this to NT running on a RISC,
         * they would be different, and this memcpy would be total crap.
         */
        memcpy( pc, &vc, sizeof( MYCONTEXT ) );
        /*
         * Sometimes crap is in the high word of EIP, ESP or EBP.  We
         * check if CS or SS is a 32-bit selector, and if they are not,
         * we zero out the high word of EIP, ESP or EBP as appropriate
         */
        if( !IsBigSel( pc->SegCs ) ) {
            pc->Eip = (DWORD)(WORD)pc->Eip;
        }
        if( !IsBigSel( pc->SegSs ) ) {
            pc->Esp = (DWORD)(WORD)pc->Esp;
            pc->Ebp = (DWORD)(WORD)pc->Ebp;
        }
#elif defined( MD_axp ) | defined( MD_ppc )
        rc = 0;
#else
        #error MyGetThreadContext not configured
#endif
        return( rc );
    } else {
        pc->ContextFlags = MYCONTEXT_TO_USE;
        return( GetThreadContext( ti->thread_handle, pc ) );
    }
#else
#if 1
    pc->ContextFlags = MYCONTEXT_TO_USE;
#if defined( MD_x64 )
    return( Wow64GetThreadContext( ti->thread_handle, pc ) );
#else
    return( GetThreadContext( ti->thread_handle, pc ) );
#endif
#else
#if defined( MD_x64 )
    if( ti->is_wow ) {
        pc->ContextFlags = WOW64CONTEXT_TO_USE;
        return( Wow64GetThreadContext( ti->thread_handle, pc ) );
    }
#endif
    pc->ContextFlags = MYCONTEXT_TO_USE;
    return( GetThreadContext( ti->thread_handle, pc ) );
#endif
#endif
}

/*
 * MySetThreadContext - set the context for a specific thread
 */
BOOL MySetThreadContext( thread_info *ti, MYCONTEXT *pc )
{
#ifdef WOW
    if( ( ti->is_wow || ti->is_dos ) && UseVDMStuff ) {
#if defined( MD_x86 )
        VDMCONTEXT      vc;
        /*
         * VDMCONTEXT and CONTEXT are the same on an x86 machine.
         * If we were ever to try to port this to NT running on a RISC,
         * they would be different, and this memcpy would be total crap.
         */
        memcpy( &vc, pc, sizeof( MYCONTEXT ) );
        vc.ContextFlags = VDMCONTEXT_TO_USE;
        return( pVDMSetThreadContext( &DebugEvent, &vc ) );
#elif defined( MD_axp ) | defined( MD_ppc )
        return( FALSE );
#else
        #error MySetThreadContext not configured
#endif
    } else {
        pc->ContextFlags = MYCONTEXT_TO_USE;
        return( SetThreadContext( ti->thread_handle, pc ) );
    }
#else
#if 1
    pc->ContextFlags = MYCONTEXT_TO_USE;
#if defined( MD_x64 )
    return( Wow64SetThreadContext( ti->thread_handle, pc ) );
#else
    return( SetThreadContext( ti->thread_handle, pc ) );
#endif
#else
#if defined( MD_x64 )
    if( ti->is_wow ) {
        pc->ContextFlags = WOW64CONTEXT_TO_USE;
        return( Wow64SetThreadContext( ti->thread_handle, pc ) );
    }
#endif
    pc->ContextFlags = MYCONTEXT_TO_USE;
    return( SetThreadContext( ti->thread_handle, pc ) );
#endif
#endif
}
