.dm category begin
.begnote
.note Category:
&*
.endnote
.dm category end

.dm syntax begin
.begnote
.note Syntax:
.us on
.* Screwing with formatting is extremely bad idea for online help!
.if &e'&dohelp eq 0 .do begin
.fo off
.do end
.sk -1 c
.dm syntax end

.dm esyntax begin
.us off
.if &e'&dohelp eq 0 .do begin
.fo on
.do end
.endnote
.dm esyntax end

.dm descrp begin
.begnote
.note Description:
.dm descrp end

.dm edescrp begin
.endnote
.dm edescrp end

.dm notes begin
.begnote
.note Notes:
&*
.endnote
.dm notes end

.*
.chap *refid=wasaxp The &wasname.
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'assembler'
This chapter describes the &wasname..
It takes as input an assembler source file (a file with extension
"&axt") and produces, as output, an object file.
.np
The &wasname command line syntax is the following.
.ix '&wascmdup' 'command line format'
.ix 'command line format' '&wascmdup'
.if '&target' eq 'QNX' .do begin
.mbigbox
&wascmdup [options] asm_file [options] [@env_var]
.embigbox
.np
The square brackets [ ] denote items which are optional.
.begnote
.mnote &wascmd
is the name of the &wasname..
.mnote asm_file
is the filename specification of the assembler source file to be
assembled.
A default filename extension of ".a" is assumed when no extension is
specified.
A filename extension consists of that portion of a filename containing
the last "." and any characters which follow it.
.exam begin
File Specification              Extension
/home/john.doe/foo              (none)
/home/john.doe/foo.             .
/home/john.doe/foo.bar          .bar
/home/john.doe/foo.goo.bar      .bar
.exam end
.mnote options
is a list of valid &wasname options, each preceded by a dash
("&minus.").
Options may be specified in any order.
.endnote
.do end
.el .do begin
.mbigbox
&wascmdup [options] [d:][path]filename[.ext] [options] [@env_var]
.embigbox
.np
The square brackets [ ] denote items which are optional.
.begnote
.mnote &wascmdup
is the name of the &wasname..
.mnote d:
is an optional drive specification such as "A:", "B:", etc.
If not specified, the default drive is assumed.
.mnote path
is an optional path specification such as "\PROGRAMS\ASM\".
If not specified, the current directory is assumed.
.mnote filename
is the file name of the assembler source file to be assembled.
.mnote ext
is the file extension of the assembler source file to be assembled.
If omitted, a file extension of "&axt" is assumed.
If the period "." is specified but not the extension, the file is
assumed to have no file extension.
.mnote options
is a list of valid options, each preceded by a slash
("/") or a dash ("&minus.").
Options may be specified in any order.
.endnote
.do end
.np
The options supported by the &wasname. are:
.begnote $compact
.note d<name>[=text]
define text macro
.note e<number>
set error limit number
.note fo=<file_name>
set object file name
.note h or ?
display options summary
.note i=<directory>
add directory to list of include directories
.note o{e,c}
object file format (ELF,COFF)
.note q or zq
operate quietly
.note we
treat all warnings as errors
.endnote
.*
.section Using the &wasname
.*
.np
Assume that the following assembler program was stored in
.fi hello.asm.
.tinyexam begin
~.new_section .text, "crx4"
~.globl  main
main:
    lda           $sp,-0x10($sp)
    stq           $ra,($sp)
    ldah          $a0,h^`L$0`($zero)
    lda           $a0,l^`L$0`($a0)
    sextl         $a0,$a0
    bsr           $ra,j^printf
    ldq           $ra,($sp)
    lda           $sp,0x10($sp)
    ret           $zero,($ra),0x00000001

~.new_section .const, "drw4"
`L$0`:
    .asciiz "Hello world\n"

~.new_section .pdata, "dr2"
    //  0000                Procedure descriptor for main
    .long   main                    //  BeginAddress      : 0
    .long   main+36                 //  EndAddress        : 36
    .long   00000000                //  ExceptionHandler  : 0
    .long   00000000                //  HandlerData       : 0
    .long   main+8                  //  PrologEnd         : 8

~.new_section .drectve, "iRr0"
    .asciiz "-defaultlib:clib -defaultlib:math "
.tinyexam end
.np
We can assemble this file using the following command:
.millust begin
&wascmd hello
.millust end
.*
.section Assembly Directives and Opcodes
.*
.np
It is not the intention of this chapter to describe assembly-language
programming in any detail.
You should consult a book that deals with this topic.
However, we present an alphabetically ordered list of the directives
that are recognized by the assembler.
.*
.beglevel
.*
.section .aent (not supported)
.*
.section .align
.*
.syntax
    .align expression
.esyntax
.*
.descrp
The .align directive sets low-order bits in the location counter to
zero. The value of expression establishes the number of bits to be
zeroed. The maximum value for expression is 6 (64 byte alignment).
Windows NT default alignment is .align 4 (octaword alignment).
.np
If the .align directive advances the location counter, the assembler
fills the skipped bytes with zeros in data sections or nop instructions
in text sections.
.np
Normally, the
.kw .word
.ct ,
.kw .long
.ct ,
.kw .quad
.ct ,
.kw .float
.ct ,
.kw .double
.ct ,
.kw .d_floating
.ct ,
.kw .f_floating
.ct ,
.kw .g_floating
.ct ,
.kw .s_floating
.ct , and
.kw .t_floating
directives
automatically align their data appropriately. For example,
.mono .word
does an implicit
.mono .align 1,
and
.mono .double
does an implicit
.mono .align 3.
.np
The automatic alignment feature can be disabled with
.mono .align 0.
The assembler reinstates automatic alignment at the next
.kw .text
.ct ,
.kw .data
.ct ,
.kw .rdata
.ct , or
.kw .sdata
directive that it encounters.
.np
Labels immediately preceding an automatic or explicit alignment are
also realigned. For example:
.millust begin
foo: .align 3
~.word 0
.millust end
.pc
is the same as
.millust begin
~.align 3
foo: .word 0
.millust end
.edescrp
.category location control directive
.*
.section .ascii
.*
.syntax
    .ascii string[, string ...]
