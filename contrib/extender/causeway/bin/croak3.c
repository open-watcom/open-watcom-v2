// croak3.c
// C source example #3 on using UserDump API function
#include <stdio.h>
#include <i86.h>

char buffer[128];
char *oldstring="This is going away, so it can say anything it wants";
char *newstring="What I really wanted to say is this: Moo!";

void main(void)
{
    int i;
    union REGS regs;
    struct SREGS sregs;

    strcpy(buffer,oldstring);
    segread(&sregs);
    sregs.es=sregs.ds;
    regs.x.edi=(int)buffer;
    regs.w.cx=128;
    regs.h.bl='A';  // ASCII dump
    regs.h.bh=1;    // preset dump bytes
    regs.w.dx='-X'; // -X fill buffer
    regs.w.ax=0xff2f;
    int386x(0x31,&regs,&regs,&sregs);
    strcpy(buffer,newstring);
    croaker();
}

void croaker(void)
{
    int i,peekaboo=0x12345678;

    printf("%d ",*(int *)peekaboo);     // if this one doesn't GPF,
    peekaboo=0x87654321;                // then
    printf("%d\n",*(int *)peekaboo);    // this one will
}
