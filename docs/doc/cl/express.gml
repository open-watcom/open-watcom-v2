.ix 'expression'
.ix 'operator'
.ix 'operand'
.pp
An
.us expression
is a sequence of operators and operands that
describes how to,
.begbull $compact
.bull
calculate a value (eg. addition)
.bull
create side-effects (eg. assignment, increment)
.endbull
.pc
or both.
.pp
The order of execution of the expression is usually determined by
a mixture of,
.autonote
.note
.ix 'parentheses'
parentheses
.mono ()
..ct , which indicate to the compiler the desired grouping
of operations,
.note
.ix 'precedence'
.ix 'priority of operators'
.ix 'operator' 'priority'
.ix 'operator' 'precedence'
the precedence of operators, which describes the relative priority
of operators in the absence of parentheses,
.note
the common algebraic ordering,
.note
the associativity of operators.
.ix 'operator' 'associativity'
.ix 'associativity of operators'
.endnote
.pp
In most other cases, the order of execution is determined by the
compiler and may not be relied upon. Exceptions to this rule are
described in the relevant section.
Most users will find that the order of execution is well-defined
and intuitive.
However, when in doubt, use parentheses.
.pp
The table below summarizes the levels of precedence in expressions.
.im orderop
.im orddesc
.pp
An
.us exception
occurs when the operands for an operator are invalid. For example,
division by zero may cause an exception.
If an exception occurs, the behavior is undefined.
If an exception is a possibility, the program should be prepared
to handle it.
.pp
In the following sections, a formal syntax is used to describe
each level in the precedence table. This syntax is used in order
to completely describe the relationships between the various levels.
.*
.section Lvalues
.*
.pp
In order to understand certain components of expressions,
it is important to understand
the term
.ix 'lvalue'
.us lvalue
..ct ..li .
.pp
An
.us lvalue
is an expression that designates an object.
The simplest form of
.us lvalue
is an identifier which is an object
(for example, an integer).
.pp
The type of the expression
may not be
.kw void
or a function. The term
.us lvalue
is derived from
.us l
..ct eft
.us value,
which refers to the fact that an
.us lvalue
is typically on the left side of an assignment expression.
.pp
If
.mono ptr
is a pointer to a type other than
.kw void
or a function, then both
.mono ptr
and
.mono *ptr
are
.us lvalues.
.pp
A
.ix 'lvalue' 'modifiable'
.ix 'modifiable lvalue'
.us modifiable lvalue
is an
.us lvalue
whose type is not an array or an incomplete type,
whose declaration does not contain the keyword
.kw const
..ct ,
and, if it is a structure or union, then none of its members contains
the keyword
.kw const
..ct ..li .
.*
.* .keep begin
.section Primary Expressions
.*
.ix 'primary expression'
.ix 'expression' 'primary'
.ix 'production'
.uillust begin
primary-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.identifier
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.constant
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.string-literal
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.:HP0.(:eHP0. expression :HP0.):eHP0.
.uillust end
.* .keep end
.pp
A
.us primary expression
is the simplest part of an expression. It consists of one of
the following:
:ZDL break termhi=0.
:ZDT.identifier
:ZDD.
An identifier that designates a function is called a
.ix 'function' 'designator'
.us function designator.
An identifier that designates an object
is an
.ix 'lvalue'
.us lvalue.
:ZDT.constant
:ZDD.
A constant is a primary expression whose type depends on its form.
See "Constants".
:ZDT.string-literal
:ZDD.
.ix 'string literal'
.ix 'string'
A string literal is a primary expression whose
type is
"array of
.kw char
..ct ".
A string literal is also an
.us lvalue
(but is not modifiable).
:ZDT.expression inside parentheses
:ZDD.
The type and value of a parenthesized expression are the same as
for the expression without parentheses. It may be an
.us lvalue,
function designator or void expression.
:ZeDL.
.keep begin
.pp
Given these declarations,
.millust begin
int     count;
int *   ctrptr;
int     f( int );
int     g( int );
.millust end
.keep end
.pc
the following are all valid primary expressions:
.millust begin
count
3
3.2
'a'
"Hello there"
(count + 3)
(*(ctrptr+1))
(f( ++i ) * g( j++ ))
.millust end
.*
.keep begin
.*
.section Postfix Operators
.*
.ix 'operator' 'postfix'
.ix 'postfix operator'
.uillust begin
postfix-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.primary-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.array-subscripting-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.function-call-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.member-designator-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.post-increment-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.post-decrement-expression
.uillust end
.keep end
.beglevel
.*
.* .keep begin
.section Array Subscripting
.*
.ix 'array' 'subscripting'
.ix 'subscript'
.cillust begin
array-subscripting-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.postfix-expression:MONO.[:eMONO.expression:MONO.]:eMONO.
.cillust end
.* .keep end
.pp
The general form for array subscripting is,
.millust array[index]
where
.mono array
must have the type "array of
.us type
..ct " or
"pointer to
.us type
..ct ", and
.mono index
must have an integral type. The result has
type "
..ct .us type
..ct ".
.pp
.mono array[index]
is equivalent to
.mono (*(array+index))
..ct ,
or the
..se hyphenmode=&syshy
..hy off
.mono index
..ct -th
..hy &hyphenmode
element of the array
.mono array
..ct ,
where the first
element is numbered zero. Note that
.mono index
is scaled automatically
to account for the size of the elements of
.mono array.
.pp
An alternate form for array subscripting is,
.millust index[array]
although this form is not commonly used.
.*
.keep begin
.section Function Calls
.*
.ix 'function' 'call'
.ix 'calling a function'
.cillust begin
function-call-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.postfix-expression:MONO.():eMONO.
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.postfix-expression
:MONO.(:eMONO.argument-expression-list:MONO.):eMONO.
.csk
argument-expression-list:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.:HP0.one or more:eHP0.
assignment-expression:HP0.s separated by commas:eHP0.
.cillust end
.keep end
.pp
A
.us postfix-expression
followed by parentheses containing zero or more comma-separated
expressions is a
.us function-call-expression
..ct ..li .
The postfix-expression denotes the function to be called, and must
evaluate to a pointer to a function.
.* returning
.* .kw void
.* or returning an object other than an array.
The simplest form of this expression is an identifier which is the name
of a function. For example,
.mono Fn()
calls the function
.mono Fn.
.pp
The expressions within the parentheses
denote the arguments to the function.
If a function
prototype has been declared, then the number of arguments must match
the parameter list in the prototype, and the arguments are converted
to the types specified in the prototype.
.pp
If the postfix-expression is simply an identifier, and no function
prototype declaration for that identifier
is in scope, then an implicit,
.cillust begin
.mono extern int
identifier:MONO.():eMONO.:MSEMI.
.cillust end
.pc
declaration is placed in the innermost block containing the
function call. This declares the function as having external
linkage,
.ix 'external linkage'
.ix 'linkage' 'external'
no information about its parameters is available, and the function
returns an integer.
.pp
The expressions are evaluated (in an undefined order) and the values
assigned to the parameters for the function. All arguments are passed
by value, allowing the function to modify its parameters without
affecting
the arguments used to create the parameters. However, an argument
can be a pointer to an object, in which case the function may
modify the object to which the pointer points.
.pp
If a function prototype is in scope at both a call to a function
and its definition (and if the prototypes are the same), then the
compiler will ensure that the required number and type of parameters
are present.
.pp
If no function prototype is in scope at a call to a function,
then the
.us default argument promotions
are performed.
(Integral types such as
.kw char
and
.kw short int
are converted to
.kw int
..ct ,
while
.kw float
values are converted to
.kw double
..ct ..li .)
When the function definition is encountered, if the parameter
types do not match the default argument promotions, then
the behavior is undefined. (Usually, the parameters to the function
will receive incorrect values.)
.pp
If a function prototype has been declared at a call to a function,
then each argument is converted, as if by assignment, to the type
of the corresponding parameter. When the function definition is
encountered, if the types of the parameters do not match the types
of the parameters in the function prototype, the behavior is
undefined.
.pp
If the
.ix 'ellipsis'
ellipsis (
..ct .mono ,...
..ct )
notation is used in a function prototype,
then those arguments in a function call that correspond to the
ellipsis have only the default argument promotions performed on
them.
(See the chapter "Functions" for a complete description of the
ellipsis notation.)
.pp
Function calls may be
..ix 'function' 'recursion'
..ix 'recursion'
recursive.
Functions
may call themselves either directly, or via other functions.
.pp
The following are some examples of function calls:
.millust begin
putchar( 'x' );
chr = getchar();
valid = isdigit( chr );
printf( "chr = %c, valid = %2x\n", chr, valid );
fnptr = &MyFunction;
(*fnptr)( parm1, parm2 );
fnptr( parm1, parm2 );
.millust end
.*
.* .keep begin
.section Structure and Union Members
.*
.ix 'structure' 'member'
.ix 'union' 'member'
.ix 'member' 'of structure'
.ix 'member' 'of union'
.uillust begin
member-designator-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.postfix-expression . identifier
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.postfix-expression:MONO.->:eMONO.identifier
.uillust end
.* .keep end
.pp
The first operand of the .
operator
.ix 'dot operator'
.ix 'operator' 'dot'
.ix 'operator' '.'
must be an object with a structure or union type. The second operand
must be the name of a member of that type.
The result is the value of the member, and is an lvalue if the
first operand is also an lvalue.
.pp
The first operand of the
.mono ->
operator
.ix 'arrow operator'
.ix 'operator' 'arrow'
.ix 'operator' '->'
must be a pointer to an
object with a structure or union type. The second operand
must be the name of a member of that type.
The result is the value of the member
of the structure or union to which
the first expression points, and is an lvalue.
.*
.keep begin
.section Post-Increment and Post-Decrement
.*
.ix 'post-increment'
.ix 'post-decrement'
.ix 'increment'
.ix 'decrement'
.ix 'operator' 'post-increment'
.ix 'operator' '++'
.ix 'operator' 'post-decrement'
.ix 'operator' '--'
.uillust begin
post-increment-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.postfix-expression:MONO.++:eMONO.

