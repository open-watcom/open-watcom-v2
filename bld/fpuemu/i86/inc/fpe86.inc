
        extrn   __GETDS : near

_DOS  = 1
_OS2  = 2

 _BP    equ     BP
 _SP    equ     SP
 _AX    equ     AX
 _BX    equ     BX
 _CX    equ     CX
 _DX    equ     DX
 _SI    equ     SI
 _DI    equ     DI

  NC = '$'

ifndef NDEBUG
public  badop,prtnyi,my87,modrm,memop,stkop,stkop1
endif

sarea   struc
control dw              0                       ; CW
status  dw              0                       ; SW
tags    dw              0                       ; TAG
iptr    dw              0,0                     ; instruction pointer
dptr    dw              0,0                     ; data operand pointer
fpstack db              80 dup(0)               ; 8 tbyte reals for stack
fptemp  db              10 dup(0)               ; temporary tbyte real
precrtn dw              0                       ; address of precision routine
sarea   ends

NDP_SIZE        equ     94
ENV_SIZE        equ     14

include fpubits.inc

ifdef __QNX__
        extrn   "C",__FPEhandler   : near;
endif

_DATA segment word public 'DATA'
ifndef __QNX__
        extrn   "C",__FPE_handler  : dword
my87            sarea           <>
endif
ifndef NDEBUG
nyimsg          db ": not yet implemented",0dH,0aH,NC
illopmsg        db "Illegal instruction encountered by 8087 emulator",0dH,0aH,NC
endif
_DATA ends

DGROUP  group   _DATA

assume  ds:DGROUP

include consts.inc
include tentab.inc
include tagtab.inc
include fpucc.inc

ifdef QNX32
S_FL    =       38H
S_CS    =       34H
S_IP    =       30H
S_AX    =       2CH
S_CX    =       28H
S_DX    =       24H
S_BX    =       20H
S_SP    =       1CH
S_BP    =       18H
S_SI    =       14H
S_DI    =       10H
S_DS    =       0CH
S_ES    =       08H
S_FS    =       04H
S_GS    =       00H

else

ifdef __PROTMODE__
ifndef __QNX__
DPMI_SS =       22H
DPMI_SP =       20H
S_FL    =       1EH
S_CS    =       1CH
S_IP    =       1AH
;
; 3 words in here for DPMI (error code, return CS, return IP)
;
else    ; __QNX__
S_FL    =       18H
S_CS    =       16H
S_IP    =       14H
endif
else    ; not __PROTMODE__
S_FL    =       18H
S_CS    =       16H
S_IP    =       14H
endif

S_AX    =       12H
S_CX    =       10H
S_DX    =       0EH
S_BX    =       0CH
S_SP    =       0AH
S_BP    =       08H
S_SI    =       06H
S_DI    =       04H
S_DS    =       02H
S_ES    =       00H
endif
S_MY87  =       S_SP

get_st0 macro
        mov     ax,word ptr [bx].status ; - di gets idx of top of stack
        mov     al,ah                   ; - move to bottom
        and     ax,0038h                ; - isolate register number
        mov     di,ax                   ; - save 8 * register number
        shr     ax,1                    ; - calc 2 * register number
        shr     ax,1                    ; - ...
        add     di,ax                   ; - calc 10 * register number
        endm


jmptab  macro   a,b,c,d,e,f,g,h
        local   table
        mov     bl,dh
        and     bx,7
        shl     bx,1
        jmp     word ptr cs:table[bx]
table   dw      a
        dw      b
        dw      c
        dw      d
        dw      e
        dw      f
        dw      g
        dw      h
        endm

_lea    macro   reg,what
        IFIDN   <what>,<st>
          lea     reg,[di+bp].fpstack
        ELSE
          IFIDN   <what>,<st(i)>
            lea     reg,[si+bp].fpstack
          ELSE
           .ERRDIF <what>,<temp>
           lea     reg,[bp].fptemp
          ENDIF
        ENDIF
        endm

saveflg macro                                   ; MACRO
        inc     ax                              ; -1,0,1,2 -> 0,1,2,3
        mov     dx,ds:[bp].status               ; get status word
        and     dx,NOT SW_CC                    ; turn off C0,C1,C2,C3
        mov     bx,ax                           ; turn on new bits
        or      dh,byte ptr cs:fpbits[bx]       ; ...
        mov     ds:[bp].status,dx               ; save in status word
        endm

savecc  macro                                   ; MACRO
        and     ax,7                            ; keep low order 3 bits of quot
        mov     dx,ds:[bp].status               ; get status word
        and     dx,NOT SW_CC                    ; turn off C0,C1,C2,C3
        mov     bx,ax                           ; turn on new bits
        or      dh,cs:ccbits[bx]                ; ...
        mov     ds:[bp].status,dx               ; save in status word
        endm

tag_st0 macro new_tag
        IFDIF   <new_tag>,<t_empty>
          mov   bx,cs:tagtab[di].t_off          ; get turn off mask
          and   ds:[bp].tags,bx                 ; turn off the bits
        ENDIF
        IFDIF   <new_tag>,<t_valid>
          mov     bx,cs:tagtab[di].&new_tag     ; get new bits
          or      ds:[bp].tags,bx               ; turn on new bits
        ENDIF
        endm

tag_sti macro new_tag
        IFDIF   <new_tag>,<t_empty>
          mov   bx,cs:tagtab[si].t_off          ; get turn off mask
          and   ds:[bp].tags,bx                 ; turn off the bits
        ENDIF
        IFDIF   <new_tag>,<t_valid>
          mov     bx,cs:tagtab[si].&new_tag     ; get new bits
          or      ds:[bp].tags,bx               ; turn on new bits
        ENDIF
        endm

pushstk macro
        decstp
        ; if full entry, to incstp and exception!
        ; mark as valid, zero or invalid
        tag_st0 t_valid
        endm

decstp  macro                                   ; MACRO
        mov     di,cs:tentab[di].t_prev         ; dec stack pointer
        mov     bx,cs:tentab[di].t_stktop       ; update top of stack pointer
        and     ds:[bp].status,NOT SW_ST        ;...
        or      word ptr ds:[bp].status,bx      ; put back into SW_ST
        endm

popstk  macro
        ; if empty entry, take exception
        ; mark as empty
        tag_st0 t_empty
        incstp
        endm

