.chap *refid=diffs Release Notes for &product &ver..&rev
.*
.np
There are a number of enhancements and changes in this new version of
&product..
The following sections outline most of the major changes.
You should consult the User's Guide for full details on these and
other changes made to the compiler and related tools.
You should check the next section to determine if you need to
recompile your application.
.*
.if '&lang' eq 'C/C++' .do begin
:cmt. Reflects main Perforce branch as of 2010/03/05
:cmt. Good way to get list of changes since certain date:
:cmt. p4 changes -l @yyyy/mm/dd,#head
.*
.*
.section Differences from Open Watcom version 1.8
.*
.*
.np
Following is a list of changes made in &product 1.9:
.begbull
.bull
The NULL macro is now defined as ((void *)0) for C; for C++, NULL is still
defined as 0 or 0L as previously. This change may require modifications to
user code if NULL was incorrectly used as an integer constant. Note: The
16-bit windows.h header defines NULL as 0; for 16-bit Windows programming,
the effective definition of NULL depends on whether windows.h or one of the
standard C headers is included first.
.bull
The C compiler has improved handling of the _Bool type (C99 mode specific).
The _Bool type can now generally be used in expressions wherever an integer
may be used.
.bull
The C compiler now considers enumeration types to be compatible with their
underlying integer type, as required by ISO C. Note that the choice of the
underlying type varies and should not be relied upon.
.bull
The C compiler now correctly handles initialization of static
floating-point variables with 64-bit integer constants.
.bull
The C and C++ compilers now support the __int8, __int16, and __int32
extension keywords.
.bull
The 16-bit C and C++ compilers now allow the :> operator to be used in
initializers of variables with static storage, as long as both operands are
integer constants. This enables the use of MK_FP macro in initializers.
.bull
The C++ compiler now treats warning W737, implicit conversion of pointers
to integral types of same size, as an error.
.bull
The C++ compiler now supports several new warnings: W931, warn about
meaningless cv-qualifier in casts; W932, warn about meaningless
cv-qualifier in function return types; and W933, warn about use of C-style
casts in C++ code. These warnings must be explicitly enabled through the
-wce option.
.bull
The code generator now eliminates redundant epilogue code for 32-bit Intel
processors when doing size optimization.
.bull
The code generator correctly handles constant folding of signed 64-bit
integer division and right shift operations.
.bull
The code generator now supports constant folding of 64-bit integer modulo
operations.
.bull
The code generator no longer mishandles floating-point comparisons where
one operand is a variable of type float and the other operand is a constant
of type double or long double.
.bull
The code generator no longer produces incorrect code when a constant
expression of type float (e.g., 1.0f + 1.0f) is passed as an argument to a
function which takes a float argument.
.bull
The code generator now makes more accurate decision when choosing whether a
multiplication by a constant should be replaced by a sequence of shifts and
additions. Results depend on target CPU type.
.bull
The 386 code generator now produces a CDQ instruction except when targeting
a Pentium and optimizing for speed, when a MOV/SAR sequence is emitted as
previously when converting a signed 32-bit integer to 64-bit.
.bull
The code generator no longer emits redundant CS segment overrides when
creating calls to symbols imported from DLLs.
.bull
The Win32 API headers and import libraries have been updated to support the
new interfaces in Windows 7.
.bull
Support for 16-bit OLE 2.0 has been added.
.bull
Support for RDOS targets has been added.
.bull
Support for ZDOS targets has been added to the linker.
.bull
The floating-point exception handler for 16-bit DOS no longer crashes if
the user signal handler modified DS.
.bull
The _floodfill() graphics library function now works correctly again.
.bull
The library no longer leaks memory when a thread created by _beginthread()
terminates.
.bull
The %Lf format specifier (and related format specifiers) used with printf
family functions now works.
.bull
The library now contains _fseeki64 and _ftelli64 functions to handle 64-bit
file offset pointer for streams.
.bull
The library implementations of _lseeki64, _telli64, _fileleni64,
_(w)stati64, _(w)findfirsti64, _(w)findnexti64 on OS/2 now properly use
64-bit file sizes and offsets.
.bull
The library implementations of puts and putws now correctly return EOF if
they fail
.bull
DOS long file name (LFN) support has been added (new doslfn??.lib model
specific libraries contain DOS LFN version of appropriate modules). By
default DOS LFN support is enabled. It can be switch off by setup LFN=N
environment variable.
.bull
The wasm assembler now implements support for Turbo Assembler compatible
IDEAL mode. The -zcm option may be used to select this feature.
.bull
The assembler now supports built-in @code and @data symbols.
.bull
The assembler now supports ELSEIF directives for conditional assembly.
.bull
The assembler no longer incorrectly assumes that absolute segments are byte
aligned; they are paragraph (16 byte) aligned.
.bull
The built-in cd command in wmake now supports quoted paths with spaces.
.bull
The linker now supports libraries in BSD, COFF, and GNU ar format.
.bull
The linker now allows stack sizes smaller than 512 bytes to be specified
and does not silently override them. However, a new warning (W1172) will be
emitted in such case.
.bull
A FULLHEADER option specific to the DOS EXE format has been added to the
linker. Please see the Linker Guide for further information.
.bull
The library manager (wlib) now supports BSD, COFF, and GNU variants of the
ar library archive format. Extended -fab, -fag, -fac options select the
variant. The -fa option now selects default format depending on host
platform.
.bull
This release introduces Open Watcom's IPF compiler (wipfc) used for
creating help files on OS/2.
.bull
Documentation is now provided in Windows CHM format as an option.
.bull
The vi editor now supports non-ASCII characters in the 0x80-0xFF range.
.bull
The console version of the vi editor for Windows now uses standard Windows
keyboard processing; AltGr and dead keys now function correctly.
Additionally, Caps Lock is only effective for keys which generate
characters.
.bull
The editor now supports for syntax highlight of awk files.
.bull
The editor now supports a Select All command.
.bull
The windowed editor now holds the position of the replace and
search/replace dialogs between invocations.
.bull
The wcl and wcl386 utilities now work correctly on a Linux host.
.bull
The DOS NetBIOS trap file and debug server have been modified to work when
running on Windows NT (through NTVDM).
.bull
The 16-bit Windows GUI tools no longer require CTL3DV2.DLL, but still use
it if the library is present.
.endbull 
.*
.section Differences from Open Watcom Version 1.7
.*
.np
Following is a list of changes made in &product 1.8:
.begbull
.bull
The C compiler now performs stricter type checking on enumerated types.
Previously, enums were treated as their underlying type (signed/unsigned char,
int, etc.) for type checks.
.bull
The C compiler now adds location information about enumerated symbols in
appropriate diagnostics.
.bull
The C compiler has been improved with respect to the handling of near/far
pointers, especially when converting or comparing pointers and integers of
different sizes.
.bull
The C compiler now properly recognizes functions that do not return, such as
longjmp(), and correctly diagnoses control flow issues such as unreachable
code or missing return statements.
.bull
The C compiler now supports a #pragma alias. This pragma emits alias records
into the object file for processing by the linker.
.bull
The C compiler now has larger capacity and can compile some complex source
files that previously caused out of memory errors (E1064).
.bull
The C compiler now always warns about unprototyped functions (W131).
Previously, some forgivable instances of missing prototypes were undiagnosed
by default, which caused users to write non-portable code usually by accident.
.bull
Warning W139 (Divisor for modulo or division operation is zero) has been added
to the C compiler. This warning is triggered whenever the right operand of an
integer division or modulo operation is a constant expression with the value
of zero.
.bull
The handling of pragma aux is now correct in cases where code is emitted and a
function body already exists for the corresponding symbolic name.
.bull
Various fixes to the handling of the include_alias pragma have been made.
.bull
The C and C++ compilers now have diagnostics for meaningless comparison of
64-bit and bit-field operands.
.bull
The C and C++ compilers now have conversion tables from CP=1250,1252
(Latin-2,1 for Windows ANSI) to Unicode to support Windows programming.
.bull
The C++ compiler now supports the explicit specification of function template
arguments. For example: f<T>().
.bull
The C++ compiler now uses lazy instantiation for class templates and template
members. Only the members actually used are instantiated.
.bull
The C++ compiler now allows member templates to be defined outside their
class.
.bull
Numerous bugs in the C++ compiler have been fixed.
.bull
The C++ compiler allows a class declaration with modifiers to have those
modifiers propagate into the class definition.
.bull
When -SH switch is used with the Fortran compiler, the default size of an
integer constant is now INTEGER*2.
.bull
The Fortran compiler no longer crashes when equivalencing common/global with
automatics.
.bull
Fortran processing for the text record EOL has been normalized. On UNIX
systems write/seek uses LF and read uses LF or CRLF. On non-UNIX systems
write/seek uses CRLF and read uses LF or CRLF.
.bull
Various code generation bug fixes to both the 16-bit and 32-bit compilers have
been made.
.bull
The Win32 headers and libraries are now Open Watcom's own internal version.
The MinGW headers and libraries are no longer being used. The new headers and
libraries provider greater compatibility with the Microsoft SDK and better
support for Microsoft Vista.
.bull
The version of Causeway in the official distribution is now version 4.03. 
.bull
The Linux run time libraries now have recvfrom() and sendto() implementations.
.bull
A POSIX compatible fnmatch() function and a corresponding fnmatch.h header has
been added.
.bull
The _dos_getfileattr() function no longer crashes in large data models on
16-bit DOS and Windows.
.bull
The C run time library now has implementations of the following functions to
improve compatibility with other compilers: _chmod, _chsize, _creat, _dup2,
_eof, _filelength, _isatty, _read, _sopen, _tell, _umask, _unlink, and _write.
.bull
The date and time arguments to _dos_getftime() and _dos_setftime() are now
using 'unsigned int' type instead of 'unsigned short'. This change has been
made to improve compatibility with other compilers.
.bull
The segment argument used with _dos_allocmem(), _dos_freemem() and
_dos_setblock() is now unsigned int instead of unsigned short. This change was
made for compatibility with other compilers.
.bull
The Fortran run time environment now works on Linux.
.bull
The Linux Fortran run time libraries now have FSYSTEM and FSPAWN
implementations.
.bull
The Fortran run time now exposes the saved stack pointer for when crashes
occur in the IO subsystems.
.bull
The debugger's options context menu now has an option to allow all child nodes
to be expanded recursively.
.bull
The debugger now uses the full size of the type when displaying hexadecimal
values (for example 0x03 instead of 0x3).
.bull
The debugger now has a new menu entry to change the display format of all
values in an array (all hex or all decimal).
.bull
The debugger now supports break on write rather than just break on change. The
trap must support exact breakpoints for this to happen.
.bull
The Windows debugger now properly handles quoted program names.
.bull
The Windows debugger now has proper color support.
.bull
The NetWare TCP server and trap have been updated for NW6.5SP7 with the latest
libcpre.obj file.
.bull
The DOS real-mode trap file (std.trp) now correctly displays high parts of
32-bit registers on 386+ CPUs. Previously, the high parts were always
displayed as zeros.
.bull
WLIB now has a new -pa option to set up library page size automatically to
optimal size.
.bull
WLIB now handles COFF import libraries more correctly.
.bull
WCL now properly handles the -fd and -fm options without the file name
specified.
.bull
WASM now handles EXTERNDEF directives properly; an EXTDEF record is created
only if the symbol is actually referenced.
.bull
WASM now handles the auto-dependency filename properly. 
.bull
WASM now implicitly creates the __UNIX__ macro for the BSD target as it has
for LINUX and QNX.
.bull
The internal version numbers for WASM and WMAKE are now compatible with that
used by the C and C++ compilers. Specifically the macro __WASM__ has the value
1280 for WASM and the macro __VERSION__ has the value 1280 for WMAKE.
.bull
The 32-bit DOS WD and WPROF can now be used with DOS/4G 2.x. The DOS4GOPTIONS
settings are no longer exported. Users may still override the defaults by
supplying their own wd.ini and wprof.ini, respectively. These files must be in
the appropriate format for the DOS/4G version used. Note that this does not
affect DOS/4GW users.
.bull
WLINK now handles offsets and groups larger than 64 KB for 32 bit code and 16
bit targets.
.bull
WLINK now ignores fixup displacement when the target relocation is absolute.
This is required for compatibility with object files generated by MASM 5.1.
.bull
WLINK now properly handles the alignment of the last segment in a group if the
last segment fragment is blank.
.bull
WLINK can now use the WLINK_LNK environment variable to override the default
directive file name (wlink.lnk). If the specified file isn't found then
default file is used as usual.
.bull
WLINK now properly emits segments overlapped by groups to output file.
.bull
WLINK now properly handles imported symbols that are locally defined with the
dllimport specifier in PE formatted files.
.bull
WLINK DLL was renamed from wlink.dll to wlinkd.dll. It requires to correct 
wmake directive "!loaddll" to use this new name if it is used in makefile.
.bull
WRC on Far-Eastern NT-based systems now honors the DBCS encoding specified on
the command line.
.bull
The text editor now supports syntax highlighting for resource files.
.bull
The Fgrep dialog box in graphical editor now has a browse button to display
the standard browse for folder dialog box on versions of Windows that support
it.
.bull
The ide2make utility has been added to the Open Watcom distribution. This
utility converts IDE project files to make files.
.bull
The exe2bin utility now performs properly if relocation items are unsorted.
.bull
The installer now generates a batch file that automatically sets the
environment variables used by the Open Watcom tools.
.endbull
.*
.section Changes in 1.8 that may Require Recompilation
.*
.begnote
.note _dos_getftime and _dos_setftime functions
32-bit users of _dos_getftime() and _dos_setftime() need to recompile.
Consider using stat()/utime() instead of the non-portable functions.
.note _dos_allocmem and _dos_freemem functions
32-bit DOS users of _dos_allocmem() and _dos_freemem() need to recompile.
Consider not using this functions in the first place.
.endnote
.*
.*
.section Differences from Open Watcom Version 1.6
.*
.np
Following is a list of changes made in &product 1.7:
.begbull
.bull
Functions using the __cdecl calling convention now return floating-point
values in FPU registers in 32-bit code. This is the correct behavior
compatible with other compilers.
.bull
The C and C++ compilers now support an include_alias pragma which can be
used, among other things, to map long include filenames onto 8.3 names. In
addition, the compilers now support a -na switch to disable the automatic
inclusion of _ialias.h (which contains new include_alias pragmas for
certain standard headers).
.bull
The C compiler now emits warnings on operations which mix multi-level
pointer types such as void** and int**. Previously a warning was only
emitted in ANSI mode. Note that while void** is not assignment compatible
with int**, void* is. The new stricter behavior is in line with most other
C compilers and helps users write cleaner and more portable code.
.bull
The C compiler now warns (W400) when an expression of plain char type is
used as array index. Because the sign of plain char changes depending on
compiler choice and settings, using plain char as array index is inherently
non-portable and likely to cause problems.
.bull
The C compiler now folds constant expression before checking for range
overflows etc. This enables the compiler to diagnose constant out of range
issues in more cases.
.bull
The C compiler now allows benign variable redefinitions where __near or
__far modifiers aren't always explicitly specified but match when current
data model is taken into account (eg. 'extern int __near x;' and 'extern
int x;' in small data models). The new behavior is compatible with the C++
compiler as well as with other vendors' compilers.
.bull
The default stack size for 32-bit DOS extended executables has been
increased from 4K to 64K. Note that the linker can still set greater or
smaller stack size.
.bull
The code generator now produces better 16-bit code for 64-bit math
operations
.bull
The code generator no longer incorrectly optimizes out conditionals. The
problem only occurred in very rare situations and probably only when
doubles were being compared.
.bull
A long-standing problem with use of certain 64-bit constants as results of
ternary operators has been fixed.
.bull
The C++ compilers now has a workaround for bugzilla bug #63
(http://bugzilla.openwatcom.org/show_bug.cgi?id=63). The compiler now
generates an error message referring to the bug entry rather than crashing.
.bull
The LIBC and CLIB thin netware libraries have been added to the
distribution as experimental.
.bull
32-bit DOS executables now correctly pass environment to child processes
started through the spawn family of functions. Previous incorrect behavior
was introduced in version 1.6.
.bull
In the NetWare libraries, __get_stdout with __get_std_stream was causing an
infinite recursion. This has been fixed.
.bull
Fixed 8087 emulator/mathlib 80-bit real multiplication if one operator is
zero and second is a power of two.
.bull
The _outgtext() function in graph.lib no longer fails on 32-bit targets.
.bull
The Win32 stat() function now returns correct results when it tries to
access a file with given name and the directory containing that file also
contained a subdirectory with the same name.
.bull
The debugger now supports a "No Source" toggle (right mouse click menu) in
the assembly code window to switch on/off associated source code.
.bull
The debugger no longer crashes when tracing F77 programs that use
variable-size arrays. This only applies to DWARF debugging information
(which is used by default).
.bull
The debugger now correctly displays multi-dimensional Fortran arrays when
DWARF debug information format is used (which is used by default).
.bull
The debugger can now display [partial] strings in Fortran code when the
string length exceeds the debugger's internal limit. The internal limit has
also been increased from 512 to 1024 characters.
.bull
The resource compiler now properly copies non-resident name table when
processing LX executables.
.bull
The console version of vi for Win32 no longer quits after Ctrl+Left is
pressed followed by any other key.
.bull
WCL now correctly takes options from the environment when there is more
then one file to compile.
.bull
The linker now supports a MIXED1632 option to allow mixing of 16- and
32-bit logical segments into a single physical segment in OS/2 LX/LE
executables.
.bull
The linker now supports a NOSTUB option for Windows and OS/2 executable
formats. This option causes no DOS stub executable to be written to the
output image.
.bull
The installer now allows you to disable creating program groups or
modifying the startup environment using the /np and /ns switches.
.endbull
.*
.section Changes in 1.7 that may Require Recompilation
.*
.begnote
.note __cdecl Calling Convention
Functions in 32-bit code that return floating-point values now do so
using the FPU registers. Such functions will need to be recompiled if
they are to be linked with object code produced by &product 1.7.
.endnote
.*
.*
.section Differences from Open Watcom Version 1.5
.*
.np
Following is a list of changes made in &product 1.6:
.begbull
.bull
The C compiler has been modified to use the underlying bit-field type and
not signed/unsigned int as the type of operand which is a bit-field. This
is consistent with the C++ compiler and fixes some problems when bit-fields
larger than int are used.
.bull
Processing of #pragma aux has been corrected in the C compiler. This fixes
problems when using the mmintrin.h header, among others.
.bull
The C compiler now accepts __declspec modifiers specifying calling conventions
applied to variables, not just functions. The new behavior is consistent with
the C++ compiler, and also with the fact that ordinary calling convention
type modifiers can be used with variables.
.bull
The C and C++ compilers have been fixed to properly declare variable names
based on calling convention specifiers. This fixes problems with building
code using IBM SOM. Note that the current behavior is the same as in Open
Watcom 1.3 and earlier.
.bull
The C compiler's preprocessor has been modified to allow use of macros with
large number of arguments (255 or more).
.bull
The C compiler no longer generates internal errors when options -ri and -oe
are specified at the same time.
.bull
The C++ compiler has been fixed to inline intrinsic functions.
.bull
The 386 compilers have been changed to default to tuning code for P6
architecture instead of Pentium. Optimizing for P6 typically results in
slightly more compact and faster code.
.bull
The 386 C compiler has been fixed to properly convert between flat and
__far16 pointers, especially pointers to functions. Its behavior should
now be compatible with the C++ compiler. The problem was most likely affecting
OS/2 users who wrote mixed 16-bit and 32-bit code.
.bull
The C compiler has been changed to allow redeclaration of functions in rare
cases where initial declaration did not specify a calling convention and
the subsequent declaration specified a calling convention which matched the
default.
.bull
A new -zwf switch has been added to the C and C++ compilers. This switch is
off by default and enables generation of FWAIT instructions on 386 and later
CPUs. It is only needed in unusual situations.
.bull
The C compiler now correctly converts 64-bit integer constants to
floating-point constants.
.bull
The code generator no longer merges memory accesses when volatile variables
are involved.
.bull
The code generator now correctly const folds 64-bit right shifts.
.bull
The code generator now properly converts between far pointers and 64-bit
integers. Attempts to convert a 48-bit far pointer to 64-bit integer no
longer cause a crash.
.bull
The code generator has been modified to slightly decrease code size when
optimizing for size (-os).
.bull
The non-standard alloca.h header has been added for compatibility with other
compilers.
.bull
The strftime() library function has been extended to support date formats
introduced in C99.
.bull
The file pointer type used with lseek() and tell() has been changed to off_t
(from long) for compatibility with POSIX.
.bull
The 386 versions of _clear87() and _status87() functions have been modified
to use the no-wait form of FPU control instructions. This allows these
functions to be used in exception handlers when there are pending unmasked
floating-point exceptions.
.bull
The 16-bit 8087 emulator has been fixed to correctly evaluate multiplies as
infinity instead of zero in rare overflow situations.
.bull
The resource compiler (wrc) has been fixed to store long integer constants as
32-bit quantities in RCDATA or user data resource statements. This behavior
applies to Win16, Win32, and OS/2 targets. Integers without the 'L' suffix
are stored as 16-bit and potentially truncated.
.bull
The OS/2 specific part of the resource compiler has been corrected to process
RCDATA statements properly.
.bull
The assembler (wasm) now supports external absolute symbols. The SIZE, SIZEOF,
LENGTH, and LENGTHOF operators have been corrected for structures.
.bull
Classification of privileged instructions in the assembler has been
updated to match MASM.
.bull
The assembler now evaluates expressions in return instructions correctly.
Previously, code such as 'ret 28+4' would be sometimes erroneously assembled
as 'ret 28' instead of 'ret 32'.
.bull
The linker has been changed to only recognize segments of class 'STACK'
as stack segment. Previously, any segment with class name ending with 'STACK'
(eg. 'FSTACK') was recognized.
.bull
Several minor problems related to creating DOS executables have been fixed
in the linker.
.bull
The RUNTIME linker directive has been extended to allow ELF ABI type and
version specification. This functionality is similar to the brandelf utility.
See the Linker Guide for details.
.bull
The wmake utility has been modified such that in native wmake mode, a
symbolic target with no command list is always considered to have had its
command list executed. That will cause any targets that are dependent on this
symbolic target to be considered out of date.
.bull
The Win32 trap file is now able to determine the full pathname of debuggee's
loaded DLLs. This may ease debugging in some cases as the debugger will be
more likely to find debugging information for DLLs.
.bull
The Win16 debugger trap file (std.dll) has been modified to allow 16-bit wdw
to run on Windows NT platforms without reporting a spurious error message on
exit.
.bull
Numerous problems with the Win386 extender support have been fixed so that
Win386 now works again.
.bull
The dmpobj utility has been enhanced to support additional OMF records, and
new command line options have been added.
.endbull
.*
.*
.section Differences from Open Watcom Version 1.4
.*
.np
Following is a list of changes made in &product 1.5:
.begbull
.bull
Support for ISO/IEC TR 24731, "Extensions to the C Library, Part I:
Bounds-checking interfaces" has been added to the C runtime library. The C
compiler now predefines the macro __STDC_LIB_EXT1__ (which evaluates to
200509L) to indicate this support. This set of functions is also known as
the Safer C Library. Please see the C Library Reference for detailed
documentation of these functions.
.bull
In C99 mode, the C compiler now supports C99 style declarations intermixed
with statements within a block, as well as declarations in the opening clause
of a for loop.
.bull
The C compiler now predefines additional macros required by the C standards.
These include __STDC_HOSTED__ (evaluates to 1) to indicate a hosted
implementation and __STDC_VERSION__ (either 199409L or 199901L) to indicate
C94 or C99 support depending on compilation switches.
.bull
A __restrict keyword has been added to the C compiler. It is functionally
equivalent to the C99 'restrict' keyword but is always visible, even in
non-C99 mode.
.bull
In C99 mode, the C compiler no longer requires explicit return statement in
a main function returning an integer. In accordance with C99 standard,
'return 0;' is implied if end of function is reached and no return statement
was encountered.
.bull
The C compiler has been fixed so that it no longer loses track of pointer
base when using __based(__self) pointers. The C compiler has also been
fixed to properly handle dereferencing of based arrays.
.bull
The 16-bit C++ compiler now correctly casts pointers to __segment type
without crashing. Proper semantics of the cast were implemented.
.bull
The 16-bit C compiler has also been modified to handle casts to __segment
type correctly, ie. return segment portion of a pointer.
.bull
The C compiler has been fixed to properly support named based segments
(ie. __based( __segname( <name> ) ) pointers) that reference the default
_DATA and _CONST segments.
.bull
The compilers and library headers have been modified to properly support
use of standard libraries when one of the -ec switches is used to change
default calling convention.
.bull
The C compiler has been modified such that segments created through #pragma
data_seg are no longer part of DGROUP. Typically, the pragma is used only
when data segments need to be located in a physically separate segment in
the output module.
.bull
New warning W137, "Extern function 'fn' redeclared as static", has been added
to the C compiler. Existing error E1072, "Storage class disagrees with
previous definition of 'symbol'" has been extended to cover redefinitions
from 'extern' to 'static' and not only from 'static' to 'extern'. Changing
the linkage of a symbol invokes undefined behavior according to ISO C.
.bull
New warning W138, "No newline at end of file", has been added to the C
compiler. It is emitted if no line terminator character was found before the
end of a source file. Such files do not conform to ISO C. The missing newline
character will be automatically inserted; this matches the C++ compiler
behavior. Note that missing newlines could previously lead to spurious
"#endif matches #if in different source file" warnings.
.bull
The C compiler has been modified to allow the __export or __declspec(dllexport)
modifier on a declaration when earlier declaration exists with no modifier.
The updated behavior is compatible with the C++ compiler as well as some
compilers from other vendors.
.bull
In ISO/ANSI mode (-za), the compiler now always warns if it encounters a call
to unprototyped function. In extensions mode (default, -ze), this warning
(W131) is suppressed if a matching prototype is found later in the source
file. Note that the behavior in extensions mode is unchanged from earlier
versions.
.bull
The C compiler now eliminates static functions that are always inlined from
the object file (the functions are of course still emitted if their address
is taken).
.bull
The C compiler has been fixed to properly evaluate boolean expressions
(especially the ternary operator) where the condition is a 64-bit integer
constant. Previously, the high 32 bits were in some cases ignored, which could
lead to erroneous results.
.bull
The C compiler has been modified to properly cast floating-point constants
to the specified type. Notably FLT_MIN stored or passed as double is now
handled correctly (without spurious precision).
.bull
Handling of empty macro arguments has been corrected in the C compiler's
preprocessor. Previously, empty macro arguments could result in invalid tokens
in certain cases.
.bull
The peephole optimizer is now run again after register allocation. This
allows the code generator to take advantage of some optimization opportunities
that were previously missed.
.bull
The code generator has been modified to copy DS into ES in __interrupt routine
prolog (right after DS was loaded) if the ES register is considered
unalterable, ie. in flat model. This may avoid crashes if ES is dereferenced
explicitly or implicitly in interrupt handlers, for instance when calling
memcpy().
.bull
The linker and other tools have been fixed to correctly classify code segments.
Previously, code segments could be misclassified as data, which led to incorrect
disassembly and generation of debugging information.
.bull
A performance problem related to emitting debugging information for structures
or unions with many members has been corrected in the code generator.
.bull
The POSIX-defined header libgen.h has been implemented. This includes two
functions, basename() and dirname().
.bull
The functions btowc(), fwide(), mbsinit(), wctrans(), and towctrans() have
been added to the C runtime library. These functions are all related to
wide-character and multi-byte support, and were first defined by the ISO C
Normative Amendment 1.
.bull
C99 functions llabs(), lldiv(), and _Exit() have been added to the C runtime
library. Note that the latter is equivalent to _exit(), defined by POSIX.
.bull
Support for C99 floating-point classification macros has been implemented.
This includes fpclassify, isfinite, isinf, isnan, isnormal, and signbit.
.bull
Modifiers 'hh', 'j', 'z', and 't' defined by C99 for the printf and scanf
family of functions have been implemented in the C runtime library. Please
see the C Library Reference for details.
.bull
The 'F' modifier for printf and scanf families of functions conflicts with
'F' format specifier defined by ISO C for floating-point conversions. It has
been replaced by a 'W' modifier which is now used to denote a far pointer.
The 'F' modifier is still recognized in DOS builds of the runtime library
(which therefore cannot handle the 'F' format specifier as defined by ISO C),
but is no longer documented and will be removed in a future release.
.bull
Several very obscure bugs have been fixed in the printf and scanf family of
functions. These problems were discovered thanks to a more stringent testing
procedure and had never been reported by users.
.bull
The strtod function has been enhanced to follow C99 specification. It will
now parse infinity and NaN, as well as hexadecimal floating-point numbers.
See the C Library Reference for details.
.bull
The math library has been fixed to perform binary to decimal floating-point
conversions with greater precision. This fixes a problem where in some cases
a conversion from binary to decimal and back was losing precision or producing
erroneous results.
.bull
The graphics library has been fixed to correctly work with VESA modes where
the number of bytes per line does not directly correspond to width of the
mode in pixels.
.bull
The owcc utility has been much improved and documented; this tool is a POSIX
style compiler driver, designed to provide certain level of command line
compatibility with gcc and ease porting.
.bull
The NOEXTension linker option has been documented; this option instructs the
linker not to add any extension (.exe, .dll, etc.) to the executable name.
Any argument to the NAME directive will be used verbatim. This option had
been supported by earlier versions of the linker but not documented.
.bull
The 'include' preprocessor directive not prefixed by an exclamation mark
is now recognized in wmake -ms mode for compatibility with Microsoft and
IBM NMAKE.
.bull
The wmake utility has been enhanced to evaluate NMAKE style '[cmd]'
expressions (ie. shell commands) in preprocessor !if directives. This
functionality is supported in both wmake and -ms mode.
.bull
Several problems related to the Watcom debugging information format have
been fixed in the linker and debugger. Note that the Watcom format is
considered obsolete and its use is not recommended.
.bull
A random but very rare startup failure of Windows based GUI tools (notably
wdw) has been fixed.
.endbull
.*
.*
.section Differences from Open Watcom Version 1.3
.*
.np
Following is a list of changes made in &product 1.4:
.begbull
.bull
Support for C99 designated initializers has been added to the C compiler,
for example "struct {int a, b;} c = {.a=0, .b=1};". This is also supported
for arrays, for example "int a[4] = {[0]=5, [3]=2};".
.bull
Handling of enumerations has been fixed in the C compiler. In certain cases,
the compiler chose the wrong type for operations on objects of enumerated
types. Enumerated constants up to 64 bits wide are now also allowed (including
in 16-bit compilers).
.bull
The C compiler will now warn if the right hand operand of a bitwise shift
expression is a constant that is either negative or greater than or equal
to the bit with of the promoted left operand. The result of such operation is
not defined by ISO C. The warnings are 'W134: Shift amount negative' and
'W135: Shift amount too large'.
.bull
The C compiler now warns in cases where an unsigned type is compared for <= 0.
This is equivalent to 'unsigned == 0' and often indicates that a signed
comparison was intended.
.bull
New __watcall keyword has been added to the C and C++ compilers to designate
the default Watcom calling convention.
.bull
New -ec? switches have been added to set the default calling convention.
.bull
The 16-bit C compiler now defines _M_I86 macro for consistency with the C++
compiler. The new macro should be used in preference to the existing M_I86.
.bull
A number of new keywords have been added to the C compiler; these were
previously defined as macros: _Cdecl, _Export, _Far16, _Fastcall, _Pascal,
__sycall, _System, __try, __except, __finally, __leave.
.bull
Analogous change has been made to the C++ compiler. The new keywords (and
removed predefined macros) are: _Cdecl, _Export, _Far16, _Fastcall, __inline,
_Pascal, __syscall, _System.
.bull
The C++ compiler now handles the "new" template specialization syntax,
and partial specialization is partially supported (no pun intended).
.bull
The C++ compiler now correctly handles the situation where control reaches
the end of main() function without encountering a return statement. In that
case, the effect is that of executing "return 0;".
.bull
The C++ compiler now properly allows return statements with a void expression
in functions that return void.
.bull
386 C and C++ compilers now support the Microsoft fastcall calling convention,
and recognize the __fastcall keyword.
.bull
The C compiler now recognizes #pragma data_seg and code_seg forms that
specify segment and class names without enclosing them in parentheses. The
new behavior is consistent with other compilers.
.bull
New -fti switch has been added to the C compiler to track #include file
opens. This helps diagnose include file problems in complex projects.
.bull
The code generator no longer emits debug information for unreferenced
typedefs when -d1+ or -d2 switch is used. This produces slightly to
significantly smaller debug information. Note that behavior of -d3 is
unchanged.
.bull
The 386 code generator will no longer select the 'and' instruction to perform
zero extension when optimizing for time on the 686 architecture (-6r or -6s
switch). The 'movzx' instruction will always be used instead, because it
avoids partial register stalls and in certain cases significantly improves
performance on P6 and newer class CPUs.
.bull
Support for long long based bitfields has been improved in the code generator.
.bull
The code generator now properly diagnoses attempts to emit symbol names
that overflow the OMF limit (255 bytes).
.bull
Several problems related to loss of segment information in 386 non-flat
models have been fixed in the code generator.
.bull
Command line processing has been changed in the Compile and Link utility
(wcl). Forward slashes now may be used as path separators in file arguments,
such that "foo/bar" is now interpreted as "foo\bar.c". Note that this does
not affect options delimited with forward slashes.
.bull
Support for raw binary and Intel Hex output has been added to the linker,
along with support for 24-bit segmented addressing architectures (HSHIFT
option) and arbitrary class/segment reordering (OUTPUT and ORDER directives).
See Linker Guide for details.
.bull
Support for overlays (16-bit DOS) has been reinstated in the linker.
A related FARCALLS/NOFARCALLS option has been added to the linker. See
Linker Guide for details.
.bull
The linker now correctly processes relocations to symbols in absolute
segments.
.bull
The linker now checks for bitness conflicts (16 vs. 32-bit) when adding
segments to a group.
.bull
The minimum accepted value for linker OBJALIGN option has been changed to
16 bytes (previously 512).
.bull
The linker no longer creates a separate .bss section when linking PE modules.
Instead, uninitialized data is added at the end of the data section. This
creates slightly smaller executables and reduces memory usage.
.bull
The st_name member was removed from struct stat and related structures. This
was done for consistency across platforms (UNIX has no such field), because
the st_name field was almost entirely useless (being limited to 13
characters), and for compatibility with Microsoft compilers; the latter
because struct _wstat and struct _wstati64 are now obsolete and struct
_stat/_stati64 can be used for wide character stat functions. NB: This change
requires recompilation. New object files will not work with old libraries
and vice versa.
.bull
The signal() function no longer modifies floating-point control word on
Win32 and 32-bit OS/2. Also, default value for SIGFPE has been changed to
SIG_DFL from SIG_IGN on all platforms.
.bull
The e/E format of printf() family of functions has been changed to format
the exponent with minimum of two digits (instead of three). This behavior
is dictated by C99 and consistent with most other compilers.
.bull
The floating-point to string conversion routines now format values with
greater precision. This means that floating-point values printed by C and C++
programs may be slightly different from earlier versions of the runtime
libraries (but more accurate).
.bull
The sleep() function is now declared in unistd.h and its return type has been
changed to unsigned int, for compatibility with POSIX.
.bull
The clock() function now uses millisecond counters (where available) on DOS
and Windows, and is no longer susceptible to problems related to TZ changes.
.bull
The DOS runtime has been tuned to produce smaller executables.
.bull
C99 functions wmemchr(), wmemcmp(), wmemcpy(), wmemmove(), and wmemset()
have been added to the C runtime library.
.bull
A POSIX compatible getopt() function has been added to the C runtime library.
.bull
A POSIX compatible mkstemp() function has been added to the C runtime library.
.bull
BSD compatible safe string copy and concatenation functions, strlcpy()
and strlcat(), have been added. Use of these functions is highly recommended
over strncpy() and strncat(), because they are safer and much easier to use.
.bull
New strings.h header has been added for POSIX compatibility, although legacy
functions index() and rindex() are not supported. Functions strcasecmp() and
strncasecmp() are also declared in string.h for compatibility with other
compilers.
.bull
The C runtime library no longer returns ESPIPE when calling write() on a pipe
or device that was opened with O_APPEND flag. The old behavior was not POSIX
conforming.
.bull
Handling of pathnames that include spaces has been improved in the make
utility (wmake).
.bull
The disassembler (wdis) now handles big endian object files on little
endian host platforms, and vice versa.
.bull
Support for MIPS R4000 and SPARC V8 instruction sets has been added to the
disassembler.
.bull
New -zz and -zzo option have been added to the assembler (wasm) for
backwards compatibility. See Tools User's Guide for details.
.bull
Default behavior of inline assembler has changed. The CPU optimization
level (-4, -5, -6) now implies the available instruction set: -5 implies
MMX and 3DNow!, -6 also implies SSE/SSE2/SSE3. Also note that any CPU
setting override now reverts to default at the end of each inline assembly
block.
.bull
16-bit DOS version of the assembler (wasmr) has been added. This version
runs on 8086 and above and requires less memory than the protected mode
version.
.bull
The debugger has been changed to look for support files in directories
relative to the debugger executable's location. This allows the debugger to
be used when no debugger specific environment variables have been set.
.bull
A problem with stepping into code (F8) right after debuggee was loaded has
been fixed in the debugger.
.bull
The debugger now looks for debug information in a .sym file when the /DOwnload
option was specified. Previously it erroneously only looked at the executable
if the download option was used.
.bull
Support for Microsoft/IBM .sym files generated by the MAPSYM utility has
been added to the debugger and profiler. This is helpful especially with
symbol files provided by IBM for OS/2 system DLLs; disassembly now shows
for instance "call DOS32EXIT" instead of "call 01C74634".
.bull
The CauseWay trap file no longer incorrectly maps symbol addresses in
'large' executables (code segment > 64K).
.bull
Interoperability with GNU tools has been improved. The debugger (wd/wdw)
should now be able to debug GNU-produced executables (with DWARF 2 debug
information) and vice versa.
.bull
New -zld option has been added to the library manager (wlib) to strip
autodependency information from OMF objects.
.bull
New exe2bin utility has been added. See Tools User's Guide for details.
.bull
Basic support for compiling OS/2 resource scripts and binding resources
into OS/2 executables (both NE and LX formats) has been added to the resource
compiler (wrc).
.bull
The include search order in the resource compiler has been changed
to be more consistent with the C/C++ compilers, as well as with IBM's and
Microsoft's resource compilers. System include files (enclosed in angle
brackets) are no longer searched in current directory or in the directory
of the file containing the #include directive.
.bull
The Windows resource compiler has been made more compatible with
scripts designed for Microsoft's RC in the way it treats string literals.
.bull
The MS LINK compatibility wrapper now supports a /RELEASE switch.
.bull
Syntax highlighting support for makefiles has been added to the editor.
The default syntax highlighting scheme has also been made more colorful.
.bull
The editor and Windows GUI tools now store configuration files in more
appropriate locations (notably on multi-user machines).
.bull
Several new DOS extender related targets have been added to the IDE. Better
support for remote debugging has also been added to the IDE.
.bull
The CauseWay DOS extender now supports SSE instructions on plain DOS.
.bull
Several simple OS/2 SOM programming examples have been added.
.endbull
.*
.section Changes in 1.4 that may Require Recompilation
.*
.begnote
.note stat()
The
.kw stat
function now uses a slightly different
.kw struct stat
argument.
Source code that uses the
.kw stat
function or references
.kw struct stat
must be recompiled before linking the application with new libraries.
.endnote
.*
.*
.section Differences from Open Watcom Version 1.2
.*
.np
Following is a list of changes made in &product 1.3:
.begbull
.bull
The C++ compiler now restricts the scope of variables declared in a for
loop to the scope of that loop in accordance with ISO C++, not extending
the scope beyond the loop (ARM compliant behavior). Code relying on the
pre-standard behavior must either be changed or compiled with new -zf
switch which reverts to old scoping rules.
.bull
Support for default template arguments has been added to the C++ compiler.
.bull
Support for alternative tokens (and, xor etc.) has been added to the C++
compiler. It is enabled by default, can be turned off with the new -zat
switch.
.bull
The C runtime library has been made significantly more C99 compliant. A number
of new headers have been added (inttypes.h, stdbool.h, stdint.h, wctype.h) and
corresponding new functions implemented. Wide character classification
functions were moved out of ctype.h into wctype.h. C99 va_copy macro was
added to stdarg.h.
.bull
Added 'cname' style C++ headers.
.bull
Support for SSE, SSE2, SSE3 and 3DNow! instruction sets has been added. Affected
tools are the assembler (wasm), as well as all x86 compilers, disassembler and
debugger. The debugger now also supports MMX registers formatted as floats
(for 3DNow!) as well as a new XMM register window for SSE.
.bull
Inline assembler directives .MMX, .K3D, .XMM, .XMM2 and .XMM3 are now supported in
the _asm as well as #pragma aux style inline assembler interface. Note: .MMX
directive is now required (in addition to .586) to use MMX instructions.
.bull
C compiler performance has been significantly improved (up to 5-10 times speedup)
when compiling large and complex source files.
.bull
All x86 compilers now have the ability to perform no truncation when converting
floating point values to integers. Additionally, 32-bit x86 compilers have the
option to inline the rounding code instead of calling __CHP.
.bull
The C lexical scanner no longer evaluates constants with (U)LL suffix that
fit into 32 bits as zero (1ULL was wrong, LONGLONG_MAX was correct).
.bull
C and C++ x86 inline assembler has been fixed to properly process hexadecimal
constants postfixed with 'h'.
.bull
The C compiler now supports the C99 'inline' keyword, in addition to previously
supported '_inline' and '__inline' keywords.
.bull
The C compiler now treats a sequence of adjacent character strings as wide if
any of the components are wide (required by C99), instead of depending on the
type of the last component. For example, L"foo " "bar" is now interpreted as
L"foo bar", instead of "foo bar".
.bull
The internal C compiler limit on complex expressions has been increased
and if it is still insufficient, the compiler now reports an error instead of
crashing.
.bull
The C compiler now issues a warning on the default warning level if a function
with no prototype is referenced. This was previously warning W301 (level 3), now
it is warning W131 (level 1).
.bull
Warning "W132: No storage class or type specified" has been added to the C compiler.
This warning is issued if a variable is declared without specifying either storage
class or type. This is not allowed in C89.
.bull
Warning "W304: Return type 'int' assumed for function 'foo'" has been added.
This warning is issued if a function is declared without specifying return type.
This is allowed in C89 but not in C99.
.bull
Warning "W305: Type 'int' assumed in declaration of 'foo'" has been added to the
C compiler. This warning is issued if a variable is declared without specifying
its type. This is allowed in C89 but not in C99. Note that if warning W132 is
issued, W305 applies as well.
.bull
The C compiler now properly warns if a function with implied 'int' return type
fails to return a value. This potential error was previously undetected.
.bull
C++ compiler diagnostic messages have been made more consistent and slightly more
detailed.
.bull
Linker for Win32 targets can now create file checksums. These are primarily used
for DLLs and device drivers, but can be applied to all Win32 PE images if required.
.bull
Linker for Win32 targets can now set operating system version requirements into
the PECOFF optional header (Microsoft extended header).
.bull
Linker for Win32 targets can now set the linker version number into the PE
optional header (Microsoft extended header).
.bull
The linker will now eliminate zero-sized segments from NE format (16-bit OS/2
and Windows) executables. This fixes a problem where Windows 3.x would refuse
to load an executable with zero sized segment. This could happen especially
with C++ programs where some segments may have ended up empty after eliminating
unused functions.
.bull
The linker now (again) produces correct Watcom style debugging information. This
was a regression introduced in previous version.
.bull
Command line parsing for wccxxx, wppxxx and cl has been changed such that a
double backslash inside a quoted string is collapsed to a single backslash,
and hence "foo\\" now translates to 'foo\' and not 'foo\"'.
.bull
The IDE and other graphical tools no longer leak system resources (a bug introduced
in version 1.2).
.bull
The Image Editor limit on bitmap size has been changed from 512x512 pixels to
2048x2048 pixels.
.bull
The source browser now correctly decodes array information; Version 11.0c of
Watcom C/C++ started emitting array browse information in a new format and the
browser hadn't been updated accordingly.
.bull
The NT debugger trap file has been changed so an exception generated during a
step operation is handled correctly. Previously, the single step flag was not being
cleared and when the exception was being offered to the debuggee's own exception
handlers, a single step exception occurred in NT's exception handler rather than
the exception being passed back to our handler.
.bull
The OS/2 debuggers now dynamically allocate buffer for the command line,
preventing crashes when the command line was over approx. 260 bytes long.
.bull
The NetWare 5 debugger NLM has been changed to use kernel primitives. Previous
version were using legacy semaphores.
.bull
The make program (wmake) has been sped up very slightly. Also the 'echo' command
is now internal and no longer spawns the system command interpreter.
.bull
The precision of DBL_MAX, DBL_MIN and DBL_EPSILON has been increased; the non-standard
variants prefixed with an underscore have been removed.
.bull
The C99 functions atoll(), lltoa(), ulltoa(), strtoll(), strtoull() and corresponding
wide character functions have been added to the C runtime library.
.bull
The _beginthread() function now consistently returns -1 in case of error on
all platforms.
.bull
The stdaux and stdprn streams are now only defined on DOS based platforms, ie.
DOS, Win16 and Win386. No other platforms support stdaux or stdprn.
.bull
The assert() macro now prints function name in addition to source file and
line number, in accordance with C99.
.bull
The _heapchk() function will now always perform a consistency check on the
heap, where it would previously only check consistency if there had been
allocations/frees since last call to _heapchk(). As a consequence, _heapchk()
previously did not detect certain instances of heap corruption.
.bull
[OS/2 32-bit] The default __disallow_single_dgroup() implementation no longer
statically links against PMWIN.DLL. This allows DLLs to load on systems where
PMWIN.DLL isn't present.
.bull
[OS/2 32-bit] Re-implemented clock(). The new implementation uses the OS
millisecond counter and is hence not susceptible to TZ changes. It is also
smaller, faster and more accurate, although it may wrap around earlier than
the original implementation.
.bull
The disassembler (wdis) now correctly processes x86 'push 8-bit immediate'
instructions.
.bull
The disassembler now correctly processes absolute memory references. All memory
references without fixup are now disassembled as ds:[...] or sreg:[...].
.bull
Several DirectX Win32 programming samples have been added. Note that a separate
DirectX SDK (available from Microsoft) is required to build these sample programs.
.endbull
.*
.*
.section Differences from Open Watcom Version 1.1
.*
.np
Following is a list of changes made in &product 1.2:
:cmt Reflects main Perforce branch as of 2003/12/1
.begbull
.bull
Handling of default libraries for Win32 targets has been changed. Previously
default library records were included in the C runtime library, now they are
specified part of the wlink Win32 target definitions in wlsystem.lnk. The
list of libraries has changed from previous version as well and now reflects
the Microsoft compiler defaults; existing IDE projects and makefiles may need
to have additional libraries specified if those are no longer part of the
default list.
.bull
The C compiler now performs stricter checking on function prototypes
and pointer operations. This may lead to previously undiagnosed
warnings/errors appearing when compiling incorrect or ambiguous code.
.bull
The C compiler diagnostic messages have been improved to print more
information, making it easier to isolate the problem.
.bull
A new warning (W130) has been added to the C compiler to diagnose possible
precision loss on assignment operations. This warning is never on by default
and must be enabled through '#pragma enable_message(130)' or '-wce=130'
switch.
.bull
Support for C99 style variable argument macros (and __VA_ARGS__) has
been added to the C and C++ compilers.
.bull
Added support for the __func__ symbol (name of the current function)
which is equivalent to the already existing __FUNCTION__ symbol.
.bull
Better C99 style support for "long long" type is now available in the
C and C++ compilers. LL, ULL and LLU suffixes are recognized for constants.
"long long int" is now also recognized.
.bull
Added C99 style *LLONG_MIN/MAX defines to limits.h.
.bull
The C++ compiler has been fixed to properly accept source files where
a template was the last item in a name space or an external linkage.
.bull
Several new -adxx options have been added to the C and C++ compilers
to support automatic generation of 'make' style dependency files.
.bull
The C compiler has been fixed to correctly diagnose illegal union
assignments.
.bull
The C compiler now issues warnings on operations involving pointers
to different but compatible unions.
.bull
The C and C++ compilers now ensure word alignment of wide character
string literals to satisfy Win32 API restrictions.
.bull
The __UNIX__ macro is now supported in C and C++ compilers, wmake and
wasm. It is currently defined for QNX and Linux targets.
.bull
Default windowing support has been re-enabled for Win16 and Win386
runtime libraries.
.bull
Since default windowing is no longer supported on most platforms, the
Programmer's Guide and IDE tutorial have been updated to reflect that
fact.
.bull
The Win32 GUI tools now support the Windows XP look and feel.
.bull
AutoCAD and MFC targets have been removed from the IDE, the -bw switch
(default windowing) is no longer available in the IDE for OS/2 and Win32
targets.
.bull
Manual for the CauseWay DOS extender has been added.
.bull
The dmpobj tool has been added. This utility dumps the contents of OMF
object files and can be useful to developers.
.bull
Several system definitions have been added to wlink: os2_pm (16-bit OS/2
Presentation Manager executable), os2_dll (16-bit OS/2 DLL) and os2v2_dll
(32-bit OS/2 DLL).
.bull
The linker has been fixed to read "AR" style archives produced by third
party tools.
.bull
The linker has been fixed to prevent crashes when linking with COFF files
providing uninitialized COMDAT entries
.bull
Several linker crashes related to ELF object files and executables have
been resolved.
.bull
Updated wlink to call wlib with the -c (case sensitive) option when
creating import libraries. This fixes problems with DLLs that export
symbols differing only in case.
.bull
The C runtime library has been optimized to produce smaller
executables.
.bull
The printf() function now supports the "ll" format specifier for
"long long" integers.
.bull
The printf() function has been enhanced to support %b format specifier
for bitfields.
.bull
Execution order of C library termination routines is now better
defined to prevent instances where temporary files created through
mktemp() could be left behind.
.bull
[OS/2 32-bit] To prevent crashes, termination code is not run if second
instance of a DLL failed to load due to single DGROUP.
.bull
[OS/2 32-bit] The __grow_handles() function was incorrectly adding n
requested handles to existing limit instead of setting the limit to n.
.bull
[OS/2 32-bit] Fixed a problem with _STACKLOW in multithreaded programs
and DLLs. This prevents crashes where Fortran DLLs would run out of stack.
.bull
[OS/2 16-bit] Fixed default math exception handler which wasn't popping
the FP status word off the stack and would therefore crash on exit.
.bull
The Win32 Image Editor has been enhanced with drag-and-drop support.
.bull
The IDE has been fixed to properly handle mixed case filenames.
.bull
The Microsoft compatibility tools (NMAKE, CL) have been fixed to better
handle command line arguments.
.bull
The Dialog Editor (wde) has been fixed to prevent occasional DDE related
crashes when run from inside the Resource Editor (wre).
.bull
The 'Change font' option no longer crashes the GUI debugger (wdw).
.bull
On OS/2, wdw now intercepts the F10 key instead of passing it on
to the system.
.bull
The code generator now deletes object files if it was interrupted.
Previously zero-length invalid object files could be left behind,
interfering with make operation.
.bull
The wasm assembler has been enhanced to generate file dependency information
usable by wmake.
.bull
Numerous minor fixes have been made to wasm.
.bull
Compatibility with MASM 6 has been improved with wasm.
.bull
Support for sysenter and sysexit instructions has been added to wasm
and wdis.
.bull
Disassembly of xchg and bound instructions has been fixed in wdis
(corrected order of operands).
.bull
Several previously undocumented wmake directives have been documented.
.bull
A -sn ('noisy') option has been added to wmake to print all commands
that wmake executes, including silent ones.
.bull
The w32api project has been updated to the latest version.
.bull
The os2api project has been enhanced - added multimedia headers and
libraries and numerous fixes have been made to the header files.
.bull
The debugger now supports the F7 key as a shortcut for "run to cursor".
This is consistent with CodeView.
.bull
New internal variable dbg$ntid (next thread id) has been added to the
debugger. This permits automated iteration of all threads.
.bull
The wsample tool has been updated to dynamically allocate storage for
command line, where previously the command line length was limited to
128 characters.
.bull
The FORTRAN compiler has been changed to preserve case of symbols with
linkage other than FORTRAN. This is especially helpful when calling OS
API functions and using case sensitive link step (now default).
.endbull
.*
.*
.section Differences from Open Watcom Version 1.0
.*
.np
A number of problems has been corrected in &product 1.1.
.begbull
.bull
Using the -ol option (loop optimization) sometimes resulted in generation
of incorrect code.
.bull
The
.kw printf()
function could access too much data when formatting strings, causing page
faults.
.bull
NANs, INFs and denormals were not handled correctly by the math emulation
library.
.bull
The assembler did not generate implicit segment override prefixes when
accessing data in code segment.
.bull
The
.kw clock()
function sometimes produced incorrect results on OS/2 if the
.mono TZ
environment variable was set.
.bull
The Open Watcom editor (vi/viw) has been changed to store temporary files in directory
designated by the
.mono TMP
environment variable, instead of using the
.mono TMPDIR
environment variable.
.bull
Many packaging problems (missing files) have been resolved.
.endbull
.*
.*
.section Differences from Version 11.0
.*
.np
&product 1.0 is not substantially different from Watcom &lang
version 11.0. There are however several changes that may require you
to change your source and/or makefiles or project files.
.begbull
.bull
The C compiler now implements stricter function prototype processing. This
may result in new warning or error messages on code that was previously
compiling cleanly but contained mismatches between function prototypes
and actual definitions. The C++ compiler did not have this problem.
.bull
The linker now defaults to case sensitive symbol resolution. In most
cases this is not a problem and may in fact avoid certain problems.
If your project relies on case insensitive linking, please add
.kw OPTION NOCASEEX
to your linker directives.
.bull
Default windowing support has been removed from the runtime libraries.
.bull
Components of the Win32 SDK and OS/2 Toolkits are no longer supplied.
.bull
MFC libraries, source code and documentation are no longer supplied.
.endbull
.if &version ge 110 .do begin
.*
.section Changes in 11.0 that may Require Recompilation
.*
.np
Do not attempt to mix object code generated by earlier versions of the
compilers with object code generated by this release or with the
libraries provided in this release.
.np
A new C++ object model has been implemented.
If you have undefined references to
.mono __wcpp_3_*
names, you have old object code.
If you have undefined references to
.mono __wcpp_4_*,
you have old libraries and new object code.
.begnote
.note clock()
The
.kw clock
function accuracy has changed from 100 ticks per second to 1000 ticks
per second (i.e.,
.kw CLOCKS_PER_SEC
has changed).  Source code that
uses the
.kw clock
function and
.kw CLOCKS_PER_SEC
in its calculations must be recompiled before linking the application
with new libraries.
.endnote
.*
.section Major Differences from Version 10.6
.*
.np
The following sections summarize the major differences from the
previous release.
.begbull
.bull
In general, we have improved Microsoft compatibility in our compilers
(more warnings instead of errors, support for MS extensions, etc.) and
tools.
.bull
Some of the Win32 and 32-bit OS/2 versions of our tools are now
available in DLL form.
.millust begin
EXE      DLL        Description
------   -------    -----------------------
wcc      wccd       16-bit x86 C compiler
wcc386   wccd386    32-bit x86 C compiler
:cmt. wccaxp   wccdaxp    Alpha AXP  C compiler
wpp      wppdi86    16-bit x86 C++ compiler
wpp386   wppd386    32-bit x86 C++ compiler
:cmt. wppaxp   wppdaxp    Alpha AXP  C++ compiler
wlink    wlink      &lnkname
wlib     wlibd      &libname
.millust end
.np
This provides better performance when using the &ide or &makname..
See the description of the
.mono !loaddll
preprocessing directive in &makname for more information.
:cmt. .bull
:cmt. The Win32 version of our tools can be used (hosted) under both Intel
:cmt. and DEC Alpha AXP platforms
:cmt. and can create Win32 applications (targets) for both Intel or DEC
:cmt. Alpha AXP platforms.
.endbull
.*
.beglevel
.*
.section Changes to the C++ Compiler for 11.0
.*
.begbull
.bull
The C++ compiler now optimizes empty base-classes to occupy zero
storage in the derived class memory layout.
The C++ Working Paper recently allowed this optimization to be
performed by conforming implementations.
Furthermore, the optimization has speed and size benefits.
There are certain classes of (broken) C++ programs that may not
function properly with the new optimization.
If you explicitly memset() an empty base class, you will be clearing
memory that you may not expect to be cleared since the "zero sized"
base class in actual fact shares storage with the first member of the
derived class.
A memset() of the entire derived class is fine though.
.bull
We have added support for the
.mono mutable
keyword which is used to indicate data members that can be modified
even if you have a
.us const
pointer to the class.
.exam begin
class S {
    mutable int x;
    void foo() const;
};

void S::foo() const {
    x = 1;  // OK since it is mutable
}
.exam end
.bull
We have added support for the
.mono bool
type along with
.mono true
and
.mono false.
.bull
We have added support for the
.mono explicit
attribute.
It marks a constructor so that it will not be considered for
overloading during implicit conversions.
.exam begin
struct S {
    explicit S( int );
};

S v = 1;    // error; cannot convert 'int' to 'S'
.exam end
.np
Suppose the class was changed as follows:
.exam begin
struct S {
  explicit S(int );
  S( char );
};

S v = 1;  // OK; S( char ) is called
.exam end
.np
The fact that
.mono S(int)
is not considered leaves
.mono S(char)
as the only way to satisfy the implicit conversion.
.bull
We have added support for name spaces.
.millust begin
namespace x {
    // anything that can go in file-scope
}
namespace {
    // anything in here is local to your module!
}
.millust end
.np
In the above example, you can access names in the namespace "x"
by "x::" scoping.
Alternatively, you can use the "using namespace x" statement
(thereby eliminating the need for "x::" scoping).
You can include a part of the namespace into the current scope with
the "using x::member" statement.
(also eliminating the need for "x::" scoping).
.autopoint
.point
Name spaces eliminate the hand mangling of names.
For example, instead of prefixing names with a distinguishing string
like "XPQ_" (e.g., XPQ_Lookup), you can put the names in a namespace
called "XPQ".
.point
Name spaces allow for private names in a module.
This is most useful for types which are used in a single module.
.point
Name spaces encourage the meaningful classification of implementation
components.
For example, code-generation components might reside in a namespace
called "CodeGen".
.endpoint
.bull
We have added support for RTTI (Run-Time Type Information).
.bull
We have added support for the new C++ cast notation.
It allows you to use less powerful casts that the all powerful C-style
cast and to write more meaningful code.
The idea is to eliminate explicit casts by using a more meaningful
new-style cast.
The new C++ casts are:
.np
.bi reinterpret_cast
.us < type-id >(expr)
.br
.bi const_cast
.us < type-id >( expr )
.br
.bi static_cast
.us < type-id >( expr )
.br
.bi dynamic_cast
.us < type-id >( expr )
(part of RTTI)
.bull
We have improved (faster) pre-compiled header support.
.bull
.ix '__int64'
We have added "long long" (64-bit integer) support in the form
of a new
.us __int64
type.
.bull
The default structure packing was changed from "zp1" to "zp2" in the
16-bit compiler and from "zp1" to "zp8" in the 32-bit compiler.
.bull
The default type of debugging information that is included in object
files is "Dwarf".
It used to be "Watcom".
.bull
A new double-byte string processing option has been added (zkl).
When this option is specified, the local or current code page
character set is used to decide if the compiler should process strings
as if they might contain double-byte characters.
.endbull
.*
.section Changes to the C Compiler for 11.0
.*
.begbull
.bull
We have improved (faster) pre-compiled header support.
.bull
.ix '__int64'
We have added "long long" (64-bit integer) support in the form
of a new
.us __int64
type.
.bull
The default structure packing was changed from "zp1" to "zp2" in the
16-bit compiler and from "zp1" to "zp8" in the 32-bit compiler.
.bull
The default type of debugging information that is included in object
files is "Dwarf".
It used to be "Watcom".
.bull
A new double-byte string processing option has been added (zkl).
When this option is specified, the local or current code page
character set is used to decide if the compiler should process strings
as if they might contain double-byte characters.
.endbull
.*
.section Changes to the Code Generator for 11.0
.*
.begbull
:cmt. .bull
:cmt. Generation of code for the Digital Equipment Corporation Alpha AXP
:cmt. processor is now possible.
:cmt. .bull
:cmt. For the DEC Alpha AXP, we support Microsoft-compatible calling conventions
:cmt. and in-line assembly formats.
.bull
We support Microsoft-compatible in-line assembly formats using the
"_asm" keyword.
.bull
A new optimization, "branch prediction", has been added.
This optimization is enabled by the "ob" or "ox" compiler options.
The code generator tries to increase the density of cache use by
predicting branches based upon heuristics (this optimization
is especially important for Intel's Pentium Pro).
.bull
We have added Multi-media Extensions (MMX) support to the in-line
assemblers.
.bull
.ix '__int64'
We have added "long long" (64-bit integer) support in the form
of a new
.us __int64
type.
.endbull
.*
.section Changes to the Compiler Tools for 11.0
.*
.begbull
:cmt. .bull
:cmt. The &dbgname now supports debugging of applications running on the DEC
:cmt. Alpha AXP processor.
.bull
The &lnkname supports incremental linking.
.bull
The &lnkname can now process COFF and ELF format object files, as
well as OMF et al.
The &lnkname can now read both AR-format (Microsoft compatible)
libraries and old-style OMF libraries.
.bull
Support for creating 16-bit DOS overlaid executables has been removed
from the linker.
.bull
The &libname (&libcmdup) can now process COFF and ELF format object
files, as well as OMF et al.
The &libname can now read/write both AR-format (Microsoft compatible)
libraries and old-style OMF libraries.
The default output format is AR-format and this can be changed by
switches.
The &libname can output various format import libraries.
.bull
We have added Multi-media Extensions (MMX) support to the &asmname
(&asmcmdup).
:cmt. .bull
:cmt. A new assembler for the DEC Alpha AXP is included in the package.
.bull
A new version of the &disname (&discmdup) can disassemble Intel
or Alpha AXP object code files.
It can process ELF, COFF or OMF object files and ELF, COFF or PE
format (Win32) executables.
The &disname looks at image file being input to determine the
processor-type (defaults to Intel).
.np
The old disassembler (WDISASM) has been retired and is not included in
the package.
.bull
We have added new tool front-ends that emulate Microsoft tools.
These are:
.begbull $compact
.bull
nmake
.bull
cl
.bull
link
.bull
lib
.bull
rc
.bull
cvtres
.endbull
.np
These programs take the usual Microsoft arguments and translate them,
where possible, into equivalent &company arguments and spawn the
equivalent &company tools.
.bull
&makname now processes Microsoft format makefiles when the "ms" option
is used.
.endbull
.*
.section Changes to the C/C++ Libraries for 11.0
.*
.begbull
.bull
We have added multi-byte and wide character (including UNICODE)
support to the libraries.
.bull
We include run-time DLLs for the C, Math and C++ Libraries.
.bull
We have added Multi-media Extensions (MMX) support to the libraries.
.bull
The following new functions were added to the library...
.begnote
.note multi-byte functions
.endnote
.bull
The
.kw clock
function accuracy has changed from 100 ticks per second to 1000 ticks
per second (i.e.,
.kw CLOCKS_PER_SEC
has changed).
.bull
A "commit" flag ("c") was added to the fopen()
.us mode
argument.
.bull
.ix 'BINMODE'
The global translation mode flag default is "text" unless you
explicitly link your program with
.fi BINMODE.OBJ.
.bull
Processing of the "0" flag in the format string for the printf()
family of functions has been corrected such that when a precision is
specified, the "0" flag is ignored.
.exam begin
printf( "%09.3lf\n", 1.34242 ); // "0" flag is ignored
printf( "%09lf\n", 1.34242 ); // "0" flag is not ignored
.exam end
.bull
.ix '__int64'
Support for printing
.us __int64
values was added to
.kw printf
and related functions.
.bull
.ix '__int64'
Support for scanning
.us __int64
values was added to
.kw scanf
and related functions.
.bull
The Win32
.kw _osver
variable was added to the library.
.bull
The Win32
.kw _winmajor
.ct ,
.kw _winminor
and
.kw _winver
variables were added to the library.
.endbull
.*
.section Changes to the DOS Graphics Library for 11.0
.*
.begbull
.bull
The graphics library now performs the VESA test
.us before
testing for vendor specific graphics cards.
This fix is intended to broaden the number of graphics cards that are
supported.
.endbull
.*
.section Changes in Microsoft Foundation Classes Support for 11.0
.*
.begbull
.bull
Version 4.1 of the 32-bit MFC is included in the package.
.bull
Version 2.52b of the 16-bit MFC is included in the package.
.endbull
.*
.section Changes in Microsoft Win32 SDK Support for 11.0
.*
.begbull
.bull
The Win32 SDK is supported for Windows 95 and Windows NT platforms.
.endbull
.*
.section Changes in Blue Sky's Visual Programmer for 11.0
.*
.begbull
.bull
A new 32-bit version of Visual Programmer is included in the package.
This version runs on 32-bit Windows 95 and NT.
The 16-bit version of Visual Programmer is no longer included in the
package.
.bull
You can generate 16-bit applications with it, but you must be careful
to avoid using Win95 controls.
.bull
This new version fixes all known bugs in the previous version.
.endbull
.*
.endlevel
.*
.do end
.*
.if &version ge 106 .do begin
.*
.section Changes in 10.6 that may Require Recompilation
.*
.begnote
.note _diskfree_t
The struct members of the _diskfree_t structure has been changed from
UNSIGNED SHORTs to UNSIGNED INTs. This is to deal with possible HPFS
partitions whose size will overflow a short, as well as Microsoft
compatibility.
.note clock()
The
.kw clock
function accuracy has changed from 100 ticks per second to 1000 ticks
per second (i.e.,
.kw CLOCKS_PER_SEC
has changed).  Source code that
uses the
.kw clock
function and
.kw CLOCKS_PER_SEC
in its calculations must be recompiled before linking the application
with new libraries.
.endnote
.*
.section Major Differences from Version 10.5
.*
.np
The following sections summarize the major differences from the
previous release of Watcom &lang..
.*
.beglevel
.*
.section Windows 95 Help File Format
.*
.np
We have included Windows 95 format help files.
.*
.section Changes to the C++ Compiler in 10.6
.*
.np
We have improved Microsoft compatibility so that Win32 SDK and MFC
header files can be compiled without change.
The following changes were required to support Win32 SDK header files.
.begbull
.bull
We recognize the single underscore versions of
.mono __stdcall,
.mono __inline,
and
.mono __fastcall
keywords.
.bull
The
.mono _fastcall
and
.mono __fastcall
keywords are scanned but ignored since they refer to a particular
Microsoft code generation technique.
&company's generated code is always "fast".
.endbull
.np
The following changes were required to support MFC source code.
.begbull
.bull
When /bt=DOS is specified, define
.mono _DOS.
.bull
When /bt=WINDOWS is specified, define
.mono _WINDOWS.
.bull
When /m[s|m|c|l|h] is specified, define
.mono __SW_M[S|M|C|L|H]
and
.mono _M_I86[S|M|C|L|H]M.
.endbull
.np
The compiler now supports the C++ Standard Template Library (STL).
This library is available at the ftp site "butler.hpl.hp.com".
When compiling applications that use the STL, you must use the
"hd" compiler option for debugging info (the "hw" option causes
too much debug information to be generated).
.*
.section Changes to the C Compiler in 10.6
.*
.np
We have improved Microsoft compatibility so that Win32 SDK and MFC
header files can be compiled without change.
The following changes were required to support Win32 SDK header files.
.begbull
.bull
Support for the single underscore version of the
.mono __stdcall
keyword.
.bull
When /bt=DOS is specified, define
.mono _DOS.
.bull
When /bt=WINDOWS is specified, define
.mono _WINDOWS.
.endbull
.np
The following changes were required to support SDK sample code.
.begbull
.bull
You can specify calling convention information in a function prototype
and you do not have to specify the same information in the definition.
(Note: This is required by the OS/2 Warp SDK samples.)
.bull
Structured exception handling is supported (
.ct
.mono __try,
.mono __except
and
.mono __finally
keywords).
.bull
Allow initialization of automatic array/struct data using variables
and function calls.
.endbull
.*
.section Changes to the C Library in 10.6
.*
.np
The following new functions were added to the library.
.begnote
.note _getw
read int from stream file
.note _putw
write int to stream file
.endnote
.np
The
.kw clock
function accuracy has changed from 100 ticks per second to 1000 ticks
per second (i.e.,
.kw CLOCKS_PER_SEC
has changed).
.*
.section Changes in Microsoft Foundation Classes Support for 10.6
.*
.begbull
.bull
Version 3.2 of the 32-bit MFC is included in the package.
.bull
Version 2.52b of the 16-bit MFC is included in the package.
.endbull
.*
.section Changes to the Image Editor in 10.6
.*
.begbull
.bull
Support has been added for 256 color bitmaps.
.bull
Support has been added for 16 X 16 icons.
.bull
Support has been added for 48 X 48 icons.
.endbull
.*
.section Changes to the Dialog Editor in 10.6
.*
.begbull
.bull
Support has been added for Windows 95 controls.
.bull
Support has been added for adding new control styles to existing
controls.
.bull
Support has been added for new dialog styles.
.bull
Support has been added for allowing help IDs to be specified in dialog
and control statements.
.bull
Support has been added for generating new resource statements in .RC
files.
.endbull
.*
.section Changes to the Resource Editor in 10.6
.*
.begbull
.bull
Support has been added for new Windows 95 DIALOGEX resource type.
.bull
Support has been added for generating new DIALOGEX resource statements
in .RC files.
.endbull
.*
.section Changes to the Resource Compiler in 10.6
.*
.begbull
.bull
Support has been added for extended styles for dialogs.
.bull
Support has been added for the RCINCLUDE keyword.
.endbull
.*
.endlevel
.*
.do end
.*
.section Major Differences from Version 10.0
.*
.begbull
.bull
New installation program
.if &e'&beta eq 0 .do begin
.bull
&vpname for Windows (MFC) applications
.do end
.bull
MFC 3.0 support
.bull
Native C++ exception handling support
.bull
Improved language compatibility with Microsoft
.bull
Browser can now be used to browse C code
.bull
OS/2 3.0 Warp support
.bull
Toolkit for OS/2 1.3
.bull
Windows NT 3.5 support
.bull
Toolkit for Windows NT 3.5
.bull
Windows 95 (Chicago) support
.bull
Source Revision Control System hooks in editor
.bull
TCP/IP remote debug servers for OS/2 and Windows NT/95
.endbull
.np
In addition to these new features, we have also made a number of
improvements to the software.
.autonote
.note
The editor is more tightly integrated with the IDE.
.note
It is now easier to select your own favorite editor from the IDE.
.note
The keyboard interface in the Integrated Development Environment (IDE)
has been improved.
.note
The "fr" option, which is supported by the compilers & assembler,
can be used to name the error file drive, path, file name and/or
extension.
.note
We have added the "t<number>" option to the C++ compiler to set the
number of spaces in a tab stop (for column numbers in error messages).
.note
The C compiler now supports @filename on the command line like the C++
compiler currently does.
.note
The "__stdcall" linkage convention has changed.
All C symbols (extern "C" symbols in C++) are now suffixed by "@nnn"
where "nnn" is the sum of the argument sizes (each size is rounded up
to a multiple of 4 bytes so that char and short are size 4).
When the argument list contains "...", the "@nnn" suffix is omitted.
This was done for compatibility with Microsoft.
Use the "zz" option for backwards compatibility.
.note
The 32-bit "__cdecl" linkage convention has changed.
Watcom &lang 10.0 __cdecl did not match the Microsoft Visual C++ __cdecl
in terms of the binary calling convention; Visual C++ saves EBX in a
__cdecl function but Watcom &lang 10.0 modified EBX.
Watcom &lang has been changed to match Visual C/C++.
.np
If you wrote a "__cdecl" function in an earlier version of Watcom &lang,
the EBX register was not saved/restored.
Starting with release 10.5, the EBX register will be saved/restored
in the prologue/epilogue of a "__cdecl" function.
.np
Another implication of this change is that "__cdecl" functions
compiled with an earlier version of Watcom &lang don't match the calling
conventions of the current version.
The solution is either to recompile the functions or to define a
"__cdecl_old" pragma that matches the old calling conventions.
.millust begin
#pragma aux __cdecl_old "_*" \
            parm caller [] \
            value struct float struct routine [eax] \
            modify [eax ebx ecx edx];

#pragma aux (__cdecl_old) foo;

extern int foo( int a, int b );

void main()
{
    printf( "%d\n", foo( 1, 2 ) );
}

.millust end
.note
We now allow:
.millust begin
extern "C" int __cdecl x;
.millust end
.np
It must be
.id extern "C"
for
.id __cdecl
to take effect since variables have their type mangled into the name
for "C++" linkage.
.note
In C++, we have removed the warning for "always true/false"
expressions if the sub-expressions are constant values.
.note
We have added support for:
.millust begin
#pragma pack(push,4);
#pragma pack(push);
#pragma pack(pop)
.millust end
.note
We have added support for:
.millust begin
#pragma comment(lib,"mylib.lib")
.millust end
.pc
which has the same semantics as:
.millust begin
#pragma library( "mylib.lib" )
.millust end
.note
We have added support for expanding macros in the code_seg/data_seg
pragmas:
.millust begin
#define DATA_SEG_NAME "MYDATA"
#define CODE_SEG_NAME "MYCODE"

#pragma data_seg( DATA_SEG_NAME )
int x = 3;

#pragma code_seg( CODE_SEG_NAME )
int fn() {
    return x;
}
.millust end
.note
We have fixed the 16-bit compiler so that it matches the Microsoft
16-bit C compiler for the following cases:
.begbull
.bull
If a pascal function is defined when compiling for Windows 3.x, use the
fat Windows 3.x prologue in the function.
.bull
If a cdecl function is defined when compiling for Windows 3.x, use the fat
Windows 3.x prologue in the function.
.endbull
.note
We have fixed the compiler so that
.millust begin
#include </dir/file.h>
.millust end
.pc
works as expected (it was searching along the INCLUDE path only).
.note
In C++, we have fixed a problem where an import was generated in the
object file for a virtual function call.
This will reduce the size of executables under certain circumstances.
.note
In C++, we have removed the prohibition of pointer to array of unknown
size declarations.
.exam begin
int (*p)[];
.exam end
.note
In C++, we have fixed the diagnosis of lexical problems during macro
expansion to remove spurious warnings.
.exam begin
#define stringize( x )  #x

stringize( 2131231236172637126371273612763612731 )
.exam end
.note
We have corrected the check for too many bytes in #pragma for
assembler style aux #pragmas.
.note
Undeclared class names in elaborated class specifiers are now declared
in the nearest enclosing non-class scope.
Undeclared classes are also allowed in arguments now.
.exam begin
struct S {
    // used to declared ::S::N but now declares ::N
    struct N *p;
};

void foo( struct Z *p );    // declares ::Z
.exam end
.note
We have fixed unduly harsh restriction on virtual ...-style functions.
They are now allowed in single inheritance hierarchies as long as the
return type is not changed when the virtual function is overridden. In
multiple inheritance hierarchies, an implementation restriction is
still present for generating a 'this' adjustment thunk for virtual
...-style functions.
.note
We have fixed line number information for multi-line statement
expressions in some weird cases.
.note
We have fixed function template parsing of user-defined conversions
that use an uninstantiated class in their operator name.
.exam begin
void ack( int );

template <class T>
    struct S {
        S( T x )
        {
            ack( x );
        }
    };
.exam break
template <class T>
    struct W {
        operator S<T>();
    };
.exam break
template <class T>
    W<T>::operator S<T>() {
        return 0;
    }
.exam end
.note
We have fixed a compiler problem that caused a linker warning "lazy
reference for <virtual-fn> has different default resolutions" in cases
where the compiler or programmer optimized virtual function calls to
direct calls in modules that also contained virtual calls.
.exam begin
T.H
    struct S {
        virtual int foo() { return __LINE__; }
    };
    struct T : S {
        virtual int foo() { return __LINE__; }
    };
.exam break
T1.CPP
    #include "t.h"
    struct Q : T {
        virtual int foo() { return S::foo() + __LINE__; }
    };

    void foo( T *p )
    {
        Q y;
        y.foo();
        p->foo();
    }
.exam break
T2.CPP
    #include "t.h"

    void foo( T *p );

    void ack( T *p ) {
        p->foo();
        foo(p);
    }
.exam break
    main() {
        T q;
        ack( &q );
    }
.exam end
.note
When a class value is returned and is immediately (in the same
expression) used to call a member function, the value may not be
stored in memory.
.np
Work around: introduce a temporary
.exam begin
struct S {
    int v;
    int member();
};

S foo();
.exam break
void example( void )
{
    // foo().member();  // replace this line with:
    S temp = foo();
    temp.member();
}
.exam end
.note
Throwing pointers to functions did not work when the size of a
function pointer is greater than the size of a data pointer.
.np
Work around: place the function pointer in a class and throw the class
object.
.note
We have fixed default argument processing for const references to an
abstract class. The following example would not compile properly:
.exam begin
struct A {
    virtual int foo() = 0;
};

A &foo();
.exam break
void ack( A const &r = foo() );

void bar() {
    ack();
}
.exam end
.note
We have made "DllMain" default to extern "C" linkage for Microsoft
Visual C++ compatibility.
.note
We have duplicated a Microsoft Visual C++ extension that was required
to parse the Windows 95 SDK header files.
.exam begin
typedef struct S {
} S, const *CSP;
     ^^^^^- not allowed in ISO C or ISO C++
.exam end
.note
We now do not warn about starting a nested comment if the comment is
just about to end.
.np
We also fixed the code that figures out where a comment was started so
that a nested comment warning is more helpful.
.exam begin
        /*/////////*/
                   ^-
.exam end
.note
We have fixed a problem where extra informational notes were not being
printed for the error message that exceeded the error message limit.
.exam begin
// compile -e2
struct S {
    void foo();
};
.exam break
void foo( S const *p )
{
    p->foo();
    p->foo();
    p->foo();
    p->foo();
}
.exam end
.note
We have fixed a problem where the line number for an error message was
incorrect.
.exam begin
struct S {
    void foo() const;
    void bar();
};
.exam break
void S::foo() const
{
    bar();

    this->bar();

}
.exam end
.note
We have fixed output of browser information for instantiated function
template typedefs.
.note
We have upgraded the C++ parser so that casts and member pointer
dereferences can appear on the left hand side of the assignment
expression without parentheses.
.exam begin
p->*mp = 1;
(int&)x = 1;
.exam end
.note
In several cases, when a function return or a construction was
immediately dotted in an expression, the generated code was incorrect:
.exam begin
struct S {
  int x;
  int foo();
};
.exam break
extern S gorf();

void bar()
{
    gorf().foo();
}
.exam end
.np
The work around was to break the statement in two:
.exam begin
S temp = gorf();
temp.foo();
.exam end
.note
In several cases, when a function return or a construction was
immediately addressed in an expression, the generated code was
incorrect:
.exam begin
struct S {
  int x;
};
.exam break
extern void fun( S* );

extern S gorf();
.exam break
void bar()
{
    fun( &gorf() );
}
.exam end
.np
The work around was to break the statement in two:
.exam begin
S temp = gorf();
fun( &temp );
.exam end
.note
We have added support for:
.millust begin
#pragma error "error message"
.millust end
.np
Use the ISO/ANSI method because it is more portable and acceptable
(Microsoft header files use the less portable #pragma when there
is a perfectly fine, portable way to issue a message).
.np
The portable, acceptable method is:
.millust begin
#error "error message"
.millust end
.note
We have added support for
.mono __declspec(dllexport),
.mono __declspec(dllimport),
.mono __declspec(thread),
and
.mono __declspec(naked)
for Win32 (i.e., WinNT 3.5 and Win95) programs. Here are some
examples:
.exam begin
__declspec(dllexport) int a;        // export 'a' variable
__declspec(dllexport) int b()       // export 'b' function
{
}
.exam break
struct __declspec(dllexport) S {
    static int a;                   // export 'a' static member
    void b();                       // export 'b' member fn
};
.exam break
extern __declspec(dllimport) int a; // import 'a' from a .DLL
extern __declspec(dllimport) int b();//import 'b' from a .DLL

struct __declspec(dllimport) I {
    static int a;                   // import 'a' static member
    void b();                       // import 'b' member fn
};
.exam end
.note
The C++ compiler generates better error messages for in-class
initializations and pure virtual functions.
.exam begin
struct S {
    static int const a = 0;
    static int const b = 1;
    void foo() = 0;
    void bar() = 1;
    virtual void ack() = 0;
    virtual void sam() = 1;
};
.exam end
.note
We have fixed macro processing code so that the following program
compiles correctly. The compiler was not treating "catch" as a keyword
after the expansion of "catch_all".
.exam begin
#define catch(n) catch(n &exception)
#define xall (...)
#define catch_all catch xall
.exam break
main()
{
    try{
    }
    catch_all{
    }
}
.exam end
.note
We have fixed a problem where
.mono #pragma code_seg
caused a page fault in the compiler when the code_seg was empty.
.note
We have fixed a rare problem where a #include of a file that was
previously included caused the primary source file to finish up if the
CR/LF pair for the line that the #include was on, straddled the C++
compiler's internal buffering boundary.
.note
We have added support for
.mono #pragma message( "message text" ).
It outputs a message to stdout when encountered. It is used in
Microsoft SDK header files to warn about directly including header
files and obsolete files.
.note
We have fixed #pragma code_seg/data_seg to properly set the class name
of the new segment in the object file.
.note
We have a fixed a problem with the -zm -d2 options that caused a
compiler fault in some circumstances.
.note
We have fixed default library records in .OBJ file so that user
libraries are ahead of default compiler libraries in the linker search
order.
.note
We have fixed handling of intrinsic math functions so that the code
generator will treat functions like sqrt as an operator.
.note
We have added support for using OS-specific exception handling
mechanisms for C++ exception handling during code generation. Enable
it with the new -zo option.
.note
.mono __stdcall
functions now have Microsoft Visual C/C++ compatible name mangling.
.note
We have added a number of new functions to the C Library. These have
been added to improve Microsoft compatibility.
.millust begin
dllmain (nt only)
libmain (nt only)
_access
_dos_commit
_dup
_ecvt
_fcvt
_fstat
_fstrdup
_gcvt
_itoa
_itoa
_locking
_lseek
_ltoa
_ltoa
_memicmp
_set_new_handler
_stat
_strdate
_strdup
_stricmp
_strlwr
_strnicmp
_strrev
_strtime
_strupr
_tolower
_toupper
__isascii
__iscsym
__iscsymf
.millust end
.note
In version 9.5, the linker used to include LIBFILE object files in
reverse order (i.e., the last one listed was the first to be
included). We have corrected this behavior so that they are included
in the order listed.
.millust begin
Directive           Old Order   New Order
-------------       ---------   ---------
FILE    obj_a           3           3
LIBFILE obj_b           2           1
LIBFILE obj_c           1           2
FILE    obj_d           4           4
.millust end
.np
In the above example, the object files will be included in the order
indicated (LIBFILE object files are always included first).
.endnote
.*
.beglevel
.*
.section Changes in 10.5 that may Require Recompilation
.*
.begnote
.note __stdcall
If you use the __stdcall attribute within a program then you must
re-compile the function definition and all callers of the __stdcall
function.
.note __cdecl
The __cdecl attribute is not heavily used in Win32 programming so the
impact should be minimal but if you do use __cdecl within your own
programs, a re-compilation will be necessary.
.endnote
.*
.endlevel
.*
.section Major Differences from Version 10.0 LA
.*
.np
If you have .tgt files created with the Limited Availability
or Beta Integrated Development Environment, when you load
them, the target window may say "Obsolete Form: rename target type".
If it does:
.autopoint
.point
Select the target window by clicking in it,
.point
Choose "rename target" from the target menu (a rename target dialog
will appear),
.point
Reselect the target type for this target (e.g., Win32 EXE), and
.point
Select OK.
.endpoint
.np
You should not continue to use .cfg files from the Limited
Availability version of the compiler. Several new features have been
added. Using the old files will cause problems.
.np
The C++ compiler calling conventions have changed. Any program that
passes a "data only" class or struct as a parameter, or returns a C++
object will need to be recompiled. We recommend that you recompile
your application.
.np
The C++ compiler now supports the use of the
.kw __export
.ct ,
.kw __cdecl
.ct ,
.kw __pascal
.ct ,
.kw __stdcall
and
.kw __syscall
keyword on class definitions.
These keywords will affect all members of the defined class.
.*
.section Major Differences from Watcom C9.5 /386
.*
.begbull
.bull
The functionality of Watcom C/C++(16) and Watcom C/C++(32) is included in a single
package.
.bull
An Integrated Development Environment for Windows 3.x,
Windows NT, Windows 95 and OS/2 PM is included.
.bull
New, redesigned debugger with GUI interfaces for Windows 3.x,
Windows NT, Windows 95 and OS/2 PM is included.
.bull
The optimizer has been enhanced.
.bull
C++ Class Browser
.bull
New, redesigned user interface for the Profiler.
.bull
New support for C and C++ precompiled header files.
.bull
Windows resource editing tools are included:
.begpoint $compact
.point Dialog Editor
.point Bitmap Editor
.point Resource Editor
.point Menu Editor
.point String Editor
.point Accelerator Editor
.endpoint
.bull
Windows development tools are included:
.begpoint $compact
.point Dr. Watcom (a post mortem debug utility)
.point Spy (Windows message spy program)
.point DDESpy
.point Heap Walker
.point Zoom
.endpoint
.bull
On-line documentation is included.
.bull
Microsoft Foundation Classes for 32-bit Windows applications (&mfc32)
and 16-bit Windows 3.1 applications (&mfc16) is included.
.bull
Creation of FlashTek DOS extender applications is supported.
.bull
Compiler executables have been created that run under all supported
operating systems. They are located in the BINW directory.
.endbull
.*
.beglevel
.*
.section Items No Longer Supported
.*
.begbull
.bull
PenPoint development
.bull
Debugging of Ergo OS/386 DOS extender applications
.bull
DESQView remote debugging
.endbull
.*
.section Changes in 10.0 that may Require Recompilation
.*
.np
.us All C++ applications will require recompilation
due to changes in the underlying object model.
C applications should not require recompilation, but
.us you should recompile your application if you want
.us to take full advantage new features in the debugger.
The changes to the C++ object model are:
.begbull
.bull
Virtual table layout changed (NULL entry at offset 0 removed)
.bull
derived class packing adjusted to minimize padding bytes
.bull
exception handling code is improved (incompatible with 9.5)
.bull
name mangling for 'char' reduced from two chars to one char
.endbull
.*
.endlevel
.*
.section Major Differences from Watcom C9.01 /386
.*
.begbull
.bull
C++ support added
.bull
Pentium optimizations added ("5r", "5s", "fp5" options)
.bull
Windows NT support added
.bull
Microsoft resource compiler (RC.EXE) replaced with Watcom resource
compiler (WRC.EXE)
.bull
OS/2 libraries modified so that single library supports single and
multiple threads and DLL's
.bull
"fpi287" switch renamed to "fp2"
.bull
#pragma intrinsic and #pragma function added
.bull
80x87 usage modified so that compiler assumes all eight registers are
available upon entry to function instead of only four registers.
"fpr" option added for reverse compatibility with object files compiled
with previous versions of compiler
.endbull
.beglevel
.*
.section Changes that may Require Recompilation
.*
.np
The
.us stat
structure in "stat.h" changed in version 9.5.
Any object files compiled with an earlier version of the compiler
will have to be recompiled if they use the stat structure.
.np
A new function
.us _grow_handles
was added to version 9.5 for growing the number of available file
handles.
Details on how to use this function can be found in the
.us Watcom C/C++&S'32. Commonly Asked Questions & Answers
booklet.
.np
If you compile with structure packing (/zp2, /zp4, /zp8) or use the
"pack" pragma to specify a packing value other than 1, and you have
structures that contain other structures, field offsets and structure
sizes may be different depending on the contents of the structures.
.np
In version 9.01,
the new Windows Supervisor now has 32 user-defined callbacks.
If you have any user-defined callbacks in your Windows program, you
must recompile because the constant definitions have changed.
.np
In version 9.0,
the compiler will not use FS by default to avoid conflicts with new
operating systems (OS/2 2.0, PenPoint, Windows NT).
This will cause compile errors if you have defined a pragma that
uses the FS register.
.np
In version 8.5,
the compiler was changed so that it by default does not save
and restore segment registers across function calls.
This is to solve problems that occur where a segment register is
saved and restored in a function that tries to free the segment.
When the segment register is popped from the stack in the epilogue,
a general protection exception occurs because the selector is no
longer valid.
In order to provide backward compatibility with existing code, we have
added a "-r" option that will cause the compiler to save and restore
segment registers across calls.
The C run-time library has been compiled with the "-r" option so that it
will work should you choose to compile your application with the same
option.
.np
The packing algorithm was also changed in version 8.5.
If you are using one of the "-zp2, -zp4, or -zp8" options to pack
structures, you must recompile your application.
The packing algorithm has been changed so that the minimum number of
slack bytes are inserted into structures to align fields onto their
required alignment.
.*
.endlevel
.*
.section Major Differences from Watcom C9.0 /386
.*
.begbull
.bull
Windows 3.1 SDK components
.bull
Support for Windows 3.1 DLLs
.bull
On-line Watcom C Library Reference help file for OS/2 2.0.
Help can be accessed by issuing the command "VIEW WATCOMC".
The command can be followed by a topic. e.g. VIEW WATCOMC PRINTF.
.endbull
.cp 11
.*
.beglevel
.*
.section Command Line Options added to Watcom C9.0 /386
.*
.begnote $break
.note 4r
Use register calling conventions and optimize for 486.
.note 4s
Use stack calling conventions and optimize for 486.
.note ee
Generate a call to __EPI at the end of a function
.note ep{=number}
Generate a call to __PRO at the start of a function
.note oe
In-line user defined functions.
.note or
Reorder instructions to take advantage of 486 pipelining.
.note zff
Allows the FS register to be used by the code generator for far pointers.
.note zfp
Disallows use of the FS register.
This is the default in flat memory model,
because operating systems are now using FS to
point to important information. (e.g. OS/2 2.0, PenPoint, Windows NT).
.note zm
Places each function into a separate segment.
This will allow for smart linking.
.endnote
.*
.endlevel
.*
.section Major Differences from Watcom C8.5 /386
.*
.np
Several major items have been added to Watcom C9.0 /386. They are:
.begbull $compact
.bull
486 instruction selection
.bull
486 instruction scheduling
.bull
Tail recursion elimination
.bull
Function inlining
.bull
strcmp function optimized for 486
.bull
Support for OS/2 2.0
.bull
New keywords added (_far16, _Seg16, _Packed, _Cdecl, _Pascal, _System)
.bull
Linkage pragma for compatibility with IBM C Set/2
.bull
Based pointers
.bull
Machine language code sequences can now be entered into pragmas
using assembly language instead of numeric codes.
.* .bull
.* Post-mortem debugging capability for Windows 3.x.
.bull
Remote debugging over the parallel port using either a "LapLink" cable
or a "Flying Dutchman" cable.
.bull
Remote debugging of PenPoint applications
.endbull
.cp 10
.*
.beglevel
.*
.section Command Line Options added to Watcom C8.5 /386
.*
.begnote $break $compact
.note d1+
to generate line number information plus typing information for global
symbols and local structs and arrays
.note ei
force all enumerated types to be of type 'int'
.note en
emit routine name before prologue
.note ez
generate PharLap EZ-OMF object file
.note fpi287
for programs that run on a system with a 287
.note of
to generate traceable stack frames
.note of+
to generate traceable stack frames for all functions
.note om
to generate in-line math functions
.note p
to generate preprocessor output
.note pl
to generate preprocessor output with #line directives
.note pc
to generate preprocessor output preserving comments
.note plc
to generate preprocessor output with #line directives and preserving
comments
.note r
save/restore segment registers across calls
.note we
treat warnings as errors
.endnote
.*
.endlevel
.*
.section Major Differences from Watcom C8.0 /386
.*
.np
Several major items were added to Watcom C8.5 /386. They are:
.begbull $compact
.bull
Royalty-free 32-bit DOS extender
.bull
Windows 3.0 support
.bull
Windows SDK components included
.bull
Improved optimizations
.bull
OS/2 hosted version of the compiler
.bull
The compiler now generates Microsoft format object files by default.
Use the '/ez' option to generate Phar Lap EZ-OMF object files.
.bull
More library functions to create higher compatibility with
Microsoft C 5.1 and Microsoft C 6.0.
.bull
Preprocessor output from the compiler
.bull
Standalone help utility
.bull
Object module convert utility can convert debugging information into
CodeView format
.bull
Protected-mode version of the linker
.bull
Debugger support for ADS applications
.bull
Support for Pharlap 3.0 and Pharlap 4.0
.bull
Support for Tenberry Software DOS/4G
.bull
Support for Intel 386/486 Code Builder Kit
.bull
Support for UNICODE.
Literal strings and character constants that are preceded by "L" map
code page 437 onto UNICODE unless one of the options "-zk0, -zk1, or -zk2"
is specified.
.endbull
.*
.beglevel
.*
.section Command Line Options added to Watcom C8.0 /386
.*
.* .begnote $break
.begnote $compact
.note d2
symbolic debugging information
.note oc
disable "call" followed by "ret" being changed into a "jmp"
optimization
.note u<name>
undefine a pre-defined name
.note zc
places literal strings in the CODE segment
.note zk{0,1,2}
double-byte character support
.endnote
.*
.endlevel
.*
.section Major Differences from Watcom C7.0 /386
.*
Several major items have been added to Watcom C8.0 /386. They are:
.begbull
.bull
Linker
.bull
Librarian
.bull
Graphics library
.bull
More library functions to create higher compatibility with
Microsoft C 5.1 and Microsoft C 6.0.
.endbull
.np
The professional edition also gives you the following most asked for
features:
.begbull
.bull
Protected-mode version of the compiler
.bull
Full-screen source-level debugger
.bull
Execution profiler
.endbull
.*
.beglevel
.*
.section Protected-mode Compiler and Linker
.*
.np
The protected-mode version of the compiler "WCC386P.EXE" and linker
"WLINKP.EXE" use a DPMI compliant DOS extender.
This allows you to run the compiler and linker on a normal
DOS system or in a Windows 3.x DOS box operating in enhanced mode.
.*
.endlevel
.*
.do end
.*
.if '&lang' eq 'FORTRAN 77' .do begin
.*
.if &version ge 110 .do begin
.*
.section Changes in 11.0 that may Require Recompilation
.*
.np
Do not attempt to mix object code generated by earlier versions of the
compilers with object code generated by this release or with the
libraries provided in this release.
.begnote
.note clock()
The
.kw clock
function accuracy has changed from 100 ticks per second to 1000 ticks
per second (i.e.,
.kw CLOCKS_PER_SEC
has changed).  Source code that
uses the
.kw clock
function and
.kw CLOCKS_PER_SEC
in its calculations must be recompiled before linking the application
with new libraries.
.endnote
.*
.section Major Differences from Version 10.6
.*
.np
The following sections summarize the major differences from the
previous release of Watcom &lang..
.begbull
.bull
Some of the Win32 and 32-bit OS/2 versions of our tools are now
available in DLL form.
.millust begin
EXE      DLL        Description
------   -------    -----------------------
wlink    wlink      &lnkname
wlib     wlibd      &libname
.millust end
.np
This provides better performance when using the &ide or &makname..
See the description of the
.mono !loaddll
preprocessing directive in &makname for more information.
.endbull
.*
.beglevel
.*
.section Changes to the Code Generator for 11.0
.*
.begbull
.bull
A new optimization, "branch prediction", has been added.
This optimization is enabled by the "OBP" or "OX" compiler options.
The code generator tries to increase the density of cache use by
predicting branches based upon heuristics (this optimization
is especially important for Intel's Pentium Pro).
.bull
We have added Multi-media Extensions (MMX) support to the in-line
assembler.
.endbull
.*
.section Changes to the Compiler Tools for 11.0
.*
.begbull
.bull
The &lnkname supports incremental linking.
.bull
The &lnkname can now process COFF and ELF format object files, as
well as OMF et al.
The &lnkname can now read both AR-format (Microsoft compatible)
libraries and old-style OMF libraries.
.bull
Support for creating 16-bit DOS overlaid executables has been removed
from the linker.
.bull
The &libname (&libcmdup) can now process COFF and ELF format object
files, as well as OMF et al.
The &libname can now read/write both AR-format (Microsoft compatible)
libraries and old-style OMF libraries.
The default output format is AR-format and this can be changed by
switches.
The &libname can output various format import libraries.
.bull
We have added Multi-media Extensions (MMX) support to the &asmname
(&asmcmdup).
.bull
A new version of the &disname (&discmdup) is included.
It can process ELF, COFF or OMF object files and ELF, COFF or PE
format (Win32) executables.
.np
The old disassembler (WDISASM) has been retired and is not included in
the package.
.bull
We have added new tool front-ends that emulate Microsoft tools.
These are:
.begbull $compact
.bull
rc
.endbull
.np
These programs take the usual Microsoft arguments and translate them,
where possible, into equivalent &company arguments and spawn the
equivalent &company tools.
.bull
&makname now processes Microsoft format makefiles when the "ms" option
is used.
.endbull
.*
.section Changes to the C/C++ Libraries for 11.0
.*
.begbull
.bull
We have added multi-byte and wide character (including UNICODE)
support to the libraries.
.bull
We have added Multi-media Extensions (MMX) support to the libraries.
.bull
The
.kw clock
function accuracy has changed from 100 ticks per second to 1000 ticks
per second (i.e.,
.kw CLOCKS_PER_SEC
has changed).
.endbull
.*
.section Changes to the DOS Graphics Library for 11.0
.*
.begbull
.bull
The graphics library now performs the VESA test
.us before
testing for vendor specific graphics cards.
This fix is intended to broaden the number of graphics cards that are
supported.
.endbull
.*
.section Changes in Microsoft Win32 SDK Support for 11.0
.*
.begbull
.bull
The Win32 SDK is included for Windows 95 and Windows NT
platforms.
.endbull
.*
.endlevel
.*
.do end
.*
.if &version ge 106 .do begin
.*
.section Changes in 10.6 that may Require Recompilation
.*
.begnote
.note _diskfree_t
The struct members of the _diskfree_t structure has been changed from
UNSIGNED SHORTs to UNSIGNED INTs. This is to deal with possible HPFS
partitions whose size will overflow a short, as well as Microsoft
compatibility.
.note clock()
The
.kw clock
function accuracy has changed from 100 ticks per second to 1000 ticks
per second (i.e.,
.kw CLOCKS_PER_SEC
has changed).  Source code that
uses the
.kw clock
function and
.kw CLOCKS_PER_SEC
in its calculations must be recompiled before linking the application
with new libraries.
.endnote
.*
.section Major Differences from Version 10.5
.*
.np
The following sections summarize the major differences from the
previous release of Watcom &lang..
.*
.beglevel
.*
.section Windows 95 Help File Format
.*
.np
We have included Windows 95 format help files.
.*
.section Changes to the C Library
.*
.np
The following new functions were added to the library.
.begnote
.note _getw
read int from stream file
.note _putw
write int to stream file
.endnote
.np
The
.kw clock
function accuracy has changed from 100 ticks per second to 1000 ticks
per second (i.e.,
.kw CLOCKS_PER_SEC
has changed).
.*
.section Changes to the Image Editor
.*
.begbull
.bull
Support has been added for > 16 colors for bitmaps.
.bull
Support has been added for 16 X 16 icons.
.bull
Support has been added for 48 X 48 icons.
.endbull
.*
.section Changes to the Dialog Editor
.*
.begbull
.bull
Support has been added for Windows 95 controls.
.bull
Support has been added for adding new control styles to existing
controls.
.bull
Support has been added for new dialog styles.
.bull
Support has been added for allowing help IDs to be specified in dialog
and control statements.
.bull
Support has been added for generating new resource statements in .RC
files.
.endbull
.*
.section Changes to the Resource Editor
.*
.begbull
.bull
Support has been added for new Windows 95 resource types.
.bull
Support has been added for generating new resource statements in .RC
files.
.endbull
.*
.section Changes to the Resource Compiler
.*
.begbull
.bull
Support has been added for extended styles for dialogs.
.bull
Support has been added for the RCINCLUDE keyword.
.endbull
.*
.endlevel
.*
.do end
.*
.section Major Differences from Version 9.5
.*
.begbull
.bull
The functionality of &watf16 and &watf32 is included in a single
package.
.bull
OS/2 3.0 Warp is supported.
.bull
Windows NT 3.5 is supported.
.bull
Windows 95 (Chicago) is supported.
.bull
An Integrated Development Environment for Windows 3.x, Windows 95,
Windows NT and OS/2 PM is included.
.bull
New, redesigned debugger with GUI interfaces for Windows 3.x, Windows
95, Windows NT and OS/2 PM is included.
.bull
TCP/IP remote debug servers for OS/2 and Windows NT are included.
.bull
The optimizer has been enhanced.
.bull
New, redesigned user interface for the Profiler.
.bull
Windows resource editing tools are included:
.begpoint $compact
.point Dialog Editor
.point Bitmap Editor
.point Resource Editor
.point Menu Editor
.point String Editor
.point Accelerator Editor
.endpoint
.bull
Windows development tools are included:
.begpoint $compact
.point Dr. Watcom (a post mortem debug utility)
.point Spy (Windows message spy program)
.point DDESpy
.point Heap Walker
.point Zoom
.endpoint
.bull
On-line documentation is included.
.bull
Creation of FlashTek DOS extender applications is supported.
.bull
Compiler executables have been created that run under all supported
operating systems.
They are located in the BINW directory.
.endbull
.*
.beglevel
.*
.section Items No Longer Supported
.*
.begbull
.bull
Debugging of Ergo OS/386 DOS extender applications
.bull
DESQView remote debugging
.endbull
.*
.section Changes that may Require Recompilation
.*
.np
FORTRAN applications should not require recompilation, but
.us you should recompile your application if you want
.us to take full advantage new features in the debugger.
.*
.endlevel
.*
.section Major Differences from Version 9.0
.*
.np
The following lists some of the new features/differences between version
9.5 and 9.0.
.autopoint
.point
The "/resource" option has been added and allows run-time messages
to be placed in the executable file as resource information instead of
being linked with the application.
The resource file containing these messages is called "wf77.res" and is
locating in the root of the directory in which you installed the software.
The strip utility (WSTRIP) can be used to place the resource information in
the executable file.
Special libraries for processing resource information will be required when
you link your application.
These libraries are called "wresf.lib" (for 32-bit register calling
convention), "wresfs.lib" (for 32-bit stack calling convention),
"wresm.lib" (for 16-bit medium memory model),
and "wresl.lib" (for 16-bit large and huge memory models)
and are located in the "lib386" and "lib286" directories of the
directory in which you installed the software.
.point
The "/5" option has been added and causes the compiler to generate
code that is optimized for the Pentium.  This is the default.
The "/fp5" option has been added and causes the compiler to generate
floating-point code that is optimized for the Pentium.  This is not
the default.  The default floating-point option is "/fp3".
.point
The "/3" option has been added and causes the compiler to generate
code optimized for a 80386 processor.  The "/4" option has been added
and causes the compiler to generate code optimized for a 80486 processor.
The "/5" option has been added and causes the compiler to generate
code that is optimized for the Pentium.  In addition, the "/fp5" option
has been added and causes the compiler to generate floating-point code
that is optimized for the Pentium.
.point
The "/descriptor" option has been added which specifies that string
descriptors are to be passed for character arguments.  You can specify
the "/nodescriptor" option if you do not want string descriptors to be
passed for character arguments.  Instead, the pointer to the actual
character data and the length will be passed as two arguments.  The
arguments for the length will be passed as additional arguments following
the normal argument list.  For character functions, the pointer to the
data and the length of the character function will be passed as the first
two arguments.
.point
The "oi" option has been added.  This option causes code for statement
functions to be generated in-line.
.point
The floating-point model used when generating in-line 80x87 instructions
has changed.  If you wish to call subprograms compiled with version 9.0
you must specify the "/fpr" option.
.point
Support for records in text files separated by only a line feed character
has been added.  Previously, records in text files had to be separated
by a carriage return/line feed sequence.
.point
The "/debug" option has been divided into two separate options.  The
"/bounds" option generates array and character subscript checking code.
The "/trace" option generates code that allows a run-time traceback
to be generated when a error is issued.
.point
The "/quiet" option has been added to suppress banner/summary
information.  The "terminal" option only affects the display of diagnostic
messages (extensions, warnings and errors).  For example, compiling with
the "/quiet" option will only display diagnostic messages (if any).
.point
The "/cc" option has been added.  It specifies that unit 6 is a carriage
control device and that output to this unit will be assumed to contain
carriage control characters.  Note that a blank carriage control
character will automatically be generated for list-directed output.
.point
The "/automatic" option causes all local variables (including arrays) to
be allocated on the stack.  Care should be taken when using this option
since the stack requirements of your application may increase
dramatically.
.point
The "/wild" option has been added. It suppresses the compile-time
checking that normally causes an error to be issued when an attempt is
made to transfer control into a block structure from outside the block
structure and vice versa.  This option is only to be used when compiling
existing code that contains GOTO statements of this type; it does not
encourage this style of programming.
.point
National language support has been added.  The "/japanese" option causes
the compiler to process Japanese double-byte characters in variable names
and character data.  Similarly, the "/chinese" option will process
Traditional Chinese characters and the "/korean" will process Korean
characters.
.point
The "/ob" option has been added.  This allows the code generator to use
register ESP as a base register to reference local variables and
subprogram arguments.  Note that when this option is specified, the
compiler will abort when there is not enough memory to optimize the
subprogram.  By default, the code generator uses more memory-efficient
algorithms when a low-on-memory condition is detected.
.point
The GROWHANDLES() function has be added.  This function increases the
number of files that can be opened by a program to the specified
amount.  The value returned is the new maximum.  This may be less than
the requested amount because of operating system limitations.
.point
The IARGC() and IGETARG() functions have been added.  IARGC is an integer
function that returns the argument count.  IGETARG is an integer function
that takes two arguments.  The first argument is of type INTEGER and is
the index of the argument to be returned.  The second argument is of type
CHARACTER and is used to return the argument.  The value returned by the
function is the length of the argument.
.point
The SHARE= specifier has been added to the OPEN statement.  It allows
the program to specify how other processes can access a file that is
opened by the original process.  Possible values for the SHARE= specifier
are as follows.
.illust begin
DENYRW          - deny read and write access
DENYWR          - deny write access
DENYRD          - deny read access
DENYNO          - allow read and write access
COMPAT          - compatibility mode
.illust end
.endpoint
.*
.do end
