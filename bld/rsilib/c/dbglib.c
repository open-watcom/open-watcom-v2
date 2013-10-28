/****************************************************************************
*
*                            Open Watcom Project
*
*    Copyright (c) 2011 Open Watcom Contributors. All Rights Reserved.
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*    Copyright (c) 1987-91, 1992 Rational Systems, Incorporated. All Rights Reserved.
*
* =========================================================================
*
* Description:  DOS/4G debugging library.
*
****************************************************************************/

#include <ctype.h>
#include <conio.h>
#include "rsi1632.h"
#include "loader.h"

/* DOS4G Entry points */
#define DOS4G_KERNEL_PACKAGE_NAME       "D32_KERNEL"
#define DOS4G_DEBUG_HOOK_ENTRY          "D32DebugHook"
#define DOS4G_NULLP_ENTRY               "D32NullPtrCheck"
#define DOS4G_INTCHAIN_ENTRY            "D32ChainInterrupt"

#define OUTDATEDWINDOWSCRAP 0
#define DEBUGHOTKEY         0

/*----------------- DEFINITIONS OF INTERRUPTS AND EXCEPTIONS --------------*/

/*
 *  What the exceptions mean
 *
 *      #       Description                     
 *      ----------------------------------------
 *      0x00    Divide by Zero                  
 *      0x01    Debugging Exceptions ( step )   
 *      0x03    Breakpoint                      
 *      0x06    Undefined Opcode                
 *      0x0A    Invalid Task State              
 *      0x0C    Stack Fault                     
 *      0x0D    General Protection Fault        
 *      0x0E    Page Fault                      
 *
 *  Since these are built into the chip, they ought to be the same regardless
 *  of the specific computer.
 */

#define EXC_DIVIDE_BY_ZERO                  0
#define EXC_SINGLE_STEP                     0x1
#define EXC_BREAKPOINT                      0x3
#define EXC_UNDEF_OPCODE                    0x6
#define EXC_INVALID_TASK_STATE              0xA
#define EXC_STACK_FAULT                     0xC
#define EXC_GP_FAULT                        0xD
#define EXC_PAGE_FAULT                      0xE

/*  Real mode interrupts used
 *
 *      #       Description       
 *      --------------------------
 *      0x02    NMI ( Breakout )  
 *      0x05    Print-Screen key  
 *      0x15    Alt-SysRq key     
 *      0x1B    Ctrl-Break key    
 *      0x21    DOS Funtions      
 *      0x23    Ctrl-C key        
 *
 */

#define INT_SINGLE_STEP                     0x01
#define INT_NMI                             0x02
#define INT_BREAKPOINT                      0x03
#define INT_PRT_SCRN_KEY                    0x05
#define INT_TIMER                           0x08
#define INT_SYS_SERVICES                    0x15
#define INT_CTRL_BREAK_KEY                  0x1B
#define INT_DOS                             0x21
#define INT_CTRL_C_KEY                      0x23

#define INT_RM_PASSUP                       INT_CTRL_BREAK_KEY      

/*------------------------ CODE ---------------------------*/

#define far_setmem( p, l, w )        _fmemset( p, w, l )
#define far_strcpy( d, s )           _fstrcpy( d, s )
#define TIMER
#define TIMER0          0x40    /* timer port */
#define INT_CTRL        0x20    /* interrupt controler port */
#define EOI             0x20    /* End-Of-Interrupt */

extern void beep( void );
#pragma aux beep = \
    "mov    ax,0e07h" \
    "mov    bx,0" \
    "int 10h" \
    modify [ax bx]

extern int _check_parrent_debugger( void );
#pragma aux _check_parrent_debugger = \
    "mov    ax,0FF00h" \
    "mov    dx,11FFh" \
    "int 21h" \
    "sbb    ax,ax" \
    value [ax] modify [dx]

//extern char vmm_present;      /* Set if VMM 4G package is present */

void FarPtr  ( CDECL_FP16 D32NullPtrCheck )( int ) = NULL_PTR;

