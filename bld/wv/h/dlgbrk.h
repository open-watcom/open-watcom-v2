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


#ifndef RC
    typedef struct {
        brkp            *bp;
        brkp            tmpbp;
        unsigned        last_edit;
        unsigned        brand_new       : 1;
        unsigned        cancel          : 1;
        unsigned        clear           : 1;
        unsigned        cmd_error       : 1;
        unsigned        cond_error      : 1;
    } dlg_brk;
#endif

#define DIALOG_BREAK 3002

#define CTL_BRK_ADDRESS 104
#define CTL_BRK_CONDITION 105
#define CTL_BRK_EXECUTE 113
#define CTL_BRK_BYTE 114
#define CTL_BRK_WORD 115
#define CTL_BRK_DWORD 116
#define CTL_BRK_QWORD 118
#define CTL_BRK_COUNTDOWN 106
#define CTL_BRK_TOTAL 107
#define CTL_BRK_RESET 103
#define CTL_BRK_CMD_LIST 111
#define CTL_BRK_RESUME 109
#define CTL_BRK_ACTIVE 110
#define CTL_BRK_WHATIS 112
#define CTL_BRK_OK 100
#define CTL_BRK_CLEAR 102
#define CTL_BRK_SYMBOL 117
#define CTL_BRK_CANCEL 101
