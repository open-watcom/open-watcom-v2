.chap *refid=fdosfil &product File Handling
.*
.np
.ix 'file handling'
.ix 'file naming'
This chapter describes the file handling and naming conventions of
&cmpname..
We discuss
.us files
and
.us devices
which are used to store, retrieve and display data.
For example, a disk can be used to store a file of student marks.
This file is accessible by other programs in order to produce
summaries of the data such as marks reports.
A device such as a printer can also be treated as if it were a file,
although it is only useful for displaying data; an attempt to read
information from this device is invalid.
.np
In the following sections, we shall describe:
.autopoint
.point
the techniques that &cmpname adopts for implementing
.us FORMATTED
and
.us UNFORMATTED
records and
.us SEQUENTIAL
and
.us DIRECT
access to these records,
.point
the handling of "print" files,
.point
file naming conventions,
.point
logical file names,
.point
the preconnection of files to units, and
.point
special device support.
.endpoint
.*
.section Record Access
.*
.np
.ix 'record access'
Two types of record access are supported by &cmpname:
.begpoint
.point Sequential
Sequential access means that records in a file are accessed in order,
starting with the first record in the file and proceeding to the
last.
Sequential access is permitted to records in both variable-length and
fixed-length record files.
.point Direct
Direct access means that records in a file are accessed in random
order.
For example, the fifth record could be accessed, then the second,
and then the tenth.
Direct access is permitted for fixed-length record files only.
.endpoint
.np
The access method is described using the
.kw ACCESS=
specifier of the FORTRAN
.kw OPEN
statement.
The default access is "SEQUENTIAL".
.*
.section Record Format
.*
.np
.ix 'record format'
.ix 'formatted record'
.ix 'unformatted record'
.ix 'formatted record'
.ix 'record' 'unformatted'
There are two record formats, "FORMATTED" and "UNFORMATTED", which
are supported by &cmpname..
The record format is described using the
.kw FORM=
specifier of the FORTRAN
.kw OPEN
statement.
The default format is "FORMATTED" for files connected for sequential
access and "UNFORMATTED" for files connected for direct access.
.np
In describing these two formats, we also refer to the two methods
of record access, "SEQUENTIAL" and "DIRECT", which are supported
by &cmpname..
.beglevel
.*
.section FORMATTED Records
.*
.np
.ix 'formatted record'
.ix 'record' 'formatted'
A
.us FORMATTED
record is one that contains an arbitrary number of ASCII characters.
The end of a record is marked by an ASCII "LF" (line feed) character
optionally preceded by an ASCII "CR" (carriage return) character.
Thus this special sequence may not appear in the middle of a record.
.np
FORMATTED records may vary in length.
If all the records in the file have the same length then the records
may be accessed both "sequentially" and "directly".
If the records vary in length then it is only possible to access the
records sequentially.
.np
For direct access, the length of the records is specified by the
.kw RECL=
specifier of the FORTRAN
.kw OPEN
statement.
The specified length must not include the record separator since
it does not form part of the record.
.np
As an extension to the FORTRAN 77 language standard, &cmpname also
supports the use of the
.kw RECL=
specifier for sequential access.
The maximum length of the records may be specified by the
.kw RECL=
specifier of the FORTRAN
.kw OPEN
statement.
The specified length must not include the record separator since
it does not form part of the record.
The length is used to allocate a record buffer for sequential access.
If the record length is not specified, a default maximum length of 1024
characters is assumed.
.*
.section UNFORMATTED Records
.*
.np
.ix 'unformatted record'
.ix 'record' 'unformatted'
An
.us UNFORMATTED
record is one that contains an arbitrary number of binary storage
units.
The interpretation of the data in such a record depends on the FORTRAN
program that is processing the record.
An UNFORMATTED record may contain integers, real numbers, character
strings, or any other type of FORTRAN data.
.np
UNFORMATTED records may also vary in length.
If all records in the file have the same length then the records may
be accessed both "sequentially" and "directly".
If the records vary in length then it is only possible to access the
records sequentially.
.np
When a file containing UNFORMATTED records is accessed sequentially,
each record must begin and end with a descriptor that contains the
length of the record.
The length of the record is represented in 32 bits or 4 bytes
(INTEGER*4).
The UNFORMATTED records of a file which are written using sequential
access will be automatically supplied with the appropriate length
descriptors.
When such a file is read, it is assumed that each record has the
appropriate length descriptors.
.np
Depending on the record length, the output produced by a single
unformatted sequential
.kw WRITE
statement may cause multiple records to be written.
As previously mentioned, each record begins and ends with a length
descriptor.
The length descriptors for the first record contain the length of the
record.
The length descriptors for the remaining records contain the length of
the record with the high bit (bit 31) set to one.
In this way, an unformatted sequential file can be viewed as a number
of logical records (a logical record corresponding to the output
produced by a
.kw WRITE
statement) with each logical record composed of a number of physical
records.
Files created in this way cannot be accessed directly unless each
logical record is composed of a single physical record and each record
is the same size.
.np
As an extension to the FORTRAN 77 language standard, &cmpname also
supports the use of the
.kw RECL=
specifier for sequential access.
The maximum length of the records may be specified by the
.kw RECL=
specifier of the FORTRAN
.kw OPEN
statement.
The specified length must not include the length descriptors since
they do not form part of the record.
The length is used to allocate a record buffer for sequential access.
If the record length is not specified, a default maximum length of 1024
characters is assumed.
.np
When a file containing UNFORMATTED records is accessed directly, each
record must be the same length.
In this case, the length of the records is specified by the
.kw RECL=
specifier of the FORTRAN
.kw OPEN
statement.
If the file was originally created with sequential access then the
specified length must include any length descriptors which form part
of the record.
In the direct access mode, no interpretation is placed on any of the
data in an UNFORMATTED record and the programmer must account for any
record length descriptors which may form part of the record.
.np
Any records which are written using direct access must include record
length descriptors if the file is to be accessed sequentially at a
later time.
As an alternative, you may specify
.id RECORDTYPE='VARIABLE'
in the FORTRAN
.kw OPEN
statement.
This specifier is an extension to the FORTRAN 77 language standard
and will cause length descriptors to be generated automatically.
In this case, the record length should not include the record length
descriptors.
.*
.section Files with no Record Structure
.*
.np
Certain files, for example a file created by a program written in
another language, do not have any internal record structure that
matches any of the record structures supported by &cmpname..
These files are simply streams of data.
There are two ways in which these files can be processed.
.autonote
.note
You can use unformatted direct access.
In this case, the value specified by the
.kw RECL=
specifier in the
.kw OPEN
statement determines the amount of data read or written by a
.kw READ
or
.kw WRITE
statement.
.note
Alternatively, you can use unformatted sequential access.
In this case, the amount of data read or written to the file is determined
by the items in the input/output list of the
.kw READ
or
.kw WRITE
statement.
When using unformatted sequential access, you must specify
.mono RECORDTYPE='FIXED'
to indicate that no record boundaries are present.
Otherwise, the default value of
.mono 'VARIABLE'
will be used.
.endnote
.endlevel
.*
.section Attributes of Files
.*
.np
The file system does not retain any information on the
contents of a file.
Unlike more sophisticated file systems, it cannot report whether a
file consists of fixed-length or variable-length records, how records
are delimited in a file, the maximum length of the records, etc.
Therefore, we have provided a mechanism which will allow you to
specify additional information about a file.
This mechanism should be used when the default assumptions about
records in a file are not true for the file in question.
.np
The
.kw RECORDTYPE=
specifier of the FORTRAN
.kw OPEN
statement can be used to specify additional information about the
type of records in the file.
This specifier is an extension to the FORTRAN 77 language standard.
.np
The
.kw RECL=
specifier of the FORTRAN
.kw OPEN
statement can be used to specify additional information about the
length of records in the file.
When used with sequential access, this specifier is an extension to
the FORTRAN 77 language standard.
.np
The
.kw CARRIAGECONTROL=
specifier of the FORTRAN
.kw OPEN
statement can be used to specify additional information about the
handling of ASA carriage control characters for an output file.
This specifier is an extension to the FORTRAN 77 language standard.
.np
The
.kw BLOCKSIZE=
specifier of the FORTRAN
.kw OPEN
statement can be used to specify the size of the internal input/output
buffer.
A buffer reduces the number of system input/output calls during
input/output to a particular file and hence improves the overall
performance of a program.
The default buffer size is 4K.
This specifier is an extension to the FORTRAN 77 language standard.
.np
The following sections describe the attributes of records supported by
the &cmpname run-time system.
.beglevel
.*
.section Record Type
.*
.np
.ix 'record type'
The
.kw RECORDTYPE=
specifier of the FORTRAN
.kw OPEN
statement can be used to specify additional information about the
type of records in the file.
This specifier is an extension to the FORTRAN 77 language standard.
The following types may be specified.
.millust begin
RECORDTYPE='TEXT'
RECORDTYPE='VARIABLE'
RECORDTYPE='FIXED'
.millust end
.begnote
.*========
.note TEXT
.*========
.ix 'RECORDTYPE' 'TEXT'
.ix 'TEXT record type'
indicates that the file contains variable-length or fixed-length
records of ASCII characters separated by an ASCII "LF" (line feed) character
optionally preceded with an ASCII "CR" (carriage return) character.
By default, the &cmpname run-time system assumes that
.kw FORMATTED
records are of
.kw TEXT
format in both the
.us sequential
and
.us direct
access modes.
.np
By default, the &cmpname run-time system uses variable-length record
.kw TEXT
files to implement
.kw FORMATTED
records in the
.us sequential
access mode.
Of course, all records may be the same length.
The record separator is not included in calculating
the maximum size of records in the file.
.np
By default, the &cmpname run-time system uses fixed-length record
.kw TEXT
files to implement
.kw FORMATTED
records in the
.us direct
access mode.
Each record must be the same length.
The record separator is not included in calculating
the size of records in the file.
.*============
.note VARIABLE
.*============
.ix 'RECORDTYPE' 'VARIABLE'
.ix 'VARIABLE record type'
indicates that the file contains variable-length or fixed-length
records in which special descriptors are employed to describe the length of
each record.
The length of each record is contained in a doubleword (INTEGER*4
item) at the beginning and end of the record.
These descriptors determine the bounds of the records.
.np
By default, the &cmpname run-time system uses
.kw VARIABLE
format files to implement
.kw UNFORMATTED
records in the
.us sequential
access mode.
The length descriptors are required to support the FORTRAN
.kw BACKSPACE
statement since no other method exists for determining the bounds of a
variable-length unformatted record in a file.
.*=========
.note FIXED
.*=========
.ix 'RECORDTYPE' 'FIXED'
.ix 'FIXED record type'
indicates that the file contains no extra information that determines
the record structure.
If the file is a direct access file, the value specified by the
.kw RECL=
specifier determines the size of each record in the file.
.np
By default, the &cmpname run-time system uses
.kw FIXED
format files to implement
.kw UNFORMATTED
records in the
.us direct
access mode.
.np
If you specify
.kw FIXED
with an unformatted sequential file, the size of the records is
determined by the items in the input/output list.
.endnote
.*
.section Record Size
.*
.np
.ix 'record size'
When access is
.us direct,
the record length must be specified in the
.kw RECL=
specifier of the FORTRAN
.kw OPEN
statement.
.code begin
OPEN( UNIT=1, FILE='TEST.DAT', ACCESS='DIRECT', RECL=size, ... )
.code end
.np
As an extension to the FORTRAN 77 language standard,
the record length may also be specified when the access is
.us sequential.
This should be done whenever access is "sequential" and the maximum
record length is greater than the default.
.code begin
OPEN( UNIT=1, FILE='TEST.DAT', ACCESS='SEQUENTIAL', RECL=size, ... )
.code end
.np
The record length specified by
.us size
should not include record separators such as CR and LF, nor should it
include record length descriptors when sequentially accessing a file
containing unformatted records.
However, for all files, records longer than the size specified will be
truncated.
The default record size is 1024.
The maximum record size is 65535 for the 16-bit run-time system.
Since record buffers are allocated in the dynamic storage region, the
size will be restricted to the amount of dynamic storage available.
.*
.section *refid=fdospfa Print File Attributes
.*
.np
.ix 'print file'
When the first character of each record written to a file will contain
an ASA (American Standards Association) carriage control character,
the
.kw CARRIAGECONTROL=
specifier of the FORTRAN
.kw OPEN
statement should be used.
This specifier is an extension to the FORTRAN 77 language standard.
The ASA character is used for vertical spacing control.
The valid characters and their interpretation are:
.begnote
.note "1"
Advance to Top of Page
.note "+"
Advance Zero Lines (overprint)
.note " "
Advance 1 Line
.note "0"
Advance 2 Lines
.note "-"
Advance 3 Lines
.endnote
.np
If
.mono CARRIAGECONTROL='YES'
is specified then the &cmpname run-time system will automatically
allocate an extra character at the beginning of a record for the
vertical spacing control.
.np
Upon transmitting a record to a file which has the "carriage"
attribute, the &cmpname run-time system will substitute the
appropriate ASCII carriage control characters as follows.
.begnote
.note "1"
Substitute a FF (form feed) for the "1".
.note "+"
Append only a CR (carriage return ) to the previous record.
.note "~b"
Throw away the blank character.
.note "0"
Substitute CR (carriage return) and LF (line feed) for the "0".
.note "-"
Substitute two pairs of CR and LF for the "-".
.np
Any other character in this position will be treated as if a blank
character had been found (i.e., it will be discarded).
.endnote
.np
If the "carriage" attribute is not specified for a file then records
will be written to the file without placing any interpretation on the
first character position of the record.
.*
.section Input/Output Buffer Size
.*
.np
.ix 'buffer size'
The
.kw BLOCKSIZE=
specifier is optional.
However if you would like to change the default buffer size of
16K for 32-bit applications and
4K for 16-bit applications,
you must specify the buffer size in the
.kw BLOCKSIZE=
specifier of the
.kw OPEN
statement.
.code begin
OPEN( UNIT=1, FILE='TEST.DAT', BLOCKSIZE=1024, ... )
.code end
.*
.section File Sharing
.*
.np
.ix 'file sharing'
On systems that support multi-tasking or networking, it is possible for
for a file to be accessed simultaneously by more that one process.
There are two specifiers in the
.KW OPEN
statement that can be used to control the way in which files are shared
between processes.
.np
The
.kw ACTION=
specifier indicates the way in which the file is initially accessed.
That is, the way in which the first process to open the file accesses the
file.
The values allowed for the
.kw ACTION=
specifier are the following.
.begnote
.note 'READ'
the file is opened for read-only access
.note 'WRITE'
the file is opened for write-only access
.note 'READWRITE'
the file is opened for both read and write access
.endnote
.np
The
.kw SHARE=
specifier can be used to indicate the manner in which subsequent processes
are allowed to access the file while the file is open.
The values allowed for the
.kw SHARE=
specifier are the following.
.begnote
.note 'COMPAT'
.ix 'COMPAT'
no other process may open the file
.note 'DENYRW'
.ix 'DENYRW'
other processes are denied read and write access
.note 'DENYWR'
.ix 'DENYWR'
other process are denied write access (allowed read-only access)
.note 'DENYRD'
.ix 'DENYRD'
other process are denied read access (allowed write-only access)
.note 'DENYNONE'
.ix 'DENYNONE'
other processes are allowed read and write access
.endnote
.np
Let us consider the following scenario.
Suppose you want several processes to read a file and prevent any process
that is reading the file from changing its contents.
We first must establish the method of access for the first process that
opens the file.
In this case, we want read-only access so the
.mono ACTION='READ'
specifier must be used.
Next, we must establish the method of access for subsequent processes.
In our example, we do not want any process to make changes to the file.
Therefore, we use the
.mono SHARE='DENYWR'
specifier.
The file would be opened using the following
.kw OPEN
statement.
.code begin
OPEN( UNIT=1, FILE='TEST.DAT', ACTION='READ', SHARE='DENYWR', ... )
.code end
.endlevel
.*
.section File Names in the FAT File System
.*
.np
.ix 'FAT file system'
.ix 'HPFS file system'
The FAT file system is supported by DOS and OS/2.
OS/2 also supports the High Performance File System (HPFS) which will
be discussed in a later section.
.ix 'file designation'
File naming conventions are used to form file designations in a given
file system.
The file designation for a FAT file system has the following form.
.millust begin
[d:][path]filename[.ext]
.millust end
.begnote
.note []
The square brackets denote items which are optional.
.ix 'drive name'
.note d:
is the
.us drive name.
If omitted, the default drive is assumed.
.np
Examples of drive names are: :FNAME.&dr1.:eFNAME., :FNAME.&dr2.:eFNAME.,
:FNAME.&dr3.:eFNAME., and :FNAME.&dr4.:eFNAME..
.ix 'path'
.note path
is called a "path" specification.
The path may be used to refer to files that are stored in
sub-directories of the disk.
The complete file specification (including drive, path and file name)
cannot exceed 143 characters.
.np
Some examples of path specifications are:
.millust begin
&pc.plot&pc.
&pc.bench&pc.tools&pc.
&pc.fortran&pc.pgms&pc.
.millust end
.pc
Your operating system manuals can tell you more about directories:
how to create them, how to store files in them, how to specify a path,
etc.
.ix 'filename'
.note filename
is the main part of the file's name.
The filename can contain up to 8 characters.
If more than 8 characters are used, only the first 8 are meaningful.
For example, "COUNTRIES" and "COUNTRIE" are treated as the same name
for a file.
.ix 'extension'
.note ext
is an optional
.us extension
consisting of 1 to 3 characters (e.g., DOC).
If an extension is specified, it is separated from the filename by a
period.
Extensions are normally used to indicate the type of information
stored in the file.
For example, a file extension of :FNAME.for:eFNAME. is a common convention
for FORTRAN programs.
.endnote
.remark
.ix 'file name' 'case sensitivity'
The file specification is case insensitive in that upper and lower
case letters can be used interchangeably.
.eremark
.*
.beglevel
.*
.section Special DOS Device Names
.*
.np
.ix 'device names'
Certain file names are reserved for devices.
These special device names are:
.ix 'CON'
.ix 'AUX'
.ix 'COM1'
.ix 'COM2'
.ix 'PRN'
.ix 'LPT1'
.ix 'LPT2'
.ix 'LPT3'
.ix 'NUL'
.ix 'device' 'CON'
.ix 'device' 'AUX'
.ix 'device' 'COM1'
.ix 'device' 'COM2'
.ix 'device' 'PRN'
.ix 'device' 'LPT1'
.ix 'device' 'LPT2'
.ix 'device' 'LPT3'
.ix 'device' 'NUL'
.millust begin
CON   the console (or terminal)
AUX   the serial port
COM1  another name for the serial port
COM2  a second serial port
PRN   the parallel printer
LPT1  another name for the printer
LPT2  a second parallel printer
LPT3  a third parallel printer
NUL   nonexistent device
.millust end
.pc
When using one of these special device names, no other part of the
file designation should be specified.
A common mistake is to attempt to create a disk file such as
.fi PRN.DAT
and attempt to write records to it.
If you do not have a parallel printer attached to your PC, there may
be a long delay before the output operation times out.
.*
.section Examples of FAT File Specifications
.*
.np
The following are some examples of valid file specifications.
.autonote
.note
The following file designation refers to a file in the current
directory of the default disk.
.code begin
OPEN( UNIT=1, FILE='DATA.FIL', ... )
.code end
.note
The following file designation refers to a print file in the current
directory of drive :FNAME.&dr3.:eFNAME..
ASA carriage control characters will be converted to the appropriate
ASCII control codes.
.code begin
OPEN( UNIT=2, FILE=':FNAME.&dr3.report&lst.:eFNAME.',
      CARRIAGECONTROL='YES', ... )