int             addr_mode = 0;          /* 1 if last address expression absolute, 2 if real */
char            page_fault = 0;
_null_checks    FarPtr nullp_checks = NULL_PTR;

static TSF32 dbgregs;           /* Execution context of debugged program */
static TSF32 debugger_tsf;      /* Execution context for debugger */
static TSF32 debug_handler_tsf; /* Execution context for kernel fault handler */

static void ( CDECL_FP16 debug_hook )( unsigned, unsigned, void FarPtr ) = NULL_PTR;
static void ( CDECL_FP16 chain_interrupt )( unsigned, TSF32 FarPtr ) = NULL_PTR;

static char timer_mult = 0;     /* Tells timer handler to take charge */
static char timer_mod = 0;
static char debugging = 0;      /* Tells crash handler to take charge */
static char being_debugged = 0; /* Another D present? */

static int  first_time = 1;
static unsigned char saved_byte;

/*
 *  This is the list of exceptions which 4gw hooks when it starts up.
 *  Since they depend only on the chip, they can be hooked on any machine.
 */
static int hook_exceptions[] = {
    EXC_DIVIDE_BY_ZERO,
    EXC_SINGLE_STEP,
    EXC_BREAKPOINT,
    EXC_UNDEF_OPCODE,
    EXC_INVALID_TASK_STATE,
    EXC_STACK_FAULT,
    EXC_GP_FAULT,
    EXC_PAGE_FAULT
};
// static int hook_exceptions[] = { 0, 1, 3, 6, 0x0A, 0x0C, 0x0D, 0x0E };

/*
 *  These are the interrupts that get hooked by 4gw, independantly from the
 *  break-type interrupts ( break, ctrl-c, prt-scrn, sys-rq ).
 */

static int hook_interrupts[] = { 1, 2, 3, 0x21 };

#if DEBUGHOTKEY

extern void outc( char );
#pragma aux outc = \
    "mov    ah,0eh" \
    "mov    bx,0" \
    "int 10h" \
    parm [al] modify [ax bx]

void outs( char *s )
{
    char c;
    while( c = *s++ ) {
        outc( c );
    }
}

char hdigit( unsigned d )
{
    if( d > 9 )
        return( ( d - 10 ) + 'a' );
    return( d + '0' );
}

void outi( unsigned i )
{
    static char foo[5];

    outs( " " );
    foo[4] = 0;
    foo[3] = hdigit( i & 0xF );
    i >>= 4;
    foo[2] = hdigit( i & 0xF );
    i >>= 4;
    foo[1] = hdigit( i & 0xF );
    i >>= 4;
    foo[0] = hdigit( i & 0xF );
    outs( foo );
}
#else
#define outs(s)
#define outi(i)
#endif

static TSF32 FarPtr find_user_code( TSF32 FarPtr client )
{
    TSF32 FarPtr curr_tsf = client;

    /*  The first TSF on the chain will always be in the DOS/16M
        passup code, for the hotkey interrupt.  We will break there
        if we couldn't find anywhere else to break, but the preferred
        location is in a user code segment because the passup code
        is not meaningful to the user.

        We start walking the TSF chain immediately after the
        passup TSF; if we find a user code segment on the chain,
        we update the break address accordingly.
    */
    while( curr_tsf->prev_tsf32 != NULL_PTR ) {
        curr_tsf = makeptr( FP_SEG( curr_tsf ), curr_tsf->prev_tsf32 );
#if DEBUGHOTKEY
        outs( "TSF at" ); outi( FP_OFF( curr_tsf ) ); outs( " cs:ip=" );
        outi( curr_tsf->cs ); outi( (int)curr_tsf->eip ); outs( "\n\r" );
        outs( "user sel" ); outi( user_sel ); outs( "\n\r" );
#endif
        /*  Break in any code segment above primary DOS/4G code segment
            (DOS/4G and DOS/16M guts may not all be reentrant).  The
            debug_hook function is conveniently exported from that
            segment.
        */
        if( curr_tsf->cs > FP_SEG( debug_hook ) ) {
            client = curr_tsf;
            break;
        }
    }
    return( client );
}

