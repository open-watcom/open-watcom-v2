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


;****************************************************************************
;***                                                                      ***
;*** WINDLL.ASM - windows 32-bit dll                                      ***
;***                                                                      ***
;****************************************************************************
.386p

 DGROUP group _NULL,_DATA,DATA,_emu_init_start,_emu_init,_emu_init_end,_BSS

public  pLocalHeap
public  pAtomTable
public  pStackTop
public  pStackMin
public  pStackBot

pLocalHeap      equ     0006H
pAtomTable      equ     0008H
pStackTop       equ     000AH
pStackMin       equ     000CH
pStackBot       equ     000EH


        DOSSEG

_BSS          segment word public 'BSS' use16
_BSS          ends

DATA    segment word public 'DATA' use16
DATA    ends
_emu_init_start segment word public 'EMU' use16
__emu_init_start label word
_emu_init_start ends

_emu_init segment byte public 'EMU' use16
_emu_init ends

_emu_init_end segment word public 'EMU' use16
__emu_init_end label word
_emu_init_end ends

_NULL   segment para public 'BEGDATA' use16
__nullarea label word
           dw   0,0
           dw   5
           dw   0               ; pLocalHeap
           dw   0               ; pAtomTable
__STACKLOW dw   0               ; pStackTop: lowest address in stack
__STACKTOP dw   0               ; pStackMin:
           dw   0               ; pStackBot: highest address in stack
        public  __nullarea
_NULL   ends

_DATA segment word public 'DATA' use16
;*
;*** externals we need
;*
assume es:nothing
assume ss:nothing
assume ds:dgroup
assume cs:_TEXT
extrn   Fini_:proc
extrn   _DataSelector:WORD
extrn   _StackSelector:WORD
extrn   _EntryStackSave:DWORD
extrn   _ReturnCode:DWORD
extrn   _SaveSP:DWORD
extrn   _StackSize:DWORD
extrn   _EDataAddr:DWORD
extrn   _CodeEntry:FWORD
extrn   __DLLEntryAddr:DWORD
extrn   __WEPAddr:DWORD

__aaltstkovr dw -1              ; alternate stack overflow routine address
__curbrk   dw 0                 ; top of usable memory
__psp      dw 0                 ; segment addr of program segment prefix
__osmajor  db 0                 ; major DOS version number
__osminor  db 0                 ; minor DOS version number
__child    dw 0                 ; non-zero => a spawned process is running
__no87     dw 0                 ; always try to use the 8087
 __FPE_handler label dword
___FPE_handler dw 0,0           ; FPE handler
__HShift   db 0                 ; Huge Shift value
__osmode   db 1                 ; mode
__WEPflag  db 0                 ; non-zero => WEP has been run

PUBLIC  _InDebugger
_InDebugger     dw 0


        public  __osmode
        public  __curbrk
        public  __psp
        public  __osmajor
        public  __osminor
        public  __STACKLOW
        public  __STACKTOP
        public  __child
        public  __no87
        public   __FPE_handler
        public  ___FPE_handler
        public  __HShift

DLLRoutine      LABEL FWORD
DLLEIP          dd 0
DLLCS           dw 0
_DATA ends

extrn LOCALINIT:FAR

;*
;*** the windows extender code lies here
;*
_TEXT segment word public 'CODE' use16

public          _small_code_
_small_code_    equ 0

;****************************************************************************
;***                                                                      ***
;*** LibEntry - 16-bit library entry point                                ***
;***                                                                      ***
;****************************************************************************
extrn LibMain:NEAR
PUBLIC LibEntry
LibEntry PROC FAR
        mov     ax,ds
        nop
        inc     bp
        push    bp
        mov     bp,sp
        push    ds
        mov     ds,ax
        push    di              ; handle of the module instance
        push    ds              ; library data segment
        push    cx              ; heap size
        push    es              ; command line segment
        push    si              ; command line offset
        jcxz    callc           ; skip heap init
        xor     ax,ax
        push    ds
        push    ax
        push    cx
        call    LOCALINIT
        or      ax,ax           ; did it do it ok ?
        jz      error           ; quit if it failed

callc:
        call    LibMain         ; invoke the 'C' routine (result in AX)
        jmp     short exit      ; LibMain is responsible for stack clean up

error:

        pop     si               ; clean up stack on a LocalInit error
        pop     es
        pop     cx
        pop     ds
        pop     di
        jmp     short exit
__exit_with_msg_:
        mov     ah,04cH                 ; DOS call to exit with return code
        int     021h                    ; back to DOS

        public  __exit_with_msg_
exit:

         lea    sp,-2H[bp]
         pop    ds
         pop    bp
         dec    bp
         retf
LibEntry ENDP

        assume ds:nothing
