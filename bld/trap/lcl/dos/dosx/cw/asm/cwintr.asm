;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*  Copyright (c) 2004-2009 The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  CauseWay trap file low-level interface routines
;*
;*****************************************************************************

.386

include cw.inc

;*******************************************************************************
;Lazy variable access equates.
;*******************************************************************************
b       equ     byte ptr
w       equ     word ptr
d       equ     dword ptr
f       equ     fword ptr

CPU_FLAG_SINGLE_STEP        equ 100h

;*******************************************************************************
;Execution status symbolic constants
;*******************************************************************************
ST1_EXECUTING               equ 01h
ST1_BREAK                   equ 02h
ST1_TRACE                   equ 04h
ST1_WATCH                   equ 08h
ST1_KEYBREAK                equ 10h
ST1_TERMINATE               equ 20h
ST1_LOAD_MODULE             equ 40h
ST1_UNLOAD_MODULE           equ 80h
;
ST2_SINGLE_STEPPING         equ 1h
ST2_SINGLE_STEPPING_WATCH   equ 2h

;*******************************************************************************
;Add a memory region to the auto-lock list.
;*******************************************************************************
autolock        macro p1,p2
        extrn __autolock:near
_AUTOLOCK       segment dword public 'DATA'
        dd p1,p2
_AUTOLOCK       ends
        endm

;*******************************************************************************
;Setup/save PM interrupt/exception vector
;*******************************************************************************
set_vect macro   vect_no, vect_func, vect_oldaddr, vect_type
        assume ds:_TEXT
        mov     bl,vect_no
ifb <vect_type>
        sys GetVect
else
        sys GetEVect
endif
        mov     d[vect_oldaddr],edx
        mov     w[vect_oldaddr+4],cx
        mov     cx,cs
        mov     edx,offset vect_func
ifb <vect_type>
        sys SetVect
else
        sys SetEVect
endif
        assume ds:DGROUP
endm

;*******************************************************************************
;Restore PM interrupt/exception vector
;*******************************************************************************
restore_vect macro   vect_no, vect_oldaddr, vect_type
        assume ds:_TEXT
        mov     edx,d[vect_oldaddr]
        mov     cx,w[vect_oldaddr+4]
        mov     bl,vect_no
ifb <vect_type>
        sys SetVect
else
        sys SetEVect
endif
        assume ds:DGROUP
endm

;*******************************************************************************
;Save all debug registers (general + segment), DS is on the stack
;*******************************************************************************
save_debug_regs macro
        mov     DebugEAX,eax
        mov     DebugEBX,ebx
        mov     DebugECX,ecx
        mov     DebugEDX,edx
        mov     DebugESI,esi
        mov     DebugEDI,edi
        mov     DebugEBP,ebp
        pop     eax
        mov     DebugDS,ax
        mov     DebugES,es
        mov     DebugFS,fs
        mov     DebugGS,gs
endm

;*******************************************************************************
;DPMI Exception handler stack frame
;*******************************************************************************
EXCPFRM struct
    reserved    dd 2 dup(?)
    errcode     dd 0
    e_eip       dd 0
    e_cs        dw 0,0
    e_flags     dd 0
    e_esp       dd 0
    e_ss        dw 0,0
EXCPFRM ends

;*******************************************************************************
;Interrupt handler stack frame
;*******************************************************************************
INTFRM struct
    i_eip       dd 0
    i_cs        dw 0,0
    i_flags     dd 0
INTFRM ends

.data

extern  "C", _psp:WORD
extern  "C", WatchCount:dword

public  "C", DebugPSP
public  "C", DebugRegs
public  "C", Exception

dLockStart      label byte

DebugPSP        dw 0

DebugRegs       label byte

DebugEAX        dd 0
DebugEBX        dd 0
DebugECX        dd 0
DebugEDX        dd 0
DebugESI        dd 0
DebugEDI        dd 0
DebugEBP        dd 0
DebugESP        dd 0
DebugEIP        dd 0
DebugEFL        dd 0
DebugCR0        dd 0
DebugCR2        dd 0
DebugCR3        dd 0
DebugDS         dw 0
DebugES         dw 0
DebugSS         dw 0
DebugCS         dw 0
DebugFS         dw 0
DebugGS         dw 0

