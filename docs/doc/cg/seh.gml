.chap *refid=seh Structured Exception Handling
.*
.np
.ix 'structured exception handling'
Microsoft-style Structured Exception Handling (SEH) is supported by
the &cmpcname compiler only.
MS SEH is supported under the Win32, Win32s and OS/2 platforms.
You should not confuse SEH with C++ exception handling.
The &cmppname compiler supports the standard C++ syntax for exception
handling.
.np
The following sections introduce some of the aspects of SEH.
For a good description of SEH, please refer to
.book Advanced Windows NT
by Jeffrey Richter (Microsoft Press, 1994).
You may also wish to read the article "Clearer, More Comprehensive
Error Processing with Win32 Structured Exception Handling" by Kevin
Goodman in the January, 1994 issue of Microsoft Systems Journal.
.*
.section Termination Handlers
.*
.np
.ix '_try'
.ix '_finally'
We begin our look at SEH with a simple model.
In this model, there are two blocks of code &mdash. the "guarded" block
and the "termination" block.
The termination code is guaranteed to be executed regardless of how
the "guarded" block of code is exited (including execution of any
"return" statement).
.millust begin
_try {
    /* guarded code */
    .
    .
    .
}
_finally {
    /* termination handler */
    .
    .
    .
}
.millust end
.np
The
.kw _finally
block of code is guaranteed to be executed no matter how the guarded
block is exited (
.kw break
.ct ,
.kw continue
.ct ,
.kw return
.ct ,
.kw goto
.ct , or
.kw longjmp
.ct ).
Exceptions to this are calls to
.kw abort
.ct ,
.kw exit
or
.kw _exit
which terminate the execution of the process.
.np
There can be no intervening code between
.us try
and
.us finally
blocks.
.np
The following is a contrived example of the use of
.kw  _try
and
.kw _finally
.ct .li .
.exam begin
#include <stdio.h>
#include <stdlib.h>
#include <excpt.h>

int docopy( char *in, char *out )
{
  FILE        *in_file = NULL;
  FILE        *out_file = NULL;
  char        buffer[256];
.exam break
  _try {
    in_file = fopen( in, "r" );
    if( in_file == NULL ) return( EXIT_FAILURE );
    out_file = fopen( out, "w" );
    if( out_file == NULL ) return( EXIT_FAILURE );

    while( fgets((char *)buffer, 255, in_file) != NULL ) {
      fputs( (char *)buffer, out_file );
    }
  }
  _finally {
    if( in_file != NULL ) {
      printf( "Closing input file\n" );
      fclose( in_file );
    }
    if( out_file != NULL ) {
      printf( "Closing output file\n" );
      fclose( out_file );
    }
    printf( "End of processing\n" );
  }
  return( EXIT_SUCCESS );
}
.exam break
void main( int argc, char **argv )
{
  if( argc < 3 ) {
    printf( "Usage: mv [in_filename] [out_filename]\n" );
    exit( EXIT_FAILURE );
  }
  exit( docopy( argv[1], argv[2] ) );
}
.exam end
.np
The
.us try
block ignores the messy details of what to do when either one of the
input or output files cannot be opened.
It simply tests whether a file can be opened and quits if it cannot.
The
.us finally
block ensures that the files are closed if they were opened,
releasing the resources associated with open files.
This simple example could have been written in C without the use of
SEH.
.np
There are two ways to enter the
.us finally
block.
One way is to exit the
.us try
block using a statement like
.kw return
.ct .li .
The other way is to fall through the end of the
.us try
block and into the
.us finally
block (the normal execution flow for this program).
Any code following the
.us finally
block is only executed in the second case.
You can think of the
.us finally
block as a special function that is invoked whenever an exit (other
than falling out the bottom) is attempted from a corresponding
.us try
block.
.np
More formally stated, a local unwind occurs when the system executes
the contents of a
.us finally
block because of the premature exit of code in a
.us try
block.
.remark
Kevin Goodman describes "unwinds" in his article.
"There are two types of unwinds: global and local. A global unwind
occurs when there are nested functions and an exception takes place. A
local unwind takes place when there are multiple handlers within one
function. Unwinding means that the stack is going to be clean by the
time your handler's code gets executed."
.eremark
.np
The
.us try/finally
structure is a rejection mechanism which is useful when a set of
statements is to be conditionally chosen for execution, but not all
of the conditions required to make the selection are available
beforehand.
It is an extension to the C language.
You start out with the assumption that a certain task can be
accomplished.
You then introduce statements into the code that test your hypothesis.
The
.us try
block consists of the code that you assume, under normal conditions,
will succeed.
Statements like
.us if ... return
can be used as tests.
Execution begins with the statements in the
.us try
block.
If a condition is detected which indicates that the assumption
of a normal state of affairs is wrong, a
.kw return
statement may be executed to cause control to be passed to the
statements in the
.us finally
block.
If the
.us try
block completes execution without executing a
.kw return
statement (i.e., all statements are executed up to the final brace),
then control is passed to the first statement following the
.us try
block (i.e., the first statement in the
.us finally
block).
.np
In the following example, two sets of codes and letters are read in
and some simple sequence checking is performed.
If a sequence error is detected, an error message is printed and
processing terminates; otherwise the numbers are processed and another
pair of numbers is read.
.exam begin
#include <stdio.h>
#include <stdlib.h>
#include <excpt.h>

