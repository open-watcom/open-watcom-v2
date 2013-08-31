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


// PPINTNAME -- C++ Internal names
//
// 91/07/01 -- J.W.Welch        -- defined
// 91/10/25 -- J.W.Welch        -- move internal function names to PPOPSDEF.H
// 93/01/18 -- Greg Bentz       -- revamp to match mangle.y grammar

// DELIMITERS

#define IN_TRUNCATE             'T'     // start of truncated name
#define IN_MANGLE               "W?"    // entire start of a mangled name
#define IN_MANGLE1              "W"     // start of a mangled name
#define IN_MANGLE2              "?"     // start of a mangled name
#define IN_NAME_PREFIX          "$"     // start of a name
#define CHR_NAME_PREFIX         '$'     // start of a name
#define IN_NAME_SUFFIX          "$"     // end of a name
#define IN_OP_PREFIX            ".@"    // indicates a operator-function name (rest is mangled)
#define IN_ANON_ENUM            "TE"    // indicates anonymous enum
#define IN_SYM_WATCOM           "W"     // indicates internal symbol mangled name
#define CHR_SYM_WATCOM          'W'     // indicates internal symbol mangled name
#define IN_VATABLE_NAME         "VA"    // a virtual function RTTI adjustor table
#define IN_VBTABLE_NAME         "VB"    // a virtual base offset table
#define IN_VFTABLE_NAME         "VF"    // a virtual function destination table
#define IN_VMTABLE_NAME         "VM"    // a virtual base mapping table
#define IN_VFTHUNK_NAME         "VT"    // a virtual function thunk
#define IN_TYPEID_NAME          "TI"    // a typeid descriptor
#define IN_CLASS_DELIM          ':'     // class delimiter
#define IN_THIS_QUAL            '.'     // this qualifier delimiter (if required)
#define IN_NO_TYPE              '_'     // this qualifier delimiter (if required)
#define IN_NAME_MEMPTR_FUNC     "MP"    // prefix for memb-ptr function names
#define IN_NAME_STATIC_INIT     "SI"    // prefix for static init
#define IN_NAME_STATIC_ONCE     "BI"    // prefix for static once only
#define IN_NAME_ARRAY_DTOR      "DA"    // prefix for array DTOR
#define IN_NAME_TYPE_SIG        "TS"    // type signature
#define IN_NAME_TYPE_STAB       "ST"    // state table for object
#define IN_NAME_STAB_CMD        "CM"    // prefix for COMDAT'ed command codes
#define IN_NAME_THROW_RO        "TH"    // prefix for throw R/O block
#define IN_NAME_DTOR_OBJ        "DO"    // prefix for object DTOR R/O block
#define IN_NAME_DEBUG_INFO      "DI"    // prefix for common debug info
#define IN_NAME_UNNAMED         "UN"    // prefix for unnamed namespace
#define IN_TYPE_SIG_ELLIPSIS    "[...]" // type name for "..."
#define IN_VIRT_SHADOW          ".vs"   // conc'ed to original name of virt. func.

#define IN_TEMPARG_INT          '0'     // integer template arg
#define IN_TEMPARG_POSITIVE_INT 'Z'     // positive int template arg
#define IN_TEMPARG_NEGATIVE_INT 'Y'     // negative int template arg
                                        // 0-9 a-t encode the number in base-32
#define IN_TEMPARG_TYPE         '1'     // template type argument

// basic types:
// 'K', 'O' are still available

#define IN_UNSIGNED     'U'
#define IN_BOOL         'Q'
#define IN_CHAR         'A'
#define IN_SCHAR        'C'
#define IN_UCHAR        'C'     // add 'U' prefix
#define IN_WCHAR        'W'
#define IN_SSHORT       'S'
#define IN_USHORT       'S'     // add 'U' prefix
#define IN_SINT         'I'
#define IN_UINT         'I'     // add 'U' prefix
#define IN_SLONG        'L'
#define IN_ULONG        'L'     // add 'U' prefix
#define IN_SLONG64      'Z'
#define IN_ULONG64      'Z'     // add 'U' prefix
#define IN_FLOAT        'B'
#define IN_DOUBLE       'D'
#define IN_LONG_DOUBLE  'T'
#define IN_POINTER      'P'
#define IN_FUNCTION     '('     // consider oo (o-open)
#define IN_FUNCTION_END ')'     // consider oc (o-close)
#define IN_ARRAY        '['     // consider ol (o-left)
#define IN_ARRAY_END    ']'     // consider or (o-right)
#define IN_DOT_DOT_DOT  'E'
#define IN_VOID         'V'


// modifiers:

#define IN_FAR          'F'
#define IN_FAR16        'G'
#define IN_HUGE         'H'
#define IN_NEAR         'N'
#define IN_VOLATILE     'Y'
#define IN_CONST        'X'
#define IN_BASED        'J'

#define IN_BASED_SELF   'S'
#define IN_BASED_VOID   'V'
#define IN_BASED_STRING 'L'
#define IN_BASED_ADD    'A'
#define IN_BASED_FETCH  'F'


// declarators:

#define IN_REFER        'R'
#define IN_MEMBER       'M'
