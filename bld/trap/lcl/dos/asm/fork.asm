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


                NAME    FORK

                PUBLIC  Fork_
                EXTRN   DOSEnvFind_             :near
                EXTRN   DbgPSP_                 :near

_text segment byte public 'CODE'
assume cs:_text


abort:          mov     dx,-1
                mov     ax,8
                jmp     return

Fork_           proc    near
                push    BP
                push    DS
                push    ES
                push    SI
                push    DI
                push    CX
                mov     BP,SP
                push    BX
                sub     SP,BX
                sub     SP,6
                mov     DI,SP
                mov     DS,DX
                mov     SI,AX

                mov     BX,0FFFFH               ; This code handles the case
                mov     AH,48H                  ; of no memory since DOS can't
                int     21H                     ; figure this out itself.
                cmp     BX,0010H
                jbe     abort

                mov     CS:savess,SS
                mov     CS:savebp,BP
                call    DbgPSP_
                push    AX
                mov     CX,006cH
                push    CX
                push    AX
                mov     CX,005cH
                push    CX
                push    SS
                push    DI
                push    SS
                pop     ES
                mov     CX,-2[BP]
                test    CX,CX
                jne     got_cmd
                  mov   word ptr SS:[DI],0dh*256+0
                  jmp   short cont
got_cmd:        add     CX,3
                mov     byte ptr SS:0[DI],CL
                push    DX
                mov     AX,3700H                ; get switch character
                int     21H
                mov     byte ptr SS:1[DI],DL
                pop     DX
                mov     byte ptr SS:2[DI],'C'
                mov     byte ptr SS:3[DI],' '
                sub     CX,3
                add     DI,4
                rep     movsb
                mov     byte ptr SS:[DI],0dh
cont:           sub     CX,CX
                push    CX
                ;; setup std in/out to console
                mov     AH,45H
                xor     BX,BX
                int     21H
                mov     cs:savestdin,AX
                mov     AH,45H
                mov     BX,1
                int     21H
                mov     cs:savestdout,AX
                mov     AX,3d02H
                mov     DX,offset _text:ConName
                push    cs
                pop     ds
                int     21H
                jc      notopen
                mov     BX,AX
                xor     CX,CX
                mov     AH,46H
                int     21H
                mov     CX,1
                mov     AH,46H
                int     21H
                mov     AH,3eH
                int     21H
notopen:
                mov     BX,SP
                mov     AX,offset _text:CmdName
                mov     DX,CS
                call    DOSEnvFind_
                push    SS
                pop     ES
                mov     DS,DX
                mov     DX,AX
                mov     AX,4b00H
                int     21H
                sbb     DX,DX
                jne     worked
                xor     AX,AX
worked:
                mov     SI,AX           ; save error code
                mov     BP,CS:savebp
                mov     SS,CS:savess
                ;; restore std in/out
                mov     BX,cs:savestdin
                xor     CX,CX
                mov     AH,46H
                int     21H
                mov     AH,3eH
                int     21H
                mov     BX,cs:savestdout
                mov     CX,1
                mov     AH,46H
                int     21H
                mov     AH,3eH
                int     21H
                mov     AX,SI           ; restore error code
return:         mov     SP,BP
                pop     CX
                pop     DI
                pop     SI
                pop     ES
                pop     DS
                pop     BP
                ret
Fork_           endp

savess          dw      0
savebp          dw      0
savestdin       dw      0
savestdout      dw      0
CmdName         db "COMSPEC",0
ConName         db "CON",0

_text           ENDS

                END
