.* (c) Portions Copyright 1990, 1995 by Tenberry Software, Inc.
.*
.chap Interrupt 21H Functions
.*
.np
.ix 'INT 21H'
When you call an Interrupt 21H function under &dos4g, the 32-bit
registers in which you pass values are translated into the appropriate
16-bit registers, since DOS works only with 16 bits.
However, you can use 32-bit values in your DOS calls.
You can allocate blocks of memory larger than 64KB or use an address
with a 32-bit offset, and &dos4g will translate the call
appropriately, to use 16-bit registers.
When the Interrupt 21H function returns, the value is widened - placed
in a 32-bit register, with the high order bits zeroed.
.np
&dos4g uses the following rules to manage registers:
.begbull
.bull
When you pass a parameter to an Interrupt 21H function that expects a
16-bit quantity in a general register (for example, AX), pass a 32-bit
quantity in the corresponding extended register (for example, EAX).
When a DOS function returns a 16-bit quantity in a general register,
expect to receive it (with high-order zero bits) in the corresponding
extended register.
.bull
When an Interrupt 21H function expects to receive a 16:16 pointer in a
segment:general register pair (for example, ES:BX), supply a 16:32
pointer using the same segment register and the corresponding extended
general register (ES:EBX).
&dos4g will copy data and translate pointers so that DOS ultimately
receives a 16:16 real-mode pointer in the correct registers.
.bull
When DOS returns a 16:16 real-mode pointer, &dos4g translates the
segment value into an appropriate protected-mode selector and
generates a 32-bit offset that results in a 16:32 pointer to the same
location in the linear address space.
.bull
Many DOS functions return an error code in AX if the function fails.
&dos4g checks the status of the carry flag, and if it is set,
indicating an error, zero-extends the code for EAX. It does not change
any other registers.
.bull
If the value is passed or returned in an 8-bit register (AL or AH, for
example), &dos4g puts the value in the appropriate location and leaves
the upper half of the 32-bit register untouched.
.endbull
.np
The table below lists all the Interrupt 21h functions.
For each, it shows the registers that are widened or narrowed.
Footnotes provide additional information about some of the interrupts
that require special handling.
Following the table is a section that provides a detailed explanation
of interrupt handling under
.us &dos4gprd..
.np
.cp 25
.se c0=&INDlvl+2
.se c1=&INDlvl+5
.se c2=&INDlvl+9
.se c3=&INDlvl+53
.tb set $
.tb &c0 &c1 &c2 &c3.r
$Function$Purpose$Managed Registers
.sk 1 c
$00H$$Terminate Process                        $None
$01H$$Character Input with Echo                $None
$02H$$Character Output                         $None
$03H$$Auxiliary Input                          $None
$04H$$Auxiliary Output                         $None
$05H$$Print Character                          $None
$06H$$Direct Console I/O                       $None
$07H$$Unfiltered Character Input Without Echo  $None
$08H$$Character Input Without Echo             $None
$09H$$Display String                           $EDX
$0AH$$Buffered Keyboard Input                  $EDX
$0BH$$Check Keyboard Status                    $None
$0CH$$Flush Buffer, Read Keyboard              $EDX
$0DH$$Disk Reset                               $None
$0EH$$Select Disk                              $None
$0FH$$Open File with FCB                       $EDX
.sk 1 c
$10H$$Close File with FCB                      $EDX
$11H$$Find First File                          $EDX
$12H$$Find Next File                           $EDX
$13H$$Delete File                              $EDX
$14H$$Sequential Read                          $EDX
$15H$$Sequential Write                         $EDX
$16H$$Create File with FCB                     $EDX
$17H$$Rename File                              $EDX
$19H$$Get Current Disk                         $None
$1AH$$Set DTA Address                          $EDX
$1BH$$Get Default Drive Data                   $Returns in EBX, ECX, and EDX
$1CH$$Get Drive Data                           $Returns in EBX, ECX, and EDX
.sk 1 c
$21H$$Random Read                              $EDX
$22H$$Random Write                             $EDX
$23H$$Get File Size                            $EDX
$24H$$Set Relative Record                      $EDX
$25H$$Set Interrupt Vector                     $EDX
$26H$$Create New Program Segment Prefix        $None
$27H$$Random Block Read                        $EDX, returns in ECX
$28H$$Random Block Write                       $EDX, returns in ECX
$29H$$Parse Filename                           $ESI, EDI, returns in EAX, ESI and EDI (1.)
$2AH$$Get Date                                 $Returns in ECX
$2BH$$Set Date                                 $None
$2CH$$Get Time                                 $None
$2DH$$Set Time                                 $None
$2EH$$Set/Reset Verify Flag                    $None
$2FH$$Get DTA Address                          $Returns in EBX
.sk 1 c
$30H$$Get MS-DOS Version Number                $Returns in ECX
$31H$$Terminate and Stay Resident              $None
$33H$$Get/Set Control-C Check Flag             $None
$34H$$Return Address of InDOS Flag             $Returns in EBX
$35H$$Get Interrupt Vector                     $Returns in EBX
$36H$$Get Disk Free Space                      $Returns in EAX, EBX, ECX, and EDX
$38H$$Get/Set Current Country                  $EDX, returns in EBX
$39H$$Create Directory                         $EDX
$3AH$$Remove Directory                         $EDX
$3BH$$Change Current Directory                 $EDX
$3CH$$Create File with Handle                  $EDX, returns in EAX
$3DH$$Open File with Handle                    $EDX, returns in EAX
$3EH$$Close File                               $None
$3FH$$Read File or Device                      $EBX, ECX, EDX, returns in EAX (2.)
.sk 1 c
$40H$$Write File or Device                     $EBX, ECX, EDX, returns in EAX (2.)
$41H$$Delete File                              $EDX
$42H$$Move File Pointer                        $Returns in EDX, EAX
$43H$$Get/Set File Attribute                   $EDX, returns in ECX
$44H$$IOCTL                                    $(3.)
$$00H$Get Device Information                   $Returns in EDX
$$01H$SetDevice Information                    $None
$$02H$Read Control Data from CDD               $EDX, returns in EAX
$$03H$Write Control Data to CDD                $EDX, returns in EAX
$$04H$Read Control Data from BDD               $EDX, returns in EAX
$$05H$Write Control Data to BDD                $EDX, returns in EAX
$$06H$Check Input Status                       $None
$$07H$Check Output Status                      $None
$$08H$Check if Block Device is Removeable      $Returns in EAX
$$09H$Check if Block Device is Remote          $Returns in EDX
$$0AH$Check if Handle is Remote                $Returns in EDX
$$0BH$Change Sharing Retry Count               $None
$$0CH$Generic I/O Control for Character Devices$EDX
$$0DH$Generic I/O Control for Block Devices    $EDX
$$0EH$Get Logical Drive Map                    $None
$$0FH$Set Logical Drive Map                    $None
$45H$$Duplicate File Handle                    $Returns in EAX
$46H$$Force Duplicate File Handle              $None
$47H$$Get Current Directory                    $ESI
$48H$$Allocate Memory Block                    $Returns in EAX
$49H$$Free Memory Block                        $None
$4AH$$Resize Memory Block                      $None
$4BH$$Load and Execute Program (EXEC)          $EBX, EDX (4.)
$4CH$$Terminate Process with Return Code       $None
$4DH$$Get Return Code of Child Process         $None
$4EH$$Find First File                          $EDX
$4FH$$Find Next File                           $None
.sk 1 c
:cmt.$52H$$Get List of Lists                        $Returns in EBX
$52H$$Get List of Lists                        $(not supported)
$54H$$Get Verify Flag                          $None
$56H$$Rename File                              $EDX, EDI
$57H$$Get/Set Date/Time of File                $Returns in ECX, and EDX
$58H$$Get/Set Allocation Strategy              $Returns in EAX
$59H$$Get Extended Error Information           $Returns in EAX
$5AH$$Create Temporary File                    $EDX, returns in EAX and EDX
$5BH$$Create New File                          $EDX, returns in EAX
$5CH$$Lock/Unlock File Region                  $None
$5EH$$Network Machine Name/Printer Setup$
$$00H$Get Machine Name                         $EDX
$$02H$Set Printer Setup String                 $ESI
$$03H$Get Printer Setup String                 $EDI, returns in ECX
$5FH$$Get/Make Assign List Entry$
$$02H$Get Redirection List Entry               $ESI, EDI, returns in ECX
$$03H$Redirect Device                          $ESI, EDI
$$04H$Cancel Device Redirection                $ESI
.sk 1 c
$62H$$Get Program Segment Prefix Address       $Returns in EBX
$63H$$Get Lead Byte Table (version 2.25 only)  $Returns in ESI
$65H$$Get Extended Country Information         $EDI
$66H$$Get or Set Code Page                     $None
$67H$$Set Handle Count                         $None
.tb set
.tb
.np
This list of functions is excerpted from
:CIT.The MS-DOS Encyclopedia:eCIT., Copyright (c) 1988 by Microsoft Press.
All Rights Reserved.
.autonote
.note
For Function 29H, DS:ESI and ES:EDI contain pointer values that are not
changed by the call.
.note
You can read and write quantities larger than 64KB with Functions 3FH
and 40H.
&dos4g breaks your request into chunks smaller than 64KB, and calls
the DOS function once for each chunk.
.note
You can't transfer more than 64KB using Function 44h, subfunctions
02H, 03H, 04H, or 05H.
&dos4g does not break larger requests into DOS-sized chunks, as it
does for Functions 3FH and 40H.
.note
When you call Function 4B under &dos4g, you pass it a data structure
that contains 16:32 bit pointers.
&dos4g translates these into 16:16 bit pointers in the structure it
passes to DOS.
.endnote
.*
.section Functions 25H and 35H: Interrupt Handling in Protected Mode
.*
.np
.ix 'interrupt handling'
.ix 'IDT'
By default, interrupts that occur in protected mode are passed down:
the entry in the IDT points to code in &dos4g that switches the CPU to
real mode and resignals the interrupt.
If you install an interrupt handler using Interrupt 21H, Function 25H,
that handler will get control of any interrupts that occur while the
processor is in protected mode.
.ix 'autopassup range'
If the interrupt for which you installed the handler is in the
.us autopassup range,
your handler will also get control of interrupts signalled in
real mode.
.np
The autopassup range runs from 08H to 2EH inclusive, but excluding 21H.
If the interrupt is in the autopassup range, the real-mode vector will
be modified when you install the protected-mode handler to point to
code in the &dos4g kernel.
This code switches the processor to protected mode and resignals the
interrupt-where your protected-mode handler will get control.
.*
.beglevel
.*
.section 32-Bit Gates
.*
.np
.ix '32-bit gates'
The &dos4g kernel always assigns a 32-bit gate for the interrupt
handlers it installs.
.ix 'DPMI'
It does not distinguish between 16-bit and 32-bit handlers for
consistency with DPMI.
.np
This 32-bit gate points into the &dos4g kernel.
When &dos4g handles the interrupt, it switches to its own 16-bit
stack, and from there it calls the interrupt handler (yours or the
default).
This translation is transparent to the handler, with one exception:
since the current stack is not the one on which the interrupt
occurred, the handler cannot look up the stack for the address at
which the interrupt occurred.
.*
.section Chaining 16-bit and 32-bit Handlers
.*
.np
.ix '&dos4gprd' 'interrupt handlers'
.ix '&dos4gprd' 'chaining handlers'
If your program hooks an interrupt, write a normal service routine
that either handles the interrupt and IRETs or chains to the previous
handler.
As part of handling the interrupt, your handler can PUSHF/CALL to the
previous handler.
The handler
.us must
IRET (or IRETD) or chain.
.np
For each protected-mode interrupt, &dos4g maintains separate chains of
16-bit and 32-bit handlers.
If your 16-bit handler chains, the previous handler is a 16-bit
program.
If your 32-bit handler chains, the previous handler is a 32-bit
program.
.np
If a 16-bit program hooks a given interrupt before any 32-bit programs
hook it, the 16-bit chain is executed first.
If all the 16-bit handlers unhook later and a new 16-bit program hooks
the interrupt while 32-bit handlers are still outstanding, the 32-bit
handlers will be executed first.
.np
If the first program to hook an interrupt is 32-bit, the 32-bit chain
is executed first.
.*
.section Getting the Address of the Interrupt Handler
.*
.np
.ix '&dos4gprd' 'interrupt handler address'
When you signal Interrupt 21H, Function 35, it always returns a
non-null address even if no other program of your bitness (i.e.,
16-bit or 32-bit) has hooked the interrupt.
The address points to a dummy handler that looks to you as though it
does an IRET to end the chain.
This means that you can't find an unused interrupt by looking for a
NULL pointer.
Since this technique is most frequently used by programs that are
looking for an unclaimed
.us real-mode
interrupt on which to install a TSR, it shouldn't cause you problems.
.*
.endlevel
