int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "fild word ptr x"
r01() { p01(); }
#pragma aux p02 = "fild dword ptr x"
r02() { p02(); }
#pragma aux p04 = "fild qword ptr x"
r04() { p04(); }
#pragma aux p06 = "fist word ptr x"
r06() { p06(); }
#pragma aux p08 = "fist dword ptr x"
r08() { p08(); }
#pragma aux p10 = "fistp word ptr x"
r10() { p10(); }
#pragma aux p12 = "fistp dword ptr x"
r12() { p12(); }
#pragma aux p14 = "fistp qword ptr x"
r14() { p14(); }
#pragma aux p16 = "fld dword ptr x"
r16() { p16(); }
#pragma aux p18 = "fld qword ptr x"
r18() { p18(); }
#pragma aux p20 = "fld tbyte ptr x"
r20() { p20(); }
#pragma aux p22 = "fld st(0)"
r22() { p22(); }
#pragma aux p24 = "fld st(2)"
r24() { p24(); }
#pragma aux p26 = "fst dword ptr x"
r26() { p26(); }
#pragma aux p28 = "fst qword ptr x"
r28() { p28(); }
#pragma aux p30 = "fst st(0)"
r30() { p30(); }
#pragma aux p32 = "fst st(2)"
r32() { p32(); }
#pragma aux p34 = "fstp dword ptr x"
r34() { p34(); }
#pragma aux p36 = "fstp qword ptr x"
r36() { p36(); }
#pragma aux p38 = "fstp st(0)"
r38() { p38(); }
#pragma aux p40 = "fstp st(2)"
r40() { p40(); }
#pragma aux p42 = "fstp tbyte ptr x"
r42() { p42(); }
