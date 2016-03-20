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
;* Description:  FlashTek 32-bit DOS extender startup code.
;*
;*****************************************************************************


; Copyright FlashTek Inc 1992 - 1993
; Modified by
; Doug Huffman
; Route 1 Box 54A
; Lenore, ID 83541
; email: doug@proto.com

comment&
        X-32 startup code for producing executables using the Watcom compiler
        and linker with clib3r.  This module must be linked first since it
        determines the segment order for all segments in the final executable.

        Portions of this Code were provided by Watcom Systems Inc and modified
        by FlashTek for the purpose of providing X-32 support for Watcom
        products.

        Portions of this Code have subsequently been modified by Watcom
        for use with the 10.0 release.
        Additional changes have been made to support WASM and both the
        -3r and -3s calling conventions.


        This must be assembled using one of the following commands:
                wasm cstrtx32 -bt=DOS -ms -3r
                wasm cstrtx32 -bt=DOS -ms -3s
&

.386p

include xinit.inc

public  __x386_zero_base_ptr
public  __x386_zero_base_selector
public  __data_bottom
public  __x386_init
public  _cstart_
public  __do_exit_with_msg__
public  "C",sbrk
public  "C",__exit
public  "C",__brk
public  __x32_zero_base_ptr
public  __x32_zero_base_selector

;define 16 bit segments first so they will be located at the beginning
;of the executable.

__X386_DATASEG_16      segment para use16 public 'DATA16'
;16 bit data segment for real mode and priv level 0 activities
        dw      _TEXT
        extrn __x386_dbg_hook:word
        dw      offset __x386_dbg_hook  ;debugger hook
        dd      offset _TEXT:_edata            ;B format information
        dd      offset _TEXT:_end              ;B format information
__X386_DATASEG_16      ends

__X386_STACK            segment para use16 stack 'STACK' ;phony stack segment
;this is a fake stack segment, the Watcom linker sets the size according to the
;stacksize parameter.  This is only used for locating the 16 bit data object
__X386_STACK            ends
__X386_GROUP_16         group   __X386_DATASEG_16,__X386_STACK

__X386_CODESEG_16       segment para use16 public 'CODE'
assume cs:__X386_CODESEG_16,ds:nothing,es:nothing,fs:nothing,gs:nothing,ss:nothing
extrn __x386_start:near
_cstart_:
        jmp     __x386_start    ;jump to 16 bit initialization
__X386_CODESEG_16       ends


        extrn   __CMain         : near
        extrn   __InitRtns      : near
        extrn   __FiniRtns      : near
        extrn   __DOSseg__      : near

        extrn   _edata          : byte          ; end of DATA (start of BSS)
        extrn   _end            : byte          ; end of BSS (start of STACK)


DGROUP group BEGTEXT,_TEXT,CODE32,_NULL,_AFTERNULL,CONST,_DATA,DATA,XIB,XI,XIE,YIB,YI,YIE,_BSS,STACK

; this guarantees that no function pointer will equal NULL
; (WLINK will keep segment 'BEGTEXT' in front)
; This segment must be at least 4 bytes in size to avoid confusing the
; signal function.

BEGTEXT  segment use32 para public 'CODE'
        assume  cs:_TEXT
        jmp     null_error
        nop     ;2
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
        assume  cs:nothing
BEGTEXT  ends

_TEXT   segment use32 word public 'CODE'

CODE32  segment use32 dword public 'CODE'
CODE32  ends

        assume  ds:DGROUP

_NULL   segment para public 'BEGDATA'
__data_bottom   label byte      ;lowest address in data segment
__nullarea label word
        db      01h,01h,01h,00h
        public  __nullarea
_NULL   ends

_AFTERNULL segment word public 'BEGDATA'
_AFTERNULL ends

CONST   segment word public 'DATA'
CONST   ends

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
extrn __x32_stack_size:dword
;debugger requires two variables below to be positioned together
__x32_zero_base_selector        label   word
__x386_zero_base_selector       dw      ?       ;writable segment, base = 0
__x32_zero_base_ptr             label dword
__x386_zero_base_ptr            dd      0f0000000h

