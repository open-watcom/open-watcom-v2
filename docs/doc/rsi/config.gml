.* (c) Portions Copyright 1990, 1995 by Tenberry Software, Inc.
.*
.chap Configuring &dos4gprd
.*
.np
This chapter explains various options that can be specified with the
.ev DOS4G
environment variable including how to suppress the banner that is
displayed by &dos4g at startup.
.ix 'extended memory'
It also explains how to use the
.ev DOS16M
environment variable to select the switch mode setting, if necessary,
and to specify the range of extended memory in which &dos4g will
operate.
&dos4g is based on Tenberry Software's DOS/16M 16-bit Protected-Mode
support;
hence the
.ev DOS16M
environment variable name remains unchanged.
.*
.section The DOS4G Environment Variable
.*
.np
A number of options can be selected by setting the
.ev DOS4G
environment variable.
The syntax for setting options is:
.millust begin
set DOS4G=option1,option2,...
.millust end
.np
Do not insert a space between
.ev DOS4G
and the equal sign.
A space to the right of the equal sign is optional.
.begnote Options:
.note QUIET
.ix 'DOS4G' 'QUIET option'
.ix 'QUIET'
Use this option to suppress the &dos4g banner.
.np
The banner that is displayed by &dos4g at startup can be suppressed by
issuing the following command:
.millust begin
set DOS4G=quiet
.millust end
.np
.bi Note:
Use of the quiet switch is only permitted pursuant to the terms and
conditions of the WATCOM Software License Agreement and the additional
redistribution rights described in the
.book Getting Started
manual.
Under these terms, suppression of the copyright by using the quiet
switch is not permitted for applications which you distribute to
others.
.note VERBOSE
.ix 'DOS4G' 'VERBOSE option'
.ix 'VERBOSE'
Use this option to maximize the information available for postmortem
debugging.
.np
Before running your application, issue the following command:
.millust begin
set DOS4G=verbose
.millust end
.np
Reproduce the crash and record the output.
.note NULLP
.ix 'DOS4G' 'NULLP option'
.ix 'NULLP'
Use this option to trap references to the first sixteen bytes of
physical memory.
.np
Before running your application, issue the following command:
.millust begin
set DOS4G=nullp
.millust end
.endnote
.np
To select a combination of options, list them with commas as
separators.
.exam begin
set DOS4G=nullp,verbose
.exam end
.*
.section Changing the Switch Mode Setting
.*
.np
.ix '&dos4gprd' 'switch mode setting'
In almost all cases, &dos4g programs can detect the type of machine
that is running and automatically choose an appropriate real- to
protected-mode switch technique.
For the few cases in which this default setting does not work we
provide the
.ev DOS16M
DOS environment variable, which overrides the default setting.
.np
Change the switch mode settings by issuing the following command:
.millust begin
set DOS16M=value
.millust end
.pc
Do not insert a space between
.ev DOS16M
and the equal sign.
A space to the right of the equal sign is optional.
.np
The table below lists the machines and the settings you would use with
them.
Many settings have mnemonics, listed in the column "Alternate Name",
that you can use instead of the number.
Settings that you must set with the
.ev DOS16M
variable have the notation
.us req'd
in the first column.
Settings you may use are marked
.us option,
and settings that will automatically be set are marked
.us auto.
.np
.ix 'NEC 98-series switch mode setting'
.ix 'PS/2 switch mode setting'
.ix 'Fujitsu FMR-70 switch mode setting'
.ix 'Hitachi B32 switch mode setting'
.ix 'OKI if800 switch mode setting'
.ix 'IBM PS/55 switch mode setting'
.ix 'switch mode setting' 'NEC 98-series'
.ix 'switch mode setting' 'PS/2'
.ix 'switch mode setting' 'Fujitsu FMR-70'
.ix 'switch mode setting' 'Hitachi B32'
.ix 'switch mode setting' 'OKI if800'
.ix 'switch mode setting' 'IBM PS/55'
.cp 18
.in 0
.if '&format' eq '7x9' .do begin
.se c0=1+1
.se c1=1+6
.se c2=1+19
.se c3=1+28
.se c4=1+36
.se c5=1+60
.do end
.el .do begin
.se c0=&INDlvl+2
.se c1=&c0.+5
.se c2=&c1.+12
.se c3=&c2.+8
.se c4=&c3.+8
.se c5=&c4.+22
.do end
.tb set $
.tb &c0 &c1 &c2 &c3 &c4 &c5
.se c0=&c0.-1
.se c1=&c1.-1
.se c2=&c2.-1
.se c3=&c3.-1
.se c4=&c4.-1
.se c5=&c5.-1
.bx on &c0 &c1 &c2 &c3 &c4 &c5
$      $               $       $Alternate
$Status$Machine        $Setting$Name      $Comment
.bx
$auto  $386/486 w/ DPMI$0      $None      $Set automatically if DPMI is active
$req'd $NEC 98-series  $1      $9801      $Must be set for NEC 98-series
$auto  $PS/2           $2      $None      $Set automatically for PS/2
$auto  $386/486        $3      $386, 80386$Set automatically for 386 or 486
$auto  $386            $INBOARD$None      $386 with Intel Inboard
$req'd $Fujitsu FMR-70 $5      $None      $Must be set for Fujitsu FMR-70
$auto  $386/486 w/ VCPI$11     $None      $Set automatically if VCPI detected
$req'd $Hitachi B32    $14     $None      $Must be set for Hitachi B32
$req'd $OKI if800      $15     $None      $Must be set for OKI if800
$option$IBM PS/55      $16     $None      $May be needed for some PS/55s
.bx off
.tb set
.tb
.in &INDlvl
.np
The following procedure shows you how to test the switch mode setting.
.autonote
.note
If you have one of the machines listed below, set the
.ev DOS16M
environment variable to the value shown for that machine and specify a
range of extended memory.
For example, if your machine is a NEC 98-series, set
.mono DOS16M=1 @2M-4M.
See the section entitled :HDREF refid='rsicfgf'. in this chapter for
more information about setting the memory range.
.np
.if '&format' eq '7x9' .do begin
.se c0=&INDlvl+2+8
.se c1=&INDlvl+2+23
.se c2=&INDlvl+2+30
.do end
.el .do begin
.se c0=&INDlvl+2+8
.se c1=&INDlvl+2+29
.se c2=&INDlvl+2+39
.do end
.cp 10
.tb set $
.tb &c0 &c1
.se c0=&c0.-2
.se c1=&c1.-2
.se c2=&c2.-2
.bx &c0 &c1 &c2
$Machine$Setting
.bx
$NEC 98-series       $1
$Fujitsu FMR-60,-70  $5
$Hitachi B32         $14
$OKI if800           $15
.bx off
.tb set
.tb
.np
Before running &dos4g applications, check the switch mode setting by
following this procedure:
.note
.ix 'PMINFO'
Run PMINFO and note the switch setting reported on the last line of
the display.
(PMINFO, which reports on the protected-mode resources available
to your programs, is described in more detail in the
chapter entitled :HDREF refid='rsiutls'.)
.np
If PMINFO runs, the setting is usable on your machine.
.note
If you changed the switch setting, add the new setting to your
AUTOEXEC.BAT file.
.endnote
.np
.bd Note:
PMINFO will run successfully on 286 machines.
If your &dos4g application does not run, and PMINFO does, check the
CPU type reported on the first line of the display.
.np
You are authorized (and encouraged) to distribute PMINFO to your
customers.
You may also include a copy of this section in your documentation.
.*
.section *refid=rsicfgf Fine Control of Memory Usage
.*
.np
.ix '&dos4gprd' 'memory control'
In addition to setting the switch mode as described above, the
.ev DOS16M
environment variable enables you to specify which portion of extended
memory &dos4g will use.
The variable also allows you to instruct &dos4g to search for extra
memory and use it if it is present.
.*
.beglevel
.*
.section Specifying a Range of Extended Memory
.*
.np
.ix '&dos4gprd' 'memory range'
Normally, you don't need to specify a range of memory with the
.ev DOS16M
variable.
You must use the variable, however, in the following cases:
.begbull
.bull
You are running on a Fujitsu FMR-series, NEC 98-series, OKI
if800-series or Hitachi B-series machine.
.bull
You have older programs that use extended memory but don't follow one
of the standard disciplines.
.bull
You want to shell out of &dos4g to use another program that requires
extended memory.
.endbull
.np
If none of these conditions applies to you, you can skip this section.
.np
The general syntax is:
.code begin
set DOS16M= [switch_mode] [@start_address [- end_address]] [:size]
.code end
.pc
In the syntax shown above,
.mono start_address,
.mono end_address
and
.mono size
represent numbers, expressed in decimal or in hexadecimal (hex
requires a
.mono 0x
prefix).
The number may end with a K to indicate an address or size in
kilobytes, or an M to indicate megabytes.
If no suffix is given, the address or size is assumed to be in
kilobytes.
If both a size and a range are specified, the more restrictive
interpretation is used.
.np
The most flexible strategy is to specify only a size.
However, if you are running with other software that does not follow a
convention for indicating its use of extended memory, and these other
programs start before &dos4g, you will need to calculate the range of
memory used by the other programs and specify a range for &dos4g
programs to use.
.np
&dos4g ignores specifications (or parts of specifications) that
conflict with other information about extended memory use.
Below are some examples of memory usage control:
.begnote $setptnt 24
.note set DOS16M= 1 @2m-4m
Mode 1, for NEC 98-series machines, and use extended memory between
2.0 and 4.0MB.
.note set DOS16M= :1M
Use the last full megabyte of extended memory, or as much as available
limited to 1MB.
.note set DOS16M= @2m
Use any extended memory available above 2MB.
.note set DOS16M= @ 0 - 5m
Use any available extended memory from 0.0 (really 1.0) to 5.0MB.
.note set DOS16M= :0
Use no extended memory.
.endnote
.np
As a default condition &dos4g applications take all extended memory
that is not otherwise in use.
Multiple &dos4g programs that execute simultaneously will share the
reserved range of extended memory.
Any non-&dos4gct programs started while &dos4g programs are executing
will find that extended memory above the start of the &dos4g range is
unavailable, so they may not be able to run.
This is very safe.
There will be a conflict only if the other program does not check the
BIOS configuration call (Interrupt 15H function 88H, get extended
memory size).
.np
.ix 'PRIVATXM'
To create a private pool of extended memory for your &dos4g
application, use the PRIVATXM program, described in the
chapter entitled :HDREF refid='rsiutls'..
.np
The default memory allocation strategy is to use extended memory if
available, and overflow into DOS (low) memory.
:cmt. :I1.D16MemStrategy
:cmt. If you require that your memory allocations be confined to your
:cmt. specified extended memory range, use the &dos4g function
:cmt. .mono D16MemStrategy(MForceExt)
:cmt. in your program.
.np
.ix 'VCPI'
.ix 'DPMI'
In a VCPI or DPMI environment, the
.mono start_address
and
.mono end_address
arguments are not meaningful.
&dos4g memory under these protocols is not allocated according to
specific addresses because VCPI and DPMI automatically prevent address
conflicts between extended memory programs.
You can specify a
.mono size
for memory managed by VCPI or DPMI, but &dos4g will not necessarily
allocate this memory from the highest available extended memory
address, as it does for memory managed under other protocols.
.*
.section Using Extra Memory
.*
.np
.ix '&dos4gprd' 'extra memory'
Some machines contain extra non-extended, non-conventional memory just
below 16MB.
.ix 'Compaq 386 memory'
When &dos4g runs on a Compaq 386, it automatically uses this memory
because the memory is allocated according to a certain protocol, which
&dos4g follows.
Other machines have no protocol for allocating this memory.
.ix 'DOS16M' '+ option'
To use the extra memory that may exist on these machines, set
.ev DOS16M
with the + option.
.millust begin
set DOS16M=+
.millust end
.np
Setting the + option causes &dos4g to search for memory in the range
from FA0000 to FFFFFF and determine whether the memory is usable.
&dos4g does this by writing into the extra memory and reading what it
has written.
In some cases, this memory is mapped for DOS or BIOS usage, or for
other system uses.
If &dos4g finds extra memory that is mapped this way, and is not
marked read-only, it will write into that memory.
This will cause a crash, but won't have any other effect on your
system.
.*
.endlevel
.*
.section Setting Runtime Options
.*
.np
.ix '&dos4gprd' 'runtime options'
.ix 'DOS16M' 'runtime options'
The
.ev DOS16M
environment variable sets certain runtime options for all &dos4g
programs running on the same system.
.np
To set the environment variable, the syntax is:
.millust begin
set DOS16M=[switch_mode_setting]^options.
.millust end
.np
.bd Note:
Some command line editing TSRs, such as CED, use the caret (^)
as a delimiter.
If you want to set
.ev DOS16M
using the syntax above while one of these TSRs is resident, modify the
TSR to use a different delimiter.
.np
These are the options:
.begnote
.note 0x01
.us check A20 line --
.ix 'A20 line'
.ix 'protected mode'
This option forces &dos4g to wait until the A20 line is enabled before
switching to protected mode.
.ix 'XMS'
.ix 'HIMEM.SYS'
.ix 'real mode'
When &dos4g switches to real mode, this option suspends your program's
execution until the A20 line is disabled, unless an XMS manager (such
as HIMEM.SYS) is active.
If an XMS manager is running, your program's execution is suspended
until the A20 line is restored to the state it had when the CPU was
last in real mode.
Specify this option if you have a machine that runs &dos4g but is not
truly AT-compatible.
For more information on the A20 line, see the
section entitled :HDREF refid='ctrl20'..
.note 0x02
.us prevent initialization of VCPI --
By default, &dos4g searches for a VCPI server and, if one is present,
forces it on.
This option is useful if your application does not use EMS explicitly,
is not a resident program, and may be used with 386-based EMS
simulator software.
.note 0x04
.us directly pass down keyboard status calls --
.ix 'keyboard status'
When this option is set, status requests are passed down immediately
and unconditionally.
.ix '8042 auxiliary processor'
When disabled, pass-downs are limited so the 8042 auxiliary processor
does not become overloaded by keyboard polling loops.
.note 0x10
.us restore only changed interrupts --
Normally, when a &dos4g program terminates, all interrupts are
restored to the values they had at the time of program startup.
When you use this option, only the interrupts changed by the &dos4g
program are restored.
.note 0x20
.us set new memory to 00 --
When &dos4g allocates a new segment or increases the size of a
segment, the memory is zeroed.
This can help you find bugs having to do with uninitialized memory.
You can also use it to provide a consistent working environment
regardless of what programs were run earlier.
This option only affects segment allocations or expansions that are
made through the &dos4g kernel (with DOS function 48H or 4AH).
This option does not affect memory allocated with a compiler's
.mono malloc
function.
.note 0x40
.us set new memory to FF --
When &dos4g allocates a new segment or increases the size of a
segment, the memory is set to 0xFF bytes.
This is helpful in making reproducible cases of bugs caused by using
uninitialized memory.
This option only affects segment allocations or expansions that are
made through the &dos4g kernel (with DOS function 48H or 4AH).
This option does not affect memory allocated with a compiler's
.mono malloc
function.
.note 0x80
.us new selector rotation --
When &dos4g allocates a new selector, it usually looks for the first
available (unused) selector in numerical order starting with the
highest selector used when the program was loaded.
When this option is set, the new selector search begins after the last
selector that was allocated.
This causes new selectors to rotate through the range.
Use this option to find references to
.us stale
selectors, i.e., segments that have been cancelled or freed.
.endnote
.*
.section *refid=ctrl20 Controlling Address Line 20
.*
.np
.ix '&dos4gprd' 'address line 20'
.ix 'address line 20'
.ix 'A20 line'
This section explains how &dos4g uses address line 20 (A20) and
describes the related
.ev DOS16M
environment variable settings.
It is unlikely that you will need to use these settings.
.np
Because the 8086 and 8088 chips have 20-bit address spaces, their
highest addressable memory location is one byte below 1MB.
If you specify an address at 1MB or over, which would require a
twenty-first bit to set, the address wraps back to zero.
Some parts of DOS depend on this wrap, so on the 286 and 386, the
twenty-first address bit is disabled.
To address extended memory, &dos4g enables the twenty-first address
bit (the A20 line).
The A20 line must be enabled for the CPU to run in protected mode, but
it may be either enabled or disabled in real mode.
.np
By default, when &dos4g returns to real mode, it disables the A20
line.
Some software depends on the line being enabled.
&dos4g recognizes the most common software in this class, the XMS
managers (such as HIMEM.SYS), and enables the A20 line when it returns
to real mode if an XMS manager is present.
.ix 'DOS16M' 'A20 option'
For other software that requires the A20 line to be enabled, use the
.mono A20
option.
The
.mono A20
option makes &dos4g restore the A20 line to the setting it had when
&dos4g switched to protected mode.
Set the environment variable as follows:
.millust begin
set DOS16M=A20
.millust end
.np
To specify more than one option on the command line, separate the
options with spaces.
.np
The
.ev DOS16M
variable also lets you to specify the length of the delay between a
&dos4g instruction to change the status of the A20 line and the next
&dos4g operation.
By default, this delay is 1 loop instruction when &dos4g is running on
a 386 machine.
In some cases, you may need to specify a longer delay for a machine
that will run &dos4g but is not truly AT-compatible.
To change the delay, set
.ev DOS16M
to the desired number of loop instructions, preceded by a comma:
.ix 'DOS16M' 'loops option'
.millust begin
set DOS16M=,loops
.millust end