DebugZero       dw 0

DebuggerESP     dd 0
DebuggerSS      dw 0

TerminateCode   db 0
DebugState      label   word
DebugState1     db 0
DebugState2     db 0
Exception       db 0

LinearAddressCheck db 0

InInt09         db 0
KeyTable        db 128 dup (0)      ;keypress table.

BreakKeyList    dd 1dh,38h,0,0,0

dLockEnd        label byte

        autolock cLockStart,cLockEnd-cLockStart
        autolock dLockStart,dLockEnd-dLockStart


.code

extern          "C", IsHardBreak:proc
extern          "C", CheckWatchPoints:proc
extern          "C", SetHBRK:proc
extern          "C", ResetHBRK:proc
extern          "C", CheckTerminate:proc

cLockStart      label byte

        assume ds:DGROUP, es:nothing

dgroup_seg      dd 0

;*******************************************************************************
;
;Interrupt handler - return control to debugger.
;
;*******************************************************************************
SwitchToDebuggerIntFrame:
;
;Save general registers.
;
        save_debug_regs
;
;Fetch original CS:EIP and flags
;
        mov     eax,[esp+INTFRM.i_eip]
;
;Correct EIP for int 3 instruction or for terminating int 21h (4Ch)
;
        cmp     Exception,3
        jz      @@skip1
        test    DebugState1,ST1_TERMINATE
        jz      @@skip2
        dec     eax
@@skip1:
        dec     eax
@@skip2:
        mov     DebugEIP,eax
        mov     eax,d[esp+INTFRM.i_cs]
        mov     DebugCS,ax
;
        mov     eax,[esp+INTFRM.i_flags]
        and     ah,not (CPU_FLAG_SINGLE_STEP shr 8)
        mov     DebugEFL,eax
;
;Now modify original CS:EIP,SS:ESP values and return control
;to debugger code via interupt structure to restore stacks.
;
        mov     [esp+INTFRM.i_flags],eax
        mov     [esp+INTFRM.i_cs],cs
        mov     [esp+INTFRM.i_eip],offset SwitchToDebugger
        iretd


;*******************************************************************************
;
;Exception handler - return control to debugger.
;
;*******************************************************************************
SwitchToDebuggerExcFrame:
;
;Save general registers.
;
        save_debug_regs
;
;Fetch origional Flags:CS:EIP
;
        mov     eax,[esp+EXCPFRM.e_eip]
;
;Correct EIP for int 3 instruction length
;
        cmp     Exception,3
        jnz     @@skip3
        dec     eax
@@skip3:
        mov     DebugEIP,eax
        mov     eax,d[esp+EXCPFRM.e_cs]
        mov     DebugCS,ax
;
;Fetch original Flags
;
        mov     eax,[esp+EXCPFRM.e_flags]
        and     ah,not (CPU_FLAG_SINGLE_STEP shr 8)
        mov     DebugEFL,eax
;
;Now modify origional CS:EIP,SS:ESP values and return control
;to debugger code via exception structure to restore stacks.
;
        mov     [esp+EXCPFRM.e_flags],eax
        mov     [esp+EXCPFRM.e_cs],cs
        mov     [esp+EXCPFRM.e_eip],offset SwitchToDebugger
        retf

;*******************************************************************************
;
;BreakKey handler - return control to debugger.
;
;*******************************************************************************
SwitchToDebuggerBreakKey:
        push    ds
        mov     ds,dgroup_seg
;
;Save general registers.
;
        save_debug_regs
;
        pushfd
        pop     eax
        mov     DebugEFL,eax
        or      DebugState1,ST1_KEYBREAK
        dec     InInt09
;Fall down

;*******************************************************************************
;
;Switch stack and return control to debugger.
;
;*******************************************************************************
SwitchToDebugger:
        mov     eax,dgroup_seg
        mov     es,eax
        mov     fs,eax
        mov     gs,eax
        mov     DebugSS,ss
        mov     DebugESP,esp
        lss     esp,f[DebuggerESP]
        ret