__D16Infoseg   dw       0020h   ; DOS/4G kernel segment

        public  __D16Infoseg

        extrn   "C",_LpPgmName          : dword
        extrn   "C",_LpCmdLine          : dword
        extrn   "C",__FPE_handler       : dword
        extrn   "C",_Envptr             : fword
        extrn   __no87                  : byte
        extrn   "C",__uselfn            : byte
        extrn   "C",_Extender           : byte
        extrn   __child                 : dword
        extrn   "C",_STACKTOP           : dword
        extrn   "C",_STACKLOW           : dword
        extrn   "C",_osminor            : byte
        extrn   "C",_osmajor            : byte
        extrn   "C",_psp                : word
        extrn   "C",_curbrk             : dword
        extrn   "C",_dynend             : dword
        extrn   __X32VM                 : byte

__x386_break    dd      ?
__saved_DS      dw      0 ; save area for DS for interrupt routines

insuf_msg       db      'Insufficient memory for stack setup',0Dh,0Ah,'$'
null_msg        db      'Null code pointer was called',0
ConsoleName     db      "con",0
NewLine         db      0Dh,0Ah

_DATA    ends


DATA    segment word public 'DATA'
DATA    ends

_BSS    segment word public 'BSS'
_BSS    ends

STACK   segment para stack 'STACK'
STACK   ends

        assume  cs:_TEXT
        assume  ds:DGROUP

;
; copyright message
;
include msgrt32.inc
include msgcpyrt.inc

;
; miscellaneous code-segment messages
;
fpe_handler:    ret

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;This is the location where 16 bit code transfers control to when
;first entering 32 bit code.  DS will = ES = __x386_data_32, GS =
;__x386_seg_env, FS = __x386_seg_psp.
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
__x386_init proc near
;The Watcom compliler uses ds and ss to point to things in the data segment
;so we cannot use the normal limits, we will use the default ds for both ds
;and ss but alter the limits.
        mov     bx,ds
        mov     ecx,offset DGROUP:brk_c
        mov     ax,3505h
        int     21h                     ;set limit to include most code
        push    ds
        pop     ss                      ;Switch SS to default DS selector
        mov     _psp,fs                 ; save segment address of PSP
        mov     ah,30h                  ;get DOS version number
        int     21h
        mov     _osmajor,al
        mov     _osminor,ah

        mov     __saved_DS,ds           ; save DS value
        mov     word ptr _Envptr+4,gs   ; save segment of environment area
        sub     eax,eax                 ; offset 0
        mov     dword ptr _Envptr,eax   ; save offset of environment area
        mov     _Extender,3             ; pretend to be PharLap V3
        mov     __X32VM,1               ; mark that this is X32VM
        push    fs
        pop     es                      ;pointer to psp
        mov     edi,81h                 ;DOS command buffer es:edi
;
;       copy command line into top of stack
;
        movzx   ecx,byte ptr es:[edi-1] ; get length of command
        cld                             ;set direction forward
        mov     al,' '
        sub     esp,ecx                 ;allocate space on stack
        lea     esp,[esp-256]           ;make room for program name
        and     sp,0fffch               ;round down to dword alignment
        repe    scasb
        lea     esi,-1[edi]
        mov     edi,esp                 ;es:edi is destination
        push    ds
        push    es
        pop     ds                      ;ds:esi points to command line
        pop     es
        je      noparm
        inc     ecx
        rep     movsb
noparm: sub     al,al
        stosb                           ; store NULLCHAR
        push    es
        pop     ds                      ;restore ds
        mov     _LpCmdLine,esp          ; save command line address
        mov     _LpPgmName,edi          ; save program name address
        stosb                           ; assume no pgm name
        lds     esi,fword ptr _Envptr   ; load pointer to environment
        dec     edi                     ; back up pointer 1
        push    edi                     ; save pointer to pgm name
        sub     ebp,ebp                 ; assume "NO87" env. var. not present
