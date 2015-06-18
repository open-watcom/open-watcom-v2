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
* Description:  Safe recursion routine with explicit stack overflow check.
*
****************************************************************************/


#include "cgstd.h"
#include "stack.h"

#if defined( __WATCOMC__ ) && defined( _M_IX86 ) && !defined( __NT__ )

#include "walloca.h"
#include "cgmem.h"
#include "memout.h"
#include "cypfunc.h"
#include "onexit.h"

extern char __near      *bp( void );
#pragma aux bp = 0x89 0xe8 value [ eax ];

extern char __near      *sp( void );
#pragma aux sp = value [ esp ];

extern void             setsp( void __near * );
#pragma aux setsp = 0x89 0xc4 parm [ eax ] modify [ esp ];

extern void             setbp( void __near * );
#pragma aux setbp = 0x89 0xc5 parm [ eax ];

extern  mem_out_action  SetMemOut(mem_out_action);

void    *SafeRecurseCG( func_sr rtn, void *arg )
/**********************************************/
/* This code assumes NO parameters on the stack! */
{
    #define SAVE_SIZE   512     /* this must be smaller than the stack */

    void                *savearea;
    void                *retval;
    mem_out_action      old_action;

    if( stackavail() < 0x2000 ) { /* stack getting low! */
        old_action = SetMemOut( MO_OK );
        savearea = CGAlloc( SAVE_SIZE );
        if( savearea == NULL ) {
            FatalError( "No memory to save stack" );
        }
        SetMemOut( old_action );
        CypCopy( bp(), savearea, SAVE_SIZE );
        CypCopy( sp(), sp() + SAVE_SIZE, bp() - sp() );
        setbp( bp() + SAVE_SIZE );
        setsp( sp() + SAVE_SIZE );
        retval = rtn( arg );
        setsp( sp() - SAVE_SIZE );
        CypCopy( sp() + SAVE_SIZE, sp(), bp() - sp() - SAVE_SIZE );
        setbp( bp() - SAVE_SIZE );
        CypCopy( savearea, bp(), SAVE_SIZE );
        CGFree( savearea );
        return( retval );
    } else {
        return( rtn( arg ) );
    }
}

#else

void    *SafeRecurseCG( func_sr rtn, void *arg )
/**********************************************/
{
    return( rtn( arg ) );
}

#endif


#if 0
#ifndef __AXP__
#ifndef STACK_OVERWRITE_CHECK

static uint_32  oldValue;
const char *errString = "Stack hit!";

#pragma aux _stashit modify exact [] = \
        "push   eax" \
        "mov    eax,oldValue" \
        "mov    +8[esp],eax" \
        "mov    eax,+12[esp]" \
        "mov    oldValue,eax" \
        "pop    eax";

#pragma aux _restoreit modify exact [] = \
        "push   eax" \
        "mov    eax,oldValue" \
        "xor    eax,+12[esp]" \
        "je     ok" \
        "lea    eax,errString" \
        "call   DumpString" \
        "call   DumpNL" \
        "ok:" \
        "mov    eax,+8[esp]" \
        "mov    oldValue,eax" \
        "pop    eax";

extern void _stashit( void );
extern void _restoreit( void );

#pragma aux __PRO "*";
#pragma aux __EPI "*";

void __PRO( void ) {
    _stashit();
}

void __EPI( void ) {
    _restoreit();
}

#endif
#endif
#endif
