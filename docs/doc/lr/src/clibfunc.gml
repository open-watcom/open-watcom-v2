.*
.dm addindent begin
.  ..in +&*.
.dm addindent end
.*
.dm noindent begin
.  ..in -&*.
.dm noindent end
.*
.dm bigterms begin
.  .addindent 3
.dm bigterms end
.*
.dm bigterm begin
.  .noindent 3
.  :P.
.  :HP2.
.dm bigterm end
.*
.dm bigdesc begin
.  :eHP2.
.  .addindent 3
.  :P.
.dm bigdesc end
.*
.dm endbigterms begin
.  .noindent 3
.dm endbigterms end
.*----------------------------------------------------
.*
.chap Implementation-Defined Behavior of the C Library
.np
This appendix describes the behavior of
.if '&machsys' eq 'PP' .do begin
the &company C Library for Penpoint
.do end
.el .do begin
the 16-bit and 32-bit &company C libraries
.do end
when the ANSI/ISO C Language standard describes the behavior as
.us implementation-defined.
The term describing each behavior is taken directly from the ANSI/ISO
C Language standard.
The numbers in parentheses at the end of each term refers to the
section of the standard that discusses the behavior.
.*
.bigterms
.*----------------------------------------------------
.section NULL Macro
.bigterm
The null pointer constant to which the macro
.kw NULL
expands (7.1.6).
.bigdesc
.np
The macro
.kw NULL
expands to
.if '&machsys' eq 'PP' .do begin
0.
.do end
.el .do begin
0 in small data models and to 0L in large data models.
.do end
.*-------------------------
.section Diagnostic Printed by the assert Function
.bigterm
The diagnostic printed by and the termination behavior of the
.kw assert
function (7.2).
.bigdesc
.np
The
.kw assert
function prints a diagnostic message to
.kw stderr
and calls the
.kw abort
routine if the expression is false.
The diagnostic message has the following form:
.millust begin
Assertion failed: [expression], file [name], line [number]
.millust end
.*-------------------------
.section Character Testing
.bigterm
The sets of characters tested for by the
.kw isalnum
.ct,
.kw isalpha
.ct,
.kw iscntrl
.ct,
.kw islower
.ct,
.kw isprint
.ct,
and
.kw isupper
functions (7.3.1).
.bigdesc
.np
.begterm 18 $compact
.termhd1 Function
.termhd2 Characters Tested For
.term isalnum
Characters 0-9, A-Z, a-z
.term isalpha
Characters A-Z, a-z
.term iscntrl
ASCII 0x00-0x1f, 0x7f
.term islower
Characters a-z
.term isprint
ASCII 0x20-0x7e
.term isupper
Characters A-Z
.endterm
.*-------------------------
.section Domain Errors
.bigterm
The values returned by the mathematics functions on domain errors (7.5.1).
.bigdesc
.np
When a domain error occurs, the listed values are returned by the
following functions:
.begterm 18 $compact
.termhd1 Function
.termhd2 Value returned
.term acos
0.0
.term acosh
&minus HUGE_VAL
.term asin
0.0
.term atan2
0.0
.term atanh
&minus HUGE_VAL
.term log
&minus HUGE_VAL
.term log10
&minus HUGE_VAL
.term log2
&minus HUGE_VAL
.term pow(neg,frac)
0.0
.term pow(0.0,0.0)
1.0
.term pow(0.0,neg)
&minus HUGE_VAL
.term sqrt
0.0
.term y0
&minus HUGE_VAL
.term y1
&minus HUGE_VAL
.term yn
&minus HUGE_VAL
.endterm
.*-------------------------
.section Underflow of Floating-Point Values
.bigterm
Whether the mathematics functions set the integer expression
.kw errno
to the value of the macro
.kw ERANGE
on underflow range errors (7.5.1).
.bigdesc
.np
The integer expression
.kw errno
is not set to
.kw ERANGE
on underflow range errors in the mathematics functions.
.*-------------------------
.section The fmod Function
.bigterm
Whether a domain error occurs or zero is returned when the
.kw fmod
function has a second argument of zero (7.5.6.4).
.bigdesc
.np
Zero is returned when the second argument to
.kw fmod
is zero.
.*-------------------------
.section The signal Function
.bigterm
The set of signals for the
.kw signal
function (7.7.1.1).
.bigdesc
.np
.if '&machsys' eq 'PP' .do begin
Signals are not completely implemented under PenPoint.
Hence, the
.kw signal
function is not described in this book.
.do end
.el .do begin
See the description of the
.kw signal
function presented earlier in this book.
.if '&machsys' eq 'QNX' .do begin
Also see the
.kw QNX System Architecture
manual.
.do end
.do end
.*-------------------------
.bigterm
The semantics for each signal recognized by the
.kw signal
function (7.7.1.1).
.bigdesc
.np
.if '&machsys' eq 'PP' .do begin
Signals are not completely implemented under PenPoint.
Hence, the
.kw signal
function is not described in this book.
.do end
.el .do begin
See the description of the
.kw signal
function presented earlier in this book.
.if '&machsys' eq 'QNX' .do begin
Also see the
.kw QNX System Architecture
manual.
.do end
.do end
.*-------------------------
.bigterm
The default handling and the handling at program startup for each
signal recognized by the
.kw signal
function (7.7.1.1).
.bigdesc
.np
.if '&machsys' eq 'PP' .do begin
Signals are not completely implemented under PenPoint.
Hence, the
.kw signal
function is not described in this book.
.do end
.el .do begin
See the description of the
.kw signal
function presented earlier in this book.
.if '&machsys' eq 'QNX' .do begin
Also see the
.kw QNX System Architecture
manual.
.do end
.do end
.*-------------------------
.section Default Signals
.bigterm
If the equivalent of
.ix 'SIG_DFL'
.kw signal
.ct ( sig, SIG_DFL );
is not executed prior to the call of a signal handler,
the blocking of the signal that is performed (7.7.1.1).
.bigdesc
.np
The equivalent of
.millust begin
signal( sig, SIG_DFL );
.millust end
.np
is executed prior to the call of a signal handler.
.*-------------------------
.section The SIGILL Signal
.bigterm
Whether the default handling is reset if the
.kw SIGILL
signal is received by a handler specified to the
.kw signal
function (7.7.1.1).
.bigdesc
.np
The equivalent of
.millust begin
signal( SIGILL, SIG_DFL );
.millust end
.np
is executed prior to the call of the signal handler.
.*-------------------------
.section Terminating Newline Characters
.bigterm
Whether the last line of a text stream requires a terminating new-line
character (7.9.2).
.bigdesc
.np
The last line of a text stream does not require a terminating new-line
character.
.*-------------------------
.section Space Characters
.bigterm
Whether space characters that are written out to a text stream immediately
before a new-line character appear when read in (7.9.2).
.bigdesc
.np
All characters written out to a text stream will appear when read in.
.*-------------------------
.section Null Characters
.bigterm
The number of null characters that may be appended to data written
to a binary stream (7.9.2).
.bigdesc
.np
No null characters are appended to data written to a binary stream.
.*-------------------------
.section File Position in Append Mode
.bigterm
Whether the file position indicator of an append mode stream is initially
positioned at the beginning or end of the file (7.9.3).
.bigdesc
.np
When a file is open in append mode, the file position indicator
initially points to the end of the file.
.*-------------------------
.section Truncation of Text Files
.bigterm
Whether a write on a text stream causes the associated file to be
truncated beyond that point (7.9.3).
.bigdesc
.np
Writing to a text stream does not truncate the file beyond that point.
.*-------------------------
.section File Buffering
.bigterm
The characteristics of file buffering (7.9.3).
.bigdesc
.np
.if '&machsys' eq 'PP' .do begin
Disk files accessed through the standard I/O functions are fully buffered.
The default buffer size is 512 bytes.
.do end
.el .if '&machsys' eq 'QNX' .do begin
Disk files accessed through the standard I/O functions are fully buffered.
The default buffer size is 1024 bytes for both 16 and 32-bit systems.
.do end
.el .do begin
Disk files accessed through the standard I/O functions are fully buffered.
The default buffer size is 512 bytes for 16-bit systems, and 4096 bytes
for 32-bit systems.
.do end
.*-------------------------
.section Zero-Length Files
.bigterm
Whether a zero-length file actually exists (7.9.3).
.bigdesc
.np
A file with length zero can exist.
.*-------------------------
.section File Names
.bigterm
The rules of composing valid file names (7.9.3).
.bigdesc
.if '&machsys' eq 'PP' .do begin
.np
A valid file specification consists of an optional volume name (which is
always preceded by two backslashes), a series of optional directory names
(each preceded by one backslash), and a file name.
If a volume name or directory name precedes the file name, then the
file name must also be preceded by a backslash.
.np
Volume names, directory names, and file names can each contain up to
33 characters.
The maximum combined length of a file specification is 255 characters.
Case is ignored.
.do end
.el .if '&machsys' eq 'QNX' .do begin
.np
A valid file specification consists of an optional node name (which is
always preceded by two slashes), a series of optional directory names
(each preceded by one slash), and a file name.
If a node name or directory name precedes the file name, then the
file name must also be preceded by a slash.
.np
Directory names and file names can contain up to 48 characters.
Case is respected.
.do end
.el .do begin
.np
A valid file specification consists of an optional drive letter (which is
always followed by a colon), a series of optional directory names
separated by backslashes, and a file name.
.np
.us FAT File System:
Directory names and file names can contain up to eight characters
followed optionally by a period and a three letter extension.
The complete path (including drive, directories and file
name) cannot exceed 143 characters.
Case is ignored (lowercase letters are converted to uppercase
letters).
.np
.us HPFS File System:
Directory names and file names can contain up to 254 characters
in the OS/2 High Performance File System (HPFS).
However, the complete path (including drive, directories and file
name) cannot exceed 259 characters.
The period is a valid file name character and can appear in a file
name or directory name as many times as required; HPFS file names do
not require file extensions as in the FAT file system.
The HPFS preserves case in file names only in directory listings but
ignores case in file searches and other system operations (i.e, a
directory cannot have more than one file whose names differ only in
case).
.do end
.*-------------------------
.section File Access Limits
.bigterm
Whether the same file can be open multiple times (7.9.3).
.bigdesc
.np
It is possible to open a file multiple times.
.*-------------------------
.section Deleting Open Files
.bigterm
The effect of the
.kw remove
function on an open file (7.9.4.1).
.bigdesc
.np
The
.kw remove
function deletes a file, even if the file is open.
.*-------------------------
.section Renaming with a Name that Exists
.bigterm
The effect if a file with the new name exists prior to a call to the
.kw rename
function (7.9.4.2).
.bigdesc
.np
The
.kw rename
function
.if '&machsys' eq 'QNX' .do begin
will succeed if you attempt to rename a file using a name that exists.
.do end
.el .do begin
will fail if you attempt to rename a file using a name that exists.
.do end
.*-------------------------
.section Printing Pointer Values
.bigterm
The output for
.mono %p
conversion in the
.kw fprintf
function (7.9.6.1).
.bigdesc
.np
Two types of pointers are supported: near pointers (%hp), and
far pointers (%lp).
.if '&machsys' eq 'PP' .do begin
The
.kw fprintf
function produces hexadecimal values of the form XXXXXXXX for 32-bit near
pointers, and XXXX:XXXXXXXX (segment and offset separated by a colon) for
48-bit far pointers.
.do end
.el .do begin
The output for %p depends on the memory model being used.
.np
In 16-bit mode, the
.kw fprintf
function produces hexadecimal values of the form XXXX for 16-bit near
pointers, and XXXX:XXXX (segment and offset separated by a colon) for
32-bit far pointers.
.np
In 32-bit mode, the
.kw fprintf
function produces hexadecimal values of the form XXXXXXXX for 32-bit near
pointers, and XXXX:XXXXXXXX (segment and offset separated by a colon) for
48-bit far pointers.
.do end
.*-------------------------
.section Reading Pointer Values
.bigterm
The input for
.mono %p
conversion in the
.kw fscanf
function (7.9.6.2).
.bigdesc
.np
The
.kw fscanf
function converts hexadecimal values into the correct address when the %p
format specifier is used.
.*-------------------------
.section Reading Ranges
.bigterm
The interpretation of a
.mono -
character that is neither the first nor the last character in the
scanlist for
.mono %[
conversion in the
.kw fscanf
function (7.9.6.2).
.bigdesc
.np
The "&minus" character indicates a character range.
The character prior to the "&minus" is the first character in the range.
The character following the "&minus" is the last character in the range.
.*-------------------------
.section File Position Errors
.bigterm
The value to which the macro
.kw errno
is set by the
.kw fgetpos
or
.kw ftell
function on failure (7.9.9.1, 7.9.9.4).
.bigdesc
.np
When the function
.kw fgetpos
or
.kw ftell
fails, they set
.kw errno
to
.kw EBADF
if the file number is bad.
The constants are defined in the
.hdrfile errno.h
header file.
.*-------------------------
.section Messages Generated by the perror Function
.bigterm
The messages generated by the
.kw perror
function (7.9.10.4).
.bigdesc
.np
The
.kw perror
function generates the following messages.
.if '&machsys' eq 'PP' .do begin
.im strerror.pp
.do end
.el .if '&machsys' eq 'QNX' .do begin
.im strerror.qnx
.do end
.el .do begin
.im strerror.msg
.do end
.*-------------------------
.section Allocating Zero Memory
.bigterm
The behavior of the
.kw calloc
.ct,
.kw malloc
.ct,
or
.kw realloc
function if the size requested is zero (7.10.3).
.bigdesc
.np
The value returned will be
.kw NULL
.ct .li .
No actual memory is allocated.
.*-------------------------
.section The abort Function
.bigterm
The behavior of the
.kw abort
function with regard to open and temporary files (7.10.4.1).
.bigdesc
.np
The
.kw abort
function does not close any files that are open or temporary, nor
does it flush any output buffers.
.*-------------------------
.section The atexit Function
.bigterm
The status returned by the
.kw exit
function if the value of the argument is other than zero,
.kw EXIT_SUCCESS
.ct,
or
.kw EXIT_FAILURE
(7.10.4.3).
.bigdesc
.np
The
.kw exit
function returns the value of its argument to the operating system
regardless of its value.
.*-------------------------
.section Environment Names
.bigterm
The set of environment names and the method for altering the environment
list used by the
.kw getenv
function (7.10.4.4).
.bigdesc
.np
.if '&machsys' eq 'PP' .do begin
The set of environment names is unlimited.
.ix 'ENVIRON.INI'
Environment variables can be set in the system initialization file
"ENVIRON.INI".
There is no method provided by PenPoint for modifying the environment
list.
.do end
.el .if '&machsys' eq 'QNX' .do begin
The set of environment names is unlimited.
Environment variables can be set from the QNX command line using the
EXPORT or SET commands.
A program can modify its environment variables with the
.kw putenv
function.
Such modifications last only until the program terminates.
.do end
.el .do begin
The set of environment names is unlimited.
Environment variables can be set from the DOS command line using the
SET command.
A program can modify its environment variables with the
.kw putenv
function.
Such modifications last only until the program terminates.
.do end
.*-------------------------
.section The system Function
.bigterm
The contents and mode of execution of the string by the
.kw system
function (7.10.4.5).
.bigdesc
.np
The
.kw system
function
.if '&machsys' eq 'PP' .do begin
is not supported by PenPoint.
.do end
.el .if '&machsys' eq 'QNX' .do begin
always executes an executable binary or a shell file, using
.filename /bin/sh.
.do end
.el .do begin
executes an internal DOS, Windows, or OS/2 command, or an EXE, COM,
BAT or CMD file from within a C program rather than from the command
line.
The
.kw system
function
examines the
.kw COMSPEC
environment variable to find the command interpreter and passes the
argument string to the command interpreter.
.do end
.*-------------------------
.section The strerror Function
.bigterm
The contents of the error message strings returned by the
.kw strerror
function (7.11.6.2).
.bigdesc
.np
The
.kw strerror
function generates the following messages.
.if '&machsys' eq 'PP' .do begin
.im strerror.pp
.do end
.el .if '&machsys' eq 'QNX' .do begin
.im strerror.qnx
.do end
.el .do begin
.im strerror.msg
.do end
.*-------------------------
.section The Time Zone
.bigterm
The local time zone and Daylight Saving Time (7.12.1).
.bigdesc
.np
.if '&machsys' eq 'PP' .do begin
.ix 'ENVIRON.INI'
In the PenPoint SDK, the time zone is set in the system initialization
file "ENVIRON.INI".
For PenPoint 1.0, the default setting is "TZ=PST8PDT" (Pacific Standard Time
and Pacific Daylight Time, 8 hours later than Universal Coordinated Time).
For PenPoint 2.0, the default setting is "TZ=Toky-9" (Tokyo time, 9 hours
earlier than Universal Coordinated Time).
.do end
.el .if '&machsys' eq 'QNX' .do begin
The time zone is set in the system initialization file for your node,
( e.g.
.mono /etc/config/sysinit.2
). See the
.us QNX User's Guide.
.do end
.el .do begin
The default time zone is "Eastern Standard Time" (EST), and the
corresponding daylight saving time zone is "Eastern Daylight Saving Time"
(EDT).
.do end
.*-------------------------
.section The clock Function
.bigterm
The era for the
.kw clock
function (7.12.2.1).
.bigdesc
.np
The
.kw clock
function's era begins with a value of 0 when the program starts to execute.
.endbigterms