static unsigned char hotkey_opcode;
static unsigned char hotkey_hit;
static int  hotkey_int = INT_RM_PASSUP;

static void set_hotkey_break( TSF32 FarPtr client )
{
    client = find_user_code( client );
    D32DebugBreakOp( &hotkey_opcode );
    D32DebugSetBreak( client->eip, client->cs, 0, &hotkey_opcode, &saved_byte );
    hotkey_hit = 1;
#ifdef DEBUGHOTKEY
    outs( "hotkey break set\r\n" );
#endif
}


static void check_hotkey( int eip_mod, TSF32 FarPtr client )
{
    if( hotkey_hit ) {
        hotkey_hit = 0;
#ifdef DEBUGHOTKEY
        outs( "hotkey seen " );
        outi( FP_OFF( client ) ); 
        outi( client->cs ); outi( (int)client->eip ); outs( "\r\n" );
#endif
        D32DebugSetBreak( client->eip + eip_mod, client->cs, 0, &saved_byte, &hotkey_opcode );
        client->int_id = hotkey_int; /* Attribute to Hotkey */
    }
}

static int fix_fpe_fault( unsigned short opcodew, TSF32 FarPtr client )
{
    static unsigned char imm8, val8;
    static unsigned short imm16;

    switch( opcodew ) {
    case 0x04C7 :           /* mov ds:[si], imm16 */
        peek32( client->eip + 2, client->cs, &imm16, 2 );
        poke32( client->esi, client->ds, &imm16, 2 );
        client->eip += 4;
        break;
    case 0x0429 :            /* sub ds:[si], ax */
        peek32( client->esi, client->ds, &imm16, 2 );
        imm16 -= (int)client->eax;
        poke32( client->esi, client->ds, &imm16, 2 );
        client->eip += 2;
        break;
    case 0x04C6 :       /*  mov ds:[si], imm8 */
        peek32( client->eip + 2, client->cs, &imm8, 1 );
        poke32( client->esi, client->ds, &imm8, 1 );
        client->eip += 3;
        break;
    case 0x0C80 :       /* or  byte ptr[si], C0 */
        peek32( client->esi, client->ds, &val8, 1 );
        peek32( client->eip + 2, client->cs, &imm8, 1 );
        val8 |= imm8;
        poke32( client->esi, client->ds, &val8, 1 );
        client->eip += 3;
        break;
    case 0x0489 :       /* mov ds:[si], ax */
        imm16 = (int)client->eax;
        poke32( client->esi, client->ds, &imm16, 2 );
        client->eip += 2;
        break;
    default :
        return( 0 );
        break;
    }
    return( 1 );
}

static int fixcrash( TSF32 FarPtr client )
{
    static unsigned char opcode;
    static unsigned short opcodew;
    static int zero = 0;

    peek32( client->eip, client->cs, &opcodew, 2 );
    opcode = (unsigned char)( opcodew & 0xFF );

    /* We zero the word on the stack because we will return to the
            POP instruction and try again.  We also zero the offending
            register, because if we take a fault while we're in the middle
            of ihandle, ihandle itself will pop the register and cause
            a recursive crash loop.
    */
    if( opcode == 0x07 ) {      /* POP ES */
        client->es = 0;
coverup:
        poke32( client->esp, client->ss, &zero, 2 );
        return( 1 );
    }
    if( opcode == 0x1F ) {      /* POP DS */
        client->ds = 0;
        goto coverup;
    }
    if( opcodew == 0xA10F ) {   /* POP FS */
        client->fs = 0;
        goto coverup;
    }
    if( opcodew == 0xA90F ) {   /* POP GS */
        client->gs = 0;
        goto coverup;
    }

    /* Attempt to fix up the Microsoft floating point emulator,
            which overstores FWAIT instructions with "mov ax,ax"
            using the instruction mov ds:[si],C089h and does lots of
            other sneaky tricks
    */
    if( fix_fpe_fault( opcodew, client ) ) {
        return( 1 );
    }
#if 0
    /* Attempt to fix up references to Phar Lap selector 0x34
    */
    int fixed = 0;
    if( client->es == 0x34 ) client->es = client->ds, fixed++;
    if( client->fs == 0x34 ) client->fs = client->ds, fixed++;
    if( client->gs == 0x34 ) client->gs = client->ds, fixed++;
    if( fixed ) return( 1 );
#endif
    return( 0 );
}

