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
* Description:  compiler command line options related definition
*
****************************************************************************/


// Options fall in 2 categories.
// 1. bit options (those that take a bit)
// 2. other options (those that take a value or cause a side effect)

    #define OPT_XLINE           1
    #define OPT_DEFINE          2
    #define CGOPT_DATA_THRESH   3
    #define CGOPT_OBJ_NAME      4
    #define OPT_INCPATH         8

typedef unsigned_32     comp_options;

    #define OPT_PRINT           0x00000001L
    #define OPT_TYPE            0x00000002L
    #define OPT_LIST            0x00000004L
    #define OPT_ERRFILE         0x00000008L
    #define OPT_INCLIST         0x00000010L
    #define OPT_EXT             0x00000020L
    #define OPT_WARN            0x00000040L
    #define OPT_TERM            0x00000080L
    #define OPT_EXPLICIT        0x00000100L
    #define OPT_REFERENCE       0x00000200L
    #define OPT_SHORT           0x00000400L
    #define OPT_LF_WITH_FF      0x00000800L
    #define OPT_EXTEND_FORMAT   0x00001000L
    #define OPT_WILD            0x00002000L
    #define OPT_CHINESE         0x00004000L
    #define OPT_JAPANESE        0x00008000L
    #define OPT_KOREAN          0x00010000L
    #define OPT_RESOURCES       0x00040000L
    #define OPT_UNIT_6_CC       0x00080000L
    #define OPT_QUIET           0x00100000L
    #define OPT_BROWSE          0x00200000L
    #define OPT_EXTEND_REAL     0x00400000L
    #define OPT_DEPENDENCY      0x00020000L
    #define OPT_COMMA_SEP       0x00800000L
    #define OPT_PROMOTE         0x01000000L
    #define OPT_TRACE           0x02000000L
    #define OPT_BOUNDS          0x04000000L
    #define OPT_DEBUG           (OPT_TRACE | OPT_BOUNDS)
    #define OPT_DESCRIPTOR      0x08000000L
    #define OPT_AUTOMATIC       0x10000000L
    #define OPT_SAVE            0x20000000L
    #define OPT_DFLT_LIB        0x40000000L
    #define OPT_SYNTAX          0x80000000L


    #define     CGOPT_DB_LINE       0x00000001L
    #define     CGOPT_DB_LOCALS     0x00000002L

    #define     CGOPT_STACK_CHK     0x00000004L
#if ( _CPU == 8086 || _CPU == 386 )
    #define     CGOPT_CONST_CODE    0x00000008L
    #define     CGOPT_STK_ARGS      0x00000010L
    #define     CGOPT_SEG_REGS      0x00000020L
    #define     CGOPT_FS_FLOATS     0x00000040L
    #define     CGOPT_GS_FLOATS     0x00000080L
#if _CPU == 8086
    #define     CGOPT_SS_FLOATS     0x00000100L
#endif

    #define     CGOPT_M_LARGE       0x00000200L
    #define     CGOPT_M_MEDIUM      0x00000400L
#if _CPU == 8086
    #define     CGOPT_M_HUGE        0x00000800L
#else
    #define     CGOPT_M_FLAT        0x00001000L
    #define     CGOPT_M_SMALL       0x00002000L
    #define     CGOPT_M_COMPACT     0x00004000L

    #define     CGOPT_EZ_OMF        0x00008000L
    #define     CGOPT_BD            0x00010000L
    #define     CGOPT_BM            0x00020000L
    #define     CGOPT_STACK_GROW    0x00040000L
#endif
#elif _CPU == _AXP || _CPU == _PPC
    #define     CGOPT_BD            0x00010000L
    #define     CGOPT_BM            0x00020000L
    #define     CGOPT_STACK_GROW    0x00040000L
#endif
    #define     CGOPT_BW            0x00080000L
    #define     CGOPT_WINDOWS       0x00100000L
    #define     CGOPT_ALIGN         0x00200000L
    #define     CGOPT_DI_CV         0x00400000L
    #define     CGOPT_DI_DWARF      0x00800000L
    #define     CGOPT_DI_WATCOM     0x01000000L
    #define     CGOPT_MANGLE        0x02000000L
