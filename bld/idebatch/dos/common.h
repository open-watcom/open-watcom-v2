/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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


#define DEFAULT_LINK_NAME           "idelink"
#define EDITOR_LINK_NAME            "edidelink"

#define LIT_MAGIC_ID                "\251\252\253\254"
#define LIT_TERMINATE_COMMAND_STR   LIT_MAGIC_ID "done"
#define LIT_TERMINATE_CLIENT_STR    LIT_MAGIC_ID "die"
#define LIT_NEW_OPEN_LIST           LIT_MAGIC_ID "open"
#define LIT_END_OPEN_LIST           LIT_MAGIC_ID "endopen"
#define LIT_GOT_OPEN_STR            LIT_MAGIC_ID "gotopenstr"
#define LIT_GET_REAL_NAME           LIT_MAGIC_ID "getrealname"

#define VxDPutLIT(x)    VxDPut(x, sizeof(x) - 1)
