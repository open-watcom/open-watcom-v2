int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "aaa"
r01() { p01(); }
#pragma aux p02 = "aad"
r02() { p02(); }
#pragma aux p04 = "aam"
r04() { p04(); }
#pragma aux p06 = "aas"
r06() { p06(); }
#pragma aux p08 = "bound bx,word ptr x"
r08() { p08(); }
#pragma aux p10 = "bound ebx,dword ptr [ebx]"
r10() { p10(); }
#pragma aux p12 = "bsf bx,word ptr x"
r12() { p12(); }
#pragma aux p14 = "bsf ebx,dword ptr [ebx]"
r14() { p14(); }
#pragma aux p16 = "bsr bx,word ptr x"
r16() { p16(); }
#pragma aux p18 = "bsr ebx,dword ptr [ebx]"
r18() { p18(); }
#pragma aux p20 = "cbw"
r20() { p20(); }
#pragma aux p22 = "cwde"
r22() { p22(); }
#pragma aux p24 = "clc"
r24() { p24(); }
#pragma aux p26 = "cld"
r26() { p26(); }
#pragma aux p28 = "cli"
r28() { p28(); }
#pragma aux p30 = "clts"
r30() { p30(); }
#pragma aux p32 = "cmc"
r32() { p32(); }
#pragma aux p34 = "cmpsb"
r34() { p34(); }
#pragma aux p36 = "cmpsw"
r36() { p36(); }
#pragma aux p38 = "cmpsd"
r38() { p38(); }
#pragma aux p40 = "cwd"
r40() { p40(); }
#pragma aux p42 = "cdq"
r42() { p42(); }
#pragma aux p44 = "daa"
r44() { p44(); }
#pragma aux p46 = "das"
r46() { p46(); }
#pragma aux p48 = "hlt"
r48() { p48(); }
#pragma aux p50 = "insb"
r50() { p50(); }
#pragma aux p52 = "insw"
r52() { p52(); }
#pragma aux p54 = "insd"
r54() { p54(); }
#pragma aux p56 = "int 3"
r56() { p56(); }
#pragma aux p58 = "int 12H"
r58() { p58(); }
#pragma aux p60 = "into"
r60() { p60(); }
#pragma aux p62 = "invd"
r62() { p62(); }
#pragma aux p64 = "invlpg x"
r64() { p64(); }
#pragma aux p66 = "iret"
r66() { p66(); }
#pragma aux p68 = "iretd"
r68() { p68(); }
#pragma aux p70 = "lahf"
r70() { p70(); }
#pragma aux p72 = "lar si,di"
r72() { p72(); }
#pragma aux p74 = "lar ax,word ptr x"
r74() { p74(); }
#pragma aux p76 = "lar esi,edi"
r76() { p76(); }
#pragma aux p78 = "lar eax,dword ptr x"
r78() { p78(); }
#pragma aux p80 = "leave"
r80() { p80(); }
#pragma aux p82 = "wbinvd"
r82() { p82(); }
