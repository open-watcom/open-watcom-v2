.chap Using the flat memory model
.np
.ix 'flat model'
Flat model is a non-segmented memory model that allows accessing data
and executing code anywhere within the 4GB linear address region via a
32-bit offset without the need to use segment registers to point to the
memory.  In many respects, the memory model is identical to the tiny
memory model well-known to DOS C and assembly language programmers, but
supporting a memory region of 4GB rather than 64KB.
.np
.ix 'DPMI host'
When using flat model, all linear addresses directly map to the physical
address specified when an application is running in pure DOS environment.
For example, writing to memory location 0B8000h will
address video memory.  Reading from memory locations in the 0FFF00h
range will access ROM code. Under DPMI there is usually no direct
relationship between linear and physical addresses, the DPMI host will
however emulate access to memory below 1MB as appropriate without
requiring any special considerations on the part of the application writer.
.np
.ix 'near model'
Generally speaking, flat is an improved version of the older near memory
model supported in previous versions of CauseWay.  Unlike near, flat model
supports multiple segments, mixing 16- and 32-bit segments, and direct
linear memory addresses without translation. Flat model
supports all near memory model code without translation, including
automatic handling of near-specific API functions.
.*
.chap Using DLLs with &product. C/C++
.*
.np
.ix 'DLL'
It is recommended that you understand and familiarize yourself with the
basic operation of DLLs (Dynamic Link Libraries) under Windows or OS/2 
before using them with CauseWay under DOS. No attempt is made here to 
explain the fundamentals of DLL architecture and operation.
You should also study the provided DLL example code.
.np
DLL code should be compiled with the &sw.s option to disable stack checking
and the &sw.bd option to generate DLL-suitable code.  Specify a system
type of 
.mono CWDLLR 
for register-based parameter passing or 
.mono CWDLLS
for stack-based parameter passing.
.np
A DLL file is a standard EXE file with the following requirement:  The
program start address should be an initialization and termination
function, rather than a main program entry point.  The entry address
will be called twice:  Once after loading to allow the DLL to perform
initialization and once just prior to the DLL being unloaded from memory
to allow it to clean up for termination.  Entry conditions are: register
EAX=0 for initialization and register EAX=1 for termination.  An
initialization code return value of EAX=0 indicates no errors.  A code
of any other value indicates an error has occurred and loading should be
terminated.  If an error condition is returned, it is up to the DLL to
display an error message, CauseWay will simply report a load error.  The
entry address is a FAR call, so the initialization code should use a
RETF to return control to the calling program.
.np
A minimal DLL startup system is provided in the 
.fi DLLSTRTR.OBJ
(register-based) and the 
.fi DLLSTRTS.OBJ
(stack-based) files. 
.np
CauseWay loads DLLs when the program being loaded has references to
external modules in the DLL.  CauseWay searches the execution path for
any DLL or EXE file (in that order) which has the proper internal module
name.  The module name is not used when searching.  For example, a file
named 
.mono USEME.DLL
contains a module named Spelling_Checker.  The name of
the module (Spelling_Checker) is set by the NAME option in your link
file.  If no NAME is specified, then the module's name will default to
its file name without an extension.  In this example, the module name
would become USEME if no NAME is specified.
.np
Following is an overview of the standard link file commands used to
create DLLs. You may also refer to the &product. Linker documentation for a
description of these commands.
.mbigbox
EXPORT function_name
.embigbox
.synote
.mnote function_name
allows you to make a symbol (function name) available to
other modules.  It must be declared as a public symbol so that the
linker can export it.
.esynote
.np
.mbigbox
IMPORT [local_name] module_name.function_name

