int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "mul dl"
r1() { p1(); }
#pragma aux p2 = "mul byte ptr x"
r2() { p2(); }
#pragma aux p4 = "mul dx"
r4() { p4(); }
#pragma aux p6 = "mul word ptr x"
r6() { p6(); }
#pragma aux p8 = "mul edx"
r8() { p8(); }
#pragma aux p10 = "mul dword ptr x"
r10() { p10(); }
