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
* Description:  Initialize MBCS support.
*
****************************************************************************/


#include "variety.h"
#ifdef __NT__
    #include <windows.h>
    #include <winnls.h>
#elif defined __OS2__
    #define INCL_DOSNLS
    #include <wos2.h>
#elif defined __OSI__
#elif defined __DOS__
    #include <dos.h>
    #ifndef __386__
        #include <i86.h>
    #else
        #include "dpmi.h"
        #include "extender.h"
    #endif
#elif defined __WINDOWS__
    #include <windows.h>
#elif defined __LINUX__
#endif
#include <string.h>
#include <mbstring.h>
#include "mbchar.h"


#if defined(__DOS__) && !defined(__OSI__)
    unsigned short _WCFAR * dos_get_dbcs_lead_table( void );
#endif

#if defined(__DOS__) || defined(__WINDOWS__)
    unsigned short          dos_get_code_page( void );
#endif


#ifdef __NT__
    unsigned int __MBCodePage = CP_OEMCP;       /* default code page */
#elif defined( __LINUX__ )
    unsigned int __MBCodePage = 0;              /* default code page */
#elif !defined(__UNIX__)
    unsigned int __MBCodePage = 0;              /* default code page */
#endif

static void set_dbcs_table( int low, int high )
{
    memset( __MBCSIsTable + low + 1, _MB_LEAD, high - low + 1 );
}

static void clear_dbcs_table( void )
{
    __IsDBCS = 0;                           /* SBCS for now */
    __MBCodePage = 0;
    memset( __MBCSIsTable, 0, 257 );
}

/****
***** Initialize a multi-byte character set.  Returns 0 on success.
****/

int __mbinit( int codepage )
{
#ifdef __NT__
    int                     countRange;
    CPINFO                  cpInfo;
    BOOL                    rc;
#elif defined __OS2__
    int                     countRange;
    COUNTRYCODE             countryInfo;
    unsigned char           leadBytes[12];
    APIRET                  rc;
    OS_UINT                 buf[8];
    OS_UINT                 bytes;
#elif defined __OSI__
#elif defined __DOS__
    int                     countRange;
    unsigned short _WCFAR * leadBytes;
    unsigned char           lowerBound, upperBound;
#elif defined __WINDOWS__
    DWORD                   version;
    int                     countVal;
#elif defined __LINUX__
#endif

    /*** Handle values from _setmbcp ***/
    if( codepage == _MBINIT_CP_ANSI ) {
        #ifdef __NT__
            codepage = GetACP();
        #else
            codepage = 0;
        #endif
    } else if( codepage == _MBINIT_CP_OEM ) {
        #ifdef __NT__
            codepage = GetOEMCP();
        #else
            codepage = 0;
        #endif
    } else if( codepage == _MBINIT_CP_SBCS ) {
        clear_dbcs_table();
        return( 0 );
    } else if( codepage == _MBINIT_CP_932 ) {
        clear_dbcs_table();
        set_dbcs_table( 0x81, 0x9F );
        set_dbcs_table( 0xE0, 0xFC );
        __IsDBCS = 1;
        __MBCodePage = 932;
        return( 0 );
    }

    #ifdef __NT__
        /*** Initialize the __MBCSIsTable values ***/
        if( codepage == 0 )  codepage = CP_OEMCP;
        rc = GetCPInfo( codepage, &cpInfo );    /* get code page info */
        if( rc == FALSE )  return( 1 );
        clear_dbcs_table();
        if( cpInfo.LeadByte[0] )  __IsDBCS = 1; /* set __IsDBCS if needed */
        for( countRange=0; !(cpInfo.LeadByte[countRange]==0x00 &&
             cpInfo.LeadByte[countRange+1]==0x00); countRange+=2 ) {
            set_dbcs_table( cpInfo.LeadByte[countRange],
                            cpInfo.LeadByte[countRange+1] );
        }
        /*** Update __MBCodePage ***/
        if( codepage == CP_OEMCP ) {
            __MBCodePage = GetOEMCP();
        } else {
            __MBCodePage = codepage;
        }
    #elif defined __OS2__
        /*** Initialize the __MBCSIsTable values ***/
        countryInfo.country = 0;                /* default country */
        countryInfo.codepage = codepage;        /* specified code page */
        #if defined(__WARP__)
            rc = DosQueryDBCSEnv( 12, &countryInfo, (PCHAR)leadBytes );
        #else
            rc = DosGetDBCSEv( 12, &countryInfo, (PCHAR)leadBytes );
        #endif
        if( rc != 0 )  return( 1 );
        clear_dbcs_table();
        if( leadBytes[0] )  __IsDBCS = 1;       /* set __IsDBCS if needed */
        for( countRange=0; !(leadBytes[countRange]==0x00 &&
             leadBytes[countRange+1]==0x00); countRange+=2 ) {
            set_dbcs_table( leadBytes[countRange],
                            leadBytes[countRange+1] );
        }
        /*** Update __MBCodePage ***/
        if( codepage == 0 ) {
            #if defined(__386__) || defined(__PPC__)
                rc = DosQueryCp( sizeof(buf), &buf, &bytes );
            #else
                rc = DosGetCp( sizeof(buf), &buf, &bytes );
            #endif
            if( rc != 0 ) {
                __MBCodePage = 0;
            } else {
                __MBCodePage = (unsigned int)buf[0];
            }
        } else {
            __MBCodePage = codepage;
        }
    #elif defined __OSI__
    #elif defined __DOS__
        /*** Initialize the __MBCSIsTable values ***/
        if( codepage != 0 )
            return( 1 );       /* can only handle default */
        leadBytes = dos_get_dbcs_lead_table();
        if( leadBytes == NULL )
            return( 0 );
        clear_dbcs_table();
        if( leadBytes[0] )
            __IsDBCS = 1;       /* set __IsDBCS if needed */
        for( countRange=0; leadBytes[countRange]!=0x0000; countRange++ ) {
            lowerBound = (unsigned char) leadBytes[countRange];
            upperBound = (unsigned char) (leadBytes[countRange] >> 8);
            set_dbcs_table( lowerBound, upperBound );
        }
        __MBCodePage = dos_get_code_page();
    #elif defined __WINDOWS__
        /*** Initialize the __MBCSIsTable values ***/
        if( codepage != 0 )  return( 1 );       /* can only handle default */
        version = GetVersion();
        if( LOWORD(version) < 0x0A03 )  return( 1 );    /* 3.1+ needed */
        clear_dbcs_table();
        for( countVal=0; countVal<256; countVal++ ) {
            if( IsDBCSLeadByte( (BYTE)countVal ) ) {
                __MBCSIsTable[countVal+1] = _MB_LEAD;
                __IsDBCS = 1;                   /* set __IsDBCS if needed */
            }
        }
        __MBCodePage = GetKBCodePage();
    #elif defined __LINUX__
    #endif

    return( 0 );                                /* return success code */
}



