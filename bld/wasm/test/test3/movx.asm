.386

_d16 segment use16 'DATA'

x16b db 0
x16w dw 0

_d16 ends

_d32 segment use32 'DATA'

x32b db 0
x32w dw 0

_d32 ends

_c16 segment use16 'CODE'

    movsx ax,dl
    movsx eax,bh
    movsx eax,si

    movzx ax,dl
    movzx eax,bh
    movzx eax,si

assume ds:_d16
    movsx ax,x16b
    movsx eax,x16b
    movsx eax,x16w
    movsx ax,byte ptr x16w
    movsx eax,byte ptr x16w
    movsx eax,word ptr x16b

    movzx ax,x16b
    movzx eax,x16b
    movzx eax,x16w
    movzx ax,byte ptr x16w
    movzx eax,byte ptr x16w
    movzx eax,word ptr x16b

assume ds:_d32
    movsx ax,x32b
    movsx eax,x32b
    movsx eax,x32w
    movsx ax,byte ptr x32w
    movsx eax,byte ptr x32w
    movsx eax,word ptr x32b

    movzx ax,x32b
    movzx eax,x32b
    movzx eax,x32w
    movzx ax,byte ptr x32w
    movzx eax,byte ptr x32w
    movzx eax,word ptr x32b

_c16 ends

_c32 segment use32 'CODE'

    movsx ax,dl
    movsx eax,bh
    movsx eax,si

    movzx ax,dl
    movzx eax,bh
    movzx eax,si

assume ds:_d16
    movsx ax,x16b
    movsx eax,x16b
    movsx eax,x16w
    movsx ax,byte ptr x16w
    movsx eax,byte ptr x16w
    movsx eax,word ptr x16b

    movzx ax,x16b
    movzx eax,x16b
    movzx eax,x16w
    movzx ax,byte ptr x16w
    movzx eax,byte ptr x16w
    movzx eax,word ptr x16b

assume ds:_d32
    movsx ax,x32b
    movsx eax,x32b
    movsx eax,x32w
    movsx ax,byte ptr x32w
    movsx eax,byte ptr x32w
    movsx eax,word ptr x32b

    movzx ax,x32b
    movzx eax,x32b
    movzx eax,x32w
    movzx ax,byte ptr x32w
    movzx eax,byte ptr x32w
    movzx eax,word ptr x32b

_c32 ends

end
