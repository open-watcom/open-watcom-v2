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


#ifdef SPECNAME_DEFINE
#define special( e, s )         s ,
#elif defined(_SPECNAME_H)
#define special( e, s )
#else
#define special( e, s )         e ,
enum {
#endif

// ".#" prefix is reserved for NameDummy names
// ".@" prefix is reserved for operator names (precedes mangled name for op)

special( SPECIAL_THIS,          "this" )
special( SPECIAL_RETURN_VALUE,  ".return" )
special( SPECIAL_COPY_ARG,      ".copy_arg" )
special( SPECIAL_MODULE_INIT,   ".mod_init" )
special( SPECIAL_INIT_FUNCTION, ".fn_init" )
special( SPECIAL_CDTOR_EXTRA,   ".cdtor" )
special( SPECIAL_CTOR_THUNK,    ".tdctor" )
special( SPECIAL_COPY_THUNK,    ".tcctor" )
special( SPECIAL_DTOR_THUNK,    ".tdtor" )
special( SPECIAL_OP_DEL_THUNK,  ".tdopdel" )
special( SPECIAL_OP_DELAR_THUNK,".tdopdelarr" )
special( SPECIAL_SETJMP_VALUE,  ".sj_val" )
special( SPECIAL_CHIPBUG,       "__chipbug" )
special( SPECIAL_DWARF_ABBREV,  "__DFABBREV" )
special( SPECIAL_TYPE_INFO,     "type_info" )
special( SPECIAL_BEGINTHREAD,   "_beginthread" )
special( SPECIAL_BEGINTHREADEX, "_beginthreadex" )
special( SPECIAL_STD,           "std" )

#undef special
#ifdef SPECNAME_DEFINE
#undef SPECNAME_DEFINE
#elif !defined(_SPECNAME_H)
};
#define _SPECNAME_H
#endif
