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
;* Description:  OS/2 (1.x) specific part of cmdedit.
;*
;*****************************************************************************


.286c

extrn           StringIn_       : near
extrn           DosGetInfoSeg   : far
extrn           InitRetrieve_   : near

fard segment para public 'far_data'
fard ends

_TEXT segment para public 'code'

_data segment para public 'data'
alias_      db "C:\ALIAS.OS2",0
index       dw 0
globalseg   dw 0
localseg    dw 0
pid         dw 0
want_alias  dw 0
arewein     dw 0
public      _AliasList
_AliasList  dw 0,0
_data ends

const segment para public 'data'
const ends

last segment para public 'last'
last ends

dgroup group _data,const

assume cs:_TEXT,ds:dgroup,ss:dgroup

public  CMDStart_
CMDStart_       proc    far
    push    si
    push    di
        push    es
        push    ds
        push    dx
        mov     dx,dgroup
        mov     ds,dx
        mov     ax,offset dgroup:alias_
        call    InitRetrieve_
    push    ds
    mov ax,offset globalseg
    push    ax
    push    ds
    mov ax,offset localseg
    push    ax
    call    DosGetInfoSeg
    mov es,localseg
    mov ax,es:[0]
    mov pid,ax
        mov     ax,1                    ; tell os2 things are ok.
        pop     dx
        pop     ds
        pop     es
    pop di
    pop si
        ret
CMDStart_       endp


public  OS2Edit_
OS2Edit_        proc    far
        enter   0,0
    push    di
    push    si
        push    es
        push    ds
        push    dx
        push    bx
        push    cx
        mov     dx,dgroup
        mov     ds,dx
    push    ds
    mov ax,offset globalseg
    push    ax
    push    ds
    mov ax,offset localseg
    push    ax
    call    DosGetInfoSeg
    mov es,localseg
    mov ax,es:[0]
    cmp pid,ax
    jne diffid
    mov want_alias,1
    jmp endid
diffid: mov want_alias,0
endid:  mov si,10[bp]
    mov index,si
    xor cx,cx
    cmp index,5
    jne not5
    ; KbdStringIn
        mov     ax,24[bp]
        mov     dx,26[bp]
    mov     bx,20[bp]
        mov     cx,22[bp]
    jmp theend
not5:   cmp index,0
    jne not0
    ; KbdCharIn
        mov     ax,20[bp]
        mov     dx,22[bp]
    jmp theend
    ; KbdPeek
not0:   mov     ax,18[bp]
        mov     dx,20[bp]
theend: push    index
        push    want_alias
        call    StringIn_
    mov ax,0
epi:    pop     cx
        pop     bx
        pop     dx
        pop     ds
        pop     es
    pop si
    pop di
        leave
        ret
OS2Edit_        endp
_TEXT ends

        end CMDStart_
