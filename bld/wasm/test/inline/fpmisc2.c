int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "ftst"
r1() { p1(); }
#pragma aux p2 = "fwait"
r2() { p2(); }
#pragma aux p04 = "fxam"
r04() { p04(); }
#pragma aux p06 = "fxch st(0)"
r06() { p06(); }
#pragma aux p08 = "fxch st(1)"
r08() { p08(); }
#pragma aux p10 = "fxch st(2)"
r10() { p10(); }
#pragma aux p12 = "fxtract"
r12() { p12(); }
#pragma aux p14 = "fyl2x"
r14() { p14(); }
#pragma aux p16 = "fyl2xp1"
r16() { p16(); }
