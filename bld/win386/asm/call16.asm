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
;*** CALL16.ASM - call a 16-bit PASCAL function                           ***
;***    DWORD Call16( FARPROC lpFunc, char *fmt, ... );                   ***
;***                                                                      ***
;****************************************************************************
.386p
include struct.inc

extrn   DPMIGetAliases_:near
extrn   DPMIFreeAlias_:near
extrn   _SaveSP:DWORD           ; save for stack
extrn   _EntryStackSave:DWORD   ; save for stack
extrn   _DataSelector:WORD      ; selector obtained for 32-bit area
extrn   _StackSelector:WORD     ; selector obtained for 32-bit stack

DGROUP group _DATA
;*
;*** 16-bit segment declarations
;*
_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
_DATA ends

_TEXT segment use16
        assume cs:_TEXT
        assume ds:dgroup

;
; DWORD __Call16( FARPROC lpFunc, char *fmt, ... );
;       edi - points to parms on 32-bit stack
;
;  Format specifiers:
;       c - 'cdecl' function as opposed to 'pascal' function (must be first)
;       b - unsigned byte
;       w - 16-bit word
;       d - 32-bit dword
;       f - double precision floating-point
;       p - 32-bit flat pointer
;
_funcptr equ    -4
_endchar equ    -6
_one    equ     -10
_four   equ     -14

        public  __Call16_
__Call16_ proc  far
        mov     ecx,esp                 ; save 32-bit sp
        mov     _SaveSP,ecx             ; ...
        lss     sp,_EntryStackSave      ; switch to 16-bit stack
        push    ecx                     ; save 32-bit stack pointer
        push    ecx                     ; save 32-bit stack pointer
        push    bp                      ; save bp
        mov     bp,sp                   ; get access to 16-bit stack
        push    dword ptr es:[edi]      ; push address of 16-bit func to call
        mov     esi,es:4[edi]           ; get pointer to format string
        add     edi,8                   ; advance past first 2 parms
        push    0                       ; push terminating character
        mov     edx,1                   ; set increment value
        mov     al,es:[esi]             ; get first char from format
        cmp     al,'c'                  ; if "cdecl" function
        _if     e                       ; then
          _loop                         ; - loop (skip to end of list)
            inc   esi                   ; - - advance to next format char
            mov   al,es:[esi]           ; - - get next format character
            cmp   al,0                  ; - - quit if end of format
            _quif e                     ; - - ...
            add   edi,4                 ; - - skip over parm
            cmp   al,'f'                ; - - if "float" (8-byte parm)
            _if   e                     ; - - then
              add   edi,4               ; - - - skip another 4 bytes
            _endif                      ; - - endif
          _endloop                      ; - endloop
          dec   esi                     ; - back up to last char
          sub   edi,4                   ; - back up to last parm
          pop   ax                      ; - remove 0
          mov   al,'c'                  ; - set terminating character
          push  ax                      ; - ...
          neg   edx                     ; - set increment value to -1
        _endif                          ; endif
        push    edx                     ; push +1 or -1
        shl     edx,2                   ; multiply by 4
        push    edx                     ; push +4 or -4
nxtparm:;_loop                          ; loop
          mov   al,es:[esi]             ; - get next format character
          add   esi,_one[bp]            ; - advance to next character
          mov   edx,es:[edi]            ; - get next dword from 32-bit stack
          cmp   al,_endchar[bp]         ; - quit if end of string
          je    short docall            ; - ...
          add   edi,_four[bp]           ; - advance parm pointer
          cmp   al,'b'                  ; - check for byte parm
          je    short byteparm          ; - ...
          cmp   al,'w'                  ; - check for word parm
          je    short wordparm          ; - ...
          cmp   al,'f'                  ; - check for float parm
          je    short floatparm         ; - ...
          cmp   al,'p'                  ; - check for pointer parm
          je    short pointerparm       ; - ...
;         assume 'd' for "dword" parm
longparm: push  edx                     ; - push long parm
          jmp   nxtparm                 ; - go get next parm

byteparm: mov   dh,0                    ; - zero high byte
wordparm: push  dx                      ; - push word parm
          jmp   nxtparm                 ; - go get next parm
floatparm:
          push  dword ptr es:[edi]      ; - push high word of double parm
          push  dword ptr es:-4[edi]    ; - push low word of double parm
          add   edi,_four[bp]           ; - advance parm pointer
          jmp   nxtparm                 ; - go get next parm

pointerparm:
        _guess                          ; guess: alias not required
          sub   ecx,8                   ; - allocate space from 32-bit stack
          mov   _SaveSP,ecx             ; - update 32-bit stack pointer
          mov   2[bp],ecx               ; - ...
          mov   es:[ecx],edx            ; - save original pointer
          mov   eax,edx                 ; - get value
          test  eax,eax                 ; - check for NULL pointer
          _quif e                       ; - quit if NULL pointer
          shr   edx,16                  ; - set dx:ax == pointer
          movzx eax,ax                  ; - zero extend low word
          cmp   dx,0FFFFh               ; - quit if magical MAKEINTRESOURCE
          _quif e                       ; - ... or MAKEINTATOM value
          push  ss                      ; - set es=ss
          pop   es                      ; - ...
          push  si                      ; - save si
          sub   sp,4                    ; - allocate space for aliased pointer
          mov   si,sp                   ; - point es:si at allocated space
          mov   cx,1                    ; - want 1 alias
          call  DPMIGetAliases_         ; - get alias
          mov   es, _DataSelector       ; - reload es
          pop   eax                     ; - load alias pointer
          pop   si                      ; - restore si
          mov   ecx,_SaveSP             ; - get 32-bit stack pointer
        _endguess                       ; endguess
        mov     es:4[ecx],eax           ; save aliased pointer
        push    eax                     ; push aliased pointer
        jmp     nxtparm                 ; go get next parm

docall: call    dword ptr _funcptr[bp]  ; invoke 16-bit function

        mov     sp,bp                   ; clean up stack
        pop     bp                      ; restore bp
        pop     esi                     ; get modified 32-bit stack pointer
        pop     ecx                     ; restore original 32-bit stack pointer
        push    dx                      ; save return value on 16-bit stack
        push    ax                      ; ...
        mov     es,_StackSelector       ; get 32-bit stack selector
        _loop                           ; loop
          cmp   esi,ecx                 ; - quit if all aliases freed
          _quif e                       ; - ...
          mov   eax,es:4[esi]           ; - get aliased pointer
          mov   edx,es:[esi]            ; - get original pointer
          cmp   eax,edx                 ; - if they are different
          _if   ne                      ; - then
            shr   eax,16                ; - - get selector
            call  DPMIFreeAlias_        ; - - free it
          _endif                        ; - endif
          add   esi,8                   ; - advance 32-bit stack pointer
        _endloop                        ; endloop
        pop     eax                     ; load return value
        mov     word ptr _EntryStackSave,SP     ; save current sp
        mov     bx,_DataSelector        ; load 32-bit data segment
        mov     ss,_StackSelector       ; switch back to 32-bit stack
        mov     esp,ecx                 ; ...
        mov     ds,bx                   ; reload ds
        mov     es,bx                   ; and es
        db      66h                     ; far return to 32-bit code
        ret                             ; ...
__Call16_ endp

_TEXT   ends
        end
