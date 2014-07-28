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
;* Description:  DOS/16M 16-bit protected mode startup code.
;*
;*****************************************************************************


;
;  To reassemble this file for DOS/16M, use the following command:
;               wasm dos16m -bt=DOS -ml -2r
;
        .286p

include xinit.inc

        name    dos16m
        .dosseg

        assume  nothing

        extrn   __CMain                 : far
        extrn   __FInitRtns             : far
        extrn   __FFiniRtns             : far
        extrn   __fatal_runtime_error_  : far
        extrn   stackavail_             : far
        extrn   _edata                  : byte  ; end of DATA (start of BSS)
        extrn   _end                    : byte  ; end of BSS (start of STACK)

public  __acrtused              ; trick to lend harmony to dealings with
        __acrtused = 9876h      ; DOS16LIB

DGROUP  group _NULL,_AFTERNULL,CONST,STRINGS,_DATA,DATA,BCSD,XIB,XI,XIE,YIB,YI,YIE,_BSS,STACK,verylast

_TEXT   segment word public 'CODE'

C_ETEXT segment para public 'ENDCODE'
        public  __first_data_16M                ; For DOS/16M seginfo struct in PML
__first_data_16M        label word
        dw      C_ETEXT         ; force reference so /packcode will work
        db      14 dup (?)      ; force one paragraph
C_ETEXT ends

nullseg segment para public 'CODE'
        assume  cs:nullseg
fake_start:     jmp     short bad_start
                nop
                nop
        public  d16_seginfo_struct
d16_seginfo_struct label word
seginfo_id      db      'DOS/16M',0     ; will be offset 0 in this segment
info_version    dw      3               ; in case structure is extended
info_init_ip    dw      OFFSET _cstart_ ; address of compiler
info_code_seg   dw      SEG _cstart_    ; startup code
info_overlay    dd      0               ; pointer to overlay info structure, if any
info_load_info  dw      ovi_data        ; seg of optional loader control info
info_first_code dw      SEG _cstart_    ; seg of first code seg
info_first_data dw      SEG __first_data_16M    ; seg of first data seg
info_data_seg   dw      SEG __main_data_16M     ; seg of main data seg (dgroup)
;;info_stack_seg  dw      SEG __stack_16M ; seg of stack (may be dgroup)
info_stack_seg  dw      SEG __main_data_16M ; seg of stack (may be dgroup)
info_verylast   dw      SEG __end_16M   ; end of program image
info_gdt        dw      gdtseg          ; seg of dummy GDT segment (8)
info_idt        dw      idtseg          ; seg of dummy IDT segment (0x10)
info_d16_data   dw      rtdataseg       ; compiler-specific info for MAKEPM (0x20)
info_flags      dw      0               ; bit vector consisting of these flags:
info_stack_is_resized   equ     1       ; flag: startup code will resize stack
                                        ;       so that MAKEPM should not worry
                                        ;       about tiny stack segs

; watch this space for exciting new pointers (but update the version when
;       new elements are added).

                dw      rtcodeseg       ; (0x18)
                dw      pspseg          ; (0x28)
                dw      envseg          ; (0x30)
                dw      mem0seg         ; (0x38)
                dw      dummy14         ; (0x70)


comment /*
        Programs that have not been processed with MAKEPM will start
        at fake_start, put out the badstart_msg and exit.  MAKEPM
        finds the actual start address from info_start in the
        d16_seginfo_struct and fixes the start address to __startup_.
                                                                        */

bad_start:                              ; .EXE indicates to start here
        mov     dx, OFFSET nullseg:badstart_msg
        mov     ax,cs
        mov     ds,ax
        mov     ah,9                    ; DOS output string
        int     21h
        mov     ax,4CFFh                ; DOS exit with code OFF
        int     21h

badstart_msg    db      '.EXE has not been processed by MAKEPM', 10, 13, '$'

nullseg ends

