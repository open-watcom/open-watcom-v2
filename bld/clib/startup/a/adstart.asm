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
;* Description:  DOS 32-bit AutoCAD ADS startup code.
;*
;*****************************************************************************


;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;%                                                                 %
;%       This module was modified for use with AutoCAD's protect   %
;%       mode ADI and ADS.  AutoCAD is a registered trademark of   %
;%       Autodesk, Inc.                                            %
;%                                                                 %
;%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
;
;       This must be assembled using one of the following commands:
;               wasm adstart -fo=adsstart.obj -dADS -3s
;               wasm adstart -fo=adifstrt.obj -dPADI -3s
;               wasm adstart -fo=adiestrt.obj -dPADI -dEADI -3s
;
        name    adsstart

.387
.386p

include xinit.inc

comment @
        Modified for AutoLISP and protect mode ADI

        Expects to be called at starting address as a subroutine, rather
        than as an executable program from the operating system.  Stack
        and segment registers must be correctly set up; and some
        additional information is expected in these registers:

        ECX     Compatibility-check value.  Must be equal to (our
                locally defined symbol) chkval.  If it isn't, we assume
                we are erroneously being executed as an independent
                separate program.  This won't work, so we exit
                immediately to the system.

        DS:ESI  Pointer to structure containing initialization information.
                A minimum structure size of 8 bytes is mandatory (12 bytes
                if PADI is set).  The first 4 bytes of the structure must
                contain the number of bytes making up the structure.  The
                second 4 bytes must contain the initial heapsize of the
                child process, obtained from the loader in AutoCAD.  If
                PADI is set, the third 4 bytes must contain the physical
                address of the packet buffer in AutoCAD to be used for
                communication with protect mode ADI drivers.  All subsequent
                bytes are the business between AutoCAD and the loaded child
                process and may vary.

        The value we return is the argument to exit() which terminates us.
        We preserve no registers, not even SS and SP.

        Some protected mode ADI, ADS, and AutoLISP modifications are
        conditioned on the tag ACAD.

        Some code for ADS only is included only if the tag ADS is
        predefined (e.g., in the assembler command line).

        Code required for AutoCAD protected mode ADI drivers only is included
        by setting PADI in the assembler command line.
        @

ACAD   equ  1   ; Protect mode ADI and AutoLISP version of module.
minmem equ  4   ; Offset within the structure containing initialization
                ; information to the inital heap size of the child process.
ifdef   PADI
physadr equ 8   ; Offset within the structure containing initialization
                ; information to the physical address of a buffer shared
                ; between AutoCAD and a protect mode ADI driver.
endif   ; PADI


        assume  nothing

        extrn   __CMain         : near
        extrn   __InitRtns      : near
        extrn   __FiniRtns      : near
        extrn   __DOSseg__      : near

if      ACAD
ifdef   ADS
getinitinfo     =    adsi_getinitinfo
map_phys_mem    =    ads_map_phys_mem
endif   ; ADS
ifndef  PADI
        extrn   getinitinfo     : near          ; initialize child process
                                                ; loaded by AutoCAD
endif   ; !PADI
;
; map_phys_mem allows a 3rd party to install a routine to map physical
; memory between the stack and heap
;
extrn   map_phys_mem            : near
endif   ; ACAD

        extrn   _edata          : byte          ; end of DATA (start of BSS)
        extrn   _end            : byte          ; end of BSS (start of STACK)

        extrn   "C",_dynend         : dword
        extrn   "C",_curbrk         : dword
        extrn   "C",_psp            : word
        extrn   "C",_osmajor        : byte
        extrn   "C",_osminor        : byte
        extrn   "C",_STACKLOW       : dword
        extrn   "C",_STACKTOP       : dword
        extrn   "C",_child          : dword
        extrn   __no87              : byte
        extrn   "C",__uselfn        : byte
        extrn   "C",_Extender       : byte
        extrn   "C",_ExtenderSubtype: byte
        extrn   "C",_Envptr         : fword
        extrn   "C",__FPE_handler   : dword
        extrn   "C",_LpCmdLine      : dword
        extrn   "C",_LpPgmName      : dword

