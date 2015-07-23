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
* Description:  Implementation of longjmpex() for Win32 on Alpha.
*
****************************************************************************/


#include <windows.h>
#include <setjmpex.h>

extern int   _ProcSetsFP( void * );
extern void  *RtlLookupFunctionEntry( unsigned long );
extern void  RtlUnwindRfp( unsigned long, unsigned long, void *, unsigned long );

typedef struct
{
    unsigned long   sp;
    unsigned long   pc;
    unsigned long   seb;
    unsigned long   type;
    unsigned long   notused[2];
    unsigned long   fp;
} _JUMPEXDATA;

void longjmpex( jmp_buf jb, int ret )
{
    EXCEPTION_RECORD    er;
    _JUMPEXDATA         *jd = (_JUMPEXDATA *)jb;

    er.ExceptionCode           = 0xE5670123;
    er.ExceptionFlags          = 0x00000002;
    er.ExceptionRecord         = 0L;
    er.ExceptionAddress        = 0L;
    er.NumberParameters        = 1L;
    er.ExceptionInformation[0] = jd->sp;

    if( !_ProcSetsFP( RtlLookupFunctionEntry( jd->pc ) ) )
        RtlUnwind( (void *)jd->sp, (void *)jd->pc, &er, (void *)ret );
    else
        RtlUnwindRfp( jd->fp, jd->pc, &er, ret );
} /* longjmp() */
