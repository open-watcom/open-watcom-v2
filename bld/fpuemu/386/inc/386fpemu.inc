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
;* Description:  387 emulator and instruction decoding. 
;*
;*****************************************************************************


ifndef NDEBUG
    if _OS eq _NW
      extrn  puts_ : near
      NC = 0
    else
      NC = '$'
    endif
endif
ifndef __WIN387__
        extrn   __GETDS : near
if _OS eq _QNX
        extrn   "C",__FPEhandler : near
else
        extrn   __FPE_exception_  : near
endif
endif

ifndef NDEBUG
  public  badop,prtnyi,my87,mrm,sib,memop,stkop,stkop1
endif

sarea   struc
control dw              0,0                     ; CW
status  dw              0,0                     ; SW
tags    dw              0,0                     ; TAG
iptr    dd              0,0                     ; instruction pointer
dataptr dd              0,0                     ; data operand pointer
fpstack db              80 dup(0)               ; 8 tbyte reals for stack
fptemp  db              10 dup(0)               ; temporary tbyte real
stktop  dd              0                       ; stack top * 10
ifndef __WIN387__
precrtn dd              0                       ; address of precision rtn
endif
sarea   ends

NDP_SIZE        equ     108
ENV_SIZE        equ     28

include fpubits.inc

        startdata
if _OS ne _QNX
my87            sarea           <>
endif
ifdef __WIN387__
precrtn dd              p_extended ; address of precision rtn
endif
ifndef NDEBUG
    nyimsg      db ": not yet implemented",0dH,0aH,NC
    illopmsg    db "Illegal instruction encountered by 8087 emulator",0dH,0aH,NC
endif
ifdef __WIN387__
    WIN8087 dd 0
    WINBase dd 0
    WINVMHandle dd 0
endif
        enddata

include consts.inc
include tentab.inc
include tagtab.inc
include fpucc.inc

ifdef SEGMENTED
    TENTBL equ  cs:tentab
    TAGTBL equ  cs:tagtab
else
    TENTBL equ  tentab
    TAGTBL equ  tagtab
endif
;
; NOTE: to make this damn thing work for windows, you have to use
;       MASM5. ANNNNND, masm5 is too stupid to figure out that these
;       offsets should be 32-bit offsets, SOOOOOO, you have to use
;       this stupid macro to make it work.  The macro
;       should be commented out if using masm, since masm doesn't
;       understand.  I hate microsoft.

        align   4
mrm     dd      m00000                 ; mod r/m byte select table
        dd      m00001
        dd      m00010
        dd      m00011
        dd      dosib
        dd      m00101
        dd      m00110
        dd      m00111
        dd      m01000
        dd      m01001
        dd      m01010
        dd      m01011
        dd      dosib
        dd      m01101
        dd      m01110
        dd      m01111
        dd      m10000
        dd      m10001
        dd      m10010
        dd      m10011
        dd      dosib
        dd      m10101
        dd      m10110
        dd      m10111

sib     dd      m00000                 ; sib base/displacement table
        dd      m00001
        dd      m00010
        dd      m00011
        dd      s00100
        dd      m00101
        dd      m00110
        dd      m00111
        dd      m01000
        dd      m01001
        dd      m01010
        dd      m01011
        dd      s01100
        dd      m01101
        dd      m01110
        dd      m01111
        dd      m10000
        dd      m10001
        dd      m10010
        dd      m10011
        dd      s10100
        dd      m10101
        dd      m10110
        dd      m10111

memop   dd      m00,m01,m02,m03,m04,m05,m06,m07         ; memory instr
        dd      m10,m11,m12,m13,m14,m15,m16,m17         ; select table
        dd      m20,m21,m22,m23,m24,m25,m26,m27
        dd      m30,m31,m32,m33,m34,m35,m36,m37
        dd      m40,m41,m42,m43,m44,m45,m46,m47
        dd      m50,m51,m52,m53,m54,m55,m56,m57
        dd      m60,m61,m62,m63,m64,m65,m66,m67
        dd      m70,m71,m72,m73,m74,m75,m76,m77

stkop   dd      s00,s01,s02,s03,s04,s05,s06,s07         ; stack operand
        dd      s10,s11,s12,s13,s14,s15,s16,s17         ; select table
        dd      s20,s21,s22,s23,s24,s25,s26,s27
        dd      s30,s31,s32,s33,s34,s35,s36,s37
        dd      s40,s41,s42,s43,s44,s45,s46,s47
        dd      s50,s51,s52,s53,s54,s55,s56,s57
        dd      s60,s61,s62,s63,s64,s65,s66,s67
        dd      s70,s71,s72,s73,s74,s75,s76,s77
        dd      calc_st0       ; _0_ entries
        dd      calc_sti       ; _i_ entries
;
;       stkop1 table entries contain an index into stkop table
;       _0_ indicates that the result will be stored in st(0)
;       _i_ indicates that the result will be stored in st(i)

_0_     equ     100o
_i_     equ     101o

stkop1  db      _0_,_0_,02o,03o,_i_,_0_,_i_,_0_         ; stack operand
        db      _0_,_0_,12o,13o,_i_,_0_,_i_,_0_         ; select table
        db      _0_,21o,22o,23o,_0_,_0_,26o,_0_
        db      _0_,_0_,32o,33o,_0_,_0_,36o,_0_
        db      _0_,41o,42o,43o,_i_,_0_,_i_,47o
        db      _0_,51o,52o,53o,_i_,_0_,_i_,57o
        db      _0_,61o,62o,63o,_i_,65o,_i_,67o
        db      _0_,71o,72o,73o,_i_,75o,_i_,77o

get_st0 macro
      ifdef SEGMENTED
        mov     edi,dword ptr ds:[ebp].stktop;- di gets index of top of stack
      else
        mov     edi,dword ptr    [ebp].stktop;- di gets index of top of stack
      endif
        endm

;;      and     edi,SW_ST               ; - ...
;;      shr     edi,SW_ST_SHIFT         ; - ...
;;      lea     edi,[edi+edi*4]         ; - ... times 5
;;      add     edi,edi                 ; - ... ... times 2
;;      endm


jmptab  macro   a,b,c,d,e,f,g,h
        local   table
        jmp     dword ptr cs:table[ebx*4]
table   dd      a
        dd      b
        dd      c
        dd      d
        dd      e
        dd      f
        dd      g
        dd      h
        endm

saveflg macro                                   ; MACRO
        inc     eax                             ; -1,0,1,2 -> 0,1,2,3
      ifdef SEGMENTED
        mov     dx,ds:[ebp].status              ; get status word
      else
        mov     dx,   [ebp].status              ; get status word
      endif
        and     dx,NOT SW_CC                    ; turn off C0,C1,C2,C3
      ifdef SEGMENTED
        or      dh,cs:fpbits[eax]               ; turn on new C0,C1,C2,C3
        mov     ds:[ebp].status,dx              ; save in status word
      else
        or      dh,   fpbits[eax]               ; turn on new C0,C1,C2,C3
        mov        [ebp].status,dx              ; save in status word
      endif
        endm

