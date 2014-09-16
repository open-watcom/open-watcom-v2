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
;* Description:  16-bit DPMI stub loader.
;*
;*****************************************************************************


        name    loader16

.386
include struct.inc

        DOSSEG

        extrn   __Int21 : near
        extrn   __fInt21: near
        extrn   _end    : byte

DGROUP  group   _DATA,STACK

_DATA   segment use16 dword public 'DATA'
        public  BaseAddr
        public  MemHandle
        public  CodeLoadAddr
        public  CodeEntryPoint
        public  _CodeSelector
        public  _DataSelector
        public  _StackSelector
        public  _16BitSelector
        public  _EntryStackSave
        public  RM_BufSeg
        public  PM_BufSeg
        public  RM_BufSize
        public  IsDBCS
        public  _PSP

BaseAddr        dd      0
MemHandle       dw      0,0
CodeLoadAddr    dd      0
CodeEntryPoint  dd      0
_CodeSelector   dw      0
_DataSelector   dw      0
_StackSelector  dw      0
_16BitSelector  dw      0
BreakFlagAddr   dw      0       ; offset into 32-bit segment to break flag
_EntryStackSave dw      0,0
RM_BufSeg       dw      0       ; real-mode segment of buffer
PM_BufSeg       dw      0       ; protect-mode selector of real-mode buffer
RM_BufSize      dw      0       ; size of real-mode buffer in bytes
IsDBCS          dw      0       ; DBCS indicator flag
_PSP    dw      0       ; selector pointing to our PSP
Ecantopen db    "Error opening '%s' rc=%d",0Dh,0Ah,0
Jcantopen db    "オープン・エラー '%s' rc=%d",0Dh,0Ah,0
_DATA   ends

STACK segment dword stack 'STACK' use16
stk     db      8192 dup(?)
STACK ends


_TEXT   segment use16 dword public 'CODE'
        assume  cs:_TEXT
        assume  ds:DGROUP

LOADER_SUCCESS          equ     0
LOADER_CANT_OPEN_EXE    equ     1
LOADER_INVALID_EXE      equ     2
LOADER_READ_ERROR       equ     3
LOADER_NOT_ENOUGH_MEMORY equ    4

OS_DOS32        equ     0
OS_WIN          equ     3

        public  Signature
        public  W32start
        public  W32size
Signature dw    0,0                     ; should be 'FC'
W32start dw     0,0                     ; offset to start of W32 file
W32size  dd     0                       ; size to read
Relocs   dd     0                       ; offset to start of relocs
MemSize  dw     0,0                     ; amount of memory to allocate
W32_EIP  dd     0                       ; starting EIP value
StackSize dd    10000h                  ; stack size
        dw      0ffffh
        dw      0ffffh
        dw      0ffffh
        dw      0ffffh

        public  _DGroupSelector
_DGroupSelector dw      0               ; in CODE segment


CtrlBreakRtn proc near
        push    ds                      ; save ds
        push    bx                      ; save bx
        mov     ds,_DGroupSelector      ; get access to our DGROUP
        mov     bx,_StackSelector       ; get 32-bit stack selector
        or      bx,bx                   ; if selector still valid
        je      short skipbreak         ; then
        push    bx                      ; - save selector
        mov     bx,BreakFlagAddr        ; - get address of break flag
        pop     ds                      ; - load 32-bit selector
        mov     byte ptr [bx],1         ; - set break flag
skipbreak:
        pop     bx                      ; restore bx
        pop     ds                      ; restore ds
        iret                            ; return from interrupt
CtrlBreakRtn endp


PrintDec proc   near
        sub     dx,dx                   ; zero high word
        mov     cx,10                   ; divisor
        div     cx                      ; calculate low order digit
        push    dx                      ; save remainder
        or      ax,ax                   ; if quotient not zero
        _if     ne                      ; then
          call  PrintDec                ; - recurse
        _endif                          ; endif
        pop     dx                      ; get digit
        add     dl,'0'                  ; make printable
        mov     ah,2                    ; print it
        int     21h                     ; ...
        ret                             ; return
