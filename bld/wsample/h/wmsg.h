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
* Description:  Execution Sampler message ordinals and functions.
*
****************************************************************************/


#define ERR_FIRST_MESSAGE               16
#define ERR_DISK_FULL                   16
#define ERR_SAMPLES_LOST                17
#define ERR_SAMPLE_TRUNCATE             18
#define MSG_EXPECTING                   19
#define MSG_INVALID_CHAR                20
#define MSG_OUT_OF_RANGE                21
#define MSG_SAMPLE_BUFF                 22
#define MSG_CALLGRAPH_BUFF              23
#define MSG_VERSION                     24
#define MSG_SAMPLE_FILE                 25
#define MSG_PROGRAM                     26
#define MSG_BUFF_SMALL                  27
#define MSG_INVALID_OPTION              28

#define MSG_USAGE_LN_1                  29
#define MSG_USAGE_LN_2                  30
#define MSG_USAGE_LN_3                  31
#define MSG_USAGE_LN_4                  32
#define MSG_USAGE_LN_5                  33

#define MSG_OPTIONS_1                   34
#define MSG_OPTIONS_2                   35
#define MSG_OPTIONS_3                   36

#define MSG_USAGE_LN_6                  37
#define MSG_USAGE_LN_7                  38
#define MSG_USAGE_LN_8                  39

#define MSG_EXPLAIN_1                   40
#define MSG_EXPLAIN_2                   41
#define MSG_EXPLAIN_3                   42
#define MSG_EXPLAIN_4                   43
#define MSG_EXPLAIN_5                   44
#define MSG_EXPLAIN_6                   45
#define MSG_EXPLAIN_7                   46
#define MSG_EXPLAIN_8                   47
#define MSG_EXPLAIN_9                   48
#define MSG_EXPLAIN_10                  49

#define MSG_USAGE_LN_9                  50

#define MSG_SAMPLE_1                    51
#define MSG_SAMPLE_2                    52
#define MSG_SAMPLE_3                    53
#define MSG_SAMPLE_4                    54
#define MSG_SAMPLE_5                    55
#define MSG_SAMPLE_6                    56
#define MSG_SAMPLE_7                    57
#define MSG_SAMPLE_8                    58
#define MSG_SAMPLE_9                    59
#define MSG_SAMPLE_10                   60
#define MSG_SAMPLE_11                   61
#define MSG_SAMPLE_12                   62
#define MSG_SAMPLE_13                   63
#define MSG_SAMPLE_14                   64
#define MSG_SAMPLE_15                   65
#define MSG_SAMPLE_16                   66

#define MSG_EXCEPT_0                    67
#define MSG_EXCEPT_1                    68
#define MSG_EXCEPT_2                    69
#define MSG_EXCEPT_3                    70
#define MSG_EXCEPT_4                    71
#define MSG_EXCEPT_5                    72
#define MSG_EXCEPT_6                    73
#define MSG_EXCEPT_7                    74
#define MSG_EXCEPT_8                    75
#define MSG_EXCEPT_9                    76
#define MSG_EXCEPT_10                   77
#define MSG_EXCEPT_11                   78
#define MSG_EXCEPT_12                   79
#define MSG_EXCEPT_13                   80
#define MSG_EXCEPT_14                   81

#define MSG_DBERROR_0                   82
#define MSG_DBERROR_1                   83
#define MSG_DBERROR_2                   84
#define MSG_DBERROR_3                   85
#define MSG_DBERROR_4                   86
#define MSG_DBERROR_5                   87
#define MSG_DBERROR_6                   88
#define MSG_DBERROR_7                   89
#define MSG_DBERROR_8                   90
#define MSG_DBERROR_9                   91
#define MSG_DBERROR_10                  92
#define MSG_DBERROR_11                  93
#define MSG_DBERROR_12                  94
#define MSG_DBERROR_13                  95
#define MSG_DBERROR_14                  96
#define MSG_DBERROR_15                  97
#define MSG_DBERROR_16                  98
#define MSG_DBERROR_17                  99
#define MSG_DBERROR_18                  100
#define MSG_DBERROR_19                  101
#define MSG_DBERROR_20                  102
#define MSG_DBERROR_21                  103
#define MSG_DBERROR_22                  104
#define MSG_DBERROR_23                  105
#define MSG_DBERROR_24                  106
#define MSG_DBERROR_25                  107
#define MSG_DBERROR_26                  108
#define ERR_LAST_MESSAGE                108

#define MSG_LANG_SPACING        1000

#define MSG_USE_E_BASE  (MSG_USAGE_LN_1 + RLE_ENGLISH*MSG_LANG_SPACING)
#define MSG_USE_J_BASE  (MSG_USAGE_LN_1 + RLE_JAPANESE*MSG_LANG_SPACING)

extern char FAR_PTR     *MsgArray[ERR_LAST_MESSAGE-ERR_FIRST_MESSAGE+1];

#ifndef __WINDOWS__
int MsgInit( void );
#endif
void MsgFini( void );
void MsgPrintfUsage( int first_ln, int last_ln );
