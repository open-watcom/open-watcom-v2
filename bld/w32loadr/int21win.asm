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
;*** INT21WIN.ASM - handler for all 32-bit int 21h calls                  ***
;***                                                                      ***
;****************************************************************************
.386p
include struct.inc

DGROUP group _DATA

extrn   _DataSelector   : word
extrn   _StackSelector  : word
extrn   _16BitSelector  : word
extrn   _DGroupSelector : word
extrn   RM_BufSeg       : word
extrn   PM_BufSeg       : word
extrn   RM_BufSize      : word
extrn   _EntryStackSave : dword
extrn   BaseAddr        : dword

;
;  int 21h classification codes
;
_NONE   equ     0*2             ; no special requirements
_READ   equ     1*2             ; read
_WRITE  equ     2*2             ; write
_FIND   equ     3*2             ; find first/find next
_RENAME equ     4*2             ; rename file (pointers in EDX, EDI)
_IOCTL  equ     5*2             ; ioctl function
_NETBIOS equ    6*2             ; netBIOS functions
_5F     equ     7*2             ; _5F functions
_ALLOC  equ     8*2             ; memory allocation function
_ASCIZ  equ     9*2             ; ascii string passed in EDX
_EDX    equ     10*2            ; pointer in EDX needs translated
_EDI    equ     11*2            ; pointer in EDI needs translated
_SETDTA equ     12*2            ; set DTA address
_GETCWD equ     13*2            ; get current working directory

DTA_SIZE equ    43              ; size of DTA

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
regs    label   word            ; real mode register structure for DPMI
                                ; simulate real-mode interrupt (AX=300h)
r_DI    dw      0,0             ; 00h - DI or EDI
r_SI    dw      0,0             ; 04h - SI or ESI
r_BP    dw      0,0             ; 08h - BP or EBP
        dd      0               ; 0Ch - reserved
r_BX    dw      0,0             ; 10h - BX or EBX
r_DX    dw      0,0             ; 14h - DX or EDX
r_CX    dw      0,0             ; 18h - CX or ECX
r_AX    dw      0,0             ; 1Ch - AX or EAX
r_FLAGS dw      0               ; 20h - flags
r_ES    dw      0               ; 22h - ES
r_DS    dw      0               ; 24h - DS
r_FS    dw      0               ; 26h - FS
r_GS    dw      0               ; 28h - GS
r_IP    dw      0               ; 2Ah - IP (ignored by DPMI 300h)
r_CS    dw      0               ; 2Ch - CS  ...
r_SP    dw      0               ; 2Eh - SP (SS:SP = 0 => DPMI host supplies
r_SS    dw      0               ; 30h - SS ... stack)

_DTA_Addr dd    0               ; DTA address

_DATA ends

_TEXT segment use16
        assume cs:_TEXT
        assume ds:DGROUP

ClassTable dw   C_NONE          ; no translation required
        dw      C_READ          ; read
        dw      C_WRITE         ; write
        dw      C_FIND          ; find first/find next
        dw      C_RENAME        ; rename file (EDX and EDI)
        dw      C_IOCTL         ; ioctl function
        dw      C_NETBIOS       ; netBIOS functions
        dw      C_5F            ; _5F functions
        dw      C_ALLOC         ; memory allocation function
        dw      C_ASCIZ         ; ASCII string passed in EDX
        dw      C_EDX           ; translate pointer in EDX
        dw      C_EDI           ; pointer in EDI needs translated
        dw      C_SETDTA        ; set DTA address
        dw      C_GETCWD        ; get current working directory

