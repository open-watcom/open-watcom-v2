.ix 'statement'
.pp
A
.us statement
describes what actions are to be performed.
Statements may only be placed inside functions.
Statements are executed in sequence, except where described below.
.pp
.*
.section Labelled Statements
.*
.pp
Any statement may be preceded by a
.ix 'label'
.ix 'statement' 'label'
.us label.
Labelled statements are usually the target of a
.kw goto
statement, and hence occur infrequently.
.pp
A label is an identifier followed by a colon. Labels do
not
affect the flow of execution of a program.
A label that is encountered during execution is ignored.
.keep begin
.pp
The following example illustrates a statement with a label:
.millust begin
xyz: i = 0;
.millust end
.keep end
.pp
Labels can only precede statements.
It follows that labels
may only appear inside functions.
.pp
A label may be defined only once within any particular function.
.pp
The identifier used for a label may be the same as another identifier
for an object, function or tag, or a label in another function.
The
.ix 'name space' 'labels'
.us name space
for labels is separate from non-label identifiers,
and each function has its own label name space.
.*
.section Compound Statements
.*
.pp
A
.ix 'compound statement'
.ix 'statement' 'compound'
.us compound statement
is a set of statements grouped together inside braces.
It may have its own
declarations of objects, with or without initializations, and
may or may not have any executable statements.
A compound statement is also called a
.ix 'block'
.us block.
.keep begin
.pp
The general form of a compound statement is:
.cillust begin
.mono {
declaration-list statement-list
.mono }
.cillust end
.keep end
.pc
where
.us declaration-list
is a list of zero or more
declarations of objects to be used in the block.
.us statement-list
is a list of zero or more
statements to be executed when the block is entered.
.pp
Any declarations for objects that have
.ix 'automatic storage duration'
.ix 'storage duration' 'automatic'
automatic storage duration and initializers for them are evaluated
in the order in which they occur.
.pp
An object declared with the keyword
.kw extern
inside a block may not be initialized in the declaration,
since the storage for that object is defined elsewhere.
.pp
An object declared in a block,
without the keyword
.kw extern
..ct ,
may not be redeclared within the same block,
except in a block contained within the current block.
.*
.section Expression Statements
.*
.pp
A statement that is an expression is evaluated as a void expression
for its side effects, such as the
assigning of a value with the
assignment operator.
The result of the expression is discarded. This discarding may be
made explicit by casting the expression as a
.kw void
..ct ..li .
.pp
For example, the statement,
.millust begin
count = 3;
.millust end
.pc
consists of the expression
.mono count = 3
..ct ,
which has the side effect of assigning the value 3 to the object
.mono count.
The result of the expression is 3, with the type the same as the
type of
.mono count.
The result is not used any further.
As another example, the statement,
.millust begin
(void) memcpy( dest, src, len );
.millust end
.pc
indicates that, regardless of the fact that
.libfn memcpy
returns a result,
the result should be ignored.
However, it is equally valid, and quite common, to write,
.millust begin
memcpy( dest, src, len );
.millust end
.pc
As a matter of programming style,
casting an expression as
.kw void
should only be done when the result of the expression might
normally be expected to be used further. In this case, casting to
.kw void
indicates that the result was intentionally discarded and is not
an error of omission.
.*
.section Null Statements
.*
.pp
.ix 'null' 'statement'
.ix 'empty statement'
.ix 'statement' 'null'
.ix 'statement' 'empty'
A null statement, which is just a semi-colon, takes no action.
It is useful for placing a label just before a block-closing brace,
or for indicating an empty block, such as in an iteration statement.
Consider the following examples of null statements:
.millust begin
{
    gets( buffer );
    while( *buffer++ != '\0' )
        ;
    /* ... */
    endblk: ;
}
.millust end
.pc
The
.kw while
iteration statement skips over characters in
.mono buffer
until the null character is found. The body of the iteration is
empty, since the controlling expression does all of the work.
The
.mono endblk:
declares a label just before the final }, which might be used by a
.ix 'statement' 'goto'
.ix 'goto statement'
.kw goto
to exit the block.
.*
.section Selection Statements
.*
.beglevel
.pp
A
.ix 'selection statement'
.ix 'statement' 'selection'
.us selection statement
evaluates an expression, called the
.ix 'controlling expression'
.us controlling expression,
then based on the result selects from a set of statements. These
statements are then executed.
.*
.section The if Statement
.ix 'if statement'
.ix 'statement' 'if'
.*
.pp
.cillust begin
.mono if(
expression
.mono )
statement
.cor
.mono if(
expression
.mono )
statement
.mono else
statement
.cillust end
.pc
In both cases, the type of the controlling expression
(inside the parentheses) is a scalar type (not a structure, union or
array). If the controlling expression evaluates to a non-zero value,
then the first statement is executed.
.pp
In the second form, the
.ix 'else statement'
.kw else
is executed if the controlling expression evaluates to zero.
.keep begin
.pp
Each statement may be a compound statement. For example,
.millust begin
if( delay > 5 ) {
    printf( "Waited too long\n" );
    ok = FALSE;
} else {
    ok = TRUE;
}
.millust end
.keep end
.pp
In the classic case of the dangling
.kw else
..ct ,
the
.kw else
is bound to the nearest
.kw if
that does not yet have an
.kw else
..ct ..li .
For example,
.millust begin
if( x > 0 )
    if( y > 0 )
        printf( "x > 0 && y > 0\n" );
