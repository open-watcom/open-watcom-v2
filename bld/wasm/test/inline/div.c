int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "div bl"
r1() { p1(); }
#pragma aux p2 = "div byte ptr x"
r2() { p2(); }
#pragma aux p04 = "div byte ptr cs:[esi]"
r04() { p04(); }
#pragma aux p06 = "div si"
r06() { p06(); }
#pragma aux p08 = "div word ptr x"
r08() { p08(); }
#pragma aux p10 = "div word ptr ss:+10H[eax+esi*2]"
r10() { p10(); }
#pragma aux p12 = "div edi"
r12() { p12(); }
#pragma aux p14 = "div dword ptr x"
r14() { p14(); }
#pragma aux p16 = "div dword ptr ss:-7ffffH[eax+esi*2]"
r16() { p16(); }