savecc  macro                                   ; MACRO
        and     eax,7                           ; keep low order 3 bits of quot
      ifdef SEGMENTED
        mov     dx,ds:[ebp].status              ; get status word
        and     dx,NOT SW_CC                    ; turn off C0,C1,C2,C3
        or      dh,cs:ccbits[eax]               ; turn on new C0,C1,C3
        mov     ds:[ebp].status,dx              ; save in status word
      else
        mov     dx,   [ebp].status              ; get status word
        and     dx,NOT SW_CC                    ; turn off C0,C1,C2,C3
        or      dh,   ccbits[eax]               ; turn on new C0,C1,C3
        mov        [ebp].status,dx              ; save in status word
      endif
        endm

tag_st0 macro newtag
        mov     bx,TAGTBL[edi].t_off            ; get turn off mask
      ifdef SEGMENTED
        and     ds:[ebp].tags,bx                ; turn off the bits
      else
        and        [ebp].tags,bx                ; turn off the bits
      endif
      ifdif   <newtag>,<t_valid>
        mov     bx,TAGTBL[edi].&newtag          ; get new bits
        ifdef SEGMENTED
          or    ds:[ebp].tags,bx                ; turn on new bits
        else
          or       [ebp].tags,bx                ; turn on new bits
        endif
      endif
        endm

tag_sti macro newtag
        mov     bx,TAGTBL[esi].t_off            ; get turn off mask
      ifdef SEGMENTED
        and     ds:[ebp].tags,bx                ; turn off the bits
      else
        and        [ebp].tags,bx                ; turn off the bits
      endif
      ifdif   <newtag>,<t_valid>
        mov     bx,TAGTBL[esi].&newtag          ; get new bits
        ifdef SEGMENTED
          or    ds:[ebp].tags,bx                ; turn on new bits
        else
          or       [ebp].tags,bx                ; turn on new bits
        endif
      endif
        endm

pushstk macro
        decstp
        tag_st0 t_valid
        ; if full entry, to incstp and exception!
        ; mark as valid, zero or invalid
        endm

decstp  macro                                   ; MACRO
        movzx   edi,TENTBL[edi].t_prev          ; dec stack pointer
        mov     bx,TENTBL[edi].t_stktop         ; update top of stack pointer
      ifdef SEGMENTED
        and     ds:[ebp].status,NOT SW_ST       ;...
        or      word ptr ds:[ebp].status,bx     ; put back into SW_ST
        mov     ds:[ebp].stktop,edi
      else
        and        [ebp].status,NOT SW_ST       ;...
        or      word ptr    [ebp].status,bx     ; put back into SW_ST
        mov     [ebp].stktop,edi
      endif
        endm

popstk  macro
        ; if empty entry, take exception
        ; mark as empty
        tag_st0 t_empty
        incstp
        endm

incstp  macro                                   ; MACRO
        movzx   edi,TENTBL[edi].t_next          ; inc stack pointer
        mov     bx,TENTBL[edi].t_stktop         ; update top of stack pointer
      ifdef SEGMENTED
        and     ds:[ebp].status,NOT SW_ST       ;...
        or      word ptr ds:[ebp].status,bx     ; put back into SW_ST
        mov     ds:[ebp].stktop,edi
      else
        and        [ebp].status,NOT SW_ST       ;...
        or      word ptr    [ebp].status,bx     ; put back into SW_ST
        mov     [ebp].stktop,edi
      endif
        endm

done    macro
        jmp     exit                            ; done instruction
        endm

popstk_done macro
        jmp     popstk_exit                     ; popstk then exit
        endm

saveflg_done    macro
        jmp     saveflg_exit                    ; saveflg then exit
        endm

saveflg_popstk_done     macro
        jmp     saveflg_popstk_exit             ; saveflg, popstk then exit
        endm


donearith macro
ifdef __WIN387__
        jmp     [precrtn]
else
      ifdef SEGMENTED
        jmp        ds:[ebp].precrtn             ; done instruction
      else
        jmp        [ebp].precrtn                ; done instruction
      endif
endif
        endm

getsavearea     macro
        ; - point to 387 save area
ifdef __WIN387__
        mov     ebp,WIN8087
else
if _OS eq _QNX
        lds     ebp, S_SAVEAREA[ebp]
else
ifdef SEGMENTED
        call    __GETDS
endif
        lea     ebp,my87
endif
endif
        endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; register conventions during instruction decoding
;
; - AH holds modrm byte
; - AL holds low 3 bits of opcode
; - CX value for segment (default DS)
; - DX value for segment to be used with [EBP] and [ESP]
; - DS:ESI points to our current byte to be picked up during decoding
;
; register conventions during instruction "execution"
;
; - DS:EBP points to my '8087' area
; - EDI is the top of stack index
; - - - DS:[EDI+EBP].fpstack points to ST(0)
; - ESI contains i from ST(i) for a non-memory operand
; - - - DS:[ESI+EBP].fpstack points to ST(i)
; - ES:ESI returns the effective address calculated for a memory operand
; - EAX ECX EBX EDX are scratch
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
ifdef __WIN387__
;
;       entry point for 387 Emulator under Microsoft Windows 3.0
;
S_DS            = Client_DS
S_SS            = Client_SS
S_ES            = Client_ES
S_FS            = Client_FS
S_GS            = Client_GS
S_EDI           = Client_EDI
S_ESI           = Client_ESI
S_EBP           = Client_EBP
S_ESP           = Client_ESP
S_EBX           = Client_EBX
S_EDX           = Client_EDX
S_ECX           = Client_ECX
S_EAX           = Client_EAX
S_EIP           = Client_EIP
S_CS            = Client_CS
S_FL            = Client_EFlags

        public  __Win387_emulator
__Win387_emulator proc near
        push    EBP                     ; save address of Client_Reg_Struct
        mov     WINBase,eax             ; save various pointers
        mov     WIN8087,ebx             ;    to the client memory
        mov     WINVMHandle,ecx         ;       in the flat addr. space

p_extended:
exit:   mov     ebp,[esp]               ; get access to saved registers

else ;===================================================================

if _OS eq _QNX
;
;       entry point for 387 Emulator under QNX/386 (32-bit side)
;
S_EFL   =       44H
S_CS    =       40H
S_EIP   =       3cH
S_EAX   =       38H
S_ECX   =       34H
S_EDX   =       30H
S_EBX   =       2cH
S_ESP   =       28H
S_EBP   =       24H
S_ESI   =       20H
S_EDI   =       1cH
S_DS    =       18H
S_ES    =       14H
S_FS    =       10H
S_GS    =       0cH
S_SS    =       08H
S_SAVEAREA =    04H

        public  __int7
__int7  proc    near
        push    ss                      ; save SS for emulator
        push    ebx                     ; push emulator save area offset
        call    start_emulation         ; start emulating
        add     esp,8                   ; throw away SS/save area value
        pop     gs                      ; restore sregs
        pop     fs                      ; ...
        pop     es                      ; ...
        pop     ds                      ; ...
        popad                           ; restore gprs
        iretd                           ; return from interrupt

start_emulation:
;;      get_st0                         ; get st*10 into edi
p_extended:
exit:   mov     ebp,esp                 ; get access to saved registers

else ;===================================================================
;
;       entry point for 387 Emulator under DOS-extenders
;
; from push instructions
Push_DS         =       4
Push_SS         =       Push_DS+2
Push_ES         =       Push_DS+4

