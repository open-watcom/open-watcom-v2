.chap *refid=fsubp Functions and Subroutines
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
Functions and subroutines are procedures that fall into one of the
following categories.
.autopoint
.point
Statement functions
.point
Intrinsic functions
.point
External functions
.point
Subroutines
.endpoint
.pc
First let us introduce some terminology.
.np
.ix 'program unit'
A
.us program unit
is a collection of &product statements and comments that can be
either a main program or a subprogram.
.np
.ix 'main program'
A
.us main program
identifies the program unit where execution is to begin.
A main program is a program unit which has as its first statement a
.kw PROGRAM
statement or one which does not have a
.kw PROGRAM
.ct ,
.kw FUNCTION
.ct ,
.kw SUBROUTINE
or
.kw BLOCK DATA
statement as its first statement.
Complete execution of the main program implies the complete execution
of the program.
Each executable program can contain only one main program.
.np
.ix 'subprogram'
A
.us subprogram
is a program unit that either has a
.kw FUNCTION
.ct ,
.kw SUBROUTINE
or
.kw BLOCK DATA
statement as its first statement.
This chapter will only deal with subprograms that have a
.kw FUNCTION
or
.kw SUBROUTINE
statement as its first statement.
.*
.section Statement Functions
.*
.np
A statement function is a procedure defined by a single statement.
Its definition must follow all specification statements and precede
the first executable statement.
The statement defining a statement function is not an executable
statement.
.np
.ix 'statement function'
.ix 'function' 'statement'
A
.us statement function
has the following form.
.mbox begin
      sf ( [d [,d] ...] ) = e
.mbox end
.synote
.mnote sf
is the name of the statement function.
.mnote d
is a statement function dummy argument.
.mnote e
is an expression.
.endnote
.pc
The expression
.id e
and the statement function name
.id sf
must conform according to the rules of assignment as described in the
chapter entitled :HDREF refid='fassmnt'..
.np
The statement function dummy arguments are variable names
and are used to indicate the order, number and type of the arguments of
the statement function.
A dummy argument name of a statement function must only appear once
in the dummy argument list of the statement function.
Its scope is the statement defining the statement function.
That is, it becomes defined when the statement function is
referenced and undefined when execution of the statement function
is completed.
A name that is a statement function dummy argument can also be the
name of a variable, a common block, the dummy argument of another
statement function or appear in the dummy argument list of a
.kw FUNCTION
.ct ,
.kw SUBROUTINE
or
.kw ENTRY
statement.
It cannot be used in any other context.
.np
The expression
.id e
can contain any of the following as operands.
.autopoint
.point
A constant.
.point
A symbolic constant.
.point
A variable reference.
This can be a reference to a statement function dummy argument or to a
variable that appears within the same program unit which defines the
statement function.
If the statement function dummy argument has the same name as
a variable in the same program unit, the statement function dummy
argument is used.
The variable reference can also be a dummy argument that appears in the
dummy argument list of a
.kw FUNCTION
or
.kw SUBROUTINE
statement.
If it is a dummy argument that has appeared in the dummy argument list
of an
.kw ENTRY
statement, then the
.kw ENTRY
statement must have previously appeared.
.point
An array element reference.
.point
An intrinsic function reference.
.point
A reference to a statement function whose defining statement has
previously appeared.
.point
An external function reference.
.point
A dummy procedure reference.
.point
An expression enclosed in parentheses which adheres to the rules
specified for the expression
.id e.
.endpoint
.beglevel
.*
.section Referencing a Statement Function
.*
.np
A statement function is referenced by its use in an expression.
The process of executing a statement function involves the following
steps.
.autopoint
.point
The expressions that form the actual arguments to the statement function
are evaluated.
.point
The dummy arguments of the statement function are associated with the
actual arguments.
.point
The expression
.id e
is evaluated.
.point
The value of the result is converted to the type of the statement
function according to the rules of assignment and is available
to the expression that contained the reference to the statement function.
.endpoint
.np
The actual arguments must agree in order, number and type with the
corresponding dummy arguments.
.cp 17
.exam begin
      SUBROUTINE CALC( U, V )
      REAL POLY, X, Y, U, V, Z, CONST
