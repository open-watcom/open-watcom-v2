.chap *refid=fexpr Expressions
.*
.np
The following topics are discussed in this chapter.
.begbull
.bull
Arithmetic Expressions
.bull
Character Expressions
.bull
Relational Expressions
.bull
Logical Expressions
.bull
Evaluating Expressions
.bull
Constant Expressions
.endbull
.*
.section Arithmetic Expressions
.*
.np
Arithmetic expressions are used to describe computations
involving operands with numeric data type, arithmetic operators
and left and right parentheses.
The result of the computation is of numeric data type.
.*
.beglevel
.*
.section Arithmetic Operators
.*
.np
The following table lists the arithmetic operators and the
operation they perform.
.* .mbox on 1 12 40
.sr c0=&INDlvl+1
.sr c1=&INDlvl+12
.sr c2=&INDlvl+40
.mbox on &c0 &c1 &c2
Operator    Arithmetic Operation
.mbox
**          Exponentiation
/           Division
*           Multiplication
-           Subtraction or Negation
+           Addition or Identity
.mbox off
.np
Some operators can be either binary or unary.
.ix 'binary operator'
.ix 'operator' 'binary'
A
.us binary operator
is one that requires two operands.
.ix 'unary operator'
.ix 'operator' 'unary'
A
.us unary operator
is one that requires one operand.
Each of the operators **, /, and * are binary operators.
The operators + and &minus. can either be binary or unary operators.
The following table describes how each operator is used with their
operands.
.sr c0=&INDlvl+1
.sr c1=&INDlvl+12
.sr c2=&INDlvl+40
.cp 12
.mbox on &c0 &c1 &c2
Operator    Arithmetic Operation
.mbox
x ** y      x is raised to the power y
x / y       x is divided by y
x * y       x is multiplied by y
x - y       y is subtracted from x
x + y       y is added to x
  - x       x is negated
  + x       identity
.mbox off
.np
Arithmetic expressions can contain more than one operator.
It is thus necessary to define rules of evaluation for such expressions.
.ix 'arithmetic operators' 'precedence'
.ix 'operator' 'precedence'
A
.us precedence relation
is defined between operators.
This relation defines the order in which operands are combined and
hence describes the evaluation sequence of an arithmetic expression.
Operands of higher precedence operators are combined using that
operator to form an operand for an operator of lower precedence.
The following rules define the precedence relation among arithmetic
operators.
.autopoint
.point
Exponentiation
.mono (**)
has highest precedence.
.point
Multiplication
.mono (*)
and division (/) have equal precedence but have lower precedence than
exponentiation.
.point
Addition (+) and subtraction (&minus.) have equal precedence but have
lower precedence than multiplication and division.
.endpoint
.np
For example, to evaluate the expression
.millust begin
    A-B**4
.millust end
.pc
.id B
is raised to the exponent 4 first and the result is then subtracted from
.id A.
.np
Parentheses can be used to alter the evaluation sequence of an
arithmetic expression.
When a left parenthesis is encountered, the entire expression enclosed
in parentheses is evaluated.
Consider the following expression.
.millust begin
    3*(4+5)
