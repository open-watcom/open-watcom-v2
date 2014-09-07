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
;*** WINEXT.ASM - windows extender                                        ***
;***                                                                      ***
;****************************************************************************
.386p

DGROUP group _DATA

;*
;*** externals we need
;*
extrn   _SaveSP:DWORD
extrn   _DataSelector:WORD
extrn   _StackSelector:WORD
extrn   _EntryStackSave:DWORD
extrn   _ReturnCode:DWORD
extrn   _StackSize:DWORD
extrn   _EDataAddr:DWORD
extrn   _CodeEntry:FWORD

_DATA segment word public 'DATA' use16

PUBLIC  _InDebugger
_InDebugger     dw 0

_DATA ends

;*
;*** the windows extender code lies here
;*
_TEXT segment word public 'CODE' use16
assume cs:_TEXT

;****************************************************************************
;***                                                                      ***
;*** InvokeWin32 - invoke a 32-bit application                            ***
;***                                                                      ***
;***       The 32 bit application must save gs, since that is our pointer ***
;***       to our data segment (given to us by windows)                   ***
;***                                                                      ***
;***       Our program image is as follows:                               ***
;***                                                                      ***
;***            0 -> send          Stack                                  ***
;***            send+1 -> cend     Code                                   ***
;***            cend+1 -> ì        Data                                   ***
;***                                                                      ***
;****************************************************************************
assume ds:DGROUP
assume ss:DGROUP
assume es:nothing
public InvokeWin32
InvokeWin32 proc near
        push    bp                      ; save bp
        mov     bp,sp                   ; point to stack
        push    si                      ; save si
        push    di                      ; save di
;*
;*** prepare to invoke the application: save stack, load selectors, and call
;*
        mov     word ptr [_EntryStackSave],sp   ; save current sp
        mov     word ptr [_EntryStackSave+2],ss ; save stack segment
        mov     edx,_SaveSP             ; load up 32-bit stack pointer
        mov     ecx,edx                 ; calc value for __STACKLOW
        sub     ecx,_StackSize          ; ...
        add     ecx,256                 ; ...
        push    ds                      ; load up gs
        pop     gs                      ;     with ds
        mov     ss,_StackSelector       ; switch to 32-bit stack
        mov     esp,edx                 ; ...
        push    _EntryStackSave         ; save 16-bit SS:SP
        assume  ss:nothing              ;
        mov     edi,_EDataAddr          ; get end of code+data
        mov     ax,_DataSelector        ; load data selector
        mov     ds,ax                   ; get 32 bit data selector
        assume  ds:nothing
        mov     es,ax                   ; give it to es
        xor     bl,bl                   ; indicate not a DLL
        cmp     gs:_InDebugger,1        ; debugging?
        jne     short docall            ; no, skip breakpoint
        mov     ax,word ptr gs:_CodeEntry+4     ; load CS for sampler
        int     3                       ; yes, break
docall:
        call    gs:[_CodeEntry]         ; invoke the 32 bit appl
;*
;*** restore data segment and stack (ds is restored before return)
;*
        assume  ds:DGROUP
        lss     sp,[esp]                ; switch back to 16-bit stack
        mov     _ReturnCode,eax         ; get return code
        xor     ax,ax                   ; get rid of any
        mov     es,ax                   ;   potential guys
        mov     fs,ax                   ;     containing our
        mov     gs,ax                   ;       32-bit selector

        pop     di                      ; restore di
        pop     si                      ; restore si
        pop     bp                      ; restore bp
        ret                             ; return

InvokeWin32 endp

_TEXT   ends
        end
