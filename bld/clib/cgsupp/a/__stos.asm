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


;
;  __STOSB( char *EAX, int EDX, unsigned ECX );
;  __STOSD( char *EAX, int EDX, unsigned ECX );
;
include mdef.inc
include struct.inc

        modstart __stos,para

        xdefp   __STOSB
        xdefp   __STOSD


        defpe   __STOSB
        or      ECX,ECX                 ; if length not zero
        _if     ne                      ; then
          cmp   [EAX],DL                ; - access cache line
          _loop                         ; - loop (get onto 4-byte boundary)
            test  AL,3                  ; - - quit if on 4-byte boundary
            _quif e                     ; - - ...
            mov   [EAX],DL              ; - - store byte
            inc   EAX                   ; - - increment address
            ror   EDX,8                 ; - - (DH can be different from DL)
            dec   ECX                   ; - - decrement length
          _until e                      ; - until done
          push  ECX                     ; - save length
          shr   ECX,2                   ; - calculate number of dwords
          call  __STOSD                 ; - store dwords
          pop   ECX                     ; - restore length
          _guess                        ; - guess: some bytes left to do
            and   ECX,3                 ; - - calculate number of bytes left
            _quif e                     ; - - quit if done
            mov   [EAX],DL              ; - - store a byte
            dec   ECX                   ; - - decrement length
            _quif e                     ; - - quit if done
            mov   1[EAX],DH             ; - - store a byte
            dec   ECX                   ; - - decrement length
            _quif e                     ; - - quit if done
            mov   2[EAX],DL             ; - - store a byte
          _endguess                     ; - endguess
        _endif                          ; endif
        ret                             ; return
        endproc __STOSB

        nop
        nop
        nop
        nop
        nop
        nop

        defpe   __STOSD
        or      ECX,ECX                 ; if length not zero
        _if     ne                      ; then
          _loop                         ; - loop (get onto 32-byte boundary)
            test  AL,1Fh                ; - - quit if on 32-byte boundary
            _quif e                     ; - - ...
            mov   [EAX],EDX             ; - - store dword
            lea   EAX,4[EAX]            ; - - increment address
            dec   ECX                   ; - - decrement length
          _until e                      ; - until done
          push  ECX                     ; - save length
          shr   ECX,2                   ; - calculate number of paragraphs
          _if   ne                      ; - if at least one paragraph
           dec    ECX                   ; - - decrement # of paragraphs
           _if    ne                    ; - - if still have paragraphs to do
            _loop                       ; - - - loop
             mov  [EAX],EDX             ; - - - - store dword
             mov  4[EAX],EDX            ; - - - - ...
             dec  ECX                   ; - - - - decrement count
             mov  8[EAX],EDX            ; - - - - store dword
             mov  12[EAX],EDX           ; - - - - ...
             je   short add16           ; - - - - quit if done
             cmp  32[EAX],DL            ; - - - - access next cache line
             mov  16[EAX],EDX           ; - - - - store dword
             mov  20[EAX],EDX           ; - - - - ...
             dec  ECX                   ; - - - - decrement count
             mov  24[EAX],EDX           ; - - - - store dword
             mov  28[EAX],EDX           ; - - - - ...
             lea  EAX,32[EAX]           ; - - - - advance to next 32-byte chunk
            _until  e                   ; - - - until done
            lea  EAX,-16[EAX]           ; - - - back up 16
add16:      lea  EAX,16[EAX]            ; - - - advance 16
           _endif                       ; - - endif
           mov  [EAX],EDX               ; - - fill last full paragraph
           mov  4[EAX],EDX              ; - - ...
           mov  8[EAX],EDX              ; - - ...
           mov  12[EAX],EDX             ; - - ...
           lea  EAX,16[EAX]             ; - - advance pointer
          _endif                        ; - endif
          pop   ECX                     ; - restore length
          _guess                        ; - guess: some dwords left to do
           and  ECX,3                   ; - - calculate number of dwords left
           _quif e                      ; - - quit if done
           mov  [EAX],EDX               ; - - store dword
           lea  EAX,4[EAX]              ; - - advance pointer
           dec  ECX                     ; - - decrement length
           _quif e                      ; - - quit if done
           mov  [EAX],EDX               ; - - store dword
           lea  EAX,4[EAX]              ; - - advance pointer
           dec  ECX                     ; - - decrement length
           _quif e                      ; - - quit if done
           mov  [EAX],EDX               ; - - store dword
           lea  EAX,4[EAX]              ; - - advance pointer
          _endguess                     ; - endguess
        _endif                          ; endif
        ret                             ; return
        endproc __STOSD

        endmod
        end
