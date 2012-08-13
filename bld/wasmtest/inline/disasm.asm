 bswap eax
 bswap esi
 arpl ax,bx
 arpl word ptr x,bx
 fbld tbyte ptr x
 fbstp tbyte ptr x
 fldcw word ptr x
 fstcw word ptr x
 fstsw word ptr x
 hlt
 lar esi,edi
 lar eax,dword ptr x
 lsl eax,ebx
 lsl eax,dword ptr x
 fucompp
 fcompp
 lea si,x
 lea esi,x
 imul di,dx,0ffffH
 imul edi,edx,0ffffffffH