PrintDec endp
;
;  input: DS:SI - address of format string
;         additional arguments on the stack
;
PrintMsg proc   near
        push    bp                      ; save bp
        mov     bp,sp                   ; point to parms
        add     bp,4                    ; ...
        push    cx                      ; save cx
        _loop                           ; loop
          mov   dl,[si]                 ; - get character
          cmp   dl,0                    ; - quit if end of string
          _quif e                       ; - ...
          inc   si                      ; - advance to next character
          _guess                        ; - guess: ordinary character
            cmp   dl,'%'                ; - - quit if escape character
            _quif e                     ; - - ...
            mov   ah,2                  ; - - display character
            int   21h                   ; - - ...
          _admit                        ; - guess: %s
            mov   al,[si]               ; - - get format specifier
            inc   si                    ; - - ...
            cmp   al,'s'                ; - - quit if not '%s'
            _quif ne                    ; - - ...
            push  si                    ; - - save si
            push  ds                    ; - - save ds
            lds   si,[bp]               ; - - get string pointer
            add   bp,4                  ; - - ...
            _loop                       ; - - loop
              mov   dl,[si]             ; - - - get character
              cmp   dl,0                ; - - - quit if end of string
              _quif e                   ; - - - ...
              inc   si                  ; - - - advance to next character
              mov   ah,2                ; - - - display character
              int   21h                 ; - - - ...
            _endloop                    ; - - endloop
            pop   ds                    ; - - restore ds
            pop   si                    ; - - restore si
          _admit                        ; - guess: %d
            cmp   al,'d'                ; - - quit if not '%d'
            _quif ne                    ; - - ...
            mov   cx,[bp]               ; - - get value
            add   bp,2                  ; - - ...
            or    cx,cx                 ; - - if number is negative
            _if   s                     ; - - then
              mov   dl,'-'              ; - - - output '-'
              mov   ah,2                ; - - - ...
              int   21h                 ; - - - ...
              neg   cx                  ; - - - make number positive
            _endif                      ; - - endif
            mov   ax,cx                 ; - - get value to convert
            call  PrintDec              ; - - print decimal number
          _endguess                     ; - endguess
        _endloop                        ; endloop
        pop     cx                      ; restore cx
        pop     bp                      ; restore bp
        ret                             ; return
PrintMsg endp

        public  Loader16