/* Step interrupt handled as is */
/* Breakpoint requires decrementing ip */
/* INT 21 exit function 4c: catch and back ip up 2, set int_id = -3 */
/* NOTE: this handler may be called with SS != DS */

unsigned __loadds __saveregs __cdecl __far debug_handler( unsigned int hNext, TSF32 FarPtr client )
{
    int     eip_mod = 0;
    char    in_debuggee;

    __asm   cld;        /* CLD to be safe */

    in_debuggee = debugging;
#ifdef TIMER
    if( client->int_id == INT_TIMER ) {
        if( --timer_mod == 0 ) {
            timer_mod = timer_mult;
            chain_interrupt( hNext, client );
        } else {
            outp( INT_CTRL, EOI );
        }
        hNext = 0;
        if( in_debuggee && find_user_code( client ) != client ) {
            in_debuggee = 0;
        }
    }
#endif

    if( in_debuggee == 0 ) {
        /* Handler should get out of the way */
        if( ( client->int_id == hotkey_int ) && !being_debugged ) {
            /* If we detect the hotkey while the debugger is in control, and
                    there is no other debugger to hotkey into, just beep.
            */
#if DEBUGHOTKEY
            outi( debugging ); 
            outs( " think we're in the debugger\r\n" );
#endif
            beep();        /* Can't hotkey, already in control */
            hNext = 0;

        } else if( client->int_id == EXC_PAGE_FAULT ) {
            /* If a page fault in debugger, assume triggered by peek32/poke32
                    and just cover it up, setting the page_fault flag.
            */
            page_fault = 1;
            client->eip += 3;
            hNext = 0;      /* Fault has been handled */
        } else if( ( client->int_id == EXC_SINGLE_STEP ) && !being_debugged ) {
            /* If a debug exception occurs while the debugger is active, it's
                    because something (such as DOS/4G with NULLP) is debugging the
                    debugger.  Because the debugger must juggle the real mode
                    interrupt vectors, we want to cover up run-of-the-mill exceptions
                    caused by NULLP.
    
                    However, if there is a parent debugger present to handle the
                    exception, we don't want to cover it up.
            */
            hNext = 0;
        }
        return( hNext );    /* Execute next in chain */
    }

#if OUTDATEDWINDOWSCRAP
analyze:
#endif
    outs( "got int " ); outi( client->int_id ); outs( "\r\n" );
    switch( client->int_id ) {
    case EXC_SINGLE_STEP:   /* Single step */
#if OUTDATEDWINDOWSCRAP
        /* NEEDWORK */
        /* If the trace bit is not turned on, assume illegal OP code */
        if( _d16info.swmode == 0 && (client->eflags & 0x100) == 0 ) {
            /* Running under Windows 3.00 and took illegal opcode */
            client->int_id = 6; /* Attribute this problem properly  */
            eip_mod = -1;       /* And back up an instruction */
            goto analyze;
        }
#endif
        check_hotkey( eip_mod, client );
        break;
    case EXC_GP_FAULT:      /* GP Fault */
        if( fixcrash( client ) )
            return( 0 );
        break;
    case EXC_PAGE_FAULT:      /* Page fault */
        /* If we get a page fault while the user program is running,
                we want to flag it immediately.  If VM is present, it will
                already have covered up any of its own page faults; the
                debug handler will only be called on a legitimate fault.
        */
        break;
    case EXC_BREAKPOINT:         /* Breakpoint */
        eip_mod = -1;
        check_hotkey( eip_mod, client );
        break;
    case EXC_DIVIDE_BY_ZERO:        /* Divide by zero */
    case INT_NMI:                   /* NMI */
    case EXC_UNDEF_OPCODE:          /* Illegal opcode */
    case EXC_INVALID_TASK_STATE:    /* Invalid task state segment */
    case EXC_STACK_FAULT:           /* stack fault */
        break;
#ifdef TIMER
    case INT_TIMER:             /* timer */
        break;
#endif
    case INT_DOS:
        /* Catch INT 21h/AH = 4Ch (terminate program) */
        if( ((unsigned short)client->eax & 0xFF00) == 0x4c00 ) {
            eip_mod = -2;
            break;
        }
        /* Catch INT 21h/AH = FFh
                    DX = 11FFh (DOS/16M debugger presence test)
        */
        if( ((unsigned short)client->eax & 0xFF00) == 0xFF00 &&
                (unsigned short)client->edx == 0x11FF ) {
            client->eflags &= ~1;   /* Clear carry flag */
            return( 0 );
        }
        return( hNext );
    case INT_PRT_SCRN_KEY:
    case INT_RM_PASSUP:             /* Possible hotkey interrupts */
    case INT_CTRL_C_KEY:
        if( client->int_id == hotkey_int ) {
            set_hotkey_break( client );
            return( 0 );
        }
        break;
//        return( hNext );
    default:
        return( hNext );
    }
    dbgregs = *client;
    dbgregs.eip += eip_mod;     /* Point to "correct" instruction */
    tsf32_exec( &debug_handler_tsf, &debugger_tsf );
    *client = dbgregs;
    return( 0 );
}