#if _CPU == _AXP || _CPU == _PPC
    #define     CGOPT_GENASM        0x10000000L
    #define     CGOPT_LOGOWL        0x20000000L
#endif
    #define     CGOPT_ECHOAPI       0x40000000L

// Optimization Options:
// ---------------------

    #define     OZOPT_O_LOOP        0x00000001L
    #define     OZOPT_O_DISABLE     0x00000002L
    #define     OZOPT_O_SPACE       0x00000004L
    #define     OZOPT_O_TIME        0x00000008L
    #define     OZOPT_O_VOLATILE    0x00000010L
    #define     OZOPT_O_CALL_RET    0x00000020L
    #define     OZOPT_O_MATH        0x00000040L
#if _CPU == 386 || _CPU == 8086
    #define     OZOPT_O_FRAME       0x00000080L
#endif
    #define     OZOPT_O_INSSCHED    0x00000100L
#if _CPU == 386
    #define     OZOPT_O_BASE_PTR    0x00000200L
#endif
    #define     OZOPT_O_INLINE      0x00000400L
    #define     OZOPT_O_NUMERIC     0x00000800L
    #define     OZOPT_O_UNROLL      0x00001000L
    #define     OZOPT_O_FASTDO      0x00002000L
    #define     OZOPT_O_BRANCH_PREDICTION \
                                    0x00004000L
    #define     OZOPT_O_FLOW_REG_SAVES \
                                    0x00008000L
    #define     OZOPT_O_SUPER_OPTIMAL \
                                    0x00010000L
    #define     OZOPT_O_LOOP_INVAR  0x00020000L

// CPU Options:
// ------------

#if ( _CPU == 8086 || _CPU == 386 )
// Floating-point reverse compatibility:
    #define     CPUOPT_FPR          0x00000001L

// Disable Pentium FDIV check:
    #define     CPUOPT_FPD          0x00000002L

// Floating-point model:
    #define     CPUOPT_FPC          0x00000004L
    #define     CPUOPT_FPI          0x00000008L
    #define     CPUOPT_FPI87        0x00000010L

// Floating-point level:
    #define     CPUOPT_FP287        0x00000020L
    #define     CPUOPT_FP387        0x00000040L
    #define     CPUOPT_FP5          0x00000080L
    #define     CPUOPT_FP6          0x00000100L

// CPU:
#if _CPU == 8086
    #define     CPUOPT_8086         0x00000200L
    #define     CPUOPT_80186        0x00000400L
    #define     CPUOPT_80286        0x00000800L
#endif
    #define     CPUOPT_80386        0x00001000L
    #define     CPUOPT_80486        0x00002000L
    #define     CPUOPT_80586        0x00004000L
    #define     CPUOPT_80686        0x00008000L

#endif
#if _CPU == 8086

#define _SmallDataModel( opts ) ( opts & ( CGOPT_M_MEDIUM ) )
#define _BigDataModel( opts )   ( opts & ( CGOPT_M_LARGE | CGOPT_M_HUGE ) )
#define CGOPT_NO_NO     (CGOPT_DB_LINE|CGOPT_DB_LOCALS| \
                         CGOPT_DI_CV|CGOPT_DI_DWARF|CGOPT_DI_WATCOM| \
                         CGOPT_M_MEDIUM|CGOPT_M_LARGE|CGOPT_M_HUGE)
#define OZOPT_NO_NO     (OZOPT_O_DISABLE|OZOPT_O_SPACE|OZOPT_O_TIME| \
                         OZOPT_O_LOOP|OZOPT_O_VOLATILE|OZOPT_O_CALL_RET| \
                         OZOPT_O_MATH|OZOPT_O_FRAME|OZOPT_O_INSSCHED| \
                         OZOPT_O_INLINE|OZOPT_O_NUMERIC|OZOPT_O_UNROLL| \
                         OZOPT_O_FASTDO|OZOPT_O_FLOW_REG_SAVES| \
                         OZOPT_O_SUPER_OPTIMAL|OZOPT_O_LOOP_INVAR)
