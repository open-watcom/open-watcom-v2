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
;* Description:  OS Independent startup code for 32-bit hosted Open Watcom
;*               character-mode apps.
;*
;*****************************************************************************


;       This must be assembled using the following command:
;               masm /mx /s cstrtosi
;

.386p

include struct.inc
include xinit.inc


DGROUP group CONST,_DATA,DATA,TIB,TI,TIE,XIB,XI,XIE,YIB,YI,YIE,_BSS,STACK

extrn __DOSseg__:byte

FarProc MACRO name
public name
name    label FWORD
              dd 0
              dw 0
ENDM

        assume  nothing

        extrn   _edata          : byte          ; end of DATA (start of BSS)
        extrn   _end            : byte          ; end of BSS (start of STACK)

        extrn   __InitRtns      : near
        extrn   __FiniRtns      : near
        extrn   exit_           : near
        extrn   __CMain         : near

; this guarantees that no function pointer will equal NULL
; (WLINK will keep segment 'BEGTEXT' in front)
; This segment must be at least 4 bytes in size to avoid confusing the
; signal function.

BEGTEXT segment use32 word public 'CODE'
        assume  cs:BEGTEXT
forever: jmp    short forever
___begtext label byte
        nop
        nop
        nop
        nop
        public ___begtext
        assume  cs:nothing
BEGTEXT ends

_TEXT   segment use32 word public 'CODE'

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

_DATA   segment use32 word public 'DATA'

;FarProc __INT10ADDR
;FarProc __INT16ADDR
FarProc __INT21ADDR
;FarProc __INT31ADDR

_LpCmdLine  dd 0                ; pointer to raw command line
_LpPgmName  dd 0                ; pointer to program name (for argv[0])
_EnvPtr     dd 0                ; pointer to environment strings
__BreakFlagPtr dd 0             ; pointer to break flag
__Copyright dd 0                ; copyright string
__IsDBCS    dd 0                ; IsDBCS indicator
_environ    dd 0                ; char **envv;
___env_mask dd 0                ; environment mask
_STACKLOW   dd 0                ; lowest address in stack
_STACKTOP   dd 0                ; highest address in stack
__ASTACKSIZ dd 0                ; alternate stack size
__ASTACKPTR dd 0                ; alternate stack pointer
_curbrk     dd 0                ; top of usable memory
__cbyte     dd 0                ; used by getch, getche
_psp        dw 0                ; program segment prefix
_osmajor    db 4                ; major DOS version number
_osminor    db 0                ; minor DOS version number
_Extender   db 0                ; 10 => 386 windows
_ExtenderSubtype  db 0          ;
__OS        db 0                ; OS Identifier

__FPE_handler dd __null_FPE_rtn ; FPE handler

        public  "C",_LpCmdLine
        public  "C",_LpPgmName
        public  "C",_EnvPtr
        public  __BreakFlagPtr
        public  __Copyright
        public  "C",__IsDBCS
        public  _environ
        public  ___env_mask
        public  "C",_STACKLOW
        public  "C",_STACKTOP
        public  __ASTACKSIZ
        public  __ASTACKPTR
        public  "C",_curbrk
        public  __cbyte
        public  "C",_psp
        public  "C",_osmajor
        public  "C",_osminor
        public  "C",_Extender
        public  "C",_ExtenderSubtype
        public  __OS
        public  "C",__FPE_handler

_DATA   ends

STACK   segment para stack 'STACK'
STACK   ends

_BSS          segment word public 'BSS'
_BSS          ends

CONST   segment word public 'DATA'
CONST   ends

DATA    segment word public 'DATA'
DATA    ends

        assume  cs:_TEXT
        assume  ds:_DATA
        assume  fs:_DATA
        assume  gs:_DATA
        assume  ss:_DATA

__saved_DS dw 0

public _cstart_
_cstart_ proc  far
        mov     __OS,ah                 ; save OS ID
        mov     _STACKTOP,esp           ; set stack top
        mov     _STACKLOW,ecx           ; and stack low
        mov     eax,[edi]               ; get program name
        mov     _LpPgmName,eax          ; ...
        mov     eax,4[edi]              ; get command line
        mov     _LpCmdLine,eax          ; ...
        mov     word ptr __INT21ADDR+4,bx ; save far addr of __Int21 handler
        mov     dword ptr __INT21ADDR+0,edx     ; ...