void main( int argc, char **argv )
{
  read_file( fopen( argv[1], "r" ) );
}
.exam break
void read_file( FILE *input )
{
  int         line = 0;
  char        buffer[256];
  char        icode;
  char        x, y;

  if( input == NULL ) {
    printf( "Unable to open file\n" );
    return;
  }
.exam break
  _try {
    for(;;) {
      line++;
      if( fgets( buffer, 255, input ) == NULL ) break;
      icode = buffer[0];
      if( icode != '1' ) return;
      x = buffer[1];
      line++;
      if( fgets( buffer, 255, input ) == NULL ) return;
      icode = buffer[0];
      if( icode != '2' ) return;
      y = buffer[1];
      process( x, y );
    }
    printf( "Processing complete\n" );
    fclose( input );
    input = NULL;
  }
.exam break
  _finally {
    if( input != NULL ) {
      printf( "Invalid sequence: line = %d\n", line );
      fclose( input );
    }
  }
}
.exam break
void process( char x, char y )
{
    printf( "processing pair %c,%c\n", x, y );
}
.exam end
.np
The above example attempts to read a code and letter.
If an end of file occurs then the loop is terminated by the
.kw break
statement.
.np
If the code is not 1 then we did not get what we expected and an
error condition has arisen.
Control is passed to the first statement in the
.us finally
block by the
.kw return
statement.
An error message is printed and the open file is closed.
.np
If the code is 1 then a second code and number are read.
If an end of file occurs then we are missing a complete set of data
and an error condition has arisen.
Control is passed to the first statement in the
.us finally
block by the
.kw return
statement.
An error message is printed and the open file is closed.
.np
Similarly if the expected code is not 2 an error condition has arisen.
The same error handling procedure occurs.
.np
If the second code is 2, the values of variables
.id x
and
.id y
are processed (printed).
The
.kw for
loop is repeated again.
.np
The above example illustrates the point that all the information
required to test an assumption (that the file contains valid pairs of
data) is not available from the start.
We write our code with the assumption that the data values are correct
(our hypothesis) and then test the assumption at various points in the
algorithm.
If any of the tests fail, we reject the hypothesis.
.np
Consider the following example.
What values are printed by the program?
.exam begin
#include <stdio.h>
#include <stdlib.h>
#include <excpt.h>

