int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "movsb"
r1() { p1(); }
#pragma aux p2 = "repe movsw"
r2() { p2(); }
#pragma aux p4 = "repne movsd"
r4() { p4(); }
