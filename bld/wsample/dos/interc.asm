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


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; these routines are part of a system largely described in SAMPLE.C
;
        name    INTERC

        extrn   StopAndSave_        : near
        extrn   WriteMark_          : near
        extrn   WriteOvl_           : near
        extrn   StartTimer_         : near
        extrn   StopTimer_          : near
        extrn   RememberComm_       : near

DGROUP  group   _DATA
_DATA   segment word public 'DATA'
        extrn   _Save_Request       : word
_DATA   ends

_TEXT   segment word public 'CODE'
;
; code segment data
;
int21_retseg    dw      SEG int21_return
int21_retoff    dw      OFFSET int21_return

intxx_retseg    dw      SEG intxx_return
intxx_retoff    dw      OFFSET intxx_return

InDOSAddr       dd      ?
ErrorModeAddr   dd      ?
InBIOS          dw      ?

                public  _old_int28,_old_int21,_old_int13
_old_int28      dd      ?
_old_int21      dd      ?
_old_int13      dd      ?

                public  _old_intx0,_old_intx1,_old_intx2,_old_intx3
                public  _old_intx4,_old_intx5,_old_intx6,_old_intx7
_old_intx0      dd      ?
_old_intx1      dd      ?
_old_intx2      dd      ?
_old_intx3      dd      ?
_old_intx4      dd      ?
_old_intx5      dd      ?
_old_intx6      dd      ?
_old_intx7      dd      ?

                public _SysCallerAddr
                public _SysCaught
                public _SysNoDOS
_SysCallerAddr  dd      ?
_SysCaught      db      ?
_SysNoDOS       db      ?

_sysCaller      db      ?
;
; code starts here
;
        ASSUME  cs:_TEXT,ds:nothing,es:nothing

;
; *** INT 13 handler ***
;
        public  int13_handler_
int13_handler_  proc    far
        inc     word ptr cs:InBIOS  ; say we're in the BIOS
        pushf                       ; invoke old handler
        call    cs:_old_int13
        pushf                       ; save flags
        dec     word ptr cs:InBIOS  ; we're out of BIOS
        popf                        ; restore flags
        ret     2                   ; return to caller
int13_handler_  endp

;
; *** Overlay handler ***
;
        public  ovl_handler_
ovl_handler_    proc    far
        push    ax              ; save registers
        push    bx              ; ...
        push    cx              ; ...
        push    dx              ; ...
        push    si              ; ...
        push    ds              ; ...
        mov     si,seg DGROUP   ; setup DS registers
        mov     ds,si           ; ...
        call    WriteOvl_       ; write out current ovl state
        pop     ds              ; restore registers
        pop     si              ; ...
        pop     dx              ; ...
        pop     cx              ; ...
        pop     bx              ; ...
        pop     ax              ; ...
        ret                     ; return to caller
ovl_handler_    endp

;
; *** INT 03 Handler ***
;
        public  int03_handler_
int03_handler_  proc    far     ; mark interceptor
        push    bp              ; save bp
        mov     bp,sp           ; get access to stack
        push    ds              ; save registers
        push    si              ; ...
        mov     si,seg DGROUP   ; setup DS register
        mov     ds,si           ; ...
        test    dx,dx           ; if dx is non-NULL, this is a real mark
        jz      NoMark          ; otherwise, it signals callgraph info
IsMark  LABEL   near            ; we have a real mark
        mov     bx,2[bp]        ; get interrupt offset
        dec     bx              ; back up to start of opcode
        mov     cx,4[bp]        ; get interrupt segment
        call    WriteMark_      ; write out the mark
        jmp     EndMark         ; return to program
NoMark  LABEL   near            ; we have new callgraph information
        mov     ax,bx
        mov     dx,cx
        call    RememberComm_   ; remember common storage area
EndMark LABEL   near            ; resume program execution
        pop     si              ; restore registers
        pop     ds              ; ...
        pop     bp              ; ...
        iret                    ; return to caller
int03_handler_  endp

;
; UnHook -- temporarily stop the timer
;
UnHook  LABEL   near
        push    ds
        push    ax
        mov     ax,seg DGROUP
        mov     ds,ax
        call    StopTimer_
        pop     ax
        pop     ds
        call    cs:_old_int21      ;this is a far indirect call
        pushf
        push    ds
        push    ax
        mov     ax,seg DGROUP
        mov     ds,ax
        ASSUME  ds:DGROUP
        call    StartTimer_
        cli
        xor     ax,ax
        xchg    ax,_Save_Request; clears _Save_Request (StopAndSave needs this)
        test    ax,ax           ; any requests?
        jz      UnHook1         ; zero -> NO
        call    DOSState        ; is DOS stable?
        jc      UnHook1         ; carry set -> NO
        call    StopAndSave_    ; _Save_Request must be 0 (DOS will be called)
