.386

CMD_OPTION equ ' C'

_d16 segment use16 'DATA'
d16w dw 0
d16d dd 0
_d16 ends

_d32 segment use32 'DATA'
d32w dw 0
d32d dd 0
_d32 ends

_c16 segment use16 'CODE'
	push 7fh
	push 0ffh
	push 07fffh
	push 0ffffh
	push 07fffffffh
	push 0ffffffffh
	pushw 7fh
	pushw 0ffh
	pushw 07fffh
	pushw 0ffffh
	pushw 07fffffffh
	pushw 0ffffffffh
	pushd 7fh
	pushd 0ffh
	pushd 07fffh
	pushd 0ffffh
	pushd 07fffffffh
	pushd 0ffffffffh
	push word ptr 7fh
	push word ptr 0ffh
	push word ptr 07fffh
	push word ptr 0ffffh
	push word ptr 07fffffffh
	push word ptr 0ffffffffh
	push dword ptr 7fh
	push dword ptr 0ffh
	push dword ptr 07fffh
	push dword ptr 0ffffh
	push dword ptr 07fffffffh
	push dword ptr 0ffffffffh
        push _c16
        push seg _c16
        push seg c16
        push _c32
        push seg _c32
        push seg c32
        pushw _c16
        pushw seg _c16
        pushw seg c16
        pushw _c32
        pushw seg _c32
        pushw seg c32
        pushd _c16
        pushd seg _c16
        pushd seg c16
        pushd _c32
        pushd seg _c32
        pushd seg c32
        push c16
        push offset c16
        push c32
        push offset c32
        push $
        push $ + 3
        push ss:1
        push ss:[1]
assume ds:_d16
	push d16w
	push d16d
	push word ptr d16w
	push word ptr d16d
	push dword ptr d16w
	push dword ptr d16d
	push [bx]
	push [ebx]
	push d16w [bx + 2]
	push d16d [bx + 2]
	push word ptr [bx]
	push word ptr [ebx]
	push word ptr d16w [bx + 2]
	push word ptr d16d [bx + 2]
	push dword ptr [bx]
	push dword ptr [ebx]
	push dword ptr d16w [bx + 2]
	push dword ptr d16d [bx + 2]
	push offset d16w
	push offset d16d
	pushw offset d16w
	pushw offset d16d
	pushd offset d16w
	pushd offset d16d
assume ds:_d32
	push d32w
	push d32d
	push word ptr d32w
	push word ptr d32d
	push dword ptr d32w
	push dword ptr d32d
	push [bx]
	push [ebx]
	push d32w [ebx + 2]
	push d32d [ebx + 2]
	push word ptr [bx]
	push word ptr [ebx]
	push word ptr d32w [ebx + 2]
	push word ptr d32d [ebx + 2]
	push dword ptr [bx]
	push dword ptr [ebx]
	push dword ptr d32w [ebx + 2]
	push dword ptr d32d [ebx + 2]
	push offset d32w
	push offset d32d
	pushd offset d32w
	pushd offset d32d
c16:
        pushw CMD_OPTION
        pushd CMD_OPTION
        push word ptr CMD_OPTION
        push dword ptr CMD_OPTION
_c16 ends

_c32 segment use32 'CODE'
	push 7fh
	push 0ffh
	push 07fffh
	push 0ffffh
	push 07fffffffh
	push 0ffffffffh
	pushw 7fh
	pushw 0ffh
	pushw 07fffh
	pushw 0ffffh
	pushw 07fffffffh
	pushw 0ffffffffh
	pushd 7fh
	pushd 0ffh
	pushd 07fffh
	pushd 0ffffh
	pushd 07fffffffh
	pushd 0ffffffffh
	push word ptr 7fh
	push word ptr 0ffh
	push word ptr 07fffh
	push word ptr 0ffffh
	push word ptr 07fffffffh
	push word ptr 0ffffffffh
	push dword ptr 7fh
	push dword ptr 0ffh
	push dword ptr 07fffh
	push dword ptr 0ffffh
	push dword ptr 07fffffffh
	push dword ptr 0ffffffffh
        push _c16
        push seg _c16
        push seg c16
        push _c32
        push seg _c32
        push seg c32
        pushw _c16
        pushw seg _c16
        pushw seg c16
        pushw _c32
        pushw seg _c32
        pushw seg c32
        pushd _c16
        pushd seg _c16
        pushd seg c16
        pushd _c32
        pushd seg _c32
        pushd seg c32
        push c16
        push offset c16
        push c32
        push offset c32
        push $
        push $ + 5
        push ss:1
        push ss:[1]
assume ds:_d16
	push d16w
	push d16d
	push word ptr d16w
	push word ptr d16d
	push dword ptr d16w
	push dword ptr d16d
	push [bx]
	push [ebx]
	push d16w [bx + 2]
	push d16d [bx + 2]
	push word ptr [bx]
	push word ptr [ebx]
	push word ptr d16w [bx + 2]
	push word ptr d16d [bx + 2]
	push dword ptr [bx]
	push dword ptr [ebx]
	push dword ptr d16w [bx + 2]
	push dword ptr d16d [bx + 2]
	push offset d16w
	push offset d16d
	pushw offset d16w
	pushw offset d16d
	pushd offset d16w
	pushd offset d16d
assume ds:_d32
	push d32w
	push d32d
	push word ptr d32w
	push word ptr d32d
	push dword ptr d32w
	push dword ptr d32d
	push [bx]
	push [ebx]
	push d32w [ebx + 2]
	push d32d [ebx + 2]
	push word ptr [bx]
	push word ptr [ebx]
	push word ptr d32w [ebx + 2]
	push word ptr d32d [ebx + 2]
	push dword ptr [bx]
	push dword ptr [ebx]
	push dword ptr d32w [ebx + 2]
	push dword ptr d32d [ebx + 2]
	push offset d32w
	push offset d32d
	pushd offset d32w
	pushd offset d32d
c32:
        pushw CMD_OPTION
        pushd CMD_OPTION
        push word ptr CMD_OPTION
        push dword ptr CMD_OPTION
_c32 ends

end