incstp  macro                                   ; MACRO
        mov     di,cs:tentab[di].t_next         ; inc stack pointer
        mov     bx,cs:tentab[di].t_stktop       ; update top of stack pointer
        and     ds:[bp].status,NOT SW_ST        ;...
        or      word ptr ds:[bp].status,bx      ; put back into SW_ST
        endm

done    macro
        jmp     exit                            ; done instruction
        endm

popstk_done     macro
        jmp     popstk_exit                     ; popstk then exit
        endm

saveflg_done    macro
        jmp     saveflg_exit                    ; saveflg then exit
        endm

saveflg_popstk_done     macro
        jmp     saveflg_popstk_exit             ; saveflg, popstk then exit
        endm

donearith macro
        jmp        ds:[bp].precrtn              ; done instruction
        endm

save_regs macro
        push    ax                      ; save registers (same order as pusha)
        push    cx                      ; ...
        push    dx                      ; ...
        push    bx                      ; ...
        push    bp                      ; ...(sp)
        push    bp                      ; ...
        push    si                      ; ...
        push    di                      ; ...
        push    ds                      ; ...
        push    es                      ; ...
        endm

getretaddr macro                        ; get return address
ifdef QNX32
        mov     si,S_IP[bp]
        mov     es,S_CS[bp]
else
        les     si,S_IP[bp]
endif
        endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       exception handling
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
        xdefp   FPDivZero
FPDivZero proc  near
        push    ax
        mov     AL,SW_ZE                ; indicate zero divide
        mov     AH,FPE_ZERODIVIDE       ; ...
        call    __Exception_HNDLR       ; process exception
        pop     ax
        ret                             ; return
FPDivZero endp

        xdefp   FPInvalidOp
FPInvalidOp proc near
        push    ax
        mov     AL,SW_IE                ; indicate invalid operation
        mov     AH,FPE_INVALID          ; ...
        call    __Exception_HNDLR       ; process exception
        pop     ax
        ret                             ; return
FPInvalidOp endp

        xdefp   FPOverFlow
FPOverFlow proc near
        push    ax
        mov     AL,SW_OE                ; get OVERFLOW mask
        mov     AH,FPE_OVERFLOW         ; set OVERFLOW exception code
        call    __Exception_HNDLR       ; process exception
        pop     ax
        ret                             ; return
FPOverFlow endp

;
; input:
;       AL - exception mask for status and control word
;       AH - exception code for signal handler

        xdefp   __Exception_HNDLR
__Exception_HNDLR proc  near
        or      byte ptr ds:[bp].status,AL   ; indicate exception occurred
        test    byte ptr ds:[bp].control,AL  ; if exception unmasked
        _if     e                       ; then
          push  si                      ; - save registers
          push  di                      ; - ...
          push  cx                      ; - ...
          push  es                      ; - ...
          push  ds                      ; - ...
          sub   sp,NDP_SIZE             ; - allocate space for copy of 387
          mov   cx,ss                   ; - set ES=SS
          mov   es,cx                   ; - ...
          mov   si,bp                   ; - point to current copy
          mov   di,sp                   ; - point to temporary save area
          mov   cx,NDP_SIZE/2           ; - number of words to copy
          cld                           ; - make sure forward direction
          rep   movsw                   ; - copy it
;          call  __GETDS                 ; - point DS to DGROUP
          mov   AL,AH                   ; - get exception code
          mov   AH,0                    ; - ...
ifdef __QNX__
          call  __FPEhandler            ; - jump to SIGFPE handler rtn
else
          call  __FPE_handler           ; - jump to SIGFPE handler rtn
endif
          mov   cx,ds                   ; - set ES=DS
          mov   es,cx                   ; - ...
          mov   cx,ss                   ; - set DS=SS
          mov   ds,cx                   ; - ...
          mov   di,bp                   ; - point to current copy
          mov   si,sp                   ; - point to temporary save area
          mov   cx,NDP_SIZE/2           ; - number of words to copy
          rep   movsw                   ; - copy it
          add   sp,NDP_SIZE             ; - deallocate space
          pop   ds                      ; - restore registers
          pop   es                      ; - ...
          pop   cx                      ; - ...
          pop   di                      ; - ...
          pop   si                      ; - ...
        _endif                          ; endif
        ret                             ; return
__Exception_HNDLR endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       interrupt int7()     ; protected-mode programs trap to here
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;
; QNX entry point: all registers have been saved on stack as above
;       also ES:BX contains address of 120 byte save area
;
ifdef __PROTMODE__
        public  __int7
__int7  proc    near
ifndef __QNX__
        push    ax                      ; dummy error code
        push    cs                      ; CS
        call    __dmy_int7              ; make stack look like DPMI exception
        add     sp,2                    ; remove dummy error code
        iret                            ; return from interrupt

__dmy_int7:
        pusha                           ; save registers
        push    ds                      ; ...
        push    es                      ; ...
        lea     bx,my87                 ; ...
        mov     ax,ss                   ; put SS in DS
        mov     ds,ax                   ; ...
        mov     bp,sp                   ; get access to registers
        jmp     short _comint7          ; goto common code
;
;       come here for DPMI hosts (e.g. Windows 3.1)
;
        public  __DPMI_int7
__DPMI_int7:
        pusha                           ; save registers
        push    ds                      ; ...
        push    es                      ; ...
        lea     bx,my87                 ; ...
        mov     bp,sp                   ; get access to registers
        mov     ds,DPMI_SS[bp]          ; set ds = user's SS
else
        mov     ax,ss                   ; put SS in DS
        mov     ds,ax                   ; ...
        mov     bp,sp                   ; get access to registers
endif
_comint7:cld                            ; make sure forward direction
        mov     S_MY87[bp],bx           ; save offset of save area
        get_st0                         ; get offset of stack top in DI
p_extended:                             ; nothing to do
;
;       peek at next instruction to see if it is also an emulator interrupt
;
exit:   mov     bp,sp                   ; get access to saved registers
        mov     cx,S_DS[bp]             ; get default data segment
        mov     ax,ds                   ; segment register for [bp] reference
        getretaddr                      ; get return address

