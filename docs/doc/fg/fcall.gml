.if '&machine' eq '8086' .do begin
:set symbol="calref" value="cal86".
:set symbol="calttl" value="16-bit Assembly Language Considerations".
.do end
.el .do begin
:set symbol="calref" value="cal386".
:set symbol="calttl" value="32-bit Assembly Language Considerations".
.do end
.*
.chap *refid=&calref. &calttl.
.*
.if &e'&dohelp eq 0 .do begin
.section Introduction
.do end
.*
.el .do begin
.   .if '&machine' eq '8086' .do begin
.   .   .helppref 16-bit:
.   .do end
.   .el .do begin
.   .   .helppref 32-bit:
.   .do end
.do end
.np
This chapter will deal with the following topics.
.autonote
.note
The memory layout of a program compiled by &cmpname..
.note
The method for passing arguments and returning values.
.note
The two methods for passing floating-point arguments and returning
floating-point values.
.if '&cmpclass' eq 'load-n-go' .do begin
.np
One method is used when :FNAME.&c2cmdup.&exe:eFNAME.,
the version of &product that requires a math coprocessor, is used.
.np
The other method is used when :FNAME.&ccmdup.&exe:eFNAME.,
the version of &product that does not require a math coprocessor, is used.
.do end
.el .do begin
.np
One method is used when one of the &cmpname
.if '&machine' eq '8086' .do begin
"fpi", "fpi87" or "fpi387"
.do end
.if '&machine' eq '80386' .do begin
"fpi", "fpi87" or "fpi287"
.do end
options is specified for the generation of in-line 80x87 instructions.
When the "fpi" option is specified, an 80x87 emulator is included from
a math library if the application includes floating-point operations.
When the
.if '&machine' eq '8086' .do begin
"fpi87" or "fpi387"
.do end
.if '&machine' eq '80386' .do begin
"fpi87" or "fpi287"
.do end
option is used exclusively, the 80x87 emulator will not be included.
.np
The other method is used when the &cmpname "fpc" option is specified.
In this case, the compiler generates calls to floating-point support
routines in the alternate math libraries.
.do end
.endnote
.pc
An understanding of the Intel 80x86 architecture is assumed.
.*
.section Calling Conventions
.*
.np
.ix 'calling conventions'
The following sections describe the method used by &cmpname to pass
arguments.
.np
The FORTRAN 77 language specifically requires that arguments be passed
by reference.
This means that instead of passing the value of an argument, its
address is passed.
This allows a called subprogram to modify the value of the actual
arguments.
The following illustrates the method used to pass arguments.
.pc
.sr c0=&sysin+1
.sr c1=&c0+20
.sr c2=&c1+20
.tb set $
.tb &c0+1 &c1+1
.keep 20
.bx on &c0 &c1 &c2
$Type of Argument$Method Used to Pass Argument
.bx
$non-character constant$address of constant
$non-character expression$address of value of expression
$non-character variable$address of variable
$character constant$address of string descriptor
$character expression$address of string descriptor
$character variable$address of string descriptor
$non-character array$address of array
$non-character array element$address of array
$character array$address of string descriptor
$character array element$address of string descriptor
$character substring$address of string descriptor
$subprogram$address of subprogram
$alternate return specifier$no argument passed
$user-defined structure$address of structure
.bx off
.keep end
.tb set
.tb
.np
When passing a character array as an argument, the string descriptor
contains the address of the first element of the array and the length
of an element of the array.
.np
.ix 'passing arguments'
The address of arguments are either passed in registers or on the
stack.
.if '&machine' eq '80386' .do begin
The registers used to pass the address of arguments to a subprogram
are EAX, EBX, ECX and EDX.
.do end
.if '&machine' eq '8086' .do begin
The registers used to pass the address of arguments to a subprogram
are AX, BX, CX and DX.
.do end
.ix 'passing arguments' 'in registers'
The address of arguments are passed in the following way.
.autonote
.if '&machine' eq '80386' .do begin
.note
The first argument is passed in register EAX, the second argument is
passed in register EDX, the third argument is passed in register EBX,
and the fourth argument is passed in register ECX.
.do end
.if '&machine' eq '8086' .do begin
.note
For memory models with a big data model, address of arguments consist
of a 16-bit offset and a 16-bit segment.
Hence, two registers are required to pass the address of an argument.
The first argument will be passed in registers DX:AX with register DX
containing the segment and register AX containing the offset.
The second argument will be passed in registers CX:BX with register CX
containing the segment and register BX containing the offset.
.note
For memory models with a small data model, address of arguments
consists of only a 16-bit offset into the default data segment.
Hence, only a single register is required to pass the address of an
argument.
The first argument is passed in register AX, the second argument is
passed in register DX, the third argument is passed in register BX,
and the fourth argument is passed in register CX.
.do end
.note
For any remaining arguments, their address is passed on the stack.
Note that addresses of arguments are pushed on the stack from right to
left.
.endnote
.*
.beglevel
.*
.if '&machine' eq '80386' and '&cmpclass' ne 'load-n-go' .do begin
.*
.section Stack-Based Calling Convention
.*
.np
.ix 'stack-based calling convention'
The previous section described a register-based calling convention in
which registers were used to pass arguments to subprograms.
A stack-based calling convention is another method that can be used to
pass arguments.
The calling convention is selected when the "sc" compiler option is
specified.
.np
The most significant difference between the stack-based calling
convention and the register-based calling convention is the way the
arguments are passed.
When using the stack-based calling conventions, no registers are used
to pass arguments.
Instead, all arguments are passed on the stack.
.do end
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.section Processing Function Return Values with &ccmdup.
.*
.do end
.el .do begin
.*
.section Processing Function Return Values with no 80x87
.*
.do end
.*
.np
The way in which function values are returned is also dependent on the
data type of the function.
The following describes the method used to return function values.
.autonote
.note
.bd LOGICAL*1
values are returned in register AL.
.note
.bd LOGICAL*4
values are returned in
.if '&machine' eq '80386' .do begin
register EAX.
.do end
.if '&machine' eq '8086' .do begin
registers DX:AX.
.do end
.note
.bd INTEGER*1
values are returned in register AL.
.note
.bd INTEGER*2
values are returned in register AX.
.note
.bd INTEGER*4
values are returned in
.if '&machine' eq '80386' .do begin
register EAX.
.do end
.if '&machine' eq '8086' .do begin
registers DX:AX.
.do end
.note
.bd REAL*4
values are returned in
.if '&machine' eq '80386' .do begin
register EAX.
.do end
.if '&machine' eq '8086' .do begin
registers DX:AX.
.do end
.note
.bd REAL*8
values are returned in
.if '&machine' eq '80386' .do begin
registers EDX:EAX.
.do end
.if '&machine' eq '8086' .do begin
registers AX:BX:CX:DX.
.do end
.note
For
.bd COMPLEX*8
functions,
space is allocated on the stack by the caller for the return value.
Register
.if '&machine' eq '80386' .do begin
ESI
.do end
.if '&machine' eq '8086' .do begin
SI
.do end
is set to point to the destination of the result.
The called function places the result at the location pointed to by
register
.if '&machine' eq '80386' .do begin
ESI.
.do end
.if '&machine' eq '8086' .do begin
SI.
.do end
.note
For
.bd COMPLEX*16
functions,
space is allocated on the stack by the caller for the return value.
Register
.if '&machine' eq '80386' .do begin
ESI
.do end
.if '&machine' eq '8086' .do begin
SI
.do end
is set to point to the destination of the result.
The called function places the result at the location pointed to by
register
.if '&machine' eq '80386' .do begin
ESI.
.do end
.if '&machine' eq '8086' .do begin
SI.
.do end
.note
For
.bd CHARACTER
functions,
an additional argument is passed.
This argument is the address of the string descriptor for the result.
Note that the address of the string descriptor can be passed in any of
the registers that are used to pass actual arguments.
.note
For functions that return a user-defined structure,
space is allocated on the stack by the caller for the return value.
Register
.if '&machine' eq '80386' .do begin
ESI
.do end
.if '&machine' eq '8086' .do begin
SI
.do end
is set to point to the destination of the result.
The called function places the result at the location pointed to by
register
.if '&machine' eq '80386' .do begin
ESI.
.do end
.if '&machine' eq '8086' .do begin
SI.
.do end
Note that a structure of size 1, 2 or 4 bytes is returned in register
AL, AX or
.if '&machine' eq '80386' .do begin
EAX
.do end
.if '&machine' eq '8086' .do begin
DX:AX
.do end
respectively.
.endnote
.if '&cmpclass' ne 'load-n-go' .do begin
.if '&machine' eq '80386' .do begin
.remark
.ix 'stack-based calling convention'
The way in which a function returns its value does not change when the
stack-based calling convention is used.
.eremark
.do end
.do end
.*
.if '&cmpclass' eq 'load-n-go' .do begin
.*
.section Processing Function Return Values with &cmp2cmdup.
.*
.do end
.el .do begin
.*
.section Processing Function Return Values Using an 80x87
.*
.do end
.*
.np
The following describes the method used to return function values when
your application is compiled using
.if '&cmpclass' eq 'load-n-go' .do begin
&cmp2cmdup..
.do end
.el .do begin
the "fpi87" or "fpi" option.
.do end
.autopoint
.point
For
.bd REAL*4
functions, the result is returned in floating-point register ST(0).
.point
For
.bd REAL*8
functions, the result is returned in floating-point register ST(0).
.point
All other function values are returned in the way described in the
previous section.
.endpoint
.if '&machine' eq '80386' and '&cmpclass' ne 'load-n-go' .do begin
.remark
.ix 'stack-based calling convention'
When the stack-based calling convention is used, floating-point values
are not returned using the 80x87.
.bd REAL*4
values are returned in register EAX.
.bd REAL*8
values are returned in registers EDX:EAX.
.eremark
.do end
.*
.section Processing Alternate Returns
.*
.np
Alternate returns are processed by the caller and are only allowed in
subroutines.
The called subroutine places the value specified in the
.kw RETURN
statement in register
.if '&machine' eq '80386' .do begin
EAX.
.do end
.if '&machine' eq '8086' .do begin
AX.
.do end
Note that the value returned in register
.if '&machine' eq '80386' .do begin
EAX
.do end
.if '&machine' eq '8086' .do begin
AX
.do end
is ignored if there are no alternate return specifiers in the actual
argument list.
.if '&machine' eq '80386' and '&cmpclass' ne 'load-n-go' .do begin
.remark
.ix 'stack-based calling convention'
The way in which a alternate returns are processed does not change
when the stack-based calling convention is used.
.eremark
.do end
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.*
.section Alternate Method of Passing Character Arguments
.*
.np
As previously described, character arguments are passed using string
descriptors.
Recall that a string descriptor contains a pointer to the actual character
data and the length of the character data.
When passing character data, both a pointer and length are required by the
subprogram being called.
When using a string descriptor, this information can be passed using a single
argument, namely the pointer to the string descriptor.
.np
.ix 'options' 'descriptor'
.ix 'descriptor option'
An alternate method of passing character arguments is also supported and is
selected when the "nodescriptor" option is specified.
In this method, the pointer to the character data and the length of the
character data are passed as two separate arguments.
The character argument lengths are appended to the end of the actual argument
list.
.np
Let us consider the following example.
.millust begin
INTEGER A, C
CHARACTER B, D
CALL SUB( A, B, C, D )
.millust end
.np
In the above example, the first argument is of type INTEGER, the second
argument is of type CHARACTER, the third argument is of type INTEGER, and
the fourth argument is of type CHARACTER.
If the character arguments were passed by descriptor, the argument list would
resemble the following.
.autopoint
.point
The first argument would be the address of
.id A.
.point
The second argument would be the address of the string descriptor for
.id B.
.point
The third argument would be the address of
.id C.
.point
The fourth argument would be the address of the string descriptor for
.id D.
.endpoint
.pc
If we specified the "nodescriptor" option, the argument list would be as
follows.
.autopoint
.point
The first argument would be the address of
.id A.
.point
The second argument would be the address of the character data for
.id B.
.point
The third argument would be the address of
.id C.
.point
The fourth argument would be the address of the character data for
.id D.
.point
A hidden argument for the length of
.id B
would be the fifth argument.
.point
A hidden argument for the length of
.id D
would be the sixth argument.
.endpoint
.np
Note that the arguments corresponding to the length of the character
arguments are passed as
.if '&machine' eq '80386' .do begin
INTEGER*4
.do end
.el .do begin
INTEGER*2
.do end
arguments.
.*
.beglevel
.*
.section Character Functions
.*
.np
By default, when a character function is called, a hidden argument is
passed at the end of the actual argument list.
This hidden argument is a pointer to the string descriptor used for the
return value of the character function.
When the alternate method of passing character arguments is specified by
using the "nodescriptor" option, the string descriptor for the return value
is passed to the function as two hidden arguments, similar to the way
character arguments were passed.
However the two hidden arguments for the return value of the character
function are placed at the beginning of the actual argument list.
The first argument is the the pointer to the storage immediately followed by
the size of the storage.
.*
.endlevel
.*
.do end
.endlevel
.*
.if '&cmpclass' ne 'load-n-go' .do begin
.im WMEMLAY
.do end
.*
.section Writing Assembly Language Subprograms
.*
.np
When writing assembly language subprograms, use the following
guidelines.
.autonote
.note
All used registers must be saved on entry and restored on exit except
those used to pass arguments and return values.
.if '&cmpclass' ne 'load-n-go' .do begin
Note that segment registers only have to be saved and restored if you
are compiling your application with the "sr" option.
.do end
.note
The direction flag must be clear before returning to the caller.
.note
.if '&cmpclass' eq 'load-n-go' .do begin
Any
.do end
.el .do begin
In a small code model, any
.do end
segment containing executable code must belong to the segment
"_TEXT" and the class "CODE".
The segment "_TEXT" must have a "combine" type of "PUBLIC".
On entry, register CS contains the segment address of the segment
"_TEXT".
.if '&cmpclass' ne 'load-n-go' .do begin
In a big code model there is no restriction on the naming of segments
which contain executable code.
.do end
.note
.if '&cmpclass' eq 'load-n-go' .do begin
Segment
.do end
.el .do begin
In a small data model, segment
.do end
register DS contains the segment address
of the default data segment (group "DGROUP").
.if '&cmpclass' ne 'load-n-go' .do begin
In a big data model, segment register SS (not DS) contains the segment
address of the default data segment (group "DGROUP").
.do end
.if '&cmpclass' eq 'load-n-go' .do begin
.note
When writing assembly language subprograms,
you must declare them as
.if '&machine' eq '80386' .do begin
"near".
.do end
.el .do begin
"far".
.do end
.do end
.el .do begin
.note
When writing assembly language subprograms for the small code model,
you must declare them as "near".
If you wish to write assembly language subprograms for the big code
model, you must declare them as "far".
.do end
.note
Use the ".8087" pseudo-op so that floating-point constants are in the
correct format.
.note
The called subprogram must remove arguments that were passed on the
stack in the "ret" instruction.
.if '&cmpclass' ne 'load-n-go' .do begin
.note
In general, when naming segments for your code or data, you should
follow the conventions described in the section
entitled "Memory Layout" in this chapter.
.do end
.endnote
.np
Consider the following example.
.mbox begin
      INTEGER HRS, MINS, SECS, HSECS
      CALL GETTIM( HRS, MINS, SECS, HSECS )
      PRINT 100, HRS, MINS, SECS, HSECS