.code end
.note
The file specification below indicates that the file is to have
fixed format records of length 80.
.code begin
OPEN( UNIT=3, FILE=':FNAME.final.tst:eFNAME.',
      RECL=80, RECORDTYPE='FIXED', ... )
.code end
.note
The file specification below indicates that the file is to have
variable format records of maximum length 145.
.code begin
OPEN( UNIT=4, FILE=':FNAME.term.rpt:eFNAME.',
      RECL=145, RECORDTYPE='VARIABLE', ... )
.code end
.note
The file designation below indicates that the file resides in the
:FNAME.records:eFNAME. directory of drive :FNAME.&dr2.:eFNAME..
.code begin
OPEN( UNIT=5, FILE=':FNAME.&dr2.&pc.records&pc.customers.dat:eFNAME.', ... )
.code end
.pc
Note that the trailing "S" in the file name will be ignored.
Thus the following designation is equivalent.
.code begin
OPEN( UNIT=5, FILE=':FNAME.&dr2.&pc.records&pc.customer.dat:eFNAME.', ... )
.code end
.note
The file designation below refers to the second serial port.
.code begin
OPEN( UNIT=6, FILE=':FNAME.com2:eFNAME.', ... )
.code end
.note
The file designation below refers to a second parallel printer.
.code begin
OPEN( UNIT=7, FILE=':FNAME.lpt2:eFNAME.', ... )
.code end
.endnote
.*
.endlevel
.*
.section File Names in the High Performance File System
.*
.np
.ix 'HPFS file system'
OS/2, in addition to supporting the FAT file system, also supports the
High Performance File System (HPFS).
The rules for forming file names in the High Performance File System
are different from those used to form file names in the FAT file
system.
In HPFS, file names and directory names can be up to 254 characters in
length.
However, the complete path (including drive, directories and file
name) cannot exceed 259 characters.
The period is a valid file name character and can appear in a file
name or directory name as many times as required; HPFS file names do
not require file extensions as in the FAT file system.
However, many applications still use the period to denote file
extensions.
.np
The HPFS preserves case in file names only in directory listings but
ignores case in file searches and other system operations.
For example, a directory cannot have more than one file whose names
differ only in case.
.*
.beglevel
.*
.section Special OS/2 Device Names
.*
.np
.ix 'device names'
The OS/2 operating system has reserved certain file names for
character devices.
These special device names are:
.ix 'CLOCK$'
.ix 'COM1'
.ix 'COM2'
.ix 'COM3'
.ix 'COM4'
.ix 'CON'
.ix 'KBD$'
.ix 'LPT1'
.ix 'LPT2'
.ix 'LPT3'
.ix 'MOUSE$'
.ix 'NUL'
.ix 'POINTER$'
.ix 'PRN'
.ix 'SCREEN$'
.ix 'device' 'CLOCK$'
.ix 'device' 'COM1'
.ix 'device' 'COM2'
.ix 'device' 'COM3'
.ix 'device' 'COM4'
.ix 'device' 'CON'
.ix 'device' 'KBD$'
.ix 'device' 'LPT1'
.ix 'device' 'LPT2'
.ix 'device' 'LPT3'
.ix 'device' 'MOUSE$'
.ix 'device' 'NUL'
.ix 'device' 'POINTER$'
.ix 'device' 'PRN'
.ix 'device' 'SCREEN$'
.millust begin
CLOCK$          Clock
COM1            First serial port
COM2            Second serial port
COM3            Third serial port
COM4            Fourth serial port
CON             Console keyboard and screen
KBD$            Keyboard
LPT1            First parallel printer
LPT2            Second parallel printer
LPT3            Third parallel printer
MOUSE$          Mouse
NUL             Nonexistent (dummy) device
POINTER$        Pointer draw device (mouse screen support)
PRN             The default printer, usually LPT1
SCREEN$         Screen
.millust end
.pc
When using one of these special device names, no other part of the
file designation should be specified.
.*
.section Examples of HPFS File Specifications
.*
.np
The following are some examples of valid file specifications.
.autonote
.note
The following file designation refers to a file in the current
directory of the default disk.
.code begin
OPEN( UNIT=1, FILE='DATA.FIL', ... )
.code end
.note
The following file designation refers to a print file in the current
directory of drive :FNAME.&dr3.:eFNAME..
ASA carriage control characters will be converted to the appropriate
ASCII control codes.
.code begin
OPEN( UNIT=2, FILE=':FNAME.&dr3.report&lst.:eFNAME.',
      CARRIAGECONTROL='YES', ... )
