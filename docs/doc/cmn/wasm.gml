.chap *refid=wasm The &asmname.
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.np
.ix 'assembler'
This chapter describes the &asmname..
It takes as input an assembler source file (a file with extension
"&axt") and produces, as output, an object file.
.np
The &asmname command line syntax is the following.
.ix '&asmcmdup' 'command line format'
.ix 'command line format' '&asmcmdup'
.if '&target' eq 'QNX' .do begin
.mbigbox
&asmcmdup [options] asm_file [options] [@env_var]
.embigbox
.np
The square brackets [ ] denote items which are optional.
.begnote
.mnote &asmcmd
is the name of the &asmname..
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
is a list of valid &asmname options, each preceded by a dash
("&minus.").
Options may be specified in any order.
.endnote
.do end
.el .do begin
.mbigbox
&asmcmdup [options] [d:][path]filename[.ext] [options] [@env_var]
.embigbox
.np
The square brackets [ ] denote items which are optional.
.begnote
.mnote &asmcmdup
is the name of the &asmname..
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
The options supported by the &asmname. are:
.begnote $compact
.note {0,1,2,3,4,5,6}{p}{r,s}
.begnote $compact
.note 0
same as ".8086"
.note 1
same as ".186"
.note 2{p}
same as ".286" or ".286p"
.note 3{p}
same as ".386" or ".386p"
(also defines "__386__" and changes the default USE attribute of
segments from "USE16" to "USE32")
.note 4{p}
same as ".486" or ".486p"
(also defines "__386__" and changes the default USE attribute of
segments from "USE16" to "USE32")
.note 5{p}
same as ".586" or ".586p"
(also defines "__386__" and changes the default USE attribute of
segments from "USE16" to "USE32")
.note 6{p}
same as ".686" or ".686p"
(also defines "__386__" and changes the default USE attribute of
segments from "USE16" to "USE32")
.note p
protect mode
.note add r
defines "__REGISTER__"
.note add s
defines "__STACK__"
.endnote
.exam begin
-2      -3p     -4pr    -5p
.exam end
.note bt=<os>
defines "__<os>__" and checks the "<os>_INCLUDE" environment variable
for include files
.note c
do not output OMF COMENT records that allow WDISASM to figure out when
data bytes have been placed in a code segment
.note d<name>[=text]
define text macro
.note d1
line number debugging support
.note e
stop reading assembler source file at END directive.
Normally, anything following the END directive will cause an error.
.note e<number>
set error limit number
.note fe=<file_name>
set error file name
.note fo=<file_name>
set object file name
.note fi=<file_name>
force <file_name> to be included
.note fpc
same as ".no87"
.note fpi
inline 80x87 instructions with emulation
.note fpi87
inline 80x87 instructions
.note fp0
same as ".8087"
.note fp2
same as ".287" or ".287p"
.note fp3
same as ".387" or ".387p"
.note fp5
same as ".587" or ".587p"
.note fp6
same as ".687" or ".687p"
.note i=<directory>
add directory to list of include directories
.note j or s
force signed types to be used for signed values
.note m{t,s,m,c,l,h,f}
memory model: (Tiny, Small, Medium, Compact, Large, Huge, Flat)
.begnote $compact
.note -mt
Same as ".model tiny"
.note -ms
Same as ".model small"
.note -mm
Same as ".model medium"
.note -mc
Same as ".model compact"
.note -ml
Same as ".model large"
.note -mh
Same as ".model huge"
.note -mf
Same as ".model flat"
.endnote
.np
Each of the model directives also defines "__<model>__"
(e.g., ".model small" defines "__SMALL__").
They also affect whether something like "foo proc" is considered a
"far" or "near" procedure.
.note nd=<name>
set data segment name
.note nm=<name>
set module name
.note nt=<name>
set name of text segment
.note o
allow C form of octal constants
.note zcm=<mode>
set compatibility mode - watcom, masm or tasm, if <mode> is not
specified then masm is used, default mode is watcom
.note zld
remove file dependency information
.note zq or q
operate quietly
.note zz
remove "@size" from STDCALL function names
.note zzo
don't mangle STDCALL symbols (WASM backward compatible)
.note ?  or h
print this message
.note w<number>
set warning level number
.note we
treat all warnings as errors
.note wx
set warning level to maximum setting
.endnote
.*
.section Assembler Directives, Operators and Assembly Opcodes
.*
.np
It is not the intention of this chapter to describe assembly-language
programming in any detail.
You should consult a book that deals with this topic.
However, we present an alphabetically ordered list of the directives,
opcodes and register names that are recognized by the assembler.
.*
:set symbol="wasmfull" value="1".
:INCLUDE file='wasmitms'.
.*
.section Unsupported Directives
.*
.np
Other assemblers support directives that this assembler does not.
The following is a list of directives that are ignored by the
&asmname. (use of these directives results in a warning message).
.ix '.alpha'
.ix '.cref'
.ix '.lfcond'
.ix '.list'
.ix '.listall'
.ix '.listif'
.ix '.listmacro'
.ix '.listmacroall'
.ix '.nocref'
.ix '.nolist'
.ix 'page'
.ix '.sall'
.ix '.seq'
.ix '.sfcond'
.ix 'subtitle'
.ix 'subttl'
.ix '.tfcond'
.ix 'title'
.ix '.xcref'
.ix '.xlist'
.millust begin
~.alpha         .cref          .lfcond        .list
~.listall       .listif        .listmacro     .listmacroall
~.nocref        .nolist        page           .sall
~.seq           .sfcond        subtitle       subttl
~.tfcond        title          .xcref         .xlist
.millust end
.np
The following is a list of directives that are flagged by the
&asmname. (use of these directives results in an error message).
.ix 'addr'
.ix '.break'
.ix 'casemap'
.ix 'catstr'
.ix '.continue'
.ix 'echo'
.ix '.else'
.ix 'endmacro'
.ix '.endif'
.ix '.endw'
.ix '.exit'
.ix 'high'
.ix 'highword'
.ix '.if'
.ix 'invoke'
.ix 'low'
.ix 'lowword'
.ix 'lroffset'
.ix 'mask'
.ix 'opattr'
.ix 'option'
.ix 'popcontext'
.ix 'proto'
.ix 'purge'
.ix 'pushcontext'
.ix '.radix'
.ix 'record'
.ix '.repeat'
.ix '.startup'
.ix 'this'
.ix 'typedef'
.ix 'union'
.ix '.until'
.ix '.while'
.ix 'width'
.millust begin
addr           .break         casemap        catstr
~.continue      echo           .else          endmacro
~.endif         .endw          .exit          high
highword       .if            invoke         low
lowword        lroffset       mask           opattr
option         popcontext     proto          purge
pushcontext    .radix         record         .repeat
~.startup       this           typedef        union
~.until         .while         width
.millust end
.*
.section &asmname Specific
.*
.np
There are a few specific features in &asmname.
.np
.beglevel
.section Naming convention
.millust begin
                 Procedure   Variable
