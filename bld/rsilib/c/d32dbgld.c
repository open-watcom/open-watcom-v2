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
* Description:  rsi 32-bit program loader.
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "rsi1632.h"
#include "loader.h"

/* DOS4G Entry points */
#define INIT_ENTRY                      "INIT"
#define LOADER_LOAD_ENTRY               "LOADER"
#define LOADER_UNLOAD_ENTRY             "UNLOAD"
#define LOADER_FREEMAP_ENTRY            "FREEMAP"
#define LOADER_RELOCATE_ENTRY           "REL"
#define LOADER_UNRELOCATE_ENTRY         "UNREL"
#define LOADER_CANLOAD_ENTRY            "CANLOAD"
#define LOADER_GETLOADTABLE_ENTRY       "GETLOADTABLE"
#define LOADER_GETLOADNAME_ENTRY        "GETLOADNAME"

#include "pushpck1.h"
typedef struct {
    unsigned char signature;                /* 'M', or 'Z' for last one */
    unsigned short owner;                   /* Segment of PSP */
    unsigned short size;                    /* Number of paragraphs excluding MCB itself */
    unsigned char unused[3];
    unsigned char name[8];                  /* Not before DOS 4.x */
} MCB;
#include "poppck.h"

static char     exp_loaded;
static SELECTOR exp_base;

static int abspath( char *filename, char *fullpath )
{
    int i;
    char *fn, *fp;
    union REGS r;

    fn = filename;
    fp = fullpath;
    if( fn[0] && fn[1] == ':' ) {
        *fp = fn[0], fn += 2;
    } else {
        r.h.ah = 0x19;  /* get current drive */
        intdos( &r, &r );
        *fp = (char)( r.h.al + 'A' );
    }
    r.h.dl = (unsigned char)( toupper( *fp ) - 'A' + 1 );
    *++fp = ':';
    ++fp;
    if( *fn != '\\' ) {
        /* filename not abs, so get current directory */
        *fp++ = '\\';
        r.h.ah = 0x47;
        r.x.si = (unsigned short)fp;
        intdos( &r, &r );
        if( r.x.cflag )
            return( 0 );
        if( i = strlen( fp ) ) {
            fp += i;
            *fp++ = '\\';
        }
    }
    /* fullpath to fp is current dir for drive, fn is relative path */
    while( *fp++ = *fn ) {
        if( *fn++ == '.' ) {
            if( *fn == '\\' ) {
                ++fn;    /* .\ is nop */
                --fp;
            } else if( *fn == '.' && *++fn == '\\' ) {
                fp -= 3;        /* back past last \ */
                while( *--fp != '\\' ) {
                    if( fp <= fullpath ) {
                        return( 0 );
                    }
                }
            }
        }
    }
    return( 1 );
}

