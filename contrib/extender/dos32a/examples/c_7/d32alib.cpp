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

/*****************************************************************************

 D32ALIB.CPP
 This example is a quick reference test of all the currently available
 functions in D32A C/C++ Library. The purpose is to show off how to use each
 library function. The example can be compiled as a C as well as C++ program.
 To switch from the C++ mode and allow WATCOM Compiler to compile this
 example in C mode rename the file to "d32alib.c".

*****************************************************************************/

#include <stdio.h>
#include <conio.h>
#include <string.h>

#include <d32a.h>
#include <debug.h>

/* Declare function prototypes (for C++ compiler) */
void	test_detect(void);
void	test_keybrd(void);
void	test_malloc(void);
void	test_comprs(void);
void	test_system(void);

/* Some temporary variables used by the functions below */
	int n,m;
	char c;
	float f;
	char *ptr, *ptr1, *ptr2, *ptr3;
	char *ptrL1, *ptrL2, *ptrL3;
	char *ptrH1, *ptrH2, *ptrH3;



/****************************************************************************/
int main() {

//	Debug_Init();		/* Initialize Debugger Library */
	printf("\n");

	test_detect();		/* Test detection functions */
	test_keybrd();		/* Test keyboard functions */
	test_malloc();		/* Test memory functions */
	test_comprs();		/* Test compression functions */
	test_system();		/* Test system functions */

	return(0);
}





/****************************************************************************/
void test_detect() {

	printf("Testing DETECT routines\n");
	printf("=======================\n\n");

	printf("Testing  d32a_detect_extender() ...\n");
	n=d32a_detect_extender();
	printf("Return value = %d\n\n",n);

	printf("Testing  d32a_detect_adpmi() ...\n");
	n=d32a_detect_adpmi();
	printf("Return value = %d\n\n",n);

	printf("Testing  d32a_detect_system_software() ...\n");
	n=d32a_detect_system_software();
	printf("Return value = %d\n\n",n);

	printf("Testing  d32a_detect_windows() ...\n");
	n=d32a_detect_windows();
	printf("Return value = %d\n\n",n);

/*--------------*/
	printf("Testing  d32a_get_extender_version() ...\n");
	n=d32a_get_extender_version();
	printf("Return value = %08X\n\n",n);

	printf("Testing  d32a_get_dpmi_version() ...\n");
	n=d32a_get_dpmi_version();
	printf("Return value = %08X\n\n",n);

	printf("Testing  d32a_get_dos_version() ...\n");
	n=d32a_get_dos_version();
	printf("Return value = %08X\n\n",n);

	printf("Testing  d32a_get_windows_version() ...\n");
	n=d32a_get_windows_version();
	printf("Return value = %08X\n\n",n);

	printf("Testing  d32a_get_library_version() ...\n");
	n=d32a_get_library_version();
	printf("Return value = %08X\n\n",n);

/*--------------*/
	printf("Testing  d32a_get_cpu_type() ...\n");
	n=d32a_get_cpu_type();
	printf("Return value = %X\n",n);
	printf("_cpu_equipment = %08X\n\n",_cpu_equipment);

	printf("Testing  d32a_get_fpu_type() ...\n");
	n=d32a_get_fpu_type();
	printf("Return value = %X\n\n",n);

	printf("Testing  d32a_get_cpu_id() ...\n");
	ptr=d32a_get_cpu_id();
	printf("Return value = %s\n\n",ptr);

	printf("Testing  d32a_get_cpu_speed() ...\n");
	n=d32a_get_cpu_speed();
	printf("Return value = %d\n\n",n);

}



/****************************************************************************/
void test_keybrd() {

	printf("Testing KEYBRD routines\n");
	printf("=======================\n\n");

	printf("Testing  d32a_kbhit() ...\n");
	while( !(n=d32a_kbhit()) );
	printf("Return value = %d\n\n",n);

	printf("<%c>\n",getch());

	printf("Testing  d32a_kbwait() ...\n");
	d32a_kbwait();
	printf("Ok.\n\n");

	printf("<%c>\n",getch());

	printf("Testing  d32a_getkey() ...\n");
	c=d32a_getkey();
	printf("Return value = <%c>\n",c);
	printf("_kbrd_asciicode = <%c>\n",_kbrd_asciicode);
	printf("_kbrd_scancode = %02Xh\n\n",_kbrd_scancode);


}



