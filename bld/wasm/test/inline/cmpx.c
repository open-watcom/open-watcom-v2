int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "cmpxchg al,dl"
r1() { p1(); }
#pragma aux p2 = "cmpxchg byte ptr x,dl"
r2() { p2(); }
#pragma aux p4 = "cmpxchg ax,dx"
r4() { p4(); }
#pragma aux p6 = "cmpxchg word ptr x,dx"
r6() { p6(); }
#pragma aux p8 = "cmpxchg eax,edx"
r8() { p8(); }
#pragma aux p10 = "cmpxchg dword ptr x,edx"
r10() { p10(); }
