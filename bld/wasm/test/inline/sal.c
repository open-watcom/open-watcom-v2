int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "shl dl,1"
r01() { p01(); }
#pragma aux p02 = "shl byte ptr [ebx],1"
r02() { p02(); }
#pragma aux p04 = "shl dl,cl"
r04() { p04(); }
#pragma aux p06 = "shl byte ptr [ebx],cl"
r06() { p06(); }
#pragma aux p08 = "shl dl,07H"
r08() { p08(); }
#pragma aux p10 = "shl byte ptr [ebx],07H"
r10() { p10(); }
#pragma aux p12 = "shl bx,1"
r12() { p12(); }
#pragma aux p14 = "shl word ptr [ebx],1"
r14() { p14(); }
#pragma aux p16 = "shl bx,cl"
r16() { p16(); }
#pragma aux p18 = "shl word ptr [ebx],cl"
r18() { p18(); }
#pragma aux p20 = "shl bx,07H"
r20() { p20(); }
#pragma aux p22 = "shl word ptr [ebx],07H"
r22() { p22(); }
#pragma aux p24 = "shl esi,1"
r24() { p24(); }
#pragma aux p26 = "shl dword ptr [ebx],1"
r26() { p26(); }
#pragma aux p28 = "shl esi,cl"
r28() { p28(); }
#pragma aux p30 = "shl dword ptr [ebx],cl"
r30() { p30(); }
#pragma aux p32 = "shl esi,07H"
r32() { p32(); }
#pragma aux p34 = "shl dword ptr [ebx],07H"
r34() { p34(); }
#pragma aux p36 = "shr dl,1"
r36() { p36(); }
#pragma aux p38 = "shr byte ptr [ebx],1"
r38() { p38(); }
#pragma aux p40 = "shr dl,cl"
r40() { p40(); }
#pragma aux p42 = "shr byte ptr [ebx],cl"
r42() { p42(); }
#pragma aux p44 = "shr dl,07H"
r44() { p44(); }
#pragma aux p46 = "shr byte ptr [ebx],07H"
r46() { p46(); }
#pragma aux p48 = "shr bx,1"
r48() { p48(); }
#pragma aux p50 = "shr word ptr [ebx],1"
r50() { p50(); }
#pragma aux p52 = "shr bx,cl"
r52() { p52(); }
#pragma aux p54 = "shr word ptr [ebx],cl"
r54() { p54(); }
#pragma aux p56 = "shr bx,07H"
r56() { p56(); }
#pragma aux p58 = "shr word ptr [ebx],07H"
r58() { p58(); }
#pragma aux p60 = "shr esi,1"
r60() { p60(); }
#pragma aux p62 = "shr dword ptr [ebx],1"
r62() { p62(); }
#pragma aux p64 = "shr esi,cl"
r64() { p64(); }
#pragma aux p66 = "shr dword ptr [ebx],cl"
r66() { p66(); }
#pragma aux p68 = "shr esi,07H"
r68() { p68(); }
#pragma aux p70 = "shr dword ptr [ebx],07H"
r70() { p70(); }
#pragma aux p72 = "sar dl,1"
r72() { p72(); }
#pragma aux p74 = "sar byte ptr [ebx],1"
r74() { p74(); }
#pragma aux p76 = "sar dl,cl"
r76() { p76(); }
#pragma aux p78 = "sar byte ptr [ebx],cl"
r78() { p78(); }
#pragma aux p80 = "sar dl,07H"
r80() { p80(); }
#pragma aux p82 = "sar byte ptr [ebx],07H"
r82() { p82(); }
#pragma aux p84 = "sar bx,1"
r84() { p84(); }
#pragma aux p86 = "sar word ptr [ebx],1"
r86() { p86(); }
#pragma aux p88 = "sar bx,cl"
r88() { p88(); }
#pragma aux p90 = "sar word ptr [ebx],cl"
r90() { p90(); }
#pragma aux p92 = "sar bx,07H"
r92() { p92(); }
#pragma aux p094 = "sar word ptr [ebx],07H"
r094() { p094(); }
#pragma aux p096 = "sar esi,1"
r096() { p096(); }
#pragma aux p098 = "sar dword ptr [ebx],1"
r098() { p098(); }
#pragma aux p100 = "sar esi,cl"
r100() { p100(); }
#pragma aux p102 = "sar dword ptr [ebx],cl"
r102() { p102(); }
#pragma aux p104 = "sar esi,07H"
r104() { p104(); }
#pragma aux p106 = "sar dword ptr [ebx],07H"
r106() { p106(); }
