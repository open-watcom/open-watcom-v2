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

 EXCEPT86.C
 This program will show how the exceptions are handled under DOS/32 Advanced.

*****************************************************************************/


#pragma aux except86 =	\
	"mov	cs:[0080h],al";

#pragma aux cls = \
	"mov	ax,03h"	\
	"int	10h"	\
	modify [eax];

#pragma aux getkey =	\
	"xor	ax,ax"	\
	"int	16h"	\
	modify [eax];

#pragma aux except86chk =	\
	"mov	ax,0FF8Ah"	\
	"int	21h"		\
	"cmp	eax,'ID32'"	\
	"mov	eax,3"		\
	"jnz	@@done"		\
	"cmp	ch,3"		\
	"mov	eax,2"		\
	"jz	@@done"		\
	"test	dl,02h"		\
	"mov	eax,1"		\
	"jz	@@done"		\
	"mov	eax,0"		\
"@@done:"			\
	modify [eax ebx ecx edx esi]\
	value [eax];





void main() {

	cls();

	printf("\
	==========================================================\n\
	 This program  will  generate  an exception #13,  General \n\
	 Protection Fault,  just to show you how  DOS/32 Advanced \n\
	 traps down run-time errors and reports them to the user, \n\
	 and how you can  debug an application  that generates an \n\
	 exception.                                               \n\
\n\
	 To debug an application that contains error,  use SUNSYS \n\
	 Debugger.  Note the unrelocated address,  and  start the \n\
	 Debugger by typing  SD <filename.exe> at DOS prompt. Now \n\
	 press the  ENTER  key and enter  the unrelocated address \n\
	 you memorized preceeded by a  <.>  (point). The Debugger \n\
	 will now show the faulting instruction under the cursor. \n\
\n\
	 You may  also want  to instruct  the linker  to generate \n\
	 a map file.  By looking at  map file  and compairing the \n\
	 unrelocated addresses you may gain the information about \n\
	 where,  in which module  and  which subroutine  in  your \n\
	 application the exception is thrown.                     \n\
	==========================================================\n\n");

	printf("Now the exception #13 will be generated.\n");
	printf("Press any key to continue...\n");
	getkey();
	printf("\n");

	switch(except86chk()) {
		case 0:	except86();
			break;

		case 1:	printf("Internal Exception Control is not active!!!\n\n");
			printf("Use the SUNSYS Setup Utility Program to turn on the Internal\n");
			printf("Exception Control.\n");
			break;

		case 2: printf("Generating exception under external DPMI host...\n\n");
			except86();
			break;

		case 3:	printf("DOS/32 Advanced DOS Extender not present!!!\n\n");
			printf("You should not run this test program under other DOS Extenders\n");
			printf("than DOS/32 Advanced.\n");
			break;
	}

	exit(0);
}