*
* Define a Statement Function.
*
      POLY(X,Y) = X**2 + Y**2 + 2.0*X*Y + CONST
*
* Invoke the Statement Function.
*
      CONST = 23.5
      Z = POLY( U, V )
      PRINT *, Z
      END
.exam end
.pc
In the previous example, note that after the execution of the statement
function, the values of
.id X
and
.id Y
are not equal to the value of
.id U
and
.id V
respectively; they are undefined.
.*
.section Statement Function Restrictions
.*
.autonote
.note
A statement function is local to the program unit in which it is defined.
Thus, a statement function name is not allowed to appear in an
.kw EXTERNAL
statement
and cannot be passed to another procedure as an actual argument.
The following example illegally attempts to pass the statement function
.id F
to the subroutine
.id SAM.
.cp 16
.exam begin
* Illegally passing a statement function
* to a subroutine.
      PROGRAM MAIN
      F(X) = X
          .
          .
          .
      CALL SAM( F )
          .
          .
          .
      END
.exam end
.note
If a statement function
.id F
contains a reference to another statement function
.id G,
then the statement defining
.id G
must have previously appeared.
In the following example, the expression defining the statement function
.id F
illegally references a statement function
.id G
whose defining statement follows
the statement defining
.id F.
.cp 13
.exam begin
* Illegal order of statement functions.
          .
          .
          .
      F(X) = X + G(X)
      G(X) = X + 2
          .
          .
          .
.exam end
.note
The statement function name must not be the same name of any other
entity in the program unit except possibly the name of a common block.
.note
If a dummy argument of a statement function is of type CHARACTER, then
its length specification must be an integer constant expression.
The following is illegal.
.cp 11
.exam begin
      SUBROUTINE SAM( X )
      CHARACTER*(*) X
* Illegal - CHARACTER*(*) dummy argument not
*           allowed in statement function.
      F(X) = X
      PRINT *, F('ABC')
      END
.exam end
.note
An actual argument to a statement function can be any expression, except
character expressions involving the
concatenation of an operand whose length specification is
.mono (*)
unless the operand is a symbolic constant.
.endnote
.endlevel
.*
.section Intrinsic Functions
.*
.np
.ix 'intrinsic function'
.ix 'function' 'intrinsic'
An
.us intrinsic function
is a function that is provided by &product..
.beglevel
.*
.section Specific Names and Generic Names of Intrinsic Functions
.*
.np
.ix 'generic name'
.ix 'specific name'
All intrinsic functions can be referenced by using the
.us generic name
or the
.us specific name
of the intrinsic function.
The specific name uniquely identifies the function to be performed.
The type of the result is predefined thus its name need not appear
in a type statement.
For example, CLOG is a specific name of the generic LOG function and
computes the natural logarithm of a complex number.
The type of the result is also COMPLEX.
.np
When the generic name is used, a specific name is selected based
on the data type of the actual argument.
For example, the generic name of the natural logarithm intrinsic
function is LOG.
To compute the natural logarithm of REAL, DOUBLE PRECISION, COMPLEX or
DOUBLE PRECISION COMPLEX data, the generic name LOG can be used.
Generic names simplify the use of intrinsic functions because the same
name can be used with more than one type of argument.
.autonote Notes:
.setptnt 0 5
.note
It is also possible to pass intrinsic functions to subprograms.
When doing so, only the specific name of the intrinsic function can
be used as an actual argument.
The specific name must have appeared in an
.kw INTRINSIC
statement.
.note
If an intrinsic function has more than one argument, each argument must
be of the same type.
.note
The generic and specific name of an intrinsic function is the same
for some intrinsic functions.
For example, the specific name of the intrinsic function which computes
the sine of an argument whose type is REAL is called SIN which is also
the generic name of the sine function.
.endnote
.if &e'&dohelp eq 0 .do begin
.im fitabmon
.do end
.el .do begin
.im fitabmon
.do end
.endlevel
.*
.section External Functions
.*
.np
.ix 'external function'
.ix 'function' 'external'
An
.us external function
is a program unit that has a
.kw FUNCTION
statement as its first statement.
It is defined externally to the program units that reference it.
The form of a
.kw FUNCTION
statement is defined in the chapter entitled :HDREF refid='fstats'..
.np
The name of an external function is treated as if it was a variable.
It is through the function name that the result of an
external function becomes defined.
This variable must become defined before the execution of the external
function is completed.
Once defined, it can be referenced or redefined.
The value of this variable when a
.kw RETURN
or
.kw END
statement is executed is the result returned by the external function.
.cp 11
.exam begin
      INTEGER FUNCTION VECSUM( A, N )
      INTEGER A(N), I
      VECSUM = 0
      DO 10 I = 1, N
          VECSUM = VECSUM + A(I)
