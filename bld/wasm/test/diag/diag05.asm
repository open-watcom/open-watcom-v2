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
assume ds:_d16
	push byte ptr d16w
	push byte ptr [bx]
	push byte ptr [ebx]
	push byte ptr d16w [bx + 2]
	push byte ptr d16d [bx + 2]
	pushw byte ptr d16w
	pushw byte ptr [bx]
	pushw byte ptr [ebx]
	pushw byte ptr d16w [bx + 2]
	pushw byte ptr d16d [bx + 2]
	pushd byte ptr d16w
	pushd byte ptr [bx]
	pushd byte ptr [ebx]
	pushd byte ptr d16w [bx + 2]
	pushd byte ptr d16d [bx + 2]
assume ds:_d32
	push byte ptr d32w
	push byte ptr [bx]
	push byte ptr [ebx]
	push byte ptr d32w [bx + 2]
	push byte ptr d32d [bx + 2]
	pushw byte ptr d32w
	pushw byte ptr [bx]
	pushw byte ptr [ebx]
	pushw byte ptr d32w [bx + 2]
	pushw byte ptr d32d [bx + 2]
	pushd byte ptr d32w
	pushd byte ptr [bx]
	pushd byte ptr [ebx]
	pushd byte ptr d32w [bx + 2]
	pushd byte ptr d32d [bx + 2]
_c16 ends

_c32 segment use32 'CODE'
assume ds:_d16
	push byte ptr d16w
	push byte ptr [bx]
	push byte ptr [ebx]
	push byte ptr d16w [bx + 2]
	push byte ptr d16d [bx + 2]
	pushw byte ptr d16w
	pushw byte ptr [bx]
	pushw byte ptr [ebx]
	pushw byte ptr d16w [bx + 2]
	pushw byte ptr d16d [bx + 2]
	pushd byte ptr d16w
	pushd byte ptr [bx]
	pushd byte ptr [ebx]
	pushd byte ptr d16w [bx + 2]
	pushd byte ptr d16d [bx + 2]
assume ds:_d32
	push byte ptr d32w
	push byte ptr [bx]
	push byte ptr [ebx]
	push byte ptr d32w [bx + 2]
	push byte ptr d32d [bx + 2]
	pushw byte ptr d32w
	pushw byte ptr [bx]
	pushw byte ptr [ebx]
	pushw byte ptr d32w [bx + 2]
	pushw byte ptr d32d [bx + 2]
	pushd byte ptr d32w
	pushd byte ptr [bx]
	pushd byte ptr [ebx]
	pushd byte ptr d32w [bx + 2]
	pushd byte ptr d32d [bx + 2]
_c32 ends

end
