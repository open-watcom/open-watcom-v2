int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "mov cr0,esi"
r01() { p01(); }
#pragma aux p02 = "mov cr2,esi"
r02() { p02(); }
#pragma aux p04 = "mov cr3,esi"
r04() { p04(); }
#pragma aux p06 = "mov edi,cr0"
r06() { p06(); }
#pragma aux p08 = "mov edi,cr2"
r08() { p08(); }
#pragma aux p10 = "mov edi,cr3"
r10() { p10(); }
#pragma aux p12 = "mov eax,dr0"
r12() { p12(); }
#pragma aux p14 = "mov esi,dr1"
r14() { p14(); }
#pragma aux p16 = "mov esi,dr2"
r16() { p16(); }
#pragma aux p18 = "mov esi,dr3"
r18() { p18(); }
#pragma aux p20 = "mov esi,dr6"
r20() { p20(); }
#pragma aux p22 = "mov esi,dr7"
r22() { p22(); }
#pragma aux p24 = "mov dr0,eax"
r24() { p24(); }
#pragma aux p26 = "mov dr1,ebx"
r26() { p26(); }
#pragma aux p28 = "mov dr2,ecx"
r28() { p28(); }
#pragma aux p30 = "mov dr3,edx"
r30() { p30(); }
#pragma aux p32 = "mov dr6,esi"
r32() { p32(); }
#pragma aux p34 = "mov dr7,edi"
r34() { p34(); }
#pragma aux p36 = "mov eax,tr3"
r36() { p36(); }
#pragma aux p38 = "mov esi,tr4"
r38() { p38(); }
#pragma aux p40 = "mov esi,tr5"
r40() { p40(); }
#pragma aux p42 = "mov esi,tr6"
r42() { p42(); }
#pragma aux p44 = "mov esi,tr7"
r44() { p44(); }
#pragma aux p46 = "mov tr3,eax"
r46() { p46(); }
#pragma aux p48 = "mov tr4,ebx"
r48() { p48(); }
#pragma aux p50 = "mov tr5,ecx"
r50() { p50(); }
#pragma aux p52 = "mov tr6,edx"
r52() { p52(); }
#pragma aux p54 = "mov tr7,esi"
r54() { p54(); }