static void set_program_name( char *filename )
/* change name of current program (in environment) */
{
    union {
        char far *cip;
        int far *ip;
        struct {
            unsigned off;
            unsigned sel;
        } w;
    } ep;
    D16REGS     r;
    char        *p;
    char        temp[70];
    unsigned    maxp;
    int         i;
    int         blocksize;
    MCB far     *blockp;
    unsigned    newenv;
    int         oldstrat;
    descriptor  g;
    ULONG       oldenv;
    SELECTOR    oldenv_sel;

    p = temp;
    abspath( filename, p );

    /* Program name not kept before DOS 3.x
    */
    if( _osmajor < 3 )
        return;

    /* Get a pointer to the MCB (arena header) which is always one
        paragraph below the environment block, even in an OS/2 DOS box.
    */
retry:
    oldenv = rsi_abs_address( makeptr( env_sel, 0 ) );
    blockp = MK_FP( rsi_sel_new_absolute( oldenv - 0x10, 0 ), 0 );
    blocksize = blockp->size;
    rsi_sel_free( FP_SEG( blockp ) );

    /* See if we have room to stuff the new name into the existing MCB;
        if not, we will have to allocate a new, larger environment block.
        maxp is the amount of room we have for a path name after subtracting
        the current contents of the environment, a two-byte field between
        the environment and the path name, and two pairs of null bytes.
    */
    maxp =( blocksize << 4 ) - 6;
    ep.ip = makeptr( env_sel, 0 );
    while( *ep.ip != 0 ) {
        ++ep.w.off;
        --maxp;
    }

    if( (i = maxp - strlen( p )) < 0 ) {
#ifdef  DSSI
        /* Can't allocate low memory
        */
        return;
#else
        /* The file name won't fit in the MCB, so we try to make a new
            one.  The number of additional bytes needed is in i, and we
            round up to a full paragraph.  Allocate using DOS so that we
            can free using DOS; otherwise we can lose low memory.
        */
        oldstrat = rsi_mem_strategy( MForceLow );
        r.ax = 0x4800;
        r.bx = blocksize + ( ( -i + 15 ) >> 4 );
        i = r.bx << 4;  /* i == size in bytes of new block */
        oldenv_sel = NULL_SEL;
        if( rsi_rm_interrupt( 0x21, &r, &r ) == 0 ) {
            newenv = r.ax;
            oldenv_sel = rsi_sel_new_absolute( (long)newenv << 4, i );
        }
        rsi_mem_strategy( oldstrat );
        if( oldenv_sel == NULL_SEL )
            return;     /* Give up */

        /* Copy the data from the old environment block into the new
            memory, and prepare to update the old environment descriptor
            with the new size and address.
        */
        movedata( env_sel, 0, oldenv_sel, 0, blocksize << 4 );
        rsi_get_descriptor( oldenv_sel, &g );
        rsi_sel_free( oldenv_sel );

        /* Update the descriptor.  This call fails in an OS/2 2.0 DOS box.
            Discard whichever one of the environment blocks will not be
            referred to in the descriptor.
        */
        if( rsi_set_descriptor( env_sel, &g ) ) {
            r.es = oldenv >> 4;
        } else {
            r.es = newenv;
        }
        r.ax = 0x4900;
        rsi_rm_interrupt( 0x21, &r, &r );
        if( r.es == newenv )
            return;     /* Give up */
        goto retry;
#endif
    }

    /* Skip over the two trailing null bytes and set the next word to 1,
        because this word precedes the program name and indicates the
        "number of items that follow".  Then copy the program name.
    */
    ep.w.off += 2;
    *ep.ip++ = 1;
    while( (*ep.cip++ = *p++) && --maxp > 0 )
            ;
    *ep.ip = 0;

    /* Update debuggee's descriptor table as well as our own.
    */
    rsi_get_descriptor( env_sel, &g );
    g.lim_0_15 = ( blocksize << 4 ) - 1;
    g.lim_16_19 = (( blocksize << 4 ) - 1) / 256 / 256;
    g.base_0_15 = oldenv;
    g.base_16_23 = oldenv >> 16;
    g.base_24_31 = oldenv >> 24;
    rsi_set_descriptor( env_sel, &g );
}

/* Multiple processes should have multiple instances of these */
static LOADER_VECTOR    lv_curr;
static long             main_cookie = -1L;
static long             current_cookie = -1L;

#define ARRAY_LENGTH(x) (sizeof(x)/sizeof(x[0]))
#define MANDATORY_NAMES 7

static char *loader_entry_names[] = {
    INIT_ENTRY,
    LOADER_LOAD_ENTRY,
    LOADER_RELOCATE_ENTRY,
    LOADER_UNRELOCATE_ENTRY,
    LOADER_UNLOAD_ENTRY,
    LOADER_FREEMAP_ENTRY,
    LOADER_CANLOAD_ENTRY,
    LOADER_GETLOADTABLE_ENTRY,
    LOADER_GETLOADNAME_ENTRY
};


/* Given a package that might be a loader, look up all of the
    potential loader entry points from the table above.  If the
    first seven entry points are found, the package is a loader
    and the function returns zero.  Otherwise, the package is not
    a loader and it returns nonzero.

    The names after the first seven are optional in a loader but
    are also filled in by this function if they exist.
*/
static int loader_bind_util( PACKAGE far *p, LOADER_VECTOR *lv )
{
    ACTION  *a;
    int i;

    for( i = 0; i < ARRAY_LENGTH( loader_entry_names ); i++ ) {
        a = rsi_find_action( p, loader_entry_names[i] );
        if( a == NULL_PTR && i < MANDATORY_NAMES )
            return( i + 1 );
        lv->loader_actions[i] = a;
    }
    lv->loader_package = p;
    return( 0 );
}

static int loader_for( FDORNAME filename, ULONG start_pos, LOADER_VECTOR *lv )
{
    PACKAGE far *p;

    /*** NEEDWORK: should check name before attempting binding */
    for( p = _d16info.package_info_p; p != NULL; p = PackageNext( p ) ) {
        /* Loop over all packages */
        if( !loader_bind_util( p, lv ) ) {
            /* When we find a loader, check it out */
            if( LOADER_CANLOAD( lv )( filename, (ULONG)0 ) ) {
                return( 0 );
            }
        }
    }
    return( 1 );
}

void D32SetCurrentObject( long cookie )
{
    if( cookie == -1L )
        cookie = main_cookie;
    current_cookie = cookie;
}