dsvalue dw      DGROUP
;****************************************************************************
;***                                                                      ***
;*** WEP - end procedure, called when dll is unloaded                     ***
;***                                                                      ***
;****************************************************************************
        public  WEP
WEP     proc    far
        inc     bp                      ; indicate far call
        push    bp                      ; save bp
        mov     bp,sp                   ; get access to parm
        push    ds                      ; save ds
        mov     ds,cs:dsvalue           ; get our DS value
        assume  ds:DGROUP
        cmp     byte ptr __WEPflag,0    ; quit if WEP already run
        jne     short WEP_exit          ; ...
        inc     byte ptr __WEPflag      ; set flag
        mov     eax,__WEPAddr           ; get addr of 32-bit __WEP routine
        or      eax,eax                 ; check for NULL routine
        je      short no_WEP            ; skip if no WEP routine defined
        sub     eax,eax                 ; zero 32-bit register
        mov     ax,6[bp]                ; get parm
        mov     word ptr [_EntryStackSave],sp   ; save 16-bit stack pointer
        mov     word ptr [_EntryStackSave+2],ss ; ...
        mov     dx,word ptr _CodeEntry+4; get 32-bit CS
        mov     es,_DataSelector        ; load 32-bit DS
        mov     ss,_StackSelector       ; switch to 32-bit stack
        mov     esp,_SaveSP             ; ...
        push    edx                     ; push 32-bit CS
        push    __WEPAddr               ; push address of __WEP routine
        push    _EntryStackSave         ; save 16-bit stack pointer
        mov     ds,_DataSelector        ; load 32-bit DS
        call    fword ptr 4[esp]        ; call __WEP
        lss     sp,[esp]                ; switch back to 16-bit stack
no_WEP: push    0                       ; indicate no message
        call    Fini_                   ; do final cleanup
        add     sp,2                    ; clean up stack
WEP_exit:mov    ax,1                    ; indicate success
        pop     ds                      ; restore ds
        pop     bp                      ; restore bp
        dec     bp                      ; decrement bp
        ret     2                       ; return
WEP     endp

        DLL_Entry       macro   num
        public  DLL&num
DLL&num proc    near
        call    __DLL_entry
        nop
DLL&num endp
        endm

        DLL_Entry       1
        DLL_Entry       2
        DLL_Entry       3
        DLL_Entry       4
        DLL_Entry       5
        DLL_Entry       6
        DLL_Entry       7
        DLL_Entry       8
        DLL_Entry       9
        DLL_Entry       10
        DLL_Entry       11
        DLL_Entry       12
        DLL_Entry       13
        DLL_Entry       14
        DLL_Entry       15
        DLL_Entry       16
        DLL_Entry       17
        DLL_Entry       18
        DLL_Entry       19
        DLL_Entry       20
        DLL_Entry       21
        DLL_Entry       22
        DLL_Entry       23
        DLL_Entry       24
        DLL_Entry       25
        DLL_Entry       26
        DLL_Entry       27
        DLL_Entry       28
        DLL_Entry       29
        DLL_Entry       30
        DLL_Entry       31
        DLL_Entry       32
        DLL_Entry       33
        DLL_Entry       34
        DLL_Entry       35
        DLL_Entry       36
        DLL_Entry       37
        DLL_Entry       38
        DLL_Entry       39
        DLL_Entry       40
        DLL_Entry       41
        DLL_Entry       42
        DLL_Entry       43
        DLL_Entry       44
        DLL_Entry       45
        DLL_Entry       46
        DLL_Entry       47
        DLL_Entry       48
        DLL_Entry       49
        DLL_Entry       50
        DLL_Entry       51
        DLL_Entry       52
        DLL_Entry       53
        DLL_Entry       54
        DLL_Entry       55
        DLL_Entry       56
        DLL_Entry       57
        DLL_Entry       58
        DLL_Entry       59
        DLL_Entry       60
        DLL_Entry       61
        DLL_Entry       62
        DLL_Entry       63
        DLL_Entry       64
        DLL_Entry       65
        DLL_Entry       66
        DLL_Entry       67
        DLL_Entry       68
        DLL_Entry       69
        DLL_Entry       70
        DLL_Entry       71
        DLL_Entry       72
        DLL_Entry       73
        DLL_Entry       74
        DLL_Entry       75
        DLL_Entry       76
        DLL_Entry       77
        DLL_Entry       78
        DLL_Entry       79
        DLL_Entry       80
        DLL_Entry       81
        DLL_Entry       82
        DLL_Entry       83
        DLL_Entry       84
        DLL_Entry       85
        DLL_Entry       86
        DLL_Entry       87
        DLL_Entry       88
        DLL_Entry       89
        DLL_Entry       90
        DLL_Entry       91
        DLL_Entry       92
        DLL_Entry       93
        DLL_Entry       94
        DLL_Entry       95
        DLL_Entry       96
        DLL_Entry       97
        DLL_Entry       98
        DLL_Entry       99
        DLL_Entry       100
        DLL_Entry       101
        DLL_Entry       102
        DLL_Entry       103
        DLL_Entry       104
        DLL_Entry       105
        DLL_Entry       106
        DLL_Entry       107
        DLL_Entry       108
        DLL_Entry       109
        DLL_Entry       110
        DLL_Entry       111
        DLL_Entry       112
        DLL_Entry       113
        DLL_Entry       114
        DLL_Entry       115
        DLL_Entry       116
        DLL_Entry       117
        DLL_Entry       118
        DLL_Entry       119
        DLL_Entry       120
        DLL_Entry       121
        DLL_Entry       122
        DLL_Entry       123
        DLL_Entry       124
        DLL_Entry       125
        DLL_Entry       126
        DLL_Entry       127
        DLL_Entry       128

