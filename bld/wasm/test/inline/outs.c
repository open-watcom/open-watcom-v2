int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "outsb"
r1() { p1(); }
#pragma aux p2 = "outsw"
r2() { p2(); }
#pragma aux p4 = "outsd"
r4() { p4(); }
