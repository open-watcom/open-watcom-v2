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


#include <string.h>
#include "commonui.h"
#include "sample.h"
#include "smpstuff.h"
#include "sampwin.h"
#include "intdata.h"

/*
 * FaultHandler - C handler for a fault
 */
WORD __cdecl FAR FaultHandler( fault_frame ff )
{
    BOOL        fault32;
    DWORD       offset;
    WORD        seg;
    WORD        value;
    int         len;
    seg_offset  where;
    char        buff[256];

    if( WDebug386 ) {
        fault32 = GetDebugInterruptData( &IntData );
        if( fault32 ) {
            ff.intnumber = IntData.InterruptNumber;
        }
    } else {
        fault32 = FALSE;
    }
    if( ff.intnumber != INT_3 && ff.intnumber != INT_1 ) {
        if( fault32 ) {
            DoneWithInterrupt( NULL );
        }
        return( CHAIN );
    }

    ff.ESP = (WORD)ff.ESP;
    ff.EBP = (WORD)ff.EBP;

    if( !fault32 ) {
        ff.IP--;
        SaveState( &IntData, &ff );
    }
    if( ff.intnumber == INT_1 ) {
        if( WaitForInt1 ) {
            HandleLibLoad( SAMP_CODE_LOAD, WaitForInt1 );
            WaitForInt1 = 0;
        }
    } else {
        if( WaitForInt3 && (WaitForInt3 == GetCurrentTask()) ) {
            TASKENTRY   te;

            if( MyTaskFindHandle( &te, GetCurrentTask() ) ) {
                HandleLibLoad( SAMP_MAIN_LOAD, te.hModule );
            }
            WaitForInt3 = 0;
        } else if( (IntData.EDX & 0xFFFF) != 0 ) {
            /*
             * get the segment and offset of the mark data
             */
            if( Is32BitSel( IntData.CS ) ) {
                offset = IntData.EAX;
            } else {
                offset = (DWORD)(WORD)IntData.EAX;
            }
            seg = (WORD)IntData.EDX;

            /*
             * copy the mark data
             */
            len = 0;
            for( ;; ) {
                ReadMem( seg, offset, &value, sizeof( value ) );
                buff[len] = (char)value;
                if( len == sizeof( buff ) )
                    buff[len] = '\0';
                if( buff[len] == '\0' )
                    break;
                len++;
                offset++;
            }

            /*
             * set the mark
             */
            where.segment = IntData.CS;
            where.offset = IntData.EIP;
            WriteMark( buff, where );
        } else {
            CommonAddr.offset = IntData.ECX & 0xffff; /* being passed ... */
            CommonAddr.segment = IntData.EBX;
        }
        IntData.EIP++;
    }
    if( !fault32 ) {
        RestoreState( &IntData, &ff );
    } else {
        DoneWithInterrupt( &IntData );
    }
    return( RESTART_APP );

} /* FaultHandler */
