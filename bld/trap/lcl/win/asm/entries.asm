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


                DOSSEG

extrn   TaskInit_:near
extrn   TaskAccess_:near
extrn   TaskFini_:near
extrn   InfoFunction_:near

_data segment word public 'DATA'
_HookRtn                dd       0
_HardModeRequired       dw      0
public  _HookRtn
public  _HardModeRequired
_data ends

dgroup group _data

_text segment word public 'CODE'

assume cs:_text, ds:dgroup

public          WinTaskInit, WinTaskAccess, WinTaskFini
public          WinInputHook, WinInfoFunction, WinHardModeCheck

; trap parm in          CX:BX
; error buffer in       DX:SI
; remote indictator in  DI


WinTaskInit     proc    far
                push    ds
                pop     ax
                nop
                inc     bp
                push    bp
                mov     bp,sp
                push    ds
                push    es              ; in case of old debugger
                mov     ax,DGROUP
                mov     ds,ax
                ; get parms setup
                push    di
                mov     ax,bx
                mov     bx,si
                xchg    dx,cx
                call    TaskInit_
                pop     es              ; in case of old debugger
                pop     ds
                pop     bp
                dec     bp
                ret
WinTaskInit     endp

; request in            DX
; infobuff in           CX:BX
; retbuff in            DI:SI

WinTaskAccess   proc    far
                push    ds
                pop     ax
                nop
                inc     bp
                push    bp
                mov     bp,sp
                push    ds
                push    es              ; in case of old debugger
                mov     ax,DGROUP
                mov     ds,ax
                ; get parms set up
                mov     ax,dx
                push    di
                push    si
                call    TaskAccess_
                pop     es              ; in case of old debugger
                pop     ds
                pop     bp
                dec     bp
                ret
WinTaskAccess   endp

WinTaskFini     proc    far
                push    ds
                pop     ax
                nop
                inc     bp
                push    bp
                mov     bp,sp
                push    ds
                push    es              ; in case of old debugger
                mov     ax,DGROUP
                mov     ds,ax
                ; get parms setup
                call    TaskFini_
                pop     es              ; in case of old debugger
                pop     ds
                pop     bp
                dec     bp
                ret
WinTaskFini      endp

WinInputHook    proc    far
                push    ds
                pop     ax
                nop
                inc     bp
                push    bp
                mov     bp,sp
                push    ds
                push    es              ; in case of old debugger
                mov     ax,DGROUP
                mov     ds,ax
                mov     word ptr ds:_HookRtn,bx
                mov     word ptr ds:_HookRtn+2,cx
                pop     es              ; in case of old debugger
                pop     ds
                pop     bp
                dec     bp
                ret
WinInputHook    endp

WinInfoFunction    proc    far
                push    ds
                pop     ax
                nop
                inc     bp
                push    bp
                mov     bp,sp
                push    ds
                push    es              ; in case of old debugger
                mov     ax,DGROUP
                mov     ds,ax
                call    InfoFunction_
                pop     es              ; in case of old debugger
                pop     ds
                pop     bp
                dec     bp
                ret
WinInfoFunction    endp

WinHardModeCheck    proc    far
                push    ds
                pop     ax
                nop
                push    ds
                mov     ax,DGROUP
                mov     ds,ax
                mov     ax, ds:_HardModeRequired
                pop     ds
                ret
WinHardModeCheck   endp

_text           ENDS

                END
