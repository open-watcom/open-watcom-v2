/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include "dbgdefn.h"
#include "dbgdata.h"
#ifdef __AXP__
#include <windows.h>
#endif
#include "dbgcmdln.h"
#include "dbglkup.h"


#ifdef __AXP__
#define SYS_OPT_DEFS \
    pick( "Popups", OPT_POPUPS          ) \
    pick( "XXAT",   OPT_ALIGN_TRAP      ) \
    pick( "XXAE",   OPT_ALIGN_EMULATE   )
#else
#define SYS_OPT_DEFS \
    pick( "Popups", OPT_POPUPS          )
#endif

enum {
    #define pick(t,e)   e,
    SYS_OPT_DEFS
    #undef pick
};

static const char SysOptNameTab[] = {
    #define pick(t,e)   t "\0"
    SYS_OPT_DEFS
    #undef pick
};

bool OptDelim( char ch )
{
    return( ch == '/' || ch == '-' );
}

/*
 * ProcSysOption -- process system option
 */

bool ProcSysOption( const char *start, unsigned len, int pass )
{
#ifdef __AXP__
    DWORD       curr_err;
#endif

#ifndef __AXP__
    /* unused parameters */ (void)pass;
#endif

    switch( Lookup( SysOptNameTab, start, len ) ) {
    case OPT_POPUPS:
        _SwitchOn( SW_DETACHABLE_WINDOWS );
        break;
#ifdef __AXP__
    case OPT_ALIGN_TRAP:
        if( pass == 2 ) {
            curr_err = SetErrorMode( 0 );
            curr_err &= ~SEM_NOALIGNMENTFAULTEXCEPT;
            SetErrorMode( curr_err );
        }
        break;
    case OPT_ALIGN_EMULATE:
        if( pass == 2 ) {
            curr_err = SetErrorMode( 0 );
            curr_err |= SEM_NOALIGNMENTFAULTEXCEPT;
            SetErrorMode( curr_err );
        }
        break;
#endif
    default:
        return( false );
    }
    return( true );
}


/*
 * ProcSysOptInit -- initial system options
 */

void ProcSysOptInit( void )
{
}
