.chap Using &product
.*
.section Command Line Format
.*
.np
This chapter describes the command-line oriented interface to
&cmpname..
The following chapter describes the editor interface to &cmpname..
&cmpname is invoked as follows:
.ix 'invoking &cmpname'
.ix 'command line' 'syntax'
.mbigbox begin
&cmpcmdup.[[options] [d:][path]filename[.ext] [options] [prog_parms]]

           or

&cmp2cmdup.[[options] [d:][path]filename[.ext] [options] [prog_parms]]
.mbigbox end
.synote
.note []
The square brackets denote items which are optional.
.ix '&cmpcmdup'
.note &cmpcmdup
is the version of &cmpname which does not require the 80x87 numeric
data processor.
.ix '&cmp2cmdup'
.note &cmp2cmdup
is the version of &cmpname which requires the 80x87 numeric
data processor.
.note options
.ix 'options' 'specifying'
is a list of valid &cmpname options, each preceded by a slash.
The list of options may not contain any blank characters.
Options may be specified in any order.
.note d:
is an optional drive specification such as
:FNAME.&dr1:eFNAME., :FNAME.&dr2:eFNAME., etc.
If not specified, the default drive is assumed.
.note path
is an optional path specification such as
:FNAME.&pc.programs&pc.src&pc.:eFNAME..
If not specified, the current directory is assumed.
.note filename
is the file name of the file to be compiled.
.note ext
is the file extension of the file to be compiled.
If omitted, a file extension of :FNAME.&cxt:eFNAME. is assumed.
If the period "." is specified but not the extension, the file is
assumed to have no file extension.
.ix 'command line' 'arguments'
.ix 'FGETCMD'
.note prog_parms
is an optional list of program parameters that may be accessed by
the executing program through use of the library function
.id FGETCMD
or the functions
.id IARGC
and
.id IGETARG.
See the chapter entitled "The &cmpname Subprogram Library" for
more information on these functions.
.endnote
.np
The FORTRAN source file may consist of variable length ASCII records
separated by a "LF" (line feed) character optionally preceded by a
"CR" (carriage return) character.
Normally, &cmpname processes only the first 72 characters of any record.
The remainder, if any, is ignored.
Records which are shorter than 72 characters in length are treated as
if they had trailing blank characters.
.ix 'XLINE option'
.ix 'options' 'XLINE'
The "XLINE" compiler option can be used to extend the last column of
the statement portion of a line to column 132.
.*
.section Invoking &cmpname
.*
.np
.ix '&cmpcmdup'
In the following sections, we will use &cmpcmdup to illustrate
how to use &cmpname..
.ix '&cmp2cmdup'
The name &cmp2cmdup. may be used in an equivalent manner when
the math coprocessor is present.
.np
Consider the following program which attempts to compute the
square root of a number and display the result.
.exam begin
*     Program to compute a square root

      NUMBER = -144
      SQROOT = SQRT( NUMBER )
      PRINT *, SQROOT
      END
.exam end
.pc
This program consists of a
.us comment line
.ix 'comment line'
and some FORTRAN
.us statements.
.ix 'statement'
Let us assume that a copy of the above program is contained in the file
:FNAME.root&cxt.:eFNAME.
in the current directory.
Type the following command and press the "Enter" key.
.millust &prompt.&cmpcmd root
.np
.ix 'message interpretation'
There are deliberate errors in the program so the message
.code begin
root&cxt.(4): *ERR* LI-12 column 15, INTEGER argument type is invalid
for this generic function
.code end
.pc
is displayed on the screen.
How should this message be interpreted?
.np
The message
.mono *ERR*
means that some violation of the rules of FORTRAN has been committed.
.ix 'message error'
.ix 'error'
.ix 'line'
Specifically, on the fourth line of the source file
:FNAME.root&cxt.:eFNAME.
near column 15 an error was detected.
The error says that the argument of the generic FORTRAN function that
was used (in this case
.id SQRT
.ct )
cannot be an
.kw INTEGER
argument.
The
.us error code
.ix 'error code'
.mono LI-12
is composed of an error category and number.
The error code may be used to look up the message in the
diagnostic summary in the appendix entitled "&cmpname Diagnostic
Messages" at the end of this guide.
This appendix provides an explanation of all diagnostic messages
issued by &cmpname..
.np
In addition to the message that is displayed on the screen, a
source program listing file,
:FNAME.root.lst:eFNAME., is created by the compiler.
.ix 'listing source'
If you examine the listing file, you will find something similar
to the following.
.code begin
.in -3
&product &ver &cdate
.cpyrit 1984
.trdmrk

