int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "neg byte ptr x"
r01() { p01(); }
#pragma aux p02 = "neg al"
r02() { p02(); }
#pragma aux p04 = "neg byte ptr -0fH[di]"
r04() { p04(); }
#pragma aux p06 = "neg byte ptr -0fH[esp+edi*4]"
r06() { p06(); }
#pragma aux p08 = "neg dx"
r08() { p08(); }
#pragma aux p10 = "neg word ptr +0fH[bx]"
r10() { p10(); }
#pragma aux p12 = "neg word ptr +0fH[edx+ebp*2]"
r12() { p12(); }
#pragma aux p14 = "neg esi"
r14() { p14(); }
#pragma aux p16 = "neg dword ptr -0fH[si]"
r16() { p16(); }
#pragma aux p18 = "neg dword ptr -0fH[eax+edx*8]"
r18() { p18(); }
