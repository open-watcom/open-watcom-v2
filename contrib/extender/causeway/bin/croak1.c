// croak1.c
// C source example #1 on using UserDump API function
#include <stdio.h>
#include <i86.h>

void main(void)
{
    int i;
    char buffer[128];
    union REGS regs;
    struct SREGS sregs;

    segread(&sregs);
    sregs.es=sregs.ds;
    regs.x.edi=(int)buffer;
    regs.w.cx=128;
    regs.h.bl=0;    // non-ASCII dump
    regs.w.ax=0xff2f;
    int386x(0x31,&regs,&regs,&sregs);
    for(i=0;i<128;i++){ // fill dump with discernable pattern
        buffer[i]=i;
    }
    croaker();
}

void croaker(void)
{
    int i,peekaboo=0x12345678;

    printf("%d ",*(int *)peekaboo);     // if this one doesn't GPF,
    peekaboo=0x87654321;                // then
    printf("%d\n",*(int *)peekaboo);    // this one will
}