UnHook1 LABEL   near
        ASSUME  ds:nothing
        pop     ax
        pop     ds
        popf
        retf    2               ; return and ignore flags

;
; *** INT 21 Handler ***
;
        public  int21_handler_
int21_handler_ proc far
        pushf
        cmp     cs:_sysCaller,0
        jne     XDD1
        cmp     cs:_SysNoDOS,0
        jne     XDD1
        popf
        pop     word ptr cs:_SysCallerAddr   ; pop off seg:off of caller
        pop     word ptr cs:_SysCallerAddr+2 ; (without changing flags)
        push    cs:int21_retseg ; replace with unwind21 seg:off
        push    cs:int21_retoff ; (without changing flags)
        pushf
        mov     cs:_sysCaller,1
        mov     cs:_SysCaught,1

XDD1    LABEL   near
        cmp     ax,2508h        ; setting the timer interrupt?
        je      UnHook          ; do magic
        cmp     ax,3508h        ; getting the timer interrupt?
        je      UnHook          ; do magic

        popf
        jmp     cs:_old_int21

;
; *** INT 21 return interceptor ***
;
int21_return LABEL near         ; we will be returning to the application
        push    word ptr cs:_SysCallerAddr+2 ; push seg:off of caller
        push    word ptr cs:_SysCallerAddr   ; (without changing flags)
        mov     cs:_SysCaught,0 ; no more samples assigned to application now
        mov     cs:_sysCaller,0
        pushf
        push    ds
        push    ax
        mov     ax,DGROUP
        mov     ds,ax
        ASSUME  ds:DGROUP
        cli
        xor     ax,ax
        xchg    ax,_Save_Request; clears _Save_Request (StopAndSave needs this)
        test    ax,ax           ; any requests?
        jz      XL2             ; zero -> NO
        call    DOSState        ; is DOS stable?
        jc      XL2             ; carry set -> NO
        call    StopAndSave_    ; _Save_Request must be 0 (DOS will be called)
XL2     LABEL   near
        ASSUME  ds:nothing
        pop     ax
        pop     ds
        popf
        retf
int21_handler_  endp

;
; *** INT 28 Handler ***
;
        public  int28_handler_
int28_handler_  proc    far
        pushf
        push    ds              ; get addressability for DGROUP
        push    ax
        mov     ax,DGROUP
        mov     ds,ax
        ASSUME  ds:DGROUP

        xor     ax,ax
        xchg    ax,_Save_Request; clears _Save_Request (StopAndSave needs this)
        test    ax,ax           ; any requests?
        je      XL1             ; 0 - NO
        call    StopAndSave_
XL1     LABEL   near
        ASSUME  ds:nothing
        pop     ax
        pop     ds
        popf
        jmp    cs:_old_int28
int28_handler_  endp

;
; *** INT XX Handlers *** (used by /i option)
;
        public  intx0_handler_
intx0_handler_ proc far
        call    intxx_splice
        jmp     cs:_old_intx0
intx0_handler_ endp

        public  intx1_handler_
intx1_handler_ proc far
        call    intxx_splice
        jmp     cs:_old_intx1
intx1_handler_ endp

        public  intx2_handler_
intx2_handler_ proc far
        call    intxx_splice
        jmp     cs:_old_intx2
intx2_handler_ endp

        public  intx3_handler_
intx3_handler_ proc far
        call    intxx_splice
        jmp     cs:_old_intx3
intx3_handler_ endp

        public  intx4_handler_
intx4_handler_ proc far
        call    intxx_splice
        jmp     cs:_old_intx4
intx4_handler_ endp

        public  intx5_handler_
intx5_handler_ proc far
        call    intxx_splice
        jmp     cs:_old_intx5
intx5_handler_ endp

        public  intx6_handler_
intx6_handler_ proc far
        call    intxx_splice
        jmp     cs:_old_intx6
intx6_handler_ endp

        public  intx7_handler_
intx7_handler_ proc far
        call    intxx_splice
        jmp     cs:_old_intx7
intx7_handler_ endp