Int21Class db   _NONE           ; 00 - terminate process
        db      _NONE           ; 01 - character input with echo
        db      _NONE           ; 02 - character output
        db      _NONE           ; 03 - Auxillary input
        db      _NONE           ; 04 - Auxillary output
        db      _NONE           ; 05 - print character
        db      _NONE           ; 06 - direct console I/O
        db      _NONE           ; 07 - unfiltered char input without echo
        db      _NONE           ; 08 - character input without echo
        db      _ASCIZ          ; 09 - display string
        db      _EDX            ; 0A - buffered keyboard input
        db      _NONE           ; 0B - check keyboard status
        db      _EDX            ; 0C - flush buffer, read keyboard
        db      _NONE           ; 0D - disk reset
        db      _NONE           ; 0E - select disk
        db      _EDX            ; 0F - open file with FCB
        db      _EDX            ; 10 - close file with FCB
        db      _EDX            ; 11 - find first file
        db      _EDX            ; 12 - find next file
        db      _EDX            ; 13 - delete file
        db      _EDX            ; 14 - sequential read
        db      _EDX            ; 15 - sequential write
        db      _EDX            ; 16 - create file with FCB
        db      _EDX            ; 17 - rename file
        db      _NONE           ; 18 - unused
        db      _NONE           ; 19 - get current disk
        db      _SETDTA         ; 1A - set DTA address
        db      _NONE           ; 1B - get default drive data
        db      _NONE           ; 1C - get drive data
        db      _NONE           ; 1D - unused
        db      _NONE           ; 1E - unused
        db      _NONE           ; 1F - unused
        db      _NONE           ; 20 - unused
        db      _EDX            ; 21 - random read
        db      _EDX            ; 22 - random write
        db      _EDX            ; 23 - get file size
        db      _EDX            ; 24 - set relative record
        db      _EDX            ; 25 - set interrupt vector
        db      _EDX            ; 26 - create new program segment prefix
        db      _EDX            ; 27 - random block read
        db      _EDX            ; 28 - random block write
        db      _EDX            ; 29 - parse filename
        db      _NONE           ; 2A - get date
        db      _NONE           ; 2B - set date
        db      _NONE           ; 2C - get time
        db      _NONE           ; 2D - set time
        db      _NONE           ; 2E - set/reset verify flag
        db      _NONE           ; 2F - get DTA address
        db      _NONE           ; 30 - get MS-DOS version number
        db      _NONE           ; 31 - terminate and stay resident
        db      _NONE           ; 32 - unused
        db      _NONE           ; 33 - get/set control-C check flag
        db      _EDX            ; 34 - return address of InDOS flag
        db      _EDX            ; 35 - get interrupt vector
        db      _NONE           ; 36 - get disk free space
        db      _NONE           ; 37 - unused
        db      _EDX            ; 38 - get/set current country
        db      _ASCIZ          ; 39 - create directory
        db      _ASCIZ          ; 3A - remove directory
        db      _ASCIZ          ; 3B - change current directory
        db      _ASCIZ          ; 3C - create file with handle
        db      _ASCIZ          ; 3D - open file with handle
        db      _NONE           ; 3E - close file
        db      _READ           ; 3F - read file or device
        db      _WRITE          ; 40 - write file or device
        db      _ASCIZ          ; 41 - delete file
        db      _NONE           ; 42 - lseek
        db      _ASCIZ          ; 43 - get/set file attribute
        db      _IOCTL          ; 44 - ioctl
        db      _NONE           ; 45 - duplicate file handle
        db      _NONE           ; 46 - dup2
        db      _GETCWD         ; 47 - get current directory
        db      _ALLOC          ; 48 - allocate memory block
        db      _NONE           ; 49 - free memory block
        db      _NONE           ; 4A - resize memory block
        db      _EDX            ; 4B - load and execute program
        db      _NONE           ; 4C - terminate process with return code
        db      _NONE           ; 4D - get return code of child process
        db      _FIND           ; 4E - find first file
        db      _FIND           ; 4F - find next file
        db      _NONE           ; 50 - unused
        db      _NONE           ; 51 - unused
        db      _NONE           ; 52 - get list of lists
        db      _NONE           ; 53 - unused
        db      _NONE           ; 54 - get verify flag
        db      _NONE           ; 55 - unused
        db      _RENAME         ; 56 - rename file
        db      _NONE           ; 57 - get/set date/time of file
        db      _NONE           ; 58 - get/set allocation strategy
        db      _NONE           ; 59 - get extended error information
        db      _ASCIZ          ; 5A - create temporary file
        db      _ASCIZ          ; 5B - create new file
        db      _NONE           ; 5C - lock/unlock file region
        db      _NONE           ; 5D - unused
        db      _NETBIOS        ; 5E - NetBIOS
        db      _5F             ; 5F - get/make assign list entry
        db      _NONE           ; 60 - unused
        db      _NONE           ; 61 - unused
        db      _NONE           ; 62 - get program segment prefix
        db      _NONE           ; 63 - get lead byte table
        db      _NONE           ; 64 - unused
        db      _EDI            ; 65 - get extended country information
        db      _NONE           ; 66 - get/set code page
        db      _NONE           ; 67 - set handle count
        db      _NONE           ; 68 - commit file

        align   2