100   FORMAT( 1X,I2.2,':',I2.2,':',I2.2,'.',I2.2 )
      END
.mbox end
.pc
.id GETTIM
is an assembly language subroutine that gets the current time.
It requires four integer arguments.
The arguments are passed by reference so that the subroutine can
return the hour, minute, seconds and hundredths of a second for the
current time.
These arguments will be passed to
.id GETTIM
in the following way.
.autopoint
.point
The address of the first argument will be passed in
.if '&machine' eq '80386' .do begin
register EAX.
.do end
.if '&machine' eq '8086' .do begin
registers DX:AX.
.do end
.point
The address of the second argument will be passed in
.if '&machine' eq '80386' .do begin
register EDX.
.do end
.if '&machine' eq '8086' .do begin
registers CX:BX.
.do end
.point
The address of the third argument will be passed
.if '&machine' eq '80386' .do begin
in register EBX.
.do end
.if '&machine' eq '8086' .do begin
on the stack.
.do end
.point
The address of the fourth argument will be passed
.if '&machine' eq '80386' .do begin
in register ECX.
.do end
.if '&machine' eq '8086' .do begin
on the stack.
.do end
.endpoint
.pc
The following is an assembly language subprogram which implements
.id GETTIM.
.if '&machine' eq '80386' .do begin
.if '&cmpclass' ne 'load-n-go' .do begin
.sk 1
.us Small or Flat Memory Model (small code, small data)
.do end
.code begin

