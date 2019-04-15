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


#include "dbgdefn.h"
#include "dbgdata.h"
#ifdef __AXP__
#include <windows.h>
#endif
#include "dbgcmdln.h"
#include "dbglkup.h"


static const char SysOptNameTab[] = {
    "Popups\0"
#ifdef __AXP__
    "XXAT\0"
    "XXAE\0"
#endif
};

enum {
    OPT_POPUPS,
    OPT_ALIGN_TRAP,
    OPT_ALIGN_EMULATE
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
