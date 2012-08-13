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



NBROWS          equ     25
NBCOLS          equ     80
VIDEO_BUFF_SIZE equ     NBROWS * NBCOLS * 2
VIDEO_BUFF      equ     0b800h


if DEBUG

keywait         MACRO
                local   getchar
                push    ax
                push    dx

getchar:        mov     dl, 0ffh
                mov     ah, 7
                int     21h
                jz      getchar

                pop     dx
                pop     ax
                ENDM

setpage         MACRO   pagenb
                push    ax
                push    bp

                mov     ah, 5
                mov     al, pagenb
                int     10h

                pop     bp
                pop     ax
                ENDM

fillpage        MACRO   char
                push    ax
                push    cx
                push    di
                push    es

                mov     ax, VIDEO_BUFF
                mov     es, ax
                mov     ah, es:1
                mov     al, char
                mov     cx, VIDEO_BUFF_SIZE
                xor     di, di
                cld
                rep     stosw

                pop     es
                pop     di
                pop     cx
                pop     ax
                ENDM

write4          MACRO   row, col, value
                local   less_than
                push    ax
                push    bx
                push    es

                mov     al, value
                cmp     al, 10
                jl      less_than
                sub     al, 10
                add     al, 'A'-'0'
less_than:      add     al, '0'

                mov     bx, VIDEO_BUFF
                mov     es, bx
                mov     byte ptr es:[((row)*NBCOLS+(col))*2], al

                pop     es
                pop     bx
                pop     ax
                ENDM

write8          MACRO   row, col, value
                push    ax
                mov     al, value
                shr     ax, 1
                shr     ax, 1
                shr     ax, 1
                shr     ax, 1
                and     al, 0fh
                write4  row, col, al
                pop     ax

                push    ax
                mov     al, value
                and     al, 0fh
                write4  row, col+1, al
                pop     ax
                ENDM

write16         MACRO   row, col, value
                push    ax

                mov     ax, value
                write8  row, col, ah
                write8  row, col+2, al

                pop     ax
                ENDM

write32         MACRO   row, col, memory
                push    ax

                mov     ax, word ptr memory+2
                write16 row, col, ax
                mov     ax, word ptr memory
                write16 row, col+4, ax

                pop     ax
                ENDM

write48         MACRO   row, col, memory
                push    ax

                mov     ax, word ptr memory+4
                write16 row, col, ax
                write32 row, col+4, memory

                pop     ax
                ENDM

writestr        MACRO   row, col, string
                local   str
                local   nextchar
                local   done
                local   start

                jmp     start
str             db string, 0

start:          push    es
                push    ds
                push    di
                push    si
                push    ax

                mov     ax, VIDEO_BUFF
                mov     es, ax
                mov     di, (row*NBCOLS+col)*2
                mov     ax, cs
                mov     ds, ax
                mov     si, OFFSET cs:str

nextchar:       cmp     byte ptr ds:[si], 0
                je      done
                mov     al, ds:[si]
                mov     es:[di], al
                inc     di
                inc     di
                inc     si
                jmp     nextchar

done:           pop     ax
                pop     si
                pop     di
                pop     ds
                pop     es
                ENDM

else

keywait         MACRO
                ENDM

setpage         MACRO   pagenb
                ENDM

fillpage        MACRO   char
                ENDM

write4          MACRO   row, col, value
                ENDM

write8          MACRO   row, col, value
                ENDM

write16         MACRO   row, col, value
                ENDM

write32         MACRO   row, col, memory
                ENDM

write48         MACRO   row, col, memory
                ENDM

writestr        MACRO   row, col, str
                ENDM

endif
