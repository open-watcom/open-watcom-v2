int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "in al,0fH"
r1() { p1(); }
#pragma aux p2 = "in ax,0fH"
r2() { p2(); }
#pragma aux p4 = "in eax,0fH"
r4() { p4(); }
#pragma aux p6 = "in al,dx"
r6() { p6(); }
#pragma aux p8 = "in ax,dx"
r8() { p8(); }
#pragma aux p10 = "in eax,dx"
r10() { p10(); }
