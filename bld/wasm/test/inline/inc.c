int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "inc byte ptr x"
r1() { p1(); }
#pragma aux p2 = "inc al"
r2() { p2(); }
#pragma aux p4 = "inc byte ptr -0fH[esp+edi*4]"
r4() { p4(); }
#pragma aux p6 = "inc dx"
r6() { p6(); }
#pragma aux p08 = "inc word ptr +0fH[edx+ebp*2]"
r08() { p08(); }
#pragma aux p10 = "inc esi"
r10() { p10(); }
#pragma aux p12 = "inc dword ptr -0fH[eax+edx*8]"
r12() { p12(); }