DGROUP group _NULL,_AFTERNULL,CONST,_DATA,DATA,XIB,XI,XIE,YIB,YI,YIE,_BSS,STACK

; this guarantees that no function pointer will equal NULL
; (WLINK will keep segment 'BEGTEXT' in front)

BEGTEXT  segment use32 word public 'CODE'
        assume  cs:BEGTEXT
___begtext label byte
forever label   near
        int     3h
        jmp    short forever
        public ___begtext
        assume  cs:nothing
BEGTEXT  ends

_TEXT   segment use32 word public 'CODE'

        assume  ds:DGROUP

_NULL   segment para public 'BEGDATA'
__nullarea label word
        db      01h,01h,01h,00h
        public  __nullarea
_NULL   ends

_AFTERNULL segment word public 'BEGDATA'
_AFTERNULL ends

CONST   segment word public 'DATA'
CONST   ends

XIB     segment word public 'DATA'
XIB     ends
XI      segment word public 'DATA'
XI      ends
XIE     segment word public 'DATA'
XIE     ends

YIB     segment word public 'DATA'
YIB     ends
YI      segment word public 'DATA'
YI      ends
YIE     segment word public 'DATA'
YIE     ends


_DATA   segment dword public 'DATA'

__x386_zero_base_selector dw 0  ; base 0 selector for X-32VM
        public  __x386_zero_base_selector
if      ACAD
chkval  equ     1234            ; magic interface-compatibility code
ifndef  ADS                     ; Avoid excess globals for ADS
        public   brkflg, stkflg
        public   info_off, info_sel
endif   ; ADS
        public  __fsavcw
ifdef   PADI
        public  cbufadr
        align 4
cbufadr  dd     0               ; ptr to common pg between ACAD and PADI
phys_adr  dd    0               ; physical address of common page
endif   ; PADI
brkflg   dd     0               ; set nonzero for Control C
stkflg   dd     0               ; set nonzero for stack overflow
info_off dd     0               ; Offset of initialization info struct
info_sel dw     0               ; Selector of initialization info struct
rtnaddr   df    0               ; Caller's return address.
min_mem   dd    0               ; inital driver heap size
sav_ds    dw    0
my_ds     dw    0
__fsavcw  dw    0               ; for saving AutoLISP's floating point cntl word
interr    db    "Incompatible program interface",0Dh,0Ah,"$"
endif   ; ACAD

_DATA   ends

DATA    segment word public 'DATA'
DATA    ends

_BSS          segment word public 'BSS'
_BSS          ends

if      ACAD
STACK_SIZE      equ     4000h
else
STACK_SIZE      equ     1000h
endif   ; ACAD

STACK   segment para stack 'STACK'
stk     label   word
        db      (STACK_SIZE) dup(?)
STACK   ends


        assume  nothing
        public  _cstart_
        public   __exit

        assume  cs:_TEXT

_cstart_ proc near
        jmp     short around

;
; copyright message
;
include msgrt32.inc
include msgcpyrt.inc

;
; miscellaneous code-segment messages
;
ConsoleName     db      "con",00h
NewLine         db      0Dh,0Ah

        dd      ___begtext      ; make sure dead code elimination
                                ; doesn't kill BEGTEXT

around: sti                             ; enable interrupts

        assume  ds:DGROUP

PSP_SEG equ     24h
ENV_SEG equ     2ch

if      ACAD
        ; Save AutoLISP's floating point control word, to be restored by
        ; adsi_farcl() before returning to AutoLISP
ifndef  EADI
        fnstcw  __fsavcw
endif

        cmp     ecx,chkval              ; Proper chkval argument?
        je      m00
        mov     edx,offset interr       ; "Incompatible program interface"
        mov     ah,9
        int     21h                     ; Print message.
        mov     ax,4C01h
        int     21h                     ; Exit to DOS.
m00:
        mov     info_off,esi            ; offset of initialization info struct
        mov     info_sel,dx             ; selector of info struct
        mov     es,dx
        mov     eax,es:[esi].minmem
        mov     min_mem,eax             ; initial heap size of driver
