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


#ifndef CMDLINE_H
#define CMDLINE_H   1
#include "watcom.h"

typedef struct action_group act_grp_t;
struct action_group {
    act_grp_t   *next;
    uint_8      dbg_parser      :2;
    uint_8      dbg_generator   :3;
    uint_8      omf_generator   :3;
    uint_8      quiet           :1; /* should we be quiet */
    uint_8      deflib          :1; /* move default library COMENTs */
    uint_8      batch           :1; /* output batch commands */
    const char  *output;            /* output file spec */
    uint        num_files;
    const char  *files[1];
};

typedef struct cmdline cmdline_t;
struct cmdline {
    act_grp_t   *action;
    uint_8      quiet           : 1;
    uint_8      need_output     : 1;    /* does current action need an output
                                            file? */
};

extern cmdline_t *CmdLineParse( void );
extern void ActionInit( cmdline_t *cmd );
extern void ActionFini( cmdline_t *cmd );
extern void ActionInfile( cmdline_t *cmd, char *buf, uint file_num );
extern void ActionOutfile( cmdline_t *cmd, char *buf, uint file_num );
extern void ActionRename( cmdline_t *cmd, const char *in, const char *out,
    uint file_num, int make_lib, size_t page_size );
extern int BeQuiet( void );

#endif
