int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "fcom dword ptr x"
r01() { p01(); }
#pragma aux p02 = "fcom qword ptr x"
r02() { p02(); }
#pragma aux p04 = "fcom st(0)"
r04() { p04(); }
#pragma aux p06 = "fcom st(2)"
r06() { p06(); }
#pragma aux p08 = "fcom st(1)"
r08() { p08(); }
#pragma aux p10 = "fcomp dword ptr x"
r10() { p10(); }
#pragma aux p12 = "fcomp qword ptr x"
r12() { p12(); }
#pragma aux p14 = "fcomp st(0)"
r14() { p14(); }
#pragma aux p16 = "fcomp st(2)"
r16() { p16(); }
#pragma aux p18 = "fcompp"
r18() { p18(); }
#pragma aux p20 = "ficom word ptr x"
r20() { p20(); }
#pragma aux p22 = "ficom dword ptr x"
r22() { p22(); }
#pragma aux p24 = "ficomp word ptr x"
r24() { p24(); }
#pragma aux p26 = "ficomp dword ptr x"
r26() { p26(); }
