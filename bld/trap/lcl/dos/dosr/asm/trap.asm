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
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


include struct.inc
include traps.inc

        extrn       _DOS_major:byte
        extrn       _DOS_minor:byte
        extrn       _BoundAppLoading:byte

CONST           segment byte public 'DATA'
CONST           ends

DGROUP group CONST

_TEXT           segment byte public 'CODE'



                extrn   DebugTask               :near
                extrn   BptTrap                 :near
                extrn   ExitDebugger            :near

                extrn   TraceRtn                :word
                extrn   TrapType                :byte

                extrn   "C", FPUExpand          :near
                extrn   "C", FPUContract        :near

                ; these macros assume ES is pointing at the interrupt vector
                ; segment and that AX and BX are available for use

SaveIntr        macro   intr,loc
        ife     dummy
                if      intr+1
                mov     BX,4*intr               ; get vector offset
                else
                xor     BH,BH
                add     BX,BX
                add     BX,BX
                endif
                mov     AX,ES:0[BX]             ; get interrupt offset
                mov     loc+0,AX                ; save it
                mov     AX,ES:2[BX]             ; get interrupt segment
                mov     loc+2,AX                ; save it
        endif
                endm

SetIntr         macro   intr,loc
        ife     dummy
                if      intr+1
                mov     BX,4*intr               ; get vector offset
                else
                xor     BH,BH
                add     BX,BX
                add     BX,BX
                endif
                mov     word ptr ES:0[BX],offset loc; set interrupt offset
                mov     ES:2[BX],CS             ; set interrupt segment
        endif
                endm

SwapIntr        macro   intr,loc
        ife     dummy
                if      intr+1
                mov     BX,4*intr               ; get vector offset
                else
                xor     BH,BH
                add     BX,BX
                add     BX,BX
                endif
                mov     AX,word ptr loc+0       ; get new interrupt offset
                xchg    ES:0[BX],AX             ; swap offsets
                mov     word ptr loc+0, AX      ; . . .
                mov     AX,word ptr loc+2       ; get new interrupt segment
                xchg    ES:2[BX],AX             ; swap segments
                mov     word ptr loc+2,AX       ; . . .
        endif
                endm

RestIntr        macro   intr,loc
        ife     dummy
                if      intr+1
                mov     BX,4*intr               ; get vector offset
                else
                xor     BH,BH
                add     BX,BX
                add     BX,BX
                endif
                mov     AX,loc+0                ; get old interrupt offset
                mov     ES:0[BX],AX             ; restore it
                mov     AX,loc+2                ; get old interrupt segment
                mov     ES:2[BX],AX             ; restore it
        endif
                endm


assume  CS:_TEXT


WATCH_DEPTH     equ     6+2+2   ; additional stack in watch point rtn

;       Code Segment variables

        public _SegmentChain
_SegmentChain   dw      ?
OurSP           dw      ?
OurSS           dw      ?
DbgPSP          dw      ?
CurrPSP         dw      ?
TaskPSP         dw      0


SaveIntSP       dw      ?
SaveIntSS       dw      ?
InDos           dd      ?


SaveBrkHndlr    dw      ?,?
SaveCrtErrHndlr dw      ?,?
SaveBusyWait    dw      ?,?


SaveVects       db      1024 dup(?)

TraceVec        equ     word ptr cs:(SaveVects+01H*4)
BptVec          equ     word ptr cs:(SaveVects+03H*4)
PrgIntVec       equ     word ptr cs:(SaveVects+05H*4)
TimerIntVec     equ     word ptr cs:(SaveVects+08H*4)
KbdIntVec       equ     word ptr cs:(SaveVects+09H*4)
SysServVec      equ     word ptr cs:(SaveVects+15H*4)
BrkVec          equ     word ptr cs:(SaveVects+1bH*4)
PrgTermVec      equ     word ptr cs:(SaveVects+20H*4)
OSIntVec        equ     word ptr cs:(SaveVects+21H*4)
DOSBrkVec       equ     word ptr cs:(SaveVects+23H*4)
CrtErrVec       equ     word ptr cs:(SaveVects+24H*4)
ResTermVec      equ     word ptr cs:(SaveVects+27H*4)
BusyWaitVec     equ     word ptr cs:(SaveVects+28H*4)

