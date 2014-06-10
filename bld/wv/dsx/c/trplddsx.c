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
* Description:  Trap file loading for DOS extended debugger.
*
****************************************************************************/


#include <i86.h>
#include <string.h>
#include <stdlib.h>
#include <stddef.h>
#include "dpmi.h"
#include "exedos.h"
#include "dbgdefn.h"
#include "dbgio.h"
#include "dsxutil.h"
#include "trpcore.h"
#include "tinyio.h"
#include "tcerr.h"
#include "digio.h"

#ifdef __OSI__
#include "extender.h"
#endif

#include <conio.h>

#include <stdio.h>

extern trap_version     TrapVer;
extern trap_req_func    *ReqFunc;

#define DOS4G_COMM_VECTOR       0x15
#define NUM_BUFF_RELOCS         16
#define DEFAULT_TRP_NAME        "STD"
#define DEFAULT_TRP_EXT         "TRP"
#define PARM_SEPARATOR          ';'
#define TRAP_VECTOR             0x1a
#define PSP_ENVSEG_OFF          0x2c

#define TRAP_SIGNATURE          0xdeaf
typedef _Packed struct {
    unsigned_16         sig;
    addr32_off          init;
    addr32_off          req;
    addr32_off          fini;
} trap_file_header;

typedef _Packed struct {
    memptr      ptr;
    unsigned_16 len;
} mx_entry16;

static dos_memory               TrapMem;

static void __far               *RawPMtoRMSwitchAddr;

extern unsigned                 EnvLkup( char *, char *, unsigned max );


unsigned_8      DPMICheck = 0;

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

static void __far *OrigPMExcepts[NUM_PM_SAVE_VECTS];
static void __far *SavePMExcepts[NUM_PM_SAVE_VECTS];

static enum {
    IS_NONE,
    IS_DPMI,
    IS_RATIONAL
}               IntrState = IS_NONE;


extern void DoRawSwitchToRM( unsigned, unsigned, unsigned );
#pragma aux DoRawSwitchToRM = \
    "push   ebp" \
    "mov    edx,eax" \
    "mov    ecx,eax" \
    "mov    esi,eax" \
    "lea    ebp,-8[esp]" \
    "call   pword ptr [RawPMtoRMSwitchAddr]" \
    "pop    ebp" \
    parm caller [eax] [ebx] [edi] \
    modify exact [eax ebx ecx edx esi edi gs];

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
    modify exact [];

typedef unsigned long P1616;
typedef P1616 extension_routine( unsigned, void *, void * );

static char sig_str[] = "RATIONAL DOS/4G";
static char pkg_name[] = "D32_KERNEL";
static char pkg_entry[] = "D32NullPtrCheck";

#pragma aux extension_routine parm [eax] [edx] [ebx] value [eax];
static extension_routine __far *RSI_extensions;

/* These are static because I'm not conversant with your inline asm
   facility, and this accomplished the desired result...
*/
//static void *parmp;
static P1616 _D32NullPtrCheck;

#if 0
void lookup_prep (void);
#pragma aux lookup_prep = \
    "sub eax, eax" \
    "mov ebx, OFFSET pkg_name" \
    "mov edx, OFFSET pkg_entry"

void call_prep (void);
#pragma aux call_prep = \
    "mov eax, 2" \
    "mov ebx, parmp" \
    "mov edx, _D32NullPtrCheck"
#endif

P1616 __cdecl find_entry( void )
{
    P1616 retval = 0;

    RSI_extensions = DPMIGetVendorSpecificAPI( sig_str );
    if (RSI_extensions != NULL ) {
        retval = RSI_extensions( 0, pkg_entry, pkg_name );
    }
    return (retval);
}