.esyntax
.*
.descrp
The
.kw .ascii
directive assembles each string from the list into
successive locations. The
.kw .ascii
directive does not pad the string with null characters. You must put
quotation marks (") around each string. You can optionally use the
backslash escape characters.
.edescrp
.category data storage
.*
.section .asciz
.*
.syntax
    .asciz string[, string ...]
.esyntax
.*
.descrp
The .asciz directive assembles each string in the list into successive
locations and adds a null character. You can optionally use the
backslash escape characters.
.edescrp
.category data storage
.notes non-Microsoft
.*
.section .asciiz
.*
.syntax
    .asciiz string[, string ...]
.esyntax
.*
.descrp
The .asciiz directive assembles each string in the list into successive
locations and adds a null character. You can optionally use the
backslash escape characters.
.edescrp
.category data storage
.*
.section .bss
.*
.syntax
    .bss
.esyntax
.*
.descrp
The .bss directive instructs the assembler to add subsequent code to
the .bss section.
.edescrp
.category location control directive
.*
.notes non-Microsoft
.*
.section .byte
.*
.syntax
    .byte expression1 [ , expression2 ] ... [ , expressionN ]
.esyntax
.*
.descrp
The .byte directive truncates the values of the expressions specified
in the comma-separated list to 8-bit values and assembles the values in
successive locations. The values of the expressions must be absolute.
The operands in a .byte directive can optionally have the form:
.illust begin
expressionVal [ :expressionRep]
.illust end
.np
The expressionVal is an 8-bit value. The optional expressionRep is a
non-negative expression that specifies how many times to replicate the
value of expressionVal. The expression value (expressionVal) and
repetition count (expressionRep) must be absolute.
.edescrp
.category data storage
.*
.section .comm
.*
.syntax
    .comm name, expression
.esyntax
.*
.descrp
The .comm directive causes name (unless defined elsewhere) to become a
global common symbol at the head of a block of at least expression
 bytes of storage. The linker overlays like-named common blocks, using
the expression value of the largest block as the byte size of the
overlay.
.edescrp
.category data storage
.*
.section .data
.*
.syntax
    .data
.esyntax
.*
.descrp
The .data directive directs the assembler to add all subsequent data to
the .data section.
.edescrp
.category location control directive
.*
.section .debug$P
.*
.notes non-Microsoft
.*
.section .debug$S
.*
.notes non-Microsoft
.*
.section .debug$T
.*
.notes non-Microsoft
.*
.section .d_floating (not supported)
.*
.section .double
.*
.syntax
    .double expression1 [, expression2] ... [, expressionN]
.esyntax
.*
.descrp
The .double directive is a synonym for .t_floating.
.edescrp
.category data storage
.*
.section .edata (not supported)
.*
.section .eflag (not supported)
.*
.section .end (not supported)
.*
.section .endr (not supported)
.*
.section .ent (not supported)
.*
.section .err
.*
.notes non-Microsoft
.*
.section .even
.*
.notes non-Microsoft
.*
.section .extern
.*
.syntax
    .extern         name
    .extern(thread) name
.esyntax
.*
.descrp
The .extern directive indicates that the specified symbol is global and
external; that is, the symbol is defined in another object module and
cannot be defined until link time. The name operand is a global
undefined symbol. Symbols declared as (thread) are declared to reside
in the .tls$ (thread local storage) section. Please refer to the .tls$
directive  description for an example of how to use symbols in thread
local storage.
.np
NOTE: This directive must be used prior to the first reference of name.
.edescrp
.category symbol declaration directive
.*
.section .f_floating (not supported)
.*
.section .file (not supported)
.*
.section .float
.*
.syntax
    .float expression1 [, expression2] ... [, expressionN]
.esyntax
.*
.descrp
The .float directive is a synonym for .s_floating.
.edescrp
.category data storage
.*
.section .fmask (not supported)
.*
.section fnop
.*
.syntax
    fnop
.esyntax
.*
.descrp
No Operation has no effect on the machine state.
.edescrp
.category instruction
.*
.section .frame (not supported)
.*
.section .g_floating (not supported)
.*
.section .globl
.*
.syntax
    .globl name
.esyntax
.*
.descrp
The .globl directive identifies name as an external symbol. If the name
is otherwise defined (for example, by its appearance as a label), the
assembler exports the symbol; otherwise, it imports the symbol. In
general, the assembler imports undefined symbols; that is, it gives
them the storage class "global undefined" and requires the linker to
resolve them.
.np
NOTE: This directive must be used prior to the first reference of name.
.edescrp
.category symbol declaration directives
.*
.section .half
.*
.notes non-Microsoft
.*
.section .ident
.*
.notes non-Microsoft
.*
.section .lcomm
.*
.syntax
    .lcomm name, expression
.esyntax
.*
.descrp
The .lcomm directive gives the named symbol (name) a data type of bss.
The assembler allocates the named symbol to the bss area, and the
expression defines the named symbol's length. If a .globl directive
also specifies name, the assembler allocates the named symbol to
external bss.
.edescrp
.category data storage
.*
.section .livereg (compiler-use only -- not supported)
.*
.section .loc (compiler-use only -- not supported)
.*
.section .long
.*
.syntax
    .long expression1 [, expression2] ... [, expressionN]
.esyntax
.*
.descrp
The .long directive truncates the values of the expressions specified
in the comma-separated list to 32-bit values and assembles the values
in successive locations. The values of the expression can be
relocatable.
.np
The operands can optionally have the form:
.illust begin
expressionVal [ :expressionRep]
.illust end
.np
The expressionVal is a 32-bit value. The optional expressionRep is a
non-negative expression that specifies how many times to replicate the
value of expressionVal. The expression value (expressionVal) and
repetition count (expressionRep) must be absolute.
.np
This directive automatically aligns its data and preceding labels to a
longword boundary. You can disable this feature with the .align 0
directive.
.edescrp
.category data storage
.*
.section .new_section
.*
.notes non-Microsoft
.*
.section nop
.*
.syntax
    nop
.esyntax
.*
.descrp
No Operation has no effect on the machine state.
.edescrp
.category instruction
.*
.section .pdata
.*
.notes non-Microsoft
.*
.section .prologue (not supported)
.*
.section .quad (not supported)
.*
.section .rdata
.*
.syntax
    .rdata
.esyntax
.*
.descrp
The .rdata directive instructs the assembler to add subsequent data
into the .rdata section.
.edescrp
.category location control directive
.*
.section .repeat (not supported)
.*
.section .save_ra (not supported)
.*
.section .sdata (not supported)
.*
.section .section
.*
.notes non-Microsoft
.*
.section .set
.*
.syntax
    .set option
.esyntax
.*
.descrp
The .set directive instructs the assembler to enable or disable certain
options. The assembler has the following default options:
.begpoint $compact
.point at,
.point macro,
.point move,
.point novolatile, and
.point reorder.
.endpoint
.pc
Only one option can be specified for each
.sy .set
directive:
.begbull

.bull
The
.kw at
option permits the assembler to use the
.kw $at
register for macros, but generates warnings if the source program uses
.kw $at
.ct .li .

.bull
When you use the
.kw noat
option and an assembler operation requires the
.kw $at
register, the assembler issues a warning message; however, the
.kw noat
option does permit source programs to use
.kw $at
without warnings being issued.

.bull
The
.kw macro
option permits the assembler to generate multiple machine instructions
from a single assembler instruction. This is the default action.

.bull
The
.kw nomacro
option causes the assembler to print a warning whenever an assembler
operation generates more than one machine language instruction. You
must select the
.kw noreorder
option before using the
.kw nomacro
option; otherwise, an error results.

.bull
The
.kw move
option cancels the effect of
.kw nomove
.ct .li .

.bull
The
.kw nomove
option instructs the assembler to mark each subsequent instruction so
that it cannot be moved during reorganization. The assembler can still
move instructions from below the nomove region to above the region or
vice versa. The
.kw nomove
option has part of the effect of the "volatile" C declaration; it
prevents otherwise independent loads or stores from occurring in a
different order than intended.

.bull
The
.kw reorder
option permits the assembler to reorder machine language instructions
to improve performance.

.bull
The
.kw noreorder
option prevents the assembler from reordering machine language
instructions. If a machine language instruction violates the hardware
pipeline constraints, the assembler issues a warning message.

.bull
The
.kw volatile
option instructs the assembler that subsequent load and store
instructions may not be moved in relation to each other or removed by
redundant load removal or other optimization. The
.kw volatile
option is less restrictive than
.kw noreorder
.'ct ; it allows the assembler to move other instructions (that is,
instructions other than load and store instructions) without
restrictions.

