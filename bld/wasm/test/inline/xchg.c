int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "xchg ax,si"
r01() { p01(); }
#pragma aux p02 = "xchg ax,dx"
r02() { p02(); }
#pragma aux p04 = "xchg eax,ebp"
r04() { p04(); }
#pragma aux p06 = "xchg eax,ebp"
r06() { p06(); }
#pragma aux p08 = "xchg bh,cl"
r08() { p08(); }
#pragma aux p10 = "xchg ah,byte ptr x"
r10() { p10(); }
#pragma aux p12 = "xchg cx,bx"
r12() { p12(); }
#pragma aux p14 = "xchg di,word ptr x"
r14() { p14(); }
#pragma aux p16 = "xchg ecx,ebx"
r16() { p16(); }
#pragma aux p18 = "xchg esp,dword ptr x"
r18() { p18(); }