else
    printf( "x <= 0\n" );
.millust end
.pc
will print
.mono x&SYSRB.<=&SYSRB.0
when
.mono x&SYSRB.>&SYSRB.0
is true and
.mono y&SYSRB.>&SYSRB.0
is false, because the
.kw else
is bound to the second
.kw if
..ct ,
not the first.
To correct this example, it would have to be changed to,
.millust begin
if( x > 0 ) {
    if( y > 0 )
        printf( "x > 0 && y > 0\n" );
} else
    printf( "x <= 0\n" );
.millust end
.pp
This example illustrates why it is a good idea to always use braces to
explicitly state the subject of the control structures, rather than
relying on the fact that a single statement is also a compound
statement. A better way of writing the above example is,
.millust begin
if( x > 0 ) {
    if( y > 0 ) {
        printf( "x > 0 && y > 0\n" );
    }
} else {
    printf( "x <= 0\n" );
}
.millust end
.pc
where all subjects of the control structures are contained within
braces, leaving no doubt about the meaning.
A dangling
.kw else
cannot occur if braces are always used.
.pp
If the statements between the
.kw if
and the
.kw else
are reached via a label, the statements following the
.kw else
will not be executed.
However, jumping into a block is poor
programming practice, since it makes the program difficult to follow.
.*
.section The switch Statement
.ix 'switch statement'
.ix 'statement' 'switch'
.*
.cillust begin
.mono switch(
expression
.mono )
statement
.cillust end
.pc
Usually,
.us statement
is a
.ix 'compound statement'
.ix 'statement' 'compound'
compound statement or
.ix 'block'
block.
Embedded within the
statement
are
.ix 'case label'
.kw case
labels and possibly a
.ix 'default label'
.kw default
label, of the following form:
.cillust begin
.mono case
constant-expression : statement
.cbr
.mono default :
statement
.cillust end
.pp
The controlling expression and the constant-expressions on each
.kw case
label all must have integral type. No two of the
.kw case
constant-expressions may be the same value. The
.kw default
label may appear at most once in any
.kw switch
block.
.pp
The controlling statement is evaluated,
and the integral promotion is performed on the result.
If the promoted value of the
expression matches any of the case labels promoted to the same type,
control is given to the
statement following that case label. Otherwise, control is given
to the statement following the
.kw default
label (if present). If no default label is present,
then no statements in the
.kw switch
block are executed.
.pp
When statements within a
.kw switch
block are being executed and another
.kw case
or
.kw default
is encountered, it is ignored and
execution continues with the statement following the label. The
.ix 'break statement'
.ix 'statement' 'break'
.kw break
statement
may be used to terminate execution of the switch block.
.keep begin
.pp
In the following example,
.millust begin
int i;

