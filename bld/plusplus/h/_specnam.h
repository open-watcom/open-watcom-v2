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


// ".#" prefix is reserved for NameDummy names
// ".@" prefix is reserved for operator names (precedes mangled name for op)

pick( SPECIAL_NAME_THIS,          "this" )
pick( SPECIAL_NAME_RETURN_VALUE,  ".return" )
pick( SPECIAL_NAME_COPY_ARG,      ".copy_arg" )
pick( SPECIAL_NAME_MODULE_INIT,   ".mod_init" )
pick( SPECIAL_NAME_INIT_FUNCTION, ".fn_init" )
pick( SPECIAL_NAME_CDTOR_EXTRA,   ".cdtor" )
pick( SPECIAL_NAME_CTOR_THUNK,    ".tdctor" )
pick( SPECIAL_NAME_COPY_THUNK,    ".tcctor" )
pick( SPECIAL_NAME_DTOR_THUNK,    ".tdtor" )
pick( SPECIAL_NAME_OP_DEL_THUNK,  ".tdopdel" )
pick( SPECIAL_NAME_OP_DELAR_THUNK,".tdopdelarr" )
pick( SPECIAL_NAME_SETJMP_VALUE,  ".sj_val" )
pick( SPECIAL_NAME_CHIPBUG,       "__chipbug" )
pick( SPECIAL_NAME_DWARF_ABBREV,  "__DFABBREV" )
pick( SPECIAL_NAME_TYPE_INFO,     "type_info" )
pick( SPECIAL_NAME_BEGINTHREAD,   "_beginthread" )
pick( SPECIAL_NAME_BEGINTHREADEX, "_beginthreadex" )
pick( SPECIAL_NAME_STD,           "std" )
