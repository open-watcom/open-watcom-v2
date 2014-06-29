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
* Description:  Message output routines called by inline assembler.
*
****************************************************************************/


#include <string.h>
#include "cvars.h"

#if _INTEL_CPU

#ifdef _M_I86
#define ASMFAR __far
#else
#define ASMFAR
#endif

#define asmerr(code,msg)   msg
#include "asmerr.h"
#include "target.h"

void AsmError( int msg_number )
{
    char    msgbuf[80];

    strcpy( msgbuf, AsmErrMsgs[msg_number] );
    CErr2p( ERR_ASSEMBLER_ERROR, msgbuf );
}

#else

void AsmError( char *msg )
{
    CErr2p( ERR_ASSEMBLER_ERROR, msg );
}

#endif
