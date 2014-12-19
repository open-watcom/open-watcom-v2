.section Header Files
.*
.np
The following header files are supplied with the C library. As has been
previously noted, when a library function is referenced in a source
file, the related header files (shown in the synopsis for that function)
should be included into that source file. The header files provide the
proper declarations for the functions and for the number and types of
arguments used with them. Constant values used in conjunction with the
functions are also declared. The files can be included multiple times
and in any order.
.if '&machsys' eq 'QNX' .do begin
.np
When the &company C compiler option "za" is used ("ISO/ANSI C conformance"),
the macro
.kw NO_EXT_KEYS
is predefined. The "za" option is used when you are creating an
application that must conform to a certain standard, whether it be ISO/ANSI
or POSIX. The effect on the inclusion of ISO/ANSI- and POSIX-defined header
files is that certain portions of the header files are omitted. For ISO/ANSI
header files, these are the portions that go beyond the ISO/ANSI standard.
For POSIX header files, these are the portions that go beyond the POSIX
standard. Feature test macros may then be defined to select those
portions which are omitted. Two feature test macros may be defined.
.begterm 17
.term _POSIX_SOURCE
Include those portions of the ISO/ANSI header files which relate to the
POSIX standard
.us (IEEE Standard Portable Operating System Interface for Computer
.us Environments - POSIX 1003.1)
.term _QNX_SOURCE
Include those portions of the ISO/ANSI and POSIX header files which relate
to the POSIX standard and all extensions provided by the QNX system.
In essence, the definition of
.kw _QNX_SOURCE
before any header files are included is equivalent to omitting the
specification of the "za" compiler option.
Note that when
.kw _QNX_SOURCE
is defined, it encompasses
.kw _POSIX_SOURCE
so it is not necessary to define
.kw _POSIX_SOURCE
also.
.endterm
.np
Feature test macros may be defined on the command line or in the source
file before any header files are included. The latter is illustrated in
the following example in which an ISO/ANSI and POSIX conforming application
is being developed.
.millust begin
    #define _POSIX_SOURCE
    #include <limits.h>
    #include <stdio.h>
      .
      .
      .
    #if defined(_QNX_SOURCE)
      #include "non_POSIX_header1.h"
      #include "non_POSIX_header2.h"
      #include "non_POSIX_header3.h"
    #endif
.millust end
.pc
The source code is then compiled using the "za" option.
.np
The following ISO/ANSI header files are affected by the
.kw _POSIX_SOURCE
feature test macro.
.millust begin
    limits.h
    setjmp.h
    signal.h
    stdio.h
    stdlib.h
    time.h
.millust end
.np
The following ISO/ANSI and POSIX header files are affected by the
.kw _QNX_SOURCE
feature test macro.
.millust begin
    ctype.h              (ISO/ANSI)
    env.h                (POSIX)
    fcntl.h              (POSIX)
    float.h              (ISO/ANSI)
    limits.h             (ISO/ANSI)
    math.h               (ISO/ANSI)
    process.h            (extension to POSIX)
    setjmp.h             (ISO/ANSI)
    signal.h             (ISO/ANSI)
    sys/stat.h           (POSIX)
    stdio.h              (ISO/ANSI)
    stdlib.h             (ISO/ANSI)
    string.h             (ISO/ANSI)
    termios.h            (POSIX)
    time.h               (ISO/ANSI)
    sys/types.h          (POSIX)
    unistd.h             (POSIX)
