int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "f2xm1"
r01() { p01(); }
#pragma aux p02 = "fabs"
r02() { p02(); }
#pragma aux p04 = "fbld tbyte ptr x"
r04() { p04(); }
#pragma aux p06 = "fbstp tbyte ptr x"
r06() { p06(); }
#pragma aux p08 = "fchs"
r08() { p08(); }
#pragma aux p10 = "fclex"
r10() { p10(); }
#pragma aux p12 = "fcos"
r12() { p12(); }
#pragma aux p14 = "fdecstp"
r14() { p14(); }
#pragma aux p16 = "ffree st(0)"
r16() { p16(); }
#pragma aux p18 = "ffree st(2)"
r18() { p18(); }
#pragma aux p20 = "fincstp"
r20() { p20(); }
#pragma aux p22 = "finit"
r22() { p22(); }
#pragma aux p24 = "fld1"
r24() { p24(); }
#pragma aux p26 = "fldl2t"
r26() { p26(); }
#pragma aux p28 = "fldl2e"
r28() { p28(); }
#pragma aux p30 = "fldpi"
r30() { p30(); }
#pragma aux p32 = "fldlg2"
r32() { p32(); }
#pragma aux p34 = "fldln2"
r34() { p34(); }
#pragma aux p36 = "fldz"
r36() { p36(); }
#pragma aux p38 = "fstcw word ptr x"
r38() { p38(); }
#pragma aux p40 = "fstsw word ptr x"
r40() { p40(); }
#pragma aux p42 = "fstsw ax"
r42() { p42(); }
#pragma aux p44 = "fldcw word ptr x"
r44() { p44(); }
#pragma aux p46 = "fstenv dword ptr x"
r46() { p46(); }
#pragma aux p48 = "fldenv dword ptr x"
r48() { p48(); }
#pragma aux p50 = "fnop"
r50() { p50(); }
#pragma aux p52 = "fpatan"
r52() { p52(); }
#pragma aux p54 = "fprem"
r54() { p54(); }
#pragma aux p56 = "fptan"
r56() { p56(); }
#pragma aux p58 = "frndint"
r58() { p58(); }
#pragma aux p60 = "frstor dword ptr x"
r60() { p60(); }
#pragma aux p62 = "fsave dword ptr x"
r62() { p62(); }
#pragma aux p64 = "fscale"
r64() { p64(); }
#pragma aux p66 = "fsin"
r66() { p66(); }
#pragma aux p68 = "fsincos"
r68() { p68(); }
#pragma aux p70 = "fsqrt"
r70() { p70(); }
#pragma aux p72 = "fprem1"
r72() { p72(); }