;
; This routine is called from a 32-bit segment
;
        public  __fInt21
__fInt21 proc   far
        push    ebp                     ; save ebp
        mov     ebp,esp                 ; save 32-bit stack pointer
        push    eax                     ; save eax
        mov     ds,cs:_DGroupSelector   ; switch to our DGROUP
        lss     sp,_EntryStackSave      ; switch to 16-bit stack
        mov     eax,es:-4[ebp]          ; reload eax
        call    __Int21                 ; call local int 21 handler
;
;       must not modify any registers or the condition codes
;
        mov     ss,_StackSelector       ; switch back to 32-bit stack
        mov     esp,ebp                 ; ...
        pop     ebp                     ; restore ebp
        mov     es,_DataSelector        ; reload 32-bit data selector
        mov     ds,_DataSelector        ; ...
        db      66h                     ; return to 32-bit code
        ret                             ; ...
__fInt21 endp

;
; This routine is called from a 16-bit segment
;
        public  __Int21
__Int21 proc    near
        mov     r_AX,ax                 ; save registers in regs struct
        mov     r_BX,bx                 ; ...
        mov     r_CX,cx                 ; ...
        mov     r_DX,dx                 ; ...
        mov     r_SI,si                 ; ...
        mov     r_DI,di                 ; ...
        mov     r_BP,bp                 ; ...
        cmp     ah,68h                  ; if valid func code
        _if     be                      ; then
          mov   bl,ah                   ; - get func code
          mov   bh,0                    ; - zero high byte
          mov   bl,Int21Class[bx]       ; - determine class of func
          mov   bx,ClassTable[bx]       ; - get addr of func
          jmp   bx                      ; - jump to func
        _endif                          ; endif
        stc                             ; indicate error
        ret                             ; return

C_NONE:                                 ; no translation required
        call    DOSint21                ; issue DOS int 21h
        call    loadregs                ; reload regs from struct
        movsx   eax,ax                  ; sign-extend value in ax
        ret                             ; return

C_ASCIZ:                                ; ASCII string passed in EDX
        mov     cl,0                    ; assume null terminated string
        cmp     ah,9                    ; if func = write string
        _if     e                       ; then
          mov   cl,'$'                  ; - it is '$' terminated string
        _endif                          ; endif
        push    edx                     ; save edx
        push    si                      ; save si
        push    ds                      ; save ds
        push    es                      ; save es
        mov     es,PM_BufSeg            ; get selector for real-mode buffer
        sub     si,si                   ; start at offset 0
        mov     ds,_DataSelector        ; get 32-bit selector
        _loop                           ; loop
          mov   al,[edx]                ; - get character
          inc   edx                     ; - advance pointer
          mov   es:[si],al              ; - store it
          inc   si                      ; - advance pointer
          cmp   al,cl                   ; - check for ending character
        _until  e                       ; until done
        pop     es                      ; restore es
        pop     ds                      ; restore ds
        mov     ax,RM_BufSeg            ; set DS to real-mode segment
        mov     r_DS,ax                 ; ...
        mov     r_DX,0                  ; set offset to 0
        call    DOSint21                ; issue DOS int 21h
        call    loadregs                ; reload regs from struct
        movsx   eax,ax                  ; sign-extend value in ax
        pop     si                      ; restore si
        pop     edx                     ; restore edx
        ret                             ; return

