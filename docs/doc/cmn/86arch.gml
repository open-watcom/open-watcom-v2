.chap 8086 Architecture
.*
.section Registers
.*
.np
.ix 'registers'
The register model for the 8086 microprocessor consists of 4
general purpose registers, 2 index registers, 2 pointer
registers, 4 segment registers, an instruction pointer and a
flags register.
Each of the registers is 16 bits wide.
.*
.section General Registers
.*
.np
.ix 'general registers'
The four general purpose 16-bit registers are named AX, BX, CX, and DX.
These registers can be used as 16-bit accumulators or they can each
be divided into 2 8-bit registers.
The eight-bit registers are named AH, AL, BH, BL, CH, CL, DH and DL.
:image depth='2.0i' file='86ARCH1' text='8086 Registers'.
.np
The general purpose registers can all be used as operands in the
arithmetic and logical instructions.
However, each register has specific functions as well.
.*
.beglevel
.*
.section The AX Register
.*
.np
.ix 'registers' 'AX'
.ix 'AX register'
The AX register is known as the accumulator.
Some arithmetic operations implicitly use this register.
For example, the multiply and divide instructions require the use
of the AX register.
The input/output instructions also use this register for
transferring data from input/output devices to the 8086
processor.
.*
.section The BX Register
.*
.np
.ix 'registers' 'BX'
.ix 'BX register'
The BX register is known as the base register.
The base register is the only general purpose register which can
be used in an address calculation.
The BX register can serve as the base address for any items in
the data segment.
.*
.section The CX Register
.*
.np
.ix 'registers' 'CX'
.ix 'CX register'
The CX register is known as the count register.
In addition to being a general purpose register, the CX register
is used as an iteration counter for the loop (LOOP, LOOPE/LOOPZ,
LOOPNE/LOOPNZ, JCXZ) and the string (CMPSB/CMPSW, LODSB/LODSW,
MOVB/MOVW, SCASB/SCASW, STOSB/STOSW) instructions.
.*
.section The DX Register
.*
.np
.ix 'registers' 'DX'
.ix 'DX register'
The DX register is known as the data register.
It is used implicitly by the word multiply and divide instructions.
This register is also used to specify the port address for the
input/output instructions.
Whenever an input/output port address is in the range 256 to 65535
the DX register must be used.
It is the only register that can perform this function.
.*
.endlevel
.*
.section Pointer Registers
.*
.np
.ix 'registers' 'SP'
.ix 'SP register'
.ix 'registers' 'BP'
.ix 'BP register'
The pointer registers are named SP and BP.
These registers use the stack segment (SS) during memory address
calculation.
Hence they can be used to access data which resides in the stack
segment.
The SP register allows the implementation of a stack in memory
and hence it is referred to as the stack pointer.
Instructions such as PUSH and POP operate implicitly on this register.
The base pointer (BP) can also be used to access data in the
stack segment.
This register is typically used to reference parameters and local
variables on the stack.
:image depth='1.1i' file='86ARCH2' text='SP/BP Registers'.
.*
.section Index Registers
.*
.np
.ix 'registers' 'SI'
.ix 'SI register'
.ix 'registers' 'DI'
.ix 'DI register'
The 8086 microprocessor has two index registers which can be
used to reference data in the data segment.
These two registers are the SI and DI registers.
Each of these registers is 16 bits wide and like the general
purpose registers can be used in arithmetic and logical
operations.
However, the primary function of these registers is found in the
string instructions.
The data segment register (DS) in conjunction with the SI
register form the source address of the string.
The extra segment register (ES) in conjunction with the DI
register form the destination address for the string move.
:image depth='1.1i' file='86ARCH3' text='SI/DI Index Registers'.
.*
.section Instruction Pointer
.*
.np
.ix 'registers' 'IP'
.ix 'IP register'
The instruction pointer (IP) is a 16-bit register which is
commonly referred to as the program counter.
The instruction pointer defines the offset in the address space
defined by the CS register of the next instruction to be
executed.
.*
.section Segment Registers
.*
.np
Physical memory addresses on the 8086 are 20 bits wide, hence we
require more than a 16-bit register to determine a unique
physical address.
The 8086 uses one of the four segment registers in all memory
address calculations to resolve this problem.
Each of the segment registers is 16 bits wide.
A physical address is determined by a 16-bit segment register and
a 16-bit offset as follows.
The contents of the segment register are multiplied by 16, then
the offset is added to form the physical address.
.np
.ix 'registers' 'CS'
.ix 'registers' 'DS'
.ix 'registers' 'ES'
.ix 'registers' 'SS'
.ix 'CS register'
.ix 'DS register'
.ix 'ES register'
.ix 'SS register'
The four segment registers are named CS, DS, ES, and SS.
Each segment register defines an address space of 65536 bytes
(64K).
Each instruction references a default segment register.
In most cases, these defaults can be overridden by specifying a
segment register with the operand.
:image depth='2.05i' file='86ARCH4' text='Segment Registers'.
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
.section The ES Register
.*
.np
.ix 'registers' 'ES'
.ix 'ES register'
The ES register is known as the extra segment register.
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
the pointer registers BP and SP.
.*
.endlevel
.*
.section Flags Register
.*
.np
.ix 'flags'
The flags register is a 16-bit register which contains a number
of status bits.
This register is sometimes referred to as the status register.
Bits in this register are numbered from 0 to 15, where 0 is the
least significant bit and 15 is the most significant bit.
On the 8086 only bits 0, 2, 4, 6, 7, 8, 9, 10, and 11 are
defined.
The remaining bits are reserved and are normally set to zero.
.np
Not all instructions change the state of the flags register.
You must look up the description of a particular instruction to
see what effect it might have, if any, on the flags register.
:image depth='2.73i' file='86ARCH5' text='Flags Register'.
.*
.beglevel
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
When set, TF causes the 8086 microprocessor to "single step".
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
instructions of the 8086.
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
OF is set to 1 (TRUE) when the result of the computation exceeds 32767
(which means that the result is now negative) or is less than -32768
(which means that the result is now positive).
OF is set to 0 (FALSE) when the result of the computation did not
overflow.
.*
.endlevel
.*
.section Addressing Modes
.*
.np
.ix 'addressing modes'
All 8086 memory addresses are 20 bits wide, this allows an 8086
microprocessor to address a maximum of 1 Megabyte of memory.
The construction of a memory address consists of two parts,
effective address calculation and memory address calculation.
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
segment is contained in the instruction pointer register (IP).
After instruction fetch, the IP register is incremented by the
size of the instruction.
A jump or call instruction can cause the IP to change in three
additional ways.
.begnote
.point Instruction Pointer Relative
.ix 'instruction fetch' 'IP relative'
.ix 'IP relative instruction fetch'
The instruction contains an 8 or 16-bit displacement which is
added to the IP as a signed offset.
The next instruction to be fetched is found at this new address.
This is referred to as an intrasegment operation since it does
not result in the segment registers being changed.
.point Direct Addressing
.ix 'addressing' 'direct'
.ix 'direct addressing'
The instruction contains two 16-bit values which are loaded into
the IP and the CS register.
This causes an absolute jump or call to any location in the 8086
memory.
Since this operation results in a change to the segment register
as well as the IP, it is referred to as an intersegment
operation.
.point Indirect Addressing
.ix 'addressing' 'indirect'
.ix 'indirect addressing'
The memory address referenced by the instruction is either one or
two 16-bit values.
If a single 16-bit value exists, it is used to replace the value
in the IP.
If two 16-bit values exist, the first value replaces the IP and
the second replaces the CS register.
.endpoint
.*
.section Data Fetching
.*
.ix 'addressing modes' 'data'
.np
The 8086 has five data addressing modes.
These addressing modes are Base, Direct, Immediate, Indexed and
Base Indexed.
.*
.beglevel
.*
.section Base Addressing
.*
.np
.ix 'addressing' 'base'
.ix 'base addressing'
Base addressing uses the BX or BP register as a base register
for the address calculation.
The choice of which base register to use depends on which segment
register you wish to have used in the calculation.
The BX base register uses the DS segment register and the BP base
register uses the SS segment register.
The contents of the base register along with the segment register
identify a unique memory location.
Base addressing allows an 8 or 16-bit displacement to be
specified along with the base register.
If a displacement is specified, it is added to the contents of
the base register to determine the offset past the segment
register during address calculation.
If the displacement is 8 bits, it is sign extended to 16 bits
during the address calculation phase.
.*
.section Direct Addressing
.*
.np
.ix 'addressing' 'direct'
.ix 'direct addressing'
Direct addressing is the simplest addressing mode on the 8086.
The two bytes following the operation code byte contain an offset
which when combined with the data segment register (DS) define a
memory address.
.*
.section Immediate Addressing
.*
.np
.ix 'addressing' 'immediate'
.ix 'immediate addressing'
Immediate addressing implies that the data can be found in the
bytes following the instruction code in the instruction space
defined by the CS segment register.
This addressing mode is usually employed with program constants.
Depending on the instruction, the immediate value may be 8 or 16 bits
long.
If the constant is 16 bits long, the low-order 8 bits are stored
before the high-order 8 bits as is the case with all 16-bit data
entities on the 8086.
.*
.section Indexed Addressing
.*
.np
.ix 'addressing' 'indexed'
.ix 'indexed addressing'
Indexed addressing is performed by using the SI and DI index
registers.
The index registers can be used as pointers to data structures.
In this addressing mode, you can optionally specify an 8 or 16-bit
displacement which is added to the value in the index register to
determine the effective address.
If an 8-bit displacement is specified, the displacement is
converted to a 16-bit value by sign extension before the addition
is performed.
.*
.section Base Indexed Addressing
.*
.np
.ix 'addressing' 'base indexed'
.ix 'base indexed addressing'
Base indexed addressing is a combination of Base addressing and
Indexed addressing as described above.
The addressing mode allows a base register, an index register and
an optional displacement to be specified.
If BX is the base register, the default segment register used
during addressing is DS.
If BP is used, the default segment register is SS.
.*
.endlevel
.*
.endlevel
.keep 12
.*
.section Memory Address Calculation in Real Mode
.*
.np
.ix 'address calculation'
.ix 'real mode' 'address calculation'
Once a 16-bit effective address is computed, an 8086 memory address
is formed by summing the computed address with the contents of a
specified segment register.
.ix 'paragraph'
To form the 20-bit memory address, the 16-bit segment register is
shifted left 4 bit positions forming a 20-bit paragraph address.
A paragraph is defined to be a memory address which is a multiple
of sixteen.
The effective address, which was calculated as described above,
is then added to the 20-bit paragraph address forming a 20-bit
memory address.
Hence the 16-bit effective address is a byte offset from a
segment register.
.np
.ix 'address space'
The 4 segment registers on the 8086 allow four address spaces to
be selected at any time.
Since the effective address calculation determines a 16-bit
offset, each address space can be a maximum of 64K bytes.
The 8086 does not restrict the values which can be placed in a
segment register.
The segment registers define an origin of an address space which
may overlap with address spaces defined by other segment
registers.
.*
.if '&target' eq 'QNX' .do begin
.keep
.*
.section Memory Address Calculation in Protected Mode
.*
.np
.ix 'address calculation'
.ix 'protected mode' 'address calculation'
In protected mode, the 286, 386, etc. processors calculate the
effective address in a slightly different way than real mode.
In protected mode, the segment register value is simply an integer
value that is used to index a table in memory called the "Descriptor
Table".
This table contains entries called "descriptors".
Each entry contains the starting address of the corresponding segment.
The value in the descriptor is added to the offset to obtain the
effective address.
On the 286, the segment start address is 24 bits long.
Thus up to 16 megabytes (2**24) of memory can be addressed.
.np
The operating system is responsible for creating the descriptor table.
It will create one for each process in the system.
Some of the bits in the descriptor entry describe how memory
protection is set up.
The table will map out which areas of memory can be accessed by the
process, which are read/write, and which are read-only.
.do end
