.chap *refid=fio Input/Output
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
FORTRAN 77 provides a means of communicating information or
.us data
.ix 'data'
between a FORTRAN program and the computing environment.
The computing environment may include a number of devices which are
capable of the recording, retrieval, display, and input of data.
Disk and magnetic tape devices are capable of storing large amounts
of data.
Other devices such as printers and display terminals can be used to
present a visual (i.e., human-readable) representation of the data.
Yet other devices such as terminal keyboards and card-readers make
possible the entry of new data into the computing environment.
.np
For the purposes of our discussion, data is any information which
can be processed by an executing FORTRAN program.
Some examples of data are names, addresses, telephone numbers,
credit card balances, flight trajectories, bus schedules, athletic
records, etc.
In computing, such information is usually well-organized in order to
make it useful for processing.
.np
To use an example, consider the entries in a telephone book.
There are essentially three pieces of data listed for each entry; a
name, an address, and a number.
.millust begin
    Smith J 32 Arthur St--------------------------555-3208
    Smith JW 512 King St--------------------------555-9229
    Smith Jack 255-113 Queen St N-----------------555-0572
.millust end
.pc
Each entry is a
.us record.
The organization of the book is clear.
The name is always listed first, the address second, and the number
last.
The records are
.us sorted,
for our convenience, by name (within each city or geographical
location).
The length of each record is the same.
This
.us fixed length
.ix 'record' 'fixed length'
does sometimes lead to problems since entries which have a long
name or address won't fit in a record.
The phone company solved this by continuing the information in
subsequent records.
We might have solved this problem by increasing the length of a
record with the disadvantage of wasting a lot of printing space.
Alternatively, we could have used a
.us variable length
.ix 'record' 'variable length'
record.
This solves the problem of wasted space but creates a severe problem
when trying to display the records in nice orderly columns.
The telephone book itself is a collection of records or a
.us file.
.ix 'file'
.np
We have introduced much of the terminology of data processing:
"data", "records", "fixed and variable record sizes", "files",
"sorted", etc.
.*
.section Reading and Writing
.*
.np
FORTRAN provides a mechanism called "reading" for transferring
data into the environment of an executing program.
The
.kw READ
statement is used to do this.
Similarly "writing" is the mechanism for transferring data out of
an executing program.
The
.kw WRITE
and
.kw PRINT
statements are used to do this.
Other statements provide additional functions such as positioning to
a certain record in a file, establishing which files are to be
processed by the program, or making inquiries about files.
.*
.section Records
.*
.np
.ix 'record'
FORTRAN distinguishes between three kinds of records, namely:
.autopoint
.point
Formatted
.point
Unformatted
.point
Endfile
.endpoint
.np
We shall describe each of these in the following sections.
.*
.beglevel
.*
.section Formatted Record
.*
.np
.ix 'formatted record'
.ix 'record' 'formatted'
A formatted record consists of characters.
The length of a formatted record is determined by the number of
characters in it.
A formatted record may contain no characters at all and thus has
zero length.
.ix 'input/output' 'formatted'
.ix 'formatted input/output'
Formatted records are read or written using
.us formatted input/output
statements.
An excellent example of a file consisting of formatted records is
our telephone book example.
.*
.section Unformatted Record
.*
.np
.ix 'unformatted record'
.ix 'record' 'unformatted'
An unformatted record consists of values such as integers, real
numbers, complex numbers, etc.
It may also consist of characters.
Essentially, these values have the same representation in a record
as they have in the computer's memory.
The length of an unformatted record depends on the amount of storage
required to represent these values in the computer's memory.
For example, on this computer an integer value is stored using 4
bytes of memory (a byte is a grouping of 8 binary digits).
Thus, integer values in unformatted records also require 4 bytes of
storage.
For example, 3 integer values stored in an unformatted record would
require 12 bytes of storage.
.ix 'input/output' 'unformatted'
.ix 'unformatted input/output'
Unformatted records are read or written using
.us unformatted input/output
statements.
.np
To illustrate the difference between a formatted and unformatted record
consider the following example.
.exam begin
      INTEGER NUMBER
      NUMBER=12345
      PRINT 100, NUMBER
100   FORMAT(1X,I5)
      WRITE( UNIT=7 ) NUMBER
