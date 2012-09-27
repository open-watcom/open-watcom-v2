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

#include "as.h"

static bool insErrFlag = FALSE;    // to tell whether we had problems or not

#define OE      IF_SETS_OVERFLOW
#define RC      IF_SETS_CC
#define AA      IF_SETS_ABSOLUTE
#define LK      IF_SETS_LINK

#define INS( a, b, c, d, e, f ) { a, b, c, 0, d, e, f, NULL }
// many simpilfied mnemonics use the special field
#define INS2( a, b, c, d, e, f, g ) { a, b, c, d, e, f, g, NULL }

ins_table PPCTable[] = {
    INS( "abs",         31, 360,        IT_UN,          OE | RC,        0 ),
    INS( "add",         31, 266,        IT_BIN,         OE | RC,        0 ),
    INS( "addc",        31, 10,         IT_BIN,         OE | RC,        0 ),
    INS( "adde",        31, 138,        IT_BIN,         OE | RC,        0 ),
    INS( "addi",        14, 0,          IT_BIN_IMM,     0,              0 ),
    INS( "addic",       12, 0,          IT_BIN_IMM,     0,              0 ),
    INS( "addic.",      13, 0,          IT_BIN_IMM,     0,              0 ),
    INS( "addis",       15, 0,          IT_BIN_IMM,     0,              0 ),
    INS( "addme",       31, 234,        IT_UN,          OE | RC,        0 ),
    INS( "addze",       31, 202,        IT_UN,          OE | RC,        0 ),
    INS( "and",         31, 28,         IT_BIN,         RC,             0 ),
    INS( "andc",        31, 60,         IT_BIN,         RC,             0 ),
    INS( "andi.",       28, 0,          IT_BIN_IMM,     0,              0 ),
    INS( "andis.",      29, 0,          IT_BIN_IMM,     0,              0 ),

    INS( "b",           18, 0,          IT_BR,          AA | LK,        0 ),
    INS( "bc",          16, 0,          IT_BR_COND,     AA | LK,        0 ),
    INS( "bcctr",       19, 528,        IT_BR_SPEC,     LK,             0 ),
    INS( "bclr",        19, 16,         IT_BR_SPEC,     LK,             0 ),
    INS( "clcs",        31, 531,        IT_UN,          RC,             0 ),
    INS( "cmp",         31, 0,          IT_CMP,         0,              0 ),
    INS( "cmpi",        11, 0,          IT_CMP_IMM,     0,              0 ),
    INS( "cmpl",        31, 32,         IT_CMP,         0,              0 ),
    INS( "cmpli",       10, 0,          IT_CMP_IMM,     0,              0 ),
    // INS( "cntlzd",   31, 58,         IT_UN,          RC,             0 ),
    INS( "cntlzw",      31, 26,         IT_UN,          RC,             0 ),
    INS( "crand",       19, 257,        IT_CRB_BIN,     0,              0 ),
    INS( "crandc",      19, 129,        IT_CRB_BIN,     0,              0 ),
    INS( "creqv",       19, 289,        IT_CRB_BIN,     0,              0 ),
    INS( "crnand",      19, 225,        IT_CRB_BIN,     0,              0 ),
    INS( "crnor",       19, 33,         IT_CRB_BIN,     0,              0 ),
    INS( "cror",        19, 449,        IT_CRB_BIN,     0,              0 ),
    INS( "crorc",       19, 417,        IT_CRB_BIN,     0,              0 ),
    INS( "crxor",       19, 193,        IT_CRB_BIN,     0,              0 ),
    INS( "dcbf",        31, 86,         IT_DC,          0,              0 ),
    INS( "dcbi",        31, 470,        IT_DC,          0,              0 ),
    INS( "dcbst",       31, 54,         IT_DC,          0,              0 ),
    INS( "dcbt",        31, 278,        IT_DC,          0,              0 ),
    INS( "dcbtst",      31, 246,        IT_DC,          0,              0 ),
    INS( "dcbz",        31, 1014,       IT_DC,          0,              0 ),
    INS( "div",         31, 331,        IT_BIN,         OE | RC,        0 ),
    // INS( "divd",     31, 489,        IT_BIN,         OE | RC,        0 ),
    // INS( "divdu",    31, 457,        IT_BIN,         OE | RC,        0 ),
    INS( "divs",        31, 363,        IT_BIN,         OE | RC,        0 ),
    INS( "divw",        31, 491,        IT_BIN,         OE | RC,        0 ),
    INS( "divwu",       31, 459,        IT_BIN,         OE | RC,        0 ),
    INS( "doz",         31, 264,        IT_BIN,         OE | RC,        0 ),
    INS( "dozi",        9, 0,           IT_BIN_IMM,     0,              0 ),

    INS( "eciwx",       31, 310,        IT_BIN,         0,              0 ),
    INS( "ecowx",       31, 438,        IT_BIN,         0,              0 ),
    INS( "eieio",       31, 854,        IT_EIEIO,       0,              OE ),
    INS( "eqv",         31, 284,        IT_BIN2,        RC,             0 ),
    INS( "extsb",       31, 954,        IT_UN2,         RC,             OE ),
    INS( "extsh",       31, 922,        IT_UN2,         RC,             OE ),
    // INS( "extsw",    31, 986,        IT_BIN,         RC,             OE ),
    INS( "fabs",        63, 264,        IT_FP_UN,       RC,             0 ),
    INS( "fadd",        63, 21,         IT_FP_BIN,      RC,             0 ),
    INS( "fadds",       59, 21,         IT_FP_BIN,      RC,             0 ),
    // INS( "fcfid",    63, 846,        IT_FCTID,       RC,             0 ),
    INS( "fcmpo",       63, 32,         IT_FP_CMP,      0,              0 ),
    INS( "fcmpu",       63, 0,          IT_FP_CMP,      0,              0 ),
    // INS( "fctid",    63, 814,        IT_FCTID,       RC,             0 ),
    // INS( "fctidz",   63, 815,        IT_FCTID,       RC,             0 ),
    INS( "fctiw",       63, 14,         IT_FP_UN,       RC,             0 ),
    INS( "fctiwz",      63, 15,         IT_FP_UN,       RC,             0 ),
    INS( "fdiv",        63, 18,         IT_FP_BIN,      RC,             0 ),
    INS( "fdivs",       59, 18,         IT_FP_BIN,      RC,             0 ),
    INS( "fmadd",       63, 29,         IT_FP_MULADD,   RC,             0 ),
    INS( "fmadds",      59, 29,         IT_FP_MULADD,   RC,             0 ),
    INS( "fmr",         63, 72,         IT_FP_UN,       RC,             0 ),
    INS( "fmsub",       63, 28,         IT_FP_MULADD,   RC,             0 ),
    INS( "fmsubs",      59, 28,         IT_FP_MULADD,   RC,             0 ),
    INS( "fmul",        63, 25,         IT_FP_MUL,      RC,             0 ),
    INS( "fmuls",       59, 25,         IT_FP_MUL,      RC,             0 ),
    INS( "fnabs",       63, 136,        IT_FP_UN,       RC,             0 ),
    INS( "fneg",        63, 40,         IT_FP_UN,       RC,             0 ),
    INS( "fnmadd",      63, 31,         IT_FP_MULADD,   RC,             0 ),
    INS( "fnmadds",     59, 31,         IT_FP_MULADD,   RC,             0 ),
    INS( "fnmsub",      63, 30,         IT_FP_MULADD,   RC,             0 ),
    INS( "fnmsubs",     59, 30,         IT_FP_MULADD,   RC,             0 ),
    // INS( "fres",     59, 24,         IT_FCTID,       RC,             0 ),

    INS( "frsp",        63, 12,         IT_FP_UN,       RC,             0 ),
    // INS( "frsqrte",  63, 26,         IT_FCTID,       RC,             0 ),
    // INS( "fsel",     63, 23,         ????????,       RC,             0 ),
    // INS( "fsqrt",    63, 22,         IT_FCTID,       RC,             0 ),
    // INS( "fsqrts",   59, 22,         IT_FCTID,       RC,             0 ),
    INS( "fsub",        63, 20,         IT_FP_BIN,      RC,             0 ),
    INS( "fsubs",       59, 20,         IT_FP_BIN,      RC,             0 ),
    INS( "icbi",        31, 982,        IT_DC,          0,              0 ),
    INS( "isync",       19, 150,        IT_EIEIO,       0,              0 ),
    INS( "lbz",         34, 0,          IT_LS,          0,              0 ),
    INS( "lbzu",        35, 0,          IT_LS,          0,              0 ),
    INS( "lbzux",       31, 119,        IT_LS_INDEX,    0,              0 ),
    INS( "lbzx",        31, 87,         IT_LS_INDEX,    0,              0 ),
    // INS( "ld",       58, 0,          IT_LS,          0,              0 ),
    // INS( "ldarx",    31, 84,         IT_LS_INDEX,    0,              0 ),
    // INS( "ldu",      58, 0,          IT_LS,          0,              0 ),
    // INS( "ldux",     31, 53,         IT_LS_INDEX,    0,              0 ),
    // INS( "ldx",      31, 21,         IT_LS_INDEX,    0,              0 ),
    INS( "lfd",         50, 0,          IT_LS_FP,       0,              0 ),
    INS( "lfdu",        51, 0,          IT_LS_FP,       0,              0 ),
    INS( "lfdux",       31, 631,        IT_LS_INDEX_FP, 0,              0 ),
    INS( "lfdx",        31, 599,        IT_LS_INDEX_FP, 0,              0 ),
    INS( "lfs",         48, 0,          IT_LS_FP,       0,              0 ),
    INS( "lfsu",        49, 0,          IT_LS_FP,       0,              0 ),
    INS( "lfsux",       31, 567,        IT_LS_INDEX_FP, 0,              0 ),
    INS( "lfsx",        31, 535,        IT_LS_INDEX_FP, 0,              0 ),
    INS( "lha",         42, 0,          IT_LS,          0,              0 ),
    INS( "lhau",        43, 0,          IT_LS,          0,              0 ),
    INS( "lhaux",       31, 375,        IT_LS_INDEX,    0,              0 ),
    INS( "lhax",        31, 343,        IT_LS_INDEX,    0,              0 ),
    INS( "lhbrx",       31, 790,        IT_LS_INDEX,    0,              0 ),
    INS( "lhz",         40, 0,          IT_LS,          0,              0 ),
    INS( "lhzu",        41, 0,          IT_LS,          0,              0 ),
    INS( "lhzux",       31, 311,        IT_LS_INDEX,    0,              0 ),

    INS( "lhzx",        31, 279,        IT_LS_INDEX,    0,              0 ),
    INS( "lmw",         46, 0,          IT_LS,          0,              0 ),
    INS( "lscbx",       31, 277,        IT_LS_INDEX,    RC,             0 ),
    INS( "lswi",        31, 597,        IT_LSWI,        0,              0 ),
    INS( "lswx",        31, 533,        IT_LS_INDEX,    0,              0 ),
    // INS( "lwa",              58, 0,          IT_LS,          0,              0 ),
    INS( "lwarx",       31, 20,         IT_LS_INDEX,    0,              0 ),
    // INS( "lwaux",    31, 373,        IT_LS_INDEX,    0,              0 ),
    // INS( "lwax",     31, 341,        IT_LS_INDEX,    0,              0 ),
    INS( "lwbrx",       31, 534,        IT_LS_INDEX,    0,              0 ),
    INS( "lwz",         32, 0,          IT_LS,          0,              0 ),
    INS( "lwzu",        33, 0,          IT_LS,          0,              0 ),
    INS( "lwzux",       31, 55,         IT_LS_INDEX,    0,              0 ),
    INS( "lwzx",        31, 23,         IT_LS_INDEX,    0,              0 ),
    INS( "maskg",       31, 29,         IT_BIN2,        RC,             0 ),
    INS( "maskir",      31, 541,        IT_BIN2,        RC,             0 ),
    INS( "mcrf",        19, 0,          IT_CRF_UN,      0,              0 ),
    INS( "mcrfs",       63, 64,         IT_CRF_UN,      0,              0 ),
    INS( "mcrxr",       31, 512,        IT_CRF_MCRXR,   0,              0 ),
    INS( "mfcr",        31, 19,         IT_MFCRMSR,     0,              0 ),
    INS( "mffs",        63, 583,        IT_MFFS,        RC,             0 ),
    INS( "mfmsr",       31, 83,         IT_MFCRMSR,     0,              0 ),
    INS( "mfspr",       31, 339,        IT_MFSPR,       0,              0 ),
    INS( "mfsr",        31, 595,        IT_MFSR,        0,              0 ),
    INS( "mfsrin",      31, 659,        IT_MFSRIN,      0,              0 ),
    // INS( "mftb",     31, 371,        IT_MFTB,        0,              0 ),
    INS( "mtcrf",       31, 144,        IT_MTCRF,       0,              0 ),
    INS( "mtfsb0",      63, 70,         IT_MTFSB,       RC,             0 ),
    INS( "mtfsb1",      63, 38,         IT_MTFSB,       RC,             0 ),
    INS( "mtfsf",       31, 711,        IT_MTFSF,       RC,             0 ),
    INS( "mtfsfi",      63, 134,        IT_MTFSF_IMM,   RC,             0 ),
    INS( "mtmsr",       31, 146,        IT_MFCRMSR,     0,              0 ),
    INS( "mtspr",       31, 467,        IT_MTSPR,       0,              0 ),
    INS( "mtsr",        31, 210,        IT_MTSR,        0,              0 ),
    INS( "mtsrin",      31, 242,        IT_MFSRIN,      0,              0 ),
    INS( "mul",         31, 107,        IT_BIN,         OE | RC,        0 ),
    // INS( "mulhd",    31, 73,         IT_BIN,         RC,             0 ),
    // INS( "mulhdu",   31, 9,          IT_BIN,         RC,             0 ),
    INS( "mulhw",       31, 75,         IT_BIN,         RC,             0 ),
    INS( "mulhwu",      31, 11,         IT_BIN,         RC,             0 ),
    // INS( "mulld",    31, 233,        IT_BIN,         OE | RC,        0 ),
    INS( "mullw",       31, 235,        IT_BIN,         OE | RC,        0 ),
    INS( "mulli",       7, 0,           IT_BIN_IMM,     0,              0 ),
    INS( "nabs",        31, 488,        IT_UN,          OE | RC,        0 ),
    INS( "nand",        31, 476,        IT_BIN2,        RC,             0 ),
    INS( "neg",         31, 104,        IT_UN,          OE | RC,        0 ),
    INS( "nor",         31, 124,        IT_BIN2,        RC,             0 ),
    INS( "or",          31, 444,        IT_BIN2,        RC,             0 ),
    INS( "orc",         31, 412,        IT_BIN2,        RC,             0 ),
    INS( "ori",         24, 0,          IT_BIN_IMM2,    0,              0 ),
    INS( "oris",        25, 0,          IT_BIN_IMM2,    0,              0 ),
    INS( "rfi",         19, 50,         IT_EIEIO,       0,              0 ),
    // INS( "rldcl",    30, 8,          IT_NYI,         0,              RC ),
    // INS( "rldcr",    30, 9,          IT_NYI,         0,              RC ),
    // INS( "rldic",    30, 2,          IT_NYI,         0,              RC ),
    // INS( "rldicl",   30, 0,          IT_NYI,         0,              RC ),
    // INS( "rldicr",   30, 1,          IT_NYI,         0,              RC ),
    // INS( "rldimi",   30, 3,          IT_NYI,         0,              RC ),
    INS( "rlmi",        22, 0,          IT_SHIFT_INDEX, RC,             0 ),
    INS( "rlwimi",      20, 0,          IT_SHIFT_IMM,   RC,             0 ),
    INS( "rlwinm",      21, 0,          IT_SHIFT_IMM,   RC,             0 ),
    INS( "rlwnm",       23, 0,          IT_SHIFT_INDEX, RC,             0 ),
    INS( "rrib",        31, 537,        IT_BIN2,        RC,             OE ),
    INS( "sc",          17, 1,          IT_EIEIO,       0,              0 ),
    // INS( "sld",              31, 27,         IT_BIN,         RC,             0 ),
    INS( "sle",         31, 153,        IT_BIN2,        RC,             0 ),
    INS( "sleq",        31, 217,        IT_BIN2,        RC,             0 ),
    INS( "sliq",        31, 184,        IT_SHIFT,       RC,             0 ),
    INS( "slliq",       31, 248,        IT_SHIFT,       RC,             0 ),
    INS( "sllq",        31, 216,        IT_BIN2,        RC,             0 ),
    INS( "slq",         31, 152,        IT_BIN2,        RC,             0 ),
    INS( "slw",         31, 24,         IT_BIN2,        RC,             0 ),
    INS( "srad",        31, 794,        IT_BIN,         RC,             OE ),
    // INS( "sradi",    31, 413,        IT_NYI,         RC,             0 ),
    INS( "sraq",        31, 920,        IT_BIN2,        RC,             OE ),
    INS( "sraiq",       31, 952,        IT_SHIFT,       RC,             OE ),
    INS( "sraw",        31, 792,        IT_BIN2,        RC,             OE ),
    INS( "srawi",       31, 824,        IT_SHIFT,       RC,             OE ),
    INS( "srd",         31, 539,        IT_BIN,         RC,             OE ),
    INS( "sre",         31, 665,        IT_BIN2,        RC,             OE ),
    INS( "srea",        31, 921,        IT_BIN2,        RC,             OE ),
    INS( "sreq",        31, 729,        IT_BIN2,        RC,             OE ),
    // INS( "srd",              31, 539,        IT_BIN,         RC,             OE ),
    INS( "sriq",        31, 696,        IT_SHIFT,       RC,             OE ),
    INS( "srliq",       31, 760,        IT_SHIFT,       RC,             OE ),
    INS( "srlq",        31, 728,        IT_BIN2,        RC,             OE ),
    INS( "srq",         31, 664,        IT_BIN2,        RC,             OE ),
    INS( "srw",         31, 536,        IT_BIN2,        RC,             OE ),
    INS( "stb",         38, 0,          IT_LS,          0,              0 ),
    INS( "stbu",        39, 0,          IT_LS,          0,              0 ),
    INS( "stbux",       31, 247,        IT_LS_INDEX,    0,              0 ),
    INS( "stbx",        31, 215,        IT_LS_INDEX,    0,              0 ),
    // INS( "std",              62, 0,          IT_NYI,         0,              0 ),
    // INS( "stdcx.",   31, 214,        IT_BIN,         0,              RC ),
    // INS( "stdu",     62, 0,          IT_NYI,         0,              RC ),
    // INS( "stdux",    31, 181,        IT_BIN,         0,              0 ),

    // INS( "stdx",     31, 149,        IT_BIN,         0,              0 ),
    INS( "stfd",        54, 0,          IT_LS_FP,       0,              0 ),
    INS( "stfdu",       55, 0,          IT_LS_FP,       0,              0 ),
    INS( "stfdux",      31, 759,        IT_LS_INDEX_FP, 0,              0 ),
    INS( "stfdx",       31, 727,        IT_LS_INDEX_FP, 0,              0 ),
    // INS( "stfiwax",  31, 983,        IT_LS_INDEX_FP, 0,              0 ),
    INS( "stfs",        52, 0,          IT_LS_FP,       0,              0 ),
    INS( "stfsu",       53, 0,          IT_LS_FP,       0,              0 ),
    INS( "stfsux",      31, 695,        IT_LS_INDEX_FP, 0,              0 ),
    INS( "stfsx",       31, 663,        IT_LS_INDEX_FP, 0,              0 ),
    INS( "sth",         44, 0,          IT_LS,          0,              0 ),
    INS( "sthbrx",      31, 918,        IT_LS_INDEX,    0,              0 ),
    INS( "sthu",        45, 0,          IT_LS,          0,              0 ),
    INS( "sthux",       31, 439,        IT_LS_INDEX,    0,              0 ),
    INS( "sthx",        31, 407,        IT_LS_INDEX,    0,              0 ),
    INS( "stmw",        47, 0,          IT_LS,          0,              0 ),
    INS( "stswi",       31, 725,        IT_LSWI,        0,              0 ),
    INS( "stswx",       31, 661,        IT_LS_INDEX,    0,              0 ),
    INS( "stw",         36, 0,          IT_LS,          0,              0 ),
    INS( "stwbrx",      31, 662,        IT_LS_INDEX,    0,              0 ),
    INS( "stwcx.",      31, 150,        IT_LS_INDEX,    0,              RC ),
    INS( "stwu",        37, 0,          IT_LS,          0,              0 ),
    INS( "stwux",       31, 183,        IT_LS_INDEX,    0,              0 ),
    INS( "stwx",        31, 151,        IT_LS_INDEX,    0,              0 ),
    INS( "subf",        31, 40,         IT_BIN,         OE | RC,        0 ),
    INS( "subfc",       31, 8,          IT_BIN,         OE | RC,        0 ),
    INS( "subfe",       31, 136,        IT_BIN,         OE | RC,        0 ),
    INS( "subfic",      8, 0,           IT_BIN_IMM,     0,              0 ),
    INS( "subfme",      31, 232,        IT_UN,          OE | RC,        0 ),
    INS( "subfze",      31, 200,        IT_UN,          OE | RC,        0 ),
    INS( "sync",        31, 598,        IT_EIEIO,       0,              0 ),
    // INS( "td",               31, 68,         IT_NYI,         0,              0 ),
    // INS( "tdi",              02, 0,          IT_NYI,         0,              0 ),

    // INS( "tlbia",    31, 0,          IT_NYI,         0,              0 ),
    INS( "tlbie",       31, 306,        IT_TLBIE,       0,              0 ),
    // INS( "tlbiex",   31, 0,          IT_NYI,         0,              0 ),
    // INS( "tlbsync",  31, 566,        IT_NYI,         0,              0 ),
    INS( "tw",          31, 4,          IT_TRAP,        0,              0 ),
    INS( "twi",         3, 0,           IT_TRAP_IMM,    0,              0 ),
    INS( "xor",         31, 316,        IT_BIN2,        RC,             0 ),
    INS( "xori",        26, 0,          IT_BIN_IMM2,    0,              0 ),
    INS( "xoris",       27, 0,          IT_BIN_IMM2,    0,              0 ),

    // Simplified mnemonics:
    INS( "li",          14, 0,          IT_SM_LI,       0,              0 ),
    INS( "lis",         15, 0,          IT_SM_LI,       0,              0 ),
    INS( "la",          14, 0,          IT_SM_LA,       0,              0 ),
    INS( "subi",        14, 0,          IT_SM_SUBI,     0,              0 ),
    INS( "mr",          31, 444,        IT_SM_MR,       RC,             0 ),
    INS( "not",         31, 124,        IT_SM_MR,       RC,             0 ),
    INS( "cmpwi",       11, 0,          IT_SM_CMPWI,    0,              0 ),
    INS( "cmpw",        31, 0,          IT_SM_CMPW,     0,              0 ),
    INS( "cmplwi",      10, 0,          IT_SM_CMPWI,    0,              0 ),
    INS( "cmplw",       31, 32,         IT_SM_CMPW,     0,              0 ),
    INS( "extlwi",      21, 0,          IT_SM_EXTLWI,   RC,             0 ),
    INS2( "srwi",       21, 0,  SRWI,   IT_SM_SRWI,     RC,             0 ),
    INS2( "clrrwi",     21, 0,  CLRRWI, IT_SM_SRWI,     RC,             0 ),
    INS( "rotlw",       23, 0,          IT_SM_ROTLW,    RC,             0 ),
    INS( "inslw",       20, 0,          IT_SM_INSLW,    RC,             0 ),
    INS( "trap",        31, 4,          IT_SM_TRAP_ANY, 0,              0 ),
    INS2( "twlt",       31, 4,  TO_LT,  IT_SM_TRAP,     0,              0 ),
    INS2( "twle",       31, 4,  TO_LE,  IT_SM_TRAP,     0,              0 ),
    INS2( "tweq",       31, 4,  TO_EQ,  IT_SM_TRAP,     0,              0 ),
    INS2( "twge",       31, 4,  TO_GE,  IT_SM_TRAP,     0,              0 ),
    INS2( "twgt",       31, 4,  TO_GT,  IT_SM_TRAP,     0,              0 ),
    INS2( "twnl",       31, 4,  TO_NL,  IT_SM_TRAP,     0,              0 ),
    INS2( "twne",       31, 4,  TO_NE,  IT_SM_TRAP,     0,              0 ),
    INS2( "twllt",      31, 4,  TO_LLT, IT_SM_TRAP,     0,              0 ),
    INS2( "twlle",      31, 4,  TO_LLE, IT_SM_TRAP,     0,              0 ),
    INS2( "twlge",      31, 4,  TO_LGE, IT_SM_TRAP,     0,              0 ),
    INS2( "twlgt",      31, 4,  TO_LGT, IT_SM_TRAP,     0,              0 ),
    INS2( "twlnl",      31, 4,  TO_LNL, IT_SM_TRAP,     0,              0 ),
    INS2( "twlti",      3, 0,   TO_LT,  IT_SM_TRAPI,    0,              0 ),
    INS2( "twlei",      3, 0,   TO_LE,  IT_SM_TRAPI,    0,              0 ),
    INS2( "tweqi",      3, 0,   TO_EQ,  IT_SM_TRAPI,    0,              0 ),
    INS2( "twgei",      3, 0,   TO_GE,  IT_SM_TRAPI,    0,              0 ),
    INS2( "twgti",      3, 0,   TO_GT,  IT_SM_TRAPI,    0,              0 ),
    INS2( "twnli",      3, 0,   TO_NL,  IT_SM_TRAPI,    0,              0 ),
    INS2( "twnei",      3, 0,   TO_NE,  IT_SM_TRAPI,    0,              0 ),
    INS2( "twllti",     3, 0,   TO_LLT, IT_SM_TRAPI,    0,              0 ),
    INS2( "twllei",     3, 0,   TO_LLE, IT_SM_TRAPI,    0,              0 ),
    INS2( "twlgei",     3, 0,   TO_LGE, IT_SM_TRAPI,    0,              0 ),
    INS2( "twlgti",     3, 0,   TO_LGT, IT_SM_TRAPI,    0,              0 ),
    INS2( "twlnli",     3, 0,   TO_LNL, IT_SM_TRAPI,    0,              0 ),
    INS2( "mtxer",      31, 467,XER,    IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mtlr",       31, 467,LR,     IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mtctr",      31, 467,CTR,    IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mtdsisr",    31, 467,DSISR,  IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mtdar",      31, 467,DAR,    IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mtdec",      31, 467,DEC,    IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mtsdr1",     31, 467,SDR1,   IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mtsrr0",     31, 467,SRR0,   IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mtsrr1",     31, 467,SRR1,   IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mtsprg",     31, 467,SPRG,   IT_SM_MTSPRN,   0,              0 ),
    INS2( "mtear",      31, 467,EAR,    IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mtibatu",    31, 467,BATU,   IT_SM_MTSPRN,   0,              0 ),
    INS2( "mtibatl",    31, 467,BATL,   IT_SM_MTSPRN,   0,              0 ),
    INS2( "mfxer",      31, 339,XER,    IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mflr",       31, 339,LR,     IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mfctr",      31, 339,CTR,    IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mfdsisr",    31, 339,DSISR,  IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mfdar",      31, 339,DAR,    IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mfdec",      31, 339,DEC,    IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mfsdr1",     31, 339,SDR1,   IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mfsrr0",     31, 339,SRR0,   IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mfsrr1",     31, 339,SRR1,   IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mfsprg",     31, 339,SPRG,   IT_SM_MFSPRN,   0,              0 ),
    INS2( "mfear",      31, 339,EAR,    IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mfpvr",      31, 339,PVR,    IT_SM_MOV_SPR,  0,              0 ),
    INS2( "mfibatu",    31, 339,BATU,   IT_SM_MFSPRN,   0,              0 ),
    INS2( "mfibatl",    31, 339,BATL,   IT_SM_MFSPRN,   0,              0 ),
    INS2( "bt",         16, 0,  B_T,            IT_SM_B_C,      AA | LK,    0 ),
    INS2( "bf",         16, 0,  B_F,            IT_SM_B_C,      AA | LK,    0 ),
    INS2( "bdnz",       16, 0,  B_DNZ,          IT_SM_B_C,      AA | LK,    0 ),
    INS2( "bdnzt",      16, 0,  B_DNZT,         IT_SM_B_C,      AA | LK,    0 ),
    INS2( "bdnzf",      16, 0,  B_DNZF,         IT_SM_B_C,      AA | LK,    0 ),
    INS2( "bdz",        16, 0,  B_DZ,           IT_SM_B_C,      AA | LK,    0 ),
    INS2( "bdzt",       16, 0,  B_DZT,          IT_SM_B_C,      AA | LK,    0 ),
    INS2( "bdzf",       16, 0,  B_DZF,          IT_SM_B_C,      AA | LK,    0 ),
    INS2( "blr",        19, 16, B_UNCOND,       IT_SM_B_SP,     LK,         0 ),
    INS2( "btlr",       19, 16, B_T,            IT_SM_B_SP,     LK,         0 ),
    INS2( "bflr",       19, 16, B_F,            IT_SM_B_SP,     LK,         0 ),
    INS2( "bdnzlr",     19, 16, B_DNZ,          IT_SM_B_SP,     LK,         0 ),
    INS2( "bdnztlr",    19, 16, B_DNZT,         IT_SM_B_SP,     LK,         0 ),
    INS2( "bdnzflr",    19, 16, B_DNZF,         IT_SM_B_SP,     LK,         0 ),
    INS2( "bdzlr",      19, 16, B_DZ,           IT_SM_B_SP,     LK,         0 ),
    INS2( "bdztlr",     19, 16, B_DZT,          IT_SM_B_SP,     LK,         0 ),
    INS2( "bdzflr",     19, 16, B_DZF,          IT_SM_B_SP,     LK,         0 ),
    INS2( "bctr",       19, 528,B_UNCOND,       IT_SM_B_SP,     LK,         0 ),
    INS2( "btctr",      19, 528,B_T,            IT_SM_B_SP,     LK,         0 ),
    INS2( "bfctr",      19, 528,B_F,            IT_SM_B_SP,     LK,         0 ),
    INS2( "blt",        16, 0,  BICC_LT,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "ble",        16, 0,  BICC_NG,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "beq",        16, 0,  BICC_EQ,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "bge",        16, 0,  BICC_NL,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "bgt",        16, 0,  BICC_GT,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "bnl",        16, 0,  BICC_NL,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "bne",        16, 0,  BICC_NE,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "bng",        16, 0,  BICC_NG,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "bso",        16, 0,  BICC_SO,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "bns",        16, 0,  BICC_NS,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "bun",        16, 0,  BICC_UN,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "bnu",        16, 0,  BICC_NU,        IT_SM_B_C_ICC,  AA | LK,    0 ),
    INS2( "bltlr",      19, 16, BICC_LT,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "blelr",      19, 16, BICC_NG,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "beqlr",      19, 16, BICC_EQ,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bgelr",      19, 16, BICC_NL,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bgtlr",      19, 16, BICC_GT,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bnllr",      19, 16, BICC_NL,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bnelr",      19, 16, BICC_NE,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bnglr",      19, 16, BICC_NG,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bsolr",      19, 16, BICC_SO,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bnslr",      19, 16, BICC_NS,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bunlr",      19, 16, BICC_UN,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bnulr",      19, 16, BICC_NU,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bltctr",     19, 528,BICC_LT,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "blectr",     19, 528,BICC_NG,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "beqctr",     19, 528,BICC_EQ,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bgectr",     19, 528,BICC_NL,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bgtctr",     19, 528,BICC_GT,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bnlctr",     19, 528,BICC_NL,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bnectr",     19, 528,BICC_NE,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bngctr",     19, 528,BICC_NG,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bsoctr",     19, 528,BICC_SO,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bnsctr",     19, 528,BICC_NS,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bunctr",     19, 528,BICC_UN,        IT_SM_B_SP_ICC, LK,         0 ),
    INS2( "bnuctr",     19, 528,BICC_NU,        IT_SM_B_SP_ICC, LK,         0 ),
};

#define MAX_NAME_LEN    20      /* maximum length of a PPC instruction mnemonic */

static void addInstructionSymbol( ins_flags flags, ins_table *table_entry ) {
//***************************************************************************
// Given an instruction name for which the optional bits in flags
// are turned on, add a symbol for it to the symbol table.

    sym_handle  sym;
    ins_symbol  *entry;
    char        buffer[ MAX_NAME_LEN ];

    strcpy( buffer, table_entry->name );
    if( flags & OE ) strcat( buffer, "o" );
    if( flags & RC ) strcat( buffer, "." );

    if( flags & LK ) strcat( buffer, "l" );
    if( flags & AA ) strcat( buffer, "a" );

    entry = MemAlloc( sizeof( ins_symbol ) );
    entry->table_entry = table_entry;
    entry->flags = flags;

    // link it into our list of symbols for this table entry
    entry->next = table_entry->symbols;
    table_entry->symbols = entry;

    sym = SymAdd( buffer, SYM_INSTRUCTION );
    SymSetLink( sym, (void *)entry );
}

static void enumBits( uint_32 mask, uint_32 remaining, void (*func)( ins_flags, ins_table * ), void *parm ) {
//******************************************************************************************************

    uint_32     low_bit;

    if( remaining == 0 ) {
        func( mask, parm );
        return;
    }
    low_bit = remaining & -remaining;
    remaining ^= low_bit;
    enumBits( mask,             remaining, func, parm );
    enumBits( mask | low_bit,   remaining, func, parm );
}

static void bitSetCover( uint_32 subset, void (*func)( ins_flags set, ins_table *parm ), void *parm ) {
//************************************************************************************************
// This is a little different - when this routine is called with a set of bits, it guarantees
// that the function passed in will be called exactly once for each subset of those bits,
// including 0 and the bit-set itself. It is recursive and can recurse up to n-levels deep,
// where n is the number of bits on in the subset.

    enumBits( 0, subset, func, parm );
}

#ifdef _STANDALONE_
#ifdef AS_DEBUG_DUMP
static char *itStrings[] = {
#define PICK( a, b, c, d, e, f, g ) #a,
#include "ppcfmt.inc"
#undef PICK
};

extern void DumpITString( ins_template template ) {
    printf( itStrings[ template ] );
}

extern void DumpInsFlags( ins_flags flags ) {
//*******************************************

    if( flags & IF_SETS_OVERFLOW ) printf( "OVERFLOW " );
    if( flags & IF_SETS_CC ) printf( "SETS_CC " );
}

extern void DumpInsTableEntry( ins_table *table_entry ) {
//*******************************************************

    ins_symbol  *symbol;

    printf( "%s: %d(%d) ", table_entry->name, table_entry->primary, table_entry->secondary );
    DumpITString( table_entry->template );
    printf( ", " );
    DumpInsFlags( table_entry->optional );
    printf( ", " );
    DumpInsFlags( table_entry->required );
    printf( "\n\tSymbol entries: " );
    symbol = table_entry->symbols;
    while( symbol != NULL ) {
        printf( " %x", symbol );
        symbol = symbol->next;
    }
    printf( "\n" );
}

extern void DumpInsTables() {
//***************************

    ins_table   *curr;
    int         i, n;

    n = sizeof( PPCTable ) / sizeof( PPCTable[0] );
    for( i = 0; i < n; i++ ) {
        curr = &PPCTable[ i ];
        DumpInsTableEntry( curr );
    }
}
#endif
#endif

extern void InsInit() {
//*********************

    ins_table   *curr;
    int         i, n;

    n = sizeof( PPCTable ) / sizeof( PPCTable[0] );
    for( i = 0; i < n; i++ ) {
        curr = &PPCTable[ i ];
        /* for each possible symbol generated by this instruction, add a symbol table entry */
        bitSetCover( curr->optional, addInstructionSymbol, curr );
    }
#ifdef AS_DEBUG_DUMP
    #ifdef _STANDALONE_
    if( _IsOption( DUMP_INS_TABLE ) ) DumpInsTables();
    #endif
#endif
}

extern instruction *InsCreate( sym_handle op_sym ) {
//**************************************************
// Allocate an instruction and initialize it.

    instruction *ins;

    ins = MemAlloc( sizeof( instruction ) );
    ins->opcode_sym = op_sym;
    ins->format = SymGetLink( op_sym );
    ins->num_operands = 0;
    return( ins );
}

extern void InsAddOperand( instruction *ins, ins_operand *op ) {
//**********************************************************
// Add an operand to the given instruction.

    if( ins->num_operands == MAX_OPERANDS ) {
        if( !insErrFlag ) {
            Error( MAX_NUMOP_EXCEEDED );
            insErrFlag = TRUE;
        }
        MemFree( op );
        return;
    }
    if( insErrFlag) insErrFlag = FALSE;
    ins->operands[ ins->num_operands++ ] = op;
}

extern void InsEmit( instruction *ins ) {
//***************************************
// Check an instruction to make sure operands match
// and encode it. The encoded instruction is emitted
// to the current OWL section.

#ifdef AS_DEBUG_DUMP
    #ifdef _STANDALONE_
    if( _IsOption( DUMP_INSTRUCTIONS ) ) {
        DumpIns( ins );
    }
    #endif
#endif
    if( insErrFlag == FALSE && PPCValidate( ins ) ) {
        #ifdef _STANDALONE_
        PPCEmit( CurrentSection, ins );
        #else
        PPCEmit( ins );
        #endif
    }
}

extern void InsDestroy( instruction *ins ) {
//******************************************
// Free up an instruction and all operands which
// are hanging off of it.

    int         i;

    for( i = 0; i < ins->num_operands; i++ ) {
        MemFree( ins->operands[ i ] );
    }
    MemFree( ins );
}

extern void InsFini() {
//*********************

    ins_table   *curr;
    ins_symbol  *next;
    ins_symbol  *entry;
    int         i, n;
    extern instruction *AsCurrIns; // from as.y

    if( AsCurrIns != NULL ) {
        InsDestroy( AsCurrIns );
        AsCurrIns = NULL;
    }
    n = sizeof( PPCTable ) / sizeof( PPCTable[0] );
    for( i = 0; i < n; i++ ) {
        curr = &PPCTable[ i ];
        for( entry = curr->symbols; entry != NULL; entry = next ) {
            next = entry->next;
            MemFree( entry );
        }
        curr->symbols = NULL;   // need to reset this pointer
    }
}

#ifdef _STANDALONE_
#ifdef AS_DEBUG_DUMP
extern void DumpIns( instruction *ins ) {
//***************************************

    int         i;

    printf( "%-11s", SymName( ins->opcode_sym ) );
    for( i = 0; i < ins->num_operands; i++ ) {
        if( i != 0 ) printf( ", " );
        DumpOperand( ins->operands[ i ] );
    }
    printf( "\n" );
}
#endif
#endif
