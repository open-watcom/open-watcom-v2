int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "shld ax,di,0fH"
r01() { p01(); }
#pragma aux p02 = "shld word ptr x,ax,0fH"
r02() { p02(); }
#pragma aux p04 = "shld eax,edi,0fH"
r04() { p04(); }
#pragma aux p06 = "shld dword ptr x,eax,0fH"
r06() { p06(); }
#pragma aux p08 = "shld ax,di,20H"
r08() { p08(); }
#pragma aux p10 = "shld word ptr x,ax,20H"
r10() { p10(); }
#pragma aux p12 = "shld eax,edi,20H"
r12() { p12(); }
#pragma aux p14 = "shld dword ptr x,eax,20H"
r14() { p14(); }
#pragma aux p16 = "shld ax,di,cl"
r16() { p16(); }
#pragma aux p18 = "shld word ptr x,ax,cl"
r18() { p18(); }
#pragma aux p20 = "shld eax,edi,cl"
r20() { p20(); }
#pragma aux p22 = "shld dword ptr x,eax,cl"
r22() { p22(); }
#pragma aux p24 = "shrd ax,di,0fH"
r24() { p24(); }
#pragma aux p26 = "shrd word ptr x,ax,0fH"
r26() { p26(); }
#pragma aux p28 = "shrd eax,edi,0fH"
r28() { p28(); }
#pragma aux p30 = "shrd dword ptr x,eax,0fH"
r30() { p30(); }
#pragma aux p32 = "shrd ax,di,20H"
r32() { p32(); }
#pragma aux p34 = "shrd word ptr x,ax,20H"
r34() { p34(); }
#pragma aux p36 = "shrd eax,edi,20H"
r36() { p36(); }
#pragma aux p38 = "shrd dword ptr x,eax,20H"
r38() { p38(); }
#pragma aux p40 = "shrd ax,di,cl"
r40() { p40(); }
#pragma aux p42 = "shrd word ptr x,ax,cl"
r42() { p42(); }
#pragma aux p44 = "shrd eax,edi,cl"
r44() { p44(); }
#pragma aux p46 = "shrd dword ptr x,eax,cl"
r46() { p46(); }