.exam end
.pc
If you print the variable
.id NUMBER
on a printer, it requires 5 character positions.
If you write it to a file using an unformatted
.kw WRITE
statement, it only requires 4 bytes or character positions in the
record.
Note that a character is conveniently represented in one byte of
storage, hence we sometimes use the term "byte" or "character"
interchangeably when talking about the size of variables.
.*
.section Endfile Record
.*
.np
.ix 'endfile record'
.ix 'record' 'endfile'
An endfile record is a special record that follows all other records
in a file.
Simply stated, an endfile record occurs at the end of a file.
Actually, an endfile record is a conceptual thing.
It has no length.
When the end of a file is reached (i.e., an attempt to read a record
results in the endfile record being read), an "end-of-file"
condition exists.
.ix 'end-of-file'
There are no more records following the endfile record.
There is only one endfile record so it is strictly illegal to
attempt to read another record after the endfile record has been
read (i.e., when the end-of-file condition exists).
.endlevel
.*
.section Files
.*
.np
.ix 'file'
Earlier we described the notion of a file as a collection of records.
In FORTRAN, there are two kinds of files:
.autopoint
.point
External
.point
Internal
.endpoint
.*
.beglevel
.*
.section External Files
.*
.np
.ix 'file' 'external'
.ix 'external file'
.ix 'file existence'
External files are files that exist or can be created upon external
media such as disks, printers, terminal displays, etc.
A file may exist before the execution of a FORTRAN program.
It may be brought into existence or "created" during execution.
It may also be deleted and therefore not exist after the execution
of a FORTRAN program.
.np
All input/output statements may refer to files that exist.
In addition, the
.kw INQUIRE
.ct ,
.kw OPEN
.ct ,
.kw CLOSE
.ct ,
.kw WRITE
.ct ,
.kw PRINT
.ct , and
.kw ENDFILE
statements may refer to files that do not exist (and in so doing, may
very well cause the file to be created).
.begnote Properties of External Files
.ix 'external file' 'properties'
.note Name
.ix 'external file' 'name'
In FORTRAN, a file may or may not have a name.
If it does have a name then, not surprisingly, it is called a
.us named
file.
.ix 'file' 'name'
All files in &product have names and so it may seem odd to
introduce this notion.
However, we do since the
.kw INQUIRE
statement lets you find out if a file is named and, if so, what its
name is.
File naming conventions may differ from one computing system to the
next.
As well, different FORTRAN 77 compilers may have different file
naming conventions.
.note Access
.ix 'access'
.ix 'external file' 'access'
"Access" simply refers to the way in which we can position to and read
or write the data in a particular record in a file.
There are two ways in which records can be accessed in a file;
.us sequentially
or
.us directly.
.np
.ix 'access' 'sequential'
.ix 'sequential access'
Using the
.us sequential access
method, records may be read or written in
order starting with the first record and proceeding to the last record.
For example, it would be quite impossible to read or write the tenth
record in a file and then read or write the third record.
Similarly the eleventh record must be read or written before we can
access the twelfth record.
If we adopt the convention that each record in a file has a record
number then the first record is record number 1, the second is 2,
and so on.
This numbering convention is important when we look at the other
access method which is "direct".
.np
.ix 'access' 'direct'
.ix 'direct access'
Using the
.us direct access
method, records may be read or written in any
order.
It is possible to read or write the tenth record of a file and then
the third and then the twelfth and so on.
A caveat: a record cannot be read if it has never been written since
the file was created.
In direct access, the idea of a record number is very important and
so by convention, we number them starting at 1 as the first record
and proceeding on up.
With direct access, if you create a new file and write record number
10 then the file has ten records regardless of the fact that only
one has been written.
You could, at some later time, write records 1 through 9 (in
whatever order you please) and add additional records by writing
records with record numbers greater than 10.
.np
Some files have the property of being able to sustain both of these
access methods.
Some files may only have one of these properties.
For example, most line printers cannot be accessed directly.
You have no choice but to write records sequentially.
Sometimes a file that was created using the sequential access method
may not be accessed using the direct method or vice versa.
FORTRAN calls this property of a file the "set of allowed access
methods".
.note Record Form
.ix 'record' 'form'
.ix 'external file' 'record form'
Some files have the property of being able to handle both formatted
and unformatted record formats.
Some files may only have one of these properties.
For example, if you tried to write unformatted records to a line
printer, the result might be gibberish.
On the other hand a graphics printer may readily accept unformatted
records for reproducing graphical images on paper.
FORTRAN calls this property of a file the "set of allowed forms".
.note Record Length
.ix 'record' 'length'
.ix 'external file' 'record length'
Another property of a file is record length.
Some files may have restrictions on the length of a record.
Some files do not allow records of zero length.
Other files, such as printers, may restrict the length of a record
to some maximum.
FORTRAN calls this property the "set of allowed record lengths".
.endnote
.*
.section Internal Files
.*
.np
.ix 'file' 'internal'
.ix 'internal file'
Internal files are special files that reside only in memory.
They do not exist before or after the execution of a FORTRAN
program, only during the execution of a program.
An internal file allows you to treat memory in the computer as if it
were one or more records in a file.
The file must be a character variable, character array element,
character array, or character substring.
A record in this file may be a character variable, character array
element or character substring.
.np
Another way of looking at this is that an internal file that is either
a character variable, character array element or character substring
can contain only one record but an internal file that is a character
array can contain several records (as many as there are elements in the
array).
.begnote Properties of Internal Files
.ix 'internal file' 'properties'
.note Records
.ix 'internal file' 'records'
Unless the name of a character array is used, only one record is
contained in an internal file.
The length of this record is the same as the length of the variable,
array element, or substring.
If the file is a character array then each element in the array is a
record.
The order of the records in the file is the same as the order of the
elements in the array.
The length of a record in this case is the same as the length of the
character array elements.
.np
If the number of characters written to a record in an internal file
is less than the length of the record then the record is padded with
blanks.
.note Definition
.ix 'internal file' 'definition'
A record may be read only if the variable, array element, or substring
is defined (i.e., it has been assigned some value).
Definition may not only result from an output statement such as
.kw WRITE
.ct .li .
It may also be defined through other means; for example, a character
assignment statement.
.note Position
.ix 'internal file' 'position'
For all input/output statements, the file is positioned at the
beginning of the first record.
Multiple records may be read or written using the "slash" format edit
descriptor (see the chapter entitled :HDREF refid='fformat'.).
.note Restrictions
.ix 'internal file' 'restrictions'
Only sequential access formatted input and output statements (
.ct
.kw READ
and
.kw WRITE
.ct ) may be used to transfer data to and from records in an
internal file.
.np
.xt begin
.ix 'list-directed'
Although FORTRAN 77 states that list-directed formatted input/output
to an internal file is not permitted, &product allows you to use
list-directed formatted input/output statements.
This is an extension to the language standard.
.exam begin
      WRITE(INTFIL,*) X, Y, Z