10    CONTINUE
      END
.exam end
.np
If the variable representing the return value of the external function
is of type CHARACTER with a length specification of
.mono (*),
it must not be the operand of a concatenation operator unless it
appears in a character assignment statement.
.np
It is also possible for an external function to return results
through its dummy arguments by assigning to them.
The following example demonstrates this.
.cp 35
.exam begin
      INTEGER MARKS(40), N
      REAL AVG, STDDEV, MEAN
      PRINT *, 'Enter number of marks'
      READ( 5, * ) N
      PRINT *, 'Enter marks'
      READ( 5, * ) (MARKS(I), I = 1, N)
      AVG = MEAN( MARKS, N, STDDEV )
      PRINT *, 'Mean = ', AVG,
     $          ' Standard Deviation = ', STDDEV
      END

*
* Define function MEAN to return the average by
* defining the function name and return the standard
* deviation by defining a dummy argument.
*
      REAL FUNCTION MEAN( A, N, STDDEV )
      INTEGER A, N, I
      REAL STDDEV
      DIMENSION A(N)
      MEAN = 0
      DO 10 I = 1, N
          MEAN = MEAN + A(I)
10    CONTINUE
      MEAN = MEAN / N
      STDDEV = 0
      DO 20 I = 1, N
          STDDEV = STDDEV + ( A(I) - MEAN )**2
20    CONTINUE
      STDDEV = SQRT( STDDEV / (N - 1) )
      END
