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
* Description:  DOS protected mode test child program (32-bit executable).
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <i86.h>
#include <conio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <io.h>
#include "tinyio.h"

#define _debug( s )     { cputs( s ); cputs( "\n\rPress a key\n\r" ); getch(); }
#define NB_VECTORS      256

static void             __far *SavePMVTable[NB_VECTORS];
static unsigned         SaveRMVTable[NB_VECTORS];
static unsigned         NewPMVTable[NB_VECTORS];
static unsigned         NewRMVTable[NB_VECTORS];

// Shouldn't this set si as well?? - MN
extern void DoInt66( uint_16, uint_16, void __far * );
#pragma aux DoInt66 =   \
    "xchg bx, cx "      \
    "shr  ecx, 16"      \
    "push bp     "      \
    "int  0x66   "      \
    "pop  bp     "      \
    parm [ax] [dx] [cx ebx] \
    modify exact [eax ebx ecx edx esi edi];

extern void StoreDTs( void *, uint_16 *, void * );
#pragma aux StoreDTs = \
    "sgdt [ebx]" \
    "sldt [esi]" \
    "sidt [edi]" \
    parm [ebx] [esi] [edi]     \
    modify exact [];

extern void CLI( void );
#pragma aux CLI = \
    "cli"       \
    modify exact [];

extern void STI( void );
#pragma aux STI = \
    "sti"       \
    modify exact [];


static void save_vects( unsigned *rmvtable, void __far **pmvtable )
{
    int         intnb;

    #ifdef __DOS4G__
        memcpy( rmvtable, 0, NB_VECTORS * sizeof( *rmvtable ) );
    #endif
    for( intnb = 0; intnb < NB_VECTORS; ++intnb ) {
        #ifndef __DOS4G__
            rmvtable[intnb] = TinyDPMIGetRealVect( intnb );
        #endif
        pmvtable[intnb] = TinyDPMIGetProtectVect( intnb );
    }
}

static void restore_vects( unsigned *rmvtable, void __far **pmvtable )
{
    int         intnb;

    #ifdef __DOS4G__
        CLI();
        memcpy( 0, rmvtable, NB_VECTORS * sizeof( *rmvtable ) );
        STI();
    #endif
    for( intnb = 0; intnb < NB_VECTORS; ++intnb ) {
        #ifndef __DOS4G__
            TinyDPMISetRealVect( intnb,
                                 ( (uint_16 *)&rmvtable[intnb] )[1],
                                 (uint_16)rmvtable[intnb] );
        #endif
        TinyDPMISetProtectVect( intnb, pmvtable[intnb] );
    }
}

static void read_vtable( unsigned *rmvtable, unsigned *pmvtable, uint_16 *cs,
                         uint_16 *ds )
{
    int         fhandle;

    fhandle = open( "vtable", O_BINARY | O_RDONLY );
    if( fhandle <= 0 ) {
        _debug( "error: fhandle <= 0" );
    } else {
        read( fhandle, rmvtable, NB_VECTORS * sizeof( *rmvtable ) );
        read( fhandle, pmvtable, NB_VECTORS * sizeof( *pmvtable ) );
        read( fhandle, cs, sizeof( *cs ) );
        read( fhandle, ds, sizeof( *ds ) );
        close( fhandle );
    }
}

static void dump_selec( uint_16 sel )
{
    tiny_dscp           d;
    char                buff[100];

    if( TinyDPMIGetDescriptor( sel, &d ) ) {
        _debug( "error getting descriptor for selector" );
    } else {
        strcpy( buff, "selector=" );
        itoa( sel, buff + strlen( buff ), 16 );
        strcat( buff, d.type.accessed ? " accessed|" : " not accessed|" );
        strcat( buff, d.type.rdwr ? "read/write|" : "read only|" );
        strcat( buff, d.type.execute ? "code|" : "data|" );
        strcat( buff, d.type.exp_down ? "expand down|" : "expand up|" );
        strcat( buff, d.type.present ? "present|" : "not present|" );
        strcat( buff, d.xtype.use32 ? "use32|" : "use16|" );
        strcat( buff, d.xtype.page_gran ? "page granular" : "byte granular" );
        strcat( buff, " dpl=" );
        itoa( d.type.dpl, buff + strlen( buff ), 10 );
        _debug( buff );
    }
}

