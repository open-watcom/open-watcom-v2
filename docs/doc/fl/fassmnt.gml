.chap *refid=fassmnt Assignment Statements
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
Assignment statements are used to define entities.
There are four different types of assignment.
.autopoint
.point
Arithmetic
.point
Logical
.point
Statement label (ASSIGN)
.point
Character
.endpoint
.*
.section Arithmetic Assignment
.*
.np
.ix 'arithmetic assignment statement'
.ix 'assignment statement' 'arithmetic'
The form of an
.us arithmetic assignment statement
is
.mbox begin
      v = e
.mbox end
.synote
.mnote v
is a variable name or array element name of type INTEGER, REAL, DOUBLE
PRECISION, COMPLEX
.xt or double precision complex (COMPLEX*16).
.mnote e
is an arithmetic expression.
.endnote
.np
The following are examples of arithmetic assignment statements.
.millust begin
    Y = X**2 + 4.0*X + 3.0
    Z(10) = 4.3*(X+Y)
.millust end
.pc
Executing an arithmetic assignment statement causes the evaluation of the
arithmetic expression
.id e,
converting the type of the expression
.id e
to the type of
.id v,
and defining
.id v
with the result.
.np
If
.id v
is of type INTEGER*1 or INTEGER*2, then the value of the expression
.id e
is first converted to type INTEGER.
The resulting integer is then assigned to
.id v
in the following way.
.autopoint
.point
If
.id v
is of type INTEGER*2 and the value of
.id e
is such that &minus.32768 <=
.id e
<= 32767,
.id v
will be assigned the value of
.id e.
Otherwise,
.id v
will be undefined.
.point
If
.id v
is of type INTEGER*1 and the value of
.id e
is such that &minus.128 <=
.id e
<= 127,
.id v
will be assigned the value of
.id e.
Otherwise,
.id v
will be undefined.
.endpoint
.*
.section Logical Assignment
.*
.np
.ix 'logical assignment statement'
.ix 'assignment statement' 'logical'
The form of a
.us logical assignment statement
is
.mbox begin
      v = e
.mbox end
.synote
.mnote v
is a variable name or array element name of type LOGICAL.
.mnote e
is a logical expression.
.endnote
.np
The following are examples of logical assignment statements.
.millust begin
    LOG1 = .TRUE.
    LOG2 = (X.GT.Y) .AND. (X.LT.Z)
    LOG3(2) = LOG2 .EQV. LOG1
.millust end
.pc
Executing a logical assignment statement causes the evaluation of the
logical expression
.id e,
and defining
.id v
with the result.
Note that the type of
.id v
and
.id e
must be LOGICAL.
.*
.section Statement Label Assignment
.*
.np
.ix 'statement label assignment'
.ix 'assignment statement' 'statement label'
The form of a
.us statement label assignment
is
.mbox begin
      ASSIGN s to i
.mbox end
.synote
.mnote s
is a statement label.
.mnote i
is the name of an integer variable.
.endnote
.np
The following is an example of a statement label assignment statement.
.millust ASSIGN 10 TO I
The result of executing an
.kw ASSIGN
statement causes the integer variable
.id i
to be defined with the value of the statement label
.id s.
.id s
must be the statement label of an executable statement or a format
statement in the same program unit in which the
.kw ASSIGN
statement appears.
It is possible to change the value of
.id i
by executing another
.kw ASSIGN
statement.
.np
During execution when
.id i
is used in an assigned
.kw GO TO
statement,
an
.kw ASSIGN
statement which defines
.id i
must have been executed prior to the execution of the assigned
.kw GO TO
statement.
.np
While the variable
.id i
is defined with a statement label, it should not be used in any other
way other than in an assigned
.kw GO TO
statement.
Consider the following example.
.exam begin
10    ASSIGN 10 TO I
* Illegal use of an ASSIGNed variable
      PRINT *, I
.exam end
.pc
The output produced by the
.kw PRINT
statement is
.us not
the integer 10.
Its value is undefined and should be treated that way.
.*
.section Character Assignment
.*
.np
.ix 'character assignment statement'
.ix 'assignment statement' 'character'
The form of a
.us character assignment statement
is
.mbox begin
      v = e
.mbox end
.synote
.mnote v
is a character variable name, character array element, or character
substring.
.mnote e
is a character expression.
.endnote
.np
The following are examples of character assignment statements.
.millust begin
    CHARACTER*20 C,D(5)
    C='ABCDEF'
    C(3:5)='XYZ'
    D(5)(14:15)='12'
.millust end
.pc
Executing a character assignment statement causes the evaluation of the
character expression
.id e
and the definition of
.id v
with the result.
.np
None of the character positions defined in
.id v
may be referenced in
.id e.
The following example is illegal since the 4th and 5th character
positions of
.id A
appear on the left and right hand side of the equal sign.
.exam begin
* Illegal character assignment.
      CHARACTER*10 A,B*5
      A(2:6) = A(4:5) // B
.exam end
.np
The length of
.id v
and
.id e
may be different.
If the length of
.id v
is less than the length of
.id e
then
the assignment has the effect of truncating
.id e
from the right to the length of
.id v.
If the length of
.id v
is greater than the length of
.id e,
the value assigned to
.id v
is the value of
.id e
padded on the right with blanks to the length of
.id v.
.*
.section Extended Assignment Statement
.*
.np
.ix 'extended assignment statement'
.ix 'assignment statement' 'extended'
&product supports an extension to the FORTRAN 77 assignment
statement, namely the 
.us extended assignment statement
shown here:
.mext begin
      v  =  v  =  v  = ... =  v  = e
      1    2    3          n
.mext end
.synote
.mnote v'i
must be one of the following:
.autonote
.note
Variable names or array element names of type INTEGER, REAL, DOUBLE
PRECISION, COMPLEX
.xt or double precision complex (COMPLEX*16).
.note
Variable names or array element names of type LOGICAL.
.note
Character variable names, character array elements, or character
substrings.
.endnote
.mnote e
must be one of the following and must follow the rules
of the arithmetic, logical or character assignment statements:
.autonote
.note
An arithmetic expression.
.note
A logical expression.
.note
A character expression.
.endnote
.endnote
.np
The extended assignment statement is equivalent to the following
individual statements.
.millust begin
    v    =   e
     n
    v    =   v
     n-1      n
         .
         .
         .
    v    =   v
     2        3
    v    =   v
     1        2
.millust end
.np
When using an extended assignment statement involving variables
of mixed type, it is important to understand the exact way in
which the assignments are performed.
Assignment of each variable is made using the value of the
variable to its immediate right, starting with the rightmost
variable which is assigned the value of the expression.
To help make this clear, consider the following program.
.cp 15
.exam begin
      CHARACTER C1*10, C2*5, C3*7
      LOGICAL L1, L2, L3
      INTEGER*2 K, L
      I = S = J = T = 1.25
      PRINT *, I, S, J, T
      I = K = J = L = 70000
      PRINT *, I, K, J, L
      C1 = C2 = C3 = 'ABCDEFGHIJKL'
      PRINT *, C1, C2, C3
      L1 = L2 = L3 = .TRUE.
      PRINT *, L1, L2, L3
      END
.exam end
.pc
The output from this program would be:
.millust begin
     1 1.0000000         1 1.2500000
  4464      4464      4464      4464
 ABCDE     ABCDEABCDEFG
     T         T         T
.millust end
.pc
Note that variables
.mono K
and
.mono L
are of type INTEGER*2 and cannot contain any value greater than
32767.
Truncation resulted and this value (4464) was propagated to the
left.
