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
;*** DLLPATCH.ASM - find entry point for DLL and patch into FunctionTable ***
;***                                                                      ***
;****************************************************************************
.386p
include struct.inc

extrn   _DataSelector:WORD
extrn   _SaveSP:DWORD
extrn   DLLHandles:WORD
extrn   DLLNames:WORD
extrn   LOADLIBRARY:FAR
extrn   GETPROCADDRESS:FAR
extrn   DLLLoadFail_:far

DGROUP group _DATA

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
_DATA ends

BUFSIZE equ     64

_TEXT segment use16
        assume cs:_TEXT
;
; This routine is called from one of the generated Thunks in winglue.asm.
; On entry:
;       bx - contains address of _FuntionTable entry
;       si - contains address of list of aliased pointers
;       stack contains parameters to desired Windows API function

        public  __DLLPatch
__DLLPatch proc far
        push    di                      ; save di
        sub     sp,BUFSIZE              ; allocate buffer
        mov     eax,_SaveSP             ; get 32-bit stack pointer
        mov     gs,_DataSelector        ; get 32-bit segment selector
        les     eax,gs:[eax]            ; get far return address
        cmp     byte ptr es:[eax],0ffh  ; if "call dword ptr" instruction
        _if     e                       ; then
          add   eax,6                   ; - skip over "call [__FLDENV]"
        _endif                          ; endif
        cmp     byte ptr es:[eax],0fh   ; if "movsx eax,ax" instruction
        _if     e                       ; then
          add   eax,3                   ; - skip over "movsx eax,ax"
        _endif                          ; endif
        cmp     byte ptr es:[eax],83h   ; if "add esp,nn" instruction
        _if     e                       ; then
          add   eax,3                   ; - skip over "add esp,nn"
        _endif                          ; endif
        ;; es:[eax] should now point to "pop esi"
        cmp     byte ptr es:3[eax],0c3h ; if "ret" instruction
        _if     e                       ; then
          add   eax,4                   ; - skip over epilog
        _else                           ; else
          add   eax,6                   ; - skip over epilog
        _endif                          ; endif
        mov     dl,es:[eax]             ; get DLL index
        inc     eax                     ; point to function name
        mov     di,sp                   ; point to buffer
        _loop                           ; loop
          mov   dh,es:[eax]             ; - get character
          mov   ss:[di],dh              ; - store on stack
          inc   eax                     ; - point to next char
          inc   di                      ; - ...
          cmp   dh,0                    ; - check for end of name
        _until  e                       ; until done
        shl     dx,1                    ; double DLL index
        mov     di,dx                   ; get DLL index
        mov     ax,DLLHandles[di]       ; get handle for DLL
        or      ax,ax                   ; if DLL not loaded yet
        _if     e                       ; then
          push  es                      ; - save es
          push  bx                      ; - save bx
          push  ds                      ; - point to DLL name
          push  word ptr DLLNames[di]   ; - ...
          call  LOADLIBRARY             ; - load DLL
          mov   DLLHandles[di],ax       ; - save handle for DLL
          pop   bx                      ; - restore bx
          pop   es                      ; - restore es
        _endif                          ; endif
        mov     dx,DLLNames[di]         ; point to DLL name
        cmp     ax,32                   ; check to see if load failed
        _if     b                       ; then
          mov   ax,dx                   ; - point to DLL name
          call  DLLLoadFail_            ; - issue message
          add   sp,BUFSIZE              ; - deallocate buffer
          pop   di                      ; - restore di
        _else                           ; else
          mov   dx,sp                   ; - point to function name
          push  bx                      ; - save bx
          push  ax                      ; - push handle
          push  ss                      ; - push address of function name
          push  dx                      ; - ...
          call  GETPROCADDRESS          ; - get address of procedure
          pop   bx                      ; - restore bx
          add   sp,BUFSIZE              ; - deallocate buffer
          pop   di                      ; - restore di
          mov   cx,ax                   ; - see if addr is NULL
          or    cx,dx                   ; - ...
          _if   ne                      ; - if not NULL
            mov   [bx],ax               ; - - back patch into _FunctionTable
            mov   2[bx],dx              ; - - ...
            push  dx                    ; - - push address of function
            push  ax                    ; - - ...
          _endif                        ; - endif
        _endif                          ; endif
        ret                             ; return to function

__DLLPatch endp

_TEXT   ends
        end
