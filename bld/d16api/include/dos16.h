/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  DOS/16M API and library interface.
*
****************************************************************************/


#ifndef DOS16_INCLUDED
#define DOS16_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <dosgen.h>

#define makeptr(s, o)   ((void FP16)((long)(s) << 16 | (unsigned)(o)))
#define makelong(h, l)  (((long)(h) << 16) | (l))

typedef union {
    CHAR FP16 p;
    void FP16 pv;
    struct {
        OFFSET16  off;
        SELECTOR  sel;
        } w;
    LONG    l;
} FPTR16;

/* Standard selectors */
extern struct {
    SHORT       d16ptr_ver;
    SELECTOR    gdt;
    SELECTOR    idt;
    SELECTOR    kernel;
    SELECTOR    kerneld;
    SELECTOR    psp;
    SELECTOR    env;
    SELECTOR    mem0;
    SELECTOR    biosdata;
    SELECTOR    rsvd0;
    SELECTOR    gdti;
    SELECTOR    rsvd1[3];
    SELECTOR    kernelx;
    SELECTOR    rsvd2;
    SELECTOR    user;
} __d16_selectors;

/* Bits in gdti */
#define gdti_locked     0x80
#define gdti_reside     0x40
#define gdti_special    0x20
#define gdti_alias      0x10
#define gdti_block      0x02
#define gdti_owner      0x01

typedef struct _intvect16 {
    OFFSET16 off;
    SELECTOR sel;
} INTVECT;

typedef struct _gdt {
    USHORT gdtlen;
    USHORT gdtaddr;
    UCHAR  gdtaddr_hi;
    UCHAR  gdtaccess;
    USHORT gdtreserved;
} GDT;

typedef struct _idt16 {
    USHORT idtoffset;
    USHORT idtsel;
    UCHAR  idtun1;
    UCHAR  idtaccess;
    USHORT idtreserved;
} IDT;

typedef struct tsf16_struct     TSF;

/* Compatibility types for 16-bit DOS/16M code */
#if defined( __I86__ ) || defined( _M_I86 )
typedef FPTR16                  FPTR;
typedef TSF NearPtr             TSFPREV;
#else
typedef USHORT                  TSFPREV;
#endif

struct tsf16_struct {
    TSFPREV prev_tsf;
    USHORT  ss, ds, es;
    USHORT  di, si, bp, sp;
    USHORT  bx, dx, cx, ax;
    SHORT   int_id, int_code;
    USHORT  ip, cs, flags;
};

typedef struct _d16regs {
    USHORT ds, es;
    USHORT di, si, bp, sp;
    USHORT bx, dx, cx, ax;
} D16REGS;

#define D16_PACKAGES_DEFINED    /* Avoid package.h inclusion */

typedef unsigned long   ACTION_RETURN;
typedef ACTION_RETURN   CDECL_FAR16 ACTION();

typedef struct action_pack {
    char FP16   action_name;
    ACTION      *action;
} ACTION_PACK;

typedef struct package {
    struct package FP16 next_package;
    char FP16           package_title;
    UCHAR               major_rev;
    UCHAR               minor_rev;
    int                 system_action_count;
    ACTION_PACK FP16    system_action;
    int                 user_action_count;
    ACTION_PACK FP16    user_action;
} PACKAGE;

#define PackageNext( pkg )      pkg->next_package
#define PackageTitle( pkg )     pkg->package_title
#define PackageRev( pkg )       (pkg->major_rev * 100 + pkg->minor_rev)
#define PackageMajRev( pkg )    pkg->major_rev
#define PackageMinRev( pkg )    pkg->minor_rev

typedef void FarPtr FDORNAME;
#define make_fdorname(h, l)  ((FDORNAME)(((long)(h) << 16) | (l)))

#define RUN_PKGS_ONLY       1
#define RUN_ALL_EXPS        0

#define MAX_PROCESS_PER_PGM 16  /* App and packages in spliced EXE */

#define LI_PGMFLAG_NORMAL   0x0000
#define LI_PGMFLAG_PACKAGE  0x0002

