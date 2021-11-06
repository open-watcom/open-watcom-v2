.chap *refid=wdisasm The Object File Disassembler
.*
.if &e'&dohelp eq 0 .do begin
.*
.section Introduction
.*
.do end
.np
.ix 'disassembler'
This chapter describes the &disname..
It takes as input an object file (a file with extension "&obj") and
produces, as output, the Intel assembly language equivalent.
The &company compilers do not produce an assembly language listing
directly from a source program.
Instead, the &disname can be used to generate an assembly language
listing from the object file generated by the compiler.
.np
The &disname command line syntax is the following.
.ix '&discmdup' 'command line format'
.ix 'command line format' '&discmdup'
.if '&target' eq 'QNX' .do begin
.mbigbox
&discmdup [options] filespec [options]
.embigbox
.np
The square brackets [ ] denote items which are optional.
.begnote
.note &discmd
is the name of the &disname..
.note filespec
is the filename specification of the object file to be
disassembled.
A default filename extension of ".o" is assumed when no extension is
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
.note options
is a list of valid &disname options, each preceded by a dash
("&minus.").
Options may be specified in any order.
.endnote
.do end
.el .do begin
.mbigbox
&discmdup [options] [d:][path]filename[.ext] [options]
.embigbox
.np
The square brackets [ ] denote items which are optional.
.begnote
.note &discmdup
is the name of the &disname..
.note d:
is an optional drive specification such as "A:", "B:", etc.
If not specified, the default drive is assumed.
.note path
is an optional path specification such as "\PROGRAMS\OBJ\".
If not specified, the current directory is assumed.
.note filename
is the file name of the object file to disassemble.
.note ext
is the file extension of the object file to disassemble.
If omitted, a file extension of "&obj" is assumed.
If the period "." is specified but not the extension, the file is
assumed to have no file extension.
.note options
is a list of valid options, each preceded by a slash
("/") or a dash ("&minus.").
Options may be specified in any order.
.endnote
.do end
.np
The options supported by the &disname. are:
.begnote $break $compact
.note l[=<list_file>]
create a listing file
.note s[=<source_file>]
using object file source line information, imbed original source
lines into the output file
.note c=<code_name>
name of additional segments that contain executable code
.if &vermacro gt 900 .do begin
.note i=<char>
redefine the initial character of internal labels (default: L)
.do end
.note a
write assembly instructions only to the listing file
.note au
.ix 'UNIX'
write assembly instructions only to the listing file using
UNIX-style assembly syntax
.note b
generate alternate form of based or indexed addressing modes
.note e
include list of external names
.note p
include list of public names
.note r
display register names in upper case
.note u
display instruction opcode mnemonics in upper case
.if &vermacro gt 900 .do begin
.note m
leave C++ names mangled
.do end
.endnote
.np
.ix '&discmdup options'
The following sections describe the list of options.
.*
.section The Listing Option - "l[=<list_file>]"
.*
.np
.ix '&discmdup options' 'l (lowercase L)'
By default, the &disname produces its output to the terminal.
The "l" (lowercase L) option instructs the &disname to produce the
output to a listing file.
The default file name of the listing file is the same as the file
name of the object file.
The default file extension of the listing file is
.fi &lst..
.exam begin
&prompt.&discmd calendar &sw.l
.exam end
.pc
In the above example, the &disname is instructed to disassemble the
contents of the file
.fi calendar&obj
and produce the output to a listing file called
.fi calendar&lst..
.np
An alternate form of this option is "l=<list_file>".
With this form, you can specify the name of the listing file.
When specifying a listing file, a file extension of
.fi &lst
is assumed if none is specified.
.exam begin
&prompt.&discmd calendar &sw.l=calendar.lis
.exam end
.pc
In the above example, the &disname is instructed to disassemble the
contents of the file
.fi calendar&obj
and produce the output to a listing file called
.fi calendar.lis.
.*
.section The Source Option - "s[=<source_file>]"
.*
.np
.ix '&discmdup options' 's'
The "s" option causes the source lines corresponding to the assembly
language instructions to be produced in the listing file.
The object file must contain line numbering information.
That is, the "d1" or "d2" option must have been specified when the
source file was compiled.
If no line numbering information is present in the object file, the "s"
option is ignored.
.np
The following defines the order in which the source file name is
determined when the "s" option is specified.
.autopoint
.point
If present,
the source file name specified on the command line.
.point
The name from the module header record.
.point
The object file name.
.endpoint
.np
In the following example, we have compiled the source file
.fi mysrc.&langsuff
with "d1" debugging information.
We then disassemble it as follows:
.exam begin
&prompt.&discmd mysrc &sw.s &sw.l
.exam end
.pc
In the above example, the &disname is instructed to disassemble the
contents of the file
.fi mysrc&obj
and produce the output to the listing file
.fi mysrc&lst..
The source lines are extracted from the file
.fi mysrc.&langsuff..
.np
An alternate form of this option is "s=<source_file>".
With this form, you can specify the name of the source file.
:cmt. When specifying a source file, a file extension of ".&langsuffup" is
:cmt. assumed if none is specified.
.exam begin
&prompt.&discmd mysrc &sw.s=myprog.&langsuff &sw.l
.exam end
.pc
The above example produces the same result as in the previous example
except the source lines are extracted from the file
.fi myprog.&langsuff..
.*
.section Identifying Code Segments Option - "c=<code_name>"
.*
.np
.ix '&discmdup options' 'c'
The "c" option permits you to specify one additional segment name
or pattern for segments that contain executable code.
Valid forms of the "c" option are:
.millust begin
c=<string>
&setdelim.c=*<string>&setdelim.
&setdelim.c=<string>*&setdelim.
.millust end
.pc
.id <string>
is any sequence of characters with letters treated in a case insensitive
manner.
The "*" character is a wildcard character.
.if '&target' eq 'QNX' .do begin
You must protect the "*" with quotation marks to prevent the shell
from attempting to expand it.
.do end
.np
By default, the following segments are assumed to contain executable code.
.autopoint
.point
segments with class name "CODE"
.point
segments whose name ends in "TEXT" or "CODE" (this is equivalent to
"c=*text" and "c=*code")
.endpoint
.np
Consider the following example.
.millust begin
&prompt.&discmd myprog &setdelim.&sw.c=T@*&setdelim.
.millust end
.pc
All segments whose name starts with the characters "T@" (in addition
to those ending in "CODE" or "TEXT") are assumed to contain
executable code.
.remark
It is not possible to instruct the &disname to interpret segments
whose name ends in "CODE" or "TEXT" as data.
.eremark
.*
.if &vermacro gt 900 .do begin
.*
.section Changing the Internal Label Character - "i=<char>"
.*
.np
.ix '&discmdup options' 'i'
The "i" option permits you to specify the first character to be used
for internal labels.
Internal labels take the form "Ln" where "n" is one or more digits.
The default character "L" can be changed using the "i" option.
The replacement character must be a letter (a-z, A-Z).
A lowercase letter is converted to uppercase.
.exam begin
&prompt.&discmd calendar &sw.i=x
.exam end
.do end
.*
.section The Assembly Format Option - "a", "au"
.*
.np
.ix '&discmdup options' 'a'
The "a" option controls the format of the output produced to the
listing file.
When specified, the &disname will produce a listing file that can be
used as input to an assembler.
.ix 'UNIX'
When "au" is specified, the assembly syntax is that employed by UNIX
systems.
.exam begin
&prompt.&discmd calendar &sw.a &sw.l=calendar.asm
.exam end
.pc
In the above example, the &disname is instructed to disassemble the
contents of the file
.fi calendar&obj
and produce the output to the file
.fi calendar.asm
so that it can be assembled by an assembler.
.*
.section The Alternate Addressing Form Option - "b"
.*
.np
.ix '&discmdup options' 'b'
The "b" option causes an alternate form of the based or indexed
addressing mode to be used in an instruction.
By default, the following form is used.
.millust begin
mov ax,-2[bp]
.millust end
.pc
If the "b" option is specified, the following form is used.
.millust begin
mov ax,[bp-2]
.millust end
.*
.section The External Symbols Option - "e"
.*
.np
.ix '&discmdup options' 'e'
The "e" option controls the amount of information produced in the
listing file.
When specified, a list of all externally defined symbols is produced
in the listing file.
.exam begin
&prompt.&discmd calendar &sw.e
.exam end
.pc
In the above example, the &disname is instructed to disassemble the
contents of the file
.fi calendar&obj
and produce the output, with a
list of all external symbols, on the screen.
A sample list of external symbols is shown below.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&arch' eq '16-bit' .do begin
.code begin
List of external symbols

