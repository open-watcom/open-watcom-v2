/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2022 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Trap file loading for DOS extended debugger.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include <conio.h>
#include <stdio.h>
#include <i86.h>
#include "dsxutil.h"
#include "exedos.h"
#include "trptypes.h"
#include "trpcore.h"
#include "trpld.h"
#include "trpsys.h"
#include "tcerr.h"
#include "digcli.h"
#include "digld.h"
#include "envlkup.h"
#include "realmod.h"


#define DOS4G_COMM_VECTOR       0x15
#define NUM_BUFF_RELOCS         16
#define DEFAULT_TRP_EXT         "TRP"
#define TRAP_VECTOR             0x1a
#define PSP_ENVSEG_OFF          0x2c

#define TRAP_SIGNATURE          0xdeaf

#define _NBPARAS( bytes )       ((bytes + 15UL) / 16)

#include "pushpck1.h"
typedef struct {
    unsigned_16         sig;
    addr32_off          init;
    addr32_off          req;
    addr32_off          fini;
} trap_file_header;

typedef struct {
    memptr      ptr;
    unsigned_16 len;
} mx_entry16;

typedef struct {
    unsigned_16     remote;
    unsigned_16     retcode;
    trap_version    version;
    addr32_off      errmsg_off;
} trap_init_struct;
#include "poppck.h"

typedef enum {
    IS_NONE,
    IS_DPMI,
    IS_RATIONAL
} intr_state;

static dpmi_dos_block   TrapMem;

static void             __far *RawPMtoRMSwitchAddr;

char                    DPMICheck = 0;

//#define FULL_SAVE

#ifdef FULL_SAVE
#define INT_VECT_COUNT  256
#define EXCEPT_COUNT    32
#else
#define INT_VECT_COUNT
#define EXCEPT_COUNT
#endif

static unsigned_8       PMVectSaveList[INT_VECT_COUNT] = {
    0x00, 0x10, 0x21, 0x2f, 0x31, 0x33
};
#define NUM_PM_SAVE_VECTS (sizeof(PMVectSaveList)/sizeof(PMVectSaveList[0]))

static void __far *OrigPMVects[NUM_PM_SAVE_VECTS];
static void __far *SavePMVects[NUM_PM_SAVE_VECTS];

static unsigned_8       PMExceptSaveList[EXCEPT_COUNT] = {
    0x01, 0x03, 0x05
};
#define NUM_PM_SAVE_EXCEPTS (sizeof(PMExceptSaveList)/sizeof(PMExceptSaveList[0]))

static void __far *OrigPMExcepts[NUM_PM_SAVE_EXCEPTS];
static void __far *SavePMExcepts[NUM_PM_SAVE_EXCEPTS];

static intr_state   IntrState = IS_NONE;

extern void DoRawSwitchToRM( unsigned, unsigned, unsigned );
#pragma aux DoRawSwitchToRM = \
        "push   ebp" \
        "mov    edx,eax" \
        "mov    ecx,eax" \
        "mov    esi,eax" \
        "lea    ebp,-8[esp]" \
        "call   pword ptr [RawPMtoRMSwitchAddr]" \
        "pop    ebp" \
    __parm __caller     [__eax] [__ebx] [__edi] \
    __value             \
    __modify __exact    [__eax __ebx __ecx __edx __esi __edi __gs]

extern void     BackFromRealMode( void );

/*
    We zero out the registers here so that there isn't any garbage
    in the high word of them in 16-bit code. It turns out that the
    Pentium sometimes uses the full 32-bit registers even
    when the instruction specifies the 16-bit version (e.g string
    instructions.
*/
extern void DoIntSwitchToRM( void );
#pragma aux DoIntSwitchToRM = \
        "pushad" \
        "xor    ebp,ebp" \
        "xor    ebx,ebx" \
        "xor    esi,esi" \
        "xor    edi,edi" \
        "xor    ecx,ecx" \
        "xor    edx,edx" \
        "xor    eax,eax" \
        "mov    ah,6" \
        "mov    cx,0xffff" \
        "int    1ah" \
        "popad" \
    __parm              [] \
    __value             \
    __modify __exact    []

#define P1616NULL   0L