.bull
The
.kw novolatile
option cancels the effect of the
.kw volatile
option.

.endbull
.edescrp
.category assembler option directive
.*
.section .s_floating
.*
.syntax
    .s_floating expression1 [, expression2] ... [, expressionN]
.esyntax
.*
.descrp
The .s_floating directive initializes memory to single precision
(32-bit) IEEE floating-point numbers. The values of the expressions
must be absolute.
.np
The operands for the .s_floating directive can optionally have the
form:
.illust begin
expressionVal [ :expressionRep]
.illust end
.np
The expressionVal is a 32-bit value. The optional expressionRep is a
non-negative expression that specifies how many times to replicate the
value of expressionVal. The expression value (expressionVal) and
repetition count (expressionRep) must be absolute.
.np
This directive automatically aligns its data and preceding labels to a
longword boundary. This feature can be disabled with the .align 0
directive.
.edescrp
.category data storage
.*
.section .short
.*
.notes non-Microsoft
.*
.section .space
.*
.syntax
    .space expression
.esyntax
.*
.descrp
The .space directive advances the location counter by the number of
bytes specified by the value of expression. The assembler fills the
space with zeros.
.edescrp
.category location control directive
.*
.section .string
.*
.notes non-Microsoft
.*
.section .struct (not supported)
.*
.section symbolic equate
.*
.syntax
    name = expression
    name = register
.esyntax
.*
.descrp
The (symbolic equate) directive takes one of the following forms:
.illust begin
name = expression
name = register
.illust end
.np
You must define name only once in the assembly, and you cannot redefine
name. The expression must be computable when you assemble the program,
and the expression must involve only operators, constants, or equated
symbols. name can be used as a constant in any later statement.
.edescrp
.category symbol declaration directives
.*
.section .text
.*
.syntax
    .text
.esyntax
.*
.descrp
The .text directive instructs the assembler to add subsequent code to
the .text section. (This is the default.)
.edescrp
.category location control directive
.*
.section .t_floating
.*
.syntax
    .t_floating expression1 [, expression2] ... [, expressionN]
.esyntax
.*
.descrp
The .t_floating directive initializes memory to double precision
(64-bit) IEEE floating-point numbers. The values of the expressions
must be absolute.
.np
The operands can optionally have the form:
.illust begin
expressionVal [ :expressionRep]
.illust end
.np
The expressionVal is a 64-bit value. The optional expressionRep is a
non-negative expression that specifies how many times to replicate the
value of expressionVal. The expression value (expressionVal) and
repetition count (expressionRep) must be absolute.
.np
This directive automatically aligns its data and any preceding labels
to a quadword boundary. This feature can be disabled with the .align 0
directive.
.edescrp
.category data storage
.*
.section .tls$ (not supported)
.*
.section unop
.*
.syntax
    unop
.esyntax
.*
.descrp
Universal No Operation has no effect on the machine state.
.edescrp
.category instruction
.*
.section .value
.*
.notes non-Microsoft
.*
.section .version
.*
.notes non-Microsoft
.*
.section .verstamp (not supported)
.*
.section .weakref (not supported)
.*
.section .word
.*
.syntax
    .word expression1 [, expression2] ... [, expressionN]
.esyntax
.*
.descrp
The .word directive truncates the values of the expressions specified
in the comma-separated list to 16-bit values and assembles the values
in successive locations. The values of the expressions must be
absolute. The operands in the .word directive can optionally have the
form:
.illust begin
expressionVal [ :expressionRep]
.illust end
.np
The expressionVal is a 16-bit value. The optional expressionRep is a
non-negative expression that specifies how many times to replicate the
value of expressionVal. The expression value (expressionVal) and
repetition count (expressionRep) must be absolute.
.np
This directive automatically aligns its data and preceding labels on a
word boundary. This feature can be disabled with the .align 0
directive.
.edescrp
.category data storage
.*
.section .xdata
.*
.notes non-Microsoft
.*
.section .ydata
.*
.notes non-Microsoft
.*
.*
.endlevel
.*
.section Instructions
.*
.sr opnd1="$s_reg1, $s_reg2, $d_reg"
.sr opnd2="$d_reg/$s_reg1, $s_reg2"
.sr opnd3="$s_reg1, val_immed, $d_reg"
.sr opnd4="$d_reg/$s_reg1, val_immed"
.sr opnd5="$s_reg, $d_reg"
.sr opnd6="$d_reg/$s_reg"
.sr opnd7="val_immed, $d_reg"
.sr opnd8="$d_reg, address"
.sr opnd9="$s_reg, address"
.*
.beglevel
.*
.section Load Address (lda)
.syntax
    lda &opnd8
.esyntax
.descrp
Compute the virtual address (by adding the 16-bit offset to the base
register) and place it in the destination register.
.exam begin
lda     $sp,-0x10($sp)

ldah    $a0,h^`L$0`($zero)
lda     $a0,l^`L$0`($a0)
.exam end
.edescrp
.category Memory Integer Load/Store Instruction
.section Load Address High (ldah)
.syntax
    ldah $d_reg, offset($b_reg)
.esyntax
.descrp
Compute the virtual address (by adding 65536 times the 16-bit offset
to the base register) and place it in the destination register.
.exam begin
ldah    $a0,h^`L$0`($zero)
lda     $a0,l^`L$0`($a0)
.exam end
.edescrp
.category Memory Integer Load/Store Instruction
.section Load F_floating (ldf)
.syntax
    ldf &opnd8
.esyntax
.descrp
The specified 4-byte F_floating datum is loaded from memory into the
specified register.
The datum must be longword aligned.
.edescrp
.category Memory Format Floating-Point Instruction
.section Load G_floating (Load D_floating) (ldg)
.syntax
    ldg &opnd8
.esyntax
.descrp
The specified 8-byte G_floating (or D_floating) datum is loaded from
memory into the specified register.
The datum must be quadword aligned.
.edescrp
.category Memory Format Floating-Point Instruction
.section Load Sign Extended Longword (ldl)
.syntax
    ldl &opnd8
.esyntax
.descrp
The specified longword is loaded from memory, sign-extended and placed
into the specified register.
.edescrp
.category Memory Integer Load/Store Instruction
.section Load Sign Extended Longword Locked (ldl_l)
.syntax
    ldl_l &opnd8
