.386
.model small
.code
mov byte ptr [bx], 0x0f
mov [bx], 0x0f
nop
mov word ptr [bx], 0x0fff
mov [bx], 0x0fff
nop
mov dword ptr [bx], 0x0fffffff
mov [bx], 0x0fffffff
end