post-decrement-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.postfix-expression:MONO.--:eMONO.
.uillust end
.keep end
.pp
The operand of post-increment and post-decrement must be a modifiable
lvalue, and a scalar (not a structure, union or array).
.pp
The effect of the operation is that
the operand is incremented or decremented by 1, adjusted for the
type of the operand. For example, if the operand is declared to be
a "pointer to
.us type
..ct ", then the increment or decrement will be by the value
.monoon
sizeof(&SYSRB.:HP1.type:eHP1.&SYSRB.)
.monooff
..ct ..li .
.pp
The result of both post-increment and post-decrement (if it is just
a subexpression of a larger expression)
is the original,
unmodified value of the operand.
In other words, the original value of the operand is used in the
expression, and then it is incremented or decremented.
Whether the operand is incremented immediately after use or after
completion of execution of the expression is undefined. Consider the
statements,
.millust begin
int i = 2;
int j;

j = (i++) + (i++);
.millust end
.pc
Depending on the compiler,
.mono j
may get the value 4 or 5. If the increments are delayed until after
the expression is evaluated,
.mono j
gets the value
.monoon
2&SYSRB.+&SYSRB.2.
.monooff
If the increment of
.mono i
happens immediately after its value is retrieved, then
.mono j
gets the value
.monoon
2&SYSRB.+&SYSRB.3.
.monooff
.* .*
.* .*********************************************************************
.* .*
.* ..if '&target' eq 'PC' ..th ..do begin
.* .shade begin
.* The &wcboth. compilers do post-increment and decrement immediately,
.* so the result of the above expression will be 5.
.* No program should rely upon this order.
.* .shade end
.* ..do end
.* ..if '&target' eq 'PC 370' ..th ..do begin
.* .shade begin
.* The &wcall. compilers
.* do post-increment and decrement immediately,
.* so the result of the above expression will be 5.
.* No program should rely upon this order.
.* .shade end
.* ..do end
.* .*
.* .*********************************************************************
.*
.keep begin
.pp
To avoid ambiguity, the above expression could be written as:
.millust begin
j = i + i;
i += 2;
.millust end
.keep end
.endlevel
.*
.keep begin
.section Unary Operators
.*
.ix 'operator' 'unary'
.ix 'unary operator'
.uillust begin
unary-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.postfix-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.pre-increment-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.pre-decrement-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.unary-operator cast-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.sizeof-expression
.uillust end
.keep end
.cillust begin
.us unary-operator:
one of
.cbr
.monoon
&SYSRB.&SYSRB.&SYSRB.&SYSRB.& * + - ~~ !
.monooff
.cillust end
.beglevel
.*
.* .keep begin
.section Pre-Increment and Pre-Decrement Operators
.*
.ix 'increment'
.ix 'pre-increment'
.ix 'decrement'
.ix 'pre-decrement'
.ix 'operator' 'pre-increment'
.ix 'operator' '++'
.ix 'operator' 'pre-decrement'
.ix 'operator' '--'
.uillust begin
pre-increment-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.:MONO.++:eMONO. unary-expression