typedef unsigned long P1616;
typedef P1616 extension_routine( unsigned, void *, void * );

static char sig_str[] = "RATIONAL DOS/4G";
static char pkg_name[] = "D32_KERNEL";
static char pkg_entry[] = "D32NullPtrCheck";

#pragma aux extension_routine __parm [__eax] [__edx] [__ebx] __value [__eax]
static extension_routine __far *RSI_extensions;

/* These are static because I'm not conversant with your inline asm
   facility, and this accomplished the desired result...
*/

static P1616 _D32NullPtrCheck;

static P1616 __cdecl find_entry( void )
{
    P1616 retval = 0;

    RSI_extensions = DPMIGetVendorSpecificAPI( sig_str );
    if( RSI_extensions != NULL ) {
        retval = RSI_extensions( 0, pkg_entry, pkg_name );
    }
    return (retval);
}

/* Returns 16:16 pointer to MONITOR array, describing state of hardware
   breakpoints.  You shouldn't care about the return value during your init.
*/
static int __cdecl D32NullPtrCheck( unsigned short on )
{
    static int  old_state;
    int         old;
    char        buff[128];

    if( _D32NullPtrCheck == P1616NULL ) {
        _D32NullPtrCheck = find_entry();
        if( _D32NullPtrCheck == P1616NULL )
            return( 0 );
        EnvLkup( "DOS4G", buff, sizeof( buff ) );
        if( strstr( strupr( buff ), "NULLP" ) ) {
            old_state = 1;
        }
    }
    old = old_state;
    if( old_state != on ) {
        RSI_extensions( 2, (void *)_D32NullPtrCheck, &on );
        old_state = on;
    }
    return( old );
}

void SaveOrigVectors( void )
{
    rm_data             *p;
    unsigned            i;
    unsigned            old;

#ifdef FULL_SAVE
    for( i = 0; i < INT_VECT_COUNT; ++i )
        PMVectSaveList[i] = i;
    for( i = 0; i < EXCEPT_COUNT; ++i )
        PMExceptSaveList[i] = i;
#endif
    old = D32NullPtrCheck( 0 );
    /* haven't moved things yet, so PMData isn't set up */
    p = (rm_data *)RMDataStart;
    for( i = 0; i < NUM_VECTS; ++i ) {
        p->orig_vects[i].a = MyGetRMVector( i );
    }
    D32NullPtrCheck( old );
    for( i = 0; i < NUM_PM_SAVE_VECTS; ++i ) {
        OrigPMVects[i] = DPMIGetPMInterruptVector( PMVectSaveList[i] );
    }
    for( i = 0; i < NUM_PM_SAVE_EXCEPTS; ++i ) {
        OrigPMExcepts[i] = DPMIGetPMExceptionVector( PMExceptSaveList[i] );
    }
    memcpy( p->vecttable1, p->orig_vects, sizeof( p->vecttable1 ) );
    memcpy( p->vecttable2, p->orig_vects, sizeof( p->vecttable2 ) );
    memcpy( SavePMVects, OrigPMVects, sizeof( SavePMVects ) );
    memcpy( SavePMExcepts, OrigPMExcepts, sizeof( SavePMExcepts ) );
}

void RestoreOrigVectors(void)
{
    unsigned            i;
    unsigned            old;

    for( i = 0; i < NUM_PM_SAVE_EXCEPTS; ++i ) {
        DPMISetPMExceptionVector( PMExceptSaveList[i], OrigPMExcepts[i] );
    }
    for( i = 0; i < NUM_PM_SAVE_VECTS; ++i ) {
        DPMISetPMInterruptVector( PMVectSaveList[i], OrigPMVects[i] );
    }
    old = D32NullPtrCheck( 0 );
    for( i = 0; i < NUM_VECTS; ++i ) {
        MySetRMVector( i, PMData->orig_vects[i].s.segment, PMData->orig_vects[i].s.offset );
    }
    D32NullPtrCheck( old );
}

