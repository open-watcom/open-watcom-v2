.* (c) Portions Copyright 1990, 1995 by Tenberry Software, Inc.
.*
.chap *refid=rsiutls Utilities
.*
.np
.ix '&dos4gprd' 'utilities'
This chapter describes the Tenberry Software &dos4g utility programs
provided with the &cmpname package.
Each program is described using the following format:
.begnote $setptnt 6
.note Purpose:
This is a brief statement of what the utility program does.
More specific information is provided under "Notes".
.note Syntax:
This shows the syntax of the program.
The fixed portion of each command is in a
.mono typewriter font,
while variable parts of the command are in
.us italics.
Optional parts are enclosed in [brackets].
.note Notes:
These are explanatory remarks noting major features and possible
pitfalls.
We explain anything special that you might need to know about the
program.
.note See Also:
This is a cross-reference to any information that is related to the
program.
.note Example:
You'll find one or more sample uses of the utility program with an
explanation of what the program is doing.
.endnote
.np
Some of the utilities are &dos4g-based, protected-mode programs.
To determine which programs run in protected mode and which in real,
run the program.
If you see the &dos4g banner, the program runs in protected mode.
.*
.npsection &dos4gnam
.*
.ix '&dos4gnam'
.begnote $setptnt 6
.note Purpose:
Loads and executes linear executables.
.note Syntax:
.us linear_executable
.note Notes:
.ix 'stub program'
The stub program at the beginning of the linear executable invokes
this program, which loads the linear executable and starts up the DOS
extender.
The stub program must be able to find &dos4gnam: make sure it is in the
path.
.endnote
.*
.npsection PMINFO
.*
.ix 'PMINFO'
.begnote $setptnt 6
.note Purpose:
.ix 'mode switching' 'performance'
.ix 'switching modes' 'performance'
Measures the performance of protected/real-mode switching and extended
memory.
.note Syntax:
.mono PMINFO.EXE
.note Notes:
We encourage you to distribute this program to your users.
.np
The time-based measurements made by PMINFO may vary slightly from run
to run.
.note Example:
The following example shows the output of the PMINFO program on a
386 AT-compatible machine.
.cbox begin
.in -2
C>pminfo
     Protected Mode and Extended Memory Performance Measurement -- 5.00
             Copyright (c) Tenberry Software, Inc. 1987 - 1993

DOS memory   Extended memory   CPU performance equivalent to 67.0 MHz 80486
----------   ---------------
       736              8012   K bytes configured (according to BIOS).
       640             15360   K bytes physically present (SETUP).
       651              7887   K bytes available for DOS/16M programs.
22.0 (3.0)        18.9 (4.0)   MB/sec word transfer rate (wait states).
42.9 (3.0)        37.0 (4.0)   MB/sec 32-bit transfer rate (wait states).

Overall cpu and memory performance (non-floating point) for typical
DOS programs is 10.36 ñ 1.04 times an 8MHz IBM PC/AT.

