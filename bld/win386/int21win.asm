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

extrn   _DataSelector   :WORD
extrn   _StackSelector  :WORD
extrn   _Int21Selector  :WORD
extrn   _DataSelectorBase:DWORD
extrn   _SaveSP         :DWORD
extrn   _EntryStackSave :DWORD

;
;  int 21h classification codes
;
_NONE   equ     0*2             ; no special requirements
_EDX    equ     1*2             ; pointer in EDX needs translated
_EDI    equ     2*2             ; pointer in EDI needs translated
_EDX_EDI equ    3*2             ; pointers in EDX, EDI need translated
_IOCTL  equ     4*2             ; ioctl function
_NETBIOS equ    5*2             ; netBIOS functions
_5F     equ     6*2             ; _5F functions
_READ   equ     7*2             ; read
_WRITE  equ     8*2             ; write
_SETDTA equ     9*2             ; set DTA
_FIND   equ     10*2            ; find first/find next
_GETCWD equ     11*2            ; get current directory
_EOPEN  equ     12*2            ; extended open

DTA_SIZE equ    43              ; size of DTA area

_TEXT segment word public 'CODE' use16
_TEXT ends

_DATA segment word public 'DATA' use16
_DTA_Addr       dd      0       ; 32-bit DTA address
_DTA_Area       db      44 dup(0) ; local DTA area
_DATA ends

_TEXT segment use16
        assume cs:_TEXT
        assume ds:DGROUP

ClassTable dw   C_NONE          ; no translation required
        dw      C_EDX           ; translate pointer in EDX
        dw      C_EDI           ; pointer in EDI needs translated
        dw      C_EDX_EDI       ; pointers in EDX, EDI need translated
        dw      C_IOCTL         ; ioctl function
        dw      C_NETBIOS       ; netBIOS functions
        dw      C_5F            ; _5F functions
        dw      C_READ          ; read
        dw      C_WRITE         ; write
        dw      C_SETDTA        ; set DTA address
        dw      C_FIND          ; find first/find next
        dw      C_GETCWD        ; get current directory
        dw      C_EOPEN         ; extended open

Int21Class db   _NONE           ; 00 - terminate process
        db      _NONE           ; 01 - character input with echo
        db      _NONE           ; 02 - character output
        db      _NONE           ; 03 - Auxillary input
        db      _NONE           ; 04 - Auxillary output
        db      _NONE           ; 05 - print character
        db      _NONE           ; 06 - direct console I/O
        db      _NONE           ; 07 - unfiltered char input without echo
        db      _NONE           ; 08 - character input without echo
        db      _EDX            ; 09 - display string
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
        db      _EDX            ; 39 - create directory
        db      _EDX            ; 3A - remove directory
        db      _EDX            ; 3B - change current directory
        db      _EDX            ; 3C - create file with handle
        db      _EDX            ; 3D - open file with handle
        db      _NONE           ; 3E - close file
        db      _READ           ; 3F - read file or device
        db      _WRITE          ; 40 - write file or device
        db      _EDX            ; 41 - delete file
        db      _NONE           ; 42 - lseek
        db      _EDX            ; 43 - get/set file attribute
        db      _IOCTL          ; 44 - ioctl
        db      _NONE           ; 45 - duplicate file handle
        db      _NONE           ; 46 - dup2
        db      _GETCWD         ; 47 - get current directory
        db      _NONE           ; 48 - allocate memory block
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
        db      _EDX_EDI        ; 56 - rename file
        db      _NONE           ; 57 - get/set date/time of file
        db      _NONE           ; 58 - get/set allocation strategy
        db      _NONE           ; 59 - get extended error information
        db      _EDX            ; 5A - create temporary file
        db      _EDX            ; 5B - create new file
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
        db      _NONE           ; 69 - unused
        db      _NONE           ; 6A - unused
        db      _NONE           ; 6B - unused
        db      _EOPEN          ; 6C - extended open

IOctlClass db   0,0,1,1,1,1,0,0,0,0,0,0,1,1,0,0

        public  __Int21_
__Int21_ proc   far
        mov     _SaveSP,esp             ; save 32-bit stack pointer
        lss     sp,_EntryStackSave      ; switch to 16-bit stack
        push    bx                      ; save bx
        mov     bl,ah                   ; get func code
        mov     bh,0                    ; zero high byte
        mov     bl,Int21Class[bx]       ; determine class of func
        mov     bx,ClassTable[bx]       ; get addr of func
        jmp     bx                      ; jump to func

C_IOCTL:                                ; ioctl function
        cmp     al,0Fh                  ; check range of sub func code
        ja      short C_Error           ; error if too large
        mov     bl,al                   ; get sub func code
        mov     bh,0                    ; zero high byte
        mov     bl,IOctlClass[BX]       ; get class of function
        cmp     bl,0                    ; if no pointer translation required
        je      C_NONE                  ; then go do it
        jmp     C_EDX                   ; translate pointer in EDX