luup:   mov     dl,es:[si]              ; get possible prefix in al
        sub     dl,26H                  ; if its es:
        js      short endlup            ; if signed, then not a prefix byte
        je      short is_es             ; - continue with next byte
        cmp     dl,09BH-26H             ; check for "fwait" opcode
        je      inc_si                  ; - yes, just skip over it
        sub     dl,08H                  ; if its cs:
        je      short is_cs             ; - record cs in cx and continue
        sub     dl,08H                  ; if its ss:
        je      short is_ss             ; - record ss in cx and continue
        sub     dl,08H                  ; if its ds:
        je      short is_ds             ; - record ds in cx and continue
        sub     dl,26H                  ; if its fs:
        je      short is_fs             ; - record fs in cx and continue
        dec     dl                      ; if its gs:
        je      short is_gs             ; - record gs in cx and continue
        dec     dl                      ; if its operand length
        je      short inc_si            ; - continue with next byte
        dec     dl                      ; if its address length
        jne     short endlup            ; - its not handled!!!!
inc_si: inc     si                      ; just bump past prefix
        jmp     luup                    ;
is_cs:  mov     cx,S_CS[bp]
        mov     ax,cx                   ; segment for [bp]
        inc     si                      ; bump past prefix
        jmp     luup                    ;
is_ds:  mov     cx,S_DS[bp]
        mov     ax,cx                   ; segment for [bp]
        inc     si                      ; bump past prefix
        jmp     luup                    ;