; The following segment definitions allow programs to have symbolic
; references to system structures.  These segments will no be in the
; .EXP file, but references to the segments will be resolved to the
; corresponding global system segment by MAKEPM.

gdtseg  segment para public 'FAR_DATA'  ; MAKEPM maps references to this
        public __gdt                    ; segment to selector 8h
__gdt   dw      0
gdtseg  ends

idtseg  segment para public 'FAR_DATA'  ; MAKEPM maps references to this
        public __idt                    ; segment to selector 10h
__idt   dw      0
idtseg  ends

rtcodeseg       segment para public 'FAR_DATA'  ; MAKEPM maps references to this
        public __rtcode                 ; segment to selector 18h
__rtcode        dw      0
rtcodeseg       ends

rtdataseg       segment para public 'FAR_DATA'  ; MAKEPM maps references to this
        public __d16info                ; segment to selector 20h
__d16info       dw      0
rtdataseg       ends

pspseg  segment para public 'FAR_DATA'  ; MAKEPM maps references to this
        public __psp16M                 ; segment to selector 28h
__psp16M        dw      0
pspseg  ends

envseg  segment para public 'FAR_DATA'  ; MAKEPM maps references to this
        public __env16M                 ; segment to selector 30h
__env16M        dw      0
envseg  ends

mem0seg segment para public 'FAR_DATA'  ; MAKEPM maps references to this
        public __mem16M                 ; segment to selector 38h
__mem16M        dw      0
mem0seg ends

dummy14 segment para public 'FAR_DATA'  ; MAKEPM maps references to this
        public __dummy16M               ; segment to selector 70h
__dummy16M      dw      0
dummy14 ends

ovi_data segment para public 'CODE'     ; MAKEPM assigns this as the first
ovi_data ends                           ; code segment (first loaded)
; ovi_data, if present, has special load_time information about segments.
; It is used by the VM and OM memory managers.

FAR_DATA segment byte public 'FAR_DATA'
FAR_DATA ends

        assume  ds:DGROUP

        INIT_VAL        equ 0101h
        NUM_VAL         equ 16

_NULL   segment para public 'BEGDATA'
        public  __main_data_16M         ; For DOS/16M seginfo struct in PML
__main_data_16M label word
__nullarea label word
        dw      NUM_VAL dup(INIT_VAL)
        public  __nullarea
_NULL   ends

_AFTERNULL segment word public 'BEGDATA'
        dw      0                       ; nullchar for string at address 0
_AFTERNULL ends

CONST   segment word public 'DATA'
CONST   ends

STRINGS segment word public 'DATA'
STRINGS ends

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

_DATA   segment word public 'DATA'

        extrn   ___d16_selectors:far
        extrn   "C",__uselfn:byte

        public  __ovlflag
        public  __intno
        public  __ovlvec

        public  "C",_curbrk
        public  "C",_psp
        public  "C",_osmajor
        public  "C",_osminor
        public  "C",_osmode
        public  "C",_HShift
        public  "C",_STACKLOW
        public  "C",_STACKTOP
        public  "C",_cbyte
        public  "C",_child
        public  __no87
        public  __get_ovl_stack
        public  __restore_ovl_stack
        public  __close_ovl_file
        public  "C",__FPE_handler
        public  "C",_LpCmdLine
        public  "C",_LpPgmName

_curbrk     dw 0                ; top of usable memory
_psp        dw 0                ; segment addr of program segment prefix
_osmajor    db 0                ; major DOS version number
_osminor    db 0                ; minor DOS version number
_osmode     db 1                ; 0 => DOS real mode, 1 =>protect mode
_HShift     db 3                ; Huge Shift amount (real-mode=12,prot-mode=3)
_STACKLOW   dw 0                ; lowest address in stack
_STACKTOP   dw 0                ; highest address in stack
_cbyte      dw 0                ; used by getch, getche
_child      dw 0                ; non-zero => a spawned process is running
__no87      db 0                ; non-zero => "NO87" enviroment var present
__get_ovl_stack dw 0,0          ; get overlay stack pointer
__restore_ovl_stack dw 0,0      ; restore overlay stack pointer
__close_ovl_file dw 0,0         ; close the overlay file handle
__FPE_handler dd 0              ; FPE handler
_LpCmdLine dw 0,0               ; lpCmdLine (for _argc, _argv processing)
_LpPgmName dw 0,0               ; lpPgmName (for _argc, _argv processing)

