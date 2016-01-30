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


;****************************************************************************
;***                                                                      ***
;*** WIN16THK.ASM - windows thunk functions                               ***
;***    This set of functions encompasses all possible types of calls.    ***
;***    Each API call has a little stub which generates the appropriate   ***
;***    call into these functions.                                        ***
;***                                                                      ***
;****************************************************************************
.386p

extrn   DPMIGetAliases_:near
extrn   DPMIFreeAlias_:near
extrn   _SaveSP:DWORD           ; save for stack
extrn   _EntryStackSave:DWORD   ; save for stack
extrn   _DataSelector:WORD      ; selector obtained for 32-bit area
extrn   _StackSelector:WORD     ; selector obtained for 32-bit stack
extrn   _FunctionTable:DWORD    ; API function address table

DGROUP group _DATA
;*
;*** 16-bit segment declarations
;*
_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
_DATA ends

_TEXT segment use16
extrn   __ThunkTable : word
        assume cs:_TEXT
        assume ds:dgroup


        public GetFirst16Alias
        public Get16Alias
GetFirst16Alias proc near
        mov     bp,sp                   ; remember address of first alias
        add     bp,2                    ; ...
Get16Alias:
        pop     cx                      ; get return address
        push    eax                     ; save original pointer value
        test    eax,eax                 ; check for NULL pointer
        je      short L0a               ; if not NULL, then
        cmp     eax,0FFFF0000h          ; see if outside our address space
        jae     short L0b               ; ...
        mov     edx,eax                 ; move eax to dx:ax
        shr     edx,16                  ; ...
        push    ss                      ; set es=ss
        pop     es                      ; ...
        sub     sp,4                    ; allocate space for aliased pointer
        mov     si,sp                   ; point es:si at allocated space
        push    cx                      ; push return address
        mov     cx,1                    ; want 1 alias
        call    DPMIGetAliases_         ; get alias
        mov     es, _DataSelector       ; reload es
        ret                             ; return

L0b:    movzx   eax,ax                  ; zero extend the value
L0a:    push    eax                     ; push aliased pointer
        jmp     cx                      ; return
GetFirst16Alias endp

        public Free16Alias
Free16Alias proc near
        call    dword ptr ds:[bx]       ; call API function
        push    dx                      ; save return value in edi
        push    ax                      ; ...
        pop     edi                     ; ...
L0c:    pop     eax                     ; get aliased pointer
        pop     edx                     ; get original pointer
        cmp     eax,edx                 ; compare them
        je      short L0d               ; if different, then
        shr     eax,16                  ; - get selector
        call    DPMIFreeAlias_          ; - free it
L0d:                                    ; endif
        cmp     sp,bp                   ; are we done?
        jne     L0c                     ; jump if not done
        mov     eax,edi                 ; get return value
        ret                             ; return
Free16Alias endp

;
; __Win16Thunk1 - either there are more than 5 parms, or at least one
;       of the parms is a pointer and requires an alias for it
;       edi - points to parms on 32-bit stack
;
        public  __Win16Thunk1_
__Win16Thunk1_ proc far
        mov     ecx,esp                 ; save 32-bit sp
        mov     _SaveSP,ecx             ; ...
        lss     sp,_EntryStackSave      ; switch to 16-bit stack
        push    ecx                     ; save 32-bit stack pointer
        mov     si,bx                   ; get thunk index
        shr     ebx,16                  ; get API function index
        add     bx,offset _FunctionTable; ...
        call    cs:__ThunkTable[si]        ; call appropriate thunk routine
        pop     ecx                     ; restore 32-bit stack pointer
        mov     word ptr _EntryStackSave,SP     ; save current sp
        mov     bx,_DataSelector        ; load 32-bit data segment
        mov     ss,_StackSelector       ; switch back to 32-bit stack
        mov     esp,ecx                 ; ...
        mov     ds,bx                   ; reload ds
        mov     es,bx                   ; and es
        db      66h                     ; far return to 32-bit code
        ret                             ; ...
__Win16Thunk1_ endp

;
; __Win16Thunk2 - there are less than 6 parms and they have been preloaded
;       into registers.  No aliases are required.
;
        public  __Win16Thunk2_