.exam end
.beglevel
.*
.section Referencing an External Function
.*
.np
When an external function is referenced in an expression
.xt on
or a
.kw CALL
statement,
.xt off
the following steps are performed.
.autopoint
.point
The actual arguments are evaluated.
.point
The actual arguments are associated with the corresponding dummy
arguments.
.point
The external function is executed.
.endpoint
.np
The type of the external function reference must be the same as the
type of the function name in the external function subprogram.
If the external function is of type CHARACTER, the length must also
match.
.*
.section Actual Arguments for an External Function
.*
.np
An actual argument must be one of the following.
.autopoint
.point
Any expression except a character expression involving the
concatenation of an operand whose length specification is
.mono (*)
unless the operand is a symbolic constant.
.point
An array name.
.point
An intrinsic function name (must be the specific name) that has appeared
in an
.kw INTRINSIC
statement.
.point
An external procedure name.
.point
A dummy procedure name.
.endpoint
.np
The actual arguments of an external function reference must match
the order, number and type of the corresponding dummy arguments.
If a subroutine is an actual argument, then type agreement is not
required since a subroutine has no type.
.*
.section External Function Subprogram Restrictions
.*
.autonote
.setptnt 0 5
.note
The name of an external function is a global name and must not be
the same as any other global name or name local to the subprogram
whose name is that of the external function.
Note that the external function name is treated as a variable within the
external function subprogram.
.note
The name of a dummy argument is a name local to the subprogram and must
not appear in an
.kw EQUIVALENCE
.ct ,
.kw PARAMETER
.ct ,
.kw SAVE
.ct ,
.kw INTRINSIC
or
.kw DATA
statement within the same subprogram.
It may appear in a
.kw COMMON
statement only as the name of a common block.
.note
The name of the external function can in no way, directly or indirectly,
be referenced as a subprogram from within the subprogram it defines.
It can appear in a type statement to establish its type only if
the type has not been established in the
.kw FUNCTION
statement.
.endnote
.endlevel
.*
.section Subroutines
.*
.np
.ix 'subroutine'
A
.us subroutine
is a program unit that has a
.kw SUBROUTINE
statement as it first statement.
It is defined externally to the program units that reference it.
The form of a
.kw SUBROUTINE
statement can be found in the chapter entitled :HDREF refid='fstats'..
.np
A subroutine differs from a function in that it does not return
a result and hence has no type associated with it.
However, it is possible to return values from a subroutine by
defining or redefining the dummy arguments of the subroutine.
.beglevel
.*
.section Referencing a Subroutine: The CALL Statement
.*
.np
Unlike a function, a subroutine cannot appear in an expression.
Subroutines are referenced by using a
.kw CALL
statement.
See the chapter entitled :HDREF refid='fstats'. for details on the
.kw CALL
statement.
When a
.kw CALL
statement is executed, the following steps are performed.
.autopoint
.point
The actual arguments are evaluated.
.point
The actual arguments are associated with the corresponding dummy
arguments.
.point
The subroutine is executed.
.endpoint
.np
A subroutine can be called from any subprogram but must not be
called by itself, indirectly or directly.
.*
.section Actual Arguments for a Subroutine
.*
.np
Each actual argument in a subroutine call must be one of the following.
.autopoint
.point
Any expression except a character expression involving the
concatenation of an operand whose length specification is
.mono (*)
unless the operand is a symbolic constant.
.point
An array name.
.point
An intrinsic function name (must be the specific name) that has appeared
in an
.kw INTRINSIC
statement.
.point
An external procedure name.
.point
A dummy procedure name.
.point
.ix 'alternate return specifier'
An
.us alternate return specifier
of the form
.id *s
where
.id s
is a statement number of an executable statement in the subprogram
which contained the
.kw CALL
statement.
This will be covered in more detail when the
.kw RETURN
statement is discussed.
.endpoint
.np
The actual arguments must agree in order, number and type with the
corresponding dummy arguments.
The type agreement does not apply to an actual argument which
is an alternate return specifier or a subroutine name since
neither has a type.
.*
.section Subroutine Subprogram Restrictions
.*
.autonote
.setptnt 0 5
.note
A subroutine subprogram can contain any statement except a
.kw FUNCTION
.ct ,
.kw BLOCK DATA
or
.kw PROGRAM
statement.
.note
The name of a subroutine is a global name and must not be used as
another global name.
Furthermore, no local name in the subroutine subprogram can have the
same name as the subroutine.
.note
The name of a dummy argument is local to the subroutine subprogram
and must not appear in an
.kw EQUIVALENCE
.ct ,
.kw PARAMETER
.ct ,
.kw SAVE
.ct ,
.kw INTRINSIC
or
.kw DATA
statement.
It may appear in a
.kw COMMON
statement only as the name of a common block.
.endnote
.endlevel
.*
.section The ENTRY Statement
.*
.np
An
.kw ENTRY
statement allows execution of a subprogram to begin at a particular
executable statement within the subprogram in which it appears.
.ix 'entry point'
An
.kw ENTRY
statement defines an alternate
.us entry point
into a subprogram
and can appear anywhere after the
.kw FUNCTION
statement in a function subprogram or the
.kw SUBROUTINE
statement in a subroutine subprogram.
Also, it must not appear as a statement between the beginning and end
of a control structure.
For example, an
.kw ENTRY
statement cannot appear between a block
.kw IF
statement and its corresponding
.kw END IF
statement or between a
.kw DO
statement and the corresponding terminal statement.
It is possible to have more than one
.kw ENTRY
statement in a subprogram.
An
.kw ENTRY
statement is a non-executable statement.
The form of an
.kw ENTRY
statement can be found in the chapter entitled :HDREF refid='fstats'..
.np
Each entry name defines an external function if it appears in a
function, or an external subroutine if it appears in a subroutine
and is referenced in the same way as the actual function or subroutine
name would be referenced.
Execution begins at the first executable statement that follows
the
.kw ENTRY
statement.
The order, number, type and names of the dummy argument lists
of an
.kw ENTRY
statement may be different from that of a
.kw FUNCTION
.ct ,
.kw SUBROUTINE
or other
.kw ENTRY
statement.
However, there must still be agreement between the actual argument list
used to reference an entry name and the dummy argument list in the
corresponding
.kw ENTRY
statement.
.beglevel
.*
.section ENTRY Statements in External Functions
.*
.np
Entry names may also appear in type statements.
Their type may or may not be the same type as other entry names or
the actual name of the external function unless the function is of type
CHARACTER.
If the function is of type CHARACTER then the type of all the entry
names must be of type CHARACTER.
Conversely, if an entry name is of type CHARACTER, then all other entry
names and the function name must be of type CHARACTER.
An entry name, like external function names, is treated as a variable
within the subprogram it appears.
Within a function subprogram, there is an association between variables
whose name is an entry name and the variable whose name corresponds to
the external function.
When such a variable becomes defined, all other such variables of the
.us same
type also become defined and other such variables not of the
same type become undefined.
This can be best illustrated by an example.
.cp 19
.exam begin
      PRINT *, EVAL(2), EVAL3(4.0)
      END

      INTEGER FUNCTION EVAL( X )
      INTEGER EVAL2, X
      REAL EVAL3, Y
      C = 1
      GOTO 10
      ENTRY EVAL2( X )
      C = 2
      GOTO 10
      ENTRY EVAL3( Y )
      C = 3
