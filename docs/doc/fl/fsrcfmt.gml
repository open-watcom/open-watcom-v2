.chap FORTRAN Source Program Format
.*
.if &e'&dohelp eq 1 .do begin
.np
The following sections describe input rules for FORTRAN source
programs.
.do end
.*
.section Character Set
.*
.np
.ix 'character set' 'FORTRAN'
The FORTRAN
.us character set
consists of twenty-six letters, ten digits, and thirteen special
characters.
.np
The letters are:
.millust begin
A B C D E F G H I J K L M N O P Q R S T U V W X Y Z
.millust end
.np
The digits are:
.mono 0 1 2 3 4 5 6 7 8 9
.np
The special characters are:
.if &e'&dohelp eq 0 .do begin
.tb set \
.se c0=&INDlvl+2+1
.se c1=&INDlvl+2+10
.se c2=&INDlvl+2+27
.tb &c0 &c1 &c2
.se c0=&INDlvl+1
.se c1=&INDlvl+10
.se c2=&INDlvl+27
.bx on &c0 &c1 &c2
\Character \Name of Character\
.bx
.se c0=&INDlvl+2+4
.se c1=&INDlvl+2+13
.se c2=&INDlvl+2+27
.tb &c0 &c1 &c2
\          \Blank
\=         \Equals
\+         \Plus
\-         \Minus
\*         \Asterisk
\/         \Slash
\(         \Left Parenthesis\
\)         \Right Parenthesis\
\,         \Comma
\.         \Decimal Point\
\$         \Currency Symbol\
\'         \Apostrophe
\:         \Colon
.bx off
.tb
.tb set
.do end
.el .do begin
.millust begin
+-----------+-----------------------+
| Character | Name of Character     |
+-----------+-----------------------+
|           | Blank                 |
| =         | Equals                |
| +         | Plus                  |
| -         | Minus                 |
| *         | Asterisk              |
| /         | Slash                 |
| (         | Left Parenthesis      |
| )         | Right Parenthesis     |
| ,         | Comma                 |
| .         | Decimal Point         |
| $         | Currency Symbol       |
| '         | Apostrophe            |
| :         | Colon                 |
+-----------+-----------------------+
.millust end
.do end
.np
The FORTRAN character set is a subset of the character set of
the computing system which you are using.
.ix 'character set' 'processor'
We shall refer to the larger character set as the
.us processor character set.
.*
.section Extended Character Set
.*
.np
.xt begin
&product also includes the following special characters.
.xt end
.if &e'&dohelp eq 0 .do begin
.se c0=&INDlvl+1
.se c1=&INDlvl+10
.se c2=&INDlvl+27
.bxt on &c0 &c2 &c1
.tb set `
`Character `Name of Character`
.bxt
.se c0=&INDlvl+2+4
.se c1=&INDlvl+2+13
.se c2=&INDlvl+2+27
.tb &c0 &c1 &c2
`!         `Exclamation Mark
`%         `Percentage Symbol
`\         `Back slash
.bxt off
.do end
.el .do begin
.millust begin
+-----------+-----------------------+
| Character | Name of Character     |
+-----------+-----------------------+
| !         | Exclamation Mark      |
| %         | Percentage Symbol     |
| \         | Back slash            |
+-----------+-----------------------+
.millust end
.do end
.*
.section Source Program Format
.*
.np
&product supports one source program format.
A FORTRAN program is composed of
.us lines.
There are three types of lines; the
.us comment
line, the
.us initial
line, and the
.us continuation
line.
.beglevel
.*
.section Comment Line
.*
.np
.ix 'comment line'
.ix 'line' 'comment'
.us Comment
lines are denoted by placing a "C" or "*" in column one of the line.
&product also allows the use of a lowercase "c" as a comment
indicator.
.ix 'blank line'
.ix 'line' 'blank'
.us Blank
lines are treated as comment lines.
Comment lines may be placed anywhere in the program source (i.e.,
they may appear before a FORTRAN statement, they may be intermingled
with continuation lines, or they may appear after a statement).
There is no restriction on the number of comment lines.
Comment lines may contain any characters from the processor character
set.
.np
.xt begin
&product allows end-of-line comments.
.ix 'end-of-line' 'comments'
.ix 'comments' 'end-of-line'
If a "!" character appears in column 1 or anywhere in the statement
portion of a source line, the remainder of that line is treated as a
comment unless the "!" appears inside quotation marks or in column 6.
.xt end
.*
.section Debug Line (Extension)
.*
.np
.xt begin
.ix 'debug line'
.ix 'line' 'debug'
.us Debug
lines are denoted by placing a "D" or "d" in column one of the line.
Debug lines contain FORTRAN statements.
There is no restriction on the number of debug lines.
Normally, the FORTRAN statements on debug lines are ignored by the
compiler.
See the User's Guide for information on activating debug statements.
.xt end
.*
.section Initial Line
.*
.np
.ix 'initial line'
.ix 'line' 'initial'
An
.us initial
line is the first line of a FORTRAN statement.
Column 6 of this line must be blank or contain the digit "0".
A comment line can never be an initial line.
.ix 'statement label'
Columns 1 through 5 of an initial line may contain a
.us statement label.
Statement labels are composed entirely of digits.
The statement label may be thought of as an integral number and, as
such, leading 0 digits are not significant.
For example, the label composed of the digits "00123" is the same as
the label "123".
The same label may not identify more than one statement in a
.us program unit.
.ix 'program unit'
A program unit is a series of comment lines and FORTRAN statements
ending in an
.kw END
statement.
The body of the FORTRAN
.us statement
.ix 'statement'
is entered starting in column 7 and stopping at column 72.
.ix 'sequence field'
Column 73 and on is called the
.us sequence field
and is ignored by the compiler.
.*
.section Continuation Line
.*
.np
A statement may be
.us continued
.ix 'continuation line'
.ix 'line' 'continuation'
on a new line.
A continuation character is placed in column 6.
The continuation character may not be a blank character or a "0"
character.
FORTRAN 77 requires that the continuation character be selected from
the FORTRAN character set but &product allows any character from
the processor's character set.
The statement number field must be blank.
The previous statement is continued on the new line, starting in
column 7 and continuing to column 72.
.xt begin
Under the control of a compiler option, &product permits the source
statement to extend to column 132.
.xt end
.np
.cp 5
FORTRAN 77 allows up to 19 continuation lines to continue a statement.
.xt begin
&product extends this by allowing more than 19 continuation lines.
A minimum of 61 continuation lines are permitted when the source
statement ends at column 72.
A minimum of 31 continuation lines are permitted when the source
statement ends at column 132.
The maximum number of continuation lines depends on the sum of the
lengths of all the continuation lines.
.xt end
.*
.section Significance of the Blank Character
.*
.np
Except in the following cases,
blank characters have no meaning within a program unit.
.begpoint
.point (1)
Character and Hollerith constants.
.point (2)
Apostrophe and H edit descriptors in format specifications.
.endpoint
.np
For example, the symbolic name
.id A B
is the same as the symbolic name
.id AB.
.*
.keep 12
.section Significance of Lower Case Characters (Extension)
.*
.np
.xt begin
.ix 'lower case'
Except in the following cases, lower case characters are treated
as if they were the upper case equivalent.
This is a &product extension to the usual rules of FORTRAN.
.begpoint
.point (1)
Character and Hollerith constants.
.point (2)
Apostrophe and H edit descriptors in format specifications.
.endpoint
.np
Hence,
.id TOTAL,
.id total,
and
.id Total
represent the same symbolic name and
.id 3F10.2
and
.id 3f10.2
represent the same format edit descriptor.
.xt end
.*
.section Examples
.*
.exam begin
C This and the following five lines are comment lines.
c The following statement "INDEX = INDEX + 2" has a
c statement number and is continued by placing a "$"
c in column 6.
* Column Numbers
*234567890

