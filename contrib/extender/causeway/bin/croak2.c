// croak2.c
// C source example #2 on using UserDump API function
#include <stdio.h>
#include <i86.h>

char buffer[128]="Help me, I'm trapped in CW.ERR";

void main(void)
{
    int i;
    union REGS regs;
    struct SREGS sregs;

    segread(&sregs);
    sregs.es=sregs.ds;
    regs.x.edi=(int)buffer;
    regs.w.cx=128;
    regs.h.bl='A';  // ASCII dump
    regs.h.bh=0;    // no presetting dump bytes
    regs.w.ax=0xff2f;
    int386x(0x31,&regs,&regs,&sregs);
    croaker();
}

void croaker(void)
{
    int i,peekaboo=0x12345678;

    printf("%d ",*(int *)peekaboo);     // if this one doesn't GPF,
    peekaboo=0x87654321;                // then
    printf("%d\n",*(int *)peekaboo);    // this one will
}
