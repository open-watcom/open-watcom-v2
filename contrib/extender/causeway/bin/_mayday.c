// _MAYDAY.C, support module for TESTTERM.C
// 
// This module MUST be compiled with stack-checking turned off (/s option)
//
#include <stdio.h>
#include <stdlib.h>

struct InfoListStruct {
	unsigned int ebp;
	unsigned int edi;
	unsigned int esi;
	unsigned int edx;
	unsigned int ecx;
	unsigned int ebx;
	unsigned int eax;
	short unsigned int gs;
	short unsigned int fs;
	short unsigned int es;
	short unsigned int ds;
	unsigned int eip;
	short unsigned int cs;
	short unsigned int reserved1;
	unsigned int eflags;
	unsigned int esp;
	short unsigned int ss;
	short unsigned int reserved2;
	short unsigned int tr;
	unsigned int cr0;
	unsigned int cr1;
	unsigned int cr2;
	unsigned int cr3;
	unsigned int csAddr;
	unsigned int dsAddr;
	unsigned int esAddr;
	unsigned int fsAddr;
	unsigned int gsAddr;
	unsigned int ssAddr;
	short unsigned int Exception;
	unsigned int Code;
};

struct InfoListStruct InfoList;

extern int _STACKTOP;	// Watcom internal variable, top of Watcom stack

// Various contortions necessary to set CauseWay internal stack to
//  Watcom DGROUP-based stack and to keep the unknown number of stack
//  parameters relative to register EBP unchanged.
extern void _ss_to_dgroup(void);
#pragma aux _ss_to_dgroup = \
	"sub ebp,esp"		\
	"xor eax,eax"		\
	"mov ax,ss"			\
	"mov es,ax"			\
	"mov ebx,esp"		\
	"mov edx,_STACKTOP"	\
	"mov cx,ds"			\
	"mov ss,cx"			\
	"mov esp,edx"		\
	"mov edx,esi"		\
	"mov edi,esi"		\
	"sub edi,ebx"		\
	"sub edx,2"			\
	"looper: mov cx,es:[edx]"	\
	"sub esp,2"			\
	"mov ss:[esp],cx"	\
	"sub edx,2"			\
	"sub edi,2"			\
	"jg	looper"			\
	"add ebp,esp"		\
	"push eax"			\
	"push esi"			\
	modify [eax ebx ecx edx edi];

extern void _restore_ss_and_go(void);
#pragma aux _restore_ss_and_go =	\
	"pop esi"			\
	"pop eax"			\
	"sub esi,8"			\
	"mov ss,ax"			\
	"mov esp,esi"		\
	"retf";

// Termination routine MUST NOT have stack-checking enabled.
// If virtual memory is in use, this routine and all code and data called
//  or used by the routine (including Watcom runtime library functions)
//  must be locked in memory.
void far __loadds Mayday(void)
{
	_ss_to_dgroup();	// lets us call Watcom runtime library functions
	printf("\nAyyahhh, the program is crashing in flames!\n");
	printf("(Incidentally, the exception was %#X and esi was %#X\n)",InfoList.Exception,InfoList.esi);
	_restore_ss_and_go();	// restore stack contorted for Watcom's use
}
