int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "xadd al,dh"
r1() { p1(); }
#pragma aux p2 = "xadd byte ptr x,ch"
r2() { p2(); }
#pragma aux p4 = "xadd bp,sp"
r4() { p4(); }
#pragma aux p6 = "xadd word ptr x,di"
r6() { p6(); }
#pragma aux p8 = "xadd edx,eax"
r8() { p8(); }
#pragma aux p10 = "xadd dword ptr x,eax"
r10() { p10(); }