.code end
.note
The file specification below indicates that the file is to have
fixed format records of length 80.
.code begin
OPEN( UNIT=3, FILE=':FNAME.final.tst:eFNAME.',
      RECL=80, RECORDTYPE='FIXED', ... )
.code end
.note
The file specification below indicates that the file is to have
variable format records of maximum length 145.
.code begin
OPEN( UNIT=4, FILE=':FNAME.term.rpt:eFNAME.',
      RECL=145, RECORDTYPE='VARIABLE', ... )
.code end
.note
The file designation below indicates that the file resides in the
:FNAME.records:eFNAME. directory of drive :FNAME.&dr2.:eFNAME..
.code begin
OPEN( UNIT=5, FILE=':FNAME.&dr2.&pc.records&pc.customers.dat:eFNAME.', ... )
.code end
.pc
Note that the trailing "S" in the file name is not ignored as is the case in
a FAT file system.
.note
The file designation below refers to the second serial port.
.code begin
OPEN( UNIT=6, FILE=':FNAME.com2:eFNAME.', ... )
.code end
.note
The file designation below refers to a second parallel printer.
.code begin
OPEN( UNIT=7, FILE=':FNAME.lpt2:eFNAME.', ... )
.code end
.endnote
.*
.endlevel
.*
.section Establishing Connections Between Units and Files
.*
.np
.ix 'file connection'
.ix 'unit connection'
Using &cmpname, FORTRAN unit numbers may range from 0 to 999.
Input/output statements such as
.kw READ
and
.kw WRITE
refer to files by a unit number.
All input/output statements except
.kw OPEN
.ct ,
.kw CLOSE
.ct , and
.kw INQUIRE
must refer to a unit that is connected to a file.
The &cmpname run-time system automatically establishes the connection
of a unit to a file if no connection previously existed.
.ix 'preconnection'
Any connection between a unit and a file that is established before
execution begins is called a preconnection.
.np
.ix 'unit *'
The &cmpname run-time system defines a preconnection of the unit
designated by "*" to the standard input and output devices
(by this we generally mean the keyboard and screen of the personal
computer but input/output can be redirected from/to a file using the
standard input/output redirectors "<" and ">" on the command line).
This preconnection cannot be altered in any way.
Unit "*" is explicitly or implicitly referred to by the following
input statements:
.millust begin
READ, ...
READ *, ...
READ format-spec, ...
READ(*,...) ...
READ(UNIT=*,...) ...
.millust end
.np
Unit "*" is explicitly or implicitly referred to by the following
output statements:
.millust begin
PRINT, ...
PRINT *, ...
PRINT format-spec, ...
WRITE(*,...) ...
WRITE(UNIT=*,...) ...
.millust end
.np
.ix 'unit 5'
The &cmpname run-time system also defines a preconnection of unit 5 to
the standard input device (by this we generally mean the keyboard of
the personal computer but input can be redirected from a file using
the standard input redirector "<" on the command line).
.np
.ix 'unit 6'
The &cmpname run-time system also defines a preconnection of unit 6 to
the standard output device (by this we generally mean the screen of
the personal computer but output can be redirected to a file using
the standard output redirector ">" on the command line).
.np
.ix 'file name' 'default'
.ix 'defaults' 'file name'
For all other allowable units, a default preconnection between unit
number "nnn" and the file
.fi FORnnn
is assumed when no connection between a unit and a file has been
established.
.us nnn
is a three-digit FORTRAN unit number.
Unit 0 is "000", unit 1 is "001", unit 2 is "002", and so on.
There is no file extension in this case.
In other words, a default file name is constructed for any unit number
for which no other connection has been established.
Input/output statements of the following forms refer to these units.
.millust begin
CLOSE(nnn,...)           OPEN(nnn,...)
CLOSE(UNIT=nnn,...)      OPEN(UNIT=nnn,...)
BACKSPACE nnn            READ(nnn,...)
BACKSPACE(nnn)           READ(UNIT=nnn,...)
BACKSPACE(UNIT=nnn)      REWIND nnn
ENDFILE nnn              REWIND(nnn)
ENDFILE(nnn)             REWIND(UNIT=nnn)
ENDFILE(UNIT=nnn)        WRITE(nnn,...) ...
INQUIRE(nnn,...)         WRITE(UNIT=nnn,...) ...
INQUIRE(UNIT=nnn,...)
.millust end
.np
Of course, it is unlikely that one would be satisfied with using such
undistinguished file names such as :FNAME.for000:eFNAME.,
:FNAME.for001:eFNAME., and so on.
Therefore, the &cmpname run-time system provides additional ways of
establishing a preconnection between a FORTRAN
.kw UNIT
and a file.
.np
The &cmpname run-time system supports the use of the "SET" command to
establish a connection between a unit and a file.
.ix 'SET command'
.ix 'environment variable'
The "SET" command is used to create, modify and remove "Environment
Variables".
The "SET" command must be issued before running a program.
The format for a preconnection using the "SET" command is:
.millust begin
SET unit=file_spec
.millust end
.synote
.note unit
is a FORTRAN unit number in the range 0 to 999.
.np
If this form of the "SET" command is used then FORTRAN unit number
.sy unit
is preconnected to the specified file.
FORTRAN input/output statements which refer to the unit number will
access the records in the specified file.
.note file_spec
is the file specification of the preconnected file.
.endnote
.np
Here are some sample "SET" commands.
.exam begin
C>set 1=input.dat
C>set 2=output.dat
C>set 3=&dr4.&pc.database&pc.customer.fil
.exam end
.pc
The above example establishes the following preconnections:
.autonote
.note
Between unit 1 and the file :FNAME.input.dat:eFNAME. which resides (or will
reside) in the current directory.
.note
Between unit 2 and the file :FNAME.output.dat:eFNAME. which resides (or will
reside) in the current directory.
.note
Between unit 3 and the file :FNAME.&dr4.&pc.database&pc.customer.fil:eFNAME.
which resides (or will reside) in another disk and directory.
.endnote
.np
Any FORTRAN input/output statements which refer to units 1, 2 or 3
will act upon one of these 3 data files.
.autonote Notes:
.note
The "SET" command must be issued before running the program.
.note
No spaces should be placed before or after the "=" in the "SET"
command.
The following two examples are quite distinct
from each other:
.exam begin
C>set 55=testbed.dat
C>set 55 = testbed.dat
.exam end
.pc
To verify this, simply enter the two commands and then enter the "SET"
command again with no arguments.
The current environment strings will be displayed.
You should find two entries, one for "55" and one for "55~b".
.note
Since the number in front of the "=" is simply a character string, you
should not specify any leading zeroes either.
.exam begin
C>set 01=input.dat
C>set 1=input.dat
.exam end
.pc
In this case, we again have two distinct environment variables.
The variable "01" will be ignored by the &cmpname run-time system.
.note
An environment variable will remain in effect until you explicitly
remove it or you turn off the personal computer.
To discontinue the preconnection between a unit number and a file, you
must issue a "SET" command of the following form.
.millust begin
&prompt.set <unit>=
.millust end
In the above command,
.mono <unit>
is the unit number for which the preconnection is to be discontinued.
.pc
By omitting the character string after the "=",
the environment variable will be removed.
For example, to remove the environment variable "01" from the list,
reenter the "SET" command specifying everything up to and including
the "=" character.
.exam begin
C>set 01=
.exam end
.note
Any time you wish to see the current list of environment strings,
simply enter the "SET" command with no arguments.
.exam begin
C>set
PROMPT=$d $t $p$_$n$g
COMSPEC=d:\dos\command.com
PATH=G:\;E:\CMDS;C:&pathnamup\BIN;D:\DOS;D:\BIN
&libvarup.=c:&pathnam.&libdir16.\dos
1=input.dat
2=output.dat
3=&dr4.&pc.database&pc.customer.fil
.exam end
.note
An alternative to preconnecting files is provided by the FORTRAN
.kw OPEN
statement which allows files to be connected at execution time.
.note
.ix 'preconnection'
The preconnection of units 5 and 6 may be overridden using
preconnection specifications or the FORTRAN
.kw OPEN
statement.
.ix 'connection precedence'
The precedence of a connection between a unit number and a file
is as follows:
.begnote
:DTHD.Precedence:
:DDHD.User option:
.note Lowest
Preconnection Specifications
.note Highest
OPEN statement
.endnote
.np
In other words, the
.kw OPEN
statement overrides a preconnection.
.endnote
.*
.section A Preconnection Tutorial
.*
.np
.ix 'preconnecting files'
In this section, we will look at some examples of how to establish the
link between a file and a FORTRAN unit.
.np
.bd Exhibit 1:
.np
Consider the following example which reads pairs of numbers from a
file and writes out the numbers and their sum.
.millust begin
* File ':FNAME.iodemo.for:eFNAME.'
10    READ( 1, *, END=99 ) X1, X2
      WRITE( 6, 20 ) X1, X2, X1 + X2
      GO TO 10
