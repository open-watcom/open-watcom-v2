Appendix C
*Glossary Of Terms*



This glossary defines technical terms used in this manual. Common
programming terms are not listed.  These terms are defined for
completeness, and knowledge of their definitions is not necessary to
develop CauseWay applications.




*Aliasing*

Allocating a selector that has the same base and limit as
another selector but not usually the same type.  Aliasing usually refers
to creating a data selector that addresses the same memory as a code
selector. Aliasing allows write access to variables within a code
segment as long as the cs: instruction override is not used.  While
aliasing isn't a particularly good term for this operation because the
new selector doesn't track changes to the original selector, it is the
term most commonly used by developers.




        API

Application Program Interface is the means by which an
application communicates with the underlying operating system or
environment.  With CauseWay, the API is handled through INT 31h calls
with the appropriate register setup.



*Callback*

A mechanism which allows real mode code to switch the CPU to
protected mode operation and continue execution.  Callbacks are used by
CauseWay to re-signal real mode hardware interrupts in protected mode.




        CauseWay Application

Any program linked with the CauseWay linker when the /ex option
is not used.




        Descriptor

An eight-byte block of memory that describes the base address,
limit, and type of a segment.  In protected mode operation, each segment
always has one associated descriptor.



*DPMI*

DOS Protected Mode Interface is an API that defines a way for
multiple protected mode applications to coexist peacefully in the same
system.  It is mainly used with DOS extenders and not enduser
applications.  DPMI is a more general API than the older Virtual Control
Program Interface (VCPI) that it is replacing.



*Exception*

A software interrupt associated with an error condition
detected by the CPU.  Exceptions can either be an abort, a fault, or a
trap.  These three types are simply indications of the severity and
recoverability of the error.  Frequently, the term exception is used
interchangeably with fault without taking notice of any distinction.




        Fault

When the processor detects an error during the processing of a
CPU instruction, a fault occurs.  Some faults are recoverable and are
handled by the DOS extender.  Other faults however, are not recoverable
and the DOS extender terminates the application that generated the
fault.  The most common fault is a General Protection Fault or GPF.
Interrupt 13 is the interrupt associated with a GPF.  GPFs are a
catchall fault which are not covered by any other fault type.  They are
typically caused by writing to or reading from memory beyond the limit
of a segment or loading a segment with an invalid selector value.



*GDT*

Global Descriptor Table.  80386 and above chips provide three
types of descriptor tables when in protected mode. The GDT is the
primary table intended for segments used by the operating system.  It is
the only descriptor table that can contain call and interrupt gates, and
other system type selectors. The GDT is not affected by task switch
instructions, that is, it is available to all processes in an multi-
tasking environment.  There is one and only one GDT in protected mode.




        GPF

General Protection Fault.  See Fault.



*INT 15h*

Miscellaneous BIOS functions API.  One of these functions is
reporting the amount of physical memory present in a machine.



*INT 31h*

The DPMI API interrupt. DPMI and CauseWay services are provided
via this interrupt.




*LDT*

Local Descriptor Table.  The LDT holds segment descriptors much
as the Global Descriptor Table (GDT), except that LDT descriptors are
available only to tasks using that particular LDT.  The LDT is loaded
from a task's state segment when task switch instructions are executed
so each task can have its own LDT in a multi-tasking environment. There
can be more than one or no LDTs at all present in protected mode.


*LE*


*Normalizing* (a pointer)

In real mode, 4096 segment values with differing offsets can be
used to address the same memory address, e.g. 5000:213h and 5020:13h.
Converting to the largest possible segment value and the smallest offset
value (in the range 0 to 0fh) is called normalizing, e.g. 5021:3h.  When
normalizing a pointer, use the largest possible segment value and the
smallest possible offset value.  Protected      mode does not support
normalizing pointers because it performs arithmetic on selector values.
Libraries and routines which normalize pointers will need to be modified
to work with CauseWay by removing the normalization code.



*Physical Memory*

Memory physically installed in the computer.  A combination of
extended and conventional memory.




*Protected Mode*

The native operating mode for 80386 and above chips which
contains system resource protection features. Protected mode also
provides enhanced features for memory addressing and manipulation.
Protected mode will not execute some real mode code without modifications.



*Real Mode*

The operating mode for 8088, 8086, and 80186 CPUs. 80386 chips
and above initially run in this mode when they start
executing instructions and behave exactly like an 8086 with the
exception of additional registers being available to the programmer but
most of the 8086 restrictions, such as access to only 1MB of memory.
Real mode is the mode DOS normally uses.




        Selector

When operating in protected mode, segment values do not
directly address memory, but rather are an index into a descriptor
table.  These indices are called selectors because they select the
proper entry in the descriptor table.  A valid entry must be present in
the descriptor table or else loading the segment register with the
selector value causes a General Protection Fault, GPF.



*VCPI*

Virtual Control Program Interface is an API that defines a way
for multiple protected mode applications to coexist within the same
system.  It is mainly used by DOS extenders and not enduser applications.

*VMM*

Virtual Memory Manager.  CauseWay's virtual memory manager
consists of the routines which handle swapping memory containing code
and data, to and from the disk, from and to physical memory.



*XMS*

XMS stands for extended memory specification, a standard
software interface for extended memory access.