; from pushad instruction
Push_EDI        =       Push_ES+4
Push_ESI        =       Push_EDI+4
Push_EBP        =       Push_ESI+4
Push_ESP        =       Push_EBP+4
Push_EBX        =       Push_ESP+4
Push_EDX        =       Push_EBX+4
Push_ECX        =       Push_EDX+4
Push_EAX        =       Push_ECX+4

; from dos extender's umbrella interrupt handler
X_EIP           =       Push_EAX+4
X_CS            =       X_EIP+4
X_FL            =       X_CS+4
Orig_EIP        =       X_FL+4
Orig_CS         =       Orig_EIP+4
Orig_FL         =       Orig_CS+4
Orig_ESP        =       Orig_FL+4
Orig_SS         =       Orig_ESP+4
Orig_ES         =       Orig_SS+4
Orig_DS         =       Orig_ES+4
Orig_FS         =       Orig_DS+4
Orig_GS         =       Orig_FS+4
X_INFO          =       Orig_GS+4
X_INUM          =       X_INFO+4
Orig_CR2        =       X_INUM+4

S_DS            = Push_DS
S_SS            = Push_SS
S_ES            = Push_ES
S_EDI           = Push_EDI
S_ESI           = Push_ESI
S_EBP           = Push_EBP
S_ESP           = Push_ESP
S_EBX           = Push_EBX
S_EDX           = Push_EDX
S_ECX           = Push_ECX
S_EAX           = Push_EAX
S_EIP           = Orig_EIP
S_CS            = Orig_CS
S_FL            = Orig_FL

;
;       entry point for Phar Lap version 3.0
;
adj     =  3*4

        public  __int7_pl3

        align   4
__int7_pl3  proc near
        sti                             ; interrupts back on
        pushad                          ; save gprs
        mov     ebp,esp                 ; get base
        push    dword ptr (Orig_ES)-adj[ebp]   ; push ES
        push    dword ptr (Orig_DS)-adj[ebp]   ; push DS
        mov     cx,Orig_SS-adj[ebp]     ; ...
        mov     S_SS-adj[ebp],cx        ; save SS
        mov     ecx,Orig_ESP-adj[ebp]   ; get original esp
        mov     S_ESP-adj[ebp],ecx      ; ...
        call    start_emulation         ; start emulating
        pop     ds                      ; restore DS
        pop     es                      ; restore ES
        popad                           ; restore gprs
        iretd                           ; return from interrupt
__int7_pl3 endp

        public  __int7_X32VM

        align   4
__int7_X32VM    proc    near
        sti                             ; interrupts back on
        mov     eax,12[esp]             ; get pointer to int_struct
        push    dword ptr ss:20[eax]    ; push original CS
        push    dword ptr ss:16[eax]    ; push original EIP
        push    eax                     ; save address of int_struct
        push    eax                     ; 2 more pushs to line up the stack
        push    eax                     ; ...
;;;;;   pushad                          ; save gprs
;;;     to avoid 386 chip bug when running on a 16-bit stack e.g. X32VM
;;;     we will do individual push instructions rather than using "pushad"
;;;                                                     22-nov-93
        push    dword ptr ss:[eax]      ; - eax
        push    ecx                     ; - ecx
        push    edx                     ; - edx
        push    ebx                     ; - ebx
        push    dword ptr ss:28[eax]    ; - push original ESP value
        mov     eax,ebp                 ; - get ebp
        push    eax                     ; - ebp
        push    esi                     ; - esi
        push    edi                     ; - edi
;;;                                     ; end of "pushad"
        push    es                      ; save es
        push    ds                      ; ...ds
        mov     2[esp],ss               ; ...and ss
        call    start_emulation         ; start emulating
        pop     ds                      ; restore ds
        pop     es                      ; restore es
;;;;;   popad                           ; restore gprs
;;;     to avoid 386 chip bug when running on a 16-bit stack e.g. X32VM
;;;     we will do individual pop instructions rather than using "popad"
;;;                                                     22-nov-93
        pop     edi                     ; - edi
        pop     esi                     ; - esi
        pop     eax                     ; - EBP
        mov     ebp,eax                 ; - ...
        pop     eax                     ; - esp (ignore)
        pop     ebx                     ; - ebx
        pop     edx                     ; - edx
        pop     ecx                     ; - ecx
        pop     eax                     ; - eax
        xchg    eax,[esp]               ; get pointer to int_struct/save eax
        pop     dword ptr ss:[eax]      ; restore eax
        add     esp,2*4                 ; readjust esp
        pop     dword ptr ss:16[eax]    ; copy up new EIP
        add     esp,4                   ; skip over CS
        mov     eax,ss:[eax]            ; reload eax
        iretd                           ; return from interrupt
__int7_X32VM endp

        public  __int7

        align   4
__int7  proc    near
        sub     esp,3*4                 ; adjust esp to match V3 entry condition
        sti                             ; interrupts back on
;;;;;   pushad                          ; save gprs
;;;     to avoid 386 chip bug when running on a 16-bit stack e.g. X32VM
;;;     we will do individual push instructions rather than using "pushad"
;;;                                                     22-nov-93
        push    eax                     ; - eax
        mov     eax,esp                 ; - get esp
        add     eax,12+4+3*4            ; - set ESP to pre-interrupt value
        push    ecx                     ; - ecx
        push    edx                     ; - edx
        push    ebx                     ; - ebx
        push    eax                     ; - adjusted ESP value
        push    ebp                     ; - ebp
        push    esi                     ; - esi
        push    edi                     ; - edi
;;;                                     ; end of "pushad"
        push    es                      ; save es
        push    ds                      ; ...ds
        mov     2[esp],ss               ; ...and ss
        call    start_emulation         ; start emulating
        pop     ds                      ; restore ds
        pop     es                      ; restore es
        popad                           ; restore gprs
        add     esp,3*4                 ; readjust esp
        iretd                           ; return from interrupt

        align   4
start_emulation:
;;      get_st0                         ; get st*10 into edi

p_extended:
exit:   mov     ebp,esp                 ; get access to saved registers

endif
endif
    ifdef __WIN387__
        mov     esi,S_EIP[ebp]          ; get eip
        add     esi,WINBase             ; add base to get at eip in ring 0
    else
      ifdef SEGMENTED
        mov     esi,S_EIP[ebp]          ; get return address
        mov     ds,S_CS[ebp]            ; ...
      else
        mov     esi,S_EIP[ebp]
      endif
    endif
        mov     cx,S_DS[ebp]            ; default segment is ds
        mov     dx,S_SS[ebp]            ; default segment for [EBP] and [ESP]
        mov     al,[esi]                ; get possible prefix in al
        cmp     al,9Bh                  ; check for "fwait"
        jbe     prefix_byte             ; ... go check for possible prefix
