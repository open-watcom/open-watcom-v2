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
* Description:  Debugger hook related constant definitions
*
****************************************************************************/


pick( "PROGStart",      HOOK_PROG_START )
pick( "PROGEnd",        HOOK_PROG_END   )
pick( "DLLStart",       HOOK_DLL_START  )
pick( "DLLEnd",         HOOK_DLL_END    )   /* not used yet */
pick( "EXECStart",      HOOK_EXEC_START )
pick( "EXECEnd",        HOOK_EXEC_END   )
pick( "Sourceinfo",     HOOK_SRC_START  )
pick( "Assemblyinfo",   HOOK_SRC_END    )
pick( "Modulechange",   HOOK_NEW_MODULE )
pick( "Quit",           HOOK_QUIT       )
