/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2013 The Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  rsi stuff header file.
*
****************************************************************************/


#ifndef RSI1632_H_INCLUDED
#define RSI1632_H_INCLUDED

#include <string.h>
#include <dos.h>
#define DOS16LIB_INCLUDED
#include <dos16.h>
#include <dpmi.h>

#undef  RMCALL_INT
#define RMCALL_INT      0x18

#define NULL_PTR        ((void *)0)
#define NULL_SEL        0

#define USESDPMI()      (gdt_sel == NULL_SEL)

#define user_sel_const  0x80
#define user_sel_start  __d16_selectors.user

#define far_strcpy(d,s)     _fstrcpy( d, s )
#define far_setmem(p,l,w)   _fmemset( p, w, l )

#define GETLIMIT(g)     (((ULONG)(g).lim_16_19)<<16 | (g).lim_0_15)
#define GDT32LIMIT(g)   (g.xtype.page_gran ? (ULONG)GETLIMIT(g)<<12 | 0xFFF : (ULONG)GETLIMIT(g))
#define GDT32BASE(g)    makelong(((g).base_24_31 << 8)|(g).base_16_23,(g).base_0_15)

#define MEMBLK_INVALID  0
#define MEMBLK_PARTIAL  1
#define MEMBLK_VALID    2

#include "pushpck1.h"
typedef struct {
    OFFSET32 off;
    SELECTOR sel;
} Fptr32;

typedef struct _null_checks {
    long        prev_val;
    long        match_val;
    long        lower_bound;
    long        upper_bound;
    OFFSET32    off;
    SELECTOR    sel;
    char        rwx;
    char        len;
    char        status;
    char        amode;
    char        reg;
} _null_checks;

/*********************************************
 * A 32-bit Mode Switch Transfer Stack Frame.
 * Must match definition in tsf32exc.asm
 */
typedef struct tsf32_struct {
    USHORT          prev_tsf32;
    SELECTOR        ss;
    SELECTOR        ds;
    SELECTOR        es;
    SELECTOR        fs;
    SELECTOR        gs;
    ULONG           edi;
    ULONG           esi;
    ULONG           ebp;
    ULONG           esp;
    ULONG           ebx;
    ULONG           edx;
    ULONG           ecx;
    ULONG           eax;
    SHORT           int_id;
    LONG            int_code;
    ULONG           eip;
    SELECTOR        cs;
    USHORT          filler1;
    ULONG           eflags;
} TSF32;
#include "poppck.h"

extern int reset_intflag( void );
#pragma aux reset_intflag = \
    "pushf" \
    "cli" \
    "pop    ax" \
    "mov    al,ah" \
    "shr    ax,1" \
    "and    ax,1" \
    value [ax];

extern void set_intflag( int );
#pragma aux set_intflag = \
    "or     ax,ax" \
    "jz short L1" \
    "sti" \
    "jmp short L2" \
"L1: cli" \
"L2:" \
    parm [ax];

extern int is_validselector( SELECTOR sel );
#pragma aux is_validselector = \
    ".386p" \
    "or     ax,ax" \
    "jz short L1" \
    "lar    ax,ax" \
    "mov    ax,0" \
    "jne short L1" \
    "inc    ax" \
"L1:" \
    parm [ax] \
    value [ax];

extern int rsi_mem_strategy( int );
#pragma aux rsi_mem_strategy = \
    "mov    ax,0ffffh" \
    "mov    dh,0" \
    "int 21h" \
    parm [dl] \
    value [ax] \
    modify [dh];

extern int          rsi_rm_get_vector( int, void FarPtr FarPtr );
extern int          rsi_rm_set_vector( int, void FarPtr );
extern int          rsi_rm_far_call( void FarPtr, D16REGS FarPtr, D16REGS FarPtr );
extern int          rsi_int_passdown( int );
extern int          rsi_int_passup( int );
extern SELECTOR     rsi_sel_new_absolute( long, unsigned );
extern int          rsi_sel_free( SELECTOR );
extern SELECTOR     rsi_sel_data_alias( SELECTOR );
extern long         rsi_abs_address( void FarPtr );
extern int          rsi_rm_interrupt( int, D16REGS FarPtr, D16REGS FarPtr );
extern int          rsi_get_descriptor( SELECTOR, descriptor FarPtr );
extern int          rsi_set_descriptor( SELECTOR, descriptor FarPtr );
extern ACTION       *rsi_find_action( PACKAGE FarPtr, char FarPtr );
extern PACKAGE      FarPtr rsi_find_package( char FarPtr );
extern SELECTOR     rsi_seg_realloc( SELECTOR csel );
extern void         FarPtr rsi_get_rm_ptr( void FarPtr );
extern int          rsi_addr32_check( OFFSET32, SELECTOR, OFFSET32, OFFSET32 * );

/* dbglib.c */
extern void         D32DebugBreakOp( unsigned char * );
extern int          D32DebugInit( TSF32 FarPtr, int );
extern void         D32DebugTerm( void );
extern void         D32DebugRun( TSF32 FarPtr );
extern void         D32HookTimer( int );
extern void         D32UnHookTimer( void );
/* tsf32exc.asm */
extern void         __cdecl tsf32_exec( TSF32 FarPtr, TSF32 FarPtr );
/* fixtrap.asm */
extern int          init_fixtrap( void );
extern void         fixtrap( void );
/* rmhdlr.asm */
extern void         FAR16 rm15_handler( void );
extern void         FAR16 rm1B_handler( void );
extern void         FAR16 passup_hotkey( void );
/* d32dbgld.c */
extern void         D32SetCurrentObject( long cookie );
extern int          D32DebugLoad( const char *, char FarPtr, TSF32 FarPtr );
extern int          D32Relocate( Fptr32 FarPtr fp );
extern int          D32Unrelocate( Fptr32 FarPtr fp );
/* mem32.asm */
extern void         __cdecl peek32( OFFSET32, SELECTOR, void FarPtr, unsigned short );
extern int          __cdecl poke32( OFFSET32, SELECTOR, const void FarPtr, unsigned short );
/* d32dbgrd.c */
extern int          D32DebugRead( OFFSET32, SELECTOR, int, void FarPtr, unsigned short );
/* d32dbgwr.c */
extern int          D32DebugWrite( OFFSET32, SELECTOR, int, const void FarPtr, unsigned short );
/* d32dbgsb.c */
extern void         D32DebugSetBreak( OFFSET32, SELECTOR, int, const unsigned char FarPtr, unsigned char FarPtr );

// global variables
extern int          addr_mode;
extern char         page_fault;
extern _null_checks FarPtr nullp_checks;
extern void         FarPtr ( CDECL_FP16 D32NullPtrCheck )( int );
extern void         (FarPtr FAR16 hotkey_passup)();
extern void         (FarPtr FAR16 org15_handler)();

#endif