.millust end
.do end
.*
.beglevel
.*
.section Header Files in &hdrdir.
.*
.np
The following header files are provided with the software. The header
files that are located in the
.id &hdrdirup.
directory are described first.
.dfbeg
.*------------------------------
.df assert.h
This ISO C90 header file is required when an
.kw assert
macro is used. These assertions will be ignored when the identifier
.mono NDEBUG
is defined.
.*------------------------------
.if '&machsys' ne 'QNX' .do begin
.df bios.h
This header file declares all BIOS related functions.
.do end
.*------------------------------
.df conio.h
.ix '&KbIo'
.ix '&PortIo'
This header file declares console and
Intel 80x86 port input/output functions.
.*------------------------------
.df ctype.h
This ISO C90 header file declares functions that perform character
classification and case conversion operations. Similar functions for
wide characters are declared in <wctype.h>.
.*------------------------------
.if '&machsys' ne 'QNX' .do begin
.df direct.h
This header file declares functions related to directories and the type
.kw DIR
which describes an entry in a directory.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df dirent.h
This POSIX header file declares functions related to directories and
the type
.kw DIR
which describes an entry in a directory.
.do end
.*------------------------------
.if '&machsys' ne 'QNX' .do begin
.df dos.h
This header file declares functions that interact with DOS. It includes
the definitions of the
.kw FP_OFF
.ct,
.kw FP_SEG
and
.kw MK_FP
macros, and for the following structures and unions:
.sfbeg
.sf DOSERROR
describes the DOS error information.
.sf REGS
describes the CPU registers for Intel 8086 family.
.sf SREGS
describes the segment registers for the Intel 8086 family.
.sf REGPACK
describes the CPU registers and segment registers for Intel 8086 family.
.sf INTPACK
describes the input parameter to an "interrupt" function.
.sfend
.do end
.*------------------------------
.df env.h
This POSIX header file declares environment string functions.
.*------------------------------
.df errno.h
.ix '&Errs'
This ISO C90 header file provides the
.kw extern
declaration for error variable
.kw errno
and provides the symbolic names for error codes that can be placed in
the error variable.
.*------------------------------
.df fcntl.h
This POSIX header file defines the flags used by the
.if '&machsys' ne 'QNX' .do begin
.kw open
and
.kw sopen
functions. The function declarations for these functions are found in the
.hdrfile &iohdr
header file.
.do end
.if '&machsys' eq 'QNX' .do begin
.kw creat
.kw fcntl
.ct,
.kw open
.ct,
and
.kw sopen
functions.
.do end
.*------------------------------
.df fenv.h
.ix 'Floating Point Environment'
This ISO C99 header file defines several types and declares several
functions that give access to the floating point environment. These
functions can be used to control status flags and control modes in the
floating point processor.
.*------------------------------
.df float.h
This ISO C90 header file declares constants related to floating-point
numbers, declarations for low-level floating-point functions, and the
declaration of the floating-point exception codes.
.*------------------------------
.df fnmatch.h
This header file declares the pattern matching function
.kw fnmatch
.*------------------------------
.df graph.h
This header file contains structure definitions and function
declarations for the &company C Graphics library functions.
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df grp.h
This POSIX header file contains structure definitions and function
declarations for group operations.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df i86.h
This header file is used with functions that interact with the Intel
architecture. It defines the structs and unions used to handle the input
and output registers for the Intel 80x86 and 80386/80486 interrupt
interface routines. It includes prototypes for the interrupt functions,
definitions for the
.kw FP_OFF
.ct,
.kw FP_SEG
and
.kw MK_FP
macros, and definitions for the following structures and unions:
.sfbeg
.sf REGS
describes the CPU registers for Intel 8086 family.
.sf SREGS
describes the segment registers for the Intel 8086 family.
.sf REGPACK
describes the CPU registers and segment registers for Intel 8086 family.
.sf INTPACK
describes the input parameter to an "interrupt" function.
.sfend
.do end
.*------------------------------
.df inttypes.h
This ISO C99 header file includes <stdint.h> and expands on it by
definition macros for printing and scanning specific sized integer
types. This header also declares several functions for manipulating
maximum sized integers.
.np
Note that the format macros are not visible in C++ programs unless the
macro
.kw __STDC_FORMAT_MACROS
is defined.
.*------------------------------
.if '&machsys' ne 'QNX' .do begin
.df io.h
This header file declares functions that perform input/output
operations at the operating system level. These functions use file
&handle.s to reference files or devices. The function
.kw fstat
is declared in the
.hdrfile sys/stat.h
header file.
.do end
.*------------------------------
.df limits.h
This ISO C90 header file contains constant declarations for limits or
boundary values for ranges of integers and characters.
.*------------------------------
.df locale.h
This ISO C90 header file contains declarations for the categories
.mono (LC...)
of locales which can be selected using the
.kw setlocale
function which is also declared.
.*------------------------------
.df malloc.h
This header file declares the memory allocation and deallocation
functions.
.*------------------------------
.df math.h
.ix '&Math'
This ISO/ANSI header file declares the mathematical functions (which operate
with floating-point numbers) and the structures:
.sfbeg
.sf exception
describes the exception structure passed to the
.kw matherr
function; symbolic constants for the types of exceptions are included
.sf complex
declares a complex number
.sfend
.*------------------------------
.if &version ge 107 .do begin
.df mmintrin.h
.ix 'Multimedia Extension'
.ix 'MMX'
.ix 'IA MMX'
This header file declares functions that interact with the Intel
Architecture Multimedia Extensions. It defines the datatype used to
store multimedia values:
.sfbeg
.sf __m64
describes the 64-bit multimedia data element. Note: the underlying
implementation details of this datatype are subject to change. Other
compilers may implement a similar datatype in a different manner.
.sfend
.np
It also contains prototypes for multimedia functions and pragmas for the
in-line generation of code that operates on multimedia registers.
.do end
.*------------------------------
.df process.h
.ix '&Process'
This header file declares the
.kw spawn&grpsfx
functions, the
.kw exec&grpsfx
functions, and the
.kw system
function. The file also contains declarations for the constants
.kw P_WAIT
.ct,
.kw P_NOWAIT
.ct,
.kw P_NOWAITO
.ct,
and
.kw P_OVERLAY
.ct .li .
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df pwd.h
This POSIX header file contains structure definitions and function
declarations for password operations.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df regex.h
This header file contains structure definitions and function
declarations for regular expression handling.
.do end
.*------------------------------
.df search.h
This header file declares the functions
.kw lfind
and
.kw lsearch
.*------------------------------
.df setjmp.h
.ix 'Non-local jumps'
This ISO C90 header file declares the
.kw setjmp
and
.kw longjmp
functions.
.*------------------------------
.df share.h
This header file defines constants for shared access to files using the
.kw sopen
function.
.*------------------------------
.df signal.h
This ISO C90 header file declares the
.kw signal
and
.kw raise
functions.
.*------------------------------
.df stdarg.h
This ISO C90 header file defines the macros which handle variable argument
lists.
.*------------------------------
.df stdbool.h
This ISO C99 header file defines the macro
.kw bool
and the macros
.kw true
and
.kw false
for use in C programs. If this header is included in a C++ program there
is no effect. The C++ reserved words will not be redefined. However the
definition of
.kw bool
,
.kw true
, and
.kw false
used in a C program will be compatible with their C++ counterparts. In
particular, a C function declared as taking a
.kw bool
parameter and a structure containing a
.kw bool
member can both be shared between C and C++ without error.
.*------------------------------
.df stddef.h
This ISO C90 header file defines a few popular constants and types
including
.mono NULL
(null pointer),
.kw size_t
(unsigned size of an object),
and
.kw ptrdiff_t
(difference between two pointers).
It also contains a declaration for the
.kw offsetof
macro.
.*------------------------------
.df stdint.h
This ISO C99 header file defines numerous type names for integers of
various sizes. Such type names provide a reasonably portable way to
refer to integers with a specific number of bits. This header file also
defines macros that describe the minimum and maximum values for these
types (similar to the macros in limits.h), and macros for writing
integer constants with specific sized types.
.np
Note that in C++ programs the limit macros are not visible unless the
macro
.kw __STDC_LIMIT_MACROS
is defined. Similarly the constant writing macros are not visible unless
the macro
.kw __STDC_CONSTANT_MACROS
is defined.
.*------------------------------
.df stdio.h
This ISO C90 header file declares the standard input/output functions.
Files, devices and directories are referenced using pointers to objects
of the type
.kw FILE
.ct .li .
.*------------------------------
.df stdlib.h
This ISO C90 header file declares many standard functions excluding those
declared in other header files discussed in this section.
.*------------------------------
.df string.h
This ISO C90 header file declares functions that manipulate strings or
blocks of memory.
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df tar.h
This POSIX header file contains header block information for the tar
format.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df term.h
This header file contains terminal information definitions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df termios.h
This POSIX header file contains terminal I/O system types.
.do end
.*------------------------------
.df time.h
.ix '&TimeFunc'
.ix 'tm'
This ISO/ANSI header file declares functions related to times and dates and
defines the structure
.kw struct tm
.ct .li .
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df unistd.h
This POSIX header file declares functions that perform input/output
operations at the operating system level. These functions use file
&handle.s to reference files or devices. The function
.kw fstat
is declared in the
.hdrfile sys/stat.h
header file.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df unix.h
This header file contains definitions that aid in porting traditional
UNIX code.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df utime.h
This POSIX header file declares the
.kw utime
function and defines the structure
.kw utimbuf
that is used by it.
.do end
.*------------------------------
.df varargs.h
This UNIX System V header file provides an alternate way of handling
variable argument lists. The equivalent ISO/ANSI header file is <stdarg.h>.
.*------------------------------
.df wchar.h
This ISO C99 header file defines several data types including
.kw wchar_t
.ct ,
.kw size_t
.ct ,
.kw mbstate_t
(an object that can hold conversion state information necessary to
convert between multibyte characters and wide characters),
.kw wctype_t
(a scalar type that can hold values which represent locale-specific
character classification),
and
.kw wint_t
which is an integral type that can hold any
.kw wchar_t
value as well as
.kw WEOF
(a character that is not in the set of "wchar_t" characters
and that is used to indicate
.us end-of-file
on an input stream). The functions that are declared in this header file
are grouped as follows:
.begbull
.bull
Wide character classification and case conversion.
.bull
Input and output of wide characters, or multibyte characters, or both.
.bull
Wide string numeric conversion.
.bull
Wide string manipulation.
.bull
Wide string data and time conversion.
.bull
Conversion between multibyte and wide character sequences.
.endbull
.*------------------------------
.df wctype.h
This ISO C99 header file declares functions that perform characater
classification and case conversion operations on wide characters.
Similar functions for ordinary characters are declared in <ctype.h>.
.*------------------------------
.dfend
.*
.section Header Files in &hdrsys.
.*
.np
The following header files are present in the
.kw sys
subdirectory.
Their presence in this directory indicates that they are
system-dependent header files.
.dfbeg
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/con_msg.h
This header file contains definitions for the console driver.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/console.h
This header file contains "public" definitions for the console driver.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/debug.h
This header file contains debugger data structures.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/dev.h
This header file contains "public" device administrator definitions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/dev_msg.h
This header file contains "public" device driver messages.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/disk.h
This header file contains non-portable file system definitions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/dumper.h
This header file contains the dumper file structure.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/fd.h
This header file contains file descriptor data structures.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/fsys.h
This header file contains non-portable file system definitions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/fsysinfo.h
This header file contains declarations related to the fsysinfo()
function.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/fsys_msg.h
This header file contains non-portable file system message
definitions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/inline.h
Contains handy pragmas that are often used when doing low-level
programming.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/io_msg.h
This header file contains non-portable low-level I/O definitions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/irqinfo.h
This header file contains structure definitions and prototypes for
interrupt request functions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/kernel.h
This header file contains prototypes and pragmas for kernel function
calls.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/lmf.h
This header file contains structure definitions for load module
format.
.do end
.*------------------------------
.df sys/locking.h
This header file contains the manifest constants used by the
.kw locking
function.
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/magic.h
This header file contains a definition for the
.kw _magic
structure.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/mman.h
This header file contains declarations related to the memory mapping
functions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/mouse.h
This header file contains structure definitions and prototypes for
mouse operations.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/mous_msg.h
This header file contains "private" definitions for the mouse driver.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/name.h
This header file contains structure definitions and prototypes for QNX
"name" functions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/osinfo.h
This header file contains manifests, structure definitions and
prototypes for operating system information.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/osstat.h
This header file contains manifests, structure definitions and
prototypes for operating system status information.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/prfx.h
This header file contains file prefix prototypes.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/proc_msg.h
This header file contains process data structures and definitions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/proxy.h
This header file contains proxy process prototypes.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/psinfo.h
This header file contains manifests and structure definitions for
process information.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/qioctl.h
This header files contains manifests and structures for common
qnx_ioctl messages.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/qnx_glob.h
This header file contains a structure definition for the QNX process
spawning global data area.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/qnxterm.h
This header file contains terminal capability definitions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/sched.h
This header file contains manifests and prototypes for process
scheduling.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/seginfo.h
This header file contains segment information data structures.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/select.h
This header file contains the prototype for the
.kw select
function.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/sendmx.h
This header file contains a definition for
.kw _setmx
and a definition
of the
.kw _mxfer_entry
structure.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/ser_msg.h
This header file contains "public" serial driver messages.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/sidinfo.h
This header file contains session information data structures.
.do end
.*------------------------------
.df sys/stat.h
This POSIX header file contains the declarations pertaining to file
status, including definitions for the
.kw fstat
and
.kw stat
functions and for the structure:
.sfbeg
.sf stat
describes the information obtained for a directory,
file or device
.sfend
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/sys_msg.h
This header file contains standard system message definitions.
.do end
.*------------------------------
.df sys/timeb.h
This header file describes the
.kw timeb
structure used in conjunction with the
.kw ftime
function.
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/timers.h
This POSIX header file contains interval timer definitions from POSIX
1003.4.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/times.h
This POSIX header file contains process timing definitions from POSIX
1003.1.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/trace.h
This header file contains trace data structures and definitions.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/tracecod.h
This header file contains the trace codes used by the Trace()
functions.
.do end
.*------------------------------
.df sys/types.h
This POSIX header file contains declarations for the types used by
system-level calls to obtain file status or time information.
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/uio.h
This header file contains declarations related to the readv() and
writev() functions.
.do end
.*------------------------------
.if '&machsys' ne 'QNX' .do begin
.df sys/utime.h
This POSIX header file contains a declaration for the
.kw utime
function and for the structured type
.kw utimbuf
used by it.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/utsname.h
This POSIX header file contains a definition of the
.kw utsname
structure and
a prototype for the
.kw uname
function.
.do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/vc.h
This header file contains manifests and prototypes for virtual circuit
functions.
.do end
.*------------------------------
:cmt. .if '&machsys' eq 'QNX' .do begin
:cmt. .df sys/vc_msg.h
:cmt. This header file contains message formats for virtual circuit
:cmt. functions.
:cmt. .do end
.*------------------------------
.if '&machsys' eq 'QNX' .do begin
.df sys/wait.h
This POSIX header file contains manifests and prototypes for "wait"
functions.
.do end
.dfend
.if '&machsys' eq 'QNX' .do begin
.*
.section Header Files Provided for Compatibility
.*
.np
The following headers are included in order to resolve references to
items found on other operating systems.
They may be helpful when porting code.
.dfbeg
.df /usr/include/ftw.h
.df /usr/include/ioctl.h
.df /usr/include/libc.h
.df /usr/include/sgtty.h
.df /usr/include/shadow.h
.df /usr/include/termcap.h
.df /usr/include/termio.h
.df /usr/include/ustat.h
.df /usr/include/utmp.h
.df /usr/include/sys/dir.h
.df /usr/include/sys/file.h
.df /usr/include/sys/ioctl.h
.df /usr/include/sys/statfs.h
.df /usr/include/sys/termio.h
.df /usr/include/sys/time.h
.dfend
.do end
.*
.endlevel