IMPORT [local_name] module_name.ordinal
.embigbox
.synote
.mnote local_name
is an optional parameter.  It is the symbol which the
importing program references the function by, i.e. the symbol declared
as external.  If no local name is specified, then 
.sy function_name 
is used.
.np
.mnote module_name 
is the name of the module that contains the function.  It
is not the file name.  IMPORT module names are resolved by searching DLL
and EXE files for the correct module name.
.np
.mnote function_name
is the symbol by which the function is known in the
EXPORTing module, i.e. the symbol that is declared as public.
.np
.mnote ordinal_number
functions can also be imported by number.  This is the
entry number in decimal, starting at 1, in the EXPORTing module's export
table to link to.
.sy local_name 
must be specified when using ordinals,
otherwise there is no symbolic reference to internally resolve.
.np
In the DLL calling code, 
.sy local_name
and
.sy function_name 
need to be declared as external.
.esynote
.*
.mbigbox
NAME   module_name
.embigbox
.synote
.mnote module_name
is a symbol by which the module should be identified when
resolving IMPORT records in a calling program.
.np
IMPORTed module names can contain a partial path.  For example,
DLL\spelling_checker would instruct the loader to look in <execution
path>\DLL for a module with a name of spelling_checker.
.np
Notes: 
.np
You must take the responsibility to make sure that the IMPORTed function
or module calling conventions match the calling code.  For example, the
loader will load a 16-bit module to resolve an IMPORT in a 32-bit
program without complaint.  In this case, if the IMPORTing program does
a 16-bit far call, then everything will work correctly, but a 32-bit
call will fail unless the 16-bit DLL module ends with a 32-bit RET
instruction.
.np
Importing by function name may slow program performance if the symbols are
frequently referenced.  In such cases, consider using the ordinal
command to speed up the access times because module name references are
automatically converted to ordinals in an internal list which will only
be processed once at load time.
.esynote
.*
.*
.chap Performance Considerations
.*
.np
There are a few ways to increase the operational efficiency of your
CauseWay applications.
.*
.section Memory Size
.*
.np
Almost without exception, the best way to increase runtime performance
of all CauseWay applications is to ensure that physical memory is large
enough to meet all of the program's needs.  Performance suffers considerably
when CauseWay creates a temporary file for virtual memory, swapping 4KB 
blocks of the program's code and data to and from disk.  Naturally this may 
not be possible in all cases, but it is a worthy goal.  Generally the more
physical memory, even when virtual memory is being used, the better an
application's performance.
.np
When using a disk cache program, be sure not to use too much extended
memory.  Although a disk cache program is beneficial, allocating it too
much memory can deprive CauseWay of required extended memory and can
degrade application performance. However completely disabling disk cache
will usually noticeably decrease performance as well. The optimal cache size 
depends on the particular application and computer system (amount of physical 
memory, disk I/O speed etc.) and there are no generally
applicable "best" settings.
.np
.ix 'virtual memory'
If your program uses virtual memory, CauseWay's VMM creates a temporary
swap file.  If you have more than one disk drive, then you may wish to
direct creation of the swap file to the faster disk drive on your system
using the 
.ev CAUSEWAY=SWAP
,
.ev TEMP
or 
.ev TMP
environment variables.  Do not
create a RAM disk if this will lower your physical memory because this
is less efficient than allowing CauseWay to use physical memory itself.
.np
Remember that virtual memory is part of total memory when using
CauseWay. If your default drive, or the drive pointed to by the 
.ev TEMP
or
.ev TMP
environment variables has little free space, this will be reflected
in total memory available to the CauseWay application.  If disk free
space is less than physical memory, then CauseWay shuts off all use of
virtual memory.  Windows and OS/2 handle virtual memory internally and
supply it through the swap file and DPMI settings for the application.
.*
.section &product. C/C++ kbhit() replacement
.*
.np
.ix 'kbhit()'
Two optimized replacement versions of the &product. C++ runtime library
kbhit() function are provided. The
files are KBHITR.OBJ and KBHITS.OBJ for, respectively, register-based
and stack-based calling conventions.  Simply link in the kbhit()
replacement file appropriate for your compile options.  These
replacement routines bypass the normal INT checking of the keyboard and
directly inspect the keyboard buffer to see if a keypress is pending.
These routines significantly reduce the overhead in tight processing
loops, which perform many kbhit()'s per second, by avoiding the
interrupt call associated with checking for keystrokes.
.np
Be aware that linking in the kbhitr or kbhits module means that the INT
28h idle call will not be made on kbhit() as normally occurs with the
standard runtime library kbhit().  This may impact background processing
in applications which depend on INT 28h idle calls, such as the DOS
PRINT utility which performs printing in the background, as well as 
operation under multitatsking environments.
.*
.section DOS API Buffer Size
.*
.np
If your CauseWay application reads and writes files using large amounts
of data on one read or write pass, you may wish to consider increasing
the size of the internal DOS memory transfer buffer used by CauseWay.
Refer to the SetDOSTrans and GetDOSTrans functions in the CauseWay API
chapter for more information.
.np
Note that the internal 8KB buffer is optimized for file transfers.
Average file transfers of greater than 8KB will not necessarily improve
performance with an increase in the buffer size.  Generally speaking,
the average file transfer must be 32KB or larger to gain any efficiency
with an increased buffer size. Also, if you are using virtual memory,
increasing the buffer size may actually slow down performance due to the
decreased available physical memory.  Test your application with both
the default buffer and the desired new buffer size before permanently
increasing the buffer size beyond the default.
.*
.section API Memory Allocation
.*
.np
Inveterate tweakers may try out the SetMCBMax and GetMCBSize functions
in the CauseWay API.  These functions allow fine-tuning of the threshold
used by CauseWay to allocate memory via a memory pool using memory
control blocks (MCBs) rather than via normal DPMI functions.  Since DPMI
allocates memory in multiples of 4KB, setting the MCB threshold too low
may result in a good deal of wasted memory and subsequent performance
degradation.
.*
.*
.chap Rules For Protected Mode Operation
.*
.np
The following information covers additional restrictions for protected
mode compatible code that are not present when writing real mode
compatible code.
.np
Using protected mode rather than real mode requires following a few new
programming rules to prevent processor faults from being generated,
terminating the CauseWay application.  These processor faults occur when
an application breaks a protected mode programming rule.
.np
Use the following rules for programming in protected mode:
.autonote
.note
A selector value (referred to as a segment value in real mode)
loaded into a segment register references an area of memory that may
occur anywhere within the machine's physical address space. The
operating system can dynamically move this area of memory.  Therefore,
when dealing with selector values:
.begbull
.bull
Never use segment registers as general purpose registers
that can be loaded with arbitrary values.  Every time a segment register
is loaded with a value the processor checks the validity of the selector
value and generates a fault if it is invalid.
.bull
Never perform segment arithmetic on a selector value.
Segment arithmetic is usually performed in real mode code to either
normalize a pointer, access a new paragraph of memory without changing
an offset, or to access a single area of memory that is greater than 64KB
in size. Since a selector value is an index into a table which contains
the actual memory addresses, addition or subtraction of different
selector values is meaningless and gives no useful results.  (There
exist special cases where contiguous selector values can be added or
subtracted from for useful effect, but detailing these cases exceed the
scope of this manual.)
.bull
Do not access data at an offset greater than the size of
the associated selector.  Attempts to do so result in a fault.  This is
one of the greatest strengths of protected mode because most obscure
bugs in real mode code occur when a bad pointer value accesses the wrong
area of memory, or when a buffer overflows and memory beyond the buffer
is overwritten.
.bull
Do not attempt to write to or read from the NULL (zero
value) segment. Attempts to do so results in a fault. In addition to
valid selector values, segment registers can be safely loaded with a
value of 0 but this selector value cannot be used to access memory or
execute code.
.endbull
.note
Do not execute code in a data segment and do not write to data in
the code segment.  Use the CauseWay AliasSel function to map a data
selector to the same physical memory area shared by a code selector when
necessary. Even in this case, however, never write to memory using a CS:
code segment override because it always causes a processor fault.
.note
CauseWay handles most of the standard DOS interrupts
transparently. When passing pointers to buffers for software interrupt
calls not handled by CauseWay, create the buffers in low (conventional)
DOS memory using the GetMemDOS function of the CauseWay API.  In
addition, convert the pointers from protected mode selector:offset pairs
to real mode segment:offset pairs prior to the interrupt, and back upon
your return from the interrupt.
.endnote
.*
.*
.chap CW.ERR File Information Format
.*
.np
.ix 'CW.ERR'
The following information describes the format of the CW.ERR file that
CauseWay creates if an exception occurs in a CauseWay application. This
information can be very useful in tracking down exactly where and why an
exception occurred.
.*
.section Quick Reference Guide
.*
.np
If you do not know how to interpret assembly language or CPU
instructions, and you want better detail on the location of an
exception, you can frequently identify which routine caused the
exception by cross-referencing the MAP file with the CW.ERR file.
Following is a short guide to determine the offending routine.
.np
Look at the value after the dash (-) listed for the CS segment register
at the beginning of the ninth line in CW.ERR.  This should be an eight
digit number starting with several zeros.  Now look at the MAP file of
your application. Following the program, creation date and time lines in
the MAP file is a listing of program segments showing their start, stop,
length, name, class and count.  Find the segment which has a start
address equal to the eight digit number listed above.  This is the entry
for the program segment where the exception occurred.
.np
Locate the public symbols listed by address in the MAP file.  Each
symbol in the program is listed in ascending address order.  The address
is composed of two values separated by a colon (:).  Find the address
group which begins with the eight digit number given above for CS
segment register without the last digit.  For example, if the CS eight
digit number was 000205E0, look for an address beginning with 0000205E.
If you cannot find any addresses beginning with the number, either no
routines in the segment were declared public or else you have a version
of the MAP file that was created at a different time than the
application EXE file which generated the CW.ERR file.
.np
If there is only one address beginning with the number, you have located
the offending routine.  If there is more than one address, examine the
EIP value in CW.ERR.  The EIP value is located in the middle of the
seventh line immediately following the "EIP=" text.  This value is the
offset in the segment where the exception occurred.  Find the symbol in
the address group which has an address value following the ":" that is
closest to the EIP but does not exceed the EIP value. This name of the
symbol is the name of the routine which generated the exception.  To
continue our example, if you have the following four symbols of address
0000205E in the first half of the address line in the map file :
.millust begin
0000205E:000008DC       __DBFGOHOT