static USHORT my_package_bind( char *package, char *action, PACKAGE FarPtr *bound_package, ACTION **bound_action )
{
    ACTION      *a;

    if( *bound_package == NULL_PTR ) {
        *bound_package = rsi_find_package( package );
    }
    if( *bound_package == NULL_PTR )     /* If no package must be error */
        return( 1 );
    if( bound_action == NULL_PTR )
        return( 0 );        /* No action requested */
    a = rsi_find_action( *bound_package, action );
    if( a == NULL_PTR )     /* Couldn't find action */
        return( 1 );
    *bound_action = a;
    return( 0 );
}

/* Returns 0 if successful
*/
static int hook_debug_interrupts( void )
{
    int         i;
    PACKAGE FarPtr package = NULL;

    if( my_package_bind( DOS4G_KERNEL_PACKAGE_NAME, DOS4G_DEBUG_HOOK_ENTRY, &package, (ACTION **)&debug_hook ) )
        return( 1 );

    if( my_package_bind( DOS4G_KERNEL_PACKAGE_NAME, DOS4G_INTCHAIN_ENTRY, &package, (ACTION **)&chain_interrupt ) )
        return( 2 );

    /* OK to not find this entry point
    */
    my_package_bind( DOS4G_KERNEL_PACKAGE_NAME, DOS4G_NULLP_ENTRY, &package, (ACTION **)&D32NullPtrCheck );

    if( D32NullPtrCheck != NULL_PTR )           /* Disable NULLP checking for now */
        nullp_checks = D32NullPtrCheck( 0 );

    for( i = 0; i < sizeof( hook_exceptions ) / sizeof( hook_exceptions[0] ); i++ )
        debug_hook( hook_exceptions[i], 1, debug_handler );
    for( i = 0; i < sizeof( hook_interrupts ) / sizeof( hook_interrupts[0] ); i++ )
        debug_hook( hook_interrupts[i], 0, debug_handler );

    /*  Hook the hot key interrupt in protected mode and make it pass up.
    */
    if( _d16info.miscellaneous & D16misc_AT_compat ) {
        debug_hook( hotkey_int, 1, debug_handler );
        debug_hook( hotkey_int, 0, debug_handler );
        rsi_int_passup( hotkey_int );
    }

#ifdef TIMER
    if( timer_mult != 0 ) {
        debug_hook( INT_TIMER, 0, debug_handler );          /* Hook as interrupt only */
        outp( TIMER0, ( 65536 / timer_mult ) & 0xff );
        outp( TIMER0, ( 65536 / timer_mult ) >> 8 );
    }
#endif
    return( 0 );
}


