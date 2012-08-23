ifndef DOSX    

ifdef ??version    ; Turbo Assembler
  JUMPS            ; use option /m2 also
endif

.MODEL LARGE,C
.CODE

;
; Packet-driver receiver upcall handler. This module is only
; used by small/large real-mode targets. Look in asmpkt2.s for
; an alternate version that does the same as this.
;

PUBLIC pkt_receiver_rm, _pkt_receiver_rm

extrn _pkt_enque_ptr : far

PKTBUF_SIZE = 1514

rx_buf_ptr     label dword
               dw rx_buf, SEG rx_buf
nullPtr        dd 0

rx_buf         db PKTBUF_SIZE dup (0)  ; receive buffer
rx_len         dw 0

rx_stk         dw 64 dup (0)           ; a small work stack
rx_stk_end     label word
               dw 0

pkt_receiver_rm:
_pkt_receiver_rm:
        pushf
        cli                        ; no interruptions now
        or   al, al                ; AL = 0 if 1st call from pkt-driver
        jnz  @enque                ; AL <>0, 2nd call; enqueue packet

        cmp  cx, PKTBUF_SIZE       ; check the packet length
        ja   @no_fnd               ; pretend none were found
        les  di, cs:rx_buf_ptr
        mov  cs:rx_len, cx         ; save buffer len for pkt_enqueue
        popf
        retf

@no_fnd:les  di, cs:nullPtr        ; return NULL ptr
        popf
        retf

        ;
        ; enque packet, DS:SI=ES:DI from 1st call, i.e. DS=CS
        ;
@enque: cmp  si, offset rx_buf
        jne  @exit                 ; not a valid pointer, don't enqueue

        mov  ax, DGROUP
        mov  ds, ax                ; load our DS
      ;
      ; Watcom assumes SS=DS in pkt_enqueue(). We therefore
      ; need to setup a new work-stack.
      ;
        mov  cx, ss
        mov  dx, sp
        mov  ss, ax                ; setup new stack (SS=DS)
        lea  sp, rx_stk_end
        push cx                    ; save SS on new stack
        push dx                    ; save SP on new stack

        cld                        ; C-code assumes forward direction
        push bx                    ; store handle, length and buf address
        push cs:rx_len
        push cs
        push si
        call dword ptr DGROUP:_pkt_enque_ptr
        add  sp, 8

        pop  dx                    ; restore old stack
        pop  cx
        mov  ss, cx
        mov  sp, dx

@exit:  popf
        retf

endif   ; ifndef DOSX

END
