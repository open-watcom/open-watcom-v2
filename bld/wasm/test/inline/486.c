int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "bswap eax"
r1() { p1(); }
#pragma aux p2 = "bswap esi"
r2() { p2(); }
