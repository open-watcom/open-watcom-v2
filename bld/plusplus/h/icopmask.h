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


#ifndef __ICOPMASK_H
#define __ICOPMASK_H

#if _INTEL_CPU
typedef uint_8 icop_mask;
#else
typedef unsigned icop_mask;
#endif

enum {                          // opcodes that affect ...
    ICOPM_DWARF         = 0x01, // Dwarf info generation
    ICOPM_VFT_SCAN      = 0x02, // virtual function table call graph scan
    ICOPM_OE_COUNT      = 0x04, // contributes to -oe count
    ICOPM_CALLGRAPH     = 0x08, // call graph analysis
    ICOPM_PCHREAD       = 0x10, // PCH read significant op codes
    ICOPM_BRINFO        = 0x20, // Browse-info mask
    ICOPM_NULL          = 0x00
};

#endif
