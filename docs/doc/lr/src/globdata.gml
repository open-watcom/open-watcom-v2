.section Global Data
.*
.np
Certain data items are used by the &company C/C++ run-time library and
may be inspected (or changed in some cases) by a program.
The defined items are:
.dfbeg
.df _amblksiz
.proto stdlib.h
This
.id unsigned int
data item contains the increment by which the "break" pointer for
memory allocation will be advanced when there is no freed block large
enough to satisfy a request to allocate a block of memory.
This value may be changed by a program at any time.
.if &version ge 107 .do begin
.df __argc
.proto stdlib.h
This
.id int
item contains the number of arguments passed to
.kw main
.ct .li .
.df __argv
.proto stdlib.h
This
.id char **
item contains a pointer to a vector containing the actual arguments
passed to
.kw main
.ct .li .
.do end
.df daylight
.proto time.h
This
.id unsigned int
has a value of one when daylight saving time is supported in this
locale and zero otherwise.
Whenever a time function is called, the
.kw tzset
function is called to set the value of the variable.
The value will be determined from the value of the
.mono TZ
environment variable.
.if '&machsys' ne 'QNX' .do begin
.df _doserrno
.proto stdlib.h
This
.id int
item contains the actual error code returned when a DOS, Windows or
OS/2 function fails.
.do end
.df environ
.proto stdlib.h
This
.id char ** __near
data item is a pointer to an array of character pointers to the
environment strings.
.cp 10
.df errno
.proto errno.h
.ix '&Errs'
This
.id int
item contains the number of the last error that was detected.
The run-time library never resets
.kw errno
to 0.
Symbolic names for these errors are found in the
.hdrfile errno.h
header file.
See the descriptions for the
.kw perror
and
.kw strerror
functions for information about the text which describes these errors.
.df fltused_
.ix 'undefined references' 'fltused_'
The C compiler places a reference to the
.kw fltused_
symbol into any module that uses a floating-point library routine or
library routine that requires floating-point support (e.g., the use of
a
.id float
or
.id double
as an argument to the
.kw printf
function).
.if '&machsys' ne 'QNX' .do begin
.df _fmode
.proto stdlib.h
This data item contains the default type of file (text or binary)
translation for a file.
It will contain a value of either
.begnote
.setptnt 0 12
.note O_BINARY
.ix 'binary files'
indicates that data is transmitted to and from streams unchanged.
.note O_TEXT
.ix 'text files'
indicates that carriage return characters are added before linefeed
characters on output operations and are removed on input operations
when they precede linefeed characters.
.endnote
.np
These values are defined in the
.hdrfile fcntl.h
header file.
The value of
.kw _fmode
may be changed by a program to change the default behavior of the
.kw open
.ct,
.kw fopen
.ct,
.kw creat
and
.kw sopen
functions.
The default setting of
.kw _fmode
is
.kw O_TEXT
.ct , for text-mode translation.
.kw O_BINARY
is the setting for binary mode.
.*
.if &version ge 110 .do begin
You can change the value of
.kw _fmode
in either of two ways:
.ix 'BINMODE.OBJ'
.begbull
.bull
You can include the object file
.filename BINMODE.OBJ
when linking your application.
This object file contains code to change the initial setting of
.kw _fmode
to
.kw O_BINARY
.ct , causing all files except
.kw stdin
.ct ,
.kw stdout
.ct , and
.kw stderr
to be opened in binary mode.
.bull
You can change the value of
.kw _fmode
directly by setting it in your program.
.endbull
.do end
.*
.do end
.if '&machsys' ne 'QNX' .do begin
.df __MaxThreads
There is a limit to the number of threads an application can create
under 16-bit OS/2 and 32-bit NetWare.
The default limit is 32.
This limit can be adjusted by statically initializing the unsigned
global variable
.id __MaxThreads.
.np
Under 32-bit OS/2, there is no limit to the number of threads an
application can create.
However, due to the way in which multiple threads are supported in the
&company libraries, there is a small performance penalty once the
number of threads exceeds the default limit of 32 (this number
includes the initial thread).
If you are creating more than 32 threads and wish to avoid this
performance penalty, you can redefine the threshold value of 32.
You can statically initialize the global variable
.id __MaxThreads.
.np
By adding the following line to your multi-threaded application, the
new threshold value will be set to 48.
.millust begin
unsigned __MaxThreads = { 48 };
.millust end
.do end
.if '&machsys' ne 'QNX' .do begin
.df __minreal
.proto stdlib.h
This data item contains the minimum amount of real memory (below 640K)
to reserve when running a 32-bit DOS extended application.
.do end
.df optarg
.proto unistd.h
This 
.id char *
variable contains a pointer to an option-argument parsed by the
.kw getopt
function.
.df opterr
.proto unistd.h
This
.id int
variable controls whether the
.kw getopt
function will print error messages. The default value is non-zero and
will cause the
.kw getopt
function to print error messages on the console.
.df optind 
.proto unistd.h
This
.id int
variable holds the index of the argument array element currently
processed by the
.kw getopt
function.
.df optopt
.proto unistd.h
This
.id int
variable contains the unrecognized option character in case the
.kw getopt
function returns an error.
.df _osmajor
.proto stdlib.h
.if '&machsys' eq 'QNX' .do begin
This
.id unsigned char
variable contains the major number for the version of QNX executing on
the computer.
If the current version is 4.10, then the value will be 4.
.do end
.el .do begin
This
.id unsigned char
variable contains the major number for the version of DOS executing on
the computer.
If the current version is 3.20, then the value will be 3.
.do end
.df _osminor
.proto stdlib.h
.if '&machsys' eq 'QNX' .do begin
This
.id unsigned char
variable contains the minor number for the version of QNX executing on
the computer.
If the current version is 4.10, then the value will be 10.
.do end
.el .do begin
This
.id unsigned char
variable contains the minor number for the version of DOS executing on
the computer.
If the current version is 3.20, then the value will be 20.
.do end
.if '&machsys' ne 'QNX' .do begin
.df _osbuild
(Win32 only)
.proto stdlib.h
This
.id unsigned short
variable contains the operating system build number for the version of
Windows executing on the computer.
.do end
.if '&machsys' ne 'QNX' .do begin
.if &version ge 110 .do begin
.df _osver
(Win32 only)
.proto stdlib.h
This
.id unsigned int
variable contains the operating system build number for the version of
Windows executing on the computer.
.np
On Win32s or Windows 95/98 platforms, the high bit of the low-order
16-bit word is turned on.
Windows 95/98 do not have build numbers.
.code begin
unsigned short dwBuild;

