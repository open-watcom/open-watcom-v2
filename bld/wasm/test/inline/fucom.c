int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "fucom st(2)"
r1() { p1(); }
#pragma aux p2 = "fucom"
r2() { p2(); }
#pragma aux p4 = "fucomp st(2)"
r4() { p4(); }
#pragma aux p6 = "fucomp"
r6() { p6(); }
#pragma aux p8 = "fucompp"
r8() { p8(); }