Loader16 proc   near
;
;       at this point ES now contains a selector pointing to the PSP
;
        mov     _PSP,es                 ; save PSP selector
        call    AllocSels               ; allocate selectors
        call    Init16Sels              ; initialize 16-bit selectors
        push    _DataSelector           ; save 32-bit data selector
        push    ds                      ; save ds
        mov     si,es:[2Ch]             ; get selector of environment area
        mov     _DataSelector,si        ; point 32-bit selector to env area
        mov     ds,si                   ; point to environment area
        sub     si,si                   ; ...
        cld                             ; clear direction flag
        _loop                           ; loop
          _loop                         ; - loop find end of string
            lodsb                       ; - - get char
            cmp  al,0                   ; - - check for end of string
          _until e                      ; - until end of string
          mov   al,[si]                 ; - get char
          cmp   al,0                    ; - check for end of area
        _until  e                       ; until end of area
        lea     si,3[si]                ; point to our file name
        pop     ds                      ; restore ds
        sub     edx,edx                 ; zero full 32-bit register
        mov     dx,si                   ; point to file name in env area
        mov     ax,3D00h                ; open file for read
        call    __Int21                 ; ...
        pop     _DataSelector           ; restore 32-bit data selector
        _if     c                       ; if error opening file
          push  ax                      ; - push return code
          mov   es,_PSP                 ; - point to environment area
          mov   es,es:[2Ch]             ; - ...
          push  es                      ; - push address of filename
          push  si                      ; - ...
          mov   si,offset DGROUP:Ecantopen; - ...
          cmp   IsDBCS,0                ; - if not english
          _if   ne                      ; - then
            mov si,offset DGROUP:Jcantopen; - - ...
          _endif                        ; - endif
          call  PrintMsg                ; - display message
          add   sp,6                    ; - remove parms
          call  FreeSels                ; - free the selectors
          mov   ax,4CFFh                ; - exit
          int   21h                     ; - ...
        _endif                          ; endif
        mov     bx,ax                   ; get file handle
        call    __Loader                ; load program
        _guess                          ; guess:
          mov   _EntryStackSave+0,sp    ; -  save 16-bit stack pointer
          mov   _EntryStackSave+2,ss    ; - ...
          mov   es,_StackSelector       ; - point to 32-bit stack
          mov   edi,10000h              ; - ...
          push  ds                      ; - save ds
          mov   ds,_PSP                 ; - point to environment area
          mov   ds,ds:[2Ch]             ; - ...
          sub   esi,esi                 ; - zero 32-bit offset
          sub   ecx,ecx                 ; - zero length
          mov   cx,si                   ; - save start offset
          _loop                         ; - loop find length of env area
            _loop                       ; - - loop find end of string
              lodsb                     ; - - - get char
              cmp  al,0                 ; - - - check for end of string
            _until e                    ; - - until end of string
            lodsb                       ; - - get char
            cmp   al,0                  ; - - check for end of area
          _until  e                     ; - until end of area
          add   si,2                    ; - point to start of filename
          _loop                         ; - loop (find end of program name)
            lodsb                       ; - - get character
            cmp   al,0                  ; - - check for end of string
          _until  e                     ; - until end of program name
          sub   si,cx                   ; - calc length of area
          xchg  cx,si                   ; - ecx=length, si=start offset
          sub   edi,ecx                 ; - allocate space from stack
          and   di,0FFFCh               ; - round down to multiple of 4
          mov   ebx,edi                 ; - remember addr of environment
          rep   movsb                   ; - copy environment to stack
          pop   ds                      ; - restore ds
          mov   fs,_StackSelector       ; - point to 32-bit stack
          mov   edi,ebx                 ; - ...
          mov   es,_PSP                 ; - point to command line
          mov   si,81h                  ; - ...
          push  bx                      ; - save bx
          mov   bl,es:-1[si]            ; - get length of command line
          xor   bh,bh                   ; - zero high byte
          mov   es:[si+bx],bh           ; - put null char on end of cmd
          pop   bx                      ; - restore bx
          call  copyname                ; - copy cmdline to 32-bit stack
          mov   ecx,edi                 ; - save address of cmdline
          mov   esi,ebx                 ; - get address of environment area
          _loop                         ; - loop (find entry with $=)
            _loop                       ; - - loop find end of string
              mov  al,fs:[esi]          ; - - - get char
              inc  esi                  ; - - - ...
              cmp  al,0                 ; - - - check for end of string
            _until e                    ; - - until end of string
            mov   al,fs:[esi]           ; - - get char
            cmp   al,0                  ; - - check for end of area
          _until  e                     ; - until end of area
          add   esi,3                   ; - point to file name
          sub   di,4                    ; - allocate space for breakflag
          mov   BreakFlagAddr,di        ; - save breakflag address
          push  ds                      ; - save ds
          push  cs                      ; - set ds=cs
          pop   ds                      ; - ...
          mov   dx,offset CtrlBreakRtn  ; - point to break routine
          mov   ax,2523h                ; - set interrupt vector
          int   21h                     ; - ...
          pop   ds                      ; - restore ds
          mov   ss,_StackSelector       ; - switch to 32-bit stack
          mov   esp,edi                 ; - ...
          sub   eax,eax                 ; - zero reg
          mov   [esp],eax               ; - set break flag to 0
          mov   ax,_CodeSelector        ; - push address of entry point
          push  eax                     ; - ...
          push  CodeEntryPoint          ; - 28:...
          sub   eax,eax                 ; - 24: max_handle
          push  eax                     ; - ...
          mov   ax,IsDBCS               ; - get DBCS indicator flag
          push  eax                     ; - 20:...
          sub   eax,eax                 ; - set __Copyright to NULL
          push  eax                     ; - 16:...
          push  edi                     ; - 12:push address of break flag
          push  ebx                     ; - 8: push addr of environment area
          push  ecx                     ; - 4: push addr of cmd line
          push  esi                     ; - 0: push addr of program name
          mov   edi,esp                 ; - point to parameter block
          sub   ecx,ecx                 ; - set stack low
          sub   edx,edx                 ; - set bx:edx = &__fInt21
          mov   dx,offset __fInt21      ; - ...
          mov   bx,cs                   ; - ...
          mov   ax,_DataSelector        ; - load 32-bit selectors
          mov   ds,ax                   ; - ...
          mov   es,ax                   ; - ...
          mov   ah,OS_WIN               ; - indicate Windows
          call  fword ptr 28[di]        ; - call 32-bit entry point
          mov   ds,cs:_DGroupSelector   ; - load our DGROUP
          sub   dx,dx                   ; - zero other segment registers
          mov   es,dx                   ; - ...
          mov   fs,dx                   ; - ...
          mov   gs,dx                   ; - ...
          lss   sp,dword ptr _EntryStackSave ; - switch back to 16-bit stack
          push  ax                      ; - save return code
          call  FreeSels                ; - free selectors
          mov   di,MemHandle+0          ; - get memory block handle
          mov   si,MemHandle+2          ; - ...
          mov   ax,0502h                ; - free 32-bit memory block
          int   31h                     ; - ...
          pop   ax                      ; - restore return code
        _endguess                       ; endguess
        mov     AH,4Ch                  ; exit
        int     21h                     ; ...
