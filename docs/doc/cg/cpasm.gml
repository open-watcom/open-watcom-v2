.chap *refid=cpasm In-line Assembly Language
.*
.np
.ix 'in-line assembly language'
.ix 'assembly language' 'in-line'
The chapters entitled :HDREF refid='prg86'. and :HDREF refid='prg386'.
briefly describe the use of the auxiliary pragma to create a sequence
of assembly language instructions that can be placed anywhere
executable C/C++ statements can appear in your source code.
This chapter is devoted to an in-depth look at in-line assembly
language programming.
.np
The reasons for resorting to in-line assembly code are varied:
.begbull
.bull
Speed - You may be interested in optimizing a heavily-used section of
code.
.bull
Size - You may wish to optimize a module for size by replacing a
library function call with a direct system call.
.bull
Architecture - You may want to access certain features of the Intel
x86 architecture that cannot be done so with C/C++ statements.
.endbull
.np
There are also some reasons for not resorting to in-line assembly code.
.begbull
.bull
Portability - The code is not portable to different architectures.
.bull
Optimization - Sometimes an optimizing compiler can do a better job of
arranging the instruction stream so that it is optimal for a
particular processor (such as the 486 or Pentium).
.endbull
.*
.section *refid=inlinenv In-line Assembly Language Default Environment
.*
.np
In next table is description of the default in-line assembler 
environment in dependency on C/C++ compilers CPU switch for x86 target
platform.
..sk 1
..im inlintab
.pc
This environment can be simply changed by appropriate directives.
.pc
.us Note:
.pc
This change is valid only for the block of assembly source code.
After this block, default setting is restored.
.*
.section In-line Assembly Language Tutorial
.*
.np
Doing in-line assembly is reasonably straight-forward with &product.
although care must be exercised.
You can generate a sequence of in-line assembly anywhere in your C/C++
code stream.
The first step is to define the sequence of instructions that you wish
to place in-line.
The auxiliary pragma is used to do this.
Here is a simple example based on a DOS function call that returns a
far pointer to the Double-Byte Character Set (DBCS) encoding table.
.exam begin
extern unsigned short far *dbcs_table( void );
#pragma aux dbcs_table = \
        "mov ax,6300h"   \
        "int 21h"        \
        value   [ds si]  \
        modify  [ax];
.exam end
.np
To set up the DOS call, the AH register must contain the hexadecimal
value "63" (63h).
A DOS function call is invoked by interrupt 21h.
DOS returns a far pointer in DS:SI to a table of byte pairs in the
form (start of range, end of range).
On a non-DBCS system, the first pair will be (0,0).
On a Japanese DBCS system, the first pair will be (81h,9Fh).
.np
With each pragma, we define a corresponding function prototype that
explains the behaviour of the function in terms of C/C++.
Essentially, it is a function that does not take any arguments and
that returns a far pointer to a unsigned short item.
.np
The pragma indicates that the result of this "function" is returned in
DS:SI (value [ds si]).
The pragma also indicates that the AX register is modified by the sequence
of in-line assembly code (modify [ax]).
.np
Having defined our in-line assembly code, let us see how it is used
in actual C code.
.exam begin
#include <stdio.h>

extern unsigned short far *dbcs_table( void );
#pragma aux dbcs_table = \
        "mov ax,6300h"   \
        "int 21h"        \
        value   [ds si]  \
        modify  [ax];
.exam break