#ifdef TIMER
void D32HookTimer( int mult )
{
    timer_mult = mult;
    timer_mod = mult;
}


void D32UnHookTimer( void )
{
    outp( TIMER0, 0 );
    outp( TIMER0, 0 );
}

char Timer_Mod( void )
{
    return( timer_mod );
}
#endif


void D32DebugBreakOp( unsigned char *Int_3 )
{
#if OUTDATEDWINDOWSCRAP
    if( _d16info.swmode == 0 ) {
        *Int_3 = 0xF1;
    } else {
        *Int_3 = 0xCC;
    }
#else
    *Int_3 = 0xCC;
#endif
}

typedef union {
    void        FarPtr pv;
    INTVECT     w;                  /* treat as selector/offset */
} farptr16;

static void FarPtr  save_int5;
static void FarPtr  save_int15;
static void FarPtr  save_int1b;
static void FarPtr  save_int23;

/* Returns 0 for success, 1 for failure
*/
int D32DebugInit( TSF32 FarPtr process_regs, int hkey )
{
    farptr16    lowmem_cs;
    farptr16    org15p;
    farptr16    hotkeyp;

    first_time = 1;
    process_regs->int_id = -1;
    debugging = 0;                  /* Tell debug handler to chain */
    if( hkey != -1 ) {
        hotkey_int = hkey;
    }

    if( _d16info.miscellaneous & D16misc_AT_compat ) {
        rsi_rm_get_vector( INT_PRT_SCRN_KEY,   &save_int5 );
        rsi_rm_get_vector( INT_CTRL_C_KEY,     &save_int23 );
        rsi_rm_get_vector( INT_CTRL_BREAK_KEY, &save_int1b );
    }

    being_debugged = _check_parrent_debugger();

    if( hook_debug_interrupts() )    /* Hook exceptions */
        return( 1 );

    /* Move real mode INT 5, 15, 1B, 23 handlers low and set some of
        them up to generate a passup hotkey_int.  The value of hotkey_int
        determines which real mode interrupt handlers are installed.

        When a selected real mode break condition occurs (Alt-SysReq,
        Ctrl-Break, Ctrl-C, or Shift-PrtSc) it will be trapped, and
        redirected to a protected mode hotkey_int, which will be caught
        by debug_handler.
    */
    if( _d16info.miscellaneous & D16misc_AT_compat ) {
        int             strat;

        strat = rsi_mem_strategy( MForceLow );
        lowmem_cs.w.sel = rsi_seg_realloc( FP_SEG( rm15_handler ) );
        rsi_mem_strategy( strat );
        if( lowmem_cs.w.sel == NULL_SEL )
            return( 1 );
        lowmem_cs.w.off = FP_OFF( rm15_handler );
        lowmem_cs.pv = rsi_get_rm_ptr( lowmem_cs.pv );

        /* Set up INT 15h chain to previous handler, and prepare to store
            address of passup handler (5, 1B, or 23) in hotkey_passup.
        */
        org15p.w.sel = rsi_sel_data_alias( FP_SEG( &org15_handler ) );
        if( org15p.w.sel == NULL_PTR )
            return( 1 );
        org15p.w.off = FP_OFF( &org15_handler );
        hotkeyp.w.sel = org15p.w.sel;
        hotkeyp.w.off = FP_OFF( &hotkey_passup );

        /* Store passup hotkey address.
        */
        rsi_rm_get_vector( hotkey_int, hotkeyp.pv );

        /* Int 15h is always hooked in real mode for the case when Alt-SysReq
            is pressed.
        */
        rsi_rm_get_vector( INT_SYS_SERVICES, org15p.pv );
        save_int15 = org15_handler;
        rsi_rm_set_vector( INT_SYS_SERVICES, lowmem_cs.pv );

        /* Int 1Bh and INT 23h are always hooked in real mode, because we don't
            want Ctrl-Break or Ctrl-C to abort from the debugger before we get
            a chance to clean up.

            Int 5 is never hooked in real mode; if it's the hotkey, it's made
            passup, and that makes an explicit hook unnecessary.  If it's not
            the hotkey, we don't attempt to handle it at all.
        */
        rsi_rm_set_vector( INT_CTRL_BREAK_KEY, MK_FP( lowmem_cs.w.sel, FP_OFF( rm1B_handler ) ) );
        rsi_rm_set_vector( INT_CTRL_C_KEY, MK_FP( lowmem_cs.w.sel, FP_OFF( passup_hotkey ) ) );
        rsi_sel_free( org15p.w.sel );
    }

    /* Initialize debugger_tsf with current register contents;
        clear interrupt flag to account for the fact that interrupts
        should remain disabled until after we complete our first jump
        back into the debugger.
    */
    tsf32_exec( &debugger_tsf, NULL );
    debugger_tsf.eflags &= ~0x200;
    return( 0 );
}