L1:     mov     eax,[esi]               ; get first 4 characters
        or      eax,2020h               ; map to lower case
        cmp     eax,37386f6eh           ; check for 'no87'
        jne     short L2                ; skip if not "no87"
        cmp     byte ptr 4[esi],'='     ; make sure next char is "="
        jne     short L2                ; no
        inc     ebp                     ; - indicate "NO87" was present
L2:     cmp     byte ptr [esi],0        ; end of string ?
        lodsb
        jne     L2                      ; until end of string
        cmp     byte ptr [esi],0        ; end of all strings ?
        jne     L1                      ; if not, then skip next string
        lodsb                           ; load a zero
        inc     esi                     ; point to program name
        inc     esi                     ; . . .

;
;       copy the program name into top of stack
;
L3:     cmp     byte ptr [esi],0        ; end of pgm name ?
        movsb                           ; copy a byte
        jne     L3                      ; until end of pgm name
        push    es
        pop     ds
        pop     esi                     ; restore address of pgm name

        mov     eax,ebp
        mov     __no87,al               ; set state of "NO87" enironment var
        and     __uselfn,ah             ; set "LFN" support status

        mov     ecx,offset DGROUP:_end  ; end of _BSS segment (start of STACK)
        mov     _dynend,ecx             ; top of dynamic memory allocation
        push    ecx
        add     ecx,0fffh
        and     cx,0f000h
        mov     _curbrk,ecx             ; current break location
        pop     ecx
        mov     edi,offset DGROUP:_edata; start of _BSS segment
        sub     ecx,edi                 ; calc # of bytes in _BSS segment
        mov     dl,cl                   ; save bottom 2 bits of count in edx
        shr     ecx,2                   ; calc # of dwords
        sub     eax,eax                 ; zero the _BSS segment
        rep     stosd                   ; ...
        mov     cl,dl                   ; get bottom 2 bits of count
        and     cl,3                    ; ...
        rep     stosb                   ; ...
;set up stack for DGROUP expansion type memory allocation systems
        mov     ecx,__x32_stack_size
;don't allow smaller than a 4k stack
        mov     eax,4096
        cmp     ecx,eax
        ja short stack_okay
        mov     ecx,eax                 ;use the larger of the two
stack_okay:
        mov     ax,3509h
        mov     ebx,esp
        mov     esi,_curbrk             ;curbrk will be adjusted if required
        int     21h
        jc      insuf_mem
        mov     _curbrk,esi             ;current break point
        mov     esp,ebx                 ;switch to new stack if required
        mov     _STACKLOW,edx           ;lower limit for stack
        mov     _STACKTOP,esp           ; set stack top

        xor     eax,eax
        mov     fs,eax
        mov     gs,eax                  ;makes reloading segs faster

        mov     eax,0FFH                ; run all initalizers
        call    __InitRtns              ; call initializer routines
        mov     eax,_LpCmdLine          ; cmd buffer pointed at by EAX
        sub     ebp,ebp                 ; ebp=0 indicate end of ebp chain
        call    __CMain
__x386_init endp

insuf_mem:
        mov     edx,offset DGROUP:insuf_msg
exit_msg:       ;InitRtns hasn't been called yet so don't call FiniRtns
        mov     ah,9
        int     21h
        mov     ax,4c01h
        int     21h

;       don't touch AL in __exit, it has the return code
__exit   proc near
ifndef __STACK__
        push    eax                     ; save return code on the stack
endif
        jmp short L5

null_error:     ; a null code pointer has been called
        push    ebp
        mov     ebp,esp
        mov     eax,offset DGROUP:null_msg
        mov     dl,1                    ;error code

; input: EAX - pointer to message to print
;        EDX - exit code

__do_exit_with_msg__:
        push    edx                     ; save return code
        push    eax                     ; save address of msg
        mov     edx,offset DGROUP:ConsoleName
        mov     ax,03d01h               ; write-only access to screen
        int     021h
        mov     bx,ax                   ; get file handle
        pop     edx                     ; restore address of msg
        mov     esi,edx                 ; get address of msg
        cld                             ; make sure direction forward