UsrInt          db      0
DOSCount        db      0
BrkStatus       db      ?


public          InitVectors_
InitVectors_    proc    near
                push    es              ; save registers
                push    ds
                push    si
                push    di
                push    dx
                push    cx
                push    bx
                mov     ax,seg DGROUP
                mov     ds,ax
                assume DS:DGROUP

                mov     AH,30H          ; get version number
                int     21H
                mov     DS:_DOS_major,AL ; and save it away
                mov     DS:_DOS_minor,AH
                ; save interrupt vectors
                mov     si,cs
                mov     es,si
                sub     si,si
                mov     ds,si
                mov     di,offset SaveVects
                mov     cx,1024/2
                cli
                rep     movsw
                sti
                mov     ax,BusyWaitVec  ; set up busy wait
                mov     CS:SaveBusyWait,ax      ; swap location
                mov     ax,BusyWaitVec+2        ; ...
                mov     CS:SaveBusyWait+2,ax    ; ...


                call    GetPSP          ; get debugger PSP
                mov     CS:DbgPSP,AX    ; and save it away

                call    near ptr ClrIntVecs_

                mov     AH,34H          ; get InDos flag
                int     21H             ; ...
                mov     word ptr CS:InDos+0,BX; save flag pointer away
                mov     word ptr CS:InDos+2,ES; ...

                mov     AX,3300H        ; request Ctrl/Brk checking status
                int     21H
                mov     CS:BrkStatus,DL
                mov     AX,3301H        ; set to check for all DOS calls
                mov     DL,1
                int     21H

                pop     bx              ; restore registers
                pop     cx
                pop     dx
                pop     di
                pop     si
                pop     ds
                pop     es
                ret
InitVectors_    endp


;       DX:AX has load regs pointer & CX:BX has save regs pointer
public          RunProg_     ; run the program and return termination condition
RunProg_        proc    near
                push    ES
                push    DS
                push    SI
                push    DI
                push    BP
                mov     CS:UsrInt,0     ; no pending user interrupts
                mov     CS:OurSS,SS     ; save SS
                mov     CS:OurSP,SP     ; save SP
                jmp     ExitDebugger    ; run program
public EnterDebugger
EnterDebugger:  cli                     ; interrupts off
                mov     SS,OurSS        ; restore stack segment
                mov     SP,CS:OurSP     ; restore stack pointer
                sti
                cld
                pop     BP              ; restore everything
                pop     DI
                pop     SI
                pop     DS
                pop     ES
                mov     AL,TRAP_SKIP
                xchg    AL,CS:TrapType  ; get cause of termination
                cbw
                ret                     ; return to caller
RunProg_        endp


public          FiniVectors_
FiniVectors_    proc    near
                push    es              ; save registers
                push    ds
                push    si
                push    di
                push    dx
                push    cx

                mov     AX,3301H        ; restore Ctrl/Brk checking state
                mov     DL,CS:BrkStatus
                int     21H

                ; restore interrupt vectors
                mov     di,cs
                mov     ds,di
                sub     di,di
                mov     es,di
                mov     si,offset SaveVects
                mov     cx,1024/2
                cli
                rep     movsw
                sti

                pop     cx              ; restore registers
                pop     dx
                pop     di
                pop     si
                pop     ds
                pop     es
                ret
FiniVectors_    endp



