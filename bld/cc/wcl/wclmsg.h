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


pick(   UNABLE_TO_OPEN_DIRECTIVE_FILE,
        E("Error: Unable to open directive file '%s'")
        J("Error: Unable to open directive file '%s'")
),
pick(   UNABLE_TO_OPEN_TEMPORARY_FILE,
        E("Error: Unable to open temporary file")
        J("Error: Unable to open temporary file")
),
pick(   UNABLE_TO_INVOKE_EXE,
        E("Error: Unable to invoke '%s'")
        J("Error: Unable to invoke '%s'")
),
pick(   COMPILER_RETURNED_A_BAD_STATUS,
        E("Error: Compiler returned a bad status compiling '%s'")
        J("Error: Compiler returned a bad status compiling '%s'")
),
pick(   LINKER_RETURNED_A_BAD_STATUS,
        E("Error: Linker returned a bad status")
        J("Error: Linker returned a bad status")
),
pick(   CVPACK_RETURNED_A_BAD_STATUS,
        E("Error: cvpack returned a bad status")
        J("Error: cvpack returned a bad status")
),
pick(   UNABLE_TO_OPEN,                 // unable to open file
        E("Unable to open '%s'")
        J("Unable to open '%s'")
),
pick(   UNABLE_TO_FIND,                 // unable to find file
        E("Error: Unable to find '%s'")
        J("Error: Unable to find '%s'")
),
pick(   OUT_OF_MEMORY,
        E("Out of memory")
        J("Out of memory")
),
pick(   PRESS_ANY_KEY_TO_CONTINUE,
        E("Press any key to continue:")
        J("Press any key to continue:")
),
