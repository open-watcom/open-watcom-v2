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
;* Description:  Win386 executable and DLL startup code (32-bit).
;*
;*****************************************************************************


;       This must be assembled using one of the following commands:
;               wasm cstrtw32 -bt=WINDOWS -mf -3r
;               wasm cstrtw32 -bt=WINDOWS -mf -3s
;

.387
.386p

include xinit.inc

DGROUP group CONST,_DATA,DATA,XIB,XI,XIE,YIB,YI,YIE,_BSS,STACK

extrn __DOSseg__:byte

        extrn   WINMAIN : near

cFarProc MACRO name
public "C",name
name    label FWORD
              dd 0
              dw 0
ENDM

FarProc MACRO name
public name
name    label FWORD
              dd 0
              dw 0
ENDM

        assume  nothing

        extrn   _edata          : byte          ; end of DATA (start of BSS)
        extrn   _end            : byte          ; end of BSS (start of STACK)

        extrn   GETMODULEFILENAME : near
        extrn   WEP             : near
        extrn   __InitRtns      : near
        extrn   __FiniRtns      : near
        extrn   "C",exit        : near

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

MAX_FILE_NAME   equ     144
TMPSTACK        equ     _STACKLOW ; location on stack to copy command line

;
; these variables must all be kept in order
;
_LocalPtr   dw  0
public _LocalPtr
;
; This must correspond with structure definition wstart_vars in wininit.c
;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; Begin of wstart_vars structure
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
hInstance      dw   0
hPrevInstance  dw   0
lpCmdLine      dd   0
cmdShow        dw   0
__no87         dw   0       ; non-zero => "NO87" environment var present
; magical selectors for real memory
public ___A000,___B000,___B800,___C000,___D000,___E000,___F000
___A000        dw   0
___B000        dw   0
___B800        dw   0
___C000        dw   0
___D000        dw   0
___E000        dw   0
___F000        dw   0

; data ptrs
cFarProc    _CodeSelectorBaseAddr
cFarProc    _DataSelectorBaseAddr
FarProc     __32BitCallBackAddr
cFarProc    _DLLEntryAddr
cFarProc    _WEPAddr

public  __16BitCallBackAddr
__16BitCallBackAddr dd 0

; proc lists
FarProc     Invoke16BitFunctionAddr
FarProc     __INT21ADDR
FarProc     __WIN16THUNK1ADDR
FarProc     __WIN16THUNK2ADDR
FarProc     __WIN16THUNK3ADDR
FarProc     __WIN16THUNK4ADDR
FarProc     __WIN16THUNK5ADDR
FarProc     __WIN16THUNK6ADDR
FarProc     __Call16Addr
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
; End of wstart_vars structure
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

_LpCmdLine  dd 0          ; pointer to raw command line
_LpPgmName  dd 0          ; pointer to program name (for argv[0])
_STACKLOW   dd 0          ; lowest address in stack
_STACKTOP   dd 0          ; highest address in stack
__ASTACKSIZ dd 0          ; alternate stack size
__ASTACKPTR dd 0          ; alternate stack pointer
_curbrk     dd 0          ; top of usable memory
_cbyte      dd 0          ; used by getch, getche
_osmajor    db 4          ; major DOS version number
_osminor    db 0          ; minor DOS version number
_Extender   db 0          ; 10 => 386 windows
__init_387_emulator db 0  ; to prevent emulator from coming in with
                          ;       -fpi

public  "C",_pid
_pid        dw 0
public  "C",_wincmdptr
_wincmdptr LABEL FWORD
cmd_offset  dd 0
cmd_seg     dw 0
filename    db MAX_FILE_NAME dup(0)
public  "C",__Is_DLL
__Is_DLL    label byte
__inDLL     db 0               ; 0 => ordinary EXE, non-zero => DLL

__FPE_handler dd __null_FPE_rtn ; FPE handler

        public  "C",_LpCmdLine
        public  "C",_LpPgmName
        public  "C",_STACKLOW
        public  "C",_STACKTOP
        public  __ASTACKSIZ
        public  __ASTACKPTR
        public  "C",_curbrk
        public  "C",_cbyte
        public  "C",_osmajor
        public  "C",_osminor
        public  "C",_Extender
        public  __no87
        public  "C",__FPE_handler
        public  __init_387_emulator

_DATA   ends

STACK_SIZE      equ     2000h

STACK   segment para stack 'STACK'
        db      (STACK_SIZE) dup(?)
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
public _wstart_
public _wstart2_
public __DLLstart_
_cstart_ proc  far
_wstart_:
_wstart2_:
__DLLstart_:
        dd      offset  hInstance ; loader starts execution 8 bytes past here
        dd      _end
        mov     _LocalPtr,gs            ; save selector of extender's data
        mov     __inDLL,bl              ; save DLL indicator flag

        mov     _STACKTOP,esp           ; set stack top
        mov     _STACKLOW,ecx           ; and stack low

        ; initialize bss
        lea     ecx,DGROUP:_end         ; end of _BSS segment (start of free)

