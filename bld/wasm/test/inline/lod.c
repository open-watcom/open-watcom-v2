int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "lodsb"
r1() { p1(); }
#pragma aux p2 = "lodsw"
r2() { p2(); }
#pragma aux p4 = "lodsd"
r4() { p4(); }
