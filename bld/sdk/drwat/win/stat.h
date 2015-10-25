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


#define STAT_AX            400
#define STAT_BX            401
#define STAT_CX            402
#define STAT_DX            403
#define STAT_DI            404
#define STAT_SI            405
#define STAT_BP            406
#define STAT_OK            407
#define STAT_CS            408
#define STAT_DS            409
#define STAT_ES            410
#define STAT_FS            411
#define STAT_GS            412
#define STAT_SS            413
#define STAT_SP            414
#define STAT_IP            415
#define STAT_DISASM_1      420
#define STAT_DISASM_2      421
#define STAT_DISASM_3      422
#define STAT_DISASM_4      423
#define STAT_DISASM_5      424
#define STAT_DISASM_6      425
#define STAT_DISASM_7      426
#define STAT_DISASM_8      427
#define STAT_CANCEL        IDCANCEL
#define STAT_SCROLL        429
#define STAT_APPLY         430
#define STAT_STACK_TRACE   431
#define STAT_SYMBOLS                490
#define STAT_SEGLIST                435
#define STAT_SEG_MAP                102
#define STAT_SRC_INFO               106
#define STAT_FLAG_A                 115
#define STAT_FLAG_C                 116
#define STAT_FLAG_D                 117
#define STAT_FLAG_I                 118
#define STAT_FLAG_Z                 119
#define STAT_FLAG_S                 120
#define STAT_FLAG_P                 121
#define STAT_FLAG_O                 123

#define FLAG_C_MASK  ( 1 << 0 )
#define FLAG_P_MASK  ( 1 << 2 )
#define FLAG_A_MASK  ( 1 << 4 )
#define FLAG_Z_MASK  ( 1 << 6 )
#define FLAG_S_MASK  ( 1 << 7 )
#define FLAG_T_MASK  ( 1 << 8 )
#define FLAG_I_MASK  ( 1 << 9 )
#define FLAG_D_MASK  ( 1 << 10 )
#define FLAG_O_MASK  ( 1 << 11 )

WINEXPORT BOOL FAR PASCAL SegMapDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
WINEXPORT BOOL FAR PASCAL StatDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
