/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DDE Spy master include.
*
****************************************************************************/

/* menu constants */

#define DDEMENU_EXIT                    101
#define DDEMENU_SAVE_AS                 103
#define DDEMENU_LOG_FILE                104
#define DDEMENU_LOG_PAUSE               105
#define DDEMENU_LOG_CONFIG              106
#define DDEMENU_SAVE                    107
#define DDEMENU_TOOLBAR                 108
#define DDEMENU_FONT                    109
#define DDEMENU_HINTBAR                 102
#define DDEMENU_TOP                     160

#define DDEMENU_ABOUT                   130
#define DDEMENU_HELP_CONTENTS           131
#define DDEMENU_HELP_SRCH               132
#define DDEMENU_HELP_ON_HELP            133

#define DDEMENU_CLEAR                   141
#define DDEMENU_MARK                    142
#define DDEMENU_SCROLL                  143
#define DDEMENU_SCREEN_OUT              144

#define DDEMENU_TASK_ALIAS              150
#define DDEMENU_CONV_ALIAS              151
#define DDEMENU_HWND_ALIAS              152
#define DDEMENU_NO_ALIAS                153
#define DDEMENU_ALIAS_PURGE             154

/*
 * When future monitoring options are to be added their constants should be
 * the next avalable integer and DDE_MON_LAST must be updated.  The fact
 * that the constants are consecutive is used in accessing the Monitoring
 * data structure.
 */

#define DDE_MON_FIRST                   DDEMENU_MON_POST
#define DDEMENU_MON_POST                110
#define DDEMENU_MON_SENT                111
#define DDEMENU_MON_STR                 112
#define DDEMENU_MON_CB                  113
#define DDEMENU_MON_ERR                 114
#define DDEMENU_MON_LNK                 115
#define DDEMENU_MON_CONV                116
#define DDE_MON_LAST                    DDEMENU_MON_CONV

#define DDEMENU_MSG_FILTER              117
#define DDEMENU_CB_FILTER               118

#define DDE_TRK_FIRST                   DDEMENU_TRK_STR
#define DDEMENU_TRK_STR                 120
#define DDEMENU_TRK_CONV                121
#define DDEMENU_TRK_LINK                122
#define DDEMENU_TRK_SERVER              123
#define DDE_TRK_LAST                    DDEMENU_TRK_SERVER

#define MAX_DDE_MON                     (DDE_MON_LAST - DDE_MON_FIRST + 1)
#define MAX_DDE_TRK                     (DDE_TRK_LAST - DDE_TRK_FIRST + 1)