10    EVAL2 = C * X
      END
.exam end
.pc
In the previous example, invoking
.id EVAL
would cause the result of
2 to be returned even though
.id EVAL
was never assigned to in the function
.id EVAL;
but since
.id EVAL2
and
.id EVAL
are of the same type they are
associated and hence defining
.id EVAL2
causes
.id EVAL
to be defined.
However, invoking
.id EVAL3
would cause an undefined result to be
returned since
.id EVAL3
is of type REAL and
.id EVAL2
is of type INTEGER and
hence are not associated.
.id EVAL3
does not become defined.
.*
.section ENTRY Statement Restrictions
.*
.autonote
.setptnt 0 5
.note
An entry name may not appear in any statement previous to the
.kw ENTRY
statement containing the entry name except in a type statement.
.note
If an entry name in a function is of type CHARACTER, each entry name
and the name of the function must also be of type CHARACTER.
If the name of the function or the name of any entry point has a
length specification of
.mono (*),
then all such entities must have a length specification of
.mono (*);
otherwise they must all have a length specification
of the same integer value.
.note
If a dummy argument appears in an executable statement, then that
statement can be executed provided that the dummy argument is in the
dummy argument list of the procedure name referenced.
.note
A name that appears as a dummy argument in an
.kw ENTRY
statement must not appear in the expression of a statement function
unless it is a dummy argument of the statement function, it has
appeared in the dummy argument list of a
.kw FUNCTION
or
.kw SUBROUTINE
statement, or the
.kw ENTRY
statement appears before the statement function statement.
.note
A name that appears as a dummy argument in an
.kw ENTRY
statement must not appear in an executable statement preceding
the
.kw ENTRY
statement unless it has also appeared in a
.kw FUNCTION
.ct ,
.kw SUBROUTINE
.ct , or
.kw ENTRY
statement that precedes the executable statement.
.endnote
.endlevel
.*
.section The RETURN Statement
.*
.np
A
.kw RETURN
statement is a way to terminate the execution of a function or
subroutine subprogram and return control to the program unit that
referenced it.
.xt on
As an extension to FORTRAN 77, &product permits the use of the
.kw RETURN
statement in the main program.
.xt off
A subprogram (or main program) may contain more than one
.kw RETURN
statement or it may contain no
.kw RETURN
statement.
In the latter case, the
.kw END
statement has the same effect as a
.kw RETURN
statement.
.np
Execution of a
.kw RETURN
or
.kw END
statement causes all local entities to become undefined except for the
following.
.autopoint
.point
Entities specified in a
.kw SAVE
statement.
.point
Entities in blank common.
.point
Initially defined entities that have neither been redefined nor become
undefined.
.point
Entities in a named common block that appears in the subprogram
and in a program unit that references the subprogram directly or
indirectly.
.endpoint
.*
.beglevel
.*
.section RETURN Statement in the Main Program (Extension)
.*
.np
.xt begin
The form of a
.kw RETURN
statement in a main program is:
.xt end
.mext begin
      RETURN
