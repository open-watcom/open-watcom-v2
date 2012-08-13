.386
.model small
.code
mov byte ptr [bx], 0fh
mov [bx], 0fh
nop
mov word ptr [bx], 0fffh
mov [bx], 0fffh
nop
mov dword ptr [bx], 0fffffffh
mov [bx], 0fffffffh
end