ifdef   PADI
        mov     eax,es:[esi].physadr
        mov     phys_adr,eax            ; phys addr of pg w/ PADI packet buffer
endif   ; PADI
        mov     ax,ds
        mov     es,ax
        pop     dword ptr rtnaddr       ; Save return address, offset
        pop     ax
        mov     word ptr rtnaddr+4,ax   ;    and segment.
        mov     my_ds,ds                ; Save DS for use in __GETDS()
endif   ; ACAD

        mov     ax,PSP_SEG              ; get segment address of PSP
        mov      _psp,ax                ; save segment address of PSP
        mov     es,ax                   ; point to PSP
        and     esp,0fffffffch          ; make sure stack is on a 4 byte bdry
        mov     ebx,esp                 ; get sp
        mov      _STACKTOP,ebx          ; set stack top
        mov      _curbrk,ebx            ; set first available memory location
;
;       get DOS & Extender version number
;
        ;mov    ebx,'PHAR'              ; set ebx to "PHAR"
        mov     ebx,50484152h           ; set ebx to "PHAR"
        sub     eax,eax                 ; set eax to 0
        mov     ah,30h
        int     21h                     ; modifies eax,ebx,ecx,edx
        mov     _osmajor,al
        mov     _osminor,ah
        mov     ecx,eax                 ; remember DOS version number
        shr     eax,16                  ; get top 16 bits of eax
        cmp     ax,'DX'                 ; if top 16 bits = "DX"
        sete    al                      ; then its Pharlap
        jne     not_pharlap             ; if its pharlap
        sub     bl,'0'                  ; - save major version number
        mov     al,bl                   ; - (was in ascii)
        xor     ah,ah                   ; - subtype
        mov     bx,14h                  ; - get value of Phar Lap data segment
        jmp     short know_extender     ; else
not_pharlap:                            ; - see if Rational DOS/4G
        mov     dx,78h                  ; - ...
        mov     ax,0FF00h               ; - ...
        int     21h                     ; - ...
        mov     bx,17h                  ; - get writeable code segment for Ergo
        cmp     al,0                    ; - ...
        je      short know_extender     ; - quit if not Rational DOS/4G
        mov     al,1                    ; - indicate Rational 32-bit Extender
        xor     ah,ah                   ; - assume zero base subtype
        mov     bx,ds                   ; - just use ds (FLAT model)
        mov      _psp,es                ; - save segment address of PSP
;
know_extender:                          ; endif
        mov     _Extender,al            ; record extender type
        mov     _ExtenderSubtype,ah     ; record extender subtype
;;      mov     es,bx                   ; get access to code segment
;;      mov     es:__saved_DS,ds        ; save DS value

;
;       copy command line into bottom of stack
;
        mov     es, _psp                ; point to PSP
        mov     edx,offset DGROUP:_end
        add     edx,0FH
        and     edx,0FFFFFFF0H
        mov     edi,81H                 ; DOS command buffer es:edi
        sub     ecx,ecx
        mov     cl,es:[edi-1]           ; get length of command
        cld                             ; set direction forward
        mov     al,' '
        repe    scasb
        lea     esi,-1[edi]
        mov     edi,edx
        mov     bx,es
        mov     dx,ds
        mov     ds,bx
        mov     es,dx                   ; es:edi is destination
        je      noparm
        inc     ecx
        rep     movsb
noparm: sub     al,al
        stosb                           ; store NULLCHAR
        xor     al,al                   ; assume no pgm name
        stosb                           ; . . .
        dec     edi                     ; back up pointer 1
        push    edi                     ; save pointer to pgm name
        mov     ds,dx                   ; restore ds
        push    ds                      ; save ds

        cmp     byte ptr  _Extender,1   ; if OS/386 or Rational
        jg      short pharlap           ; then
          mov   dx,PSP_SEG              ; - get PSP segment descriptor
          mov   ds,dx                   ; - ... into ds
          mov   dx,ds:[02ch]            ; - get environment segment into dx
          jmp   short haveenv           ; else