void main()
{
    if( *dbcs_table() != 0 ) {
        /*
            we are running on a DOS system that
            supports double-byte characters
        */
        printf( "DBCS supported\n" );
    }
}
.exam end
.np
Before you attempt to compile and run this example, consider this:
The program will not work!  At least, it will not work in most
16-bit memory models.
And it doesn't work at all in 32-bit protected mode using a DOS
extender.
What is wrong with it?
.np
We can examine the disassembled code for this program in order to see
why it does not always work in 16-bit real-mode applications.
.code begin
    if( *dbcs_table() != 0 ) {
        /*
            we are running on a DOS system that
            supports double-byte characters
        */
 0007  b8 00 63                          mov     ax,6300H
 000a  cd 21                             int     21H
 000c  83 3c 00                          cmp     word ptr [si],0000H
 000f  74 0a                             je      L1

        printf( "DBCS supported\n" );
    }
 0011  be 00 00                          mov     si,offset L2
 0014  56                                push    si
 0015  e8 00 00                          call    printf_
 0018  83 c4 02                          add     sp,0002H

}
.code end
.np
After the DOS interrupt call, the DS register has been altered and the
code generator does nothing to recover the previous value.
In the small memory model, the contents of the DS register never
change (and any code that causes a change to DS must save and restore
its value).
It is the programmer's responsibility to be aware of the restrictions
imposed by certain memory models especially with regards to the use of
segmentation registers.
So we must make a small change to the pragma.
.millust begin
extern unsigned short far *dbcs_table( void );
#pragma aux dbcs_table = \
        "push ds"        \
        "mov ax,6300h"   \
        "int 21h"        \
        "mov di,ds"      \
        "pop ds"         \
        value   [di si]  \
        modify  [ax];
.millust end
.np
If we compile and run this example with a 16-bit compiler, it
will work properly.
We can examine the disassembled code for this revised program.
.code begin
    if( *dbcs_table() != 0 ) {
        /*
            we are running on a DOS system that
            supports double-byte characters
        */
 0008  1e                                push    ds
 0009  b8 00 63                          mov     ax,6300H
 000c  cd 21                             int     21H
 000e  8c df                             mov     di,ds
 0010  1f                                pop     ds
 0011  8e c7                             mov     es,di
 0013  26 83 3c 00                       cmp     word ptr es:[si],0000H
 0017  74 0a                             je      L1

        printf( "DBCS supported\n" );
    }
 0019  be 00 00                          mov     si,offset L2
 001c  56                                push    si
 001d  e8 00 00                          call    printf_
 0020  83 c4 02                          add     sp,0002H
.code end
.np
If you examine this code, you can see that the DS register is saved
and restored by the in-line assembly code.
The code generator, having been informed that the far pointer is
returned in (DI:SI), loads up the ES register from DI in order to
reference the far data correctly.
.np
That takes care of the 16-bit real-mode case.
What about 32-bit protected mode?
When using a DOS extender, you must examine the accompanying
documentation to see if the system call that you wish to make is
supported by the DOS extender.
One of the reasons that this particular DOS call is not so clear-cut
is that it returns a 16-bit real-mode segment:offset pointer.
A real-mode pointer must be converted by the DOS extender into a
protected-mode pointer in order to make it useful.
As it turns out, neither the Tenberry Software DOS/4G(W) nor Phar Lap
DOS extenders support this particular DOS call (although others may).
The issues with each DOS extender are complex enough that the relative
merits of using in-line assembly code are not worth it.
We present an excerpt from the final solution to this problem.
.ix 'Phar Lap example'
.ix 'DOS/4GW example'
.ix 'DPMI example'
.tinyexam begin
#ifndef __386__

extern unsigned short far *dbcs_table( void );
#pragma aux dbcs_table = \
        "push ds"        \
        "mov ax,6300h"   \
        "int 21h"        \
        "mov di,ds"      \
        "pop ds"         \
        value   [di si]  \
        modify  [ax];
.tinyexam break
#else