Convention         Name        Name
---------------  ----------  ---------
C                  '_*'        '_*'
WATCOM_C         see section &company "C" name mangler
SYSCALL             '*'         '*'
STDCALL           '_*@nn'      '_*'
STDCALL            '_*'        '_*'    see note 1
STDCALL             '*'         '*'    see note 2
BASIC               '^'         '^'
FORTRAN             '^'         '^'
PASCAL              '^'         '^'
.millust end
.autonote Notes:
.note
In STDCALL procedures name 'nn' is overall parametrs size in bytes.
'@nn' is suppressed when -zz command line option is used (WATCOM 10.0 compatibility).
.note
STDCALL symbols mangling is suppressed by -zzo command line option (WASM backward compatible).
.endnote
.np
.section &company "C" name mangler
.millust begin
Command line     Procedure   Variable
  option           Name        Name
---------------  ----------  ---------
0,1,2              '*_'        '_*'
3,4,5,6 with r     '*_'        '_*'
3,4,5,6 with s      '*'        '*'
.millust end
.np
.section Calling convention
.millust begin
                      Parameters    Parameters   Cleanup caller
Convention   Vararg    passed by       order         stack
-----------  ------  ------------  ------------- --------------
C             yes      stack       right to left    no
WATCOM_C      yes      registers   right to left    see note 1
              yes      stack       right to left    no