Loader16 endp

        public  __Loader
__Loader proc   near
        push    bp                      ; save bp
        _guess                          ; guess: no errors
          call  init32                  ; - initialize 32-bit segments
          _quif c                       ; - quit if error
          call  load32                  ; - load in 32-bit program
          call  reloc32                 ; - apply relocations
        _endguess                       ; endguess
        pop     bp                      ; restore bp
        ret                             ; return
__Loader endp

load32  proc    near                    ; load 32-bit program into memory
        mov     dx,W32start+0           ; get offset to start of W32 code
        mov     cx,W32start+2           ; ...
        mov     ax,4200h                ; seek to start of W32 file
        call    __Int21                 ; ...
        mov     ecx,W32size             ; get length of W32 file
        mov     edx,CodeLoadAddr        ; get address to load code into
        mov     ah,3Fh                  ; read
        call    __Int21                 ; perform 32-bit read
        mov     ah,3Eh                  ; close the file
        call    __Int21                 ; ...
        ret                             ; return
load32  endp

reloc32 proc    near                    ; apply relocations
        push    ds                      ; save ds
        mov     esi,CodeLoadAddr        ; get load address of code
        add     esi,Relocs              ; add offset to start of relocs
        mov     edx,CodeLoadAddr        ; get relocation factor
        mov     ds,_DataSelector        ; get selector for 32-bit segment
        sub     eax,eax                 ; zero whole reg
        _loop                           ; loop (apply relocations)
          mov   cx,[esi]                ; - get count
          or    cx,cx                   ; - quit if end of relocations
          _quif e                       ; - ...
          mov   di,2[esi]               ; - get page number
          shl   edi,16                  ; - shift to top
          mov   di,4[esi]               ; - get offset of first reloc in page
          add   esi,6                   ; - point to next reloc
          add   edi,edx                 ; - calc. address of dword to reloc
          _loop                         ; - loop
            add   [edi],edx             ; - - apply 32-bit relocation
            dec   cx                    ; - - decrement count
            _quif e                     ; - - quit if done
            mov   ax,[esi]              ; - - get offset to next reloc
            add   esi,2                 ; - - advance to next relocation
            add   edi,eax               ; - - calc. address of next reloc
          _endloop                      ; - endloop
        _endloop                        ; endloop
        pop     ds                      ; restore ds
        ret                             ; return
reloc32 endp


init32  proc    near                    ; initialize 32-bit segments
        push    bx                      ; save file handle
        _guess                          ; guess: no errors
          mov   cx,MemSize+0            ; - get amount of memory to allocate
          mov   bx,MemSize+2            ; - ...
          inc   bx                      ; - +64K for stack
          mov   ax,0501h                ; - DPMI allocate memory block
          int   31h                     ; - ...
          _quif c                       ; - quit if error
          ; bx:cx = linear address of allocated memory
          ; si:di = memory block handle
          mov   MemHandle+0,di          ; - save Memory Block Handle
          mov   MemHandle+2,si          ; - ...
          push  bx                      ; - save BaseAddr
          push  cx                      ; - ...
          pop   edx                     ; - ...
          mov   BaseAddr,edx            ; - ...
          mov   edx,10000h              ; - add 64K for stack
          mov   CodeLoadAddr,edx        ; - set CodeLoadAddr
          add   edx,W32_EIP             ; - calc CodeEntryPoint
          mov   CodeEntryPoint,edx      ; - ...
          mov   dx,cx                   ; - copy bx:cx to cx:dx
          mov   cx,bx                   ; - ...
          mov   bx,_CodeSelector        ; - set CODE segment
          push  cx                      ; - save cx
          mov   ax,9                    ; - set descriptor access rights
          mov   cx,cs                   ; - get CS in order to get CPL
          and   cx,3                    ; - isolate CPL
          shl   cl,5                    ; - shift into place for DPL
          or    cx,0C09Bh               ; - 4GB;32-bit;CODE
          int   31h                     ; - ...
          pop   cx                      ; - restore cx
          call  setselector             ; - ...
          mov   bx,_DataSelector        ; - set DATA segment
          push  cx                      ; - save cx
          mov   ax,9                    ; - set descriptor access rights
          mov   cx,cs                   ; - get CS in order to get CPL
          and   cx,3                    ; - isolate CPL
          shl   cl,5                    ; - shift into place for DPL
          or    cx,0C093h               ; - 4GB;32-bit;DATA
          int   31h                     ; - ...
          pop   cx                      ; - restore cx
          call  setselector             ; - ...
          clc                           ; - indicate no error
        _admit                          ; admit: error
          call  FreeSels                ; - free 32-bit selectors
          jmp   MsgNoMemory             ; - display no memory msg and exit
        _endguess                       ; endguess
        pop     bx                      ; restore file handle
        ret                             ; return
