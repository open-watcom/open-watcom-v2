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

 NULLPTR.C
 This is an example of what happens when you use an uninitialized (null)
 pointer to access memory under DOS/32 Advanced with Null-Pointer Protection
 turned on.

*****************************************************************************/


	char	*mypointer;

/*
#pragma aux nullptr =	\
	"mov	ds:[0000h],al";
*/

void nullptr() {
	mypointer=0;
	*mypointer=0;
}

#pragma aux cls =	\
	"mov	ax,03h"	\
	"int	10h"	\
	modify [eax];

#pragma aux getkey =	\
	"xor	ax,ax"	\
	"int	16h"	\
	modify [eax];

#pragma aux nullptrchk =	\
	"mov	ax,0FF89h"	\
	"int	21h"		\
	"cmp	eax,'ID32'"	\
	"mov	eax,2"		\
	"jnz	@@done"		\
	"test	edx,80h"	\
	"mov	eax,1"		\
	"jz	@@done"		\
	"mov	eax,0"		\
"@@done:"			\
	modify [eax ebx ecx edx esi edi]\
	value [eax];



void main() {

	cls();

	printf("\
	==================================================================\n\
	 This program will show how DOS/32 Advanced built-in Null-Pointer \n\
	 Protection feature can be used to trap down writes to the memory \n\
	 in range 0:00000000h to 0:00000010h (abs. addresses).            \n\
\n\
	 When a null-pointer  is used  to access  (write to) memory,  the \n\
	 real mode interrupt vector table  is most likely to be modified, \n\
	 and the first interrupt vectors, starting with INT 00h are to be \n\
	 overwritten  since the bases of DS, ES and SS selectors are zero \n\
	 under  DOS/4GW  and  DOS/32 Advanced.  This may lead to improper \n\
	 exception handling in real mode and in worst case to application \n\
	 crashes,  which  in  their  turn  can  lead  to loss of data. To \n\
	 prevent this, DOS/32 Advanced offers the programmers the ability \n\
	 to  write-protect  the  first  16 bytes of memory,  starting  at \n\
	 absolute address 0:00000000h. The protection is done by hardware \n\
	 and 80x86 debug registers are used to accomplish this task.      \n\
\n\
	 When developing an application,  turn on the Null-Ptr Protection \n\
	 to ensure  that your program does not contain  any uninitialized \n\
	 (null) pointers.                                                 \n\
	==================================================================\n\n");

	printf("Now the Null-Pointer Protection will be triggered and debugger mode entered.\n");
	printf("Press any key to continue...\n");
	getkey();
	printf("\n");

	switch(nullptrchk()) {

		case 0:	nullptr();
			break;

		case 1:	printf("Null-Pointer Protection is not active!!!\n\n");
			printf("To turn on  Null-Pointer Protection either use\n");
			printf("the  SUNSYS Setup Utility Program,  or add the\n");
			printf("following keyword to DOS32A environment string\n");
			printf("in your autoexec.bat: /NULLP\n\n");
			break;

		case 2:	printf("DOS/32 Advanced not detected!!!\n\n");
			break;
	}
	exit(0);
}