Symbol
----------------
___iob           000002bc 000001c4 000001af 00000128 00000111
__STK            000002fc 000002c9 0000027f 000001e4 000000a9 00000004
Box_             000000d2
Calendar_        0000008e 00000066 0000003e
ClearScreen_     00000012
fflush_          000002bf 000001c7 000001b2 0000012b 00000114
int86_           00000323 000002f1
Line_            00000272 00000252 00000235 0000021c 00000205
localtime_       00000023
memset_          0000029d
PosCursor_       000002af 000001a1 0000011c 00000100 00000097
printf_          000002b6 000001be 000001a9 00000122 0000010b
strlen_          000000e3
time_            00000017
------------------------------------------------------------
.code end
.do end
.if '&arch' eq '32-bit' .do begin
.code begin
List of external symbols

Symbol
----------------
___iob           0000032f 00000210 000001f4 00000158 00000139
__CHK            00000381 00000343 000002eb 00000237 000000cb 00000006
Box_             000000f2
Calendar_        000000a7 00000079 00000049
ClearScreen_     00000016
fflush_          00000334 00000215 000001f9 0000015d 0000013e
int386_          000003af 00000372
Line_            000002db 000002b5 00000293 00000274 0000025a
localtime_       00000028
memset_          00000308
PosCursor_       0000031e 000001e1 00000148 00000123 000000b6
printf_          00000327 00000208 000001ec 00000150 00000131
strlen_          00000108
time_            0000001d
------------------------------------------------------------
.code end
.do end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.if '&arch' eq '16-bit' .do begin
.code begin
List of external symbols