pharlap:mov   dx,ENV_SEG                ; - PharLap environment segment
haveenv:                                ; endif
        mov     es:word ptr _Envptr+4,dx ; save segment of environment area
        mov     ds,dx                   ; get segment addr of environment area
        sub     ebp,ebp                 ; assume "NO87" env. var. not present
        sub     esi,esi                 ; offset 0
        mov     es:dword ptr _Envptr,esi ; save offset of environment area
L1:     mov     eax,[esi]               ; get first 4 characters
        or      eax,2020h               ; map to lower case
        cmp     eax,37386f6eh           ; check for "no87"
        jne     short L2                ; skip if not "no87"
        cmp     byte ptr 4[esi],'='     ; make sure next char is "="
        jne     short L2                ; no
        inc     ebp                     ; - indicate "NO87" was present
L2:     cmp     byte ptr [esi],0        ; end of string ?
        lodsb
        jne     L2                      ; until end of string
        cmp     byte ptr [esi],0        ; end of all strings ?
        jne     L1                      ; if not, then skip next string
        lodsb
        inc     esi                     ; point to program name
        inc     esi                     ; . . .
;
;       copy the program name into bottom of stack
;
L3:     cmp     byte ptr [esi],0        ; end of pgm name ?
        movsb                           ; copy a byte
        jne     L3                      ; until end of pgm name
        pop     ds                      ; restore ds
        pop     esi                     ; restore address of pgm name
        mov     ebx,esp                 ; end of stack in data segment


        assume  ds:DGROUP
if      ACAD
 ifdef   EADI
        mov     bp,1                    ; force "NO87" env. var as present
 endif
endif   ; ACAD
        mov     eax,ebp
        mov     __no87,al               ; set state of "NO87" environment var
        and     __uselfn,ah             ; set "LFN" support status

        mov     _STACKLOW,edi           ; save low address of stack

if      ACAD

