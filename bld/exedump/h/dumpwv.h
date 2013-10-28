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
* Description:  Structures and constants related to debugging information.
*
****************************************************************************/


#include "machtype.h"
#include "dbginfo.h"

#ifdef DEBUG
extern void debugout( char *, ... );
#define dbgout( a ) debugout a
#else
#define dbgout( a )
#endif

#include "pushpck1.h"

typedef struct {
    unsigned_32 off;
    unsigned_16 seg;
} myaddr;

typedef struct {
    unsigned_16 off;
    unsigned_16 seg;
} addr16;

typedef struct {
    unsigned_32 size;
    unsigned_32 seg;
    unsigned_32 off;
    bool        is32;
} block_defn;

typedef struct {
    char        *name;
    unsigned_32 retoff;
    unsigned_32 pro_size;
    unsigned_32 epi_size;
    block_defn  bd;
    bool        isfar;
    bool        islocal;
} proc_defn;

typedef struct {
    unsigned_8  size;
    unsigned_8  type;
    unsigned_16 start_offset;
    unsigned_16 code_size;
    unsigned_16 parent_block_offset;
} block;

typedef struct {
    unsigned_8  size;
    unsigned_8  type;
    unsigned_32 start_offset;
    unsigned_32 code_size;
    unsigned_16 parent_block_offset;
} block_386;

typedef struct {
    unsigned_8  size;
    unsigned_8  type;
    unsigned_16 off;
    unsigned_16 seg;
} set_base;

typedef struct {
    unsigned_8  size;
    unsigned_8  type;
    unsigned_32 off;
    unsigned_16 seg;
} set_base386;

#include "poppck.h"

/*
 * locals
 */
#define MODULE          0x10
#define LOCAL           0x11
#define MODULE_386      0x12
#define MODULE_LOC      0x13

#define BLOCK           0x20
#define NEAR_RTN        0x21
#define FAR_RTN         0x22
#define BLOCK_386       0x23
#define NEAR_RTN_386    0x24
#define FAR_RTN_386     0x25
#define MEMBER_SCOPE    0x26

#define ADD_PREV_SEG    0x30
#define SET_BASE        0x31
#define SET_BASE_386    0x32

/*
 * types
 */
#define SCALAR          0x10
#define SCOPE           0x11
#define NAME            0x12
#define CUE_TABLE       0x13
#define TYPE_EOF        0x14

#define BYTE_INDEX      0x20
#define WORD_INDEX      0x21
#define LONG_INDEX      0x22
#define TYPE_INDEX      0x23
#define DESC_INDEX      0x24
#define DESC_INDEX_386  0x25

#define BYTE_RANGE      0x30
#define WORD_RANGE      0x31
#define LONG_RANGE      0x32

#define PTR_NEAR            0x40
#define PTR_FAR             0x41
#define PTR_HUGE            0x42
#define PTR_NEAR_DEREF      0x43
#define PTR_FAR_DEREF       0x44
#define PTR_HUGE_DEREF      0x45
#define PTR_NEAR386         0x46
#define PTR_FAR386          0x47
#define PTR_NEAR386_DEREF   0x48
#define PTR_FAR386_DEREF    0x49

#define CLIST           0x50
#define CONST_BYTE      0x51
#define CONST_WORD      0x52
#define CONST_LONG      0x53

#define FLIST           0x60
#define FIELD_BYTE      0x61
#define FIELD_WORD      0x62
#define FIELD_LONG      0x63
#define BIT_BYTE        0x64
#define BIT_WORD        0x65
#define BIT_LONG        0x66
#define FIELD_CLASS     0x67
#define BIT_CLASS       0x68
#define INHERIT_CLASS   0x69

#define PNEAR           0x70
#define PFAR            0x71
#define PNEAR386        0x72
#define PFAR386         0x73
#define EXT_PARMS       0x74

#define CHAR_BYTE       0x80
#define CHAR_WORD       0x81
#define CHAR_LONG       0x82
#define CHAR_IND        0x83
#define CHAR_IND_386    0x84
#define CHAR_LOCATION   0x85

/*
 * Location expression info
 */
#define BP_OFFSET_BYTE          0x10
#define BP_OFFSET_WORD          0x11
#define BP_OFFSET_DWORD         0x12

#define CONST_ADDR286           0x20
#define CONST_ADDR386           0x21
#define CONST_INT_1             0x22
#define CONST_INT_2             0x23
#define CONST_INT_4             0x24

#define IND_REG_CALLOC_NEAR     0x50
#define IND_REG_CALLOC_FAR      0x51
#define IND_REG_RALLOC_NEAR     0x52
#define IND_REG_RALLOC_FAR      0x53

#define OPERATOR_IND_2          0x60
#define OPERATOR_IND_4          0x61
#define OPERATOR_IND_ADDR286    0x62
#define OPERATOR_IND_ADDR386    0x63
#define OPERATOR_ZEB            0x64
#define OPERATOR_ZEW            0x65
#define OPERATOR_MK_FP          0x66
#define OPERATOR_POP            0x67
#define OPERATOR_XCHG           0x68
#define OPERATOR_ADD            0x69
#define OPERATOR_DUP            0x6a
#define OPERATOR_NOP            0x6b

#define REGBYTE_AL              0
#define REGBYTE_AH              1
#define REGBYTE_BL              2
#define REGBYTE_BH              3
#define REGBYTE_CL              4
#define REGBYTE_CH              5
#define REGBYTE_DL              6
#define REGBYTE_DH              7
#define REGBYTE_AX              8
#define REGBYTE_BX              9
#define REGBYTE_CX              10
#define REGBYTE_DX              11
#define REGBYTE_SI              12
#define REGBYTE_DI              13
#define REGBYTE_BP              14
#define REGBYTE_SP              15
#define REGBYTE_CS              16
#define REGBYTE_SS              17
#define REGBYTE_DS              18
#define REGBYTE_ES              19
#define REGBYTE_ST0             20
#define REGBYTE_ST1             21
#define REGBYTE_ST2             22
#define REGBYTE_ST3             23
#define REGBYTE_ST4             24
#define REGBYTE_ST5             25
#define REGBYTE_ST6             26
#define REGBYTE_ST7             27
#define REGBYTE_EAX             28
#define REGBYTE_EBX             29
#define REGBYTE_ECX             30
#define REGBYTE_EDX             31
#define REGBYTE_ESI             32
#define REGBYTE_EDI             33
#define REGBYTE_EBP             34
#define REGBYTE_ESP             35
#define REGBYTE_FS              36
#define REGBYTE_GS              37

#define LANG_HAS_C              0x00000001
#define LANG_HAS_FORTRAN        0x00000002

#define NO_OFFSET       -1L
#define NO_INDEX        0xffff
