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


/* Note: We've got problems if there are more than 32 hook_types */
typedef unsigned_8 hook_type; enum {
    HOOK_PROG_START,
    HOOK_PROG_END,
    HOOK_DLL_START,
    HOOK_DLL_END,       /* not used yet */
    HOOK_EXEC_START,
    HOOK_EXEC_END,
    HOOK_SRC_START,
    HOOK_SRC_END,
    HOOK_NEW_MODULE,
    HOOK_QUIT,
    HOOK_LAST  = HOOK_QUIT,
    HOOK_FIRST = HOOK_PROG_START,
    HOOK_NUM   = HOOK_LAST - HOOK_FIRST + 1
};


extern void             HookNotify( bool, hook_type );