init32  endp

Init16Sels proc near                    ; initialize 16-bit selectors
        push    cx                      ; save cx
        _guess                          ; guess: no errors
          mov   bx,_16BitSelector       ; - initialize 16 bit alias segment
          call  set64Ksel               ; - ...
          mov   bx,PM_BufSeg            ; - initialize PM_BufSeg selector
          call  set64Ksel               ; - ...
          mov   ax,7                    ; - set segment base addr
          mov   bx,PM_BufSeg            ; - ...for PM_BufSeg
          mov   dx,RM_BufSeg            ; - get real-mode segment of buffer
          mov   cx,dx                   ; - ...
          shl   dx,4                    ; - multiply by 16
          shr   cx,12                   ; - ...
          int   31h                     ; - set the segment base address
          mov   bx,cs                   ; - get base address of our CODE seg
          mov   ax,6                    ; - ...
          int   31h                     ; - ...
          mov   bx,_16BitSelector       ; - point 16 bit alias segment to CS
          mov   ax,7                    ; - ...
          int   31h                     ; - ...
          push  es                      ; - save es
          mov   es,bx                   ; - point es to our CODE segment
          mov   bx,offset _DgroupSelector; - get addr of DGROUP selector
          mov   es:[bx],ds              ; - save our DS value in CODE segment
          pop   es                      ; - restore es
        _endguess                       ; endguess
        pop     cx                      ; restore cx
        ret                             ; return
Init16Sels endp

        public  SetupSelectors
SetupSelectors proc near
;       no need to change the limit of _CodeSelector
;               just change its base address
;       must change base address, limit (and possibly access rights)
;               of _DataSelector
        ret                             ; return
SetupSelectors endp

; input:
;       bx    = selector
;       cx:dx = linear base address of memory block
;       also need size of the memory
;       must also set the access rights
;
setselector proc near                   ; initialize selector
        push    cx                      ; save linear base address
        push    dx                      ; ...
        _guess                          ; guess: no errors
          mov   ax,7                    ; - set segment base address
          int   31h                     ; - ...
          _quif c                       ; - quit if error
          mov   cx,0ffffh               ; - set limit to 4GB
          mov   dx,cx                   ; - ...
          mov   ax,8                    ; - set segment limit
          int   31h                     ; - ...
          _quif c                       ; - quit if error
        _admit                          ; admit: error
          jmp   MsgDPMI_Error           ; - exit
        _endguess                       ; endguess
        pop     dx                      ; restore linear base address
        pop     cx                      ; ...
        ret                             ; return
setselector endp

; input:
;       bx - selector
;
set64Ksel proc  near                    ; set 64K selector
        _guess                          ; guess
          push  cx                      ; - save cx
          mov   ax,9                    ; - set descriptor access rights
          mov   cx,cs                   ; - get CS in order to get CPL
          and   cx,3                    ; - isolate CPL
          shl   cl,5                    ; - shift into place for DPL
          or    cx,00093h               ; - 64K;16-bit;DATA
          int   31h                     ; - ...
          mov   dx,0ffffh               ; - set limit to 64K
          sub   cx,cx                   ; - ...
          mov   ax,8                    ; - set segment limit
          int   31h                     ; - ...
          pop   cx                      ; - restore cx
        _endguess                       ; endguess
        ret                             ; return
set64Ksel endp