20    FORMAT( 3F6.2 )
99    END
.millust end
.pc
The FORTRAN
.kw READ
statement will read records from a file connected to unit 1.
The FORTRAN
.kw WRITE
statement will write records to a file connected to unit 6.
As we described in the previous section, unit 6 is preconnected by the
&cmpname run-time system to the screen.
.np
What file will be read when the
.kw READ
statement refers to unit 1?
By default, we know that it will read a file called :FNAME.for001:eFNAME..
However, suppose the data was actually stored in the file called
:FNAME.numbers.dat:eFNAME..
We can direct the program to read the data in this file by using a
"SET" command before running the program.
.exam begin
C>set 1=numbers.dat
C>iodemo
  1.40  2.50  3.90
  3.90  8.70 12.60
  1.10  9.90 11.00
  8.30  7.10 15.40
  8.20  3.50 11.70
.exam end
.np
.bd Exhibit 2:
.np
Suppose that we now wish to write the output from the above program to
a disk file instead of the screen.
We can do this without modifying the program.
Since we know that the
.kw WRITE
statement refers to unit 6, we can alter the default preconnection of
unit 6 to the screen by issuing another "SET" command.
.exam begin
C>set 6=numbers.rpt
C>iodemo
C>type numbers.rpt
  1.40  2.50  3.90
  3.90  8.70 12.60
  1.10  9.90 11.00
  8.30  7.10 15.40
  8.20  3.50 11.70
