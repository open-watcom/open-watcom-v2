int     x;
#pragma aux pinit0=".486p"
#pragma aux p1 = "push  0fH"
r1() { p1(); }
#pragma aux p2 = "push  7fH"
r2() { p2(); }
#pragma aux p4 = "push  0ffH"
r4() { p4(); }
#pragma aux p6 = "push  7fffH"
r6() { p6(); }
#pragma aux p08 = "push 0ffffH"
r08() { p08(); }
#pragma aux p10 = "push 7fffffffH"
r10() { p10(); }
#pragma aux p12 = "push 0ffffffffH"
r12() { p12(); }