pre-decrement-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.:MONO.--:eMONO. unary-expression
.uillust end
.* .keep end
.pp
The operand of the pre-increment and pre-decrement operators must be
a modifiable lvalue, and a scalar (not a structure, union or array).
.pp
The operand is incremented or decremented by 1, adjusted for the
type of the operand.
For example, if the operand is declared to be
a "pointer to
.us type
..ct ", then the increment or decrement will be by the value
.monoon
sizeof(&SYSRB.:HP1.type:eHP1.&SYSRB.)
..ct ..li .
.monooff
.pp
The expression
.mono ++obj
is equivalent to
.mono (obj&SYSRB.+=&SYSRB.1)
..ct ,
while
..se hyphenmode=&syshy
..hy off
.mono --obj
..hy &hyphenmode
is equivalent to
.mono (obj&SYSRB.-=&SYSRB.1).
.*
.section Address-of and Indirection Operators
.*
.uillust begin
unary-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.:MONO.&:eMONO. cast-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.:MONO.*:eMONO. cast-expression
.uillust end
.pp
The unary
.mono &
symbol denotes the
.ix 'operator' 'address-of'
.ix 'operator' 'unary &'
.ix 'operator' '&'
.ix 'unary operator' '&'
.ix 'address-of operator'
.us address-of
operator.
Its operand must designate a
function or an array, or be an lvalue that designates an
object that is not a bit-field and is not declared with the
.kw register
storage-class specifier.
If the type of the operand is "
..ct .us type
..ct ",
then the type of the result is "pointer to
.us type
..ct " and the result is the address of the operand.
.pp
If the type of the operand is "array of
.us type
..ct ", then the type of the result is "pointer to
.us type
..ct " and the result is the address of the first element of the array.
.pp
The
.mono *
symbol, in its unary form, denotes the
.ix 'operator' 'indirection'
.ix 'operator' 'unary *'
.ix 'operator' '*'
.ix 'unary operator' '*'
.ix 'indirection operator'
.us indirection
or
.ix 'operator' 'pointer'
.ix 'pointer operator'
.us pointer
operator. Its operand must be a pointer type, except that it may not
be a pointer to
.kw void
..ct ..li .
If the operand is a "pointer to
.us type
..ct ", then the type of the result is "
..ct .us type
..ct ",
and the result is the object to which the operand points.
.pp
No checking is performed to ensure that the value of the pointer is
valid. If an invalid pointer value is used, the behavior of
.mono *
is
undefined.
.keep begin
..sk 1 c
Examples:
.millust begin
int   counter;
int * ctrptr;
void  (*fnptr)( int, int * );

ctrptr  = &counter;
*ctrptr = 3;