;;FWC   03-jan-95
;;FWC   lea     edi,DGROUP:_edata       ; start of _BSS segment
;;FWC   Windows extender now passes in EDI the end address of code+data
;;FWC   This handles cases like F77 with uninitialized COMMON blocks

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
        lea     edx,DGROUP:_end         ; start of free
        add     edx,3
        and     edx,not 3
        mov     _curbrk,edx

        push    es                      ; save es
        les     ecx,_WEPAddr            ; get addr of pointer to fill in
        mov     eax,offset __WEP        ; get addr of WEP routine
        mov     es:[ecx],eax            ; so supervisor knows
        pop     es                      ; restore es

        ; put command string on the stack
        push    es
        mov     ebx,TMPSTACK
        mov     _LpCmdLine,ebx          ; for use by initargv
        mov     esi,lpCmdLine           ; offset + selector
        mov     edx,esi                 ; get the
        shr     edx,10h                 ;   selector
        test    edx,edx                 ; is it zero?
        jne     short okcpy             ; no, do the copy
        mov     byte ptr ds:[ebx],0     ; put a trailing zero
        jmp     short donecpy
okcpy:  mov     es,dx
        mov     ds:cmd_seg,es           ; save for later
        movzx   esi,si                  ;    use by getcmd
        mov     ds:cmd_offset,esi
again:  mov     al,byte ptr es:[esi]
        mov     byte ptr ds:[ebx],al
        test    al,al
        je      short donecpy
        inc     esi
        inc     ebx
        jmp     short again
donecpy:pop     es

        movzx   eax,hInstance
        push    eax
        mov     edi, offset filename
        mov     _LpPgmName,edi
        push    edi
        push    MAX_FILE_NAME
        call    GETMODULEFILENAME

        mov     eax,0ffh                ; run all initializers
        call    __InitRtns              ; call initializer routines

;;      /* allocate alternate stack for F77 */
;;      _ASTACKPTR = (char *)alloca( _ASTACKSIZ ) + _ASTACKSIZ;
        cmp     byte ptr __inDLL,0      ; if in DLL
        je      short not_dll           ; then
          mov   eax,_STACKLOW           ; - put alternate stack on bottom
          add   eax,__ASTACKSIZ         ; - ...
          mov   __ASTACKPTR,eax         ; - ...
          jmp   short not_dll2          ; else
not_dll:mov     __ASTACKPTR,esp         ; - save address of alternate stack
        sub     esp,__ASTACKSIZ         ; - allocate alternate stack for F77
not_dll2:                               ; endif

        ; push parms for WINMAIN
        mov     ax,hInstance
        mov     _pid,ax                 ; save for use by getpid()
        movzx   eax,ax
        push    eax
        mov     ax,hPrevInstance
        movzx   eax,ax
        push    eax
        mov     eax,TMPSTACK
        push    eax
        mov     ax,cmdShow
        movzx   eax,ax
        push    eax

        call    WINMAIN
        jmp     exit                    ; exit
;
; copyright message
;
include msgrt32.inc
include msgcpyrt.inc

        dd      ___begtext              ; make sure dead code elimination
                                        ; doesn't kill BEGTEXT segment
_cstart_ endp


__exit  proc far
        public  "C",__exit
ifndef __STACK__
        push    eax                     ; save return code on stack
endif
        cmp     byte ptr __inDLL,0      ; are we in a DLL?
        jne     short skip_fini         ; skip fini rtns if we are
        push    edx                     ; save edx
        xor     eax,eax                 ; run finalizers
        mov     edx,FINI_PRIORITY_EXIT-1; less than exit
        call    __FiniRtns              ; call finalizer routines
        pop     edx                     ; restore edx
skip_fini:
        pop     eax                     ; restore return code from stack
        mov     esp,_STACKTOP           ; reset stack pointer
        mov     ds,_LocalPtr            ; restore ds
        ret
__exit endp

__null_FPE_rtn proc near
        ret                             ; return
__null_FPE_rtn endp

__WEP   proc    far
        push    eax                     ; push parm
        push    cs                      ; simulate far call
        call    WEP                     ; call user's WEP function
        push    edx                     ; save edx
        xor     eax,eax                 ; run all finalizers
        mov     edx,0ffh                ; run all finalizers
        call    __FiniRtns              ; call finalizer routines
        pop     edx                     ; restore edx
        mov     ds,_LocalPtr            ; restore ds
        ret                             ; return
__WEP   endp

public __GETDS
__GETDS proc near
        mov     ds,cs:__saved_DS
        ret
__GETDS endp

public  __Int21
__Int21 proc    near
        push    ebp                     ; save ebp
        mov     ds,_LocalPtr            ; load extenders data segment
        call    fword ptr es:__INT21ADDR; call extender
        pop     ebp                     ; restore ebp
        ret                             ; return
__Int21 endp

_TEXT   ends

        end _cstart_