for( i = 1; i <= 8; i++ ) {
    printf( "%d ", i );
    switch( i ) {
      case 2:
      case 4:
        printf( "less than 5 " );
      case 6:
      case 8:
        printf( "even\n" );
        break;
      default:
        printf( "odd\n" );
    }
}
.millust end
.keep end
.keep begin
.pc
the following output is produced:
.millust begin
1 odd
2 less than 5 even
3 odd
4 less than 5 even
5 odd
6 even
7 odd
8 even
.millust end
.keep end
.endlevel
.*
.section Iteration Statements
.*
.ix 'looping'
.ix 'iteration'
.ix 'statement' 'looping'
.ix 'statement' 'iteration'
.pp
Iteration statements control looping. There are three forms of
iteration statements:
.kw while
..ct ,
.kw do
..ct /
..ct .kw while
and
.kw for
..ct ..li .
.pp
The controlling expression must have a scalar type.
The
.us loop body
(often a
.ix 'compound statement'
.ix 'statement' 'compound'
compound statement
or
.ix 'block'
block)
is executed repeatedly until the controlling expression
is equal to zero.
.beglevel
.keep begin
.*
.section The while Statement
.*
.ix 'while statement'
.ix 'statement' 'while'
.cillust begin
.mono while
.mono (
expression
.mono )
statement
.cillust end
.keep end
.pp
The evaluation of the controlling expression takes place before each
execution of the loop body (:ITAL.statement:eITAL.).
If the expression evaluates to zero the first time, the loop body
is not executed at all.
.pp
The
.us statement
may be a compound statement.
.keep begin
..sk 1 c
For example,
.millust begin
char * ptr;
/* ... */
while( *ptr != '\0' ) {
    if( *ptr == '.' )break;
    ++ptr;
}
.millust end
.keep end
.pc
The loop will scan characters pointed at by
.mono ptr
until either a null character or a dot is found.
If the initial value of
.mono ptr
points at a null character, then no part of the loop body will
be executed, leaving
.mono ptr
pointing at the null character.
.*
.section The do Statement
.*
.ix 'do statement'
.ix 'statement' 'do'
.cillust begin
.mono do
statement
.mono while
.mono (
expression
.monoon
);
.monooff
.cillust end
.pp
The evaluation of the controlling expression takes place after each
execution of the loop body (
..ct .us statement
..ct ).
If the expression evaluates to zero the first time, the loop body
is executed exactly once.
.pp
The
.us statement
may be a compound statement.
.keep begin
..sk 1 c
For example,
.millust begin
char * ptr;
char * endptr;
/* ... */
endptr = ptr + strlen( ptr );
do {
    --endptr;
} while( endptr >= ptr  &&  *endptr == ' ' );
.millust end
.keep end
.pc
In this example, the loop will terminate when
.mono endptr
finds a non-blank character starting from the right, or when
.mono endptr
goes past the beginning of the string.
If a non-blank character is found,
.mono endptr
will be left pointing at that character.
.*
.section The for Statement
.*
.pp
The statement,
.ix 'for statement'
.ix 'statement' 'for'
.cillust begin
.mono for
.mono (
expr1:MSEMI.
expr2:MSEMI.
expr3
.mono )
statement
.cillust end
.pc
is almost equivalent to,
.cillust begin
expr1:MSEMI.
.cbr
.mono while
.mono (
expr2
.mono )
.mono {
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.statement
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.expr3:MSEMI.
.cbr
.mono }
.cillust end
.pc
The
difference is that the
.ix 'continue statement'
.ix 'statement' 'continue'
.kw continue
statement will pass control to the statement
.us expr3
rather than to the end of the loop body.
.pp
.us expr1
is an initialization expression and may be omitted.
.pp
.us expr2
is the controlling expression, and specifies an evaluation to be
made before each iteration of the loop body. If the expression
evaluates to zero, the loop body is not executed, and
control is passed to the statement following the loop body.
If
.us expr2
is omitted,
then a non-zero (true) value is substituted in its place. In this
case, the statements in the loop must cause an explicit break
from the loop.
.pp
.us expr3
specifies an operation to be performed after each iteration.
A common operation would be the incrementing of a counter.
.us expr3
may be omitted.
.pp
The
.us statement
may be a compound statement.
.keep begin
..sk 1 c
For example,
.millust begin
char charvec[256];
int  count;

