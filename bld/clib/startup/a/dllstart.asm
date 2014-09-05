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
;* Description:  CauseWay DLL startup code.
;*
;*****************************************************************************

        name    dllstart

.386

include xinit.inc

FLG_NO87    equ 1
FLG_LFN     equ 100h

        assume  nothing

        extrn   __LibMain               : near
        extrn   "C",__CommonTerm        : near

        extrn   _edata                  : byte  ; end of DATA (start of BSS)
        extrn   _end                    : byte  ; end of BSS (start of STACK)

        extrn   "C",_dynend             : dword
        extrn   "C",_curbrk             : dword
        extrn   "C",_psp                : word
        extrn   "C",_osmajor            : byte
        extrn   "C",_osminor            : byte
        extrn   "C",_STACKLOW           : dword
        extrn   "C",_STACKTOP           : dword
        extrn   __no87                  : byte
        extrn   "C",__uselfn            : byte
        extrn   "C",_Extender           : byte
        extrn   "C",_ExtenderSubtype    : byte
        extrn   "C",_Envptr             : fword
        extrn   "C",_LpCmdLine          : dword
        extrn   "C",_LpPgmName          : dword

DGROUP group _NULL,_AFTERNULL,CONST,_DATA,DATA,TIB,TI,TIE,XIB,XI,XIE,YIB,YI,YIE,_BSS,PRIVATE_STACK,STACK

DPMIGetSegmentBaseAddress   equ     6

X_RATIONAL                  equ     1
XS_RATIONAL_ZEROBASE        equ     0
XS_RATIONAL_NONZEROBASE     equ     1

        .dosseg

; this guarantees that no function pointer will equal NULL
; (WLINK will keep segment 'BEGTEXT' in front)
; This segment must be at least 4 bytes in size to avoid confusing the
; signal function.

BEGTEXT  segment use32 para public 'CODE'
        assume  cs:BEGTEXT
forever label   near
        int     3h
        jmp     short forever
___begtext label byte
        nop     ;3
        nop     ;4
        nop     ;5
        nop     ;6
        nop     ;7
        nop     ;8
        nop     ;9
        nop     ;A
        nop     ;B
        nop     ;C
        nop     ;D
        nop     ;E
        nop     ;F
        public ___begtext
        assume  cs:nothing
BEGTEXT  ends

_TEXT   segment use32 dword public 'CODE'

        assume  ds:DGROUP

_NULL   segment para public 'BEGDATA'
__nullarea label word
        db      01h,01h,01h,00h
        public  __nullarea
_NULL   ends

_AFTERNULL segment word public 'BEGDATA'
_AFTERNULL ends

CONST   segment word public 'DATA'
CONST   ends

TIB     segment byte public 'DATA'
TIB     ends
TI      segment byte public 'DATA'
TI      ends
TIE     segment byte public 'DATA'
TIE     ends

XIB     segment word public 'DATA'
XIB     ends
XI      segment word public 'DATA'
XI      ends
XIE     segment word public 'DATA'
XIE     ends

YIB     segment word public 'DATA'
YIB     ends
YI      segment word public 'DATA'
YI      ends
YIE     segment word public 'DATA'
YIE     ends


_DATA    segment dword public 'DATA'

        public  __D16Infoseg
        public  __x386_zero_base_selector
        public  __init_387_emulator
        public  "C",__int23_exit

__D16Infoseg                dw  0020h   ; DOS/4G kernel segment
__x386_zero_base_selector   dw  0       ; base 0 selector for X-32VM

__init_387_emulator         dd  0       ; prevent emulator even with -fpi
__int23_exit                dd  __null_int23_exit

caller_stack    label   fword
caller_esp      dd      0
caller_ss       dw      0

_DATA    ends

DATA    segment word public 'DATA'
DATA    ends

_BSS          segment word public 'BSS'
_BSS          ends

STACK_SIZE      equ     1000h

; STACK segment can not be used in DLLs because linker sets stack length
; to zero for DLLs (as it should). Stack space for init/exit DLL routines
; is added to the end of BSS class. Note that the stack will contain
; a copy of the environment and the program name.

PRIVATE_STACK  segment word public 'BSS'
        db      (STACK_SIZE) dup(?)
_stack_top LABEL BYTE
PRIVATE_STACK  ends

