int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "test al,0fH"
r01() { p01(); }
#pragma aux p02 = "test ax,07ffH"
r02() { p02(); }
#pragma aux p04 = "test eax,0f0f0f0fH"
r04() { p04(); }
#pragma aux p06 = "test dl,0fH"
r06() { p06(); }
#pragma aux p08 = "test dx,07ffH"
r08() { p08(); }
#pragma aux p10 = "test eax,0f0f0f0fH"
r10() { p10(); }
#pragma aux p12 = "test byte ptr x,0fH"
r12() { p12(); }
#pragma aux p14 = "test word ptr x,07ffH"
r14() { p14(); }
#pragma aux p16 = "test dword ptr x,0f0f0f0fH"
r16() { p16(); }
#pragma aux p18 = "test dl,bl"
r18() { p18(); }
#pragma aux p20 = "test dx,bx"
r20() { p20(); }
#pragma aux p22 = "test eax,ebx"
r22() { p22(); }
#pragma aux p24 = "test byte ptr x,bl"
r24() { p24(); }
#pragma aux p26 = "test word ptr x,bx"
r26() { p26(); }
#pragma aux p28 = "test dword ptr x,ebx"
r28() { p28(); }
