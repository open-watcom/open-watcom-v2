/*
 * Copyright (C) 1996-2006 by Narech K. All rights reserved.
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

#include <dos.h>
#include <math.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <typedefs.h>
#include <sys\stat.h>
//#include <debug.h>

#define	BLACK		0
#define BLUE		1
#define	GREEN		2
#define	CYAN		3
#define	RED		4
#define	MAGENTA		5
#define	BROWN		6
#define	WHITE		7
#define	LIGHTGREEN	10
#define	LIGHTBLUE	11
#define LIGHTRED	12
#define	YELLOW		14
#define	LIGHTWHITE	15

#define	ESC		0x01
#define	ALT_X		0x2D
#define TAB		0x0F
#define	ENTER		0x1C
#define	HOME		0x47
#define	END		0x4F
#define	PAGEUP		0x49
#define	PAGEDOWN	0x51
#define	SPACE		0x39
#define	UP		0x48
#define	DOWN		0x50
#define	LEFT		0x4B
#define	RIGHT		0x4D
#define	F1		0x3B
#define	F2		0x3C
#define	F3		0x3D
#define	F4		0x3E
#define	F5		0x3F
#define F6		0x40
#define	F7		0x41
#define	F8		0x42
#define	F9		0x43
#define	F10		0x44

void Print(char *, ...);
void Print_At(int, int, char *, ...);
void CloseAllWindows(void);
void ShowSysInfo(void);
void ShowEnvInfo(void);
void ShowHelp(int, int);
void ShowCopyright(int, char *[]);
void AskExit(void);
void ApplyExit(void);
void DiscardExit(void);
char *InputString(int, int, int, char *);

void ShowKernelMenu(void);
void ShowKernelConfig(void);
void SelectKernelMenu(void);

void ShowExtenderMemu(void);
void ShowExtenderConfig(void);
void SelectExtenderMenu(void);

void ShowMainMenu(void);
void SelectMainMenu(void);
unsigned long Input(int, int, int, unsigned long);
void ShowMemory(void);


extern	void VideoInit(void);
extern	void VideoReset(void);
extern	void DrawBackground(void);
extern	void SetPos(char, char);
extern	void SetColor(char);
extern	void SetBackColor(char);
extern	void SetBlink(char);
extern	void Prints(char *);
extern	void PrintHelp(char *);
extern	short int GetKey(void);
extern	void OpenWindow(char, char, char, char);
extern	void DrawWindow(char, char, char, char);
extern	void CloseWindow(void);
extern	void ShowCursor(char, char, char);
extern	dword get_cpu_type(void);
extern	dword get_fpu_type(void);
extern	dword get_sys_type(void);
extern	dword get_himem_size(void);
extern	dword get_lomem_size(void);
extern	dword test_cpu(void);
extern	dword get_dosmem_used(int);
extern	dword get_total_mem(void);
extern	dword get_dpmi_bufsize(void);
extern	int ReadHeader(char *, void *);
extern	int WriteHeader(void *);
extern	void CloseFile(void);
extern	int GetFileType(void);
extern	int GetExtenderType(void);

extern	char keychar;
extern	char keycode;


/************** Variables ***************************************************/

	struct header {
		dword	id;
		byte	kernel_misc;
		byte	kernel_pagetables;
		byte	kernel_phystables;
		byte	kernel_callbacks;
		word	kernel_selectors;
		byte	kernel_rmstacks;
		byte	kernel_pmstacks;
		word	kernel_rmstacklen;
		word	kernel_pmstacklen;
		dword	kernel_maxextmem;
		byte	dos32a_misc;
		byte	dos32a_misc2;
		word	dos32a_lowbufsize;
		word	dos32a_version;
		word	dos32a_reserved;

		char	dos32a_buildname[0x09];
		char	dos32a_buildvers[0x10];
		char	dos32a_copyright1[0x24];
		char	dos32a_copyright2[0x15];
		char	dos32a_builddate[0x09];
		char	dos32a_buildtime[0x09];
	};

	char*	version =	"9.1.2";
	int	version2 =	0x090C;
	int	changed	=	0;
	int	hexmode =	0;

	char	startdir[80];

	char	cfgfilename[80];

	int	systype =	0;
	int	memtotal =	0;
	int	mem_dos32a =	0x75D0;
	int	mem_kernel =	0x27A0;
	int	file_type =	0;
	int	extn_type =	0;

	int	silent = FALSE;
	int	modified_flag = FALSE;

	struct	header id32;
	struct	header id32_old;

	int	mainmenu_sel=0;
	int	mainmenu_xpos[7] = { 36,36,36,36, 36,36,36 };
	int	mainmenu_ypos[7] = { 3, 4, 5, 6,  8, 9, 10 };
	int	mainmenu_xlen[7] = { 40,40,40,40, 40,40,40 };

	int	kernelmenu_sel=0;
	int	kernelmenu_xpos[16] = { 9, 9, 9, 9, 9, 9, 9,  9, 9, 9, 9,  39,39,39,39, 19 };
	int	kernelmenu_ypos[16] = { 3, 4, 5, 6, 7, 8, 9, 11,12,13,14, 11,12,13,14, 15 };
	int	kernelmenu_xlen[16] = { 62,62,62,62,62,62,62,28,28,28,28, 32,32,32,32, 30 };

	int	extendermenu_sel=0;
	int	extendermenu_xpos[13] = { 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,  18 };
	int	extendermenu_ypos[13] = { 3, 4, 5, 6, 7, 8, 9, 10,11,12,13,14, 15 };
	int	extendermenu_xlen[13] = { 62,62,62,62,62,62,62,62,62,62,62,62, 35 };

	char*	errstr="SS/32A fatal:";


	char*	mainhlp[]={

"Configure DOS/32 Advanced built-in DPMI Server.",


"Configure DOS/32 Advanced DOS Extender.",


"Create <Predefined Configuration> that can be used for quick configuration\n\
of your DOS/32 Advanced programs from the command line.",


"Restore previously created <Predefined Configuration>.",


"Discard the changes and restore old configuration.",


"Discard the changes you've made and Exit.",


"Apply the changes you've made and Exit."

};


	char*	kernelhlp[]={
/*
"Controls  the  order of <VCPI/DPMI detection>.  When set to VCPI/DPMI, the\n\
DOS  Extender  will  try to detect,  and  if succeeded initialize a VCPI\n\
server even though a DPMI host may be present in the system.",

"Controls  the  order  of <XMS/VCPI detection>.  When  set to XMS/VCPI, the\n\
DOS Extender will whenever possible favour  Clean and XMS systems before\n\
the slower VCPI/DPMI ones.",
*/
"<Deprecated>.",
"<Deprecated>.",

/*
"Controls  whether DOS/32 Advanced DPMI will internally handle and report\n\
all the exceptions occured during program execution, or the program will\n\
handle exceptions itself (no INT/EXC buffering and IRQ callbacking).",
*/

"When  turned on, DOS/32 Advanced will use <0th PageTable> which maps first\n\
4MB (including DOS' 640K), to map the extended memory it allocated. This\n\
will often reduce the amount of used DOS memory by 4KB (one pagetable).",


"When  turned  on, DOS/32 Advanced will  try  to allocate extended memory\n\
from  both VCPI and XMS servers, making it possible to have an access to\n\
all the memory available even when EMM386 with \"RAM nnn\" is installed.",

/*
"Interrupts in range INT 08-0Fh can be <Exceptions>, <IRQs> or <software INTs>.\n\
DOS/32 Advanced is capable of trapping  software INTs, issued to emulate\n\
IRQs 0-7 (unremapped), and will do so when this option is turned on.",


"When  turned on, DOS/32 Advanced will check, that extended memory blocks\n\
allocated  with ADPMI are not overwritten or corrupted. An error will be\n\
reported when a program tries to write to memory it hadn't allocated.",
*/
"<Deprecated>.",
"<Deprecated>.",

/*
"DPMI function 0008h - <Set Segment Limit> requires limits greater than 1MB\n\
to have low 12 bits set. DOS/32 Advanced allows users to manually choose\n\
whether this check is to be done. Turn it off for DOS/4GW compatibility.",
*/

"When this option is turned on, DOS/32 Advanced DPMI will ignore calls to\n\
function 0A00h  with  <DOS/4G ID-string>  pointer  in DS:ESI, thus forcing\n\
programs to not to use DOS/4G API extensions.",


"The number of <Selectors> DOS/32 Advanced will allocate and make available\n\
for  program's  use. It is not recommended  to  set this value below 16.",


"The number of <Callbacks> DOS/32 Advanced will allocate and make available\n\
for program's use. Note that at least 8 callbacks should be allocated to\n\
ensure that DOS/32 Advanced itself will operate properly.",


"The number of <Real-Mode Virtual Stacks> that will be allocated by the DOS\n\
Extender.  Each time the protected mode  program needs to call real mode\n\
INT- or IRQ-handler, a Virtual stack will be used.",


"The  number  of <Protected-Mode Virtual Stacks>  that will be allocated by\n\
DOS/32  Advanced. Whenever a real mode  INT- or IRQ-handler needs to get\n\
an access to the protected mode code, a Virtual stack will be used.",


"Maximum  number  of  <PageTables> that will  be  allocated (in DOS memory)\n\
under  VCPI. Each PageTable will map 4MB  of memory. Note that this is a\n\
limit, not the actual number of pagetables that will be allocated.",


"Maximum  number  of <PageTables> for  physical  memory mapping under VCPI.\n\
If you are sure that an application does not use physical memory mapping\n\
feature, reset this value to zero.",


"The size of one <Real-Mode Virtual Stack> in paragraphs.",


"The size of one <Protected-Mode Virtual Stack> in paragraphs.",


"Amount  of <Extended Memory> that will be allocated and made available for\n\
program's  use.  Values greater than 2GB  will cause the DOS Extender to\n\
allocate all the extended memory available in the machine."

};


	char*	extenderhlp[]={

"When turned off, no warnings will be displayed on the screen. Only fatal\n\
and run-time errors will be reported to the user.",


"When  turned  on,  run-time errors trapped  by  the DOS Extender will be\n\
accompanied  by  a tone from the PC  speaker. Note that no sound will be\n\
generated on warnings and fatal errors.",


"When  turned on, DOS/32 Advanced will save the whole <Real Mode interrupt\n\
table> (all 256 interrupt vectors) on startup and restore it on exit.",


"When  turned on, the DOS Extender will  on exit check and report all the\n\
<Real Mode interrupt vectors>  which had  been modified (and not restored)\n\
by a protected mode program.",


"Controls how application's <Objects> are loaded into memory:\n\
<#1> 16bit/32bit -> Low, High;   <#2> 16bit -> Low, High  32bit -> High only\n\
<#3> 16bit/32bit -> Low only;    <#4> 16bit/32bit -> High only (as DOS/4GW)",


"Defines  <Alignment  of  Objects>  that  are  loaded  high  (into extended\n\
memory).  Can be either <PARA>graph (16 bytes), or <PAGE> (4096 bytes). Note\n\
that Objects loaded low (into DOS memory) are always PARAgraph aligned.",


"When  turned on, DOS/32 Advanced will  set VideoMode 03h (VideoBIOS Text\n\
Mode) when it encounters a run-time error before writing a report on the\n\
screen.",


"When this option is turned on and running under Windows (v3 and 4+), the\n\
DOS Extender will switch the DOS Box  of the Virtual Machine in which it\n\
is currently running to <Full-Screen mode>.",


"When turned on, DOS/32 Advanced will write-protect the first 16 bytes of\n\
memory  starting at absolute address 0:0.  Note that 386 debug registers\n\
DR0-DR3 will be used to trap down writes to the memory at run-time.",


"This  option controls whether the <Copyright banner> is to be displayed on\n\
DOS Extender's startup.",


"When  turned on, DOS/32 Advanced will search for \"<DOS32A=>\" string in the\n\
environment, and if it is defined, configure itself according to it.",


"When  this option is turned on, the  DOS Extender  will start in <Verbose\n\
Mode>,  monitoring its actions  as it  loads a protected mode application\n\
and showing additional information in exception listing on exceptions.",


"Specifies  the  size of the <DOS Transfer Buffer> (located in conventional\n\
memory)  which  will be used by  the DOS Extender when transferring data\n\
from DOS to extended memory and back."


};