_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
.code break

        public  GETTIM
GETTIM  proc    near
        push    EAX       ; save registers modified by
        push    ECX       ; ... DOS function call
        push    EDX       ; ...
.code break
        mov     AH,2ch    ; set DOS "get time" function
        int     21h       ; issue DOS function call
        movzx   EAX,DH    ; get seconds
        mov     [EBX],EAX ; return seconds
        pop     EBX       ; get address of minutes
.code break
        movzx   EAX,CL    ; get minutes
        mov     [EBX],EAX ; return minutes
        pop     EBX       ; get address of ticks
        movzx   EAX,DL    ; get ticks
.code break
        mov     [EBX],EAX ; return ticks
        pop     EBX       ; get address of hours
        movzx   EAX,CH    ; get hours
        mov     [EBX],EAX ; return hours
        ret               ; return
.code break
GETTIM  endp
_TEXT   ends

        end
.code end
.autonote Notes:
.note
No arguments were passed on the stack so a simple "ret" instruction is
used to return to the caller.
If a single argument was passed on the stack, a "ret 4" instruction
would be required to return to the caller.
.note
Registers EAX, EBX, ECX and EDX were not saved and restored since they
were used to pass arguments.
.endnote
.do end
.if '&machine' eq '8086' .do begin
.if '&cmpclass' ne 'load-n-go' .do begin
.sk 1
.us Large Memory Model (big code, big data)
.do end
.code begin

