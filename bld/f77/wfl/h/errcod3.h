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


#define    NO_CARROT  0
#define    OPR_CARROT 1
#define    OPN_CARROT 2
#define    MS_OPT_LIST 11010
#define    MS_OPT_PRINT 11011
#define    MS_OPT_TYPE 11012
#define    MS_OPT_DISK 11013
#define    MS_OPT_ERRFILE 11014
#define    MS_OPT_EXT 11015
#define    MS_OPT_REFERENCE 11016
#define    MS_OPT_WARN 11017
#define    MS_OPT_SHORT 11018
#define    MS_OPT_INCLIST 11019
#define    MS_OPT_TERM 11020
#define    MS_OPT_EXPLICIT 11021
#define    MS_OPT_XLINE 11022
#define    MS_OPT_DEBUG 11024
#define    MS_OPT_DEFINE 11025
#define    MS_OPT_EXTEND_FORMAT 11026
#define    MS_OPT_WILD 11027
#define    MS_OPT_SYNTAX 11044
#define    MS_OPT_DFLT_LIB 11045
#define    MS_CPUOPT_FPC 11046
#define    MS_CPUOPT_FPI 11047
#define    MS_CPUOPT_FPI87 11048
#define    MS_CPUOPT_FP287 11049
#define    MS_CPUOPT_FP387 11050
#define    MS_CPUOPT_FP5 11051
#define    MS_CPUOPT_FPR 11052
#define    MS_CGOPT_DB_LINE 11053
#define    MS_CGOPT_DB_LOCALS 11054
#define    MS_OZOPT_O_BASE_PTR 11055
#define    MS_OZOPT_O_CALL_RET 11056
#define    MS_OZOPT_O_DISABLE 11057
#define    MS_OZOPT_O_NEED_FRAME 11058
#define    MS_OZOPT_O_INLINE 11059
#define    MS_OZOPT_O_LOOP 11060
#define    MS_OZOPT_O_MATH 11061
#define    MS_OZOPT_O_NUMERIC 11062
#define    MS_OZOPT_O_VOLATILE 11063
#define    MS_OZOPT_O_INSSCHED 11064
#define    MS_OZOPT_O_SPACE 11065
#define    MS_OZOPT_O_TIME 11066
#define    MS_OZOPT_O_X 11067
#define    MS_CGOPT_STACK_CHK 11070
#define    MS_CGOPT_DATA_THRESH 11071
#define    MS_OPT_AUTOMATIC 11072
#define    MS_OPT_DESCRIPTOR 11073
#define    MS_OPT_SAVE 11074
#define    MS_CGOPT_OBJ_NAME 11075
#define    MS_CGOPT_CONST_CODE 11076
#define    MS_CGOPT_STK_ARGS 11083
#define    MS_CGOPT_EZ_OMF 11084
#define    MS_CGOPT_BW 11085
#define    MS_CGOPT_BM 11086
#define    MS_CGOPT_BD 11088
#define    MS_CGOPT_STACK_GROW 11090
#define    MS_CGOPT_M_FLAT 11091
#define    MS_CGOPT_M_SMALL 11092
#define    MS_CGOPT_M_COMPACT 11093
#define    MS_CGOPT_M_MEDIUM 11094
#define    MS_CGOPT_M_LARGE 11095
#define    MS_CPUOPT_80386 11096
#define    MS_CPUOPT_80486 11097
#define    MS_CPUOPT_80586 11098
#define    MS_CGOPT_SEG_REGS 11099
#define    MS_CGOPT_FS_FLOATS 11100
#define    MS_CGOPT_GS_FLOATS 11101
#define    MS_CGOPT_WINDOWS 11103
#define    MS_CTG_FILE_MANAGEMENT 11169
#define    MS_CTG_DIAGNOSTIC 11170
#define    MS_CTG_DEBUGGING 11171
#define    MS_CTG_MISC 11172
#define    MS_CTG_FPUOPTS 11173
#define    MS_CTG_OPTIMIZATIONS 11174
#define    MS_CTG_MEMORY_MODELS 11175
#define    MS_CTG_CPU 11176
#define    CL_CTG_COMPILE_AND_LINK 11177
#define    CL_USAGE_LINE 11179
#define    CL_COMPILE_ONLY 11180
#define    CL_IGNORE_WFL 11181
#define    CL_RUN_PROTECT 11182
#define    CL_GENERATE_MAP 11183
#define    CL_NAME_EXECUTABLE 11184
#define    CL_KEEP_DIRECTIVE 11185
#define    CL_INCLUDE_DIRECTIVE 11186
#define    CL_LINK_FOR_SYSTEM 11189
#define    CL_STACK_SIZE 11190
#define    CL_LINKER_DIRECTIVES 11191
#define    CL_OUT_OF_MEMORY 11192
#define    CL_ERROR_OPENING_TMP_FILE 11193
#define    CL_UNABLE_TO_OPEN 11194
#define    CL_UNABLE_TO_INVOKE_COMPILER 11195
#define    CL_BAD_COMPILE 11196
#define    CL_UNABLE_TO_INVOKE_LINKER 11197
#define    CL_BAD_LINK 11198
#define    CL_UNABLE_TO_FIND 11199
#define    MS_OPT_TRACE 11200
#define    MS_OPT_BOUNDS 11201
#define    MS_CTG_DBCS 11202
#define    MS_OPT_CHINESE 11203
#define    MS_OPT_JAPANESE 11204
#define    MS_OPT_KOREAN 11205
#define    MS_OPT_DEPENDENCY 11206
#define    MS_CTG_APPTYPE 11207
#define    MS_OPT_RESOURCES 11208
#define    MS_OPT_UNIT_6_CC 11209
#define    MS_OPT_QUIET 11210
#define    MS_OPT_BROWSE 11216
#define    MS_OPT_EXTEND_REAL 11217
#define    MS_OZOPT_O_UNROLL 11218
#define    MS_CGOPT_ALIGN 11219
#define    MS_OPT_LF_WITH_FF 11220
#define    MS_CGOPT_DI_WATCOM 11221
#define    MS_CGOPT_DI_DWARF 11222
#define    MS_CGOPT_DI_CV 11223
#define    CL_UNABLE_TO_INVOKE_CVPACK 11224
#define    CL_BAD_CVPACK 11225
#define    MS_OZOPT_O_FASTDO 11226
#define    MS_OPT_INCPATH 11227
#define    MS_CPUOPT_FPD 11228
#define    MS_OZOPT_O_BRANCH_PREDICTION 11229
#define    MS_CGOPT_MANGLE 11230
#define    MS_OPT_PROMOTE 11231
#define    MS_OPT_COMMA_SEP 11232
#define    MS_CPUOPT_FP6 11233
#define    MS_CPUOPT_80686 11234
#define    MS_OZOPT_O_SUPER_OPTIMAL 11235
#define    MS_OZOPT_O_FLOW_REG_SAVES 11236
#define    MS_OZOPT_O_LOOP_INVAR 11237
