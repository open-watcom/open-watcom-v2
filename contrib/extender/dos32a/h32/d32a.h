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

/*==========================================================================*/
/*      C/C++ Run-Time Library: D32A Library Header file                    */
/*==========================================================================*/

#ifndef __D32A__
#define __D32A__

#ifdef __cplusplus
extern "C" {
#endif


/********************************************************************
    The line below will provide WATCOM Linker with information on
    where the library corresponding to this include file is located.

    Modify the line below to match your WATCOM Library directory.
*********************************************************************/
#pragma library ("C:\WATCOM\LIB386\L32\d32a.lib");


#ifndef TRUE
#define TRUE    1
#endif

#ifndef FALSE
#define FALSE   0
#endif

#ifndef NULL
#define NULL    0
#endif


/*--------------------------------------------------------------------------*/
/*      DOS/32A Detection Routines                                          */
/*--------------------------------------------------------------------------*/

/* DETECT functions */
extern  int d32a_detect_extender(void);
extern  int d32a_detect_adpmi(void);
extern  int d32a_detect_system_software(void);
extern  int d32a_detect_windows(void);

/* returned by d32a_detect_system_software */
#define D32A_SYSTEM_CLEAN       0       /* INT 15h */
#define D32A_SYSTEM_XMS         1       /* XMS */
#define D32A_SYSTEM_VCPI        2       /* VCPI */
#define D32A_SYSTEM_DPMI        3       /* DPMI */
#define D32A_SYSTEM_UNKNOWN     -1      /* Unknown */

/* GET VERSION functions */
extern  int d32a_get_extender_version(void);
extern  int d32a_get_dpmi_version(void);
extern  int d32a_get_dos_version(void);
extern  int d32a_get_windows_version(void);
extern  int d32a_get_library_version(void);

/* CPU RELATED functions */
extern  int d32a_get_cpu_type(void);
extern  int d32a_get_fpu_type(void);
extern  int d32a_get_cpu_speed(void);
extern  char *d32a_get_cpu_id(void);

/* set by d32a_get_cpu_type */
extern  unsigned long _cpu_equipment;   /* returned in EDX by CPUID (1) */

#define D32A_CPUEQ_FPU  0x00000001      /* Floating Point Unit on-chip */
#define D32A_CPUEQ_VME  0x00000002      /* Virtual Mode Extension */
#define D32A_CPUEQ_DE   0x00000004      /* Debugging Extensions */
#define D32A_CPUEQ_PSE  0x00000008      /* Page Size Extension */
#define D32A_CPUEQ_TSC  0x00000010      /* Time Stamp Counter */
#define D32A_CPUEQ_MSR  0x00000020      /* Model Specific Registers */
#define D32A_CPUEQ_PAE  0x00000040      /* Physical Address Extension */
#define D32A_CPUEQ_MCE  0x00000080      /* Machine Check Exception */
#define D32A_CPUEQ_CX8  0x00000100      /* CMPXCHG8 Instruction Support */
#define D32A_CPUEQ_APIC 0x00000200      /* On-chip APIC Hardware Support */
#define D32A_CPUEQ_SEP  0x00000800      /* Fast System Call */
#define D32A_CPUEQ_MTRR 0x00001000      /* Memory Type Range Registers */
#define D32A_CPUEQ_PGE  0x00002000      /* Page Global Enable */
#define D32A_CPUEQ_MCA  0x00004000      /* Machine Check Architecture */
#define D32A_CPUEQ_CMOV 0x00008000      /* CMOVcc Instructions Support */
#define D32A_CPUEQ_PAT  0x00010000      /* Page Attribute Table */
#define D32A_CPUEQ_MMX  0x00800000      /* Intel MMX(TM) Technology Support */
#define D32A_CPUEQ_FXSR 0x01000000      /* Fast Floating Point Save/Restore */
#define D32A_CPUEQ_3DNOW 0x80000000     /* AMD K6-3 3DNOW! Instructions */



/*--------------------------------------------------------------------------*/
/*      DOS/32A Keyboard Routines                                           */
/*--------------------------------------------------------------------------*/
extern  int d32a_kbhit(void);
extern  void d32a_kbwait(void);
extern  unsigned char d32a_getkey(void);

extern  unsigned char _kbrd_asciicode;  /* ASCII code returned by getkey */
extern  unsigned char _kbrd_scancode;   /* SCAN code retuurned by getkey */



/*--------------------------------------------------------------------------*/
/*      DOS/32A Memory Routines                                             */
/*--------------------------------------------------------------------------*/

/* MALLOC functions */
extern  char *d32a_malloc(unsigned long);
extern  char *d32a_malloc_low(unsigned long);
extern  char *d32a_malloc_high(unsigned long);

/* FREE functions */
extern  int d32a_free(char *);
extern  void d32a_free_all(void);

/* MEMORY INFO functions */
extern  unsigned long d32a_getfreemem(void);
extern  unsigned long d32a_getfreemem_low(void);
extern  unsigned long d32a_getfreemem_high(void);
extern  unsigned long d32a_getfreemem_total(void);

typedef struct _D32A_MEMINFO {
        unsigned long LargestFreeBlock;
        unsigned long MaxUnlockedPages;
        unsigned long MaxLockedPages;
        unsigned long LinearSpaceInPages;
        unsigned long TotalNumOfUnlockedPages;
        unsigned long TotalNumOfFreePages;
        unsigned long TotalNumOfPhysPages;
        unsigned long FreeLinearSpaceInPages;
        unsigned long SizeOfPagingFile;
        unsigned long DPMI_Reserved[3];
} D32A_MEMINFO;

typedef struct _D32A_MEMBLOCKINFO {
        unsigned long BlockBase;        /* base address */
        unsigned long BlockSize;        /* size in bytes */
        unsigned long BlockHandle;      /* handle returned by DPMI */
        unsigned long BlockAttr;        /* attributes, used by library */
} D32A_MEMBLOCKINFO;

extern  void d32a_getfreemem_info(void *);
extern  int d32a_getmemblock_info(char *, void *);



/*--------------------------------------------------------------------------*/
/*      DOS/32A Compress/Decompres Data Routines                            */
/*--------------------------------------------------------------------------*/
extern  unsigned long d32a_encode_data(char *, char *, unsigned long);
extern  unsigned long d32a_decode_data(char *, char *, unsigned long);



/*--------------------------------------------------------------------------*/
/*      DOS/32A System Routines                                             */
/*--------------------------------------------------------------------------*/
struct  _D32A_DDREGS {
        unsigned long eax;
        unsigned long ebx;
        unsigned long ecx;
        unsigned long edx;
        unsigned long esi;
        unsigned long edi;
        unsigned long ebp;
        unsigned long eflags;
};

struct  _D32A_DWREGS {
        unsigned short ax; unsigned short _1;
        unsigned short bx; unsigned short _2;
        unsigned short cx; unsigned short _3;
        unsigned short dx; unsigned short _4;
        unsigned short si; unsigned short _5;
        unsigned short di; unsigned short _6;
        unsigned short bp; unsigned short _7;
        unsigned short flags;  unsigned short _8;
};

struct  _D32A_DBREGS {
        unsigned char al, ah; unsigned short _1;
        unsigned char bl, bh; unsigned short _2;
        unsigned char cl, ch; unsigned short _3;
        unsigned char dl, dh; unsigned short _4;
};

typedef union _D32A_REGS {
        struct _D32A_DDREGS d;
        struct _D32A_DWREGS w;
        struct _D32A_DBREGS b;
} D32A_REGS;

typedef struct _D32A_SREGS {
        unsigned short es;
        unsigned short cs;
        unsigned short ss;
        unsigned short ds;
        unsigned short fs;
        unsigned short gs;
} D32A_SREGS;

/* INTERRUPT functions */
extern  void d32a_int86rm(int, union _D32A_REGS *, union _D32A_REGS *);
extern  void d32a_int86pm(int, union _D32A_REGS *, union _D32A_REGS *);
extern  void d32a_int86xrm(int, union _D32A_REGS *, union _D32A_REGS *, struct _D32A_SREGS *);
extern  void d32a_int86xpm(int, union _D32A_REGS *, union _D32A_REGS *, struct _D32A_SREGS *);

#define d32a_int386rm   d32a_int86rm
#define d32a_int386pm   d32a_int86pm
#define d32a_int386xrm  d32a_int86xrm
#define d32a_int386xpm  d32a_int86xpm

/* SELECTOR functions */
extern  unsigned short d32a_alloc_selector(unsigned long, unsigned long, unsigned short);
extern  int d32a_free_selector(unsigned short);


#ifdef __cplusplus
};
#endif

#endif  // __D32A__
