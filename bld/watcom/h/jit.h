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


#ifndef _JIT_H_INCLUDED
#define _JIT_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

// this file defines the layout of a code 'string' which is
// included in FAT class files for use by our micro-JIT. Any
// symbols which pollute the global namespace should be
// prefixed with the letters "jit" (any case) (NB except for
// that stupid protecting-define, which is done as _JIT for
// consistency because we always do this, even though that
// stomps on the namespace reserved for compiler implementa-
// tions and we'll all go to hell someday).

// everything should be naturally (4-byte) aligned, but just
// in case...
#pragma pack(__push,8)
#pragma enum int

typedef unsigned_32     jit_offset;     // offsets into our little code string
typedef unsigned_32     jit_rt_index;   // number representing a runtime cg function
typedef signed_32       jit_cp_index;   // index into the constant pool

typedef enum {
    JIT_ARCH_I86,
    JIT_ARCH_SPARC,
    JIT_ARCH_LAST
} jit_arch;

typedef struct {
    jit_offset          offset;         // offset from beginning of code string
    jit_offset          size;           // length of this section
} jit_data_sec;

typedef struct {
    jit_arch            arch;
    jit_data_sec        code;
    jit_data_sec        data;
    jit_data_sec        pdata;
    jit_data_sec        relocs;
    jit_data_sec        eh_table;
} jit_code_str;

#define JitRtcs              \
 JitRtc( New )               \
 JitRtc( NewArray )          \
 JitRtc( ANewArray )         \
 JitRtc( ANewMulti )         \
 JitRtc( Throw )             \
 JitRtc( ThrowArrayIndex )   \
 JitRtc( InstanceOf )        \
 JitRtc( CheckCast )         \
 JitRtc( MonitorEnter )      \
 JitRtc( MonitorExit )       \
 JitRtc( MethodEnter )       \
 JitRtc( MethodExit )        \
 JitRtc( InterfaceVtable )   \
 JitRtc( AString )           \

typedef enum {
    #define JitRtc(x) JIT_RTC_##x,
    JitRtcs
    #undef JitRtc
    JIT_RTC_end
} jit_helper_index;

#define JitRelocs            \
 JitReloc( METHOD_OFFSET )   /* offset of a member in a vtable */                   \
 JitReloc( FIELD_OFFSET )    /* ditto for fields */                                 \
 JitReloc( METHOD_ADDR )     /* 32-bit address of a static method */                \
 JitReloc( FIELD_ADDR )      /* ditto for fields */                                 \
 JitReloc( HELPER_FUNC )     /* call to one of above rtc */                         \
 JitReloc( RT_FUNC )         /* call to a cg runtime */                             \
 JitReloc( DATA_REF )        /* offset to start of data section */                  \
 JitReloc( METHOD_HANDLE )   /* 32-bit methods handle for given method in class */  \
 JitReloc( INTERFACE_HANDLE )/* 32-bit class handle for given class */              \
 JitReloc( CLASS_HANDLE )    /* 32-bit class handle for given class */              \

typedef enum {
    #define JitReloc(x) JIT_RELOC_##x,
    JitRelocs
    #undef JitReloc
    JIT_RELOC_LAST
} jit_reloc_type;

typedef enum {
    JIT_SECTION_CODE,
    JIT_SECTION_DATA,
    JIT_SECTION_EH_TABLE,
} jit_section_type;

typedef struct {
    jit_reloc_type      type;
    jit_offset          location;
    union {
        jit_helper_index   helper_index;
        jit_cp_index       cp_index;
        jit_rt_index       rt_index;
        jit_offset         addend;                 /* for JIT_RELOC_DATA_REF - added to addr of code section */
        unsigned_32        place_holder;           /* in case idx's were 16-bit, force u to be 4 bytes */
    } u;
} jit_reloc;

typedef struct {
    jit_offset      beg;
    jit_offset      end;
    jit_offset      handler;
    jit_cp_index    cp_index;
} jit_eh_table;

typedef struct {
    jit_offset      rtn_beg;
    jit_offset      pro_end;
    jit_offset      epi_beg;
    jit_offset      rtn_end;
    jit_offset      parms;
    jit_offset      locals;
} jit_pdata;

extern void DumpJitCodeStr( const char *buffer, unsigned len, void (*output)( char ) );
extern void DumpCodeStr( const char *buffer, unsigned len, void (*output)( char ) );

#pragma enum pop
#pragma pack(__pop)

#ifdef __cplusplus
}
#endif

#endif