C_GETCWD:                               ; get current working directory
        push    di                      ; save di
        push    esi                     ; save esi
        mov     ax,RM_BufSeg            ; set DS to real-mode segment
        mov     r_DS,ax                 ; ...
        mov     r_SI,0                  ; set offset to buffer
        call    DOSint21                ; issue DOS int 21h
        call    loadregs                ; reload regs from struct
        pop     esi                     ; restore pointer to buffer
        _if     nc                      ; if no error
          push  ds                      ; - save ds
          push  bx                      ; - save bx
          mov   es,_DataSelector        ; - get 32-bit selector
          mov   ds,PM_BufSeg            ; - get selector for real-mode buffer
          sub   bx,bx                   ; - start at offset 0
          _loop                         ; - loop (copy cwd to 32-bit buffer)
            mov   al,[bx]               ; - - get character
            inc   bx                    ; - - advance pointer
            mov   es:[esi],al           ; - - copy to 32-bit buffer
            inc   esi                   ; - - increment pointer
            cmp   al,0                  ; - - check for end of string
          _until  e                     ; - until done
          pop   bx                      ; - restore bx
          pop   ds                      ; - restore ds
          mov   ax,r_AX                 ; - get return value
          clc                           ; - indicate success
        _endif                          ; endif
        movsx   eax,ax                  ; sign extend return code
        pop     di                      ; restore di
        ret                             ; return


C_RENAME:                               ; rename file
        push    edx                     ; save edx
        push    edi                     ; save edi
        push    si                      ; save si
        push    ds                      ; save ds
        push    es                      ; save es
        mov     es,PM_BufSeg            ; get selector for real-mode buffer
        sub     si,si                   ; start at offset 0
        mov     ds,_DataSelector        ; get 32-bit selector
        sub     cx,cx                   ; start at offset 0 in real-mode buf
        _loop                           ; loop
          mov   al,[edx]                ; - get character
          inc   edx                     ; - advance pointer
          mov   es:[si],al              ; - store it
          inc   si                      ; - advance pointer
          inc   cx                      ; - increment offset into buf
          cmp   al,0                    ; - check for ending character
        _until  e                       ; until done
        _loop                           ; loop
          mov   al,[edi]                ; - get character
          inc   edi                     ; - advance pointer
          mov   es:[si],al              ; - store it
          inc   si                      ; - advance pointer
          cmp   al,0                    ; - check for ending character
        _until  e                       ; until done
        pop     es                      ; restore es
        pop     ds                      ; restore ds
        mov     ax,RM_BufSeg            ; set DS to real-mode segment
        mov     r_DS,ax                 ; ...
        mov     r_ES,ax                 ; set ES to real-mode segment
        mov     r_DX,0                  ; set offset to old file
        mov     r_DI,cx                 ; set offset to new file
        call    DOSint21                ; issue DOS int 21h
        call    loadregs                ; reload regs from struct
        movsx   eax,ax                  ; sign-extend value in ax
        pop     si                      ; restore si
        pop     edi                     ; restore edi
        pop     edx                     ; restore edx
        ret                             ; return

C_READ:                                 ; read file
        mov     bx,r_BX                 ; restore bx
        push    ebx                     ; save ebx
        push    ecx                     ; save ecx
        push    edx                     ; save edx
        push    edi                     ; save edi
        push    esi                     ; save esi
        mov     edi,edx                 ; get 32-bit buffer address
        add     edi,BaseAddr            ; add in BaseAddr
        sub     edx,edx                 ; initialize return value
        mov     esi,ecx                 ; save length to read
        mov     ax,RM_BufSeg            ; set DS to real-mode segment
        mov     r_DS,ax                 ; ...
        mov     r_DX,0                  ; set offset to start of buffer
        _loop                           ; loop
          sub   ecx,ecx                 ; - zero ecx
          mov   cx,RM_BufSize           ; - get length of real-mode buffer
          cmp   esi,ecx                 ; - if length left < real-mode buffer
          _if   b                       ; - then
            mov   cx,si                 ; - - use amount left
          _endif                        ; - endif
          mov   r_CX,cx                 ; - set length to read
          push  edi                     ; - save edi
          mov   r_AX,3F00h              ; - set AH = read func code
          call  DOSint21                ; - issue DOS int 21h
          pop   edi                     ; - restore edi
          _quif c                       ; - quit if error
          mov   cx,r_AX                 ; - get length read
          add   edx,ecx                 ; - accumulate # of bytes read
          call  CopyFromRMBuffer        ; - copy from RM to protect-mode
          sub   eax,eax                 ; - zero whole register
          mov   ax,r_AX                 ; - get length read
          add   edi,eax                 ; - advance protect-mode buffer addr
          cmp   ax,r_CX                 ; - quit if length != cx
          clc                           ; - clear carry bit
          _quif ne                      ; - quit if length != cx
          sub   esi,eax                 ; - subtract from total
        _until  e                       ; until all done
        _if     c                       ; if error
          mov   dx,ax                   ; - store error code in dx
        _endif                          ; endif
        mov     eax,edx                 ; return length read or error
        pop     esi                     ; restore esi
        pop     edi                     ; restore edi
        pop     edx                     ; restore edx
        pop     ecx                     ; restore ecx
        pop     ebx                     ; restore ebx
        ret                             ; return