.exam end
.pc
Now any time a program writes or prints to unit 6, the output will be
written to the disk file :FNAME.numbers.rpt:eFNAME..
If you are going to run other programs, it would be wise to remove the
connection between unit 6 and this file so that it is not accidentally
overwritten.
This can be done by issuing the following command.
.exam begin
C>set 6=
.exam end
.np
You should also do the same for unit 1.
.np
.bd Exhibit 3:
.np
Must we always use "SET" commands to establish the connection between
a unit and a file?
Suppose that you want to run the program quite often and that you do
not want to issue "SET" commands every time.
We can do this by modifying the program to include FORTRAN
.kw OPEN
statements.
.millust begin
* File ':FNAME.iodemo.for:eFNAME.'
      OPEN( 1, FILE='NUMBERS.DAT' )
      OPEN( 6, FILE='NUMBERS.RPT' )
10    READ( 1, *, END=99 ) X1, X2
      WRITE( 6, 20 ) X1, X2, X1 + X2
      GO TO 10
20    FORMAT( 3F6.2 )
99    END
.millust end
.pc
This is an example of a connection that is established at execution
time.
The connection that is established by the
.kw OPEN
statement overrides any preconnection that we might have established
using a "SET" command.
We say that the
.kw OPEN
statement has a higher precedence.
However, even the
.kw OPEN
statement does not have the final word on which files will be
accessed.
You may wish to read the next section on the &cmpname run-time system
logical file name support to find out why this is so.
.*
.section Logical File Name Support
.*
.np
.ix 'logical file name'
.ix 'SET command'
The &cmpname run-time system supports logical or symbolic file names
using the "SET" command.
The "SET" command may be used to define a logical file name and its
corresponding actual file name.
The format for defining a logical file name is as follows:
.millust begin
SET name=file_spec
.millust end
.synote
.note name
is any character string.
The letters in "name" may be specified in upper or lower case.
Lower case letters are treated as if they had been specified in upper
case.
Thus "SYSINPUT" and "sysinput" are equivalent.
Note, however, that blank characters must not be specified before and
after the "=" character.
.note file_spec
is the file specification of logical file.
.endnote
.autonote Notes and Examples:
.note
A logical file name may be used in the
.kw FILE=
specifier of the FORTRAN
.kw OPEN
and
.kw INQUIRE
statements.
.exam begin
* File ':FNAME.iodemo.for:eFNAME.'
      OPEN( 1, FILE='SYSINPUT' )
10    READ( 1, *, END=99 ) X1, X2
      WRITE( 6, 20 ) X1, X2, X1 + X2
      GO TO 10
20    FORMAT( 3F6.2 )
99    END
.exam end
.np
In the following example, we define the logical file name "SYSINPUT"
to correspond to the file :FNAME.numbers.dat:eFNAME..
.exam begin
C>set sysinput=numbers.dat
C>iodemo
  1.40  2.50  3.90
  3.90  8.70 12.60
  1.10  9.90 11.00
  8.30  7.10 15.40
  8.20  3.50 11.70
.exam end
.note
If the name in a
.kw FILE=
specifier is not included in one of the environment variable names
then it is assumed to be the actual name of a file.
.exam begin
OPEN( 2, FILE='SYSOUT' )
.exam end
.ix 'logical file name' 'extended file names'
.note
The logical file name feature can also be used to provide additional
information regarding the file name at execution time.
.exam begin
* File ':FNAME.iodemo.for:eFNAME.'
      OPEN( 1, FILE=':FNAME.numbers.dat:eFNAME.' )
10    READ( 1, *, END=99 ) X1, X2
      WRITE( 6, 20 ) X1, X2, X1 + X2
      GO TO 10
20    FORMAT( 3F6.2 )
99    END
.exam end
.pc
In the following example, the actual location (and name) of the file
:FNAME.numbers.dat:eFNAME. is described through the use of an environment
variable.
.exam begin
C>set numbers.dat=&dr2.&pc.data&pc.input.dat
C>iodemo
.exam end
.pc
As you can see, a logical file name can resemble an actual
file name.
.np
Of course, the entire file name could have been specified in the
FORTRAN program.
.exam begin
OPEN( 1, FILE=':FNAME.&dr2.&pc.data&pc.input.dat:eFNAME.' )
.exam end
.note
Only one level of lookup is performed.
.exam begin
* File ':FNAME.iodemo.for:eFNAME.'
      OPEN( 1, FILE=':FNAME.sysinput:eFNAME.' )
10    READ( 1, *, END=99 ) X1, X2
      WRITE( 6, 20 ) X1, X2, X1 + X2
      GO TO 10