// Get build numbers for Win32 or Win32s

if( _osver < 0x8000 )       // Windows NT/2000
    dwBuild = _osver;
else if (_winmajor < 4)     // Win32s
    dwBuild = _osver & ~0x8000;
else                        // Windows 95 or 98
    dwBuild =  0;           // No build numbers provided
.code end
.np
Note that the Win32
.kw GetVersionEx
function is the preferred method for obtaining operating system
version number information.
.do end
.do end
.if '&machsys' ne 'QNX' .do begin
.df _osmode
(16-bit only)
.proto stdlib.h
This
.id unsigned char
variable contains either the value DOS_MODE which indicates the program
is running in real address mode, or it contains the value OS2_MODE
which indicates the program is running in protected address mode.
.do end
.if '&machsys' ne 'QNX' .do begin
.df _psp
.proto stdlib.h
.ix 'DOS' 'Program Segment Prefix'
.ix 'DOS' 'PSP'
.ix 'Program Segment Prefix'
.ix 'PSP'
This data item contains the segment value for the DOS Program Segment
Prefix.
Consult the technical documentation for your DOS system for the
process information contained in the Program Segment Prefix.
.do end
.if '&machsys' ne 'QNX' .do begin
.df _stacksize
On 16-bit 80x86 systems, this
.id unsigned int
value contains the size of the stack for a TINY memory model program.
Changing the value of this item during the execution of a program
will have no effect upon the program, since the value is used when
the program starts execution.
To change the size of the stack to be 8K bytes, a statement such as
follows can be included with the program.
.millust begin
unsigned int _stacksize = { 8 * 1024 };
.millust end
.do end
.if '&machsys' ne 'QNX' .do begin
.df stdaux
.proto stdio.h
This variable (with type
.mono FILE *)
indicates the standard auxiliary port
(not available in some Windows platforms).
.do end
.df stderr
.proto stdio.h
This variable (with type
.mono FILE *)
indicates the standard error stream (set to the
console by default).
.df stdin
.proto stdio.h
This variable (with type
.mono FILE *)
indicates the standard input stream (set to the
console by default).
.df stdout
.proto stdio.h
This variable (with type
.mono FILE *)
indicates the standard output stream (set to the
console by default).
.if '&machsys' ne 'QNX' .do begin
.df stdprn
.proto stdio.h
This variable (with type
.mono FILE *)
indicates the standard printer.
(not available in some Windows platforms).
.do end
.if '&machsys' ne 'QNX' .do begin
.df sys_errlist
.proto stdlib.h
This variable is an array of pointers to character strings for each
error code defined in the
.hdrfile errno.h
header file.
.do end
.if '&machsys' ne 'QNX' .do begin
.df sys_nerr
.proto stdlib.h
This
.id int
variable contains the number of messages declared in
.kw sys_errlist
.ct .li .
.do end
.if '&machsys' ne 'QNX' .do begin
.df _threadid
.proto stddef.h
This variable/function may be used to obtain the id of the current
thread which is an
.id int.
In the 32-bit libraries,
.kw _threadid
is a function that returns a pointer to an int.
In the 16-bit libraries,
.kw _threadid
is a far pointer to an int.
Note that the value stored where
.kw _threadid
points does not necessarily change when a thread context switch occurs
(so do not make a copy of the pointer ... it may change).
To obtain the current thread identifier, simply code:
.millust begin
int tid = *_threadid;
.millust end
.do end
.df timezone
.proto time.h
This
.id long int
contains the number of seconds of time that the local time zone is
earlier than Coordinated Universal Time (UTC) (formerly known as
Greenwich Mean Time (GMT)).
Whenever a time function is called, the
.kw tzset
function is called to set the value of the variable.
The value will be determined from the value of the
.mono TZ
environment variable.
.df tzname
.proto time.h
This array of two pointers to character strings indicates the name of
the standard abbreviation for the time zone and the name of the
abbreviation for the time zone when daylight saving time is in effect.
Whenever a time function is called, the
.kw tzset
function is called to set the values in the array.
These values will be determined from the value of the
.mono TZ
environment variable.
.if '&machsys' ne 'QNX' .do begin
.df __wargc
.proto stdlib.h
This
.id int
item contains the number of arguments passed to
.kw wmain
.ct .li .
.df __wargv
.proto stdlib.h
This
.id wchar_t **
item contains a pointer to a vector containing the actual arguments
passed to
.kw wmain
.ct .li .
.df _wenviron
.proto stdlib.h
This
.id wchar_t ** __near
data item is a pointer to an array of wide-character pointers to the
wide-character equivalents of the environment strings.
.df __win_alloc_flags
.proto stdlib.h
This
.id unsigned long int
variable contains the flags to be used when allocating memory in
Windows.
.df __win_realloc_flags
.proto stdlib.h
This
.id unsigned long int
variable contains the flags to be used when reallocating memory in
Windows.
.do end
.if '&machsys' ne 'QNX' .do begin
.df _winmajor
(Win32 only)
.proto stdlib.h
This
.id unsigned int
variable contains the operating system major version number for the
version of Windows executing on the computer.
For example, the major version number of the Daytona release of
Windows NT is 3.
.np
Note that the Win32
.kw GetVersionEx
function is the preferred method for obtaining operating system
version number information.
.do end
.if '&machsys' ne 'QNX' .do begin
.df _winminor
(Win32 only)
.proto stdlib.h
This
.id unsigned int
variable contains the operating system minor version number for the
version of Windows executing on the computer.
For example, the minor version number of the Daytona release of
Windows NT is 5.
.np
Note that the Win32
.kw GetVersionEx
function is the preferred method for obtaining operating system
version number information.
.do end
.if '&machsys' ne 'QNX' .do begin
.df _winver
(Win32 only)
.proto stdlib.h
This
.id unsigned int
variable contains the operating system version number for the version
of Windows executing on the computer.
The low-order byte contains the minor version number
(see also
.kw _winminor
.ct ).
The next byte contains the major version number
(see also
.kw _winmajor
.ct ).
The high-order word contains no useful information.
.np
Note that the Win32
.kw GetVersionEx
function is the preferred method for obtaining operating system
version number information.
.do end
.dfend
