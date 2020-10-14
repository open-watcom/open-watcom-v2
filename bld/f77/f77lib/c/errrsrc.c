/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  for error messages in resource files
*
****************************************************************************/


#include "ftnstd.h"
#include <stdio.h>
#include "cioconst.h"
#include "wressetr.h"
#include "wresset2.h"
#include "wreslang.h"
#include "errrsrc.h"
#include "blderr.h"
#include "errrtns.h"


static  HANDLE_INFO     hInstance = { 0 };
static  unsigned        MsgShift;

static bool LoadMsg( unsigned int msg, char *buffer, int buff_size )
// Load a message into the specified buffer.  This function is called
// by WLINK when linked with 16-bit version of WATFOR-77.
{
    return( hInstance.status && ( WResLoadString( &hInstance, msg + MsgShift, buffer, buff_size ) > 0 ) );
}


static void BldErrMsg( unsigned int err, char *buffer, va_list args )
// Build error message.
{
    *buffer = NULLCHAR;
    if( LoadMsg( err, &buffer[1], ERR_BUFF_SIZE - 1 ) ) {
        buffer[0] = ' ';
        Substitute( buffer, buffer, args );
    }
}

static void ErrorInit( const char *pgm_name )
{
    hInstance.status = 0;
    if( OpenResFile( &hInstance, pgm_name ) ) {
        MsgShift = _WResLanguage() * MSG_LANG_SPACING;
        return;
    }
    CloseResFile( &hInstance );
}

static void ErrorFini( void )
{
    CloseResFile( &hInstance );
}

void __InitResource( void )
{
    __ErrorInit = &ErrorInit;
    __ErrorFini = &ErrorFini;
    __BldErrMsg = &BldErrMsg;
}
