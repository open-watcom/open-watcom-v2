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
; Interface routine to call _far16 functions from 32-bit Flat OS/2 2.0
;

extrn           DOSFLATTOSEL    : near
extrn           DOSSELTOFLAT    : near

                name    far16fun

.386p

                assume  nothing

_TEXT16 segment use16 word public 'CODE'
_TEXT16 ends

_TEXT           segment use32 word public 'CODE'
                assume          cs:_TEXT


;=============================================================================
; - input       ESI = pointer to 16 bit parm list
;               ECX = size of parms
;               EAX = flat address of function to call
;               EBX = (optional) - points to space for struct return value
;=============================================================================
prol32          macro   n, p
                push    ebp                     ; save registers
                push    edi                     ; ...
                push    ebx
                push    edx
                push    es                      ; ...
                push    ds                      ; ...
                mov     ebp,esp                 ; get address of saved registers
                push    ss                      ; save 32-bit SS:ESP
                push    ebp                     ; ...
ifidn           <p>,<1>
                call    DOSFLATTOSEL            ; convert func addr to _far16
endif
                push    eax                     ; save address of function
                add     ecx,3                   ; parms size to multiple of 4
                and     cl,0FCh                 ; ...
                xor     ebp,ebp                 ; save parm size
                mov     bp,cx                   ; ...
                sub     esp,ecx                 ; allocate space for parms
                mov     edi,esp                 ; set up to copy parms down
                shr     ecx,1                   ; calc number of words
                rep     movsw                   ; copy the parms
                mov     eax,esp                 ; get stack pointer
                call    DOSFLATTOSEL            ; convert to _far16 format
                push    eax                     ; ...
                add     bp,ax                   ; ss:bp points to address of
                endm                            ; ... 16:16 function


;=============================================================================
;
;=============================================================================
jump16          macro   n, p
                lss     sp,dword ptr [esp]      ; load 16-bit SS:SP
                ifdef __WASM__
                jmp     far ptr l16&n&&p        ; jump to 16-bit code segment
ret32&n&&p      label   far
                else
                jmp     far ptr l16&n&p         ; jump to 16-bit code segment
ret32&n&p       label   far
                endif
                lss     esp,fword ptr 4[ebp]    ; load 32-bit SS:ESP
                endm


;=============================================================================
;
;=============================================================================
epi32           macro   n, p
                pop     ds                      ; restore registers
                pop     es                      ; ...
                pop     edx                     ; ...
                pop     ebx                     ; ...
                pop     edi                     ; ...
                pop     ebp                     ; ...
                endm


;=============================================================================
;
;=============================================================================
proc16          macro   n, p
ifdef __WASM__
p16&n&&p        proc    far
l16&n&&p        label   far
else
p16&n&p         proc    far
l16&n&p         label   far
endif
                mov     cx,ss                   ; set up 16-bit seg registers
                mov     ds,cx                   ; ...
                mov     es,cx                   ; ...
ifidn           <n>,<Pascal>
                push    ax
endif
                call    dword ptr [bp]          ; call 16:16 function
                ifdef __WASM__
                jmp     far ptr _TEXT:ret32&n&&p; jump back to 32-bit segment
p16&n&&p        endp
                else
                jmp     far ptr _TEXT:ret32&n&p ; jump back to 32-bit segment
p16&n&p         endp
                endif
                endm


;=============================================================================
;
;       __Far16Func2( EAX=func_addr, ESI=addr_of_parms, ECX=size_of_parms )
;
;=============================================================================
                public          __Far16Func2
__Far16Func2    proc near export                ; for scalar return value
                prol32  Func, 0                 ; set up the stack
                jump16  Func, 0                 ; jump to the 16 bit func
                rol     eax,16                  ; get DX:AX into EAX
                mov     ax,dx                   ; ...
                rol     eax,16                  ; ...
                epi32   Func, 0                 ; restore stack etc
                ret
__Far16Func2    endp


;=============================================================================
;
;       __Far16Func( EAX=func_addr, ESI=addr_of_parms, ECX=size_of_parms )
;
;=============================================================================
                public          __Far16Func