C_WRITE:                                ; write file
        mov     bx,r_BX                 ; restore bx
        push    ebx                     ; save ebx
        push    ecx                     ; save ecx
        push    edx                     ; save edx
        push    edi                     ; save edi
        push    esi                     ; save esi
        mov     esi,edx                 ; get 32-bit buffer address
        add     esi,BaseAddr            ; add in BaseAddr
        sub     edx,edx                 ; initialize return value
        mov     edi,ecx                 ; save length to write
        mov     ax,RM_BufSeg            ; set DS to real-mode segment
        mov     r_DS,ax                 ; ...
        mov     r_DX,0                  ; set offset to start of buffer
        _loop                           ; loop
          sub   ecx,ecx                 ; - zero ecx
          mov   cx,RM_BufSize           ; - get length of real-mode buffer
          cmp   edi,ecx                 ; - if length left < real-mode buffer
          _if   b                       ; - then
            mov   cx,di                 ; - - use amount left
          _endif                        ; - endif
          mov   r_CX,cx                 ; - set length to write
          push  edi                     ; - save edi
          call  CopyToRMBuffer          ; - copy data to real-mode buffer
          mov   r_AX,4000h              ; - set AH = write func code
          call  DOSint21                ; - issue DOS int 21h
          pop   edi                     ; - restore edi
          _quif c                       ; - quit if error
          sub   eax,eax                 ; - get length written
          mov   ax,r_AX                 ; - ...
          add   edx,eax                 ; - accumulate # of bytes written
          add   esi,eax                 ; - advance 32-bit buffer pointer
          cmp   ax,r_CX                 ; - quit if length != cx
          clc                           ; - clear carry bit
          _quif ne                      ; - quit if length != cx
          sub   edi,eax                 ; - subtract from total
        _until  e                       ; until all done
        _if     c                       ; if error
          mov   dx,ax                   ; - store error code in dx
        _endif                          ; endif
        mov     eax,edx                 ; return length written or error
        pop     esi                     ; restore esi
        pop     edi                     ; restore edi
        pop     edx                     ; restore edx
        pop     ecx                     ; restore ecx
        pop     ebx                     ; restore ebx
        ret                             ; return