public          SetIntVecs_
SetIntVecs_     proc    near

                push    DS
                push    ES
                push    BX
                push    AX

                sub     AX,AX
                mov     ES,AX

                cli

        ife     dummy
                mov     BX,0004H                ; 1 * 4 = 4
                mov     AX,CS:TraceRtn
                mov     ES:[BX],AX
                mov     AX,CS
                mov     ES:2[BX],AX
        endif

                SetIntr  03H,BptTrap
                SetIntr  05H,UsrIntHandler
                SetIntr  15H,SysServHandler
                SetIntr  20H,ProgTerminate
                SwapIntr 28H,SaveBusyWait
                mov     ax,seg DGROUP
                mov     ds,ax
                cmp     byte ptr ds:_BoundAppLoading,0
                _if     ne
                SetIntr  21H,LoadOSHandler
                _else
                SetIntr  21H,OSHandler
                _endif
                SetIntr  27H,ProgTerminate

                sti

                pop     AX
                pop     BX
                pop     ES
                pop     DS

                jmp     near ptr SetUsrTask_

SetIntVecs_     endp


public          ClrIntVecs_
ClrIntVecs_     proc    near

                push    ES
                push    BX
                push    AX

                sub     AX,AX
                mov     ES,AX

                cli
                SetIntr  01H,NullHandler
                SetIntr  03H,NullHandler
                RestIntr 05H,PrgIntVec
                RestIntr 15H,SysServVec
                RestIntr 20H,PrgTermVec
                RestIntr 21H,OSIntVec
                RestIntr 27H,ResTermVec
                SwapIntr 28H,SaveBusyWait
                sti

                call    GetPSP
                mov     CS:CurrPSP,AX

                pop     AX
                pop     BX
                pop     ES

                jmp     near ptr SetDbgTask_

ClrIntVecs_     endp

                assume ds:nothing


SetBreak:       mov     byte ptr CS:UsrInt,-1
NullHandler:    iret


TimerHandler:
                pushf                   ; call old handler
                call    dword ptr TimerIntVec; ...
                cmp     byte ptr CS:UsrInt,0; check for requested interrupt
                je      NullHandler     ; quit if no interrupt requested
                push    ds              ; save DS
                push    bx              ; save BX
                lds     bx, CS:InDos    ; get InDos pointer
                cmp     byte ptr [bx],0 ; check if InDos is non-zero
                pop     bx              ; restore BX
                pop     ds              ; restore DS
                jne     NullHandler     ; quit if can't interrupt right now
DoIntTask:      mov     byte ptr CS:TrapType,TRAP_USER  ; user interrupt request
                mov     byte ptr UsrInt,0       ; clear pending request
                jmp     DebugTask


SysServHandler:
                cmp     AH,85H          ; is it a SysReq key press?
                je      UsrIntHandler   ; if so then handle the interrupt
                jmp     dword ptr SysServVec ; jump to old hander


UsrIntHandler:  mov     CS:UsrInt, -1   ; request user interrupt
                iret

ChkReturn:      mov     SS,CS:SaveIntSS
                mov     SP,CS:SaveIntSP
                pop     CS:SaveIntSP
                pop     CS:SaveIntSS
                push    BP                      ; save BP
                mov     BP,SP                   ; get access to stack
                push    AX                      ; save AX
                pushf                           ; save flags
                mov     AX,6[BP]                ; get old flags
                and     AX,0100H                ; isolate T-bit
                or      AX,-4[BP]               ; merge with new flags
                mov     6[BP],AX                ; save new flags
                pop     AX                      ; get rid of extra flags
                pop     AX                      ; restore AX
                pop     BP                      ; restore BP

                dec     byte ptr CS:DOSCount
                jne     rtrntouser
                cmp     byte ptr CS:UsrInt,00H
                jne     DoIntTask
rtrntouser:     iret