;*******************************************************************************
;
;Catch single instruction trace and debug register traps.
;
;*******************************************************************************
Int01Handler    proc    near
        test    cs:DebugState1,ST1_EXECUTING
        jz      @@Oldi01
        push    ds
        mov     ds,dgroup_seg
        push    eax
        call    IsHardBreak
        or      eax,eax
        pop     eax
        jz      @@0i01
        or      DebugState1,ST1_WATCH
        jmp     @@0i02
@@0i01: 
        or      DebugState1,ST1_TRACE
@@0i02: 
        mov     Exception,1
        jmp     SwitchToDebuggerIntFrame
;
@@Oldi01:
        jmp     f[OldInt01]
OldInt01        df 0
Int01Handler    endp


;*******************************************************************************
;
;Catch single step or debug register traps.
;
;*******************************************************************************
EInt01Handler   proc    near
        test    cs:DebugState1,ST1_EXECUTING
        jz      @@Olde01
        push    ds
        mov     ds,dgroup_seg
        push    eax
        call    IsHardBreak
        or      eax,eax
        pop     eax
        jz      @@0e01
        or      DebugState1,ST1_WATCH
        jmp     @@0e02
@@0e01:
        or      DebugState1,ST1_TRACE
@@0e02:
        mov     Exception,1
        jmp     SwitchToDebuggerExcFrame
;
@@Olde01:
        jmp     f[OldEInt01]
OldEInt01       df 0
EInt01Handler   endp


;*******************************************************************************
;
;Catch INT 3's.
;
;*******************************************************************************
Int03Handler    proc    near
        test    cs:DebugState1,ST1_EXECUTING
        jz      @@Oldi03
        push    ds
        mov     ds,dgroup_seg
        or      DebugState1,ST1_BREAK
        mov     Exception,3
        jmp     SwitchToDebuggerIntFrame
;
@@Oldi03:
        jmp     f[OldInt03]
OldInt03        df 0
Int03Handler    endp


;*******************************************************************************
;
;Catch break exceptions.
;
;*******************************************************************************
EInt03Handler   proc    near
        test    cs:DebugState1,ST1_EXECUTING
        jz      @@Olde03
        push    ds
        mov     ds,dgroup_seg
        or      DebugState1,ST1_BREAK
        mov     Exception,3
        jmp     SwitchToDebuggerExcFrame
;
@@Olde03:
        jmp     f[OldEInt03]
OldEInt03       df 0
EInt03Handler   endp


;*******************************************************************************
;
;Catch divide by zero faults.
;
;*******************************************************************************
Exc00Handler    proc    near
        test    cs:DebugState1,ST1_EXECUTING
        jz      @@Olde00
        push    ds
        mov     ds,dgroup_seg
        mov     Exception,0
        jmp     SwitchToDebuggerExcFrame
;
@@Olde00:
        jmp     f[OldExc00]
OldExc00        df 0
Exc00Handler    endp


;*******************************************************************************
;
;Catch stack exceptions.
;
;*******************************************************************************
Exc0CHandler    proc    near
        test    cs:DebugState1,ST1_EXECUTING
        jz      @@Olde0C
        push    ds
        mov     ds,dgroup_seg
        mov     Exception,0Ch
        jmp     SwitchToDebuggerExcFrame
;
@@Olde0C:
        jmp     f[OldExc0C]
OldExc0C        df 0
Exc0CHandler    endp


;*******************************************************************************
;
;Catch general protection faults.
;
;*******************************************************************************
Exc0DHandler    proc    near
        test    cs:DebugState1,ST1_EXECUTING
        jz      @@Olde0D
        push    ds
        mov     ds,dgroup_seg
        mov     Exception,0Dh
        jmp     SwitchToDebuggerExcFrame
;
@@Olde0D:
        jmp     f[OldExc0D]
OldExc0D        df 0
Exc0DHandler    endp


;*******************************************************************************
;
;Catch page faults.
;
;*******************************************************************************
Exc0EHandler    proc    near
        push    ds
        mov     ds,dgroup_seg
        test    DebugState1,ST1_EXECUTING
        jz      @@Olde0E
        mov     Exception,0Eh
        jmp     SwitchToDebuggerExcFrame
        ;
@@Olde0E:
        cmp     LinearAddressCheck,0
        jz      @@shite0E
        mov     LinearAddressCheck,0
        pop     ds
