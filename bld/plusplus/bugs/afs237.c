// -d2 crashes in CG (reported to Brad)
// WatCom 10.5  report 000  26.01.1996
// Compiler crash with switch -d2
extern  __segment Seg;
struct  S { unsigned I,J; };
#define P (Seg:>(struct S __based(void) *)0)
long    x6(long i) { i*=P->J-P->I; return i; }

// WatCom 10.5  report 001  26.01.1996
// No reference to 'Seg'
#define Q ((struct S __based(Seg) *)0)
int    x11(int i) { i=Q->J; return i; }

// WatCom 10.5  report 002  26.01.1996
// Unnecessary warning W112
__segment pSeg;
struct  S1 { int I; } __based(pSeg) *P1;
struct  S2 { int J; struct S1 S[]; } __based(pSeg) *P2;
void    x20(void) { P1=P2->S; }