.millust end
.pc
We first evaluate the expression in the parentheses, the result being 9.
We now multiply the result by 3 giving a final result of 27.
Now suppose we remove the parentheses.
According to the precedence rules,
.mono *
has precedence over + so we perform the multiplication before the
addition.
The result in this case is 17.
.*
.section Rules for Forming Standard Arithmetic Expressions
.*
.np
.ix 'arithmetic expression' 'primary'
.ix 'expression' 'primary'
The building blocks for arithmetic expressions are called
.us arithmetic primaries.
They are one of the following:
.autopoint
.point
unsigned arithmetic constant
.point
arithmetic symbolic constant
.point
arithmetic variable reference
.point
arithmetic array element reference
.point
arithmetic function reference
.point
( arithmetic expression )
.endpoint
.pc
A grammar for forming arithmetic expressions can be described which
reflects the precedence relation among arithmetic operators.
.np
Exponentiation has highest precedence.
.ix 'arithmetic expression' 'factor'
.ix 'expression' 'factor'
We define a
.us factor
as:
.autopoint
.point
primary
.point
primary ** factor
.endpoint
.pc
A factor is simply a sequence of primaries, each separated by the
exponentiation operator.
Rule (2) specifies that the primaries involving exponentiation
operators are combined from right to left
when evaluating a factor.
.np
Next in the precedence hierarchy are the multiplication and division
operators.
.ix 'arithmetic expression' 'term'
.ix 'expression' 'term'
We define a
.us term
as:
.autopoint
.point
factor
.point
term / factor
.point
term * factor
.endpoint
.pc
A term is simply a sequence of factors, each separated by a
multiplication operator or a division operator.
Rules (2) and (3) imply that in such a sequence, factors are combined
from left to right when evaluating a term.
Factors can be interpreted as the result obtained from evaluating
them.
This implies that all factors are evaluated before any of the
multiplication or division operands are combined.
This interpretation is consistent with the precedence relation
between the exponentiation operator and the division and
multiplication operators.
.np
.ix 'arithmetic expression'
.ix 'expression' 'arithmetic'
An
.us arithmetic expression
can now be defined as follows.
.autopoint
.point
term
.point
+ term
.point
&minus. term
.point
arithmetic expression + term
.point
arithmetic expression &minus. term
.endpoint
.pc
An arithmetic expression is simply a sequence of terms, each separated
by an addition operator or a subtraction operator.
Rules (4) and (5) imply that terms are evaluated from left to right.
Rules (2) and (3) imply that only the first term of an arithmetic
expression can be preceded by a unary + or &minus. operator.
Terms can be interpreted in the same way as factors were interpreted
in the definition of terms.
.np
Note that consecutive operators are not permitted.
For example, the expression
.millust begin
    A+-B
.millust end
.pc
is illegal.
However, expressions of the form
.millust begin
    A+(-B)
.millust end
.pc
are allowed.
.*
.section Arithmetic Constant Expression
.*
.np
.ix 'arithmetic constant expression'
.ix 'expression' 'arithmetic constant'
An
.us arithmetic constant expression
is an arithmetic expression in which all primaries are one of the
following.
.autopoint
.point
arithmetic constant
.point
symbolic arithmetic constant
.point
( arithmetic constant expression )
.endpoint
.pc
There is a further restriction with the exponentiation operator;
the exponent must be of type INTEGER.
.np
.xt begin
.ix 'intrinsic function' 'ISIZEOF'
.ix 'ISIZEOF'
As an extension to the FORTRAN 77 language, &product supports the use
of the intrinsic function
.kw ISIZEOF
in an arithmetic constant expression.
.exam begin
      PARAMETER (INTSIZ = ISIZEOF(INTEGER))
.exam end
.xt end
.np
.ix 'integer constant expression'
.ix 'expression' 'integer constant'
An
.us integer constant expression
is an arithmetic constant expression in which all constants and
symbolic constants are of type INTEGER.
.exam begin
      123
      -753+2
      -(12*13)
.exam end
.np
.ix 'real constant expression'
.ix 'expression' 'real constant'
A
.us real constant expression
is an arithmetic constant expression in which at least one
constant or symbolic constant is of type REAL and all other
constants or symbolic constants are of type REAL or INTEGER.
.exam begin
      123.
      -753+2.0
      -(13E0*12)
.exam end
.np
.ix 'double precision constant expression'
.ix 'expression' 'double precision constant'
A
.us double precision constant expression
is an arithmetic constant expression in which at least one constant or
symbolic constant is of type DOUBLE PRECISION and all other constants
or symbolic constants are of type DOUBLE PRECISION, REAL or INTEGER.
.exam begin
      123.4D0
      -753D0*2+.5
      -(12D0*12.2)
.exam end
.np
.ix 'complex constant expression'
.ix 'expression' 'complex constant'
A
.us complex constant expression
is an arithmetic constant expression in which at least one
constant or symbolic constant is of type COMPLEX and all other
constants or symbolic constants are of type COMPLEX, REAL or
INTEGER.
.exam begin
      (123,0)
      (-753,12.3)*2
      -(12,-12.4)-(1.0,.2)