10    INDEX = INDEX
     $ + 2

* The above blank lines are treated like comment lines.
.exam end
.np
The following example demonstrates the use of comment lines,
blanks lines, and continuation lines.
We use the symbol "$" to denote continuation lines although any
character other than a blank or "0" could have been used.
.keep 23
.exam begin
* From the quadratic equation
*
*       2
*     ax  + bx + c = 0
*
* we derive the following two equations:
*              ____________
*         +   / 2
*      -b - \/ b  - 4ac
*  x = ---------------------
*              2a
*
* and express these equations in FORTRAN as:

      X1 = ( -B + SQRT( B**2 - 4 * A * C ) )
     $   /          ( 2 * A )

      X2 = ( -B - SQRT( B**2 - 4 * A * C ) )
     $   /          ( 2 * A )
.exam end
.endlevel
.*
.section Order of FORTRAN Statements and Lines
.*
.np
The first statement of a program unit may be a
.kw PROGRAM
.ct ,
.kw FUNCTION
.ct ,
.kw SUBROUTINE
.ct , or
.kw BLOCK DATA
statement.
The
.kw PROGRAM
statement identifies the start of a main program
.ix 'main program'
and there may only be one of these in an executable FORTRAN
program.
Execution of a FORTRAN program begins with the first
.us executable
statement in the main program.
The other statements identify the start of a subprogram.
.ix 'subprogram'
If the first statement of a program unit is not one of the above
then the program unit is considered to be a main program.
.np
Although you may not be familiar with all of the terms used here, it is
important that you understand that FORTRAN 77 has specific rules
regarding the ordering of FORTRAN statements.
You may wish to refer to this section at later times.
In general, the following rules apply to the order of statements and
comment lines within a program unit:
.autonote
.note
Comment lines
.xt on
and
.kw INCLUDE
statements
.xt off
may appear anywhere.
.note
.kw FORMAT
statements may appear anywhere in a subprogram.
.note
All specification statements must precede all
.kw DATA
statements,
.us statement function
.ix 'statement function'
statements, and executable statements.
.note
All statement function statements must precede all executable
statements.
.note
.kw DATA
statements may appear anywhere after the specification
statements.
.note
.kw ENTRY
statements may appear anywhere except between a block
.kw IF
statement and its corresponding
.kw END IF
statement, or between
a
.kw DO
statement and its corresponding terminal statement.
.xt on
&product extends these rules to apply to all program structure
blocks resulting from the use of statements introduced to the language
by &product (e.g.,
.kw WHILE
.ct ,
.kw LOOP
.ct ,
.kw SELECT
.ct ).
.xt off
.note
.kw IMPLICIT
statements must precede all other specification statements, except
.kw PARAMETER
statements.
A specification statement that defines the type of a symbolic constant
must appear before the
.kw PARAMETER
statement that defines the name and value of a symbolic constant.
A
.kw PARAMETER
statement that defines the name and value of a symbolic constant
must precede all other statements containing a reference to that
symbolic constant.
.endnote
.np
.ix 'order' 'statement'
.ix 'statement order'
The following chart illustrates the required order of FORTRAN
statements.
Vertical lines delineate varieties of statements that may be
interspersed, while horizontal lines mark varieties of statements that
may not be interspersed.
.if &e'&dohelp eq 0 .do begin
.np
.in 0
.sr c0=&INDlvl+1
.sr c1=&INDlvl+12
.sr c2=&INDlvl+24
.sr c3=&INDlvl+36
.sr c4=&INDlvl+48
.sr t0=&c0+2
.sr t1=&c1+2
.sr t2=&c2+2
.sr t3=&c3+2
.sr t4=&c4+2
.tb set \
.tb &t0 &t1 &t2 &t3 &t4
.*
.keep 25
.bx &c0 &c1 &c4
\\PROGRAM, FUNCTION, SUBROUTINE, or
.br
\\    BLOCK DATA Statement
.bx new &c1 &c2 &c3 &c4
\\\\IMPLICIT
\\\PARAMETER\Statements
.bx new &c2 / &c3 &c4
\Comment\ENTRY\Statements\Other
\Lines\\\Specification
\\and\\Statements
.bx off &c2 &c3 &c4
\\FORMAT\\Statement
\INCLUDE\\\Function
\Statement\Statements\DATA\Statements
.bx new &c2 / &c3 &c4
\\\Statements\Executable
\\\\Statements
.* .bx &c0 &c4
.bx &c1 &c4
\\        END\Statement
.bx can
.bx can
.bx can
.bx off &c0 &c1 &c4
.tb
.tb set
.in &INDlvl
.us Required Order of Comment Lines and Statements
.do end
.el .do begin
.millust begin
+-----------+-------------------------------------------+
|           | PROGRAM, FUNCTION, SUBROUTINE, or         |
|           | BLOCK DATA Statement                      |
|           +-----------+---------------+---------------+
|           |           |               | IMPLICIT      |
|           |           |  PARAMETER    | Statements    |
|           |           |               +---------------+
| Comment   | ENTRY     |  Statements   | Other         |
| Lines     |           |               | Specification |
|           | and       |               | Statements    |
|           |           |---------------+---------------+
|           | FORMAT    |               | Statement     |
| INCLUDE   |           |               | Function      |
| Statement | Statements|  DATA         | Statements    |
|           |           |               +---------------+
|           |           |  Statements   | Executable    |
|           |           |               | Statements    |
|           +-----------+---------------+---------------+
|           | END Statement                             |
+-----------+-------------------------------------------+
.millust end
.np
.us Required Order of Comment Lines and Statements
.do end
.np
For example,
.kw DATA
statements may be interspersed with
.us statement function
statements and executable statements but
.us statement function
statements must precede executable statements.
