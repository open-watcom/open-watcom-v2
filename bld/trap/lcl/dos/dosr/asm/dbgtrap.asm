;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  DOS real mode Trap routines for WATCOM Debugger
;*
;*****************************************************************************


include traps.inc

                NAME    DBGTRAP


        extrn   EnterDebugger:near   ; enter the debugger
        extrn   SetIntVecs_:near     ; set int vectors while in user's program
        extrn   ClrIntVecs_:near     ; clear int vectors while in debugger
        extrn   ChkInt:near          ; check the interrupt number
        extrn   _Flags:byte          ; CPU flags etc

_text segment byte public 'CODE'

WatchTbl   dw   ?,?             ; watch point table address
WatchCnt   dw   ?               ; number of watch points
SaveRegs   dw   ?,?             ; save regs pointer
AreWatching db  0               ; have we got watch points

public  TrapType
TrapType   db   TRAP_NONE       ; trap type
public  TraceRtn
TraceRtn   dw   0               ; routine for int 1 vector


REG_GROUP       struc
        RAX     dw ?
        _EAX    dw ?
        RBX     dw ?
        _EBX    dw ?
        RCX     dw ?
        _ECX    dw ?
        RDX     dw ?
        _EDX    dw ?
        RSI     dw ?
        _ESI    dw ?
        RDI     dw ?
        _EDI    dw ?
        RBP     dw ?
        _EBP    dw ?
        RSP     dw ?
        _ESP    dw ?
        RIP     dw ?
        _EIP    dw ?
        RFL     dw ?
        _EFL    dw ?
                dd ?    ; CR0
                dd ?    ; CR2
                dd ?    ; CR3
        RDS     dw ?
        RES     dw ?
        RSS     dw ?
        RCS     dw ?
        RFS     dw ?
        RGS     dw ?
REG_GROUP       ends



assume  cs:_text

;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
;
; These equ must match those in the FLAGS enum in dosacc.c
F_Is386         equ     0x0001

; These offsets must match the struct in dosacc.c
WP_ADDR         equ     0       ; offset of watch point address
WP_VALUE        equ     4       ; offset of watch point value
WP_SIZE         equ     16      ; size of the watch point structure
;
;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

VALID   equ     1234H           ; value used to validate debugger data segment

public  TrapTypeInit_
TrapTypeInit_ proc       near           ; initialize trap information
        mov     byte ptr CS:TrapType,TRAP_NONE ; no trap at startup time

        ; fall into SetSingleStep
TrapTypeInit_ endp

public  SetSingle386_
SetSingle386_   proc near
        mov     CS:TraceRtn,offset _text:TraceTrap386
        ret
SetSingle386_   endp

public  SetSingleStep_
SetSingleStep_  proc near       ; set interrupt vector 1 at single step routine
        mov     CS:TraceRtn,offset _text:TraceTrap
        ret                     ; return to caller
SetSingleStep_  endp


public  SetWatchPnt_
SetWatchPnt_    proc near       ; set interrupt vector 1 at watch point routine
        mov     CS:TraceRtn,offset _text:WatchTrap
        mov     CS:WatchTbl+0,BX; point at watch point table;
        mov     CS:WatchTbl+2,CX; . . .
        mov     CS:WatchCnt,AX  ; set number of watch points
        ret                     ; return to caller
SetWatchPnt_    endp


public  SetWatch386_
SetWatch386_    proc near       ; set interrupt vector 1 at watch point routine
        call    SetWatchPnt_
        mov     CS:TraceRtn,offset _text:WatchTrap386
        ret                     ; return to caller
SetWatch386_    endp


RetAddr dw  ?,?

public  OvlTrap_
OvlTrap_:                       ; Overlay state change trap
        mov     byte ptr CS:TrapType,TRAP_OVL_CHANGE_LOAD
        pop     CS:RetAddr+0    ; save return offset
        pop     CS:RetAddr+2    ; save return segment
        pushf                   ; fake up an interrupt stack frame
        push    CS:RetAddr+2    ; . . .
        push    CS:RetAddr+0    ; . . .
        test    dl,1
        jz      short DebugTask ; invoke the debugger
        mov     byte ptr CS:TrapType,TRAP_OVL_CHANGE_RET
        jmp     short DebugTask ; invoke the debugger