GETTIM_TEXT segment byte public 'CODE'
        assume  CS:GETTIM_TEXT
.code break

        public  GETTIM
GETTIM  proc    far
        push    DI          ; save register(s)
        push    ES          ; ...
.code break
        push    DS          ; ...
        push    BP          ; get addressability to arguments
        mov     BP,SP       ; ...
        mov     ES,DX       ; ES:DI points to hours
        mov     DI,AX       ; ...
.code break
        mov     DS,CX       ; DS:BX points to minutes
        mov     AH,2ch      ; set DOS "get time" function
        int     21h         ; issue DOS function call
        mov     AL,CH       ; get hours
        cbw                 ; ...
.code break
        mov     ES:[DI],AX  ; return hours
        sub     AX,AX       ; ...
        mov     ES:2[DI],AX ; ...
        mov     AL,CL       ; get minutes
        cbw                 ; ...
.code break
        mov     [BX],AX     ; return minutes
        sub     AX,AX       ; ...
        mov     2[BX],AX    ; ...
        mov     DS,14[BP]   ; get address of seconds
        mov     DI,12[BP]   ; ...
.code break
        mov     AL,DH       ; get seconds
        cbw                 ; ...
        mov     [DI],AX     ; return seconds
        sub     AX,AX       ; ...
        mov     2[DI],AX    ; ...
.code break
        mov     DS,18[BP]   ; get address of ticks
        mov     DI,16[BP]   ; ...
        mov     AL,DL       ; get ticks
        cbw                 ; ...
.code break
        cwd                 ; ...
        mov     [DI],AX     ; return ticks
        mov     2[DI],DX    ; ...
        pop     BP          ; restore register(s)
        pop     DS          ; ...
        pop     ES          ; ...
.code break
        pop     DI          ; ...
        ret     8           ; return
GETTIM  endp
GETTIM_TEXT ends

        end
