int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "lgdt word ptr x"
r01() { p01(); }
#pragma aux p02 = "lidt word ptr x"
r02() { p02(); }
#pragma aux p04 = "lldt word ptr x"
r04() { p04(); }
#pragma aux p06 = "lldt ax"
r06() { p06(); }
#pragma aux p08 = "lmsw ax"
r08() { p08(); }
#pragma aux p10 = "lmsw word ptr x"
r10() { p10(); }
#pragma aux p12 = "lsl si,di"
r12() { p12(); }
#pragma aux p14 = "lsl eax,ebx"
r14() { p14(); }
#pragma aux p16 = "lsl bx,word ptr x"
r16() { p16(); }
#pragma aux p18 = "lsl eax,dword ptr x"
r18() { p18(); }
#pragma aux p20 = "nop"
r20() { p20(); }
#pragma aux p22 = "ret"
r22() { p22(); }
#pragma aux p24 = "ret 0004H"
r24() { p24(); }
#pragma aux p26 = "sahf"
r26() { p26(); }
#pragma aux p28 = "sgdt word ptr x"
r28() { p28(); }
#pragma aux p30 = "sidt word ptr x"
r30() { p30(); }
#pragma aux p32 = "sldt word ptr x"
r32() { p32(); }
#pragma aux p34 = "sldt ax"
r34() { p34(); }
#pragma aux p36 = "smsw ax"
r36() { p36(); }
#pragma aux p38 = "smsw word ptr x"
r38() { p38(); }
#pragma aux p40 = "stc"
r40() { p40(); }
#pragma aux p42 = "std"
r42() { p42(); }
#pragma aux p44 = "sti"
r44() { p44(); }
#pragma aux p46 = "stosb"
r46() { p46(); }
#pragma aux p48 = "stosw"
r48() { p48(); }
#pragma aux p50 = "stosd"
r50() { p50(); }
#pragma aux p52 = "str bx"
r52() { p52(); }
#pragma aux p54 = "str word ptr x"
r54() { p54(); }
#pragma aux p56 = "verr si"
r56() { p56(); }
#pragma aux p58 = "verw bx"
r58() { p58(); }
#pragma aux p60 = "verr word ptr x"
r60() { p60(); }
#pragma aux p62 = "verw word ptr x"
r62() { p62(); }
#pragma aux p64 = "wait"
r64() { p64(); }
#pragma aux p66 = "xlatb"
r66() { p66(); }
