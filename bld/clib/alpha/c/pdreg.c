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
#include <windows.h>
#include "rtdata.h"
#ifdef BROKEN
#include "pdreg.h"
#endif

#define GetContextReg(ctx, reg) (*(void**)&((ctx)->Int##reg))

#define AXP_MOV_SP_FP   0x47FE040F
#define AXP_INSTR_SIZE  4


typedef struct __PDATA
{
    unsigned long entry;
    unsigned long end;
    unsigned long handler;
    unsigned long data;
    unsigned long endProlog;
} _PDATA;

_WCRTLINK int _ProcSetsFP(_PDATA *pdata)
{
    unsigned int inst;
    int          usesFP = 0;

    if (pdata->entry != pdata->endProlog)
    {
        inst = *(unsigned int *)(((char *)pdata->endProlog) - AXP_INSTR_SIZE);
        if (inst == AXP_MOV_SP_FP)
            usesFP = 1;
    } /* if */

    return usesFP;
} /* _ProcSetsFP() */

#ifdef BROKEN

_WCRTLINK _EXCINFO *_SetPData(_EXCINFO *info)
{
    info->pdata = (_PDATA *)RtlLookupFunctionEntry(info->pc);
    return info;
} /* _SetPData() */


_WCRTLINK void _NextExcInfo(_EXCINFO *info)
{
    info->pc = RtlVirtualUnwind(info->pc, info->pdata, &info->context,
                                &info->in_func, info->est_frame, 0);
    _SetPData(info);
} /* _NextExcInfo() */


_WCRTLINK void _InitExcInfo(_EXCINFO *info)
{
    RtlCaptureContext(&info->context);
    info->in_func = 1;
    info->pc      = GetContextReg(&info->context, Ra);

    _SetPData(info);
    _NextExcInfo(info);
} /* _InitExcInfo() */
#endif