unsigned short far * dbcs_table( void )
{
    union REGPACK       regs;
    static short        dbcs_dummy = 0;

    memset( &regs, 0, sizeof( regs ) );
.tinyexam break
    if( _IsPharLap() ) {
        PHARLAP_block pblock;

        memset( &pblock, 0, sizeof( pblock ) );
        pblock.real_eax = 0x6300;       /* get DBCS vector table */
        pblock.int_num = 0x21;          /* DOS call */
        regs.x.eax = 0x2511;            /* issue real-mode interrupt */
        regs.x.edx = FP_OFF( &pblock ); /* DS:EDX -> parameter block */
        regs.w.ds = FP_SEG( &pblock );
        intr( 0x21, &regs );
        return( firstmeg( pblock.real_ds, regs.w.si ) );
.tinyexam break
    } else if( _IsDOS4G() ) {
        DPMI_block dblock;

        memset( &dblock, 0, sizeof( dblock ) );
        dblock.eax = 0x6300;            /* get DBCS vector table */
        regs.w.ax = 0x300;              /* DPMI Simulate R-M intr */
        regs.h.bl = 0x21;               /* DOS call */
        regs.h.bh = 0;                  /* flags */
        regs.w.cx = 0;                  /* # bytes from stack */
        regs.x.edi = FP_OFF( &dblock );
        regs.x.es = FP_SEG( &dblock );
        intr( 0x31, &regs );
        return( firstmeg( dblock.ds, dblock.esi ) );
.tinyexam break
    } else {
        return( &dbcs_dummy );
    }
}

#endif
.tinyexam end
.np
The 16-bit version will use in-line assembly code but the 32-bit
version will use a C function that has been crafted to work with both
Tenberry Software DOS/4G(W) and Phar Lap DOS extenders.
The
.id firstmeg
function used in the example is shown below.
.ix 'memory' 'first megabyte'
.ix 'real-mode memory'
.code begin
#define REAL_SEGMENT    0x34

void far *firstmeg( unsigned segment, unsigned offset )
{
    void far    *meg1;

    if( _IsDOS4G() ) {
        meg1 = MK_FP( FP_SEG( &meg1 ), ( segment << 4 ) + offset );
    } else {
        meg1 = MK_FP( REAL_SEGMENT, ( segment << 4 ) + offset );
    }
    return( meg1 );
}
.code end
.np
We have taken a brief look at two features of the auxiliary pragma,
the "modify" and "value" attributes.
.np
The "modify" attribute describes those registers that are modified by
the execution of the sequence of in-line code.
You usually have two choices here; you can save/restore registers that
are affected by the code sequence in which case they need not appear
in the modify list or you can let the code generator handle the fact
that the registers are modified by the code sequence.
When you invoke a system function (such as a DOS or BIOS call), you
should be careful about any side effects that the call has on
registers.
If a register is modified by a call and you have not listed it in the
modify list or saved/restored it, this can have a disastrous affect on
the rest of the code in the function where you are including the
in-line code.
.np
The "value" attribute describes the register or registers in
which a value is returned (we use the term "returned", not in the
sense that a function returns a value, but in the sense that a
result is available after execution of the code sequence).
.np
This leads the discussion into the third feature of the auxiliary
pragma, the feature that allows us to place the results of C
expressions into specific registers as part of the "setup"
for the sequence of in-line code.
To illustrate this, let us look at another example.
.exam begin
extern void BIOSSetCurPos( unsigned short __rowcol,
                           unsigned char __page );
#pragma aux BIOSSetCurPos =     \
        "push bp"               \
        "mov ah,2"              \
        "int 10h"               \
        "pop bp"                \
        parm   [dx] [bh]        \
        modify [ah];
.exam end
.np
The "parm" attribute specifies the list of registers into which values
are to be placed as part of the prologue to the in-line code sequence.
In the above example, the "set cursor position" function requires
three pieces of information.
It requires that the cursor row value be placed in the DH register,
that the cursor column value be placed in the DL register, and that
the screen page number be placed in the BH register.
In this example, we have decided to combine the row and column
information into a single "argument" to the function.
Note that the function prototype for
.id BIOSSetCurPos
is important.
It describes the types and number of arguments to be set up for the
in-line code.
It also describes the type of the return value (in this case there is
none).
.np
Once again, having defined our in-line assembly code, let us see how
it is used in actual C code.
.exam begin
#include <stdio.h>

extern void BIOSSetCurPos( unsigned short __rowcol,
                           unsigned char __page );
#pragma aux BIOSSetCurPos =     \
        "push bp"               \
        "mov ah,2"              \
        "int 10h"               \
        "pop bp"                \
        parm   [dx] [bh]        \
        modify [ah];
