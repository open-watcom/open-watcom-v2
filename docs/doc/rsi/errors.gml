.* (c) Portions Copyright 1990, 1995 by Tenberry Software, Inc.
.*
.chap Error Messages
.*
.np
.ix 'messages' '&dos4gprd'
.ix 'extender messages' '&dos4gprd'
.ix '&dos4gprd' 'extender messages'
:cmt. .begnote $break
:cmt. .note &dos4gnam Fatal error: Syntax -- &dos4gnam <executable.xxx>
:cmt. This message occurs when you execute the &dos4gnam program without
:cmt. giving the name of an executable program on the command line.
:cmt. .note &dos4gnam Fatal error: Can't find file to load -- <filename>
:cmt. This message indicates that &dos4gnam couldn't find the executable
:cmt. file you asked it to execute.
:cmt. This should not happen if the program was spliced with a stub loader.
:cmt. .note &dos4gnam Fatal error: Can't find loader for file[#code#]  -- <filename>
:cmt. This message indicates that &dos4gnam was unable to find a loader that
:cmt. can load an executable of this format.
:cmt. .note &dos4gnam Fatal error: Can't initialize loader[#code#]  -- <loader>
:cmt. This error should not occur -- please call technical support if it
:cmt. does.
:cmt. .note &dos4gnam Fatal error: Loader failed -- <loader>
:cmt. This error should not occur, and may indicate that the executable file
:cmt. has been corrupted in some way.
:cmt. .note &dos4gnam Fatal error: INT31 initialization error
:cmt. This error should not occur -- please call technical support if it
:cmt. does.
:cmt. .note &dos4gnam Fatal error: No INT31 action found
:cmt. This error should not occur -- please call technical support if it
:cmt. does.
:cmt. .note &dos4gnam Fatal error: Can't locate DOS extender
:cmt. This error should not occur -- please call technical support if it
:cmt. does.
:cmt. You may get this error if you change the name of the "&dos4gexe" file
:cmt. to something else.
:cmt. .note &dos4gnam Fatal error: VMM initialization error[#code#]
:cmt. This error should not occur -- please call technical support if it
:cmt. does.
:cmt. .endnote
:cmt. .np
:cmt. The
:cmt. .us #code#
:cmt. mentioned in these error messages will be a decimal or hexadecimal
:cmt. number -- please be prepared to give it to technical support.
.np
:cmt. This excerpt from the DOS/4G manual currently being written lists
:cmt. DOS/4G error messages, with descriptions of the circumstances in which
:cmt. the error is most likely to occur, and suggestions for remedying the
:cmt. problem.
:cmt. .np
:cmt. With a few exceptions, this information applies to DOS/4GW Professional
:cmt. as well.  (Naturally, error messages that pertain to DOS extender
:cmt. features -- like DLLs -- that are not supported in DOS/4GW
:cmt. Professional will not arise with that product.)
The following lists DOS/4G error messages, with descriptions of the
circumstances in which the error is most likely to occur, and
suggestions for remedying the problem.
Some error messages pertaining to features &mdash like DLLs &mdash that
are not supported in &dos4g will not arise with that product.
In the following descriptions, references to DOS/4G, DOS4G, or
DOS4G.EXE may be replaced by &dos4gprd, &dos4gnam, or &dos4gexe should
the error message arise when using &dos4g..
.*
.section Kernel Error Messages
.*
.np
.ix '&dos4gprd' 'kernel error messages'
.ix 'kernel error messages'
.ix 'error messages' 'kernel'
This section describes error messages from the DOS/16M kernel
embedded in DOS/4G.
Kernel error messages may occur because of severe resource shortages,
corruption of DOS4GW.EXE, corruption of memory, operating system
incompatibilities, or internal errors in DOS/4GW.
All of these messages are quite rare.
.begnote $setptnt 2
.*
.note 0. involuntary switch to real mode
.np
The computer was in protected mode but switched to real mode without
going through DOS/16M.  This error most often occurs because of an
unrecoverable stack segment exception (stack overflow), but can also
occur if the Global Descriptor Table or Interrupt Descriptor Table is
corrupted.  Increase the stack size, recompile your program with
stack overflow checking, or look into ways that the descriptor tables
may have been overwritten.
.*
.note 1. not enough extended memory
.*
.note 2. not a DOS/16M executable <filename>
.np
DOS4G.EXE, or a bound DOS/4G application, has probably been corrupted
in some way.  Rebuild or recopy the file.
.*
.note 3. no DOS memory for transparent segment
.*
.note 4. cannot make transparent segment
.*
.note 5. too many transparent segments
.*
.note 6. not enough memory to load program
.np
There is not enough memory to load DOS/4G.  Make more memory
available and try again.
.*
.note 7. no relocation segment
.*
.note 8. cannot open file <filename>
.np
The DOS/16M loader cannot load DOS/4G, probably because DOS has run
out of file units.  Set a larger FILES= entry in CONFIG.SYS, reboot,
and try again.
.*
.note 9. cannot allocate tstack
.np
There is not enough memory to load DOS/4G.  Make more memory
available and try again.
.*
.note 10. cannot allocate memory for GDT
.np
There is not enough memory to load DOS/4G.  Make more memory
available and try again.
.*
.note 11. no passup stack selectors -- GDT too small
.np
This error indicates an internal error in DOS/4G or an
incompatibility with other software.
.*
.note 12. no control program selectors -- GDT too small
.np
This error indicates an internal error in DOS/4G or an
incompatibility with other software.
.*
.note 13. cannot allocate transfer buffer
.np
There is not enough memory to load DOS/4G.  Make more memory
available and try again.
.*
.note 14. premature EOF
.np
DOS4G.EXE, or a bound DOS/4G application, has probably been corrupted
in some way.  Rebuild or recopy the file.
.*
.note 15. protected mode available only with 386 or 486
.np
DOS/4G requires an 80386 (or later) CPU.  It cannot run on an 80286
or earlier CPU.
.*
.note 16. cannot run under OS/2
.*
.note 17. system software does not follow VCPI or DPMI specifications
.np
Some memory resident program has put your 386 or 486 CPU into Virtual
8086 mode.  This is done to provide special memory services to DOS
programs, such as EMS simulation (EMS interface without EMS hardware)
or high memory.  In this mode, it is not possible to switch into
protected mode unless the resident software follows a standard that
DOS/16M supports (DPMI, VCPI, and XMS are the most common).  Contact
the vendor of your memory management software.
.*
.note 18. you must specify an extended memory range (SET DOS16M= )
.np
On some Japanese machines that are not IBM AT-compatible, and have no
protocol for managing extended memory, you must set the DOS16M
environment variable to specify the range of available extended
memory.
.*
.note 19. computer must be AT- or PS/2- compatible
.*
.note 20. unsupported DOS16M switchmode choice
.*
.note 21. requires DOS 3.0 or later
.*
.note 22. cannot free memory
.np
This error probably indicates that memory was corrupted during
execution of your program.
.*
.note 23. no memory for VCPI page table
.np
There is not enough memory to load DOS/4G.  Make more memory
available and try again.
.*
.note 24. VCPI page table address incorrect
.np
This is an internal error.
.*
.note 25. cannot initialize VCPI
.np
This error indicates an incompatibility with other software.  DOS/16M
has detected that VCPI is present, but VCPI returns an error when
DOS/16M tries to initialize the interface.
.*
.note 26. 8042 timeout
.*
.note 27. extended memory is configured but it cannot be allocated
.*
.note 28. memory error, avail loop
.np
This error probably indicates that memory was corrupted during
execution of your program.  Using an invalid or stale alias selector
may cause this error.  Incorrect manipulation of segment descriptors
may also cause it.
.*
.note 29. memory error, out of range
.np
This error probably indicates that memory was corrupted during
execution of your program.  Writing through an invalid or stale alias
selector may cause this error.
.*
.note 30. program must be built -AUTO for DPMI
.*
.note 31. protected mode already in use in this DPMI virtual machine
.*
.note 32. DPMI host error (possibly insufficient memory)
.*
.note 33. DPMI host error (need 64K XMS)
.*
.note 34. DPMI host error (cannot lock stack)
.np
Any of these errors (32, 33, 34) probably indicate insufficient memory
under DPMI.  Under Windows, you might try making more physical memory
available by eliminating or reducing any RAM drives or disk caches.
You might also try editing DEFAULT.PIF so that at least 64KB of XMS
memory is available to non-Windows programs.  Under OS/2, you want to
increase the DPMI_MEMORY_LIMIT in the DOS box settings.
.*
.note 35. General Protection Fault
.np
This message probably indicates an internal error in DOS/4G.  Faults
generated by your program should cause error 2001 instead.
.*
.note 36. The DOS16M.386 virtual device driver was never loaded
.*
.note 37. Unable to reserve selectors for DOS16M.386 Windows driver
.*
.note 38. Cannot use extended memory: HIMEM.SYS not version 2
.np
This error indicates an incompatibility with an old version of
HIMEM.SYS.
.*
.note 39. An obsolete version of DOS16M.386 was loaded
.*
.note 40. not enough available extended memory (XMIN)
.np
This message probably indicates an incompatibility with your memory
manager or its configuration.  Try configuring the memory manager to
provide more extended memory, or change memory managers.
.endnote
.*
.section DOS/4G Errors
.*
.ix '&dos4gprd' 'error messages'
.ix 'error messages' '&dos4gprd'
.begnote $setptnt 4
.*
.note 1000 "can't hook interrupts"
.np
A DPMI host has prevented DOS/4G from loading.  Please contact
Tenberry Technical Support.
.*
.note 1001 "error in interrupt chain"
.np
DOS/4G internal error. Please contact Tenberry Technical Support.
.*
.note 1003 "can't lock extender kernel in memory"
.np
DOS/4G couldn't lock the kernel in physical memory, probably because
of a memory shortage.
.*
.note 1004 "syntax is DOS4G <executable.xxx>"
.np
You must specify a program name.
.*
.note 1005 "not enough memory for dispatcher data"
.np
There is not enough memory for DOS/4G to manage user-installed
interrupt handlers properly.  Free some memory for the DOS/4G
application.
.*
.note 1007 "can't find file <program> to load"
.np
DOS/4G could not open the specified program.  Probably the file
didn't exist.  It is possible that DOS ran out of file handles, or
that a network or similar utility has prohibited read access to the
program.  Make sure that the file name was spelled correctly.
.*
.note 1008 "can't load executable format for file <filename> [<error code>]"
.np
DOS/4G did not recognize the specified file as a valid executable
file.  DOS/4G can load linear executables (LE and LX) and EXPs (BW).
The error code is for Tenberry Software's use.
.*
.note 1009 "program <filename> is not bound"
.np
This message does not occur in DOS/4G, only DOS/4GW Professional; the
latter requires that the DOS extender be bound to the program file.
The error signals an attempt to load
.*
.note 1010 "can't initialize loader <loader> [<error code>]"
.np
DOS/4G could not initialize the named loader, probably because of a
resource shortage.  Try making more memory available.  If that doesn't
work, please contact Tenberry Technical Support.  The error code is for
Tenberry Software' use.
.*
.note 1011 "VMM initialization error [<error code>]"
.np
DOS/4G could not initialize the Virtual Memory Manager, probably
because of a resource shortage.  Try making more memory available. If
that doesn't work, please contact Tenberry Technical Support.  The error code
is for Tenberry Software' use.
.*
.note 1012 "<filename> is not a WATCOM program"
.np
This message does not occur in DOS/4G, only DOS/4GW and DOS/4GW
Professional.  Those extenders only support WATCOM 32-bit compilers.
.*
.note 1013 "int 31h initialization error"
.np
DOS/4G was unable to initialize the code that handles Interrupt 31h,
probably because of an internal error.  Please call Tenberry Technical
Support.
.*
.note 1100 "assertion \"<statement>\" failed (<file>:<line>)"
.np
DOS/4G internal error.  Please contact Tenberry Technical Support.
.*
.note 1200 "invalid EXP executable format"
.np
DOS/4G tried to load an EXP, but couldn't.  The executable file is
probably corrupted.
.*
.note 1201 "program must be built -AUTO for DPMI"
.np
Under DPMI, DOS/4G can only load EXPs that have been linked with the
GLU -AUTO or -DPMI switch.
.*
.note 1202 "can't allocate memory for GDT"
.np
There is not enough memory available for DOS/4G to build a Global
Descriptor Table.  Make more memory available.
.*
.note 1203 "premature EOF"
.np
DOS/4G tried to load an EXP but couldn't.  The file is probably
corrupted.
.*
.note 1204 "not enough memory to load program"
.np
There is not enough memory available for DOS/4G to load your program.
Make more memory available.
.*
.note 1301 "invalid linear executable format"
.np
DOS/4G cannot recognize the program file as a LINEXE format.  Make
sure that you specified the correct file name.
.*
.note 1304 "file I/O seek error"
.np
DOS/4G was unable to seek to a file location that should exist.  This
usually indicates truncated program files or problems with the
storage device from which your program loads.  Run CHKDSK or a
similar utility to begin determining possible causes.
.*
.note 1305 "file I/O read error"
.np
DOS/4G was unable to read a file location that should contain program
data.  This usually indicates truncated program files or problems
with the storage device from which your program loads.  Run CHKDSK or
a similar utility to begin determining possible causes.
.*
.note 1307 "not enough memory"
.np
As it attempted to load your program, DOS/4G ran out of memory.  Make
more memory available, or enable VMM.
.*
.note 1308 "can't load requested program"
.note 1309 "can't load requested program"
.note 1311 "can't load requested program"
.note 1312 "can't load requested program"
.np
DOS/4G cannot load your program for some reason.  Contact Tenberry Technical
Support.
.*
.note 1313 "can't resolve external references"
.np
DOS/4G was unable to resolve all references to DLLs for the requested
program, or the program contained unsupported fixup types.  Use EXEHDR
or a similar LINEXE dump utility to see what references your program
makes and what special fixup records might be present.
.*
.note 1314 "not enough lockable memory"
.np
As it attempted to load your program, DOS/4G encountered a refusal to
lock a virtual memory region.  Some memory must be locked in order to
handle demand-load page faults.  Make more physical memory available.
.*
.note 1315 "can't load requested program"
.note 1316 "can't load requested program"
.np
DOS/4G cannot load your program for some reason.  Contact Tenberry Technical
Support.
.*
.note 1317 "program has no stack"
.np
DOS/4G reports this error when you try to run a program with no
stack.  Rebuild your program, building in a stack.
.*
.note 2000 "deinitializing twice"
.np
DOS/4G internal error.  Please contact Tenberry Technical Support.
.*
.note 2001 "exception <exception_number> (<exception_description>) at
<selector:offset>"
.np
Your program has generated an exception.  For information about
interpreting this message, see the file COMMON.DOC.
.*
.note 2002 "transfer stack overflow at <selector:offset>"
.np
Your program has overflowed the DOS/4G transfer stack.  For
information about interpreting this message, see the file COMMON.DOC.
.*
.note 2300 " can't find <DLL>.<ordinal> - referenced from <module>"
.np
DOS/4G could not find the ordinal listed in the specified DLL, or it
could not find the DLL at all.  Correct or remove the reference, and
make sure that DOS/4G can find the DLL.
.np
DOS/4G looks for DLLs in the following directories:
.begbull
.bull
The directory specified by the Libpath32 configuration option
(which defaults to the directory of the main application file).
.bull
The directory or directories specified by the LIBPATH32
environment variable.
.bull
Directories specified in the PATH.
.endbull
.note 2301 "can't find <DLL>.<name> - referenced from <module>"
.np
DOS/4G could not find the entry point named in the specified module.
Correct or remove the reference, and make sure that DOS/4G can find
the DLL.
.note 2302 "DLL modules not supported"
.np
This DOS/4GW Professional error message arises when an application
references or tries to explicitly load a DLL.  DOS/4GW Professional
does not support DLLs.
.*
.note 2303 "internal LINEXE object limit reached"
.np
DOS/4G currently handles a maximum of 128 LINEXE objects, including
all .DLL and .EXE files.  Most .EXE or .DLL files use only three or
four objects.  If possible, reduce the number of objects, or contact
Tenberry Technical Support.
.*
.note 2500 "can't connect to extender kernel"
.np
DOS/4G internal error.  Please contact Tenberry Technical Support.
.*
.note 2503 "not enough disk space for swapping - <count> byes required"
.np
VMM was unable to create a swap file of the required size.  Increase
the amount of disk space available.
.*
.note 2504 "can't create swap file \<filename>\""
.np
VMM was unable to create the swap file.  This could be because the
swap file is specified for a nonexistent drive or on a drive that is
read-only.  Set the SWAPNAME parameter to change the location of
the swap file.
.*
.note 2505 "not enough memory for <table>"
.np
VMM was unable to get sufficient extended memory for internal tables.
Make more memory available.  If <table> is page buffer, make more DOS
memory available.
.*
.note 2506 "not enough physical memory (minmem)"
.np
There is less physical memory available than the amount specified by
the MINMEM parameter.  Make more memory available.
.*
.note 2511 "swap out error [<error code>]"
.np
Unknown disk error.  The error code is for Tenberry Software' use.
.*
.note 2512 "swap in error [<error code>]"
.np
Unknown disk error.  The error code is for Tenberry Software' use.
.*
.note 2514 "can't open trace file"
.np
VMM could not open the VMM.TRC file in the current directory for
writing.  If the directory already has a VMM.TRC file, delete it.  If
not, there may not be enough memory on the drive for the trace file,
or DOS may not have any more file handles.
.*
.note 2520 "can't hook int 31h"
.np
DOS/4G internal error.  Please contact Tenberry Technical Support.
.*
.note 2523 "page fault on non-present mapped page"
.np
Your program references memory that has been mapped to a nonexistent
physical device, using DPMI function 508h.  Make sure the device is
present, or remove the reference.
.*
.note 2524 "page fault on uncommitted page"
.np
Your program references memory reserved with a call to DPMI function
.note 504h, but never committed (using a DPMI 507h or 508h call).  Commit
the memory before you reference it.
.*
.note 3301 "unhandled EMPTYFWD, GATE16, or unknown relocation"
.note 3302 "unhandled ALIAS16 reference to unaliased object"
.note 3304 "unhandled or unknown relocation"
.np
If your program was built for another platform that supports the
LINEXE format, it may contain a construct that DOS/4G does not
currently support, such as a call gate.  This message may also occur
if your program has a problem mixing 16- and 32-bit code.  A linker
error is another likely cause.
.endnote