Symbol
----------------
CALENDAR         000000f8 000000aa 0000005c
CLEARSCREEN      0000000b
GETDAT           0000001f
POSCURSOR        00000117
------------------------------------------------------------
.code end
.do end
.if '&arch' eq '32-bit' .do begin
.code begin
List of external symbols

Symbol
----------------
CALENDAR         000000cf 0000008b 00000047
CLEARSCREEN      0000000a
GETDAT           00000018
POSCURSOR        000000e8
------------------------------------------------------------
.code end
.do end
.do end
.pc
Each externally defined symbol is followed by a list of location
counter values indicating where the symbol is referenced.
.np
The "e" option is ignored when the "a" option is specified.
.*
.section The Public Symbols Option - "p"
.*
.np
.ix '&discmdup options' 'p'
The "p" option controls the amount of information produced in the
listing file.
When specified, a list of all public symbols is produced in the
listing file.
.exam begin
&prompt.&discmd calendar &sw.p
.exam end
.pc
In the above example, the &disname is instructed to disassemble the
contents of the file
.fi calendar&obj
and produce the output, with a list of all exported symbols, to the
screen.
A sample list of public symbols is shown below.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&arch' eq '16-bit' .do begin
.code begin
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
Box_                            _TEXT            000001df
Calendar_                       _TEXT            000000a4
ClearScreen_                    _TEXT            000002f8
Line_                           _TEXT            0000027a
main_                           _TEXT            00000000
PosCursor_                      _TEXT            000002c4

------------------------------------------------------------
.code end
.do end
.if '&arch' eq '32-bit' .do begin
.code begin
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
Box_                            _TEXT            00000231
Calendar_                       _TEXT            000000c5
ClearScreen_                    _TEXT            0000037b
Line_                           _TEXT            000002e5
main_                           _TEXT            00000000
PosCursor_                      _TEXT            0000033d

------------------------------------------------------------
.code end
.do end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.if '&arch' eq '16-bit' .do begin
.code begin
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
BOX                             BOX_TEXT         00000000
CALENDAR                        CALENDAR_TEXT    00000000
CLEARSCREEN                     CLEARSCREEN_TEXT 00000000
FMAIN                           FMAIN_TEXT       00000000
LINE                            LINE_TEXT        00000000
POSCURSOR                       POSCURSOR_TEXT   00000000

------------------------------------------------------------
.code end
.do end
.if '&arch' eq '32-bit' .do begin
.code begin
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
BOX                             BOX_TEXT         00000000
CALENDAR                        CALENDAR_TEXT    00000000
CLEARSCREEN                     CLEARSCREEN_TEXT 00000000
FMAIN                           FMAIN_TEXT       00000000
LINE                            LINE_TEXT        00000000
POSCURSOR                       POSCURSOR_TEXT   00000000