;
; skip failing REP MOVSB instruction if memory access exception occures
; see MemoryRead and MemoryWrite functions
;
        add     [esp+EXCPFRM.e_eip],2
        retf
@@shite0E:
        pop     ds
        jmp     f[OldExc0E]
OldExc0E        df 0
Exc0EHandler    endp


;*******************************************************************************
;
;INT 21h handler to catch program termination.
;
;*******************************************************************************
Int21Handler    proc    near
        assume ds:nothing
        pushfd
        cmp     ah,4ch          ;terminate?
        jnz     @@Oldi21
;
        push    ds
        mov     ds,dgroup_seg
        push    eax
        call    CheckTerminate
        or      eax,eax
        pop     eax
        pop     ds
        jz      @@Oldi21
;
        assume ds:DGROUP
        popfd
        push    ds
        mov     ds,dgroup_seg
        or      DebugState1,ST1_TERMINATE
        mov     TerminateCode,al
        jmp     SwitchToDebuggerIntFrame
;
@@Oldi21:
        popfd
        jmp     f[OldInt21]
OldInt21        df 0
Int21Handler    endp


;*******************************************************************************
;
;Check if our break keys combination is set.
;
;*******************************************************************************
CheckBreakKey   proc    near
        mov     ebx,offset BreakKeyList
        xor     eax,eax
@@6bc:  cmp     d[ebx],0                ;End of the list?
        jz      @@8bc
        mov     eax,d[ebx]              ;Get scan code.
        cmp     b[KeyTable+eax],0
        jz      @@7bc
        add     ebx,4
        jmp     @@6bc
;
@@7bc:  xor     eax,eax
@@8bc:  ret
CheckBreakKey   endp


;*******************************************************************************
;
;Keyboard interrupt handler
;
;*******************************************************************************
BreakChecker    proc    near
        push    ds
        push    eax
        push    ebx
        mov     ds,dgroup_seg
        inc     InInt09
;
;Update the key table.
;
        in      al,60h          ;get the scan code.
        mov     bl,al
        and     ebx,7Fh         ;isolate scan code.
        add     ebx,offset KeyTable
        and     al,80h
        xor     al,80h
        mov     [ebx],al                ;set key state.
;
;Check we havn't already been here.
;
        cmp     InInt09,2
        jnc     @@oldbc
;
;Check if anything is running.
;
;
        test    DebugState1,ST1_EXECUTING
        jz      @@oldbc
;
;Check if our break combination is set.
;
        call    CheckBreakKey
        or      eax,eax
        jz      @@oldbc
;
;Check if return CS:EIP & stack belong to the program we're running.
;
        push    es
        mov     es,DebugPSP
        mov     ax,es:[EPSP_SegBase]
        pop     es

STACK_DEPTH = 3 * 4

        cmp     [esp+STACK_DEPTH+INTFRM.i_cs],ax
        jc      @@oldbc
        mov     eax,[esp+STACK_DEPTH+INTFRM.i_eip]
        mov     DebugEIP,eax
        mov     eax,d[esp+STACK_DEPTH+INTFRM.i_cs]
        mov     DebugCS,ax
;
;Want to break into the program so swollow this key press.
;
        in      al,61h
        mov     ah,al
        or      al,1 shl 7
        out     61h,al          ;say comming back.
        xchg    ah,al
        out     61h,al          ;code we got.
        ;
        mov     al,20h
        out     20h,al          ;re-enable interupts.
;
;Clean up the key table.
;
        mov     ebx,offset BreakKeyList
@@60bc: cmp     d[ebx],0                ;End of the list?
        jz      @@70bc
        mov     eax,d[ebx]              ;Get scan code.
        mov     b[KeyTable+eax],0
        add     ebx,4
        jmp     @@60bc
@@70bc:
;
;Swap the return address for our own.
;
        mov     [esp+STACK_DEPTH+INTFRM.i_cs],cs
        mov     [esp+STACK_DEPTH+INTFRM.i_eip],offset SwitchToDebuggerBreakKey
;
        pop     ebx
        pop     eax
        pop     ds
        iretd
;
;Pass control to the origional handler.
;
@@oldbc:
        dec     InInt09
        pop     ebx
        pop     eax
        pop     ds
        jmp     f[OldInt09]