#define CPUOPT_NO_NO    (CPUOPT_FPC|CPUOPT_FPI|CPUOPT_FPI87| \
                         CPUOPT_FP287|CPUOPT_FP387|CPUOPT_FP5|CPUOPT_FP6| \
                         CPUOPT_8086|CPUOPT_80286|CPUOPT_80286| \
                         CPUOPT_80386|CPUOPT_80486|CPUOPT_80586|CPUOPT_80686)
#define _FloatingDS( opts ) ( ( opts & (CGOPT_M_MEDIUM | CGOPT_WINDOWS) ) == 0 )
#define _FloatingES( opts ) ( 1 )
#define _FloatingFS( opts ) ( opts & CGOPT_FS_FLOATS )
#define _FloatingGS( opts ) ( opts & CGOPT_GS_FLOATS )

#define OZOPT_O_X       (OZOPT_O_LOOP | OZOPT_O_MATH | OZOPT_O_TIME | \
                         OZOPT_O_INSSCHED | OZOPT_O_INLINE | OZOPT_O_FASTDO | \
                         OZOPT_O_BRANCH_PREDICTION | OZOPT_O_FLOW_REG_SAVES )

#elif _CPU == 386

#define _SmallDataModel( opts ) ( opts &                                       \
                                ( CGOPT_M_FLAT|CGOPT_M_SMALL|CGOPT_M_MEDIUM ) )
#define _BigDataModel( opts )   ( opts & ( CGOPT_M_COMPACT | CGOPT_M_LARGE ) )
#define CGOPT_NO_NO     (CGOPT_DB_LINE|CGOPT_DB_LOCALS| \
                         CGOPT_DI_CV|CGOPT_DI_DWARF|CGOPT_DI_WATCOM| \
                         CGOPT_M_FLAT|CGOPT_M_SMALL|CGOPT_M_MEDIUM| \
                         CGOPT_M_COMPACT|CGOPT_M_LARGE| \
                         CGOPT_BD|CGOPT_BM)
#define OZOPT_NO_NO     (OZOPT_O_DISABLE|OZOPT_O_SPACE|OZOPT_O_TIME| \
                         OZOPT_O_LOOP|OZOPT_O_VOLATILE|OZOPT_O_FRAME| \
                         OZOPT_O_INSSCHED|OZOPT_O_MATH|OZOPT_O_CALL_RET| \
                         OZOPT_O_INLINE|OZOPT_O_BASE_PTR|OZOPT_O_NUMERIC| \
                         OZOPT_O_UNROLL|OZOPT_O_FASTDO|OZOPT_O_FLOW_REG_SAVES| \
                         OZOPT_O_BRANCH_PREDICTION|OZOPT_O_SUPER_OPTIMAL| \
                         OZOPT_O_LOOP_INVAR)
#define CPUOPT_NO_NO    (CPUOPT_FPC|CPUOPT_FPI|CPUOPT_FPI87| \
                         CPUOPT_FP287|CPUOPT_FP387|CPUOPT_FP5|CPUOPT_FP6| \
                         CPUOPT_80386|CPUOPT_80486|CPUOPT_80586|CPUOPT_80686)
#define _FloatingDS( opts ) ( ( opts & (CGOPT_M_MEDIUM | CGOPT_M_SMALL |       \
                                        CGOPT_M_FLAT) ) == 0 )
#define _FloatingES( opts ) ( ( opts & CGOPT_M_FLAT ) == 0 )
#define _FloatingFS( opts ) ( ( ( opts & CGOPT_M_FLAT ) == 0 ) || \
                              ( opts & CGOPT_FS_FLOATS ) )
#define _FloatingGS( opts ) ( opts & CGOPT_GS_FLOATS )