.mext end
.np
.xt begin
When a
.kw RETURN
statement is executed in the main program, program execution
terminates in the same manner as the
.kw STOP
or
.kw END
statement.
This is an extension to FORTRAN 77.
.xt end
.*
.section RETURN Statement in Function Subprograms
.*
.np
The form of a
.kw RETURN
statement in a function subprogram is:
.mbox begin
      RETURN
.mbox end
.np
When a
.kw RETURN
statement is executed in a function subprogram, the function value
must be defined.
Control is then passed back to the program unit that referenced it.
.*
.section RETURN Statement in Subroutine Subprograms
.*
.np
The form of a
.kw RETURN
statement in a subroutine subprogram is:
.mbox begin
      RETURN [ e ]
.mbox end
.synote
.mnote e
is an integer expression.
.endnote
.pc
If the expression
.id e
is omitted or has a value less than one or greater than the number of
asterisks appearing in the dummy argument list of the subroutine or
entry name referenced, then control is returned to the next
executable statement that follows the
.kw CALL
statement in the referencing program unit.
If 1 <=
.id e
<=
.id n
where
.id n
is the number of asterisks appearing in the
.kw SUBROUTINE
or
.kw ENTRY
statement which contains the referenced name, then the expression
.id e
identifies the
.id e
.ct th
asterisk in the dummy argument list.
Control is returned to the statement identified by the alternate
return specified in the
.kw CALL
statement that corresponds to the
.id e
.ct th
asterisk in the dummy argument list of the referenced subroutine.
The following example demonstrates the use of alternate return
specifiers in conjunction with the
.kw RETURN
statement.
.cp 19
.exam begin
      REAL X, Y
      READ *, X, Y
      CALL CMP( X, Y, *10, *20 )
      PRINT *, 'X equals Y'
      GOTO 30
10    PRINT *, 'X less than Y'
      GOTO 30
20    PRINT *, 'X greater than Y'
30    END

      SUBROUTINE CMP( X, Y, *, * )
      IF( X .LT. Y )RETURN 1
      IF( X .GT. Y )RETURN 2
      RETURN
      END