__Win16Thunk2_ proc far
        mov     _SaveSP,esp             ; save 32-bit stack pointer
        lss     sp,_EntryStackSave      ; switch to 16-bit stack
        push    _SaveSP                 ; save 32-bit sp
        mov     bp,bx                   ; get thunk index
        shr     ebx,16                  ; get API function index
        add     bx,offset _FunctionTable; ...
        call    cs:__ThunkTable[bp]        ; call appropriate thunk routine
        push    dx                      ; load eax with return value
        push    ax                      ; ...
        pop     eax                     ; ...
        pop     ecx                     ; restore 32-bit stack pointer
        mov     word ptr _EntryStackSave,SP     ; save current sp
        mov     bx,_DataSelector        ; load 32-bit data segment
        mov     ss,_StackSelector       ; switch back to 32-bit stack
        mov     esp,ecx                 ; ...
        mov     ds,bx                   ; reload ds
        mov     es,bx                   ; and es
        db      66h                     ; far return to 32-bit code
        ret                             ; ...
__Win16Thunk2_ endp

;
; __Win16Thunk3 - either there are more than 5 parms, or at least one
;       of the parms is a pointer and requires an alias for it.
;       This thunk is the same as __Win16Thunk1 except the return value
;       is a pointer.
;       edi - points to parms on 32-bit stack
;
        public  __Win16Thunk3_
__Win16Thunk3_ proc far
        mov     ecx,esp                 ; save 32-bit sp
        mov     _SaveSP,ecx             ; ...
        lss     sp,_EntryStackSave      ; switch to 16-bit stack
        push    ecx                     ; save 32-bit stack pointer
        mov     si,bx                   ; get thunk index
        shr     ebx,16                  ; get API function index
        add     bx,offset _FunctionTable; ...
        call    cs:__ThunkTable[si]        ; call appropriate thunk routine
        push    eax                     ; load 16:16 pointer into dx:ax
        pop     ax                      ; ...
        pop     dx                      ; ...
map2flat:or     dx,dx                   ; check for NULL pointer
        je      short nullptr           ; ...
        movzx   esi,ax                  ; save offset
        mov     bx,dx                   ; get selector
        mov     ax,6                    ; load function code
        int     31h                     ; get base address of selector
        shl     ecx,16                  ; cx:dx is base
        mov     cx,dx                   ; ...
        add     ecx,esi                 ; add offset to base
        mov     eax,ecx                 ; to get 32-bit flat address
        jmp     short mapdone           ; ...

nullptr:sub     eax,eax                 ; NULL pointer
mapdone:pop     ecx                     ; restore 32-bit stack pointer
        mov     word ptr _EntryStackSave,SP     ; save current sp
        mov     bx,_DataSelector        ; load 32-bit data segment
        mov     ss,_StackSelector       ; switch back to 32-bit stack
        mov     esp,ecx                 ; ...
        mov     ds,bx                   ; reload ds
        mov     es,bx                   ; and es
        db      66h                     ; far return to 32-bit code
        ret                             ; ...
__Win16Thunk3_ endp

;
; __Win16Thunk4 - there are less than 6 parms and they have been preloaded
;       into registers.  No aliases are required.
;       This thunk is the same as __Win16Thunk2 except the return value
;       is a pointer.
;
        public  __Win16Thunk4_
__Win16Thunk4_ proc far
        mov     _SaveSP,esp             ; save 32-bit stack pointer
        lss     sp,_EntryStackSave      ; switch to 16-bit stack
        push    _SaveSP                 ; save 32-bit sp
        mov     bp,bx                   ; get thunk index
        shr     ebx,16                  ; get API function index
        add     bx,offset _FunctionTable; ...
        call    cs:__ThunkTable[bp]        ; call appropriate thunk routine
        jmp     map2flat                ; map 16:16 pointer to 32-bit flat

__Win16Thunk4_ endp

GetNestedAliases proc near
        push    ebx                     ; save ebx
        push    edi                     ; save edi
        lea     edi,4[ecx]              ; get pointer to 32-bit stack
        mov     cx,es:4[edi]            ; get count
        push    cx                      ; save it
        mov     bp,sp                   ; point to saved registers
