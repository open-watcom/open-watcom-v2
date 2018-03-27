/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DOS stub stuff
*
****************************************************************************/


#include <stddef.h>
#include "watcom.h"
#include "linkstd.h"
#include "msg.h"
#include "wlnkmsg.h"
#include "exedos.h"
#include "loadfile.h"
#include "dosstub.h"


static unsigned_8       DOSDefStub[] = {
    0x4D, 0x5A, 0x80, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x04, 0x00, 0x00, 0x00, 0xFF, 0xFF, 0x00, 0x00,
    0xB8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00,
    0x0E, 0x1F, 0xBA, 0x0E, 0x00, 0xB4, 0x09, 0xCD,
    0x21, 0xB8, 0x01, 0x4C, 0xCD, 0x21
};

static size_t doExeTypeString( void )
/***********************************/
/* make up the "program is %f" string, and put it in tokbuff.*/
{
    char        rc_buff[RESOURCE_MAX_SIZE];
    size_t      msgsize;

    Msg_Get( MSG_IS_A_EXE, rc_buff );
    msgsize = FmtStr( TokBuff, TokSize, rc_buff );
    TokBuff[msgsize++] = '\r';
    TokBuff[msgsize++] = '\n';
    TokBuff[msgsize] = '$';     /* end of string for int 21 fn. 9 */
    return( msgsize + 1 );
}

size_t GetDOSDefStubSize( void )
/******************************/
{
    return( sizeof( DOSDefStub ) + doExeTypeString() );
}

unsigned_32 WriteDOSDefStub( unsigned_32 stub_align )
/***************************************************/
/* write the default stub to the executable file */
{
    size_t              msgsize;
    unsigned_32         fullsize;
    unsigned_32         *stubend;

    msgsize = doExeTypeString();
    fullsize = ROUND_UP( msgsize + sizeof( DOSDefStub ), stub_align );
    stubend = (unsigned_32 *)( DOSDefStub + NH_OFFSET );
    *stubend = fullsize;
    WriteLoad( DOSDefStub, sizeof( DOSDefStub ) );
    WriteLoad( TokBuff, msgsize );
    PadLoad( fullsize - msgsize - sizeof( DOSDefStub ) );
    return( fullsize );
}