.code end
.autonote Notes:
.note
Two arguments were passed on the stack so a "ret 8" instruction is
used to return to the caller.
.note
Registers AX, BX, CX and DX were not saved and restored since they
were used to pass arguments.
However, registers DS, ES, DI and BP were modified in the subprogram
and hence must be saved and restored.
.endnote
.np
Let us look at the stack upon entry to
.id GETTIM.
.np
.keep 14
.us Large Model (big code, big data)
.millust begin
Offset
    0     +----------------+ <- SP points here
          | return address |
    4     +----------------+
          | argument #3    |
    8     +----------------+
          | argument #4    |
   12     +----------------+
          |                |
.millust end
.autonote Notes:
.note
The top element of the stack is a segment/offset pair forming a 32-bit
return address.
Hence, the third argument will be at offset 4 from the top of the
stack and the fourth argument at offset 8.
.endnote
.np
Register SP cannot be used as a base register to address the arguments
on the stack.
.ix 'addressing arguments'
Register BP is normally used to address arguments on the stack.
Upon entry to the subroutine, registers that are modified (except
those used to pass arguments) are saved and register BP is set to
point to the stack.
After performing this prologue sequence, the stack looks like this.
.np
.keep 22
.us Large Model (big code, big data)
.millust begin
Offset
    0     +----------------+ <- BP and SP point here
          | saved BP       |
    2     +----------------+
          | saved DS       |
    4     +----------------+
          | saved ES       |
    6     +----------------+
          | saved DI       |
    8     +----------------+
          | return address |
   12     +----------------+
          | argument #3    |
   16     +----------------+
          | argument #4    |
   20     +----------------+
          |                |
.millust end
.pc
As the above diagram shows, the third argument is at offset 12 from
register BP and the fourth argument is at offset 16.
.do end
.*
.beglevel
.*
.if '&machine' eq '80386' and '&cmpclass' ne 'load-n-go' .do begin
.*
.section Using the Stack-Based Calling Convention
.*
.np
.ix 'stack-based calling convention' 'writing assembly language subprograms'
When writing assembly language subprograms that use the stack-based
calling convention, use the following guidelines.
.autopoint
.point
All used registers, except registers EAX, ECX and EDX must be saved on
entry and restored on exit.
Also, if segment registers ES and DS are used, they must be saved on
entry and restored on exit.
Note that segment registers only have to be saved and restored if you
are compiling your application with the "sr" option.
.point
The direction flag must be clear before returning to the caller.
.point
In a small code model, any segment containing executable code must
belong to the segment "_TEXT" and the class "CODE".
The segment "_TEXT" must have a "combine" type of "PUBLIC".
On entry, register CS contains the segment address of the segment
"_TEXT".
In a big code model there is no restriction on the naming of segments
which contain executable code.
.point
In a small data model, segment register DS contains the segment
address of the default data segment (group "DGROUP").
In a big data model, segment register SS (not DS) contains the segment
address of the default data segment (group "DGROUP").
.point
When writing assembly language subprograms for the small code model,
you must declare them as "near".
If you wish to write assembly language subprograms for the big code
model, you must declare them as "far".
.point
Use the ".8087" pseudo-op so that floating-point constants are in the
correct format.
.point
The caller will remove arguments that were passed on the stack.
.point
In general, when naming segments for your code or data, you should
follow the conventions described in the section
entitled "Memory Layout" in this chapter.
.endpoint
.np
Consider the following example.
.mbox begin
      INTEGER HRS, MINS, SECS, HSECS
      CALL GETTIM( HRS, MINS, SECS, HSECS )
      PRINT 100, HRS, MINS, SECS, HSECS
100   FORMAT( 1X,I2.2,':',I2.2,':',I2.2,'.',I2.2 )
      END
.mbox end
.pc
.id GETTIM
is an assembly language subroutine that gets the current time.
It requires four integer arguments.
The arguments are passed by reference so that the subroutine can
return the hour, minute, seconds and hundredths of a second for the
current time.
These arguments will be passed to
.id GETTIM
on the stack.
.pc
The following is an assembly language subprogram which implements
.id GETTIM.
.sk 1
.us Small or Flat Memory Model (small code, small data)
.code begin

_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
.code break

        public  GETTIM
GETTIM  proc    near
        push    EBP        ; save registers
        mov     EBP,ESP    ; ...
.code break
        push    ESI        ; ...
        mov     AH,2ch     ; set DOS "get time" function
        int     21h        ; issue DOS function call
        movzx   EAX,CH     ; get hours
        mov     ESI,8[EBP] ; get address of hours
.code break
        mov     [ESI],EAX  ; return hours
        movzx   EAX,CL     ; get minutes
        mov     ESI,12[BP] ; get address of minutes
        mov     [ESI],EAX  ; return minutes
        movzx   EAX,DH     ; get seconds
.code break
        mov     ESI,16[BP] ; get address of seconds
        mov     [ESI],EAX  ; return seconds
        movzx   EAX,DL     ; get ticks
        mov     ESI,20[BP] ; get address of ticks
        mov     [ESI],EAX  ; return ticks
.code break
        pop     ESI        ; restore registers
        mov     ESP,EBP    ; ...
        pop     EBP        ; ...
        ret                ; return
.code break
GETTIM  endp
_TEXT   ends

        end