nest1:; _loop                           ; loop
          add   edi,8                   ; - point to next pointer to alias
          mov   ebx,es:[edi]            ; - get pointer
          add   ebx,es:4[edi]           ; - add offset
          mov   eax,es:[ebx]            ; - get enclosed pointer
          call  Get16Alias              ; - get alias for the pointer
          pop   eax                     ; - retrieve alias
          push  eax                     ; - save it again
          mov   es:[ebx],eax            ; - store alias pointer in structure
          dec   word ptr [bp]           ; - decrement count
        jne     nest1                   ; until e
        push    _SaveSP                 ; save 32-bit stack pointer
        mov     edi,2[bp]               ; restore edi
        mov     ebx,6[bp]               ; restore ebx
        mov     cx,10[bp]               ; get return address
        jmp     cx                      ; return
GetNestedAliases endp

FreeNestedAliases proc near
        pop     bp                      ; save return address
        mov     esi,eax                 ; save return value
        pop     ecx                     ; get 32-bit stack pointer
        lea     edi,4[ecx]              ; get pointer to 32-bit stack
        mov     es,_DataSelector        ; load 32-bit data segment
        mov     ecx,es:4[edi]           ; get count
        lea     edi,8[edi+ecx*8]        ; point past first alias pushed
nest2:; _loop                           ; loop
          sub   edi,8                   ; - point to next pointer to alias
          mov   edx,es:[edi]            ; - get pointer
          add   edx,es:4[edi]           ; - add offset
          pop   eax                     ; - get alias
          pop   ebx                     ; - get old value
          mov   es:[edx],ebx            ; - restore old value
          cmp   eax,ebx                 ; - compare them
          je    short nest3             ; - if different, then
            shr   eax,16                ; - - get selector
            call  DPMIFreeAlias_        ; - - free it
            mov   es,_DataSelector      ; - - load 32-bit data segment
nest3:                                  ; - endif
          dec   cx                      ; - decrement count
        jne     nest2                   ; until e
        mov     eax,esi                 ; restore return value
        add     sp,12                   ; del items saved by GetNestedAliases
        jmp     bp                      ; return
FreeNestedAliases endp

;
; __Win16Thunk5 - at least one of the parms is a pointer and requires
;       an alias for it. At least one of the pointer parms points to
;       a structure that contains a pointer that also needs aliasing.
;       edi - points to parms on 32-bit stack
;               offset
;               pointer
;               offset
;               pointer
;               count
;               far return address
;
        public  __Win16Thunk5_
__Win16Thunk5_ proc far
        mov     ecx,esp                 ; save 32-bit sp
        mov     _SaveSP,ecx             ; ...
        lss     sp,_EntryStackSave      ; switch to 16-bit stack
        push    ecx                     ; save 32-bit stack pointer
        call    GetNestedAliases        ; get nested aliases
        mov     si,bx                   ; get thunk index
        shr     ebx,16                  ; get API function index
        add     bx,offset _FunctionTable; ...
        call    cs:__ThunkTable[si]        ; call appropriate thunk routine
        call    FreeNestedAliases       ; free nested aliases
        pop     ecx                     ; restore 32-bit stack pointer
        mov     word ptr _EntryStackSave,SP     ; save current sp
        mov     bx,_DataSelector        ; load 32-bit data segment
        mov     ss,_StackSelector       ; switch back to 32-bit stack
        mov     esp,ecx                 ; ...
        mov     ds,bx                   ; reload ds
        mov     es,bx                   ; and es
        db      66h                     ; far return to 32-bit code
        ret                             ; ...
__Win16Thunk5_ endp

;
; __Win16Thunk6 - at least one of the parms is a pointer and requires
;       an alias for it. At least one of the pointer parms points to
;       a structure that contains a pointer that also needs aliasing.
;       This thunk is the same as __Win16Thunk5 except the return value
;       is a pointer.
;       edi - points to parms on 32-bit stack
;
        public  __Win16Thunk6_
__Win16Thunk6_ proc far
        mov     ecx,esp                 ; save 32-bit sp
        mov     _SaveSP,ecx             ; ...
        lss     sp,_EntryStackSave      ; switch to 16-bit stack
        push    ecx                     ; save 32-bit stack pointer
        mov     si,bx                   ; get thunk index
        shr     ebx,16                  ; get API function index
        add     bx,offset _FunctionTable; ...
        call    cs:__ThunkTable[si]        ; call appropriate thunk routine
        push    eax                     ; load 16:16 pointer into dx:ax
        pop     ax                      ; ...
        pop     dx                      ; ...
        jmp     map2flat                ; map 16:16 pointer to 32-bit flat

__Win16Thunk6_ endp

_TEXT   ends
        end
