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


                NAME    FINDRET
_TEXT           SEGMENT PUBLIC BYTE 'CODE'
                ASSUME  CS:_TEXT
                PUBLIC  FINDRET_
FINDRET_:       push    bx
                push    ds
                push    si
                push    bp
                mov     bx,bp
                mov     bp,sp

;                lds     si, 8H[bp]       ;BIG_CODE
                mov     si, 8[bp]
                mov     ax,cs
                mov     ds,ax

L1:             dec     si
                cmp     byte ptr [si],0e5H
                jne     L1
                cmp     byte ptr -1H[si],89H
                jne     L1
                cmp     byte ptr -2H[si],55H
                jne     L1
                sub     si,0002H
                xor     ax,ax
L2:             inc     ax
                dec     si
                cmp     byte ptr [si],57H
                jg      L3
                cmp     byte ptr [si],50H
                jg      L2
                cmp     byte ptr [si],06H
                je      L2
L3:             shl     ax,1
                mov     bp,ax
                add     bp,bx

 ;               mov     dx,word ptr 2H[bp]      ;BIG_CODE
 ;               mov     ah,51H      ; get psp    BIG_CODE
 ;               int     21H                      ; BIG_CODE
 ;               add     bx,10H      ; get start address of program BIG_CODE
 ;               sub     dx,bx                    ;BIG_CODE
                mov     ax,word ptr [bp]    ; get offset
                pop     bp
                pop     si
                pop     ds
                pop     bx

                ret
_TEXT           ENDS

                END