w386_e: mov     byte ptr CS:TrapType,al                 ; set trap type
        .386
        pop     eax                                     ; restore ax
        jmp     short DebugTask                         ; enter debugger

TraceTrap386:
        push    eax                                     ; save ax
        db      00FH,021H,0F0H                          ; mov eax,dr6
        test    ax,04000H                               ; if trace trap
        je      w386_1                                  ; then
        mov     al,TRAP_TRACE_POINT                     ; - indicate trace trap
        jmp     short w386_e                            ; -
w386_1: test    ax,0Fh                                  ; else if watch trap
        je      w386_2                                  ; - then
        mov     al,TRAP_WATCH_POINT                     ; - indicate watch point
        jmp     short w386_e                            ; -
w386_2:
        pop     eax
        ; no indicator in DR6, fall into TraceTrap and check T-bit
        .8086

TraceTrap:                      ; T-trap trap
        mov     byte ptr CS:TrapType,TRAP_TRACE_POINT ; indicate T-bit trap
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        test    byte ptr 7[BP],1; check to see if the T-bit's off
        pop     BP              ; restore BP
        jnz     DebugTask       ; if T-bit is on then DebugTask

        ; At this point we have just encountered a bug in the 8088
        ; trace trap processing. The return address on the stack is
        ; actually the first instruction of a hardware interrupt handler.
        ; There is another stack frame on top of that indicating where the
        ; handler wants to return to (which is when this routine should really
        ; have been invoked). We are going return to the hardware handler
        ; routine which will eventually return and cause another trace trap.
        ; This second trace trap will come back here and be properly handled.

        iret                  ; return to hardware interrupt handler


public  BptTrap
BptTrap:                        ; come here for breakpoint trap
        mov     byte ptr CS:TrapType,TRAP_BREAK_POINT
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        dec     word ptr 2[BP]  ; decrement IP
        pop     BP              ; restore BP

        ; fall into DebugTask

public  DebugTask
DebugTask:
        push    BX              ; save BX
        push    DS              ; save DS
        lds     BX,dword ptr CS:SaveRegs; point to register save area
        mov     [BX].RES,ES     ; save ES
        mov     [BX].RSI,SI     ; SI
        mov     [BX].RCX,CX     ; CX
        mov     [BX].RAX,AX     ; AX
save_rest:
        call    ClrIntVecs_
        sti                     ; enable interrupts
        mov     [BX].RDX,DX     ; DX
        mov     [BX].RDI,DI     ; DI
        mov     [BX].RBP,BP     ; BP
        mov     [BX].RSS,SS     ; SS
        pop     [BX].RDS        ; save DS
        pop     [BX].RBX        ; save BX
        pop     [BX].RIP        ; save IP
        pop     [BX].RCS        ; save CS
        pop     [BX].RFL        ; save flags
        mov     [BX].RSP,SP     ; SP

; If no 32-bit registers available, just clear the high parts

        xor     SI,SI           ; zero out high parts of 32 bit registers
        mov     [BX]._EAX,SI    ; ...
        mov     [BX]._EBX,SI    ; ...
        mov     [BX]._ECX,SI    ; ...
        mov     [BX]._EDX,SI    ; ...
        mov     [BX]._ESI,SI    ; ...
        mov     [BX]._EDI,SI    ; ...
        mov     [BX]._ESP,SI    ; ...
        mov     [BX]._EBP,SI    ; ...
        mov     [BX]._EIP,SI    ; ...
        mov     [BX]._EFL,SI    ; ...

; If we're on 386+, save high parts of registers, too

                                        ; DS already has correct segment value
        test    DS:_Flags, F_Is386      ; check processor type
        jz      regsSaved               ; jump if 32-bit registers are not available

