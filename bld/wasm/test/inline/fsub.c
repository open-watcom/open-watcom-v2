int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "fsub dword ptr x"
r01() { p01(); }
#pragma aux p02 = "fsub qword ptr x"
r02() { p02(); }
#pragma aux p04 = "fsub st,st(0)"
r04() { p04(); }
#pragma aux p06 = "fsub st,st(2)"
r06() { p06(); }
#pragma aux p08 = "fsub st(2),st"
r08() { p08(); }
#pragma aux p10 = "fsubp st(0),st"
r10() { p10(); }
#pragma aux p12 = "fsubp st(2),st"
r12() { p12(); }
#pragma aux p14 = "fsubp st(1),st"
r14() { p14(); }
#pragma aux p16 = "fisub dword ptr x"
r16() { p16(); }
#pragma aux p18 = "fisub word ptr x"
r18() { p18(); }
#pragma aux p20 = "fsubr dword ptr x"
r20() { p20(); }
#pragma aux p22 = "fsubr qword ptr x"
r22() { p22(); }
#pragma aux p24 = "fsubr st,st(0)"
r24() { p24(); }
#pragma aux p26 = "fsubr st,st(2)"
r26() { p26(); }
#pragma aux p28 = "fsubr st(2),st"
r28() { p28(); }
#pragma aux p30 = "fsubrp st(0),st"
r30() { p30(); }
#pragma aux p32 = "fsubrp st(2),st"
r32() { p32(); }
#pragma aux p34 = "fsubrp st(1),st"
r34() { p34(); }
#pragma aux p36 = "fisubr dword ptr x"
r36() { p36(); }
#pragma aux p38 = "fisubr word ptr x"
r38() { p38(); }