static void GoToRealMode( void *rm_func )
{
    unsigned    i;

    PMData->rm_func = RM_OFF( rm_func );
    if( IntrState == IS_DPMI ) {
        for( i = 0; i < NUM_PM_SAVE_VECTS; ++i ) {
            DPMISetPMInterruptVector( PMVectSaveList[i], SavePMVects[i] );
        }
        for( i = 0; i < NUM_PM_SAVE_EXCEPTS; ++i ) {
            DPMISetPMExceptionVector( PMExceptSaveList[i], SavePMExcepts[i] );
        }
        DoRawSwitchToRM( RMData.rm, offsetof( rm_data, stack ) + STACK_SIZE, RM_OFF( RawSwitchHandler ) );
        for( i = 0; i < NUM_PM_SAVE_EXCEPTS; ++i ) {
            SavePMExcepts[i] = DPMIGetPMExceptionVector( PMExceptSaveList[i] );
            DPMISetPMExceptionVector( PMExceptSaveList[i], OrigPMExcepts[i] );
        }
        for( i = 0; i < NUM_PM_SAVE_VECTS; ++i ) {
            SavePMVects[i] = DPMIGetPMInterruptVector( PMVectSaveList[i] );
            DPMISetPMInterruptVector( PMVectSaveList[i], OrigPMVects[i] );
        }
    } else {
        DoIntSwitchToRM();
    }
}

static uint_16 EnvAreaSize( char __far *envarea )
{
    char        __far *envptr;

    envptr = envarea;
    while( *envptr ) {
        envptr += _fstrlen( envptr ) + 1;
    }
    return( envptr - envarea + 1 );
}

static char *CopyEnv( void )
{
    char        __far *envarea;
    uint_16     envsize;

    envarea = _MK_FP( *(addr_seg __far *)_MK_FP( _psp, PSP_ENVSEG_OFF ), 0 );
    envsize = EnvAreaSize( envarea );
    PMData->envseg = DPMIAllocateDOSMemoryBlock( _NBPARAS( envsize ) );
    if( PMData->envseg.pm == 0 ) {
        return( TC_ERR_OUT_OF_DOS_MEMORY );
    }
    _fmemcpy( EXTENDER_RM2PM( PMData->envseg.rm, 0 ), envarea, envsize );
    return( NULL );
}

static char *SetTrapHandler( void )
{
    char                dummy;
    long                sel;
    descriptor          desc;
    version_info        ver;

    PMData->vecttable1[DOS4G_COMM_VECTOR].s.segment = RMData.rm;
    PMData->vecttable1[DOS4G_COMM_VECTOR].s.offset = RM_OFF( Interrupt15 );
    PMData->vecttable2[DOS4G_COMM_VECTOR].s.segment = RMData.rm;
    PMData->vecttable2[DOS4G_COMM_VECTOR].s.offset = RM_OFF( Interrupt15 );
    if( IntrState == IS_NONE ) {
        DPMIGetVersion( &ver );
        if( (ver.major_version >= 1 || ver.minor_version > 90) || DPMICheck == 2 ) {
            RawPMtoRMSwitchAddr = DPMIRawPMtoRMAddr();
            PMData->switchaddr.a= DPMIRawRMtoPMAddr();
        }
        if( RawPMtoRMSwitchAddr == 0 || PMData->switchaddr.a == 0 || DPMICheck == 1 ) {
            IntrState = IS_RATIONAL;
        } else {
            PMData->saveaddr.a = DPMISavePMStateAddr();
            PMData->savesize   = DPMISaveStateSize();
            if( PMData->savesize == 0 ) {
                PMData->saveseg.rm = 0;
                PMData->saveseg.pm = 0;
            } else {
                PMData->saveseg = DPMIAllocateDOSMemoryBlock( _NBPARAS( PMData->savesize * 2 ) );
                if( PMData->saveseg.pm == 0 ) {
                    return( TC_ERR_OUT_OF_DOS_MEMORY );
                }
            }
            PMData->othersaved = false;
            sel = DPMIAllocateLDTDescriptors( 1 );
            if( sel < 0 ) {
                return( TC_ERR_CANT_LOAD_TRAP );
            }
            DPMIGetDescriptor( _FP_SEG( PMData ), &desc );
            PMData->pmode_cs = sel;
            desc.xtype.use32 = 0;
            desc.type.execute = 1;
            DPMISetDescriptor( sel, &desc );
            PMData->pmode_eip = RM_OFF( BackFromRealMode );
            PMData->pmode_ds  = _FP_SEG( &PMData );
            PMData->pmode_es  = PMData->pmode_ds;
            PMData->pmode_ss  = _FP_SEG( &dummy );
            IntrState = IS_DPMI;
        }
    }
    if( IntrState == IS_RATIONAL ) {
        MySetRMVector( TRAP_VECTOR, RMData.rm, RM_OFF( RMTrapHandler ) );
    }
    return( NULL );
}

