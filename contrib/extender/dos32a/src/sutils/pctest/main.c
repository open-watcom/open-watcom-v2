/*
 * Copyright (C) 1996-2002 Supernar Systems, Ltd. All rights reserved.
 *
 * Redistribution  and  use  in source and  binary  forms, with or without
 * modification,  are permitted provided that the following conditions are
 * met:
 *
 * 1.  Redistributions  of  source code  must  retain  the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2.  Redistributions  in binary form  must reproduce the above copyright
 * notice,  this  list of conditions and  the  following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. The end-user documentation included with the redistribution, if any,
 * must include the following acknowledgment:
 *
 * "This product uses DOS/32 Advanced DOS Extender technology."
 *
 * Alternately,  this acknowledgment may appear in the software itself, if
 * and wherever such third-party acknowledgments normally appear.
 *
 * 4.  Products derived from this software  may not be called "DOS/32A" or
 * "DOS/32 Advanced".
 *
 * THIS  SOFTWARE AND DOCUMENTATION IS PROVIDED  "AS IS" AND ANY EXPRESSED
 * OR  IMPLIED  WARRANTIES,  INCLUDING, BUT  NOT  LIMITED  TO, THE IMPLIED
 * WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN  NO  EVENT SHALL THE  AUTHORS  OR  COPYRIGHT HOLDERS BE
 * LIABLE  FOR  ANY DIRECT, INDIRECT,  INCIDENTAL,  SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE  GOODS  OR  SERVICES;  LOSS OF  USE,  DATA,  OR  PROFITS; OR
 * BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE)  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "main.h"

	char	*version=	"9.00";

	char	*_cpuvendor=	NULL;		// vendor id string
	byte	_cputype=	-1;
	byte	_fputype=	-1;
	byte	_fpuinfo=	0;		// info bits from CR0
	byte	_systype=	-1;
	byte	_extendertype=	-1;
	byte	_dpmiflags=	-1;
	byte	_modetype=	0;		// Real or V86 mode
	byte	_codecpl=	-1;
	byte	_codeiopl=	-1;
	byte	real_intel=	-1;

	dword	_himemsize=	-1;
	dword	_lomemsize=	-1;
	dword	_totalmemsize=	-1;
	float	_rawcpuspeed=	-1;

	float	cpu_speed=	-1;	// measured CPU speed in MHz
	float	fpu_speed=	-1;	// measured FPU speed in Clocks
	float	risc_speed=	-1;	// measured RISC speed in Clocks
	float	priv_speed=	-1;	// measured privelege instr. speed
	float	intPR_speed=	-1;
	float	intRP_speed=	-1;
	float	irqPR_speed=	-1;
	float	irqRP_speed=	-1;

	float	lmem_movsw=	-1;
	float	lmem_movsd=	-1;
	float	lmem_stosw=	-1;
	float	lmem_stosd=	-1;
	float	hmem_movsw=	-1;
	float	hmem_movsd=	-1;
	float	hmem_stosw=	-1;
	float	hmem_stosd=	-1;
	float	vmem_movsw=	-1;
	float	vmem_movsd=	-1;
	float	vmem_stosw=	-1;
	float	vmem_stosd=	-1;



/****************************************************************************/
void main () {
//	Debug_Init();
//	Debug();

	_cputype=	-1;
	_fputype=	-1;
	_fpuinfo=	0;
	_systype=	-1;
	_extendertype=	-1;
	_dpmiflags=	-1;
	_modetype=	0;		// Real or V86 mode

	cpu_speed=	-1;
	fpu_speed=	-1;
	risc_speed=	-1;
	intPR_speed=	-1;
	intRP_speed=	-1;
	irqPR_speed=	-1;
	irqRP_speed=	-1;

	lmem_movsw=	-1;
	lmem_movsd=	-1;
	lmem_stosw=	-1;
	lmem_stosd=	-1;
	hmem_movsw=	-1;
	hmem_movsd=	-1;
	hmem_stosw=	-1;
	hmem_stosd=	-1;
	vmem_movsw=	-1;
	vmem_movsd=	-1;
	vmem_stosw=	-1;
	vmem_stosd=	-1;


	printf("\n");
	printf("PCTEST -- Protected Mode Diagnostic Utility  Version %s",version);
	printf("\n");
	printf("Copyright (C) Supernar Systems, Ltd. 1996-2002");
	printf("\n");
	printf("---------------------------------------------------------");
	printf("\n");
	get_system_information();
	measure_system_speed();

	show_copyright();
	show_system_information();


	exit(0);
}





