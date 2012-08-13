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


.386p

    PUBLIC  RMTrapHandler_
    PUBLIC  RawSwitchHandler_
    PUBLIC  BackFromRealMode_
    PUBLIC RMTrapInit_
    PUBLIC RMTrapAccess_
    PUBLIC RMTrapFini_
    PUBLIC Interrupt10_
    PUBLIC Interrupt15_
    PUBLIC Interrupt1b_23_
    PUBLIC Interrupt24_
    PUBLIC _RMSegStart
    PUBLIC _RMDataStart
    PUBLIC _RMSegEnd


DEBUG           equ     0
include dsxdebug.inc

ifdef _NEC_PC
 DOS4G_COMM_VECTOR equ  7fh
else
 DOS4G_COMM_VECTOR equ  15h
endif


STKSIZE         equ     4096                    ;size of stack in bytes
MXENTRY_SIZE    equ     6                       ;sizeof( mx_entry )
PSP_ENVSEG_OFF  equ     2ch                     ;off in PSP holding seg of env
VECT_TABLE_SIZE equ     200h                    ;vector table size (in words)
MAX_MSG_SIZE    equ     400h                    ;max # of bytes for messages
TRAP_VER_SIZE   equ     3                       ;sizeof( trap_version )

;
; has match structure in CallTrapInit in DSXHDLR.C
;
INITSTRUCT      STRUC
remote          dw ?
initret         dw ?                            ;retcode from TrapInit()
version         db TRAP_VER_SIZE dup(?)         ;trap_version structure
buffoff         dw ?                            ;where to store errmsg
INITSTRUCT      ENDS

;
; has match structure in TrapAccess in DSXHDLR.C
;
REQSTRUCT       STRUC
in_ptr          dd ?                            ;ptr to input buffer
in_len          dw ?                            ;len of input buffer
out_ptr         dd ?                            ;ptr to output buffer
out_len         dw ?                            ;len of output buffer
reqret          dw ?                            ;ret code from TrapAccess()
REQSTRUCT       ENDS

;
; has to match rm_data structure in DSXUTIL.H
;
STATICDATA      STRUC
orig_vects      dw VECT_TABLE_SIZE dup('JJ')    ;original vector values
vecttable1      dw VECT_TABLE_SIZE dup('HH')    ;area to hold a vector table
vecttable2      dw VECT_TABLE_SIZE dup('II')    ;area to hold a vector table
rm_func         dw 0                            ;function in file to call
initfunc        dd 0aaaaaaaah                   ;addr of TrapInit()
reqfunc         dd 0bbbbbbbbh                   ;addr of TrapAccess()
finifunc        dd 0cccccccch                   ;addr of TrapFini()
envseg          dw 0                            ;seg of environment
envseg_pm       dw 0                            ;pm seg of environment
switchaddr      dd 0                            ;addr to switch back to pmode
saveaddr        dd 0                            ;addr of save state procedure
saveseg         dw 0                            ;segment of save state buffer
saveseg_pm      dw 0                            ; pm seg of save state buffer
savesize        dw 0 
pmode_ds        dw 0                            ;value to put in ds on return
pmode_es        dw 0                            ; ...
pmode_cs        dw 0                            ; ...
pmode_eip       dd 0                            ; ...
pmode_ss        dw 0                            ; ...
pmode_esp       dd 0                            ; ...
save_env        dw 'JJ'
savess          dw 'BB'                         ;ss provided by DOS4G
savesp          dw 'CC'                         ;sp corresponding to 'savess'
OldInt10        DD      0 
Pending         DB      0 
Fail            DB      0
ActFontTbls     DB      0
othersaved      db      0
parmarea        db 40h + MAX_MSG_SIZE dup('M')
stack           db STKSIZE dup('A')
STATICDATA      ENDS


rmhandlr_TEXT16 SEGMENT BYTE PUBLIC USE16 'CODE'
ASSUME  cs:rmhandlr_TEXT16, ds:rmhandlr_TEXT16, ss:rmhandlr_TEXT16

_RMSegStart     LABEL   BYTE


_RMDataStart    LABEL   BYTE
                STATICDATA      <>              ;make sure this is at offset 0