/* Returns 16:16 pointer to MONITOR array, describing state of hardware
   breakpoints.  You shouldn't care about the return value during your init.
*/
int __cdecl D32NullPtrCheck( unsigned short on )
{
    static int      old_state;
    int         old;
    char        buff[128];

    if( _D32NullPtrCheck == NULL ) {
        _D32NullPtrCheck = find_entry();
        if( _D32NullPtrCheck == NULL )
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
    return (old);
}

void SaveOrigVectors( void )
{
    rm_data             *p;
    unsigned            i;
    unsigned            old;

#ifdef FULL_SAVE
    for( i = 0; i < INT_VECT_COUNT; ++i ) PMVectSaveList[i]=i;
    for( i = 0; i < EXCEPT_COUNT; ++i ) PMExceptSaveList[i]=i;
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
        MySetRMVector( i,
                PMData->orig_vects[i].s.segment,
                PMData->orig_vects[i].s.offset );
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
        DoRawSwitchToRM( RMData.segm.rm, offsetof( rm_data, stack ) + STACK_SIZE, RM_OFF( RawSwitchHandler ) );
        for( i = 0; i < NUM_PM_SAVE_EXCEPTS; ++i ) {
            SavePMExcepts[i] = DPMIGetPMExceptionVector(PMExceptSaveList[i]);
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
    char                __far *envptr;

    envptr = envarea;
    while( *envptr ) {
        envptr += _fstrlen( envptr ) + 1;
    }
    return( envptr - envarea + 1 );
}

static char *CopyEnv( void )
{
    char                __far *envarea;
    uint_16             envsize;

#ifdef __OSI__
    {
        extern char *_EnvPtr;
        envarea = _EnvPtr;
    }
#else
    envarea = MK_FP( *(addr_seg __far *)MK_FP( _psp, PSP_ENVSEG_OFF ), 0 );
#endif
    envsize = EnvAreaSize( envarea );
    PMData->envseg.dpmi_adr = DPMIAllocateDOSMemoryBlock( _NBPARAS( envsize ) );
    if( PMData->envseg.segm.pm == 0 ) {
        return( TC_ERR_OUT_OF_DOS_MEMORY );
    }
    _fmemcpy( MK_PM( PMData->envseg.segm.rm, 0 ), envarea, envsize );
    return( NULL );
}

static char *SetTrapHandler( void )
{
    char                dummy;
    long                result;
    descriptor          desc;
    version_info        ver;

    PMData->vecttable1[DOS4G_COMM_VECTOR].s.segment = RMData.segm.rm;
    PMData->vecttable1[DOS4G_COMM_VECTOR].s.offset = RM_OFF( Interrupt15 );
    PMData->vecttable2[DOS4G_COMM_VECTOR].s.segment = RMData.segm.rm;
    PMData->vecttable2[DOS4G_COMM_VECTOR].s.offset = RM_OFF( Interrupt15 );
    if( IntrState == IS_NONE ) {
        DPMIGetVersion( &ver );
        if( (ver.major_version >= 1 || ver.minor_version > 90) || DPMICheck == 2 ) {
            RawPMtoRMSwitchAddr = DPMIRawPMtoRMAddr();
            PMData->switchaddr.a= DPMIRawRMtoPMAddr();
        }
        if( RawPMtoRMSwitchAddr == 0
         || PMData->switchaddr.a == 0
         || DPMICheck == 1 ) {
            IntrState = IS_RATIONAL;
        } else {
            PMData->saveaddr.a = DPMISavePMStateAddr();
            PMData->savesize   = DPMISaveStateSize();
            if( PMData->savesize == 0 ) {
                PMData->saveseg.dpmi_adr = 0;
            } else {
                PMData->saveseg.dpmi_adr = DPMIAllocateDOSMemoryBlock(
                                            _NBPARAS(PMData->savesize*2) );
                if( PMData->saveseg.segm.pm == 0 ) {
                    return( TC_ERR_OUT_OF_DOS_MEMORY );
                }
            }
            PMData->othersaved = FALSE;
            result = DPMIAllocateLDTDescriptors( 1 );
            if( result < 0 ) {
                return( TC_ERR_CANT_LOAD_TRAP );
            }
            DPMIGetDescriptor( FP_SEG( PMData ), &desc );
            PMData->pmode_cs   = (unsigned_16)result;
            desc.xtype.use32 = 0;
            desc.type.execute = 1;
            DPMISetDescriptor( PMData->pmode_cs, &desc );
            PMData->pmode_eip  = RM_OFF( BackFromRealMode );
            PMData->pmode_ds   = FP_SEG( &PMData );
            PMData->pmode_es   = PMData->pmode_ds;
            PMData->pmode_ss   = FP_SEG( &dummy );
            IntrState = IS_DPMI;
        }
    }
    if( IntrState == IS_RATIONAL ) {
        MySetRMVector( TRAP_VECTOR, RMData.segm.rm, RM_OFF( RMTrapHandler ) );
    }
    return( NULL );
}

static bool CallTrapInit( char *parm, char *errmsg, trap_version *trap_ver )
{
    _Packed struct {
        unsigned_16     remote;
        unsigned_16     retcode;
        trap_version    version;
        addr32_off      errmsg_off;
    }                   __far *callstruct;

    callstruct = (void __far *)PMData->parmarea;
    callstruct->remote = trap_ver->remote;
    if( parm == NULL ) parm = "";
    _fstrcpy( (char __far *)&callstruct[1], parm );
    callstruct->errmsg_off = sizeof( *callstruct ) + strlen( parm ) + 1;
    GoToRealMode( RMTrapInit );
    *trap_ver = callstruct->version;
    _fstrcpy( errmsg, (char __far *)callstruct + callstruct->errmsg_off );
    return( *errmsg == NULLCHAR );
}

static char *ReadInTrap( tiny_handle_t fh )
{
    dos_exe_header      hdr;
    memptr              relocbuff[NUM_BUFF_RELOCS];
    unsigned            relocnb;
    unsigned            imagesize;
    unsigned            hdrsize;
    rm_call_struct      read;
    unsigned            offset;

    if( TINY_ERROR( TinyRead( fh, &hdr, sizeof( hdr ) ) ) ) {
        return( TC_ERR_CANT_LOAD_TRAP );
    }
    if( hdr.signature != DOS_SIGNATURE ) {
        return( TC_ERR_BAD_TRAP_FILE );
    }

    hdrsize = hdr.hdr_size * 16;
    imagesize = (hdr.file_size * 0x200) - (-hdr.mod_size & 0x1ff) - hdrsize;
    TrapMem.dpmi_adr = DPMIAllocateDOSMemoryBlock( _NBPARAS( imagesize ) + hdr.min_16 );
    if( TrapMem.segm.pm == 0 ) {
        return( TC_ERR_OUT_OF_DOS_MEMORY );
    }
    TinySeek( fh, hdrsize, TIO_SEEK_SET );

    memset( &read, 0, sizeof( read ) );
    offset = 0;
    for( ;; ) {
        read.ss = RMData.segm.rm;
        read.sp = offsetof( rm_data, stack ) + STACK_SIZE;
        read.edx = offset;
        read.ebx = fh;
        read.ds = TrapMem.segm.rm;
        read.ecx = imagesize - offset;
        read.eax = 0x3f00;
#if 1
        relocnb = DPMISimulateRealModeInterrupt( 0x21, 0, 0, &read );
        if( (read.flags & 1) || (unsigned_16)read.eax == 0 ) {
            return( TC_ERR_CANT_LOAD_TRAP );
        }
#else
        read.eax = TinyRead( fh, (void *)((TrapMem.segm.rm << 4) + offset), imagesize - offset );
        if( (signed_32)read.eax < 0 ) {
            return( TC_ERR_CANT_LOAD_TRAP );
        }
#endif
        offset += (unsigned_16)read.eax;
        if( offset == imagesize ) break;
    }
    TinySeek( fh, hdr.reloc_offset, TIO_SEEK_SET );
    for( relocnb = NUM_BUFF_RELOCS; hdr.num_relocs > 0;
         --hdr.num_relocs, ++relocnb ) {
        if( relocnb >= NUM_BUFF_RELOCS ) {
            if( TINY_ERROR( TinyRead( fh, relocbuff, sizeof( memptr ) *
                                      NUM_BUFF_RELOCS ) ) ) {
                return( TC_ERR_CANT_LOAD_TRAP );
            }
            relocnb = 0;
        }
        *(addr_seg __far *)MK_PM( TrapMem.segm.rm + relocbuff[relocnb].s.segment,
                      relocbuff[relocnb].s.offset ) += TrapMem.segm.rm;
    }
    return( NULL );
}

static trap_retval DoTrapAccess( trap_elen num_in_mx, mx_entry_p mx_in, trap_elen num_out_mx, mx_entry_p mx_out )
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
    callstruct->in.ptr.s.segment = RMData.segm.rm;
    callstruct->in.ptr.s.offset = (unsigned)msgptr - (unsigned)PMData;
    callstruct->in.len = 0;
    for( j = 0; j < num_in_mx; ++j ) {
        _fmemcpy( msgptr, mx_in[j].ptr, mx_in[j].len );
        callstruct->in.len += mx_in[j].len;
        msgptr += mx_in[j].len;
    }

    callstruct->out.len = 0;
    if( mx_out != NULL ) {
        callstruct->out.ptr.s.segment = RMData.segm.rm;
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
    if( *(access_req *)mx_in->ptr == REQ_CONNECT ) {
        if( ( (connect_ret *)mx_out->ptr )->max_msg_size > MAX_MSG_SIZE ) {
            ( (connect_ret *)mx_out->ptr )->max_msg_size = MAX_MSG_SIZE;
        }
    }
    return( callstruct->retlen );
}

char *LoadTrap( char *trapbuff, char *buff, trap_version *trap_ver )
{
    char                *err;
    char                *parm;
    char                *end;
    dig_fhandle         dh;
    trap_file_header    __far *head;


    if( trapbuff == NULL ) {
        trapbuff = DEFAULT_TRP_NAME;
    }
    end = strchr( trapbuff, PARM_SEPARATOR );
    if( end == NULL ) {
        end = &trapbuff[strlen( trapbuff )];
        parm = end;
    } else {
        parm = end + 1;
    }
    dh = DIGPathOpen( trapbuff, end - trapbuff, DEFAULT_TRP_EXT, NULL, 0 );
    if( dh == DIG_NIL_HANDLE ) {
        sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trapbuff );
        return( buff );
    }
    err = ReadInTrap( DIGGetSystemHandle( dh ) );
    DIGPathClose( dh );
    sprintf( buff, TC_ERR_CANT_LOAD_TRAP, trapbuff );
    if( err == NULL ) {
        if( (err = SetTrapHandler()) != NULL || (err = CopyEnv()) != NULL ) {
            strcpy( buff, err );
        } else {
            strcpy( buff, TC_ERR_WRONG_TRAP_VERSION );
            head = MK_PM( TrapMem.segm.rm, 0 );
            if( head->sig == TRAP_SIGNATURE ) {
                PMData->initfunc.s.offset = head->init;
                PMData->reqfunc.s.offset  = head->req;
                PMData->finifunc.s.offset = head->fini;
                PMData->initfunc.s.segment = TrapMem.segm.rm;
                PMData->reqfunc.s.segment  = TrapMem.segm.rm;
                PMData->finifunc.s.segment = TrapMem.segm.rm;
                if( CallTrapInit( parm, buff, trap_ver ) ) {
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
    if( TrapMem.segm.pm != 0 ) {
        DPMIFreeDOSMemoryBlock( TrapMem.segm.pm );
    }
    if( PMData->envseg.segm.pm != 0 ) {
        DPMIFreeDOSMemoryBlock( PMData->envseg.segm.pm );
    }
    if( PMData->saveseg.segm.pm != 0 ) {
        DPMIFreeDOSMemoryBlock( PMData->saveseg.segm.pm );
    }
}