/****************************************************************************/
void show_copyright(void)
{
	setvideomode(3);
	printf("     Protected Mode 32-bit and Extended Memory Diagnostic Program -- v%s \n",version);
	printf("                Copyright (C) Supernar Systems, Ltd. 1996-2002\n");
}

void get_system_information(void)
{
	cprintf("Getting system information: ");
	cprintf("CPU...");
	_cputype=get_cpu_type();_cpuvendor=get_cpu_id();
	cprintf("\b\b\b\b\b\bFPU...");
	_fputype=get_fpu_type();_fpuinfo=get_fpu_info();
	cprintf("\b\b\b\b\b\bSYS...");
	_systype=get_sys_type();_codecpl=get_cpl();_codeiopl=get_iopl();
	_extendertype=get_extender_type();
	_dpmiflags=get_dpmi_flags();
	if((_dpmiflags&0x02)==0) _modetype=1; else _modetype=0;
	cprintf("\b\b\b\b\b\bMEM...");
	_totalmemsize=get_total_size();
	_lomemsize=get_lomem_size();
	_himemsize=get_himem_size();
	printf("\b\b\b\b\b\bDone.  \n");
}

void measure_system_speed(void)
{
	cprintf("Measuring system performance: ");
	cprintf("CPU..."); measure_cpu_speed();
	cprintf("\b\b\b\b\b\bFPU..."); measure_fpu_speed();
	cprintf("\b\b\b\b\b\bPRIV..."); measure_priv_speed();
	cprintf("\b\b\b\b\b\b\bRISC..."); measure_risc_speed();
	cprintf("\b\b\b\b\b\b\bINT... \b"); measure_int_speed();
	cprintf("\b\b\b\b\b\bIRQ..."); measure_irq_speed();
	cprintf("\b\b\b\b\b\bLMEM..."); measure_lmem_speed();
	cprintf("\b\b\b\b\b\b\bHMEM..."); measure_hmem_speed();
	cprintf("\b\b\b\b\b\b\bVMEM..."); measure_vmem_speed();
	printf ("\b\b\b\b\b\b\bDone.   \n");
}






/****************************************************************************/