/* Program load information */
typedef struct _pgm_load_info {
    UCHAR       type_or_error;
    UCHAR       overload;
    SELECTOR    first_sel;
    SELECTOR    last_sel;
    SELECTOR    init_cs;
    USHORT      init_ip;
    USHORT      pgm_flags;
    int         fd;
    long        next;
    char        package_path[65];
    USHORT      process_first_sel[MAX_PROCESS_PER_PGM];
    USHORT      process_last_sel[MAX_PROCESS_PER_PGM];
    USHORT      process_level;
} PGM_LOAD_INFO;

#define CarryFlag   0x1         /* Carry flag bit */

typedef /* _Packed */ struct {
    SHORT      version;         /* Version times 100 */
    USHORT     passdn0;         /* Offset of first pass-down in kernel seg */
    USHORT     passdn255;       /* Offset of last pass-down */
    FPTR16     PassupIntP;      /* pointer to pass-up interrupt index */
    FPTR16     xfer_buff;       /* Transfer buffer pointer */
    USHORT     xfer_bsize;      /* Transfer buffer size */
    USHORT     delay_286;       /* 286 switch delay */
    USHORT     delay_386;       /* 386 switch delay */
    SHORT      switch_control;  /* Switch flags - see below */
    CHAR FP16  (CDECL_FP16 D16MStkP)();    /* D16MoveStack routine */
    USHORT     (CDECL_FP16 D16ToRMP)();    /* D16ToReal routine */
    SHORT      (CDECL_FP16 D16ToPMP)();    /* D16ToProtected (real ptr) */
    SHORT      (CDECL_FP16 D16rmInP)(int, D16REGS FP16, D16REGS FP16);
                                            /* D16rmInterrupt routine */
    USHORT     bannerp;         /* Banner display flag */
    SELECTOR   D16rdata;        /* Real mode data segment */
    CHAR FP16  versionp;        /* Version string pointer */
    UCHAR      swmode;          /* Switch method */
    UCHAR      cpumod;          /* Processor type (2 for 286, 3 for 386) */
    USHORT     copyright;       /* Offset of copyright string */
    USHORT     dbginfop;
    UCHAR      D16_intno;       /* Interrupt for itnernal calls */
    UCHAR      A20;
    UCHAR      A20_rigor;
    UCHAR      A20_orig;
    USHORT     last_delseg;     /* Last freed segment for fault suppression */
    TSF FP16 FP16 tsp_ptr;      /* Address of TSF pointer */
    UCHAR      has_87;          /* Not emulating FPU (real x87 present) */
    UCHAR      slow_A20;        /* Unsynchronized A20 resets */
    USHORT     MSW_bits;        /* MSW bits to set in protected mode */
    void FP16  package_info_p;  /* Packages information */
    UCHAR      machine_type;
    UCHAR      miscellaneous;   /* See below */
    USHORT     xbufadj;         /* Transfer buffer size adjustment */
    USHORT     master_8259;     /* PIC (8259A) and PIT (8254) information */
    USHORT     slave_8259;
    USHORT     portstep_8259;
    UCHAR      m8259_int;
    UCHAR      s8259_int;
    USHORT     base_8254;
    USHORT     portstep_8254;
    LONG       means_unused;    /* Unused interrupt vector contents */
} D16INFO;

/* D16INFO.miscellaneous bits */
#define D16misc_AT_compat   0x01    /* IBM PC/AT compatible system */
#define D16misc_extra_mem   0x02    /* Extra memory just under 16MB used */
#define D16misc_pluto       0x04    /* Plutonium kernel */
#define D16misc_DVX         0x80    /* DesqView/X */

/* D16INFO.switch_control bits */
#define SwCtrl_A20          0x01    /* Disable A20 on 386s in real mode */
#define NoVCPI              0x02    /* Disable VCPI support */
#define BIOS16_inPM         0x04    /* Do not pass down int 16h fn 1 */
#define EnOverload          0x08    /* Enable overloading */
#define Rest_Int            0x10    /* Restore modified interrupts */
#define MemInit00           0x20    /* Initialize allocated memory to 0x00 */
#define MemInitFF           0x40    /* Initialize allocated memory to 0xFF */
#define RotateSelector      0x80    /* Delay reuse of freed selectors */