public          LoadOSHandler
LoadOSHandler:
                cmp     AH,48H                  ; if allocate memory request
                _if     e                       ; - then
                pushf                           ; - do the request
                push    cs
                call    near ptr OSHandler
                push    bp                      ; - save bp
                push    ax                      ; - save ax
                pushf                           ; - get flags register into ax
                pop     ax                      ; - ...
                mov     bp,sp                   ; - get addressability to stack
                mov     8[bp],ax                ; - zap the flags image
                pop     ax                      ; - restore ax
                _if     nc                      ; - if memory allocated
                push    ds                      ; - - save ds
                dec     ax                      ; - - point to DOS mem block
                mov     ds,ax                   ; - - ...
                inc     ax                      ; - - restore ax
                mov     bp,cs:_SegmentChain     ; - - link into our seg chain
                mov     ds:[14],bp              ; - - ...
                mov     cs:_SegmentChain,ds     ; - - ...
                pop     ds                      ; - - restore ds
                _endif                          ; - endif
                pop     bp                      ; - restore bp
                iret                            ; - return
                _else                           ; else
                cmp     ah,49H                  ; - if free memory request
                _if     e                       ; - - then
                push    ax                      ; - - save ax
                push    ds                      ; - - save ds
                mov     ax,es                   ; - - get segment
                dec     ax                      ; - - minus 1
                mov     es,ax                   ; - - ...
                cmp     ax,cs:_SegmentChain     ; - - if it's our chain head
                _if     e                       ; - - - then
                mov     ax,es:[14]              ; - - - bump head pointer up
                mov     cs:_SegmentChain,ax     ; - - - ...
                _else                           ; - - else
                mov     ax,cs:_SegmentChain     ; - - - h = SegmentChain
                _loop                           ; - - - loop
                mov     ds,ax                   ; - - - - if h->next == chunk
                mov     ax,es                   ; - - - - - then
                cmp     ds:[14],ax              ; - - - - - ...
                _if     e                       ; - - - - - ...
                mov     ax,es:[14]              ; - - - - - h->nex=h->nex->nex
                mov     ds:[14],ax              ; - - - - - ...
                                                ; - - - - - Z flag set !!!!!
                _else                           ; - - - - else
                mov     ax,ds:[14]              ; - - - - - head = hext->next
                test    ax,ax                   ; - - - - - test head,head
                _endif                          ; - - - - endif
                _until  e                       ; - - - until end of list
                _endif                          ; - - endif
                mov     ax,es                   ; - - restore es
                inc     ax                      ; - - ...
                mov     es,ax                   ; - - ...
                pop     ds                      ; - - restore ds
                pop     ax                      ; - - restore ax
                pushf                           ; - do the request
                push    cs                      ; - ...
                call    near ptr OSHandler      ; ...
                push    bp                      ; - save bp
                push    ax                      ; - save ax
                pushf                           ; - get flags register into ax
                pop     ax                      ; - ...
                mov     bp,sp                   ; - get addressability to stack
                mov     8[bp],ax                ; - zap the flags image
                pop     ax                      ; - restore ax
                pop     bp                      ; - restore bp
                iret                            ; - return
                _endif                          ; - endif
                _endif                          ; endif

OSHandler:
                cmp     AX,4B00H
                je      OSJmp
                cmp     AH,0
                je      ProgTerminate
                cmp     AH,31H
                je      ProgTerminate
                cmp     AH,4CH
                je      ProgTerminate


                inc     byte ptr CS:DOSCount
                push    CS:SaveIntSS
                push    CS:SaveIntSP
                mov     CS:SaveIntSS,SS
                mov     CS:SaveIntSP,SP
                pushf
                push    CS
                push    AX
                push    BP
                mov     BP,SP
                mov     AX,16[BP]
                and     AH,0FEH                         ; clear T-bit
                mov     6[BP],AX
                mov     AX,offset ChkReturn
                xchg    AX,2[BP]
                pop     BP

OSJmp:          jmp     dword ptr OSIntVec  ; jump to old handler


ProgTerminate:
                push    AX                      ; save AX
                call    GetPSP                  ; get the current PSP
                cmp     AX,CS:TaskPSP           ; is it the debugged task?
                pop     AX                      ; restore AX
                jne     OSJmp                   ; let DOS see it if another task
terminate:      push    BP                      ; save BP
                mov     BP,SP                   ; get access to stack
                push    BX                      ; save BX
                push    DS                      ; save DS
                lds     BX,2[BP]                ; get return offset/segment
                cmp     byte ptr -2[BX],0CDH    ; was it a software interrupt ?
                jne     not_an_int              ; if it was then
                sub     word ptr 2[BP],2        ; - backup return addr