static bool CallTrapInit( const char *parms, char *errmsg, trap_version *trap_ver )
{
    trap_init_struct    __far *callstruct;

    callstruct = (void __far *)PMData->parmarea;
    callstruct->remote = trap_ver->remote;
    _fstrcpy( (char __far *)&callstruct[1], parms );
    callstruct->errmsg_off = sizeof( *callstruct ) + strlen( parms ) + 1;
    GoToRealMode( RMTrapInit );
    *trap_ver = callstruct->version;
    _fstrcpy( errmsg, (char __far *)callstruct + callstruct->errmsg_off );
    return( *errmsg == '\0' );
}

static char *ReadInTrap( FILE *fp )
{
    dos_exe_header      hdr;
    memptr              relocbuff[NUM_BUFF_RELOCS];
    unsigned            relocnb;
    unsigned            imagesize;
    unsigned            hdrsize;

    if( DIGLoader( Read )( fp, &hdr, sizeof( hdr ) ) ) {
        return( TC_ERR_CANT_LOAD_TRAP );
    }
    if( hdr.signature != DOS_SIGNATURE ) {
        return( TC_ERR_BAD_TRAP_FILE );
    }

    hdrsize = hdr.hdr_size * 16;
    imagesize = ( hdr.file_size * 0x200 ) - (-hdr.mod_size & 0x1ff) - hdrsize;
    TrapMem = DPMIAllocateDOSMemoryBlock( _NBPARAS( imagesize ) + hdr.min_16 );
    if( TrapMem.pm == 0 ) {
        return( TC_ERR_OUT_OF_DOS_MEMORY );
    }
    DIGLoader( Seek )( fp, hdrsize, DIG_ORG );
    if( DIGLoader( Read )( fp, (void *)DPMIGetSegmentBaseAddress( TrapMem.pm ), imagesize ) ) {
        return( TC_ERR_CANT_LOAD_TRAP );
    }
    DIGLoader( Seek )( fp, hdr.reloc_offset, DIG_ORG );
    for( relocnb = NUM_BUFF_RELOCS; hdr.num_relocs > 0; --hdr.num_relocs, ++relocnb ) {
        if( relocnb >= NUM_BUFF_RELOCS ) {
            if( DIGLoader( Read )( fp, relocbuff, sizeof( memptr ) * NUM_BUFF_RELOCS ) ) {
                return( TC_ERR_CANT_LOAD_TRAP );
            }
            relocnb = 0;
        }
        *(addr_seg __far *)EXTENDER_RM2PM( TrapMem.rm + relocbuff[relocnb].s.segment, relocbuff[relocnb].s.offset ) += TrapMem.rm;
    }
    return( NULL );
}