C_NONE:                                 ; no translation required
        pop     bx                      ; restore bx
        int     21h                     ; do interrupt
        jmp     short done_int21        ; and exit

C_EDI:                                  ; pointer in EDI needs translated
C_NETBIOS:                              ; netBIOS functions
C_5F:                                   ; _5F functions
C_Error:                                ; error
        pop     bx                      ; restore bx
        sub     eax,eax                 ; zero eax
        stc                             ; indicate error
        jmp     short done_int21        ; and exit

C_READ:                                 ; read
C_WRITE:                                ; write
        pop     bx                      ; restore bx
        push    ds                      ; save ds
        push    ax                      ; save ax
        push    bx                      ; save bx
        push    cx                      ; save cx
        mov     bx,_Int21Selector       ; get 16-bit alias selector
        add     edx,_DataSelectorBase   ; add in BaseAddr
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
        int     21h                     ; issue int 21h
        pop     ds                      ; restore ds
        jc      short done_int21        ; exit if error
        movzx   eax,ax                  ; zero extend length of read/write
        jmp     short exit_int21        ; and exit

;
;       EDX contains a pointer
;
C_EDX_EDI:                              ; pointers in EDX, EDI need translated
C_EDX:                                  ; translate pointer in EDX
        pop     bx                      ; restore bx
        or      edx,edx                 ; check for NULL pointer
        jne     short not_null          ; ...
        push    ds                      ; save ds
        mov     ds,dx                   ; set ds:dx to NULL
        int     21h                     ; do int 21
        pop     ds                      ; restore ds
        jmp     short done_int21        ; and exit
;
;       EDX points to a filename
;
not_null:
        push    ds                      ; save ds
        push    edx                     ; save edx
        push    bp                      ; save bp
        push    ax                      ; save ax
        push    si                      ; save si
        push    0                       ; push a NULLCHAR for end of buffer
        mov     bp,sp                   ; point to stack frame
        sub     sp,256                  ; allocate local buffer
        mov     si,sp                   ; point to buffer
        call    docopy                  ; copy buffer from edx to stack
        mov     ax,4[bp]                ; restore function code
        cmp     ah,56h                  ; if rename function
        jne     not_rename              ; then
          mov   edx,edi                 ; - get address of second argument
          mov   di,si                   ; - save address of copied argument
          call  docopy                  ; - copy it to stack
          push  ss                      ; - set es=ss
          pop   es                      ; - ...
not_rename:                             ; endif
        mov     dx,sp                   ; point to local buffer
        mov     si,2[bp]                ; restore registers
        mov     ax,4[bp]                ; restore function code
        push    ss                      ; set ds=ss
        pop     ds                      ; ...
        int     21h                     ; do int 21
        lea     sp,6[bp]                ; clean up stack
        pop     bp                      ; restore bp
        pop     edx                     ; restore edx
        pop     ds                      ; restore ds

done_int21:
;
;       must not modify any registers or the condition codes
;
        movsx   eax,ax                  ; sign-extend value in ax
exit_int21:
        mov     ss,_StackSelector       ; switch back to 32-bit stack
        mov     esp,_SaveSP             ; ...
        mov     es,_DataSelector        ; reload 32-bit data selector
        mov     ds,_DataSelector        ; ...
        db      66h                     ; return to 32-bit code
        ret                             ; ...

C_SETDTA:                               ; set DTA address
        pop     bx                      ; restore bx
        mov     _DTA_Addr,edx           ; save DTA address
        jmp     short done_int21        ; and exit

