 bswap eax
 bswap esi
 mov ds,bx
 mov word ptr x,0fH
 mov word ptr x,7fH
 mov dword ptr x,0fH
 mov dword ptr x,7fH
 mov dword ptr x,0ffffffffH
 bt dword ptr x,0fH
 bt dword ptr x,1aH
 cmpxchg al,dl
 cmpxchg byte ptr x,dl
 cmpxchg ax,dx
 cmpxchg word ptr x,dx
 cmpxchg eax,edx
 cmpxchg dword ptr x,edx
 enter	0ffffH,1
 enter	0fH,0ffH
 fucom st(2)
 fucom
 fucomp st(2)
 fucomp
 fucompp
