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
;* Description:  DOS memory management routines for 32-bit DOS.
;*
;*****************************************************************************


;
;
include mdef.inc
include struct.inc

        xref    __doserror_
        modstart dosmem

        defp    _dos_allocmem
        xdefp   "C",_dos_allocmem
;
;       unsigned _dos_allocmem( unsigned size, unsigned *segment );
;
        push    EBX             ; save EBX
        push    EDX             ; save EDX
ifdef __STACK__
        mov     EAX,12[ESP]     ; get size
        mov     EDX,16[ESP]     ; get pointer to segment
        push    EDX             ; workaround for EDX getting trashed
                                ; under DOS/4GW on NT
endif
        mov     EBX,EAX         ; get # of paragraphs wanted
        mov     AH,48h          ; allocate memory
        int     21h             ; ...
        _if     nc              ; if no error
          mov   EBX,EAX         ; - get segment of allocated memory
          sub   EAX,EAX         ; - indicate no error
        _else                   ; else
          call  __doserror_     ; - set error code
        _endif                  ; endif
        pop     EDX             ; restore EDX
        mov     [EDX],EBX       ; store size of largest block or segment
ifdef __STACK__
        pop     EDX             ; restore EDX
endif
        pop     EBX             ; restore EBX
        ret                     ; return to caller
        endproc _dos_allocmem

        defp    _dos_freemem
        xdefp   "C",_dos_freemem
;
;       unsigned _dos_freemem( unsigned segment );
;
ifdef __STACK__
        mov     EAX,4[ESP]      ; get segment
endif
        push    ES              ; save ES
        mov     ES,EAX          ; get segment to be freed
        mov     AH,49h          ; free allocated memory
        int     21h             ; ...
        call    __doserror_     ; set return code
        pop     ES              ; restore ES
        ret                     ; return to caller
        endproc _dos_freemem

        defp    _dos_setblock
        xdefp   "C",_dos_setblock
;
;       unsigned _dos_setblock( unsigned size,
;                               unsigned segment, unsigned *maxsize );
;
;
ifdef __STACK__
        push    EDX
        push    EBX
        mov     EAX,12[ESP]
        mov     EDX,16[ESP]
        mov     EBX,20[ESP]
endif
        push    ES              ; save ES
        push    EBX             ; save pointer to maxsize
        mov     ES,EDX          ; get segment to be modified
        mov     EBX,EAX         ; get new size
        mov     AH,4Ah          ; modify allocated memory
        int     21h             ; ...
        mov     EDX,EBX         ; get maximum block size if failure
        pop     EBX             ; restore pointer to maxsize
        _if     c               ; if error
          mov   [EBX],EDX       ; - store the maxsize
        _endif                  ; endif
        call    __doserror_     ; set return code
        pop     ES              ; restore ES
ifdef __STACK__
        pop     EBX
        pop     EDX
endif
        ret                     ; return to caller
        endproc _dos_setblock

        endmod
        end