fnptr = FnRetVoid;
fnptr( *ctrptr, &counter );
.millust end
.keep end
.*
.section Unary Arithmetic Operators
.*
.uillust begin
unary-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.:MONO.+:eMONO. cast-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.:MONO.-:eMONO. cast-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.:MONO.~~:eMONO. cast-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.:MONO.!:eMONO. cast-expression
.uillust end
.pp
The
.mono +
symbol, in its unary form,
.ix 'plus' 'unary'
.ix 'unary operator' '+'
.ix 'unary operator' 'plus'
.ix 'operator' 'plus'
.ix 'plus operator'
simply returns the value of its operand.
The type of its operand must be an arithmetic type
(character, integer or floating-point).
Integral promotion is performed on the operand, and the result
has the promoted type.
.pp
The
.mono -
symbol, in its unary form, is the
.us negation
or
.ix 'unary operator' '-'
.ix 'unary operator' 'minus'
.ix 'unary operator' 'negative'
.ix 'negative' 'unary'
.ix 'minus' 'unary'
.ix 'operator' 'negative'
.ix 'operator' 'unary minus'
.us negative
operator.
The type of its operand must be an arithmetic type
(character, integer or floating-point). The result is the
negative of the operand. Integral promotion is performed on
the operand, and the result has the promoted type.
The expression
.mono -obj
is equivalent to
.mono (0-obj).
.pp
The
.mono ~~
symbol is the
.ix 'tilde'
.ix 'bitwise complement'
.ix 'ones complement'
.ix 'bitwise NOT'
.ix 'complement operator'
.ix 'NOT operator' 'bitwise'
.ix 'operator' '~~'
.ix 'operator' 'complement'
.ix 'operator' 'bitwise complement'
.ix 'operator' "1's complement"
.ix 'operator' 'not'
.ix 'operator' 'bitwise NOT'
.us bitwise complement, 1's complement
or
.us bitwise not
operator.
The type of the operand must be an integral type, and integral
promotion is performed on the operand. The type of the result is the
type of the promoted operand.
Each bit of the result is the complement of the corresponding bit in
the operand, effectively turning 0 bits to 1, and 1 bits to 0.
.*
.************************************************************************
.*
.* ..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.* .shade begin
.* With &wc286.,
.* if
.* .mono obj
.* is declared to be
.* .kw unsigned long
.* ..ct ,
.* then
.* .mono ~~obj
.* is equivalent to
.* .mono (4294967295U-obj)
.* ..ct ..li .
.* If
.* .mono obj
.* is declared to be any other unsigned type,
.* then
.* .mono ~~obj
.* is equivalent to
.* .mono (65535U-obj)
.* ..ct ..li .
.* .pp
.* With &wc386.,
.* if
.* .mono obj
.* is declared to be
.* .kw unsigned int
.* or
.* .kw unsigned long
.* ..ct , then
.* .mono ~~obj
.* is equivalent to
.* .mono (4294967295U-obj)
.* ..ct ..li .
.* If
.* .mono obj
.* is declared to be any other unsigned type,
.* then
.* .mono ~~obj
.* is equivalent to
.* .mono (65535U-obj)
.* ..ct ..li .
.* .shade end
.* ..do end
.* ..if '&target' eq 'PC 370' ..th ..do begin
.* .shade begin
.* With &wlooc.,
.* .mono ~~obj
.* is equivalent to
.* .mono (4294967295U-obj)
.* ..ct ..li .
.* .shade end
.* ..do end
.*
.************************************************************************
.*
.pp
The
.mono !
symbol is the
.ix 'logical NOT'
.ix 'NOT operator' 'logical'
.ix 'operator' '!'
.ix 'operator' 'logical NOT'
.us logical not
operator.
Its operand must be a scalar type (not a structure, union or array).
The result type is
.kw int
..ct ..li .
If the operand has the value zero, then the result value is 1.
If the operand has some other value, then the result is 0.
.*
.* .keep begin
.section The sizeof Operator
.*
.cillust begin
sizeof-expression:
.cbr
.monoon
&SYSRB.&SYSRB.&SYSRB.&SYSRB.sizeof
.monooff
unary-expression
.cor
.monoon
&SYSRB.&SYSRB.&SYSRB.&SYSRB.sizeof(
.monooff
type-name
.mono )
.cillust end
.* .keep end
.pp
The
.ix 'sizeof operator'
.ix 'operator' 'sizeof'
.kw sizeof
operator gives the size (in bytes) of its operand.
The operand may be an expression,
or a type in parentheses.
In either case, the type must not be a function, bit-field
or incomplete type (such as
.kw void
..ct ,
or an array that has not had
its length declared).
.pp
Note that an expression operand to
.kw sizeof
is not evaluated. The expression is examined to determine the result
type, from which the size is determined.
.pp
If the operand has a character type, then the result is 1.
.pp
If the type is a structure or union, then the result is the total
number of bytes in the structure or union, including any internal or
trailing padding included by the compiler for alignment purposes.
The size of a structure can be greater than the sum of the
sizes of its members.
.pp
If the type is an array, then the result is the total number of bytes
in the array, unless the operand is a parameter in the function
definition enclosing the current block,
in which case the result is the size of a pointer.
.pp
The type of the result of the
.kw sizeof
operator is implementation-defined,
but it is an unsigned integer type, and is represented by
.ix 'size_t'
.kw size_t
in the
.hdr <stddef.h>
header.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
For the &wcboth. compilers, the macro
.ix 'size_t'
.kw size_t
is
.kw unsigned int
..ct ..li .
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
For the &wlooc. compiler, the macro
.ix 'size_t'
.kw size_t
is
.kw unsigned int
..ct ..li .
.shade end
..do end
.*
.************************************************************************
.*
.keep begin
..sk 1 c
Example:
.millust begin
struct s {
    struct s * next;
    int        obj1;
    int        obj2;
};
.millust end
.keep break
.millust begin
static struct s * SAllocAndFill( const struct s * def_s )
/*******************************************************/
{
    struct s * sptr;

    sptr = malloc( sizeof( struct s ) );
    if( sptr != NULL ) {
        memcpy( sptr, def_s, sizeof( struct s ) );
    }
    return( sptr );
}
.millust end
.keep end
.pc
The function
.mono SAllocAndFill
receives a pointer to a
.mono struct&SYSRB.s.
It allocates such a structure, and copies
the contents of the structure pointed to by
.mono def_s
into the allocated memory. A pointer to the allocated structure is
returned.
.pp
The library function
.libfn malloc
takes the number of bytes to allocate as a parameter and
.monoon
sizeof(&SYSRB.struct&SYSRB.s&SYSRB.)
.monooff
provides that value. The library function
.libfn memcpy
also takes, as the third parameter, the number of bytes to copy
and again
.monoon
sizeof(&SYSRB.struct&SYSRB.s&SYSRB.)
.monooff
provides that value.
.endlevel
.*
.* .keep begin
.section Cast Operator
.*
.uillust begin
cast-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.unary-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.:HP0.(:eHP0. type-name :HP0.):eHP0. cast-expression
.uillust end
.* .keep end
.pp
When an expression is preceded by a type name
in parentheses, the value of the expression is converted to the
named type. This is called a
.ix 'cast operator'
.ix 'operator' 'cast'
.ix 'converting types explicitly'
.us cast.
Both the type name and the operand type must be scalar (not a
structure, union or array), unless the type name is
.kw void
..ct ..li .
If the type name is
.kw void
..ct ,
the operand type must be a complete type
(not an array of unknown size, or a structure or union that has
not yet been defined).
.pp
A cast does not yield an lvalue.
.pp
Pointers may be freely converted from "pointer to
.ix 'pointer' 'to void'
.ix 'void' 'pointer to'
.kw void
..ct " to any other pointer type without
using an explicit cast operator.
Pointers also may be converted
from any pointer type to "pointer to
.kw void
..ct ".
.pp
A pointer may be converted to a pointer to another type.
However,
the pointer may be invalid
if the resulting pointer is not properly
.ix 'alignment'
aligned for
the type.
Converting a pointer to a pointer to a type with less strict
alignment, and back again, will yield the same pointer.
However, converting it to a pointer to a type with more strict
alignment, and back again, may yield a different pointer.
On many computers, where alignment is not required (but may improve
performance), conversion of pointers may take place freely.
.*
.**********************************************************************
.*
.shade begin
With
..if '&target' eq 'PC' ..th ..do begin
&wcboth.,
..do end
..el ..if '&target' eq 'PC 370' ..th .do begin
&wcall.,
..do end
..el ..if '&target' eq '370' ..th ..do begin
&wlooc.,
..do end
alignment of integers, pointers
and floating-point numbers is not
required, so the compiler does not do any alignment.
However,
aligning these types may make a program run slightly faster.
..if '&target' eq 'PC 370' ..th ..do begin
.pp
With both compilers, a
..do end
..el ..if '&target' eq 'PC' ..th ..do begin
.pp
A
..do end
command line switch may be used to force the compiler
to do alignment on all structures.
.shade end
.*
.**********************************************************************
.*
.pp
A pointer to a function may be converted to a pointer to a different
type of function, and back again. The resulting pointer will be the
same as the original pointer.
.pp
If a pointer is converted to a pointer
to a different type of function, and a call is made using that
pointer, the behavior is undefined.
.* (If the number and types of parameters and
.* the type of the return value are all identical, then there shouldn't
.* be a problem.)
.pp
A pointer may be converted to an integral type. The type of integer
required to hold the value of the pointer is implementation-defined.
If the integer is not large enough to fully contain the value, then
the behavior is undefined.
.pp
An integer may be converted to a pointer. The result is
implementation-defined.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wc286.,
for the purposes of conversion between pointers and integers,
.kwix &kwnear_sp.
.kwfont &kwnear.
pointers are treated as
.kw unsigned int
..ct ..li .
.kwix &kwfar_sp.
.kwfont &kwfar.
and
.kwix &kwhuge_sp.
.kwfont &kwhuge.
pointers are treated as
.kw unsigned long int
..ct ,
with the pointer's segment value in the high-order
(most significant)
two bytes.
All the usual integer conversion rules then apply.
Note that
huge
pointers
are not normalized in any way.
.shade end
.shade begin
With &wc386.,
for the purposes of conversion between pointers and integers,
.kwix &kwnear_sp.
.kwfont &kwnear.
pointers are treated as
.kw unsigned int
..ct ..li .
.kwix &kwfar16_sp.
.kwfont &kwfar16.
and
.kwix &kwisg16_sp.
.kwfont &kwiseg16.
pointers are also treated as
.kw unsigned int
..ct ,
with the pointer's segment value in the high-order
(most significant)
two bytes.
All the usual integer conversion rules then apply.
Note that
.kwix &kwfar_sp.
.kwfont &kwfar.
pointers may not be converted to an integer without losing
the segment information.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wlooc.,
for the purposes of conversion between pointers and integers,
pointers are treated as
.kw unsigned int
..ct ..li .
All the usual integer conversion rules then apply.
.*
.shade end
..do end
.*
.************************************************************************
.*
.keep begin
.section Multiplicative Operators
.*
.cillust begin
multiplicative-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.cast-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.multiplicative-expression
.mono *
cast-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.multiplicative-expression
.mono /
cast-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.multiplicative-expression
.mono %
cast-expression
.cillust end
.keep end
.pp
The
.mono *
symbol, in its binary form, yields the
.ix 'product'
.ix 'operator' 'product'
.ix 'operator' 'times'
.ix 'operator' 'binary *'
.us product
of its operands.
The operands must have arithmetic type,
and have the usual arithmetic conversions performed on them.
.* .pp
.* The product operator is both commutative and associative, so the
.* compiler, in the absence of the unary
.* .ix 'unary operator' '+'
.* + operator, may regroup a series of multiplications.
.pp
The
.mono /
symbol yields the
.ix 'quotient'
.ix 'operator' 'quotient'
.ix 'operator' 'division'
.ix 'operator' '/'
.us quotient
from the division of the first operand
by the second operand.
The operands must have arithmetic type,
and have the usual arithmetic conversions performed on them.
Note that when a division by zero occurs, the behavior is
undefined.
.pp
When both operands of / are
of integer type and positive value,
and the division is inexact,
the result is the
.ix 'rounding'
.ix 'truncation'
.ix 'division' 'rounding'
.ix 'integer' 'division rounding'
largest integer less than the algebraic (exact)
quotient. (The result is rounded down.)
.pp
When one or both operands of / is negative and the division is inexact,
whether the compiler rounds the value up or down is
implementation-defined.
.*
.************************************************************************
.*
.shade begin
..if '&target' eq 'PC' ..th ..do begin
The &wcboth. compilers always round
..do end
..el ..if '&target' eq 'PC 370' ..th ..do begin
The &wcall. compilers always round
..do end
.ix 'integer' 'division rounding'
.ix 'division rounding'
the result of integer division
toward zero.
This action is also called
.ix 'integer' 'division truncation'
.ix 'division truncation'
truncation.
.shade end
.*
.************************************************************************
.*
.pp
The
.mono %
symbol yields the
.ix 'remainder'
.ix 'modulus'
.ix 'operator' 'remainder'
.ix 'operator' 'modulus'
.ix 'operator' '%'
.us remainder
from the division of the first operand
by the second operand.
The operands of
.mono %
must have integral type.
.pp
When both operands of
.mono %
are positive, the result is
a positive value smaller than the second operand. When one or both
operands is negative, whether the result is positive or negative
is implementation-defined.
.*
.************************************************************************
.*
..if '&target' eq 'PC' ..th ..do begin
.shade begin
With the &wcboth. compiler,
the remainder has the same sign as the first operand.
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
With the &wcall. compilers,
the remainder has the same sign as the first operand.
.shade end
..do end
.*
.************************************************************************
.*
.pp
For integral types
.mono a
and
.mono b
..ct ,
if
.mono b
is not zero, then
.monoon
(a/b)*b&SYSRB.+&SYSRB.a%b
.monooff
will equal
.mono a
..ct ..li .
.*
.keep begin
.section Additive Operators
.*
.uillust begin
additive-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.multiplicative-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.additive-expression :MONO.+:eMONO. multiplicative-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.additive-expression :MONO.-:eMONO. multiplicative-expression
.uillust end
.keep end
.pp
The
.mono +
symbol, in its binary form,
denotes the
.ix 'addition'
.ix 'sum'
.ix 'operator' 'addition'
.ix 'operator' 'sum'
.ix 'operator' 'binary +'
.ix 'plus' 'binary'
.us sum
of its operands.
.pp
If both operands have arithmetic type, then the usual arithmetic
conversions are performed on them.
.pp
If one of the operands is a pointer, then the other operand must
have an integral type.
The pointer operand may not be a pointer to
.kw void
..ct ..li .
Before being added to the pointer value, the integral value is
multiplied by the size of the object to which the pointer points. The
result type is the same as the pointer operand type.
If the pointer value is a pointer to a member of an array, then the
resulting pointer will point to a member of the same array, provided
the array is large enough. If the resulting pointer does not point
to a member of the array, then its use with the unary
.mono *
(indirection)
or
.mono ->
(arrow)
operator will yield undefined
.* (and probably incorrect)
behavior.
.pp
The :MINUS. symbol, in its binary form,
denotes the
.ix 'subtraction'
.ix 'difference'
.ix 'operator' 'subtraction'
.ix 'operator' 'difference'
.ix 'operator' 'binary -'
.ix 'minus' 'binary'
.us difference
resulting from the subtraction of the second operand from the first.
If both operands have arithmetic type, then the usual arithmetic
conversions are performed on them.
.pp
If the first operand is a pointer, then the second operand must either
be a pointer to the same type or an integral type.
.pp
In the same manner as for adding a pointer and an integral value, the
integral value is multiplied by the size of the object to which the
pointer points.
The pointer operand may not be a pointer to
.kw void
..ct ..li .
The result type is the same type as the pointer operand.
.pp
If both operands are pointers to the same type, the difference is
divided by the size of the type, representing the difference of
the subscripts of the two array members (assuming the type is "array of
.us type
..ct ").
The type of the result is implementation-defined, and is represented
by
.ix 'ptrdiff_t'
.kw ptrdiff_t
(a signed integral type)
defined in the
.hdr <stddef.h>
header.
.*
.************************************************************************
.*
..if '&target' eq 'PC' or '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wcboth.,
.ix 'ptrdiff_t'
.kw ptrdiff_t
is
.kw int
..ct ,
unless the
.ix 'memory model' 'huge'
.ix 'huge memory model'
huge memory model is being used, in which case
.ix 'ptrdiff_t'
.kw ptrdiff_t
is
.kw long int
..ct ..li .
.shade end
..do end
..if '&target' eq 'PC 370' ..th ..do begin
.shade begin
With &wlooc., the type of
.ix 'ptrdiff_t'
.kw ptrdiff_t
is
.kw int
..ct ..li .
.shade end
..do end
.*
.************************************************************************
.*
.*
.* .keep begin
.section Bitwise Shift Operators
.*
.cillust begin
shift-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.additive-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.shift-expression
.mono <<
additive-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.shift-expression
.mono >>
additive-expression
.cillust end
.* .keep end
.pp
The
.mono <<
symbol denotes the
.ix 'left shift'
.ix 'shift' 'left'
.ix 'operator' 'left shift'
.us left-shift
operator. Both operands must have an integral type, and the
integral promotions are performed on them.
The type of the result is the type of the promoted left operand.
.pp
The result of
.monoon
op&SYSRB.<<&SYSRB.amt
.monooff
is
.mono op
left-shifted
.mono amt
bit positions. Zero bits are filled on the right.
Effectively, the high bits shifted out of
.mono op
are discarded, and the resulting set of bits is re-interpreted as the
result.
Another interpretation is that
.mono op
is multiplied by 2 raised to the power
.mono amt
..ct ..li .
.pp
The
.mono >>
symbol denotes the
.ix 'right shift'
.ix 'shift' 'right'
.ix 'operator' 'right shift'
.us right-shift
operator. Both operands must have an integral type, and the
integral promotions are performed on them.
The type of the result is the type of the promoted left operand.
.pp
The result of
.monoon
op&SYSRB.>>&SYSRB.amt
.monooff
is
.mono op
right-shifted
.mono amt
bit positions.
If
.mono op
has an unsigned type, or a signed type and a non-negative value,
then
.mono op
is divided by 2 raised to the power
.mono amt
..ct ..li .
Effectively, the low bits shifted out of
.mono op
are discarded, zero bits are filled on the left,
and the resulting set of bits is re-interpreted as the
result.
.pp
If
.mono op
has a signed type and negative value, then the behavior of
.monoon
op&SYSRB.>>&SYSRB.amt
.monooff
is implementation-defined.
Usually, the high bits vacated by the right shift are filled with
the sign bit from before the shift (arithmetic right shift), or with
0 (logical right shift).
.*
.************************************************************************
.*
.shade begin
..if '&target' eq 'PC' ..th ..do begin
With &wcboth.,
..do end
..el ..if '&target' eq 'PC 370' ..th ..do begin
With &wcall.,
..do end
a right shift of a negative value of a signed type
causes the sign bit to be propogated throughout the
bits vacated by the shift.
Essentially, the vacated bits are filled with 1 bits.
.shade end
.*
.************************************************************************
.*
.pp
For both
bitwise shift operators, if the number of bits to shift exceeds the
number of bits in the type, the result is undefined.
.*
.keep begin
.section Relational Operators
.*
.cillust begin
relational-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.shift-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.relational-expression
.mono <
shift-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.relational-expression
.mono >
shift-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.relational-expression
.mono <=
shift-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.relational-expression
.mono >=
shift-expression
.cillust end
.keep end
.pp
Each of the symbols
.ix 'less than'
.ix 'operator' 'less than'
.ix 'operator' '<'
.mono <
.us (less than),
.ix 'greater than'
.ix 'operator' 'greater than'
.ix 'operator' '>'
.mono >
.us (greater than),
.ix 'less than or equal to'
.ix 'not greater than'
.ix 'operator' 'less than or equal to'
.ix 'operator' 'not greater than'
.ix 'operator' '<='
.mono <=
.us (less than or equal to),
.ix 'greater than or equal to'
.ix 'not less than'
.ix 'operator' 'greater than or equal to'
.ix 'operator' 'not less than'
.ix 'operator' '>='
.mono >=
.us (greater than or equal to),
yields the value 1 if the relation is true, and 0 if the relation
is false. The result type is
.kw int
..ct ..li .
.pp
If both operands have arithmetic type, then the usual arithmetic
conversions are performed on them.
.pp
If one of the operands is a pointer, then the other operand must be
a pointer to a compatible type.
The result depends on where (in the address space of the computer)
the pointers actually point.
.pp
If both pointers point to
members of the same array object,
then the pointer that points
to the member with a higher subscript will be greater than the other
pointer.
.pp
If both pointers point to different members within the same structure,
then the pointer pointing to the member declared later in the structure
will be greater than the other pointer.
.pp
If both pointers point to the same union object, then they will be
equal.
.pp
All other comparisons yield undefined behavior. As discussed above,
the relationship between pointers is determined by the locations in
the machine storage that the pointers reference.
Typically, the numeric values of the pointer operands are compared.
.*
.keep begin
.section Equality Operators
.*
.cillust begin
equality-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.relational-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.equality-expression
.mono ==
relational-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.equality-expression
.mono !=
relational-expression
.cillust end
.keep end
.pp
The symbols
.mono ==
.ix 'equal to'
.ix 'operator' 'equal to'
.ix 'operator' '=='
.us (equal to)
and
.mono !=
.ix 'not equal to'
.ix 'operator' 'not equal to'
.ix 'operator' '!='
.us (not equal to)
yield the value 1 if the relation is true, and 0 if the relation
is false. The result type is
.kw int
..ct ..li .
.pp
If both operands have arithmetic type, then the usual arithmetic
conversions are performed on them.
.pp
If both operands are pointers to the same type and they compare equal,
then they are pointers to the same object.
.pp
If both operands are pointers and one is a pointer to
.kw void
..ct ,
then the other is converted to a pointer to
.kw void
..ct ..li .
.pp
If one of the operands is a pointer, the other may be a null pointer
constant (zero).
.pp
No other combinations are valid.
.*
.* .keep begin
.section Bitwise AND Operator
.*
.cillust begin
and-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.equality-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.and-expression
.mono &
equality-expression
.cillust end
.* .keep end
.pp
The
.mono &
symbol, in its binary form, denotes the
.ix 'bitwise AND'
.ix 'operator' 'bitwise AND'
.ix 'operator' '&'
.ix 'operator' 'binary &'
.us bitwise AND
operator. Each of the operands must have integral type, and the
usual arithmetic conversions are performed.
.pp
The result is the bitwise AND of the two operands. That is, the bit
in the result is set if and only if each of the corresponding bits
in the operands are set.
.keep begin
.pp
The following table illustrates some
bitwise AND
operations:
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 14
.  .boxcol  6
..do end
..el ..do begin
.  .boxcol 16
.  .boxcol  7
..do end
.boxbeg
$Operation      $Result
.boxline
.monoon
$0x0000 & 0x7A4C$0x0000
$0xFFFF & 0x7A4C$0x7A4C
$0x1001 & 0x0001$0x0001
$0x29F4 & 0xE372$0x2170
.monooff
.boxend
.do end
.el .do begin
.millust begin
Operation         Result
---------------   ------
0x0000 & 0x7A4C   0x0000
0xFFFF & 0x7A4C   0x7A4C
0x1001 & 0x0001   0x0001
0x29F4 & 0xE372   0x2170
.millust end
.do end
.keep end
.*
.keep begin
.section Bitwise Exclusive OR Operator
.*
.cillust begin
exclusive-or-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.and-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.exclusive-or-expression
.mono ^
and-expression
.cillust end
.keep end
.pp
The
.mono ^
symbol denotes the
.ix 'bitwise exclusive OR'
.ix 'operator' 'bitwise exclusive OR'
.ix 'operator' '^'
.us bitwise exclusive OR
operator. Each of the operands must have integral type, and the
usual arithmetic conversions are performed.
.pp
The result is the bitwise exclusive OR
of the two operands. That is, the bit
in the result is set if and only if exactly one of the corresponding bits
in the operands is set.
.pp
Another interpretation is that, if one of the operands is treated as
a mask, then every 1 bit in the mask causes the corresponding bit
in the other operand to be complemented (0 becomes 1, 1 becomes 0)
before being placed in the
result, while every 0 bit in the mask
causes the corresponding bit in the other operand to be placed
unchanged in the result.
.keep begin
.pp
The following table illustrates some
exclusive OR
operations:
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 14
.  .boxcol  6
..do end
..el ..do begin
.  .boxcol 16
.  .boxcol  7
..do end
.boxbeg
$Operation       $Result
.boxline
.monoon
$0x0000 ^ 0x7A4C$0x7A4C
$0xFFFF ^ 0x7A4C$0x85B3
$0xFFFF ^ 0x85B3$0x7A4C
$0x1001 ^ 0x0001$0x1000
$0x29F4 ^ 0xE372$0xCA86
.monooff
.boxend
.do end
.el .do begin
.millust begin
Operation         Result
---------------   ------
0x0000 ^ 0x7A4C   0x7A4C
0xFFFF ^ 0x7A4C   0x85B3
0xFFFF ^ 0x85B3   0x7A4C
0x1001 ^ 0x0001   0x1000
0x29F4 ^ 0xE372   0xCA86
.millust end
.do end
.keep end
.*
.keep begin
.section Bitwise Inclusive OR Operator
.*
.cillust begin
inclusive-or-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.exclusive-or-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.inclusive-or-expression
.mono |
exclusive-or-expression
.cillust end
.keep end
.pp
The
.mono |
symbol denotes the
.ix 'bitwise inclusive OR'
.ix 'operator' 'bitwise inclusive OR'
.ix 'operator' '|'
.us bitwise inclusive OR
operator. Each of the operands must have integral type, and the
usual arithmetic conversions are performed.
.pp
The result is the bitwise inclusive OR
of the two operands. That is, the bit
in the result is set if at least one of the corresponding bits
in the operands is set.
.pp
The following table illustrates some
inclusive OR
operations:
..sk 1 c
.if &e'&nobox eq 0 .do begin
.boxdef
..if '&format' eq '7x9' ..th ..do begin
.  .boxcol 14
.  .boxcol  6
..do end
..el ..do begin
.  .boxcol 16
.  .boxcol  7
..do end
.boxbeg
$Operation      $Result
.boxline
.monoon
$0x0000 | 0x7A4C$0x7A4C
$0xFFFF | 0x7A4C$0xFFFF
$0x1100 | 0x0022$0x1122
$0x29F4 | 0xE372$0xEBF6
.monooff
.boxend
.do end
.el .do begin
.millust begin
Operation         Result
---------------   ------
0x0000 | 0x7A4C   0x7A4C
0xFFFF | 0x7A4C   0xFFFF
0x1100 | 0x0022   0x1122
0x29F4 | 0xE372   0xEBF6
.millust end
.do end
.*
.keep begin
.section Logical AND Operator
.*
.cillust begin
logical-and-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.inclusive-or-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.logical-and-expression
.mono &&
inclusive-or-expression
.cillust end
.keep end
.pp
The
.mono &&
symbol denotes the
.ix 'logical AND'
.ix 'operator' 'logical AND'
.ix 'operator' '&&'
.us logical AND
operator. Each of the operands must have scalar type.
.pp
If both of the operands are not equal to zero, then the result
is 1.
Otherwise, the result is zero.
The result type is
.kw int
..ct ..li .
.pp
If the first operand is zero, then the second operand is not
evaluated.
Any side effects that would have happened if the second operand had
been executed do not happen. Any function calls encountered in the
second operand do not take place.
.*
.keep begin
.section Logical OR Operator
.*
.cillust begin
logical-or-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.logical-and-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.logical-or-expression
.mono ||
logical-and-expression
.cillust end
.keep end
.pp
The
.mono ||
symbol denotes the
.ix 'logical OR'
.ix 'operator' 'logical OR'
.ix 'operator' '||'
.us logical OR
operator. Each of the operands must have scalar type.
.pp
If one or both of the operands is not equal to zero, then the result
is 1.
Otherwise, the result is zero (both operands are zero).
The result type is
.kw int
..ct ..li .
.pp
If the first operand is not zero, then the second operand is not
evaluated.
Any side effects that would have happened if the second operand had
been executed do not happen. Any function calls encountered in the
second operand do not take place.
.*
.keep begin
.section Conditional Operator
.*
.cillust begin
conditional-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.logical-or-expression
.cor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.logical-or-expression
.mono ?
expression
.mono :
conditional-expression
.cillust end
.keep end
.pp
The
.mono ?
symbol separates the first two parts of a
.ix 'conditional operator'
.ix 'operator' 'conditional'
.ix 'operator' '?'
.us conditional
operator, and the
.mono :
symbol separates the
second and third parts.
The first operand must have a scalar type (not a structure, union or
array).
.pp
The first operand is evaluated. If its value is not equal to zero,
then the second operand is evaluated and its value is the result.
Otherwise, the third operand is evaluated and its value is the result.
.pp
Whichever operand is evaluated, the other is not evaluated. Any side
effects that might have happened during the evaluation of the other
operand do not happen.
.pp
If both the second and third operands have arithmetic type, then the
usual arithmetic conversions are performed on them, and the type of
the result is the same type as the converted operands.
.pp
If both operands have the same structure, union or pointer type,
then the result has that type.
.pp
If both operands are pointers, and one is
"pointer to
.kw void
..ct ",
then the result type is
"pointer to
.kw void
..ct ".
.pp
If one operand is a pointer, and the other is a null pointer constant
(0), the result type is that of the pointer.
.pp
If both operands are void expressions, then the result is a void
expression.
.pp
No other combinations of result types are permitted.
.pp
Note that, unlike most other operators, the conditional operator
associates from right to left.
For example, the expression,
.millust begin
a = b ? c : d ? e : f;
.millust end
.pc
is translated as if it had been parenthesized as follows:
.millust begin
a = b ? c : (d ? e : f);
.millust end
.pp
This construct is confusing, and so should probably be avoided.
.*
.keep begin
.section Assignment Operators
.*
.uillust begin
assignment-expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.conditional-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.simple-assignment-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.compound-assignment-expression
.uillust end
.keep end
.pp
An
.ix 'assignment operator'
.ix 'operator' 'assignment'
.us assignment operator
stores a value in the object designated by the left operand.
The left operand must be a modifiable lvalue.
.pp
The result type and value are those of the left operand
after the assignment.
.pp
Whether the left or right operand is evaluated first is undefined.
.pp
Note that, unlike most other operators, the assignment
operators associate from right to left.
For example, the expression,
.millust begin
a += b = c;
.millust end
.pc
is translated as if it had been bracketed as follows:
.millust begin
a += (b = c);
.millust end
.beglevel
.*
.keep begin
.section Simple Assignment
.*
.uillust begin
simple-assignment-operator:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.unary-expression :MONO.=:eMONO. assignment-expression
.uillust end
.keep end
.pp
The
.mono =
symbol denotes
.ix 'assignment operator'
.ix 'simple assignment'
.ix 'operator' 'assignment'
.ix 'operator' 'simple assignment'
.ix 'operator' '='
.us simple assignment.
The value of the right operand is converted
to the type of the left operand and replaces the value designated by
the left operand.
.pp
The two operands must obey one of the following rules,
.begbull
.bull
both have arithmetic types,
.bull
both have the same structure or union type, or the right operand
differs only in the presence of the
.kw const
or
.kw volatile
keywords,
.bull
both are pointers to the same type,
.bull
both are pointers and one is a pointer to
.kw void
..ct ,
.bull
the left operand is a pointer, and the right is a
.ix 'null pointer'
.ix 'pointer' 'null'
null pointer constant (0).
.endbull
.*
.* .keep begin
.section Compound Assignment
.*
.cillust begin
compound-assignment-expression:
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.unary-expression
assignment-operator assignment-expression
.cillust end
.* .keep end
.cillust begin
assignment-operator:
:HP0.one of:eHP0.
.cbr
.monoon
&SYSRB.&SYSRB.&SYSRB.&SYSRB.+=&SYSRB.&SYSRB.-=
.monooff
.cbr
.monoon
&SYSRB.&SYSRB.&SYSRB.&SYSRB.*=&SYSRB.&SYSRB./=&SYSRB.&SYSRB.%=
.monooff
.cbr
.monoon
&SYSRB.&SYSRB.&SYSRB.&SYSRB.&=&SYSRB.&SYSRB.^=&SYSRB.&SYSRB.|=
.monooff
.cbr
.monoon
&SYSRB.&SYSRB.&SYSRB.&SYSRB.<<=&SYSRB.>>=
.monooff
.cillust end
.pp
A
.ix 'compound assignment'
.ix 'operator' 'compound assignment'
.ix 'operator' '*='
.ix 'operator' '/='
.ix 'operator' '%='
.ix 'operator' '+='
.ix 'operator' '-='
.ix 'operator' '<<='
.ix 'operator' '>>='
.ix 'operator' '&='
.ix 'operator' '^='
.ix 'operator' '|='
.us compound assignment
operator of the form
.monoon
a&SYSRB.:ITAL.op:eITAL.=&SYSRB.b
.monooff
is equivalent to the simple assignment expression
.monoon
a&SYSRB.=&SYSRB.a
.monooff
.us op
.mono (b)
..ct ,
except that the left operand
.mono a
is evaluated only once.
.pp
The compound assignment
operator must have operands consistent with those allowed by the
corresponding binary operator.
.endlevel
.*
.keep begin
.section Comma Operator
.*
.uillust begin
expression:
&SYSRB.&SYSRB.&SYSRB.&SYSRB.assignment-expression
.uor
&SYSRB.&SYSRB.&SYSRB.&SYSRB.expression:MONO.,:eMONO. assignment-expression
.uillust end
.keep end
.pp
At the lowest precedence, the
.ix 'comma operator'
.ix 'operator' 'comma'
.ix 'operator' ','
.us comma operator
evaluates the left operand as a void expression (it is evaluated and
its result, if any, is discarded), and then evaluates the right operand.
The result has the type and value of the second operand.
.pp
In contexts where the comma is also used as a separator
(function argument lists and initializer lists),
a comma expression must be placed in parentheses.
.keep begin
..sk 1 c
For example,
.millust begin
Fn( (pi=3.14159,two_pi=2*pi) );
.millust end
.pc
the function
.mono Fn
has one parameter, which has the value
2 times
.mono pi
..ct ..li .
.keep break
.uillust begin
.monoon
for( i = 0, j = 0, k = 0;; i++, j++, k++ )
.monooff
&SYSRB.&SYSRB.&SYSRB.&SYSRB.statement:MSEMI.
.uillust end
.pc
The
.ix 'statement' 'for'
.ix 'for statement'
.kw for
statement allows three expressions.
In this example, the first
expression initializes three objects and
the third expression
increments the three objects.
.keep end
.*
.section Constant Expressions
.*
.ix 'constant expression'
.ix 'expression' 'constant'
.pp
A constant expression may be specified in several places:
.begbull $compact
.bull
the size of a
.ix 'bit-field'
bit-field member of a structure,
.bull
the value of an
.ix 'enumeration constant'
enumeration constant,
.bull
an
.ix 'initialization'
initializer list,
.bull
the number of elements in an
.ix 'array' 'specifying size'
array,
.bull
the value of a
.ix 'case label'
.kw case
label constant,
.bull
with the
.kwpp #if
and
.kwpp #elif
preprocessor directives.
.endbull
.pp
In most cases, a constant expression consists of a series of constant
values and operations that evaluate to a constant value.
Certain operations may only appear within the operand of the
.kw sizeof
operator. These include:
.begbull $compact
.bull
a function call,
.bull
pre- or post-increment or decrement,
.bull
assignment,
.bull
comma operator,
.bull
array subscripting,
.bull
the . and,
.mono ->
operators (structure member access),
.bull
the unary
.mono &
(address-of) operator (see exception below),
.bull
the unary
.mono *
(indirection) operator,
.bull
casts to a type other than an integer type.
.endbull
.pp
In a constant expression that is an initializer, floating-point
constants and casts may be specified. Objects that have
static storage duration, and function designators (names), may be
used to provide addresses, either explicitly using the unary
.mono &
(address-of) operator, or implicitly by specifying the
identifier only.
.pp
The following examples illustrate constant expressions that may
be used anywhere:
.millust begin
3
256*3 + 27
OPSYS == OS_DOS /* These are macro names */
.millust end
.pc
The next set of examples are constant expressions that are only valid
in an initializer:
.millust begin
&SomeObject
SomeFunction
3.5 * 7.2 / 6.5
.millust end
.pp
In a constant expression that is part of a
.ix 'constant expression' 'in #if or #elif'
.kwpp #if
or
.kwpp #elif
preprocessor directive,
.* the
.* .kw sizeof
.* operator, casts and enumeration constants may not be specified.
.* Since
.* .kw sizeof
.* can't be used, it follows that function calls, assignment, pre- and
.* post-increment and decrement, array indexing, structure member access,
.* address-of, indirection, casts, comma operator, and floating-point
.* numbers are not permitted.
.* Specifically,
only integral constants
and operators are permitted
(and macros that, when replaced,
follow these same rules).