ifdef   PADI
        add     ebx,4095                ; round top of stack up a page
        shr     ebx,12                  ; get number of pages
        push    ebx                     ; store for later use
        push    ds
        pop     es
        mov     ah,4Ah                  ; set PADI memory size to smallest #
        int     21h                     ;   of pages ecompassing top of stack

        mov     ebx,phys_adr            ; get phys addr of pg w/ packet buffer
        mov     eax,250Ah
        mov     ecx,1
        int     21h                     ; map pg w/ pkt buf to end of new PADI
                                        ;   memory block, above top of stack
        mov     cbufadr,eax             ; store mapped offset as ptr to buf

        call    map_phys_mem            ; allow 3rd party a chance to map in
                                        ;   phys mem (returns # of pgs mapped)
        pop     ebx                     ; get # of pgs to top of stack
        inc     ebx                     ; add 1 for page mapped to end of seg
        add     ebx,eax                 ; add add'l pgs mapped by 3rd party
        shl     ebx,12                  ; get bottom of heap, in bytes
        mov     _dynend,ebx             ; set top of dynamic memory area
        mov     _curbrk,ebx             ; set bottom of dynamic memory area
else    ; PADI
        add     ebx,4095                ; round it up a page
        shr     ebx,12                  ; get number of pages
        push    ebx                     ; store for later use
        push    ds
        pop     es
        mov     ah,4Ah                  ; set memory size to smallest #
        int     21h                     ;   of pages encompassing top of stack

        call    map_phys_mem            ; allow 3rd party a chance to map in
                                        ;   phys mem (returns # of pgs mapped)
        pop     ebx                     ; Get # of pgs to top of stack
        add     ebx,eax                 ; If so, add it in
        shl     ebx,12                  ; Get bottom of heap, in bytes
        mov     _curbrk,ebx             ; set bottom of dynamic memory area
        mov     _dynend,ebx             ; set top of dynamic memory area
endif   ; PADI
        ;mov    ????,min_mem            ; initial size of heap
else    ; ACAD
        mov      _dynend,ebx            ; set top of dynamic memory area
endif   ; ACAD

        mov     ecx,offset DGROUP:_end  ; end of _BSS segment (start of STACK)
        mov     edi,offset DGROUP:_edata; start of _BSS segment
        sub     ecx,edi                 ; calc # of bytes in _BSS segment
        mov     dl,cl                   ; save bottom 2 bits of count in edx
        shr     ecx,2                   ; calc # of dwords
        sub     eax,eax                 ; zero the _BSS segment
        rep     stosd                   ; ...
        mov     cl,dl                   ; get bottom 2 bits of count
        and     cl,3                    ; ...
        rep     stosb                   ; ...

        mov     eax,offset DGROUP:_end  ; cmd buffer pointed at by EAX
        add     eax,0FH
        and     al,0F0H
        mov      _LpCmdLine,eax         ; save command line address
        mov      _LpPgmName,esi         ; save program name address
        mov     eax,0FFh                ; run all initalizers
        call    __InitRtns              ; call initializer routines

if      ACAD
ifndef  PADI
Comment @
        Take information from a structure in AutoCAD, pointed to by
        info_sel:info_off, and store it for use by routines in the
        program loaded by AutoCAD to which this module is linked.
        @
        movzx   eax,word ptr info_sel
        push    eax
        push    info_off
        call    getinitinfo
        add     esp,8
endif
endif

ifdef ADS
        push    es
        mov     cl,3
        mov     ax,2502h
        int     21h
        jc      noi3
        mov     ax,es
        verr    ax
        jnz     noi3
        cmp     ebx,8
        jb      noi3
        sub     ebx,8
        mov     eax,es:[ebx]
        cmp     eax,'DIVW'
        jne     noi3
        mov     eax,es:4[ebx]
        cmp     eax,'!!OE'
        jne     noi3
        int     3
noi3:   pop     es
endif

        sub     ebp,ebp                 ; ebp=0 indicates end of ebp chain
        call    __CMain
_cstart_ endp


if      ACAD
comment @
        For applications which are called as a subroutine from AutoCAD
        (namely AutoLISP), here is the ultimate exit point which returns
        back to AutoCAD.
        @
ifdef   PADI
        public   exit_to_acad
exit_to_acad:
        jmp   rtnaddr                   ; Return to AutoCAD
endif


;       Where we want attempted program exits to go:
ifdef   ADS
child_exit    equ    adsi_child_exit     ; Adhere to ADS naming convention
endif
              extrn  child_exit:near
endif   ; ACAD


__exit   proc near
        pop     eax                     ; get return code
        jmp     short   ok

        public   __do_exit_with_msg__

; input: ( char *msg, int rc ) always in registers

__do_exit_with_msg__:
        push    edx                     ; save return code
        push    eax                     ; save msg
        mov     edx,offset ConsoleName
        mov     ax,03d01h               ; write-only access to screen
        int     021h
        mov     bx,ax                   ; get file handle
        pop     edx                     ; get address of msg
        mov     esi,edx                 ; get address of msg
        cld                             ; make sure direction forward
L4:     lodsb                           ; get char
        cmp     al,0                    ; end of string?
        jne     L4                      ; no
        mov     ecx,esi                 ; calc length of string
        sub     ecx,edx                 ; . . .
        dec     ecx                     ; . . .
        mov     ah,040h                 ; write out the string
        int     021h                    ; . . .
        mov     edx, offset NewLine     ; write out the new line
        mov     ecx, sizeof NewLine     ; . . .
        mov     ah,040h                 ; . . .
        int     021h                    ; . . .
        pop     eax                     ; get return code
ok:

        push    eax                     ; save return code
        xor     eax,eax                 ; run finalizers
        mov     edx,FINI_PRIORITY_EXIT-1; less than exit
        call    __FiniRtns              ; call finalizer routines
        pop     eax                     ; restore return code

if      ACAD
        jmp     child_exit              ; do something more appropriate
else    ; ACAD
        mov     ah,04cH                 ; DOS call to exit with return code
        int     021h                    ; back to DOS
endif   ; ACAD

__exit   endp


__null_FPE_rtn proc near
        ret                             ; return
__null_FPE_rtn endp

        public  __GETDS
__GETDS proc    near
public "C",__GETDSStart_
__GETDSStart_ label byte
        mov     ds,cs:my_ds             ; load saved DS value
        ret
public "C",__GETDSEnd_
__GETDSEnd_ label byte
__GETDS endp

_TEXT   ends

        end     _cstart_
