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
* Description:  Overlay manager debug service prototype and constant
*                   declarations
*
****************************************************************************/

#ifndef _OVLDBG_H_
#define _OVLDBG_H_

#define OVL_SIGNATURE   0x2112

enum ovldbg_service {
    OVLDBG_GET_STATE_SIZE,
    OVLDBG_GET_OVERLAY_STATE,
    OVLDBG_SET_OVERLAY_STATE,
    OVLDBG_TRANSLATE_VECTOR_ADDR,
    OVLDBG_TRANSLATE_RETURN_ADDR,
    OVLDBG_GET_OVL_TBL_ADDR,
    OVLDBG_GET_MOVED_SECTION,
    OVLDBG_GET_SECTION_DATA
};

typedef void    __far ovl_dbg_hook_func( unsigned, char, void __far * );
typedef int     __far ovl_dbg_req_func( int, void __far * );

#include "pushpck1.h"               /* make sure no structures are padded. */

struct ovl_header {
    unsigned char       short_jmp[2];
    unsigned_16         signature;
    ovl_dbg_hook_func   *hook;
    unsigned_16         handler_offset;
};

#include "poppck.h"

#endif