------------------------------------------------------------
.code end
.do end
.do end
.np
The "p" option is ignored when the "a" option is specified.
.*
.section The Uppercase Register Option - "r"
.*
.np
.ix '&discmdup options' 'r'
The "r" option instructs the &disname to display register names
in uppercase.
The default is to display register names in lowercase.
.*
.section The Uppercase Opcode Option - "u"
.*
.np
.ix '&discmdup options' 'u'
The "u" option instructs the &disname to display instruction opcode
mnemonics in uppercase.
The default is to display instruction opcode mnemonics in lowercase.
.*
.if &vermacro gt 900 .do begin
.*
.section Retain C++ Mangled Names  - "m"
.*
.np
.ix '&discmdup options' 'm'
The "m" option instructs the &disname to retain C++ mangled names
rather than displaying their demangled form.
The default is to interpret mangled C++ names and display them in
a somewhat more intelligible form.
.do end
.*
.section An Example
.*
.np
.ix '&discmdup example'
.ix 'disassembly example'
Consider the following program contained in the file
.fi hello.&langsuff..
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.mbox begin
#include <stdio.h>

void main()
    {
        printf( "Hello world\n" );
    }
.mbox end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.mbox begin
      program main
      print *, 'Hello world'
      end
.mbox end
.do end
.np
Compile it with the "d1" option.
An object file called
.fi hello&obj
will be produced.
The "d1" option causes line numbering information to be generated in
the object file.
We can use the &disname to disassemble the contents of the object file
by issuing the following command.
.millust begin
&prompt.&discmd hello &sw.l &sw.e &sw.p &sw.s &sw.r
.millust end
.pc
The output will be written to a listing file called
.fi hello&lst
(the "l" option was specified").
It will contain a list of external symbols
(the "e" option was specified),
a list of public symbols (the "p" option was specified) and
the source lines corresponding to the assembly language instructions
(the "s" option was specified).
The source input file is called
.fi hello.&langsuff..
The register names will be displayed in upper case (the "r" option was
specified).
The output, shown below, is the result of using the
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&cmpcname
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
&cmpname
.do end
compiler.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&arch' eq '16-bit' .do begin
.code begin
Module: hello.c
Group: 'DGROUP' CONST,CONST2,_DATA,_BSS

Segment: '_TEXT' BYTE  00000011 bytes

#include <stdio.h>

void main()
{
 0000  b8 04 00          main_           mov     AX,0004H
 0003  e8 00 00                          call    __STK

    printf( "Hello world\n" );
 0006  b8 00 00                          mov     AX,offset L1
 0009  50                                push    AX
 000a  e8 00 00                          call    printf_
 000d  83 c4 02                          add     SP,0002H

}
 0010  c3                                ret

No disassembly errors

.code break
List of external symbols

Symbol
----------------
__STK            00000004
printf_          0000000b
------------------------------------------------------------

Segment: 'CONST' WORD  0000000d bytes
 0000  48 65 6c 6c 6f 20 77 6f L1              - Hello wo
 0008  72 6c 64 0a 00                          - rld..

No disassembly errors

.code break
------------------------------------------------------------
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
main_                           _TEXT            00000000

------------------------------------------------------------
.code end
.do end
.if '&arch' eq '32-bit' .do begin
.code begin
Module: hello.c
Group: 'DGROUP' CONST,CONST2,_DATA,_BSS

Segment: _TEXT  BYTE USE32  00000018 bytes

#include <stdio.h>

void main()
    {
 0000  68 08 00 00 00    main_           push    00000008H
 0005  e8 00 00 00 00                    call    __CHK

        printf( "Hello world\n" );
 000a  68 00 00 00 00                    push    offset L1
 000f  e8 00 00 00 00                    call    printf_
 0014  83 c4 04                          add     ESP,00000004H

    }
 0017  c3                                ret

No disassembly errors

.code break
List of external symbols

Symbol
----------------
__CHK            00000006
printf_          00000010
------------------------------------------------------------

Segment: CONST  DWORD USE32  0000000d bytes
 0000  48 65 6c 6c 6f 20 77 6f L1              - Hello wo
 0008  72 6c 64 0a 00                          - rld..

.code break
No disassembly errors

------------------------------------------------------------
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
main_                           _TEXT            00000000

------------------------------------------------------------
.code end
.do end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.if '&arch' eq '16-bit' .do begin
.code begin
Module: hello.for
Group: 'DGROUP' CONST,_DATA,_BSS

Segment: 'FMAIN_TEXT' BYTE  00000017 bytes

        program main
 0000  52                FMAIN           push    DX

        print *, 'Hello world'
 0001  9a 00 00 00 00                    call    far RT@IOWrite
 0006  b8 00 00                          mov     AX,offset L2
 0009  8c d2                             mov     DX,SS
 000b  9a 00 00 00 00                    call    far RT@OutCHAR
 0010  9a 00 00 00 00                    call    far RT@EndIO

        end
 0015  5a                                pop     DX
 0016  cb                                retf
.code break
No disassembly errors

List of external symbols

Symbol
----------------
RT@EndIO         00000011
RT@IOWrite       00000002
RT@OutCHAR       0000000c
------------------------------------------------------------

Segment: 'CONST' WORD  0000000b bytes
 0000  48 65 6c 6c 6f 20 77 6f L1              - Hello wo
 0008  72 6c 64                                - rld
.code break
No disassembly errors

------------------------------------------------------------

Segment: '_DATA' WORD  00000006 bytes
 0000  00 00 00 00             L2              DD      DGROUP:L1
 0004  0b 00                                   - ..
.code break
No disassembly errors

------------------------------------------------------------
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
FMAIN                           FMAIN_TEXT       00000000

------------------------------------------------------------

.code end
.do end
.if '&arch' eq '32-bit' .do begin
.code begin
Module: hello.for
Group: 'DGROUP' CONST,_DATA,_BSS
Group: 'FLAT'

Segment: 'FMAIN_TEXT' BYTE USE32  00000014 bytes

        program main
        print *, 'Hello world'
 0000  e8 00 00 00 00    FMAIN           call    RT@IOWrite
 0005  b8 00 00 00 00                    mov     EAX,offset L2
 000a  e8 00 00 00 00                    call    RT@OutCHAR
 000f  e9 00 00 00 00                    jmp     RT@EndIO
.code break
No disassembly errors

List of external symbols

Symbol
----------------
RT@EndIO         00000010
RT@IOWrite       00000001
RT@OutCHAR       0000000b
------------------------------------------------------------

Segment: 'CONST' WORD USE32  0000000b bytes
 0000  48 65 6c 6c 6f 20 77 6f L1              - Hello wo
 0008  72 6c 64                                - rld
.code break
No disassembly errors

------------------------------------------------------------

Segment: '_DATA' WORD USE32  00000008 bytes
 0000  00 00 00 00             L2              DD      DGROUP:L1
 0004  0b 00 00 00                             - ....
.code break
No disassembly errors

------------------------------------------------------------
List of public symbols

SYMBOL          GROUP           SEGMENT          ADDRESS
---------------------------------------------------------
FMAIN                           FMAIN_TEXT       00000000

------------------------------------------------------------

.code end
.do end
.do end
.keep
.np
Let us create a form of the listing file that can be used
as input to an assembler.
.millust begin
&prompt.&discmd hello &sw.l=hello.asm &sw.r &sw.a
.millust end
.pc
The output will be produced in the file
.fi hello.asm.
The output, shown below, is the result of using the
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
&cmpcname
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
&cmpname
.do end
compiler.
.if '&lang' eq 'C' or '&lang' eq 'C/C++' .do begin
.if '&arch' eq '16-bit' .do begin
.code begin
                NAME    hello
                EXTRN   _small_code_:BYTE
                EXTRN   _cstart_:BYTE
                EXTRN   printf_:BYTE
                EXTRN   __STK:BYTE
DGROUP          GROUP   CONST,CONST2,_DATA,_BSS
_TEXT           SEGMENT BYTE PUBLIC 'CODE'
                ASSUME  CS:_TEXT,DS:DGROUP,SS:DGROUP
                PUBLIC  main_
main_:          mov     AX,0004H
                call    near ptr __STK
                mov     AX,offset DGROUP:L1
                push    AX
                call    near ptr printf_
                add     SP,0002H
                ret
_TEXT           ENDS

.code break
CONST           SEGMENT WORD PUBLIC 'DATA'
L1              LABEL   BYTE
                DB      48H,65H,6cH,6cH,6fH,20H,77H,6fH
                DB      72H,6cH,64H,0aH,00H
CONST           ENDS

CONST2          SEGMENT WORD PUBLIC 'DATA'
CONST2          ENDS

_DATA           SEGMENT WORD PUBLIC 'DATA'
_DATA           ENDS

_BSS            SEGMENT WORD PUBLIC 'BSS'
_BSS            ENDS

                END
.code end
.do end
.if '&arch' eq '32-bit' .do begin
.code begin
&sysper.386
                NAME    hello
                EXTRN   _cstart_ :BYTE
                EXTRN   printf_ :BYTE
                EXTRN   __CHK :BYTE
DGROUP          GROUP   CONST,CONST2,_DATA,_BSS
_TEXT           SEGMENT BYTE PUBLIC USE32 'CODE'
                ASSUME  CS:_TEXT ,DS:DGROUP,SS:DGROUP
                PUBLIC  main_
main_           push    00000008H
                call    near ptr __CHK
                push    offset DGROUP:L1
                call    near ptr printf_
                add     ESP,00000004H
                ret
_TEXT           ENDS

.code break
CONST           SEGMENT DWORD PUBLIC USE32 'DATA'
L1              DB      48H,65H,6cH,6cH,6fH,20H,77H,6fH
                DB      72H,6cH,64H,0aH,00H
CONST           ENDS

CONST2          SEGMENT DWORD PUBLIC USE32 'DATA'
CONST2          ENDS

_DATA           SEGMENT DWORD PUBLIC USE32 'DATA'
_DATA           ENDS

_BSS            SEGMENT DWORD PUBLIC USE32 'BSS'
_BSS            ENDS

                END
.code end
.do end
.do end
.if '&lang' eq 'FORTRAN 77' .do begin
.if '&arch' eq '16-bit' .do begin
.code begin
retf            MACRO   POP_COUNT
                LOCAL   DUMMY
                DUMMY   PROC FAR
                RET     POP_COUNT
                DUMMY   ENDP
                ENDM
                NAME    hello
                EXTRN   _cstart_:WORD
                EXTRN   RT@EndIO:WORD
                EXTRN   RT@OutCHAR:WORD
                EXTRN   RT@IOWrite:WORD
DGROUP          GROUP   CONST,_DATA,_BSS
FMAIN_TEXT      SEGMENT PUBLIC BYTE 'CODE'
                ASSUME  CS:FMAIN_TEXT,DS:DGROUP,SS:DGROUP
                PUBLIC  FMAIN
FMAIN:          push    DX
                call    far ptr RT@IOWrite
                mov     AX,offset DGROUP:L2
                mov     DX,SS
                call    far ptr RT@OutCHAR
                call    far ptr RT@EndIO
                pop     DX
                retf
FMAIN_TEXT      ENDS
.code break
CONST           SEGMENT PUBLIC WORD 'DATA'
L1              LABEL   BYTE
                DB      48H,65H,6cH,6cH,6fH,20H,77H,6fH
                DB      72H,6cH,64H
CONST           ENDS

_DATA           SEGMENT PUBLIC WORD 'DATA'
L2              LABEL   BYTE
                DD      DGROUP:L1
                DB      0bH,00H
_DATA           ENDS

_BSS            SEGMENT PUBLIC WORD 'BSS'
_BSS            ENDS

                END
.code end
.do end
.if '&arch' eq '32-bit' .do begin
.code begin
~.386
                NAME    hello
                EXTRN   __init_387_emulator:WORD
                EXTRN   _cstart_:WORD
                EXTRN   RT@EndIO:WORD
                EXTRN   RT@OutCHAR:WORD
                EXTRN   RT@IOWrite:WORD
DGROUP          GROUP   CONST,_DATA,_BSS
FLAT            GROUP
FMAIN_TEXT      SEGMENT PUBLIC BYTE USE32 'CODE'
                ASSUME  CS:FMAIN_TEXT,DS:DGROUP,SS:DGROUP
                PUBLIC  FMAIN
FMAIN:          call    near ptr RT@IOWrite
                mov     EAX,offset DGROUP:L2
                call    near ptr RT@OutCHAR
                jmp     near ptr RT@EndIO
FMAIN_TEXT      ENDS
.code break
CONST           SEGMENT PUBLIC WORD USE32 'DATA'
L1              LABEL   BYTE
                DB      48H,65H,6cH,6cH,6fH,20H,77H,6fH
                DB      72H,6cH,64H
CONST           ENDS

_DATA           SEGMENT PUBLIC WORD USE32 'DATA'
L2              LABEL   BYTE
                DD      DGROUP:L1
                DB      0bH,00H,00H,00H
_DATA           ENDS

_BSS            SEGMENT PUBLIC WORD USE32 'BSS'
_BSS            ENDS

                END
.code end
.do end
.do end
