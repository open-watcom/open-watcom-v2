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

 PROG2.C
 See SPAWN.C

*****************************************************************************/


	unsigned char CpuType;
	unsigned char SysType;

struct memory {
	unsigned long LargestFreeBlock;
	unsigned long MaxUnlockedPages;
	unsigned long MaxLockedPages;
	unsigned long LinearSpaceInPages;
	unsigned long TotalNumOfUnlockedPages;
	unsigned long TotalNumOfFreePages;
	unsigned long TotalNumOfPhysPages;
	unsigned long FreeLinearSpaceInPages;
	unsigned long SizeOfPagingFile;
	unsigned long Reserved[3];
} Mem;


#pragma aux getmem =			\
	"mov	eax,0500h"		\
	"mov	edi,offset Mem"		\
	"int	31h"			\
	modify [eax edi];

#pragma aux getinfo =			\
	"push	fs"			\
	"mov	eax,0FF8Ah"		\
	"int	21h"			\
	"mov	CpuType,cl"		\
	"mov	SysType,ch"		\
	"pop	fs"			\
	modify [eax ebx ecx edx esi];




int main(int argc, char *argv[]) {
	int n,i;

	getmem();
	getinfo();
	printf("    PROG2:  SYS=%02Xh, MEM=%08Xh\n",
		SysType, Mem.FreeLinearSpaceInPages);

	return(0x88);
}
