.begnote
:DTHD.Subprogram Arguments
:DDHD.~b
.note AR-01
.bi invalid number of arguments to intrinsic function %s1
.pc
The number of actual arguments specified in the argument list for the
intrinsic function %s1 does not agree with the dummy argument list.
Consult the Language Reference for information on intrinsic functions and
their arguments.
.note AR-02
.bi dummy argument %s1 appears more than once
.pc
The same dummy argument %s1 is named more than once in the dummy
argument list.
.note AR-03
.bi argument number %d1 in call to %s2 must be CHARACTER*%d3 or bigger but size passed was %d4
.pc
Argument number %d1 in the call to subprogram %s2 was declared in the calling
subprogram to be CHARACTER*%d4.
In subprogram %s2, the argument was declared to be CHARACTER*%d3.
The size of the argument, in the calling program, must be greater
than or equal to the size in the subroutine/function.
.note AR-04
.bi argument number %d1 in call to %s2 - actual argument was %s3 but dummy argument was %s4
.pc
Argument number %d1 in the call to subprogram %s2 was a %s3 in the calling
subprogram but a %s4 in the called subprogram.
This message is issued when, for example, an function name is passed to
an array name.
.note AR-05
.bi argument number %d1 - %s2 was passed to an intrinsic function
.pc
Argument number %d1 to an intrinsic function was declared as a %s2 and cannot
be specified as an argument to an intrinsic function.
For example, it is illegal to pass an array to the SIN intrinsic function.
Constants, simple variables, array elements, and substrung array
elements may be used as arguments.
.note AR-06
.bi argument number %d1 in call to %s2 - a subroutine was passed but a function was expected
.pc
Argument number %d1 in the call to subprogram %s2 is a subroutine but
was used as a function in subprogram %s2.
.note AR-07
.bi number of arguments expected in call to %s1 was %d2, but found %d3
.pc
The number of arguments in the call to subprogram %s1 was %d3 and does not
match the definition of subprogram %s1 which was defined with %d2 arguments.
.note AR-08
.bi argument number %d1 in call to %s2 - a function was passed but a subroutine was expected
.pc
Argument number %d1 in the call to subprogram %s2 is a function but
was used as a subroutine in subprogram %s2.
.note AR-09
.bi argument number %d1 in call to %s2 - %s3 argument was passed but %s4 argument was expected
.pc
The type of argument number %d1 in the call to subprogram %s2 was %s3.
The type of argument number %d1 was declared as %s4 in subprogram %s2.
The number and types of actual and dummy arguments must match.
.note AR-10
.bi argument number %d1 in call to %s2 - type of structure passed does not match type of structure expected
.pc
The structure type of argument number %d1 in the call to subprogram
%s2 does not match the expected structure type.
Two structures are equivalent if the type of the corresponding field names
are the same.
Unions are considered equivalent if their sizes are the same.
Field names, and the structure name itself, do not have to be the same.
.note AR-11
.bi argument number %d1 in call to %s2 - argument passed by value but expected by reference
.pc
Argument number %d1 in the call to subprogram %s2 was passed by value.
The called subprogram expected the address of the argument instead of its
value.
.endnote
.begnote
:DTHD.Block Data Subprograms
:DDHD.~b
.note BD-01
.bi %s1 was initialized in a block data subprogram but is not in COMMON
.pc
The variable or array element, %s1, was initialized in a BLOCK DATA
subprogram but was not specified in a named COMMON block.
.note BD-02
.bi %s1 statement is not permitted in a BLOCK DATA subprogram
.pc
The statement, %s1, is not allowed in a BLOCK DATA subprogram.
The only statements which are allowed to appear are:
IMPLICIT, PARAMETER, DIMENSION, COMMON, SAVE, EQUIVALENCE, DATA, END,
and type statements.
.endnote
.begnote
:DTHD.Source Format and Contents
:DDHD.~b
.note CC-01
.bi invalid character encountered in source input
.pc
The indicated statement contains an invalid character.
Valid characters are: letters, digits, $, *, ., +, &minus., /, :, =, (, ),
!, %,  ', and ,(comma).
Any character may be used inside a character or hollerith string.
.note CC-02
.bi invalid character in statement number columns
.pc
A column in columns 1 to 5 of the indicated statement contains a
non-digit character.
Columns 1 to 5 contain the statement number label.
It is made up of digits from 0 to 9 and is greater than 0 and less
than or equal to 99999.
.note CC-03
.bi character in continuation column, but no statement to continue
.pc
The character in column 6 indicates that this line is a continuation
of the previous statement but there is no previous statement to
continue.
.note CC-04
.bi character encountered is not FORTRAN 77 standard
.pc
A non-standard character was encountered in the source input stream.
This is most likely caused by the use of lower case letters.
.note CC-05
.bi columns 1-5 in a continuation line must be blank
.pc
When column 6 is marked as a continuation statement to the previous
line, columns 1 to 5 must be left blank.
.note CC-06
.bi more than 19 continuation lines is not FORTRAN 77 standard
.pc
More than 19 continuation lines is an extension to the FORTRAN 77 language.
.note CC-07
.bi end-of-line comment is not FORTRAN 77 standard
.pc
End-of-line comments are an extension to the FORTRAN 77 language.
End-of-line comments start with the exclamation mark (!) character.
.note CC-08
.bi D in column 1 is not FORTRAN 77 standard
.pc
A "D" in column 1 signifies a debug statement that is compiled
when the "__debug__" macro symbol is defined.
If the "__debug__" macro symbol is not defined, the statement is ignored.
The "c$define" compiler directive or the "define" compiler option can be
used to define the "__debug__" macro symbol.
.note CC-09
.bi too many continuation lines
.pc
The limit on the number of continuation lines has been reached.
This limit depends on the size of each continuation line.
A minimum of 61 continuation lines is permitted.
If the "xline" option is used, a minimum of 31 continuation lines is
permitted.
.endnote
.begnote
:DTHD.COMMON Blocks
:DDHD.~b
.note CM-01
.bi %s1 already in COMMON
.pc
The variable or array name, %s1, has already been specified in this or
another COMMON block.
.note CM-02
.bi initializing %s1 in COMMON outside of block data subprogram is not FORTRAN 77 standard
.pc
The symbol %s1, in a named COMMON block, has been initialized outside of a
block data subprogram.
This is an extension to the FORTRAN 77 language.
.note CM-03
.bi character and non-character data in COMMON is not FORTRAN 77 standard
.pc
The FORTRAN 77 standard specifies that a COMMON block cannot contain
both numeric and character data.
Allowing COMMON blocks to contain both numeric and character data is an
extension to the FORTRAN 77 standard.
.note CM-04
.bi COMMON block %s1 has been defined with a different size
.pc
The COMMON block %s1 has been defined with a different size in another
subprogram.
A named COMMON block must define the same amount of storage units
where ever named.
.note CM-05
.bi named COMMON block %s1 appears in more than one BLOCK DATA subprogram
.pc
The named COMMON block, %s1, may not appear in more than one BLOCK
DATA subprogram.
.note CM-06
.bi blank COMMON block has been defined with a different size
.pc
The blank COMMON block has been defined with a different size in
another subprogram.
This is legal but a warning message is issued.
.endnote
.begnote
:DTHD.Constants
:DDHD.~b
.note CN-01
.bi DOUBLE PRECISION COMPLEX constants are not FORTRAN 77 standard
.pc
Double precision complex numbers are an extension to the FORTRAN 77
language.
The indicated number is a complex number and at least one of the
parts, real or imaginary, is a double precision constant.
Both real and imaginary parts will be double precision.
.note CN-02
.bi invalid floating-point constant %s1
.pc
The floating-point constant %s1 is invalid.
Refer to the chapter entitled "Names, Data Types and Constants"
in the Language Reference.
.note CN-03
.bi zero length character constants are not allowed
.pc
FORTRAN 77 does not allow character constants of length 0 (i.e., an
empty string).
.note CN-04
.bi invalid hexadecimal/octal constant
.pc
An invalid hexadecimal or octal constant was specified.
Hexadecimal constants can only contain digits or the letters 'a' through
'f' and 'A' through 'F'.
Octal constants can only contain the digits '0' through '7'.
.note CN-05
.bi hexadecimal/octal constant is not FORTRAN 77 standard
.pc
Hexadecimal and octal constants are extensions to the FORTRAN 77 standard.
.endnote
.begnote
:DTHD.Compiler Options
:DDHD.~b
.note CO-01
.bi %s1 is already being included
.pc
An attempt has been made to include a file that is currently being
included in the program.
.note CO-02
.bi '%s1' option cannot take a NO prefix
.pc
The compiler option %s1, cannot have the NO prefix specified.
The NO prefix is used to negate an option.
Certain options, including all options that require a value cannot have a
NO prefix.
.note CO-03
.bi expecting an equals sign following the %s1 option
.pc
The compiler option %s1, requires an equal sign to be between the
option keyword and its associated value.
.note CO-04
.bi the '%s1' option requires a number
.pc
The compiler option %s1 and an equal sign has been detected but the
required associated value is missing.
.note CO-05
.bi option '%s1' not recognized - ignored
.pc
The option %s1 is not a recognized compiler option and has been ignored.
Consult the User's Guide for a complete list of compiler options.
.note CO-06
.bi '%s1' option not allowed in source input stream
.pc
The option %s1 can only be specified on the command line.
Consult the User's Guide for a description of which options are allowed
in the source input stream.
.note CO-07
.bi nesting level exceeded for compiler directives
.pc
Use of the C$IFDEF or C$IFNDEF compiler directives has caused the maximum
nesting level to be exceeded.
The maximum nesting level is 16.
.note CO-08
.bi mismatching compiler directives
.pc
This error message is issued if, for example, a C$ENDIF directive is used and
no matching C$IFDEF or C$IFNDEF precedes it.
Incorrect nesting of C$IFDEF, C$IFNDEF, C$ELSE and C$ENDIF directives will
also cause this message to be issued.
.note CO-09
.bi DATA option not allowed
.pc
A source file has been included into the current program through the
use of the INCLUDE compiler option.
This included source file cannot contain the DATA compiler option.
.note CO-10
.bi maximum limit exceeded in the '%s1' option - option ignored
.pc
The user has specified a value on an option which exceeds the maximum allowed
value.
.note CO-11
.bi DATA option not allowed with OBJECT option
.pc
The DATA compiler option can not appear a file that is compiled with the
OBJECT option.
.endnote
.begnote
:DTHD.Compiler Errors
:DDHD.~b
.note CP-01
.bi program abnormally terminated
.pc
This message is issued during the execution of the program.
If you are running WATFOR-77, this message indicates that an internal error
has occurred in the compiler.
Please report this error and any other helpful information about the program
being compiled to WATCOM so that the problem can be fixed.
.pc
If you are running an application compiled by the WATCOM FORTRAN 77
optimizing compiler, this message may indicate a problem with the compiler
or a problem with your program.
Try compiling your application with the "debug" option.
This causes the generation of run-time checking code to validate, for
example, array subscripts and will help ensure that your program is not
in error.
.note CP-09
.bi dynamic memory not freed
.pc
This message indicates an internal compiler error.
Please report this error and any other helpful information about the program
being compiled to WATCOM so that the problem can be fixed.
.note CP-10
.bi freeing unowned dynamic memory
.pc
This message indicates an internal compiler error.
Please report this error and any other helpful information about the program
being compiled to WATCOM so that the problem can be fixed.
.endnote
.begnote
:DTHD.Character Variables
:DDHD.~b
.note CV-01
.bi CHARACTER variable %s1 with length (*) not allowed in this expression
.pc
The length of the result of evaluating the expression is
indeterminate.
One of the operands has an indeterminate length and the result is
being assigned to a temporary.
.note CV-02
.bi character variable %s1 with length (*) must be a subprogram argument
.pc
The character variable %s1 with a length specification (*) can only be
used to declare dummy arguments in the subprogram.
The length of a dummy argument assumes the length of the
corresponding actual argument.
.note CV-03
.bi left and right hand sides overlap in a character assignment statement
.pc
The expression on the right hand side defines a substring of a
character variable and tries to assign it to an overlapping part of
the same character variable.
.endnote
.begnote
:DTHD.Data Initialization
:DDHD.~b
.note DA-01
.bi implied DO variable %s1 must be an integer variable
.pc
The implied DO variable %s1 must be declared as a variable of type
INTEGER or must have an implicit INTEGER type.
.note DA-02
.bi repeat specification must be a positive integer
.pc
The repeat specification in the constant list of the DATA statement
must be an unsigned positive integer.
.note DA-03
.bi %s1 appears in an expression but is not an implied DO variable
.pc
The variable %s1 is used to express the array elements in the DATA
statement but the variable is not used as an implied DO variable.
.note DA-04
.bi %s1 in blank COMMON block cannot be initialized
.pc
A blank or unnamed COMMON block is a COMMON statement with the block
name omitted.
The entries in blank COMMON blocks cannot be initialized using DATA
statements.
.note DA-05
.bi data initialization with hexadecimal constant is not FORTRAN 77 standard
.pc
Data initialization with hexadecimal constants is an extension to the
FORTRAN 77 language.
.note DA-06
.bi cannot initialize %s1 %s2
.pc
Symbol %s2 was used as a %s1.
It is illegal for such a symbol to be initialized in a DATA statement.
The DATA statement can only be used to initialize variables, arrays,
array elements, and substrings.
.note DA-07
.bi data initialization in %s1 statement is not FORTRAN 77 standard
.pc
Data initialization in type specification statements is an extension
to the FORTRAN 77 language.
These include: CHARACTER, COMPLEX, DOUBLE PRECISION, INTEGER,
LOGICAL, and REAL.
.note DA-09
.bi not enough constants for list of variables
.pc
There are not enough constants specified to initialize all of the
names listed in the DATA statement.
.note DA-11
.bi too many constants for list of variables
.pc
There are too many constants specified to initialize the names listed
in the DATA statement.
.note DA-13
.bi cannot initialize %s1 variable with %s2 constant
.pc
The constant of type %s2 cannot be used to initialize a variable of
type %s1.
.note DA-14
.bi entity can only be initialized once during data initialization
.pc
An attempt has been made to initialize an entity more than once in
DATA statements.
.endnote
.begnote
:DTHD.Dimensioned Variables
:DDHD.~b
.note DM-01
.bi using %s1 incorrectly in dimension expression
.pc
The name used as a dimension declarator has been previously declared
as type %s1 and cannot be used as a dimension declarator. A dimension
declarator must be an integer expression.
.note DM-02
.bi allocatable array is not FORTRAN 77 standard
.pc
Allocatable arrays are extensions to the FORTRAN 77 standard.
.note DM-03
.bi array or array element (possibly substrung) associated with %s1 too small
.pc
The dummy argument, array %s1, is defined to be larger than the size
of the actual argument.
.endnote
.begnote
:DTHD.DO-loops
:DDHD.~b
.note DO-01
.bi statement number %d1 already defined in line %d2 - DO loop is backwards
.pc
The statement number to indicate the end of the DO control structure
has been used previously in the program unit and cannot be used to
terminate the DO loop.
The terminal statement named in the DO statement must follow the DO
statement.
.note DO-02
.bi %s1 statement not allowed at termination of DO range
.pc
A non-executable statement cannot be used as the terminal statement
of a DO loop.
These statements include: all declarative statements,
ADMIT, AT END, BLOCK DATA, CASE, DO, ELSE, ELSE IF, END, END AT END,
END BLOCK, END GUESS, END IF, END LOOP, END SELECT, END WHILE, ENTRY,
FORMAT, FUNCTION, assigned GO TO, unconditional GO TO, GUESS,
arithmetic and block IF, LOOP, OTHERWISE, PROGRAM, RETURN, SAVE,
SELECT, STOP, SUBROUTINE, UNTIL, and WHILE.
.note DO-03
.bi improper nesting of DO loop
.pc
A nested DO loop has not been properly terminated before the
termination of the outer DO loop.
.note DO-04
.bi ENDDO cannot terminate DO loop with statement label
.pc
The ENDDO statement can only terminate a DO loop in which no
statement label was specified in the defining DO statement.
.note DO-05
.bi this DO loop form is not FORTRAN 77 standard
.pc
As an extension to FORTRAN 77, the following forms of the DO loop are
also supported.
.autonote
.note
A DO loop with no statement label specified in the defining DO
statement.
.note
The DO WHILE form of the DO statement.
.endnote
.note DO-06
.bi expecting comma or DO variable
.pc
The item following the DO keyword and the terminal statement-label
(if present) must be either a comma or a DO variable.
A DO variable is an integer, real or double precision variable name.
The DO statement syntax is as follows:
.millust begin
DO <tsl> <,> DO-var = ex, ex <, ex>
.millust end
.note DO-07
.bi DO variable cannot be redefined while DO loop is active
.pc
The DO variable named in the DO statement cannot have its value
altered by a statement in the DO loop structure.
.note DO-08
.bi incrementation parameter for DO-loop cannot be zero
.pc
The third expression in the DO statement cannot be zero.
This expression indicates the increment to the DO variable each
iteration of the DO loop.
If the increment expression is not specified a value of 1 is assumed.
.endnote
.begnote
:DTHD.Equivalence and/or Common
:DDHD.~b
.note EC-01
.bi equivalencing %s1 has caused extension of COMMON block %s2 to the left
.pc
The name %s1 has been equivalenced to a name in the COMMON block %s2.
This relationship has caused the storage of the COMMON block to be
extended to the left.
FORTRAN 77 does not allow a COMMON block to be extended in this way.
.note EC-02
.bi %s1 and %s2 in COMMON are equivalenced to each other
.pc
The names %s1 and %s2 appear in different COMMON blocks and each
occupies its own piece of storage and therefore cannot be
equivalenced.
.endnote
.begnote
:DTHD.END Statement
:DDHD.~b
.note EN-01
.bi missing END statement
.pc
The END statement for a PROGRAM, SUBROUTINE, FUNCTION or BLOCK DATA subprogram
was not found before the next subprogram or the end of the source input
stream.
.endnote
.begnote
:DTHD.Equal Sign
:DDHD.~b
.note EQ-01
.bi target of assignment is illegal
.pc
The target of an assignment statement, an input/output status specifier
in an input/output statement, or an inquiry specifier in an INQUIRE
statement, is illegal.
The target in any of the above cases must be a variable name, array element,
or a substring name.
.note EQ-02
.bi cannot assign value to %s1
.pc
An attempt has been made to assign a value to a symbol with class %s1.
For example, an array name cannot be the target of an assignment
statement.
This error may also be issued when an illegal target is used for the
input/output status specifier in an input/output statement or an inquiry
specifier in an INQUIRE statement.
.note EQ-03
.bi illegal use of equal sign
.pc
An equal sign has been found in the statement but the statement is
not an assignment statement.
.note EQ-04
.bi multiple assignment is not FORTRAN 77 standard
.pc
More than one equal sign has been found in the assignment statement.
.note EQ-05
.bi expecting equals sign
.pc
The equal sign is missing or misplaced.
The PARAMETER statement uses an equal sign to equate a symbolic name
to the value of a constant expression.
The I/O statements use an equal sign to equate the appropriate values
to the various specifiers.
The DO statement uses an equal sign to assign the initial value to
the DO variable.
.endnote
.begnote
:DTHD.Equivalenced Variables
:DDHD.~b
.note EV-01
.bi %s1 has been equivalenced to 2 different relative positions
.pc
The storage unit referenced by %s1 has been equivalenced to two
different storage units starting in two different places.
One name cannot be associated to two different values at the same
time.
.note EV-02
.bi EQUIVALENCE list must contain at least 2 names
.pc
The list of names to make a storage unit equivalent to several names
must contain at least two names.
.note EV-03
.bi %s1 incorrectly subscripted in %s2 statement
.pc
The name %s1 has been incorrectly subscripted in a %s2 statement.
.note EV-04
.bi incorrect substring of %s1 in %s2 statement
.pc
An attempt has been made to incorrectly substring %s1 in a %s2
statement.
For example, if a CHARACTER variable was declared to be of length 4
then (2:5) would be an invalid substring expression.
.note EV-05
.bi equivalencing CHARACTER and non-CHARACTER data is not FORTRAN 77 standard
.pc
Equivalencing numeric and character data is an extension to the
FORTRAN 77 language.
.note EV-06
.bi attempt to substring %s1 in EQUIVALENCE statement but type is %s2
.pc
An attempt has been made to substring the symbolic name %s1 in an
EQUIVALENCE statement but the type of the name is %s2 and should be
of type CHARACTER.
.endnote
.begnote
:DTHD.Exponentiation
:DDHD.~b
.note EX-01
.bi zero**J where J <= 0 is not allowed
.pc
Zero cannot be raised to a power less than or equal to zero.
.note EX-02
.bi X**Y where X < 0.0, Y is not of type INTEGER, is not allowed
.pc
When X is less than zero, Y may only be of type INTEGER.
.note EX-03
.bi (0,0)**Y where Y is not real is not allowed
.pc
In complex exponentiation, when the base is zero, the exponent may only be a
real number or a complex number whose imaginary part is zero.
.endnote
.begnote
:DTHD.ENTRY Statement
:DDHD.~b
.note EY-01
.bi type of entry %s1 does not match type of function %s2
.pc
If the type of a function is CHARACTER or a user-defined STRUCTURE, then the
type of all entry names must match the type of the function name.
.note EY-02
.bi ENTRY statement not allowed within structured control blocks
.pc
FORTRAN 77 does not allow an ENTRY statement to appear between the
start and end of a control structure.
.note EY-03
.bi size of entry %s1 does not match size of function %s2
.pc
The name %s1 found in an ENTRY statement must be declared to be the
same size as that of the function name.
If the name of the function or the name of any entry point has a
length specification of (*), then all such entries must have a length
specification of (*) otherwise they must all have a length
specification of the same integer value.
.endnote
.begnote
:DTHD.Format
:DDHD.~b
.note FM-01
.bi missing delimiter in format string, comma assumed
.pc
The omission of a comma between the descriptors listed in a format
string is an extension to the FORTRAN 77 language.
Care should be taken when omitting the comma since the assumed
separation may not occur in the intended place.
.note FM-02
.bi missing or invalid constant
.pc
An unsigned integer constant was expected with the indicated edit
descriptor but was not correctly placed or was missing.
.note FM-03
.bi Ew.dDe format code is not FORTRAN 77 standard
.pc
The edit descriptor Ew.dDe is an extension to the FORTRAN 77
language.
.note FM-04
.bi missing decimal point
.pc
The indicated edit descriptor must have a decimal point and an
integer to indicate the number of decimal positions.
These edit descriptors include: F, E, D and G.
.note FM-05
.bi missing or invalid edit descriptor in format string
.pc
In the format string, two delimiters were found in succession with no
valid descriptor in between.
.note FM-06
.bi unrecognizable edit descriptor in format string
.pc
An edit descriptor has been found in the format string that is not a
valid code.
Valid codes are: apostrophe ('), I, F, E, D, G, L, A, Z, H, T, TL,
TR, X, /, :, S, SP, SS, P, BN, B, $, and \.
.note FM-07
.bi invalid repeat specification
.pc
The indicated repeatable edit descriptor is invalid.
The forms of repeatable edit descriptors are:
Iw, Iw.m, Fw.d, Ew.d, Ew.dEe, Dw.d, Gw.d, Gw.dEe, Lw, A, Aw, Ew.dDe,
and Zw where w and e are positive unsigned integer constants, and d
and m are unsigned integer constants.
.note FM-08
.bi $ or \ format code is not FORTRAN 77 standard
.pc
The non-repeatable edit descriptors $ and \ are extensions to the
FORTRAN 77 language.
.note FM-09
.bi invalid field modifier
.pc
The indicated edit descriptor for a field is incorrect.
Consult the Language Reference for the correct form of the edit descriptor.
.note FM-10
.bi expecting end of FORMAT statement but found more text
.pc
The right parenthesis was encountered in the FORMAT statement to
terminate the statement and more text was found on the line.
.note FM-11
.bi repeat specification not allowed for this format code
.pc
A repeat specification was found in front of a format code that is a
nonrepeatable edit descriptor.
These include: apostrophe, H, T, TL, TR, X, /, :, S, SP, SS, P, BN,
BZ, $,and \.
.note FM-12
.bi no statement number on FORMAT statement
.pc
The FORMAT statement must have a statement label.
This statement number is used by I/O statements to reference the
FORMAT statement.
.note FM-13
.bi no closing quote on apostrophe edit descriptor
.pc
The closing quote of an apostrophe edit descriptor was not found.
.note FM-14
.bi field count greater than 256 is invalid
.pc
The repeat specification of the indicated edit descriptor is greater
than the maximum allowed of 256.
.note FM-15
.bi invalid field width specification
.pc
The width specifier on the indicated edit descriptor is invalid.
.note FM-16
.bi Z format code is not FORTRAN 77 standard
.pc
The Z (hexadecimal format) repeatable edit descriptor is an extension
to the FORTRAN 77 language.
.note FM-17
.bi FORMAT statement exceeds allotted storage size
.pc
The maximum allowable size of a FORMAT statement has exceeded.
The statement must be split into two or more FORMAT statements.
.note FM-18
.bi format specification not allowed on input
.pc
A format specification, in the FORMAT statement, is not allowed to be
used as an input specification.
Valid specifications include: T, TL,TR, X, /, :, P, BN, BZ, I, F, E,
D, G, L, A, and Z.
.note FM-19
.bi FORMAT missing repeatable edit descriptor
.pc
An attempt has been made to read or write a piece of data without a
valid repeatable edit descriptor.
All data requires a repeatable edit descriptor in the format.
The forms of repeatable edit descriptors are:
Iw, Iw.m, Fw.d, Ew.d, Ew.dEe, Dw.d, Gw.d, Gw.dEe, Lw, A, Aw, Ew.dDe,
and Zw where w and e are positive unsigned integer constants, and d
and m are unsigned integer constants.
.note FM-20
.bi missing constant before X edit descriptor, 1 assumed
.pc
The omission of the constant before an X edit descriptor in a format
specification is an extension to the FORTRAN 77 language.
.endnote
.begnote
:DTHD.GOTO and ASSIGN Statements
:DDHD.~b
.note GO-01
.bi %s1 statement label may not appear in ASSIGN statement but did in line %d2
.pc
The statement label in the ASSIGN statement in line %d2 references a
%s1 statement.
The statement label in the ASSIGN statement must appear in the same
program unit and must be that of an executable statement or a FORMAT
statement.
.note GO-02
.bi ASSIGN of statement number %d1 in line %d2 not allowed
.pc
The statement label %d1 in the ASSIGN statement is used in the line
%d2 which references a non-executable statement.
A statement label must appear in the same program unit as the ASSIGN
statement and must be that of an executable statement or a FORMAT
statement.
.note GO-03
.bi expecting TO
.pc
The keyword TO is missing or misplaced in the ASSIGN statement.
.endnote
.begnote
:DTHD.Hollerith Constants
:DDHD.~b
.note HO-01
.bi hollerith constant is not FORTRAN 77 standard
.pc
Hollerith constants are an extension to the FORTRAN 77 language.
.note HO-02
.bi not enough characters for hollerith constant
.pc
The number of characters following the H or h is not equal to the
constant preceding the H or h.
A hollerith constant consists of a positive unsigned integer constant
n followed by the letter H or h followed by a string of exactly n
characters.
.endnote
.begnote
:DTHD.IF Statements
:DDHD.~b
.note IF-01
.bi ELSE block must be the last block in block IF
.pc
Another ELSE IF block has been found after the ELSE block.
The ELSE block must be the last block in an IF block.
The form of the block IF is as follows:
.millust begin
IF (logical expression) THEN [:block-label]
         {statement}
{ ELSE IF
         {statement} }
[ELSE
         {statement}  ]
ENDIF
.millust end
.note IF-02
.bi expecting THEN
.pc
The keyword THEN is missing or misplaced in the block IF statement.
The form of the block IF is as follows:
.millust begin
IF (logical expression) THEN [:block-label]
         {statement}
{ ELSE IF
         {statement} }
[ELSE
         {statement}  ]
ENDIF
.millust end
.endnote
.begnote
:DTHD.I/O Lists
:DDHD.~b
.note IL-01
.bi missing or invalid format/FMT specification
.pc
A valid format specification is required on all READ and WRITE
statements.
The format specification is specified by:
.millust begin
[FMT=] <format identifier>
.millust end
.pc
.sy <format identifier>
is one of the following: statement label, integer variable-name,
character array-name, character expression, or *.
.note IL-02
.bi the UNIT may not be an internal file for this statement
.pc
An internal file may only be referenced in a READ or WRITE statement.
An internal file may not be referenced in a BACKSPACE, CLOSE,
ENDFILE, INQUIRE, OPEN, or REWIND statement.
.note IL-03
.bi %s1 statement cannot have %s2 specification
.pc
The I/O statement %s1 may not have the control information %s2
specified.
.note IL-04
.bi variable must have a size of 4
.pc
The variable used as a specifier in an I/O statement must be of size
4 but another size was specified.
These include the EXIST, OPENED, RECL, IOSTAT, NEXTREC, and NUMBER.
The name used in the ASSIGN statement must also be of size 4 but a
different size was specified.
.note IL-05
.bi missing or unrecognizable control list item %s1
.pc
A control list item %s1 was encountered in an I/O statement and is not
a valid control list item for that statement, or a control list item
was expected and was not found.
.note IL-06
.bi attempt to specify control list item %s1 more than once
.pc
The control list item %s1 in the indicated I/O statement, has been
named more than once.
.note IL-07
.bi implied DO loop has no input/output list
.pc
The implied DO loop specified in the I/O statement does not
correspond with a variable or expression in the input/output list.
.note IL-08
.bi list-directed input/output with internal files is not FORTRAN 77 standard
.pc
List-directed input/output with internal files is an extension to the
FORTRAN 77 language.
.note IL-09
.bi FORTRAN 77 standard requires an asterisk for list-directed formatting
.pc
An optional asterisk for list-directed formatting is an extension to
the FORTRAN 77 language.
The standard FORTRAN 77 language specifies that an asterisk is required.
.note IL-10
.bi missing or improper unit identification
.pc
The control specifier, UNIT, in the I/O statement is either missing
or identifies an improper unit.
The unit specifier specifies an external unit or internal file.
The external unit identifier is a non-negative integer expression or
an asterisk.
The internal file identifier is character variable, character array,
character array element, or character substring.
.note IL-11
.bi missing unit identification or file specification
.pc
An identifier to specifically identify the required file is missing.
The UNIT specifier is used to identify the external unit or internal
file.
The FILE specifier in the INQUIRE and OPEN statements is used to
identify the file name.
.note IL-12
.bi asterisk unit identifier not allowed in %s1 statement
.pc
The BACKSPACE, CLOSE, ENDFILE, INQUIRE, OPEN, and REWIND statements
require the external unit identifier be an unsigned positive integer
from 0 to 999.
.note IL-13
.bi cannot have both UNIT and FILE specifier
.pc
There are two valid forms of the INQUIRE statement; INQUIRE by FILE
and INQUIRE by UNIT.
Both of these specifiers cannot be specified in the same statement.
.note IL-14
.bi internal files require sequential access
.pc
An attempt has been made to randomly access an internal file.
Internal files may only be accessed sequentially.
.note IL-15
.bi END specifier with REC specifier is not FORTRAN 77 standard
.pc
The FORTRAN 77 standard specifies that an end-of-file condition can only
occur with a file connected for sequential access or an internal file.
The REC specifier indicates that the file is connected for direct access.
This extension allows the programmer to detect an end-of-file condition when
reading the records sequentially from a file connected for direct access.
.note IL-16
.bi %s1 specifier in i/o list is not FORTRAN 77 standard
.pc
The specified i/o list item is provided as an extension to the
FORTRAN 77 language.
.note IL-17
.bi i/o list is not allowed with NAMELIST-directed format
.pc
An i/o list is not allowed when the format specification is a NAMELIST.
.note IL-18
.bi non-character array as format specifier is not FORTRAN 77 standard
.pc
A format specifier must be of type character unless it is an array name.
Allowing a non-character array name is an extension to the FORTRAN 77 standard.
.endnote
.begnote
:DTHD.IMPLICIT Statements
:DDHD.~b
.note IM-01
.bi illegal range of characters
.pc
In the IMPLICIT statement, the first letter in the range of
characters must be smaller in the collating sequence than
the second letter in the range.
.note IM-02
.bi letter can only be implicitly declared once
.pc
The indicated letter has been named more than once in this or a
previous IMPLICIT statement.
A letter may only be named once.
.note IM-03
.bi unrecognizable type
.pc
The type declared in the IMPLICIT statement is not one of INTEGER,
REAL, DOUBLE PRECISION, COMPLEX, LOGICAL or CHARACTER.
.note IM-04
.bi (*) length specifier in an IMPLICIT statement is not FORTRAN 77 standard
.pc
A character length specified of (*) in an IMPLICIT statement is an
extension to the FORTRAN 77 language.
.note IM-05
.bi IMPLICIT NONE allowed once or not allowed with other IMPLICIT statements
.pc
The IMPLICIT NONE statement must be the only IMPLICIT statement in
the program unit in which it appears.
Only one IMPLICIT NONE statement is allowed in a program unit.
.endnote
.begnote
:DTHD.Input/Output
:DDHD.~b
.note IO-01
.bi BACKSPACE statement requires sequential access mode
.pc
The file connected to the unit specified in the BACKSPACE statement
has not been opened for sequential access.
.note IO-02
.bi input/output is already active
.pc
An attempt has been made to read or write a record when there is an
already active read or write in progress.
The execution of a READ or WRITE statement has caused transfer to a
function that contains a READ or WRITE statement.
.note IO-03
.bi ENDFILE statement requires sequential access mode
.pc
The specified external unit identifier must be connected for
sequential access but was connected for direct access.
.note IO-04
.bi formatted connection requires formatted input/output statements
.pc
The FORM specifier in the OPEN statement specifies FORMATTED and the
subsequent READ and/or WRITE statement does not use formatted I/O.
If the FORM specifier has been omitted and access is SEQUENTIAL then
FORMATTED is assumed.
If the access is DIRECT then UNFORMATTED is assumed.
.note IO-05
.bi unformatted connection requires unformatted input/output statements
.pc
The FORM specifier in the OPEN statement specifies UNFORMATTED and
the subsequent READ and/or WRITE statement uses formatted I/O.
If the FORM specifier has been omitted and access is SEQUENTIAL then
FORMATTED is assumed.
If the access is DIRECT then UNFORMATTED is assumed.
.note IO-06
.bi REWIND statement requires sequential access
.pc
The external unit identifier is not connected to a sequential file.
The REWIND statement positions to the first record in the file.
.note IO-07
.bi bad character in input field
.pc
The data received from the record in a file does not match the type
of the input list item.
.note IO-08
.bi BLANK specifier requires FORM specifier to be 'FORMATTED'
.pc
In the OPEN statement, the BLANK specifier may only be used when the
FORM specifier has the value of FORMATTED.
The BLANK specifier indicates whether blanks are treated as zeroes or
ignored.
.note IO-09
.bi system file error - %s1
.pc
A system error has occurred while attempting to access a file.
The I/O system error message is displayed.
.note IO-10
.bi format specification does not match data type
.pc
A format specification in the FMT specifier or FORMAT statement
specifies data of one type and the variable list specifies data of a
different type.
.note IO-11
.bi input item does not match the data type of list variable
.pc
In the READ statement, the data type of a variable listed is not of
the same data type in the data file.
For example, non-digit character data being read into an integer
item.
.note IO-12
.bi internal file is full
.pc
The internal file is full of data.
If a file is a variable then the file may only contain one record.
If the file is a character array then there can be one record for
each array element.
.note IO-13
.bi RECL specifier is invalid
.pc
In the OPEN statement, the record length specifier must be a positive
integer expression.
.note IO-14
.bi invalid STATUS specifier in CLOSE statement
.pc
The STATUS specifier can only have a value of KEEP or DELETE.
If the STATUS in the OPEN statement is SCRATCH then the KEEP status
on the CLOSE statement cannot be used.
.note IO-15
.bi unit specified is not connected
.pc
The unit number specified in the I/O statement has not been
previously connected.
.note IO-16
.bi attempt to perform data transfer past end of file
.pc
An attempt has been made to read or write data after the end of file
has been read or written.
.note IO-17
.bi invalid RECL specifier/ACCESS specifier combination
.pc
In the OPEN statement, if the ACCESS specifier is DIRECT then the
RECL specifier must be given.
.note IO-18
.bi REC specifier required in direct access input/output statements
.pc
In the OPEN statement, the ACCESS  specified was DIRECT.
All subsequent input/output statements for that file must use the REC
specifier to indicate which record to access.
.note IO-19
.bi REC specifier not allowed in sequential access input/output statements
.pc
In the OPEN statement, the ACCESS specified was SEQUENTIAL.
The REC specifier may not be used in subsequent I/O statements for
that file.
The REC specifier is used to indicate which record to access when
access is DIRECT.
.note IO-20
.bi %s1 specifier may not change in a subsequent OPEN statement
.pc
The %s1 specifier may not be changed on a subsequent OPEN statement
for the same file, in the same program.
Only the BLANK specifier may be changed.
.note IO-21
.bi invalid STATUS specifier for given file
.pc
In the OPEN statement, the STATUS specifier does not match with the
actual file status: OLD means the file must exist, NEW means the file
must not exist.
If the STATUS specifier is omitted, UNKNOWN is assumed.
.note IO-22
.bi invalid STATUS specifier/FILE specifier combination
.pc
In the OPEN statement, if the STATUS is SCRATCH, the FILE specifier
cannot be used.
If the STATUS is NEW or OLD, the FILE specifier must be given.
.note IO-23
.bi record size exceeded during unformatted input/output
.pc
This error is issued when the size of an i/o list item exceeds the
maximum record size of the file.
The record size can be specified using the RECL= specified in the OPEN
statement.
.note IO-24
.bi unit specified does not exist
.pc
The external unit identifier specified in the input/output statement
has not yet been connected.
Use preconnection or the OPEN statement to connect a file to the
external unit identifier.
.note IO-25
.bi REC specifier is invalid
.pc
The REC specifier must be an unsigned positive integer.
.note IO-26
.bi UNIT specifier is invalid
.pc
The UNIT specifier must be an unsigned integer between 0 and 999
inclusive.
.note IO-27
.bi formatted record or format edit descriptor is too large for record size
.pc
This error is issued when the amount of formatted data in a READ, WRITE or
PRINT statement exceeds the maximum record size of the file.
The record size can be specified using the RECL= specified in the OPEN
statement.
.note IO-28
.bi illegal '%s1=' specifier
.pc
In the OPEN or CLOSE statement the value associated with the %s1
specifier is not a valid value.
In the OPEN statement,
STATUS may only be one of OLD, NEW, SCRATCH or UNKNOWN;
ACCESS may only be one of SEQUENTIAL, APPEND or DIRECT;
FORM may only be one of FORMATTED or UNFORMATTED;
CARRIAGECONTROL may only be one of YES or NO;
RECORDTYPE may only be one of FIXED, TEXT or VARIABLE;
ACTION may only be one of READ, WRITE or READ/WRITE; and
BLANK may only be one of NULL, or ZERO.
In the CLOSE statement the STATUS may only be one of KEEP or DELETE.
.note IO-29
.bi invalid CARRIAGECONTROL specifier/FORM specifier combination
.pc
The CARRIAGECONTROL specifier is only allowed with formatted i/o statements.
.note IO-30
.bi i/o operation not consistent with file attributes
.pc
An attempt was made to read from a file that was opened with
ACTION=WRITE or write to a file that was opened with ACTION=READ.
This message is also issued if you attempt to write to a read-only file or
read from a write-only file.
.note IO-31
.bi symbol %s1 not found in NAMELIST
.pc
During NAMELIST-directed input, a symbol was specified that does not belong
to the NAMELIST group specified in the i/o statement.
.note IO-32
.bi syntax error during NAMELIST-directed input
.pc
Bad input was encountered during NAMELIST-directed input.
Data must be in a special form during NAMELIST-directed input.
.note IO-33
.bi subscripting error during NAMELIST-directed i/o
.pc
An array was incorrectly subscripted during NAMELIST-directed input.
.note IO-34
.bi substring error during NAMELIST-directed i/o
.pc
An character array element or variable was incorrectly substrung during
NAMELIST-directed input.
.note IO-35
.bi BLOCKSIZE specifier is invalid
.pc
In the OPEN statement, the block size specifier must be a positive
integer expression.
.note IO-36
.bi invalid operation for files with no record structure
.pc
An attempt has been made to perform an i/o operation on a file that requires
a record structure.
For example, it is illegal to use a BACKSPACE statement for a file that
has no record structure.
.endnote
.begnote
:DTHD.Program Termination
:DDHD.~b
.note KO-01
.bi floating-point divide by zero
.pc
An attempt has been made to divide a number by zero in a
floating-point expression.
.note KO-02
.bi floating-point overflow
.pc
The floating-point expression result has exceeded the maximum
floating-point number.
.note KO-03
.bi floating-point underflow
.pc
The floating-point expression result has exceeded the minimum
floating-point number.
.note KO-04
.bi integer divide by zero
.pc
An attempt has been made to divide a number by zero in an integer
expression.
.note KO-05
.bi program interrupted from keyboard
.pc
The user has interrupted the compilation or execution of a program
through use of the keyboard.
.note KO-06
.bi integer overflow
.pc
The integer expression result has exceeded the maximum integer
number.
.note KO-07
.bi maximum pages of output exceeded
.pc
The specified maximum number of output pages has been exceeded.
The maximum number of output pages can be increased
by using the "pages=n" option in the command line
or specifying C$PAGES=n in the source file.
.note KO-08
.bi statement count has been exceeded
.pc
The maximum number of source statements has been executed.
The maximum number of source statements that can be executed can be
increased by using the "statements=n" option in the command line or
specifying C$STATEMENTS=n in the source file.
.note KO-09
.bi time limit exceeded
.pc
The maximum amount of time for program execution has been exceeded.
The maximum amount of time can be increased
by using the "time=t" option in the command line or specifying C$TIME=t
in the source file.
.endnote
.begnote
:DTHD.Library Routines
:DDHD.~b
.note LI-01
.bi argument must be greater than zero
.pc
The argument to the intrinsic function must be greater than zero
(i.e., a positive number).
.note LI-02
.bi absolute value of argument to arcsine, arccosine must not exceed one
.pc
The absolute value of the argument to the intrinsic function ASIN or
ACOS cannot be greater than or equal to the value 1.0.
.note LI-03
.bi argument must not be negative
.pc
The argument to the intrinsic function must be greater than or equal
to zero.
.note LI-04
.bi argument(s) must not be zero
.pc
The argument(s) to the intrinsic function must not be zero.
.note LI-05
.bi argument of CHAR must be in the range zero to 255
.pc
The argument to the intrinsic function CHAR must be in the range 0 to
255 inclusive.
CHAR returns the character represented by an 8-bit pattern.
.note LI-06
.bi %s1 intrinsic function cannot be passed 2 complex arguments
.pc
The second argument to the intrinsic function CMPLX and DCMPLX cannot
be a complex number.
.note LI-07
.bi argument types must be the same for the %s1 intrinsic function
.pc
The second argument to the intrinsic function CMPLX or DCMPLX must be
of the same type as the first argument.
The second argument may only be used when the first argument is of
type INTEGER, REAL or DOUBLE PRECISION.
.note LI-08
.bi expecting numeric argument, but %s1 argument was found
.pc
The argument to the intrinsic function, INT, REAL, DBLE, CMPLX, or
DCMPLX was of type %s1 and a numeric argument was expected.
.note LI-09
.bi length of ICHAR argument greater than one
.pc
The length of the argument to the intrinsic function ICHAR must be of
type CHARACTER and length of 1.
ICHAR converts a character to its integer representation.
.note LI-10
.bi cannot pass %s1 as argument to intrinsic function
.pc
The item %s1 cannot be used as an argument to an intrinsic function.
Only constants, simple variables, array elements, and substrung array
elements may be used as arguments.
.note LI-11
.bi intrinsic function requires argument(s)
.pc
An attempt has been made to invoke an intrinsic function and no
actual arguments were listed.
.note LI-12
.bi %s1 argument type is invalid for this generic function
.pc
The type of the argument used in the generic intrinsic function is
not correct.
.note LI-13
.bi this intrinsic function cannot be passed as an argument
.pc
Only the specific name of the intrinsic function can be used as an
actual argument.
The generic name may not be used.
When the generic and intrinsic names are the same, use the INTRINSIC
statement.
.note LI-14
.bi expecting %s1 argument, but %s2 argument was found
.pc
An argument of type %s2 was passed to a function but an argument of
type %s1 was expected.
.note LI-15
.bi intrinsic function was assigned wrong type
.pc
The declared type of an intrinsic function does not agree with the
actual type.
.note LI-16
.bi intrinsic function %s1 is not FORTRAN 77 standard
.pc
The specified intrinsic function is provided as an extension to the
FORTRAN 77 language.
.note LI-17
.bi argument to ALLOCATED intrinsic function must be an allocatable array
.pc
The argument to the intrinsic function ALLOCATED must be an allocatable
array.
.note LI-18
.bi invalid argument to ISIZEOF intrinsic function
.pc
The argument to the intrinsic function ISIZEOF must be a user-defined
structure name, a symbol name, or a constant.
.endnote
.begnote
:DTHD.Mixed Mode
:DDHD.~b
.note MD-01
.bi relational operator has a logical operand
.pc
The operands of a relational expression must either be both
arithmetic or both character expressions.
The operand indicated is a logical expression.
.note MD-02
.bi mixing DOUBLE PRECISION and COMPLEX types is not FORTRAN 77 standard
.pc
The mixing of items of type DOUBLE PRECISION and COMPLEX in an
expression is an extension to the FORTRAN 77 language.
.note MD-03
.bi operator not expecting %s1 operands
.pc
Operands of type %s1 cannot be used with the indicated operator.
The operators **, /, *, +, and &minus. may only have numeric type data.
The operator // may only have character type data.
.note MD-04
.bi operator not expecting %s1 and %s2 operands
.pc
Operands of conflicting type have been encountered.
For example, in a relational expression, it is not possible to compare a
character expression to an arithmetic expression.
Also, the type of the left hand operand of the field selection operator must
be a user-defined structure.
.note MD-05
.bi complex quantities can only be compared using .EQ. or .NE.
.pc
Complex operands cannot be compared using less than (.LT.), less than
or equal (.LE.), greater than (.GT.), or greater than or equal (.GE.)
operators.
.note MD-06
.bi unary operator not expecting %s1 type
.pc
The unary operators, + and &minus., may only be used with numeric types.
The unary operator .NOT. may be used only with a logical or integer operand.
The indicated operand was of type %s1 which is not one of the valid types.
.note MD-07
.bi logical operator with integer operands is not FORTRAN 77 standard
.pc
Integer operands are permitted with the logical
operators .AND., .OR., .EQV., .NEQV., .NOT. and .XOR. as an extension to the
FORTRAN 77 language.
.note MD-08
.bi logical operator %s1 is not FORTRAN 77 standard
.pc
The specified logical operator is an extension to the FORTRAN 77 standard.
.endnote
.begnote
:DTHD.Memory Overflow
:DDHD.~b
.note MO-01
.bi %s1 exceeds compiler limit of %d2 bytes
.pc
An internal compiler limit has been reached.
%s1 describes the limit and %d2 specifies the limit.
.note MO-02
.bi dynamic memory exhausted
.pc
The dynamic memory has been entirely used up.
During the compilation phase, dynamic memory is primarily used for the
symbol table.
During execution, dynamic memory is used for file descriptors and buffers,
and dynamically allocatable arrays.
.note MO-03
.bi dynamic memory exhausted due to length of this statement - statement ignored
.pc
There was not enough memory to encode the specified statement.
This message is usually issued when the compiler is low on memory or if the
statement is a very large statement that spans many continuation lines.
This error does not terminate the compiler since it may have been caused by
a very large statement.
The compiler attempts to compile the remaining statements.
.note MO-04
.bi attempt to deallocate an unallocated array
.pc
An attempt has been made to deallocate an array that has not been previously
allocated.
An array must be allocated using an ALLOCATE statement.
.note MO-05
.bi attempt to allocate an already allocated array
.pc
An attempt has been made to allocate an array that has been previously
allocated in an ALLOCATE statement.
.note MO-06
.bi object memory exhausted
.pc
The amount of object code generated for the program has exceeded the amount
of memory allocated to store the object code.
The "/codesize" option can be used to increase the amount of memory
allocated for object code.
.endnote
.begnote
:DTHD.Parentheses
:DDHD.~b
.note PC-01
.bi missing or misplaced closing parenthesis
.pc
An opening parenthesis '(' was found but no matching closing
parenthesis ')' was found before the end of the statement.
.note PC-02
.bi missing or misplaced opening parenthesis
.pc
A closing parenthesis ')' was found before the matching opening
parenthesis '('.
.note PC-03
.bi unexpected parenthesis
.pc
A parenthesis was found in a statement where parentheses are not
expected.
.note PC-04
.bi unmatched parentheses
.pc
The parentheses in the expression are not balanced.
.endnote
.begnote
:DTHD.PRAGMA Compiler Directive
:DDHD.~b
.note PR-01
.bi expecting symbolic name
.pc
Every auxiliary pragma must refer to a symbol.
This error is issued when the symbolic name is illegal or missing.
Valid symbolic names are formed from the following characters: a dollar
sign, an underscrore, digits and any letter of the alphabet.
The first character of a symbolic name must be alphabetic, a dollar sign, or
an underscore.
.note PR-02
.bi illegal size specified for VALUE attribute
.pc
The VALUE argument attribute of an auxiliary pragma contains in illegal
length specification.
Valid length specifications are 1, 2, 4 and 8.
.note PR-03
.bi illegal argument attribute
.pc
An illegal argument attribute was specified.
Valid argument attributes are VALUE, REFERENCE, or DATA_REFERENCE.
.note PR-04
.bi continuation line must contain a comment character in column 1
.pc
When continuing a line of an auxiliary pragma directive, the continued line
must end with a back-slash ('\') character and the continuation line must
begin with a comment character ('c', 'C' or '*') in column 1.
.note PR-05
.bi expecting '%s1' near '%s2'
.pc
A syntax error was found while processing a PRAGMA directive.
%s1 identifies the expected information and %s2 identifies where in the
pragma the error occurred.
.endnote
.begnote
:DTHD.RETURN Statement
:DDHD.~b
.note RE-01
.bi alternate return specifier only allowed in subroutine
.pc
An alternate return specifier, in the RETURN statement, may only be
specified when returning from a subroutine.
.note RE-02
.bi RETURN statement in main program is not FORTRAN 77 standard
.pc
A RETURN statement in the main program is allowed as an extension to the
FORTRAN 77 standard.
.endnote
.begnote
:DTHD.SAVE Statement
:DDHD.~b
.note SA-01
.bi COMMON block %s1 saved but not properly defined
.pc
The named COMMON block %s1 was listed in a SAVE statement but there is
no named COMMON block defined by that name.
.note SA-02
.bi COMMON block %s1 must be saved in every subprogram in which it appears
.pc
The named COMMON block %s1 appears in a SAVE statement in another
subprogram and is not in a SAVE statement in this subprogram.
If a named COMMON block is specified in a SAVE statement in a
subprogram, it must be specified in a SAVE statement in every
subprogram in which that COMMON block appears.
.note SA-03
.bi name already appeared in a previous SAVE statement
.pc
The indicated name has already been referenced in another SAVE
statement in this subprogram.
.endnote
.begnote
:DTHD.Statement Functions
:DDHD.~b
.note SF-01
.bi statement function definition contains duplicate dummy arguments
.pc
A dummy argument is repeated in the argument list of the statement function.
.note SF-02
.bi character length of statement function name must not be (*)
.pc
If the type of a character function is character, its length specification
must not be (*); it must be a constant integer expression.
.note SF-03
.bi statement function definition contains illegal dummy argument
.pc
A dummy argument of type CHARACTER must have a length specification
of an integer constant expression that is not (*).
.note SF-04
.bi cannot pass %s1 %s2 to statement function
.pc
The actual arguments to a statement function can be any expression
except character expressions involving the concatenation of an
operand whose length specification is (*) unless the operand is a
symbolic constant.
.note SF-05
.bi %s1 actual argument was passed to %s2 dummy argument
.pc
The indicated actual argument is of type %s1 which is not the same
type as that of the dummy argument of type %s2.
.note SF-06
.bi incorrect number of arguments passed to statement function %s1
.pc
The number of arguments passed to statement function %s1 does not
agree with the number of dummy arguments specified in its definition.
.note SF-07
.bi type of statement function name must not be a user-defined structure
.pc
The type of a statement function cannot be a user-defined structure.
Valid types for statement functions are: LOGICAL*1, LOGICAL, INTEGER*1,
INTEGER*2, INTEGER, REAL, DOUBLE PRECISION, COMPLEX, DOUBLE COMPLEX, and
CHARACTER.
If the statement function is of type CHARACTER, its length specification
must not be (*); it must be an integer constant expression.
.endnote
.begnote
:DTHD.Source Management
:DDHD.~b
.note SM-01
.bi system file error reading %s1 - %s2
.pc
An I/O error, described by %s2, has occurred while reading the
FORTRAN source file %s1.
.note SM-02
.bi error opening file %s1 - %s2
.pc
The FORTRAN source file %s1 could not be opened.
The error is described by %s2.
.note SM-03
.bi system file error writing %s1 - %s2
.pc
An I/O error, described by %s2, has occurred while writing to the
file %s1.
.note SM-04
.bi error spawning %s1 - %s2
.pc
An error, described by %s2, occurred while trying to spawn the external
program named %s1.
.note SM-05
.bi error while linking
.pc
An error occurred while trying to create the executable file.  See the
WLINK documentation for a description of the error.
.endnote
.begnote
:DTHD.Structured Programming Features
:DDHD.~b
.note SP-01
.bi cannot have both ATEND and the END= specifier
.pc
It is not valid to use the AT END control statement and the END=
option on the READ statement.
Only one method can be used to control the end-of-file condition.
.note SP-02
.bi ATEND must immediately follow a READ statement
.pc
The indicated AT END control statement or block does not immediately
follow a READ statement.
The AT END control statement or block is executed when an end-of-file
condition is encountered during the read.
.note SP-03
.bi block label must be a symbolic name
.pc
The indicated block label must be a symbolic name.
A symbolic name must start with a letter and contain no more than 32
letters and digits.
A letter is an upper or lower case letter of the alphabet, a dollar sign ($),
or an underscore (_).
A digit is a character in the range '0' to '9'.
.note SP-04
.bi could not find a structure to %s1 from
.pc
This message is issued in the following cases.
.autonote
.note
There is no control structure to QUIT from.
The QUIT statement will transfer control to the statement following the
currently active control structure or return from a REMOTE BLOCK if no
other control structures are active within the REMOTE BLOCK.
.note
There is no control structure to EXIT from.
The EXIT statement is used to exit a loop-processing structure such as
DO, DO WHILE, WHILE and LOOP, to return from a REMOTE BLOCK regardless of the
number of active control structures within the REMOTE BLOCK, or to transfer
control from a GUESS or ADMIT block to the statement following the ENDGUESS
statement.
.note
There is no active looping control structure from which a CYCLE statement
can be used.
A CYCLE statement can only be used within a
DO, DO WHILE, WHILE and LOOP control structure.
.endnote
.note SP-05
.bi REMOTE BLOCK is not allowed in the range of any control structure
.pc
An attempt has been made to define a REMOTE BLOCK inside a control
structure.
Control structures include IF, LOOP, WHILE, DO, SELECT and GUESS.
When a REMOTE BLOCK definition is encountered during execution,
control is transferred to the statement following the corresponding
END BLOCK statement.
.note SP-06
.bi the SELECT statement must be followed immediately by a CASE statement
.pc
The statement immediately after the SELECT statement must be a CASE
statement.
The SELECT statement allows one of a number of blocks of code (case
blocks) to be selected for execution by means of an integer
expression in the SELECT statement.
.note SP-07
.bi cases are overlapping
.pc
The case lists specified in the CASE statements in the SELECT control
structure are in conflict.
Each case list must specify a unique integer constant expression or
range.
.note SP-08
.bi select structure requires at least one CASE statement
.pc
In the SELECT control structure, there must be at least one CASE
statement.
.note SP-09
.bi cannot branch to %d1 from outside control structure in line %d2
.pc
The statement in line %d2 passes control to the statement %d1 in a
control structure.
Control may only be passed out of a control structure or to another
place in that control structure.
Control structures include DO, GUESS, IF, LOOP, SELECT, and WHILE.
.note SP-10
.bi cannot branch to %d1 inside structure on line %d2
.pc
The statement attempts to pass control to statement %d1 in line %d2
which is in a control structure.
Control may only be passed out of a control structure or to another
place in that control structure.
Control structures include DO, GUESS, IF, LOOP, SELECT, and WHILE.
.note SP-11
.bi low end of range exceeds the high end
.pc
The first number, the low end of the range, is greater than the
second number, the high end of the range.
.note SP-12
.bi default case block must follow all case blocks
.pc
The default case block in the SELECT control structure must be the last
case block.
A case block may not follow the default case block.
.note SP-13
.bi attempt to branch out of a REMOTE BLOCK
.pc
An attempt has been made to transfer execution control out of a
REMOTE BLOCK.
A REMOTE BLOCK may only be terminated with the END BLOCK statement.
Execution of a REMOTE BLOCK is similar in concept to execution of a
subroutine.
.note SP-14
.bi attempt to EXECUTE undefined REMOTE BLOCK %s1
.pc
The REMOTE BLOCK %s1 referenced in the EXECUTE statement does not
exist in the current program unit.
A REMOTE BLOCK is local to the program unit in which it is defined
and may not be referenced from another program unit.
.note SP-15
.bi attempted to use REMOTE BLOCK recursively
.pc
An attempt was made to execute a REMOTE BLOCK which was already
active.
.note SP-16
.bi cannot RETURN from subprogram within a REMOTE BLOCK
.pc
An illegal attempt has been made to execute a RETURN statement within
a REMOTE BLOCK in a subprogram.
.note SP-17
.bi %s1 statement is not FORTRAN 77 standard
.pc
The statement %s1 is an extension to the FORTRAN 77 language.
.note SP-18
.bi %s1 block is unfinished
.pc
The block starting with the statement %s1 does not have the ending
block statement.
For example: ATENDDO-ENDATEND, DO-ENDDO, GUESS-ENDGUESS, IF-ENDIF,
LOOP-ENDLOOP, SELECT-ENDSELECT, STRUCTURE-ENDSTRUCTURE and WHILE-ENDWHILE.
.note SP-19
.bi %s1 statement does not match with %s2 statement
.pc
The statement %s1, which ends a control structure, cannot be used with
statement %s2 to form a control structure.
Valid control structures are: LOOP - ENDLOOP, LOOP - UNTIL, WHILE -
ENDWHILE, and WHILE - UNTIL.
.note SP-20
.bi incomplete control structure found at %s1 statement
.pc
The ending control structure statement %s1 was found and there was no
preceding matching beginning statement.
Valid control structures include: ATENDDO - ENDATEND, GUESS -
ENDGUESS, IF - ENDIF, LOOP - ENDLOOP, REMOTE BLOCK - ENDBLOCK, and
SELECT - ENDSELECT.
.note SP-21
.bi %s1 statement is not allowed in %s2 definition
.pc
Statement %s1 is not allowed between a %s2 statement and the
corresponding END %s2 statement.
For example, an EXTERNAL statement is not allowed between a STRUCTURE and
END STRUCTURE statement, a UNION and END UNION statement, or a MAP and
END MAP statement.
.note SP-22
.bi no such field name found in structure %s1
.pc
A structure reference contained a field name that does not belong to the
specified structure.
.note SP-23
.bi multiple definition of field name %s1
.pc
The field name %s1 has already been defined in a structure.
.note SP-24
.bi structure %s1 has not been defined
.pc
An attempt has been made to declare a symbol of user-defined type %s1.
No structure definition for %s1 has occurred.
.note SP-25
.bi structure %s1 has already been defined
.pc
The specified structure has already been defined as a structure.
.note SP-26
.bi structure %s1 must contain at least one field
.pc
Structures must contain at least one field definition.
.note SP-27
.bi recursion detected in definition of structure %s1
.pc
Structure %s1 has been defined recursively.
For example, it is illegal for structure X to contain a field that is itself
a structure named X.
.note SP-28
.bi illegal use of structure %s1 containing union
.pc
Structures containing unions cannot be used in formatted I/O statements or
data initialized.
.note SP-29
.bi allocatable arrays cannot be fields within structures
.pc
An allocatable array cannot appear as a field name within a structure
definition.
.note SP-30
.bi an integer conditional expression is not FORTRAN 77 standard
.pc
A conditional expression is the expression that is evaluated and checked to
determine a path of execution.
A conditional expression can be found in an IF or WHILE statement.
FORTRAN 77 requires that the conditional expression be a logical expression.
As an extension, an integer expression is also allowed.
When an integer expression is used, it is converted to a logical expression
by comparing the value of the integer expression to zero.
.endnote
.begnote
:DTHD.Subprograms
:DDHD.~b
.note SR-01
.bi name can only appear in an EXTERNAL statement once
.pc
A function/subroutine name appears more than once in an EXTERNAL
statement.
.note SR-02
.bi character function %s1 may not be called since size was declared as (*)
.pc
In the declaration of the character function name, the length was
defined to be (*).
The (*) length specification is only allowed for external functions,
dummy arguments or symbolic character constants.
.note SR-03
.bi %s1 can only be used an an argument to a subroutine
.pc
The specified class of an argument must only be passed to a subroutine.
For example, an alternate return specifier is illegal as a subscript or
an argument to a function.
.note SR-04
.bi name cannot appear in both an INTRINSIC and EXTERNAL statement
.pc
The same name appears in an INTRINSIC statement and in an EXTERNAL
statement.
.note SR-05
.bi expecting a subroutine name
.pc
The subroutine named in the CALL statement does not define a
subroutine.
A subroutine is declared in a SUBROUTINE statement.
.note SR-06
.bi %s1 statement not allowed in main program
.pc
The main program can contain any statements except a FUNCTION,
SUBROUTINE, BLOCK DATA, or ENTRY statement.
A SAVE statement is allowed but has no effect in the main program.
A RETURN statement in the main program is an extension to the
FORTRAN 77 language.
.note SR-07
.bi not an intrinsic FUNCTION name
.pc
A name in the INTRINSIC statement is not an intrinsic function name.
Refer to the Language Reference for a complete list of the intrinsic
functions.
.note SR-08
.bi name can only appear in an INTRINSIC statement once
.pc
An intrinsic function name appears more than once in the intrinsic
function list.
.note SR-09
.bi subprogram recursion detected
.pc
An attempt has been made to recursively invoke a subprogram, that is,
to invoke an already active subprogram.
.note SR-10
.bi two main program units in the same file
.pc
There are two places in the program that signify the start of a main
program.
The PROGRAM statement or the first statement that is not enclosed by
a PROGRAM, FUNCTION, SUBROUTINE or BLOCK DATA statement specifies the
main program start.
.note SR-11
.bi only one unnamed %s1 is allowed in an executable program
.pc
There may only be one unnamed BLOCK DATA subprogram or main program
in an executable program.
.note SR-12
.bi function referenced as a subroutine
.pc
An attempt has been made to invoke a function using the CALL
statement.
.note SR-13
.bi attempt to invoke active function/subroutine
.pc
An attempt has been made to invoke the current function/subroutine or
a function/subroutine that was used to invoke current
function/subroutine.
The traceback produced when the error occurred lists all currently
active functions/subroutines.
.note SR-14
.bi dummy argument %s1 is not in dummy argument list of entered subprogram
.pc
The named dummy argument found in the ENTRY statement does not appear
in the subroutine's dummy argument list in the subprogram statement.
.note SR-15
.bi function referenced as %s1 but defined to be %s2
.pc
An attempt has been made to invoke a function of the type %s1 but the
function was defined as %s2 in the FUNCTION or ENTRY statement.
The function name's type must be correctly declared in the main
program.
.note SR-16
.bi function referenced as CHARACTER*%d1 but defined to be CHARACTER*%d2
.pc
The character length of the function in the calling subprogram is %d1 but
the length used to define the function is %d2.
These two lengths must match.
.note SR-17
.bi missing main program
.pc
The program file is either empty or contains only subroutines and
functions.
Each program require a main program.
A main program starts with an optional PROGRAM statement and ends
with an END statement.
.note SR-18
.bi subroutine referenced as a function
.pc
An attempt has been made to invoke a name as a function and has been
defined as a subroutine in a SUBROUTINE or ENTRY statement.
.note SR-19
.bi attempt to invoke a block data subprogram
.pc
An attempt has been made to invoke a block data subprogram.
Block data subprograms are used to initialize variables before
program execution commences.
.note SR-20
.bi structure type of function %s1 does not match expected structure type
.pc
The function returns a structure that is not equivalent to the structure
expected.
Two structures are equivalent if the types and orders of each field
are the same.
Unions are considered equivalent if their sizes are the same.
Field names, and the structure name itself, do not have to be the same.
.endnote
.begnote
:DTHD.Subscripts and Substrings
:DDHD.~b
.note SS-01
.bi substringing of function or statement function return value is not FORTRAN 77 standard
.pc
The character value returned from a CHARACTER function or statement
function cannot be substrung.
Only character variable names and array element names may be
substrung.
.note SS-02
.bi substringing valid only for character variables and array elements
.pc
An attempt has been made to substring a name that is not defined to
be of type CHARACTER and is neither a variable nor an array element.
.note SS-03
.bi subscript expression out of range; %s1 does not exist
.pc
An attempt has been made to reference an element in an array that is
out of bounds of the declared array size.
The array element %s1 does not exist.
.note SS-04
.bi substring expression (%d1:%d2) is out of range
.pc
An expression in the substring is larger than the string length or
less than the value 1.
The substring expression must be one in which
.millust begin
1 <= %d1 <= %d2 <= len
.millust end
.endnote
.begnote
:DTHD.Statements and Statement Numbers
:DDHD.~b
.note ST-01
.bi statement number %d1 has already been defined in line %d2
.pc
The two statements, in line %d2 and the current line, in the current
program unit have the same statement label number, namely %d1.
.note ST-02
.bi statement function definition appears after first executable statement
.pc
There is a statement function definition after the first executable
statement in the program unit.
Statement function definitions must follow specification statements
and precede executable statements.
Check that the statement function name is not an undeclared array
name.
.note ST-03
.bi %s1 statement must not be branched to but was in line %d2
.pc
Line %d2 passed execution control down to the statement %s1.
The specification statements, ADMIT, AT END, BLOCK DATA, CASE, ELSE,
ELSE IF, END AT END, END BLOCK, END DO, END LOOP, END SELECT, END
WHILE, ENTRY, FORMAT, FUNCTION, OTHERWISE, PROGRAM, QUIT, REMOTE
BLOCK, SAVE, SUBROUTINE, and UNTIL statements may not have control of
execution transferred to it.
.note ST-04
.bi branch to statement %d1 in line %d2 not allowed
.pc
An attempt has been made to pass execution control up to the
statement labelled %d1 in line %d2.
The specification statements, ADMIT, AT END, BLOCK DATA, CASE, ELSE,
ELSE IF, END AT END, END BLOCK, END DO, END LOOP, END SELECT, END
WHILE, ENTRY, FORMAT, FUNCTION, OTHERWISE, PROGRAM, QUIT, REMOTE
BLOCK, SAVE, SUBROUTINE, and UNTIL statements may not have control of
execution transferred to it.
.note ST-05
.bi specification statement must appear before %s1 is initialized
.pc
The variable %s1 has been initialized in a specification statement.
A COMMON or EQUIVALENCE statement then references the variable.
The COMMON or EQUIVALENCE statement must appear before the item can
be initialized.
Use the DATA statement to initialize data in this case.
.note ST-06
.bi statement %d1 was referenced as a FORMAT statement in line %d2
.pc
The statement in line %d2 references statement label %d1 as a FORMAT
statement.
The statement at that label is not a FORMAT statement.
.note ST-07
.bi IMPLICIT statement appears too late
.pc
The current IMPLICIT statement is out of order.
The IMPLICIT statement may be interspersed with the PARAMETER
statement but must appear before other specification statements.
.note ST-08
.bi this statement will never be executed due to the preceding branch
.pc
Because execution control will always be passed around the indicated
statement, the statement will never be executed.
.note ST-09
.bi expecting statement number
.pc
The keyword GOTO or ASSIGN has been detected and the next part of the
statement was not a statement number as was expected.
.note ST-10
.bi statement number %d1 was not used as a FORMAT statement in line %d2
.pc
The statement at line %d2 with statement number %d1 is not a FORMAT
statement but the current statement uses statement number %d1
as if it labelled a FORMAT statement.
.note ST-11
.bi specification statement appears too late
.pc
The indicated specification statement appears after
a statement function definition or an executable statement.
All specification statements must appear before these types of
statements.
.note ST-12
.bi %s1 statement not allowed after %s2 statement
.pc
The statement %s1 cannot be the object of a %s2 statement.
%s2 represents a logical IF or WHILE statement.
These statements include: specification statements, ADMIT, AT END,
CASE, DO, ELSE, ELSE IF END, END AT END, END BLOCK, END DO, END
GUESS, ENDIF, END LOOP, END SELECT, END WHILE, ENTRY, FORMAT,
FUNCTION, GUESS, logical IF, block IF, LOOP, OTHERWISE, PROGRAM,
REMOTE BLOCK, SAVE, SELECT, SUBROUTINE, UNTIL, and WHILE.
.note ST-13
.bi statement number must be 99999 or less
.pc
The statement label number specified in the indicated statement has
more than 5 digits.
.note ST-14
.bi statement number cannot be zero
.pc
The statement label number specified in the indicated statement is
zero.
Statement label numbers must be greater than 0 and less than or equal
to 99999.
.note ST-15
.bi this statement could branch to itself
.pc
The indicated statement refers to a statement label number which
appears on the statement itself and therefore could branch to itself,
creating an endless loop.
.note ST-16
.bi missing statement number %d1 - used in line %d2
.pc
A statement with the statement label number %d1 does not exist in the
current program unit.
The statement label number is referenced in line %d2 of the program
unit.
.note ST-17
.bi undecodeable statement or misspelled word %s1
.pc
The statement cannot be identified as an assignment statement or any
other type of FORTRAN statement.
The first word of a FORTRAN statement must be a statement keyword or
the statement must be an assignment statement.
.note ST-18
.bi statement %d1 will never be executed due to the preceding branch
.pc
The statement with the statement label number of %d1 will never be
executed because the preceding statement will always pass execution
control around the statement and no other reference is made to the
statement label.
.note ST-19
.bi expecting keyword or symbolic name
.pc
The first character of a statement is not an alphabetic.
The first word of a statement must be a statement keyword or a
symbolic name.
Symbolic names must start with a letter (upper case or lower case), a
dollar sign ($) or an underscore (_).
.note ST-20
.bi number in %s1 statement is longer than 5 digits
.pc
The number in the PAUSE or STOP statement is longer than 5 digits.
.note ST-21
.bi position of DATA statement is not FORTRAN 77 standard
.pc
The FORTRAN 77 standard requires DATA statements to appear after all
specification statements.
As an extension to the standard, WATCOM FORTRAN 77 allows DATA statements
to appear before specification statements.
Note that in the latter case, the type of the symbol must be established
before data initialization occurs.
.note ST-22
.bi no FORMAT statement with given label
.pc
The current statement refers to the label of a FORMAT statement but
the label appears on some other statement that is not a FORMAT
statement.
.note ST-23
.bi statement number not in list or not the label of an executable statement
.pc
The specified statement number in the indicated statement is not in
the list of statement numbers or it is not the statement label number
of an executable statement.
.note ST-24
.bi attempt to branch into a control structure
.pc
An attempt has been made to pass execution control into a control
structure.
A statement uses a computed statement label number to transfer
control.
This value references a statement inside a control structure.
.endnote
.begnote
:DTHD.Subscripted Variables
:DDHD.~b
.note SV-01
.bi variable %s1 in array declarator must be in COMMON or a dummy argument
.pc
The variable %s1 was used as an array declarator in a subroutine or
function but the variable was not in a COMMON block nor was it a
dummy argument in the FUNCTION, SUBROUTINE or ENTRY statement.
.note SV-02
.bi adjustable/assumed size array %s1 must be a dummy argument
.pc
The array %s1 used in the current subroutine or function must be a
dummy argument.
When the array declarator is adjustable or assumed-size, the array
name must be a dummy argument.
.note SV-03
.bi invalid subscript expression
.pc
The indicated subscript expression is not a valid integer expression
or the high bound of the array is less than the low bound of the
array when declaring the size of the array.
.note SV-04
.bi invalid number of subscripts
.pc
The number of subscripts used to describe an array element does not
match the number of subscripts in the array declaration.
The maximum number of subscripts allowed is 7.
.note SV-05
.bi using %s1 name incorrectly without list
.pc
An attempt has been made to assign a value to the declared array %s1.
Values may only be assigned to elements in the array.
An array element is the array name followed by integer expressions
enclosed in parentheses and separated by commas.
.note SV-06
.bi cannot substring array name %s1
.pc
An attempt has been made to substring the array %s1.
Only an array element may be substrung.
.note SV-07
.bi %s1 treated as an assumed size array
.pc
A dummy array argument has been declared with 1 in the last
dimension.
The array is treated as if an '*' had been specified in place of the
1.
This is done to support a feature called "pseudo-variable
dimensioning" which was supported by some FORTRAN IV compilers and is
identical in concept to FORTRAN 77 assumed-size arrays.
.note SV-08
.bi assumed size array %s1 cannot be used as an i/o list item or a format/unit identifier
.pc
Assumed size arrays (arrays whose last dimension is '*') must not
appear as an i/o list item (i.e. in a PRINT statement), a format
identifier or an internal file specifier.
.note SV-09
.bi limit of 65535 elements per dimension has been exceeded
.pc
On the IBM PC, for 16-bit real mode applications, the number of elements in a
dimension must not exceed 65535.
.endnote
.begnote
:DTHD.Syntax Errors
:DDHD.~b
.note SX-01
.bi unexpected number or name %s1
.pc
The number or name %s1 is in an unexpected place in the statement.
.note SX-02
.bi bad sequence of operators
.pc
The indicated arithmetic operator is out of order.
An arithmetic operator is one of the following:
**, *, /, +, and &minus..
All arithmetic operators must be followed by at least a primary.
A primary is an array element, constant, (expression), function name,
or variable name.
.note SX-03
.bi invalid operator
.pc
The indicated operator between the two arithmetic primaries is not a
valid operator.
Valid arithmetic operators include: **, *, /, +, and &minus..
A primary is an array element, constant, (expression), function name,
or variable name.
.note SX-04
.bi expecting end of statement after right parenthesis
.pc
The end of the statement is indicated by the closing right
parenthesis but more characters were found on the line.
Multiple statements per line are not allowed in FORTRAN 77.
.note SX-05
.bi expecting an asterisk
.pc
The next character of the statement should be an asterisk but another
character was found instead.
.note SX-06
.bi expecting colon
.pc
A colon (:) was expecting but not found.
For example, the colon separating the low and high bounds of a
character substring was not found.
.note SX-07
.bi expecting colon or end of statement
.pc
On a control statement, a word was found at the end of the statement
that cannot be related to the statement.
The last word on several of the control statements may be a block
label.
All block labels must be preceded by a colon (:).
.note SX-08
.bi missing comma
.pc
A comma was expected and is missing.
There must be a comma after the statement keyword AT END when a
statement follows.
A comma must occur between the two statement labels in the GO TO
statement.
A comma must occur between the expressions in the DO statement.
A comma must occur between the names listed in the DATA statement
and specification statements.
A comma must occur between the specifiers in I/O statements.
.note SX-09
.bi expecting end of statement
.pc
The end of the statement was expected but more words were found on
the line and cannot be associated to the statement.
FORTRAN 77 only allows for one statement per line.
.note SX-10
.bi expecting integer variable
.pc
The name indicated in the statement must be of type INTEGER but is
not.
.note SX-11
.bi expecting %s1 name
.pc
A name with the characteristic %s1 was expected at the indicated place
in the statement but is missing.
.note SX-12
.bi expecting an integer
.pc
The length specifier, as in the IMPLICIT statement, must be an
integer constant or an integer constant expression.
The repeat specifier of the value to be assigned to the variables, as
in the DATA statement, must be an integer constant or an integer
constant expression.
.note SX-13
.bi expecting INTEGER, REAL, or DOUBLE PRECISION variable
.pc
The indicated DO variable is not one of the types INTEGER, REAL, or
DOUBLE PRECISION.
.note SX-14
.bi missing operator
.pc
Two primaries were found in an expression and an operator was not
found in between.
A primary is an array element, constant, (expression), function name,
or variable name.
.note SX-15
.bi expecting a slash
.pc
A slash is expected in the indicated place in the statement.
Slashes must be balanced as parentheses.
Slashes are used to enclose the initial data values in specification
statements or to enclose names of COMMON blocks.
.note SX-16
.bi expecting %s1 expression
.pc
An expression of type %s1 is required.
.note SX-17
.bi expecting a constant expression
.pc
A constant expression is required.
.note SX-18
.bi expecting INTEGER, REAL, or DOUBLE PRECISION expression
.pc
The indicated expression is not one of type INTEGER, REAL, or DOUBLE
PRECISION.
Each expression following the DO variable must be an expression of
one of these types.
.note SX-19
.bi expecting INTEGER or CHARACTER constant
.pc
In the PAUSE and STOP statement, the name following the keyword must
be a constant of type INTEGER or of type CHARACTER.
This constant will be printed on the console when the statement is
executed.
.note SX-20
.bi unexpected operator
.pc
An operand was expected but none was found.
For example, in an I/O statement, the comma is used to separate I/O
list items.
Two consecutive commas without an I/O list item between them would
result in this error.
.note SX-21
.bi no closing quote on literal string
.pc
The closing quote of a literal string was not found before the end of
the statement.
.note SX-22
.bi missing or invalid constant
.pc
In a DATA statement, the constant required to initialize a variable
was not found or incorrectly specified.
.note SX-23
.bi expecting character constant
.pc
A character constant is required.
.endnote
.begnote
:DTHD.Type Statements
:DDHD.~b
.note TY-01
.bi length specification before array declarator is not FORTRAN 77 standard
.pc
An array declarator specified immediately after the length
specification of the array is an extension to the FORTRAN 77
language.
.note TY-02
.bi %d1 is an illegal length for %s2 type
.pc
The length specifier %d1 is not valid for the type %s2.
For type LOGICAL, valid lengths are 1 and 4.
For the type INTEGER, valid lengths are 1, 2, and 4.
For the type REAL, valid lengths are 4 and 8.
For the type COMPLEX, valid lengths are 8 and 16.
On the IBM PC, the length specifier for items of type CHARACTER must
be greater than 0 and not exceed 65535.
.note TY-03
.bi length specifier in %s1 statement is not FORTRAN 77 standard
.pc
A length specifier in certain type specification statements is an
extension to the FORTRAN 77 language.
These include: LOGICAL*1, LOGICAL*4, INTEGER*1, INTEGER*2, INTEGER*4,
REAL*4, REAL*8, COMPLEX*8, and COMPLEX*16.
.note TY-04
.bi length specification not allowed with type %s1
.pc
A length specification is not allowed in a DOUBLE PRECISION or
DOUBLE COMPLEX statement.
.note TY-05
.bi type of %s1 has already been established as %s2
.pc
The indicated name %s1 has already been declared to have a different
type, namely %s2.
The name %s1 cannot be used in this specification statement.
.note TY-06
.bi type of %s1 has not been declared
.pc
The indicated name %s1 has not been declared.
This message is only issued when the IMPLICIT NONE specification
statement is used.
.note TY-07
.bi %s1 of type %s2 is illegal in %s3 statement
.pc
The symbol %s1 with type %s2 cannot be used in statement %s3.
For example, a symbol of type STRUCTURE cannot be used in a PARAMETER
statement.
.endnote
.begnote
:DTHD.Undefined Variables
:DDHD.~b
.note UV-01
.bi substring (%d1:%d2) of expression on right side of equal sign is undefined
.pc
The substring (%d1:%d2) has been used on the right hand side of an
expression and its value is undefined.
A value must be assigned to the variable name or array element name
before it can be used in an expression.
.note UV-02
.bi substring (%d1:%d2) of return value of character function %s3 is undefined
.pc
The character function %s3 has been referenced and the function name
has not been assigned a value in the function.
The substring (%d1:%d2) does not have a value.
.note UV-03
.bi substring (%d1:%d2) of an operand in a relational expression is undefined
.pc
The substring (%d1:%d2) has been used in a relational expression and
has not previously been assigned a value.
.note UV-04
.bi substring (%d1:%d2) of an argument to an intrinsic function is undefined
.pc
The substring (%d1:%d2), which is an actual argument to an intrinsic
function, does not have a value assigned to it.
.note UV-05
.bi the value of %s1 is undefined
.pc
The variable name %s1 has been used in a statement and a value has not
yet been assigned to it.
All variables must first be initialized through the use of statements
or data initialization in specification statements.
.note UV-06
.bi argument number %d1 passed to intrinsic function is undefined
.pc
The actual argument number %d1 does not have a value assigned to it.
.endnote
.begnote
:DTHD.Variable Names
:DDHD.~b
.note VA-01
.bi illegal use of %s1 name %s2 in %s3 statement
.pc
The name %s2 has been defined as %s1 and cannot be used as a name in
the statement %s3.
.note VA-02
.bi symbolic name %s1 is longer than 6 characters
.pc
Symbolic names greater than 6 characters is an extension to the
FORTRAN 77 language.
The maximum length is 32 characters.
.note VA-03
.bi %s1 has already been defined as a %s2
.pc
The name %s1 has been previously defined as a %s2 in another
statement and cannot be redefined as specified in the indicated
statement.
.note VA-04
.bi %s1 %s2 has not been defined
.pc
The name %s2 has been referenced to be a %s1 but has not been defined
as such in the program unit.
.note VA-05
.bi %s1 is an unreferenced symbol
.pc
The name %s1 has been defined but not referenced.
.note VA-06
.bi %s1 already belongs to this NAMELIST group
.pc
The name %s1 can only appear in a NAMELIST group once.
However, a name can belong to multiple NAMELIST groups.
.note VA-07
.bi %s1 has been used but not defined
.pc
%s1 has not been defined before using it in a way that requires its
definition.
Note that symbols that are equivalenced, belong to a common block, are
dummy arguments, or passed as an argument to a subprogram, will not be
checked to ensure that they have been defined before requiring a value.
.endnote
