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


                NAME    myhook
                EXTRN   _OldHook :BYTE
                EXTRN   _AsynchStop : BYTE
DGROUP          GROUP   CONST

CONST           SEGMENT WORD PUBLIC  'DATA'
                ORG     00000000H
ds_seg          LABEL   BYTE
CONST           ENDS


myhook_TEXT     SEGMENT BYTE PUBLIC  'CODE'
                ASSUME  CS:myhook_TEXT ,DS:DGROUP,SS:NOTHING

                PUBLIC  AsynchHook_
AsynchHook_:    push    ds                          ; save the regs
                push    dx
                push    es
                mov     dx,seg ds_seg               ; load ds
                mov     ds,dx
                cmp     ax,0063H                    ; is it asyc-stop
                je      short handle_asyc
                mov     dx,word ptr _OldHook        ; is someone else hooked
                test    dx,dx                       ; i.e. OldHook != NULL
                je      short return
                mov     dx,word ptr _OldHook+2H
                test    dx,dx
                je      short return
                pop     es                          ; restore all but ds
                pop     dx
                call    dword ptr _OldHook          ; call OldHook
                pop     ds                          ; restore ds and return
                retf
handle_asyc:    mov     _AsynchStop,1               ; enter debugger
return:         pop     es
                pop     dx
                pop     ds
                retf

myhook_TEXT     ENDS

                END