/* Pass-up interrupt registers */
typedef struct {
    USHORT  pu_ss;
    USHORT  pu_es;
    USHORT  pu_ds;
    USHORT  pu_rsvd;
    USHORT  pu_ip;
    USHORT  pu_cs;
    USHORT  pu_flags;
} PUF;

#ifdef KERNEL
    extern D16INFO  _d16info;
#else
    extern D16INFO  _far _d16info;
#endif

#define D16MoveStack(new_sp, context)       (*_d16info.D16MStkP)(new_sp, context)
#define D16ToReal()                         (*_d16info.D16ToRMP)()
#define D16ToProtected(pcs, pss, pds, pes)  (*_d16info.D16ToPMP)(pcs, pss, pds, pes)
#ifdef KERNEL
#define D16rmInterrupt(intno, inrs, outrs)  (*_d16info.D16rmInP)(intno, inrs, outrs)
#endif

#define RMCALL_INT      0x20    /* Interrupt used by rmRCall */
#define MAX_PASSUP_INDEX   8    /* Number of supported pass-up interrupts */
#define PASSUP_INDEX_INC   4    /* Size of pass-up entry */

#ifdef __cplusplus
}
#endif

#endif  /* DOS16_INCLUDED */

#ifndef DOS16LIB_INCLUDED
#define DOS16LIB_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#undef  FP_SEG
#undef  FP_OFF
#define FP_SEG(p)   (*((USHORT FP16) &(p) + 1))
#define FP_OFF(p)   (*(USHORT FP16) &(p))

#define NULL_PTR    ((void *)0)
#define NULL_SEGREG 0

#define acc_ref_bit 1   /* Descriptor referenced bit */

#define RSIAPI      __cdecl __loadds __far
#define RSIRTN      __cdecl __far

#ifdef KERNEL
extern int __cdecl _intflag( int );
#else
extern int __cdecl __far _intflag( int );
#endif

extern  int RSIRTN __DOSX( unsigned, D16REGS FarPtr, unsigned, unsigned );
extern  int RSIRTN __INT31X( unsigned, unsigned, D16REGS FarPtr, unsigned, unsigned );
extern  int RSIRTN __DOS( unsigned, D16REGS FarPtr );
extern  int RSIRTN __INT31( unsigned, unsigned, D16REGS FarPtr );
extern  int RSIRTN _is_pm( void );
extern  int RSIRTN chainint( void (FarPtr)( void ) );
extern  int RSIRTN D16Memcopy( void FarPtr, void FarPtr, unsigned short );
extern  int RSIRTN D16Memset( void FarPtr, unsigned short, unsigned char );

