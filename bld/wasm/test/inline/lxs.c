int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "lds si,x"
r01() { p01(); }
#pragma aux p02 = "lds esi,x"
r02() { p02(); }
#pragma aux p04 = "lss si,x"
r04() { p04(); }
#pragma aux p06 = "lss esi,x"
r06() { p06(); }
#pragma aux p08 = "lfs si,x"
r08() { p08(); }
#pragma aux p10 = "lfs esi,x"
r10() { p10(); }
#pragma aux p12 = "lgs si,x"
r12() { p12(); }
#pragma aux p14 = "lgs esi,x"
r14() { p14(); }
#pragma aux p16 = "les si,x"
r16() { p16(); }
#pragma aux p18 = "les esi,x"
r18() { p18(); }