/****
***** Query DOS to find the valid lead byte ranges.
****/

#if defined(__DOS__) && !defined(__OSI__)
#ifndef __386__

// Some DOS implementation do nothing but do not return an error (NTVDM).
// Some versions report success but do not modify registers (US DOS 3.3).
// We set DS to zero and if it stays unchanged, consider that a failure.
#if 1
#pragma aux             dos_get_dbcs_lead_table = \
        "push ds"       \
        "xor ax,ax"     \
        "mov ds,ax"     \
        "mov ah,63h"    /* get DBCS vector table */ \
        "int 21h"       \
        "mov di,ds"     \
        "jnc label1"    \
        "xor di,di"     \
        "label1:"       \
        "test di,di"    \
        "jnz exit1"     \
        "mov si,di"     \
        "exit1:"        \
        "pop ds"        \
        value           [di si] \
        modify          [ax bx cx dx si di es];
#else
unsigned short _WCFAR *dos_get_dbcs_lead_table( void )
/****************************************************/
{
    union REGS        regs;
    struct SREGS      sregs;

    regs.w.ax = 0x6300;                     /* get lead byte table code */
    sregs.ds = 0;
    sregs.es = 0;
    intdosx( &regs, &regs, &sregs );        /* call DOS */
    if( regs.w.cflag || ( sregs.ds == 0 ))  /* ensure function succeeded */
        return( NULL );
    return( MK_FP( sregs.ds, regs.w.si ) ); /* return pointer to table */
}
#endif

#if 0
unsigned short dos_get_code_page( void )
/**************************************/
{
    union REGS          regs;
    struct SREGS        sregs;
    unsigned char       buf[7];

    regs.w.ax = 0x6501;                     /* get international info */
    regs.w.bx = 0xFFFF;                     /* global code page */
    regs.w.cx = 7;                          /* buffer size */
    regs.w.dx = 0xFFFF;                     /* current country */
    regs.w.di = FP_OFF( (void __far*)buf ); /* buffer offset */
    sregs.es = FP_SEG( (void __far*)buf );  /* buffer segment */
    sregs.ds = 0;                           /* in protected mode (dos16m) DS must be initialized */
    intdosx( &regs, &regs, &sregs );        /* call DOS */
    if( regs.w.cflag )  return( 0 );        /* ensure function succeeded */
    return( * (unsigned short*)(buf+5) );   /* return code page */
}
#else
#pragma aux dos_get_code_page = \
        "push ds"       \
        "push bp"       \
        "mov bp,sp"     \
        "sub sp,8"      \
        "xor ax,ax"     \
        "mov ds,ax"     \
        "mov ax,6501h"  /* get international info */ \
        "mov bx,0ffffh" /* global code page */ \
        "mov cx,0007h"  /* buffer size */ \
        "mov dx,0ffffh" /* current country */ \
        "lea di,[bp-8]" /* buffer offset */ \
        "push ss"       \
        "pop es"        /* buffer segment */ \
        "int 21h"       /* call DOS */ \
        "mov ax,[bp-8+5]" /* code page */ \
        "jnc NoError"   \
        "xor ax,ax"     \
        "NoError:"      \
        "mov sp,bp"     \
        "pop bp"        \
        "pop ds"        \
        value           [ax] \
        modify          [ax bx cx dx di es];
