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


#include "madimp.h"
#include "madx86.h"
#include "regdef.h"
#include "instr.h"

typedef enum { LN, L1, L2, L3, L5, LX } processor_level;

struct imp_mad_state_data {
    enum {
        CT_HEX          = 0x1,
        CT_EXTENDED     = 0x2,
        CT_OS           = 0x4
    }           cpu_toggles;
    enum {
        FT_HEX          = 0x1
    }           fpu_toggles;
    enum {
        MT_HEX          = 0x1,
        MT_SIGNED       = 0x2,
        MT_BYTE         = 0x4,
        MT_WORD         = 0x8,
        MT_DWORD        = 0x10
    }           mmx_toggles;
    enum {
        DT_UPPER        = 0x1,
        DT_INSIDE       = 0x2
    }                   disasm_toggles;
};

typedef struct x86_reg_info     x86_reg_info;
struct x86_reg_info {
    mad_reg_info        info;
    const x86_reg_info  * const *sublist;
    unsigned            cpulevel        : 3;
    unsigned            fpulevel        : 3;
};

struct mad_disasm_data {
    instruction         ins;
    address             addr;
    unsigned            characteristics;
    char                buff[80];
};

struct mad_trace_data {
    address             brk;
    unsigned            prev_opcode;
    unsigned            prev_pref;
};

struct mad_call_up_data {
    unsigned_8          dummy;
};


extern imp_mad_state_data       *MADState;
extern address                  DbgAddr;
extern const x86_reg_info       CPU_eax;
extern const x86_reg_info       CPU_ebx;
extern const x86_reg_info       CPU_ecx;
extern const x86_reg_info       CPU_edx;
extern const x86_reg_info       CPU_ax;
extern const x86_reg_info       CPU_bx;
extern const x86_reg_info       CPU_cx;
extern const x86_reg_info       CPU_dx;

extern address                  GetRegIP( const mad_registers * );
extern address                  GetRegSP( const mad_registers * );
extern address                  GetRegFP( const mad_registers * );
typedef                         walk_result (MEMREF_WALKER)( address, mad_type_handle, mad_memref_kind write, void * );
extern walk_result              DoDisasmMemRefWalk( mad_disasm_data *, MEMREF_WALKER *, const mad_registers *, void * );
extern void                     DecodeIns( address *a, instruction *ins, int big );
extern mad_status               GetDisasmPrev( address *a );
extern void                     InitCache( address start, unsigned len );
extern unsigned                 RegDispType( mad_type_handle, const void *, unsigned, char * );
extern unsigned                 AddrCharacteristics( address );
extern char                     *CnvRadix( unsigned long value, unsigned radix, char base, char *buff, int len );

#define BIG_SEG( a )    ((AddrCharacteristics(a)&X86AC_BIG)!=0)