AllocSels proc  near
        mov     cx,4                    ; allocate 4 selectors
        xor     ax,ax                   ; CODE, DATA, 16-bit Alias
        int     31h                     ; ...and PM_BufSeg
        _if     c                       ; if error
          jmp   MsgNoSelectors          ; - exit
        _endif                          ; endif
        push    bx                      ; save bx
        mov     cx,ax                   ; save base selector
        mov     _CodeSelector,ax        ; save CODE selector
        mov     ax,3                    ; get selector increment value
        int     31h                     ; ...
        add     cx,ax                   ; calc DATA selector
        mov     _DataSelector,cx        ; save it
        mov     _StackSelector,cx       ; save it
        add     cx,ax                   ; calc 16-bit alias selector
        mov     _16BitSelector,cx       ; ...
        add     cx,ax                   ; calc PM_BufSeg selector
        mov     PM_BufSeg,cx            ; ...
        pop     bx                      ; restore bx
        ret                             ; return
AllocSels endp

        public  FreeSels
FreeSels proc   near                    ; free 32-bit selectors
        push    bx                      ; save bx
        mov     ax,1                    ; free CODE selector
        mov     bx,_CodeSelector        ; ...
        int     31h                     ; ...
        mov     ax,1                    ; free DATA selector
        mov     bx,_DataSelector        ; ...
        int     31h                     ; ...
        mov     ax,1                    ; free STACK selector
        mov     bx,_StackSelector       ; ...
        mov     _StackSelector,0        ; zero STACK selector
        cmp     bx,_DataSelector
        je      short free16
        int     31h                     ; ...
free16: mov     ax,1                    ; free 16 bit alias selector
        mov     bx,_16BitSelector       ; ...
        int     31h                     ; ...
        mov     ax,1                    ; free protect-mode buffer selector
        mov     bx,PM_BufSeg            ; ...
        int     31h                     ; ...
        pop     bx                      ; restore bx
        ret                             ; return
FreeSels endp


        public  copyname
copyname proc   near                    ; copy string to 32-bit stack
        push    bx                      ; save bx
        sub     bx,bx                   ; calc length of string
        _loop                           ; loop
          cmp     byte ptr es:[si+bx],0 ; - check for null character
          _quif e                       ; - quit if end of cmd
          inc     bx                    ; - increment length
        _endloop                        ; endloop
        add     bx,4                    ; round up to multiple of 4
        and     bl,0fch                 ; ...
        sub     di,bx                   ; allocate space on 32-bit stack
        mov     bx,di                   ; get address
        _loop                           ; loop
          mov     al,es:[si]            ; - get character
          mov     fs:[bx],al            ; - store on 32-bit stack
          inc     bx                    ; - increment pointer
          inc     si                    ; - ...
          cmp     al,0                  ; - check for end of cmd line
        _until  e                       ; until end of cmd line
        pop     bx                      ; restore bx
        ret                             ; return
copyname endp

DispMsgAndExit proc     near
        pop     dx                      ; get address of msg in code segment
        push    cs                      ; set DS=CS
        pop     ds                      ; ...
        mov     ah,9                    ; display message
        int     21h                     ; ...
        mov     ax,4CFFh                ; exit with error
        int     21h                     ; ...
DispMsgAndExit endp

MsgNoMemory proc near
        cmp     IsDBCS,0                ; if english
        _if     e                       ; then
          call  DispMsgAndExit
          db    "Error allocating memory for 32-bit program",0Dh,0Ah,"$"
        _endif                          ; endif
        call    DispMsgAndExit          ; display japanese msg
        db      "32ビット・プログラム用のメモリ割り付けエラー",0Dh,0Ah,"$"
MsgNoMemory endp

MsgDPMI_Error proc near
        cmp     IsDBCS,0                ; if english
        _if     e                       ; then
          call  DispMsgAndExit
          db    "DPMI error modifying selectors",0Dh,0Ah,"$"
        _endif                          ; endif
        call    DispMsgAndExit          ; display japanese msg
        db      "セレクタ変更に関する DPMI エラー",0Dh,0Ah,"$"
MsgDPMI_Error endp

MsgNoSelectors proc near
        cmp     IsDBCS,0                ; if english
        _if     e                       ; then
          call  DispMsgAndExit
          db    "Error allocating protect-mode selectors",0Dh,0Ah,"$"
        _endif                          ; endif
        call    DispMsgAndExit          ; display japanese msg
        db      "プロテクト・モード・セレクタ割り付けエラー",0Dh,0Ah,"$"
MsgNoSelectors endp

_TEXT   ends
        end