.exam break

void main()
{
    BIOSSetCurPos( (5 << 8) | 20, 0 );
    printf( "Hello world\n" );
}
.exam end
.np
To see how the code generator set up the register values for the
in-line code, let us take a look at the disassembled code.
.code begin
    BIOSSetCurPos( (5 << 8) | 20, 0 );
 0008  ba 14 05                          mov     dx,0514H
 000b  30 ff                             xor     bh,bh
 000d  55                                push    bp
 000e  b4 02                             mov     ah,02H
 0010  cd 10                             int     10H
 0012  5d                                pop     bp
.code end
.pc
As we expected, the result of the expression for the row and column is
placed in the DX register and the page number is placed in the BH
register.
The remaining instructions are our in-line code sequence.
.np
Although our examples have been simple, you should be able to
generalize them to your situation.
.np
To review, the "parm", "value" and "modify" attributes are used to:
.autonote
.note
convey information to the code generator about the way data values are
to be placed in registers in preparation for the code burst (parm),
.note
convey information to the code generator about the result, if any,
from the code burst (value), and
.note
convey information to the code generator about any side effects to the
registers after the code burst has executed (modify).
It is important to let the code generator know all of the side effects
on registers when the in-line code is executed; otherwise it assumes
that all registers, other than those used for parameters, are
preserved.
In our examples, we chose to push/pop some of the registers that are
modified by the code burst.
.endnote
.*
.section Labels in In-line Assembly Code
.*
.np
.ix 'assembly language' 'labels'
.ix 'in-line assembly language' 'labels'
Labels can be used in in-line assembly code.
Here is an example.
.exam begin
extern void _disable_video( unsigned );
#pragma aux _disable_video =    \
 "again: in al,dx"              \
        "test al,8"             \
        "jz again"              \
        "mov dx,03c0h"          \
        "mov al,11h"            \
        "out dx,al"             \
        "mov al,0"              \
        "out dx,al"             \
        parm [dx]               \
        modify [al dx];
.exam end
.*
.section Variables in In-line Assembly Code
.*
.np
.ix 'assembly language' 'variables'
.ix 'in-line assembly language' 'variables'
To finish our discussion, we provide examples that illustrate the use
of variables in the in-line assembly code.
The following example illustrates the use of static variable
references in the auxiliary pragma.
.exam begin
#include <stdio.h>

static short         _rowcol;
static unsigned char _page;

.exam break
extern void BIOSSetCurPos( void );
#pragma aux BIOSSetCurPos =     \
        "mov  dx,_rowcol"       \
        "mov  bh,_page"         \
        "push bp"               \
        "mov ah,2"              \
        "int 10h"               \
        "pop bp"                \
        modify [ah bx dx];

.exam break
void main()
{
    _rowcol = (5 << 8) | 20;
    _page = 0;
    BIOSSetCurPos();
    printf( "Hello world\n" );
}
.exam end
.np
The only rule to follow here is that the auxiliary pragma must be
defined after the variables are defined.
The in-line assembler is passed information regarding the sizes of
variables so they must be defined first.
.np
If we look at a fragment of the disassembled code, we can see the
result.
.code begin
    _rowcol = (5 << 8) | 20;
 0008  c7 06 00 00 14 05                 mov     word ptr __rowcol,0514H

    _page = 0;
 000e  c6 06 00 00 00                    mov     byte ptr __page,00H

    BIOSSetCurPos();
 0013  8b 16 00 00                       mov     dx,__rowcol
 0017  8a 3e 00 00                       mov     bh,__page
 001b  55                                push    bp
 001c  b4 02                             mov     ah,02H
 001e  cd 10                             int     10H
 0020  5d                                pop     bp
.code end
.np
.ix 'assembly language' 'automatic variables'
.ix 'in-line assembly language' 'automatic variables'
The following example illustrates the use of automatic variable
references in the auxiliary pragma.
Again, the auxiliary pragma must be defined after the variables are
defined so the pragma is placed in-line with the function.
.exam begin
#include <stdio.h>