void main( int argc, char **argv )
{
  int ctr = 0;
.exam break
  while( ctr < 10 ) {
    printf( "%d\n", ctr );
    _try {
      if( ctr == 2 ) continue;
      if( ctr == 3 ) break;
    }
    _finally {
      ctr++;
    }
.exam break
    ctr++;
  }
  printf( "%d\n", ctr );
}
.exam end
.np
At the top of the loop,
the value of
.id ctr
is 0.
The next time we reach the top of the loop,
the value of
.id ctr
is 2 (having been incremented twice, once by the
.us finally
block and once at the bottom of the loop).
When
.id ctr
has the value 2, the
.kw continue
statement will cause the
.us finally
block to be executed (resulting in
.id ctr
being incremented to 3),
after which execution continues at the top of the
.kw while
loop.
When
.id ctr
has the value 3, the
.kw break
statement will cause the
.us finally
block to be executed (resulting in
.id ctr
being incremented to 4),
after which execution continues after the
.kw while
loop.
Thus the output is:
.millust begin
0
2
3
4
.millust end
.np
The point of this exercise was that after the
.us finally
block is executed, the normal flow of execution is resumed
at the
.kw break
.ct ,
.kw continue
.ct ,
.kw return
.ct , etc. statement and the normal behaviour for that statement occurs.
It is as if the compiler had inserted a function call just before
the statement that exits the
.us try
block.
.millust begin
_try {
  if( ctr == 2 ) invoke_finally_block() continue;
  if( ctr == 3 ) invoke_finally_block() break;
}
.millust end
.np
There is some overhead associated with local unwinds such as that
incurred by the use of
.kw break
.ct ,
.kw continue
.ct ,
.kw return
.ct , etc.
To avoid this overhead, a new transfer keyword called
.kw _leave
can be used.
The use of this keyword causes a jump to the end of the
.us try block.
Consider the following modified version of an earlier example.
.exam begin
#include <stdio.h>
#include <stdlib.h>
#include <excpt.h>

void main( int argc, char **argv )
{
  read_file( fopen( argv[1], "r" ) );
}
.exam break
void read_file( FILE *input )
{
  int         line = 0;
  char        buffer[256];
  char        icode;
  char        x, y;

  if( input == NULL ) {
    printf( "Unable to open file\n" );
    return;
  }
.exam break
  _try {
    for(;;) {
      line++;
      if( fgets( buffer, 255, input ) == NULL ) break;
      icode = buffer[0];
      if( icode != '1' ) _leave;
      x = buffer[1];
      line++;
      if( fgets( buffer, 255, input ) == NULL ) _leave;
      icode = buffer[0];
      if( icode != '2' ) _leave;
      y = buffer[1];
      process( x, y );
    }
    printf( "Processing complete\n" );
    fclose( input );
    input = NULL;
  }
.exam break
  _finally {
    if( input != NULL ) {
      printf( "Invalid sequence: line = %d\n", line );
      fclose( input );
    }
  }
}
.exam break
void process( char x, char y )
{
  printf( "processing pair %c,%c\n", x, y );
}
.exam end
.np
There are two ways to enter the
.us finally
block.
One way is caused by unwinds &mdash. either local (by the use of
.kw break
.ct ,
.kw continue
.ct ,
.kw return
.ct , or
.kw goto
.ct )
or global (more on global unwinds later).
The other way is through the normal flow of execution (i.e., simply by
falling through the bottom of the
.us try
block).
There is a function called
.kw AbnormalTermination
that can be used to determine which of these two methods was used
to enter the
.us finally
block.
If the function returns
.id TRUE
(1)
then the
.us finally
block was entered using the first method;
if the function returns
.id FALSE
(0)
then the
.us finally
block was entered using the second method.
This information may be useful in some circumstances.
For example, you may wish to avoid executing any code in a
.us finally
block if the block was entered through the normal flow of execution.
.exam begin
#include <stdio.h>
#include <stdlib.h>
#include <excpt.h>

void main( int argc, char **argv )
{
  read_file( fopen( argv[1], "r" ) );
}
.exam break
void read_file( FILE *input )
{
  int         line = 0;
  char        buffer[256];
  char        icode;
  char        x, y;

  if( input == NULL ) {
    printf( "Unable to open file\n" );
    return;
  }
.exam break
  _try {
    for(;;) {
      line++;
      if( fgets( buffer, 255, input ) == NULL ) break;
      icode = buffer[0];
      if( icode != '1' ) return;
      x = buffer[1];
      line++;
      if( fgets( buffer, 255, input ) == NULL ) return;
      icode = buffer[0];
      if( icode != '2' ) return;
      y = buffer[1];
      process( x, y );
    }
    printf( "Processing complete\n" );
  }
.exam break
  _finally {
    if( AbnormalTermination() )
        printf( "Invalid sequence: line = %d\n", line );
    fclose( input );
  }
}
.exam break
void process( char x, char y )
{
  printf( "processing pair %c,%c\n", x, y );
}
.exam end
.np
In the above example, we reverted back to the use of the
.kw return
statement since the execution of a
.kw _leave
statement is considered part of the normal flow of execution
and is not considered an "abnormal termination" of the
.us try
block.
Note that since it is not possible to determine whether the
.us finally
block is executing as the result of a local or global unwind,
it may not be appropriate to use the
.kw AbnormalTermination
function as a way to determine what has gone on.
However, in our simple example, we expect that nothing could go wrong
in the "processing" routine.
.*
.section Exception Filters and Exception Handlers
.*
.np
We would all like to create flawless software but situations arise for
which we did not plan.
An event that we did not expect which causes the software to cease to
function properly is called an exception.
The computer can generate a hardware exception when the software
attempts to execute an illegal instruction.
We can force this quite easily in C by dereferencing a NULL pointer as
shown in the following sample fragment of code.
.exam begin
    char *nullp = NULL;

    *nullp = '\1';
