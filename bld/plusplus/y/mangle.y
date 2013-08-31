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

Modified        By              Reason
--------        --              ------
93/01/14        Greg Bentz      ground out with sweat and blood.
93/04/14        A.F.Scian       added truncated mangled names
93/07/23        A.F.Scian       reserved names for operator new [] and
                                operator delete [] (recently passed by ANSI)
94/08/19        A.F.Scian       added bool basic type
******************************************************************************/

%token  ignore-stuff

%start  full-mangled-name

%%

full-mangled-name
    : 'W' '?' mangled-name
    | 'T' '?' truncate-hash mangled-name
    ;
    
truncate-hash
    : base-36 base-36 base-36 base-36
    ;

embedded-mangled-name
    : '?' mangled-name
    ;
    
mangled-name
    : scoped-name type-encoding
    ;

scoped-name
    : sym-name
    | sym-name scopes
    ;

sym-name
    : name '$'
    | op-name
    ;

name
    : id_char
    | id_char identifier
    | replicator
    ;

identifier
    : id_char 
    | numeric
    | id_char identifier
    | numeric identifier
    ;

id_char
    : alpha
    | '_'
    | '.'
    ;

replicator
    : numeric
    ;

op-name
    : '$' op-code
    ;

op-code
    : ctor
    | dtor
    | new
    | array-new
    | delete
    | array-delete
    | user-conversion
    | assignment-function
    | relational-function
    | operator-function
    | special-type-names
    | watcom-object
    ;

ctor
    : 'C' 'T'
    ;

dtor
    : 'D' 'T'
    ;

new
    : 'N' 'W'
    ;

array-new
    : 'N' 'A'
    ;

delete
    : 'D' 'L'
    ;

array-delete
    : 'D' 'A'
    ;

user-conversion
    : 'C' 'V'
    ;

assignment-function
    : 'A' 'A' /* = */
    | 'A' 'B' /* *= */
    | 'A' 'C' /* += */
    | 'A' 'D' /* -= */
    | 'A' 'E' /* /= */
    | 'A' 'F' /* %= */
    | 'A' 'G' /* >>= */
    | 'A' 'H' /* <<= */
    | 'A' 'I' /* &= */
    | 'A' 'J' /* |= */
    | 'A' 'K' /* ^= */
    ;

relational-function
    : 'R' 'A' /* == */
    | 'R' 'B' /* != */
    | 'R' 'C' /* < */
    | 'R' 'D' /* <= */
    | 'R' 'E' /* > */
    | 'R' 'F' /* >= */
    ;

operator-function
    : 'O' 'A'   /* >> */
    | 'O' 'B'   /* << */
    | 'O' 'C'   /* ! */
    | 'O' 'D'   /* [] */
    | 'O' 'E'   /* -> */
    | 'O' 'F'   /* * */
    | 'O' 'G'   /* ++ */
    | 'O' 'H'   /* -- */
    | 'O' 'I'   /* - */
    | 'O' 'J'   /* + */
    | 'O' 'K'   /* & */
    | 'O' 'L'   /* ->* */
    | 'O' 'M'   /* / */
    | 'O' 'N'   /* % */
    | 'O' 'O'   /* , */
    | 'O' 'P'   /* () */
    | 'O' 'Q'   /* ~ */
    | 'O' 'R'   /* ^ */
    | 'O' 'S'   /* | */
    | 'O' 'T'   /* && */
    | 'O' 'U'   /* || */
    ;

special-type-names
    : 'T' 'E'   /* anonymous enum type */
    ;

watcom-object
    : 'W' alpha alpha zz-len ignore-stuff '$'
    ;

zz-len  /* zz == 1296, reserved for future use */
    : base-36 base-36
    ;

scopes
    : scope
    | scopes scope
    ;

scope
    : ':' sym-name
    | ':' embedded-mangled-name
    | ':' template-name
    ;

template-name
    : ':' template-arg-list
    ;

template-arg-list
    : template-arg
    | template-arg-list template-arg
    ;

template-arg
    : '0' base-32-num 'Y'   /* negative number */
    | '0' base-32-num 'Z'   /* positive number */
    | '1' type-encoding
    | embedded-mangled-name
    ;

type-encoding
    : unmodified-type
    | modifier-list unmodified-type
    ;

modifier-list
    : modifier
    | modifier-list modifier
    ;

modifier
    : 'F'                   /* far */
    | 'G'                   /* far16 */
    | 'H'                   /* huge */
    | 'J' based-encoding    /* based */
    | 'N'                   /* near */
    | 'U'                   /* unsigned */
    | 'X'                   /* const */
    | 'Y'                   /* volatile */
    ;

based-encoding
    : 'S'
    | 'V'
    | 'L' zz-len ignore-stuff '$'
    | 'A' '$' scoped-name 
    | 'F' '$' scoped-name 
    ;

unmodified-type
    : '$' scoped-name '$'
    | function
    | tq-function
    | array
    | pointer type-encoding
    | basic-type
    ;

tq-function
    : '.' modifier-list function
    ;

function
    : '(' type-encoding-list ')' type-encoding
    ;

type-encoding-list
    : type-encoding
    | type-encoding type-encoding-list
    ;

array
    : dimension type-encoding
    ;

dimension
    : '[' ']'
    | '[' base-10-num ']'
    ;

pointer
    : 'M' '$' scoped-name   /* member pointer */
    | 'P'                   /* pointer */
    | 'R'                   /* reference */
    ;

basic-type
    : 'A'       /* char */
    | 'B'       /* float */
    | 'C'       /* signed char (unsigned char if preceeded by 'unsigned' */
    | 'D'       /* double */
    | 'E'       /* ... */
    | 'I'       /* int */
    | 'L'       /* long */
    | 'Q'       /* bool */
    | 'S'       /* short */
    | 'T'       /* long double */
    | 'V'       /* void */
    | 'W'       /* wchar_t */
    | '_'       /* <no type> for CTOR/DTOR return type */
    ;

alpha
    : 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j' | 'k'
    | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't' | 'u' | 'v'
    | 'w' | 'x' | 'y' | 'z'
    | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J' | 'K'
    | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T' | 'U' | 'V'
    | 'W' | 'X' | 'Y' | 'Z'
    ;

numeric
    : '0' | '1' | '2' | '3' | '4' | '5' | '6' | '7' | '8' | '9'
    ;

base-10-num
    : numeric
    | numeric base-10-num
    ;

base-32-num
    : base-32
    | base-32 base-32-num
    ;

base-32
    : numeric
    | 'a' | 'b' | 'c' | 'd' | 'e' | 'f' | 'g' | 'h' | 'i' | 'j'
    | 'k' | 'l' | 'm' | 'n' | 'o' | 'p' | 'q' | 'r' | 's' | 't'
    | 'u' | 'v'
    | 'A' | 'B' | 'C' | 'D' | 'E' | 'F' | 'G' | 'H' | 'I' | 'J'
    | 'K' | 'L' | 'M' | 'N' | 'O' | 'P' | 'Q' | 'R' | 'S' | 'T'
    | 'U' | 'V'
    ;

base-36
    : numeric | alpha
    ;
    
%%