.exam end
.np
.ix 'double precision complex constant expression'
.ix 'expression' 'double precision complex constant'
.xt begin
A
.us double precision complex constant expression
is an arithmetic constant expression in which at least one constant or
symbolic constant is of type COMPLEX*16 and all other constants or
symbolic constants are of type COMPLEX*16, DOUBLE PRECISION, REAL or
INTEGER.
If there are no constants or symbolic constants of type COMPLEX*16 in
a constant expression, the type of the constant expression will be
COMPLEX*16 if it contains at least one constant or symbolic constant
of type COMPLEX and at least one constant or symbolic constant of type
DOUBLE PRECISION.
&product supports this type of constant expression as an extension of
the FORTRAN 77 language.
.xt end
.keep
.xt begin
.exam begin
      (123,0D0)
      (-753,12.3D0)*2
      -(12D0,-12.4)-(1.0,.2)
.exam end
.xt end
.*
.section Data Type of Arithmetic Expressions
.*
.np
Evaluating an arithmetic expression produces a result which has a
type.
The type of the result is determined by the type of its operands.
.ix 'arithmetic expression' 'type of'
The following table describes the rules for determining the type of
arithmetic expressions.
The letters I, R, D, C and Z stand for INTEGER, REAL, DOUBLE
PRECISION, COMPLEX
.xt and COMPLEX*16
respectively.
An entry in the table represents the data type of the result when the
operands are of the type indicated by the row and column in which the
entry belongs.
The column represents the type of the operand to the right of the
operator, and the row represents the type of the operand to the left
of the operator.
The table is valid for all of the arithmetic operators.
.if &e'&dohelp eq 0 .do begin
.* .box on 4 8 12 16 21 25 29 33 37 41
.sr c0=&INDlvl+1
.sr c1=&INDlvl+5
.sr c2=&INDlvl+9
.sr c3=&INDlvl+13
.sr c4=&INDlvl+18
.sr c5=&INDlvl+22
.sr c6=&INDlvl+26
.sr c7=&INDlvl+30
.sr c8=&INDlvl+34
.sr c9=&INDlvl+38
.cp 15
.box on &c0 &c1 &c2 &c3 &c4 &c5 &c6 &c7 &c8 &c9
\ op  \ I*1 \ I*2 \ I*4 \  R  \  D  \  C  \  Z \
.box
\ I*1 \ I*1 \ I*2 \ I*4 \  R  \  D  \  C  \  Z \
\ I*2 \ I*2 \ I*2 \ I*4 \  R  \  D  \  C  \  Z \
\ I*4 \ I*4 \ I*4 \ I*4 \  R  \  D  \  C  \  Z \
\  R  \  R  \  R  \  R  \  R  \  D  \  C  \  Z \
\  D  \  D  \  D  \  D  \  D  \  D  \  Z  \  Z \
\  C  \  C  \  C  \  C  \  C  \  Z  \  C  \  Z \
\  Z  \  Z  \  Z  \  Z  \  Z  \  Z  \  Z  \  Z \
.box off
.do end
.el .do begin
.millust begin
+-------+-------+-------+-------+-------+-------+-------+-------+
|  op   |  I*1  |  I*2  |  I*4  |   R   |   D   |   C   |   Z   |
+-------+-------+-------+-------+-------+-------+-------+-------+
|  I*1  |  I*1  |  I*2  |  I*4  |   R   |   D   |   C   |   Z   |
|  I*2  |  I*2  |  I*2  |  I*4  |   R   |   D   |   C   |   Z   |
|  I*4  |  I*4  |  I*4  |  I*4  |   R   |   D   |   C   |   Z   |
|   R   |   R   |   R   |   R   |   R   |   D   |   C   |   Z   |
|   D   |   D   |   D   |   D   |   D   |   D   |   Z   |   Z   |
|   C   |   C   |   C   |   C   |   C   |   Z   |   C   |   Z   |
|   Z   |   Z   |   Z   |   Z   |   Z   |   Z   |   Z   |   Z   |
+-------+-------+-------+-------+-------+-------+-------+-------+
.millust end
.do end
.autonote Notes:
.setptnt 0 5
.note
I*1 represents the
.xt on
.mono INTEGER*1
.xt off
data type,
I*2 represents the
.xt on
.mono INTEGER*2
.xt off
data type, and
I*4 represents the
.mono INTEGER
.xt on
or
.mono INTEGER*4
.xt off
data type.
.note
The data type of the result obtained by dividing an integer datum by
an integer datum is also of type INTEGER even though the mathematical
result may not be an integer.
.ix 'integer quotient'
This result is called the
.us integer quotient
and is defined as the integer part of the mathematical quotient.
.note
.xt on
&product supports the double precision complex data type (COMPLEX*16)
as an extension of the FORTRAN 77 language.
Combining an operand of type DOUBLE PRECISION with an operand of type
COMPLEX yields a result of type COMPLEX*16.
.xt off
.* .note
.* Operands of type INTEGER*1 and INTEGER*2 are converted to type INTEGER
.* prior to performing the operation.
.* Note that an expression can never have type INTEGER*1 or INTEGER*2.
.endnote
.endlevel
.*
.section Character Expressions
.*
.np
Character expressions are used to describe computations involving
operands of type CHARACTER, the concatenation operator (//) and left
and right parentheses.
The result of the computation is of type CHARACTER.
.*
.beglevel
.*
.section Character Operators
.*
.np
There is only one character operator, namely the concatenation
operator (//).
It requires two operands of type CHARACTER.
If
.id x
is the left operand and
.id y
is the right operand, then the result is
.id y
concatenated to
.id x.
The length of the result is the sum of the lengths of the two
operands.
For example, the result of
.millust begin
    'AAAAA'//'BBB'
.millust end
.pc
is the string
.mono AAAAABBB.
.*
.section Rules for Forming Character Expressions
.*
.np
.ix 'character expression' 'primaries'
The building blocks for character expressions are called
.us character primaries.
They are one of the following.
.autopoint
.point
character constant
.point
character symbolic constant
.point
character variable reference
.point
character array element reference
.point
character substring reference
.point
character function reference
.point
( character expression )
.endpoint
.pc
.ix 'character expression'
.us Character expressions
are defined as follows:
.autopoint
.point
character primary
.point
character expression // character primary
.endpoint
.pc
A character expression is simply a sequence of character primaries,
each separated by the concatenation operator (//).
Rule 2 implies that character primaries are combined from left to
right.
Except in a character assignment statement, the operands in a
character expression must not contain operands whose length
specification is (*) unless the operand is a symbolic constant.
.np
Note that, unlike arithmetic expressions, parentheses have no effect
on the result of evaluating a character expression.
For example, the result of the expressions
.millust begin
    'A'//'B'//'C'
.millust end
.pc
and
.millust begin
    'A'//('B'//'C')
.millust end
.pc
is identically the string
.mono ABC.
.*
.section Character Constant Expressions
.*
.np
.ix 'character constant expression'
A
.us character constant expression
is a character expression in which all primaries are one of the
following.
.autopoint
.point
character constant
.point
symbolic character constant
.point
( character constant expression )
.endpoint
.np
.xt begin
.ix 'intrinsic function' 'CHAR'
.ix 'CHAR'
As an extension to the FORTRAN 77 language, &product supports the use
of the intrinsic function
.kw CHAR
in a character constant expression.
.exam begin
      CHARACTER*6 HELLO, WORLD
      PARAMETER (HELLO = 'Hello'//CHAR(0))
      PARAMETER (WORLD = 'world'//CHAR(7))
      PRINT *, HELLO, WORLD
      END
.exam end
.xt end
.endlevel
.*
.section Relational Expressions
.*
.np
A relational expression is used to compare two arithmetic expressions
or two character expressions.
It is not possible to compare a character expression to an arithmetic
expression.
Evaluation of a relational expression produces a result of type
logical.
.*
.beglevel
.*
.section Relational Operators
.*
.np
.ix 'relational operator'
.ix 'operator' 'relational'
The following table lists the
.us relational operators
and the operation they perform.
.sr c0=&INDlvl+1
.sr c1=&INDlvl+12
.sr c2=&INDlvl+40
.mbox on &c0 &c1 &c2
Operator    Relational Operation
.mbox
 .LT.        Less than
 .LE.        Less than or equal
 .EQ.        Equal
 .NE.        Not equal
 .GT.        Greater than
 .GE.        Greater than or equal
.mbox off
.*
.section Form of a Relational Expression
.*
.np
.ix 'relational expression'
.ix 'expression' 'relational'
The form of a
.us relational expression
is as follows.
.sr c0=&INDlvl+1
.sr c1=&INDlvl+26
.mbox on &c0 &c1
  e1 relop e2
.mbox off
.synote 8
.mnote relop
is a relational operator.
.mnote e1, e2
are both arithmetic expressions or both character expressions.
.endnote
.*
.beglevel
.*
.section Arithmetic Relational Expressions
.*
.np
.ix 'arithmetic relational expression'
An
.us arithmetic relational expression
is a relational expression in which
.id e1
and
.id e2
are both arithmetic expressions.
An arithmetic relational expression has a value of true if the
operands satisfy the relation specified by the relational operator and
false otherwise.
.np
A complex operand is only permitted when using either the .EQ. or .NE.
relational operators.
.xt &product allows operands of type COMPLEX*16.
.*
.section Character Relational Expressions
.*
.np
.ix 'character relational expression'
.us Character relational expressions
are relational expressions whose operands are of type CHARACTER.
.ix 'collating sequence'
The value of a relation between character strings is established by
using the
.us collating sequence
of the processor character set.
The collating sequence is an ordering of the characters in the
processor character set.
Note, for example, that the EBCDIC character set has a different
collating sequence than that of the ASCII character set.
For example,
.id e1
is greater than
.id e2
if the value of
.id e1
follows the value of
.id e2
in the processor collating sequence.
The value of a character relational expression depends on the
collating sequence.
In the case of the .NE. and .EQ. operators, the collating sequence has
no effect.
.exam begin
      IF( 'A' .LT. 'a' )THEN
          PRINT *, 'The processor character set'
          PRINT *, 'appears to be ASCII'
      ELSE
          PRINT *, 'The processor character set'
          PRINT *, 'appears to be EBCDIC'
      END IF
      END
.exam end
.pc
The above example is a crude test for determining the character set
used on your processor.
.np
It is possible to have operands of unequal length.
In this case, the character string of smaller length is treated as if
blanks were padded to the right of it to the length of the larger
string.
The relational operator is then applied.
.endlevel
.endlevel
.*
.section Logical Expressions
.*
.np
Logical expressions are used to describe computations involving
operands whose type is LOGICAL
.xt or INTEGER
.ct , logical operators and left and
right parentheses.
The result of the computation is of type LOGICAL
.xt on
unless both operands are of type INTEGER in which case the result of
the computation is of type INTEGER.
.xt off
.*
.beglevel
.*
.section Logical Operators
.*
.np
.ix 'logical operator'
The following table lists the
.us logical operators
and the operation they perform.
.sr c0=&INDlvl+1
.sr c1=&INDlvl+12
.sr c2=&INDlvl+40
.mbox on &c0 &c1 &c2
Operator    Logical Operation
.mbox
 .NOT.       Logical negation
 .AND.       Logical conjunction
 .OR.        Logical inclusive disjunction
 .EQV.       Logical equivalence
 .NEQV.      Logical non-equivalence
 .XOR.       Exclusive or
.mbox off
.np
The logical operator .NOT. is a unary operator; all other logical
operators are binary.
The following tables describe the result of each operator when it
is used with logical operands.
.* .mbox on 1 12 30
.sr c0=&INDlvl+1
.sr c1=&INDlvl+12
.sr c2=&INDlvl+30
.mbox on &c0 &c1 &c2
  x           .NOT. x
.mbox
true          false
false         true
.mbox off
.* .mbox on 1 9 17 30
.sr c0=&INDlvl+1
.sr c1=&INDlvl+9
.sr c2=&INDlvl+17
.sr c3=&INDlvl+30
.mbox on &c0 &c1 &c2 &c3
  x       y       x .AND. y
.mbox
true    true        true
true    false       false
false   true        false
false   false       false
.mbox off
.mbox on &c0 &c1 &c2 &c3
  x       y       x .OR. y
.mbox
true    true        true
true    false       true
false   true        true
false   false       false
.mbox off
.mbox on &c0 &c1 &c2 &c3
  x       y       x .EQV. y
.mbox
true    true        true
true    false       false
false   true        false
false   false       true
.mbox off
.mbox on &c0 &c1 &c2 &c3
x       y         x .NEQV. y
 ................ x .XOR. y
.mbox
true    true        false
true    false       true
false   true        true
false   false       false
.mbox off
.np
Note that the operators .NEQV.
.xt on
and .XOR.
.xt off
perform the same logical operation.
.np
.xt begin
The following tables describe the result of the logical operators
when they are used with integer operands.
.* If one of the operands is of type LOGICAL, it is converted to INTEGER
.* type (.TRUE. is 1, .FALSE. is 0).
These operators apply to bits in the operand(s), hence we show only
the result of operations on individual bits.
The way to read the entries in the following tables is:
.autonote
.note
If the bit in "x" is 0 then the corresponding bit in ".NOT.x" is 1,
and so on.
.note
If the bit in "x" is 1 and the corresponding bit in "y" is 1 then
the corresponding bit in "x.AND.y" is 1, and so on.
.endnote
.xt end
.np
.* .mext on 1 12 30
.sr c0=&INDlvl+1
.sr c1=&INDlvl+12
.sr c2=&INDlvl+30
.mext on &c0 &c2 &c1
x           .NOT. x
.mext
0              1
1              0
.mext off
.* .mext on 1 8 16 30
.sr c0=&INDlvl+1
.sr c1=&INDlvl+8
.sr c2=&INDlvl+15
.sr c3=&INDlvl+30
.mext on &c0 &c3 &c1 &c2
x       y       x .AND. y
.mext
1       1           1
1       0           0
0       1           0
0       0           0
.mext off
.mext on &c0 &c3 &c1 &c2
x       y       x .OR. y
.mext
1       1           1
1       0           1
0       1           1
0       0           0
.mext off
.mext on &c0 &c3 &c1 &c2
x       y       x .EQV. y
.mext
1       1           1
1       0           0
0       1           0
0       0           1
.mext off
.mext on &c0 &c3 &c1 &c2
x       y       x .NEQV. y
 .............. x .XOR. y
.mext
1       1           0
1       0           1
0       1           1
0       0           0
.mext off
.np
Note that the operators .NEQV.
.xt on
and .XOR.
.xt off
perform the same mathematical operation on bits.
.np
As is the case with arithmetic operators, we must define rules in
order to evaluate logical expressions.
Again we define rules of precedence for logical operators which
dictate the evaluation sequence of logical expressions.
The following lists the logical operators in order of precedence.
.autopoint
.point
.li .NOT. (highest precedence)
.point
.li .AND.
.point
.li .OR.
.point
.li .EQV., .NEQV.
.xt and .XOR.
(lowest precedence)
.endpoint
.pc
For example, in the expression
.millust begin
    A .OR. B .AND. C
.millust end
.pc
the .AND. operator has higher precedence than the .OR. operator so
.id B
and
.id C
are combined first using the .AND. operator.
The result is then combined with
.id A
using the .OR. operator.
.np
Parentheses can be used to alter the sequence of evaluation of logical
expressions.
If in the previous example we had written
.millust begin
    (A .OR. B) .AND. C
.millust end
.pc
then
.id A
and
.id B
would have been combined first.
.*
.section Rules for Forming Logical Expressions
.*
.np
Logical primaries are the building blocks for logical expressions.
They are one of the following.
.autopoint
.point
logical
.xt or integer
constant
.point
symbolic logical
.xt or integer
constant
.point
logical
.xt or integer
variable reference
.point
logical
.xt or integer
array element reference
.point
logical
.xt or integer
function reference
.point
relational expression
.point
( logical
.xt or integer
expression )
.endpoint
.pc
As was done with arithmetic expressions, a grammar can be defined
which dictates the precedence relation among logical operators.
.np
The .NOT. logical operator has highest precedence.
.ix 'logical expression' 'logical factor'
We define a
.us logical factor
as:
.autopoint
.point
logical primary
.point
.li .NOT. logical primary
.endpoint
.np
Next in the precedence hierarchy is the .AND. operator.
.ix 'logical expression' 'logical term'
We define a
.us logical term
as:
.autopoint
.point
logical factor
.point
logical term .AND. logical factor
.endpoint
.pc
A logical term is simply a sequence of logical factors, each separated
by the .AND. operator.
Rule (2) specifies that the logical factors are combined from left to
right.
.np
Next is the .OR. operator.
.ix 'logical expression' 'logical disjunct'
We define a
.us logical disjunct
as:
.autopoint
.point
logical term
.point
logical disjunct .OR. logical term
.endpoint
.pc
A logical disjunct is simply a sequence of logical terms each
separated by the .OR. operator.
Rule (2) specifies that the logical terms are combined from left to
right.
.np
.ix 'logical expression'
.ix 'expression' 'logical'
A
.us logical expression
can now be defined as follows.
.autopoint
.point
logical disjunct
.point
logical expression .EQV. logical disjunct
.point
logical expression .NEQV. logical disjunct
.xt or logical expression .XOR. logical disjunct
.endpoint
.pc
A logical expression is therefore a sequence of logical disjuncts,
each separated by the .EQV. operator or the .NEQV.
.xt or .XOR.
operator.
Rules (2) and (3) indicate that logical disjuncts are combined from
left to right.
.np
Consider the following example.
.millust begin
    A .OR. .NOT. B .AND. C
.millust end
.pc
Since the .NOT. operator has highest precedence we first logically
negate
.id B.
The result is then combined with
.id C
using the .AND. operator.
That result is then combined with
.id A
using the .OR. operator to form the final result.
.*
.section Logical Constant Expressions
.*
.np
.ix 'logical constant expression'
.ix 'expression' 'logical constant'
A
.us logical constant expression
is a logical expression in which each primary is one of the following:
.autopoint
.point
logical constant
.point
symbolic logical constant
.point
a relational expression in which each primary is a constant expression
.point
( logical constant expression )
.endpoint
.np
The following are examples of a logical constant expression (assume
that
.id A,
.id B,
.id C
and
.id D
are arithmetic constants appearing in
.kw PARAMETER
statements).
.millust begin
    .TRUE. .AND. .NOT. .FALSE.
    'A' .LT. 'a'
    A * B .GT. C * D
.millust end
.endlevel
.*
.section Evaluating Expressions
.*
.np
Four different types of operators have been discussed; arithmetic,
character, relational and logical.
It is possible to form an expression which contains all of these
operators.
Consider the following example.
.millust begin
    A+B .LE. C .AND. X // Y .EQ. Z .AND. L
.millust end
.pc
where
.id A, B
and
.id C
are of numeric type,
.id X, Y
and
.id Z
are of type CHARACTER
and
.id L
is of type LOGICAL.
In this expression, + is an arithmetic operator, // is a character
operator, .EQ. is a relational operator and .AND. is a logical
operator.
Since we can mix these four types of operators, it is necessary to
define a precedence among these four classes of operators.
The following defines this precedence of operators.
.ix 'expression' 'evaluation of'
.autopoint
.point
arithmetic operators (highest precedence)
.point
character operators
.point
relational operators
.point
logical operators (lowest precedence)
.endpoint
.pc
With this precedence any expression can now be evaluated without
ambiguity.
.*
.section Constant Expressions
.*
.np
.ix 'constant expression'
A
.us constant expression
is an arithmetic constant expression, a character constant expression
or a logical constant expression.
