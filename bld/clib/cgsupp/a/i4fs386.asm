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


;========================================================================
;==     Name:           I4FS, U4FS                                     ==
;==     Operation:      Convert Integer types to single precision      ==
;==     Inputs:         EAX,   unsigned or signed integer              ==
;==     Outputs:        EAX   single precision floating point          ==
;==     Volatile:       none                                           ==
;==                                                                    ==
;========================================================================
include mdef.inc
include struct.inc

        modstart        i4fs386

        xdefp   __I4FS
        xdefp   __U4FS


        defpe   __U4FS
        or      EAX,EAX         ; if number is not zero
        _if     ne              ; then
          push  ECX             ; - save ECX
          bsr   ECX,EAX         ; - find most significant non-zero bit
          mov   CH,CL           ; - save shift count
          neg   CL              ; - calculate # of bits to rotate by
          add   CL,23           ; - ...
          and   CL,1fh          ; - just keep last 5 bits
          rol   EAX,CL          ; - shift bits into position
          and   EAX,007FFFFFh   ; - mask out sign and exponent bits
          mov   CL,CH           ; - get shift count
          add   CL,7Fh          ; - calculate exponent
          and   ECX,0FFh        ; - isolate exponent
          shl   ECX,23          ; - shift exponent into position
          or    EAX,ECX         ; - place into result
          pop   ECX             ; - restore ECX
        _endif                  ; endif
        ret                     ; return

        endproc __U4FS

        defpe   __I4FS
        or      EAX,EAX         ; check sign
        jns     __U4FS          ; if positive, just convert
        neg     EAX             ; take absolute value of number
        call    __U4FS          ; convert to FS
        or      EAX,80000000h   ; set sign bit on
        ret                     ; and return
        endproc __I4FS

        endmod
        end