void main()
{
    short         _rowcol;
    unsigned char _page;

    extern void BIOSSetCurPos( void );
#   pragma aux  BIOSSetCurPos = \
        "mov  dx,_rowcol"       \
        "mov  bh,_page"         \
        "push bp"               \
        "mov ah,2"              \
        "int 10h"               \
        "pop bp"                \
        modify [ah bx dx];
.exam break

    _rowcol = (5 << 8) | 20;
    _page = 0;
    BIOSSetCurPos();
    printf( "Hello world\n" );
}
.exam end
.np
If we look at a fragment of the disassembled code, we can see the
result.
.code begin
    _rowcol = (5 << 8) | 20;
 000e  c7 46 fc 14 05                    mov     word ptr -4H[bp],0514H

    _page = 0;
 0013  c6 46 fe 00                       mov     byte ptr -2H[bp],00H

    BIOSSetCurPos();
 0017  8b 96 fc ff                       mov     dx,-4H[bp]
 001b  8a be fe ff                       mov     bh,-2H[bp]
 001f  55                                push    bp
 0020  b4 02                             mov     ah,02H
 0022  cd 10                             int     10H
 0024  5d                                pop     bp
.code end
.np
You should try to avoid references to automatic variables as
illustrated by this last example.
Referencing automatic variables in this manner causes them to be
marked as volatile and the optimizer will not be able to do a good job
of optimizing references to these variables.
.*
.section In-line Assembly Language using _asm
.*
.np
There is an alternative to &company's auxiliary pragma method for
creating in-line assembly code.
You can use one of the
.kw _asm
or
.kw __asm
keywords to imbed assembly code into the generated code.
The following is a revised example of the cursor positioning example
introduced above.
.exam begin
#include <stdio.h>

void main()
{
    unsigned short _rowcol;
    unsigned char _page;

    _rowcol = (5 << 8) | 20;
    _page = 0;
    _asm {
        mov     dx,_rowcol
        mov     bh,_page
        push    bp
        mov     ah,2
        int     10h
        pop     bp
    };
    printf( "Hello world\n" );
}
.exam end
.np
The assembly language sequence can reference program variables to
retrieve or store results.
There are a few incompatibilities between Microsoft and &company
implementation of this directive.
.begnote
.note __LOCAL_SIZE
.ix '__LOCAL_SIZE'
is not supported by &product..
This is illustrated in the following example.
.exam begin
void main()
{
    int i;
    int j;

    _asm {
        push    bp
        mov     bp,sp
        sub     sp,__LOCAL_SIZE
    };
}
.exam end
.note structure
references are not supported by &product..
This is illustrated in the following example.
.exam begin
#include <stdio.h>

struct rowcol {
    unsigned char col;
    unsigned char row;
};

void main()
{
    struct rowcol _pos;
    unsigned char _page;

    _pos.row = 5;
    _pos.col = 20;
    _page = 0;
    _asm {
        mov     dl,_pos.col
        mov     dh,_pos.row
        mov     bh,_page
        push    bp
        mov     ah,2
        int     10h
        pop     bp
    };
    printf( "Hello world\n" );
}
.exam end
.endnote
.*
.section In-line Assembly Directives and Opcodes
.*
.np
.ix 'assembly language' 'directives'
.ix 'in-line assembly language' 'directives'
.ix 'directives' 'assembly language'
.ix 'assembly language' 'opcodes'
.ix 'in-line assembly language' 'opcodes'
.ix 'opcodes' 'assembly language'
It is not the intention of this chapter to describe assembly-language
programming in any detail.
You should consult a book that deals with this topic.
However, we present a list of the directives, opcodes and register
names that are recognized by the assembler built into the compiler's
auxiliary pragma processor.
.*
:set symbol="wasmfull" value="0".
:INCLUDE file='wasmitms'.
.*
.np
A separate assembler is also included with this product and is
described in the
.book &product Tools User's Guide