plsdebug        MACRO
        ;for debugging pls only
                cmp     word ptr ds:parmarea, TRAPREQ_FLAG
                jne     backtopm
                push    dx
                push    bx
                mov     bx, OFFSET parmarea + SIZE REQSTRUCT
                mov     dx, ds:[bx]
                mov     bx, dx
                mov     dl, byte ptr ds:[bx]
                setpage 0
                write8  24, 50, dl
                pop     bx
                cmp     dl, 23
                je      loadreq
                pop     dx
                jmp     backtopm
loadreq:        write16 2, 0, ds:pmode_ds
                write16 2, 5, ds:pmode_es
                write16 2, 10, ds:pmode_cs
                write16 2, 15, ds:pmode_ss
                write32 3, 0, ds:pmode_esp
                write32 3, 9, ds:pmode_eip
                write32 4, 0, ds:switchaddr
dummy:          jmp     dummy
                ENDM

;purpose of set_envseg:
;       DOS/4GW does not provide us with an environment so the debugger copies its
;       environment into low memory and we set/restore the env ptr in our
;       psp to this area just after we are called and just before we return
;       to protected mode
;on entry to set_envseg:
;       save_env is the address where the old psp environment seg will be
;                saved
;       new_env  is the address containing the seg of the new environment
set_envseg      MACRO   save_env, new_env
                push    ax
                push    bx
                push    es
                pushf

                mov     ah, 51h                 ;get the current PSP seg
                int     21h
                mov     es, bx
                mov     ax, es:[PSP_ENVSEG_OFF]
                mov     save_env, ax
                mov     ax, new_env
                mov     es:[PSP_ENVSEG_OFF], ax

                popf
                pop     es
                pop     bx
                pop     ax
                ENDM

copy_vectors    MACRO   src_seg, src_off, dst_seg, dst_off
                push    cx
                push    si
                push    di
                push    ds
                push    es

                push    src_seg
                push    dst_seg
                mov     si,src_off
                mov     di,dst_off
                pop     es
                pop     ds
                mov     cx, VECT_TABLE_SIZE

                cli
                rep     movsw
                sti

                pop     es
                pop     ds
                pop     di
                pop     si
                pop     cx
                ENDM

; set DI to offset of save area before calling routine
restore_state   PROC NEAR
                push    ax
                push    es
                mov     es, ds:saveseg
                mov     al, 1                   ;al=1 means restore state
                call    ds:saveaddr

                pop     es
                pop     ax
                ret
restore_state   ENDP

; set DI to offset of save area before calling routine
save_state      PROC NEAR
                push    ax
                push    es

                mov     es, ds:saveseg
                mov     al, 0                   ;al=0 means save state
                call    ds:saveaddr

                pop     es
                pop     ax
                ret
save_state      ENDP


;on entry to calltrapinit, the format of the parameter area is described by
;the INITSTRUCT STRUC - in addition, the string parm is located immediately
;after the last field in INITSTRUCT
;
;to call TrapInit( char far *parm, char far *buff, bool remote ) :
;       dx:ax   holds addr of parm
;       cx:bx   holds addr of buff
;       stack   holds value of remote
RMTrapInit_     PROC NEAR
                push    es
                push    cx
                push    si

                sub     sp, 4                   ;make space for return struct
                mov     si, sp                  ;ret value is stored at si

                push    ds:parmarea.remote      ;place remote on stack
                mov     ax, SIZE INITSTRUCT + OFFSET parmarea
                mov     dx, cs                  ;now dx:ax points to parm
                mov     bx, ds:parmarea.buffoff
                add     bx, OFFSET parmarea
                mov     cx, dx                  ;now cx:bx points to buff
                call    dword ptr ds:initfunc   ;call TrapInit
                pop     dx                      ;put loword of version in dx
                mov     word ptr ds:parmarea.version, dx
                pop     ax                      ;put hibyte of version in al
                mov     ds:parmarea.version+2, al

                pop     si
                pop     cx
                pop     es
                ret
RMTrapInit_     ENDP