.exam end
.xt end
.np
No other input/output statements (
.ct
.kw OPEN
.ct ,
.kw ENDFILE
.ct ,
.kw REWIND
.ct , etc.) may be used.
.endnote
.np
Internal files may be used to convert data from one format to another.
The following example illustrates one use of internal files.
.cp 15
.exam begin
      CHARACTER*11 INPUT
      PRINT *, 'TYPE IN ''I'' FOLLOWED BY AN INTEGER'
      PRINT *, 'OR TYPE IN ''R'' FOLLOWED BY A REAL'
      READ 100, INPUT
100   FORMAT( A11 )
      IF( INPUT(1:1) .EQ. 'I' )THEN
          READ( UNIT=INPUT(2:11), FMT='(I10)' ) IVAR
          PRINT *, 'AN INTEGER WAS ENTERED ', IVAR
      ELSE IF( INPUT(1:1) .EQ. 'R' )THEN
          READ( UNIT=INPUT(2:11), FMT='(F10.3)' ) RVAR
          PRINT *, 'A REAL NUMBER WAS ENTERED ', RVAR
      END IF
      END
.exam end
.pc
After checking for an
.mono "I"
or
.mono "R"
as the first character of the character variable
.id INPUT,
the appropriate internal
.kw READ
statement is executed.
.endlevel
.*
.section *refid=units Units
.*
.np
Many FORTRAN 77 input/output statements refer to external files using a
mechanism called the
.us unit.
.ix 'unit'
There are many units available to the FORTRAN 77 programmer.
&product numbers these units from 0 to 999;
thus the unit number is a non-negative integer less than 1000.
.np
A unit may be associated with a particular file.
This association is called
.us connection.
.ix 'connection' 'unit'
.ix 'connection' 'file'
Any unit may or may not be connected to a file.
There are a number of ways in which this connection may be
established.
.np
A unit may be
.us preconnected
.ix 'preconnection'
to a file before execution of a program begins.
The User's Guide describes the mechanism for preconnecting
a unit to a file.
.np
Alternatively, a unit may become connected to a file by the
execution of an
.kw OPEN
statement.
.np
All input/output statements except
.kw OPEN
.ct ,
.kw CLOSE
.ct , and
.kw INQUIRE
must refer to a unit that is connected to a file.
&product automatically establishes a connection of the unit to a
file if no connection previously existed.
Consider the following example in which unit number 1 is not
previously connected to a file.
.exam begin
      WRITE(1,*) 'Output on unit 1'
      END