Protected/Real switch rate = 36156/sec (27 usec/switch, 15 up + 11 down),
DOS/16M switch mode 11 (VCPI).
.in +2
.cbox end
.np
The top information line shows that the CPU performance is equivalent
to a 67.0 MHz 80486.
Below are the configuration and timings for both the DOS memory and
extended memory.
If the computer is not equipped with extended memory, or none is
available for &dos4g, the extended memory measurements may be omitted
("--").
.np
The line "according to BIOS" shows the information provided by the
BIOS (interrupts 12h and 15h function 88h).
The line "SETUP", if displayed, is the configuration obtained directly
from the CMOS RAM as set by the computer's setup program.
It is displayed only if the numbers are different from those in the
BIOS line.
They will be different for computers where the BIOS has reserved
memory for itself or if another program has allocated some memory and
is intercepting the BIOS configuration requests to report less memory
available than is physically configured.
The "DOS/16M memory range", if displayed, shows the low and high
addresses available to &dos4g in extended memory.
.np
.ix 'memory transfer rate'
.ix 'transfer rate' 'memory'
Below the configuration information is information on the memory speed
.us (transfer rate).
PMINFO tries to determine the memory architecture.
Some architectures will perform well under some circumstances and
poorly under others; PMINFO will show both the best and worst cases.
The architectures detected are cache, interleaved, page-mode (or
static column), and direct.
Measurements are made using 32-bit accesses and reported as the number
of megabytes per second that can be transferred.
.ix 'memory wait states'
The number of wait states is reported in parentheses.
The wait states can be a fractional number, like 0.5, if there is a
wait state on writes but not on reads.
Memory bandwidth (i.e., how fast the CPU can access memory) accounts
for 60% to 70% of the performance for typical programs (that are not
heavily dependent on floating-point math).
.np
.ix 'performance'
A performance metric developed by Tenberry Software is displayed,
showing the expected throughput for the computer relative to a
standard 8MHz IBM PC/AT (disk accesses and floating point are
excluded).
Finally, the speed with which the computer can switch between real and
protected mode is displayed, both as the maximum number of round-trip
switches that can occur per second, and the time for a single
round-trip switch, broken out into the real-to-protected (up) and
protected-to-real (down) components.
.endnote
.*
.npsection PRIVATXM
.*
.ix 'PRIVATXM'
.begnote $setptnt 6
.note Purpose:
.ix 'private memory pool'
Creates a private pool of memory for &dos4g programs.
.note Syntax:
.mono PRIVATXM [-r]
.note Notes:
This program may be distributed to your users.
.np
Without PRIVATXM, a &dos4g program that starts up while another &dos4g
program is active uses the pool of memory built by the first program.
The new program cannot change the parameters of this memory pool, so
setting
.ev DOS16M
to increase the size of the pool has no effect.
To specify that the two programs use different pools of memory, use
PRIVATXM.
.np
PRIVATXM marks the active &dos4g programs as private, preventing
subsequent &dos4g programs from using the same memory pool.
The first &dos4g program to start after PRIVATXM sets up a new pool of
memory for itself and any subsequent &dos4g programs.
To release the memory used by the private programs, use the PRIVATXM
.mono -r
option.
.np
PRIVATXM is a TSR that requires less than 500 bytes of memory.
It is not supported under DPMI.
.note Example:
The following example creates a 512KB memory pool that is shared by
two &dos4g TSRs.
Subsequent &dos4g programs use a different memory pool.
.begnote $setptnt 15
.note C>set DOS16M= :512
Specifies the size of the memory pool.
.note C>TSR1
Sets up the memory pool at startup.
.note C>TSR2
This TSR shares the pool built by TSR1.
.note C>PRIVATXM
Makes subsequent &dos4g programs use a new memory pool.
.note C>set DOS16M=
Specifies an unlimited size for the new pool.
.note C>PROGRAM3
This program uses the new memory pool.
.note C>PRIVATXM -R
Releases the 512KB memory pool used by the TSRs.
(If the TSRs shut down, their memory is not released unless PRIVATXM
is released.)
.endnote
.endnote
.*
.npsection RMINFO
.*
.ix 'RMINFO'
.begnote $setptnt 6
.note Purpose:
.ix 'mode switching' 'basis'
Supplies configuration information and the basis for
real/protected-mode switching in your machine.
.note Syntax:
.mono RMINFO.EXE
.note Notes:
This program may be distributed to your users.
.np
RMINFO starts up &dos4g, but stops your machine just short of
switching from real mode to protected mode and displays configuration
information about your computer.
The information shown by RMINFO can help determine why &dos4g
applications won't run on a particular machine.
Run RMINFO if PMINFO does not run to completion.
.note Example:
The following example shows the output of the RMINFO program on an
386 AT-compatible machine.
.cbox begin
.in -2
C>rminfo

DOS/16M Real Mode Information Program 5.00
Copyright (C) Tenberry Software, Inc. 1987 - 1993