Options: list,disk,extensions,reference,warnings,terminal,run,xtype,check,
arraycheck,statements=0,pages=0,time=0,codesize=524288,pagesize=66,fover

        *     Program to compute a square root

      1       NUMBER = -144
      2       SQROOT = SQRT( NUMBER )
                      $
*ERR* LI-12 INTEGER argument type is invalid for this generic function
      3       PRINT *, SQROOT
      4       END


Compile time (in seconds):          0  Execution time (in seconds):     0
Size of object code:                0  Number of extensions:            0
Size of local data area(s):         0  Number of warnings:              0
Size of global data area:           0  Number of errors:                1
Object bytes free:                  0  Statements Executed:             0
.in +3
.code end
.np
In the listing file, we find a copy of the original source program and
some additional lines which the compiler has added.
The line that was in error is followed by a line containing an error
indicator or marker (a dollar sign "$").
An error message, similar to the one that was displayed on the screen,
follows that line.
.np
Near the start of the program listing is a line indicating which
options were in effect at the time the compilation of the program
began.
These and other options are described in the chapter entitled
"Compiler Options".
.ix 'statistics compile-time'
.ix 'statistics execution-time'
.ix 'compiler statistics'
At the end of the program listing, we find some statistics such as how
long it took to compile the program and the number of error messages
that were issued.
The compile and execution times are reported as actual elapsed time in
seconds using the "time-of-day" clock.
.np
Some statistics on memory utilization also appear.
The object code size is the number of bytes of executable code
generated by &cmpname for the program.
The local data area size includes scalar variables (undimensioned
variables), numeric and logical constants, and temporaries used
in evaluating expressions.
The global data area size includes character variables, dimensioned
variables, and common blocks.
The amount of unused memory is reflected in the "Object bytes free"
statistic.
This number represents the amount of memory still available to the compiler
for object code.
.ix 'CODESIZE option'
.ix 'options' 'CODESIZE'
The amount of memory used for object code can be set by using the "codesize="
option.
The default is 512k.
.np
A number appears to the left of
.us some
of the FORTRAN source lines.
This number is generated by the compiler for each FORTRAN statement.
It is used primarily at execution time when the compiler detects an
error and issues a message.
.ix 'statement number'
.ix 'internal statement number'
.ix 'ISN'
The statement number is sometimes referred to as an "Internal
Statement Number" or ISN.
We will demonstrate the usefulness of the ISN in a subsequent example.
.np
Upon checking out the rules about the use of the
.id SQRT
function, we find that the argument must be a floating-point
quantity of type real, double precision, complex, or double
precision complex.
Because of the rules of FORTRAN,
.id NUMBER
is an integer variable by default.
The program must be corrected either by changing the name of the
variable or by explicitly declaring the type of the variable.
.np
This is just one example of how &cmpname handles compile-time
errors.
.ix 'compile-time'
We use the term "compile-time" since the error was detected
before any execution of the program was attempted.
.np
Change the program by adding a variable declaration so that it
resembles the following:
.exam begin
*     Program to compute a square root

      REAL NUMBER
      NUMBER = -144
      SQROOT = SQRT( NUMBER )
      PRINT *, SQROOT
      END
.exam end
.np
When you try to compile and execute the revised program, a different
message is displayed on the screen:
.code begin
*ERR* LI-03 argument must not be negative
- Executing in MAIN PROGRAM, statement 3 in file root&cxt.
.code end
.pc
.ix 'execution-time'
This message is issued at execution time.
.ix 'trace-back'
Execution-time messages are accompanied by a program "trace-back".
The trace-back is a sequence of one or more messages which describes
the state of the executing program at the time the error was detected.
.np
If you examine the listing file, you will find something similar to
the following.
.code begin
.in -3
&product &ver &cdate
.cpyrit 1984
.trdmrk

Options: list,disk,extensions,reference,warnings,terminal,run,xtype,check,
arraycheck,statements=0,pages=0,time=0,codesize=524288,pagesize=66,fover

        *     Program to compute a square root

      1       REAL NUMBER
      2       NUMBER = -144
      3       SQROOT = SQRT( NUMBER )
      4       PRINT *, SQROOT
      5       END

*ERR* LI-03 argument must not be negative
- Executing in MAIN PROGRAM, statement 3 in file root&cxt.

