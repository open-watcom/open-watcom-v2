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
;  char *__COPY( char *dst, char *src, unsigned len );
;
;
include mdef.inc

        name    __copy
_TEXT   segment use32 para public 'CODE'
        assume  cs:_TEXT

        public  __COPY
        public  __COPY_

__COPY  proc    near
__COPY_ proc    near
        push    edi                     ; save regs
        push    esi                     ; ...
        push    ecx                     ; ...
        mov     edi,16+0[esp]           ; get dst
        mov     esi,16+4[esp]           ; get src
        mov     ecx,16+8[esp]           ; get len
        mov     eax,edi                 ; return with dst ptr in eax
        shr     ecx,2                   ; calc. # of dwords
        rep     movsd                   ; do the copy
        mov     ecx,16+8[esp]           ; get len
        and     ecx,3                   ; plus extra bytes
        rep     movsb                   ; do the copy
        pop     ecx                     ; restore regs
        pop     esi                     ; ...
        pop     edi                     ; ...
        ret     12                      ; return and remove parms
__COPY_ endp
__COPY  endp

_TEXT   ends
        end