.esyntax
.descrp
The specified longword is loaded from memory, sign-extended and placed
into the specified register.
The memory block containing the specifed longword is locked against
physical access until a conditional store is done.
The size of the block is 2**n where "n" is implementation dependent.
.np
The sequence LDL_L, modify, STL_C, BEQ xxx executed on a given
processor does an atomic read-modify-write of a datum in shared memory
if the branch falls through; if the branch is taken, the store did not
modify memory and the sequence may be repeated until it succeeds.
The recommended method for an atomic update of a single datum is:
.exam begin
try_again:
    ldl_l   $r1,x
    <modify $r1>
    stl_c   $r1
    beq     $r1,no_store
    .
    .
    .
no_store:
    <code to check for excessive iterations>
    br      try_again
.exam end
.edescrp
.category Memory Integer Load/Store Instruction
.section Load Quadword (ldq)
.syntax
    ldq &opnd8
.esyntax
.descrp
The specified quadword is loaded from memory and placed into the
specified register.
.edescrp
.category Memory Integer Load/Store Instruction
.section Load Quadword Locked (ldq_l)
.syntax
    ldq_l &opnd8
.esyntax
.descrp
The specified quadword is loaded from memory and placed into the
specified register.
The memory block containing the specifed quadword is locked against
physical access until a conditional store is done.
The size of the block is 2**n where "n" is implementation dependent.
.np
The sequence LDQ_L, modify, STQ_C, BEQ xxx executed on a given
processor does an atomic read-modify-write of a datum in shared memory
if the branch falls through; if the branch is taken, the store did not
modify memory and the sequence may be repeated until it succeeds.
The recommended method for an atomic update of a single datum is:
.exam begin
try_again:
    ldq_l   $r1,x
    <modify $r1>
    stq_c   $r1
    beq     $r1,no_store
    .
    .
    .
no_store:
    <code to check for excessive iterations>
    br      try_again
.exam end
.edescrp
.category Memory Integer Load/Store Instruction
.section Load Quadword Unaligned (ldq_u)
.syntax
    ldq_u &opnd8
.esyntax
.descrp
The specified quadword is loaded from memory and placed into the
specified register.
The memory address that is computed is truncated to a multiple of 8
(i.e., the 3 bottom bits of the effective address are cleared).
.edescrp
.category Memory Integer Load/Store Instruction
.section Load S_floating (Load Longword) (lds)
.syntax
    lds &opnd8
.esyntax
.descrp
The specified 4-byte S_floating (or Longword Integer) datum is loaded
from memory into the specified register.
The datum must be longword aligned.
.edescrp
.begnote
.note Notes:
Longword integers in floating registers are stored in bits
<63:62,58:29>, with bits <61:59> ignored and zeros in bits <28:0>.
.endnote
.category Memory Format Floating-Point Instruction
.section Load T_floating (Load Quadword) (ldt)
.syntax
    ldt &opnd8
.esyntax
.descrp
The specified 8-byte T_floating (or Quadword Integer) datum is loaded
from memory into the specified register.
The datum must be quadword aligned.
.edescrp
.category Memory Format Floating-Point Instruction
.section Store F_floating (stf)
.syntax
    stf &opnd9
.esyntax
.descrp
The 4-byte F_floating datum in the specified register is stored into
memory. The location must be longword aligned.
.edescrp
.category Memory Format Floating-Point Instruction
.section Store G_floating (Store D_floating) (stg)
.syntax
    stg &opnd9
.esyntax
.descrp
The 8-byte G_floating (or D_floating) datum in the specified register
is stored into memory.
The location must be quadword aligned.
.edescrp
.category Memory Format Floating-Point Instruction
.section Store Longword (stl)
.syntax
    stl &opnd9
.esyntax
.descrp
The longword in the specified register is stored into memory.
If the data is not naturally aligned, an alignment exception is
generated.
.edescrp
.category Memory Integer Load/Store Instruction
.section Store Longword Conditional (stl_c)
.syntax
    stl_c &opnd9
.esyntax
.descrp
The longword in the specified register is stored into memory provided
that the lock_flag is set (see ldl_l).
The lock_flag is returned in the same register.
.np
The memory block containing the specifed longword should have been
locked against physical access using the ldl_l instruction.
.np
The sequence LDL_L, modify, STL_C, BEQ xxx executed on a given
processor does an atomic read-modify-write of a datum in shared memory
if the branch falls through; if the branch is taken, the store did not
modify memory and the sequence may be repeated until it succeeds.
The recommended method for an atomic update of a single datum is:
.exam begin
try_again:
    ldl_l   $r1,x
    <modify $r1>
    stl_c   $r1
    beq     $r1,no_store
    .
    .
    .
no_store:
    <code to check for excessive iterations>
    br      try_again
.exam end
.edescrp
.category Memory Integer Load/Store Instruction
.section Store Quadword (stq)
.syntax
    stq &opnd9
.esyntax
.descrp
The quadword in the specified register is stored into memory.
If the data is not naturally aligned, an alignment exception is
generated.
.edescrp
.category Memory Integer Load/Store Instruction
.section Store Quadword Conditional (stq_c)
.syntax
    stq_c &opnd9
.esyntax
.descrp
The quadword in the specified register is stored into memory provided
that the lock_flag is set (see ldq_l).
The lock_flag is returned in the same register.
.np
The memory block containing the specifed quadword should have been
locked against physical access using the ldq_l instruction.
.np
The sequence LDQ_L, modify, STQ_C, BEQ xxx executed on a given
processor does an atomic read-modify-write of a datum in shared memory
if the branch falls through; if the branch is taken, the store did not
modify memory and the sequence may be repeated until it succeeds.
The recommended method for an atomic update of a single datum is:
.exam begin
try_again:
    ldq_l   $r1,x
    <modify $r1>
    stq_c   $r1
    beq     $r1,no_store
    .
    .
    .
no_store:
    <code to check for excessive iterations>
    br      try_again
.exam end
.edescrp
.category Memory Integer Load/Store Instruction
.section Store Quadword Unaligned (stq_u)
.syntax
    stq_u &opnd9
.esyntax
.descrp
The quadword in the specified register is stored into memory.
The memory address that is computed is truncated to a multiple of 8
(i.e., the 3 bottom bits of the effective address are cleared).
.edescrp
.category Memory Integer Load/Store Instruction
.section Store S_floating (Store Longword) (sts)
.syntax
    sts &opnd9
.esyntax
.descrp
The 4-byte S_floating (or Longword Integer) datum in the specified
register is stored into memory.
The location must be longword aligned.
.edescrp
.category Memory Format Floating-Point Instruction
.section Store T_floating (Store Quadword) (stt)
.syntax
    stt &opnd9
.esyntax
.descrp
The 8-byte T_floating (or Quadword Integer) datum in the specified
register is stored into memory.
The location must be quadword aligned.
.edescrp
.category Memory Format Floating-Point Instruction
.section Prefetch Data (fetch)
.syntax
    fetch address
.esyntax
.descrp
The specified address is used to designate an aligned 512-byte block
of data.
An implementation may optionally attempt to move all or part of this
block (or a larger surrounding block) of data to a faster-access part
of the memory hierarchy, in anticipation of subsequent Load or Store
instructions that access the data.
.edescrp
.category Memory Format Instruction with Function Code
.section Prefetch Data, Modify Intent (fetch_m)
.syntax
    fetch_m address
