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


#include "msgattr.gh"
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
#include  "msgdefs.gh"
typedef enum msg_codes {
#define MSG_DEF( name, group, kind, level, group_index ) name = group##_level+group_index,
    MSG_DEFS
#undef MSG_DEF
    HIGHEST_MESSAGE_NUMBER = PHRASE_PRESS_RETURN+1 //masking msg crap
}msg_codes;
typedef enum msgtype{
#define MSGTYPES_DEF( name )   msgtype_##name,
    MSGTYPES_DEFS
#undef MSGTYPES_DEF
    msgtype_err
}msgtype;