.exam end
.np
We can also generate software exceptions from software by calling a
special function for this purpose.
We will look at software exceptions in more detail later on.
.np
Given that exceptions are generally very difficult to avoid in large
software projects, we can acknowledge that they are a fact of life and
prepare for them.
A mechanism similar to
.us try/finally
has been devised that makes it possible to gain control when an
exception occurs and to execute procedures to handle the situation.
.np
The exception handling mechanism involves the pairing up
of a
.kw _try
block with an
.kw _except
block.
This is illustrated in the following example.
.exam begin
#include <stdio.h>
#include <stdlib.h>
#include <excpt.h>

void main( int argc, char **argv )
{
  char *nullp = NULL;
.exam break
  printf( "Attempting illegal memory reference.\n" );
  _try {
    *nullp = '\1';
  }
  _except (EXCEPTION_EXECUTE_HANDLER) {
    printf( "Oh no! We had an exception!\n" );
  }
  printf( "We recovered fine...\n" );
}
.exam end
.np
In this example, any exception that occurs while executing "inside"
the
.us try
block will cause the
.us except
block to execute.
Unlike the
.us finally
block, execution of the
.us except
block occurs only when an exception is generated and only when the
expression after the
.kw _except
keyword evaluates to
.ix 'EXCEPTION_EXECUTE_HANDLER'
.id EXCEPTION_EXECUTE_HANDLER.
The expression can be quite complex and can involve the execution of a
function that returns one of the permissible values.
The expression is called the exception "filter" since it determines
whether or not the exception is to be handled by the
.us except
block.
The permissible result values for the exception filer are:
.begnote $break
.note EXCEPTION_EXECUTE_HANDLER
.ix 'EXCEPTION_EXECUTE_HANDLER'
meaning "I will handle the exception".
.note EXCEPTION_CONTINUE_EXECUTION
.ix 'EXCEPTION_CONTINUE_EXECUTION'
meaning "I want to resume execution at the point where the exception
was generated".
.note EXCEPTION_CONTINUE_SEARCH
.ix 'EXCEPTION_CONTINUE_SEARCH'
meaning "I do not want to handle the exception so continue looking
down the
.us try/except
chain until you find an exception handler that does want to handle the
exception".
.endnote
.*
.section Resuming Execution After an Exception
.*
.np
Why would you want to resume execution of the instruction that caused
the exception?
Since the exception filter can involve a function call, that function
can attempt to correct the problem.
For example, if it is determined that the exception has occurred
because of the NULL pointer dereference, the function could modify the
pointer so that it is no longer NULL.
.exam begin
#include <stdio.h>
#include <stdlib.h>
#include <excpt.h>

char *NullP = NULL;
.exam break
int filter( void )
{
  if( NullP == NULL ) {
    NullP = malloc( 20 );
    return( EXCEPTION_CONTINUE_EXECUTION )
  }
  return( EXCEPTION_EXECUTE_HANDLER )
}
.exam break
void main( int argc, char **argv )
{

  printf( "Attempting illegal memory reference.\n" );
  _try {
    *NullP = '\1';
  }
.exam break
  _except (filter()) {
    printf( "Oh no! We had an exception!\n" );
  }
  printf( "We recovered fine...\n" );
}
.exam end
.np
Unfortunately, this is does not solve the problem.
Understanding why it does not involves looking at the sequence of
computer instructions that is generated for the expression in
question.
.millust begin
    *NullP = '\1';
        mov     eax,dword ptr _NullP
        mov     byte ptr [eax],01H