.code end
.autonote Notes:
.note
The four arguments that were passed on the stack will be removed by
the caller.
.note
Registers ESI and EBP were saved and restored since they
were used in
.id GETTIM.
.endnote
.np
Let us look at the stack upon entry to
.id GETTIM.
.millust begin
Offset
    0     +----------------+ <- ESP points here
          | return address |
    4     +----------------+
          | argument #1    |
    8     +----------------+
          | argument #2    |
   12     +----------------+
          | argument #3    |
   16     +----------------+
          | argument #4    |
   20     +----------------+
          |                |
.millust end
.autonote Notes:
.note
The top element of the stack is a the 32-bit return address.
The first argument is at offset 4 from the top of the stack, the
second argument at offset 8, the third argument at offset 12, and the
fourth argument at offset 16.
.endnote
.np
.ix 'addressing arguments'
Register EBP is normally used to address arguments on the stack.
Upon entry to the subroutine, registers that are modified (except
registers EAX, ECX and EDX) are saved and register EBP is set to point
to the stack.
After performing this prologue sequence, the stack looks like this.
.millust begin
Offset from EBP
   -4     +----------------+ <- ESP point here
          | saved ESI      |
    0     +----------------+ <- EBP point here
          | saved EBP      |
    4     +----------------+
          | return address |
    8     +----------------+
          | argument #1    |
   12     +----------------+
          | argument #2    |
   16     +----------------+
          | argument #3    |
   20     +----------------+
          | argument #4    |
   24     +----------------+
          |                |
.millust end
.pc
As the above diagram shows, the first argument is at offset 8 from
register EBP,
the second argument is at offset 12,
the third argument is at offset 16,
and the fourth argument is at offset 20.
.do end
.*
.section Returning Values from Assembly Language Functions
.*
.np
The following illustrates the way function values are to be returned
from assembly language functions.
.if '&machine' eq '80386' and '&cmpclass' ne 'load-n-go' .do begin
.remark
.ix 'stack-based calling convention'
The way in which a function returns its value does not change when the
stack-based calling convention is used.
.eremark
.do end
.if '&machine' eq '8086' .do begin
.autopoint
.point
A
.bd LOGICAL*1
function.
.code begin
L1_TEXT segment byte public 'CODE'
        assume  CS:L1_TEXT
        public  L1
L1      proc    far
.code break
        mov     AL,1
        ret
L1      endp
L1_TEXT ends
        end
.code end
.pc
.point
A
.bd LOGICAL*4
function.
.code begin
L4_TEXT segment byte public 'CODE'
        assume  CS:L4_TEXT
        public  L4
L4      proc    far
        mov     AX,0
.code break
        cwd
        ret
L4      endp
L4_TEXT ends
        end
.code end
.pc
.point
An
.bd INTEGER*1
function.
.code begin
I1_TEXT segment byte public 'CODE'
        assume  CS:I1_TEXT
        public  I1
I1      proc    far
.code break
        mov     AL,73
        ret
I1      endp
I1_TEXT ends
        end
.code end
.pc
.point
An
.bd INTEGER*2
function.
.code begin
I2_TEXT segment byte public 'CODE'
        assume  CS:I2_TEXT
        public  I2
I2      proc    far
        mov     AX,7143
.code break
        ret
I2      endp
I2_TEXT ends
        end
.code end
.pc
.point
An
.bd INTEGER*4
function.
.code begin
I4_TEXT segment byte public 'CODE'
        assume  CS:I4_TEXT
        public  I4
I4      proc    far
        mov     AX,383
.code break
        cwd
        ret
I4      endp
I4_TEXT ends
        end
.code end
.pc
.point
A
.bd REAL*4
function.
.code begin
.li .8087

DGROUP  group R4_DATA

R4_TEXT segment byte public 'CODE'
        assume  CS:R4_TEXT
        assume  SS:DGROUP
        public  R4
R4      proc    far
.code break
        mov     AX,word ptr SS:R4Val
        mov     DX,word ptr SS:R4Val+2
        ret
R4      endp
R4_TEXT ends
.code break

R4_DATA segment byte public 'DATA'
R4Val   dd 1314.3
R4_DATA ends

        end
.code end
.pc
.point
A
.bd REAL*8
function.
.code begin
.li .8087

DGROUP  group R8_DATA

R8_TEXT segment byte public 'CODE'
        assume  CS:R8_TEXT
        assume  SS:DGROUP
        public  R8
R8      proc    far
        mov     DX,word ptr SS:R8Val
.code break
        mov     CX,word ptr SS:R8Val+2
        mov     BX,word ptr SS:R8Val+4
        mov     AX,word ptr SS:R8Val+6
        ret
R8      endp
R8_TEXT ends
.code break

R8_DATA segment byte public 'DATA'
R8Val   dq 103.3
R8_DATA ends

        end
.code end
.pc
.point
A
.bd COMPLEX*8
function.
.code begin
.li .8087

DGROUP  group C8_DATA

C8_TEXT segment byte public 'CODE'
        assume  CS:C8_TEXT
        assume  SS:DGROUP
.code break
        public  C8
C8      proc    far
        push    DI
        push    ES
        xchg    DI,SI
        push    SS
.code break
        pop     ES
        mov     SI,offset SS:C8Val
        movsw
        movsw
.code break
        movsw
        movsw
        pop     ES
        pop     DI
        ret
.code break
C8      endp
C8_TEXT ends

