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


        name    TIMASM

_TEXT   segment byte public 'CODE'

        ASSUME  cs:_TEXT

;
; DOSLoadProg -- load the DOS program without executing it
;
        public  DOSLoadProg_
DOSLoadProg_    proc    near
        push    es
        push    bx

        push    ss
        pop     es

        mov     bx,dx           ; get parmblock in BX
        mov     dx,ax           ; get file name in DX
        mov     ax,4b01h        ; load program, maintain control
        int     21h             ; (internal DOS call)

        pop     BX

        call    GetPSP_         ; get the PSP
        mov     es,ax
        mov     es:0ah,bx       ; set terminate address
        mov     es:0ch,cs

        pop     es
        ret
DOSLoadProg_    endp


;
; DOSRunProg -- run the loaded program
;
        public  DOSRunProg_
DOSRunProg_     proc    far
        mov     bx,ax
        mov     ss,2[bx]
        mov     sp,0[bx]
        push    6[bx]
        push    4[bx]
        call    GetPSP_
        mov     ds,ax
        mov     es,ax
        ret
DOSRunProg_     endp

;
; GetPSP -- return PSP segment
;
        public  GetPSP_
GetPSP_ proc    near
        pushf                   ; save flags
        push    bx              ; save BX
        mov     ah,51h          ; get PSP request (internal DOS call)
        int     21h             ; get the PSP
        mov     ax,bx           ; put PSP in proper return register
        pop     bx              ; restore BX
        popf                    ; restore flags
        ret                     ; return
GetPSP_ endp

_TEXT   ends

        end
