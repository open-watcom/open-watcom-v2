int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "fmul dword ptr x"
r01() { p01(); }
#pragma aux p02 = "fmul qword ptr x"
r02() { p02(); }
#pragma aux p04 = "fmul st,st(0)"
r04() { p04(); }
#pragma aux p06 = "fmul st,st(2)"
r06() { p06(); }
#pragma aux p08 = "fmul st(2),st"
r08() { p08(); }
#pragma aux p10 = "fmulp st(0),st"
r10() { p10(); }
#pragma aux p12 = "fmulp st(2),st"
r12() { p12(); }
#pragma aux p14 = "fmulp st(1),st"
r14() { p14(); }
#pragma aux p16 = "fimul dword ptr x"
r16() { p16(); }
#pragma aux p18 = "fimul word ptr x"
r18() { p18(); }
