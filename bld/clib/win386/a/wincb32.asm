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
;*** WINCB32.ASM - callback handling for 32 bit windows                   ***
;***                                                                      ***
;****************************************************************************
.386p

DGROUP group _DATA

_TEXT segment word public 'CODE'
_TEXT ends

_DATA segment word public 'DATA'
_DATA ends

;*
;*** callback data defined here
;*
_DATA segment

extrn _CBJumpTable : dword

_DATA ends

_TEXT segment
assume cs:_TEXT
;****************************************************************************
;***                                                                      ***
;*** __32BitCallBack - invoke a specified call back function.             ***
;***                                                                      ***
;***   Incoming registers : DX - callback routine number                  ***
;***   Outgoing registers : DL - amount to pop from stack                 ***
;***                                                                      ***
;****************************************************************************

        public  __32BitCallBack
__32BitCallBack proc    far
        push    ESI                     ; save ESI
        push    EDI                     ; save EDI
        push    ECX                     ; save ecx
        push    EBX                     ; save ebx
        movzx   ECX,BP                  ; get access to 16-bit stack
        mov     DI,DS                   ; save 16-bit DS
        mov     DS,AX                   ; get 16-bit stack selector
        movzx   EDX,DX                  ; zero high part
        mov     EDX,ES:_CBJumpTable[EDX]; get address of callback rtn
        call    EDX                     ; call the generated callback
        mov     DS,DI                   ; restore 16-bit DS
        pop     EBX                     ; restore EBX
        pop     ECX                     ; restore ECX
        pop     EDI                     ; restore EDI
        pop     ESI                     ; restore ESI
        ret                             ; return
__32BitCallBack endp

_TEXT   ends
        end
