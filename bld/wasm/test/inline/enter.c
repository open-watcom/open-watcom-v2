int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "enter 07ffH,00H"
r1() { p1(); }
#pragma aux p2 = "enter 0ffffH,01H"
r2() { p2(); }
#pragma aux p4 = "enter 000fH,0fH"
r4() { p4(); }
#pragma aux p6 = "enter 000fH,7fH"
r6() { p6(); }
#pragma aux p8 = "enter 000fH,0ffH"
r8() { p8(); }
