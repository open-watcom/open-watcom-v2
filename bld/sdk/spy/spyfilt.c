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
* Description:  Spy message filter function.
*
****************************************************************************/


#include "spy.h"
#include <stdio.h>
#include <string.h>

/*
 * HandleMessage
 */
void CALLBACK HandleMessage( LPMSG pmsg )
{
    static char msg[80];
    static char class_name[80];
    int         i;

    if( SpyState != ON ) {
        return;
    }
    if( IsMyWindow( pmsg->hwnd ) ) {
        return;
    }
    if( WindowCount != 0 ) {
        for( i = 0; i < WindowCount; i++ ) {
            if( pmsg->hwnd == WindowList[i] ) {
                break;
            }
        }
        if( i == WindowCount ) {
            return;
        }
    }
    GetClassName( pmsg->hwnd, class_name, 80 );
    ProcessIncomingMessage( pmsg->message, class_name, msg );
    if( msg[0] != 0 ) {
        SpyOut( msg, pmsg );
    }

} /* HandleMessage */