#define OZOPT_O_X       (OZOPT_O_LOOP | OZOPT_O_MATH | OZOPT_O_TIME | \
                         OZOPT_O_INSSCHED | OZOPT_O_INLINE | \
                         OZOPT_O_BASE_PTR | OZOPT_O_FASTDO | \
                         OZOPT_O_BRANCH_PREDICTION | OZOPT_O_FLOW_REG_SAVES )

#elif _CPU == _AXP
#define CGOPT_NO_NO     (CGOPT_DB_LINE|CGOPT_DB_LOCALS| \
                         CGOPT_DI_CV|CGOPT_DI_DWARF|CGOPT_DI_WATCOM| \
                         CGOPT_BD|CGOPT_BM)
#define OZOPT_NO_NO     (OZOPT_O_DISABLE|OZOPT_O_SPACE|OZOPT_O_TIME| \
                         OZOPT_O_LOOP|OZOPT_O_VOLATILE| \
                         OZOPT_O_INSSCHED|OZOPT_O_MATH|OZOPT_O_CALL_RET| \
                         OZOPT_O_INLINE|OZOPT_O_NUMERIC| \
                         OZOPT_O_UNROLL|OZOPT_O_FASTDO| \
                         OZOPT_O_FLOW_REG_SAVES|OZOPT_O_BRANCH_PREDICTION| \
                         OZOPT_O_SUPER_OPTIMAL|OZOPT_O_LOOP_INVAR)
#define CPUOPT_NO_NO     0x00000000L
#define OZOPT_O_X       (OZOPT_O_LOOP | OZOPT_O_MATH | OZOPT_O_TIME | \
                         OZOPT_O_INSSCHED | OZOPT_O_INLINE | \
                         OZOPT_O_FASTDO | OZOPT_O_BRANCH_PREDICTION | \
                         OZOPT_O_FLOW_REG_SAVES )
#elif _CPU == _PPC
#define CGOPT_NO_NO     (CGOPT_DB_LINE|CGOPT_DB_LOCALS| \
                         CGOPT_DI_CV|CGOPT_DI_DWARF|CGOPT_DI_WATCOM| \
                         CGOPT_BD|CGOPT_BM)
#define OZOPT_NO_NO     (OZOPT_O_DISABLE|OZOPT_O_SPACE|OZOPT_O_TIME| \
                         OZOPT_O_LOOP|OZOPT_O_VOLATILE| \
                         OZOPT_O_INSSCHED|OZOPT_O_MATH|OZOPT_O_CALL_RET| \
                         OZOPT_O_INLINE|OZOPT_O_NUMERIC| \
                         OZOPT_O_UNROLL|OZOPT_O_FASTDO| \
                         OZOPT_O_FLOW_REG_SAVES|OZOPT_O_BRANCH_PREDICTION| \
                         OZOPT_O_SUPER_OPTIMAL|OZOPT_O_LOOP_INVAR)
#define CPUOPT_NO_NO     0x00000000L
#define OZOPT_O_X       (OZOPT_O_LOOP | OZOPT_O_MATH | OZOPT_O_TIME | \
                         OZOPT_O_INSSCHED | OZOPT_O_INLINE | \
                         OZOPT_O_FASTDO | OZOPT_O_BRANCH_PREDICTION | \
                         OZOPT_O_FLOW_REG_SAVES )
#else
  #error Unknown Platform
#endif


#define OPT_NO_NO       (OPT_TYPE|OPT_PRINT|OPT_CHINESE|OPT_JAPANESE| \
                         OPT_KOREAN)

#define MAX_OPTIONS     64


struct opt_entry;

typedef struct opt_entry {
    char        *option;        // pointer to option name
    uint        description;    // description id
    unsigned_8  flags;          // option flags
#if !defined( __WFL__ )
    unsigned_32 value;          // value of option
    void        (*proc_rtnstr)(struct opt_entry *, char *);  // option processing for strin options
    void        (*proc_rtnbool)(struct opt_entry *, bool);   // for negatable options
#endif
} opt_entry;