;on entry to calltrapaccess, the format of the parameter area is described
;by the REQSTRUCT STRUC
;
;to call TrapAccess( unsigned_8 num_in_mx, mx_entry far *mx_in,
;                    unsigned_8 num_out_mx, mx_entry far *mx_out ) :
;       ax      holds num_in_mx (==1)
;       cx:bx   holds addr of mx_in
;       dx      holds num_out_mx (==1)
;       stack   holds addr of mx_out
;on calltrapaccess exit :
;       ax      holds TrapAccess return value
RMTrapAccess_   PROC NEAR
                push    ax
                push    bx
                push    cx
                push    dx

                cmp     word ptr cs:parmarea.out_ptr, 0
                jne     have_ret
                push    word ptr 0
                mov     dx,0
                push    word ptr 0
                jmp     short remainder
have_ret:
                mov     dx,1
                push    cs
                push    offset parmarea.out_ptr
remainder:
                mov     bx,offset parmarea.in_ptr
                mov     cx,cs
                mov     ax,1
                call    dword ptr ds:reqfunc
                mov     ds:parmarea.reqret, ax  ;store ret code

                pop     dx
                pop     cx
                pop     bx
                pop     ax
                ret
RMTrapAccess_   ENDP

RMTrapFini_     PROC NEAR
                call    dword ptr ds:finifunc
                ret
RMTrapFini_     ENDP

calltrapfile    PROC NEAR
                copy_vectors 0, 0, ds, vecttable2
                copy_vectors ds, vecttable1, 0, 0
                set_envseg ds:save_env, ds:envseg
                call    ds:rm_func
                set_envseg ds:envseg, ds:save_env
                copy_vectors 0, 0, ds, vecttable1
                copy_vectors ds, vecttable2, 0, 0
                ret
calltrapfile    ENDP

chain_down:
                jmp     dword ptr cs:orig_vects+1ah*4

RMTrapHandler_:
                cmp     ah,6
                jne     chain_down
                cmp     cx,0ffffh
                jne     chain_down
                push    ds
                push    ax

                mov     ax, cs
                mov     ds, ax
                mov     ds:savess, ss   ;save ss given to us by DOS4G
                mov     ds:savesp, sp   ;save sp
                mov     ss, ax
                mov     sp, OFFSET stack + STKSIZE

                call    calltrapfile

                mov     ss, ds:savess   ;restore ss
                mov     sp, ds:savesp   ;restore sp
                pop     ax
                pop     ds                      ;restore old ds
                iret

;on entry to RawSwitchProc_ ds, cs, es, and ss already point to
;rmhandlr_TEXT16, and ebp holds the value to put in esp when we return
;to the debugger

RawSwitchHandler_:
                sti
                mov     sp, OFFSET stack + STKSIZE
                mov     ds:pmode_esp, ebp
                xor     di,di
                call    save_state
                cmp     ds:othersaved,0
                je      skip_restore
                mov     di,ds:savesize
                call    restore_state
skip_restore:

                call    calltrapfile

                mov     ds:othersaved,1
                mov     di,ds:savesize
                call    save_state
                xor     di,di
                call    restore_state

                mov     ax, ds:pmode_ds
                mov     cx, ds:pmode_es
                mov     dx, ds:pmode_ss
                mov     si, ds:pmode_cs
                mov     edi, ds:pmode_eip
                mov     ebx, ds:pmode_esp
                jmp     dword ptr ds:switchaddr


Interrupt10_:
                cmp     ax, 1130h
                jne     continue
                mov     cs:ActFontTbls, bl
continue:       jmp     dword ptr cs:OldInt10


Interrupt1b_23_:
                mov     byte ptr cs:Pending, 0ffh
                iret

Interrupt24_:
                mov     al,byte ptr cs:Fail
                iret

Interrupt15_:
        cmp     ax, 0BF02h              ; DOS/16M find xbrk function
        je      hide_self
        cmp     ax, 0BFDCh              ; DOS/16M alternate find xbrk function
        je      hide_self
        jmp     dword ptr cs:orig_vects+DOS4G_COMM_VECTOR*4; chain to previous
hide_self:
        iret                            ; Don't share memory with anyone!

;;
;; The following routine executes in protected mode, but it
;; needs to be in a USE16 segment, which is why it's here
;;

BackFromRealMode_ PROC FAR
                db      066h    ; turn RET into RETD
                ret
BackFromRealMode_ ENDP

_RMSegEnd       LABEL   BYTE

rmhandlr_TEXT16 ENDS
                END
