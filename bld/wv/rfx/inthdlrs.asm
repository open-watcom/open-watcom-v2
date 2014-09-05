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


        name    inthdlrs
_TEXT   segment word public 'CODE'
        assume  cs:_TEXT

i23off          dw      0               ; ctrl-C vector
i23seg          dw      0
i24off          dw      0               ; critical error vector
i24seg          dw      0
CtrlCHit        dw      0

        public  InitInt_
InitInt_ proc   far
        push    es                      ; save work registers
        push    ds                      ; ...
        push    dx                      ; ...
        push    bx                      ; ...
        push    ax                      ; ...
        mov     dx,cs                   ; point ds at code segment
        mov     ds,dx                   ; ...
        mov     ax,3523H                ; - get old <CTRL>C handler
        int     21H                     ; - ...
        mov     i23off,bx               ; - save it
        mov     i23seg,es               ; - ...
        mov     dx,offset CtrlCHdlr     ; - set up new <CTRL>C handler
        mov     ax,2523H                ; - ...
        int     21H                     ; - ...
        mov     ax,3524H                ; get old critical error handler
        int     21H                     ; ...
        mov     i24off,bx               ; save it
        mov     i24seg,es               ; ...
        mov     dx,offset CErrHdlr      ; set up new critical error handler
        mov     ax,2524H                ; ...
        int     21H                     ; ...
        pop     ax                      ; restore work regs
        pop     bx                      ; ...
        pop     dx                      ; ...
        pop     ds                      ; ...
        pop     es                      ; ...
        ret                             ; return to caller
InitInt_ endp

        public  FiniInt_
FiniInt_ proc far
        push    es                      ; save work registers
        push    ds                      ; ...
        push    dx                      ; ...
        push    bx                      ; ...
        push    ax                      ; ...
        mov     dx,cs                   ; point ds at code segment
        mov     ds,dx                   ; ...
        mov     dx,i23off               ; get saved CTRL C vector
        mov     ds,i23seg               ; ...
        mov     ax,2523H                ; - restore CTRL C handler
        int     21H                     ; ...
        mov     dx,cs:i24off            ; restore critical error handler
        mov     ds,cs:i24seg            ; ...
        mov     ax,2524H                ; ...
        int     21H                     ; ...
        pop     ax                      ; restore work regs
        pop     bx                      ; ...
        pop     dx                      ; ...
        pop     ds                      ; ...
        pop     es                      ; ...
        ret                             ; return to caller
FiniInt_ endp

CtrlCHdlr proc far
        mov     word ptr cs:CtrlCHit,-1
        iret
CtrlCHdlr endp

CErrHdlr proc far
        add     sp,6            ; remove ip,cs,flags from int $24
        pop     ax              ; restore ax
        mov     ax,di           ; get error code
        xor     ah,ah           ; zero high byte
        add     al,23           ; adjust error number
        pop     bx              ; restore bx
        pop     cx              ; restore cx
        pop     dx              ; restore dx
        pop     si              ; restore si
        pop     di              ; restore di
        mov     bp,sp           ; get access to stack
        or      byte ptr 10[bp],1; set carry to indicate error
        pop     bp              ; restore bp
        pop     ds              ; restore ds
        pop     es              ; restore es
        iret                    ; return to interrupted program
CErrHdlr endp

        public  CtrlCHit_
CtrlCHit_ proc far
        mov     ah,0BH          ; check input status
        int     21H             ; (to clear dos CTRL-C flag)
        sub     ax,ax           ;
        xchg    ax,cs:CtrlCHit  ; return break flag
        ret
CtrlCHit_ endp

_TEXT   ends

        end