OldInt09        df 0
BreakChecker    endp


;*******************************************************************************
;
;This must be just before cwAPIpatch
;
;*******************************************************************************
cwIdentity      db "CAUSEWAY"
cwVersion label word
cwMajorVersion  db 0
cwMinorVersion  db 0

;*******************************************************************************
;
;This intercepts get/set int 9 vector stuff and makes sure BreakChecker stays
;first in the chain. Also monitor exec calls.
;
;*******************************************************************************
Int31Intercept  proc    near
        cmp     ax,0205h                ;Set vector?
        jnz     @@notseti31
        cmp     bl,9                    ;INT 9?
        jnz     @@oldi31
        push    ds
        mov     ds,dgroup_seg           ;make our data addresable.
        assume ds:_TEXT
        mov     d[OldInt09],edx
        mov     w[OldInt09+4],cx
        pop     ds
        jmp     @@reti31
        ;
@@notseti31:
        assume ds:DGROUP
        cmp     ax,0204h                ;Get vector?
        jnz     @@notgeti31
        cmp     bl,9                    ;INT 9?
        jnz     @@oldi31
        mov     edx,d[OldInt09]
        mov     cx,w[OldInt09+4]
        ;
@@reti31:
        and     b[esp+INTFRM.i_flags],not 1 ;clear carry
        iretd
        ;
@@notgeti31:
        test    cs:DebugState1,ST1_EXECUTING
        jz      @@oldi31
        cmp     ax,0FFF7h               ;Load module?
        jnz     @@notloadmi31
        push    ds
        mov     ds,dgroup_seg
        or      DebugState1,ST1_LOAD_MODULE
        jmp     SwitchToDebuggerIntFrame
        ;
@@notloadmi31:
        cmp     ax,0FFF8h               ;Unload module?
        jnz     @@oldi31
        push    ds
        mov     ds,dgroup_seg
        or      DebugState1,ST1_UNLOAD_MODULE
        jmp     SwitchToDebuggerIntFrame
        ;
@@oldi31:
        jmp     f[OldInt31]
OldInt31        df 0
Int31Intercept  endp


;*******************************************************************************
;
;Execute the debugee.
;
;On Entry:
;
;EAX    - mode, 0=go, 1=step.
;
;Returns:
;
;EAX    - status (see REQ_PROG_GO/STEP return flags)
;
;*******************************************************************************
Execute proc    "C" public  uses es fs gs ebx ecx edx esi edi ebp
;
        or      al,al
        jz      nostep
        mov     al,ST2_SINGLE_STEPPING
nostep: 
;
; Force watch point checking if watches are present.
;
        cmp     WatchCount,0
        jz      nowatch
        or      al,ST2_SINGLE_STEPPING_WATCH
nowatch:
        mov     DebugState2,al
;
;Switch to debuggee's PSP.
;
        call    SetUsrTask
;
;Install hardware break points.
;
        call    SetHBRK
;
;Set debuggee trap flag if it's a single instruction trace else clear it if not.
;
@@7exec:
        test    DebugState2,ST2_SINGLE_STEPPING or ST2_SINGLE_STEPPING_WATCH
        jz      @@0exec
        or      b[DebugEFL+1],CPU_FLAG_SINGLE_STEP shr 8
        jmp     @@11exec
@@0exec:
        and     b[DebugEFL+1],not (CPU_FLAG_SINGLE_STEP shr 8)
@@11exec:
;
;Set flags ready for execution.
;
        mov     DebugState1,ST1_EXECUTING
        mov     Exception,-1
;
;Put return address on the stack.
;
        mov     eax,offset @@backexec   ;store return address for int 3.
        push    eax
        mov     w[DebuggerSS],ss
        mov     d[DebuggerESP],esp
;
;Execute the program.
;
        mov     ss,DebugSS
        mov     esp,DebugESP
        push    d[DebugEFL]
        push    w[DebugZero]
        push    w[DebugCS]
        push    d[DebugEIP]
        mov     eax,DebugEAX
        mov     ebx,DebugEBX
        mov     ecx,DebugECX
        mov     edx,DebugEDX
        mov     esi,DebugESI
        mov     edi,DebugEDI
        mov     ebp,DebugEBP
        mov     gs,DebugGS
        mov     fs,DebugFS
        mov     es,DebugES
        mov     ds,DebugDS
        iretdf