void show_system_information(void)
{

	float f, f1,f2,f3,f4,f5;
	char *systype[]={ "None","XMS","VCPI","DPMI" };
	char *cputype_i[]={ "80386","80486","Pentium","Pentium Pro" };
	char *cputype_o[]={ "80386","80486","80586","80686" };
	char *fputype_i[]={ "None","8087","80287","80387","80487","Pentium","Pentium Pro" };
	char *fputype_o[]={ "None","8087","80287","80387","80487","80587","80687" };
	char *dosextender[]={ 	"DOS/32A",
				"PMODE/W",
				"PMODE/W v",
				"DOS/4G",
				"DOS/4GW",
				"CauseWay",
				"Enhanced Windows",
				"E.O.S." };
	char *modetype[]={ "Real","V86","Protected" };


	printf("\n");
	printf("DOS Version:  %d.%02d\n",_osmajor,_osminor);


	printf("DOS Extender: ");
	if(_extendertype==0xFF)
		printf("Unknown");
	else
		printf("%s",dosextender[_extendertype]);
	if(_extendertype==2)
		printf("%01d.%02d", (get_pmodew_ver()&0x0FF00)>>8,get_pmodew_ver()&0xFF);
	if(_systype!=0xFF)
		printf(" (%s)",systype[_systype]);
	printf("\n");


	gotoxy(39,3);
	printf("CPU type: ");
	real_intel=0;
	if(strcmp(_cpuvendor,"GenuineIntel")==0)
	{
		printf("Intel ");
		real_intel = 1;
	}
	else if(strcmp(_cpuvendor,"CyrixInstead")==0)
		printf("Cyrix ");
	else if(strcmp(_cpuvendor,"AuthenticAMD")==0)
		printf("AMD ");
	else if(strcmp(_cpuvendor,"UMC UMC UMC ")==0)
		printf("UMC ");
	if(real_intel==0)
		printf("%s %2.1fMHz",cputype_o[_cputype-3],cpu_speed);
	else
		printf("%s %2.1fMHz",cputype_i[_cputype-3],cpu_speed);
//	printf(" %1.1f",_rawcpuspeed);
	printf("\n");


	gotoxy(39,4);
	printf("FPU type: ");
	if((_fpuinfo&0x02)==0x02)
	{
		printf("Emulated");
	}
	else if((_fpuinfo&0x01)==0x01 && _cputype>3)
	{
		printf("Built-in");
	}
	else
	{
		printf("Unknown");
	}
	printf("\n");


	printf("DOS Memory: %dKB (%d bytes)\n",_lomemsize/1024,_lomemsize);
	gotoxy(39,5);
	printf("High Memory: %dKB (%d bytes)\n",_himemsize/1024,_himemsize);
	printf("\n");


	printf(" MemMove (MB/s)  | 16-bit  | 32-bit      MemStore (MB/s) | 16-bit  | 32-bit\n");
	printf("-----------------+---------+---------   -----------------+---------+---------\n");
	printf(" DOS memory      |         |             DOS memory      |         |\n");
	printf(" High memory     |         |             High memory     |         |\n");
	printf(" Video memory    |         |             Video memory    |         |\n");

	gotoxy(19,9);
	if(lmem_movsw > 0)	printf("%4.1f\n", lmem_movsw/1024.0);
	else			printf("N/A\n");

	gotoxy(29,9);
	if(lmem_movsd > 0)	printf("%4.1f\n", lmem_movsd/1024.0);
	else			printf("N/A\n");

	gotoxy(59,9);
	if(lmem_stosw > 0)	printf("%4.1f\n", lmem_stosw/1024.0);
	else			printf("N/A\n");

	gotoxy(69,9);
	if(lmem_stosd > 0)	printf("%4.1f\n", lmem_stosd/1024.0);
	else			printf("N/A\n");


	gotoxy(19,10);
	if(hmem_movsw > 0)	printf("%4.1f\n", hmem_movsw/1024.0);
	else			printf("N/A\n");

	gotoxy(29,10);
	if(hmem_movsd > 0)	printf("%4.1f\n", hmem_movsd/1024.0);
	else			printf("N/A\n");

	gotoxy(59,10);
	if(hmem_stosw > 0)	printf("%4.1f\n", hmem_stosw/1024.0);
	else			printf("N/A\n");

	gotoxy(69,10);
	if(hmem_stosd > 0)	printf("%4.1f\n", hmem_stosd/1024.0);
	else			printf("N/A\n");


	gotoxy(19,11);
	if(vmem_movsw > 0)	printf("%4.1f\n", vmem_movsw/1024.0);
	else			printf("N/A\n");

	gotoxy(29,11);
	if(vmem_movsd > 0)	printf("%4.1f\n", vmem_movsd/1024.0);
	else			printf("N/A\n");

	gotoxy(59,11);
	if(vmem_stosw > 0)	printf("%4.1f\n", vmem_stosw/1024.0);
	else			printf("N/A\n");

	gotoxy(69,11);
	if(vmem_stosd > 0)	printf("%4.1f\n", vmem_stosd/1024.0);
	else			printf("N/A\n");


// (lmem_movsw/1024.0),(lmem_movsd/1024.0), (lmem_stosw/1024.0),(lmem_stosd/1024.0));
// (hmem_movsw/1024.0),(hmem_movsd/1024.0), (hmem_stosw/1024.0),(hmem_stosd/1024.0));
// (vmem_movsw/1024.0),(vmem_movsd/1024.0), (vmem_stosw/1024.0),(vmem_stosd/1024.0));

	gotoxy(0,12);


	printf("\n");
	printf("%s/%s/%s mode switch rate by INT: ", modetype[2], modetype[_modetype], modetype[2]);
	printf("%4.1f switches/second\n",(1/ (intPR_speed+intRP_speed)+0.05) );
	printf("(%2.2f æsec/switch:  %1.1f æsec up + %1.1f æsec down)", (intRP_speed+intPR_speed)*1000000+0.005, (intRP_speed*1000000)+0.05, (intPR_speed*1000000)+0.05);
	printf("  IRQ up switch: %1.1f æsec\n",(irqRP_speed*1000000)+0.05);
//	printf("IRQ down switch: %1.1f æsec\n",(irqPR_speed*1000000)+0.05);
	printf("Protected mode code is running at CPL=%d",_codecpl);
	if(_codecpl==0)
		printf(" (lowest),");
	if(_codecpl==3)
		printf(" (highest),");
	printf("  IOPL=%d",_codeiopl);
	if(_codeiopl==0)
		printf(" (lowest)");
	if(_codeiopl==3)
		printf(" (highest)");
	printf("\n");


	printf("\n");
	printf("CPU performance: %1.1f MIPS  ",risc_speed/1000000);
	printf("(%1.1f mil. privileged instr/sec)\n",priv_speed/1000000);
	printf("Floating-point performance: %1.1f MIPS (FADD instruction)\n",fpu_speed/1000000);

/*
	f1=	(long)(1)+(long)(risc_speed/10000);
	f2=	(long)(1)+(long)(fpu_speed/1000);
	f3=	(long)(1)+(long)((1/(intPR_speed+intRP_speed))/10);
	f4=	(long)(1)+(long)((lmem_movsd+lmem_stosd)/10);
	f5=	(long)(1)+(long)((hmem_movsd+hmem_stosd)/10);

	f=(f1 + f2 + f3 + f4 + f5) /50;
	//printf("\n 1) %f\n 2) %f\n 3) %f\n 4) %f\n 5) %f\n",f1,f2,f3,f4,f5);

	printf("Overall system performance running ");
	if(_extendertype==0xFF)
		printf("Unknown");
	else
		printf("%s",dosextender[_extendertype]);
	if(_extendertype==2)
		printf("%01d.%02d", (get_pmodew_ver()&0x0FF00)>>8,get_pmodew_ver()&0xFF);
	printf(" DOS Extender\n");
	printf("is measured to:  %1.1f NRPP (non-relative performance points)",f);
*/
	printf("\n");
}