.exam end
.pc
&product constructs a file name using the specified unit number.
The format of the file name is described in the User's Guide
since it varies from one computer system to the next.
.np
Connection of a unit to a file does not imply that the file must
exist.
For example, it could be a new file.
When we speak of a unit being connected to a file, we can also say
that a file is connected to a unit.
Under the rules of FORTRAN, it is illegal to connect the same file
to more than one unit
.us at the same time.
However, a file may be connected to different units at different
times.
We shall explain how this is possible.
.np
.ix 'disconnection'
A file may be
.us disconnected
from a unit by the use of the
.kw CLOSE
statement.
.exam begin
      CLOSE(UNIT=1)
.exam end
.np
Under certain circumstances, the file may be disconnected from a unit
by the use of the
.kw OPEN
statement.
.exam begin
      OPEN(UNIT=1,FILE='FILE1')
      .
      .
      .
      OPEN(UNIT=1,FILE='FILE2')
.exam end
.pc
In the above example, the second
.kw OPEN
statement disconnects unit 1 from one file and connects it to a
second file.
You may think of the second
.kw OPEN
statement as automatically closing the first file and then
establishing a connection to the second file.
.np
If a unit has been disconnected from a file through the execution of
a
.kw CLOSE
statement, the unit may subsequently be connected to the same
file or to a different file.
It also follows that a file which has been disconnected from one
unit number may be connected to the same unit number or a
different unit number.
The following example may help to illustrate this last point.
.exam begin
      OPEN(UNIT=1,FILE='FILE1')
      .
      .
      .
      CLOSE(UNIT=1)
      OPEN(UNIT=2,FILE='FILE1')
