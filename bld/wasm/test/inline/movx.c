int     x;
#pragma aux pinit0=".486p"
#pragma aux p01 = "movsx ax,byte ptr x"
r01() { p01(); }
#pragma aux p02 = "movsx eax,byte ptr x"
r02() { p02(); }
#pragma aux p04 = "movsx eax,word ptr x"
r04() { p04(); }
#pragma aux p06 = "movsx ax,dl"
r06() { p06(); }
#pragma aux p08 = "movsx eax,bh"
r08() { p08(); }
#pragma aux p10 = "movsx eax,si"
r10() { p10(); }
#pragma aux p12 = "movzx ax,byte ptr x"
r12() { p12(); }
#pragma aux p14 = "movzx eax,byte ptr x"
r14() { p14(); }
#pragma aux p16 = "movzx eax,word ptr x"
r16() { p16(); }
#pragma aux p18 = "movzx ax,dl"
r18() { p18(); }
#pragma aux p20 = "movzx eax,bh"
r20() { p20(); }
#pragma aux p22 = "movzx eax,si"
r22() { p22(); }
