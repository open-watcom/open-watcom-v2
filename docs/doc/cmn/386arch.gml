.chap 386 Architecture
.*
.section Registers
.*
.np
.ix 'registers'
The register model for the 386 microprocessor consists of 8 32-bit
general-purpose registers used primarily to contain operands for
arithmetic and logical operations,
6 segment registers which determine which segments of memory are
addressable,
and status and instruction registers used to record and alter aspects
of the 386 processor state.
.*
.section General Registers
.*
.np
.ix 'general registers'
The general-purpose registers are named EAX, EBX, ECX, EDX, EBP, ESP,
ESI and EDI.
The low-order word of these registers have different names and can be
used for handling 16-bit data items.
The word registers are called AX, BX, CX, DX, BP, SP, SI and DI.
Each byte of the word registers AX, BX, CX and DX has a separate name.
The byte registers are named AH, AL, BH, BL, CH, CL, DH and DL and are
useful for handling characters and other 8-bit data.
:image depth='7.0i' file='386ARCH1' text='386 Registers'.
.np
All general-purpose registers can be used for addressing calculations
and for results of most arithmetic and logical calculations.
However, each register has specific functions as well.
Instructions that use specific registers include double-precision
multiply and divide, i/o, string instructions, translate, loop,
variable shift and rotate, and stack operations.
.*
.beglevel
.*
.section The EAX Register
.*
.np
.ix 'registers' 'EAX'
.ix 'EAX register'
The EAX register is known as the accumulator.
Some arithmetic operations implicitly use this register.
For example, the multiply and divide instructions require the use of
the EAX register.
The input/output instructions also use this register for transferring
data from input/output devices to the 386 processor.
.*
.section The EBX Register
.*
.np
.ix 'registers' 'EBX'
.ix 'EBX register'
The EBX register (along with the DS segment register) is used to point
to the translate table for the XLAT instruction.
.*
.section The ECX Register
.*
.np
.ix 'registers' 'ECX'
.ix 'ECX register'
The ECX register is known as the count register.
In addition to being a general-purpose register, the ECX register is
used as an iteration counter for the loop (LOOP, LOOPE/LOOPZ,
LOOPNE/LOOPNZ, JECXZ) and the string (CMPSB/CMPSW/CMPSD,
LODSB/LODSW/LODSD, MOVSB/MOVSW/MOVSD, SCASB/SCASW/SCASD,
STOSB/STOSW/STOSD) instructions.
.*
.section The EDX Register
.*
.np
.ix 'registers' 'EDX'
.ix 'EDX register'
The EDX register is known as the data register.
It is used implicitly by the multiply and divide instructions.
The DX register (low-order 16 bits of the EDX register) can also be
used for the port address for the IN and OUT instructions.
.*
.section The ESP and EBP Registers
.*
.np
.ix 'registers' 'ESP'
.ix 'ESP register'
.ix 'registers' 'EBP'
.ix 'EBP register'
These registers use the stack segment (SS) during memory address
calculation.
Hence they can be used to access data which resides in the stack
segment.
The ESP register allows the implementation of a stack in memory and
hence it is referred to as the stack pointer.
Instructions such as PUSH and POP operate implicitly on this register.
The base pointer (EBP) can also be used to access data in the stack
segment.
This register is typically used to reference parameters and local
variables on the stack.
.*
.section The ESI and EDI Registers
.*
.np
.ix 'registers' 'ESI'
.ix 'ESI register'
.ix 'registers' 'EDI'
.ix 'EDI register'
The primary function of these registers is found in the string
instructions.
The data segment register (DS) in conjunction with the ESI register
form the source address of the string.
The extra segment register (ES) in conjunction with the EDI register
form the destination address for the string move.
.*
.endlevel
.*
.section Segment Registers
.*
.np
.ix 'registers' 'CS'
.ix 'registers' 'DS'
.ix 'registers' 'ES'
.ix 'registers' 'SS'
.ix 'registers' 'FS'
.ix 'registers' 'GS'
.ix 'CS register'
.ix 'DS register'
.ix 'ES register'
.ix 'SS register'
.ix 'FS register'
.ix 'GS register'
The 6 segment registers are named CS, DS, ES, SS, FS and GS.
Each segment register defines an address space of up to 4 gigabytes
(4GB).
Each instruction references a default segment register.
In most cases, these defaults can be overridden by specifying a
segment register with the operand.
:image depth='3.5i' file='386ARCH2' text='Segment Registers'.
.*
.beglevel
.*
.section The CS Register
.*
.np
.ix 'registers' 'CS'
.ix 'CS register'
The CS register is known as the code segment register.
The code segment register is used in conjunction with the
instruction pointer to determine the address of the instruction
to be executed.
.*
.section The DS Register
.*
.np
.ix 'registers' 'DS'
.ix 'DS register'
The DS register is known as the data segment register.
References
to data are relative to this register.
.*
.section The ES, FS and GS Registers
.*
.np
.ix 'registers' 'ES'
.ix 'ES register'
.ix 'registers' 'FS'
.ix 'FS register'
.ix 'registers' 'GS'
.ix 'GS register'
The ES, FS and GS registers are additional data segment registers.
The ES register is used in memory address calculations for the
destination of string operations.
.*
.section The SS Register
.*
.np
.ix 'registers' 'SS'
.ix 'SS register'
The SS register is known as the stack segment register.
The SS register is used in memory address calculations involving
the pointer registers EBP and ESP.
.*
.endlevel
.*
.section Instruction Pointer
.*
.np
.ix 'registers' 'EIP'
.ix 'EIP register'
The instruction pointer (EIP) is a 32-bit register which
defines the offset in the address space defined by the CS segment
register of the next instruction to be executed.
.*
.section Flags Register
.*
.np
.ix 'flags'
The flags register is a 32-bit register named EFLAGS which contains a
number of status bits.
This register is sometimes referred to as the status register.
Bits in this register are numbered from 0 to 31, where 0 is the least
significant bit and 31 is the most significant bit.
On the 386 only bits 0, 2, 4, 6, 7, 8, 9, 10, 11, 12, 13, 14, 16 and
17 are defined.
The remaining bits are reserved.
The low-order 16 bits of EFLAGS is called FLAGS and can be treated as
a unit.
This is particularly useful when executing 8086 and 286 code since
this part of EFLAGS is identical to the FLAGS register of the 8086 and
286.
.np
Not all instructions change the state of the flags register.
You must look up the description of a particular instruction to see
what effect it might have, if any, on the flags register.
:image depth='1.5i' file='386ARCH3' text='Flags Register'.
.synote compact
.note VM
= Virtual 8086 Mode
.note RF
= Resume Flag
.note NT
= Nested Task Flag
.note IOPL
= I/O Privilege Level
.note OF
= Overflow Flag
.note DF
= Direction Flag
.note IF
= Interrupt Flag
.note TF
= Trace Flag
.note SF
= Sign Flag
.note ZF
= Zero Flag
.note AF
= Auxiliary Carry Flag
.note PF
= Parity Flag
.note CF
= Carry Flag
.esynote
.*
.beglevel
.*
.section The Virtual 8086 Mode
.*
.np
.ix 'flags' 'virtual 8086 mode'
.ix 'virtual 8086 mode'
This flag, when set, indicates that the task is executing an 8086
program.
.*
.section The Resume Flag
.*
.np
.ix 'flags' 'resume'
.ix 'resume flag'
This flag temporarily disables debug exceptions so that an instruction
can be restarted after a debug exception without immediately causing
another debug exception.
.*
.section The Nested Task Flag
.*
.np
.ix 'flags' 'nested task'
.ix 'nested task flag'
The processor uses this flag to control chaining of interrupted and called
tasks.
.*
.section The I/O Privilege Level
.*
.np
.ix 'flags' 'i/o privilege level'
.ix 'i/o privilege level'
This is used to restrict the use of input/output instructions.
.*
.section The Carry Flag
.*
.np
.ix 'flags' 'carry'
.ix 'carry flag'
The Carry flag is bit 0 of the status register and is referred to as
CF.
CF reflects a carry out of the high bit following an arithmetic
operation.
CF is set to 1 (TRUE) when there is a carry.
CF is set to 0 (FALSE) when there is no carry.
.*
.section The Parity Flag
.*
.np
.ix 'flags' 'parity'
.ix 'parity flag'
The Parity flag is bit 2 of the flags register and is referred to
as PF.
PF is set to 1 (TRUE) when the resulting lower 8 bits of a data
operation has an even number of 1 bits.
An odd number of 1 bits results in the PF flag being set to 0 (FALSE).
.*
.section The Auxiliary Carry Flag
.*
.np
.ix 'flags' 'auxiliary carry'
.ix 'auxiliary carry flag'
The Auxiliary carry flag is bit 4 of the flags register and is
referred to as AF.
The AF flag represents a 4-bit (half-byte or nibble) carry, which is
a carry out of bit 3 in an 8-bit data item.
AF is set to 1 (TRUE) when there is a carry.
AF is set to 0 (FALSE) when there is no carry.
.*
.section The Zero Flag
.*
.np
.ix 'flags' 'zero'
.ix 'zero flag'
The Zero flag is bit 6 of the flags register and is referred to as ZF.
ZF is set to 1 (TRUE) when the result of the computation is zero.
ZF is set to 0 (FALSE) when the result of the computation is not
zero.
.*
.section The Sign Flag
.*
.np
.ix 'flags' 'sign'
.ix 'sign flag'
The Sign flag is bit 7 of the flags register and is referred
to as SF.
SF is set equal to the high-order bit of the result of a computation.
SF is set to 1 (TRUE) if the result is negative.
SF is set to 0 (FALSE) if the result is not negative.
.*
.section The Trap Flag
.*
.np
.ix 'flags' 'trap'
.ix 'trap flag'
The Trap flag is bit 8 of the flags register and is referred to as TF.
When set, TF causes the 386 microprocessor to "single step".
Each instruction that is executed when TF is set results in a
software interrupt being generated.
This is useful in debugging when a trace of the instructions
executed is desired.
.*
.section The Interrupt Enable Flag
.*
.np
.ix 'flags' 'interrupt enable'
.ix 'interrupt enable flag'
The Interrupt Enable flag is bit 9 of the flags register and
is referred to as IF.
IF determines whether interrupt processing is enabled or disabled.
When IF is set to 1 (TRUE) interrupts are enabled; when IF is 0
(FALSE) interrupts are disabled.
The setting of IF can be controlled using the CLI and STI
instructions of the 386.
.*
.section The Direction Flag
.*
.np
.ix 'flags' 'direction'
.ix 'direction flag'
The Direction flag is bit 10 of the flags register and is
referred to as DF.
DF determines whether string operations increment or decrement their
operands.
When DF is 1 (TRUE) the operands will be decremented; when DF is 0
(FALSE) the operands will be incremented.
The setting of DF is controlled by the CLD and STD instructions.
.*
.section The Overflow Flag
.*
.np
.ix 'flags' 'overflow'
.ix 'overflow flag'
The Overflow flag is bit 11 of the flags register and is referred to
as OF.
OF represents a magnitude overflow in signed binary arithmetic.
OF is set to 1 (TRUE) when the result of a 16-bit computation exceeds
32767 or the result of a 32-bit computation exceeds 2147483647
(which means that the result is now negative)
or is less than -32768 in the 16-bit case or -2147483648 in the 32-bit
case (which means that the result is now positive).
OF is set to 0 (FALSE) when the result of the computation did not
overflow.
.*
.endlevel
.*
.section Effective Address Calculation
.*
.np
.ix 'effective address'
Effective address calculation consists of instruction fetching
and data fetching.
Instruction fetching describes how flow of control is performed.
Data fetching describes how data can be referenced by a program.
.*
.beglevel
.*
.section Instruction Fetching
.*
.np
.ix 'instruction fetching'
.ix 'IP register'
The code segment register (CS) defines the instruction space from
which instructions can be fetched.
The offset of the next instruction to be fetched from the code
segment is contained in the instruction pointer register (EIP).
After instruction fetch, the EIP register is incremented by the
size of the instruction.
A jump or call instruction can cause the EIP to change in three
additional ways.
.begpoint
.point Instruction Pointer Relative
.ix 'instruction fetch' 'EIP relative'
.ix 'EIP relative instruction fetch'
The instruction contains an 8 or 32-bit displacement which is
added to the EIP as a signed offset.
The next instruction to be fetched is found at this new address.
This is referred to as an intrasegment operation since it does
not result in the segment registers being changed.
.point Direct Addressing
.ix 'addressing' 'direct'
.ix 'direct addressing'
The instruction contains a 16-bit value for the segment and a 32-bit value
for the offset which are loaded into
the CS and the EIP register respectively.
This causes an absolute jump or call to any location in the 386
memory.
Since this operation results in a change to the segment register
as well as the EIP, it is referred to as an intersegment
operation.
.point Indirect Addressing
.ix 'addressing' 'indirect'
.ix 'indirect addressing'
The memory address referenced by the instruction is either a single
32-bit value or a 32-bit value and a 16-bit value.
If a single 32-bit value exists, it is used to replace the value
in the EIP.
If a 16-bit value is also specified, it replaces the CS register.
.endpoint
.*
.section Data Fetching
.*
.np
The following sections describe the addressing modes available on the
386 processor.
.*
.beglevel
.*
.section Direct Addressing
.*
.np
.ix 'addressing' 'direct'
.ix 'direct addressing'
Direct addressing is the simplest addressing mode on the
386.
The 16 or 32 bits following the operation code byte define an
offset which when combined with the data segment register (DS)
defines a memory address.
.*
.section Immediate Addressing
.*
.np
.ix 'addressing' 'immediate'
.ix 'immediate addressing'
Certain instructions can use data from the instruction itself.
These operands are called immediate operands.
Immediate operands may be 8, 16 or 32 bits.
.*
.section Base Addressing
.*
.np
.ix 'addressing' 'base'
The offset of the operand is specified indirectly in one of the
general registers.
When ESP or EBP is specified as the base register, segment register
SS is assumed, otherwise segment register DS is assumed.
.*
.section Base + Displacement Addressing
.*
.np
.ix 'addressing' 'base + displacement'
This addressing mode consists of a register called the base and a
displacement.
The base register and displacement can be combined in two ways.
.autopoint
.point
The displacement defines the offset of the beginning of an array.
The base register contains the result of a computation which is the
offset into the array of a particular element.
.point
The displacement locates an item within a record and the base register
points to a particular record.
.endpoint
.*
.section (Index * Scale) + Displacement Addressing
.*
.np
.ix 'addressing' 'index * scale + displacement'
This addressing mode consists of an index register, a scale factor and
a displacement.
The index register can be any of the general registers except ESP.
This allows efficient indexing into a static array.
The displacement defines the start of the array, the index register
contains the subscript value, and the scale represents the size of an
element in the array.
The value of the scale factor can be 1, 2, 4 or 8.
.*
.section Base + Index + Displacement Addressing
.*
.np
.ix 'addressing' 'base + index + displacement'
This addressing mode consists of a base register, an index register, and
a displacement.
This combination provides the following functions.
.autopoint
.point
The two registers support a 2-dimensional array with the displacement
defining the start of the array.
.point
The two registers identify a particular record in an array of records
with the displacement pointing to an item in the record.
.endpoint
.*
.section Base + (Index * Scale) + Displacement Addressing
.*
.np
.ix 'addressing' 'base + (index * scale) + displacement'
This addressing mode provides an efficient indexing of 2-dimensional arrays
when the size of an array element is 1, 2, 4 or 8 bytes.
.*
.endlevel
.*
.endlevel