.code break
C8_DATA segment byte public 'DATA'
C8Val   dd 2.2
        dd 2.2
C8_DATA ends

        end
.code end
.pc
.point
A
.bd COMPLEX*16
function.
.code begin
.li .8087

DGROUP  group C16_DATA

C16_TEXT segment byte public 'CODE'
        assume  CS:C16_TEXT
        assume  SS:DGROUP
.code break
        public  C16
C16     proc    far
        push    DI
        push    ES
        push    CX
.code break
        xchg    DI,SI
        push    SS
        pop     ES
        mov     SI,offset SS:C16Val
        mov     CX,8
.code break
        repe    movsw
        pop     CX
        pop     ES
        pop     DI
        ret
.code break
C16     endp
C16_TEXT ends

.code break
C16_DATA segment byte public 'DATA'
C16Val  dq 3.3
        dq 3.3
C16_DATA ends

        end
.code end
.pc
.point
A
.bd CHARACTER
function.
.code begin
CHR_TEXT segment byte public 'CODE'
        assume  CS:CHR_TEXT
        public  CHR
CHR     proc    far
        push    DI
.code break
        push    ES
        mov     ES,DX
        mov     DI,AX
        les     DI,ES:[DI]
.code break
        mov     byte ptr ES:[DI],'F'
        pop     ES
        pop     DI
        ret
.code break
CHR     endp
CHR_TEXT ends

        end
.code end
.pc
.point
A function returning a user-defined structure.
.code begin
DGROUP  group STRUCT_DATA

STRUCT_TEXT segment byte public 'CODE'
        assume  CS:STRUCT_TEXT
        assume  SS:DGROUP
.code break
        public  C16
STRUCT  proc    far
        push    DI
        push    ES
.code break
        push    CX
        xchg    DI,SI
        push    SS
        pop     ES
        mov     SI,offset SS:StructVal
.code break
        mov     CX,4
        repe    movsw
        pop     CX
        pop     ES
        pop     DI
        ret
.code break
STRUCT  endp
STRUCT_TEXT ends

.code break
STRUCT_DATA segment byte public 'DATA'
StructVal dd 7
          dd 3
STRUCT_DATA ends

        end
.code end
.endpoint
.np
If you are using
.if '&cmpclass' eq 'load-n-go' .do begin
&cmp2cmdup,
.do end
.el .do begin
an 80x87 to return floating-point values,
.do end
only assembly language functions of type
.bd REAL*4
and
.bd REAL*8
need to be modified.
.autopoint
.point
A
.bd REAL*4
function using an 80x87.
.code begin
.li .8087

DGROUP  group R4_DATA

R4_TEXT segment byte public 'CODE'
        assume  CS:R4_TEXT
        assume  SS:DGROUP
.code break
        public  R4
R4      proc    far
        fld     dword ptr SS:R4Val
        ret
R4      endp
R4_TEXT ends
.code break

R4_DATA segment byte public 'DATA'
R4Val   dd 1314.3
R4_DATA ends

        end
.code end
.point
A
.bd REAL*8
function using an 80x87.
.code begin
.li .8087

DGROUP  group R8_DATA

R8_TEXT segment byte public 'CODE'
        assume  CS:R8_TEXT
        assume  SS:DGROUP
.code break
        public  R8
R8      proc    far
        fld     qword ptr SS:R8Val
        ret
R8      endp
R8_TEXT ends
.code break

R8_DATA segment byte public 'DATA'
R8Val   dq 103.3
R8_DATA ends

        end
.code end
.endpoint
.autonote Notes:
.note
The ".8087" pseudo-op must be specified so that all floating-point
constants are generated in 8087 format.
.note
When returning values on the stack, remember to use a segment override
to the stack segment (SS).
.endnote
.do end
.if '&machine' eq '80386' .do begin
.autopoint
.point
A
.bd LOGICAL*1
function.
.code begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  L1
L1      proc    near
.code break
        mov     AL,1
        ret
L1      endp
_TEXT   ends
        end
.code end
.pc
.point
A
.bd LOGICAL*4
function.
.code begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  L4
L4      proc    near
.code break
        mov     EAX,0
        ret
L4      endp
_TEXT   ends
        end
.code end
.pc
.point
An
.bd INTEGER*1
function.
.code begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  I1
I1      proc    near
.code break
        mov     AL,73
        ret
I1      endp
_TEXT   ends
        end
.code end
.pc
.point
An
.bd INTEGER*2
function.
.code begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  I2
I2      proc    near
.code break
        mov     AX,7143
        ret
I2      endp
_TEXT   ends
        end
.code end
.pc
.point
An
.bd INTEGER*4
function.
.code begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  I4
I4      proc    near
.code break
        mov     EAX,383
        ret
I4      endp
_TEXT   ends
        end
.code end
.pc
.point
A
.bd REAL*4
function.
.code begin
.li .8087

DGROUP  group R4_DATA

_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        assume  DS:DGROUP
        public  R4
R4      proc    near
.code break
        mov     EAX,dword ptr R4Val
        ret
R4      endp
_TEXT   ends

.code break
R4_DATA segment byte public 'DATA'
R4Val   dd 1314.3
R4_DATA ends

        end
