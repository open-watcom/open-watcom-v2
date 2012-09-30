        .model tiny

CWCStackSize    equ     1024    ; cannot exceed EXECopyStubLen size (1135 bytes)

        .code

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

b       equ     byte ptr
w       equ     word ptr
d       equ     dword ptr
f       equ     fword ptr

start:  cli
        push    ss              ;make data addressable.
        pop     ds
        mov     ax,es:[2]               ;Get highest address.

;       sub     ax,10h
        sub     ax,(CWCStackSize/10h)

        mov     ss,ax
        sti
        push    es
        push    ss
        pop     es
;       mov     si,100h-10h
        mov     si,CWCStackSize-10h

        mov     di,si
        mov     cx,10h
        rep     movsb
        pop     es
        mov     ss:w[var_EntryES],es    ;need this for program entry.
        ;
        ;Setup destination copy address.
        ;
        mov     bx,ax
        mov     dx,cs
        add     dx,ss:w[var_SourceSeg]
        mov     bp,ss:w[var_CopyLen]
        ;
        ;Do the copy.
        ;
        std
L1:     mov     ax,bp
        cmp     ax,1000h
        jbe     L2
        mov     ax,1000h
L2:     sub     bp,ax
        sub     dx,ax
        sub     bx,ax
        mov     ds,dx
        mov     es,bx
        mov     cl,3
        shl     ax,cl
        mov     cx,ax
        shl     ax,1
        dec     ax
        dec     ax
        mov     si,ax
        mov     di,ax
        rep     movsw
        or      bp,bp
        jnz     L1
        ;
        ;Point to the data to decompress.
        ;
        mov     si,es
        mov     ds,si
        xor     si,si
        mov     di,cs
        mov     es,di
        xor     di,di
        ;
        ;Setup the decompressor entry address.
        ;
        mov     ax,ds
        add     ss:w[var_EntryCS],ax
        ;
        ;Jump into the decompressor code.
        ;
        jmp     ss:dword ptr[var_EntryIP]

        db 1056 dup (0)

        end     start

