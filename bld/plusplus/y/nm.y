/*
**********************************************************************
*       Copyright by WATCOM International Corporation, 1987, 1996.   *
*       All rights reserved. No part of this software may be         *
*       reproduced in any form or by any means - graphic, electronic,*
*       mechanical or otherwise, including, without limitation,      *
*       photocopying, recording, taping or information storage and   *
*       retrieval systems - except with the written permission of    *
*       WATCOM International Corporation.                            *
**********************************************************************

NM.Y -- future name mangling grammar?

Modified        By              Reason
--------        --              ------
96/02/08	A.F.Scian	defined

ideas:
    - try to optimize the mangling of something like:
    
    	T<p1,p2,p3>::RT T<p1,p2,p3>::mf( T<p1,p2,p3>::P1, T<p1,p2,p3>::P4 );

00000 - increment to force compile
*/

%start mangled-name

%%

mangled-name
	: import-prefix mangled-prefix full-name
	| 		mangled-prefix full-name
	;
	
import-prefix
	: '_' '_' 'i' 'm' 'p' '_'
	;
	
mangled-prefix
	: 'v' '?'			/* prefix for a full name */
	| 's' '?' truncated-hash	/* prefix for a truncated name */
	;
	
truncated-hash
	: base-36-digit base-36-digit base-36-digit base-36-digit
	;
	
full-name
	: scoped-name type
	;
	
scoped-name
	: id scope
	;
	
id
	: id-replicator
	| '@' special-id
	| simple-id '@'
	;
	
id-replicator
	: base-10-digit
	;
	
special-id
	: 'a' type			/* operator <type> */
	| 'b' scoped-name
	| 'c'
	| 'd'
	| 'e'
	| 'f'
	| 'g'
	| 'h'
	| 'i'
	| 'j'
	| 'k'
	| 'l'
	| 'm'
	| 'n'
	| 'o'
	| 'p'
	| 'q'
	| 'r'
	| 's'
	| 't'
	| 'u'
	| 'v'
	| 'w'
	| 'x'
	| 'y'
	| 'z'
	;
	
scope
	: 'a'				/* file scope */
	| 'b' id			/* final namespace scope */
	| 'c' id			/* final class scope */
	| 'd' template-scope		/* final class template scope */
	| nested-scope scope
	;
	
nested-scope
	: 'z' id			/* nested class scope <id> */
	| 'y' id			/* nested namespace scope <id> */
	| 'x' template-scope		/* nested class template scope <parms> */
	| 'w' base-16-number		/* block scope <num> */
	| 'v' id type			/* function scope <id> <type> */
	;
	
template-scope
    	: id template-parms
	;
	
template-parms
	: trailing-template-parm
	| leading-template-parm-seq trailing-template-parm
	;

leading-template-parm-seq
	: leading-template-parm
	| leading-template-parm-seq leading-template-parm
	;
	
leading-template-parm
	: 'a' base-16-number		/* +ve integer */
	| 'b' base-16-number		/* -ve integer */
	| 'c' full-name			/* &global */
	| 'd' type			/* type */
	;
	
trailing-template-parm
	: 'z' base-16-number		/* +ve integer */
	| 'y' base-16-number		/* -ve integer */
	| 'x' full-name			/* &global */
	| 'w' type			/* type */
	;
	
type
	: type-declspec type-md-seq
	;
	
type-declspec
	: 'a'				/* char */
	| 'b'				/* signed char */
	| 'c'				/* unsigned char */
	| 'd'				/* wchar_t */
	| 'e'				/* short */
	| 'f'				/* unsigned short */
	| 'g'				/* int */
	| 'h'				/* unsigned */
	| 'i'				/* long */
	| 'j'				/* unsigned long */
	| 'k'				/* __int64 */
	| 'l'				/* unsigned __int64 */
	| 'm'				/* __int128 */
	| 'n'				/* unsigned __int128 */
	| 'o'				/* float */
	| 'p'				/* double */
	| 'q'				/* long double */
	| 'r'				/* bool */
	| 's' scoped-name		/* enum <id> */
	| 't'				/* anonymous enum */
	| 'u' scoped-name		/* class <id> */
	| 'v'				/* void */
	| 'w'				/* ... */
	| 'x'				/* (nothing) */
	| 'y'				/* - */
	| 'z'				/* - */
	;
	
type-md-seq
	: 'a'				/* nothing */
	| type-md type-md-seq
	;
	
