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
* Description:  Alpha exception processing helpers.
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <windows.h>
#include "rtdata.h"
#include "pdreg.h"

#define GetContextReg(ctx, reg) (*(void**)&((ctx)->Int##reg))

#define AXP_MOV_SP_FP   0x47FE040F
#define AXP_INSTR_SIZE  4


_WCRTLINK int _ProcSetsFP( _PDATA *pdata )
{
    unsigned int inst;
    int          usesFP = 0;

    if( pdata->BeginAddress != pdata->PrologEndAddress ) {
        inst = *(unsigned int *)( ((char *)pdata->PrologEndAddress) - AXP_INSTR_SIZE);
        if (inst == AXP_MOV_SP_FP) {
            usesFP = 1;
        }
    }

    return( usesFP );
}

#ifdef BROKEN

_WCRTLINK _EXCINFO *_SetPData( _EXCINFO *info )
{
    info->FunctionEntry = RtlLookupFunctionEntry( info->ControlPC );
    return info;
}


_WCRTLINK void _NextExcInfo( _EXCINFO *info )
{
    info->ControlPC = RtlVirtualUnwind( info->ControlPC, info->FunctionEntry, &info->ContextRecord, &info->InFunction, info->EstablisherFrame, NULL );
    _SetPData( info );
}


_WCRTLINK void _InitExcInfo( _EXCINFO *info )
{
    RtlCaptureContext( &info->ContextRecord );
    info->InFunction = 1;
    info->ControlPC = GetContextReg( &info->ContextRecord, Ra );
    _SetPData( info );
    _NextExcInfo( info );
}
#endif
