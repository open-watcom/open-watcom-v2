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
;* Description:  DOS specific code for cmdedit (TSR stuff etc.).
;*
;*****************************************************************************


extrn       SaveSave_   : far
extrn       StringIn_   : near
extrn       InitRetrieve_   : near
extrn       _edata      : byte      ; end of DATA (start of BSS)
extrn       _end        : byte      ; end of BSS (start of STACK)

FARD segment para public 'FAR_DATA'
FARD ends

_TEXT segment para public 'CODE'

_DATA segment para public 'DATA'
stk     db 512 dup(?)
top     db 0
l_in        db 0,0
l_out       db 0,0
alias_      db "\ALIAS.DOS",0
_DATA ends

CONST segment para public 'DATA'
CONST ends

STACK segment para stack 'STACK'
    db 512 dup (?)
STACK ends

DGROUP group _DATA,CONST,STACK

assume cs:_TEXT,ds:_TEXT,ss:_TEXT,es:_TEXT

dosedit     proc far
        jmp start

canfind     db  "Program is already loaded$"
cantfind    db  "Cannot locate previous version$"
savesp      dw 0
savess      dw 0
string_in   dw 5


SIGNATURE   equ 0CED1H

NEWSIGOFF   equ -14H
newsig      dw  SIGNATURE

SAVESAVEOFF equ -12H
savesave    dd  SaveSave_

MYDSOFF     equ -0EH
saveds      dw 0

ALIASOFF    equ -0CH
public      _AliasList
_AliasList  dw 0,0

INT21OFFOFF equ -08H
int21off    dw 0

INT21SEGOFF equ -06H
int21seg    dw 0

MYPSPOFF    equ -04H
mypsp       dw 0

SIGOFF      equ -02H
sig     dw  SIGNATURE

int21       proc    near
        cmp ah,0AH          ; is Buffered kbd input call?
        jne short old       ; no ... bop off to old 21 hdlr
        push    ax          ; save regs
        push    dx          ; ...
        push    bx          ; ...
        push    cx          ; ...
        xor bx,bx           ; handle is stdin
        mov ax,4400H        ; get handle information
        int 21H         ; ...
        test    dx,80H          ; see if this is console input
        pop cx          ; restore regs
        pop bx          ; ...
        pop dx          ; ...
        pop ax          ; ...
        je  short old       ; don't bother if not console
        sti             ; interrupts back on
        push    di          ; save some regs
        mov di,sp           ; point at parms
        push    bp
        push    es
        push    si          ; ...
        push    ax          ; ...
        push    bx          ; ...
        push    cx          ; ...
        push    dx          ; ...
        push    ds          ; ...
        push    ds          ; save pointer at buffer
        push    dx          ; ...
        mov ah,051H         ; - get psp of current process
        int 021H            ; - ...
;       mov ds,bx           ; - point at processes psp
;       cmp bx,ds:[02CH]        ; - if environment area < psp
        cmp     bx,cs:mypsp         ; - if running psp >= cmdedit psp
        jb  short else1     ; - - ...
        xor di,di           ; - - no aliases
        jmp short endif1        ; - else
else1:      mov di,1            ; - - do aliases
endif1:                     ; - endif
        pop si          ; restore buffer pointer
        pop ds          ; ...
        lea ax,2[si]        ; point at buffer + 2
        mov dx,ds           ; ...
        mov cl,[si]         ; set input length to stringin
        mov byte ptr cs:l_in,cl ; ...
        mov cx,DGROUP       ; point at length struct
        mov bx,offset DGROUP:l_in   ; ...
        push    si          ; save buffer pointer
        mov bp,sp           ; ...
        mov si,ss           ; ...
        mov ss,cx           ; ...
        mov sp,offset DGROUP:top    ; ...
        push    ds          ; save ds
        mov es,cs:string_in     ; push requested routine num
        push    es          ; ...
        mov ds,cx           ; point it at our data segment
        push    di          ; do aliasing?
        call    StringIn_       ; command line edit
        pop ds          ; reset ds
        mov ss,si           ; point to old stack
        mov sp,bp           ; ...
        pop si          ; restore buffer pointer
        mov cl,cs:l_out     ; set length gotten in buffer
        mov byte ptr 1[si],cl   ; put length gotten in buffer
done:       pop ds          ; restore some regs
        pop dx          ; restore some regs
        pop cx          ; ...
        pop bx          ; ...
        pop ax          ; ...
        pop si          ; ...
        pop es
        pop bp
        pop di          ; ...
        xor al,al           ; Why? I dunno
        iret
old:        jmp dword ptr cs:int21off
int21       endp