extern  int RSIAPI D16GetAccess( void FarPtr );
extern  int RSIAPI D16SetAccess( void FarPtr, int );
extern  int RSIAPI D16SegProtect( void FarPtr, int );
extern  void FarPtr RSIAPI D16SegDSAlias( void (FarPtr)() );
extern  void (FarPtr RSIAPI D16SegCSAlias( void FarPtr ))();
extern  int RSIAPI D16CallDebug( char FarPtr );
extern  int RSIAPI D16SegCancel( void FarPtr );
extern  int RSIAPI D16GetROMCheksum( unsigned long FarPtr );
extern  int RSIAPI D16GetDelay( unsigned int FarPtr );
extern  int RSIAPI D16SetDelay( unsigned int );
extern  int RSIAPI D16GetDesc( unsigned int, struct _gdt FarPtr );
extern  int RSIAPI D16SetDesc( unsigned int, struct _gdt FarPtr );
extern  unsigned int RSIAPI D16SegLimit( void FarPtr );
extern  int RSIAPI D16rmGetVector( unsigned int, INTVECT FarPtr );
extern  int RSIAPI D16pmGetVector( unsigned int, INTVECT FarPtr );
extern  void HugePtr RSIAPI huge_alloc( unsigned long, unsigned int );
extern  void HugePtr RSIAPI D16HugeAlloc( long );
extern  long RSIAPI D16HugeResize( void FarPtr, long );
extern  void HugePtr RSIAPI D16SelReserve( int );
extern  int RSIAPI D16isDOS16M( void );
extern  int RSIAPI D16isDPMI( void );
extern  int RSIAPI D16MemStrategy( int );
extern  void FarPtr RSIAPI D16MemAlloc( unsigned int );
extern  int RSIAPI D16MemFree( void FarPtr );
extern  long RSIAPI D16ExtAvail( void );
extern  long RSIAPI D16LowAvail( void );
extern  void FarPtr RSIAPI D16SegRealloc( void FarPtr );
extern  void FarPtr RSIAPI D16SegTransparent( unsigned int, unsigned int );
extern  void FarPtr RSIAPI D16SegDataPtr( void FarPtr, unsigned int );
extern  int RSIAPI D16SegResize( void FarPtr, unsigned int );
extern  int RSIAPI D16IntStrategy( int );
extern  int RSIAPI D16Passdown( int );
extern  int RSIAPI D16Passup( int );
extern  int RSIAPI D16pmInterrupt( int, struct _d16regs FarPtr, struct _d16regs FarPtr );
extern  unsigned long RSIAPI D16AbsAddress( void FarPtr );
extern  void FarPtr RSIAPI D16SegAbsolute( long, unsigned int );
extern  void FarPtr RSIAPI D16ProtectedPtr( void FarPtr, unsigned int );
extern  long RSIAPI D16ExtReserve( long );
extern  void FarPtr RSIAPI D16RealPtr( void FarPtr );
extern  int RSIAPI D16rmRCall( unsigned int, unsigned int, struct _d16regs FarPtr, struct _d16regs FarPtr );
extern  unsigned int RSIAPI D16NewSelector( void );
extern  int RSIAPI D16FreeSelector( unsigned int );
extern  int RSIAPI D16rmInstall( unsigned int, unsigned int, unsigned int, INTVECT FarPtr );
extern  int RSIAPI D16pmInstall( unsigned int, unsigned int, unsigned int, INTVECT FarPtr );
extern  int RSIAPI D16pmSetVector( int, char FarPtr, char FarPtr);
extern  int RSIAPI D16rmSetVector( int, char FarPtr, char FarPtr );
extern  void RSIAPI D16TermFunction( void (__loadds __cdecl FarPtr)() );
extern  int RSIAPI D16Lock( void FarPtr );
extern  int RSIAPI D16UnLock( void FarPtr );
extern  int RSIAPI D16Reside( void FarPtr );
extern  int RSIAPI D16UnReside( void FarPtr );
extern  long RSIAPI D16PhysAddress( long );
extern  int RSIAPI D16GetSelIncr( void );

extern  int RSIAPI D16ProgramLoad( FDORNAME, long, TSF FarPtr );
extern  int RSIAPI D16ProgramLoadContinue( TSF FarPtr );
extern  int RSIAPI D16ProgramExec( struct tsf_struct FarPtr, char FarPtr, int );
extern void RSIAPI D16ProgramDispatch( TSF FarPtr );
extern  int RSIAPI D16ProgramUnload( void );
extern void RSIAPI D16GetLoadInfo( PGM_LOAD_INFO FarPtr (FarPtr) );
extern void RSIAPI D16SetLoadInfo( PGM_LOAD_INFO FarPtr );

extern  int RSIAPI D16ExpLoad( int, TSF FarPtr );
extern  int RSIAPI D16ExpLoadP( int, TSF FarPtr, long );
extern  int RSIAPI D16Overload( int, TSF FarPtr );
extern  int RSIAPI D16Execute( TSF FarPtr, char FarPtr );


extern void FarPtr RSIAPI D16FindAction( PACKAGE FarPtr, char FarPtr );
extern PACKAGE FarPtr RSIAPI D16FindPackage( char FarPtr );
extern  int RSIAPI D16PackageUnregister( PACKAGE FarPtr );
extern void RSIAPI D16PackageRegister( PACKAGE FarPtr );
extern void RSIAPI D16PackageTsr( int );

#ifndef KERNEL
extern  int RSIAPI D16rmInterrupt( int, D16REGS FarPtr, D16REGS FarPtr );
#endif

#ifdef __cplusplus
}
#endif

#endif  /* DOS16LIB_INCLUDED */