not_an_int:     pop     DS                      ; restore DS
                pop     BX                      ; restore BX
                pop     BP                      ; restore BP
                mov     byte ptr CS:TrapType,TRAP_TERMINATE ; indicate program termintation
                jmp     DebugTask               ; enter the debugger



public          EndUser_
EndUser_        proc    near
                push    DS                      ; save registers
                push    ES
                push    SI
                push    DI
                push    DX
                push    CX
                push    BX
                push    AX
                push    BP
                mov     CS:OurSS,SS             ; mark stack level
                mov     CS:OurSP,SP             ; ...

                ; restore interrupt vectors
                mov     di,cs
                mov     ds,di
                sub     di,di
                mov     es,di
                mov     si,offset SaveVects
                mov     cx,1024/2
                cli
                rep     movsw
                sti

                ; DOS saves the the SS:SP value of a task in its PSP
                ; every time a task invokes int 21H. When a 4CH request
                ; is done DOS uses that stored value for a location to
                ; build the return interrupt frame to restart the parent
                ; task of the one that is being killed. since the
                ; last DOS call we've done might not be as deep in our
                ; stack as we currently are, we have to do a harmless, quick
                ; request so that DOS will set the proper values to restore
                ; SS:SP from (otherwise DOS will write over active stack
                ; locations).
                mov     AH,2CH                  ; get time
                int     21H

                call    near ptr SetUsrTask_    ; we're the user task

clearatask:
                mov     SS,CS:OurSS             ; get our stack back
                mov     SP,CS:OurSP             ; . . .
                call    GetPSP                  ; get current PSP
                cmp     AX,CS:DbgPSP            ; is it the debugger?
                je      allcleared              ; quit loop if so
                mov     DS,AX                   ; get access to PSP
                mov     DS:0AH,offset clearatask; set terminate offset
                mov     DS:0CH,CS               ; set terminate segment
                cli                             ; interrupts off
                mov     SS,AX                   ; set up a safe stack
                mov     SP,100H                 ; . . .
                sti                             ; interrupts on
                mov     AX,4C00H                ; terminate the task
                int     21H

allcleared:
                pop     BP                      ; restore registers
                pop     AX
                pop     BX
                pop     CX
                pop     DX
                pop     DI
                pop     SI
                pop     ES
                pop     DS
                cld
                ret
EndUser_        endp


;   DX:AX has program name, CX:BX has parm block
public          DOSLoadProg_
DOSLoadProg_    proc    near
                push    DS
                push    ES

                mov     CS:TaskPSP,0    ; assume we don't get a task
                mov     ES,CX           ; get parmblock in ES:BX
                mov     DS,DX           ; get file name in DS:DX
                mov     DX,AX           ; . . .
                mov     AX,4B01H        ; load program, maintain control
                int     21H             ; (internal DOS call)

                sbb     DX,DX           ; set DX based on carry

                jne     fini            ; if not equal then we have no task
                call    GetPSP          ; get current PSP
                mov     CS:TaskPSP,AX   ; save task PSP address
                mov     DS,AX           ; get access to new task
                mov     word ptr DS:0AH,offset debugprogend;set new terminate
                mov     DS:0CH,CS                          ; address
                mov     AH,25H          ; set up vectors
                mov     AL,23H          ; set Ctrl/C vector
                mov     DX,offset SetBreak
                push    CS
                pop     DS
                int     21H
                mov     AL,24H          ; set critical error vector
                mov     DX,CrtErrVec+0
                mov     DS,CrtErrVec+2
                int     21H

                call    ClrIntVecs_

                xor     ax,ax           ; get pointer to interrupt table
                mov     es,ax
                SetIntr 08H,TimerHandler

                xor     AX,AX           ; return zero
                xor     DX,DX           ; . . .
fini:
                pop     ES
                pop     DS
                ret
DOSLoadProg_    endp

debugprogend:
                mov     CS:TaskPSP,0            ; don't have a task anymore
                mov     byte ptr CS:TrapType,TRAP_TERMINATE ; program terminated
                jmp     DebugTask