.esyntax
.descrp
The specified address is used to designate an aligned 512-byte block
of data.
An implementation may optionally attempt to move all or part of this
block (or a larger surrounding block) of data to a faster-access part
of the memory hierarchy, in anticipation of subsequent Load or Store
instructions that access the data.
.np
The fetch_m instruction differs from the fetch instruction in that
it gives the additional hint that modifications (stores) to some or
all of the data block are anticipated.
.edescrp
.category Memory Format Instruction with Function Code
.section Memory Barrier (mb)
.syntax
    mb
.esyntax
.descrp
Memory Barrier is used to serialize access to memory.
It is only required in multiprocessor systems.
In the absence of an mb instruction, loads and stores to different
physical locations are allowed to complete out of order on the issuing
processor as observed by other processors.
.edescrp
.category Memory Format Instruction with Function Code
.section Write Memory Barrier (wmb)
.syntax
    wmb
.esyntax
.descrp
Write Memory Barrier guarantees that all previous store instructions
access memory before any store instructions issued after the wmb
instruction.
.edescrp
.category Memory Format Instruction with Function Code
.section Read and Clear
.syntax
    rc  Ra.wq
.esyntax
.descrp

.edescrp
.category Memory Format Instruction with Function Code
.section Read Process Cycle Counter (rpcc)
.syntax
    rpcc address
.esyntax
.descrp
.edescrp
.category Memory Format Instruction with Function Code
.section Read and Set
.syntax
    rs
.esyntax
.descrp
.edescrp
.category Memory Format Instruction with Function Code
.section Trap Barrier (trapb)
.syntax
    trapb
.esyntax
.descrp
.edescrp
.category Memory Format Instruction with Function Code
.section Exception Barrier (excb)
.syntax
    excb
.esyntax
.descrp
.edescrp
.category Memory Format Instruction with Function Code

:CMT.  Memory Branch Instructions

.section Jump (jmp)
.syntax
    jmp     $d_reg, ($s_reg), jhint
    jmp     $d_reg, ($s_reg)
    jmp     ($s_reg), jhint
    jmp     ($s_reg)
    jmp     ($s_reg), jhint
    jmp     &opnd8
    jmp     address
.esyntax
.descrp
.edescrp
.category Memory Branch Instruction
.section Jump to Subroutine (jsr)
.syntax
    jsr     $d_reg, ($s_reg), jhint
    jsr     $d_reg, ($s_reg)
    jsr     ($s_reg), jhint
    jsr     ($s_reg)
    jsr     ($s_reg), jhint
    jsr     &opnd8
    jsr     address
.esyntax
.descrp
.edescrp
.category Memory Branch Instruction
.section Jump to Subroutine Return (jsr_coroutine)
.syntax
    jsr_coroutine   $d_reg, ($s_reg), rhint
    jsr_coroutine   $d_reg, ($s_reg)
    jsr_coroutine   $d_reg, rhint
    jsr_coroutine   $d_reg
    jsr_coroutine   ($s_reg), rhint
    jsr_coroutine   rhint
    jsr_coroutine
.esyntax
.descrp
.edescrp
.category Memory Branch Instruction
.section Return from Subroutine (ret)
.syntax
    ret     $d_reg, ($s_reg), rhint
    ret     $d_reg, ($s_reg)
    ret     $d_reg, rhint
    ret     $d_reg
    ret     ($s_reg), rhint
    ret     rhint
    ret
.esyntax
.descrp
.edescrp
.category Memory Branch Instruction

:CMT.  Branch Format Instructions

.section Branch (br)
.syntax
    br $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch Equal to Zero (fbeq)
.syntax
    fbeq $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch Less Than Zero (fblt)
.syntax
    fblt $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch Less Than or Equal to Zero (fble)
.syntax
    fble $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch to Subroutine (bsr)
.syntax
    bsr $d_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch Not Equal to Zero (fbne)
.syntax
    fbne $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch Greater Than or Equal to Zero (fbge)
.syntax
    fbge $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch Greater Than Zero (fbgt)
.syntax
    fbgt $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch if Low Bit is Clear (blbc)
.syntax
    blbc $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch if Equal to Zero (beq)
.syntax
    beq $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch if Less Than Zero (blt)
.syntax
    blt $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch if Less Than or Equal to Zero (ble)
.syntax
    ble $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch if Low Bit is Set (blbs)
.syntax
    blbs $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch if Not Equal to Zero (bne)
.syntax
    bne $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch if Greater Than or Equal to Zero (bge)
.syntax
    bge $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction
.section Branch if Greater Than Zero (bgt)
.syntax
    bgt $s_reg, label
.esyntax
.descrp
.edescrp
.category Branch Format Instruction

:CMT.  Operate Format Instruction

.section Add Longword (without overflow) (addl)
.syntax
    addl    &opnd1
    addl    &opnd2
    addl    &opnd3
    addl    &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Add Quadword (without overflow) (addq)
.syntax
    addq    &opnd1
    addq    &opnd2
    addq    &opnd3
    addq    &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Compare Byte (cmpbge)
.syntax
    cmpbge  &opnd1
    cmpbge  &opnd2
    cmpbge  &opnd3
    cmpbge  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Compare Signed Quadword Equal (cmpeq)
.syntax
    cmpeq   &opnd1
    cmpeq   &opnd2
    cmpeq   &opnd3
    cmpeq   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Compare Signed Quadword Less Than or Equal (cmple)
.syntax
    cmple   &opnd1
    cmple   &opnd2
    cmple   &opnd3
    cmple   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Compare Signed Quadword Less Than (cmplt)
.syntax
    cmplt   &opnd1
    cmplt   &opnd2
    cmplt   &opnd3
    cmplt   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Compare Unsigned Quadword Less Than or Equal (cmpule)
.syntax
    cmpule  &opnd1
    cmpule  &opnd2
    cmpule  &opnd3
    cmpule  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Compare Unsigned Quadword Less Than (cmpult)
.syntax
    cmpult  &opnd1
    cmpult  &opnd2
    cmpult  &opnd3
    cmpult  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Subtract Longword (without overflow) (subl)
.syntax
    subl    &opnd1
    subl    &opnd2
    subl    &opnd3
    subl    &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Subtract Quadword (without overflow) (subq)
.syntax
    subq    &opnd1
    subq    &opnd2
    subq    &opnd3
    subq    &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Scaled Longword Add by 4 (s4addl)
.syntax
    s4addl  &opnd1
    s4addl  &opnd2
    s4addl  &opnd3
    s4addl  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Scaled Quadword Add by 4 (s4addq)
.syntax
    s4addq  &opnd1
    s4addq  &opnd2
    s4addq  &opnd3
    s4addq  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Scaled Longword Subtract by 4 (s4subl)
.syntax
    s4subl  &opnd1
    s4subl  &opnd2
    s4subl  &opnd3
    s4subl  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Scaled Quadword Subtract by 4 (s4subq)