SYSCALL       yes      stack       right to left    no
STDCALL       yes      stack       right to left    yes see note 2
BASIC         no       stack       left to right    yes
FORTRAN       no       stack       left to right    yes
PASCAL        no       stack       left to right    yes
.millust end
.autonote Notes:
.note
If any parameter is passed on the stack then WASM automaticaly cleanup caller stack.
.note
For STDCALL procedures WASM automaticaly cleanup caller stack,
except case when vararg parameter is used.
.endnote
.endlevel
.*
.section &asmname. Diagnostic Messages
.*
.dm errnote begin
.   .se *ctxn=&'substr(&*,1,&'pos(' ',&*)-1)
.   .se *ttl$=&'substr(&*,&'pos(' ',&*)+1)
.   .note &*ctxn &*ttl$
.*  .us &*ttl$
.dm errnote end
.*
.begnote
.*
.errnote 001 Size doesn't match with previous definition
.errnote 002 Invalid instruction with current CPU setting
.errnote 003 LOCK prefix is not allowed on this instruction
.errnote 004 REP prefix is not allowed on this instruction
.errnote 005 Invalid memory pointer
.errnote 006 Cannot use 386 addressing mode with current CPU setting
.errnote 007 Too many base registers
.errnote 008 Invalid index register
.errnote 009 Scale factor must be 1, 2, 4 or 8
.errnote 010 invalid addressing mode with current CPU setting
.errnote 011 ESP cannot be used as index
.errnote 012 Too many base/index registers
.errnote 013 Memory offset cannot reference to more than one label
.errnote 014 Offset must be relocatable
.errnote 015 Memory offset expected
.errnote 016 Invalid indirect memory operand
.errnote 017 Cannot mix 16 and 32-bit registers
.errnote 018 CPU type already set
.errnote 019 Unknown directive
.errnote 020 Expecting comma
.errnote 021 Expecting number
.errnote 022 Invalid label definition
.errnote 023 Invalid use of SHORT, NEAR, FAR operator
.errnote 024 No memory
.errnote 025 Cannot use 386 segment register with current CPU setting
.errnote 026 POP CS is not allowed
.errnote 027 Cannot use 386 register with current CPU setting
.errnote 028 Only MOV can use special register
.errnote 029 Cannot use TR3, TR4, TR5 in current CPU setting
.errnote 030 Cannot use SHORT with CALL
.errnote 031 Only SHORT displacement is allowed
.errnote 032 Syntax error
.errnote 033 Prefix must be followed by an instruction
.errnote 034 No size given before 'PTR' operator
.errnote 035 Invalid IMUL format
.errnote 036 Invalid SHLD/SHRD format
.errnote 037 Too many commas
.errnote 038 Syntax error: Unexpected colon
.errnote 039 Operands must be the same size
.errnote 040 Invalid instruction operands
.errnote 041 Immediate constant too large
.errnote 042 Can not use short or near modifiers with this instruction
.errnote 043 Jump out of range
.errnote 044 Displacement cannot be larger than 32k
.errnote 045 Initializer value too large
.errnote 046 Symbol already defined
.errnote 047 Immediate data too large
.errnote 048 Immediate data out of range
.errnote 049 Can not transfer control to stack symbol
.errnote 050 Offset cannot be smaller than WORD size
.errnote 051 Can not take offset of stack symbol
.errnote 052 Can not take segment of stack symbol
.errnote 053 Segment too large
.errnote 054 Offset cannot be larger than 32k
.errnote 055 Operand 2 too big
.errnote 056 Operand 1 too small
.errnote 057 Too many arithmetic operators
.errnote 058 Too many open square brackets
.errnote 059 Too many close square brackets
.errnote 060 Too many open brackets
.errnote 061 Too many close brackets
.errnote 062 Invalid number digit
.errnote 063 Assembler Code is too long
.errnote 064 Brackets are not balanced
.errnote 065 Operator is expected
.errnote 066 Operand is expected
.errnote 067 Too many tokens in a line
.errnote 068 Bracket is expected
.errnote 069 Illegal use of register
.errnote 070 Illegal use of label
.errnote 071 Invalid operand in addition
.errnote 072 Invalid operand in subtraction
.errnote 073 One operand must be constant
.errnote 074 Constant operand is expected
.errnote 075 A constant operand is expected in addition
.errnote 076 A constant operand is expected in subtraction
.errnote 077 A constant operand is expected in multiplication
.errnote 078 A constant operand is expected in division
.errnote 079 A constant operand is expected after a positive sign
.errnote 080 A constant operand is expected after a negative sign
.errnote 081 Label is not defined
.errnote 082 More than one override
.errnote 083 Label is expected
.errnote 084 Only segment or group label is allowed
.errnote 085 Only register or label is expected in override
.errnote 086 Unexpected end of file
.errnote 087 Label is too long
.errnote 088 This feature has not been implemented yet
.errnote 089 Internal Error #1
.errnote 090 Can not take offset of group
.errnote 091 Can not take offset of segment
.errnote 092 Invalid character found
.errnote 093 Invalid operand size for instruction
.errnote 094 This instruction is not supported
.errnote 095 size not specified -- BYTE PTR is assumed
.errnote 096 size not specified -- WORD PTR is assumed
.errnote 097 size not specified -- DWORD PTR is assumed
.*
.errnote 500 Segment parameter is defined already
.errnote 501 Model parameter is defined already
.errnote 502 Syntax error in segment definition
.errnote 503 'AT' is not supported in segment definition
.errnote 504 Segment definition is changed
.errnote 505 Lname is too long
.errnote 506 Block nesting error
.errnote 507 Ends a segment which is not opened
.errnote 508 Segment option is undefined
.errnote 509 Model option is undefined
.errnote 510 No segment is currently opened
.errnote 511 Lname is used already
.errnote 512 Segment is not defined
.errnote 513 Public is not defined
.errnote 514 Colon is expected
.errnote 515 A token is expected after colon
.errnote 516 Invalid qualified type
.errnote 517 Qualified type is expected
.errnote 518 External definition different from previous one
.errnote 519 Memory model is not found in .MODEL
.errnote 520 Cannot open include file
.errnote 521 Name is used already
.errnote 522 Library name is missing
.errnote 523 Segment name is missing
.errnote 524 Group name is missing
.errnote 525 Data emitted with no segment
.errnote 526 Seglocation is expected
.errnote 527 Invalid register
.errnote 528 Cannot address with assumed register
.errnote 529 Invalid start address
.errnote 530 Label is already defined
.errnote 531 Token is too long
.errnote 532 The line is too long after expansion
.errnote 533 A label is expected after colon
.errnote 534 Must be associated with code
.errnote 535 Procedure must have a name
.errnote 536 Procedure is alreadly defined
.errnote 537 Language type must be specified
.errnote 538 End of procedure is not found
.errnote 539 Local variable must immediately follow PROC or MACRO statement
.errnote 540 Extra character found
.errnote 541 Cannot nest procedures
.errnote 542 No procedure is currently defined
.errnote 543 Procedure name does not match
.errnote 544 Vararg requires C calling convention
.errnote 545 Model declared already
.errnote 546 Model is not declared
.errnote 547 Backquote expected
.errnote 548 COMMENT delimiter expected
.errnote 549 End directive required at end of file
.errnote 550 Nesting level too deep
.errnote 551 Symbol not defined
.errnote 552 Insert Stupid warning #1 here
.errnote 553 Insert Stupid warning #2 here
.errnote 554 Spaces not allowed in command line options
.errnote 555 Error:
.errnote 556 Source File
.errnote 557 No filename specified.
.errnote 558 Out of Memory
.errnote 559 Cannot Open File -
.errnote 560 Cannot Close File -
.errnote 561 Cannot Get Start of Source File -
.errnote 562 Cannot Set to Start of Source File -
.errnote 563 Command Line Contains More Than 1 File To Assemble
.errnote 564 include path %s.
.errnote 565 Unknown option %s. Use /? for list of options.
.errnote 566 read more command line from %s.
.errnote 567 Internal error in %s(%u)
.errnote 568 OBJECT WRITE ERROR !!
.errnote 569 NO LOR PHARLAP !!
.errnote 570 Parameter Required
.errnote 571 Expecting closing square bracket
.errnote 572 Expecting file name
.errnote 573 Floating point instruction not allowed with /fpc
.errnote 574 Too many errors
.errnote 575 Build target not recognised
.errnote 576 Public constants should be numeric
.errnote 577 Expecting symbol
.errnote 578 Do not mix simplified and full segment definitions
.errnote 579 Parms passed in multiple registers must be accessed separately, use %s
.errnote 580 Ten byte variables not supported in register calling convention
.errnote 581 Parameter type not recognised
.errnote 582 forced error:
.errnote 583 forced error: Value not equal to 0 : %d
.errnote 584 forced error: Value equal to 0: %d
.errnote 585 forced error: symbol defined: %s
.errnote 586 forced error: symbol not defined: %s
.errnote 587 forced error: string blank : <%s>
.errnote 588 forced error: string not blank : <%s>
.errnote 589 forced error: strings not equal : <%s> : <%s>
.errnote 590 forced error: strings equal : <%s> : <%s>
.errnote 591 included by file %s(%d)
.errnote 592 macro called from file %s(%d)
.errnote 593 Symbol %s not defined
.errnote 594 Extending jump
.errnote 595 Ignoring inapplicable directive
.errnote 596 Unknown symbol class '%s'
.errnote 597 Symbol class for '%s' already established
.errnote 598 number must be a power of 2
.errnote 599 alignment request greater than segment alignment
.errnote 600 '%s' is already defined
.errnote 601 %u unclosed conditional directive(s) detected
.*
.endnote