save386regs:
        .386
        mov     [BX].RFS,FS     ; save FS
        mov     [BX].RGS,GS     ; save GS
        push    EAX             ; save EAX
        rol     EAX, 16
        mov     [BX]._EAX,AX
        mov     EAX, EBX        ; use EAX to preserve BX
        rol     EAX, 16
        mov     [BX]._EBX,AX
        mov     EAX, ECX        ; use EAX to preserve CX
        rol     EAX, 16
        mov     [BX]._ECX,AX
        mov     EAX, EDX        ; use EAX to preserve DX
        rol     EAX, 16
        mov     [BX]._EDX,AX
        mov     EAX, ESI        ; use EAX to preserve SI
        rol     EAX, 16
        mov     [BX]._ESI,AX
        mov     EAX, EDI        ; use EAX to preserve DI
        rol     EAX, 16
        mov     [BX]._EDI,AX
        mov     EAX, ESP        ; use EAX to preserve SP
        rol     EAX, 16
        mov     [BX]._ESP,AX
        mov     EAX, EBP        ; use EAX to preserve BP
        rol     EAX, 16
        mov     [BX]._EBP,AX

; cannot get at EIP so ignore it, must be zero anyway!

        pushfd                  ; use EAX to preserve flags
        pop     EAX
        rol     EAX, 16
        mov     [BX]._EFL,AX
        pop     EAX             ; recover original EAX
        .8086

; End of 32-bit register save code

regsSaved:
        mov     byte ptr CS:AreWatching,0; we aren't watching anymore

        jmp     EnterDebugger   ; enter the debugger


;       DX:AX has load regs pointer & CX:BX has save regs pointer
public  ExitDebugger
ExitDebugger:
        mov     CS:SaveRegs+0,BX; save save regs pointer
        mov     CS:SaveRegs+2,CX; . . .
        mov     SI,AX           ; get pointer to the current reg set
        mov     DS,DX           ; . . .
        test    byte ptr 27[SI],1;are we trace trapping?
        je      not_watch       ; - quit if not
        mov     DI,CS:TraceRtn
        cmp     DI,offset _text:WatchTrap; - are we watch trapping?
        je      watching        ; - quit if not
        cmp     DI,offset _text:WatchTrap386; - are we watch trapping?
        je      watching        ; - quit if not
        jmp     short not_watch ;
watching:
        mov   byte ptr CS:AreWatching,0ffH; - indicate watch points
not_watch:

; Enable use and display of 32-bit registers on 386+

        ; DS already has correct segment value
        test    DS:_Flags, F_Is386
        jnz     restore386regs

; If no 32-bit registers available, only restore 16-bit regs

        cli                     ; interrupts off
        mov     SS,[SI].RSS     ; get their SS
        mov     SP,[SI].RSP     ; and their SP
        sti                     ; interrupts on
        mov     AX,[SI].RAX     ; AX
        mov     BX,[SI].RBX     ; BX
        mov     CX,[SI].RCX     ; CX
        mov     DX,[SI].RDX     ; DX
        mov     DI,[SI].RDI     ; DI
        mov     BP,[SI].RBP     ; BP
        mov     ES,[SI].RES     ; ES
        push    [SI].RFL        ; flags
        push    [SI].RCS        ; CS
        push    [SI].RIP        ; IP
        push    [SI].RDS        ; DS
        mov     SI,[SI].RSI     ; SI
        pop     DS              ; restore DS
        jmp     regsRestored

; Restore the additional segment registers & upper 16-bits of the 32-bit registers

restore386Regs:
        .386
        mov     AX,[SI]._EFL    ; use EAX to restore flags while we have our own stack
        shl     EAX,16
        mov     AX,[SI].RFL     ; EAX now has target's flags
        pushf                   ; save our flags
        cli                     ; just in case!
        push    EAX
        popfd                   ; update target's extended flags
        popf                    ; restore our flags - automatically restores interrupt flag to correct state
        mov     AX,[SI]._ESP
        shl     EAX,16
        mov     AX,[SI].RSP     ; SP
        cli                     ; interrupts off
        mov     SS,[SI].RSS     ; get their SS
        mov     ESP,EAX         ; and their ESP
        sti                     ; interrupts back on

