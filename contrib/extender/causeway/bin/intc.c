// INTC.C
// Demonstrates calling interrupt not natively supported by CauseWay
//  that uses segment:offset pointers.
// C language version, assembly language version is INTASM.ASM
//
#include <stdio.h>
#include <string.h>
#include <i86.h>

typedef unsigned short int SELECTOR;
typedef long int REGISTER32;
typedef short int REGISTER16;
typedef unsigned short int RMSEGMENT;

struct RMRL		// Used by IntXX
{
   REGISTER32  EDI;
   REGISTER32  ESI;
   REGISTER32  EBP;
   long        Reserved;
   REGISTER32  EBX;
   REGISTER32  EDX;
   REGISTER32  ECX;
   REGISTER32  EAX;
   REGISTER16  Flags;
   SELECTOR    ES;
   SELECTOR    DS;
   SELECTOR    FS;
   SELECTOR    GS;
   REGISTER16  IP;
   SELECTOR    CS;
   REGISTER16  SP;
   SELECTOR    SS;
};

char FileName[128]="INTC.EXE";

unsigned short int ASCIIZSegment;
unsigned short int ASCIIZSelector;
unsigned short int CanonicalSegment;
unsigned short int CanonicalSelector;

union REGS regs;
struct SREGS sregs;
struct RMRL RealRegs;

// optionally pass a different file name than current on command line
int	main(int argc, char *argv[])
{
	if(argc>1){
		strcpy(FileName,argv[1]);
	}
	regs.w.bx=8;
	regs.w.ax=0xff21;
	int386(0x31,&regs,&regs);	// get low dos memory
	if(regs.x.cflag & 1){	// carry set
		bail();
	}
	ASCIIZSelector=regs.w.dx;
	ASCIIZSegment=regs.w.ax;

	regs.w.bx=8;
	regs.w.ax=0xff21;
	int386(0x31,&regs,&regs);	// get low dos memory
	if(regs.x.cflag & 1){	// carry set
		bail();
	}
	CanonicalSelector=regs.w.dx;
	CanonicalSegment=regs.w.ax;

	movedata(FP_SEG(FileName),FP_OFF(FileName),ASCIIZSelector,0,strlen(FileName)+1);

	RealRegs.ESI=0;
	RealRegs.EDI=0;
	RealRegs.DS=ASCIIZSegment;
	RealRegs.ES=CanonicalSegment;
	RealRegs.EAX=0x6000;
	regs.w.ax=0xff01;
	regs.h.bl=0x21;
	regs.x.edi=(long)&RealRegs;
    int386(0x31,&regs,&regs);
	if(RealRegs.Flags & 1){	// carry set
		bail();
	}

	printf("\n%Fs",MK_FP(CanonicalSelector,0));

	return(0);
}

void bail(void)
{
	printf("\nError occurred!");
	exit(1);
}
