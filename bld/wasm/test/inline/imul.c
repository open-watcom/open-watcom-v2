int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "imul al"
r01() { p01(); }
#pragma aux p02 = "imul byte ptr x"
r02() { p02(); }
#pragma aux p04 = "imul dx"
r04() { p04(); }
#pragma aux p06 = "imul word ptr x"
r06() { p06(); }
#pragma aux p08 = "imul esi"
r08() { p08(); }
#pragma aux p10 = "imul dword ptr x"
r10() { p10(); }
#pragma aux p12 = "imul si,bx"
r12() { p12(); }
#pragma aux p14 = "imul di,word ptr x"
r14() { p14(); }
#pragma aux p16 = "imul esi,edx"
r16() { p16(); }
#pragma aux p18 = "imul edx,dword ptr x"
r18() { p18(); }
#pragma aux p20 = "imul cx,dx,0fH"
r20() { p20(); }
#pragma aux p22 = "imul cx,dx,7fH"
r22() { p22(); }
#pragma aux p24 = "imul cx,dx,00ffH"
r24() { p24(); }
#pragma aux p26 = "imul cx,word ptr x,0fH"
r26() { p26(); }
#pragma aux p28 = "imul cx,word ptr x,7fH"
r28() { p28(); }
#pragma aux p30 = "imul cx,word ptr x,00ffH"
r30() { p30(); }
#pragma aux p32 = "imul bx,0fH"
r32() { p32(); }
#pragma aux p34 = "imul bx,7fH"
r34() { p34(); }
#pragma aux p36 = "imul bx,00ffH"
r36() { p36(); }
#pragma aux p38 = "imul ebx,0fH"
r38() { p38(); }
#pragma aux p40 = "imul ebx,7fH"
r40() { p40(); }
#pragma aux p42 = "imul ebx,000000ffH"
r42() { p42(); }
#pragma aux p44 = "imul di,dx,7fffH"
r44() { p44(); }
#pragma aux p46 = "imul di,dx,0ffffH"
r46() { p46(); }
#pragma aux p48 = "imul di,word ptr x,7fffH"
r48() { p48(); }
#pragma aux p50 = "imul di,word ptr x,0ffffH"
r50() { p50(); }
#pragma aux p52 = "imul edi,edx,7fffffffH"
r52() { p52(); }
#pragma aux p54 = "imul edi,edx,0ffffffffH"
r54() { p54(); }
#pragma aux p56 = "imul edi,dword ptr x,7fffffffH"
r56() { p56(); }
#pragma aux p58 = "imul edi,dword ptr x,0ffffffffH"
r58() { p58(); }
#pragma aux p60 = "imul si,7fffH"
r60() { p60(); }
#pragma aux p62 = "imul si,0ffffH"
r62() { p62(); }
#pragma aux p64 = "imul edx,7fffffffH"
r64() { p64(); }
#pragma aux p66 = "imul edx,0ffffffffH"
r66() { p66(); }