endlup:
        mov     bl,al                   ; get opcode
        and     bl,0F8h                 ; check to see if 8087 opcode
        cmp     bl,0D8h                 ; ...
        jne     exit99                  ; exit if not 8087 opcode
        mov     ah,1[esi]               ; get modrm in ah
        add     esi,2                   ; bump past opcode and modrm byte
        and     al,7                    ; isolate real opcode portion 00000ooo
        cmp     ah,0C0H                 ; if its a memory reference
        jnb     stkins                  ; - then
        mov     bl,ah                   ; - get r/m bits in bl        mmRRRrrr
        mov     bh,ah                   ; - get r/m bits in bh        mmRRRrrr
        shr     bh,3                    ; - ...                       000mmRRR
        and     ebx,01807H              ; -                  000mm000 00000rrr
        or      bl,bh                   ; - glue them                 000mmrrr
        mov     bh,0                    ; - zero top bits

      ifdef SEGMENTED
        call    dword ptr cs:mrm[ebx*4] ; - do effective address calcn
      else
        call    dword ptr    mrm[ebx*4] ; - do effective address calcn
      endif
      ifdef SEGMENTED
        mov     es,cx                   ; - segment of memory operand in es
      endif
ifdef __WIN387__
        _guess                          ; - guess: have to map in address
          cmp   cx,S_DS[ebp]            ; - - is it client's ds(/es/ss)?
          _quif e                       ; - - yes, use pre-calculated base
          cmp   cx,S_CS[ebp]            ; - - is it client's cs?
          _quif e                       ; - - yes, use pre-calculated base
          push  eax                     ; - - save eax
          movzx ecx,cx                  ; - - selector to map

          VxDcall3 _SelectorMapFlat, WINVMHandle, ecx, 0

          add   ebx,eax                 ; - - add base of selector to relative
                                        ;     offset from start of selector
          pop   eax                     ; - - restore eax
        _admit                          ; - admit: can use WINBase
          add   ebx,WINBase             ; - - shift address correctly!
        _endguess                       ; - endguess
        sub     esi,WINBase             ; - set to EIP within client's cs
        mov     S_EIP[ebp],esi          ; - save it
else
        mov     S_EIP[ebp],esi          ; - zap return address
endif
        mov     esi,ebx                 ; - get address

        mov     bl,ah                   ; - get r/m bits again
        and     ebx,038H                ; - isolate opcode portion     0RRR000
        or      bl,al                   ; - or in other part of opcode 0RRRooo

        getsavearea

        get_st0                         ; get st*10 into edi
      ifdef SEGMENTED
        jmp     dword ptr cs:memop[ebx*4];- emulate the instruction
      else
        jmp     dword ptr    memop[ebx*4];- emulate the instruction
      endif

luup:   mov     al,[esi]                ; get possible prefix in al
        cmp     al,9Bh                  ; check for "fwait"
        ja      endlup                  ; ... go check for possible prefix
prefix_byte: je short inc_esi           ; skip over "fwait" opcode
        cmp     al,26h                  ; check for "es:"
        je      short is_es             ; - continue with next byte
        cmp     al,2Eh                  ; if its cs:
        je      short is_cs             ; - record cs in dx and continue
        cmp     al,36H                  ; if its ss:
        je      short is_ss             ; - record ss in dx and continue
        cmp     al,3EH                  ; if its ds:
        je      short is_ds             ; - record ds in dx and continue
        cmp     al,64H                  ; if its fs:
        je      short is_fs             ; - record fs in dx and continue
        cmp     al,65H                  ; if its gs:
        je      short is_gs             ; - record gs in dx and continue
        cmp     al,66H                  ; if its operand length
        je      short inc_esi           ; - continue with next byte
        cmp     al,67H                  ; if its address length
        jne     endlup                  ; - its not handled!!!!
inc_esi:inc     esi                     ; just bump past prefix
        jmp     short luup              ;
is_cs:  mov     cx,S_CS[ebp]
        mov     edx,ecx                 ;
        inc     esi                     ; bump past prefix
        jmp     short luup              ;
is_ds:  mov     cx,S_DS[ebp]
        mov     edx,ecx                 ;
        inc     esi                     ; bump past prefix
        jmp     short luup              ;
is_ss:  mov     cx,S_SS[ebp]            ;
        mov     edx,ecx                 ;
        inc     esi                     ; bump past prefix
        jmp     short luup              ;
is_es:  mov     cx,S_ES[ebp]            ;
        mov     edx,ecx                 ;
        inc     esi                     ; bump past prefix
        jmp     short luup              ;
is_fs:
        ifdef __WIN387__
          mov   cx,S_FS[ebp]
        else
          mov   cx,fs                   ;
        endif
        mov     edx,ecx                 ;
        inc     esi                     ; bump past prefix
        jmp     short luup              ;
is_gs:
        ifdef __WIN387__
          mov   cx,S_GS[ebp]
        else
          mov   cx,gs                   ;
        endif
        mov     edx,ecx                 ;
        inc     esi                     ; bump past prefix
        jmp     short luup              ;


calc_st0:
calc_sti:

stkins:                                 ; else

        ifdef __WIN387__
          sub   esi,WINBase             ; - set to addr in client
          mov   S_EIP[ebp],esi          ; - save it
        else
          mov   S_EIP[ebp],esi          ; - zap return address
        endif

        getsavearea

;       al has bottom 3 bits of opcode
;       ah has modrm bytes

        mov     dl,ah                   ; - get modrm byte           mmRRRrrr
        and     edx,038H                ; - isolate opcode portion   00RRR000
        or      dl,al                   ; - + other part of opcode   00RRRooo

      ifdef SEGMENTED
        mov     edi,dword ptr ds:[ebp].stktop;- di gets index of top of stack
      else
        mov     edi,dword ptr    [ebp].stktop;- di gets index of top of stack
      endif
;;    ifdef SEGMENTED
;;      mov     edi,dword ptr ds:[ebp].status;- di gets index of top of stack
;;    else
;;      mov     edi,dword ptr    [ebp].status;- di gets index of top of stack
;;    endif
;;      and     edi,SW_ST               ; - ...
;;      shr     edi,SW_ST_SHIFT         ; - ...
        mov     bl,ah                   ; - get the "i" from ST(I)   mmRRRrrr
        and     ebx,7                   ; - ...
;;      mov     esi,ebx                 ; - ...
;;      add     esi,edi                 ; - add to top of stack
        movzx   esi,TENTBL[edi].t_regnum; - get top of stack reg #
        add     esi,ebx                 ; - plus "i"
        and     esi,7                   ; - - modulo 7
;;      lea     edi,[edi+edi*4]         ; - multiply ST index by 10
;;      add     edi,edi                 ; - ...
        lea     esi,[esi+esi*4]         ; - multiply ST(i) index by 10
        add     esi,esi                 ; - ...
      ifdef SEGMENTED
        jmp    dword ptr cs:stkop[edx*4]; - execute the instruction
      else
        jmp    dword ptr    stkop[edx*4]; - execute the instruction
      endif

exit99:
;
; save updated stack top
;
        ifdef __WIN387__
          pop   EBP                     ; restore EBP
        endif
        ret                             ; return from emulation