static void setup_sel( uint_16 cs, uint_16 ds, uint_16 *pmcs, uint_16 *pmds )
{
    tiny_dscp           d;

    *pmcs = TinyDPMICreateSel( 1 );
    *pmds = TinyDPMICreateSel( 1 );
    if( !*pmcs || !*pmds ) {
        _debug( "error creating selectors" );
    }
    if( TinyDPMIGetDescriptor( FP_SEG( &setup_sel ), &d ) ) {
        _debug( "error obtaining descriptor for new cs selector" );
    }
    d.xtype.use32 = 0;
    d.xtype.page_gran = 0;
    if( TinyDPMISetRights( *pmcs, ( *(uint_16 *)&d.xtype << 8 )
                                  | *(uint_16 *)&d.type ) ) {
        _debug( "error setting segment rights for new cs selector" );
    }
    if( TinyDPMISetBase( *pmcs, (unsigned)cs << 4 ) ) {
        _debug( "error setting segment base for new cs selector" );
    }
    if( TinyDPMISetLimit( *pmcs, 0xffff ) ) {
        _debug( "error setting segment limit for new cs selector" );
    }
    if( TinyDPMIGetDescriptor( FP_SEG( &SavePMVTable ), &d ) ) {
        _debug( "error obtaining descriptor for new ds selector" );
    }
    d.xtype.use32 = 0;
    d.xtype.page_gran = 0;
    if( TinyDPMISetRights( *pmds, ( *(uint_16 *)&d.xtype << 8 )
                                  | *(uint_16 *)&d.type ) ) {
        _debug( "error setting segment rights for new ds selector" );
    }
    if( TinyDPMISetBase( *pmds, (unsigned)ds << 4 ) ) {
        _debug( "error setting segment base for new ds selector" );
    }
    if( TinyDPMISetLimit( *pmds, 0xffff ) ) {
        _debug( "error setting segment limit for new ds selector" );
    }
}

static void set_new_vects( unsigned *rmvtable, unsigned *pmvtable,
                           uint_16 pmcs )
{
    int         intnb;

    #ifdef __DOS4G__
        CLI();
        memcpy( 0, rmvtable, NB_VECTORS * sizeof( *rmvtable ) );
        STI();
    #endif
    for( intnb = 0; intnb < NB_VECTORS; ++intnb ) {
        #ifndef __DOS4G__
            TinyDPMISetRealVect( intnb,
                                 ( (uint_16 *)&rmvtable[intnb] )[1],
                                 (uint_16)rmvtable[intnb] );
        #endif
        if( intnb == 0x66 ) {
            TinyDPMISetProtectVect( intnb, MK_FP( pmcs,
                                           (uint_16)pmvtable[intnb] ) );
        } else {
            TinyDPMISetProtectVect( intnb,
                                MK_FP( ( (uint_16 *)&pmvtable[intnb] )[1],
                                       (uint_16)pmvtable[intnb] ) );
        }
    }
    _debug( "new vectors have been set" );
}

extern void main( void )
{
    uint_16             ds;
    uint_16             cs;
    uint_16             pmcs;
    uint_16             pmds;
    uint_16             ldt;
    struct {
        uint_16 limit;
        uint_32 base;
    }                   gdt,
                        idt;

    _debug( "hi from protected mode task" );
    _debug( "reading debugger's real and protected mode vector tables" );
    read_vtable( NewRMVTable, NewPMVTable, &cs, &ds );
    _debug( "setting up selectors based at debugger's cs and ds" );
    setup_sel( cs, ds, &pmcs, &pmds );
    _debug( "saving my real and protected mode vector tables" );
    save_vects( SaveRMVTable, SavePMVTable );
    _debug( "switching to debugger's real and protected mode vector tables" );
    set_new_vects( NewRMVTable, NewPMVTable, pmcs );
    StoreDTs( &gdt, &ldt, &idt );
    dump_selec( pmcs );
    dump_selec( pmds );
    //dump_selec( ldt );
    _debug( "doing an int 0x66" );
    DoInt66( pmcs, pmds, SavePMVTable[0x66] );
    _debug( "completed int 0x66" );
    _debug( "switching back to my real and protected mode vector tables" );
    restore_vects( SaveRMVTable, SavePMVTable );
    _debug( "press any key to terminate task" );
}