.syntax
    s4subq  &opnd1
    s4subq  &opnd2
    s4subq  &opnd3
    s4subq  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Scaled Longword Add by 8 (s8addl)
.syntax
    s8addl  &opnd1
    s8addl  &opnd2
    s8addl  &opnd3
    s8addl  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Scaled Quadword Add by 8 (s8addq)
.syntax
    s8addq  &opnd1
    s8addq  &opnd2
    s8addq  &opnd3
    s8addq  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Scaled Longword Subtract by 8 (s8subl)
.syntax
    s8subl  &opnd1
    s8subl  &opnd2
    s8subl  &opnd3
    s8subl  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Scaled Quadword Subtract by 8 (s8subq)
.syntax
    s8subq  &opnd1
    s8subq  &opnd2
    s8subq  &opnd3
    s8subq  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Logical Product (AND) (and)
.syntax
    and     &opnd5
    and     &opnd6
    and     &opnd7
    and     &opnd1
    and     &opnd2
    and     &opnd3
    and     &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Logical Product with Complement (ANDNOT) (bic)
.syntax
    bic     &opnd5
    bic     &opnd6
    bic     &opnd7
    bic     &opnd1
    bic     &opnd2
    bic     &opnd3
    bic     &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Logical Sum (OR) (bis)
.syntax
    bis     &opnd5
    bis     &opnd6
    bis     &opnd7
    bis     &opnd1
    bis     &opnd2
    bis     &opnd3
    bis     &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Move if Equal to Zero (cmoveq)
.syntax
    cmoveq  &opnd1
    cmoveq  &opnd2
    cmoveq  &opnd3
    cmoveq  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Move if Low Bit Clear (cmovlbc)
.syntax
    cmovlbc &opnd1
    cmovlbc &opnd2
    cmovlbc &opnd3
    cmovlbc &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Move if Low Bit Set (cmovlbs)
.syntax
    cmovlbs &opnd1
    cmovlbs &opnd2
    cmovlbs &opnd3
    cmovlbs &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Move if Greater Than or Equal to Zero (cmovge)
.syntax
    cmovge  &opnd1
    cmovge  &opnd2
    cmovge  &opnd3
    cmovge  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Move if Greater Than Zero (cmovgt)
.syntax
    cmovgt  &opnd1
    cmovgt  &opnd2
    cmovgt  &opnd3
    cmovgt  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Move if Less Than or Equal to Zero (cmovle)
.syntax
    cmovle  &opnd1
    cmovle  &opnd2
    cmovle  &opnd3
    cmovle  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Move if Less Than Zero (cmovlt)
.syntax
    cmovlt  &opnd1
    cmovlt  &opnd2
    cmovlt  &opnd3
    cmovlt  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Move if Not Equal to Zero (cmovne)
.syntax
    cmovne  &opnd1
    cmovne  &opnd2
    cmovne  &opnd3
    cmovne  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Logical Equivalence (XORNOT) (eqv)
.syntax
    eqv     &opnd5
    eqv     &opnd6
    eqv     &opnd7
    eqv     &opnd1
    eqv     &opnd2
    eqv     &opnd3
    eqv     &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Logical Sum with Complement (ORNOT) (ornot)
.syntax
    ornot   &opnd5
    ornot   &opnd6
    ornot   &opnd7
    ornot   &opnd1
    ornot   &opnd2
    ornot   &opnd3
    ornot   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Logical Difference (XOR) (xor)
.syntax
    xor     &opnd5
    xor     &opnd6
    xor     &opnd7
    xor     &opnd1
    xor     &opnd2
    xor     &opnd3
    xor     &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Extract Byte Low (extbl)
.syntax
    extbl   &opnd1
    extbl   &opnd2
    extbl   &opnd3
    extbl   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Extract Longword High (extlh)
.syntax
    extlh   &opnd1
    extlh   &opnd2
    extlh   &opnd3
    extlh   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Extract Longword Low (extll)
.syntax
    extll   &opnd1
    extll   &opnd2
    extll   &opnd3
    extll   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Extract Quadword High (extqh)
.syntax
    extqh   &opnd1
    extqh   &opnd2
    extqh   &opnd3
    extqh   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Extract Quadword Low (extql)
.syntax
    extql   &opnd1
    extql   &opnd2
    extql   &opnd3
    extql   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Extract Word High (extwh)
.syntax
    extwh   &opnd1
    extwh   &opnd2
    extwh   &opnd3
    extwh   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Extract Word Low (extwl)
.syntax
    extwl   &opnd1
    extwl   &opnd2
    extwl   &opnd3
    extwl   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Insert Byte Low (insbl)
.syntax
    insbl   &opnd1
    insbl   &opnd2
    insbl   &opnd3
    insbl   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Insert Longword High (inslh)
.syntax
    inslh   &opnd1
    inslh   &opnd2
    inslh   &opnd3
    inslh   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Insert Longword Low (insll)
.syntax
    insll   &opnd1
    insll   &opnd2
    insll   &opnd3
    insll   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Insert Quadword High (insqh)
.syntax
    insqh   &opnd1
    insqh   &opnd2
    insqh   &opnd3
    insqh   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Insert Quadword Low (insql)
.syntax
    insql   &opnd1
    insql   &opnd2
    insql   &opnd3
    insql   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Insert Word High (inswh)
.syntax
    inswh   &opnd1
    inswh   &opnd2
    inswh   &opnd3
    inswh   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Insert Word Low (inswl)
.syntax
    inswl   &opnd1
    inswl   &opnd2
    inswl   &opnd3
    inswl   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Mask Byte Low (mskbl)
.syntax
    mskbl   &opnd1
    mskbl   &opnd2
    mskbl   &opnd3
    mskbl   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Mask Longword High (msklh)
.syntax
    msklh   &opnd1
    msklh   &opnd2
    msklh   &opnd3
    msklh   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Mask Longword Low (mskll)
.syntax
    mskll   &opnd1
    mskll   &opnd2
    mskll   &opnd3
    mskll   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Mask Quadword High (mskqh)
.syntax
    mskqh   &opnd1
    mskqh   &opnd2
    mskqh   &opnd3
    mskqh   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Mask Quadword Low (mskql)
.syntax
    mskql   &opnd1
    mskql   &opnd2
    mskql   &opnd3
    mskql   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Mask Word High (mskwh)
.syntax
    mskwh   &opnd1
    mskwh   &opnd2
    mskwh   &opnd3
    mskwh   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Mask Word Low (mskwl)
.syntax
    mskwl   &opnd1
    mskwl   &opnd2
    mskwl   &opnd3
    mskwl   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Shift Left Logical (sll)
.syntax
    sll     &opnd5
    sll     &opnd6
    sll     &opnd7
    sll     &opnd1
    sll     &opnd2
    sll     &opnd3
    sll     &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Shift Right Arithmetic (sra)
.syntax
    sra     &opnd5
    sra     &opnd6
    sra     &opnd7
    sra     &opnd1
    sra     &opnd2
    sra     &opnd3
    sra     &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Shift Right Logical (srl)
