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
* Description:  Real mode selector setup for DOS extenders and Win386.
*
****************************************************************************/


#define __WATCOM_LFN__
#include "widechar.h"
#include "variety.h"
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include "watcom.h"
#include "extender.h"
#include "exitwmsg.h"
#include "rtinit.h"
#include "_doslfn.h"

extern unsigned __alloc_dos_tb( unsigned short size, unsigned short *segm );
#pragma aux __alloc_dos_tb = \
        "mov  ax,100h"  \
        "int  31h"      \
        "jc short L1"   \
        "mov  [ecx],ax" \
        "xor  eax,eax"  \
        "mov  ax,dx"    \
        "jmp short L2"  \
"L1:     xor  eax,eax"  \
"L2:"                   \
        parm caller     [bx] [ecx] \
        modify exact    [eax bx edx];

extern unsigned __free_dos_tb( unsigned short );
#pragma aux __free_dos_tb = \
        "mov  ax,101h"  \
        "int  31h"      \
        "sbb  eax,eax"  \
        parm caller     [dx] \
        modify exact    [eax dx];

char                    * const __lfn_rm_tb_linear = 0;
unsigned short          const __lfn_rm_tb_segment = 0;

static unsigned short   __lfn_rm_tb_selector = 0;

#define TOTAL_RM_TB_SIZE_PARA   ((RM_TB_PARM1_SIZE + RM_TB_PARM2_SIZE + 15)/16)

static void init( void )
/**********************/
{
    __lfn_rm_tb_selector = __alloc_dos_tb( TOTAL_RM_TB_SIZE_PARA, 
                                      (unsigned short *)&__lfn_rm_tb_segment );
    if( __lfn_rm_tb_selector == 0 ) {
        __fatal_runtime_error( "Unable to allocate LFN real mode transfer buffer", -1 );
    }
    *(long *)&__lfn_rm_tb_linear = TinyDPMIBase( __lfn_rm_tb_selector );
}

static void fini( void )
/**********************/
{
    __free_dos_tb( __lfn_rm_tb_selector );
}

AXI( init, INIT_PRIORITY_RUNTIME )
AYI( fini, INIT_PRIORITY_RUNTIME )

