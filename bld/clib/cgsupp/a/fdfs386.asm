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


;=========================================================================
;==     Name:           FDFS                                            ==
;==     Operation:      Float double to float single conversion         ==
;==     Inputs:         EDX:EAX         double precision float          ==
;==     Outputs:        EAX             single precision float          ==
;==     Volatile:       EDX destroyed                                   ==
;=========================================================================
include mdef.inc
include struct.inc

        modstart        fdfs386

        xdefp   __FDFS

        defpe   __FDFS
        push    EBX             ; save EBX
        test    EDX,07ff00000h  ; check exponent
        je      short retzero   ; if exponent = 0 then just return 0
        sub     EBX,EBX         ; set to 0
        _shl    EAX,1           ; shift number over
        _rcl    EDX,1           ; ...
        rcr     EBX,1           ; save sign
        add     EAX,20000000h   ; round floating point number
        adc     EDX,0           ; ...
        je      oflow           ; overflow if exponent went to 0
        cmp     EDX,(03ffh+80h) shl 21 ; check for maximum exponent
        jae     oflow           ; overflow if above or equal
        cmp     EDX,(03ffh-7eh) shl 21 ; check for minimum exponent
        jb      uflow           ; underflow if below
        sub     EDX,(03ffh-7fh) shl 21 ; correct bias
        _shl    EAX,1           ; do rest of shift
        _rcl    EDX,1           ; ...
        _shl    EAX,1           ; ...
        _rcl    EDX,1           ; ...
        or      EDX,EBX         ; put in sign bit
        mov     EAX,EDX         ; get result into EAX
        pop     EBX             ; restore EBX
        ret                     ; return

oflow:  mov     EAX,7F800000h   ; return maximum possible number
        or      EAX,EBX         ; put in sign bit
        pop     EBX             ; restore EBX
        ret                     ; and return

uflow:
retzero:sub     EAX,EAX         ; set result to 0
        pop     EBX             ; restore EBX
        ret
        endproc __FDFS

        endmod
        end
