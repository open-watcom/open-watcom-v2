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


#include "metaconv.h"

#pragma aux (metaware) dbg_init;
#pragma aux (metaware) dbg_exit;
#pragma aux (metaware) dbg_edebug;
#pragma aux (metaware) dbg_ldebug;
#pragma aux (metaware) dbg_load;
#pragma aux (metaware) dbg_go;
#pragma aux (metaware) dbg_kill;
#pragma aux (metaware) dbg_rdmsb;
#pragma aux (metaware) dbg_wrmsb;
#pragma aux (metaware) dbg_pread;
#pragma aux (metaware) dbg_pwrite;
#pragma aux (metaware) dbg_rread;
#pragma aux (metaware) dbg_rwrite;
#pragma aux (metaware) dbg_phread;
#pragma aux (metaware) dbg_phwrite;
#pragma aux (metaware) dbg_lread;
#pragma aux (metaware) dbg_lwrite;
#pragma aux (metaware) dbg_iport;
#pragma aux (metaware) dbg_oport;
#pragma aux (metaware) dbg_ptolin;
#pragma aux (metaware) dbg_lintop;
#pragma aux (metaware) dbg_rdsdes;
#pragma aux (metaware) dbg_wrsdes;
#pragma aux (metaware) dbg_selinfo;
#pragma aux (metaware) dbg_desinfo;
#pragma aux (metaware) dbg_rdides;
#pragma aux (metaware) dbg_writes;
#pragma aux (metaware) dbg_ccreg;

long    _mwstack_limit = 0; /* metaware stack check variable */
#pragma aux (metaware) _mwstack_limit;
#define DRLen( l ) ( ( (l)==1 ) ? DR7_L1 : ( ( (l)==2 ) ? DR7_L2 : DR7_L4 ) )