Machine and Environment:
        Processor:              i386, coprocessor present
        Machine type:           10 (AT-compatible)
        A20 now:                enabled
        A20 switch rigor:       disabled
        DPMI host found
Switching Functions:
        To PM switch:           DPMI
        To RM switch:           DPMI
        Nominal switch mode:    0
        Switch control flags:   0000
Memory Interfaces:
        DPMI may provide:       16384K returnable
        Contiguous DOS memory:  463K
.in +2
.cbox end
.np
The information provided by RMINFO includes:
:CMT. .begnote $break
:CMT. :DTHD.Measurement
:CMT. :DDHD.Purpose
:CMT. .note Triple fault flag
:CMT. .ix 'triple fault flag'
:CMT. Whether triple faulting is used to switch back to real mode from
:CMT. protected mode.
:CMT. .note A20 switch rigor
:CMT. .ix 'address line 20'
:CMT. Whether &dos4g rigorously controls enabling and disabling of Address
:CMT. line 20 when switching modes.
:CMT. .note A20 now
:CMT. Current state of Address line 20.
:CMT. .note PS feature flag
:CMT. .note XMS host found
:CMT. .ix 'XMS'
:CMT. Whether your system has any software using extended memory under the
:CMT. XMS discipline.
:CMT. .note VCPI host found
:CMT. .ix 'VCPI'
:CMT. Whether your system has any software using extended memory under the
:CMT. VCPI discipline.
:CMT. .note VDISK device found
:CMT. .ix 'VDISK'
:CMT. Whether your system has any software using extended memory under the
:CMT. VDISK discipline.
:CMT. .note QEXT device found
:CMT. .ix 'QEXT'
:CMT. Whether your system has any software using extended memory under the
:CMT. QEXT discipline.
:CMT. .note XMS system found
:CMT. .ix 'XMS'
:CMT. Whether your system has any software using extended memory under the
:CMT. XMS discipline.
:CMT. .note Configured memory
:CMT. Amount of memory &dos4g will try to use.
:CMT. .note Actual memory
:CMT. Amount of memory &dos4g can actually use.
:CMT. .note This is the ...
:CMT. This number will vary according to how many copies of &dos4g or
:CMT. DOS/16M-based applications are currently active on your system.
:CMT. .note VCPI (not) in use
:CMT. .ix 'VCPI'
:CMT. Whether your system has any software using extended memory under the
:CMT. VCPI discipline.
.*
.begnote Machine and Environment:
.note Processor:
processor type, coprocessor present/not present
.note Machine type:
.illust begin
(NEC 9801)
(PS/2-compatible)
(AT-compatible)
(FM R)
(AT&T 6300+)
(AT-compatible)
(C&T 230 chipset)
(AT-compatible)
(AT-compatible)
(Acer)
(Zenith)
(Hitachi)
(Okidata)
(PS/55)
.illust end
.note A20 now:
Current state of Address line 20.
.note A20 switch rigor:
Whether &dos4gnam rigorously controls enabling and disabling of
Address line 20 when switching modes.
.note PS feature flag
.note XMS host found
Whether your system has any software using extended memory under the
XMS discipline.
.note VCPI host found
Whether your system has any software using extended memory under the
VCPI discipline.
.note page table 0 at: x000h
.note DPMI host found
.note DOS/16M resident with private/public memory
.endnote
.*
.begnote Switching Functions:
.note A20 switching:
.note To PM switch:
reset catch:
.br
pre-PM prep:
.br
post-PM-switch:
.note To RM switch:
.br
pre-RM prep:
.br
reset method:
.br
post-reset:
.br
reset uncatch:
.note Nominal switch mode:    x
.note Switch control flags:   xxxxh
.endnote
.*
.begnote Memory Interfaces:
.note (VCPI remapping in effect)
.note DPMI may provide: xxxxxK returnable
.note VCPI may provide: xxxxxK returnable
.note Top-down
.note Other16M
.note Forced
.note Contiguous DOS memory:
.endnote
.endnote