STACK   segment para stack 'STACK'
STACK   ends


        assume  nothing
        public  __DLLstart_

        assume  cs:_TEXT

__DLLstart_ proc far
        jmp     short around

        align   4
        dd      ___begtext      ; make sure dead code elimination
                                ; doesn't kill BEGTEXT
;
; miscellaneous code-segment messages
;
ConsoleName     db      "con",00h
NewLine         db      0Dh,0Ah

        assume  ds:DGROUP

around: mov     si,DGROUP               ; set DS to DGROUP
        mov     ds,esi                  ;
        mov     caller_esp,esp          ; save caller stack
        mov     caller_ss,ss            ;
        mov     ecx,ss
        mov     edx,esp
        mov     ss,esi                  ; set new stack
        mov     esp,offset _stack_top

        and     esp,0fffffffch          ; make sure stack is on a 4 byte bdry
        push    ecx
        push    ebx
        mov     ebx,esp                 ; get sp
        mov     _STACKTOP,ebx           ; set stack top
        mov     _curbrk,ebx             ; set first available memory location

        or      eax,eax                 ; check if init/exit DLL
        je      init_DLL

        mov     es,esi                  ; necessary?
        sub     ebp,ebp                 ; ebp=0 indicate end of ebp chain
        push    1                       ; indicate termination
        call    __LibMain               ; call DLL exit code
        add     esp,4
        xor     eax,eax
        jmp     do_exit

init_DLL:
        mov     dx,78h                  ; - see if Rational DOS/4G
        mov     ax,0FF00h               ; - ...
        int     21h                     ; - ...
        test    al,al                   ; - ...
        je      error_exit              ; - quit if not Rational DOS/4G
        mov     ax,gs                   ; - get segment address of kernel
        test    ax,ax                   ; - if not zero
        je      short rat9              ; - then
        mov     __D16Infoseg,ax         ; - - remember it
rat9:                                   ; - endif
        mov     ax,DPMIGetSegmentBaseAddress ; - check data segment base
        mov     bx,ds                   ; - set up data segment
        int     31h                     ; - DPMI call
        mov     ax,XS_RATIONAL_ZEROBASE*256+X_RATIONAL ; - asssume Rational 32-bit Extender / extender subtype
        or      dx,cx                   ; - if base is non-zero
        jz      rat10                   ; - then
        mov     ah,XS_RATIONAL_NONZEROBASE; - Rational non-zero based data
rat10:                                  ; - endif
        mov     _psp,es                 ; - save segment address of PSP
        mov     cx,es:[02ch]            ; - get environment segment into cx

        mov     _Extender,al            ; record extender type
        mov     _ExtenderSubtype,ah     ; record extender subtype

        xor     esi,esi                 ; offset 0 for environment strings
        mov     ebx,ds
        mov     es,ebx                  ; get access to code segment
        mov     es:__saved_DS,ds        ; save DS value
        mov     dword ptr _Envptr,esi   ; save address of environment strings
        mov     word ptr _Envptr+4,cx   ; save segment of environment area
        push    esi                     ; save address of environment strings
;
;       copy command line into bottom of private stack
;
        mov     edi,81h
        mov     es,_psp                 ; point to PSP
        mov     edx,offset _end - STACK_SIZE
        add     edx,0FH
        and     dl,0F0H
        sub     ecx,ecx
;        mov     cl,es:[edi-1]          ; get length of command
        xor     cl,cl
        cld                             ; set direction forward
        mov     al,' '
        repe    scasb
        lea     esi,-1[edi]
        mov     edi,edx
        mov     ebx,es
        mov     edx,ds

        assume  ds:nothing

        mov     ds,ebx
        mov     es,edx                   ; es:edi is destination
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
        mov     ebp,FLG_LFN             ; assume 'no87=' and 'lfn=n' env. var. not present
L1:     mov     eax,[esi]               ; get first 4 characters
        or      eax,20202020h           ; map to lower case
        cmp     eax,37386f6eh           ; check for 'no87'
        jne     short L2                ; skip to next test if not 'no87'
        cmp     byte ptr 4[esi],'='     ; make sure next char is '='
        jne     short L3                ; no
        or      ebp,FLG_NO87            ; - indicate 'no87' was present
        jmp     L3