.millust end
.pc
The exception is caused by the second instruction which contains a
pointer to the referenced memory location (i.e., 0) in register EAX.
.ix 'EXCEPTION_CONTINUE_EXECUTION'
This is the instruction that will be repeated when the filter returns
.id EXCEPTION_CONTINUE_EXECUTION.
Since EAX did not get changed by our fix, the exception will reoccur.
Fortunately,
.id NullP
is changed and this prevents our program from looping forever.
The moral here is that there are very few instances where you can
correct "on the fly" a problem that is causing an exception to occur.
Certainly, any attempt to do so must involve a careful inspection of
the computer instruction sequence that is generated by the compiler
(and this sequence usually varies with the selection of compiler
optimization options).
The best solution is to add some more code to detect the problem
before the exception occurs.
.*
.section Mixing and Matching _try/_finally and _try/_except
.*
.np
Where things really get interesting is in the interaction between
.us try/finally
blocks and
.us try/except
blocks.
These blocks can be nested within each other.
In an earlier part of the discussion, we talked about global unwinds
and how they can be caused by exceptions being generated in nested
function calls.
All of this should become clear after studying the following example.
.tinyexam begin
#include <stdio.h>
#include <stdlib.h>
#include <excpt.h>

void func_level4( void )
{
  char *nullp = NULL;

  printf( "Attempting illegal memory reference\n" );
  _try {
    *nullp = '\1';
  }
  _finally {
    if( AbnormalTermination() )
      printf( "Unwind in func_level4\n" );
  }
  printf( "Normal return from func_level4\n" );
}
.tinyexam break
void func_level3( void )
{
  _try {
    func_level4();
  }
  _finally {
    if( AbnormalTermination() )
      printf( "Unwind in func_level3\n" );
  }
  printf( "Normal return from func_level3\n" );
}
.tinyexam break
void func_level2( void )
{
  _try {
    _try {
      func_level3();
    }
    _except (EXCEPTION_CONTINUE_SEARCH) {
      printf( "Exception never handled in func_level2\n" );
    }
  }
  _finally {
    if( AbnormalTermination() )
      printf( "Unwind in func_level2\n" );
  }
  printf( "Normal return from func_level2\n" );
}
.tinyexam break
void func_level1( void )
{
  _try {
    func_level2();
  }
  _finally {
    if( AbnormalTermination() )
      printf( "Unwind in func_level1\n" );
  }
  printf( "Normal return from func_level1\n" );
}
.tinyexam break
void func_level0( void )
{
  _try {
    _try {
      func_level1();
    }
    _except (EXCEPTION_EXECUTE_HANDLER) {
      printf( "Exception handled in func_level0\n" );
    }
  }
  _finally {
    if( AbnormalTermination() )
      printf( "Unwind in func_level0\n" );
  }
  printf( "Normal return from func_level0\n" );
}
.tinyexam break
void main( int argc, char **argv )
{
  _try {
    _try {
      func_level0();
    }
    _except (EXCEPTION_EXECUTE_HANDLER) {
      printf( "Exception handled in main\n" );
    }
  }
  _finally {
    if( AbnormalTermination() )
      printf( "Unwind in main\n" );
  }
  printf( "Normal return from main\n" );
}
.tinyexam end
.np
In this example,
.autonote
.note
.id main
calls
.id func_level0
.note
.id func_level0
calls
.id func_level1
.note
.id func_level1
calls
.id func_level2
.note
.id func_level2
calls
.id func_level3
.note
.id func_level3
calls
.id func_level4
.endnote
.np
It is in
.id func_level4
where the exception occurs.
The run-time system traps the exception and performs a search
of the active
.us try
blocks looking for one that is paired up with an
.us except
block.
.np
.ix 'EXCEPTION_EXECUTE_HANDLER'
When it finds one, the filter is executed and, if the result is
.id EXCEPTION_EXECUTE_HANDLER,
then the
.us except
block is executed after performing a global unwind.
.np
.ix 'EXCEPTION_CONTINUE_EXECUTION'
If the result is
.id EXCEPTION_CONTINUE_EXECUTION,
the run-time system resumes execution at the instruction that
caused the exception.
.np
.ix 'EXCEPTION_CONTINUE_SEARCH'
If the result is
.id EXCEPTION_CONTINUE_SEARCH,
the run-time system continues its search for an
.us except
block with a filter that returns one of the other possible values.
If it does not find any exception handler that is prepared to handle
the exception, the application will be terminated with the appropriate
exception notification.
.np
Let us look at the result of executing the example program.
The following messages are printed.
.millust begin
Attempting illegal memory reference
Unwind in func_level4
Unwind in func_level3
Unwind in func_level2
Unwind in func_level1
Exception handled in func_level0
Normal return from func_level0
Normal return from main
.millust end
.np
The run-time system searched down the
.us try/except
chain until it got to
.id func_level0
which had an
.us except
filter that evaluated to
.id EXCEPTION_EXECUTE_HANDLER.
It then performed a global unwind in which the
.us try/finally
blocks of
.id func_level4,
.id func_level3,
.id func_level2,
and
.id func_level1
were executed.
After this, the exception handler in
.id func_level0
did its thing and execution resumed in
.id func_level0
which returned back to
.id main
which returned to the run-time system for normal program termination.
Note the use of the built-in
.kw AbnormalTermination
function in the
.us finally
blocks of each function.
.np
This sequence of events permits each function to do any cleaning up
that it deems necessary before it is wiped off the execution stack.
.*
.section Refining Exception Handling
.*
.np
The decision to handle an exception must be weighed carefully.
It is not necessarily a desirable thing for an exception handler to
handle all exceptions.
.ix 'EXCEPTION_EXECUTE_HANDLER'
In the previous example, the expression in the exception filter in
.id func_level0
always evaluates to
.id EXCEPTION_EXECUTE_HANDLER
which means it will snag every exception that comes its way.
There may be other exception handlers further on down the chain that
are better equipped to handle certain types of exceptions.
.ix 'GetExceptionCode'
There is a way to determine the exact type of exception using the
built-in
.id GetExceptionCode()
function.
It may be called only from within the exception handler filter
expression or within the exception handler block.
Here is a description of the possible return values from the
.id GetExceptionCode()
function.
.begnote $break $setptnt 15
.notehd1 Value
.notehd2 Meaning
.note EXCEPTION_ACCESS_VIOLATION
.ix 'EXCEPTION_ACCESS_VIOLATION'
The thread tried to read from or write to a virtual address for which
it does not have the appropriate access.
.note EXCEPTION_BREAKPOINT
.ix 'EXCEPTION_BREAKPOINT'
A breakpoint was encountered.
.note EXCEPTION_DATATYPE_MISALIGNMENT
The thread tried to read or write data that is misaligned on hardware
that does not provide alignment. For example, 16-bit values must be
aligned on 2-byte boundaries; 32-bit values on 4-byte boundaries, and
so on.
.note EXCEPTION_SINGLE_STEP
.ix 'EXCEPTION_SINGLE_STEP'
A trace trap or other single-instruction mechanism signaled that one
instruction has been executed.
.note EXCEPTION_ARRAY_BOUNDS_EXCEEDED
The thread tried to access an array element that is out of bounds and
the underlying hardware supports bounds checking.
.note EXCEPTION_FLT_DENORMAL_OPERAND
.ix 'EXCEPTION_FLT_DENORMAL_OPERAND'
One of the operands in a floating-point operation is denormal. A
denormal value is one that is too small to represent as a standard
floating-point value.
.note EXCEPTION_FLT_DIVIDE_BY_ZERO
.ix 'EXCEPTION_FLT_DIVIDE_BY_ZERO'
The thread tried to divide a floating-point value by a floating-point
divisor of zero.
.note EXCEPTION_FLT_INEXACT_RESULT
.ix 'EXCEPTION_FLT_INEXACT_RESULT'
The result of a floating-point operation cannot be represented exactly
as a decimal fraction.
.note EXCEPTION_FLT_INVALID_OPERATION
.ix 'EXCEPTION_FLT_INVALID_OPERATION'
This exception represents any floating-point exception not included in
this list.
.note EXCEPTION_FLT_OVERFLOW
.ix 'EXCEPTION_FLT_OVERFLOW'
The exponent of a floating-point operation is greater than the
magnitude allowed by the corresponding type.
.note EXCEPTION_FLT_STACK_CHECK
.ix 'EXCEPTION_FLT_STACK_CHECK'
The stack overflowed or underflowed as the result of a floating-point
operation.
.note EXCEPTION_FLT_UNDERFLOW
.ix 'EXCEPTION_FLT_UNDERFLOW'
The exponent of a floating-point operation is less than the magnitude
allowed by the corresponding type.
.note EXCEPTION_INT_DIVIDE_BY_ZERO
The thread tried to divide an integer value by an integer divisor of
zero.
.note EXCEPTION_INT_OVERFLOW
.ix 'EXCEPTION_INT_OVERFLOW'
The result of an integer operation caused a carry out of the most
significant bit of the result.
.note EXCEPTION_PRIV_INSTRUCTION
.ix 'EXCEPTION_PRIV_INSTRUCTION'
The thread tried to execute an instruction whose operation is not
allowed in the current machine mode.
.note EXCEPTION_NONCONTINUABLE_EXCEPTION
.ix 'EXCEPTION_NONCONTINUABLE_EXCEPTION'
The thread tried to continue execution after a non-continuable exception
occurred.
.endnote
.np
These constants are defined by including
.fi WINDOWS.H
in the source code.
.np
The following example is a refinement of the
.id func_level1()
function in our previous example.
.tinyexam begin
#include <windows.h>