__Far16Func     proc near export                ; for scalar return value
                prol32  Func, 1                 ; set up the stack
                jump16  Func, 1                 ; jump to the 16 bit func
                rol     eax,16                  ; get DX:AX into EAX
                mov     ax,dx                   ; ...
                rol     eax,16                  ; ...
                epi32   Func, 1                 ; restore stack etc
                ret
__Far16Func     endp


;=============================================================================
;
;       __Far16Pascal2( EAX=func_addr, ESI=addr_of_parms,
;                      ECX=size_of_parms, EBX=pointer_to_struct_return_value )
;
;
;=============================================================================
                public          __Far16Pascal2
__Far16Pascal2  proc near export                ; for pascal style struct return
                prol32  Pascal, 0               ; set up the stack
                mov     eax,ebx                 ; convert the struct pointer
                call    DOSFLATTOSEL            ; ...
                jump16  Pascal, 0               ; jump to the 16 bit function
                epi32   Pascal, 0               ; restore stack etc
                ret
__Far16Pascal2  endp


;=============================================================================
;
;       __Far16Pascal( EAX=func_addr, ESI=addr_of_parms,
;                      ECX=size_of_parms, EBX=pointer_to_struct_return_value )
;
;
;=============================================================================
                public          __Far16Pascal
__Far16Pascal   proc near export                ; for pascal style struct return
                prol32  Pascal, 1               ; set up the stack
                mov     eax,ebx                 ; convert the struct pointer
                call    DOSFLATTOSEL            ; ...
                jump16  Pascal, 1               ; jump to the 16 bit function
                epi32   Pascal, 1               ; restore stack etc
                ret
__Far16Pascal   endp


;=============================================================================
;
;       __Far16CDecl2( EAX=func_addr, ESI=addr_of_parms, ECX=size_of_parms )
;
;       returns FLAT pointer to struct return value in EAX
;
;=============================================================================
                public          __Far16Cdecl2
__Far16Cdecl2   proc near export                ; for cdecl style struct return
                prol32  Cdecl, 0                ; set up the stack
                jump16  Cdecl, 0                ; jump to the 16 bit function
                rol     eax,16                  ; get DX:AX into EAX
                mov     ax,ds                   ; ...
                rol     eax,16                  ; ...
                epi32   Cdecl, 0                ; restore stack etc
                call    DOSSELTOFLAT            ; convert the struct pointer
                ret
__Far16Cdecl2   endp


;=============================================================================
;
;       __Far16CDecl( EAX=func_addr, ESI=addr_of_parms, ECX=size_of_parms )
;
;       returns FLAT pointer to struct return value in EAX
;
;=============================================================================
                public          __Far16Cdecl
__Far16Cdecl    proc near export                ; for cdecl style struct return
                prol32  Cdecl, 1                ; set up the stack
                jump16  Cdecl, 1                ; jump to the 16 bit function
                rol     eax,16                  ; get DX:AX into EAX
                mov     ax,ds                   ; ...
                rol     eax,16                  ; ...
                epi32   Cdecl, 1                ; restore stack etc
                call    DOSSELTOFLAT            ; convert the struct pointer
                ret
__Far16Cdecl    endp


;=============================================================================
;
;       __FlatToFar16   - convert 32 bit flat pointer in EAX to 16:16 in EAX
;
;=============================================================================
                public          __FlatToFar16
__FlatToFar16   proc near export
        or      eax,eax
        jnz     DOSFLATTOSEL
        ret
__FlatToFar16   endp


;=============================================================================
;
;       __Far16ToFlat   - convert 16:16 point in EAX to 32 bit flat
;
;=============================================================================
                public          __Far16ToFlat
__Far16ToFlat   proc near export
        or      eax,eax
        jnz     DOSSELTOFLAT
        ret
__Far16ToFlat   endp

_TEXT           ends


;=============================================================================
;
;       16 bit helper routines
;
;=============================================================================
_TEXT16 segment use16 word public 'CODE'
                proc16          Func, 0
                proc16          Cdecl, 0
                proc16          Pascal, 0
                proc16          Func, 1
                proc16          Cdecl, 1
                proc16          Pascal, 1
_TEXT16         ends


                end