p_single:                               ; round result to single precision
        lea     EBX,[edi+ebp].fpstack   ; point to stack top
        mov     EDX,0FFFFFF00h          ; get mask of bits to keep
        mov     EAX,4[EBX]              ; get top part
        shl     EAX,25                  ; get rounding bit
        _if     c                       ; if have to round
          _if   e                       ; - if half way between
            cmp   dword ptr [EBX],0     ; - - if bottom part is also 0
            _if   e                     ; - - then (half way)
              _shl  EDX,1               ; - - - adjust mask
            _endif                      ; - - endif
          _endif                        ; - endif
          add   dword ptr 4[EBX],0100h  ; - round up
          _if   c                       ; - if exponent needs adjusting
            mov   dword ptr 4[EBX],80000000h; - - set fraction
            inc   word ptr 8[EBX]       ; - - increment exponent
            ;  check for overflow
          _endif                        ; - endif
        _endif                          ; endif
        and     4[EBX],EDX              ; mask off bottom bits
        mov     dword ptr [EBX],0       ; zero the bottom part
        jmp     exit                    ; exit

p_double:                               ; round result to double precision
        lea     EBX,[edi+ebp].fpstack   ; point to stack top
        mov     EDX,0FFFFF800h          ; get mask of bits to keep
        mov     EAX,[EBX]               ; get bottom part
        shl     EAX,22                  ; get rounding bit
        _if     c                       ; if have to round
          _if   e                       ; - if half way between
            _shl  EDX,1                 ; - - adjust mask
          _endif                        ; - endif
          add   dword ptr [EBX],0800h   ; - round up
          adc   dword ptr 4[EBX],0      ; - ...
          _if   c                       ; - if exponent needs adjusting
            mov   dword ptr 4[EBX],80000000h; - - set fraction
            inc   word ptr 8[EBX]       ; - - increment exponent
            ;  check for overflow
          _endif                        ; - endif
        _endif                          ; endif
        and     [EBX],EDX               ; mask off bottom bits
        jmp     exit                    ; exit

saveflg_popstk_exit:
        saveflg
popstk_exit:
        popstk
        jmp     exit

saveflg_exit:
        saveflg
        jmp     exit


        xdefp   FPDivZero
FPDivZero proc  near
        push    eax
        mov     AL,SW_ZE                ; indicate zero divide
        mov     AH,FPE_ZERODIVIDE       ; ...
        call    __Exception_HNDLR       ; process exception
        pop     eax
        ret                             ; return
FPDivZero endp

        xdefp   FPInvalidOp
FPInvalidOp proc near
        push    eax
        mov     AL,SW_IE                ; indicate invalid operation
        mov     AH,FPE_INVALID          ; ...
        call    __Exception_HNDLR       ; process exception
        pop     eax
        ret                             ; return
FPInvalidOp endp

        xdefp   FPOverFlow
FPOverFlow proc near
        push    eax
        mov     AL,SW_OE                ; get OVERFLOW mask
        mov     AH,FPE_OVERFLOW         ; set OVERFLOW exception code
        call    __Exception_HNDLR       ; process exception
        pop     eax
        ret                             ; return
FPOverFlow endp

        xdefp   __Exception_HNDLR
;
;  input:
;       AL - exception mask
;       AH - exception code
;
__Exception_HNDLR proc  near
      ifdef SEGMENTED
        or      byte ptr ds:[ebp].status,AL   ; indicate exception occurred
        test    byte ptr ds:[ebp].control,AL  ; if exception unmasked
      else
        or      byte ptr    [ebp].status,AL   ; indicate exception occurred
        test    byte ptr    [ebp].control,AL  ; if exception unmasked
      endif
ifndef __WIN387__
        _if     e                       ; then
          push  ESI                     ; - save registers
          push  EDI                     ; - ...
          push  ECX                     ; - ...
          push  ES                      ; - ...
          push  DS                      ; - ...
          sub   ESP,NDP_SIZE            ; - allocate space for copy of 387
          mov   CX,SS                   ; - set ES=SS
          mov   ES,CX                   ; - ...
          mov   ESI,EBP                 ; - point to current copy
          mov   EDI,ESP                 ; - point to temporary save area
          mov   ECX,108/4               ; - number of dwords to copy
          cld                           ; - make sure forward direction
          rep   movsd                   ; - copy it
          movzx EAX,AH                  ; - get exception code  21-jul-90
if _OS eq _QNX
          call  __FPEhandler            ; - jump to SIGFPE handler rtn
else
          call  __FPE_exception_        ; - jump to SIGFPE handler rtn
endif
          mov   CX,DS                   ; - set ES=DS
          mov   ES,CX                   ; - ...
          mov   CX,SS                   ; - set DS=SS
          mov   DS,CX                   ; - ...
          mov   EDI,EBP                 ; - point to current copy
          mov   ESI,ESP                 ; - point to temporary save area
          mov   ECX,108/4               ; - number of dwords to copy
          cld                           ; - make sure forward direction
          rep   movsd                   ; - copy it
          add   ESP,NDP_SIZE            ; - deallocate space
          pop   DS                      ; - restore registers
          pop   ES                      ; - ...
          pop   ECX                     ; - ...
          pop   EDI                     ; - ...
          pop   ESI                     ; - ...
        _endif                          ; endif
endif
        ret                             ; return
__Exception_HNDLR endp

;===========================MOD/RM calculations=================================
; NB: 16 bit addressing modes are NYI

DOMODRM macro   disp,reg1
        IFIDN   <disp>,<D0>                     ; if D0
        xor     EBX,EBX                         ; - start ea at 0
        ELSE                                    ; else
        IFIDN   <disp>,<D8>                     ; - if D8
        movsx   EBX,byte ptr [ESI]              ; - - start it off at D8
        inc     ESI                             ; - - bump past displacement
        ELSE                                    ; - else
        .ERRDIF <disp>,<D32>                    ; - - it must be D32
        mov     EBX,dword ptr [ESI]             ; - - pick up D32
        add     ESI,4
        ENDIF                                   ; - endif
        ENDIF                                   ; endif
        IFNB    <reg1>                          ; if there's a register
        add     EBX,&reg1[ebp]                  ; - add the register in too
        IFIDN   <reg1>,<S_EBP>                  ; - if register is EBP
        mov     ecx,edx                         ; - - get segment for [EBP]
        ENDIF                                   ; - endif
        IFIDN   <reg1>,<S_ESP>                  ; - if register is ESP
        mov     ecx,edx                         ; - - get segment for [EBP]
        ENDIF                                   ; - endif
        ENDIF                                   ; endif
        ret                                     ; return to caller
        endm                                    ; nice macro eh?

m00000: DOMODRM D0,S_EAX                        ; mod r/m byte actions
m00001: DOMODRM D0,S_ECX
m00010: DOMODRM D0,S_EDX
m00011: DOMODRM D0,S_EBX
s00100: DOMODRM D0,S_ESP
m00101: DOMODRM D32
m00110: DOMODRM D0,S_ESI
m00111: DOMODRM D0,S_EDI
m01000: DOMODRM D8,S_EAX
m01001: DOMODRM D8,S_ECX
m01010: DOMODRM D8,S_EDX
m01011: DOMODRM D8,S_EBX
s01100: DOMODRM D8,S_ESP
m01101: DOMODRM D8,S_EBP
m01110: DOMODRM D8,S_ESI
m01111: DOMODRM D8,S_EDI
m10000: DOMODRM D32,S_EAX
m10001: DOMODRM D32,S_ECX
m10010: DOMODRM D32,S_EDX
m10011: DOMODRM D32,S_EBX
s10100: DOMODRM D32,S_ESP
m10101: DOMODRM D32,S_EBP
m10110: DOMODRM D32,S_ESI
m10111: DOMODRM D32,S_EDI