Compile time (in seconds):          0  Execution time (in seconds):     0
Size of object code:              114  Number of extensions:            0
Size of local data area(s):        80  Number of warnings:              0
Size of global data area:           0  Number of errors:                1
Object bytes free:             524244  Statements Executed:             2
.in +3
.code end
.np
.ix 'ISN'
Here, we see the use of the ISN.
The error has occurred in the third
.us statement
of the FORTRAN program.
Note that this is
.us not
the third line in the file.
.np
The name of the file which contained the program is also
displayed in the trace-back messages (in this case
:FNAME.root&cxt.:eFNAME.).
This is done since FORTRAN source lines can come from several
different files.
.np
The above illustrates the execution-time error detection
(or diagnostic) capability of &cmpname..
.np
We would like to close this discussion by correcting the program
so that it produces the desired result.
.exam begin
*     Program to compute a square root

      REAL NUMBER
      NUMBER = 144
      SQROOT = SQRT( NUMBER )
      PRINT *, SQROOT
      END
.exam end
.np
When you compile and execute the program, the result
.millust begin
              12.0000000
.millust end
.pc
is displayed on the screen.
This is indeed the square root of the number 144.
The decimal point and trailing zeroes indicate that the result is
a floating-point quantity.
.np
The contents of the listing file are:
.code begin
.in -3
&product &ver &cdate
.cpyrit 1984
.trdmrk

Options: list,disk,extensions,reference,warnings,terminal,run,xtype,check,
arraycheck,statements=0,pages=0,time=0,codesize=524288,pagesize=66,fover

        *     Program to compute a square root

      1       REAL NUMBER
      2       NUMBER = 144
      3       SQROOT = SQRT( NUMBER )
      4       PRINT *, SQROOT
      5       END


Compile time (in seconds):          0  Execution time (in seconds):     0
Size of object code:              114  Number of extensions:            0
Size of local data area(s):        80  Number of warnings:              0
Size of global data area:           0  Number of errors:                0
Object bytes free:             524100  Statements Executed:             4
.in +3
.code end
.np
You may have noticed that the printed result of this program appeared
on the screen but not in the listing file.
.ix 'listing'
.ix 'printed result'
.ix 'NOXTYPE option'
.ix 'options' 'NOXTYPE'
&cmpname provides an option ("NOXTYPE") which allows you to direct
such printed results to the listing file instead of the screen.
Recompile the program but this time also specify the "NOXTYPE" option.
.millust &prompt.&cmpcmd./noxtype root
See the chapter entitled "Compiler Options" for more information on
the "NOXTYPE" option.
.*
.section More Examples Using Compiler Options
.*
.np
Assume that the file
:FNAME.test&cxt.:eFNAME.
contains the following statements.
.exam begin
      X=12.
      Y=X*-X
      PRINT *,X,Y
      END
.exam end
.pc
To compile and execute this program using &cmpname,
type
.millust &prompt.&cmpcmd./type test
On the screen will appear
something similar to the following:
.code begin
.in -3
&product &ver &cdate
.cpyrit 1984
.trdmrk

Options: list,extensions,reference,warnings,terminal,run,xtype,check,
arraycheck,statements=0,pages=0,time=0,codesize=524288,pagesize=66,fover

      1       X=12.
      2       Y=X*-X
                  $
*ERR* SX-02 bad sequence of operators
      3       PRINT *,X,Y
      4       END


