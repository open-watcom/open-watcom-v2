        .model tiny

CWCStackSize    equ     1024    ; cannot exceed EXECopyStubLen size (1135 bytes)

decode_c_struc struc
DecC_ID db "CWC"
DecC_Bits       db ?
DecC_Len        dw ?,?
DecC_Size       dw ?,?
decode_c_struc  ends

b       equ     byte ptr
w       equ     word ptr
d       equ     dword ptr
f       equ     fword ptr


RepMinSize      equ     2

var_struc       struc
;       db 100h-10h dup (0)
        db CWCStackSize-10h dup (0)

var_SourceSeg   dw ?
var_CopyLen     dw ?
var_EntryIP     dw 0
var_EntryCS     dw ?
var_ImageLen    dw ?,?
var_EntryES     dw ?
var_struc       ends

        .code

start   proc    near
;
;Decompress the EXE and it's header/relocation entries.
;
        cld
        push    di
        push    es
        call    Decode
        pop     es
        pop     di
        ;
        ;Get a pointer to the EXE header.
        ;
        mov     ax,ss:w[var_ImageLen]
        mov     bx,ss:w[var_ImageLen+2]
        shr     bx,1
        rcr     ax,1
        shr     bx,1
        rcr     ax,1
        shr     bx,1
        rcr     ax,1
        shr     bx,1
        rcr     ax,1
        mov     dx,es
        add     dx,ax
        mov     ds,dx
        mov     si,ss:w[var_ImageLen]
        and     si,15
;
;Process the relocation entries.
;
        push    ds
        push    si
        mov     cx,[si+6]               ;get number of relocation entries.
        add     si,1bh
        ;
L1:     or      cx,cx
        jz      L2
        push    si
        push    ds
        push    es
        mov     ax,[si+2]               ;/
        mov     si,[si]         ;Get the offset read.
        mov     bx,es
        add     ax,bx           ;/
        mov     es,ax           ;/
        add     es:[si],bx              ;"Relocate" the value...
        pop     es
        pop     ds
        pop     si
        dec     cx
        add     si,4
        jnz     L1
        mov     ax,ds
        add     ax,1000h
        mov     ds,ax
        jmp     L1
        ;
L2:     pop     si
        pop     ds
;
;Get entry register values setup.
;
        mov     bx,es
        mov     ax,[si+0eh]
        add     ax,bx
        mov     dx,ax
        mov     bp,[si+10h]
        mov     ax,[si+16h]
        add     ax,bx
        mov     cs:EntryCS,ax
        mov     ax,[si+14h]
        mov     cs:EntryIP,ax
        mov     es,ss:w[var_EntryES]
        mov     ds,ss:w[var_EntryES]
        mov     ss,dx
        mov     sp,bp
;
;Pass control to the real program.
;
        jmp     cs:dword ptr [EntryIP]
Start   endp


Decode  proc    near
;
;Get next input bit.
;
_DCD_ReadBit    macro
        local __0
        adc     bp,bp
;       adc     bx,bx
        dec     dl
        jnz     __0
        mov     bp,[si]
;       mov     bx,[si+2]
        inc     si
        inc     si
;       lea     si,4[si]
        mov     dl,dh
__0:    ;
        endm

        ;
        ;Check for main ID string.
        ;
        xor     ch,ch
        mov     cl,b[si+DecC_Bits]
        mov     ax,1
        shl     ax,cl
        dec     ax
        mov     cs:w[@@Masker+2],ax
        sub     cl,8
        mov     cs:b[@@Shifter+1],cl
        add     si,size decode_c_struc
;
;Get on with decodeing the data.
;
        mov     bp,[si]
;       mov     bx,[si+2]
        add     si,2
        mov     dl,16
        mov     dh,dl
;
;The main decompresion loop.
;
L4:     _DCD_ReadBit
        jnc     L5
        ;
        ;Read a raw byte.
        ;
        mov     al,[si]
        mov     es:[di],al
        inc     si
        inc     di
        jmp     L4
        ;
L5:     _DCD_ReadBit
        jnc     L8
        ;
        ;Do a rep with 8 bit position, 2 bit length.
        ;
        xor     cx,cx
        _DCD_ReadBit
        adc     cl,cl
        _DCD_ReadBit
        adc     cl,cl
        add     cl,2
        xor     ax,ax
        mov     al,[si]
        inc     si
        dec     ax
        js      L7
        ;
L6:     ;do a rep.
        ;
        add     ax,cx
        push    si
        push    ds
        push    es
        pop     ds
        mov     si,di
        sub     si,ax
        rep     movsb
        pop     ds
        pop     si
        jmp     L4
        ;
        ;Do a run.
        ;
L7:     mov     al,[si]
        inc     si
        inc     cx
        rep     stosb
        jmp     L4
        ;
L8:     _DCD_ReadBit
        jnc     L9
        ;
        ;Do a rep with 12 bit position, 4 bit length.
        ;
        xor     ax,ax
        mov     ax,[si]
        add     si,2
        mov     ch,al
        mov     cl,4
        shr     ax,cl
        mov     cl,ch
        and     cx,15
        add     cl,2
        dec     ax
        jns     L6
        jmp     L7
        ;
        ;Do a rep with 12-bit position, 12-bit length.
        ;
L9:     mov     ax,[si+1]
        mov     ch,al
@@Shifter:
        mov     cl,12-8
        shr     ax,cl
        mov     cl,[si]
        add     si,3
@@Masker:
        and     cx,4095
        add     cx,2
        dec     ax
        jns     L6
        ;
        ;Check for special codes of 0-15 (Would use 2 or 4 bit if really needed)
        ;
        cmp     cx,RepMinSize+15+1
        jnc     L7
        cmp     cl,RepMinSize+2 ;Rationalise destination?
        jz      L12
        cmp     cl,RepMinSize+1 ;Rationalise source?
        jz      L11
        cmp     cl,RepMinSize+3
        jz      L10
        cmp     cl,RepMinSize   ;Terminator?
        jz      L13
        jmp     L13
        ;
        ;Copy literal string.
        ;
L10:    xor     ch,ch
        mov     cl,[si]
        inc     si
        rep     movsb
        jmp     L4
        ;
        ;rationalise DS:SI
        ;
L11:    cmp     si,49152
        jc      L4
        mov     ax,si
        shr     ax,1
        shr     ax,1
        shr     ax,1
        shr     ax,1
        push    bx
        mov     bx,ds
        add     ax,bx
        mov     ds,ax
        and     si,15
        pop     bx
        jmp     L4
        ;
        ;rationalise ES:DI
        ;
L12:    cmp     di,49152
        jc      L4
        mov     ax,di
        sub     ax,49152
        push    ax
        push    bx
        shr     ax,1
        shr     ax,1
        shr     ax,1
        shr     ax,1
        mov     bx,es
        add     ax,bx
        mov     es,ax
        pop     bx
        pop     ax
        sub     di,ax
        and     ax,15
        add     di,ax
        jmp     L4
        ;
        ;We're all done so exit.
        ;
L13:    ret
Decode  endp

EntryIP dw ?
EntryCS dw ?

        end     start

