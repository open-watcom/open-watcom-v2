int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "mov dl,bl"
r01() { p01(); }
#pragma aux p02 = "mov byte ptr x,bl"
r02() { p02(); }
#pragma aux p04 = "mov dx,bx"
r04() { p04(); }
#pragma aux p06 = "mov word ptr x,bx"
r06() { p06(); }
#pragma aux p08 = "mov edx,ebx"
r08() { p08(); }
#pragma aux p10 = "mov dword ptr x,ebx"
r10() { p10(); }
#pragma aux p12 = "mov dl,byte ptr x"
r12() { p12(); }
#pragma aux p14 = "mov dx,word ptr x"
r14() { p14(); }
#pragma aux p16 = "mov edx,dword ptr x"
r16() { p16(); }
#pragma aux p18 = "mov bx,ds"
r18() { p18(); }
#pragma aux p20 = "mov word ptr x,ds"
r20() { p20(); }
#pragma aux p22 = "mov ds,bx"
r22() { p22(); }
#pragma aux p24 = "mov ds,word ptr x"
r24() { p24(); }
#pragma aux p26 = "mov al,byte ptr x"
r26() { p26(); }
#pragma aux p28 = "mov ax,word ptr x"
r28() { p28(); }
#pragma aux p30 = "mov eax,dword ptr x"
r30() { p30(); }
#pragma aux p32 = "mov byte ptr x,al"
r32() { p32(); }
#pragma aux p34 = "mov word ptr x,ax"
r34() { p34(); }
#pragma aux p36 = "mov dword ptr x,eax"
r36() { p36(); }
#pragma aux p38 = "mov byte ptr x,0fH"
r38() { p38(); }
#pragma aux p40 = "mov byte ptr x,7fH"
r40() { p40(); }
#pragma aux p42 = "mov byte ptr x,0ffH"
r42() { p42(); }
#pragma aux p44 = "mov word ptr x,000fH"
r44() { p44(); }
#pragma aux p46 = "mov word ptr x,007fH"
r46() { p46(); }
#pragma aux p48 = "mov word ptr x,00ffH"
r48() { p48(); }
#pragma aux p50 = "mov word ptr x,7fffH"
r50() { p50(); }
#pragma aux p52 = "mov word ptr x,0ffffH"
r52() { p52(); }
#pragma aux p54 = "mov dword ptr x,0000000fH"
r54() { p54(); }
#pragma aux p56 = "mov dword ptr x,0000007fH"
r56() { p56(); }
#pragma aux p58 = "mov dword ptr x,000000ffH"
r58() { p58(); }
#pragma aux p60 = "mov dword ptr x,00007fffH"
r60() { p60(); }
#pragma aux p62 = "mov dword ptr x,0000ffffH"
r62() { p62(); }
#pragma aux p64 = "mov dword ptr x,7fffffffH"
r64() { p64(); }
#pragma aux p66 = "mov dword ptr x,0ffffffffH"
r66() { p66(); }
#pragma aux p68 = "mov al,0fH"
r68() { p68(); }
#pragma aux p70 = "mov al,7fH"
r70() { p70(); }
#pragma aux p72 = "mov al,0ffH"
r72() { p72(); }
#pragma aux p074 = "mov ax,000fH"
r074() { p074(); }
#pragma aux p076 = "mov ax,007fH"
r076() { p076(); }
#pragma aux p078 = "mov ax,00ffH"
r078() { p078(); }
#pragma aux p080 = "mov ax,7fffH"
r080() { p080(); }
#pragma aux p082 = "mov ax,0ffffH"
r082() { p082(); }
#pragma aux p084 = "mov eax,0000000fH"
r084() { p084(); }
#pragma aux p086 = "mov eax,0000007fH"
r086() { p086(); }
#pragma aux p088 = "mov eax,000000ffH"
r088() { p088(); }
#pragma aux p090 = "mov eax,00007fffH"
r090() { p090(); }
#pragma aux p092 = "mov eax,0000ffffH"
r092() { p092(); }
#pragma aux p094 = "mov eax,7fffffffH"
r094() { p094(); }
#pragma aux p096 = "mov eax,0ffffffffH"
r096() { p096(); }
#pragma aux p098 = "mov dl,0fH"
r098() { p098(); }
#pragma aux p100 = "mov dl,7fH"
r100() { p100(); }
#pragma aux p102 = "mov dl,0ffH"
r102() { p102(); }
#pragma aux p104 = "mov dx,000fH"
r104() { p104(); }
#pragma aux p106 = "mov dx,007fH"
r106() { p106(); }
#pragma aux p108 = "mov dx,00ffH"
r108() { p108(); }
#pragma aux p110 = "mov dx,7fffH"
r110() { p110(); }
#pragma aux p112 = "mov dx,0ffffH"
r112() { p112(); }
#pragma aux p114 = "mov edx,0000000fH"
r114() { p114(); }
#pragma aux p116 = "mov edx,0000007fH"
r116() { p116(); }
#pragma aux p118 = "mov edx,000000ffH"
r118() { p118(); }
#pragma aux p120 = "mov edx,00007fffH"
r120() { p120(); }
#pragma aux p122 = "mov edx,0000ffffH"
r122() { p122(); }
#pragma aux p124 = "mov edx,7fffffffH"
r124() { p124(); }
#pragma aux p126 = "mov edx,0ffffffffH"
r126() { p126(); }