;
;Clear execution flag.
;
@@backexec:
        and     DebugState1,not ST1_EXECUTING
;
;Check if we're single stepping to allow for watches.
;
        test    DebugState2,ST2_SINGLE_STEPPING_WATCH
        jz      @@8exec
        test    DebugState1,(ST1_TERMINATE or ST1_KEYBREAK or ST1_BREAK or ST1_WATCH or ST1_LOAD_MODULE or ST1_UNLOAD_MODULE)
        jnz     @@8exec
        test    DebugState1,ST1_TRACE
        jz      @@8exec
;
;Check state of all watches.
;
        call    CheckWatchPoints
        or      eax,eax
        jnz     @@10exec
;
;Check it wasn't a single step anyway.
;
        test    DebugState2,ST2_SINGLE_STEPPING ;single steping anyway?
        jnz     @@8exec
        jmp     @@7exec

@@10exec:
        or      DebugState1,ST1_WATCH
@@8exec:
;
;Re-enable interrupts
;
        mov     al,20h
        out     20h,al
;
;Remove HBRK's
;
        call    ResetHBRK
;
;Store current Debugee's PSP.
;
        mov     ah,62h
        int     21h
        mov     DebugPSP,bx
;
;Switch back to helper's PSP.
;
        call    SetDbgTask
;
        movzx   eax,DebugState
        ret
Execute endp


;*******************************************************************************
;
;Load program to debug
;
;On Entry:
;
;EAX     - program name
;EDX     - command line
;
;Returns:
;
;EAX     - error
;
;ALL registers preserved.
;
;*******************************************************************************
DebugLoad   proc    "C" public  uses ebx ecx edx esi edi
;
        mov     esi,edx
        mov     edx,eax
        xor     ecx,ecx
        push    ebp
        sys     ExecDebug
        pop     ebp
;!!!! TODO - Causeway bug, change ES !!!!
        push    ds
        pop     es
;!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        jnc     @@3load
;
;Some sort of error occured so set status.
;
        movsx   eax,ax
        jmp     @@9load
;
;Setup initial register values.
;
@@3load:
        mov     DebugCS,cx
        mov     DebugEIP,edx
        mov     DebugSS,bx
        mov     DebugESP,eax
        mov     DebugPSP,si
        mov     DebugDS,di
        mov     DebugES,si
        pushfd
        pop     eax
        mov     DebugEFL,eax
        xor     eax,eax
        mov     DebugEAX,eax
        mov     DebugEBX,eax
        mov     DebugECX,eax
        mov     DebugEDX,eax
        mov     DebugESI,eax
        mov     DebugEDI,eax
        mov     DebugEBP,eax
;
;Setup a new transfer buffer to stop debugger interfering.
;
        mov     bx,200h     ;8192/16
        sys     GetMemDOS
        jc      @@4load
;
        call    SetUsrTask
;
        mov     bx,ax
        mov     ecx,2000h   ;8192
        sys     SetDOSTrans
;
        call    SetDbgTask
@@4load:
        xor     eax,eax
@@9load:
;
;Return results to caller.
;
        ret
DebugLoad   endp


;*******************************************************************************
;
;Switch to Debugee's PSP
;
;*******************************************************************************
SetUsrTask  proc    near    "C" public
        push    ebx
        push    eax
        mov     bx,DebugPSP
        mov     ah,50h
        int     21h
        pop     eax
        pop     ebx
        ret
SetUsrTask  endp


;*******************************************************************************
;
;Switch to Helper's PSP
;
;*******************************************************************************
SetDbgTask  proc    near    "C" public
        push    ebx
        push    eax
        mov     bx,_psp
        mov     ah,50h
        int     21h
        pop     eax
        pop     ebx
        ret
SetDbgTask  endp