main        proc near
notin:      lea dx,cantfind
        jmp short putmsg
alreadyin:  lea dx,canfind
putmsg:
        push    cs
        pop ds
        mov ah,9
        int 21H
        jmp exit

start:
        mov dx,es           ; save pointer to PSP
        xor ax,ax           ; point to interrupt tables
        mov ds,ax           ; ...
        mov di,080H         ; ...
        mov cl,es:[di]      ; pick up parm length
        xor ch,ch           ; do
again:      inc di          ; - ++parm
        dec cx          ; - --length
        cmp byte ptr es:[di],' '    ; while( *parm == ' ' )
        je  short again     ; ...
        inc cx          ; ++length
        cmp byte ptr es:[di],'-'    ; if - option
        jne short install       ; - then
not_a:      cmp byte ptr es:1[di],'q'   ; - ...
        je  short kickout       ; - ...
        cmp byte ptr es:1[di],'Q'   ; - ...
        jne short install
kickout:
        cli             ; interrupt off for a second
        mov dx,es           ; save the command line
        mov ax,di           ; ...
        les bx,ds:[84h]     ; pick up old int21 hdlr
        push    es          ; - ...
        cmp word ptr es:SIGOFF[bx],SIGNATURE; if already in memory
        jne short notin     ; - then
        mov di,es:INT21OFFOFF[bx]   ; - restore int21
        mov ds:[84h],di     ; - ...
        mov di,es:INT21SEGOFF[bx]   ; - restore int21
        mov ds:[86h],di     ; - ...
        cmp word ptr es:NEWSIGOFF[bx],SIGNATURE; if not old style
        jne short oldstyle      ; - - then
        mov ds,es:MYDSOFF[bx]   ; - - set up ds
        mov si,ss           ; - - save stack
        mov di,sp           ; - - ...
        mov ss,es:MYDSOFF[bx]   ; - - set new stack
        mov sp,offset DGROUP:top    ; - - ...
        call    dword ptr es:SAVESAVEOFF[bx]; save saved commands
        mov ss,si           ; - - restore stack
        mov sp,di           ; - - ...
oldstyle:   sti
        pop ds          ; - ...
        cmp word ptr ds:ALIASOFF+2[bx],0; if there are aliases
        je  short noalias       ; - - then
        mov es,ds:ALIASOFF+2[bx]    ; - - free up aliases
        mov ah,49H          ; - -
        int 21H         ; - - ...
noalias:    mov es,ds:MYPSPOFF[bx]  ; - free up code
        mov ah,49H          ; - prepare to free up memory
        int 21H         ; - ...
exit:                       ; - endif
        mov ax,4C00H        ; - ...
        int 21H         ; - ...
                        ; endif
install:
        cli             ; interrupt off for a second
        les bx,ds:[84h]     ; pick up old int21 hdlr
        cmp word ptr es:SIGOFF[bx],SIGNATURE; if already in memory
        jne notalready      ; - then
        jmp alreadyin       ; - then
notalready:
        mov cs:int21off,bx      ; save old handler
        mov cs:int21seg,es      ; ...
        mov word ptr ds:[84h],offset _TEXT:int21; set new handler
        mov ds:[86h],cs     ; ...
        sti             ; interrupts back on
        mov cs:mypsp,dx     ; save our PSP
        mov es,dx           ; restore pointer to PSP
        mov bx,di           ; get pointer to end of parm
        add bx,cx           ; ...
        mov byte ptr es:[bx],0  ; store a null character
        mov bx,DGROUP       ; set up stack
        mov cs:saveds,bx
        mov ss,bx           ; ...
        mov sp,offset DGROUP:top    ; ...
        ;
        ; initialize bss to 0
        ;
            push    di
        push    ax
        push    es
        mov ax,DGROUP
        mov es,ax
        mov cx,offset DGROUP:_end
        mov di,offset DGROUP:_edata
        sub cx,di
        xor al,al
        rep stosb
        pop es
        pop ax
        pop di

        mov ds,bx           ; point ds to PSP

        mov bx,offset DGROUP:_end   ; calculate memory size needed
        mov cl,4            ; ...
        shr bx,cl           ; ...
        add bx,DGROUP       ; ...
        mov cx,es           ; ...
        sub bx,cx           ; ...
        mov cl,4            ; ...
        inc bx          ; ...
        mov ah,4AH          ; setblock
        int 21H         ; ...

        mov ax,di           ; ...
        call    InitRetrieve_       ; initialize

        mov ax,3100H        ; terminate and stay resident
        mov dx,bx           ; ...
        int 21H         ; ...

main        endp

dosedit     endp

_TEXT ends

end dosedit
