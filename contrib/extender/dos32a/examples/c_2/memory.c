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

 MEMORY.C
 This program will show memory availability under DOS/32 Advanced.
 Note that this program is linked in LE format, so that you could run it
 under other DOS Extenders.

*****************************************************************************/



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

#pragma aux getmeminfo =		\
	"mov	eax,0500h"		\
	"mov	edi,offset Mem"		\
	"int	31h"			\
	modify [eax edi];



void main(int argc, char *argv[]) {

	getmeminfo();

	printf("\n");
	printf("DPMI Memory Information:\n");
	printf("-----------------------------------------------------------------\n");

	printf("Largest Available Free Block Size:   ");
		printf("0x%08X (%9d) bytes",
		Mem.LargestFreeBlock,
		Mem.LargestFreeBlock);
	printf("\n");

	printf("Maximum Unlocked Page Allocations:   ");
	if(Mem.MaxUnlockedPages!=-1)
		printf("0x%08X (%9d) pages",
		Mem.MaxUnlockedPages,
		Mem.MaxUnlockedPages);
	else printf("information not available");
	printf("\n");

	printf("Maximum Locked Page Allocations:     ");
	if(Mem.MaxLockedPages!=-1)
		printf("0x%08X (%9d) pages",
		Mem.MaxLockedPages,
		Mem.MaxLockedPages);
	else printf("information not available");
	printf("\n");

	printf("Linear Address Space Size:           ");
	if(Mem.LinearSpaceInPages!=-1)
		printf("0x%08X (%9d) pages",
		Mem.LinearSpaceInPages,
		Mem.LinearSpaceInPages);
	else printf("information not available");
	printf("\n");

	printf("Total Number of Unlocked Pages:      ");
	if(Mem.TotalNumOfUnlockedPages!=-1)
		printf("0x%08X (%9d) pages",
		Mem.TotalNumOfUnlockedPages,
		Mem.TotalNumOfUnlockedPages);
	else printf("information not available");
	printf("\n");

	printf("Total Number of Free Pages:          ");
	if(Mem.TotalNumOfFreePages!=-1)
		printf("0x%08X (%9d) pages",
		Mem.TotalNumOfFreePages,
		Mem.TotalNumOfFreePages);
	else printf("information not available");
	printf("\n");

	printf("Total Number of Physical Pages:      ");
	if(Mem.TotalNumOfPhysPages!=-1)
		printf("0x%08X (%9d) pages",
		Mem.TotalNumOfPhysPages,
		Mem.TotalNumOfPhysPages);
	else printf("information not available");
	printf("\n");

	printf("Free Linear Address Space:           ");
	if(Mem.FreeLinearSpaceInPages!=-1)
		printf("0x%08X (%9d) pages",
		Mem.FreeLinearSpaceInPages,
		Mem.FreeLinearSpaceInPages);
	else printf("information not available");
	printf("\n");

	printf("Size of Paging File/Partition:       ");
	if(Mem.SizeOfPagingFile!=-1)
		printf("0x%08X (%9d) pages",
		Mem.SizeOfPagingFile,
		Mem.SizeOfPagingFile);
	else printf("information not available");
	printf("\n");


	exit(0);
}
