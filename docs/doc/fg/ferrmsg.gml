.chap *refid=ferrmsg &cmpname Diagnostic Messages
.*
.np
The &product compiler checks for errors both at compile time and
execution time.
.np
Compile time errors may result from incorrect program syntax,
violations of the rules of the language, underflow and overflow as a
result of evaluation of expressions, etc.
Three types of messages are issued:
.begnote
.note EXTENSION
.ix 'EXTENSION message'
*EXT* - This indicates that the programmer has used a feature which is
strictly an extension of the FORTRAN 77 language definition.
Such extensions may not be accepted by other FORTRAN 77 compilers.
.note WARNING
.ix 'WARNING message'
*WRN* - This indicates that a possible problem has been detected by
the compiler.
For example, an unlabelled executable statement which follows an
unconditional transfer of control can never be executed and so the
compiler will issue a message about this condition.
.note ERROR
.ix 'ERROR message'
*ERR* - This indicates that some error was detected which must be
corrected by the programmer.
.endnote
.pc
An object file will be created as long as no ERROR type messages are
issued.
.np
Execution or run time errors may result from arithmetic underflow or
overflow, input/output errors, etc.
An execution time error causes the program to cease execution.
.np
Consider the following program, named "DEMO1.FOR", which contains errors.
.cp 22
.tinyexam begin
* This program demonstrates the following features of
* &company's FORTRAN 77 compiler:
*
*    1. Extensions to the FORTRAN 77 standard are flagged.
*
*    2. Compile time error diagnostics are extensive.  As many
*       errors as possible are diagnosed.
*
*    3. Warning messages are displayed where potential problems
*       can arise.
*
        PROGRAM MAIN
        DIMENSION A(10)
        DO I=1,10
            A(I) = I
            I = I + 1
        ENDLOOP
        GO TO 30
        J = J + 1
30      END
.tinyexam end
.pc
If we compile this program with the "extensions" option, the following
output appears on the screen.
.code begin
C>&ccmd16 demo1 /exten
WATCOM FORTRAN 77/16 Optimizing Compiler Version &ver..&rev &cdate
.cpyrit 1984
.trdmrk
demo1.for(14): *EXT* DO-05 this DO loop form is not FORTRAN 77 standard
demo1.for(16): *ERR* DO-07 column 13, DO variable cannot be redefined
    while DO loop is active
demo1.for(17): *ERR* SP-19 ENDLOOP statement does not match with DO
    statement
demo1.for(19): *WRN* ST-08 this statement will never be executed due
    to the preceding branch
demo1.for: 9 statements, 0 bytes, 1 extensions, 1 warnings, 2 errors
.code end
.pc
The diagnostic messages consist of the following information:
.autopoint
.point
the name of the file being compiled,
.point
the line number of the line containing the error (in parentheses),
.point
a message type of either extension (*EXT*), error (*ERR*) or warning
(*WRN*),
.point
a message class and number (e.g., ST-08), and
.point
text explaining the nature of the error.
.endpoint
.pc
In the above example, the first error occurred on line 16 of the file
"DEMO1.FOR".
Error number DO-07 was diagnosed.
The second error occurred on line 17 of the file "DEMO1.FOR".
Error number SP-20 was diagnosed.
The other errors are informational messages that do not prevent the
successful compilation of the source file.
.np
The following is a list of all messages produced by &cmpname followed
by a brief description.
Run-time messages (messages displayed during execution) are also
presented.
The messages contain references to
.id %s
and
.id %d.
They represent strings that are substituted by &cmpname to make the
error message more exact.
.id %d
represents a string of digits;
.id %s
any string, usually a symbolic name such as
.id AMOUNT,
a type such as
.id INTEGER,
or a symbol class such as
.id SUBROUTINE.
An error message may contain more than one reference to %d.
In such a case, the description will reference them as %dn where n is
the occurrence of %d in the error message.
The same is true for references to %s.