Compile time (in seconds):          0  Execution time (in seconds):     0
Size of object code:                0  Number of extensions:            0
Size of local data area(s):         0  Number of warnings:              0
Size of global data area:           0  Number of errors:                1
Object bytes free:                  0  Statements Executed:             0
.in +3
.code end
.pc
.ix 'TYPE option'
.ix 'options' 'TYPE'
In this case, we have used the "TYPE" option.
This option causes &cmpname to output the program listing,
complete with diagnostics, to the screen instead of a disk file.
.millust &prompt.&cmpcmd./nowarn/ext d:example1
The above example requests &cmpname to compile and execute the file
:FNAME.d:example1&cxt.:eFNAME., specifying that no warning messages
are to be issued, and that extension messages are to be issued.
Since we did not use the "TYPE" option, &cmpname produces a file with
an extension of :FNAME.&lst:eFNAME. using the source filename as the
filename for the listing file.
.ix 'listing file'
If you were to examine the contents of the current directory
of the default disk, you would find the listing file:
.millust begin
EXAMPLE1.LST
.millust end
.np
.ix 'ERRORFILE option'
.ix 'options' 'ERRORFILE'
The following example is similar to the previous one except that the
"ERRORFILE" option is also specified.
.millust &prompt.&cmpcmd./nowarn/ext/errorfile d:example1
If any diagnostic messages are issued, &cmpname produces a file with
an extension of :FNAME.&err:eFNAME.
using the source filename as the filename for the error file.
.ix 'error file'
If diagnostic messages were issued and you were to examine the
contents of the current directory of the default disk, you
would find the error file:
.millust begin
:FNAME.example1&err:eFNAME.
.millust end
.pc
The error file will contain a summary of the error messages that also
appear in the listing file.
The messages are presented in the same form as they appear on the
screen.
.remark
The listing and error file names are constructed by using only
the filename part of the full source file specification.
.ix 'listing file'
.ix 'error file'
.millust begin
:FNAME.&dr3.&pc.imsl&pc.single&pc.main&pc.lineqsmn&cxt:eFNAME.
.millust end
.pc
Given the source file specification above, &cmpname constructs
the following names.
.millust begin
Listing file name: :FNAME.lineqsmn&lst:eFNAME.
Error file name:   :FNAME.lineqsmn&err:eFNAME.
.millust end
.pc
If the current directory is
.millust begin
:FNAME.&dr2.&pc.tests:eFNAME.
.millust end
.pc
then the full listing and error file specifications are:
.millust begin
Listing file: :FNAME.&dr2.&pc.tests&pc.lineqsmn&lst:eFNAME.
Error file:   :FNAME.&dr2.&pc.tests&pc.lineqsmn&err:eFNAME.
.millust end
.eremark
.*
.section Loading &cmpname Into Memory
.*
.np
In the examples we have shown so far, &cmpname is loaded into memory,
immediately begins to process the specified file, and then returns to
the operating system.
In many cases, it would be helpful if the compiler could be loaded
into the memory of the personal computer and then left there.
In doing so, we can eliminate successive loading of the compiler.
On a networked system, this may improve response time if the compiler
resides on a network disk.
To make effective use of this feature, we should be able to compile
programs in succession.
It would also be very useful if we could issue operating system
commands and run other programs while the compiler is loaded in
memory.
.np
&cmpname supports all of these things.
Try the following example.
.millust &prompt.&cmpcmd.
&cmpname will be loaded into memory and then the prompt
.ix 'command line' 'prompt'
.ix 'prompt'
.millust begin
To exit, press Ctrl/Z and then press Enter
WATFOR>
.millust end
.pc
is displayed.
A command line may be specified at the prompt.
The format of the command line is identical to that described
previously.
.millust begin
&prompt.&cmpcmd.
To exit, press Ctrl/Z and then press Enter
WATFOR>/type/nowarn test
.millust end
.pc
If you do not enter any command but simply press the line entering
key, &cmpname will redisplay the prompt.
After processing a FORTRAN program, the prompt will reappear.
Additional programs can be processed.
This is illustrated in the next example.
.millust begin
&prompt.&cmpcmd.
To exit, press Ctrl/Z and then press Enter
WATFOR>/type simplex

To exit, press Ctrl/Z and then press Enter
WATFOR>/type simplex2

To exit, press Ctrl/Z and then press Enter
WATFOR>^Z
.millust end
.pc
If the Ctrl/Z sequence is entered and the line entering key is pressed
then &cmpname will exit.
.np
Operating system commands and other programs may be executed at the prompt by
preceding the line with an exclamation point (!).
.millust begin
&prompt.&cmpcmd.
To exit, press Ctrl/Z and then press Enter
WATFOR>!dir *&cxt
WATFOR>simplex
To exit, press Ctrl/Z and then press Enter
WATFOR>!type simplex&lst
WATFOR>simplex2
To exit, press Ctrl/Z and then press Enter
WATFOR>^Z
.millust end
.pc
In the above example, a "directory" command is used to list the names
of files with an extension of :FNAME.&cxt:eFNAME..
The program "SIMPLEX" is then processed by &cmpname..
Subsequently, the type command is used to examine the listing file
and, finally, the program "SIMPLEX2" is processed.
.autonote Notes:
.note
A certain amount of free memory is required to successfully execute a
command.
A message is issued if the command cannot be executed.
.note
&cmpname will remain resident in memory until the Ctrl/Z sequence is
entered.
.note
If an exclamation point is entered with no argument, the operating
system command prompt will be displayed.
In this case, the "EXIT" command must be used to return to &cmpname..
.endnote
.np
.ix 'command line' '?'
.ix '? on command line'
.ix 'options' 'summary'
The command line format is displayed when a "?" is specified
or when an error is made in specifying the command line.
.ix 'command line' 'syntax'
.code begin
.in -3
WATFOR>?
&product &ver &cdate
.cpyrit 1984
.trdmrk

 Usage: &cmpcmd. {- or /<option>} <file-spec> {- or /<option>} {<prog-parm>}

                   *File Management Options*
