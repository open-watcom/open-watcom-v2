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


;****************************************************************************
;***                                                                      ***
;*** INT21.ASM - trap int 21 calls                                        ***
;***                                                                      ***
;****************************************************************************
.386

extrn ___ConvId:DWORD

DGROUP group _DATA
_DATA segment word public 'DATA' use16
_DATA ends

extrn _NameList:WORD
extrn _NameCount:WORD
extrn _RealNameStrLen:WORD
extrn _GetRealNamePrefix:BYTE
extrn _TmpBuff:BYTE

_TEXT segment word public 'CODE' use16
assume cs:_TEXT

PUBLIC _OldInt21
_OldInt21       dd      0
save_ptr        dw      0
dat             db      0

SendUp proc near
        pusha
        push    ds
        push    es
        mov     ax,ds
        mov     es,ax                           ; data ptr (dx has offset)
        mov     si,cx                           ; len
        mov     di,1                            ; BLOCK
        mov     ax, SEG DGROUP
        mov     ds,ax
        mov     cx,word ptr ds:[___ConvId+2]    ; conv id number
        mov     bx,word ptr ds:[___ConvId]
        mov     ax,0fa17h                       ; ConvPut
        int     02fh
        pop     es
        pop     ds
        popa
        ret
SendUp endp

ReceiveBack proc near
        pusha
        push    ds
        push    es
        mov     ax,ds
        mov     es,ax                           ; data ptr (dx has offset)
        mov     si,cx                           ; len
        mov     di,1                            ; BLOCK
        mov     ax, SEG DGROUP
        mov     ds,ax
        mov     cx,word ptr ds:[___ConvId+2]    ; conv id number
        mov     bx,word ptr ds:[___ConvId]
        mov     ax,0fa16h                       ; get
        int     02fh
        pop     es
        pop     ds
        popa
        ret
ReceiveBack endp

PUBLIC  TrapInt21_
TrapInt21_ proc far

        pushf
        cmp     ah,40h                  ; check for output request
        je      short req40h
        cmp     ah,09h                  ; output string request
        je      short req09h
        cmp     ah,02h                  ; output char
        je      short req02h
        cmp     ah,06h                  ; direct input/output
        je      short req06h
        cmp     ah,3dh                  ; open?
        je      req3dh
        popf
        jmp     cs:[_OldInt21]          ; go to old handler
;*
;*** req02h,06h - dl contains char to print
;*
req06h:
        cmp     dl,0ffh                 ; input request?
        jne     req02h                  ; nope, print char
        popf
        jmp     cs:[_OldInt21]          ; go to old handler
req02h:
        push    cx
        push    dx
        push    ds
        mov     cx,1                    ; number of bytes
        mov     cs:[dat],dl             ; data to print
        lea     dx,dat                  ; offset of data
        push    cs
        pop     ds                      ; segment of data
        call    SendUp
        pop     ds
        pop     dx
        pop     cx
        popf
        jmp     cs:[_OldInt21]          ; go to old handler

;*
;*** req09h - ds:dx points to '$' terminated string
;*
req09h:
        push    cx
        push    bx
        mov     bx,dx
again:
        cmp     byte ptr ds:[bx],'$'
        je      done
        inc     bx
        jmp     short again
done:
        sub     bx,dx
        mov     cx,bx
        call    SendUp
        pop     bx
        pop     cx
        popf
        jmp     cs:[_OldInt21]
;*
;*** req40h - ds:dx points to data, cx contains length
;*
req40h:
        push    ax
        push    dx
        mov     ax,4400h                ; get i/o control info
        int     21H
        and     dl,0a2h                 ; check for console output
        cmp     dl,082h
        je      short send_up
        pop     dx
        pop     ax
        popf
        jmp     cs:[_OldInt21]
send_up:
        pop     dx
        pop     ax
        call    SendUp
        popf
        jmp     cs:[_OldInt21]
;*
;*** req3dh - change open file name
;*
req3dh:
        push    ax
        and     al,03h
        cmp     al,0                    ; open read request?
        je      short yes_read          ; yes
bug_out:
        pop     ax
        popf
        jmp     cs:[_OldInt21]          ; go to old handler
yes_read:
        push    es
        mov     ax,SEG DGROUP
        mov     es,ax
        cmp     es:_NameCount,0         ; any name?
        pop     es
        je      short bug_out           ; nope
        pusha
        push    es
        mov     ax,SEG DGROUP           ; local data
        mov     es,ax
        mov     bx,dx

        mov     cx,es:_NameCount
        mov     di,es:_NameList

        ;*
        ;*** check for matching name ( case insensitive)
        ;*
loop_main:
        mov     si,es:[di]              ; pointer to name
loop2:
        mov     al,es:[si]              ; char from name to change
        mov     ah,ds:[bx]              ; char from name to open
        cmp     ah,'A'
        jb      nocc
        cmp     ah,'Z'
        ja      nocc
        sub     ah,'A'
        add     ah,'a'
nocc:
        cmp     al,ah                   ; do they match?
        jne     short try_next          ; no, try next name
        cmp     al,0                    ; at end of string?
        je      short get_new_name      ; yes, get the new name
        inc     bx
        inc     si
        jmp     short loop2             ; try next char

try_next:
        add     di,2                    ; next ptr
        dec     cx                      ; done?
        jne     loop_main               ; nope
        jmp     short next_int21        ; yes, then we are done


        ;*
        ;*** try to get new name from above
        ;*
get_new_name:
        inc     si                      ; point to new name
        mov     cs:[save_ptr],si        ; save pointer to new name
        cmp     byte ptr es:[si],'?'    ; do we need to inquire for name?
        jne     short noenquire
        mov     si,es:[di]              ; get pointer at original string
        lea     di,_GetRealNamePrefix
        add     di,es:_RealNameStrLen
cont:
        mov     al,es:[si]
        mov     es:[di],al
        inc     si
        inc     di
        cmp     al,0
        jne     cont

        lea     dx,_GetRealNamePrefix
        mov     cx,di
        sub     cx,OFFSET _GetRealNamePrefix ; number of bytes
        push    ds
        push    es
        pop     ds
        call    SendUp                  ; send request for real name
        lea     dx,_TmpBuff
        mov     cs:[save_ptr],dx        ; save the pointer
        mov     cx,144                  ; number of bytes
        call    ReceiveBack             ; get real name
        pop     ds

        ;*
        ;*** reset DS:DX to point at new name and re-issue request
        ;*
noenquire:
        pop     es                      ; restore registers
        popa
        pop     ax
        popf
        push    ds                      ; save original DS:DX
        push    dx
        push    SEG DGROUP              ; point DS:DX at new name
        pop     ds
        mov     dx,cs:[save_ptr]
        pushf
        call    cs:[_OldInt21]          ; call old handler
        pop     dx                      ; restore original pointer to name
        pop     ds
        retf    2                       ; go back, maintain flags after call
next_int21:
        pop     es                      ; restore registers
        popa
        pop     ax
        popf
        jmp     cs:[_OldInt21]          ; go to old handler
TrapInt21_ endp
_TEXT ends
        end