;       Variables filled in by Microsoft Overlay Manager
;       These are here for people who want to link with Microsoft Linker
;       and use CodeView for debugging overlayed programs.
__ovlflag  db 0                 ; non-zero => program is overlayed
__intno    db 0                 ; interrupt number used by MS Overlay Manager
__ovlvec   dd 0                 ; saved contents of interrupt vector used

_DATA   ends

DATA    segment word public 'DATA'
DATA    ends

BCSD    segment word public 'DATA'
BCSD    ends

_BSS    segment word public 'BSS'
_BSS    ends

STACK_SIZE      equ    1000h

STACK   segment para stack 'STACK'
        public  __stack_16M     ; For DOS/16M seginfo struct in PML
__stack_16M     label word
        db      (STACK_SIZE) dup(?)
stkend  label   word
STACK   ends

verylast segment para 'STACK'
        public  __end_16M       ; For DOS/16M seginfo struct in PML
__end_16M       label word
verylast ends

        assume  nothing
        public  _cstart_
        public  _Not_Enough_Memory_

        assume  cs:_TEXT

_startup_ proc near
        dd      stackavail_
_cstart_:
        jmp     around

;
; copyright message
;
include msgrt16.inc
include msgcpyrt.inc

;
; miscellaneous code-segment messages
;
NullAssign      db      '*** NULL assignment detected',0
NoMemory        db      'Not enough memory',0
ConsoleName     db      'con',0
NewLine         db      0Dh,0Ah
msg_notPM       db      'requires DOS/16M', 0Dh, 0Ah, '$'

_Not_Enough_Memory_:
        mov     bx,1                    ; - set exit code
        mov     ax,offset NoMemory      ;
        mov     dx,cs                   ;
        call    __fatal_runtime_error_  ; - display msg and exit

around:
        mov     ax, 0FF00h              ; *RSI* see if DOS/16M really there
        mov     dx, 78h                 ; set illegal mem strategy
        int     21h                     ; maybe DOS, maybe DOS/16M
        inc     ax                      ; DOS/16M returns -1 in ax
        jz      setup                   ; okay, DOS/16M active
        mov     ah, 9                   ; not DOS/16M, display error
        mov     dx, offset _text:msg_notPM
        push    cs
        pop     ds
        int     21h                     ; with a message
        mov     ax, 4CFFh               ; and return code -1
        int     21h

setup:  sti                             ; enable interrupts
        mov     cx,DGROUP               ; get proper stack segment

        assume  es:DGROUP

        mov     es,cx                   ; point to data segment
        mov     bx,offset DGROUP:_end   ; get bottom of stack
        add     bx,0Fh                  ; ...
        and     bl,0F0h                 ; ...
        mov     es:_STACKLOW,bx         ; ...
        mov     es:_psp,ds              ; save segment address of PSP

        mov     bx,offset DGROUP:verylast ; get top of stack
        add     bx, 15                  ; round up to paragraph boundary
        and     bx, 0FFF0h              ; ...
        jne     not64k                  ; if 64K
        mov     bx,0fffeh               ; - set _curbrk to 0xfffe
not64k:                                 ; endif
        mov     es:_curbrk,bx           ; set top of memory owned by process
        mov     ss,cx                   ; set stack segment
        mov     sp,bx                   ; set sp relative to DGROUP
        mov     es:_STACKTOP,bx         ; set stack top