20    FORMAT( 3F6.2 )
99    END
.exam end
.np
This is illustrated by the following commands.
.exam begin
C>set sysinput=datafile
C>set datafile=input.dat
C>iodemo
.exam end
.pc
In the above example, unit 1 is connected to the file :FNAME.datafile:eFNAME.
and not the file :FNAME.input.dat:eFNAME..
.ix 'logical file name' 'device remapping'
.note
Logical file names can be used to direct output normally intended for
one device to another device.
Consider the following examples.
.exam begin
C>set lpt1=lpt2
.exam end
.pc
If the FORTRAN program specifies the name "LPT1" in an
.kw OPEN
or
.kw INQUIRE
statement, the &cmpname run-time system will map this name to "LPT2".
In an
.kw INQUIRE
statement, the
.kw NAME=
specifier will return the name "LPT2".
.note
As we mentioned earlier, the case of the name does not matter.
Upper or lower case can be used interchangeably.
.exam begin
C>set sysinput=&dr2.&pc.data&pc.input.dat
C>set SYSINPUT=&dr2.&pc.data&pc.input.dat
.exam end
.note
No spaces should be placed before or after the "=" in the "SET"
command.
The following two examples are considered quite
distinct from each other:
.exam begin
C>set sysinput=testbed.dat
C>set sysinput = testbed.dat
.exam end
.pc
This example will define two variables, "SYSINPUT" and
"SYSINPUT~b".
.note
An environment variable will remain in effect until you explicitly
remove it or you turn off the personal computer.
To remove an environment variable from the list, reenter the "SET"
command specifying everything up to and including the "=" character.
For example, to remove the definition for "SYSINPUT", the following
command can be issued.
.exam begin
C>set sysinput=
.exam end
.note
.ix 'logical file name' 'display'
Any time you wish to see the current list of environment strings,
simply enter the "SET" command with no arguments.
.exam begin
C>set
PROMPT=$d $t $p$_$n$g
COMSPEC=d:\dos\command.com
PATH=G:\;E:\CMDS;C:&pathnamup\BIN;D:\DOS;D:\BIN
&libvarup.=c:&pathnam.&libdir16.\dos
1=input.dat
2=output.dat
3=&dr4.&pc.database&pc.customer.fil
SYSINPUT=&dr2.&pc.data&pc.input.dat
LPT1=lpt2
.exam end
.endnote
:cmt. JBS.*
:cmt. JBS.section Open Files in the Run-time Environment
:cmt. JBS.*
:cmt. JBS.np
:cmt. JBS.ix 'open files'
:cmt. JBS.ix 'files' 'open'
:cmt. JBSUnder &machsys, a number of files are open during the
:cmt. JBSexecution of your program.
:cmt. JBSThe following describes how the &cmpname run-time system makes use of
:cmt. JBSstandard &machsys files and describes those files which are opened by
:cmt. JBSyour program.
:cmt. JBSThis information may be used to determine how many files are actually
:cmt. JBSopen during the execution of your program.
:cmt. JBS.beglevel
:cmt. JBS.*
:cmt. JBS.section Standard Input
:cmt. JBS.*
:cmt. JBS.np
:cmt. JBS.ix 'standard input'
:cmt. JBS.ix 'DOS' 'standard input'
:cmt. JBS.ix 'OS/2' 'standard input'
:cmt. JBS.ix 'CTTY'
:cmt. JBS.ix 'DOS' 'CTTY'
:cmt. JBSThe "Standard Input" file (handle 0) is permanently opened by &machsys
:cmt. JBSwhen the PC is started up.
:cmt. JBSNormally, standard input records are entered at the console keyboard.
:cmt. JBSStandard input may be redirected to another device using the DOS
:cmt. JBS"CTTY" command or to a file through the use of the "<" character on
:cmt. JBSthe program command line (see next section).
:cmt. JBSDuring program execution, unit 5 is preconnected to this file.
:cmt. JBS.*
:cmt. JBS.section Standard Input Redirection
:cmt. JBS.*
:cmt. JBS.np
:cmt. JBS.ix 'standard input' 'redirection'
:cmt. JBSAnother file is opened by &machsys if standard input redirection ("<")
:cmt. JBSwas specified on the program command line.
:cmt. JBSDuring program execution, the specified file will be preconnected to
:cmt. JBSunit 5.
:cmt. JBS.*
:cmt. JBS.section Standard Output
:cmt. JBS.*
:cmt. JBS.np
:cmt. JBS.ix 'standard output'
:cmt. JBS.ix 'DOS' 'standard output'
:cmt. JBS.ix 'OS/2' 'standard output'
:cmt. JBS.ix 'CTTY'
:cmt. JBS.ix 'DOS' 'CTTY'
:cmt. JBSThe "Standard Output" file (handle 1) is permanently opened by
:cmt. JBS&machsys when the PC is started up.
:cmt. JBSNormally, standard output records are printed on the console screen.
:cmt. JBSStandard output may be redirected to another device using the DOS
:cmt. JBS"CTTY" command or to a file through the use of the ">" character on
:cmt. JBSthe program command line (see next section).
:cmt. JBSDuring program execution, unit 6 is preconnected to this file.
:cmt. JBS.*
:cmt. JBS.section Standard Output Redirection
:cmt. JBS.*
:cmt. JBS.np
:cmt. JBS.ix 'standard output' 'redirection'
:cmt. JBSAnother file is opened by &machsys if standard output redirection
:cmt. JBS(">") was specified on the program command line.
:cmt. JBSDuring program execution, the specified file will be preconnected to
:cmt. JBSunit 6.
:cmt. JBS.*
:cmt. JBS.section Standard Error File
:cmt. JBS.*
:cmt. JBS.np
:cmt. JBS.ix 'standard error'
:cmt. JBS.ix 'DOS' 'standard error'
:cmt. JBS.ix 'OS/2' 'standard error'
:cmt. JBSThe "Standard Error" file (handle 2) is used by the &cmpname run-time
:cmt. JBSsystem as a last resort for displaying messages.
:cmt. JBSThis may occur when, for example, a diskette becomes entirely full and
:cmt. JBSit is no longer possible for the run-time system to write a message to
:cmt. JBSthe diskette.
:cmt. JBSOutput to the standard error file is produced on the screen.
:cmt. JBSUnder DOS 2.x, the standard error file is not normally opened until
:cmt. JBSsome output is written to it.
:cmt. JBSUnder DOS 3.x or later, the standard error file is permanently open.
:cmt. JBSDuring program execution, this file is used by the &cmpname run-time
:cmt. JBSsystem to issue diagnostic messages.
:cmt. JBS.*
:cmt. JBS.section Standard Auxiliary Device
:cmt. JBS.*
:cmt. JBS.np
:cmt. JBS.ix 'standard auxiliary'
:cmt. JBS.ix 'DOS' 'standard auxiliary'
:cmt. JBSUnder DOS 3.x or later, the "Standard Auxiliary" device (handle 3) is
:cmt. JBSpermanently open.
:cmt. JBSThe auxiliary device is the asynchronous communications adapter
:cmt. JBSor serial port.
:cmt. JBSThis file is not used by the &cmpname run-time system but must be
:cmt. JBSconsidered as an open file when determining how many files are
:cmt. JBSactually open during the execution of a FORTRAN program.
:cmt. JBS.*
:cmt. JBS.section Standard Printer Device
:cmt. JBS.*
:cmt. JBS.np
:cmt. JBS.ix 'standard printer'
:cmt. JBS.ix 'DOS' 'standard printer'
:cmt. JBS.ix 'OS/2' 'standard printer'
:cmt. JBSUnder DOS, the "Standard Printer" device (handle 4) is available to
:cmt. JBSthe executing process.
:cmt. JBSThis file is not opened unless written to.
:cmt. JBSThis file is not used by the &cmpname run-time system but must be
:cmt. JBSconsidered when determining how many files can be opened during the
:cmt. JBSexecution of a FORTRAN program.
:cmt. JBS.* .*
:cmt. JBS.* .section Unit 5
:cmt. JBS.* .*
:cmt. JBS.* .np
:cmt. JBS.* .ix 'unit 5'
:cmt. JBS.* Unit 5 is opened by the &cmpname run-time system if any input/output
:cmt. JBS.* statement references this file.
:cmt. JBS.* .exam
:cmt. JBS.* READ *, X, Y
:cmt. JBS.* .eexam
:cmt. JBS.* .pc
:cmt. JBS.* As we described in an earlier section on preconnected files, this
:cmt. JBS.* file is normally connected to the terminal keyboard.
:cmt. JBS.* .*
:cmt. JBS.* .section Unit 6
:cmt. JBS.* .*
:cmt. JBS.* .np
:cmt. JBS.* .ix 'unit 6'
:cmt. JBS.* Unit 6 is opened by the &cmpname run-time system if any input/output
:cmt. JBS.* statement references this file.
:cmt. JBS.* .exam
:cmt. JBS.* PRINT *, 'Enter a number'
:cmt. JBS.* .eexam
:cmt. JBS.* .pc
:cmt. JBS.* As we described in an earlier section on preconnected files, this
:cmt. JBS.* file is normally connected to the terminal screen.
:cmt. JBS.*
:cmt. JBS.section Files Opened by a FORTRAN Program
:cmt. JBS.*
:cmt. JBS.np
:cmt. JBS.ix 'open files'
:cmt. JBSOther files can be opened by the executing FORTRAN program.
:cmt. JBSA file handle is associated with every file opened by &machsys..
:cmt. JBS.*
:cmt. JBS.section DOS File Handle Considerations
:cmt. JBS.*
:cmt. JBS.np
:cmt. JBS.ix 'handle'
:cmt. JBS.ix 'DOS' 'handle'
:cmt. JBSDOS manages files through the use of handles.
:cmt. JBSOne file handle is required by DOS for every open file.
:cmt. JBS.ix 'FILES='
:cmt. JBS.ix 'DOS' 'FILES='
:cmt. JBS.ix 'CONFIG.SYS'
:cmt. JBS.ix 'DOS' 'CONFIG.SYS'
:cmt. JBSUnder DOS, the maximum number of file handles available to all
:cmt. JBSprocesses executing in the personal computer is controlled by the
:cmt. JBS"FILES=" statement of the DOS
:cmt. JBS.bd CONFIG.SYS
:cmt. JBSfile (this file must be placed in the root directory of the boot
:cmt. JBSdisk).
:cmt. JBSIn the absence of
:cmt. JBS.bd CONFIG.SYS
:cmt. JBSfile, DOS will allocate 8 file handles by default.
:cmt. JBSThis default may be altered using the "FILES=" statement in the DOS
:cmt. JBS.bd CONFIG.SYS
:cmt. JBSfile.
:cmt. JBSThis special DOS file must be placed in the root directory of the boot
:cmt. JBSdisk.
:cmt. JBSThe "FILES=" parameter defines the total number of file handles for
:cmt. JBSall processes in your system.
:cmt. JBS.np
:cmt. JBSUnder DOS, each process can use a maximum of 20 file handles
:cmt. JBSnumbered 0, 1, up to 19.
:cmt. JBSFile handles 0 through 4 are preassigned by DOS to the standard input,
:cmt. JBSoutput, error, auxiliary, and printer devices.
:cmt. JBSFile handles 5 through 19 (a total of 15 handles) are also available
:cmt. JBSfor use by the &cmpname run-time system.
:cmt. JBSThus an additional 15 files can be opened by the &cmpname run-time
:cmt. JBSsystem, provided that you have specified a large enough number of file
:cmt. JBShandles in the "FILES=" statement in the
:cmt. JBS.bd CONFIG.SYS
:cmt. JBSfile.
:cmt. JBS.np
:cmt. JBSAs we stated above, one file handle is used for each open file.
:cmt. JBSUnder DOS 2.x, 3 files are permanently opened, thus 3 file handles are
:cmt. JBSalready in use.
:cmt. JBSUnder DOS 3.x or later, 4 files are permanently opened, thus 4 file
:cmt. JBShandles are already in use.
:cmt. JBSIf you use DOS input and output redirection using the "<" and ">"
:cmt. JBScharacters then additional file handles will be used by DOS thereby
:cmt. JBSreducing the number available to the &cmpname run-time system.
:cmt. JBS.np
:cmt. JBSThus, if you specify "FILES=10" in the DOS
:cmt. JBS.bd CONFIG.SYS
:cmt. JBSfile,
:cmt. JBSthe following table describes the number of file handles available
:cmt. JBSto the &cmpname run-time system for file input/output.
:cmt. JBS.millust begin
:cmt. JBS                      DOS 2.x   DOS 3.x or later
:cmt. JBSwithout "<" and ">"      7         6
:cmt. JBSwith "<"                 6         5
:cmt. JBSwith ">"                 6         5
:cmt. JBSwith both "<" and ">"    5         4
:cmt. JBS.millust end
:cmt. JBS.np
:cmt. JBSSince each file opened by the &cmpname run-time system requires a DOS
:cmt. JBSfile handle,
:cmt. JBSthe maximum number of files which can be opened by the &cmpname
:cmt. JBSrun-time system at one time can be calculated as:
:cmt. JBS.millust begin
:cmt. JBSif FILES=n then
:cmt. JBS    (under DOS 2.x)
:cmt. JBS        min(15, n - 3 - NumberOf(std i/o redirections))
:cmt. JBS    (under DOS 3.x or later)
:cmt. JBS        min(15, n - 4 - NumberOf(std i/o redirections))
:cmt. JBS.millust end
:cmt. JBS.pc
:cmt. JBSprovided that no other tasks have opened any files.
:cmt. JBSFor example, if you have specified "FILES=20" and you are using
:cmt. JBSa DOS 3.x system then the maximum number of file handles available
:cmt. JBSto your program is 20 &minus 4 = 16.
:cmt. JBSSince the run-time system can use a maximum of 15 handles (5 through
:cmt. JBS19), there is an adequate number of handles to open 15 files.
:cmt. JBSOn the other hand, if you have specified "FILES=15" and you are using
:cmt. JBSa DOS 3.x system then the maximum number of file handles available to
:cmt. JBSyour program is 15 &minus 4 = 11.
:cmt. JBSSince the run-time system can use a maximum of 15 handles (5 through
:cmt. JBS19) and 11 are available, only 11 files can be opened.
:cmt. JBS.np
:cmt. JBSTo calculate the number of DOS file handles that should be allocated,
:cmt. JBSconsider that the following files will be open when a FORTRAN program
:cmt. JBSbegins execution.
:cmt. JBS.begpoint
:cmt. JBS.point (i)
:cmt. JBS.ix 'standard input'
:cmt. JBS.ix 'DOS standard input'
:cmt. JBSThe DOS "Standard Input" file is already opened by DOS and is preconnected
:cmt. JBSto unit 5 by the &cmpname run-time system.
:cmt. JBS.point (ii)
:cmt. JBS.ix 'standard output'
:cmt. JBS.ix 'DOS standard output'
:cmt. JBSThe DOS "Standard Output" file is already opened by DOS and is preconnected
:cmt. JBSto unit 6 by the &cmpname run-time system.
:cmt. JBS.point (iii)
:cmt. JBS.ix 'standard error'
:cmt. JBS.ix 'DOS standard error'
:cmt. JBSThe DOS "Standard Error" file is already opened by DOS
:cmt. JBS(used by the &cmpname run-time system to issue messages).
:cmt. JBS.point (iv)
:cmt. JBS.ix 'standard auxiliary'
:cmt. JBS.ix 'DOS standard auxiliary'
:cmt. JBSUnder DOS 3.x or later, the "Standard Auxiliary" file is already opened.
:cmt. JBS.* .point (v)
:cmt. JBS.* .ix 'unit 5'
:cmt. JBS.* Unit 5 is opened by the &cmpname run-time system at execution-time if
:cmt. JBS.* any input/output statement references this file.
:cmt. JBS.* .exam
:cmt. JBS.* READ *, X, Y
:cmt. JBS.* .eexam
:cmt. JBS.* .point (vi)
:cmt. JBS.* .ix 'unit 6'
:cmt. JBS.* Unit 6 is opened by the &cmpname run-time system at execution-time if
:cmt. JBS.* any input/output statement references this file.
:cmt. JBS.* .exam
:cmt. JBS.* PRINT *, 'Enter a number'
:cmt. JBS.* .eexam
:cmt. JBS.endpoint
:cmt. JBS.np
:cmt. JBSIn addition, the following files could be open.
:cmt. JBS.begpoint
:cmt. JBS.point (v)
:cmt. JBS.ix 'standard input ' 'redirection'
:cmt. JBSAnother file is opened by DOS if standard input redirection ("<") was
:cmt. JBSspecified on the command line.
:cmt. JBS.point (vi)
:cmt. JBS.ix 'standard output ' 'redirection'
:cmt. JBSAnother file is opened by DOS if standard output redirection (">") was
:cmt. JBSspecified on the command line.
:cmt. JBS&cmpname run-time system error messages which are normally printed
:cmt. JBSon the screen will appear in the specified file.
:cmt. JBS.endpoint
:cmt. JBS.np
:cmt. JBSFiles (i) through (vi), that we have described above, each require
:cmt. JBSrequire DOS file handles.
:cmt. JBS.np
:cmt. JBSUnder DOS 2.x, a typical situation is where files (i), (ii) and (iii)
:cmt. JBSabove are already opened by DOS.
:cmt. JBSThis gives a total of 3 open files.
:cmt. JBSIf the current maximum number of DOS file handles is 8 then at most 5
:cmt. JBSmore files can be opened by the executing FORTRAN program.
:cmt. JBS.ix 'FILES='
:cmt. JBS.ix 'DOS' 'FILES='
:cmt. JBS.ix 'CONFIG.SYS'
:cmt. JBS.ix 'DOS' 'CONFIG.SYS'
:cmt. JBSIf 7 more files are to be opened by the executing program then the
:cmt. JBSremedy is to specify "FILES=10" in the DOS
:cmt. JBS.bd CONFIG.SYS
:cmt. JBSfile and then reboot the personal computer.
:cmt. JBSUnder DOS 3.x or later, another file (iv) is open so we must specify
:cmt. JBS"FILES=11".
:cmt. JBS.endlevel
.*
.section Terminal or Console Device Support
.*
.np
.ix 'terminal device'
.ix 'console device'
.ix 'device' 'console'
Input can come from the console or output can be written to the
console by using the console device name :FNAME.con:eFNAME. as the
file name.
The console can be specified in a "SET" command or
through the
.kw FILE=
specifier of the FORTRAN
.kw OPEN
statement.
.np
.ix 'CON' 'Win32'
.ix 'Win32' 'CON'
The default action for any file is to open the file for both read and
write access (i.e., ACTION='READWRITE').
Under Win32, there is a problem accessing the console device
:FNAME.con:eFNAME. for both read and write access.
This problem is overcome by using the
.kw ACTION=
specifier in the
.kw OPEN
statement.
The
.kw ACTION=
specifier indicates the way in which the file is initially accessed.
The values allowed for the
.kw ACTION=
specifier are the following.
.begnote
.note 'READ'
the file is opened for read-only access
.note 'WRITE'
the file is opened for write-only access
.note 'READWRITE'
the file is opened for both read and write access
.endnote
.np
To open the console device under Win32, you must specify whether you
are going to "READ" or "WRITE" to the file.
If you wish to do both reading and writing, then you must use two
separate units.
.exam begin
OPEN( UNIT=1, FILE='CON', ACTION='READ')
OPEN( UNIT=2, FILE='CON', ACTION='WRITE')
.exam end
.np
The console can be treated as a carriage control device.
This is requested by using the
.mono CARRIAGECONTROL='YES'
specifier of the FORTRAN
.kw OPEN
statement.
.exam begin
OPEN( UNIT=1, FILE=':FNAME.con:eFNAME.', CARRIAGECONTROL='YES' )
.exam end
.np
Carriage control handling is described in the
section entitled :HDREF refid='fdospfa'..
.np
The console is not capable of supporting carriage control in a
fashion identical to a printer.
For example, overprinting of records on the console is destructive in
that the previous characters are erased.
.np
End of file is signalled by first pressing the Ctrl/Z key combination
and then the line entering key.
End of file may be handled by using the
.kw END=
specification of the
FORTRAN
.kw READ
statement.
.exam begin
    READ( UNIT=*, FMT=*, END=100 ) X, Y
    .
    .
    .
