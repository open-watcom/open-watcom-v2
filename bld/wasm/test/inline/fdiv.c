int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "fdiv dword ptr x"
r01() { p01(); }
#pragma aux p02 = "fdiv qword ptr x"
r02() { p02(); }
#pragma aux p04 = "fdiv st,st(0)"
r04() { p04(); }
#pragma aux p06 = "fdiv st,st(2)"
r06() { p06(); }
#pragma aux p08 = "fdiv st(2),st"
r08() { p08(); }
#pragma aux p10 = "fdivp st(0),st"
r10() { p10(); }
#pragma aux p12 = "fdivp st(2),st"
r12() { p12(); }
#pragma aux p14 = "fdivp st(1),st"
r14() { p14(); }
#pragma aux p16 = "fidiv dword ptr x"
r16() { p16(); }
#pragma aux p18 = "fidiv word ptr x"
r18() { p18(); }
#pragma aux p20 = "fdivr dword ptr x"
r20() { p20(); }
#pragma aux p22 = "fdivr qword ptr x"
r22() { p22(); }
#pragma aux p24 = "fdivr st,st(0)"
r24() { p24(); }
#pragma aux p26 = "fdivr st,st(2)"
r26() { p26(); }
#pragma aux p28 = "fdivr st(2),st"
r28() { p28(); }
#pragma aux p30 = "fdivrp st(0),st"
r30() { p30(); }
#pragma aux p32 = "fdivrp st(2),st"
r32() { p32(); }
#pragma aux p34 = "fdivrp st(1),st"
r34() { p34(); }
#pragma aux p36 = "fidivr dword ptr x"
r36() { p36(); }
#pragma aux p38 = "fidivr word ptr x"
r38() { p38(); }
