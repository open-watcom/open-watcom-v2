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


#define DIALOG_WIND 3001

#define CTL_WIND_ASM_SOURCE 103
#define CTL_WIND_ASM_HEX 106
#define CTL_WIND_FILE_TAB 115
#define CTL_WIND_VAR_EXPR 107
#define CTL_WIND_VAR_CODE 108
#define CTL_WIND_VAR_INHERIT 109
#define CTL_WIND_VAR_COMPILER 110
#define CTL_WIND_VAR_MEMBERS 113
#define CTL_WIND_VAR_PRIVATE 111
#define CTL_WIND_VAR_PROTECTED 112
#define CTL_WIND_VAR_STATIC 114
#define CTL_WIND_MOD_ALL 120
#define CTL_WIND_FUNC_TYPED 118
#define CTL_WIND_GLOB_TYPED 119
#define CTL_WIND_OK 100
#define CTL_WIND_DEFAULTS 102
#define CTL_WIND_CANCEL 101

#ifndef RC
    typedef struct dlg_window_set {
        unsigned        cancel : 1;
    } dlg_window_set;
#endif