dosib:  mov     bl,[esi]                ; get sib byte in bl           ssiiibbb
        inc     esi                     ; bump over sib byte
        push    ebx                     ; save sib byte
        and     bl,7                    ; isolate base                 00000bbb
        mov     bh,ah                   ; get modrm in bh              mmRRRrrr
        shr     bh,3                    ; mod field in position        000mmRRR
        and     bh,18H                  ; isolate mod field            000mm000
        or      bl,bh                   ; glue it to get mod:base      000mmbbb
        mov     bh,0                    ; zero top byte
      ifdef SEGMENTED
        call    dword ptr cs:sib[ebx*4] ; calcuate base + displacement
      else
        call    dword ptr    sib[ebx*4] ; calcuate base + displacement
      endif
        pop     edx                     ; sib in edx again             ssiiibbb
        push    ecx                     ; save segment
        mov     ecx,edx                 ; get sib
        and     dl,038H                 ; isolate index                00iii000
        cmp     dl,020H                 ; if no index                  00100000
        _if     ne                      ; - then
          shr   edx,1                   ; - get 00iii000 / 2           000iii00
          neg   edx                     ; - negate for index into stack
          mov   edx,S_EAX[ebp+edx]      ; - fish the right register off stack
          shr   cl,6                    ; - get scale factor in cl     000000ss
          shl   edx,cl                  ; - scale the index
          add   ebx,edx                 ; - add it into effective address
        _endif                          ; endif
        pop     ecx                     ; restore segment
        ret                             ; return to caller


        xrefp   __FLDA
        xrefp   __FLDS
        xrefp   __FLDM
        xrefp   __FLDD
        xrefp   __FLDC
        xrefp   __I4LD
        xrefp   __I8LD
        xrefp   __LDI4
        xrefp   __LDI8
        xrefp   __EmuFSLD
        xrefp   __EmuLDFS
        xrefp   __EmuFDLD
        xrefp   __EmuLDFD
        xrefp   __sqrt
        xrefp   __fprem
        xrefp   __sin
        xrefp   __cos
        xrefp   __tan
        xrefp   __fpatan
        xrefp   __fyl2x
        xrefp   __f2xm1
        xrefp   __frndint                 ; 24-mar-90

ifndef NDEBUG
badop   proc    near
      if _OS eq _PLDT
        mov     edx,offset illopmsg
        mov     ah,9H
        int     21H
        ret
      else
        mov     eax,offset illopmsg
        jmp     puts_
      endif
badop   endp
endif

illop   macro
ifndef NDEBUG
        call    badop
endif
        endm

ifndef NDEBUG
prtnyi  proc    near
      if _OS eq _PLDT
        mov     ah,9H
        int     21H
        mov     edx,offset nyimsg
        mov     ah,9H
        int     21H
        ret
      else
        mov     eax,edx
        call    puts_
        mov     eax,offset nyimsg
        jmp     puts_
      endif
prtnyi  endp

nyi     macro   string
        local   msg
        startdata
msg     db      '&string',NC
        enddata
        mov     edx,offset msg
        call    prtnyi
        endm
else
nyi     macro   string
        endm
endif

_lea    macro   reg,what
        IFIDN   <what>,<st>
        lea     reg,[edi+ebp].fpstack
        ELSE
        IFIDN   <what>,<st(i)>
        lea     reg,[esi+ebp].fpstack
        ELSE
        .ERRDIF <what>,<temp>
        lea     reg,[ebp].fptemp
        ENDIF
        ENDIF
        endm

_add    macro   a,b,c
        _lea    EAX,a
        _lea    EDX,b
        _lea    EBX,c
        call    __FLDA
        endm

_sub    macro   a,b,c
        _lea    EAX,a
        _lea    EDX,b
        _lea    EBX,c
        call    __FLDS
        endm

_mul    macro   a,b,c
        _lea    EAX,a
        _lea    EDX,b
        _lea    EBX,c
        call    __FLDM
        endm

_div    macro   a,b,c
        _lea    EAX,a
        _lea    EDX,b
        _lea    EBX,c
        call    __FLDD
        endm

_neg    macro   a
        _lea    EAX,a
        xor     byte ptr 9[EAX],80h
        endm

_abs    macro   a
        _lea    EAX,st
        and     byte ptr 9[EAX],7Fh     ; turn off sign bit
        endm

_cmp    macro   a,b
        _lea    EAX,a
        _lea    EDX,b
        call    __FLDC
        endm

_xchg   macro   a,b
        _lea    EBX,a
        _lea    ESI,b
        mov     eax,[esi]
        xchg    eax,[ebx]
        mov     [esi],eax
        mov     eax,4[esi]
        xchg    eax,4[ebx]
        mov     4[esi],eax
        mov     ax,8[esi]
        xchg    ax,8[ebx]
        mov     8[esi],ax
        endm

st_top  macro rtn
        _lea    EAX,st
        call    __&rtn
        endm

st_top2 macro rtn
        _lea    EAX,st
        popstk
        _lea    EDX,st
        call    __&rtn
        endm

real10x  struc
        bytes1to4       dd      0
        bytes5to8       dd      0
        bytes9n10       dw      0
real10x  ends

ifdef SEGMENTED
mov10   macro   src_seg,src_off,dst_seg,dst_off
        mov     eax,src_seg:src_off.bytes1to4
        mov     dst_seg:dst_off.bytes1to4,eax
        mov     eax,src_seg:src_off.bytes5to8
        mov     dst_seg:dst_off.bytes5to8,eax
        mov     ax,src_seg:src_off.bytes9n10
        mov     dst_seg:dst_off.bytes9n10,ax
        endm
else
mov10   macro   src_seg,src_off,dst_seg,dst_off
        mov     eax,        src_off.bytes1to4
        mov             dst_off.bytes1to4,eax
        mov     eax,        src_off.bytes5to8
        mov             dst_off.bytes5to8,eax
        mov     ax,        src_off.bytes9n10
        mov             dst_off.bytes9n10,ax
        endm
endif
putword macro   what,where
      ifdef SEGMENTED
        mov     ds:[ebp].&where,what
      else
        mov        [ebp].&where,what
      endif
        endm

getword macro   what,where
      ifdef SEGMENTED
        mov     what,ds:[ebp].&where
      else
        mov     what,   [ebp].&where
      endif
        endm

loadword macro  what
      ifdef SEGMENTED
        mov     what,es:[esi]
      else
        mov     what,   [esi]
      endif
        endm

saveword macro  what
      ifdef SEGMENTED
        mov     es:[esi],what
      else
        mov        [esi],what
      endif
        endm

save_st macro
        _lea    ebx,st
        push    [ebx]
        push    4[ebx]
        push    6[ebx]
        endm

restore_st macro
        _lea    ebx,st
        pop     6[ebx]
        pop     4[ebx]
        pop     [ebx]
        endm

rndint  macro
        _lea    eax,st
        getword dx,control
        and     dx,CW_RC
        call    __frndint
        endm

ldcons  macro   cons
        mov10   cs,cons,ds,[ebp].fptemp
        endm