type-md
	: 'b'  				/* const */
	| 'c'				/* volatile */
	| 'd'				/* __unaligned */
	| 'e'				/* near */
	| 'f'				/* far */
	| 'g'				/* huge */
	| 'h' complex-mod		/* complex <mod> */
	| 'i'				/* - */
	| 'j'				/* - */
	| 'k'				/* - */
	| 'l'				/* - */
	| 'm'				/* - */
	| 'n'				/* - */
	| 'o'				/* - */
	| 'p'				/* - */
	| 'q'				/* - */
	| 's' this-qual args args	/* member function ( <a>* ) <tq> throw(<a>*) */
	| 't'           args args	/* function ( <a>* ) throw(<a>*) */
	| 'u' base-16-number		/* array [dim] */
	| 'v'				/* array [] */
	| 'w' scoped-name		/* member pointer <class> */
	| 'x'				/* pointer */
	| 'y'				/* reference */
	| 'z'				/* generic boundary */
	;
	
this-qual
	: tq-mod-seq tq-final
	|            tq-final
	;
	
tq-mod-seq
	: tq-mod
	| tq-mod-seq tq-mod
	;
	
tq-mod
	: 'z'				/* __unaligned */
	;
	
tq-final
	: 'a'				/* nothing */
	| 'b'				/* const */
	| 'c'				/* volatile */
	| 'd'				/* const volatile */
	;
	
complex-mod
	: 'a'				/* far16 */
	| 'b'				/* __declspec(thread) */
	| 'c'				/* __based((__segment)__self)
	| 'd'				/* __based(void) */
	| 'e' full-name			/* __based((__segment) id) */
	| 'f' full-name			/* __based(ptr) */
	| 'g' seg-name			/* __based("SEG_NAME") */
	;
	
seg-name
	: simple-id '@'			/* non-id chars replaced with _ */
	;
	
args
	: '_'
	| arg-seq '_'
	;
	
arg-seq
	: arg-type
	| arg-seq arg-type
	; 
	
arg-type
	: arg-replicator
	| type
	;
	
arg-replicator
	: base-10-digit
	;
	
simple-id
	: '_'
	| alpha
	| simple-id '_'
	| simple-id alpha
	| simple-id base-10-digit
	;
	
base-16-number
	: base-16-trailing-digit
	| base-16-leading-digit-seq base-16-trailing-digit
	;

base-16-trailing-digit
	: '0'			/* 0 */
	| '1'			/* 1 */
	| '2'			/* 2 */
	| '3'			/* 3 */
	| '4'			/* 4 */
	| '5'			/* 5 */
	| '6'			/* 6 */
	| '7'			/* 7 */
	| '8'			/* 8 */
	| '9'			/* 9 */
	| 'a'			/* a */
	| 'b'			/* b */
	| 'c'			/* c */
	| 'd'			/* d */
	| 'e'			/* e */
	| 'f'			/* f */
	;
	
base-16-leading-digit-seq
	: base-16-leading-digit
	| base-16-leading-digit-seq base-16-leading-digit
	;
	
base-16-leading-digit
	: 'g'			/* 0 */
	| 'h'			/* 1 */
	| 'i'			/* 2 */
	| 'j'			/* 3 */
	| 'k'			/* 4 */
	| 'l'			/* 5 */
	| 'm'			/* 6 */
	| 'n'			/* 7 */
	| 'o'			/* 8 */
	| 'p'			/* 9 */
	| 'q'			/* a */
	| 'r'			/* b */
	| 's'			/* c */
	| 't'			/* d */
	| 'u'			/* e */
	| 'v'			/* f */
	;
	
base-36-digit
	: base-10-digit
	| alpha
	;
	
alpha
	: 'a'
	| 'b'
	| 'c'
	| 'd'
	| 'e'
	| 'f'
	| 'g'
	| 'h'
	| 'i'
	| 'j'
	| 'k'
	| 'l'
	| 'm'
	| 'n'
	| 'o'
	| 'p'
	| 'q'
	| 'r'
	| 's'
	| 't'
	| 'u'
	| 'v'
	| 'w'
	| 'x'
	| 'y'
	| 'z'
	;
	
base-10-digit
	: '0'
	| '1'
	| '2'
	| '3'
	| '4'
	| '5'
	| '6'
	| '7'
	| '8'
	| '9'
	;
	
%%