/* Load an executable
*/
int D32DebugLoad( char *filename, char far *cmdtail, TSF32 far *tspv )
{
    int             result;
    char            cmdline[129];
    LOADER_VECTOR   lv_temp;

    set_program_name( filename );

    if( loader_for( (FDORNAME)filename, 0L, &lv_temp ) )
        return( -1 );

    if( result = (int)LOADER_INIT( &lv_temp )() )
        return( result );

    /* Store the filename in the command line buffer and skip over
        it.  We must construct a command line consisting of two
        consecutive ASCII strings, with the the second one being the
        command tail.
    */
    strcpy( cmdline, filename );
    far_strcpy( strchr( cmdline, '\0' ) + 1, cmdtail );

    /* We need to load programs in the target interrupt space, in
        case code such as a DLL initializer actually runs.
    */
    if( D32NullPtrCheck != NULL_PTR )   /* Disable NULLP checking, if possible */
        D32NullPtrCheck( 1 );

    result = LOADER_LOAD( &lv_temp )( (FDORNAME)filename, (ULONG)0L, tspv, (void far *)&main_cookie, (char far *)cmdline );

    if( D32NullPtrCheck != NULL_PTR )   /* Disable NULLP checking, if possible */
        nullp_checks = D32NullPtrCheck( 0 );

    if( result )
        return( -2 );

    /* Loader bug - it's ignoring the 'cmdline' parm */
    {
        unsigned        len;
        unsigned char   far *dst;

        len = _fstrlen( cmdtail );
        dst = MK_FP( (SELECTOR)tspv->es, 0x80 );
        _fmemcpy( dst + 1, cmdtail, len );
        dst[0] = len;
        dst[len + 1] = '\r';
    }
    user_sel_start = (SELECTOR)tspv->esi;     /* Set user_sel to first user sel */

/* KLUDGE UNTIL LINEXE LOADER IS FIXED -- it only sets low word of
    cookie and looks at both words when called back
*/
    if( main_cookie != -1L )
        main_cookie &= 0xFFFF;


    lv_curr = lv_temp;          /* lv_curr now safe to use */
    D32SetCurrentObject( main_cookie );
    tspv->eflags = 0x200;       /* interrupts enabled */
    tspv->ebp = 0L;             /* for backtrace */

    filename = "EXP_LOADER";
    cmdtail = lv_curr.loader_package->package_title;

    while( *filename ) {
        if( !*cmdtail )
            return( 0 );
        filename++;
        cmdtail++;
    }
    if( !*cmdtail ) {
        exp_loaded = 1;
        exp_base = user_sel_start - user_sel_const;
    }
    return( 0 );
}

#ifdef XXX0
/* Unload a loaded executable -- not yet used anywhere */
int D32DebugUnLoad( void )
{
    if( LOADER_UNLOAD( &lv_curr )( (void far *)&current_cookie ) )
        return( -1 );
    return( 0 );
}

static Fptr32 fptr = { 0, 0 };
#endif

static SELECTOR exp_relocate( SELECTOR sel )
{
    if( !sel )
        return( sel );

    /* Look up system selectors (less than 0x80) in the __d16_selectors
        table.
    */
    if( sel < user_sel_const )
        return( ((SELECTOR *)&__d16_selectors)[sel >> 3] );
    return( sel + exp_base );
}

static SELECTOR exp_unrelocate( SELECTOR sel )
{
    if( !sel )
        return( sel );

    /* Look for the selector in the system selectors table and compute
        the unrelocated value from the index.  We only need to do this
        for selectors that are out of range of the user program.
    */
    if( sel < user_sel_start ) {
        int     i;

        for( i = 1; i < ( user_sel_const >> 3 ); i++ ) {
            if( sel == ((SELECTOR *)&__d16_selectors)[i] ) {
                return( i << 3 );
            }
        }
        return( 0 );
    }
    return( sel - exp_base );
}

int D32Unrelocate( Fptr32 FarPtr fptrp )
{
    Fptr32 old;

    old = *fptrp;
    if( exp_loaded ) {
        fptrp->sel = exp_unrelocate( fptrp->sel );
    } else if( LOADER_UNREL( &lv_curr ) ) {
        LOADER_UNREL( &lv_curr )( fptrp, current_cookie );
    }
    return( ( fptrp->sel != old.sel ) || ( fptrp->off != old.off ) );
}

int D32Relocate( Fptr32 FarPtr fptrp )
{
    Fptr32 old;

    old = *fptrp;
    if( exp_loaded ) {
        fptrp->sel = exp_relocate( fptrp->sel );
    } else if( LOADER_REL( &lv_curr ) ) {
        /* Make sure there's a relo function */
        LOADER_REL( &lv_curr )( fptrp, current_cookie );
    }
    return( ( fptrp->sel != old.sel ) || ( fptrp->off != old.off ) );
}