.exam end
.np
Once a file has been disconnected, the only means for referring to
the file is by its name in an
.kw OPEN
statement or an
.kw INQUIRE
statement.
.*
.section Specifiers
.*
.np
All input/output statements contain one or more specifiers.
They appear in a list separated by commas.
Some of the more common specifiers are those listed below.
Not all of them need be used in every input/output statement.
You should consult the description of the input/output statement
under consideration to discover which specifiers are allowed and
what they mean.
.begnote $setptnt 16
.ix 'UNIT='
.mnote [UNIT =] u
the unit specifier
.ix 'FMT='
.mnote [FMT =] f
the format specifier
.ix 'REC='
.mnote REC = rn
the record specifier
.ix 'IOSTAT='
.mnote IOSTAT = ios
the input/output status specifier
.ix 'ERR='
.mnote ERR = s
the error specifier
.ix 'END='
.mnote END = s
the end-of-file specifier
.endnote
.np
We shall look at these specifiers in more detail.
.*
.beglevel
.*
.section The Unit Specifier
.*
.np
.ix 'unit specifier'
.ix 'specifier' 'unit'
.np
The form of a unit specifier in an input/output statement is:
.begnote
.ix 'UNIT='
.mnote [UNIT =] u
.id u
is an
.us external unit identifier
or an
.us internal file identifier.
.autopoint
.point
An external unit identifier is a non-negative integer
expression or an asterisk
.mono (*)
in which case unit 5 is assumed for an input statement
and unit 6 is assumed for an output statement.
The unit identifier must not be an asterisk for the
.kw BACKSPACE
.ct ,
.kw ENDFILE
and
.kw REWIND
statements.
.point
An internal file identifier is the name of a character variable,
character array, character array element, or character substring.
.endpoint
.endnote
.np
If the optional
.kw UNIT=
specifier is omitted then the unit specifier must be the first
item in the list of specifiers.
.*
.section Format Specifier
.*
.np
.ix 'format specifier'
.ix 'specifier' 'format'
.np
The form of a format specifier in an input/output statement is:
.begnote
.ix 'FMT='
.mnote [FMT =] f
.id f
is a
.us format identifier.
A format identifier is one of the following:
.autopoint
.point
A statement label of a
.kw FORMAT
statement that appears in the same program unit as the format
identifier.
.point
An integer variable name that has been assigned the statement label of
a
.kw FORMAT
statement that appears in the same program unit as the format
identifier (see the
.kw ASSIGN
statement).
.point
.xt on
An integer array name.
.xt off
.point
A character array name.
.point
Any character expression except one involving the concatenation of an
operand whose length specification is
.mono (*)
unless the operand is a symbolic constant (see the
.kw PARAMETER
statement).
.point
.ix 'list-directed format'
.ix 'format' 'list-directed'
An asterisk
.mono (*),
indicating
.us list-directed
formatting.
.point
.xt on
.ix 'namelist-directed format'
.ix 'format' 'namelist-directed'
A
.kw NAMELIST
name, indicating
.us namelist-directed
formatting.
.xt off
.endpoint
.endnote
.np
If the optional
.kw FMT=
specifier is omitted then the format specifier must be the second item
in the list of specifiers and
.kw UNIT=
must not be specified for the first item in the list.
.*
.section Record Specifier
.*
.np
.ix 'record specifier'
.ix 'specifier' 'record'
.np
The form of a record specifier in an input/output statement is:
.begnote
.ix 'REC='
.mnote REC = rn
.id rn
is an integer expression whose value must be positive.
It is the number of the record to be read when a file is
connected for
.us direct access.
.ix 'direct access'
.ix 'access' 'direct'
.endnote
.*
.section Input/Output Status Specifier
.*
.np
.ix 'status specifier'
.ix 'specifier' 'status'
.np
The form of an input/output status specifier in an input/output
statement is:
.begnote
.ix 'IOSTAT='
.mnote IOSTAT = ios
.id ios
is an integer variable or integer array element.
It is defined with zero if no error occurs, a positive integer value
if an error occurs, or a negative integer value if an end-of-file
occurs.
.endnote
.np
If an input/output error or end-of-file condition occurs during the
execution of an input/output statement and the input/output status
specifier is present then execution of the program is not
terminated.
Input/output errors may result from a violation of the rules of
FORTRAN or from a file system error.
For example, a negative unit number will result in an error since
this is a violation of the rules of FORTRAN.
An example of a file system error might be an attempt to create a
file on a non-existent file storage device.
.np
Consult the User's Guide for a list of &product diagnostic messages.
An input/output status of
.mono nn
corresponds to the message
.mono IO-nn.
For example, if the status returned is 3 then the error is:
.millust begin
IO-03  ENDFILE statement requires sequential access mode
.millust end
.*
.section Error Specifier
.*
.np
.ix 'error specifier'
.ix 'specifier' 'error'
The form of an error specifier in an input/output statement is:
.begnote
.ix 'ERR='
.mnote ERR = s
.id s
is a statement label.
When an error occurs, execution is transferred to the statement
labelled by
.id s.
.endnote
.np
If an input/output error occurs during the execution of an input/output
statement and the
.kw ERR=
specifier is present then execution of the program is not
terminated.
.*
.section End-of-File Specifier
.*
.np
.ix 'end-of-file specifier'
.ix 'specifier' 'end-of-file'
The form of an end-of-file specifier in an input/output statement is:
.begnote
.ix 'END='
.mnote END = s
.id s
is a statement label.
When an end-of-file condition occurs, execution is transferred to
the statement labelled by
.id s.
.endnote
.np
If an end-of-file condition occurs during the execution of an
input/output statement and the
.kw END=
specifier is present then execution of the program is not
terminated.
.endlevel
.*
.section Printing of Formatted Records
.*
.np
.ix 'printing'
Printing occurs when
.us formatted
records are transferred to a device which interprets the first
character of the record as a special spacing command.
The remaining characters in the record are "printed".
Printing can be accomplished by use of either the
.kw PRINT
statement or the
.kw WRITE
statement.
What actually determines whether or not you are "printing" is the
device (or file) to which records are transferred.
.np
.ix 'ASA'
.ix 'carriage control'
The first character of the record controls the vertical spacing.
This feature is quite often called ASA (American Standards
Association) carriage control.
.sk 1 c
.* .mbox on 1 13 45
.sr c0=&INDlvl+1
.sr c1=&INDlvl+13
.sr c2=&INDlvl+45
.mbox on &c0 &c1 &c2
Character    Vertical Spacing Before Printing
.mbox
Blank           One Line
0               Two Lines
-               Three Lines
1               To First Line of Next Page
+               No Advance
.mbox off
.np
.xt begin
The "&minus." control character is an extension to the FORTRAN 77 language
that is supported by many "printing" devices.
.xt end