.exam end
.endlevel
.*
.section Subprogram Arguments
.*
.np
Arguments provide a means of communication between program units.
Arguments are passed to subprograms through argument lists and
are received by subprograms through argument lists.
.ix 'actual argument list'
.ix 'actual argument'
.ix 'argument' 'actual'
The argument list used to pass arguments to a subprogram is called the
.us actual argument list
and the arguments are called
.us actual arguments.
.ix 'dummy argument list'
.ix 'dummy argument'
.ix 'argument' 'dummy'
The argument list of the receiving subprogram is called the
.us dummy argument list
and the arguments are called
.us dummy arguments.
The actual argument list must agree with the dummy argument list in
number, order and type.
.beglevel
.*
.section Dummy Arguments
.*
.np
Statement function, external functions and subroutines use dummy
arguments to define the type of actual arguments they expect.
A dummy argument is one of the following.
.autopoint
.point
Variable.
.point
Array.
.point
Dummy procedure.
.point
Asterisk
.mono (*)
indicating a statement label.
.endpoint
.autonote Notes:
.setptnt 0 5
.note
A statement function dummy argument may only be a variable.
.note
An asterisk can only be a dummy argument for
a subroutine subprogram.
.endnote
.np
Dummy arguments that are variables of type INTEGER can be used in
dummy array declarators.
No dummy argument may appear in an
.kw EQUIVALENCE
.ct ,
.kw DATA
.ct ,
.kw PARAMETER
.ct ,
.kw SAVE
.ct ,
.kw INTRINSIC
or
.kw COMMON
statement except as a common block name.
A dummy argument must not be the same name as the subprogram name
specified in the
.kw FUNCTION
.ct ,
.kw SUBROUTINE
or
.kw ENTRY
statement.
Other than these restrictions, dummy arguments can be used in the same
way an actual name of the same class would be used.
.*
.section Actual Arguments
.*
.np
Actual arguments specify the entities that are to be associated with
the dummy arguments when referencing a subroutine or function.
Actual arguments can be any of the following.
.autopoint
.point
Any expression, except character expression involving the concatenation
of an operand whose length specification is
.mono (*)
unless the operand is a symbolic constant.
.point
An array name.
.point
An intrinsic function name.
.point
An external function or subroutine name.
.point
A dummy procedure name.
.point
.ix 'alternate return specifier'
An
.us alternate return specifier
of the form
.id *s
where
.id s
is a statement number of an executable statement in the subprogram
which contained the
.kw CALL
statement.
.endpoint
.autonote Notes:
.setptnt 0 5
.note
A statement function actual argument can only be a variable or an
expression.
.note
An alternate return specifier can only be an actual argument in the
actual argument list of a subroutine reference.
.endnote
.*
.section Association of Actual and Dummy Arguments
.*
.np
When a function or subroutine reference is executed, an association is
established between the actual arguments and the corresponding
dummy arguments.
The first dummy argument is associated with the first actual argument,
the second dummy argument is associated with the second actual argument,
etc.
Association requires that the types of the actual and dummy arguments
agree.
A subroutine has no type and when used as an actual argument
must be associated with a dummy procedure.
An alternate return specifier has no type and must be associated with
an asterisk.
Arguments can be passed through more than one level of procedure
reference.
In this case, valid association must exist at all intermediate levels
as well as the last level.
Argument association is terminated upon the execution of a
.kw RETURN
or
.kw END
statement.
.beglevel
.*
.section Length of Character Actual and Dummy Arguments
.*
.np
.ix 'dummy argument' 'of type CHARACTER'
If a dummy argument is of type CHARACTER, the corresponding actual
argument must also be of type CHARACTER and the length of the
dummy argument must be less than or equal to the length of the actual
argument.
If the length of the dummy argument is
.id len
then the
.id len
leftmost characters of the actual argument are associated with the
dummy argument.
.np
If a dummy argument of type CHARACTER is an array name, then the
restriction on the length is on the whole array and not for each
array element.
The length of an array element of the dummy argument may be different
from the length of the array element of the corresponding actual
array, array element, or array element substring, but the
dummy array argument must not extend beyond the end of the associated
actual array.
.*
.section Variables as Dummy Arguments
.*
.np
.ix 'dummy argument' 'variable'
A dummy argument that is a variable may be associated with an actual
argument that is a variable, array element, substring or expression.
Only if the actual argument is a variable, array element or substring
can the corresponding actual argument be redefined.
.*
.section Arrays as Dummy Arguments
.*
.np
.ix 'dummy argument' 'array'
A dummy argument that is an array may be associated with an actual
argument that is an array, array element or array element substring.
The number and size of the dimensions in the actual argument array
declarator may be different from the number and size of the dimensions
in the dummy argument array declarator.
.np
If the actual argument is a non-character array name, then the size of
the dummy argument array must not exceed the size of the actual argument
array.
An element of the actual array becomes associated with the
element in the dummy array with the same subscript value.
Association by array element of character arrays exists only if the
lengths of the array elements are the same.
If their lengths are not the same, the dummy and actual array elements
will not consist of the same characters.
.np
If the actual argument is a non-character array element name
whose subscript value is
.us asv
the size of the dummy argument array must not exceed the size of the
actual argument array less
.us asv - 1.
Furthermore, the dummy argument array element whose subscript value is
.us dsv
becomes associated with the actual argument array element whose
subscript value is
.us asv + dsv - 1.
Consider the following example.
.cp 14
.exam begin
      DIMENSION A(10)
      CALL SAM( A(3) )
      END

      SUBROUTINE SAM( B )
      DIMENSION B(5)
          .
          .
          .
      END