__DLL_entry proc        near
        sub     EDX,EDX                 ; zero full register
        pop     DX                      ; get return address
        sub     DX,offset DLL1 - 1      ; calc. index * 4
        inc     bp                      ; indicate far return address
        push    bp                      ; save bp
        mov     bp,sp                   ; get access to parms
        lea     cx,[bp+6-2]             ; point to stack parms
        call    __CommonLibEntry        ; goto common library entry
        pop     bp                      ; restore bp
        dec     bp                      ; decrement bp
        pop     ebx                     ; return address
        add     sp,cx                   ; number of bytes for parms
        push    ebx                     ; restore return address
        retf                            ; go back
__DLL_entry endp

;****************************************************************************
;***                                                                      ***
;*** Win386LibEntry - the single 16-bit entry point for all stuff         ***
;***                  the first word on the stack is the function number, ***
;***                  all other data is determined by the function -      ***
;***                  the user must assign all library entry points       ***
;***                  during initialization                               ***
;***                                                                      ***
;****************************************************************************
public Win386LibEntry
Win386LibEntry proc far
        inc     bp                      ; indicate far call
        push    bp                      ; save bp
        mov     bp,sp                   ; get access to parms
        sub     edx,edx                 ; zero full register
        mov     dx,6[bp]                ; get DLL entry number
        shl     dx,2                    ; times 4
        lea     cx,[bp+6]               ; point to stack parms
        call    __CommonLibEntry        ; goto common library entry
        pop     bp                      ; restore bp
        dec     bp                      ; decrement bp
        pop     ebx                     ; return address
        add     sp,cx                   ; number of bytes for parms
        add     sp,2                    ; remove the extra parm
        push    ebx                     ; restore return address
        ret                             ; go back
Win386LibEntry endp

__CommonLibEntry proc near
        push    si                      ; save si
        push    di                      ; save di
        push    ds                      ; save ds
        push    es                      ; save es
;*
;** save stack info, set up new stack
;*
        mov     ax,ss                   ; get old ss
        mov     es,ax                   ;   to unload parms
        mov     ds,cs:dsvalue           ; get our DS value
        assume  ds:DGROUP
        mov     ax,ds                   ; set GS=DS
        mov     gs,ax                   ; ...

        push    _SaveSP                 ; save current 32-bit sp

        mov     word ptr ds:[_EntryStackSave],sp   ; save current sp
        mov     word ptr ds:[_EntryStackSave+2],ss ; save stack segment

        mov     ss,_StackSelector       ; new sp
        mov     esp,_SaveSP             ; current 32 bit stack

;*
;*** load the magic registers
;*
        mov     ax,_DataSelector        ; load data selector
        mov     ds,ax                   ; get 32 bit data selector
        push    gs                      ; save this ds

;*
;*** build entry point
;*** entry as follows:
;***
;***    LONG  - routine         (offset 0)
;***    SHORT - number of parms (offset 4)
;***    SHORT - total bytes     (offset 6)
;***    SHORT - size 1          (offset 8)
;***    ...
;***    SHORT - size N          (offset 6+N*2)
;*
        mov     si,cx                   ; point to stack parms
        add     edx,gs:__DLLEntryAddr   ; dll table addr
        mov     ebx,ds:[edx]            ; get specific dll entry
        mov     eax,ds:[ebx]            ; get address of routine
        mov     gs:DLLEIP,eax           ; save it
        mov     cx,word ptr ds:[ebx+4]  ; # of parms
        mov     dx,word ptr ds:[ebx+6]  ; # of bytes
        lea     edi,ds:[ebx+8]          ; first parm size entry
        add     si,dx                   ; go to first parm
        push    dx                      ; save # of bytes
        cmp     word ptr ds:[edi],4     ; is parm 4 bytes in size?
        jne     loopme                  ; no
        sub     si,2                    ; get to start of parm