is_ss:  mov     cx,ds                   ; (DS contains user's SS)
        mov     ax,cx                   ; segment for [bp]
        inc     si                      ; bump past prefix
        jmp     luup                    ;
is_es:  mov     cx,S_ES[bp]             ;
        mov     ax,cx                   ; segment for [bp]
        inc     si                      ; bump past prefix
        jmp     luup                    ;
is_fs:;;;  mov     cx,fs                   ;
        inc     si                      ; bump past prefix
        jmp     luup                    ;
is_gs:;;;  mov     cx,gs                   ;
        inc     si                      ; bump past prefix
        jmp     luup                    ;
endlup:                                 ; segment of memory operand in cx
        mov     dx,es:[si]              ; load up opcode + modrm
        inc     si                      ; point es:si at rest of instruction
        inc     si                      ; ...
        mov     bl,dl                   ; get opcode
        and     dl,07H                  ; get opcode (0-7) into dl
        and     bl,0F8h                 ; isolate top bits
        cmp     bl,0D8h                 ; if it is an 8087 opcode
        je      emulate                 ; then execute it
        jmp     exit99                  ; otherwise exit
__int7  endp

else    ; not __PROTMODE__
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       interrupt int34()               ; int 34h to int 3Bh all come here
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
public  __int34
__int34 proc    near
        sti                             ; interrupts back on
        save_regs                       ; save registers (same order as pusha)
        mov     bp,sp                   ; get access to registers
        mov     cx,ds                   ; save data segment in CX
        cld                             ; make sure forward direction
        call    __GETDS                 ; point ds at "8087" save area
        lea     bx,my87                 ; ...
        mov     S_MY87[bp],bx           ; save offset of save area
        get_st0                         ; get offset of stack top in DI
        mov     ax,ss                   ; segment register for [bp] reference
        getretaddr                      ; get return address
        mov     dx,es:-1[si]            ; load up opcode + modrm
        inc     si                      ; point ds:si at rest of instruction
        and     dl,0FH                  ; get opcode (0-7) into dl
        sub     dl,4                    ; ... (was 34-3B)
        jmp     emulate                 ; go emulate instruction
__int34 endp


;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;       interrupt int3c()               ; segment override present
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

public  __int3c
__int3c proc    near
        sti                             ; interrupts back on
        save_regs                       ; save registers (same order as pusha)
        mov     bp,sp                   ; get access to registers
        cld                             ; make sure forward direction
        call    __GETDS                 ; point ds at "8087" save area
        lea     bx,my87                 ; ...
        mov     S_MY87[bp],bx           ; save offset of save area
        get_st0                         ; get offset of stack top in DI
        getretaddr                      ; get return address
        mov     dh,es:[si]              ; load up opcode, seg override
        inc     si                      ; point ds:si at rest of instruction
next_int3c:
        mov     dl,dh                   ; get opcode + segment override
        mov     dh,es:[si]              ; get modrm byte
        inc     si                      ; ...
        mov     bl,dl                   ; get segment override
        _guess                          ; guess: load cx with segment register
          mov   cx,S_DS[bp]             ; - assume DS
          and   bl,0C0h                 ; - isolate segment override bits
          _quif e                       ; - quit if 00 (DS)
          mov   cx,S_CS[bp]             ; - assume CS (10)
          shl   bl,1                    ; - shift out top bit
          _quif e                       ; - quit if 10 (CS)
          mov   cx,ss                   ; - assume SS (01)
          _quif nc                      ; - quit if 01 (SS)
          mov   cx,S_ES[bp]             ; - it has to be ES (11)
        _endguess                       ; endguess
        mov     ax,cx                   ; segment register for [bp] reference
        and     dl,0FH                  ; get opcode (0-7) into al
        sub     dl,8                    ; ... was D8-DF
        jmp     short emulate           ; do the emulation
__int3c endp
endif

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      AT THIS POINT ...
;;      DL      = OPCODE 0-7
;;      DH      = MODRM byte
;;      ES:SI   = pointer to rest of instruction (or next instruction)
;;      CX      = has the segment for a data reference (if any)
;;      AX      = segment register to be used for [bp] reference
;;      DI      = offset into 8087 register stack to stack top
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
mem_inst:
        mov     bl,dh                   ; get mmxxxrrr bits in bl
        shr     bl,1                    ; ... 0mmxxxrr
        shr     bl,1                    ; ... 00mmxxxr
        shr     bl,1                    ; ... 000mmxxx
        and     bl,018H                 ; isolate them 000mm000
        mov     bh,dh                   ; get r/m bits in bh mmxxxrrr
        and     bh,7                    ; isolate them       00000rrr
        or      bl,bh                   ; glue them          000mmrrr
        mov     bh,0                    ; zero top byte
        shl     bx,1                    ; make it an index
        call    word ptr cs:modrm[bx]   ; do effective address calcn
        mov     S_IP[bp],si             ; zap return address
        mov     es,cx                   ; get segment for memory operand
        mov     si,ax                   ; get offset for memory operand
        mov     bl,dh                   ; get r/m bits again
        and     bl,038H                 ; isolate opcode portion
        or      bl,dl                   ; or in other part of opcode
        shl     bx,1                    ; make it an index
        mov     bp,S_MY87[bp]           ; point BP to save area
        push    word ptr cs:memop[bx]   ; emulate the instruction
        and     bl,0Eh                  ; get index for load routine
        jmp     word ptr cs:loadrtn[bx] ; load memory operand into temp

ifndef __PROTMODE__
next_instruction:
        mov     dx,es:1[si]             ; get next interrupt #, plus next byte
        add     si,3                    ; skip past these bytes
        sub     dl,34h                  ; make sure it is int 34h to 3ch
        cmp     dl,3ch-34h              ; ...
        je      next_int3c              ; if int 3ch, go handle it
        ja      exit99                  ; exit if not an emulator interrupt
        mov     ax,ss                   ; segment register for [bp] reference
        mov     cx,S_DS[bp]             ; get segment register for memory opnd
endif
;
;       fall into emulate for this instruction
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;      AT THIS POINT ...
;;      DL      = OPCODE 0-7
;;      DH      = MODRM byte
;;      ES:SI   = pointer to rest of instruction (or next instruction)
;;      CX      = has the segment for a data reference (if any)
;;      AX      = segment register to be used for [bp] reference
;;      DI      = offset into 8087 register stack to stack top
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

emulate proc    near
        cmp     dh,0C0H                 ; if its a memory reference
        jb      mem_inst                ; then go execute it
        mov     S_IP[bp],si             ; zap return address
        mov     bp,S_MY87[bp]           ; point bp at "8087"
        mov     bl,dh                   ; get r/m bits again
        and     bx,038H                 ; isolate opcode portion
        or      bl,dl                   ; or in other part of opcode
        shl     bx,1                    ; make it an index
        jmp     word ptr cs:stkop1[bx]  ; jmp to prepare/emulation routine


_i_:    push    word ptr cs:stkop[bx]   ; push address of emulation routine
        mov     bl,dh                   ; get "i" from ST(i)
        and     bl,7                    ; ...
        mov     si,bx                   ; save "i" in si
        add     si,cs:tentab[di].t_regnum; calc. reg num of st(i)
        and     si,7                    ; modulo 8
        mov     cx,si                   ; save a copy
        add     si,si                   ; i times 2
        add     si,si                   ; times 4
        add     si,cx                   ; times 5
        add     si,si                   ; times 10
        _lea    ax,st                   ; point AX to st(0)
        _lea    dx,st(i)                ; point DX to st(i)
        mov     bx,ax                   ; result goes to st(0)
        ret                             ; return to emulation routine

_r_:    push    word ptr cs:stkop[bx]   ; push address of emulation routine
        mov     bl,dh                   ; get "i" from ST(i)
        and     bl,7                    ; ...
        mov     si,bx                   ; save "i" in si
        add     si,cs:tentab[di].t_regnum; calc. reg num of st(i)
        and     si,7                    ; modulo 8
        mov     cx,si                   ; save a copy
        add     si,si                   ; i times 2
        add     si,si                   ; times 4
        add     si,cx                   ; times 5
        add     si,si                   ; times 10
        _lea    ax,st(i)                ; point AX to st(i)
        _lea    dx,st                   ; point DX to st(0)
        mov     bx,ax                   ; result goes to st(i)
        ret                             ; return to emulation routine

exit99:
ifdef QNX32
        add     sp,8
comment ~
        db      066h                    ; restore registers
        pop     gs
        db      066h
        pop     fs
~
        db      066h
        pop     es
        db      066h
        pop     ds
        db      066h
        popa
        db      066h
        iret
else
        pop     es                      ; restore registers
        pop     ds                      ; ...
        pop     di                      ; ...
        pop     si                      ; ...
        pop     bp                      ; ...
        pop     bx                      ; ...(sp)
        pop     bx                      ; ...
        pop     dx                      ; ...
        pop     cx                      ; ...
        pop     ax                      ; ...
 ifdef __PROTMODE__
  ifndef __QNX__
        retf                            ; return from interrupt
  else
        iret                            ; return from interrupt
  endif
 else
        iret                            ; return from interrupt
 endif
endif           ; QNX32

ifndef __PROTMODE__
p_extended:                             ; nothing to do
;
;       peek at next instruction to see if it is also an emulator interrupt
;
exit:   mov     bp,sp                   ; get access to saved registers
        getretaddr                      ; get cs:ip of next instruction
        cmp     byte ptr es:[si],0CDh   ; check for 'int' instruction
        jne     exit99
        jmp     next_instruction        ; if it is, then go for it
endif

saveflg_popstk_exit:
        saveflg
popstk_exit:
        popstk
        jmp     exit

saveflg_exit:
        saveflg
        jmp     exit


p_single:                               ; round result to single precision
        lea     bx,[bp+di].fpstack      ; point to stack top
        mov     dx,0FF00h               ; get mask of bits to keep
        mov     ax,4[bx]                ; get top part
        test    ax,80H                  ; if have to round
        _if     ne                      ; then
          test    al,7FH                ; - if half way between
          _if     e                     ; - then
            _guess                      ; - - guess
              cmp     word ptr [bx],0   ; - - - quif rest is not zero
              _quif   ne                ; - - - ...
              cmp     word ptr 2[bx],0  ; - - - quif rest is not zero
              _quif   ne                ; - - - ...
              shl     DX,1              ; - - - adjust mask
            _endguess                   ; - - enduess
          _endif                        ; - endif
          add     word ptr 4[bx],0100h  ; - round up
          adc     word ptr 6[bx],0      ; - ...
          _if     c                     ; - if exponent needs adjusting
            mov   word ptr 6[bx],8000h  ; - - set fraction
            inc   word ptr 8[bx]        ; - - increment exponent
            ;  check for overflow       ; - -
          _endif                        ; - endif
        _endif                          ; endif
        and     4[bx],dx                ; mask off bottom bits
        mov     word ptr 2[bx],0        ; zero the bottom part
        mov     word ptr [bx],0         ; zero the bottom part
        jmp     exit                    ; exit

p_double:                               ; round result to double precision
        lea     bx,[bp+di].fpstack      ; point to stack top
        mov     dx,0F800h               ; get mask of bits to keep
        mov     ax,[bx]                 ; get bottom part
        test    ax,0400H                ; if rounding bit on
        _if     ne                      ; then
          test  ax,03FFH                ; - if half way between
          _if   e                       ; - then
            shl   dx,1                  ; - - adjust mask
          _endif                        ; - endif
          add   word ptr  [bx],0800h    ; - round up
          adc   word ptr 2[bx],0        ; - ...
          adc   word ptr 4[bx],0        ; - ...
          adc   word ptr 6[bx],0        ; - ...
          _if   c                       ; - if exponent needs adjusting
            mov   word ptr 6[bx],8000h  ; - - set fraction
            inc   word ptr 8[bx]        ; - - increment exponent
            ;  check for overflow       ; - -
          _endif                        ; - endif
        _endif                          ; endif
        and     [bx],dx                 ; mask off bottom bits
        jmp     exit                    ; exit


;===========================MOD/RM calculations=================================
;       AX contains segment value to be used for [BP] reference
;       AX will contain SS unless we came in through int3c with an
;       explicit segment override value.
;
DOMODRM macro   disp,reg1,reg2
        IFIDN   <reg1>,<S_BP>                   ; if BP is in the MODRM
          mov   cx,ax                           ; - implicit ss override
        ENDIF                                   ; endif
        IFIDN   <disp>,<D0>                     ; if no displacement
          mov   ax,reg1[bp]                     ; - initialize ax to reg1
        ELSE                                    ; else if 8 byte displacement
          IFIDN   <disp>,<D8>                   ; - then
            mov     al,es:[si]                  ; - grab it
            cbw                                 ; - promote to a word
            inc     si                          ; - bump past it
          ELSE                                  ; else if 16 byte displacement
            .ERRDIF <disp>,<D16>                ; - then
            mov     ax,es:[si]                  ; - grab it
            add     si,2                        ; - bump past it
          ENDIF                                 ; ...
        ENDIF                                   ; ...
        IFDIF   <disp>,<D0>                     ; if there was a diplacement
          IFNB    <reg1>                        ; - if reg1 is there
            add     ax,reg1[bp]                 ; - - add reg1 to address
          ENDIF                                 ; - endif
        ENDIF                                   ; endif
        IFNB    <reg2>                          ; if reg2 is there
          add     ax,reg2[bp]                   ; - add it to address
        ENDIF                                   ; endif
        ret                                     ; return to caller
        endm                                    ; nice macro eh?

modrm:  dw      m00000                          ; mod r/m byte select table
        dw      m00001
        dw      m00010
        dw      m00011
        dw      m00100
        dw      m00101
        dw      m00110
        dw      m00111
        dw      m01000
        dw      m01001
        dw      m01010
        dw      m01011
        dw      m01100
        dw      m01101
        dw      m01110
        dw      m01111
        dw      m10000
        dw      m10001
        dw      m10010
        dw      m10011
        dw      m10100
        dw      m10101
        dw      m10110
        dw      m10111

m00000: DOMODRM D0,S_BX,S_SI                    ; mod r/m byte actions
m00001: DOMODRM D0,S_BX,S_DI
m00010: DOMODRM D0,S_BP,S_SI
m00011: DOMODRM D0,S_BP,S_DI
m00100: DOMODRM D0,S_SI
m00101: DOMODRM D0,S_DI
m00110: DOMODRM D16
m00111: DOMODRM D0,S_BX
m01000: DOMODRM D8,S_BX,S_SI
m01001: DOMODRM D8,S_BX,S_DI
m01010: DOMODRM D8,S_BP,S_SI
m01011: DOMODRM D8,S_BP,S_DI
m01100: DOMODRM D8,S_SI
m01101: DOMODRM D8,S_DI
m01110: DOMODRM D8,S_BP
m01111: DOMODRM D8,S_BX
m10000: DOMODRM D16,S_BX,S_SI
m10001: DOMODRM D16,S_BX,S_DI
m10010: DOMODRM D16,S_BP,S_SI
m10011: DOMODRM D16,S_BP,S_DI
m10100: DOMODRM D16,S_SI
m10101: DOMODRM D16,S_DI
m10110: DOMODRM D16,S_BP
m10111: DOMODRM D16,S_BX

memop:  dw      m00,m01,m02,m03,m04,m05,m06,m07         ; memory instruction
        dw      m10,m11,m12,m13,m14,m15,m16,m17         ; select table
        dw      m20,m21,m22,m23,m24,m25,m26,m27
        dw      m30,m31,m32,m33,m34,m35,m36,m37
        dw      m40,m41,m42,m43,m44,m45,m46,m47
        dw      m50,m51,m52,m53,m54,m55,m56,m57
        dw      m60,m61,m62,m63,m64,m65,m66,m67
        dw      m70,m71,m72,m73,m74,m75,m76,m77

stkop1: dw      _i_,_i_,s02,s03,_r_,_i_,_r_,_i_         ; stack operand
        dw      _i_,_i_,s12,s13,_r_,_i_,_r_,_i_         ; select table
        dw      _i_,s21,s22,s23,_i_,_i_,s26,_i_
        dw      _i_,_i_,s32,s33,_i_,_i_,s36,_i_
        dw      _i_,s41,s42,s43,_r_,_i_,_r_,s47
        dw      _i_,s51,s52,s53,_r_,_i_,_r_,s57
        dw      _i_,s61,s62,s63,_r_,s65,_r_,s67
        dw      _i_,s71,s72,s73,_r_,s75,_r_,s77

stkop:  dw      s00,s01,s02,s03,s04,s05,s06,s07         ; stack operand
        dw      s10,s11,s12,s13,s14,s15,s16,s17         ; select table
        dw      s20,s21,s22,s23,s24,s25,s26,s27
        dw      s30,s31,s32,s33,s34,s35,s36,s37
        dw      s40,s41,s42,s43,s44,s45,s46,s47
        dw      s50,s51,s52,s53,s54,s55,s56,s57
        dw      s60,s61,s62,s63,s64,s65,s66,s67
        dw      s70,s71,s72,s73,s74,s75,s76,s77

;========================================================

        xrefp   __FLDA
        xrefp   __FLDS
        xrefp   __FLDM
        xrefp   __FLDD
        xrefp   __FLDC
        xrefp   __I4LD
        xrefp   __I8LD
        xrefp   __U4LD
        xrefp   __LDI4
        xrefp   __LDU4
        xrefp   __LDI8
        xrefp   __EmuFSLD
        xrefp   __EmuLDFS
        xrefp   __EmuFDLD
        xrefp   __EmuLDFD
        xrefp   __sqrt
        xrefp   __fprem
        xrefp   __sin
        xrefp   __cos
        xrefp   __tan
        xrefp   __fpatan
        xrefp   __fyl2x
        xrefp   __fyl2xp1
        xrefp   __f2xm1
        xrefp   __fxam
        xrefp   __frndint

ifndef NDEBUG
badop   proc    near
        lea     dx,illopmsg
        mov     ah,9H
        int     21H
        ret
badop   endp
endif

illop   macro
ifndef NDEBUG
        call    badop
endif
        endm

ifndef NDEBUG
prtnyi  proc    near
        mov     ah,9H
        int     21H
        lea     dx,nyimsg
        mov     ah,9H
        int     21H
        ret
prtnyi  endp

nyi     macro   string
        local   msg
        _DATA segment word public 'DATA'
msg     db      '&string',NC
        _DATA ends
        lea     dx,msg
        call    prtnyi
        endm
else
nyi     macro   string
        endm
endif

_add    macro   a,b,c
        _lea    AX,a
        _lea    DX,b
        _lea    BX,c
        call    __FLDA
        endm

_sub    macro   a,b,c
        _lea    AX,a
        _lea    DX,b
        _lea    BX,c
        call    __FLDS
        endm

_mul    macro   a,b,c
        _lea    AX,a
        _lea    DX,b
        _lea    BX,c
        call    __FLDM
        endm

_div    macro   a,b,c
        _lea    AX,a
        _lea    DX,b
        _lea    BX,c
        call    __FLDD
        endm

_neg    macro   a
        _lea    BX,a
        xor     byte ptr 9[BX],80h
        endm

_abs    macro   a
        _lea    BX,st
        and     byte ptr 9[BX],7Fh      ; turn off sign bit
        endm

_cmp    macro   a,b
        _lea    AX,a
        _lea    DX,b
        call    __FLDC
        endm

_xchg   macro   a,b
        mov     ax,cs:tagtab[di].t_off
        and     ax,cs:tagtab[si].t_off
        and     ds:[bp].tags,ax         ; mark both regs as valid
        _lea    BX,a
        _lea    SI,b
        mov     ax,[si]
        xchg    ax,[bx]
        mov     [si],ax
        mov     ax,2[si]
        xchg    ax,2[bx]
        mov     2[si],ax
        mov     ax,4[si]
        xchg    ax,4[bx]
        mov     4[si],ax
        mov     ax,6[si]
        xchg    ax,6[bx]
        mov     6[si],ax
        mov     ax,8[si]
        xchg    ax,8[bx]
        mov     8[si],ax
        endm

i4ld    macro
        call    __I4LD
        endm

ldi4    macro
        call    __LDI4
        endm

u4ld    macro
        call    __U4LD
        endm

ldu4    macro
        call    __LDU4
        endm

st_top  macro rtn
        _lea    AX,st
        call    __&rtn
        endm

st_top2 macro rtn
        _lea    AX,st
        popstk
        _lea    DX,st
        call    __&rtn
        endm

real10  struc
        bytes1to2       dw      0
        bytes3to4       dw      0
        bytes5to6       dw      0
        bytes7to8       dw      0
        bytes9n10       dw      0
real10  ends

mov10   macro   src_seg,src_off,dst_seg,dst_off
        mov     ax,src_seg:src_off.bytes1to2
        mov     dst_seg:dst_off.bytes1to2,ax
        mov     ax,src_seg:src_off.bytes3to4
        mov     dst_seg:dst_off.bytes3to4,ax
        mov     ax,src_seg:src_off.bytes5to6
        mov     dst_seg:dst_off.bytes5to6,ax
        mov     ax,src_seg:src_off.bytes7to8
        mov     dst_seg:dst_off.bytes7to8,ax
        mov     ax,src_seg:src_off.bytes9n10
        mov     dst_seg:dst_off.bytes9n10,ax
        endm

putword macro   what,where
        mov     ds:[bp].&where,what
        endm

getword macro   what,where
        mov     what,ds:[bp].&where
        endm

loadword macro  what
        mov     what,es:[si]
        endm

saveword macro  what
        mov     es:[si],what
        endm

save_st macro
        _lea    bx,st
        push    [bx]
        push    2[bx]
        push    4[bx]
        push    6[bx]
        push    8[bx]
        endm

restore_st macro
        _lea    bx,st
        pop     8[bx]
        pop     6[bx]
        pop     4[bx]
        pop     2[bx]
        pop      [bx]
        endm

rndint  macro
        _lea    ax,st
        getword dx,control
        and     dx,CW_RC
        call    __frndint
        endm

ldcons  macro   cons
        mov10   cs,cons,ds,[bp].fptemp
        endm

stcons  macro   w1,w2,w3,w4,w5
        _lea    si,st
if w5 eq 0
        sub     ax,ax
        mov     [si],ax
else
        mov     word ptr [si],w5
endif
if w4 eq 0
        mov     2[si],ax
else
        mov     word ptr 2[si],w4
endif
if w3 eq 0
        mov     4[si],ax
else
        mov     word ptr 4[si],w3
endif
if w2 eq 0
        mov     6[si],ax
else
        mov     word ptr 6[si],w2
endif
if w1 eq 0
        mov     8[si],ax
else
        mov     word ptr 8[si],w1
endif
        endm

setp_extended   macro
        lea     dx,p_extended
        mov     ds:[bp].precrtn,dx
        endm

setp_double     macro
        lea     dx,p_double
        mov     ds:[bp].precrtn,dx
        endm

setp_single     macro
        lea     dx,p_single
        mov     ds:[bp].precrtn,dx
        endm

_fclex  macro
        and     word ptr ds:[bp].status, 07f00H
        endm

_finit  macro
        putword CW_INIT,control
        putword SW_INIT,status
        putword TAG_INIT,tags
        setp_extended
        sub     di,di                   ; set top of stack pointer to 0
        endm

save87  macro size                      ; put the 87 into user memory
        push    di                      ; save top of stack pointer
        mov     bx,di                   ; get top of stack pointer
        call    __save87env             ; save 8087 environment
IFDIF   <size>,<ENV_SIZE>
        mov     cx,8                    ; do 8 times
fslup:  lea     si,[bp].fpstack         ; - point source to stack top
        add     si,bx                   ; - ...
        movsw                           ; - save the value
        movsw                           ; - ...
        movsw                           ; - ...
        movsw                           ; - ...
        movsw                           ; - ...
        mov     bx,cs:tentab[bx].t_next ; - 'pop' the stack
        loop    fslup                   ; end do
ENDIF
        pop     di                      ; restore top of stack pointer
        endm

rest87  macro size                      ; put user memory into the 87
        mov     cx,ENV_SIZE / 2 ; set up size reg
        push    ds                      ; save registers
        push    es                      ; ...
        push    di                      ; ...
        mov     ax,es                   ; xchg ds,es
        mov     dx,ds                   ; ...
        mov     ds,ax                   ; source is user memory (was es:si)
        mov     es,dx                   ; dest is 8087
        mov     di,bp                   ; ...
        rep     movsw                   ; put into 8087
IFDIF   <size>,<ENV_SIZE>
        mov     bx,es:[bp].status       ; - bx gets index of top of stack
        and     bx,SW_ST                ; - ...
        mov     cl,SW_ST_SHIFT          ; - ...
        shr     bx,cl                   ; - ...
        mov     cx,bx                   ; - make a copy
        add     bx,bx                   ; - times 2
        add     bx,bx                   ; - times 4
        add     bx,cx                   ; - times 5
        add     bx,bx                   ; - times 10
        mov     cx,8                    ; do 8 times
frlup:  lea     di,[bp].fpstack         ; - point to st(0)
        add     di,bx                   ; - ...
        movsw                           ; - restore st(0)
        movsw                           ; - ...
        movsw                           ; - ...
        movsw                           ; - ...
        movsw                           ; - ...
        mov     bx,cs:tentab[bx].t_next ; - 'pop' the stack
        loop    frlup                   ; end do
ENDIF
        pop     di                      ; restore top of stack pointer
        pop     es                      ; restore segment registers
        pop     ds                      ; ...
        endm

load    macro  where,what                       ; load up hi:lo
        IFIDN           <what>,<double>         ; - then
        mov     dx,es:[si]                      ; - load up double
        mov     cx,es:2[si]                     ; - load up double
        mov     bx,es:4[si]                     ; - ...
        mov     ax,es:6[si]                     ; - ...
        _lea    si,where                        ; - ...
        call    __EmuFDLD                       ; - convert to tbyte
        ELSE                                    ; else if long
        IFIDN      <what>,<long>                ; - then
        mov     ax,es:[si]                      ; - load long integer
        mov     dx,es:2[si]                     ; - load long integer
        _lea    bx,where                        ; - ...
        i4ld                                    ; - convert to tbyte
        ELSE                                    ; else if int
        IFIDN      <what>,<integer>             ; - then
        mov     ax,es:[si]                      ; - load an int
        cwd                                     ; - convert to long
        _lea    bx,where                        ; - ...
        i4ld                                    ; - convert to tbyte
        ELSE                                    ; else if double long (8 bytes)
        IFIDN      <what>,<dlong>               ; - then
        mov     dx,es:[si]                      ; - load up the 64-bit integer
        mov     cx,es:2[si]                     ; - ...
        mov     bx,es:4[si]                     ; - ...
        mov     ax,es:6[si]                     ; - ...
        _lea    si,where                        ; - ...
        call    __I8LD                          ; - convert to tbyte
        ELSE                                    ; else if float
        IFIDN      <what>,<float>               ; - then
        mov     ax,es:[si]                      ; - load a float
        mov     dx,es:2[si]                     ; - load a float
        _lea    bx,where                        ; - ...
        call    __EmuFSLD                       ; - convert to tbyte
        ELSE                                    ; else if st(i)
        IFIDN      <what>,<st(i)>               ; - then
        IFIDN      <where>,<temp>               ; - if going to temp
        mov10   ds,[si+bp].fpstack,ds,[bp].fptemp
        ELSE                                    ; - else going to st
        mov10   ds,[si+bp].fpstack,ds,[di+bp].fpstack
        ENDIF                                   ; - endif
        ELSE                                    ; else if st
        IFIDN      <what>,<st>                  ; - then
        .ERRDIF   <where>,<temp>                ; - if going to temp
        mov10   ds,[di+bp].fpstack,ds,[bp].fptemp
        ELSE                                    ; else if treal
        IFIDN      <what>,<treal>               ; - then
        IFIDN     <where>,<st>                  ; - if load to st
        mov10   es,[si],ds,[di+bp].fpstack      ; - - move 10 bytes
        ELSE                                    ; - else load to temp
        .ERRDIF   <where>,<temp>                ; - - ...
        mov10   es,[si],ds,[bp].fptemp          ; - - move 10 bytes
        ENDIF                                   ; - endif
        ELSE                                    ; else if temp
        IFIDN     <what>,<temp>                 ; - then
        .ERRDIF   <where>,<st>                  ; - loading into st
        mov10   ds,[bp].fptemp,ds,[bp+di].fpstack; - move 10 bytes
        ELSE                                    ; else
        .ERR                                    ; - I can't use my own macro's
        %OUT bad load macro                     ; - ...
        ENDIF                                   ; endif
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ... oh for an elseif
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        endm

store   macro   what                            ; store st somewhere
        IFIDN   <what>,<double>                 ; - then it
        _lea    bx,st                           ; - point to top of stack
        call    __EmuLDFD                       ; - convert to double
        mov     es:[si],dx                      ; - store it
        mov     es:2[si],cx                     ; - store it
        mov     es:4[si],bx                     ; - store it
        mov     es:6[si],ax                     ; - ...
        ELSE                                    ; - ...
        IFIDN   <what>,<long>                   ; else if long
        _lea    bx,st                           ; - point to top of stack
        ldi4                                    ; - convert to long
        mov     es:[si],ax                      ; - store it
        mov     es:2[si],dx                     ; - store it
        ELSE                                    ; else if float
        IFIDN   <what>,<float>                  ; - then
        _lea    bx,st                           ; - point to top of stack
        call    __EmuLDFS                       ; - convert to float
        mov     es:[si],ax                      ; - store it
        mov     es:2[si],dx                     ; - store it
        ELSE                                    ; else if integer
        IFIDN   <what>,<integer>                ; - then
        _lea    bx,st                           ; - point to top of stack
        ldi4                                    ; - convert to integer
        mov     bx,dx                           ; - save value
        cwd                                     ; - sign extend ax to dx:ax
        cmp     dx,bx                           ; - if not equal
        _if     ne                              ; - then
          mov   ax,8000h                        ; - - indicate overflow
        _endif                                  ; - endif
        mov     es:[si],ax                      ; - store it
        ELSE                                    ; else if double long (8 bytes)
        IFIDN   <what>,<dlong>                  ; - then
        _lea    ax,st                           ; - point to top of stack
        call    __LDI8                          ; - convert to 64-bit integer
        mov     es:[si],dx                      ; - store it
        mov     es:2[si],cx                     ; - ...
        mov     es:4[si],bx                     ; - ...
        mov     es:6[si],ax                     ; - ...
        ELSE                                    ; else if st(i)
        IFIDN   <what>,<st(i)>                  ; - then
        mov10   ds,[di+bp].fpstack,ds,[si+bp].fpstack
        ELSE                                    ; else if st(i)
        IFIDN   <what>,<treal>                  ; - then
        mov10   ds,[di+bp].fpstack,es,[si]      ; - move 10 bytes
        ELSE                                    ; else
        .ERR                                    ; - I cant use my own macro
        %OUT bad store macro                    ; - ...
        ENDIF                                   ; endif
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        ENDIF                                   ; ...
        endm

;;;;;;;;;;;;;;;;;;;;;;;;;;;; memory instruction emulation ;;;;;;;;;;;;;;;;;;;;;
;;
;; AT THIS POINT
;; ES:SI                   - pointer to the memory operand
;; DS:[DI+BP].fpstack      - pointer to the top of stack
;; ON EXIT
;; DI                      - will be updated to reflect top of stack pointer
;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include memops.inc

;;;;;;;;;;;;;;;;;;;;;;;;;;; stack instruction emulation ;;;;;;;;;;;;;;;;;;;;;;;;
;; AT THIS POINT
;; DS:[DI+BP].fpstack - pointer to ST(0)
;; DS:[SI+BP].fpstack - pointer to ST(i)
;; BX is the bottom 3 bits of modrm byte
;; ON EXIT
;; DI           - will have been updated to reflect top of stack pointer
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

include stkops.inc

; here are some specific stack ops

s613:;  fpatan
;===================================================================
        _lea    dx,st           ; X
        popstk
        _lea    ax,st           ; Y
        mov     bx,ax           ; destination
        call    __fpatan        ; calc. atan2(Y,X)
        donearith

s614:;  fxtract
;===================================================================
        _lea    bx,st
        pushstk                                 ; push stack pointer
        _lea    si,st
        mov     ax,[bx]
        mov     [si],ax
        mov     ax,2[bx]
        mov     2[si],ax
        mov     ax,4[bx]
        mov     4[si],ax
        mov     ax,6[bx]
        mov     6[si],ax
        mov     ax,8[bx]
        mov     dx,ax
        and     dx,8000h
        or      dx,3fffh
        mov     8[si],dx
        and     ax,7fffh                        ; remove sign
        sub     ax,3fffh                        ; remove bias
        cwd                                     ; sign extend
        i4ld                                    ; load exponent
        done

s615:;  fprem1
s710:;  fprem
;===================================================================
        _lea    AX,st                           ; point to st(0)
        mov     SI,cs:tentab[DI].t_next         ; get index for st(1)
        _lea    DX,st(i)                        ; point to st(1)
        call    __fprem                         ; do remainder
        savecc                                  ; save condition codes
        donearith

s714:;  frndint
;===================================================================
        rndint
        done

s715:;  fscale
;===================================================================
        mov     SI,cs:tentab[DI].t_next         ; get index for st(1)
        _lea    BX,st(i)        ; point to st(1)
        ldi4                    ; convert number to integer
;
;       check to see if scale factor is too large;  29-mar-91
;
        mov     CX,DX           ; save high word of scale factor
        cwd                     ; sign extend low order 16 bits
        cmp     CX,DX           ; if exponent doesn't fit in 16 bits
        _if     ne              ; then
          mov   CX,8[BX]        ; - get exponent and sign
          and   CX,8000h        ; - isolate sign
          _if   s               ; - if negative scale factor
            sub   DX,DX         ; - - underflow
            sub   CX,CX         ; - - clear the sign
          _else                 ; - else
            mov   DX,7FFFh      ; - - overflow
            mov   CX,word ptr ds:8[bp+di].fpstack ; - - get current exponent & sign
            and   CX,8000h      ; - - isolate sign
          _endif                ; - endif
        _else                   ; else
          mov   DX,word ptr ds:8[bp+di].fpstack ; get current exponent
          mov   CX,DX           ; - get sign
          and   CX,8000h        ; - ...
          and   DX,7FFFh        ; - isolate exponent
          add   DX,AX           ; - calc. new exponent
          _if   s               ; - if overflow/underflow
            or    AX,AX         ; - - if negative
            _if   s             ; - - then
              sub   DX,DX       ; - - - underflow
              sub   CX,CX       ; - - - clear the sign
            _else               ; - - else
              mov   DX,7FFFh    ; - - - overflow
            _endif              ; - - endif
          _endif                ; - endif
        _endif                  ; endif
        or      DX,CX           ; put in the sign
        mov     word ptr ds:8[bp+di].fpstack,DX ; store new exponent
        done

s470:;  fstsw   ax
;===================================================================
        getword ax,status               ; get status word
        mov     bp,sp                   ; get access to registers
        mov     S_AX[bp],ax             ; update ax register in stack
        done
emulate endp

__save87env proc near
        push    ds:[BP].status          ; save status word (fxam changes it)
        mov     CX,8                    ; 8 registers
        sub     DI,DI                   ; start at register 0
fxlup:; loop                            ; loop (update tag word for each reg)
          lea   AX,[BP].fpstack         ; - point to start of registers
          add   AX,DI                   ; - point to next register
          call  __fxam                  ; - examine reg and update tag word
          add   DI,10                   ; - point to next register
        loop    fxlup                   ; until all registers examined
        pop     ds:[BP].status          ; restore status word
        mov     CX,ENV_SIZE / 2         ; set up size reg
        mov     DI,SI                   ; destination is user memory
        mov     SI,BP                   ; source is the '8087'
        rep     movsw                   ; copy environment to users memory
        ret                             ; return
__save87env endp