LISt       generate a listing file      PRint      listing file to printer
TYpe       listing file to terminal     DIsk       listing file to disk
INclist    list INCLUDE files           ERrorfile  generate an error file
Object     generate object code         LInk       generate an executable
                      *Diagnostic Options*
EXtensions issue extension messages     Reference  issue unreferenced warning
WArnings   issue warning messages       EXPlicit   explicit typing required
CHeck      undefined variable checking  ARraycheck array type checking
                      *Debugging Options*
DEBug      invoke run-time debugger
                    *Character Set Options*
CHInese    Chinese character set        Japanese   Japanese character set
KOrean     Korean character set
                    *Miscellaneous Options*
SHort      INTEGER/LOGICAL size 2/1     Xline      extend line length
DEFine     define macro                 FORmat     relax FORMAT type checking
WILd       relax wild branch checking   TErminal   display diagnostic messages
Quiet      operate quietly              RESources  messages in resource file
CC         unit 6 is carriage control   EDit       use editor interface
RUN        run the executable file      LOgio      log units 5 and 6
XType      unit 6 output to terminal    Statements statement limit
Pages      page limit                   TIme       time limit
COdesize   code size limit              PAGESIze   number of lines per page
IOVer      report integer overflows     FOVer      report float-pt. overflows
FUNder     report float-pt. underflows
To exit, press Ctrl/Z and then press Enter
WATFOR>
.in +3
.code end
.pc
The curly braces indicate that any number of options may be specified.
.*
.section Default Options
.*
.np
The defaults for most compiler options can be altered by setting the
.ev WATFOR
environment variable.
The following command will suppress extension and warning messages
each time &cmpname is invoked.
.exam begin
&prompt.&setcmd watfor=/noext/nowarn
.exam end
.np
Any options that are specified on the command line become default
options for all subsequent processing until &cmpname is terminated.
Consider the following example.
.millust begin
&prompt.&cmpcmd. /noexten/nowarn
To exit, press Ctrl/Z and then press Enter
WATFOR>/type simplex
.millust end
.pc
The above example illustrates the memory-resident mode of operation of
the compiler as described in the previous section.
.ix 'NOEXTENSIONS option'
.ix 'options' 'NOEXTENSIONS'
.ix 'NOWARNINGS option'
.ix 'options' 'NOWARNINGS'
The "NOEXTENSIONS" and "NOWARNINGS" options will be chosen as defaults
whenever a program is compiled.
These options, together with "TYPE", are in effect when the program
"SIMPLEX" is compiled.
.millust begin
&prompt.&cmpcmd. /noexten/nowarn
To exit, press Ctrl/Z and then press Enter
WATFOR>/type simplex
To exit, press Ctrl/Z and then press Enter
WATFOR>/extensions simplex2
To exit, press Ctrl/Z and then press Enter
WATFOR>^Z
.millust end
.pc
.ix 'EXTENSIONS option'
.ix 'options' 'EXTENSIONS'
A second program is processed and, in this case, we specified the
"EXTENSIONS" option to obtain extension diagnostic messages.
.ix 'NOLIST option'
.ix 'options' 'NOLIST'
This time a listing file called
:FNAME.simplex2.lst:eFNAME.
is created (assuming that you have not included the "NOLIST" option in
the
.ev WATFOR
environment variable).
.*
.section Continuing the Command Line
.*
.np
.ix 'command line' 'continuation'
If you wish to continue the command line on a second line, a series of
three dots (...) may be typed as the last part of the command line.
&cmpname will prompt for the remainder of the command line.
.millust begin
&prompt.&cmpcmd./type...
WATFOR> test
.millust end
.pc
The command line must be continued at the very next prompt.
.autonote Notes:
.note
In the above example, a space preceded the file specification since
the second line is "glued" onto the first line without introducing any
extra blanks.
.note
The "..." command line continuation signal may be used only once per
command line.
.note
.ix 'TYPE option'
.ix 'options' 'TYPE'
Since the "TYPE" option was specified on the command line, it becomes
a default option for this and all subsequent processing until &cmpname
is terminated.
.endnote
.np
Some more examples follow.
.millust begin
&prompt.&cmpcmd./type/nowarn/errorfile/debug &pc.imsl&pc....
WATFOR>double&pc.main&pc.statsmn
To exit, press Ctrl/Z and then press Enter
WATFOR>^Z
.millust end
.millust begin
&prompt.&cmpcmd./type/nowarn/errorfile/debug ...
WATFOR>&pc.imsl&pc.double&pc.main&pc.statsmn
To exit, press Ctrl/Z and then press Enter
WATFOR>^Z
.millust end
.millust begin
&prompt.&cmpcmd.
To exit, press Ctrl/Z and then press Enter
WATFOR>/warn/exten tests&pc....
WATFOR>simplex2
To exit, press Ctrl/Z and then press Enter
WATFOR>^Z
.millust end
.millust begin
&prompt.&cmpcmd./warn/exten
To exit, press Ctrl/Z and then press Enter
WATFOR>tests&pc....
WATFOR>simplex2
To exit, press Ctrl/Z and then press Enter
WATFOR>/nowarn tests&pc.simplex3
To exit, press Ctrl/Z and then press Enter
WATFOR>^Z
.millust end
.*
.section Batch Processing
.*
.np
.ix 'batch processing'
In the previous sections, we described how &cmpname can be loaded into
memory and then used to process FORTRAN programs until the Ctrl/Z
sequence is entered.
This feature has been generalized in the following manner.
.cp 8
.mbigbox begin
&cmpcmdup. [options] @[d:][path]filename[.ext] [options]

      or

