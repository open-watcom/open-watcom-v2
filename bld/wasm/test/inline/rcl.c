int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "rcl dl,1"
r01() { p01(); }
#pragma aux p02 = "rcl byte ptr [ebx],1"
r02() { p02(); }
#pragma aux p04 = "rcl dl,cl"
r04() { p04(); }
#pragma aux p06 = "rcl byte ptr [ebx],cl"
r06() { p06(); }
#pragma aux p08 = "rcl dl,07H"
r08() { p08(); }
#pragma aux p10 = "rcl byte ptr [ebx],07H"
r10() { p10(); }
#pragma aux p12 = "rcl bx,1"
r12() { p12(); }
#pragma aux p14 = "rcl word ptr [ebx],1"
r14() { p14(); }
#pragma aux p16 = "rcl bx,cl"
r16() { p16(); }
#pragma aux p18 = "rcl word ptr [ebx],cl"
r18() { p18(); }
#pragma aux p20 = "rcl bx,07H"
r20() { p20(); }
#pragma aux p22 = "rcl word ptr [ebx],07H"
r22() { p22(); }
#pragma aux p24 = "rcl esi,1"
r24() { p24(); }
#pragma aux p26 = "rcl dword ptr [ebx],1"
r26() { p26(); }
#pragma aux p28 = "rcl esi,cl"
r28() { p28(); }
#pragma aux p30 = "rcl dword ptr [ebx],cl"
r30() { p30(); }
#pragma aux p32 = "rcl esi,07H"
r32() { p32(); }
#pragma aux p34 = "rcl dword ptr [ebx],07H"
r34() { p34(); }
#pragma aux p36 = "rcr dl,1"
r36() { p36(); }
#pragma aux p38 = "rcr byte ptr [ebx],1"
r38() { p38(); }
#pragma aux p40 = "rcr dl,cl"
r40() { p40(); }
#pragma aux p42 = "rcr byte ptr [ebx],cl"
r42() { p42(); }
#pragma aux p44 = "rcr dl,07H"
r44() { p44(); }
#pragma aux p46 = "rcr byte ptr [ebx],07H"
r46() { p46(); }
#pragma aux p48 = "rcr bx,1"
r48() { p48(); }
#pragma aux p50 = "rcr word ptr [ebx],1"
r50() { p50(); }
#pragma aux p52 = "rcr bx,cl"
r52() { p52(); }
#pragma aux p54 = "rcr word ptr [ebx],cl"
r54() { p54(); }
#pragma aux p56 = "rcr bx,07H"
r56() { p56(); }
#pragma aux p058 = "rcr word ptr [ebx],07H"
r058() { p058(); }
#pragma aux p060 = "rcr esi,1"
r060() { p060(); }
#pragma aux p062 = "rcr dword ptr [ebx],1"
r062() { p062(); }
#pragma aux p064 = "rcr esi,cl"
r064() { p064(); }
#pragma aux p066 = "rcr dword ptr [ebx],cl"
r066() { p066(); }
#pragma aux p068 = "rcr esi,07H"
r068() { p068(); }
#pragma aux p070 = "rcr dword ptr [ebx],07H"
r070() { p070(); }
#pragma aux p072 = "rol dl,1"
r072() { p072(); }
#pragma aux p074 = "rol byte ptr [ebx],1"
r074() { p074(); }
#pragma aux p076 = "rol dl,cl"
r076() { p076(); }
#pragma aux p078 = "rol byte ptr [ebx],cl"
r078() { p078(); }
#pragma aux p080 = "rol dl,07H"
r080() { p080(); }
#pragma aux p082 = "rol byte ptr [ebx],07H"
r082() { p082(); }
#pragma aux p084 = "rol bx,1"
r084() { p084(); }
#pragma aux p086 = "rol word ptr [ebx],1"
r086() { p086(); }
#pragma aux p088 = "rol bx,cl"
r088() { p088(); }
#pragma aux p090 = "rol word ptr [ebx],cl"
r090() { p090(); }
#pragma aux p092 = "rol bx,07H"
r092() { p092(); }
#pragma aux p094 = "rol word ptr [ebx],07H"
r094() { p094(); }
#pragma aux p096 = "rol esi,1"
r096() { p096(); }
#pragma aux p098 = "rol dword ptr [ebx],1"
r098() { p098(); }
#pragma aux p100 = "rol esi,cl"
r100() { p100(); }
#pragma aux p102 = "rol dword ptr [ebx],cl"
r102() { p102(); }
#pragma aux p104 = "rol esi,07H"
r104() { p104(); }
#pragma aux p106 = "rol dword ptr [ebx],07H"
r106() { p106(); }
#pragma aux p108 = "ror dl,1"
r108() { p108(); }
#pragma aux p110 = "ror byte ptr [ebx],1"
r110() { p110(); }
#pragma aux p112 = "ror dl,cl"
r112() { p112(); }
#pragma aux p114 = "ror byte ptr [ebx],cl"
r114() { p114(); }
#pragma aux p116 = "ror dl,07H"
r116() { p116(); }
#pragma aux p118 = "ror byte ptr [ebx],07H"
r118() { p118(); }
#pragma aux p120 = "ror bx,1"
r120() { p120(); }
#pragma aux p122 = "ror word ptr [ebx],1"
r122() { p122(); }
#pragma aux p124 = "ror bx,cl"
r124() { p124(); }
#pragma aux p126 = "ror word ptr [ebx],cl"
r126() { p126(); }
#pragma aux p128 = "ror bx,07H"
r128() { p128(); }
#pragma aux p130 = "ror word ptr [ebx],07H"
r130() { p130(); }
#pragma aux p132 = "ror esi,1"
r132() { p132(); }
#pragma aux p134 = "ror dword ptr [ebx],1"
r134() { p134(); }
#pragma aux p136 = "ror esi,cl"
r136() { p136(); }
#pragma aux p138 = "ror dword ptr [ebx],cl"
r138() { p138(); }
#pragma aux p140 = "ror esi,07H"
r140() { p140(); }
#pragma aux p142 = "ror dword ptr [ebx],07H"
r142() { p142(); }
