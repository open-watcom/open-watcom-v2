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

 VESA.C
 An example program that shows how easy it is to use the extended VESA VBE
 functions under DOS/32 Advanced. Neither DPMI calls nor pointer translation
 are now required. Simply call VBE functions as if it was a real mode
 program, and the DOS Extender will do the rest of the job.
 Note that the converted pointers returned in structures by extended
 function 4F00h are relative to absolute 0 (beginning of the memory). You can
 use any zero selector (selector with Base=0) in combination with returned
 pointers to access the data (the DS, ES and SS selectors under DOS/4GW and
 DOS/32 Advanced are all zero selectors).

*****************************************************************************/


struct vbeinfoblock  {
	unsigned char	VbeSignature[4];
	unsigned short	VbeVersion;
	char		*OemStringPtr;
	unsigned long	Capabilities;
	unsigned short	*VideoModePtr;
	unsigned short	TotalMemory;

	unsigned short	OemSoftwareRev;
	char		*OemVendorNamePtr;
	char		*OemProductNamePtr;
	char		*OemProductRevPtr;
	unsigned char	Reserved1[222];
	unsigned char	OemData[256];
} VIB;


struct modeinfoblock {
	unsigned short	ModeAttributes;
	unsigned char	WinAAttributes;
	unsigned char	WinBAttributes;
	unsigned short	WinGranularity;
	unsigned short	WinSize;
	unsigned short	WinASegment;
	unsigned short	WinBSegment;
	unsigned long	WinFuncPtr;
	unsigned short	BytesPerScanLine;

	unsigned short	XResolution;
	unsigned short	YResolution;
	unsigned char	XCharSize;
	unsigned char	YCharSize;
	unsigned char	NumberOfPlanes;
	unsigned char	BitsPerPixel;
	unsigned char	NumberOfBanks;
	unsigned char	MemoryModel;
	unsigned char	BankSize;
	unsigned char	NumberOfImagePages;
	unsigned char	Reserved;

	unsigned char	RedMaskSize;
	unsigned char	RedFieldPosition;
	unsigned char	GreenMaskSize;
	unsigned char	GreenFieldPosition;
	unsigned char	BlueMaskSize;
	unsigned char	BlueFieldPosition;
	unsigned char	RsvdMaskSize;
	unsigned char	RsvdFieldPosition;
	unsigned char	DirectColorModeInfo;

	unsigned long	PhysBasePtr;
	unsigned long	OffScreenMemOffset;
	unsigned short	OffScreenMemSize;
	unsigned char	Reserved2[206];
} MIB;


#pragma aux cls = \
	"mov	ax,03h"	\
	"int	10h"	\
	modify [eax];

#pragma aux getkey =	\
	"xor	ax,ax"	\
	"int	16h"	\
	modify [eax];


// This is not the best way to check for presence of VGA, but it is just an
// example of how you can use the extended VGA BIOS functions without need
// to call DPMI.

#pragma aux detectvga =		\
	"sub	esp,40h"	\
	"mov	ah,1Bh"		\
	"xor	bx,bx"		\
	"mov	edi,esp"	\
	"int	10h"		\
	"cmp	al,1Bh"		\
	"mov	eax,1"		\
	"jnz	@@done"		\
	"mov	eax,0"		\
"@@done:"			\
	"add	esp,40h"	\
	modify [eax ebx edi]	\
	value [eax];


// Note that all the "...Ptr" real mode far pointers returned in VIB structure
// by real mode VESA VBE function 4F00h will be automatically converted by the
// DOS Extender to linear pointers relative to 0.
// All the pointers returned in MIB structure by real mode VESA VBE function
// 4F01h (WinFuncPtr for example) will be returned as real mode far pointers
// and it is up to the programmer to convert them to the protected mode values
// when and if needed.

// WARNING: VBEs ("Video BIOSes") on some graphics cards (for example Matrox
// Mystique/Millenium) will return the mode list in the "reserved" field of
// VIB structure. The DOS Extender is aware of this and will always return
// the correct pointer in the "VideoModePtr". However, if you later on move
// the VIB structure some place else, the pointer will be incorrect!!! To
// avoid this situation do not move the data in structure to other memory
// locations after you have called the extended VBE function 4F00h, ie
// don't "alloc memory on stack", "mov edi,esp and call 4F00h" and then
// "rep movs somewhere,ds:[edi]"!

#pragma aux detectvesa =	\
	"mov	ax,4F00h"	\
	"mov	edi,offset VIB"	\
	"int	10h"		\
	"cmp	ax,004Fh"	\
	"mov	eax,1"		\
	"jnz	@@done"		\
	"mov	eax,0"		\
"@@done:"			\
	modify [eax ecx esi edi]\
	value [eax];

#pragma aux getmodeinfo =	\
	"mov	ax,4F01h"	\
	"mov	edi,offset MIB"	\
	"int	10h"		\
	"cmp	ax,004Fh"	\
	"mov	eax,1"		\
	"jnz	@@done"		\
	"mov	eax,0"		\
"@@done:"			\
	modify [eax ecx esi edi]\
	value [eax]		\
	parm [ecx];



void ShowVI() {
	printf("Video BIOS Extension Information:\n");
	printf("=================================\n");

	printf("VBE Signature        :   %.4s\n",
		VIB.VbeSignature);

	printf("VBE Version          :   %d.%d\n",
		VIB.VbeVersion>>8,
		VIB.VbeVersion&0xFF);

	printf("OEM String           :   %s\n",
		VIB.OemStringPtr);

	printf("OEM Vendor Name      :   %s\n",
		VIB.OemVendorNamePtr);

	printf("OEM Product Name     :   %s\n",
		VIB.OemProductNamePtr);

	printf("OEM Product Revision :   %s\n",
		VIB.OemProductRevPtr);

	printf("Capabilities         :   0x%08X\n",
		VIB.Capabilities);

	printf("Mode List Ptr        :   0x%08X\n",
		VIB.VideoModePtr);

	printf("Total Memory         :   %4d KB\n",
		VIB.TotalMemory*64);
}




void ShowMI() {
	int n;
	int mode=0;

	printf("Video Mode Information:\n");
	printf("=======================\n");

	for(;;) {
		if((mode=(*VIB.VideoModePtr++))==0xFFFF) break;

		printf("Video Mode:  %04Xh  -  ",mode);

		if(getmodeinfo(mode)!=0) printf("invalid");
		else {
			if((MIB.ModeAttributes&0x0010)==0)
				printf("text     ");
			else	printf("graphics ");

			printf("%4d x ",MIB.XResolution);
			printf("%4d x ",MIB.YResolution);
			printf("%2d bits/pixel,  ",MIB.BitsPerPixel);
			printf("%d planes",MIB.NumberOfPlanes);

			if((MIB.ModeAttributes&0x0080)!=0)
				printf(",  linear");
		}
		printf("\n");
	}
}



void main() {

	if(detectvga()) {
		printf("VGA BIOS not detected!\n");
		exit(-1);
	}
	if(detectvesa()) {
		printf("VESA BIOS not detected!\n");
		exit(-1);
	}

	cls();
	ShowVI();

	printf("\n\nPress any key to view Video Mode Information...\n");
	getkey();

	cls();
	ShowMI();

	exit(0);
}
