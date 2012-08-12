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


#define ERR_NONE                   0 // No error occured

/*----------- RERR_ -- These errors occur during when file is read -------*/
#define RERR_MIN                   1
#define RERR_INV_CHAR              2 // Invalid character encountered
#define RERR_INV_PREPROCESSOR      3 // Invalid preprocessor directive
#define RERR_EOF_IN_COMMENT        4 // EOF encountered inside a comment
#define RERR_EOF_IN_STRING         5 // EOF encountered inside a string
#define RERR_INV_CHAR_CONST        6 // Invalid char constant
#define RERR_INV_INT_CONST         7 // Invalid integer constant
#define RERR_INV_FLOAT_CONST       8 // Invalid floating point constant
#define RERR_INV_STRING_CONST      9 // Invalid string constant
#define RERR_INV_TYPE_COMBO        10 // Invalid types/specifiers/qualifiers combination"
#define RERR_2_OR_MORE_ARG_LISTS   11 // Only one argument list is allowed per function
#define RERR_CANT_HAVE_ARR_OF_FUNC 12 // Arrays of functions are not allowed
#define RERR_FUNC_CANT_RET_ARR     13 // Function cannot return an array
#define RERR_CPARSE_WITH_CONTEXT   14 // Parser Syntax error. Context: %s %s
#define RERR_INCOMPLETE_TYPE       17 // Incomplete type
#define RERR_DCLR_NOT_PRESENT      18 // Declarator is missing
#define RERR_DCLR_CANT_BE_INIT_TWICE 19 //Declarator cannot be initialized twice
#define RERR_INV_MEM_MODEL_COMBO   20 // Invalid memory model combination
#define RERR_CANT_COMBINE_2_TOKENS  21 // Cannot combine %s and %s
#define RERR_TYPEDEF_CANT_BE_INITIALIZED  22 // Cannot initialize typedef
#define RERR_CANT_CONV_PARAM_OF_FUNC  23 // Cannot convert a parameter to function
#define RERR_SYNTAX_PRE_PARSER     24 // Syntax error while preprocessing
#define RERR_TYPEDEF_REDECLARED    25 // Cannot redefine symbol '%s'
#define RERR_MISSING_RIGHT_BRACE   26 // Missing '}'
#define RERR_INV_INCLUDE_FILE_NAME 27 // Invalid include file name
#define RERR_INCOMPLETE_ENUM_DECL  28 // Incomplete enum declaration
#define RERR_NOT_ENOUGH_ARGS       29 // Not enough arguments for %s
#define RERR_EXPECTING_BUT_FOUND   30 // Expecting %s but found %s
#define RERR_MAX                   31

/*-------- CERR_ -- These occur during conversion/output process ----------*/
#define CERR_MIN                   100
#define CERR_PREDIR_NOT_SUPPORTED  101 //Cannot convert preprocessor directive '%s'
#define CERR_CANT_CONVERT_BIT_FIELDS 102 // Cannot convert bit fields
#define CERR_CANT_CONVERT_QUALIFIER 103 // Cannot convert 'const' or 'volatile' qualifiers
#define CERR_EXTERN_NOT_SUPPORTED  104 // External variables not supported: '%s'
#define CERR_FORW_DEFN_NOT_ALLOWED 105 // Cannot convert forward definition: '%s'
#define CERR_REDEFN_IGNORED        106 // Redefinition of symbol '%s' igonred
#define CERR_CANT_CONVERT          107 // Cannot convert '%s'
#define CERR_MAX                   108

/*---------------- ERR_ -- These are general errors --------------*/
#define ERR_MIN                    200
#define ERR_OPEN_FILE              201 // Could not open file '%s'
#define ERR_CLOSE_FILE             202 // Could not close file. Reason: %s
#define ERR_COULD_NOT_SEEK         203 // Could not seek file '%s'. Reason: %s
#define ERR_FILE_WAS_INCLUDED      204 // File '%s' was already included
#define ERR_NEST_INCLUDE           205 // Maximum nested #include level exceeded
#define ERR_INC_FILE_NOT_FOUND     206 // Include file '%s' not found
#define ERR_INTERNAL_STACK_OVERFLOW 207 // Internal stack overflow
#define ERR_LINE_LEN_OUT_OF_RANGE  208 // Line length of %d is out of range
#define ERR_INV_CMD_LINE_OPTION    209 // Invalid option: %c
#define ERR_MAX                    210

/*----- FATAL_ -- These errors cause the WIC to perform a suicide ---*/
#define FATAL_MIN                  300
#define FATAL_FILE_NOT_SPECIFIED   301 // File not specified
#define FATAL_INIT_FAILED          302 // Initialization failed
#define FATAL_PARSER_FAILED        303 // Parser failed
#define FATAL_OUT_OF_MEM           304 // Out of memory
#define FATAL_INTERNAL             305 // Internal Error
#define FATAL_INTERNAL_ASSERTION_FAILED 306 // Assertion failed
#define FATAL_DIE                  307 // Could not recover
#define FATAL_MAX                  308

typedef int     WicErrors;  // This used to be enum

#define MSG_USAGE_BASE             400

#define MSG_LANG_SPACING           1000

#define MAX_RESOURCE_SIZE          200  // Max size of any string