100    code to handle "End of File"
.exam end
.np
End of file may also be handled by using the
.kw IOSTAT=
specifier
of the FORTRAN
.kw READ
statement.
.exam begin
READ( UNIT=*, FMT=*, IOSTAT=IOS ) X, Y
IF( IOS .NE. 0 )THEN
.
.   code to handle "End of File"
.
ENDIF
.exam end
.*
.section Printer Device Support
.*
.np
.ix 'printer device'
.ix 'device' 'printer'
Output can be written to a printer by using a printer device name as
the file name.
A printer can be specified in a "SET" command or through
the
.kw FILE=
specifier of the FORTRAN
.kw OPEN
statement.
Several device names may be used:
.millust begin
:FNAME.prn:eFNAME. or :FNAME.lpt1:eFNAME.
:FNAME.lpt2:eFNAME.
:FNAME.lpt3:eFNAME.
.millust end
.np
The printer can be treated as a carriage control device.
This is requested by using the
.mono CARRIAGECONTROL='YES'
specifier of the FORTRAN
.kw OPEN
statement.
.exam begin
OPEN( UNIT=1, FILE=':FNAME.prn:eFNAME.', CARRIAGECONTROL='YES' )
.exam end
.np
Carriage control handling is described in the
section entitled :HDREF refid='fdospfa'..
.*
.section Serial Device Support
.*
.np
.ix 'serial device'
.ix 'device' 'serial'
Output can be written to a serial port by using a serial device name
as the file name.
A serial device can be specified in a "SET" command or
through the
.kw FILE=
specifier of the FORTRAN
.kw OPEN
statement.
Three device names may be used:
.millust begin
:FNAME.aux:eFNAME. or :FNAME.com1:eFNAME.
:FNAME.com2:eFNAME.
.millust end
.np
The serial device can be treated as a carriage control device.
This is requested by using the
.mono CARRIAGECONTROL='YES'
specifier of the FORTRAN
.kw OPEN
statement.
.exam begin
OPEN( UNIT=1, FILE=':FNAME.com1:eFNAME.', CARRIAGECONTROL='YES' )
.exam end
.np
Carriage control handling is described in the
section entitled :HDREF refid='fdospfa'..
.np
To set serial characteristics such as speed, parity, and word length,
the "MODE" command may be used.
.exam begin
C>mode com1:9600,n,8,1
.exam end
.pc
The above example sets serial port 1 to a speed of 9600 BAUD with no
parity, a word length of 8 and 1 stop bit.
.*
.section File Handling Defaults
.*
.np
.ix 'file defaults'
The following defaults apply to file specifications:
.begbull
.bull
.ix 'record type' 'default'
.ix 'defaults' 'record type'
The following table indicates the default
.us record type
for the allowable access methods and forms.
.millust begin
File                    Form
Access         Formatted   Unformatted
              +-----------+-----------+
