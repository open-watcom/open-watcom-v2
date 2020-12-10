/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include "msgattr.gh"
#include "msgdefs.gh"


#define WLEVEL_MAX      5
#define WLEVEL_ERROR    0
#define WLEVEL_DEFAULT  1
#define WLEVEL_WX       (WLEVEL_MAX - 1)
#define WLEVEL_ENABLED  1
#define WLEVEL_DISABLED WLEVEL_MAX
#define WLEVEL_NOTE     -1

enum {
    MESSAGE_COUNT = 0
    #define MSG_DEF( name, group, kind, level, group_index ) +1
        MSG_DEFS
    #undef MSG_DEF
};

enum warning_codes {
    #define warn(code,level) code = level,
    #include "cwngs.h"
    #undef warn
};

enum group_levels {
    #define GRP_DEF( name,prefix,num,index,eindex ) name##_level = num,
        GRP_DEFS
    #undef GRP_DEF
};

typedef enum msg_codes {
    ERR_NONE = 0,
    #define MSG_DEF( name, group, kind, level, group_index ) name = group##_level+group_index,
        MSG_DEFS
    #undef MSG_DEF
} msg_codes;

typedef enum msg_type {
    #define MSGTYPES_DEF( name )   MSG_TYPE_##name,
        MSGTYPES_DEFS
    #undef MSGTYPES_DEF
} msg_type;

typedef struct msg_level_info {
    unsigned    type        : 4;
    unsigned    level       : 4;
    unsigned    enabled     : 1;
} msg_level_info;

extern msg_level_info   msg_level[MESSAGE_COUNT];
