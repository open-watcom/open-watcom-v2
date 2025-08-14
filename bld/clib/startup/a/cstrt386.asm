;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;* Copyright (c) 2002-2025 The Open Watcom Contributors. All Rights Reserved.
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
;* Description:  DOS 32-bit startup code.
;*
;*****************************************************************************


; Note: This module must contain the string 'WATCOM' (all caps) for DOS/4GW
;       to recognize a 'Watcom' executable.

;       This must be assembled using one of the following commands:
;               wasm cstrt386 -bt=DOS -ms -3r
;               wasm cstrt386 -bt=DOS -ms -3s
;
;   NOTE: All C library data should be defined in crwdata.asm -- That way
;         it's also available to ADS applications (who use adstart.asm).
;

        name    cstart

.387
.386p

include langenv.inc
include tinit.inc
include xinit.inc
include extender.inc

FLG_NO87        equ     1
FLG_LFN         equ     1

SEL_PHARLAP_PSP equ     24h
SEL_PHARLAP_ENV equ     2ch

PSP_ENV_SEG     equ     2ch


        assume  nothing

        extrn   __CMain                 : near
        extrn   __InitRtns              : near
        extrn   __FiniRtns              : near
        extrn   __DOSseg__              : near

        extrn   _edata                  : byte  ; end of DATA (start of BSS)
        extrn   _end                    : byte  ; end of BSS (start of STACK)

        extrn   "C",_dynend             : dword
        extrn   "C",_curbrk             : dword
        extrn   "C",_psp                : word
        extrn   "C",_osmajor            : byte
        extrn   "C",_osminor            : byte
        extrn   "C",_STACKLOW           : dword
        extrn   "C",_STACKTOP           : dword
        extrn   __child                 : dword
        extrn   __no87                  : byte
        extrn   "C",__uselfn            : byte
        extrn   "C",_Extender           : byte
        extrn   "C",_ExtenderSubtype    : byte
        extrn   "C",_Envptr             : fword
        extrn   "C",__FPE_handler       : dword
        extrn   "C",_LpCmdLine          : dword
        extrn   "C",_LpPgmName          : dword

DGROUP group _NULL,_AFTERNULL,CONST,_DATA,DATA,TIB,TI,TIE,XIB,XI,XIE,YIB,YI,YIE,_BSS,STACK

; this guarantees that no function pointer will equal NULL
; (WLINK will keep segment 'BEGTEXT' in front)
; This segment must be at least 4 bytes in size to avoid confusing the
; signal function.

BEGTEXT  segment use32 para public 'CODE'
        assume  cs:BEGTEXT
forever label   near
        int     3h              ;0
        jmp short forever       ;1
        public ___begtext
___begtext label byte
        nop                     ;3
        nop                     ;4
        nop                     ;5
        nop                     ;6
        nop                     ;7
        nop                     ;8
        nop                     ;9
        nop                     ;A
        nop                     ;B
        nop                     ;C
        nop                     ;D
        nop                     ;E
        nop                     ;F
        assume  cs:nothing
BEGTEXT  ends

_TEXT   segment use32 dword public 'CODE'

        assume  ds:DGROUP

_NULL   segment para public 'BEGDATA'
__nullarea label word
        db      1,1,1,0
        public  __nullarea
_NULL   ends

_AFTERNULL segment word public 'BEGDATA'
_AFTERNULL ends

CONST   segment word public 'DATA'
CONST   ends

_DATA   segment dword public 'DATA'

__D16Infoseg   dw       0020h   ; DOS/4G kernel segment
__x386_zero_base_selector dw 0  ; base 0 selector for X-32VM

        public  __D16Infoseg
        public  __x386_zero_base_selector
_DATA   ends


DATA    segment word public 'DATA'
DATA    ends

_BSS          segment word public 'BSS'
_BSS          ends

STACK_SIZE      equ     10000h

STACK   segment para stack 'STACK'
        db      (STACK_SIZE) dup(?)
STACK   ends


        assume  nothing
        public  _cstart_

        assume  cs:_TEXT

_cstart_ proc near
        jmp short around

;
; signature for DOS4GW to identify OW run-time
;
        db      "WATCOM",0

;
; miscellaneous code-segment messages
;
ConsoleName     db      "con",0
NewLine         db      0Dh,0Ah

        align   4
        dd      ___begtext              ; make sure dead code elimination
                                        ; doesn't kill BEGTEXT

around: sti                             ; enable interrupts

        assume  ds:DGROUP

        and     esp,0fffffffch          ; make sure stack is on a 4 byte bdry
        mov     ebx,esp                 ; get sp
        mov     _STACKTOP,ebx           ; set stack top
        mov     _curbrk,ebx             ; set first available memory location