0000205E:00000944       __DBFGOCOLD

0000205E:00000986       __DBFGOTOID

0000205E:00000AEE       __DBFGOTOP
.millust end
and the EIP value is 00000953, the closest routine name that does not
exceed the EIP value in the second half of the address line is
.mono __DBFGOCOLD.
Therefore, the exception occurred in the 
.mono __DBFGOCOLD
routine.
.np
This method of locating the exception is not foolproof since it requires
that the routine creating the exception in the program segment be
declared public, but it should work for the majority of cases.
.*
.section Sequence
.*
.np
The first line in CW.ERR is the CauseWay copyright message including the
version of CauseWay used in the program.  The version number may prove
useful in tracking down problems that have been addressed in later
versions of CauseWay.
.np
The version is followed by the exception number and error code. These
numbers, as well as all others in the CW.ERR file, are in hexadecimal.
The values listed are those reported by the processor when the exception
occurred.  Detailed information about the significance of the values can
be obtained in most 386 and above reference books.  Typically the
exception value will be 0DH, a General Protection Fault; 0CH, a stack
fault usually due to stack overflow or underflow; or 0EH, a page fault
due to improper memory reference.  The error code is generally of little
use for debugging purposes.
.np
Next comes the general register values which indicate the state of the
program when the exception occurred. The significance of register values
is entirely dependent on the program being run at the time.  CS:EIP
register values can help track down the problem area by pinpointing
exactly where in the code the exception occurred.  Other register values
may help determine why the exception occurred.  In particular, look for
use of registers as memory indices with values beyond the limit of the
associated selector.
.np
Next, the segment register values are displayed as a real selector value
followed by the program relative value in bytes. If the second value is
non-numeric (xxxxxxxx) then the segment register didn't contain a
selector value allocated to the program at load time, although the value
may be valid if it was dynamically allocated by the operating program.
If there is a second value, it also appears in the program's .MAP
file as the segment start address.  This shows which segment a segment
register is pointing to at the time of the exception. The CS (Code
Segment) register points to the segment containing the code which is
executing.  The EIP register value indicates the offset within the CS
segment where the exception occurred.  With these two values, you can
not only determine the segment, but the routine within the segment
closest to where the exception occurred.
.np
Segment register values are also useful in determining why an exception
occurred.  One common error is using an invalid selector value in DS,
ES, FS, or GS.  A segment register value of zero does not automatically
indicate problem, but will cause a GPF if used to read or write to
memory.  In particular, be highly suspicious of DS and ES segment
register values of 0000-xxxxxxxx since they are almost constantly used to
to access memory.  A zero value in DS or ES usually indicates
a bad (NULL) memory pointer passed to a routine.
.np
Next, the processor control register values are listed.  These registers
are unlikely to be of much use for debugging and will only be filled in
when not running under a true DPMI host. For an exception 0Eh (page
fault), CR2 is the linear address that was accessed for which no memory
was mapped in.  This may help track down the problem area.
.np
Info Flags comes next. This value is returned by CauseWay's Info API
function.  Check it against the documentation for Info in the CauseWay
API chapter to determine some aspects of the environment in which the
program was running when the exception occurred, e.g.  whether a DPMI
host was being used.
.np
Program Linear Load Address follows Info Flags.  This value is th
executable's load address in linear memory.  It corresponds to one of
the linear memory block entries described later.
.np
In flat models, the EIP value minus the program linear load address is
the address offset of the faulting location relative to the start of the
program.
.np
Following Program Linear Load Address is a display of the next 128 byte
values at the CS:EIP location when the exception occurred.  These are
the hexadecimal byte values of the CPU instructions at the time of the
exception.  386 reference books or some debuggers can be used to
reconstruct the instruction operation codes that correspond to these
hexadecimal byte values.
.np
The SS:ESP displays follows CS:EIP.  This display shows the last 128
bytes values stored on the CPU stack.
.np
SS:EBP is next.  It shows 128 byte values of the current stack frame
negatively and positively offset from the current EBP value. C and other
high level language routines use the EBP register to reference
parameters passed on the stack and this display can show which
parameters were passed.
.np
The resource tracking details come next. Selectors are listed with the
following headings:
.*
.millust begin
sel  base  limit  type  D  mem  count
.millust end
.*
.synote
.mnote sel
Selector value.
.mnote base
Linear base address of selector.
.mnote limit
Limit of selector.
.mnote type
CODE or DATA.
.mnote D
16 or 32 to signify segment D bit.
.mnote mem
Y or N to indicate if the selector has a memory
block associated with it.
.mnote count
segment count in MAP file, xxxx if dynamically allocated.
.esynote
.*
The selector list is finished off with a display of the total number of
selectors allocated to the program.  For example:
.millust begin
Total selectors: 0107
.millust end
Linear memory blocks are listed following the selector list, and contain
the following headings:
.*
.millust begin
handle  base  length
.millust end
.*
.synote
.mnote handle
Linear memory [de]allocation uses handles to control
the blocks.  This field is the block's handle.
.mnote base
Linear base address of the memory block.
.mnote length
Length of the block in bytes.
.esynote
The linear memory list ends with a display of the total linear memory
allocated to the program, the real (rounded to 4KB pages) memory
allocated in parentheses, and finally the total number of memory
blocks.  For example:
.millust begin
Total Linear memory: 000FEAC9 (001AA000) in 00000103 blocks
.millust end
Entries in the selector list that have Y under "mem" should have a
corresponding entry in the linear memory list.
.np
Linear memory locks are listed after the linear memory block list and
contain the following headings:
.*
.millust begin
base  length
.millust end
.*
.synote
.mnote base
Linear base of the locked region.
.mnote limit
Length of the locked region.
.esynote
.*
Note: These values are passed by the program but the actual
values have the base rounded down a page and the length rounded up a
page to match 4KB boundary restrictions on locking.  The values are
reported in CW.ERR without using a rounded format to make it easier to
cross reference this list to the other lists in CW.ERR.
.np
Next, protected mode interrupt vectors are listed with the following
headings:
.*
.millust begin
No  sel  offset
.millust end
.*
.synote
.mnote No
Vector number.
.mnote sel
Selector value for handler.
.mnote offset
Offset value for handler.
.esynote
.*
This information allows a cross-reference with the other lists to ensure
CauseWay application installed handlers have been properly made.
.np
Next, protected mode exception vectors are listed using the same format
as protected mode interrupts.
.np
After the protected mode exception vector list, real mode interrupt
vectors are listed.  They are shown in the same format as protected mode
interrupts although the selector values are real mode segment values.
.np
Only those interrupt and exception vectors altered by the program will
be listed.
.np
Lastly, Callbacks are listed in the CW.ERR file.  They list all active
Callbacks for the active application at the time of its termination.
