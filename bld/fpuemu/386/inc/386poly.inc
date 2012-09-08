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


        modstart  386poly

        xref    ___LDA          ; add
        xref    ___LDM          ; multiply

        xdefp   __EvalPoly      ; calc polynomial
        xdefp   __OddPoly       ; calc polynomial
        xdefp   __poly          ; evaluate polynomial


;;void _EvalPoly( long double *x, long double *poly, int degree )
;;/*************************************************************/
;;    {
;;        double  z;
;;
;;        z = *poly++;
;;        do {
;;            z = z * x + *poly++;
;;        } while( --degree != 0 );

        defp    __EvalPoly
        push    ESI                     ; save registers
        push    ECX                     ; ...
        push    EAX                     ; save address of x
        mov     ECX,EDX                 ; get address of polynomial
        mov     SI,8[EAX]               ; load value of x
        mov     EDX,4[EAX]              ; ...
        mov     EAX,[EAX]               ; ...
        call    __poly                  ; evaluate polynomial
        pop     ECX                     ; restore address of x
        mov     [ECX],EAX               ; store z in x
        mov     4[ECX],EDX              ; ...
        mov     8[ECX],SI               ; ...
        pop     ECX                     ; restore registers
        pop     ESI                     ; ...
        ret                             ; return
__EvalPoly endp


;  input:
;       long double in SI:EDX:EAX
;       pointer to polynomial in CS:ECX
;       degree of polynomial in EBX
;  output:
;       long double in SI:EDX:EAX

        defp    __poly
        push    EBP                     ; save EBP
        push    EDI                     ; save registers
        push    ECX                     ; ...
        push    EBX                     ; push the degree
        push    ESI                     ; push x
        push    EDX                     ; ...
        push    EAX                     ; ...
        mov     EBP,ESP                 ; get access to locals

        mov     EDI,ECX                 ; save address of polynomial
        mov     EAX,CS:[EDI]            ; load first element of polynomial
        mov     EDX,CS:4[EDI]           ; ... into z
        mov     SI,CS:8[EDI]            ; ...
        _loop                           ; loop
          rol   ESI,16                  ; - load exponent of x
          mov   SI,8[EBP]               ; - ...
          rol   ESI,16                  ; - ...
          mov   EBX,[EBP]               ; - load x
          mov   ECX,4[EBP]              ; - ...
          call  ___LDM                  ; - calc. z * x
          lea   EDI,10[EDI]             ; - point to next coefficient
          mov   EBX,CS:[EDI]            ; - load *poly
          mov   ECX,CS:4[EDI]           ; - ...
          rol   ESI,16                  ; - ...
          mov   SI,CS:8[EDI]            ; - ...
          rol   ESI,16                  ; - ...
          call  ___LDA                  ; - z * x + *poly
          dec   dword ptr 12[EBP]       ; - --degree
        _until  e                       ; until done

;;        return( z );
;;    }
        add     ESP,16                  ; pop the stack
        pop     ECX                     ; restore registers
        pop     EDI                     ; ...
        pop     EBP                     ; ...
        ret                             ; return
__poly  endp

;;
;;void _OddPoly( long double *x, long double *poly, int degree )
;;/************************************************************/
;;    {
;;        return( _EvalPoly( x * x, poly, degree ) * x );
;;    }

        defp    __OddPoly
        push    EDI                     ; save registers
        push    ESI                     ; ...
        push    ECX                     ; ...
        push    EBX                     ; save the degree
        push    EDX                     ; save address of polynomial
        mov     EDI,EAX                 ; save address of x
        mov     EAX,[EDI]               ; load x
        mov     EDX,4[EDI]              ; ...
        mov     SI,8[EDI]               ; ...
        shl     ESI,16                  ; load x again
        mov     SI,8[EDI]               ; ...
        mov     EBX,EAX                 ; ...
        mov     ECX,EDX                 ; ...
        call    ___LDM                  ; calc. x * x
        pop     ECX                     ; restore address of polynomial
        pop     EBX                     ; restore degree
        call    __poly                  ; evaluate polynomial
        rol     ESI,16                  ; load x
        mov     EBX,[EDI]               ; ...
        mov     ECX,4[EDI]              ; ...
        mov     SI,8[EDI]               ; ...
        rol     ESI,16                  ; ...
        call    ___LDM                  ; multiply result by x
        mov     [EDI],EAX               ; store result
        mov     4[EDI],EDX              ; ...
        mov     8[EDI],SI               ; ...
        pop     ECX                     ; restore ECX
        pop     ESI                     ; restore ESI
        pop     EDI                     ; restore EDI
        ret                             ; return
__OddPoly endp