;
;       get DOS & Extender version number
;
        ;mov    ebx,'PHAR'              ; set ebx to "PHAR"
        mov     ebx,50484152h           ; set ebx to "PHAR"
        sub     eax,eax                 ; set eax to 0
        mov     ah,30h
        int     21h                     ; modifies eax,ebx,ecx,edx
        mov     _osmajor,al
        mov     _osminor,ah
        mov     ecx,eax                 ; remember DOS version number
        sub     esi,esi                 ; offset 0 for environment strings
        mov     edi,81H                 ; DOS command buffer es:edi
        shr     eax,16                  ; get top 16 bits of eax
        cmp     ax,'DX'                 ; if top 16 bits = "DX"
        jne     not_pharlap             ; then its pharlap
        mov     ah,XS_PHARLAP_NORMAL    ; - extender subtype
        cmp     ebx,04A613231h          ; - if ebx is '12aJ'
        jne short normal_pharlap        ; - then Japanese version
        mov     ah,XS_PHARLAP_FUJITSU   ; - - setup Japanese extender subtype
normal_pharlap:                         ; - endif
        sub     bl,'1' - X_PHARLAP_V1   ; - ASCII -> bin version
        mov     al,bl                   ; - save major version number
        push    eax                     ; - save version number
        mov     eax,SEL_PHARLAP_PSP     ; get segment address of PSP
        mov     _psp,ax                 ; save segment address of PSP
        mov     es,eax                  ; - point to PSP
        mov     ebx,es:[5Ch]            ; - get highest addr used
        add     ebx,000000FFFh          ; - round up to 4K boundary
        and     ebx,0FFFFF000h          ; - ...
        mov     _curbrk,ebx             ; - set first available memory locn
        shr     ebx,12                  ; - calc. # of 4k pages
        mov     eax,ds                  ; - set ES=data segment
        mov     es,eax                  ; - ...
        mov     ah,4Ah                  ; - shrink block to minimum amount
        int     21h                     ; - ...
        pop     eax                     ; - restore version number
        mov     ebx,ds                  ; - get value of Phar Lap data segment
        mov     cx,SEL_PHARLAP_ENV      ; - PharLap environment segment
        jmp short know_extender         ; else

not_pharlap:                            ; - assume DOS/4G or compatible
        mov     dx,78h                  ; - see if Rational DOS/4G
        mov     ax,0FF00h               ; - ...
        int     21h                     ; - ...
        test    al,al                   ; - ...
        je      short not_rational      ; - quit if not Rational DOS/4G
        mov     eax,gs                  ; - get segment address of kernel
        test    ax,ax                   ; - if not zero
        je      short rat9              ; - then
        mov     __D16Infoseg,ax         ; - - remember it
rat9:                                   ; - endif
        mov     ax,6                    ; - check data segment base
        mov     ebx,ds                  ; - set up data segment
        int     31h                     ; - DPMI call
        mov     al,X_RATIONAL           ; - asssume Rational 32-bit Extender
        mov     ah,XS_RATIONAL_ZEROBASE ; - asssume Rational 32-bit Extender subtype
        or      dx,cx                   ; - if base is non-zero
        jz      rat10                   ; - then
        mov     ah,XS_RATIONAL_NONZEROBASE; - DOS/4G non-zero based data
rat10:                                  ; - endif
        jmp short save_psp              ; else

not_rational:                           ; - check other extenders

unknow_extender:                        ; - unknown extender
        xor     eax,eax
save_psp:
        mov     _psp,es                 ; - save segment address of PSP
        mov     cx,es:[PSP_ENV_SEG]     ; - get environment segment into cx
know_extender:                          ; endif
        mov     _Extender,al            ; record extender type
        mov     _ExtenderSubtype,ah     ; record extender subtype
        mov     es,ebx                  ; get access to code segment
        mov     es:__saved_DS,ds        ; save DS value
        mov     dword ptr _Envptr,esi   ; save address of environment strings
        mov     word ptr _Envptr+4,cx   ; save segment of environment area
        push    esi                     ; save address of environment strings
;
;       copy command line into bottom of stack
;
        mov     es,_psp                 ; point to PSP
        mov     edx,offset DGROUP:_end
        add     edx,0FH
        and     dl,0F0H
        sub     ecx,ecx
        mov     cl,es:[edi-1]           ; get length of command
        cld                             ; set direction forward
        mov     al,' '
        repe    scasb
        lea     esi,-1[edi]
        mov     edi,edx
        mov     ebx,es
        mov     edx,ds
        mov     ds,ebx
        mov     es,edx                  ; es:edi is destination
        je      noparm
        inc     ecx
        rep     movsb
noparm: sub     al,al
        stosb                           ; store NULLCHAR
        stosb                           ; assume no pgm name
        pop     esi                     ; restore address of environment strings
        dec     edi                     ; back up pointer 1
        push    edi                     ; save pointer to pgm name
        push    edx                     ; save ds(stored in dx)
        mov     ds,es:word ptr _Envptr+4 ; get segment addr of environment area
        mov     bx,FLG_LFN*256          ; assume 'lfn=n' env. var. not present / assume 'no87=' env. var. not present
L1:     mov     eax,[esi]               ; get first 4 characters
        or      eax,20202020h           ; map to lower case
        cmp     eax,37386f6eh           ; check for 'no87'
        jne     short L2                ; skip to next test if not 'no87'
        cmp     byte ptr 4[esi],'='     ; make sure next char is '='
        jne     short L4                ; no
        or      bl,FLG_NO87             ; - indicate 'no87' was present
        jmp     L4
