int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "lea si,word ptr x"
r1() { p1(); }
#pragma aux p2 = "lea eax,dword ptr x"
r2() { p2(); }
