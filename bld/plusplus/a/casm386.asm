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


;
; CASMSUPP:     C front-end assembler coded support
;
; - these routines are easier to code in assembler than in C
;

        name    casm386

        public  InitHandlers_
        public  FiniHandlers_
        public  TBreak_


BREAK_INT       =       1bh
CRITICAL_INT    =       24h
DOS_SET_INT     =       25h
DOS_GET_INT     =       35h


; must be para because local CS data is accessed
rm_code   segment para public 'CODE' use16
                assume  cs:rm_code

BreakCount      dw      0
SaveBreakOff    dw      0
SaveBreakSeg    dw      0
SaveCriticalOff dw      0
SaveCriticalSeg dw      0

BreakKeyInterrupt       proc    far

        sti                     ; increment BreakCount
        inc     CS:BreakCount   ; sti, cli are necessary for atomic increment
        cli                     ;  operation
        iret

BreakKeyInterrupt       endp


CriticalErrorInterrupt  proc    far

        sub     al,al           ; ignore critical error
        iret

CriticalErrorInterrupt  endp


TBreak_ proc    far

        sub     ax,ax
        xchg    ax,cs:BreakCount
        ret

TBreak_ endp


_init_rtn proc  far
        push    ax
        push    bx
        push    dx
        push    es
        push    ds
        push    di
        mov     di,cs
        add     di,8
        mov     ds,di
        mov     ds:BreakCount,0 ; BreakCount = 0
        mov     ah,DOS_GET_INT  ; get break key handler
        mov     al,BREAK_INT
        int     21h             ; returns in es:bx
        mov     ds:SaveBreakOff,bx ; save in SaveBreak
        mov     ds:SaveBreakSeg,es
        mov     dx,offset BreakKeyInterrupt
        mov     ax,cs
        mov     ds,ax
        mov     ah,DOS_SET_INT  ; set break key handler
        mov     al,BREAK_INT    ; (might by modified by INT 21h)
        int     21h
        mov     ah,DOS_GET_INT  ; get critical error handler
        mov     al,CRITICAL_INT
        int     21h             ; returns in es:bx
        mov     ds,di
        mov     ds:SaveCriticalOff,bx ; save in SaveCritical
        mov     ds:SaveCriticalSeg,es
        mov     dx,offset CriticalErrorInterrupt
        mov     ax,cs
        mov     ds,ax
        mov     ah,DOS_SET_INT  ; set break key handler
        mov     al,CRITICAL_INT ; (might by modified by INT 21h)
        int     21h
        pop     di
        pop     ds
        pop     es
        pop     dx
        pop     bx
        pop     ax
        ret
_init_end:

_init_rtn   endp


_fini_rtn proc  far
        push    ax
        push    dx
        push    ds
        mov     dx,cs:SaveBreakOff
        mov     ds,cs:SaveBreakSeg
        mov     ah,DOS_SET_INT  ; set break key handler
        mov     al,BREAK_INT
        int     21h
        mov     dx,cs:SaveCriticalOff
        mov     ds,cs:SaveCriticalSeg
        mov     ah,DOS_SET_INT  ; set critical error handler
        mov     al,CRITICAL_INT
        int     21h
        pop     ds
        pop     dx
        pop     ax
        ret
        public  _fini_end
_fini_end:

_fini_rtn endp

rm_code ends


pm_code   segment para public 'CODE' use32
                assume  cs:pm_code

InitHandlers_   proc  near
        push    ebx
        push    ecx
;        lea     ebx,_init_rtn
;        lea     ecx,_init_end
;       call    rmcall
        lea     ebx,CriticalErrorInterrupt
        mov     ecx,20
        mov     ax,cs
        mov     es,ax
        mov     ax,250fh
        int     21h
        mov     ebx,ecx
        mov     cl,24h
        mov     ax,2505h
        int     21h
        pop     ecx
        pop     ebx
        ret
InitHandlers_   endp

FiniHandlers_   proc  near
        ret

        push    ebx
        push    ecx
        lea     ebx,_fini_rtn
        lea     ecx,_fini_end
        call    rmcall
        pop     ecx
        pop     ebx
        ret
FiniHandlers_   endp


rmcall  proc    near
        push    eax
        push    ebx
        push    ecx
        push    edx
        push    es
        push    ds
        mov     edx,ebx         ; save address
        sub     ecx,ebx         ; calc length
        mov     ax,ds
        mov     es,ax
        mov     ax,250fh
        int     21h
        mov     ebx,ecx
        sub     ecx,ecx         ; no parms on stack
        mov     ax,250eh        ; call real-mode rtn
        int     21h
        pop     ds
        pop     es
        pop     edx
        pop     ecx
        pop     ebx
        pop     eax
        ret
rmcall  endp

pm_code ends

end