loopme:
        cmp     cx,0                    ; done?
        je      doneparms               ; yep
        mov     ax,word ptr ds:[edi]    ; get size
        cmp     ax,2                    ; two bytes?
        jne     get4
        movzx   eax,word ptr es:[si]    ; got it
        jmp     short nextone
get4:   mov     eax,dword ptr es:[si]
nextone:push    eax
        dec     cx                      ; decrement count
        add     edi,2                   ; next size
        mov     ax,word ptr ds:[edi]
        sub     si,ax                   ; point at next guy
        jmp     loopme                  ; try again

;*
;*** invoke the 32-bit call back routine
;*
doneparms:
        mov     ax,ds
        mov     es,ax                   ; point es into flat area
        call    gs:[DLLRoutine]         ; invoke the 32 bit appl

;*
;*** restore state
;*
        pop     cx                      ; number of bytes extra on stack
        pop     ds                      ; dll's ds

        mov     ss,word ptr ds:[_EntryStackSave+2]
        movzx   esp,word ptr ds:[_EntryStackSave]

        pop     ds:_SaveSP

        pop     di                      ; restore es
        lsl     dx,di                   ; see if still valid?
        jz      short es_ok             ; jump if selector is valid
        sub     di,di                   ; set selector to 0
es_ok:  mov     es,di                   ; restore es

        pop     di                      ; restore ds
        lsl     dx,di                   ; see if still valid?
        jz      short ds_ok             ; jump if selector is valid
        sub     di,di                   ; set selector to 0
ds_ok:  mov     ds,di                   ; restore ds

        pop     di                      ; restore di
        pop     si                      ; restore si
        mov     edx,eax                 ; put the return code
        shr     edx,16                  ;   from eax to dx:ax
        ret                             ; return
__CommonLibEntry endp


;****************************************************************************
;***                                                                      ***
;*** StartDLL32 - invoke a 32-bit dll's entry code                        ***
;***                                                                      ***
;***       The 32 bit application must save gs, since that is our pointer ***
;***       to our data segment (given to us by windows)                   ***
;***                                                                      ***
;***       Our program image is as follows:                               ***
;***                                                                      ***
;***            0 -> send          Stack                                  ***
;***            send+1 -> cend     Code                                   ***
;***            cend+1 -> ì        Data                                   ***
;***                                                                      ***
;****************************************************************************
public StartDLL32
StartDLL32 proc near
        push    bp                      ; save bp
        mov     bp,sp                   ; get access to stack
        push    si                      ; save si
        push    di                      ; save di

;*
;*** prepare to invoke the application: save stack, load selectors, and call
;*
        mov     ax,word ptr ds:_CodeEntry+4     ; get CS of 32 bit app
        mov     DLLCS,ax                        ; stash it
        mov     word ptr [_EntryStackSave],sp   ; save current sp
        mov     word ptr [_EntryStackSave+2],ss ; save stack segment
        mov     edx,_SaveSP             ; load up 32-bit stack pointer
        mov     ecx,edx                 ; calc value for __STACKLOW
        sub     ecx,_StackSize          ; ...
        add     ecx,256                 ; ...
        push    ds                      ; load up gs
        pop     gs                      ;     with ds
        mov     ss,_StackSelector       ; switch to 32-bit stack
        mov     esp,edx                 ; ...
        push    _EntryStackSave         ; save 16-bit SS:SP
        mov     edi,_EDataAddr          ; get end of code+data
        mov     ax,_DataSelector        ; load data selector
        mov     ds,ax                   ; get 32 bit data selector
        mov     es,ax                   ; give it to es
        mov     fs,ax                   ;     and fs too
        mov     bl,1                    ; indicate DLL
        cmp     gs:_InDebugger,1        ; debugging?
        jne     short docall            ; no, skip breakpoint
        mov     ax,word ptr gs:_CodeEntry+4     ; load CS for sampler
        int     1                       ; yes, break
docall:
        call    gs:[_CodeEntry]         ; invoke the 32 bit appl
;*
;*** restore data segment and stack (ds is restored before return)
;*
        lss     sp,[esp]                ; switch back to 16-bit stack
        mov     _ReturnCode,eax         ; get return code
        xor     ax,ax                   ; get rid of any
        mov     es,ax                   ;   potential guys
        mov     fs,ax                   ;     containing our
        mov     gs,ax                   ;       32-bit selector

        pop     di                      ; restore di
        pop     si                      ; restore si
        pop     bp                      ; restore bp
        ret                             ; return

StartDLL32 endp
_TEXT   ends
        end     LibEntry