Sequential    |   Text    | Variable  |
              +-----------+-----------+
Direct        |   Text    | Fixed     |
              +-----------+-----------+
.millust end
.pc
Unless the record type of the file does not correspond to the default
assumed by &cmpname, the record type attribute should not be
specified.
.bull
.ix 'record length' 'default'
.ix 'defaults' 'record length'
Unless otherwise stated, the default
.us record length
for a file is 1024 characters.
When access is "direct", the record length must be specified in the
.kw RECL=
specifier of the FORTRAN
.kw OPEN
statement.
The record length may also be specified when the access is
"sequential".
This should be done whenever access is "sequential" and the maximum
record length is greater than the default.
.bull
.ix 'record access' 'default'
.ix 'defaults' 'record access'
The default
.us record access
is "sequential".
.bull
.ix 'file name' 'default'
.ix 'defaults' 'file name'
When reading from or writing to a unit for which no preconnection has
been specified or no "FILE=" form of the FORTRAN
.kw OPEN
statement has been executed, the default
.us file name
takes the form:
.millust begin
FORnnn
.millust end
.pc
.us nnn
is a three-digit FORTRAN unit number.
Unit 0 is "000", unit 1 is "001", unit 2 is "002", and so on.
There is no file extension in this case.
.bull
If the connection between a unit number and a file is discontinued
through use of the FORTRAN
.kw CLOSE
statement, the same rule for constructing a file name will apply on
the next attempt to read from or write to the specified unit.
.endbull