L2:     cmp     eax,3d6e666ch           ; check for 'lfn='
        jne     short L4                ; skip if not 'lfn='
        mov     al,byte ptr 4[esi]      ; get next character
        or      al,20h                  ; map to lower case
        cmp     al,'n'                  ; make sure next char is 'n'
        jne     short L4                ; no
        and     bh,not FLG_LFN          ; indicate no 'lfn=n' present
L4:     cmp     byte ptr [esi],0        ; end of string ?
        lodsb
        jne     L4                      ; until end of string
        cmp     byte ptr [esi],0        ; end of all strings ?
        jne     L1                      ; if not, then skip next string
        lodsb
        inc     esi                     ; point to program name
        inc     esi                     ; . . .
;
;       copy the program name into bottom of stack
;
L5:     cmp     byte ptr [esi],0        ; end of pgm name ?
        movsb                           ; copy a byte
        jne     L5                      ; until end of pgm name
        pop     ds                      ; restore ds
        pop     esi                     ; restore address of pgm name

        assume  ds:DGROUP
        mov     __no87,bl               ; set state of "NO87" enironment var
        and     __uselfn,bh             ; set "LFN" support status
        mov     _STACKLOW,edi           ; save low address of stack
        mov     ebx,esp                 ; end of stack in data segment
        mov     _dynend,ebx             ; set top of dynamic memory area

        mov     ecx,offset DGROUP:_end  ; end of _BSS segment (start of STACK)
        mov     edi,offset DGROUP:_edata; start of _BSS segment
        sub     ecx,edi                 ; calc # of bytes in _BSS segment
        cmp     byte ptr _Extender,X_RATIONAL   ; if not Rational DOS extender
        jne     short zerobss           ; then zero whole BSS
        cmp     ecx,1000h               ; if size of BSS <= 4K
        jbe     short zerobss           ; then just zero it
        mov     ecx,1000h               ; only zero first 4K under Rational
                                        ; DOS extender will zero rest of pages
zerobss:mov     dl,cl                   ; save bottom 2 bits of count in edx
        shr     ecx,2                   ; calc # of dwords
        sub     eax,eax                 ; zero the _BSS segment
        rep     stosd                   ; ...
        mov     cl,dl                   ; get bottom 2 bits of count
        and     cl,3                    ; ...
        rep     stosb                   ; ...

        mov     eax,offset DGROUP:_end  ; cmd buffer pointed at by EAX
        add     eax,0FH
        and     al,0F0H
        mov     _LpCmdLine,eax          ; save command line address
        mov     _LpPgmName,esi          ; save program name address
        sub     ebp,ebp                 ; ebp=0 indicate end of ebp chain
        mov     eax,0FFH                ; run all initalizers
        call    __InitRtns              ; call initializer routines
        call    __CMain
_cstart_ endp

;       don't touch AL in __exit, it has the return code

__exit  proc near                       ; never return
        public  "C",__exit
ifndef __STACK__
        push    eax                     ; save return code on the stack
endif
        jmp short L7

        public  __do_exit_with_msg_

; input: ( char *msg, int rc ) always in registers
;       EAX - pointer to message to print
;       EDX - exit code

__do_exit_with_msg_:                    ; never return
        push    edx                     ; save return code
        push    eax                     ; save address of msg
        mov     edx,offset ConsoleName
        mov     ax,03d01h               ; write-only access to screen
        int     021h
        mov     bx,ax                   ; get file handle
        pop     edx                     ; restore address of msg
        mov     esi,edx                 ; get address of msg
        cld                             ; make sure direction forward
L6:     lodsb                           ; get char
        test    al,al                   ; end of string?
        jne     L6                      ; no
        mov     ecx,esi                 ; calc length of string
        sub     ecx,edx                 ; . . .
        dec     ecx                     ; . . .
        mov     ah,040h                 ; write out the string
        int     021h                    ; . . .
        mov     edx,offset NewLine      ; write out the string
        mov     ecx,sizeof NewLine      ; . . .
        mov     ah,040h                 ; . . .
        int     021h                    ; . . .
L7:
        xor     eax,eax                 ; run finalizers
        mov     edx,FINI_PRIORITY_EXIT-1; less than exit
        call    __FiniRtns              ; call finializer routines
        pop     eax                     ; restore return code from stack
        mov     ah,04cH                 ; DOS call to exit with return code
        int     021h                    ; back to DOS
__exit  endp

;
; copyright message (special - see comment at top)
;
include msgcpyrt.inc

        align   4

        public  __GETDS
__GETDS proc    near
public "C",__GETDSStart_
__GETDSStart_ label byte
        mov     ds,cs:__saved_DS        ; load saved DS value
        ret                             ; return
__saved_DS  dw  0                       ; DS save area for interrupt routines
public "C",__GETDSEnd_
__GETDSEnd_ label byte
__GETDS endp

_TEXT   ends

        end     _cstart_