; now running from target's stack
; no stack operations allowed beyond this point
; except those required to return to target program

        mov     AX,SI           ; hang on to SI - we need it!
        mov     SI,[SI]._ESI    ; now restore upper 16 bits of ESI value
        shl     ESI,16          ; SI must wait til later
        mov     SI, AX          ; Continue restoring registers from [SI]
        mov     AX,[SI]._EAX
        shl     EAX,16
        mov     AX,[SI].RAX     ; AX
        mov     BX,[SI]._EBX
        shl     EBX,16
        mov     BX,[SI].RBX     ; BX
        mov     CX,[SI]._ECX
        shl     ECX,16
        mov     CX,[SI].RCX     ; CX
        mov     DX,[SI]._EDX
        shl     EDX,16
        mov     DX,[SI].RDX     ; DX
        mov     DI,[SI]._EDI
        shl     EDI,16
        mov     DI,[SI].RDI     ; DI
        mov     BP,[SI]._EBP
        shl     EBP,16
        mov     BP,[SI].RBP     ; BP
        mov     ES,[SI].RES     ; ES
        mov     FS,[SI].RFS     ; FS
        mov     GS,[SI].RGS     ; GS
        push    [SI].RFL        ; put target flags back on stack for correct interrupt stack structure
        push    [SI].RCS        ; CS
        push    [SI].RIP        ; IP - can do nothing with upper 16-bits of EIP anyway
        push    [SI].RDS        ; DS
        mov     SI,[SI].RSI     ; SI
        pop     DS              ; restore DS
        .8086

; End of 32-bit register restore code

regsRestored:
        cli                     ; interrupts off
        call    SetIntVecs_
        cmp     byte ptr CS:AreWatching,0; check for watch points
        jnz     WatchTrap       ; check watch points first (for software ints)
        iret                    ; return to user's program


watch_trap:                     ; we have a watch point trap
        lds     BX,dword ptr CS:SaveRegs; point at save area
        mov     byte ptr CS:TrapType,TRAP_WATCH_POINT
        jmp     save_rest       ; save rest of the registers & enter debugger

WatchRestart:                   ; restart a watch point after a soft int
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    AX              ; save AX
        pushf                   ; put flags into AX
        pop     AX              ; . . .
        mov     6[BP],AX        ; update flags image
        pop     AX              ; restore AX
        pop     BP              ; restore BP
        jmp     short WatchTrap ; go do rest of watch point

WatchTrap386:                   ; we have a watchpoint trap
        .386
        push    EAX             ; save AX
        db      00FH,021H,0F0H  ; mov eax,dr6
        test    ax,0A00FH       ; check if exception or break
        pop     EAX             ; restore AX
        .8086
        je      WatchTrap       ;
        jmp     TraceTrap386    ;

WatchTrap:                      ; we have a watchpoint trap
        push    BX              ; save BX
        push    DS              ; save DS
do_watch:
        sti                     ; enable interrupts
        lds     BX,dword ptr CS:SaveRegs; get save area
; NYI- this could be fine tuned
        mov     [BX].RAX,AX       ; save AX
        mov     [BX].RCX,CX       ; save CX
        mov     [BX].RSI,SI       ; save SI
        mov     [BX].RES,ES       ; save ES
        lds     SI,dword ptr CS:WatchTbl; get address of watch point table
        mov     CX,CS:WatchCnt  ; get number of watch points