public          SetDbgTask_
SetDbgTask_     proc    near
                push    BX              ; save BX
                mov     BX,CS:DbgPSP    ; tell DOS we're the debugger
                jmp     short SetPSP
SetDbgTask_     endp


DbgDOSCall      macro
                ; do a DOS request, but make sure to use the debugger's
                ; int 21 vector and NOT the program's
                pushf
                call    dword ptr CS:SaveVects+21H*4
                endm

public          SetUsrTask_
SetUsrTask_     proc    near
                push    BX              ; save BX
                mov     BX,CS:CurrPSP   ; tell DOS we're the current task
SetPSP:
                pushf                   ; save flags
                push    AX              ; save AX
                mov     AH,50H          ; set PSP request (internal DOS call)
                DbgDOSCall              ; do the call
                pop     AX              ; restore AX
                popf                    ; restore flags
                pop     BX              ; restore BX
                ret                     ; return
SetUsrTask_     endp


GetPSP          proc    near
                pushf                   ; save flags
                push    BX              ; save BX
                mov     AH,51H          ; get PSP request (internal DOS call)
                DbgDOSCall              ; do the call
                mov     AX,BX           ; put PSP in proper return register
                pop     BX              ; restore BX
                popf                    ; restore flags
                ret                     ; return
GetPSP          endp


public          DOSTaskPSP_
DOSTaskPSP_     proc    near
                mov     AX,CS:TaskPSP
                ret
DOSTaskPSP_     endp


public          DbgPSP_
DbgPSP_         proc    near
                mov     AX,CS:DbgPSP
                ret
DbgPSP_         endp


; return with carry set if we don't want to watch this interrupt
public          ChkInt
ChkInt          proc    near
                cmp     BL,8FH
                jbe     ms_overlay
watch_it:       clc
                ret
ms_overlay:
                cmp     BL,3FH
                je      watch_it
                stc
                ret
ChkInt          endp

EMU_OTHER   equ 3cH
EMU_INT     equ 39H

public          Null87Emu_
Null87Emu_      proc    near
                push    es
                xor     ax,ax
                mov     es,ax
                mov     es:[EMU_INT*4+0],ax  ; null out emulator interrupt
                mov     es:[EMU_INT*4+2],ax  ; ...
                pop     es
                ret
Null87Emu_      endp

        public  Read87EmuState_
Read87EmuState_ proc near
        push    ds
        push    bx
        mov     ds,dx
        mov     bx,ax
check_byte label byte
        db      0cdH, 039H, 037H ;fsave   ds:[bx]
        pop     bx
        pop     ds
        call    FPUExpand
        ret
Read87EmuState_ endp

        public  Write87EmuState_
Write87EmuState_ proc near
        call    FPUContract
        push    ds
        push    bx
        mov     ds,dx
        mov     bx,ax
        db      0cdH, 039H, 027H ;frstor  ds:[bx]
        pop     bx
        pop     ds
        ret
Write87EmuState_ endp

public          Have87Emu_
Have87Emu_      proc    near
                push    es
                push    bx
                xor     ax,ax
                mov     es,ax
                les     bx,es:[EMU_INT*4]  ; get emulator interrupt
                _guess
                mov     ax,es
                or      ax,bx           ; is interrupt NULL?
                _quif   e               ; quit if it is
                cmp     byte ptr es:[bx], 0cfH; is it pointing at an iret?
                _quif   e               ; quit if it is
                xor     ax,ax
                push    ax
                mov     ax,es
                pop     es
                cmp     bx,es:[EMU_OTHER*4+0]; are both emu interrupts
                _if     e                   ; pointing at the same thing?
                cmp     ax,es:[EMU_OTHER*4+2]
                _endif
                _quif   e               ; quit if so
                cmp     byte ptr cs:check_byte, 0cdH; has emu fsave been patched?
                _quif   ne              ; quit if so
                mov     ax,1            ; have emulator
                _admit
                xor     ax,ax           ; no emulator
                _endguess
                pop     bx
                pop     es
                ret
Have87Emu_      endp

_TEXT           ends