stcons  macro   w1,w2,w3,w4,w5
        pushstk                                 ; push stack pointer
        _lea    esi,st
    if ((w4 shl 16) + w5) eq 0
        sub     eax,eax
        mov     [esi],eax
    else
        mov     dword ptr [esi],(w4 shl 16) + w5
    endif
    if ((w2 shl 16) + w3) eq 0
        mov     4[esi],eax
    else
        mov     dword ptr 4[esi],(w2 shl 16) + w3
    endif
    if w1 eq 0
        mov     8[esi],ax
    else
        mov     word ptr 8[esi],w1
    endif
        endm

setp_extended   macro
        lea     edx,p_extended
ifdef __WIN387__
        mov     precrtn, edx
else
        putword edx,precrtn
endif
        endm

setp_double     macro
        lea     edx,p_double
ifdef __WIN387__
        mov     precrtn, edx
else
        putword edx,precrtn
endif
        endm

setp_single     macro
        lea     edx,p_single
ifdef __WIN387__
        mov     precrtn,edx
else
        putword edx,precrtn
endif
        endm

_fclex  macro
        ifdef SEGMENTED
          and   word ptr ds:[ebp].status, 07f00H
        else
          and   word ptr [ebp].status, 07f00H
        endif
        endm

_finit  macro
        putword CW_INIT,control
        putword SW_INIT,status
        putword TAG_INIT,tags
        setp_extended
        sub     edi,edi                 ; set top of stack pointer to 0
        putword edi,stktop
        endm

save87  macro size                      ; put the 87 into user memory
        mov     ecx,ENV_SIZE / 4        ; set up size reg
        mov     ebx,edi                 ; save top of stack pointer
        mov     edi,esi                 ; destination is user memory
        mov     esi,ebp                 ; source is the '8087'
        rep     movsd                   ; stash it in users memory
IFDIF   <size>,<ENV_SIZE>
        mov     ecx,8                   ; do 8 times
fslup:  lea     esi,[ebx+ebp].fpstack   ; - point source to stack top
        movsd                           ; - save the value
        movsd                           ; - ...
        movsw                           ; - ...
        movzx   ebx,TENTBL[ebx].t_next  ; - 'pop' the stack
        loop    fslup                   ; end do
ENDIF
        mov     edi,ebx
        endm

rest87  macro size                      ; put user memory into the 87
        mov     ecx,ENV_SIZE / 4        ; set up size reg
        push    ds                      ; save registers
        push    es                      ; and xchg ds,es
        mov     ax,es                   ; ...
        mov     dx,ds                   ; ...
        mov     es,dx                   ; ...
        mov     ds,ax                   ; ...
        mov     edi,ebp                 ; ...
        rep     movsd                   ; put into 8087
IFDIF   <size>,<ENV_SIZE>
      ifdef SEGMENTED
        mov     ebx,dword ptr es:[ebp].status;- ebx gets index of top of stack
      else
        mov     ebx,dword ptr    [ebp].status;- ebx gets index of top of stack
      endif
        and     ebx,SW_ST               ; - ...
        shr     ebx,SW_ST_SHIFT         ; - ...
        lea     ebx,[ebx+ebx*4]         ; - ... times 5
        add     ebx,ebx                 ; - ... ... times 2
      ifdef SEGMENTED
        mov     dword ptr es:[ebp].stktop,ebx;- save index of top of stack
      else
        mov     dword ptr    [ebp].stktop,ebx;- save index of top of stack
      endif
        mov     ecx,8                   ; do 8 times
frlup:  lea     edi,[ebp+ebx].fpstack   ; - point to st(0)
        movsd                           ; - restore st(0)
        movsd                           ; - ...
        movsw                           ; - ...
        movzx   ebx,TENTBL[ebx].t_next  ;- 'pop' the stack
        loop    frlup                   ; end do
ENDIF
        pop     es                      ; ...
        pop     ds                      ; ...
        endm

load    macro  where,what                       ; load up hi:lo
        IFIDN           <what>,<double>         ; - then
      ifdef SEGMENTED
        mov     eax,es:[esi]                    ; - load up double
        mov     edx,es:4[esi]                   ; - ...
      else
        mov     eax,   [esi]                    ; - load up double
        mov     edx,   4[esi]                   ; - ...
      endif
        _lea    ebx,where                       ; - ...
        call    __EmuFDLD                       ; - convert to tbyte
        ELSE                                    ; else if long
        IFIDN      <what>,<long>                ; - then
      ifdef SEGMENTED
        mov     eax,es:[esi]                    ; - load long integer
      else
        mov     eax,   [esi]                    ; - load long integer
      endif
        _lea    edx,where                       ; - ...
        call    __I4LD                          ; - convert to tbyte
        ELSE                                    ; else if int
        IFIDN      <what>,<integer>             ; - then
      ifdef SEGMENTED
        mov     ax,es:[esi]                     ; - load an int
      else
        mov     ax,   [esi]                     ; - load an int
      endif
        cwde                                    ; - convert to long
        _lea    edx,where                       ; - ...
        call    __I4LD                          ; - convert to tbyte
        ELSE                                    ; else if double long (8 bytes)
        IFIDN      <what>,<dlong>               ; - then
      ifdef SEGMENTED
        mov     eax,es:[esi]                    ; - load up long long
        mov     edx,es:4[esi]                   ; - ...
      else
        mov     eax,   [esi]                    ; - load up long long
        mov     edx,   4[esi]                   ; - ...
      endif
        _lea    ebx,where                       ; - ...
        call    __I8LD                          ; - convert to tbyte
        ELSE                                    ; else if float
        IFIDN      <what>,<float>               ; - then
      ifdef SEGMENTED
        mov     eax,es:[esi]                    ; - load a float
      else
        mov     eax,   [esi]                    ; - load a float
      endif
        _lea    edx,where                       ; - ...
        call    __EmuFSLD                       ; - convert to double
        ELSE                                    ; else if st(i)
        IFIDN      <what>,<st(i)>               ; - then
        IFIDN      <where>,<temp>               ; - if going to temp
        mov10   ds,[esi+ebp].fpstack,ds,[ebp].fptemp
        ELSE                                    ; - else going to st
        mov10   ds,[esi+ebp].fpstack,ds,[edi+ebp].fpstack
        ENDIF                                   ; - endif
        ELSE                                    ; else if st
        IFIDN      <what>,<st>                  ; - then
        .ERRDIF   <where>,<temp>                ; - if going to temp
        mov10   ds,[edi+ebp].fpstack,ds,[ebp].fptemp
        ELSE                                    ; else if treal
        IFIDN      <what>,<treal>               ; - then
        IFIDN     <where>,<st>                  ; - if load to st
        mov10   es,[esi],ds,[edi+ebp].fpstack   ; - - move 10 bytes
        ELSE                                    ; - else load to temp
        .ERRDIF   <where>,<temp>                ; - - ...
        mov10   es,[esi],ds,[ebp].fptemp        ; - - move 10 bytes
        ENDIF                                   ; - endif
        ELSE                                    ; else if temp
        IFIDN     <what>,<temp>                 ; - then
        .ERRDIF   <where>,<st>                  ; - loading into st
        mov10   ds,[ebp].fptemp,ds,[ebp+edi].fpstack; - move 10 bytes
        ELSE                                    ; else
        .ERR                                    ; - I can't use my own macro's
        %OUT bad load macro                     ; - ...
        ENDIF                                   ; endif
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ... oh for an elseif
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        endm