void D32DebugTerm( void )
{
    /* Make interrupt pass down unless it was auto-passup to begin with.
    */
    if( _d16info.miscellaneous & D16misc_AT_compat ) {
        if( ( hotkey_int < 8 ) || ( hotkey_int > 0x2f ) ) {
            rsi_int_passdown( hotkey_int );
        }
        rsi_rm_set_vector( INT_SYS_SERVICES, save_int15 );
        rsi_rm_set_vector( INT_PRT_SCRN_KEY, save_int5 );
        rsi_rm_set_vector( INT_CTRL_BREAK_KEY, save_int1b );
        rsi_rm_set_vector( INT_CTRL_C_KEY, save_int23 );
    }
}

void D32DebugRun( TSF32 FarPtr process_regs )
{
    static char need_fixtrap;

    if( first_time )
        need_fixtrap = init_fixtrap();

    if( process_regs ) {
        dbgregs = *process_regs;

        /* DJT fix for Windows 95 cleverness: call fixtrap if we're
            trying to single-step under Windows 95, to make sure 
            we get the single-step interrupt we're expecting.
        */

        if( need_fixtrap && (dbgregs.eflags & 0x100) )
            fixtrap();

        debugging = 1;              /* Tell debug_handler to be active */

        if( D32NullPtrCheck != NULL_PTR )
            D32NullPtrCheck( 1 );           /* Reactivate NULLP checking */
        if( first_time ) {
            tsf32_exec( &debugger_tsf, &dbgregs );
        } else {
            tsf32_exec( &debugger_tsf, &debug_handler_tsf );
        }
        if( D32NullPtrCheck != NULL_PTR )   /* Disable NULLP checking and query status */
            nullp_checks = D32NullPtrCheck( 0 );

        debugging = 0;              /* Tell debug handler to chain */
    }

    /* We come back here from debug handler */
    debugging = 0;      /* Tell debug handler to chain */
    first_time = 0;

    if( process_regs ) {
#if OUTDATEDWINDOWSCRAP
        /* Windows 3.00 gibberish */
        if( dbgregs.int_id == 6 ) {  /* NEEDWORK */
            static unsigned char opcode[3];

            /* If illegal instruction, check to see if WIN 3.00 BP */
            /* This code used to look in dbgregs.ds (perhaps the   */
            /* access rights aren't right under Windows?) but that */
            /* was clearly wrong when not in flat model            */

            peek32( dbgregs.eip, dbgregs.cs, &opcode, 1 );
            if( opcode[0] == (unsigned char)0xF1 ) {
                dbgregs.int_id = 3; /* Fake breakpoint */
            }
        }
#endif
        *process_regs = dbgregs;
    }
}