start_loop:                     ; loop
        les     BX,WP_ADDR[SI]  ; - get address of watch point
        mov     AX,ES:[BX]      ; - get low order word
        cmp     AX,WP_VALUE[SI] ; - compare with entry in table
        jne     watch_trap      ; - set watchpoint trap if different
        mov     AX,ES:2[BX]     ; - get high order word
        cmp     AX,WP_VALUE+2[SI];- compare with entry in table
        jne     watch_trap      ; - set watchpoint trap if different
        add     SI,WP_SIZE      ; - point to next entry
        loop    start_loop      ; until --CX = 0
        lds     BX,dword ptr CS:SaveRegs; point at save area
        mov     ES,[BX].RES     ; restore ES
        mov     SI,[BX].RSI     ; restore SI
        mov     CX,[BX].RCX     ; restore CX
        mov     AX,[BX].RAX     ; restore CX
        mov     BX,SP           ; get pointer to interrupt stack frame
        or      byte ptr SS:9[BX],1; make sure the T-bit is on
        lds     BX,SS:4[BX]     ; get return offset and segment
        mov     BX,[BX]         ; get instruction and byte following it
        cmp     BL,0CDH         ; is instruction a software interrupt?
        cli                     ; interrupts off
        je      soft_int        ; handle software interrupt
        cmp     BL,0CCH         ; is it a break point instruction?
        je      brk_point       ; handle breakpoint
cont2:
        pop     DS              ; restore DS
        pop     BX              ; restore BX
        iret                    ; continue execution

soft_int:                       ; software interrupt routine
        ; simulate sofware interrupt action
        push    BP              ; save BP
        mov     BP,SP           ; get pointer to stack frame
        add     word ptr 6[BP],2; add two to the return address
        mov     BL,BH           ; convert interrupt number to offset
        sub     BH,BH           ; . . .
        shl     BX,1            ; . . .
        shl     BX,1            ; . . .
        push    4[BP]           ; save old BX
        push    2[BP]           ; save old DS
        push    0[BP]           ; save old BP
        push    AX              ; save AX
        sub     BP,6            ; point at old BP again
        sub     AX,AX           ; get access to interrupt vector table
        mov     DS,AX           ; . . .
        mov     AX,0[BX]        ; get interrupt offset
        mov     06H[BP],AX      ; new return offset is handler routine
        mov     AX,2[BX]        ; get interrupt segment
        mov     08H[BP],AX      ; new return segment is handler routine
        mov     AX,010H[BP]     ; get original flags
        and     AX,0fcffH       ; turn off T,I bits
        mov     0aH[BP],AX      ; set new flags word
        pop     AX              ; restore AX
        pop     BP              ; restore BP
        shr     BX,1            ; convert offset back to interrupt number
        shr     BX,1            ; . . .
        call    ChkInt          ; do we want to watch this interrupt?
        jc      abort_watch     ; restart watch point routine if so
        jmp     do_watch        ; . . .
abort_watch:
        ; we want the soft interrupt handler to return to the WatchTrap
        ; routine so that it can start watch pointing again
        pop     DS              ; restore DS
        pop     BX              ; restore BX
        sub     SP,6            ; allocate new frame
        push    BP              ; save BP
        mov     BP,SP           ; get access to stack
        push    AX              ; save AX
        mov     AX,0CH[BP]      ; transfer flags
        mov     06H[BP],AX      ; to new stack frame
        mov     AX,0AH[BP]      ; transfer code seg
        mov     04H[BP],AX      ; to new stack frame
        mov     AX,08H[BP]      ; transfer instr pointer
        mov     02H[BP],AX      ; to new stack frame
        mov     0aH[BP],CS      ; set code segment for soft int stack frame
        mov     AX,offset _text:WatchRestart; set intstruction pointer for
        mov     08H[BP],AX      ; int stack frame
        pop     AX              ; restore AX
        pop     BP              ; restore BP
        iret                    ; execute soft int handler

brk_point:                      ; next instruction is a break point
        pop     DS              ; restore DS
        pop     BX              ; restore BX
        mov     byte ptr CS:TrapType,TRAP_BREAK_POINT ; we have a brk point trap
        jmp     DebugTask       ; enter the debugger

_text           ENDS

                END
