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


#include "standard.h"
#include "hostsys.h"

#if defined( __AXP__ ) || defined( __NT__ )

pointer SafeRecurse( pointer (* rtn)(), pointer arg ) {
/*****************************************************/

    return( rtn( arg ) );
}

#else

#include "stackok.h"
#include "sysmacro.h"
#include "cypfunc.h"
#include "memout.h"
#include "cg.h"

pointer SafeRecurse( pointer (* rtn)(), pointer arg ) {
/*****************************************************/

    #define SAVE_SIZE   512     /* this must be smaller than the stack */

    extern  mem_out_action  SetMemOut(mem_out_action);
    extern  void            FatalError(char*);

    pointer             savearea;
    pointer             retval;
    mem_out_action      old_action;

    if( (unsigned)(sp() - stacklow()) < 0x200 ) { /* stack getting low! */
/*      This code assumes NO parameters on the stack! */
        old_action = SetMemOut( MO_OK );
        _Alloc( savearea, SAVE_SIZE );
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
        _Free( savearea, SAVE_SIZE );
        return( retval );
    } else {
        return( rtn( arg ) );
    }
}

#endif

#if 0
#ifndef __AXP__
#ifndef STACK_OVERWRITE_CHECK

static uint_32  oldValue;
const char *errString = "Stack hit!";

extern void DumpString( char * );
extern void DumpNL( void );

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
