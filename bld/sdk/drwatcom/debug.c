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


#include <dos.h>
#include <stdio.h>
#include <string.h>
#include "drwatcom.h"
#include "wdebug.h"

/*
 * NumToAddr - convert a segment number to an address
 */
WORD NumToAddr( HANDLE modhandle, WORD num )
{
    GLOBALENTRY ge;

    if( !DoGlobalEntryModule( &ge, modhandle, num ) ) {
        return( NULL );
    }
    return( GlobalHandleToSel( ge.hBlock ) );

} /* NumToAddr */


/*
 * GetFaultString
 */
DWORD GetFaultString( int intnum, char *buff )
{
    DWORD       msgid;

    switch( intnum ) {
    case INT_DIV0:
        msgid = STR_DIV_BY_ZERO;
        break;
    case INT_1:
        msgid = STR_TRACE_TRAP;
        break;
    case 2:
        msgid = STR_PARITY_ERROR;
        break;
    case INT_3:
        msgid = STR_BREAKPOINT;
        break;
    case INT_UDINSTR:
        msgid = STR_INV_INSTRUC;
        break;
    case INT_STKFAULT:
        msgid = STR_STACK_FAULT;
        break;
    case INT_GPFAULT:
        msgid = STR_GP_FAULT;
        break;
    case INT_BADPAGEFAULT:
        msgid = STR_PAGE_FAULT;
        break;
    default:
        msgid = STR_UNKNOWN_FAULT;
        break;
    }
    if( buff != NULL ) {
        strcpy( buff, GetRCString( msgid ) );
    }
    return( msgid );

} /* GetFaultString */