.syntax
    srl     &opnd5
    srl     &opnd6
    srl     &opnd7
    srl     &opnd1
    srl     &opnd2
    srl     &opnd3
    srl     &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Zero Bytes (zap)
.syntax
    zap     &opnd1
    zap     &opnd2
    zap     &opnd3
    zap     &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Zero Bytes NOT (zapnot)
.syntax
    zapnot  &opnd1
    zapnot  &opnd2
    zapnot  &opnd3
    zapnot  &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Multiply Longword (without overflow) (mull)
.syntax
    mull    &opnd1
    mull    &opnd2
    mull    &opnd3
    mull    &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Multiply Quadword (without overflow) (mulq)
.syntax
    mulq    &opnd1
    mulq    &opnd2
    mulq    &opnd3
    mulq    &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction
.section Unsigned Quadword Multiply High (umulh)
.syntax
    umulh   &opnd1
    umulh   &opnd2
    umulh   &opnd3
    umulh   &opnd4
.esyntax
.descrp
.edescrp
.category Operate Format Instruction

:CMT. Floating-Point Operate Format Instruction - Data Type Independent

.section Copy Sign (cpys)
.syntax
    cpys    &opnd1
    cpys    &opnd2
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Copy Sign and Exponent (cpyse)
.syntax
    cpyse   &opnd1
    cpyse   &opnd2
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Copy Sign Negate (cpysn)
.syntax
    cpysn   &opnd1
    cpysn   &opnd2
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Convert Longword to Quadword (cvtlq)
.syntax
    cvtlq   &opnd5
    cvtlq   &opnd6
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Convert Quadword to Longword (cvtql)
.syntax
    cvtql   &opnd5
    cvtql   &opnd6
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Move if Equal to Zero (fcmoveq)
.syntax
    fcmoveq     &opnd1
    fcmoveq     &opnd2
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Move if Greater Than or Equal to Zero (fcmovge)
.syntax
    fcmovge     &opnd1
    fcmovge     &opnd2
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Move if Greater Than Zero (fcmovgt)
.syntax
    fcmovgt     &opnd1
    fcmovgt     &opnd2
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Move if Less Than or Equal to Zero (fcmovle)
.syntax
    fcmovle     &opnd1
    fcmovle     &opnd2
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Move If Less Than Zero (fcmovlt)
.syntax
    fcmovlt     &opnd1
    fcmovlt     &opnd2
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Move if Not Equal to Zero (fcmovne)
.syntax
    fcmovne     &opnd1
    fcmovne     &opnd2
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Move from FP Control Register (mf_fpcr)
.syntax
    mf_fpcr     $d_reg
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent
.section Move to FP Control Register (mt_fpcr)
.syntax
    mt_fpcr     $d_reg
.esyntax
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - Data Type Independent

:CMT. Floating-Point Operate Format Instruction - IEEE

.section Add S_floating (adds)
.syntax
    adds    &opnd1
    adds    &opnd2
.esyntax
.im wasqual adds
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Add T_floating (addt)
.syntax
    addt    &opnd1
    addt    &opnd2
.esyntax
.im wasqual addt
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Compare T_floating Equal (cmpteq)
.syntax
    cmpteq  &opnd1
    cmpteq  &opnd2
.esyntax
.im wasqual cmpteq
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Compare T_floating Less Than (cmptlt)
.syntax
    cmptlt  &opnd1
    cmptlt  &opnd2
.esyntax
.im wasqual cmptlt
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Compare T_floating Less Than or Equal (cmptle)
.syntax
    cmptle  &opnd1
    cmptle  &opnd2
.esyntax
.im wasqual cmptle
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Compare T_floating Unordered (cmptun)
.syntax
    cmptun  &opnd1
    cmptun  &opnd2
.esyntax
.im wasqual cmptun
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Convert Quadword to S_floating (cvtqs)
.syntax
    cvtqs   &opnd5
    cvtqs   &opnd6
.esyntax
.im wasqual cvtqs
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Convert Quadword to T_floating (cvtqt)
.syntax
    cvtqt   &opnd5
    cvtqt   &opnd6
.esyntax
.im wasqual cvtqt
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Convert T_floating to S_floating (cvtts)
.syntax
    cvtts   &opnd5
    cvtts   &opnd6
.esyntax
.im wasqual cvtts
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Divide S_floating (divs)
.syntax
    divs    &opnd1
    divs    &opnd2
.esyntax
.im wasqual divs
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Divide T_floating (divt)
.syntax
    divt    &opnd1
    divt    &opnd2
.esyntax
.im wasqual divt
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Multiply S_floating (muls)
.syntax
    muls    &opnd1
    muls    &opnd2
.esyntax
.im wasqual muls
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Multiply T_floating (mult)
.syntax
    mult    &opnd1
    mult    &opnd2
.esyntax
.im wasqual mult
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Subtract S_floating (subs)
.syntax
    subs    &opnd1
    subs    &opnd2
.esyntax
.im wasqual subs
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Subtract T_floating (subt)
.syntax
    subt    &opnd1
    subt    &opnd2
.esyntax
.im wasqual subt
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE
.section Convert T_floating to Quadword (cvttq)
.syntax
    cvttq   &opnd5
    cvttq   &opnd6
.esyntax
.im wasqual cvttq
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - IEEE

:CMT. Floating-Point Operate Format Instruction - VAX

.section Add F_floating (addf)
.syntax
    addf    &opnd1
    addf    &opnd2
.esyntax
.im wasvqual addf
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Convert D_floating to G_floating (cvtdg)
.syntax
    cvtdg   &opnd5
    cvtdg   &opnd6
.esyntax
.im wasvqual cvtdg
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Add G_floating (addg)
.syntax
    addg    &opnd1
    addg    &opnd2
.esyntax
.im wasvqual addg
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Compare G_floating Equal (cmpgeq)
.syntax
    cmpgeq  &opnd1
    cmpgeq  &opnd2
.esyntax
.im wasvqual cmpgeq
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Compare G_floating Less Than (cmpglt)
.syntax
    cmpglt  &opnd1
    cmpglt  &opnd2
.esyntax
.im wasvqual cmpglt
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Compare G_floating Less Than or Equal (cmpgle)
.syntax
    cmpgle  &opnd1
    cmpgle  &opnd2
.esyntax
.im wasvqual cmpgle
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Convert G_floating to F_floating (cvtgf)
.syntax
    cvtgf   &opnd5
    cvtgf   &opnd6
.esyntax
.im wasvqual cvtgf
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Convert G_floating to D_floating (cvtgd)
.syntax
    cvtgd   &opnd5
    cvtgd   &opnd6
.esyntax
.im wasvqual cvtgd
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Convert Quadword to F_floating (cvtqf)
.syntax
    cvtqf   &opnd5
    cvtqf   &opnd6
.esyntax
.im wasvqual cvtqf
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Convert Quadword to G_floating (cvtqg)
.syntax
    cvtqg   &opnd5
    cvtqg   &opnd6
.esyntax
.im wasvqual cvtqg
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Divide F_floating (divf)
.syntax
    divf    &opnd1
    divf    &opnd2