store   macro   what                            ; store st somewhere
        IFIDN   <what>,<double>                 ; - then it
        _lea    eax,st                          ; - point to top of stack
        call    __EmuLDFD                       ; - convert to double
      ifdef SEGMENTED
        mov     es:[esi],eax                    ; - store it
        mov     es:4[esi],edx                   ; - ...
      else
        mov        [esi],eax                    ; - store it
        mov        4[esi],edx                   ; - ...
      endif
        ELSE                                    ; - ...
        IFIDN   <what>,<long>                   ; else if long
        _lea    eax,st                          ; - point to top of stack
        call    __LDI4                          ; - convert to long
      ifdef SEGMENTED
        mov     es:[esi],eax                    ; - store it
      else
        mov        [esi],eax                    ; - store it
      endif
        ELSE                                    ; else if float
        IFIDN   <what>,<float>                  ; - then
        _lea    eax,st                          ; - point to top of stack
        call    __EmuLDFS                       ; - convert to float
      ifdef SEGMENTED
        mov     es:[esi],eax                    ; - store it
      else
        mov        [esi],eax                    ; - store it
      endif
        ELSE                                    ; else if integer
        IFIDN   <what>,<integer>                ; - then
        _lea    eax,st                          ; - point to top of stack
        call    __LDI4                          ; - convert to long
        mov     edx,eax                         ; - save value
        cwde                                    ; - sign extend ax to eax
        cmp     eax,edx                         ; - if not equal
        _if     ne                              ; - then
          mov   ax,8000h                        ; - - indicate overflow
        _endif                                  ; - endif
      ifdef SEGMENTED
        mov     es:[esi],ax                     ; - store it
      else
        mov        [esi],ax                     ; - store it
      endif
        ELSE                                    ; else if double long (8 bytes)
        IFIDN   <what>,<dlong>                  ; - then
        _lea    eax,st                          ; - point to top of stack
        call    __LDI8                          ; - convert to long long
      ifdef SEGMENTED
        mov     es:[esi],eax                    ; - store it
        mov     es:4[esi],edx                   ; - ...
      else
        mov        [esi],eax                    ; - store it
        mov        4[esi],edx                   ; - ...
      endif
        ELSE                                    ; else if st(i)
        IFIDN   <what>,<st(i)>                  ; - then
        mov10   ds,[edi+ebp].fpstack,ds,[esi+ebp].fpstack
        ELSE                                    ; else if st(i)
        IFIDN   <what>,<treal>                  ; - then
        mov10   ds,[edi+ebp].fpstack,es,[esi]   ; - move 10 bytes
        ELSE                                    ; else
        .ERR                                    ; - I cant use my own macro
        %OUT bad store macro                    ; - ...
        ENDIF                                   ; endif
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;; memory instruction emulation ;;;;;;;;;;;;;;;;;;;;;
;;
;; AT THIS POINT
;; ES:ESI                  - pointer to the memory operand
;; DS:[EDI+EBP].fpstack    - pointer to the top of stack
;; ON EXIT
;; EDI                     - will be updated to reflect top of stack pointer
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include memops.inc

;;;;;;;;;;;;;;;;;;;;;;;;;;; stack instruction emulation ;;;;;;;;;;;;;;;;;;;;;;;;
;; AT THIS POINT
;; DS:[EDI+EBP].fpstack - pointer to ST(0)
;; DS:[ESI+EBP].fpstack - pointer to ST(i)
;; EBX is the bottom 3 bits of modrm byte
;; ON EXIT
;; EDI          - will have been updated to reflect top of stack pointer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include stkops.inc

; here are some 386 specific stack ops

s613:;  fpatan
;===================================================================
        _lea    edx,st          ; X
        popstk
        _lea    eax,st          ; Y
        mov     ebx,eax         ; destination
        call    __fpatan
        donearith

s614:;  fxtract
;===================================================================
        _lea    esi,st
        pushstk                                 ; push stack pointer
        _lea    ebx,st
        mov     eax,[esi]
        mov     [ebx],eax
        mov     eax,4[esi]
        mov     4[ebx],eax
        mov     ax,8[esi]
        mov     dx,ax
        and     dx,8000h
        or      dx,3fffh
        mov     8[ebx],dx
        and     ax,7fffh                        ; remove sign
        sub     ax,3fffh                        ; remove bias
        cwde                                    ; sign extend
        mov     edx,esi                         ; st(1)
        call    __I4LD                          ; load exponent
        done

s615:;  fprem1
s710:;  fprem
;===================================================================
        _lea    EAX,st                  ; point to st(0)
        movzx   ESI,TENTBL[EDI].t_next  ; get index for st(1)
        _lea    EDX,st(i)               ; point to st(1)
        call    __fprem                 ; do remainder
        savecc                          ; save condition codes
        donearith

s714:;  frndint
;===================================================================
        rndint                  ; round top of stack
        done

s715:;  fscale
;===================================================================
        mov     esi,edi         ; save top of stack index
;;;        incstp                       ; point to st(1)
        movzx   edi,TENTBL[edi].t_next  ; inc stack pointer
        _lea    EAX,st          ; point to scale factor
        mov     CX,8[EAX]       ; save exponent and sign
        call    __LDI4          ; convert scale factor to integer
        mov     edi,esi         ; restore pointer to st(0)
        _lea    EBX,st          ; point to number to be scaled
;
;       check to see if scale factor is too large;  29-mar-91
;
        mov     EDX,EAX         ; get integer scale factor
        cwde                    ; sign extend low order 16 bits
        cmp     EAX,EDX         ; if exponent doesn't fit in 16 bits
        _if     ne              ; then
          or    CH,CH           ; - if negative scale factor
          _if   s               ; - then
            sub   DX,DX         ; - - underflow
            sub   CX,CX         ; - - clear the sign
          _else                 ; - else
            mov   DX,7FFFh      ; - - overflow
            mov   CX,8[EBX]     ; - - get exponent and sign
            and   CX,8000h      ; - - isolate sign
          _endif                ; - endif
        _else                   ; else
          mov   DX,8[EBX]       ; - get current exponent
          mov   CX,DX           ; - get sign
          and   CX,8000h        ; - ...
          and   DX,7FFFh        ; - isolate exponent
          add   DX,AX           ; - calc. new exponent
          _if     s             ; - if overflow/underflow
            or    AH,AH         ; - - if negative
            _if   s             ; - - then
              sub   DX,DX       ; - - - underflow
              sub   CX,CX       ; - - - clear the sign
            _else               ; - - else
              mov   DX,7FFFh    ; - - - overflow
            _endif              ; - - endif
          _endif                ; - endif
        _endif                  ; endif
        or      DX,CX           ; put in the sign
        mov     8[EBX],DX       ; store new exponent
        done

s470:;  fstsw   ax
;===================================================================
        getword ax,status       ; get status word
ifdef __WIN387__
        mov     ebp,[esp]       ; get access to saved registers
        mov     word ptr S_EAX[ebp],ax   ; update user's AX
else
        mov     ebp,esp         ; get access to saved registers
        mov     S_EAX[ebp],ax   ; update user's AX
endif
        done

ifdef __WIN387__
__Win387_emulator endp
else
__int7  endp
endif
