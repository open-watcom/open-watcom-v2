.386
SEGMENTED=1
.model small

xreal10  struc
        bytes1to4       dd      0
        bytes5to8       dd      0
        bytes9n10       dw      0
xreal10  ends

mov10   macro   src_seg,src_off,dst_seg,dst_off
        mov     eax,src_seg:src_off.bytes1to4
        mov     dst_seg:dst_off.bytes1to4,eax
        mov     eax,src_seg:src_off.bytes5to8
        mov     dst_seg:dst_off.bytes5to8,eax
            mov ax,src_seg:src_off.bytes9n10
            mov dst_seg:dst_off.bytes9n10,ax
        endm
.data
foo xreal10 <>
bar xreal10 <>
sam db 0,1,2,3,4,5,6,7,8,9

.code
    mov10 es, foo, ds, bar
    mov10 es, [ebx], ds, [ecx+2]
    mov10 es, foo, ds, sam

;mov eax, ds:foo.bytes1to4
;mov ds:bar.bytes1to4, eax
end