static trap_retval DoTrapAccess( trap_elen num_in_mx, in_mx_entry_p mx_in, trap_elen num_out_mx, mx_entry_p mx_out )
{
    uint_8              __far *msgptr;
    unsigned_8          j;
    struct {
        mx_entry16      in;
        mx_entry16      out;
        unsigned_16     retlen;
    }                   __far *callstruct;
    unsigned            len;
    unsigned            copy;

    callstruct = (void __far *)PMData->parmarea;
    msgptr = (void __far *)&callstruct[1];
    callstruct->in.ptr.s.segment = RMData.rm;
    callstruct->in.ptr.s.offset = (unsigned)msgptr - (unsigned)PMData;
    callstruct->in.len = 0;
    for( j = 0; j < num_in_mx; ++j ) {
        _fmemcpy( msgptr, mx_in[j].ptr, mx_in[j].len );
        callstruct->in.len += mx_in[j].len;
        msgptr += mx_in[j].len;
    }

    callstruct->out.len = 0;
    if( mx_out != NULL ) {
        callstruct->out.ptr.s.segment = RMData.rm;
        callstruct->out.ptr.s.offset = (unsigned)msgptr - (unsigned)PMData;
        for( j = 0; j < num_out_mx; ++j ) {
            callstruct->out.len += mx_out[j].len;
        }
    } else {
        callstruct->out.ptr.a = 0;
    }
    GoToRealMode( RMTrapAccess );
    if( callstruct->retlen == (unsigned_16)REQUEST_FAILED ) {
        return( REQUEST_FAILED );
    }
    if( mx_out != NULL ) {
        /* msgptr is pointing at the start of the output buffer */
        j = 0;
        for( len = callstruct->retlen; len != 0; len -= copy ) {
            copy = len;
            if( copy > mx_out[j].len )
                copy = mx_out[j].len;
            _fmemcpy( mx_out[j].ptr, msgptr, copy );
            ++j;
            msgptr += copy;
        }
    } else {
        callstruct->retlen = 0;
    }
    if( TRP_REQUEST( mx_in ) == REQ_CONNECT ) {
        if( ((connect_ret *)mx_out->ptr)->max_msg_size > MAX_MSG_SIZE ) {
            ((connect_ret *)mx_out->ptr)->max_msg_size = MAX_MSG_SIZE;
        }
    }
    return( callstruct->retlen );
}

char *LoadTrap( const char *parms, char *buff, trap_version *trap_ver )
{
    FILE                *fp;
    trap_file_header    __far *head;
    char                filename[256];
    char                *p;
    char                chr;

    if( parms == NULL || *parms == '\0' )
        parms = DEFAULT_TRP_NAME;
    p = filename;
    for( ; (chr = *parms) != '\0'; parms++ ) {
        if( chr == TRAP_PARM_SEPARATOR ) {
            parms++;
            break;
        }
        *p++ = chr;
    }
#ifdef USE_FILENAME_VERSION
    *p++ = ( USE_FILENAME_VERSION / 10 ) + '0';
    *p++ = ( USE_FILENAME_VERSION % 10 ) + '0';
#endif
    *p = '\0';
    fp = DIGLoader( Open )( filename, p - filename, DEFAULT_TRP_EXT, NULL, 0 );
    if( fp == NULL ) {
        sprintf( buff, "%s '%s'", TC_ERR_CANT_LOAD_TRAP, filename );
        return( buff );
    }
    p = ReadInTrap( fp );
    DIGLoader( Close )( fp );
    sprintf( buff, "%s '%s'", TC_ERR_CANT_LOAD_TRAP, filename );
    if( p == NULL ) {
        if( (p = SetTrapHandler()) != NULL || (p = CopyEnv()) != NULL ) {
            strcpy( buff, p );
        } else {
            strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
            head = EXTENDER_RM2PM( TrapMem.rm, 0 );
            if( head->sig == TRAP_SIGNATURE ) {
                PMData->initfunc.s.offset = head->init;
                PMData->reqfunc.s.offset  = head->req;
                PMData->finifunc.s.offset = head->fini;
                PMData->initfunc.s.segment = TrapMem.rm;
                PMData->reqfunc.s.segment  = TrapMem.rm;
                PMData->finifunc.s.segment = TrapMem.rm;
                if( CallTrapInit( parms, buff, trap_ver ) ) {
                    if( TrapVersionOK( *trap_ver ) ) {
                        TrapVer = *trap_ver;
                        ReqFunc = DoTrapAccess;
                        return( NULL );
                    }
                    strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
                }
            }
        }
    }
    KillTrap();
    return( buff );
}

void KillTrap( void )
{
    if( IntrState != IS_NONE ) {
        GoToRealMode( RMTrapFini );
        IntrState = IS_NONE;
    }
    if( TrapMem.pm != 0 ) {
        DPMIFreeDOSMemoryBlock( TrapMem.pm );
    }
    if( PMData->envseg.pm != 0 ) {
        DPMIFreeDOSMemoryBlock( PMData->envseg.pm );
    }
    if( PMData->saveseg.pm != 0 ) {
        DPMIFreeDOSMemoryBlock( PMData->saveseg.pm );
    }
}
