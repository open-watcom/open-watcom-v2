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


.286p
DEBUG   equ 0
include ..\asm\dsxdebug.inc

        EXTRN   _GDT            :fword
        EXTRN   _LDT            :word
        EXTRN   _IDT            :fword
        EXTRN   _PMProcCalled   :byte
        EXTRN   _OldPMHandler   :dword
        EXTRN   cputs_          :near
        EXTRN   getch_          :near

        PUBLIC  GetPModeAddr_
        PUBLIC  EnterPMode_
        PUBLIC  SaveState_
        PUBLIC  DoRawSwitch_
        PUBLIC  PM66Handler_
        PUBLIC  PMProc_
        PUBLIC  StoreDTs_


DGROUP          GROUP CONST

CONST           SEGMENT BYTE PUBLIC 'DATA'
procmsg         db "Hi from PMProc", 10, 13, 0
pmmsg           db "Hi from PM66Handler", 10, 13, 0
CONST           ENDS

dbg_switch      MACRO row, swaddr, procaddr
                write16 row, 0, ax
                write16 row, 5, cx
                write16 row, 10, dx
                write16 row, 15, bx
                write16 row, 20, si
                write16 row, 25, di
                write32 row, 30, swaddr
                write16 row, 45, ds
                write16 row, 50, es
                write16 row, 55, ss
                write16 row, 60, cs
                write16 row, 65, procaddr
                keywait
ENDM

_TEXT           SEGMENT BYTE PUBLIC 'CODE'
ASSUME cs:_TEXT, ds:DGROUP

;on entry to GetPModeAddr_
;       bx      unsigned * in which the DPMI memory size will be put
;on exit
;       ax:dx   a far ptr to the DPMI address
GetPModeAddr_   PROC NEAR
                push    bp
                push    bx
                push    cx
                push    si
                push    ax
                mov     bp, sp

                mov     ax, 1687h
                int     2fh
                mov     bx, [bp]
                test    ax, ax
                jnz     cant_get_addr
                mov     [bx], si
                mov     ax, es
                mov     dx, di
                jmp     done

cant_get_addr:  mov     [bx], si
                xor     ax, ax
                xor     dx, dx

done:           add     sp, 2
                pop     si
                pop     cx
                pop     bx
                pop     bp
                ret
GetPModeAddr_   ENDP

;on entry to EnterPMode_
;       ax:dx   far address to call, to enter pmode
;       bx      ptr to memory area needed by DPMI
;on exit
;       ax      return code, 0=failure
EnterPMode_     PROC NEAR
                push    bp
                mov     bp, sp
                push    ax
                push    dx

                mov     es, bx
                xor     ax, ax
                call    dword ptr [bp-4]
                jc      cant_enter_pm
                xor     ax, ax
                not     ax
                jmp     done_enter_pm

cant_enter_pm:  xor     ax, ax

done_enter_pm:  mov     sp, bp
                pop     bp
                ret
EnterPMode_     ENDP

;on entry to SaveState_
;       ax      0 means save state, 1 means restore state
;       di      ptr to state save buffer
;       cx:bx   far ptr to DPMI's save state procedure
;on exit
;       no returns
SaveState_      PROC NEAR
                push    es
                push    bp

                mov     bp, sp
                push    ds
                pop     es
                push    cx
                push    bx

                ;write16 22, 0, ax
                ;write16 22, 5, es
                ;write16 22, 9, di
                ;write32 22, 15, [bp-4]
                ;keywait

                call    dword ptr [bp-4]

                mov     sp, bp
                pop     bp
                pop     es
                ret
SaveState_      ENDP

;on entry to DoRawSwitch_
;       ss:sp[2] = DPMI's raw switch address (far pointer) - to do switch
;       ss:sp[6] = DPMI's raw switch address (far pointer) - to switch back
;       ss:sp[10] = new mode's IP (ie. offset of proc to call after switch)
;       ss:sp[12] = a ptr to a SREGS struct which gives values for the
;                   new mode's segment registers
;on exit
;       no returns
DoRawSwitch_    PROC NEAR
                pushf
                push    bp
                mov     bp, sp

                push    cs
                push    ss
                push    es
                push    ds

                mov     di, [bp+16]             ;ptr to new mode's seg values
                mov     ax, [di+6]              ;new mode's ds
                mov     cx, [di]                ;new mode's es
                mov     dx, [di+4]              ;new mode's ss
                mov     si, [di+2]              ;new mode's cs
                mov     di, OFFSET new_mode     ;new mode's ip
                mov     bx, sp                  ;new mode's sp

                dbg_switch 23, [bp+6], [bp+14]  ;for debugging only

                jmp     dword ptr [bp+6]        ;switch modes

new_mode:
                nop                             ;for debugging only
                nop                             ;for debugging only
                nop                             ;for debugging only

                call    [bp+14]                 ;call the proc

                pop     ax                      ;old mode's ds
                pop     cx                      ;old mode's es
                pop     dx                      ;old mode's ss
                pop     si                      ;old mode's cs
                mov     di, OFFSET old_mode     ;old mode's ip
                mov     bx, sp                  ;old mode's sp

                dbg_switch 24, [bp+10], [bp+4]  ;for debugging only

                jmp     dword ptr [bp+10]       ;switch back to previous mode

old_mode:       pop     bp
                popf
                ret     12
DoRawSwitch_    ENDP

;on entry to PMProc_ :
;       no parameters
;on exit :
;       no returns
PMProc_         PROC NEAR
                push    ax

        ;currently, it looks as though issueing an interrupt here causes a
        ;crash, so we are just setting a flag to indicate we did make it here
                ;mov    ax, OFFSET DGROUP:procmsg
                ;call   cputs_                  ;this causes a crash
                ;call   getch_                  ;this causes a crash

                mov     _PMProcCalled, 0ffh

                pop     ax
                ret
PMProc_         ENDP

StoreDTs_       PROC NEAR
                push    bx

                sidt    fword ptr [bx]
                mov     bx, dx
                sldt    word ptr [bx]
                mov     bx, ax
                sgdt    fword ptr [bx]

                pop     bx
                ret
StoreDTs_       ENDP

;on entry to PM66Handler_ :
;       ax      holds a selector to our code
;       dx      holds a selector to our DGROUP
;       bx      holds selector of old int 66 handler
;       cx      holds high word of 32 bit offset of old int 66 handler
;       si      holds low word of 32 bit offset of old int 66 handler
;on exit :
;       no returns
PM66Handler_:
                test    bx, bx
                jz      no_32bit_ret
                push    0               ;push the 32 bit far address of
                push    bx              ;the old int 66 handler
                push    cx              ;...
                push    si              ;...
                jmp     say_hi

no_32bit_ret:   push    word ptr _OldPMHandler+2
                push    word ptr _OldPMHandler

say_hi:         push    ax
                push    ds
                mov     ds, dx
                mov     ax, OFFSET DGROUP:pmmsg
                call    cputs_
                pop     ds
                pop     ax

                ;mov    ax, OFFSET DGROUP:_GDT
                ;mov    dx, OFFSET DGROUP:_LDT
                ;mov    bx, OFFSET DGROUP:_IDT
                ;call   StoreDTs_

                test    bx, bx
                jz      retf16
                db      66h                     ;do a far 32 bit return to
retf16:         retf                            ;chain to old int 66 handler
_TEXT           ENDS

                END
