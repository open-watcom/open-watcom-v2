/*
    SIMULATE.C - Shows how to issue a real-mode interrupt
    from protected mode using DPMI call 300h.  Any buffers
    to be passed to DOS must be allocated in DOS memory
    This can be done with DPMI call 100h.  This program
    will call DOS int 21, function 39h, "Create
    Directory".

    Compile & Link: wcl386 /l=dos4g simulate
*/
#include <i86.h>
#include <dos.h>
#include <stdio.h>
#include <string.h>

static struct rminfo {
    long EDI;
    long ESI;
    long EBP;
    long reserved_by_system;
    long EBX;
    long EDX;
    long ECX;
    long EAX;
    short flags;
    short ES,DS,FS,GS,IP,CS,SP,SS;
} RMI;

void main()
{
    union REGS regs;
    struct SREGS sregs;
    int interrupt_no=0x31;
    short selector;
    short segment;
    char far *str;

    /* DPMI call 100h allocates DOS memory */
    memset(&sregs,0,sizeof(sregs));
    regs.w.ax=0x0100;
    regs.w.bx=0x0001;
    int386x( interrupt_no, &regs, &regs, &sregs);
    segment=regs.w.ax;
    selector=regs.w.dx;

    /* Move string to DOS real-mode memory */
    str=MK_FP(selector,0);
    _fstrcpy( str, "myjunk" );

    /* Set up real-mode call structure */
    memset(&RMI,0,sizeof(RMI));
    RMI.EAX=0x00003900; /* call service 39h ah=0x39  */
    RMI.DS=segment;     /* put DOS seg:off into DS:DX*/
    RMI.EDX=0;          /* DOS ignores EDX high word */

    /* Use DMPI call 300h to issue the DOS interrupt */
    regs.w.ax = 0x0300;
    regs.h.bl = 0x21;
    regs.h.bh = 0;
    regs.w.cx = 0;
    sregs.es = FP_SEG(&RMI);
    regs.x.edi = FP_OFF(&RMI);
    int386x( interrupt_no, &regs, &regs, &sregs );
}