C_FIND:                                 ; find first/find next
        mov     bx,r_BX                 ; restore bx
        push    ebx                     ; save ebx
        push    ecx                     ; save ecx
        push    edx                     ; save edx
        push    edi                     ; save edi
        push    esi                     ; save esi
        mov     ax,RM_BufSeg            ; set DS to real-mode segment
        mov     r_DS,ax                 ; ...
        mov     r_DX,DTA_SIZE           ; set offset past real-mode DTA
        mov     ax,r_AX                 ; get func code
        cmp     ah,4Eh                  ; if find first
        _if     e                       ; then
          mov   _DTA_addr,ebx           ; - save 32-bit DTA addr
          push  ds                      ; - save ds
          push  es                      ; - save es
          mov   es,PM_BufSeg            ; - point to real-mode buffer
          mov   di,DTA_SIZE             ; - place name after the DTA
          mov   ds,_DataSelector        ; - get 32-bit selector
          _loop                         ; - loop
            mov   al,[edx]              ; - - get character
            inc   edx                   ; - - advance pointer
            mov   es:[di],al            ; - - store it
            inc   di                    ; - - advance pointer
            cmp   al,0                  ; - - check for ending character
          _until  e                     ; - until done
          pop   es                      ; - restore es
          pop   ds                      ; - restore ds
        _else                           ; else findnext/findclose
          mov   _DTA_addr,edx           ; - save 32-bit DTA addr
          mov   esi,_DTA_addr           ; - get addr of 32-bit DTA
          add   esi,BaseAddr            ; - ...
          mov   cx,DTA_SIZE             ; - get size of DTA
          call  CopyToRMBuffer          ; - copy DTA to real-mode
        _endif                          ; endif
        call    DOSint21                ; issue DOS int 21h
        mov     edi,_DTA_addr           ; get addr of 32-bit DTA
        add     edi,BaseAddr            ; ...
        mov     cx,DTA_SIZE             ; get size of DTA
        call    CopyFromRMBuffer        ; copy DTA to protect-mode
        mov     ax,r_FLAGS              ; get flags
        mov     ah,al                   ; ...
        sahf                            ; set flags
        mov     ax,r_AX                 ; get AX
        movsx   eax,ax                  ; sign extend
        pop     esi                     ; restore esi
        pop     edi                     ; restore edi
        pop     edx                     ; restore edx
        pop     ecx                     ; restore ecx
        pop     ebx                     ; restore ebx
        ret                             ; return

C_SETDTA:                               ; set DTA address
        mov     _DTA_Addr,edx           ; save 32-bit DTA address
        call    loadregs                ; reload regs from struct
        ret                             ; return

;
;  input:
;       ds - 16-bit DGROUP
;       cx - length to copy
;       edi - 32-bit offset to copy to
;
CopyFromRMBuffer:
        push    ds                      ; save ds
        push    es                      ; save es
        push    dx                      ; save dx
        push    si                      ; save si
        push    di                      ; save di
        push    cx                      ; save length
        mov     bx,_16BitSelector       ; get 16-bit alias selector
        push    edi                     ; transfer base address to cx:dx
        pop     dx                      ; ...
        pop     cx                      ; ...
        mov     ax,7                    ; set 'segment base address'
        int     31h                     ; ...
        pop     cx                      ; restore length
        mov     ds,PM_BufSeg            ; get selector of real-mode buffer
        sub     si,si                   ; offset 0
        mov     es,bx                   ; set es = 16-bit alias selector
        sub     di,di                   ; offset 0
        shr     cx,1                    ; calc. # of words to copy
        rep     movsw                   ; copy to 32-bit buffer
        adc     cx,cx                   ; get last byte count
        rep     movsb                   ; copy it
        pop     di                      ; restore di
        pop     si                      ; restore si
        pop     dx                      ; restore dx
        pop     es                      ; restore es
        pop     ds                      ; restore ds
        ret                             ; return

;
;  input:
;       ds - 16-bit DGROUP
;       cx - length to copy
;       esi - 32-bit offset to copy from
;
CopyToRMBuffer:
        push    ds                      ; save ds
        push    es                      ; save es
        push    dx                      ; save dx
        push    si                      ; save si
        push    di                      ; save di
        push    cx                      ; save length
        mov     bx,_16BitSelector       ; get 16-bit alias selector
        push    esi                     ; transfer base address to cx:dx
        pop     dx                      ; ...
        pop     cx                      ; ...
        mov     ax,7                    ; set 'segment base address'
        int     31h                     ; ...
        pop     cx                      ; restore length
        mov     es,PM_BufSeg            ; get selector of real-mode buffer
        sub     di,di                   ; offset 0
        mov     ds,bx                   ; set ds = 16-bit alias selector
        sub     si,si                   ; offset 0
        shr     cx,1                    ; calc. # of words to copy
        rep     movsw                   ; copy to 32-bit buffer
        adc     cx,cx                   ; get last byte count
        rep     movsb                   ; copy it
        pop     di                      ; restore di
        pop     si                      ; restore si
        pop     dx                      ; restore dx
        pop     es                      ; restore es
        pop     ds                      ; restore ds
        ret                             ; return