void func_level0( void )
{
  _try {
    _try {
      func_level1();
    }
    _except (
        (GetExceptionCode() == EXCEPTION_ACCESS_VIOLATION)
        ? EXCEPTION_EXECUTE_HANDLER
        : EXCEPTION_CONTINUE_SEARCH
        ) {
      printf( "Exception handled in func_level0\n" );
    }
  }
  _finally {
    if( AbnormalTermination() )
      printf( "Unwind in func_level0\n" );
  }
  printf( "Normal return from func_level0\n" );
}
.tinyexam end
.np
.ix 'access violation'
In this version, only an "access violation" will be handled by the
exception handler in the
.id func_level0()
function.
All other types of exceptions will be passed on to
.id main
(which can also be modified to be somewhat more selective about the
types of exceptions it should handle).
.np
.ix 'GetExceptionInformation'
More information on the exception that has occurred can be obtained by
the use of the
.id GetExceptionInformation()
function.
The use of this function is also restricted.
It can be called only from within the filter expression of an
exception handler.
However, the return value of
.id GetExceptionInformation()
can be passed as a parameter to a filter function.
This is illustrated in the following example.
.tinyexam begin
int GetCode( LPEXCEPTION_POINTERS exceptptrs )
{
    return (exceptptrs->ExceptionRecord->ExceptionCode );
}

