int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "not byte ptr x"
r1() { p1(); }
#pragma aux p2 = "not al"
r2() { p2(); }
#pragma aux p4 = "not byte ptr -0fH[esp+edi*4]"
r4() { p4(); }
#pragma aux p6 = "not dx"
r6() { p6(); }
#pragma aux p08 = "not word ptr +0fH[edx+ebp*2]"
r08() { p08(); }
#pragma aux p10 = "not esi"
r10() { p10(); }
#pragma aux p12 = "not dword ptr -0fH[eax+edx*8]"
r12() { p12(); }