;       mov     word ptr __INT10ADDR+4,bx ; save far addr of __Int10 handler
;       mov     eax,16[edi]                     ; ...
;       mov     dword ptr __INT10ADDR+0,eax     ; ...
;       mov     word ptr __INT16ADDR+4,bx ; save far addr of __Int16 handler
;       mov     eax,20[edi]                     ; ...
;       mov     dword ptr __INT16ADDR+0,eax     ; ...
;       mov     word ptr __INT31ADDR+4,bx ; save far addr of __Int31 handler
;       mov     eax,24[edi]                     ; ...
;       mov     dword ptr __INT31ADDR+0,eax     ; ...
        mov     eax,12[edi]             ; get address of break flag
        mov     __BreakFlagPtr,eax      ; save it
        mov     eax,16[edi]             ; get address of copyright string
        mov     __Copyright,eax         ; save it
        mov     eax,20[edi]             ; get IsDBCS indicator
        mov     __IsDBCS,eax            ; save it

        mov     esi,8[edi]              ; get environment pointer
        mov     _EnvPtr,esi             ; save environment pointer
        push    0                       ; NULL marks end of env array
        _loop                           ; loop
          push  esi                     ; - push ptr to next string
          _loop                         ; - loop (find end of string)
            lodsb                       ; - - get character
            cmp   al,0                  ; - - check for null char
          _until  e                     ; - until end of string
          cmp   byte ptr [esi],0        ; - check for double null char
        _until  e                       ; until end of environment strings
        mov     _environ,esp            ; set pointer to array of ptrs

        ; initialize bss
        lea     ecx,_end                ; end of _BSS segment (start of free)
        lea     edi,_edata              ; start of _BSS segment
        sub     ecx,edi                 ; calc # of bytes in _BSS segment
        xor     eax,eax                 ; zero the _BSS segment
        mov     dl,cl                   ; copy the lower bits of size
        shr     ecx,2                   ; get number of dwords
        rep     stosd                   ; copy them
        mov     cl,dl                   ; get lower bits
        and     cl,3                    ; get number of bytes left (modulo 4)
        rep     stosb

        ; save ds
        mov     ecx,offset __saved_DS
        mov     ds:[ecx],ds

        ; set up heap
        lea     edx,_end                ; start of free
        add     edx,3
        and     edx,not 3
        mov     _curbrk,edx

        mov     eax,0ffh                ; run all initialiers
        call    __InitRtns              ; call initializer routines

;;      /* allocate alternate stack for F77 */
;;      _ASTACKPTR = (char *)alloca( _ASTACKSIZ ) + _ASTACKSIZ;
        mov     __ASTACKPTR,esp         ; save address of alternate stack
        sub     esp,__ASTACKSIZ         ; allocate alternate stack for F77

        sub     ebp,ebp                 ; ebp=0 indicate end of ebp chain
        call    __CMain

        jmp     exit_                   ; exit
        dd      ___begtext              ; make sure dead code elimination
_cstart_ endp


__exit_  proc far
public __exit_
        push    eax                     ; save return value
        push    edx                     ; save edx
        xor     eax,eax                 ; run finalizers
        mov     edx,FINI_PRIORITY_EXIT-1; less than exit
        call    __FiniRtns              ; call finalizer routines
        pop     edx                     ; restore edx
        pop     eax                     ; restore return value
        mov     esp,_STACKTOP           ; reset stack pointer
        ret
__exit_ endp

__null_FPE_rtn proc near
        ret                             ; return
__null_FPE_rtn endp

;public __Int10
;public __Int10_
;__Int10        proc    near
;__Int10_:push  ebp                     ; save ebp
;       call    fword ptr __INT10ADDR   ; call extender
;       pop     ebp                     ; restore ebp
;       ret                             ; return
;__Int10        endp
;
;public __Int16
;public __Int16_
;__Int16        proc    near
;__Int16_:push  ebp                     ; save ebp
;       call    fword ptr __INT16ADDR   ; call extender
;       pop     ebp                     ; restore ebp
;       ret                             ; return
;__Int16        endp

public  __Int21
public  __Int21_
__Int21 proc    near
__Int21_:push   ebp                     ; save ebp
        call    fword ptr __INT21ADDR   ; call extender
        pop     ebp                     ; restore ebp
        ret                             ; return
__Int21 endp

;public __Int31
;public __Int31_
;__Int31        proc    near
;__Int31_:push  ebp                     ; save ebp
;       call    fword ptr __INT31ADDR   ; call extender
;       pop     ebp                     ; restore ebp
;       ret                             ; return
;__Int31        endp

_TEXT   ends

        end     _cstart_