.code end
.pc
.point
A
.bd REAL*8
function.
.code begin
.li .8087

DGROUP  group R8_DATA

_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        assume  DS:DGROUP
        public  R8
.code break
R8      proc    near
        mov     EAX,dword ptr R8Val
        mov     EDX,dword ptr R8Val+4
        ret
R8      endp
_TEXT   ends

.code break
R8_DATA segment byte public 'DATA'
R8Val   dq 103.3
R8_DATA ends

        end
.code end
.pc
.point
A
.bd COMPLEX*8
function.
.code begin
.li .8087

DGROUP  group C8_DATA

_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        assume  DS:DGROUP
        public  C8
.code break
C8      proc    near
        push    EAX
        mov     EAX,C8Val
        mov     [ESI],EAX
        mov     EAX,C8Val+4
        mov     4[ESI],EAX
        pop     EAX
        ret
.code break
C8      endp
_TEXT   ends

C8_DATA segment byte public 'DATA'
C8Val   dd 2.2
        dd 2.2
C8_DATA ends

        end
.code end
.pc
.point
A
.bd COMPLEX*16
function.
.code begin
.li .8087

DGROUP  group C16_DATA

_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        assume  DS:DGROUP
.code break
        public  C16
C16     proc    near
        push    EAX
        mov     EAX,dword ptr C16Val
        mov     [ESI],EAX
.code break
        mov     EAX,dword ptr C16Val+4
        mov     4[ESI],EAX
        mov     EAX,dword ptr C16Val+8
        mov     8[ESI],EAX
.code break
        mov     EAX,dword ptr C16Val+12
        mov     12[ESI],EAX
        pop     EAX
        ret
C16     endp
_TEXT   ends
.code break

C16_DATA segment byte public 'DATA'
C16Val  dq 3.3
        dq 3.3
C16_DATA ends

        end
.code end
.pc
.point
A
.bd CHARACTER
function.
.code begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  CHR
CHR     proc    near
        push    EAX
.code break
        mov     EAX,[EAX]
        mov     byte ptr [EAX],'F'
        pop     EAX
        ret
CHR     endp
_TEXT   ends
.code break

        end
.code end
.if '&cmpclass' ne 'load-n-go' .do begin
.np
Remember, if you are using stack calling conventions (i.e., you
specified the "sc" compiler option), arguments will be passed on the
stack.
The above character function must be modified as follows.
.code begin
_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        public  CHR
CHR     proc    near
        push    EAX
.code break
        mov     EAX,8[ESP]
        mov     EAX,[EAX]
        mov     byte ptr [EAX],'F'
        pop     EAX
        ret
CHR     endp
_TEXT   ends
.code break

        end
.code end
.do end
.pc
.point
A function returning a user-defined structure.
.code begin
DGROUP  group STRUCT_DATA

_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        assume  DS:DGROUP
        public  STRUCT
.code break
STRUCT  proc    near
        push    EAX
        mov     EAX,dword ptr StructVal
        mov     [ESI],EAX
.code break
        mov     EAX,dword ptr StructVal+4
        mov     4[ESI],EAX
        pop     EAX
        ret
.code break
STRUCT  endp
_TEXT   ends

.code break
STRUCT_DATA segment byte public 'DATA'
StructVal dd 7
          dd 3
STRUCT_DATA ends

        end
.code end
.endpoint
.np
If you are using an 80x87 to return floating-point values, only
.bd REAL*4
and
.bd REAL*8
assembly language functions need to be modified.
.if '&machine' eq '80386' and '&cmpclass' ne 'load-n-go' .do begin
.ix 'stack-based calling convention'
.us Remember, this does not apply if you are using the stack-based
.us calling convention.
.do end
.autopoint
.point
A
.bd REAL*4
function using an 80x87.
.code begin
.li .8087

DGROUP  group R4_DATA

_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        assume  DS:DGROUP
        public  R4
.code break
R4      proc    near
        fld     dword ptr R4Val
        ret
R4      endp
_TEXT   ends

.code break
R4_DATA segment byte public 'DATA'
R4Val   dd 1314.3
R4_DATA ends

        end
.code end
.point
A
.bd REAL*8
function using an 80x87.
.code begin
.li .8087

DGROUP  group R8_DATA

_TEXT   segment byte public 'CODE'
        assume  CS:_TEXT
        assume  DS:DGROUP
.code break
        public  R8
R8      proc    near
        fld     qword ptr R8Val
        ret
R8      endp
_TEXT   ends

.code break
R8_DATA segment byte public 'DATA'
R8Val   dq 103.3
R8_DATA ends

        end
.code end
.endpoint
.do end
.np
The following is an example of a &cmpname program calling the above
assembly language subprograms.
.millust begin
      logical l1*1, l4*4
      integer i1*1, i2*2, i4*4
      real r4*4, r8*8
      complex c8*8, c16*16
      character chr
      structure /coord/
          integer x, y
      end structure
      record /coord/ struct
      print *, l1()
      print *, l4()
      print *, i1()
      print *, i2()
      print *, i4()
      print *, r4()
      print *, r8()
      print *, c8()
      print *, c16()
      print *, chr()
      print *, struct()
      end
.millust end
.endlevel
