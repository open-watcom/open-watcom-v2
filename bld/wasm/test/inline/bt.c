int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "bt dx,ax"
r01() { p01(); }
#pragma aux p02 = "bt word ptr x,dx"
r02() { p02(); }
#pragma aux p04 = "bt eax,edx"
r04() { p04(); }
#pragma aux p06 = "bt dword ptr x,edx"
r06() { p06(); }
#pragma aux p08 = "bt ax,0fH"
r08() { p08(); }
#pragma aux p10 = "bt ax,30H"
r10() { p10(); }
#pragma aux p12 = "bt word ptr x,0fH"
r12() { p12(); }
#pragma aux p14 = "bt word ptr x,1aH"
r14() { p14(); }
#pragma aux p16 = "bt eax,0fH"
r16() { p16(); }
#pragma aux p18 = "bt eax,1aH"
r18() { p18(); }
#pragma aux p20 = "bt dword ptr x,0fH"
r20() { p20(); }
#pragma aux p22 = "bt dword ptr x,1aH"
r22() { p22(); }
#pragma aux p24 = "btc dx,ax"
r24() { p24(); }
#pragma aux p26 = "btc word ptr x,dx"
r26() { p26(); }
#pragma aux p28 = "btc eax,edx"
r28() { p28(); }
#pragma aux p30 = "btc dword ptr x,edx"
r30() { p30(); }
#pragma aux p32 = "btc ax,0fH"
r32() { p32(); }
#pragma aux p34 = "btc ax,30H"
r34() { p34(); }
#pragma aux p36 = "btc word ptr x,0fH"
r36() { p36(); }
#pragma aux p38 = "btc word ptr x,1aH"
r38() { p38(); }
#pragma aux p40 = "btc eax,0fH"
r40() { p40(); }
#pragma aux p42 = "btc eax,1aH"
r42() { p42(); }
#pragma aux p44 = "btc dword ptr x,0fH"
r44() { p44(); }
#pragma aux p46 = "btc dword ptr x,1aH"
r46() { p46(); }
#pragma aux p48 = "btr dx,ax"
r48() { p48(); }
#pragma aux p50 = "btr word ptr x,dx"
r50() { p50(); }
#pragma aux p52 = "btr eax,edx"
r52() { p52(); }
#pragma aux p54 = "btr dword ptr x,edx"
r54() { p54(); }
#pragma aux p56 = "btr ax,0fH"
r56() { p56(); }
#pragma aux p58 = "btr ax,30H"
r58() { p58(); }
#pragma aux p60 = "btr word ptr x,0fH"
r60() { p60(); }
#pragma aux p62 = "btr word ptr x,1aH"
r62() { p62(); }
#pragma aux p64 = "btr eax,0fH"
r64() { p64(); }
#pragma aux p66 = "btr eax,1aH"
r66() { p66(); }
#pragma aux p68 = "btr dword ptr x,0fH"
r68() { p68(); }
#pragma aux p70 = "btr dword ptr x,1aH"
r70() { p70(); }
#pragma aux p72 = "bts dx,ax"
r72() { p72(); }
#pragma aux p74 = "bts word ptr x,dx"
r74() { p74(); }
#pragma aux p76 = "bts eax,edx"
r76() { p76(); }
#pragma aux p78 = "bts dword ptr x,edx"
r78() { p78(); }
#pragma aux p80 = "bts ax,0fH"
r80() { p80(); }
#pragma aux p82 = "bts ax,30H"
r82() { p82(); }
#pragma aux p84 = "bts word ptr x,0fH"
r84() { p84(); }
#pragma aux p86 = "bts word ptr x,1aH"
r86() { p86(); }
#pragma aux p88 = "bts eax,0fH"
r88() { p88(); }
#pragma aux p90 = "bts eax,1aH"
r90() { p90(); }
#pragma aux p92 = "bts dword ptr x,0fH"
r92() { p92(); }
#pragma aux p94 = "bts dword ptr x,1aH"
r94() { p94(); }