;
        mov     di,ds                   ; point es to PSP
        mov     es,di                   ; ...
;
;       copy command line into bottom of stack
;
        mov     di,81H                  ; DOS command buffer ds:si
        mov     cl,-1[di]               ; get length of command
        xor     ch,ch
        cld                             ; set direction forward
        mov     al,' '
        repe    scasb
        lea     si,-1[di]

        mov     dx,DGROUP
        mov     es,dx                   ; es:di is destination
        mov     di,es:_STACKLOW
        mov     es:_LpCmdLine+0,di      ; stash lpCmdLine pointer
        mov     es:_LpCmdLine+2,es      ; ...
        je      noparm
        inc     cx
        rep     movsb
noparm: sub     al,al
        stosb                           ; store NULLCHAR
        stosb                           ; assume no pgm name, store NULLCHAR
        dec     di                      ; back up pointer 1
;
;       get DOS version number
;
        mov     ah,30h
        int     21h
        mov     es:_osmajor,al
        mov     es:_osminor,ah
        mov     cx,di                   ; remember address of pgm name
        cmp     al,3                    ; if DOS version 3 or higher
        jb      nopgmname               ; then
;
;       copy the program name into bottom of stack
;
        mov     ax,ds:2ch               ; get segment addr of environment area
        mov     dx,es:_psp              ; get segment addr of PSP

        mov     ds,ax                   ; get segment addr of environment area
        sub     si,si                   ; offset 0
        xor     bp,bp                   ; no87 not present!
L0:     mov     ax,[si]                 ; get first part of environment var
        or      ax,2020H                ; lower case
        cmp     ax,"on"                 ; if first part is 'NO'
        jne     L1                      ; - then
        mov     ax,2[si]                ; - get second part
        cmp     ax,"78"                 ; - if second part is '87'
        jne     L1                      ; - then
        inc     bp                      ; - - set bp to indicate NO87
L1:     cmp     byte ptr [si],0         ; end of string ?
        lodsb
        jne     L1                      ; until end of string
        cmp     byte ptr [si],0         ; end of all strings ?
        jne     L0                      ; if not, then skip next string
        lodsb
        inc     si                      ; - point to program name
        inc     si                      ; - . . .
L2:     cmp     byte ptr [si],0         ; - end of pgm name ?
        movsb                           ; - copy a byte
        jne     L2                      ; - until end of pgm name
nopgmname:                              ; endif
        mov     si,cx                   ; save address of pgm name
        mov     es:_LpPgmName+0,si      ; stash LpPgmName pointer
        mov     es:_LpPgmName+2,es      ; ...

        assume  ds:DGROUP
        mov     dx,DGROUP
        mov     ds,dx
        mov     es,dx
        mov     ax,bp
        mov     __no87,al               ; set state of "NO87" environment var
        and     __uselfn,ah             ; set "LFN" support status
        mov     _STACKLOW,di            ; save low address of stack

        mov     cx,offset DGROUP:_end   ; end of _BSS segment (start of STACK)
        mov     di,offset DGROUP:_edata ; start of _BSS segment
        sub     cx,di                   ; calc # of bytes in _BSS segment
        xor     al,al                   ; zero the _BSS segment
        rep     stosb                   ; . . .

        cmp     word ptr __get_ovl_stack,0 ; if program not overlayed
        jne     _is_ovl                 ; then
        mov     ax,offset __null_ovl_rtn; - set vectors to null rtn
        mov     __get_ovl_stack,ax      ; - ...
        mov     __get_ovl_stack+2,cs    ; - ...
        mov     __restore_ovl_stack,ax  ; - ...
        mov     __restore_ovl_stack+2,cs; - ...
        mov     __close_ovl_file,ax     ; - ...
        mov     __close_ovl_file+2,cs   ; - ...
