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


;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
;<>
;<> __FSC compares EAX with EDX
;<>       if EAX > EDX,  1 is returned in EAX
;<>       if EAX = EDX,  0 is returned in EAX
;<>       if EAX < EDX, -1 is returned in EAX
;<>
;<>  =========    ===           =======
;<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
include mdef.inc
include struct.inc

        modstart        fsc386

        xdefp   __FSC

        defpe   __FSC
        _guess    have_cmp      ; guess - comparison done
          xor     EAX,EDX       ; - check if signs differ
          _if     ns            ; - if signs are the same
            xor     EAX,EDX     ; - - restore EAX
            sub     EAX,EDX     ; - - find difference, EAX=0 if equal
            _quif   e,have_cmp  ; - - done if mantissas also equal
                                ; - - carry=1 iff |EAX| < |EDX|
            rcr     EAX,1       ; - - EAX sign set iff |EAX| < |EDX|
            xor     EDX,EAX     ; - - EDX sign set iff EAX < EDX
            not     EDX         ; - - EDX sign set iff EAX > EDX
          _endif                ; - endif
          sub     EAX,EAX       ; - clear result
          _shl    EDX,1         ; - carry=1 iff EAX > EDX
          adc     EAX,EAX       ; - EAX = 1 iff EAX > EDX, else EAX = 0
          _shl    EAX,1         ; - EAX = 2 iff EAX > EDX, else EAX = 0
          dec     EAX           ; - EAX = 1 iff EAX > EDX, else EAX = -1
        _endguess               ; endguess
        ret                     ; return with sign in EAX, conditions set
        endproc __FSC

        endmod
        end