L2:     cmp     eax,3d6e666ch           ; check for 'lfn='
        jne     short L3                ; skip if not 'lfn='
        mov     al,byte ptr 4[esi]      ; get next character
        or      al,20h                  ; map to lower case
        cmp     al,'n'                  ; make sure next char is 'n'
        jne     short L3                ; no
        and     ebp,not FLG_LFN         ; indicate no 'lfn=n' present
L3:     cmp     byte ptr [esi],0        ; end of string ?
        lodsb
        jne     L3                      ; until end of string
        cmp     byte ptr [esi],0        ; end of all strings ?
        jne     L1                      ; if not, then skip next string
        lodsb
        inc     esi                     ; point to program name
        inc     esi                     ; . . .
;
;       copy the program name into bottom of stack
;
L4:     cmp     byte ptr [esi],0        ; end of pgm name ?
        movsb                           ; copy a byte
        jne     L4                      ; until end of pgm name
        pop     ds                      ; restore ds
        pop     esi                     ; restore address of pgm name
        mov     ebx,esp                 ; end of stack in data segment

        assume  ds:DGROUP
        mov     eax,ebp
        and     ebp,FLG_NO87            ; only leave the NO87 bit in ebp
        mov     __no87,al               ; set state of "NO87" enironment var
        and     __uselfn,ah             ; set "LFN" support status

        mov     _STACKLOW,edi           ; save low address of stack
        mov     _dynend,ebx             ; set top of dynamic memory area

        mov     ecx,offset _end - STACK_SIZE ; end of _BSS segment (start of STACK)
        mov     edi,offset _edata       ; start of _BSS segment
        sub     ecx,edi                 ; calc # of bytes in _BSS segment
        cmp     _Extender,X_RATIONAL    ; if not Rational DOS extender
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

        mov     eax,offset _end - STACK_SIZE ; cmd buffer pointed at by EAX
        add     eax,0FH
        and     al,0F0H
        mov     _LpCmdLine,eax          ; save command line address
        mov     _LpPgmName,esi          ; save program name address
        sub     ebp,ebp                 ; ebp=0 indicate end of ebp chain
        push    0                       ; indicate initialization
        call    __LibMain               ; call DLL init code
        add     esp,4

;       don't touch AL here, it has the return code
        jmp     exit_code_eax

error_exit:
        or      eax,-1                  ; exit code -1
        jmp     do_exit

        public  __do_exit_with_msg__

; input: ( char *msg, int rc )  always in registers
; may be only called from startup code, after that there is
; nowhere to exit to!

__do_exit_with_msg__:
        push    edx                     ; save return code
        push    eax                     ; save address of msg
        mov     edx,offset ConsoleName
        mov     ax,03d01h               ; write-only access to screen
        int     021h
        mov     bx,ax                   ; get file handle
        pop     edx                     ; restore address of msg
        mov     esi,edx                 ; get address of msg
        cld                             ; make sure direction forward
nextc:  lodsb                           ; get char
        test    al,al                   ; end of string?
        jne     nextc                   ; no
        mov     ecx,esi                 ; calc length of string
        sub     ecx,edx                 ; . . .
        dec     ecx                     ; . . .
        mov     ah,040h                 ; write out the string
        int     021h                    ; . . .
        mov     edx,offset NewLine      ; write out the string
        mov     ecx,sizeof NewLine      ; . . .
        mov     ah,040h                 ; . . .
        int     021h                    ; . . .
        pop     eax                     ; restore return code
exit_code_eax:
        or      eax,eax
        je      do_exit
        push    eax                     ; don't destroy error code
        call    __CommonTerm            ; terminate the runtime
        pop     eax

do_exit:
        mov     si,DGROUP
        mov     ds,esi
        lss     esp,caller_stack
        ret
__DLLstart_ endp

        public  __GETDS
        align   4
__GETDS proc    near
public "C",__GETDSStart_
__GETDSStart_ label byte
        mov     ds,cs:__saved_DS        ; load saved DS value
        ret
__saved_DS  dw  0               ; save area for DS for interrupt routines
public "C",__GETDSEnd_
__GETDSEnd_ label byte
__GETDS endp

        align   4
__null_int23_exit proc near
public "C",__null_int23_exit
        ret
__null_int23_exit endp

_TEXT   ends

        end __DLLstart_