&cmp2cmdup. [options] @[d:][path]filename[.ext] [options]
.mbigbox end
.synote
.note []
The square brackets denote items which are optional.
.ix '&cmpcmdup'
.note &cmpcmdup
is the version of &cmpname which does not require the 80x87
numeric data processor.
.ix '&cmp2cmdup'
.note &cmp2cmdup
is the version of &cmpname which requires the 80x87 numeric
data processor.
.note options
.ix 'options' 'specifying'
is a list of valid &cmpname options, each preceded by a slash.
The list of options may not contain any blank characters.
Options may be specified in any order.
Any options that are specified on the command line become
default options for all subsequent processing until &cmpname is
terminated.
.note @
is the signal that the file specification which follows is the
name of a file containing &cmpname command lines.
.note d:
is an optional drive specification such as
:FNAME.&dr1:eFNAME., :FNAME.&dr2:eFNAME., etc.
If not specified, the default drive is assumed.
.note path
is an optional path specification such as
:FNAME.&pc.batch&pc.commands&pc.:eFNAME..
If not specified, the current directory is assumed.
.note filename
is the file name of the command file.
.note ext
is the file extension of the command file.
If omitted, no file extension is assumed.
.endnote
.np
Assume that the following lines were stored in a file called
:FNAME.batch.run:eFNAME..
.millust begin
/nowarn/errorfile &pc.imsl&pc.single&pc.main&pc.statsmn
/nocheck &pc.imsl&pc.double&pc.main&pc.regrsmn
/warn/exten tests&pc.simplex2
.millust end
.pc
To process these three commands, simply type:
.millust &prompt.&cmpcmd. @batch.run
&cmpname will exit upon encountering the end of the file of commands.
.millust &prompt.&cmpcmd./noext @batch.run
.ix 'NOEXTENSIONS option'
.ix 'options' 'NOEXTENSIONS'
This example is similar to the previous except that the "NOEXTENSIONS"
option is selected as a default for the entire batch.
.*
.section Halting an Executing FORTRAN Program
.*
.np
.ix 'compilation' 'halting'
.ix 'execution' 'halting'
.ix 'interrupting execution'
.ix 'interrupting compilation'
.ix 'Ctrl/Break'
While the FORTRAN program is being compiled, &cmpname may be halted by
entering a Ctrl/Break sequence from the keyboard.
Compilation of the program will be terminated.
.np
While a FORTRAN program is executing, it may also be halted by
entering a Ctrl/Break sequence from the keyboard.
A message will appear indicating that the program was interrupted and
then a program traceback will follow.
.ix 'DEBUG option'
.ix 'options' 'DEBUG'
If the "DEBUG" option was specified when the program was compiled then
the &cmpname debugger will be entered.
Otherwise, a prompt will appear which will allow you to resume
or terminate execution.