/****************************************************************************/
void measure_cpu_speed(void)
{
	int n;
	float f;
	float cpu_time;
	float cpu_test[5];
	float timer_time=1193181;		// timer freq in Hz

	if(_cputype==3) cpu_time=41000;		// div ebx on 386 = 41 clocks
	if(_cputype==4) cpu_time=40000;		// div ebx on 486 = 40 clocks

	test_cpu();				// preload cache
	if(_cputype<5)
	{
		for(n=0; n<5; n++) cpu_test[n]=test_cpu();
		f=(cpu_test[0]+cpu_test[1]+cpu_test[2]+cpu_test[3]+cpu_test[4])/5.0;
		cpu_speed=((timer_time*cpu_time)/(f*1000000))*2.0+0.05;_rawcpuspeed=f;
	}
	else
	{
		cpu_speed=(test_cpu()/1000000.0);
	}
}


void measure_fpu_speed(void)
{
	int n;
	float f;
	float test_time[5];
	float instructions=1000;
	float timer_time=1193181;		// timer freq in Hz

	test_fpu();				// preload cache
	for(n=0; n<5; n++) test_time[n]=test_fpu();
	f=(test_time[0]+test_time[1]+test_time[2])/3.0;
	fpu_speed=1.0/( ((f/2.0)/timer_time) /instructions);
}

void measure_risc_speed(void)
{
	int n;
	float test_time[16];
	float f=0;
	float instructions=1000;
	float timer_time=1193181;		// timer freq in Hz

	test_risc();				// preload cache
	for(n=0; n<16; n++) test_time[n]=test_risc();
	for(n=0; n<16; n++) f=f+test_time[n];
	f=f/16.0;
	risc_speed=1.0/( ((f/2.0)/timer_time) /instructions);
}

void measure_priv_speed(void)
{
	int n;
	float f;
	float test_time[5];
	float instructions=1000;
	float timer_time=1193181;		// timer freq in Hz

	test_priv();				// preload cache
	for(n=0; n<5; n++) test_time[n]=test_priv();
	f=(test_time[0]+test_time[1]+test_time[2])/3.0;
	priv_speed=1.0/( ((f/2.0)/timer_time) /instructions);
}

