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
* Description:  Front End callback types (RISC).
*
****************************************************************************/


        LIBRARY_NAME,
        NEXT_LIBRARY,
        LINKAGE_REGS,
        AUX_OFFSET,
        AUX_HAS_MAIN,
        ASM_NAME,
        DBG_NAME,
        CSECT_NAME,
        CLASS_APPENDED_NAME,
        IMPORT_NAME,
        NEXT_IMPORT,
        IMPORT_NAME_S,
        NEXT_IMPORT_S,
        IMPORT_TYPE,
#define IMPORT_IS_WEAK              0
#define IMPORT_IS_LAZY              1
#define IMPORT_IS_CONDITIONAL       2
#define IMPORT_IS_CONDITIONAL_PURE  3
        CONDITIONAL_IMPORT,
        NEXT_CONDITIONAL,
        CONDITIONAL_SYMBOL,
        VIRT_FUNC_REFERENCE,
        VIRT_FUNC_SYM,
        VIRT_FUNC_NEXT_REFERENCE,
        EXCEPTION_HANDLER,
        EXCEPTION_DATA,
        NEXT_ALIAS,
        ALIAS_NAME,
        ALIAS_SYMBOL,
        ALIAS_SUBST_NAME,
        ALIAS_SUBST_SYMBOL,
