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


        name    cmain32
.386p
include struct.inc

        extrn   main_   : near
        extrn   exit_   : near
        extrn   __Envseg: word
        extrn   __Envptr: dword

_TEXT   segment use32 dword public 'CODE'
        assume  cs:_TEXT

        public  __CMain
__CMain proc    near
        mov     bx,ds                   ; save DS
        mov     esi,__Envptr            ; point to environment strings
        mov     ecx,esi                 ; save starting address
        mov     ds,__Envseg             ; ...
        _loop                           ; loop (for each string)
          _loop                         ; - loop (find end of string)
            lodsb                       ; - - get byte
            cmp   al,0                  ; - - check for end
          _until  e                     ; - until end of string
          lodsb                         ; - get start of next string
          cmp   al,0                    ; - check for end of strings
        _until  e                       ; until all done
        sub     esi,ecx                 ; calculate length
        add     esi,3                   ; round up to multiple of 4
        and     esi,not 3               ; ...
        sub     esp,esi                 ; allocate space from stack
        mov     edi,esp                 ; point to allocated space
        mov     ax,ss                   ; set ES
        mov     es,ax                   ; ... to point to stack
        xchg    ecx,esi                 ; ecx=length, esi=start of env
        rep     movsb                   ; copy it
        mov     ds,bx                   ; restore ds
        mov     es,bx                   ; and es
        mov     __Envptr,esp            ; save pointer to environment strings
        call    main_                   ; call main
        call    exit_                   ; and exit
__CMain endp

_TEXT   ends
        end