void measure_int_speed(void)
{
#undef	TESTS
#define	TESTS 1024

	int n;
	dword t;
	float f;
	float test_timePR[TESTS];
	float test_timeRP[TESTS];
	float timer_time=1193181;		// timer freq in Hz

	test_int();
	for(n=0; n<TESTS; n++)
	{
		t=test_int();
		test_timePR[n]=(t&0x0000FFFF);
		test_timeRP[n]=(t&0xFFFF0000)>>16;
	}
	f=0;
	for(n=0; n<TESTS; n++) f=f+test_timePR[n];
	f=f/TESTS;
	intPR_speed=(f/2)/timer_time;

	f=0;
	for(n=0; n<TESTS; n++) f=f+test_timeRP[n];
	f=f/TESTS;
	intRP_speed=(f/2)/timer_time;

}

void measure_irq_speed(void)
{
#undef	TESTS
#define	TESTS 64

	int n;
	dword t;
	float f;
	float test_timePR[TESTS];
	float test_timeRP[TESTS];
	float timer_time=1193181;		// timer freq in Hz

	test_irq();
	for(n=0; n<TESTS; n++)
	{
		t=test_irq();
		test_timePR[n]=(t&0x0000FFFF);
		test_timeRP[n]=(t&0xFFFF0000)>>16;
	}
	f=0;
	for(n=0; n<TESTS; n++) f=f+test_timePR[n];
	f=f/TESTS;
	irqPR_speed=(f/2)/timer_time;

	f=0;
	for(n=0; n<TESTS; n++) f=f+test_timeRP[n];
	f=f/TESTS;
	irqRP_speed=(f/2)/timer_time;
}


void measure_lmem_speed(void)
{
	int n;
	float f;
	float mem_test[5];
	float mem_size=150*1024;
	float timer_time=1193181;			// timer freq in Hz

	if(test_low_movsw()==0xFFFFFFFF) lmem_movsw=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_low_movsw();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	lmem_movsw=((mem_size/(f/timer_time))/1024)*2;
	}
	if(test_low_movsd()==0xFFFFFFFF) lmem_movsd=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_low_movsd();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	lmem_movsd=((mem_size/(f/timer_time))/1024)*2;
	}

	if(test_low_stosw()==0xFFFFFFFF) lmem_stosw=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_low_stosw();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	lmem_stosw=((mem_size*2/(f/timer_time))/1024)*2;
	}

	if(test_low_stosd()==0xFFFFFFFF) lmem_stosd=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_low_stosd();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	lmem_stosd=((mem_size*2/(f/timer_time))/1024)*2;
	}

}

void measure_hmem_speed(void)
{
	int n;
	float f;
	float mem_test[5];
	float mem_size=150*1024;
	float timer_time=1193181;			// timer freq in Hz

	if(test_high_movsw()==0xFFFFFFFF) hmem_movsw=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_high_movsw();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	hmem_movsw=((mem_size/(f/timer_time))/1024)*2;
	}

	if(test_high_movsd()==0xFFFFFFFF) hmem_movsd=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_high_movsd();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	hmem_movsd=((mem_size/(f/timer_time))/1024)*2;
	}


	if(test_high_stosw()==0xFFFFFFFF) hmem_stosw=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_high_stosw();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	hmem_stosw=((mem_size*2/(f/timer_time))/1024)*2;
	}

	if(test_high_stosd()==0xFFFFFFFF) hmem_stosd=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_high_stosd();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	hmem_stosd=((mem_size*2/(f/timer_time))/1024)*2;
	}

}

void measure_vmem_speed(void)
{
	int n;
	float f;
	float mem_test[5];
	float mem_size=64*1024;
	float timer_time=1193181;			// timer freq in Hz

	setvideomode(0x13);
	if(test_vid_movsw()==0xFFFFFFFF) vmem_movsw=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_vid_movsw();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	vmem_movsw=((mem_size/(f/timer_time))/1024)*2;
	}

	if(test_vid_movsd()==0xFFFFFFFF) vmem_movsd=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_vid_movsd();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	vmem_movsd=((mem_size/(f/timer_time))/1024)*2;
	}


	if(test_vid_stosw()==0xFFFFFFFF) vmem_stosw=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_vid_stosw();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	vmem_stosw=((mem_size/(f/timer_time))/1024)*2;
	}

	if(test_vid_stosd()==0xFFFFFFFF) vmem_stosd=-1;
	else {
	for(n=0; n<5; n++) mem_test[n]=test_vid_stosd();
	f=(mem_test[0]+mem_test[1]+mem_test[2]+mem_test[3]+mem_test[4])/5.0;
	vmem_stosd=((mem_size/(f/timer_time))/1024)*2;
	}
	setvideomode(3);
}