/****************************************************************************/
void test_malloc() {

/* Note that the structures are "typedef'ed" in D32A.H */
	D32A_MEMINFO mem;		/* MemInfo structure */
	D32A_MEMBLOCKINFO block;	/* MemBlockInfo structure */


	printf("Testing MALLOC routines\n");
	printf("=======================\n\n");

	printf("Testing  d32a_getfreemem_low() ...\n");
	n=d32a_getfreemem_low();
	printf("Return value = %08Xh\n\n",n);

	printf("Testing  d32a_getfreemem_high() ...\n");
	n=d32a_getfreemem_high();
	printf("Return value = %08Xh\n\n",n);

	printf("Testing  d32a_getfreemem_total() ...\n");
	n=d32a_getfreemem_total();
	printf("Return value = %08Xh\n\n",n);

/*--------------*/
	printf("Testing  d32a_malloc() ...\n");
	ptrH1=d32a_malloc(0x11111);
	printf("Return ptr = %08Xh\n\n",ptrH1);

	printf("Testing  d32a_malloc_low() ...\n");
	ptrL1=d32a_malloc_low(0x10000);
	printf("Return ptr = %08Xh\n\n",ptrL1);

	printf("Testing  d32a_malloc_high() ...\n");
	ptrH2=d32a_malloc_high(0x22222);
	printf("Return ptr = %08Xh\n\n",ptrH2);

	printf("Testing  d32a_malloc_low() ...\n");
	ptrL2=d32a_malloc_low(0x20000);
	printf("Return ptr = %08Xh\n\n",ptrL2);

	printf("Testing  d32a_malloc_high() ...\n");
	ptrH3=d32a_malloc_high(0x33333);
	printf("Return ptr = %08Xh\n\n",ptrH3);

	printf("Testing  d32a_malloc_low() ...\n");
	ptrL3=d32a_malloc_low(0x30000);
	printf("Return ptr = %08Xh\n\n",ptrL3);

/*--------------*/
	printf("Testing  d32a_getfreemem_info() ...\n");
	d32a_getfreemem_info(&mem);
	printf("Return = %08X, %08X\n\n",
		mem.LargestFreeBlock,
		mem.LinearSpaceInPages);

	printf("Testing  d32a_getmemblock_info() ...\n");
	d32a_getmemblock_info(ptrL2, &block);
	printf("Return = %08X, %08X\n\n",
		block.BlockBase,
		block.BlockSize);

/*--------------*/
	printf("Testing  d32a_free() ...\n");
	n=d32a_free(ptrH2);
	printf("Return value = %d\n\n",n);

	printf("Testing  d32a_free() ...\n");
	n=d32a_free(ptrL2);
	printf("Return value = %d\n\n",n);

	printf("Testing  d32a_free_all() ...\n");	/* free all blocks */
	d32a_free_all();
	printf("Ok.\n\n",n);

	printf("Testing  d32a_free() ...\n");	/* this one should fail */
	n=d32a_free(ptrH1);
	printf("Return value = %d\n\n",n);

/*--------------*/
	printf("Testing  d32a_getfreemem_low() ...\n");
	n=d32a_getfreemem_low();
	printf("Return value = %08Xh\n\n",n);

	printf("Testing  d32a_getfreemem_high() ...\n");
	n=d32a_getfreemem_high();
	printf("Return value = %08Xh\n\n",n);

	printf("Testing  d32a_getfreemem_total() ...\n");
	n=d32a_getfreemem_total();
	printf("Return value = %08Xh\n\n",n);


}



/****************************************************************************/
void test_comprs() {

	char *str="1234567890_AAAAAAAA_BBBBBBBB_CCCCCCCC_DDDDDDDD";
	char buf1[512];
	char buf2[512];


	printf("Testing COMPRS routines\n");
	printf("=======================\n\n");

	memset(&buf1, 0, sizeof(buf1));
	memset(&buf2, 0, sizeof(buf2));

	printf("Testing  d32a_encode_data() ...\n");
	n=d32a_encode_data(str,buf1,strlen(str));
	printf("Original string = %s\n",str);
	printf("Original length = %d\n",strlen(str));
	printf("Return value = %d\n\n",n);

	printf("Testing  d32a_decode_data() ...\n");
	n=d32a_decode_data(buf1,buf2,n);
	printf("Decoded string  = %s\n",buf2);
	printf("Return value = %d\n\n",n);

}



/****************************************************************************/
void test_system() {

/* Note that the structures are "typedef'ed" in D32A.H */
	D32A_REGS in_regs;		/* InRegs structure */
	D32A_REGS out_regs;		/* OutRegs structure */
	D32A_SREGS seg_regs;		/* SegRegs structure */


	printf("Testing SYSTEM routines\n");
	printf("=======================\n\n");

	memset(&in_regs, 0, sizeof(in_regs));
	memset(&out_regs, 0, sizeof(out_regs));
	memset(&seg_regs, 0, sizeof(seg_regs));

/*
   A special note:
   The way of accessing the registers in the structure is slightly different
   from the standard WATCOM Run-time library. To access 1 byte large registers
   use "b" (ex InRegs.b.ah), to access 2 byte large registers use "w" and
   for 4 byte large registers use "d" (think of "b"=byte, "w"=word and
   "d"=dword)
*/
	in_regs.w.ax	= 0xFF88;
	in_regs.d.ebx	= 0x11111111;
	in_regs.d.ecx	= 0x22222222;
	in_regs.d.edx	= 0x33333333;
	in_regs.d.esi	= 0x44444444;
	in_regs.d.edi	= 0x55555555;
	in_regs.d.ebp	= 0x66666666;
	in_regs.d.eflags= 0x77777777;

	seg_regs.ds	= 0x0000;
	seg_regs.es	= 0x0040;	// BIOS 0040h selector
	seg_regs.fs	= 0x0001;	// Invalid selectors in protected mode
	seg_regs.gs	= 0x0002;	//  ignored by d32a_int86xpm function

	printf("Testing  d32a_int86pm() ...\n");
	d32a_int86pm(0x21, &in_regs, &out_regs);
	printf("Return EAX value = %08X\n\n",out_regs.d.eax);

	printf("Testing  d32a_int86rm() ...\n");
	d32a_int86rm(0x21, &in_regs, &out_regs);
	printf("Return EAX value = %08X\n\n",out_regs.d.eax);

	printf("Testing  d32a_int86xpm() ...\n");
	d32a_int86xpm(0x21, &in_regs, &out_regs, &seg_regs);
	printf("Return EAX value = %08X\n\n",out_regs.d.eax);

	printf("Testing  d32a_int86xrm() ...\n");
	d32a_int386xrm(0x21, &in_regs, &out_regs, &seg_regs);	// 386 = 86
	printf("Return EAX value = %08X\n\n",out_regs.d.eax);


}

