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
;* Description:  WIN386 floating point function to save/restore WIN386 
;*                  floating point environment before/after calling 
;*                  16-bit Windows function
;*
;*****************************************************************************


;****************************************************************************
;***                                                                      ***
;*** FLDENV.ASM - save/restore floating-point environment around thunks   ***
;***                                                                      ***
;****************************************************************************
.386p
.387
include struct.inc

DGROUP group _DATA
_TEXT segment dword public 'CODE' use32
_TEXT ends

_DATA segment dword public 'DATA' use32
        public  __FLDENV
        public  __FSTENV
__FLDENV dd     ___RestoreInfo
__FSTENV dd     ___SaveInfo
_DATA ends

_TEXT segment use32
        assume cs:_TEXT
        assume ds:dgroup

        public  _FLOATINGPOINT

_FLOATINGPOINT proc near
        mov     __FSTENV,offset ___FSTENV ; redirect ptr to save routine
        mov     __FLDENV,offset ___FLDENV ; redirect ptr to restore routine
        ret                               ; return
_FLOATINGPOINT endp

___FSTENV proc  near
        pop     EBX                     ; get return address
        sub     ESP,28                  ; allocate space for environment
        fstenv  [ESP]                   ; save the environment
        push    FS                      ; save FS
        finit                           ; re-initialize FPU
        jmp     EBX                     ; return
___FSTENV endp

___FLDENV proc  near
        push    edx                     ; save edx
        mov     dx,8[ESP]               ; get value for FS
        verr    dx                      ; verify segment
        _if     ne                      ; if invalid
          sub   edx,edx                 ; - set to 0
        _endif                          ; endif
        mov     fs,dx                   ; reload FS
        pop     edx                     ; restore edx
        fldenv  8[ESP]                  ; reload environment
        fwait                           ; wait
        ret     28+4                    ; return and remove environment
___FLDENV endp

___SaveInfo proc near
        pop     EBX                     ; get return address
        push    FS                      ; save FS
        jmp     EBX                     ; return
___SaveInfo endp

___RestoreInfo proc near
        push    edx                     ; save edx
        mov     dx,8[ESP]               ; get value for FS
        verr    dx                      ; verify segment
        _if     ne                      ; if invalid
          sub   edx,edx                 ; - set to 0
        _endif                          ; endif
        mov     fs,dx                   ; reload FS
        pop     edx                     ; restore edx
        ret     4                       ; return and remove FS
___RestoreInfo endp

_TEXT   ends
        end
