int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "pop word ptr x"
r01() { p01(); }
#pragma aux p02 = "pop dword ptr x"
r02() { p02(); }
#pragma aux p04 = "pop di"
r04() { p04(); }
#pragma aux p06 = "pop eax"
r06() { p06(); }
#pragma aux p08 = "pop ds"
r08() { p08(); }
#pragma aux p10 = "pop es"
r10() { p10(); }
#pragma aux p12 = "pop ss"
r12() { p12(); }
#pragma aux p14 = "pop fs"
r14() { p14(); }
#pragma aux p16 = "pop gs"
r16() { p16(); }
#pragma aux p18 = "popa"
r18() { p18(); }
#pragma aux p20 = "popad"
r20() { p20(); }
#pragma aux p22 = "popf"
r22() { p22(); }
#pragma aux p24 = "popfd"
r24() { p24(); }
#pragma aux p26 = "push word ptr x"
r26() { p26(); }
#pragma aux p28 = "push dword ptr x"
r28() { p28(); }
#pragma aux p30 = "push di"
r30() { p30(); }
#pragma aux p32 = "push 0fH"
r32() { p32(); }
#pragma aux p34 = "push 7fH"
r34() { p34(); }
#pragma aux p36 = "push 0ffH"
r36() { p36(); }
#pragma aux p38 = "push 7fffH"
r38() { p38(); }
#pragma aux p40 = "push 0ffffH"
r40() { p40(); }
#pragma aux p42 = "push 7fffffffH"
r42() { p42(); }
#pragma aux p44 = "push 0ffffffffH"
r44() { p44(); }
#pragma aux p46 = "push eax"
r46() { p46(); }
#pragma aux p48 = "push cs"
r48() { p48(); }
#pragma aux p50 = "push ds"
r50() { p50(); }
#pragma aux p52 = "push es"
r52() { p52(); }
#pragma aux p54 = "push ss"
r54() { p54(); }
#pragma aux p56 = "push fs"
r56() { p56(); }
#pragma aux p58 = "push gs"
r58() { p58(); }
#pragma aux p60 = "pusha"
r60() { p60(); }
#pragma aux p62 = "pushad"
r62() { p62(); }
#pragma aux p64 = "pushf"
r64() { p64(); }
#pragma aux p66 = "pushfd"
r66() { p66(); }
#pragma aux p68 = "pusha"
r68() { p68(); }
#pragma aux p70 = "pushad"
r70() { p70(); }
#pragma aux p72 = "pushf"
r72() { p72(); }
#pragma aux p74 = "pushfd"
r74() { p74(); }