C_GETCWD:                               ; get current directory
        pop     bx                      ; restore bx
        push    esi                     ; save 32-bit pointer
        push    bp                      ; save bp
        mov     bp,sp                   ; get access to stack
        sub     sp,128                  ; allocate buffer (only 64 req'd)
        mov     si,sp                   ; point to buffer
        push    ds                      ; save ds
        push    ss                      ; set ds=ss
        pop     ds                      ; ...
        int     21h                     ; issue int 21
        pop     ds                      ; restore ds
        _if     nc                      ; if no error
          mov   esi,2[bp]               ; - get 32-bit pointer
          mov   es,_DataSelector        ; - point to 32-bit segment
          mov   bp,sp                   ; - point to buffer
          push  ax                      ; - save ax
          _loop                         ; - loop (copy cwd to 32-bit buffer)
            mov   al,[bp]               ; - - get character
            inc   bp                    ; - - advance pointer
            mov   es:[esi],al           ; - - store character
            inc   esi                   ; - - advance pointer
            cmp   al,0                  ; - - check for end of string
          _until  e                     ; - until done
          pop   ax                      ; - restore ax
          mov   bp,sp                   ; - point bp to right place
          add   bp,128                  ; - ...
          clc                           ; - clear carry
        _endif                          ; endif
        mov     sp,bp                   ; restore stack
        pop     bp                      ; restore bp
        pop     esi                     ; restore esi
        jmp     short done_int21        ; and exit

C_FIND:                                 ; find first/find next
        pop     bx                      ; restore bx
        push    ecx                     ; save ecx (attribute)
        push    edx                     ; save edx (filename address)
        push    edi                     ; save edi
        push    esi                     ; save esi
        mov     esi,_DTA_addr           ; get addr of 32-bit DTA
        lea     di,_DTA_Area            ; point to local DTA
        push    cx                      ; save attribute
        mov     cx,DTA_SIZE             ; get size of DTA
        push    ax                      ; save ax
        _loop                           ; loop
          mov   al,es:[esi]             ; - get byte from 32-bit
          inc   esi                     ; - ...
          mov   [di],al                 ; - store in local DTA
          inc   di                      ; - ...
          dec   cx                      ; - decrement count
        _until  e                       ; until done
        push    dx                      ; save dx
        lea     dx,_DTA_Area            ; point to local DTA
        mov     ah,1Ah                  ; set DTA to local DTA
        int     21h                     ; ...
        pop     dx                      ; restore dx
        pop     ax                      ; restore ax
        pop     cx                      ; restore cx (attribute)
        cmp     ah,4Eh                  ; if find first
        _if     e                       ; then
          push  ds                      ; - save ds
          push  bp                      ; - save bp
          push  0                       ; - push nullchar
          mov   bp,sp                   ; - point to stack frame
          sub   sp,256                  ; - allocate local buffer
          mov   si,sp                   ; - point to buffer
          call  docopy                  ; - copy name from edx to stack
          mov   dx,sp                   ; - point to filename
          push  ss                      ; - set ds=ss
          pop   ds                      ; - ...
          mov   ah,4Eh                  ; - set find first
          int   21h                     ; - issue interrupt
          mov   sp,bp                   ; - clean up stack
          pop   bp                      ; - pop 0
          pop   bp                      ; - restore bp
          pop   ds                      ; - restore ds
        _else                           ; else find next
          int   21h                     ; - issue interrupt
        _endif                          ; endif
        pushf                           ; save flags
        mov     esi,_DTA_addr           ; get addr of 32-bit DTA
        lea     di,_DTA_Area            ; point to local DTA
        mov     cx,DTA_SIZE             ; get size of DTA
        push    ax                      ; save ax
        _loop                           ; loop
          mov   al,[di]                 ; - get byte from local DTA
          inc   di                      ; - ...
          mov   es:[esi],al             ; - copy to 32-bit DTA
          inc   esi                     ; - ...
          dec   cx                      ; - decrement count
        _until  e                       ; until done
        pop     ax                      ; restore ax
        popf                            ; restore flags
        pop     esi                     ; restore esi
        pop     edi                     ; restore edi
        pop     edx                     ; restore edx
        pop     ecx                     ; restore ecx
        jmp     done_int21              ; and exit

;
;       ESI contains a pointer
;
C_EOPEN:                                ; extended file open/create
        pop     bx                      ; restore bx
        or      esi,esi                 ; check for NULL pointer
        jne     short esi_not_null      ; ...
        push    ds                      ; save ds
        mov     ds,si                   ; set ds:si to NULL
        int     21h                     ; do int 21
        pop     ds                      ; restore ds
        jmp     done_int21              ; and exit
;
;       ESI points to a filename
;
esi_not_null:
        push    ds                      ; save ds
        push    esi                     ; save esi
        push    edx                     ; save edx
        push    bp                      ; save bp
        push    ax                      ; save ax
        push    0                       ; push a NULLCHAR for end of buffer
        mov     edx,esi                 ; get address of filename
        mov     bp,sp                   ; point to stack frame
        sub     sp,256                  ; allocate local buffer
        mov     si,sp                   ; point to buffer
        call    docopy                  ; copy buffer from edx to stack
        mov     ax,2[bp]                ; restore function code
        mov     si,sp                   ; point to local buffer
        mov     dx,6[bp]                ; restore registers
        push    ss                      ; set ds=ss
        pop     ds                      ; ...
        int     21h                     ; do int 21
        lea     sp,4[bp]                ; clean up stack
        pop     bp                      ; restore bp
        pop     edx                     ; restore edx
        pop     esi                     ; restore esi
        pop     ds                      ; restore ds
        jmp     done_int21              ; and exit

__Int21_ endp

docopy  proc    near
        _loop                           ; loop
          cmp   si,bp                   ; - quit if at end of local buffer
          _quif e                       ; - ...
          mov   ax,es:[edx]             ; - get 2 characters
          mov   ss:[si],ax              ; - copy to local buffer
          add   edx,2                   ; - increment pointer
          add   si,2                    ; - ...
          cmp   ah,0                    ; - quit if end of string
          _quif e                       ; - ...
          cmp   al,0                    ; - check for end of string
        _until  e                       ; until end of string
        ret                             ; return
docopy  endp


_TEXT   ends
        end