void func_level0( void )
{
  _try {
    _try {
      func_level1();
    }
    _except (
        (GetCode( GetExceptionInformation() )
            == EXCEPTION_ACCESS_VIOLATION)
        ? EXCEPTION_EXECUTE_HANDLER
        : EXCEPTION_CONTINUE_SEARCH
        ) {
      printf( "Exception handled in func_level0\n" );
    }
  }
  _finally {
    if( AbnormalTermination() )
      printf( "Unwind in func_level0\n" );
  }
  printf( "Normal return from func_level0\n" );
}
.tinyexam end
.np
.ix 'GetExceptionInformation'
The return value of
.id GetExceptionInformation()
is a pointer to an
.ix 'EXCEPTION_POINTERS'
.id EXCEPTION_POINTERS
structure that contains pointers to two other structures: an
.ix 'EXCEPTION_RECORD'
.id EXCEPTION_RECORD
structure containing a description of the exception, and a
.ix 'CONTEXT'
.id CONTEXT
structure containing the machine-state information.
The filter function can make a copy of the structures if a more
permanent copy is desired.
Check your Win32 SDK documentation for more information on these
structures.
.*
.section Throwing Your Own Exceptions
.*
.np
You can use the same exception handling mechanisms to deal with
software exceptions raised by your application.
.ix 'RaiseException'
The
.id RaiseException()
function can be used to throw your own application-defined exceptions.
The first argument to this function is the exception code.
It would be wise to define your exception codes so that they do not
collide with system defined ones.
The following example shows how to throw an exception.
.exam begin
#define MY_EXCEPTION ( (DWORD) 123L )

    RaiseException( MY_EXCEPTION,
                    EXCEPTION_NONCONTINUABLE,
                    0, NULL );
.exam end
.np
In this example, the
.id GetExceptionCode()
function, when used in an exception handler filter expression or in
the body of an exception handler, would return the value 123.
.np
.ix 'RaiseException'
See the Win32 SDK documentation for more information on the arguments
to the
.id RaiseException()
function.
