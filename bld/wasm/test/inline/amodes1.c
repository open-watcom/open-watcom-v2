int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "add eax,dword ptr [eax+eax*2]"
r01() { p01(); }
#pragma aux p02 = "add eax,dword ptr [eax+ebx*2]"
r02() { p02(); }
#pragma aux p04 = "add eax,dword ptr [eax+ecx*2]"
r04() { p04(); }
#pragma aux p06 = "add eax,dword ptr [eax+edx*2]"
r06() { p06(); }
#pragma aux p08 = "add eax,dword ptr [eax+esi*2]"
r08() { p08(); }
#pragma aux p10 = "add eax,dword ptr [eax+edi*2]"
r10() { p10(); }
#pragma aux p12 = "add eax,dword ptr [eax+ebp*2]"
r12() { p12(); }
#pragma aux p14 = "add eax,dword ptr [edx+eax*2]"
r14() { p14(); }
#pragma aux p16 = "add eax,dword ptr [edx+ebx*2]"
r16() { p16(); }
#pragma aux p18 = "add eax,dword ptr [edx+ecx*2]"
r18() { p18(); }
#pragma aux p20 = "add eax,dword ptr [edx+edx*2]"
r20() { p20(); }
#pragma aux p22 = "add eax,dword ptr [edx+esi*2]"
r22() { p22(); }
#pragma aux p24 = "add eax,dword ptr [edx+edi*2]"
r24() { p24(); }
#pragma aux p26 = "add eax,dword ptr [edx+ebp*2]"
r26() { p26(); }
#pragma aux p28 = "add eax,dword ptr [ebx+eax*2]"
r28() { p28(); }
#pragma aux p30 = "add eax,dword ptr [ebx+ebx*2]"
r30() { p30(); }
#pragma aux p32 = "add eax,dword ptr [ebx+ecx*2]"
r32() { p32(); }
#pragma aux p34 = "add eax,dword ptr [ebx+edx*2]"
r34() { p34(); }
#pragma aux p36 = "add eax,dword ptr [ebx+esi*2]"
r36() { p36(); }
#pragma aux p38 = "add eax,dword ptr [ebx+edi*2]"
r38() { p38(); }
#pragma aux p40 = "add eax,dword ptr [ebx+ebp*2]"
r40() { p40(); }
#pragma aux p42 = "add eax,dword ptr [ecx+eax*2]"
r42() { p42(); }
#pragma aux p44 = "add eax,dword ptr [ecx+ebx*2]"
r44() { p44(); }
#pragma aux p46 = "add eax,dword ptr [ecx+ecx*2]"
r46() { p46(); }
#pragma aux p48 = "add eax,dword ptr [ecx+edx*2]"
r48() { p48(); }
#pragma aux p50 = "add eax,dword ptr [ecx+esi*2]"
r50() { p50(); }
#pragma aux p52 = "add eax,dword ptr [ecx+edi*2]"
r52() { p52(); }
#pragma aux p54 = "add eax,dword ptr [ecx+ebp*2]"
r54() { p54(); }
#pragma aux p56 = "add eax,dword ptr [esi+eax*2]"
r56() { p56(); }
#pragma aux p58 = "add eax,dword ptr [esi+ebx*2]"
r58() { p58(); }
#pragma aux p60 = "add eax,dword ptr [esi+ecx*2]"
r60() { p60(); }
#pragma aux p62 = "add eax,dword ptr [esi+edx*2]"
r62() { p62(); }
#pragma aux p64 = "add eax,dword ptr [esi+esi*2]"
r64() { p64(); }
#pragma aux p66 = "add eax,dword ptr [esi+edi*2]"
r66() { p66(); }
#pragma aux p68 = "add eax,dword ptr [esi+ebp*2]"
r68() { p68(); }
#pragma aux p70 = "add eax,dword ptr [edi+eax*2]"
r70() { p70(); }
#pragma aux p72 = "add eax,dword ptr [edi+ebx*2]"
r72() { p72(); }
#pragma aux p74 = "add eax,dword ptr [edi+ecx*2]"
r74() { p74(); }
#pragma aux p76 = "add eax,dword ptr [edi+edx*2]"
r76() { p76(); }
#pragma aux p78 = "add eax,dword ptr [edi+esi*2]"
r78() { p78(); }
#pragma aux p80 = "add eax,dword ptr [edi+edi*2]"
r80() { p80(); }
#pragma aux p82 = "add eax,dword ptr [edi+ebp*2]"
r82() { p82(); }
#pragma aux p84 = "add eax,dword ptr [esp+eax*2]"
r84() { p84(); }
#pragma aux p86 = "add eax,dword ptr [esp+ebx*2]"
r86() { p86(); }
#pragma aux p88 = "add eax,dword ptr [esp+ecx*2]"
r88() { p88(); }
#pragma aux p090 = "add eax,dword ptr [esp+edx*2]"
r090() { p090(); }
#pragma aux p092 = "add eax,dword ptr [esp+esi*2]"
r092() { p092(); }
#pragma aux p094 = "add eax,dword ptr [esp+edi*2]"
r094() { p094(); }
#pragma aux p096 = "add eax,dword ptr [esp+ebp*2]"
r096() { p096(); }
#pragma aux p098 = "add eax,dword ptr +0H[ebp+eax*2]"
r098() { p098(); }
#pragma aux p100 = "add eax,dword ptr +0H[ebp+ebx*2]"
r100() { p100(); }
#pragma aux p102 = "add eax,dword ptr +0H[ebp+ecx*2]"
r102() { p102(); }
#pragma aux p104 = "add eax,dword ptr +0H[ebp+edx*2]"
r104() { p104(); }
#pragma aux p106 = "add eax,dword ptr +0H[ebp+esi*2]"
r106() { p106(); }
#pragma aux p108 = "add eax,dword ptr +0H[ebp+edi*2]"
r108() { p108(); }
#pragma aux p110 = "add eax,dword ptr +0H[ebp+ebp*2]"
r110() { p110(); }