for( count = 0; count <= 255; count++ ) {
    charvec[count] = count;
}
.millust end
.keep end
.pc
This example will initialize the character array
.mono charvec
to the values from 0 to 255.
.pp
The following are examples of
.kw for
statements:
.cillust begin
.monoon
for( ;; )
.monooff
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.statement:MSEMI.
.cillust end
.pc
All statements in the body of the loop will be executed until a
.kw break
or
.kw goto
statement is executed which passes control outside of the loop, or
a
.kw return
statement is executed which exits the function.
This is sometimes called
.ix 'loop forever'
.us loop forever
..ct ..li .
.cillust begin
.monoon
for( i = 0; i <= 100; ++i )
.monooff
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.statement:MSEMI.
.cillust end
.pc
The object
.mono i
is given the initial value zero, and after each iteration of the loop
is incremented by one. The loop is executed 101 times, with
.mono i
having the successive values
.mono 0
..ct ,
.mono 1
..ct ,
.mono 2 ... 99
..ct ,
.mono 100
..ct ,
and having the value
.mono 101
after termination of the loop.
.cillust begin
.monoon
for( ; *bufptr != '\0'; ++bufptr )
.monooff
.cbr
&SYSRB.&SYSRB.&SYSRB.&SYSRB.statement:MSEMI.
.cillust end
.pc
The object
.mono bufptr
is already initialized, and the loop will continue until
.mono bufptr
points at a null character. After each iteration of the loop,
.mono bufptr
will be incremented to point at the next character.
.endlevel
.*
.section Jump Statements
.*
.pp
A jump statement causes execution to continue at
a specific place in a program, without executing any other intervening
statements.
There are four jump statements:
.kw goto
..ct ,
.kw continue
..ct ,
.kw break
and
.kw return
..ct ..li .
.beglevel
.*
.section The goto Statement
.ix 'goto statement'
.ix 'statement' 'goto'
.*
.cillust begin
.mono goto
identifier:MSEMI.
.cillust end
.pc
.us identifier
is a label somewhere in the current function (including any
block within the function).
The next statement executed will be the one following that label.
.pp
.bd Note:
it can be confusing
to use the
.kw goto
statement excessively. It is easy to create
.ix 'spaghetti code'
.us spaghetti code,
which is very difficult to understand, even by the person who wrote it.
It is recommended that the
.kw goto
statement be used, at most, to jump
.us out of
blocks, never into them.
.*
.section The continue Statement
.ix 'continue statement'
.ix 'statement' 'continue'
.*
.millust begin
continue;
.millust end
.pp
A
.kw continue
statement may only appear within a loop body, and causes a jump
to the inner-most loop's loop-continuation statement (the end of
the loop body).
.pp
In a
.ix 'continue statement' 'in a while'
.kw while
statement, the jump is effectively back to the
.kw while
..ct ..li .
.pp
In a
.ix 'continue statement' 'in a do'
.kw do
statement, the jump is effectively down to the
.kw while
..ct ..li .
.pp
In a
.ix 'continue statement' 'in a for'
.kw for
statement,
the jump is effectively to the closing brace of the compound-statement
that is the subject of the
.kw for
loop. The third expression in the
.kw for
statement, which is often an increment or decrement, is then
executed before control is returned to the top of the loop.
.*
.section The break Statement
.ix 'break statement'
.ix 'statement' 'break'
.*
.millust begin
break;
.millust end
.pp
A
.kw break
statement may only appear in an iteration (loop) body or a
.kw switch
statement.
.pp
In a loop, a
.kw break
will cause execution to continue at
the statement following the loop body.
.pp
In a
.kw switch
statement, a
.kw break
will cause execution to continue at
the statement following the
switch.
If the loop or
.kw switch
that contains the
.kw break
is enclosed inside another loop or
.kw switch
..ct ,
only the
inner-most loop or
.kw switch
is terminated.
The
.kw goto
statement may be used to terminate more than one loop or
.kw switch
..ct ..li .
.*
.section The return Statement
.ix 'return statement'
.ix 'statement' 'return'
.*
.cillust begin
.monoon
&SYSRB.&SYSRB.&SYSRB.&SYSRB.return:MSEMI.
.monooff
.cor
.monoon
&SYSRB.&SYSRB.&SYSRB.&SYSRB.return
.monooff
expression:MSEMI.
.cillust end
.pc
A popular variation of the second form is,
.cillust begin
.monoon
&SYSRB.&SYSRB.&SYSRB.&SYSRB.return(
:ITAL.expression:eITAL.
);
.monooff
.cillust end
.pc
The
.kw return
statement causes execution of the current function to be terminated,
and control is passed to the caller.
A function may contain
any number of
.kw return
statements.
.pp
If the function is declared with a return type of
.kw void
(no value is returned), then no
.kw return
statement within that function may return a value.
.pp
If the function is declared as having a return type of other than
.kw void
..ct ,
then any
.kw return
statement with an expression
will evaluate the expression and convert it to the return type.
That value will be the value returned by the function.
If a
.kw return
is executed without an expression, and the caller uses the value
returned by the function, the behavior is undefined since no value
was returned. An arbitrary value will probably be used.
.pp
Reaching the closing brace } that terminates the function is equivalent
to executing a
.kw return
statement without an expression.
.endlevel
