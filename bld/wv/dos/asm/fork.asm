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

                PUBLIC  _fork_
                EXTRN   DOSEnvFind_             :far
                EXTRN   __psp                   :word

                EXTRN   GrabHandlers_           :far
                EXTRN   RestoreHandlers_        :far

_text segment byte public 'CODE'
dgroup group const
assume cs:_text, ds:dgroup, es:dgroup, ss:dgroup


abort:          mov     dx,-1
                mov     ax,8
                jmp     return

_fork_          proc    far
                push    BP
                push    ES
                push    SI
                push    DI
                push    CX
                push    BX
                mov     SI,AX                   ; offset of cmd

                mov     cs:cmdseg,dx            ; save seg of cmd
                mov     dx,bx                   ; get length of cmd

                push    DS

                mov     BP,SP
                push    DX
                sub     SP,DX
                sub     SP,6
                mov     DI,SP
                call    RestoreHandlers_

                mov     BX,0FFFFH               ; This code handles the case
                mov     AH,48H                  ; of no memory since DOS can't
                int     21H                     ; figure this out itself.
                cmp     BX,0010H
                jbe     abort

                mov     CS:savess,SS
                mov     CS:savebp,BP
                mov     ax,seg __psp
                mov     ds,ax                   ; point ds at dgroup
                mov     AX,DS:__psp             ; get PSP segment into AX
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
                  mov   word ptr es:[DI],0dh*256+0
                  jmp   short cont
got_cmd:        add     CX,3
                mov     byte ptr es:0[DI],CL
                push    DX
                mov     AX,3700H                ; get switch character
                int     21H
                mov     byte ptr es:1[DI],DL
                pop     DX
                mov     byte ptr es:2[DI],'C'
                mov     byte ptr es:3[DI],' '
                sub     CX,3
                add     DI,4
                push    ds                      ; save current
                mov     ax,cs:cmdseg            ; set up with
                mov     ax,ds                   ;   segment of cmd
                rep     movsb
                pop     ds                      ; restore old ds
                mov     byte ptr es:[DI],0dh
cont:           sub     CX,CX
                push    CX
                ;; setup std in/out to console
                mov     AH,45H
                mov     BX,0
                int     21H
                mov     cs:savestdin,AX
                mov     AH,45H
                mov     BX,1
                int     21H

                mov     cs:savestdout,AX
                push    ds                              ; save current
                mov     ax,seg dgroup                   ; get segment
                mov     ds,ax                           ;   with ConName
                mov     AX,3d02H
                mov     DX,offset DGROUP:ConName
                int     21H
                pop     ds                              ; restore old ds

                jc      notopen
                mov     BX,AX
                mov     CX,0
                mov     AH,46H
                int     21H
                mov     CX,1
                mov     AH,46H
                int     21H
                mov     AH,3eH
                int     21H
notopen:
                mov     AX,offset DGROUP:CmdName
                mov     dx,seg DGROUP   ; segment for cmd name
                call    DOSEnvFind_
                mov     BX,SP
                push    SS
                pop     ES
                mov     DS,DX
                mov     DX,AX
                mov     AX,4b00H
                int     21H
                mov     SI,AX           ; save error code
                sbb     DX,DX
                mov     BP,CS:savebp
                mov     SS,CS:savess
                ;; restore std in/out
                mov     BX,cs:savestdin
                mov     CX,0
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
                mov     AX,SI       ; get error code back
return:         mov     SP,BP
                pop     DS
                push    AX          ; save return code
                call    GrabHandlers_
                pop     AX          ; restore return code
                pop     BX
                pop     CX
                pop     DI
                pop     SI
                pop     ES
                pop     BP
                ret
_fork_          endp

savestdin       dw      0
savestdout      dw      0
savess          dw      0
savebp          dw      0
cmdseg          dw      0

_text           ENDS

const segment byte public 'DATA'

CmdName         db "COMSPEC",0
ConName         db "CON",0

const           ENDS
                END