C_EDX:                                  ; translate pointer in EDX
        mov     bx,r_BX                 ; restore bx
        push    ds                      ; save ds
        call    alias_EDX               ; alias pointer in EDX
        int     21h                     ; do interrupt
        movsx   eax,ax                  ; sign-extend value in ax
        pop     ds                      ; restore ds
        ret                             ; return

C_EDI:                                  ; pointer in EDI needs translated
C_IOCTL:                                ; ioctl function
C_NETBIOS:                              ; netBIOS functions
C_5F:                                   ; _5F functions

        mov     bx,r_BX                 ; restore bx
        int     21h                     ; do the function
        movsx   eax,ax                  ; sign-extend value in ax
        ret                             ; return

C_ALLOC:                                ; ebx = length to allocate
        mov     bx,r_BX                 ; restore bx
        push    bx                      ; save bx
        push    cx                      ; save cx
        push    si                      ; save si
        push    di                      ; save di
        _guess                          ; guess: no error
          push  ebx                     ; - push amount to allocate
          pop   cx                      ; - transfer to bx:cx
          pop   bx                      ; - ...
          mov   ax,0501h                ; - DPMI allocate memory block
          int   31h                     ; - ...
          mov   eax,0                   ; - assume error
          _quif c                       ; - quit if error
          ; bx:cx = linear address of allocated memory
          ; si:di = memory block handle
          push  bx                      ; - place address in eax
          push  cx                      ; - ...
          pop   eax                     ; - ...
          sub   eax,BaseAddr            ; - calc. offset relative to BaseAddr
          clc                           ; - indicate success
        _endguess                       ; endguess
        pop     di                      ; restore di
        pop     si                      ; restore si
        pop     cx                      ; restore cx
        pop     bx                      ; restore bx
        ret                             ; return
__Int21 endp
;
;       EDX contains a pointer
;
alias_EDX proc  near
        or      edx,edx                 ; check for NULL pointer
        jne     short not_null          ; ...
        mov     ds,dx                   ; set ds:dx to NULL
        ret                             ; return
not_null:
        push    ax                      ; save ax
        push    bx                      ; save bx
        push    cx                      ; save cx
        mov     ds,cs:_DGroupSelector   ; switch to our DGROUP
        mov     bx,_16BitSelector       ; get 16-bit alias selector
        add     edx,BaseAddr            ; add in BaseAddr
        push    edx                     ; transfer to cx:dx
        pop     dx                      ; ...
        pop     cx                      ; ...
        mov     ax,7                    ; set 'segment base address'
        int     31h                     ; ...
        mov     ds,bx                   ; set ds = 16-bit alias selector
        sub     dx,dx                   ; offset 0
        pop     cx                      ; restore cx
        pop     bx                      ; restore bx
        pop     ax                      ; restore ax
        ret                             ; return
alias_EDX endp


loadregs proc   near                    ; reload registers from regs struct
        mov     ax,r_AX                 ; ...
        mov     bx,r_BX                 ; ...
        mov     cx,r_CX                 ; ...
        mov     dx,r_DX                 ; ...
        mov     si,r_SI                 ; ...
        mov     di,r_DI                 ; ...
        mov     bp,r_BP                 ; ...
        ret                             ; return
loadregs endp

DOSint21 proc   near                    ; use DPMI to invoke DOS int 21h
        push    es                      ; save es
        mov     bl,21h                  ; interrupt 21h
        mov     bh,0                    ; flags must be 0
        sub     cx,cx                   ; 0 words to copy to real mode stack
        push    ds                      ; set es:di = addr of regs struct
        pop     es                      ; ...
        mov     di,offset DGROUP:regs   ; ...
        mov     ax,300h                 ; 'Simulate real mode interrupt'
        int     31h                     ; issue int 21h via DPMI
        mov     ax,r_FLAGS              ; get flags
        mov     ah,al                   ; ...
        sahf                            ; set flags
        pop     es                      ; restore es
        ret                             ; return
DOSint21 endp

_TEXT   ends
        end