#endif

#else


#pragma pack(__push,1);
typedef struct {
    unsigned short  int_num;
    unsigned short  real_ds;
    unsigned short  real_es;
    unsigned short  real_fs;
    unsigned short  real_gs;
    unsigned long   real_eax;
    unsigned long   real_edx;
} PHARLAP_block;
#pragma pack(__pop);

unsigned short _WCFAR *dos_get_dbcs_lead_table( void )
/****************************************************/
{
    if( _IsPharLap() ) {
        PHARLAP_block   pblock;
        union REGS      regs;
        struct SREGS    sregs;

        memset( &pblock, 0, sizeof( pblock ) );
        memset( &regs, 0, sizeof( regs ) );
        memset( &sregs, 0, sizeof( sregs ) );
        pblock.real_eax = 0x6300;           /* get DBCS vector table */
        pblock.int_num = 0x21;              /* DOS call */
        regs.x.eax = 0x2511;                /* issue real-mode interrupt */
        regs.x.edx = FP_OFF( &pblock );     /* DS:EDX -> parameter block */
        sregs.ds = FP_SEG( &pblock );
        intdosx( &regs, &regs, &sregs );
        if( pblock.real_ds != 0xFFFF ) {    /* weird OS/2 value */
            return( MK_FP( _ExtenderRealModeSelector,
                           (((unsigned)pblock.real_ds)<<4) + regs.w.si ) );
        }
    } else if( _IsRational() ) {
        rm_call_struct  dblock;

        memset( &dblock, 0, sizeof( dblock ) );
        dblock.eax = 0x6300;                /* get DBCS vector table */
        DPMISimulateRealModeInterrupt( 0x21, 0, 0, &dblock );
        if( (dblock.flags & 1) == 0 && dblock.ds ) {
            return( MK_FP( _ExtenderRealModeSelector,
                           (((unsigned)dblock.ds)<<4) + dblock.esi ) );
        }
    }
    return( NULL );
}

unsigned short dos_get_code_page( void )
/**************************************/
{
    unsigned short __far  * temp;
    unsigned short          real_seg;
    unsigned short          codepage = 0;


    /*** Get the code page ***/
    if( _IsPharLap() ) {
        union REGS      r;
        struct SREGS    sregs;
        PHARLAP_block   pblock;

        /*** Alloc DOS Memory under Phar Lap ***/
        memset( &r, 0, sizeof( r ) );
        memset( &sregs, 0, sizeof( sregs ) );
        r.x.ebx = 1;
        r.x.eax = 0x25c0;
        intdosx( &r, &r, &sregs );
        real_seg = r.w.ax;

        memset( &pblock, 0, sizeof( pblock ) );
        pblock.real_eax = 0x6501;           /* get international info */
        pblock.real_edx = 0xFFFF;           /* current country */
        pblock.real_es = real_seg;          /* buffer segment */
        r.x.ebx = 0xFFFF;                   /* global code page */
        r.x.ecx = 7;                        /* buffer size */
        r.x.edi = 0;                        /* buffer offset */
        pblock.int_num = 0x21;              /* DOS call */
        r.x.eax = 0x2511;                   /* issue real-mode interrupt */
        r.x.edx = FP_OFF( &pblock );        /* DS:EDX -> parameter block */
        sregs.ds = FP_SEG( &pblock );
        intdosx( &r, &r, &sregs );
        if( pblock.real_ds != 0xFFFF ) {    /* weird OS/2 value */
            temp = MK_FP( _ExtenderRealModeSelector, (real_seg<<4) + 5 );
            codepage = *temp;
        }

        /*** Free DOS Memory under Phar Lap ***/
        r.x.ecx = real_seg;
        r.x.eax = 0x25c1;
        intdosx( &r, &r, &sregs );
    } else if( _IsRational() ) {
        unsigned long       dpmi_rc;
        unsigned short      selector;
        rm_call_struct      dblock;

        /*** Allocate some DOS memory with DPMI ***/
        dpmi_rc = DPMIAllocateDOSMemoryBlock( 1 );      /* one paragraph is enough */
        real_seg = (unsigned short) dpmi_rc;
        selector = (unsigned short) (dpmi_rc>>16);

        memset( &dblock, 0, sizeof( dblock ) );
        dblock.eax = 0x6501;                /* get international info */
        dblock.ebx = 0xFFFF;                /* global code page */
        dblock.ecx = 7;                     /* buffer size */
        dblock.edx = 0xFFFF;                /* current country */
        dblock.edi = 0;                     /* buffer offset */
        dblock.es = real_seg;               /* buffer segment */
        DPMISimulateRealModeInterrupt( 0x21, 0, 0, &dblock );
        if( (dblock.flags & 1) == 0 ) {
            temp = MK_FP( _ExtenderRealModeSelector, (real_seg<<4) + 5 );
            codepage = *temp;
        }
        /*** Free DOS memory with DPMI ***/
        DPMIFreeDOSMemoryBlock( selector );
    }

    return( codepage );
}


#endif
#endif