.esyntax
.im wasvqual divf
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Divide G_floating (divg)
.syntax
    divg    &opnd1
    divg    &opnd2
.esyntax
.im wasvqual divg
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Multiply F_floating (mulf)
.syntax
    mulf    &opnd1
    mulf    &opnd2
.esyntax
.im wasvqual mulf
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Multiply G_floating (mulg)
.syntax
    mulg    &opnd1
    mulg    &opnd2
.esyntax
.im wasvqual mulg
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Subtract F_floating (subf)
.syntax
    subf    &opnd1
    subf    &opnd2
.esyntax
.im wasvqual subf
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Subtract G_floating (subg)
.syntax
    subg    &opnd1
    subg    &opnd2
.esyntax
.im wasvqual subg
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.section Convert G_floating to Quadword (cvtgq)
.syntax
    cvtgq   &opnd5
    cvtgq   &opnd6
.esyntax
.im wasvqual cvtgq
.descrp
.edescrp
.category Floating-Point Operate Format Instruction - VAX
.*
:CMT. PALcode
.*
.section Call Privileged Architecture Library (call_pal)
.syntax
    call_pal    palcode
.esyntax
.descrp
.edescrp
.category PALcode
.*
:CMT. Stylized Code Forms
:CMT. nop and fnop are in directiv.c since they don't require much parsing
.*
.section Clear (clr)
.syntax
    clr     $d_reg
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Floating Clear (fclr)
.syntax
    fclr    $d_reg
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Floating Move (fmov)
.syntax
    fmov    &opnd5
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Negate Longword (without overflow) (negl)
.syntax
    negl    &opnd5
    negl    &opnd6
    negl    &opnd7
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Negate Quadword (without overflow) (negq)
.syntax
    negq    &opnd5
    negq    &opnd6
    negq    &opnd7
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Negate F_floating (negf)
.syntax
    negf    &opnd5
    negf    &opnd6
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Negate G_floating (negg)
.syntax
    negg    &opnd5
    negg    &opnd6
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Negate S_floating (negs)
.syntax
    negs    &opnd5
    negs    &opnd6
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Negate T_floating (negt)
.syntax
    negt    &opnd5
    negt    &opnd6
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Floating Negate (fneg)
.syntax
    fneg    &opnd5
    fneg    &opnd6
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Floating Negate F_floating (fnegf)
.syntax
    fnegf   &opnd5
    fnegf   &opnd6
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Floating Negate G_floating (fnegg)
.syntax
    fnegg   &opnd5
    fnegg   &opnd6
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Floating Negate S_floating (fnegs)
.syntax
    fnegs   &opnd5
    fnegs   &opnd6
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Floating Negate T_floating (fnegt)
.syntax
    fnegt   &opnd5
    fnegt   &opnd6
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Logical Complement (NOT) (not)
.syntax
    not     &opnd5
    not     &opnd6
    not     &opnd7
    not     &opnd1
    not     &opnd2
    not     &opnd3
    not     &opnd4
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Logical Sum (OR) (or)
.syntax
    Synonym for BIS
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Logical Product with Complement (ANDNOT) (andnot)
.syntax
    Synonym for BIC
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Logical Equivalence (XORNOT) (xornot)
.syntax
    Synonym for EQV
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Floating Absolute Value (fabs)
.syntax
    fabs    &opnd5
    fabs    &opnd6
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.section Sign-Extension Longword (sextl)
.syntax
    sextl   &opnd5
    sextl   &opnd6
    sextl   &opnd7
.esyntax
.descrp
.edescrp
.category Stylized Code Forms
.*
:CMT. The following pseudo-instructions might emit multiple real instructions
.*
.section Move (mov)
.syntax
    mov     &opnd5
    mov     val_immed, d_reg
.esyntax
.descrp
.edescrp
.category Pseudo Instruction
.*
:CMT. abs pseudo ins (opcode & funccode are from subl/v, subq/v)
.*
.section Absolute Value Longword (absl)
.syntax
    absl    &opnd5
    absl    &opnd6
    absl    &opnd7
.esyntax
.descrp
.edescrp
.category Pseudo Instruction
.section Absolute Value Quadword (absq)
.syntax
    absq    &opnd5
    absq    &opnd6
    absq    &opnd7
.esyntax
.descrp
.edescrp
.category Pseudo Instruction
.*
.*
:CMT. NYI: (more pseudo-ins that are in MS asaxp)
:CMT. -------------------------------------------
.*
.*
:CMT. Load instructions that emit more than one instruction
.*
.section Load Byte (ldb)
.syntax
    ldb     &opnd8
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Load Byte Unsigned (ldbu)
.syntax
    ldbu    &opnd8
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Load Word (ldw)
.syntax
    ldw     &opnd8
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Load Word Unsigned (ldwu)
.syntax
    ldwu    &opnd8
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Unaligned Load Word (uldw)
.syntax
    uldw    &opnd8
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Unaligned Load Word Unsigned (uldwu)
.syntax
    uldwu   &opnd8
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Unaligned Load Longword (uldl)
.syntax
    uldl    &opnd8
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Unaligned Load Quadword (uldq)
.syntax
    uldq    &opnd8
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.*
:CMT. Store instructions that emit more than one instruction
.*
.section Store Byte (stb)
.syntax
    stb     &opnd9
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Store Word (stw)
.syntax
    stw     &opnd9
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Unaligned Store Word (ustw)
.syntax
    ustw    &opnd9
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Unaligned Store Longword (ustl)
.syntax
    ustl    &opnd9
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Unaligned Store Quadword (ustq)
.syntax
    ustq    &opnd9
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.*
:CMT. Integer divide and remainder instructions that are unsupported in
:CMT. hardward level.
.*
:CMT. (MS asaxp does this by generating bsr to some _div function elsewhere)
.section Divide Longword (divl)
.syntax
    divl    &opnd1
    divl    &opnd2
    divl    &opnd3
    divl    &opnd4
.*
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Divide Longword Unsigned (divlu)
.syntax
    divlu   &opnd1
    divlu   &opnd2
    divlu   &opnd3
    divlu   &opnd4
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Divide Quadword (divq)
.syntax
    divq    &opnd1
    divq    &opnd2
    divq    &opnd3
    divq    &opnd4
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Divide Quadword Unsigned (divqu)
.syntax
    divqu   &opnd1
    divqu   &opnd2
    divqu   &opnd3
    divqu   &opnd4
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Longword Remainder (reml)
.syntax
    reml    &opnd1
    reml    &opnd2
    reml    &opnd3
    reml    &opnd4
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Longword Remainder Unsigned (remlu)
.syntax
    remlu   &opnd1
    remlu   &opnd2
    remlu   &opnd3
    remlu   &opnd4
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Quadword Remainder (remq)
.syntax
    remq    &opnd1
    remq    &opnd2
    remq    &opnd3
    remq    &opnd4
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.section Quadword Remainder Unsigned (remqu)
.syntax
    remqu   &opnd1
    remqu   &opnd2
    remqu   &opnd3
    remqu   &opnd4
.esyntax
.descrp
.edescrp
.category unimplemented Pseudo Instruction
.*
.*
.endlevel
.*