;*******************************************************************************
;
;Check and calculate memory block info.
;
;On Entry:
;
;EAX     - memory block offset
;EDX     - memory block selector
;ECX     - memory block length
;
;Returns:
;
;ESI     - memory block start linear address
;ECX     - memory block length (available length)
;
;*******************************************************************************
CheckMemoryBlockInfo proc private uses ebx edi
        mov     esi,eax
        movzx   ebx,dx
        or      eax,ebx
        jz      @@calc2
        mov     edi,ecx
        sys     GetSelDet32
        sub     ecx,esi
        jbe     @@calc2
        cmp     ecx,edi
        jb      @@calc1
        mov     ecx,edi
@@calc1:
        add     esi,edx
        jmp     @@calc3
@@calc2:
        xor     ecx,ecx
@@calc3:
        ret
CheckMemoryBlockInfo endp


;*******************************************************************************
;
;Read memory block to buffer.
;
;On Entry:
;
;EAX     - memory block offset
;EDX     - memory block selector
;EBX     - buffer address
;ECX     - memory block length
;
;Returns:
;
;EAX     - number of readed bytes
;
;*******************************************************************************
MemoryRead  proc    "C" public uses esi edi
        mov     edi,ebx
        call    CheckMemoryBlockInfo
        push    ecx
        or      LinearAddressCheck,1
        rep movsb
        mov     LinearAddressCheck,0
        pop     eax
        sub     eax,ecx
        ret
MemoryRead  endp


;*******************************************************************************
;
;Write memory block from buffer.
;
;On Entry:
;
;EAX     - memory block offset
;EDX     - memory block selector
;EBX     - buffer address
;ECX     - memory block length
;
;Returns:
;
;EAX     - number of writen bytes
;
;*******************************************************************************
MemoryWrite proc    "C" public  uses esi edi
        mov     edi,ebx
        call    CheckMemoryBlockInfo
        xchg    edi,esi
        push    ecx
        or      LinearAddressCheck,1
        rep movsb
        mov     LinearAddressCheck,0
        pop     eax
        sub     eax,ecx
        ret
MemoryWrite endp


;*******************************************************************************
;
;Save and setup interupt and exception vectors
;
;*******************************************************************************
GrabVectors proc    near    "C" public uses    ebx ecx edx
;
;Save data selector for interupt and exception handlers
;
        assume ds:_TEXT
        mov     eax,DGROUP
        mov     dgroup_seg,eax
        assume ds:DGROUP
;
;Patch interupts and exceptions
;
        set_vect    1,   Int01Handler,   OldInt01
        set_vect    3,   Int03Handler,   OldInt03
        set_vect    1,   EInt01Handler,  OldEInt01, E
        set_vect    3,   EInt03Handler,  OldEInt03, E
        set_vect    0,   Exc00Handler,   OldExc00,  E
        set_vect    0Ch, Exc0CHandler,   OldExc0C,  E
        set_vect    0Dh, Exc0DHandler,   OldExc0D,  E
        set_vect    0Eh, Exc0EHandler,   OldExc0E,  E
;
;Patch INT 9 & INT 31h so we always see keyboard activity
;and can interrupt the program.
;
        set_vect    31h, Int31Intercept, OldInt31
        set_vect    9,   BreakChecker,   OldInt09
        set_vect    21h, Int21Handler,   OldInt21
;
;Duplicate CauseWay version number.
;
        assume ds:_TEXT
        push    es
        les     edx,OldInt31
        movzx   eax,word ptr es:[edx-2]
        pop     es
        mov     cwVersion,ax
        xchg    al,ah
        assume ds:DGROUP
;
        ret
GrabVectors endp


;*******************************************************************************
;
;Restore interupt and exception vectors
;
;*******************************************************************************
ReleaseVectors proc    near     "C" public uses    eax ebx ecx edx
        restore_vect    9,   OldInt09
        restore_vect    21h, OldInt21
        restore_vect    1,   OldInt01
        restore_vect    3,   OldInt03
        restore_vect    1,   OldEInt01, E
        restore_vect    3,   OldEInt03, E
        restore_vect    0,   OldExc00,  E
        restore_vect    0Ch, OldExc0C,  E
        restore_vect    0Dh, OldExc0D,  E
        restore_vect    0Eh, OldExc0E,  E
        restore_vect    31h, OldInt31
        ret
ReleaseVectors endp

cLockEnd    label byte

end
