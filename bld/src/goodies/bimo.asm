;**
;** bimo.asm:
;**     Interrupt handlers for real- and protected-mode INT 0xC, or 0x0B
;** if the -2 command-line option is used.  See the bimodal.c example
;** program for details.
;**

.286

_TEXT16 SEGMENT BYTE PUBLIC 'CODE'
        ASSUME  cs:_TEXT16

        ;**
        ;** The real-mode interrupt handler is in a 16-bit code segment
        ;** so that the assembler will generate the right code.
        ;**
        ;** We will copy this code to a 16-bit segment in low memory
        ;** and install it as a real mode interrupt handler before it
        ;** gets executed.  (There's no distinction between code and
        ;** data in real mode.)
        ;**
        ;** Since the handler doesn't get executed before it gets copied,
        ;** putting it in this segment makes the C code simpler.
        ;**
        ;** The buffer into which we will store the data being sent
        ;** to us is also in this segment, for simplicity.  (It's
        ;** possible to read and write code segments in real mode.)
        ;** We will lay out this segment using fixed offsets, so that
        ;** we can omit the data in the executable file.  The symbolic
        ;** offsets are only used to reference the segment BEFORE
        ;** it has been copied low.
        ;**
        ;** Both the real mode and protected mode handlers will store
        ;** their data in this same buffer (that is, in the copy in
        ;** low memory).
        ;**
        ;** OFFSET    LENGTH     CONTENTS
        ;**
        ;** 0         <128       Real mode interrupt handler
        ;** 128       4          Next available slot in buffer
        ;** 132       1024       Data receive buffer
        ;** 132+1024  1          Overflow indicator
        ;**

maxbuf  EQU     1024

        PUBLIC  rmhandler_, _com_port_low
rmhandler_:
        push    ds
        push    bx
        mov     bx, 0B800h
        mov     ds, bx                          ; DS = 0xB800
        sub     bx, bx                          ; BX = 0
        mov     WORD PTR [bx], 0720h            ; Clear 2 character cells
        mov     WORD PTR [bx+2], 0720h

        push    cx
        push    ax
        mov     cx, 10h
rmdelay:
        mov     ax, 1
        mul     cl
        loop    rmdelay                         ; RM delay loop (flicker makes
        pop     ax                              ;   screen effects understandable)
        pop     cx

        mov     BYTE PTR [bx],'R'               ; Write 'R' to memory map

        db      0BBh                            ; mov bx,...
_com_port_low   DW      ?                       ;   com port base address
        push    ax
        push    dx
        lea     dx, [bx+2]                      ; int id register
        in      al, dx                          ; Read ports so interrupts
        mov     dx, bx                          ; can continue to be
        in      al, dx                          ; generated; get data in AL

        mov     bx, cs:[128]                    ; get buffer index
        mov     cs:[132+bx], al                 ; store data
        inc     bx                              ; increment count, which
        cmp     bx, maxbuf+1                    ;   can overflow by one,
        jl      no_ov1                          ;   resetting it to zero
        sub     bx, bx                          ;   on an overflow so we
no_ov1:                                         ;   don't clobber memory --
        mov     cs:[128], bx                    ;   and store new value

        mov     dx,020h
        mov     al,dl
        out     dx,al                           ; Send EOI
        pop     dx
        pop     ax
        pop     bx
        pop     ds
        iret
        ASSUME  cs:NOTHING

; These variables only exist in the real mode copy of this segment
; (the one which actually gets executed)
;
;               ORG     128
;next           DD      0
;databuf        DB      maxbuf dup(0)
;overflow       DB      0

_TEXT16 ENDS

.386p

_DATA   SEGMENT DWORD PUBLIC 'DATA'
        EXTRN   _com_port:WORD
        EXTRN   _com_id:BYTE
        EXTRN   _lowmem_seg:WORD
        EXTRN   _pm_count:DWORD
_DATA   ENDS

_TEXT   SEGMENT BYTE PUBLIC 'CODE'
        ASSUME  cs:_TEXT, ds:_DATA

        PUBLIC  com_init_
com_init_:
        mov     ax,0F3h                         ; 9600,n,8,1
        mov     dl,[_com_id]
        sub     dh,dh
        dec     dx                              ; 0 = COM1, 1 = COM2
        int     14h                             ; Initialize device
        mov     bx,[_com_port]                  ; Base of port space
        lea     dx,[bx+5]                       ; line status register
        in      al,dx
        lea     dx,[bx+4]                       ; modem control register
        in      al,dx
        or      al,8                            ; enable OUT2 interrupt
        out     dx,al
        lea     dx,[bx+2]                       ; int id register
        in      al,dx
        mov     dx,bx                           ; data receive register
        in      al,dx
        mov     dl,NOT 10h                      ; mask for IRQ4
        cmp     [_com_id],1
        je      maskit
        mov     dl,NOT 8h                       ; mask for IRQ3
maskit:
        in      al,21h                          ; interrupt mask register
        and     al,dl                           ; force IRQ unmasked
        out     21h,al
        lea     dx,[bx+1]                       ; int enable register
        mov     al,1
        out     dx,al                           ; enable received-data int
        ret

        ASSUME  ds:NOTHING

        ;**
        ;** The protected-mode interrupt handler is in a 32-bit code
        ;** segment.  Even so, we have to be sure to force an IRETD
        ;** at the end of the handler, because MASM doesn't generate
        ;** one.  This handler will be called on a 32-bit stack by
        ;** DOS/4GW.
        ;**

        PUBLIC  pmhandler_
pmhandler_:
        push    ds
        push    ebx
        mov     bx,_DATA
        mov     ds,bx

        ASSUME  ds:_DATA

        mov     ebx,0B8000h                     ; DS:EBX = flat:0B8000h
        mov     DWORD PTR [ebx],07200720h       ; Clear 2 character cells

        push    eax
        push    ecx
        mov     ecx, 10h
pmdelay:
        mov     ax, 1
        mul     cl
        loop    pmdelay                         ; PM delay loop (for clarity
        pop     ecx                             ;   of display only)

        mov     BYTE PTR [ebx+2],'P'            ; Write 'P' to memory map

        push    edx
        mov     bx,[_com_port]
        lea     dx,[bx+2]
        in      al,dx                           ; Read ports so interrupts
        mov     dx,bx                           ; can continue to be
        in      al,dx                           ; generated

        inc     [_pm_count]                     ; Count interrupts handled in PM
        mov     bx, [_lowmem_seg]
        mov     ds, bx                          ; Get DS addressability to buffer

        ASSUME  ds:NOTHING

        mov     ebx, ds:[128]                   ; get buffer index
        mov     ds:[132+ebx], al                ; store data
        inc     ebx                             ; increment count, which
        cmp     ebx, maxbuf+1                   ;   can overflow by one,
        jl      no_ov2                          ;   resetting it to zero
        sub     ebx, ebx                        ;   on an overflow so we
no_ov2:                                         ;   don't clobber memory --
        mov     ds:[128], ebx                   ;   and store new value

        mov     dx,020h
        mov     al,dl
        out     dx,al                           ; Send EOI
        pop     edx
        pop     eax
        pop     ebx
        pop     ds
        iretd

_TEXT   ENDS

END
