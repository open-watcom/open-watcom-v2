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


/*  op1       op2         res        zap    reserve   name  */
RG( RL_,       RL_,       RL_,       RL_,    RL_,      RG_ ),
RG( RL_BYTE,   RL_BYTE,   RL_BYTE,   RL_,    RL_,      RG_BYTE ),
RG( RL_BYTE,   RL_BYTE,   RL_BYTE,   RL_,    RL_BYTE,  RG_BYTE_NEED ),
RG( RL_WORD,   RL_WORD,   RL_WORD,   RL_,    RL_,      RG_WORD ),
RG( RL_WORD,   RL_WORD,   RL_WORD,   RL_,    RL_WORD,  RG_WORD_NEED ),
RG( RL_DWORD,  RL_DWORD,  RL_DWORD,  RL_,    RL_,      RG_DWORD ),
RG( RL_DWORD,  RL_DWORD,  RL_DWORD,  RL_,    RL_DWORD, RG_DWORD_NEED ),
RG( RL_QWORD,  RL_QWORD,  RL_QWORD,  RL_,    RL_,      RG_QWORD ),
RG( RL_QWORD,  RL_QWORD,  RL_QWORD,  RL_,    RL_QWORD, RG_QWORD_NEED ),
RG( RL_FLOAT,  RL_FLOAT,  RL_FLOAT,  RL_,    RL_,      RG_FLOAT  ),
RG( RL_FLOAT,  RL_FLOAT,  RL_FLOAT,  RL_,    RL_FLOAT, RG_FLOAT_NEED),
/*  following are all for conversion routines */
RG( RL_BYTE,   RL_BYTE,   RL_WORD,   RL_,    RL_,      RG_BW ),
RG( RL_BYTE,   RL_BYTE,   RL_WORD,   RL_,    RL_WORD,  RG_BW_NEED ),
RG( RL_BYTE,   RL_BYTE,   RL_DWORD,  RL_,    RL_,      RG_BD ),
RG( RL_BYTE,   RL_BYTE,   RL_DWORD,  RL_,    RL_DWORD, RG_BD_NEED ),
RG( RL_BYTE,   RL_BYTE,   RL_QWORD,  RL_,    RL_,      RG_BQ ),
RG( RL_BYTE,   RL_BYTE,   RL_QWORD,  RL_,    RL_QWORD, RG_BQ_NEED ),
/* word to other integer conversions */
RG( RL_WORD,   RL_WORD,   RL_BYTE,   RL_,    RL_,      RG_WB ),
RG( RL_WORD,   RL_WORD,   RL_BYTE,   RL_,    RL_WORD,  RG_WB_NEED ),
RG( RL_WORD,   RL_WORD,   RL_DWORD,  RL_,    RL_,      RG_WD ),
RG( RL_WORD,   RL_WORD,   RL_DWORD,  RL_,    RL_DWORD, RG_WD_NEED ),
RG( RL_WORD,   RL_WORD,   RL_QWORD,  RL_,    RL_,      RG_WQ ),
RG( RL_WORD,   RL_WORD,   RL_QWORD,  RL_,    RL_QWORD, RG_WQ_NEED ),
/* dword to other integer conversions */
RG( RL_DWORD,  RL_DWORD,  RL_BYTE,   RL_,    RL_,      RG_DB ),
RG( RL_DWORD,  RL_DWORD,  RL_BYTE,   RL_,    RL_DWORD, RG_DB_NEED ),
RG( RL_DWORD,  RL_DWORD,  RL_WORD,   RL_,    RL_,      RG_DW ),
RG( RL_DWORD,  RL_DWORD,  RL_WORD,   RL_,    RL_DWORD, RG_DW_NEED ),
RG( RL_DWORD,  RL_DWORD,  RL_QWORD,  RL_,    RL_,      RG_DQ ),
RG( RL_DWORD,  RL_DWORD,  RL_QWORD,  RL_,    RL_QWORD, RG_DQ_NEED ),
/* qword to other integer conversions */
RG( RL_QWORD,  RL_QWORD,  RL_BYTE,   RL_,    RL_,      RG_QB ),
RG( RL_QWORD,  RL_QWORD,  RL_BYTE,   RL_,    RL_QWORD, RG_QB_NEED ),
RG( RL_QWORD,  RL_QWORD,  RL_WORD,   RL_,    RL_,      RG_QW ),
RG( RL_QWORD,  RL_QWORD,  RL_WORD,   RL_,    RL_QWORD, RG_QW_NEED ),
RG( RL_QWORD,  RL_QWORD,  RL_DWORD,  RL_,    RL_,      RG_QD ),
RG( RL_QWORD,  RL_QWORD,  RL_DWORD,  RL_,    RL_QWORD, RG_QD_NEED ),
/* qword to float and vice versa conversions */
RG( RL_DWORD,  RL_DWORD,  RL_FLOAT,  RL_,    RL_,      RG_DF ),
RG( RL_DWORD,  RL_DWORD,  RL_FLOAT,  RL_,    RL_FLOAT, RG_DF_NEED ),
RG( RL_FLOAT,  RL_FLOAT,  RL_DWORD,  RL_,    RL_,      RG_FD ),
RG( RL_FLOAT,  RL_FLOAT,  RL_DWORD,  RL_,    RL_FLOAT, RG_FD_NEED ),

#define MAX_RG  66
/*  the number of elements in the largest RG_ */
/*  table entry in GENREG */
