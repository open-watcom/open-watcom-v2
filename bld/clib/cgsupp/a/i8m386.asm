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
;==     Name:           I8M,U8M                                        ==
;==     Operation:      integer eight byte multiply                    ==
;==     Inputs:         EDX;EAX   integer M1                           ==
;==                     ECX;EBX   integer M2                           ==
;==     Outputs:        EDX;EAX   product                              ==
;==     Volatile:       ECX, EBX destroyed                             ==
;========================================================================
include mdef.inc
include struct.inc

        modstart        i8m

        xdefp   __I8M
        xdefp   __U8M

        defpe   __I8M
        defpe   __U8M

        test    edx,edx         ; first check for easy (hiwords == 0) case
        jnz     L1
        test    ecx,ecx
        jnz     L1
        mul     ebx
        ret

L1:     push    eax             ; save M1.l
        push    edx             ; save M1.h
        mul     ecx             ; calc M1.l * M2.h -> eax
        mov     ecx,eax         ; save M1.l * M2.h in ecx
        pop     eax             ; get  M1.h in eax
        mul     ebx             ; calc M1.h * M2.l -> eax
        add     ecx,eax         ; add  above to previous total
        pop     eax             ; get  M1.l in eax
        mul     ebx             ; calc M1.l * M2.l -> edx:eax
        add     edx,ecx         ; add previous hiword contribs to hiword
        ret                     ; and return!!!

        endproc __U8M
        endproc __I8M

        endmod
        end