L4:     lodsb                           ; get char
        test    al,al                   ; end of string?
        jne     L4                      ; no
        mov     ecx,esi                 ; calc length of string
        sub     ecx,edx                 ; . . .
        dec     ecx                     ; . . .
        mov     ah,040h                 ; write out the string
        int     021h                    ; . . .
        mov     edx,offset DGROUP:NewLine ; write out the new line
        mov     ecx,sizeof NewLine      ; . . .
        mov     ah,040h                 ; . . .
        int     021h                    ; . . .
L5:
        xor     eax,eax                 ; run finalizers
        mov     edx,FINI_PRIORITY_EXIT-1; less than exit
        call    __FiniRtns              ; call finializer routines
        pop     eax                     ; restore return code from stack
        mov     ah,4cH                  ; DOS call to exit with return code
        int     021h                    ; back to DOS
__exit   endp

        public  __GETDS
__GETDS proc    near
public "C",__GETDSStart_
__GETDSStart_ label byte
        mov     ds,cs:__saved_DS        ; load saved DS value
        ret                             ; return
public "C",__GETDSEnd_
__GETDSEnd_ label byte
__GETDS endp

sbrk   proc    near
comment&
        The amount of memory requested is passed in register eax.  We will not
        assume at this time that the amount is page granular but we will
        always allocate page granular quantities anyway.
&
ifdef __STACK__
        mov     eax,ss:[esp]
endif
        add     eax,_curbrk
        jnc     sbrk_enter
        jmp     brk_c

sbrk   endp

comment&
        This is called when it is desired to expand the data segment.  The
        parameter passed in eax is the requested new top of the data segment.
        _curbrk and x386_break are updated.
&
__brk   proc    near
ifdef __STACK__
        mov     eax,ss:[esp+4]
endif
sbrk_enter:
        cmp     eax,__x386_break
        jbe short brk_a                 ;jmp if no action required
        push    ecx
        push    eax
        mov     ecx,eax
        mov     ax,3508h
        int     21h                     ;expand DGROUP block
        jc short brk_b
;eax is the new lowest legal value for esp, ecx is the actual break value.
        mov     _STACKLOW,eax
        mov     __x386_break,ecx
        pop     eax
        pop     ecx
brk_a:
        xchg    eax,_curbrk
        clc                             ; indicate success
        ret
brk_b:  ;insufficient memory
        pop     ecx                     ;pop and discard original eax
        pop     ecx                     ;restore original ecx
brk_c:
        or      eax,-1
        stc                             ; indicate allocation failure
        ret

__brk  endp

;
;       __Int21 interface to OS int 21h translator
;
        public  __Int21_
        public  __fInt21_

__fInt21_ proc  far
        call    __Int21_
        ret
__fInt21_ endp

__Int21_ proc   near
        cmp     AH,48h                  ; allocate memory?
        je      short AllocMem          ; branch if yes
        cmp     AH,4Eh                  ; DOS Find First?
        je      short FindFirst         ; branch if yes
        cmp     AH,4Fh                  ; DOS Find Next?
        je      short FindNext          ; branch if yes
        int     21h                     ; issue int 21
        ret                             ; return
AllocMem:
        mov     eax,ebx                 ; get length to allocate
        call    sbrk                    ; allocate it
        ret                             ; return
FindFirst:
        push    EDX                     ; save filename address
        mov     EDX,EBX                 ; get DTA address
        mov     AH,1Ah                  ; set DTA address
        int     21h                     ; ...
        pop     EDX                     ; restore filename address
        mov     AH,4Eh                  ; find first
        int     21h                     ; ...
        ret                             ; return
FindNext:
        test    AL,AL                   ; if not FIND NEXT
        jne     short findret           ; then return
        push    EDX                     ; save EDX
        mov     AH,1Ah                  ; set DTA address
        int     21h                     ; ...
        mov     AH,4Fh                  ; find next
        int     21h                     ; ...
        pop     EDX                     ; restore EDX
findret:
        ret                             ; return
__Int21_ endp

_TEXT   ends

        end     _cstart_
