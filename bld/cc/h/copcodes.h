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
* Description:  Parse tree operators.
*
****************************************************************************/


//      enum                dump               cgenum
// 0x00
pick1( OPR_ADD,             "+",              O_PLUS )        // +
pick1( OPR_SUB,             "-",              O_MINUS )       // -
pick1( OPR_MUL,             "*",              O_TIMES )       // *
pick1( OPR_DIV,             "/",              O_DIV )         // /
pick1( OPR_NEG,             "-",              O_UMINUS )      // negate
pick1( OPR_CMP,             "cmp",            O_NOP )         // compare
pick1( OPR_MOD,             "%",              O_MOD )         // %
pick1( OPR_COM,             "~",              O_COMPLEMENT )  // ~
pick1( OPR_NOT,             "!",              O_FLOW_NOT )    // !
pick1( OPR_OR,              "|",              O_OR )          // |
pick1( OPR_AND,             "&",              O_AND )         // &
pick1( OPR_XOR,             "^",              O_XOR )         // ^
pick1( OPR_RSHIFT,          ">>",             O_RSHIFT )      // >>
pick1( OPR_LSHIFT,          "<<",             O_LSHIFT )      // <<
pick1( OPR_EQUALS,          "=",              O_GETS )        // lvalue = rvalue
pick1( OPR_OR_EQUAL,        "|=",             O_OR )          // |=
// 0x10
pick1( OPR_AND_EQUAL,       "&=",             O_AND )         // &=
pick1( OPR_XOR_EQUAL,       "^=",             O_XOR )         // ^=
pick1( OPR_RSHIFT_EQUAL,    ">>=",            O_RSHIFT )      // >>=
pick1( OPR_LSHIFT_EQUAL,    "<<=",            O_LSHIFT )      // <<=
pick1( OPR_PLUS_EQUAL,      "+=",             O_PLUS )        // +=
pick1( OPR_MINUS_EQUAL,     "-=",             O_MINUS )       // -=
pick1( OPR_TIMES_EQUAL,     "*=",             O_TIMES )       // *=
pick1( OPR_DIV_EQUAL,       "/=",             O_DIV )         // /=
pick1( OPR_MOD_EQUAL,       "%=",             O_MOD )         // %=
pick1( OPR_QUESTION,        "?",              O_NOP )         // ?
pick1( OPR_COLON,           ":",              O_NOP )         // :
pick1( OPR_OR_OR,           "||",             O_FLOW_OR )     // ||
pick1( OPR_AND_AND,         "&&",             O_FLOW_AND )    // &&
pick1( OPR_POINTS,          "*",              O_POINTS )      // *ptr
pick1( OPR_PUSHBACKHDL,     "pushbackhdl",    O_NOP )         // created by XCGBackName
pick1( OPR_CALLBACK,        "callback",       O_NOP )         // callback
// 0x20
pick1( OPR_POSTINC,         "++",             O_PLUS )        // lvalue++
pick1( OPR_POSTDEC,         "--",             O_MINUS )       // lvalue--
pick1( OPR_CONVERT,         "convert",        O_CONVERT )     // do conversion
pick1( OPR_PUSHSYM,         "pushsym",        O_NOP )         // push sym_handle
pick1( OPR_PUSHADDR,        "pushaddr",       O_NOP )         // push address of sym_handle
pick1( OPR_PUSHINT,         "pushint",        O_NOP )         // push integer constant
pick1( OPR_PUSHFLOAT,       "pushfloat",      O_NOP )         // push float constant
pick1( OPR_PUSHSTRING,      "pushstring",     O_NOP )         // push address of string literal
pick1( OPR_PUSHSEG,         "pushseg",        O_NOP )         // push seg of sym_handle
pick1( OPR_DUPE,            "dupe",           O_NOP )         // dupe value
pick1( OPR_CONVERT_PTR,     "convert_ptr",    O_NOP )         // convert pointer
pick1( OPR_CONVERT_SEG,     "convert_seg",    O_NOP )         // convert pointer to segment value
pick1( OPR_NOP,             "nop",            O_NOP )         // no operation
pick1( OPR_DOT,             ".",              O_NOP )         // sym.field
pick1( OPR_ARROW,           "->",             O_NOP )         // sym->field
pick1( OPR_INDEX,           "[]",             O_NOP )         // array[index]
// 0x30
pick1( OPR_ADDROF,          "&",              O_NOP )         // & expr
pick1( OPR_FARPTR,          ":>",             O_NOP )         // segment :> offset
pick1( OPR_FUNCNAME,        "funcname",       O_NOP )         // function name
pick1( OPR_CALL,            "call",           O_NOP )         // function call
pick1( OPR_CALL_INDIRECT,   "*call",          O_NOP )         // indirect function call
pick1( OPR_PARM,            ",",              O_NOP )         // function parm
pick1( OPR_COMMA,           ",",              O_NOP )         // expr , expr
pick1( OPR_RETURN,          "return",         O_NOP )         // return( expr )
pick1( OPR_LABEL,           "label",          O_NOP )         // label
pick1( OPR_CASE,            "case",           O_NOP )         // case label
pick1( OPR_JUMPTRUE,        "jumptrue",       O_NOP )         // jump if true
pick1( OPR_JUMPFALSE,       "jumpfalse",      O_NOP )         // jump if false
pick1( OPR_JUMP,            "jump",           O_NOP )         // jump
pick1( OPR_SWITCH,          "switch",         O_NOP )         // switch
pick1( OPR_FUNCTION,        "function",       O_NOP )         // start of function
pick1( OPR_FUNCEND,         "funcend",        O_NOP )         // end of function
// 0x40
pick1( OPR_STMT,            "stmt",           O_NOP )         // node for linking statements together
pick1( OPR_NEWBLOCK,        "{",              O_NOP )         // start of new block with local variables
pick1( OPR_ENDBLOCK,        "}",              O_NOP )         // end of block
pick1( OPR_TRY,             "_try",           O_NOP )         // start of try block
pick1( OPR_EXCEPT,          "_except",        O_NOP )         // start of except block
pick1( OPR_EXCEPT_CODE,     "_exception_code",O_NOP )         // __exception_code
pick1( OPR_EXCEPT_INFO,     "_exception_info",O_NOP )         // __exception_info
pick1( OPR_UNWIND,          "unwind",         O_NOP )         // unwind from try block
pick1( OPR_FINALLY,         "_finally",       O_NOP )         // finally block
pick1( OPR_END_FINALLY,     "_end_finally",   O_NOP )         // end of finally block
pick1( OPR_ERROR,           "error",          O_NOP )         // error node
pick1( OPR_CAST,            "cast",           O_NOP )         // cast type
pick1( OPR_LABELCOUNT,      "label_count",    O_NOP )         // number of labels used in function
pick1( OPR_MATHFUNC,        "mathfunc",       O_NOP )         // intrinsic math function eg. sin, cos,...
pick1( OPR_MATHFUNC2,       "mathfunc2",      O_NOP )         // intrinsic math function with 2 parms, eg. atan2
pick1( OPR_VASTART,         "vastart",        O_NOP )         // va_start (for ALPHA)
// 0x50
pick1( OPR_INDEX2,          "index2[]",       O_NOP )         // part of a multi-dimensional array
pick1( OPR_ALLOCA,          "alloca",         O_NOP )         // alloca (for ALPHA)
pick1( OPR_PATCHNODE,       "patchnode",      O_NOP )         // patch node
pick1( OPR_INLINE_CALL,     "inline_call",    O_NOP )         // call is to be made inline
pick1( OPR_TEMPADDR,        "temp_addr",      O_NOP )         // address of temp
pick1( OPR_PUSHTEMP,        "push_temp",      O_NOP )         // push value of temp
pick1( OPR_PUSH_PARM,       "push_parm",      O_NOP )         // push parm onto stack (Alpha)
pick1( OPR_POST_OR,         "post ||",        O_NOP )         // C++ "bool++" operation
pick1( OPR_SIDE_EFFECT,     "side_efect",     O_NOP )         // similar to OPR_COMMA
pick1( OPR_INDEX_ADDR,      "&index",         O_NOP )         // want the address of an index expression
pick1( OPR_DBG_BEGBLOCK,    "dbg{",           O_NOP )         // start of new block with local variables
pick1( OPR_DBG_ENDBLOCK,    "dbg}",           O_NOP )         // end of block
pick1( OPR_ABNORMAL_TERMINATION, "_abnormal_termination", O_NOP )  // SEH _abnormal_termination()