_is_ovl:                                ; endif
        xor     bp,bp                   ; set up stack frame
        push    bp                      ; ... for new overlay manager
        mov     bp,sp                   ; ...
        ; DON'T MODIFY BP FROM THIS POINT ON!
        mov     ax,offset __null_FPE_rtn; initialize floating-point exception
        mov     word ptr __FPE_handler,ax       ; ... handler address
        mov     word ptr __FPE_handler+2,cs     ; ...

        mov     ax,0ffh                 ; run all initializers
        call    __FInitRtns             ; call initializer routines
        call    __CMain
_startup_ endp

;       don't touch AL in __exit, it has the return code

__exit  proc  far
        public  "C",__exit
        push    ax
        mov     dx,DGROUP
        mov     ds,dx
        cld                             ; check lower region for altered values
        lea     di,__nullarea           ; set es:di for scan
        mov     es,dx
        mov     cx,NUM_VAL
        mov     ax,INIT_VAL
        repe    scasw
        pop     ax                      ; restore return code
        je      ok
;
; low memory has been altered
;
        mov     bx,ax                   ; get exit code
        mov     ax,offset NullAssign    ; point to msg
        mov     dx,cs                   ; . . .

        public  __do_exit_with_msg__

; input: DX:AX - far pointer to message to print
;        BX    - exit code

__do_exit_with_msg__:
        mov     sp,offset DGROUP:_end+80h; set a good stack pointer
        push    bx                      ; save return code
        push    ax                      ; save address of msg
        push    dx                      ; . . .
        mov     di,cs
        mov     ds,di
        mov     dx,offset ConsoleName
        mov     ax,03d01h               ; write-only access to screen
        int     021h
        mov     bx,ax                   ; get file handle
        pop     ds                      ; restore address of msg
        pop     dx                      ; . . .
        mov     si,dx                   ; get address of msg
        cld                             ; make sure direction forward
L3:     lodsb                           ; get char
        test    al,al                   ; end of string?
        jne     L3                      ; no
        mov     cx,si                   ; calc length of string
        sub     cx,dx                   ; . . .
        dec     cx                      ; . . .
        mov     ah,040h                 ; write out the string
        int     021h                    ; . . .
        mov     ds,di
        mov     dx,offset NewLine       ; write out the new line
        mov     cx,sizeof NewLine       ; . . .
        mov     ah,040h                 ; . . .
        int     021h                    ; . . .
        pop     ax                      ; restore return code
ok:
        mov     dx,DGROUP               ; get access to DGROUP
        mov     ds,dx                   ; . . .
        cmp     byte ptr __ovlflag,0    ; if MS Overlay Manager present
        je      no_ovl                  ; then
        push    ax                      ; - save return code
        mov     al,__intno              ; - get interrupt number used
        mov     ah,25h                  ; - DOS func to set interrupt vector
        lds     dx,__ovlvec             ; - get previous contents of vector
        int     21h                     ; - restore interrupt vector
        pop     ax                      ; - restore return code
no_ovl:                                 ; endif
        push    ax                      ; save return code
        xor     ax,ax                   ; run finalizers
        mov     dx,FINI_PRIORITY_EXIT-1 ; less than exit
        call    __FFiniRtns             ; call finalizer routines
        pop     ax                      ; restore return code
        mov     ah,04cH                 ; DOS call to exit with return code
        int     021h                    ; back to DOS
__exit  endp


;
;       set up addressability without segment relocations for emulator
;
public  __GETDS
__GETDS proc    near
        push    ss                      ; set DS=SS
        pop     ds                      ; ...
        ret
__GETDS endp


__null_FPE_rtn proc far
        ret                             ; return
__null_FPE_rtn endp

__null_ovl_rtn proc far
        ret                             ; return
__null_ovl_rtn endp


        public  __DOSseg__
__DOSseg__:                     ; prevent .DOSSEG from occurring

_TEXT   ends

        end     fake_start              ; mark start address in case
                                        ; program is loaded under DOS.