.exam end
.pc
In the previous example, array
.id A
is an actual argument and the array
.id B
is the dummy argument.
Suppose we wanted to know which element of
.id A
is associated with the 4th element of
.id B.
Then
.us asv
would have value 3 since the array element
.id A(3)
is the actual argument, and
.us dsv
is 4.
Then the 4th element in
.id B
is 3 + 4 - 1 = 6th element of
A.
.np
If the actual argument is a character array name, character array
element name or character array element substring which begins at
character storage unit
.us ach,
then the character storage unit
.us dch
of the dummy argument array is associated with the character storage unit
.us ach + dch - 1
of the actual array.
The size of the dummy character array must not exceed the size of the
actual argument array.
.*
.section Procedures as Dummy Arguments
.*
.np
.ix 'dummy argument' 'dummy procedure'
A dummy argument that is a dummy procedure can only be associated
with an actual argument that is one of the following.
.autopoint
.point
Intrinsic function.
.point
External function.
.point
External Subroutine.
.point
Another dummy procedure.
.endpoint
.np
If the dummy argument is used as a subroutine (that is it is invoked
using a
.kw CALL
statement) then the corresponding actual argument must either be
a subroutine or a dummy procedure.
If the dummy argument is used as an external function, then the
corresponding actual argument must be an intrinsic function, external
function or dummy procedure.
Note that it may not be possible to determine in a given program unit
whether a dummy procedure is associated with a function or subroutine.
In the following example
it is not possible to tell by looking at this program unit whether
.id PROC
is an external subroutine or function.
.cp 13
.exam begin
      SUBROUTINE SAM( PROC )
      EXTERNAL PROC
          .
          .
      CALL SAM1( PROC )
          .
          .
          .
      END
.exam end
.*
.section Asterisks as Dummy Arguments
.*
.np
.ix 'dummy argument' 'asterisk'
A dummy argument that is an asterisk may only appear in the dummy
argument list of a
.kw SUBROUTINE
statement or an
.kw ENTRY
statement in a subroutine subprogram and may be associated only with
an actual argument that is an alternate return specifier in a
.kw CALL
statement which references the subroutine.
.cp 15
.exam begin
      CHARACTER*10 RECORD(5)
      I = 2
      CALL SAM( I, *999, 3HSAM )
      PRINT *, 'I should be skipped'
999   PRINT *, 'I should be printed'
      END
      SUBROUTINE SAM( I, *, K)
      CHARACTER*3 K
      PRINT *, K
      RETURN 1
      END
.exam end
.endlevel
.endlevel