;
; intxx_splice -- an INT XX has occurred; check whether an application
;                 return address is stored already (will be set if it isn't)
;
public intxx_splice
intxx_splice proc near
        pushf
        cmp     cs:_sysCaller,0
        jne     intxx_go
        push    bp
        mov     bp,sp
        push    ax
        mov     ax,cs:intxx_retoff      ; replace seg:off of caller with
        xchg    ax,6[bp]                ;  intxx_return address
        mov     word ptr cs:_SysCallerAddr,ax
        mov     ax,cs:intxx_retseg
        xchg    ax,8[bp]
        mov     word ptr cs:_SysCallerAddr+2,ax
        mov     cs:_sysCaller,1
        mov     cs:_SysCaught,1
        pop     ax
        pop     bp
intxx_go LABEL  near
        popf
        ret
intxx_splice endp

;
; *** INT XX return interceptor ***
;
intxx_return proc far
        push    word ptr cs:_SysCallerAddr+2 ; push seg:off of caller
        push    word ptr cs:_SysCallerAddr   ; (without changing flags)
        mov     cs:_SysCaught,0 ; no more samples assigned to application now
        mov     cs:_sysCaller,0
        ret                     ; return to application
intxx_return endp

;
; InDOS_ -- returns non-zero if DOS cannot be entered
;
        public  InDOS_
InDOS_  proc    near
        call    DOSState        ; sets carry if DOS is unavailable
        sbb     ax,ax           ; 0 - OK, -1 - DOS unavailable
        ret
InDOS_  endp

;
; VersionCheck -- initializes the addresses for internal DOS flags
;                 (InDOSAddr & ErrorModeAddr)
;
        public  VersionCheck_
VersionCheck_ proc near
        push    bx
        push    cx
        push    es
        push    di
        push    ds

        push    ss
        pop     ds
        ASSUME  ds:DGROUP

        mov     ah,34h
        int     21h             ; &InDOS -> es:bx
        mov     word ptr cs:InDOSAddr,bx
        mov     word ptr cs:InDOSAddr+2,es

        mov     ah,30h          ; Get MS-DOS Version Number
        int     21h             ; sets bx=0, cx=0
        xchg    ah,al           ; original order is major:al minor:ah
        cmp     ah,02h          ; less than V2.xx?
        jb      Verror

        cmp     ax,030ah        ; less than V3.10? (0ah = 10 decimal)
        jb      tough_search
        cmp     ax,0a00h        ; OS/2 DOS box?
        jae     tough_search

;
; V3.10 and higher have the flags right beside each other
; (one byte below INDOS flag)
;
        mov     bx,word ptr cs:InDOSAddr
        dec     bx
        jmp     Vsuccess

tough_search:
        mov     cx,0ffffh
        xor     di,di           ; ES is set to correct seg from 34h function

find_int28 LABEL near
        mov     ax,word ptr cs:LF2      ; scan for INT 28
try_again LABEL near
        repne   scasb
        jne     Verror
        cmp     ah,es:[di]              ; check second byte for 28h
        jne     try_again

        mov     ax,word ptr cs:LF1+1    ; CMP opcode (+1 skips SS override)
        cmp     ax,es:[di][LF1-LF2]
        jne     try_test_op             ; if it's not CMP

        mov     bx,es:[di][(LF1-LF2)+2] ; found offset
        jmp     VSuccess

try_test_op LABEL near
        mov     ax,word ptr cs:LF3+1    ; TEST opcode (+1 skips SS override)
        cmp     ax,es:[di][LF3-LF4]
        jne     find_int28

        mov     bx, es:[di][(LF3-LF4)+2]; found offset
        jmp     Vsuccess

Vsuccess LABEL  near
        mov     word ptr cs:ErrorModeAddr,bx
        mov     word ptr cs:ErrorModeAddr+2,es
        mov     ax,1
Vexit   LABEL   near
        pop     ds
        pop     di
        pop     es
        pop     cx
        pop     bx
        ret

Verror: xor     ax,ax
        jmp     short Vexit

;
; the following are DOS code fragments that are used to find the code within
; DOS that checks the flags in question (see MSDOS encyclopedia)
;
LFnear      LABEL   near
LFbyte      LABEL   byte
LFword      LABEL   word

;
; *** PATTERN 1 ***
;
LF1:    cmp     ss:LFbyte,0     ; cmp    ErrorMode,0
        jne     LFnear
LF2:    int     28h

;
; *** PATTERN 2 ***
;
LF3:    test    ss:LFbyte, 0ffH ; test   ErrorMode,0ffH
        jne     LFnear
        push    ss:LFword
LF4:    int     28h

VersionCheck_ endp

;
; DOSState -- set carry if DOS cannot be called yet
;
DOSState proc   near
        push    ax
        push    bx
        push    es
        xor     bx,bx
        cmp     bx,cs:InBIOS    ; carry set if InBIOS != 0
        jc      bad
        les     bx,cs:ErrorModeAddr
        mov     ah,es:[bx]
        les     bx,cs:InDOSAddr
        mov     al,es:[bx]
        xor     bx,bx
        cmp     bx,ax           ; carry set if ax != 0
bad:    pop     es
        pop     bx
        pop     ax
        ret
DOSState endp

;
; DOSVer -- get DOS version number in ah:al (major:minor)
;
DOSVer  proc    near
        push    bx
        push    cx
        mov     ah,30h          ; Get MS-DOS Version Number
        int     21h             ; sets bx=0, cx=0
        xchg    ah,al           ; original order is major:al minor:ah
        pop     cx
        pop     bx
        ret                     ; major:ah minor:al
DOSVer  endp

_TEXT   ends

        end
