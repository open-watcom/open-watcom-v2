.errhead Subprogram Arguments
.errnote AR-01 invalid number of arguments to intrinsic function %s1
.pc
The number of actual arguments specified in the argument list for the
intrinsic function %s1 does not agree with the dummy argument list.
Consult the Language Reference for information on intrinsic functions and
their arguments.
.errnote AR-02 dummy argument %s1 appears more than once
.pc
The same dummy argument %s1 is named more than once in the dummy
argument list.
.errnote AR-12 dummy argument %s1 must not appear before definition of ENTRY %s2
.pc
The dummy argument %s1 has appeared in an executable statement before
its appearance in the definition of %s2 in an ENTRY statement.
This is illegal.
.errtail
.errhead Block Data Subprograms
.errnote BD-01 %s1 was initialized in a block data subprogram but is not in COMMON
.pc
The variable or array element, %s1, was initialized in a BLOCK DATA
subprogram but was not specified in a named COMMON block.
.errnote BD-02 %s1 statement is not permitted in a BLOCK DATA subprogram
.pc
The statement, %s1, is not allowed in a BLOCK DATA subprogram.
The only statements which are allowed to appear are:
IMPLICIT, PARAMETER, DIMENSION, COMMON, SAVE, EQUIVALENCE, DATA, END,
and type statements.
.errtail
.errhead Source Format and Contents
.errnote CC-01 invalid character encountered in source input
.pc
The indicated statement contains an invalid character.
Valid characters are: letters, digits, $, *, ., +, &minus., /, :, =, (, ),
!, %,  ', and ,(comma).
Any character may be used inside a character or hollerith string.
.errnote CC-02 invalid character in statement number columns
.pc
A column in columns 1 to 5 of the indicated statement contains a
non-digit character.
Columns 1 to 5 contain the statement number label.
It is made up of digits from 0 to 9 and is greater than 0 and less
than or equal to 99999.
.errnote CC-03 character in continuation column, but no statement to continue
.pc
The character in column 6 indicates that this line is a continuation
of the previous statement but there is no previous statement to
continue.
.errnote CC-04 character encountered is not FORTRAN 77 standard
.pc
A non-standard character was encountered in the source input stream.
This is most likely caused by the use of lower case letters.
.errnote CC-05 columns 1-5 in a continuation line must be blank
.pc
When column 6 is marked as a continuation statement to the previous
line, columns 1 to 5 must be left blank.
.errnote CC-06 more than 19 continuation lines is not FORTRAN 77 standard
.pc
More than 19 continuation lines is an extension to the FORTRAN 77 language.
.errnote CC-07 end-of-line comment is not FORTRAN 77 standard
.pc
End-of-line comments are an extension to the FORTRAN 77 language.
End-of-line comments start with the exclamation mark (!) character.
.errnote CC-08 D in column 1 is not FORTRAN 77 standard
.pc
A "D" in column 1 signifies a debug statement that is compiled
when the "__debug__" macro symbol is defined.
If the "__debug__" macro symbol is not defined, the statement is ignored.
The "c$define" compiler directive or the "define" compiler option can be
used to define the "__debug__" macro symbol.
.errnote CC-09 too many continuation lines
.pc
The limit on the number of continuation lines has been reached.
This limit depends on the size of each continuation line.
A minimum of 61 continuation lines is permitted.
If the "xline" option is used, a minimum of 31 continuation lines is
permitted.
.errtail
.errhead COMMON Blocks
.errnote CM-01 %s1 already in COMMON
.pc
The variable or array name, %s1, has already been specified in this or
another COMMON block.
.errnote CM-02 initializing %s1 in COMMON outside of block data subprogram is not FORTRAN 77 standard
.pc
The symbol %s1, in a named COMMON block, has been initialized outside of a
block data subprogram.
This is an extension to the FORTRAN 77 language.
.errnote CM-03 character and non-character data in COMMON is not FORTRAN 77 standard
.pc
The FORTRAN 77 standard specifies that a COMMON block cannot contain
both numeric and character data.
Allowing COMMON blocks to contain both numeric and character data is an
extension to the FORTRAN 77 standard.
.errnote CM-04 COMMON block %s1 has been defined with a different size
.pc
The COMMON block %s1 has been defined with a different size in another
subprogram.
A named COMMON block must define the same amount of storage units
where ever named.
.errnote CM-05 named COMMON block %s1 appears in more than one BLOCK DATA subprogram
.pc
The named COMMON block, %s1, may not appear in more than one BLOCK
DATA subprogram.
.errnote CM-06 blank COMMON block has been defined with a different size
.pc
The blank COMMON block has been defined with a different size in
another subprogram.
This is legal but a warning message is issued.
.errtail
.errhead Constants
.errnote CN-01 DOUBLE PRECISION COMPLEX constants are not FORTRAN 77 standard
.pc
Double precision complex numbers are an extension to the FORTRAN 77
language.
The indicated number is a complex number and at least one of the
parts, real or imaginary, is a double precision constant.
Both real and imaginary parts will be double precision.
.errnote CN-02 invalid floating-point constant %s1
.pc
The floating-point constant %s1 is invalid.
Refer to the chapter entitled "Names, Data Types and Constants"
in the Language Reference.
.errnote CN-03 zero length character constants are not allowed
.pc
FORTRAN 77 does not allow character constants of length 0 (i.e., an
empty string).
.errnote CN-04 invalid hexadecimal/octal constant
.pc
An invalid hexadecimal or octal constant was specified.
Hexadecimal constants can only contain digits or the letters 'a' through
'f' and 'A' through 'F'.
Octal constants can only contain the digits '0' through '7'.
.errnote CN-05 hexadecimal/octal constant is not FORTRAN 77 standard
.pc
Hexadecimal and octal constants are extensions to the FORTRAN 77 standard.
.errtail
.errhead Compiler Options
.errnote CO-01 %s1 is already being included
.pc
An attempt has been made to include a file that is currently being
included in the program.
.errnote CO-02 '%s1' option cannot take a NO prefix
.pc
The compiler option %s1, cannot have the NO prefix specified.
The NO prefix is used to negate an option.
Certain options, including all options that require a value cannot have a
NO prefix.
.errnote CO-03 expecting an equals sign following the %s1 option
.pc
The compiler option %s1, requires an equal sign to be between the
option keyword and its associated value.
.errnote CO-04 the '%s1' option requires a number
.pc
The compiler option %s1 and an equal sign has been detected but the
required associated value is missing.
.errnote CO-05 option '%s1' not recognized - ignored
.pc
The option %s1 is not a recognized compiler option and has been ignored.
Consult the User's Guide for a complete list of compiler options.
.errnote CO-06 '%s1' option not allowed in source input stream
.pc
The option %s1 can only be specified on the command line.
Consult the User's Guide for a description of which options are allowed
in the source input stream.
.errnote CO-07 nesting level exceeded for compiler directives
.pc
Use of the C$IFDEF or C$IFNDEF compiler directives has caused the maximum
nesting level to be exceeded.
The maximum nesting level is 16.
.errnote CO-08 mismatching compiler directives
.pc
This error message is issued if, for example, a C$ENDIF directive is used and
no matching C$IFDEF or C$IFNDEF precedes it.
Incorrect nesting of C$IFDEF, C$IFNDEF, C$ELSE and C$ENDIF directives will
also cause this message to be issued.
.errtail
.errhead Compiler Errors
.errnote CP-01 program abnormally terminated
.pc
This message is issued during the execution of the program.
If you are running WATFOR-77, this message indicates that an internal error
has occurred in the compiler.
Please report this error and any other helpful information about the program
being compiled to Watcom so that the problem can be fixed.
.pc
If you are running an application compiled by the Watcom FORTRAN 77
optimizing compiler, this message may indicate a problem with the compiler
or a problem with your program.
Try compiling your application with the "debug" option.
This causes the generation of run-time checking code to validate, for
example, array subscripts and will help ensure that your program is not
in error.
.errnote CP-02 argument %d1 incompatible with register
.pc
The register specified in an auxiliary pragma for argument number %d1
is invalid.
.errnote CP-03 subprogram %s1 has invalid return register
.pc
The register specified in an auxiliary pragma for the return value of
function %s1 is invalid.
This error is issued when, for example, an auxiliary pragma is used to
specify EAX as the return register for a double precision function.
.errnote CP-04 low on memory - unable to fully optimize %s1
.pc
There is not enough memory for the code generator to fully optimize
subprogram %s1.
.errnote CP-05 internal compiler error %d1
.pc
This error is an internal code generation error.
Please report the specified internal compiler error number and any other
helpful information about the program being compiled to Watcom so that
the problem can be fixed.
.errnote CP-06 illegal register modified by %s1
.pc
An illegal register was said to be modified by %s1 in the auxiliary pragma
for %s1.
In a 32-bit flat memory model, the base pointer register EBP and segment
registers CS, DS, ES, and SS cannot be modified.
In small data models, the base pointer register (32-bit EBP or 16-bit BP)
and segment registers CS, DS, and SS cannot be modified.
In large data models, the base pointer register (32-bit EBP or 16-bit BP)
and segment registers CS, and SS cannot be modified.
.errnote CP-07 %s1
.pc
The message specified by %s1 indicates an error during the code generation
phase.
The most probable cause is an invalid instruction in the in-line assembly
code specified in an auxiliary pragma.
.errnote CP-08 fatal: %s1
.pc
The specified error indicates that the code generator has been abnormally
terminated.
This message will be issued if any internal limit is reached or a keyboard
interrupt sequence is pressed during the code generation phase.
.errnote CP-09 dynamic memory not freed
.pc
This message indicates an internal compiler error.
Please report this error and any other helpful information about the program
being compiled to Watcom so that the problem can be fixed.
.errnote CP-10 freeing unowned dynamic memory
.pc
This message indicates an internal compiler error.
Please report this error and any other helpful information about the program
being compiled to Watcom so that the problem can be fixed.
.errnote CP-11 The automatic equivalence containing %s1 exceeds 32K limit
.pc
In 16-bit environments, the size of an equivalence on the stack must not
exceed 32767 bytes.
.errnote CP-12 The return value of %s1 exceeds 32K limit
.pc
In 16-bit environments, the size of the return value of a function must not
exceed 32767 bytes.
.errnote CP-13 The automatic variable %s1 exceeds 32K limit
.pc
In 16-bit environments, the size of any variable on the stack must not
exceed 32767 bytes.
.errtail
.errhead Character Variables
.errnote CV-01 CHARACTER variable %s1 with length (*) not allowed in this expression
.pc
The length of the result of evaluating the expression is
indeterminate.
One of the operands has an indeterminate length and the result is
being assigned to a temporary.
.errnote CV-02 character variable %s1 with length (*) must be a subprogram argument
.pc
The character variable %s1 with a length specification (*) can only be
used to declare dummy arguments in the subprogram.
The length of a dummy argument assumes the length of the
corresponding actual argument.
.errtail
.errhead Data Initialization
.errnote DA-01 implied DO variable %s1 must be an integer variable
.pc
The implied DO variable %s1 must be declared as a variable of type
INTEGER or must have an implicit INTEGER type.
.errnote DA-02 repeat specification must be a positive integer
.pc
The repeat specification in the constant list of the DATA statement
must be an unsigned positive integer.
.errnote DA-03 %s1 appears in an expression but is not an implied DO variable
.pc
The variable %s1 is used to express the array elements in the DATA
statement but the variable is not used as an implied DO variable.
.errnote DA-04 %s1 in blank COMMON block cannot be initialized
.pc
A blank or unnamed COMMON block is a COMMON statement with the block
name omitted.
The entries in blank COMMON blocks cannot be initialized using DATA
statements.
.errnote DA-05 data initialization with hexadecimal constant is not FORTRAN 77 standard
.pc
Data initialization with hexadecimal constants is an extension to the
FORTRAN 77 language.
.errnote DA-06 cannot initialize %s1 %s2
.pc
Symbol %s2 was used as a %s1.
It is illegal for such a symbol to be initialized in a DATA statement.
The DATA statement can only be used to initialize variables, arrays,
array elements, and substrings.
.errnote DA-07 data initialization in %s1 statement is not FORTRAN 77 standard
.pc
Data initialization in type specification statements is an extension
to the FORTRAN 77 language.
These include: CHARACTER, COMPLEX, DOUBLE PRECISION, INTEGER,
LOGICAL, and REAL.
.errnote DA-08 not enough constants for list of variables
.pc
There are not enough constants specified to initialize all of the
names listed in the DATA statement.
.errnote DA-10 too many constants for list of variables
.pc
There are too many constants specified to initialize the names listed
in the DATA statement.
.errnote DA-12 cannot initialize %s1 variable %s2 with %s3 constant
.pc
The constant of type %s3 cannot be used to initialize the variable %s2 of
type %s1.
.errtail
.errhead Dimensioned Variables
.errnote DM-01 using %s1 incorrectly in dimension expression
.pc
The name used as a dimension declarator has been previously declared
as type %s1 and cannot be used as a dimension declarator. A dimension
declarator must be an integer expression.
.errtail
.errhead DO-loops
.errnote DO-01 statement number %d1 already defined in line %d2 - DO loop is backwards
.pc
The statement number to indicate the end of the DO control structure
has been used previously in the program unit and cannot be used to
terminate the DO loop.
The terminal statement named in the DO statement must follow the DO
statement.
.errnote DO-02 %s1 statement not allowed at termination of DO range
.pc
A non-executable statement cannot be used as the terminal statement
of a DO loop.
These statements include: all declarative statements,
ADMIT, AT END, BLOCK DATA, CASE, DO, ELSE, ELSE IF, END, END AT END,
END BLOCK, END GUESS, END IF, END LOOP, END SELECT, END WHILE, ENTRY,
FORMAT, FUNCTION, assigned GO TO, unconditional GO TO, GUESS,
arithmetic and block IF, LOOP, OTHERWISE, PROGRAM, RETURN, SAVE,
SELECT, STOP, SUBROUTINE, UNTIL, and WHILE.
.errnote DO-03 improper nesting of DO loop
.pc
A nested DO loop has not been properly terminated before the
termination of the outer DO loop.
.errnote DO-04 ENDDO cannot terminate DO loop with statement label
.pc
The ENDDO statement can only terminate a DO loop in which no
statement label was specified in the defining DO statement.
.errnote DO-05 this DO loop form is not FORTRAN 77 standard
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
.errnote DO-06 expecting comma or DO variable
.pc
The item following the DO keyword and the terminal statement-label
(if present) must be either a comma or a DO variable.
A DO variable is an integer, real or double precision variable name.
The DO statement syntax is as follows:
.millust begin
DO <tsl> <,> DO-var = ex, ex <, ex>
.millust end
.errnote DO-07 DO variable cannot be redefined while DO loop is active
.pc
The DO variable named in the DO statement cannot have its value
altered by a statement in the DO loop structure.
.errtail
.errhead Equivalence and/or Common
.errnote EC-01 equivalencing %s1 has caused extension of COMMON block %s2 to the left
.pc
The name %s1 has been equivalenced to a name in the COMMON block %s2.
This relationship has caused the storage of the COMMON block to be
extended to the left.
FORTRAN 77 does not allow a COMMON block to be extended in this way.
.errnote EC-02 %s1 and %s2 in COMMON are equivalenced to each other
.pc
The names %s1 and %s2 appear in different COMMON blocks and each
occupies its own piece of storage and therefore cannot be
equivalenced.
.errtail
.errhead END Statement
.errnote EN-01 missing END statement
.pc
The END statement for a PROGRAM, SUBROUTINE, FUNCTION or BLOCK DATA subprogram
was not found before the next subprogram or the end of the source input
stream.
.errtail
.errhead Equal Sign
.errnote EQ-01 target of assignment is illegal
.pc
The target of an assignment statement, an input/output status specifier
in an input/output statement, or an inquiry specifier in an INQUIRE
statement, is illegal.
The target in any of the above cases must be a variable name, array element,
or a substring name.
.errnote EQ-02 cannot assign value to %s1
.pc
An attempt has been made to assign a value to a symbol with class %s1.
For example, an array name cannot be the target of an assignment
statement.
This error may also be issued when an illegal target is used for the
input/output status specifier in an input/output statement or an inquiry
specifier in an INQUIRE statement.
.errnote EQ-03 illegal use of equal sign
.pc
An equal sign has been found in the statement but the statement is
not an assignment statement.
.errnote EQ-04 multiple assignment is not FORTRAN 77 standard
.pc
More than one equal sign has been found in the assignment statement.
.errnote EQ-05 expecting equals sign
.pc
The equal sign is missing or misplaced.
The PARAMETER statement uses an equal sign to equate a symbolic name
to the value of a constant expression.
The I/O statements use an equal sign to equate the appropriate values
to the various specifiers.
The DO statement uses an equal sign to assign the initial value to
the DO variable.
.errtail
.errhead Equivalenced Variables
.errnote EV-01 %s1 has been equivalenced to 2 different relative positions
.pc
The storage unit referenced by %s1 has been equivalenced to two
different storage units starting in two different places.
One name cannot be associated to two different values at the same
time.
.errnote EV-02 EQUIVALENCE list must contain at least 2 names
.pc
The list of names to make a storage unit equivalent to several names
must contain at least two names.
.errnote EV-03 %s1 incorrectly subscripted in %s2 statement
.pc
The name %s1 has been incorrectly subscripted in a %s2 statement.
.errnote EV-04 incorrect substring of %s1 in %s2 statement
.pc
An attempt has been made to incorrectly substring %s1 in a %s2
statement.
For example, if a CHARACTER variable was declared to be of length 4
then (2:5) would be an invalid substring expression.
.errnote EV-05 equivalencing CHARACTER and non-CHARACTER data is not FORTRAN 77 standard
.pc
Equivalencing numeric and character data is an extension to the
FORTRAN 77 language.
.errnote EV-06 attempt to substring %s1 in EQUIVALENCE statement but type is %s2
.pc
An attempt has been made to substring the symbolic name %s1 in an
EQUIVALENCE statement but the type of the name is %s2 and should be
of type CHARACTER.
.errtail
.errhead Exponentiation
.errnote EX-01 zero**J where J <= 0 is not allowed
.pc
Zero cannot be raised to a power less than or equal to zero.
.errnote EX-02 X**Y where X < 0.0, Y is not of type INTEGER, is not allowed
.pc
When X is less than zero, Y may only be of type INTEGER.
.errnote EX-03 (0,0)**Y where Y is not real is not allowed
.pc
In complex exponentiation, when the base is zero, the exponent may only be a
real number or a complex number whose imaginary part is zero.
.errtail
.errhead ENTRY Statement
.errnote EY-01 type of entry %s1 does not match type of function %s2
.pc
If the type of a function is CHARACTER or a user-defined STRUCTURE, then the
type of all entry names must match the type of the function name.
.errnote EY-02 ENTRY statement not allowed within structured control blocks
.pc
FORTRAN 77 does not allow an ENTRY statement to appear between the
start and end of a control structure.
.errnote EY-03 size of entry %s1 does not match size of function %s2
.pc
The name %s1 found in an ENTRY statement must be declared to be the
same size as that of the function name.
If the name of the function or the name of any entry point has a
length specification of (*), then all such entries must have a length
specification of (*) otherwise they must all have a length
specification of the same integer value.
.errtail
.errhead Format
.errnote FM-01 missing delimiter in format string, comma assumed
.pc
The omission of a comma between the descriptors listed in a format
string is an extension to the FORTRAN 77 language.
Care should be taken when omitting the comma since the assumed
separation may not occur in the intended place.
.errnote FM-02 missing or invalid constant
.pc
An unsigned integer constant was expected with the indicated edit
descriptor but was not correctly placed or was missing.
.errnote FM-03 Ew.dDe format code is not FORTRAN 77 standard
.pc
The edit descriptor Ew.dDe is an extension to the FORTRAN 77
language.
.errnote FM-04 missing decimal point
.pc
The indicated edit descriptor must have a decimal point and an
integer to indicate the number of decimal positions.
These edit descriptors include: F, E, D and G.
.errnote FM-05 missing or invalid edit descriptor in format string
.pc
In the format string, two delimiters were found in succession with no
valid descriptor in between.
.errnote FM-06 unrecognizable edit descriptor in format string
.pc
An edit descriptor has been found in the format string that is not a
valid code.
Valid codes are: apostrophe ('), I, F, E, D, G, L, A, Z, H, T, TL,
TR, X, /, :, S, SP, SS, P, BN, B, $, and \.
.errnote FM-07 invalid repeat specification
.pc
The indicated repeatable edit descriptor is invalid.
The forms of repeatable edit descriptors are:
Iw, Iw.m, Fw.d, Ew.d, Ew.dEe, Dw.d, Gw.d, Gw.dEe, Lw, A, Aw, Ew.dDe,
and Zw where w and e are positive unsigned integer constants, and d
and m are unsigned integer constants.
.errnote FM-08 $ or \ format code is not FORTRAN 77 standard
.pc
The non-repeatable edit descriptors $ and \ are extensions to the
FORTRAN 77 language.
.errnote FM-09 invalid field modifier
.pc
The indicated edit descriptor for a field is incorrect.
Consult the Language Reference for the correct form of the edit descriptor.
.errnote FM-10 expecting end of FORMAT statement but found more text
.pc
The right parenthesis was encountered in the FORMAT statement to
terminate the statement and more text was found on the line.
.errnote FM-11 repeat specification not allowed for this format code
.pc
A repeat specification was found in front of a format code that is a
nonrepeatable edit descriptor.
These include: apostrophe, H, T, TL, TR, X, /, :, S, SP, SS, P, BN,
BZ, $,and \.
.errnote FM-12 no statement number on FORMAT statement
.pc
The FORMAT statement must have a statement label.
This statement number is used by I/O statements to reference the
FORMAT statement.
.errnote FM-13 no closing quote on apostrophe edit descriptor
.pc
The closing quote of an apostrophe edit descriptor was not found.
.errnote FM-14 field count greater than 256 is invalid
.pc
The repeat specification of the indicated edit descriptor is greater
than the maximum allowed of 256.
.errnote FM-15 invalid field width specification
.pc
The width specifier on the indicated edit descriptor is invalid.
.errnote FM-16 Z format code is not FORTRAN 77 standard
.pc
The Z (hexadecimal format) repeatable edit descriptor is an extension
to the FORTRAN 77 language.
.errnote FM-17 FORMAT statement exceeds allotted storage size
.pc
The maximum allowable size of a FORMAT statement has exceeded.
The statement must be split into two or more FORMAT statements.
.errnote FM-18 format specification not allowed on input
.pc
A format specification, in the FORMAT statement, is not allowed to be
used as an input specification.
Valid specifications include: T, TL,TR, X, /, :, P, BN, BZ, I, F, E,
D, G, L, A, and Z.
.errnote FM-19 FORMAT missing repeatable edit descriptor
.pc
An attempt has been made to read or write a piece of data without a
valid repeatable edit descriptor.
All data requires a repeatable edit descriptor in the format.
The forms of repeatable edit descriptors are:
Iw, Iw.m, Fw.d, Ew.d, Ew.dEe, Dw.d, Gw.d, Gw.dEe, Lw, A, Aw, Ew.dDe,
and Zw where w and e are positive unsigned integer constants, and d
and m are unsigned integer constants.
.errnote FM-20 missing constant before X edit descriptor, 1 assumed
.pc
The omission of the constant before an X edit descriptor in a format
specification is an extension to the FORTRAN 77 language.
.errnote FM-21 Ew.dQe format code is not FORTRAN 77 standard
.pc
The edit descriptor Ew.dQe is an extension to the FORTRAN 77
language.
.errnote FM-22 Qw.d format code is not FORTRAN 77 standard
.pc
The edit descriptor Qw.d is an extension to the FORTRAN 77
language.
.errtail
.errhead GOTO and ASSIGN Statements
.errnote GO-01 %s1 statement label may not appear in ASSIGN statement but did in line %d2
.pc
The statement label in the ASSIGN statement in line %d2 references a
%s1 statement.
The statement label in the ASSIGN statement must appear in the same
program unit and must be that of an executable statement or a FORMAT
statement.
.errnote GO-02 ASSIGN of statement number %d1 in line %d2 not allowed
.pc
The statement label %d1 in the ASSIGN statement is used in the line
%d2 which references a non-executable statement.
A statement label must appear in the same program unit as the ASSIGN
statement and must be that of an executable statement or a FORMAT
statement.
.errnote GO-03 expecting TO
.pc
The keyword TO is missing or misplaced in the ASSIGN statement.
.errtail
.errhead Hollerith Constants
.errnote HO-01 hollerith constant is not FORTRAN 77 standard
.pc
Hollerith constants are an extension to the FORTRAN 77 language.
.errnote HO-02 not enough characters for hollerith constant
.pc
The number of characters following the H or h is not equal to the
constant preceding the H or h.
A hollerith constant consists of a positive unsigned integer constant
n followed by the letter H or h followed by a string of exactly n
characters.
.errtail
.errhead IF Statements
.errnote IF-01 ELSE block must be the last block in block IF
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
.errnote IF-02 expecting THEN
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
.errtail
.errhead I/O Lists
.errnote IL-01 missing or invalid format/FMT specification
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
.errnote IL-02 the UNIT may not be an internal file for this statement
.pc
An internal file may only be referenced in a READ or WRITE statement.
An internal file may not be referenced in a BACKSPACE, CLOSE,
ENDFILE, INQUIRE, OPEN, or REWIND statement.
.errnote IL-03 %s1 statement cannot have %s2 specification
.pc
The I/O statement %s1 may not have the control information %s2
specified.
.errnote IL-04 variable must have a size of 4
.pc
The variable used as a specifier in an I/O statement must be of size
4 but another size was specified.
These include the EXIST, OPENED, RECL, IOSTAT, NEXTREC, and NUMBER.
The name used in the ASSIGN statement must also be of size 4 but a
different size was specified.
.errnote IL-05 missing or unrecognizable control list item %s1
.pc
A control list item %s1 was encountered in an I/O statement and is not
a valid control list item for that statement, or a control list item
was expected and was not found.
.errnote IL-06 attempt to specify control list item %s1 more than once
.pc
The control list item %s1 in the indicated I/O statement, has been
named more than once.
.errnote IL-07 implied DO loop has no input/output list
.pc
The implied DO loop specified in the I/O statement does not
correspond with a variable or expression in the input/output list.
.errnote IL-08 list-directed input/output with internal files is not FORTRAN 77 standard
.pc
List-directed input/output with internal files is an extension to the
FORTRAN 77 language.
.errnote IL-09 FORTRAN 77 standard requires an asterisk for list-directed formatting
.pc
An optional asterisk for list-directed formatting is an extension to
the FORTRAN 77 language.
The standard FORTRAN 77 language specifies that an asterisk is required.
.errnote IL-10 missing or improper unit identification
.pc
The control specifier, UNIT, in the I/O statement is either missing
or identifies an improper unit.
The unit specifier specifies an external unit or internal file.
The external unit identifier is a non-negative integer expression or
an asterisk.
The internal file identifier is character variable, character array,
character array element, or character substring.
.errnote IL-11 missing unit identification or file specification
.pc
An identifier to specifically identify the required file is missing.
The UNIT specifier is used to identify the external unit or internal
file.
The FILE specifier in the INQUIRE and OPEN statements is used to
identify the file name.
.errnote IL-12 asterisk unit identifier not allowed in %s1 statement
.pc
The BACKSPACE, CLOSE, ENDFILE, INQUIRE, OPEN, and REWIND statements
require the external unit identifier be an unsigned positive integer
from 0 to 999.
.errnote IL-13 cannot have both UNIT and FILE specifier
.pc
There are two valid forms of the INQUIRE statement; INQUIRE by FILE
and INQUIRE by UNIT.
Both of these specifiers cannot be specified in the same statement.
.errnote IL-14 internal files require sequential access
.pc
An attempt has been made to randomly access an internal file.
Internal files may only be accessed sequentially.
.errnote IL-15 END specifier with REC specifier is not FORTRAN 77 standard
.pc
The FORTRAN 77 standard specifies that an end-of-file condition can only
occur with a file connected for sequential access or an internal file.
The REC specifier indicates that the file is connected for direct access.
This extension allows the programmer to detect an end-of-file condition when
reading the records sequentially from a file connected for direct access.
.errnote IL-16 %s1 specifier in i/o list is not FORTRAN 77 standard
.pc
The specified i/o list item is provided as an extension to the
FORTRAN 77 language.
.errnote IL-17 i/o list is not allowed with NAMELIST-directed format
.pc
An i/o list is not allowed when the format specification is a NAMELIST.
.errnote IL-18 non-character array as format specifier is not FORTRAN 77 standard
.pc
A format specifier must be of type character unless it is an array name.
Allowing a non-character array name is an extension to the FORTRAN 77 standard.
.errtail
.errhead IMPLICIT Statements
.errnote IM-01 illegal range of characters
.pc
In the IMPLICIT statement, the first letter in the range of
characters must be smaller in the collating sequence than
the second letter in the range.
.errnote IM-02 letter can only be implicitly declared once
.pc
The indicated letter has been named more than once in this or a
previous IMPLICIT statement.
A letter may only be named once.
.errnote IM-03 unrecognizable type
.pc
The type declared in the IMPLICIT statement is not one of INTEGER,
REAL, DOUBLE PRECISION, COMPLEX, LOGICAL or CHARACTER.
.errnote IM-04 (*) length specifier in an IMPLICIT statement is not FORTRAN 77 standard
.pc
A character length specified of (*) in an IMPLICIT statement is an
extension to the FORTRAN 77 language.
.errnote IM-05 IMPLICIT NONE allowed once or not allowed with other IMPLICIT statements
.pc
The IMPLICIT NONE statement must be the only IMPLICIT statement in
the program unit in which it appears.
Only one IMPLICIT NONE statement is allowed in a program unit.
.errtail
.errhead Input/Output
.errnote IO-01 BACKSPACE statement requires sequential access mode
.pc
The file connected to the unit specified in the BACKSPACE statement
has not been opened for sequential access.
.errnote IO-02 input/output is already active
.pc
An attempt has been made to read or write a record when there is an
already active read or write in progress.
The execution of a READ or WRITE statement has caused transfer to a
function that contains a READ or WRITE statement.
.errnote IO-03 ENDFILE statement requires sequential access mode
.pc
The specified external unit identifier must be connected for
sequential access but was connected for direct access.
.errnote IO-04 formatted connection requires formatted input/output statements
.pc
The FORM specifier in the OPEN statement specifies FORMATTED and the
subsequent READ and/or WRITE statement does not use formatted I/O.
If the FORM specifier has been omitted and access is SEQUENTIAL then
FORMATTED is assumed.
If the access is DIRECT then UNFORMATTED is assumed.
.errnote IO-05 unformatted connection requires unformatted input/output statements
.pc
The FORM specifier in the OPEN statement specifies UNFORMATTED and
the subsequent READ and/or WRITE statement uses formatted I/O.
If the FORM specifier has been omitted and access is SEQUENTIAL then
FORMATTED is assumed.
If the access is DIRECT then UNFORMATTED is assumed.
.errnote IO-06 REWIND statement requires sequential access
.pc
The external unit identifier is not connected to a sequential file.
The REWIND statement positions to the first record in the file.
.errnote IO-07 bad character in input field
.pc
The data received from the record in a file does not match the type
of the input list item.
.errnote IO-08 BLANK specifier requires FORM specifier to be 'FORMATTED'
.pc
In the OPEN statement, the BLANK specifier may only be used when the
FORM specifier has the value of FORMATTED.
The BLANK specifier indicates whether blanks are treated as zeroes or
ignored.
.errnote IO-09 system file error - %s1
.pc
A system error has occurred while attempting to access a file.
The I/O system error message is displayed.
.errnote IO-10 format specification does not match data type
.pc
A format specification in the FMT specifier or FORMAT statement
specifies data of one type and the variable list specifies data of a
different type.
.errnote IO-11 input item does not match the data type of list variable
.pc
In the READ statement, the data type of a variable listed is not of
the same data type in the data file.
For example, non-digit character data being read into an integer
item.
.errnote IO-12 internal file is full
.pc
The internal file is full of data.
If a file is a variable then the file may only contain one record.
If the file is a character array then there can be one record for
each array element.
.errnote IO-13 RECL specifier is invalid
.pc
In the OPEN statement, the record length specifier must be a positive
integer expression.
.errnote IO-14 invalid STATUS specifier in CLOSE statement
.pc
The STATUS specifier can only have a value of KEEP or DELETE.
If the STATUS in the OPEN statement is SCRATCH then the KEEP status
on the CLOSE statement cannot be used.
.errnote IO-15 unit specified is not connected
.pc
The unit number specified in the I/O statement has not been
previously connected.
.errnote IO-16 attempt to perform data transfer past end of file
.pc
An attempt has been made to read or write data after the end of file
has been read or written.
.errnote IO-17 invalid RECL specifier/ACCESS specifier combination
.pc
In the OPEN statement, if the ACCESS specifier is DIRECT then the
RECL specifier must be given.
.errnote IO-18 REC specifier required in direct access input/output statements
.pc
In the OPEN statement, the ACCESS  specified was DIRECT.
All subsequent input/output statements for that file must use the REC
specifier to indicate which record to access.
.errnote IO-19 REC specifier not allowed in sequential access input/output statements
.pc
In the OPEN statement, the ACCESS specified was SEQUENTIAL.
The REC specifier may not be used in subsequent I/O statements for
that file.
The REC specifier is used to indicate which record to access when
access is DIRECT.
.errnote IO-20 %s1 specifier may not change in a subsequent OPEN statement
.pc
The %s1 specifier may not be changed on a subsequent OPEN statement
for the same file, in the same program.
Only the BLANK specifier may be changed.
.errnote IO-21 invalid STATUS specifier for given file
.pc
In the OPEN statement, the STATUS specifier does not match with the
actual file status: OLD means the file must exist, NEW means the file
must not exist.
If the STATUS specifier is omitted, UNKNOWN is assumed.
.errnote IO-22 invalid STATUS specifier/FILE specifier combination
.pc
In the OPEN statement, if the STATUS is SCRATCH, the FILE specifier
cannot be used.
If the STATUS is NEW or OLD, the FILE specifier must be given.
.errnote IO-23 record size exceeded during unformatted input/output
.pc
This error is issued when the size of an i/o list item exceeds the
maximum record size of the file.
The record size can be specified using the RECL= specified in the OPEN
statement.
.errnote IO-24 unit specified does not exist
.pc
The external unit identifier specified in the input/output statement
has not yet been connected.
Use preconnection or the OPEN statement to connect a file to the
external unit identifier.
.errnote IO-25 REC specifier is invalid
.pc
The REC specifier must be an unsigned positive integer.
.errnote IO-26 UNIT specifier is invalid
.pc
The UNIT specifier must be an unsigned integer between 0 and 999
inclusive.
.errnote IO-27 formatted record or format edit descriptor is too large for record size
.pc
This error is issued when the amount of formatted data in a READ, WRITE or
PRINT statement exceeds the maximum record size of the file.
The record size can be specified using the RECL= specified in the OPEN
statement.
.errnote IO-28 illegal '%s1=' specifier
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
.errnote IO-29 invalid CARRIAGECONTROL specifier/FORM specifier combination
.pc
The CARRIAGECONTROL specifier is only allowed with formatted i/o statements.
.errnote IO-30 i/o operation not consistent with file attributes
.pc
An attempt was made to read from a file that was opened with
ACTION=WRITE or write to a file that was opened with ACTION=READ.
This message is also issued if you attempt to write to a read-only file or
read from a write-only file.
.errnote IO-31 symbol %s1 not found in NAMELIST
.pc
During NAMELIST-directed input, a symbol was specified that does not belong
to the NAMELIST group specified in the i/o statement.
.errnote IO-32 syntax error during NAMELIST-directed input
.pc
Bad input was encountered during NAMELIST-directed input.
Data must be in a special form during NAMELIST-directed input.
.errnote IO-33 subscripting error during NAMELIST-directed i/o
.pc
An array was incorrectly subscripted during NAMELIST-directed input.
.errnote IO-34 substring error during NAMELIST-directed i/o
.pc
An character array element or variable was incorrectly substrung during
NAMELIST-directed input.
.errnote IO-35 BLOCKSIZE specifier is invalid
.pc
In the OPEN statement, the block size specifier must be a positive
integer expression.
.errnote IO-36 invalid operation for files with no record structure
.pc
An attempt has been made to perform an i/o operation on a file that requires
a record structure.
For example, it is illegal to use a BACKSPACE statement for a file that
has no record structure.
.errnote IO-37 integer overflow converting character data to integer
.pc
An overflow has occurred while converting the character data read to its
internal representation as an integer.
.errnote IO-38 range exceeded converting character data to floating-point
.pc
An overflow or underflow has occurred while converting the character data
read to its internal representation as a floating-point number.
.errtail
.errhead Program Termination
.errnote KO-01 floating-point divide by zero
.pc
An attempt has been made to divide a number by zero in a
floating-point expression.
.errnote KO-02 floating-point overflow
.pc
The floating-point expression result has exceeded the maximum
floating-point number.
.errnote KO-03 floating-point underflow
.pc
The floating-point expression result has exceeded the minimum
floating-point number.
.errnote KO-04 integer divide by zero
.pc
An attempt has been made to divide a number by zero in an integer
expression.
.errnote KO-05 program interrupted from keyboard
.pc
The user has interrupted the compilation or execution of a program
through use of the keyboard.
.errnote KO-06 integer overflow
.pc
The integer expression result has exceeded the maximum integer
number.
.errtail
.errhead Library Routines
.errnote LI-01 argument must be greater than zero
.pc
The argument to the intrinsic function must be greater than zero
(i.e., a positive number).
.errnote LI-02 absolute value of argument to arcsine, arccosine must not exceed one
.pc
The absolute value of the argument to the intrinsic function ASIN or
ACOS cannot be greater than or equal to the value 1.0.
.errnote LI-03 argument must not be negative
.pc
The argument to the intrinsic function must be greater than or equal
to zero.
.errnote LI-04 argument(s) must not be zero
.pc
The argument(s) to the intrinsic function must not be zero.
.errnote LI-05 argument of CHAR must be in the range zero to 255
.pc
The argument to the intrinsic function CHAR must be in the range 0 to
255 inclusive.
CHAR returns the character represented by an 8-bit pattern.
.errnote LI-06 %s1 intrinsic function cannot be passed 2 complex arguments
.pc
The second argument to the intrinsic function CMPLX and DCMPLX cannot
be a complex number.
.errnote LI-07 argument types must be the same for the %s1 intrinsic function
.pc
The second argument to the intrinsic function CMPLX or DCMPLX must be
of the same type as the first argument.
The second argument may only be used when the first argument is of
type INTEGER, REAL or DOUBLE PRECISION.
.errnote LI-08 expecting numeric argument, but %s1 argument was found
.pc
The argument to the intrinsic function, INT, REAL, DBLE, CMPLX, or
DCMPLX was of type %s1 and a numeric argument was expected.
.errnote LI-09 length of ICHAR argument greater than one
.pc
The length of the argument to the intrinsic function ICHAR must be of
type CHARACTER and length of 1.
ICHAR converts a character to its integer representation.
.errnote LI-10 cannot pass %s1 as argument to intrinsic function
.pc
The item %s1 cannot be used as an argument to an intrinsic function.
Only constants, simple variables, array elements, and substrung array
elements may be used as arguments.
.errnote LI-11 intrinsic function requires argument(s)
.pc
An attempt has been made to invoke an intrinsic function and no
actual arguments were listed.
.errnote LI-12 %s1 argument type is invalid for this generic function
.pc
The type of the argument used in the generic intrinsic function is
not correct.
.errnote LI-13 this intrinsic function cannot be passed as an argument
.pc
Only the specific name of the intrinsic function can be used as an
actual argument.
The generic name may not be used.
When the generic and intrinsic names are the same, use the INTRINSIC
statement.
.errnote LI-14 expecting %s1 argument, but %s2 argument was found
.pc
An argument of type %s2 was passed to a function but an argument of
type %s1 was expected.
.errnote LI-15 intrinsic function was assigned wrong type
.pc
The declared type of an intrinsic function does not agree with the
actual type.
.errnote LI-16 intrinsic function %s1 is not FORTRAN 77 standard
.pc
The specified intrinsic function is provided as an extension to the
FORTRAN 77 language.
.errnote LI-17 argument to ALLOCATED intrinsic function must be an allocatable array or character*(*) variable
.pc
The argument to the intrinsic function ALLOCATED must be an allocatable
array or character*(*) variable.
.errnote LI-18 invalid argument to ISIZEOF intrinsic function
.pc
The argument to the intrinsic function ISIZEOF must be a user-defined
structure name, a symbol name, or a constant.
.errtail
.errhead Mixed Mode
.errnote MD-01 relational operator has a logical operand
.pc
The operands of a relational expression must either be both
arithmetic or both character expressions.
The operand indicated is a logical expression.
.errnote MD-02 mixing DOUBLE PRECISION and COMPLEX types is not FORTRAN 77 standard
.pc
The mixing of items of type DOUBLE PRECISION and COMPLEX in an
expression is an extension to the FORTRAN 77 language.
.errnote MD-03 operator not expecting %s1 operands
.pc
Operands of type %s1 cannot be used with the indicated operator.
The operators **, /, *, +, and &minus. may only have numeric type data.
The operator // may only have character type data.
.errnote MD-04 operator not expecting %s1 and %s2 operands
.pc
Operands of conflicting type have been encountered.
For example, in a relational expression, it is not possible to compare a
character expression to an arithmetic expression.
Also, the type of the left hand operand of the field selection operator must
be a user-defined structure.
.errnote MD-05 complex quantities can only be compared using .EQ. or .NE.
.pc
Complex operands cannot be compared using less than (.LT.), less than
or equal (.LE.), greater than (.GT.), or greater than or equal (.GE.)
operators.
.errnote MD-06 unary operator not expecting %s1 type
.pc
The unary operators, + and &minus., may only be used with numeric types.
The unary operator .NOT. may be used only with a logical or integer operand.
The indicated operand was of type %s1 which is not one of the valid types.
.errnote MD-07 logical operator with integer operands is not FORTRAN 77 standard
.pc
Integer operands are permitted with the logical
operators .AND., .OR., .EQV., .NEQV., .NOT. and .XOR. as an extension to the
FORTRAN 77 language.
.errnote MD-08 logical operator %s1 is not FORTRAN 77 standard
.pc
The specified logical operator is an extension to the FORTRAN 77 standard.
.errtail
.errhead Memory Overflow
.errnote MO-01 %s1 exceeds compiler limit of %d2 bytes
.pc
An internal compiler limit has been reached.
%s1 describes the limit and %d2 specifies the limit.
.errnote MO-02 out of memory
.pc
All available memory has been used up.
During the compilation phase, memory is primarily used for the symbol table.
During execution, memory is used for file descriptors and buffers,
and dynamically allocatable arrays and character*(*) variables.
.errnote MO-03 dynamic memory exhausted due to length of this statement - statement ignored
.pc
There was not enough memory to encode the specified statement.
This message is usually issued when the compiler is low on memory or if the
statement is a very large statement that spans many continuation lines.
This error does not terminate the compiler since it may have been caused by
a very large statement.
The compiler attempts to compile the remaining statements.
.errnote MO-04 attempt to deallocate an unallocated array or character*(*) variable
.pc
An attempt has been made to deallocate an array that has not been previously
allocated.
An array or character*(*) variable must be allocated using an ALLOCATE
statement.
.errnote MO-05 attempt to allocate an already allocated array or character*(*) variable
.pc
An attempt has been made to allocate an array or character*(*) variable that
has been previously allocated in an ALLOCATE statement.
.errtail
.errhead Parentheses
.errnote PC-01 missing or misplaced closing parenthesis
.pc
An opening parenthesis '(' was found but no matching closing
parenthesis ')' was found before the end of the statement.
.errnote PC-02 missing or misplaced opening parenthesis
.pc
A closing parenthesis ')' was found before the matching opening
parenthesis '('.
.errnote PC-03 unexpected parenthesis
.pc
A parenthesis was found in a statement where parentheses are not
expected.
.errnote PC-04 unmatched parentheses
.pc
The parentheses in the expression are not balanced.
.errtail
.errhead PRAGMA Compiler Directive
.errnote PR-01 expecting symbolic name
.pc
Every auxiliary pragma must refer to a symbol.
This error is issued when the symbolic name is illegal or missing.
Valid symbolic names are formed from the following characters: a dollar
sign, an underscore, digits and any letter of the alphabet.
The first character of a symbolic name must be alphabetic, a dollar sign, or
an underscore.
.errnote PR-02 illegal size specified for VALUE attribute
.pc
The VALUE argument attribute of an auxiliary pragma contains in illegal
length specification.
Valid length specifications are 1, 2, 4 and 8.
.errnote PR-03 illegal argument attribute
.pc
An illegal argument attribute was specified.
Valid argument attributes are VALUE, REFERENCE, or DATA_REFERENCE.
.errnote PR-04 continuation line must contain a comment character in column 1
.pc
When continuing a line of an auxiliary pragma directive, the continued line
must end with a back-slash ('\') character and the continuation line must
begin with a comment character ('c', 'C' or '*') in column 1.
.errnote PR-05 expecting '%s1' near '%s2'
.pc
A syntax error was found while processing a PRAGMA directive.
%s1 identifies the expected information and %s2 identifies where in the
pragma the error occurred.
.errnote PR-06 in-line byte sequence limit exceeded
.pc
The limit on the number of bytes of code that can be generated in-line using
a an auxiliary pragma has been exceeded.
The limit is 127 bytes.
.errnote PR-07 illegal hexadecimal data in byte sequence
.pc
An illegal hexadecimal constant was encountered while processing a in-line
byte sequence of an auxiliary pragma.
Valid hexadecimal constants in an in-line byte sequence must begin with the
letter Z or z and followed by a string of hexadecimal digits.
.errnote PR-08 symbol '%s1' in in-line assembly code cannot be resolved
.pc
The symbol %s1, referenced in an assembly language instruction in an
auxiliary pragma, could not be resolved.
.errtail
.errhead RETURN Statement
.errnote RE-01 alternate return specifier only allowed in subroutine
.pc
An alternate return specifier, in the RETURN statement, may only be
specified when returning from a subroutine.
.errnote RE-02 RETURN statement in main program is not FORTRAN 77 standard
.pc
A RETURN statement in the main program is allowed as an extension to the
FORTRAN 77 standard.
.errtail
.errhead SAVE Statement
.errnote SA-01 COMMON block %s1 saved but not properly defined
.pc
The named COMMON block %s1 was listed in a SAVE statement but there is
no named COMMON block defined by that name.
.errnote SA-02 COMMON block %s1 must be saved in every subprogram in which it appears
.pc
The named COMMON block %s1 appears in a SAVE statement in another
subprogram and is not in a SAVE statement in this subprogram.
If a named COMMON block is specified in a SAVE statement in a
subprogram, it must be specified in a SAVE statement in every
subprogram in which that COMMON block appears.
.errnote SA-03 name already appeared in a previous SAVE statement
.pc
The indicated name has already been referenced in another SAVE
statement in this subprogram.
.errtail
.errhead Statement Functions
.errnote SF-01 statement function definition contains duplicate dummy arguments
.pc
A dummy argument is repeated in the argument list of the statement function.
.errnote SF-02 character length of statement function name must not be (*)
.pc
If the type of a character function is character, its length specification
must not be (*); it must be a constant integer expression.
.errnote SF-03 statement function definition contains illegal dummy argument
.pc
A dummy argument of type CHARACTER must have a length specification
of an integer constant expression that is not (*).
.errnote SF-04 cannot pass %s1 %s2 to statement function
.pc
The actual arguments to a statement function can be any expression
except character expressions involving the concatenation of an
operand whose length specification is (*) unless the operand is a
symbolic constant.
.errnote SF-05 %s1 actual argument was passed to %s2 dummy argument
.pc
The indicated actual argument is of type %s1 which is not the same
type as that of the dummy argument of type %s2.
.errnote SF-06 incorrect number of arguments passed to statement function %s1
.pc
The number of arguments passed to statement function %s1 does not
agree with the number of dummy arguments specified in its definition.
.errnote SF-07 type of statement function name must not be a user-defined structure
.pc
The type of a statement function cannot be a user-defined structure.
Valid types for statement functions are: LOGICAL*1, LOGICAL, INTEGER*1,
INTEGER*2, INTEGER, REAL, DOUBLE PRECISION, COMPLEX, DOUBLE COMPLEX, and
CHARACTER.
If the statement function is of type CHARACTER, its length specification
must not be (*); it must be an integer constant expression.
.errtail
.errhead Source Management
.errnote SM-01 system file error reading %s1 - %s2
.pc
An I/O error, described by %s2, has occurred while reading the
FORTRAN source file %s1.
.errnote SM-02 error opening file %s1 - %s2
.pc
The FORTRAN source file %s1 could not be opened.
The error is described by %s2.
.errnote SM-03 system file error writing %s1 - %s2
.pc
An I/O error, described by %s2, has occurred while writing to the
file %s1.
.errnote SM-06 error opening %s1 - too many temporary files exist
.pc
The compiler was not able to open a temporary file for intermediate storage
during code generation.
Temporary files are created in the directory specified by the TMP
environment variable.
If the TMP environment variable is not set, the temporary file is created
in the current directory.
This error is issued if an non-existent directory is specified in the TMP
environment variable, or more than 26 concurrent compiles are taking place
in a multi-tasking environment and the directory in which the temporary
files are created is the same for all compilation processes.
.errnote SM-07 generation of browsing information failed
.pc
An error occurred during the generation of browsing information.
For example, a disk full condition encountered during the creation of the
browser module file will cause this message to be issued.
Browsing information is generated when the /db switch is specified.
.errtail
.errhead Structured Programming Features
.errnote SP-01 cannot have both ATEND and the END= specifier
.pc
It is not valid to use the AT END control statement and the END=
option on the READ statement.
Only one method can be used to control the end-of-file condition.
.errnote SP-02 ATEND must immediately follow a READ statement
.pc
The indicated AT END control statement or block does not immediately
follow a READ statement.
The AT END control statement or block is executed when an end-of-file
condition is encountered during the read.
.errnote SP-03 block label must be a symbolic name
.pc
The indicated block label must be a symbolic name.
A symbolic name must start with a letter and contain no more than 32
letters and digits.
A letter is an upper or lower case letter of the alphabet, a dollar sign ($),
or an underscore (_).
A digit is a character in the range '0' to '9'.
.errnote SP-04 could not find a structure to %s1 from
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
.errnote SP-05 REMOTE BLOCK is not allowed in the range of any control structure
.pc
An attempt has been made to define a REMOTE BLOCK inside a control
structure.
Control structures include IF, LOOP, WHILE, DO, SELECT and GUESS.
When a REMOTE BLOCK definition is encountered during execution,
control is transferred to the statement following the corresponding
END BLOCK statement.
.errnote SP-06 the SELECT statement must be followed immediately by a CASE statement
.pc
The statement immediately after the SELECT statement must be a CASE
statement.
The SELECT statement allows one of a number of blocks of code (case
blocks) to be selected for execution by means of an integer
expression in the SELECT statement.
.errnote SP-07 cases are overlapping
.pc
The case lists specified in the CASE statements in the SELECT control
structure are in conflict.
Each case list must specify a unique integer constant expression or
range.
.errnote SP-08 select structure requires at least one CASE statement
.pc
In the SELECT control structure, there must be at least one CASE
statement.
.errnote SP-09 cannot branch to %d1 from outside control structure in line %d2
.pc
The statement in line %d2 passes control to the statement %d1 in a
control structure.
Control may only be passed out of a control structure or to another
place in that control structure.
Control structures include DO, GUESS, IF, LOOP, SELECT, and WHILE.
.errnote SP-10 cannot branch to %d1 inside structure on line %d2
.pc
The statement attempts to pass control to statement %d1 in line %d2
which is in a control structure.
Control may only be passed out of a control structure or to another
place in that control structure.
Control structures include DO, GUESS, IF, LOOP, SELECT, and WHILE.
.errnote SP-11 low end of range exceeds the high end
.pc
The first number, the low end of the range, is greater than the
second number, the high end of the range.
.errnote SP-12 default case block must follow all case blocks
.pc
The default case block in the SELECT control structure must be the last
case block.
A case block may not follow the default case block.
.errnote SP-13 attempt to branch out of a REMOTE BLOCK
.pc
An attempt has been made to transfer execution control out of a
REMOTE BLOCK.
A REMOTE BLOCK may only be terminated with the END BLOCK statement.
Execution of a REMOTE BLOCK is similar in concept to execution of a
subroutine.
.errnote SP-14 attempt to EXECUTE undefined REMOTE BLOCK %s1
.pc
The REMOTE BLOCK %s1 referenced in the EXECUTE statement does not
exist in the current program unit.
A REMOTE BLOCK is local to the program unit in which it is defined
and may not be referenced from another program unit.
.errnote SP-15 attempted to use REMOTE BLOCK recursively
.pc
An attempt was made to execute a REMOTE BLOCK which was already
active.
.errnote SP-16 cannot RETURN from subprogram within a REMOTE BLOCK
.pc
An illegal attempt has been made to execute a RETURN statement within
a REMOTE BLOCK in a subprogram.
.errnote SP-17 %s1 statement is not FORTRAN 77 standard
.pc
The statement %s1 is an extension to the FORTRAN 77 language.
.errnote SP-18 %s1 block is unfinished
.pc
The block starting with the statement %s1 does not have the ending
block statement.
For example: ATENDDO-ENDATEND, DO-ENDDO, GUESS-ENDGUESS, IF-ENDIF,
LOOP-ENDLOOP, SELECT-ENDSELECT, STRUCTURE-ENDSTRUCTURE and WHILE-ENDWHILE.
.errnote SP-19 %s1 statement does not match with %s2 statement
.pc
The statement %s1, which ends a control structure, cannot be used with
statement %s2 to form a control structure.
Valid control structures are: LOOP - ENDLOOP, LOOP - UNTIL, WHILE -
ENDWHILE, and WHILE - UNTIL.
.errnote SP-20 incomplete control structure found at %s1 statement
.pc
The ending control structure statement %s1 was found and there was no
preceding matching beginning statement.
Valid control structures include: ATENDDO - ENDATEND, GUESS -
ENDGUESS, IF - ENDIF, LOOP - ENDLOOP, REMOTE BLOCK - ENDBLOCK, and
SELECT - ENDSELECT.
.errnote SP-21 %s1 statement is not allowed in %s2 definition
.pc
Statement %s1 is not allowed between a %s2 statement and the
corresponding END %s2 statement.
For example, an EXTERNAL statement is not allowed between a STRUCTURE and
END STRUCTURE statement, a UNION and END UNION statement, or a MAP and
END MAP statement.
.errnote SP-22 no such field name found in structure %s1
.pc
A structure reference contained a field name that does not belong to the
specified structure.
.errnote SP-23 multiple definition of field name %s1
.pc
The field name %s1 has already been defined in a structure.
.errnote SP-24 structure %s1 has not been defined
.pc
An attempt has been made to declare a symbol of user-defined type %s1.
No structure definition for %s1 has occurred.
.errnote SP-25 structure %s1 has already been defined
.pc
The specified structure has already been defined as a structure.
.errnote SP-26 structure %s1 must contain at least one field
.pc
Structures must contain at least one field definition.
.errnote SP-27 recursion detected in definition of structure %s1
.pc
Structure %s1 has been defined recursively.
For example, it is illegal for structure X to contain a field that is itself
a structure named X.
.errnote SP-28 illegal use of structure %s1 containing union
.pc
Structures containing unions cannot be used in formatted I/O statements or
data initialized.
.errnote SP-29 allocatable arrays cannot be fields within structures
.pc
An allocatable array cannot appear as a field name within a structure
definition.
.errnote SP-30 an integer conditional expression is not FORTRAN 77 standard
.pc
A conditional expression is the expression that is evaluated and checked to
determine a path of execution.
A conditional expression can be found in an IF or WHILE statement.
FORTRAN 77 requires that the conditional expression be a logical expression.
As an extension, an integer expression is also allowed.
When an integer expression is used, it is converted to a logical expression
by comparing the value of the integer expression to zero.
.errnote SP-31 %s1 statement must be used within %s2 definition
.pc
The statement identified by %s1 must appear within a definition identified
by %s2.
.errtail
.errhead Subprograms
.errnote SR-01 name can only appear in an EXTERNAL statement once
.pc
A function/subroutine name appears more than once in an EXTERNAL
statement.
.errnote SR-02 character function %s1 may not be called since size was declared as (*)
.pc
In the declaration of the character function name, the length was
defined to be (*).
The (*) length specification is only allowed for external functions,
dummy arguments or symbolic character constants.
.errnote SR-03 %s1 can only be used an an argument to a subroutine
.pc
The specified class of an argument must only be passed to a subroutine.
For example, an alternate return specifier is illegal as a subscript or
an argument to a function.
.errnote SR-04 name cannot appear in both an INTRINSIC and EXTERNAL statement
.pc
The same name appears in an INTRINSIC statement and in an EXTERNAL
statement.
.errnote SR-05 expecting a subroutine name
.pc
The subroutine named in the CALL statement does not define a
subroutine.
A subroutine is declared in a SUBROUTINE statement.
.errnote SR-06 %s1 statement not allowed in main program
.pc
The main program can contain any statements except a FUNCTION,
SUBROUTINE, BLOCK DATA, or ENTRY statement.
A SAVE statement is allowed but has no effect in the main program.
A RETURN statement in the main program is an extension to the
FORTRAN 77 language.
.errnote SR-07 not an intrinsic FUNCTION name
.pc
A name in the INTRINSIC statement is not an intrinsic function name.
Refer to the Language Reference for a complete list of the intrinsic
functions.
.errnote SR-08 name can only appear in an INTRINSIC statement once
.pc
An intrinsic function name appears more than once in the intrinsic
function list.
.errnote SR-09 subprogram recursion detected
.pc
An attempt has been made to recursively invoke a subprogram, that is,
to invoke an already active subprogram.
.errnote SR-10 two main program units in the same file
.pc
There are two places in the program that signify the start of a main
program.
The PROGRAM statement or the first statement that is not enclosed by
a PROGRAM, FUNCTION, SUBROUTINE or BLOCK DATA statement specifies the
main program start.
.errnote SR-11 only one unnamed %s1 is allowed in an executable program
.pc
There may only be one unnamed BLOCK DATA subprogram or main program
in an executable program.
.errnote SR-12 function referenced as a subroutine
.pc
An attempt has been made to invoke a function using the CALL
statement.
.errtail
.errhead Subscripts and Substrings
.errnote SS-01 substringing of function or statement function return value is not FORTRAN 77 standard
.pc
The character value returned from a CHARACTER function or statement
function cannot be substrung.
Only character variable names and array element names may be
substrung.
.errnote SS-02 substringing valid only for character variables and array elements
.pc
An attempt has been made to substring a name that is not defined to
be of type CHARACTER and is neither a variable nor an array element.
.errnote SS-03 subscript expression out of range; %s1 does not exist
.pc
An attempt has been made to reference an element in an array that is
out of bounds of the declared array size.
The array element %s1 does not exist.
.errnote SS-04 substring expression (%d1:%d2) is out of range
.pc
An expression in the substring is larger than the string length or
less than the value 1.
The substring expression must be one in which
.millust begin
1 <= %d1 <= %d2 <= len
.millust end
.errtail
.errhead Statements and Statement Numbers
.errnote ST-01 statement number %d1 has already been defined in line %d2
.pc
The two statements, in line %d2 and the current line, in the current
program unit have the same statement label number, namely %d1.
.errnote ST-02 statement function definition appears after first executable statement
.pc
There is a statement function definition after the first executable
statement in the program unit.
Statement function definitions must follow specification statements
and precede executable statements.
Check that the statement function name is not an undeclared array
name.
.errnote ST-03 %s1 statement must not be branched to but was in line %d2
.pc
Line %d2 passed execution control down to the statement %s1.
The specification statements, ADMIT, AT END, BLOCK DATA, CASE, ELSE,
ELSE IF, END AT END, END BLOCK, END DO, END LOOP, END SELECT, END
WHILE, ENTRY, FORMAT, FUNCTION, OTHERWISE, PROGRAM, QUIT, REMOTE
BLOCK, SAVE, SUBROUTINE, and UNTIL statements may not have control of
execution transferred to it.
.errnote ST-04 branch to statement %d1 in line %d2 not allowed
.pc
An attempt has been made to pass execution control up to the
statement labelled %d1 in line %d2.
The specification statements, ADMIT, AT END, BLOCK DATA, CASE, ELSE,
ELSE IF, END AT END, END BLOCK, END DO, END LOOP, END SELECT, END
WHILE, ENTRY, FORMAT, FUNCTION, OTHERWISE, PROGRAM, QUIT, REMOTE
BLOCK, SAVE, SUBROUTINE, and UNTIL statements may not have control of
execution transferred to it.
.errnote ST-05 specification statement must appear before %s1 is initialized
.pc
The variable %s1 has been initialized in a specification statement.
A COMMON or EQUIVALENCE statement then references the variable.
The COMMON or EQUIVALENCE statement must appear before the item can
be initialized.
Use the DATA statement to initialize data in this case.
.errnote ST-06 statement %d1 was referenced as a FORMAT statement in line %d2
.pc
The statement in line %d2 references statement label %d1 as a FORMAT
statement.
The statement at that label is not a FORMAT statement.
.errnote ST-07 IMPLICIT statement appears too late
.pc
The current IMPLICIT statement is out of order.
The IMPLICIT statement may be interspersed with the PARAMETER
statement but must appear before other specification statements.
.errnote ST-08 this statement will never be executed due to the preceding branch
.pc
Because execution control will always be passed around the indicated
statement, the statement will never be executed.
.errnote ST-09 expecting statement number
.pc
The keyword GOTO or ASSIGN has been detected and the next part of the
statement was not a statement number as was expected.
.errnote ST-10 statement number %d1 was not used as a FORMAT statement in line %d2
.pc
The statement at line %d2 with statement number %d1 is not a FORMAT
statement but the current statement uses statement number %d1
as if it labelled a FORMAT statement.
.errnote ST-11 specification statement appears too late
.pc
The indicated specification statement appears after
a statement function definition or an executable statement.
All specification statements must appear before these types of
statements.
.errnote ST-12 %s1 statement not allowed after %s2 statement
.pc
The statement %s1 cannot be the object of a %s2 statement.
%s2 represents a logical IF or WHILE statement.
These statements include: specification statements, ADMIT, AT END,
CASE, DO, ELSE, ELSE IF END, END AT END, END BLOCK, END DO, END
GUESS, ENDIF, END LOOP, END SELECT, END WHILE, ENTRY, FORMAT,
FUNCTION, GUESS, logical IF, block IF, LOOP, OTHERWISE, PROGRAM,
REMOTE BLOCK, SAVE, SELECT, SUBROUTINE, UNTIL, and WHILE.
.errnote ST-13 statement number must be 99999 or less
.pc
The statement label number specified in the indicated statement has
more than 5 digits.
.errnote ST-14 statement number cannot be zero
.pc
The statement label number specified in the indicated statement is
zero.
Statement label numbers must be greater than 0 and less than or equal
to 99999.
.errnote ST-15 this statement could branch to itself
.pc
The indicated statement refers to a statement label number which
appears on the statement itself and therefore could branch to itself,
creating an endless loop.
.errnote ST-16 missing statement number %d1 - used in line %d2
.pc
A statement with the statement label number %d1 does not exist in the
current program unit.
The statement label number is referenced in line %d2 of the program
unit.
.errnote ST-17 undecodeable statement or misspelled word %s1
.pc
The statement cannot be identified as an assignment statement or any
other type of FORTRAN statement.
The first word of a FORTRAN statement must be a statement keyword or
the statement must be an assignment statement.
.errnote ST-18 statement %d1 will never be executed due to the preceding branch
.pc
The statement with the statement label number of %d1 will never be
executed because the preceding statement will always pass execution
control around the statement and no other reference is made to the
statement label.
.errnote ST-19 expecting keyword or symbolic name
.pc
The first character of a statement is not an alphabetic.
The first word of a statement must be a statement keyword or a
symbolic name.
Symbolic names must start with a letter (upper case or lower case), a
dollar sign ($) or an underscore (_).
.errnote ST-20 number in %s1 statement is longer than 5 digits
.pc
The number in the PAUSE or STOP statement is longer than 5 digits.
.errnote ST-21 position of DATA statement is not FORTRAN 77 standard
.pc
The FORTRAN 77 standard requires DATA statements to appear after all
specification statements.
As an extension to the standard, Watcom FORTRAN 77 allows DATA statements
to appear before specification statements.
Note that in the latter case, the type of the symbol must be established
before data initialization occurs.
.errtail
.errhead Subscripted Variables
.errnote SV-01 variable %s1 in array declarator must be in COMMON or a dummy argument
.pc
The variable %s1 was used as an array declarator in a subroutine or
function but the variable was not in a COMMON block nor was it a
dummy argument in the FUNCTION, SUBROUTINE or ENTRY statement.
.errnote SV-02 adjustable/assumed size array %s1 must be a dummy argument
.pc
The array %s1 used in the current subroutine or function must be a
dummy argument.
When the array declarator is adjustable or assumed-size, the array
name must be a dummy argument.
.errnote SV-03 invalid subscript expression
.pc
The indicated subscript expression is not a valid integer expression
or the high bound of the array is less than the low bound of the
array when declaring the size of the array.
.errnote SV-04 invalid number of subscripts
.pc
The number of subscripts used to describe an array element does not
match the number of subscripts in the array declaration.
The maximum number of subscripts allowed is 7.
.errnote SV-05 using %s1 name incorrectly without list
.pc
An attempt has been made to assign a value to the declared array %s1.
Values may only be assigned to elements in the array.
An array element is the array name followed by integer expressions
enclosed in parentheses and separated by commas.
.errnote SV-06 cannot substring array name %s1
.pc
An attempt has been made to substring the array %s1.
Only an array element may be substrung.
.errnote SV-07 %s1 treated as an assumed size array
.pc
A dummy array argument has been declared with 1 in the last
dimension.
The array is treated as if an '*' had been specified in place of the
1.
This is done to support a feature called "pseudo-variable
dimensioning" which was supported by some FORTRAN IV compilers and is
identical in concept to FORTRAN 77 assumed-size arrays.
.errnote SV-08 assumed size array %s1 cannot be used as an i/o list item or a format/unit identifier
.pc
Assumed size arrays (arrays whose last dimension is '*') must not
appear as an i/o list item (i.e. in a PRINT statement), a format
identifier or an internal file specifier.
.errnote SV-09 limit of 65535 elements per dimension has been exceeded
.pc
On the IBM PC, for 16-bit real mode applications, the number of elements in a
dimension must not exceed 65535.
.errtail
.errhead Syntax Errors
.errnote SX-01 unexpected number or name %s1
.pc
The number or name %s1 is in an unexpected place in the statement.
.errnote SX-02 bad sequence of operators
.pc
The indicated arithmetic operator is out of order.
An arithmetic operator is one of the following:
**, *, /, +, and &minus..
All arithmetic operators must be followed by at least a primary.
A primary is an array element, constant, (expression), function name,
or variable name.
.errnote SX-03 invalid operator
.pc
The indicated operator between the two arithmetic primaries is not a
valid operator.
Valid arithmetic operators include: **, *, /, +, and &minus..
A primary is an array element, constant, (expression), function name,
or variable name.
.errnote SX-04 expecting end of statement after right parenthesis
.pc
The end of the statement is indicated by the closing right
parenthesis but more characters were found on the line.
Multiple statements per line are not allowed in FORTRAN 77.
.errnote SX-05 expecting an asterisk
.pc
The next character of the statement should be an asterisk but another
character was found instead.
.errnote SX-06 expecting colon
.pc
A colon (:) was expecting but not found.
For example, the colon separating the low and high bounds of a
character substring was not found.
.errnote SX-07 expecting colon or end of statement
.pc
On a control statement, a word was found at the end of the statement
that cannot be related to the statement.
The last word on several of the control statements may be a block
label.
All block labels must be preceded by a colon (:).
.errnote SX-08 missing comma
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
.errnote SX-09 expecting end of statement
.pc
The end of the statement was expected but more words were found on
the line and cannot be associated to the statement.
FORTRAN 77 only allows for one statement per line.
.errnote SX-10 expecting integer variable
.pc
The name indicated in the statement must be of type INTEGER but is
not.
.errnote SX-11 expecting %s1 name
.pc
A name with the characteristic %s1 was expected at the indicated place
in the statement but is missing.
.errnote SX-12 expecting an integer
.pc
The length specifier, as in the IMPLICIT statement, must be an
integer constant or an integer constant expression.
The repeat specifier of the value to be assigned to the variables, as
in the DATA statement, must be an integer constant or an integer
constant expression.
.errnote SX-13 expecting INTEGER, REAL, or DOUBLE PRECISION variable
.pc
The indicated DO variable is not one of the types INTEGER, REAL, or
DOUBLE PRECISION.
.errnote SX-14 missing operator
.pc
Two primaries were found in an expression and an operator was not
found in between.
A primary is an array element, constant, (expression), function name,
or variable name.
.errnote SX-15 expecting a slash
.pc
A slash is expected in the indicated place in the statement.
Slashes must be balanced as parentheses.
Slashes are used to enclose the initial data values in specification
statements or to enclose names of COMMON blocks.
.errnote SX-16 expecting %s1 expression
.pc
An expression of type %s1 is required.
.errnote SX-17 expecting a constant expression
.pc
A constant expression is required.
.errnote SX-18 expecting INTEGER, REAL, or DOUBLE PRECISION expression
.pc
The indicated expression is not one of type INTEGER, REAL, or DOUBLE
PRECISION.
Each expression following the DO variable must be an expression of
one of these types.
.errnote SX-19 expecting INTEGER or CHARACTER constant
.pc
In the PAUSE and STOP statement, the name following the keyword must
be a constant of type INTEGER or of type CHARACTER.
This constant will be printed on the console when the statement is
executed.
.errnote SX-20 unexpected operator
.pc
An operand was expected but none was found.
For example, in an I/O statement, the comma is used to separate I/O
list items.
Two consecutive commas without an I/O list item between them would
result in this error.
.errnote SX-21 no closing quote on literal string
.pc
The closing quote of a literal string was not found before the end of
the statement.
.errnote SX-22 missing or invalid constant
.pc
In a DATA statement, the constant required to initialize a variable
was not found or incorrectly specified.
.errnote SX-23 expecting character constant
.pc
A character constant is required.
.errtail
.errhead Type Statements
.errnote TY-01 length specification before array declarator is not FORTRAN 77 standard
.pc
An array declarator specified immediately after the length
specification of the array is an extension to the FORTRAN 77
language.
.errnote TY-02 %d1 is an illegal length for %s2 type
.pc
The length specifier %d1 is not valid for the type %s2.
For type LOGICAL, valid lengths are 1 and 4.
For the type INTEGER, valid lengths are 1, 2, and 4.
For the type REAL, valid lengths are 4 and 8.
For the type COMPLEX, valid lengths are 8 and 16.
On the IBM PC, the length specifier for items of type CHARACTER must
be greater than 0 and not exceed 65535.
.errnote TY-03 length specifier in %s1 statement is not FORTRAN 77 standard
.pc
A length specifier in certain type specification statements is an
extension to the FORTRAN 77 language.
These include: LOGICAL*1, LOGICAL*4, INTEGER*1, INTEGER*2, INTEGER*4,
REAL*4, REAL*8, COMPLEX*8, and COMPLEX*16.
.errnote TY-04 length specification not allowed with type %s1
.pc
A length specification is not allowed in a DOUBLE PRECISION or
DOUBLE COMPLEX statement.
.errnote TY-05 type of %s1 has already been established as %s2
.pc
The indicated name %s1 has already been declared to have a different
type, namely %s2.
The name %s1 cannot be used in this specification statement.
.errnote TY-06 type of %s1 has not been declared
.pc
The indicated name %s1 has not been declared.
This message is only issued when the IMPLICIT NONE specification
statement is used.
.errnote TY-07 %s1 of type %s2 is illegal in %s3 statement
.pc
The symbol %s1 with type %s2 cannot be used in statement %s3.
For example, a symbol of type STRUCTURE cannot be used in a PARAMETER
statement.
.errtail
.errhead Variable Names
.errnote VA-01 illegal use of %s1 name %s2 in %s3 statement
.pc
The name %s2 has been defined as %s1 and cannot be used as a name in
the statement %s3.
.errnote VA-02 symbolic name %s1 is longer than 6 characters
.pc
Symbolic names greater than 6 characters is an extension to the
FORTRAN 77 language.
The maximum length is 32 characters.
.errnote VA-03 %s1 has already been defined as a %s2
.pc
The name %s1 has been previously defined as a %s2 in another
statement and cannot be redefined as specified in the indicated
statement.
.errnote VA-04 %s1 %s2 has not been defined
.pc
The name %s2 has been referenced to be a %s1 but has not been defined
as such in the program unit.
.errnote VA-05 %s1 is an unreferenced symbol
.pc
The name %s1 has been defined but not referenced.
.errnote VA-06 %s1 already belongs to this NAMELIST group
.pc
The name %s1 can only appear in a NAMELIST group once.
However, a name can belong to multiple NAMELIST groups.
.errnote VA-07 %s1 has been used but not defined
.pc
%s1 has not been defined before using it in a way that requires its
definition.
Note that symbols that are equivalenced, belong to a common block, are
dummy arguments, or passed as an argument to a subprogram, will not be
checked to ensure that they have been defined before requiring a value.
.errnote VA-08 dynamically allocating %s1 is not FORTRAN 77 standard
.pc
Allocatable storage are extensions to the FORTRAN 77 standard.
.errnote VA-09 %s1 in NAMELIST %s2 is illegal
.pc
Symbol %s1 appearing in NAMELIST %s2 is illegal.
Symbols appearing in a NAMELIST cannot be dummy arguments, allocatable, or of
a user-defined type.
.errtail
