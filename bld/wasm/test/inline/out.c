int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "out 0fH,al"
r1() { p1(); }
#pragma aux p2 = "out 0fH,ax"
r2() { p2(); }
#pragma aux p4 = "out 0fH,eax"
r4() { p4(); }
#pragma aux p6 = "out dx,al"
r6() { p6(); }
#pragma aux p8 = "out dx,ax"
r8() { p8(); }
#pragma aux p10 = "out dx,eax"
r10() { p10(); }
