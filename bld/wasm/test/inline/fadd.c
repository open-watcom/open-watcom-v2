int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "fadd dword ptr x"
r01() { p01(); }
#pragma aux p02 = "fadd qword ptr x"
r02() { p02(); }
#pragma aux p04 = "fadd st,st(0)"
r04() { p04(); }
#pragma aux p06 = "fadd st,st(2)"
r06() { p06(); }
#pragma aux p08 = "fadd st(2),st"
r08() { p08(); }
#pragma aux p10 = "faddp st(0),st"
r10() { p10(); }
#pragma aux p12 = "faddp st(2),st"
r12() { p12(); }
#pragma aux p14 = "faddp st(1),st"
r14() { p14(); }
#pragma aux p16 = "fiadd dword ptr x"
r16() { p16(); }
#pragma aux p18 = "fiadd word ptr x"
r18() { p18(); }
