/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Win386 Supervisor 32-bit DPMI mode startup code (16-bit).
*
****************************************************************************/


#include <stddef.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <io.h>
#include <fcntl.h>
#ifndef DLL32
#include <stdarg.h>
#else
typedef char __far *va_list[1];
#define va_start(ap,pn) ((ap)[0]=(char __far*)&pn+((sizeof(pn)+1)&~1),(void)0)
#define va_arg(ap,type) ((ap)[0]+=((sizeof(type)+1)&~1),\
                        (*(type __far *)((ap)[0]-((sizeof(type)+1)&~1))))
#define va_end(ap)      ((ap)[0]=0,(void)0)
#endif

#include <dos.h>
#include <share.h>
#include <sys/stat.h>
#include <windows.h>
#include "winext.h"
#include "watcom.h"
#include "tinyio.h"
#include "wdebug.h"
#include "windpmi.h"
#include "wininit.h"
#include "windata.h"

// define FLAT to 1 to get flat address space for entire memory
#define FLAT    1

#define READSIZE        (0x8000)
#define FPU_AREA        0x40            // offset into stack for fpu

struct  fpu_area {
    unsigned short      control_word;
    unsigned short      unused1;
    unsigned short      status_word;
    unsigned short      unused2;
    unsigned short      tag_word;
    unsigned short      unused3;
    unsigned long       ip_offset;
    unsigned short      cs_selector;
    unsigned short      unused4;
    unsigned long       data_operand_offset;
    unsigned short      operand_selector;
    unsigned short      unused5;
    unsigned short      fpregs[5 * 8];
};


extern void BreakPoint( void );
#pragma aux BreakPoint = "int 3";

#include "pushpck1.h"
struct wstart_vars {
    WORD        thishandle;
    WORD        prevhandle;
    DWORD       cmdline;
    WORD        cmdshow;
    WORD        _no87;
    WORD        _A000H;
    WORD        _B000H;
    WORD        _B800H;
    WORD        _C000H;
    WORD        _D000H;
    WORD        _E000H;
    WORD        _F000H;
    addr_48     CodeSelectorBase;
    addr_48     DataSelectorBase;
    addr_48     _32BitCallBackAddr;
    addr_48     _DLLEntryAddr;
    addr_48     _WEPAddr;
    void        (FAR *_16BitCallBackAddr)();
    addr_48     gluertns[5];
};
#include "poppck.h"

extern DWORD    __far deadbeef;
extern WORD     InDebugger;
extern WORD     _A000H;
extern WORD     _B000H;
extern WORD     _B800H;
extern WORD     _C000H;
extern WORD     _D000H;
extern WORD     _E000H;
extern WORD     _F000H;
extern BYTE     _no87;

extern void     FAR __CallBack( void );

static void     CodeRelocate( DWORD __far *reloc, WORD cnt );

static char     outOfSelectors[] = "Out of selectors";

/*
 * dwordToStr - convert a DWORD to a string (base 10)
 */
static char _FAR *dwordToStr( DWORD value )
{
    static char buff[16];
    int         i;

    i = 15;
    buff[i] = 0;
    do {
        --i;
        buff[i] = (value % 10) + '0';
        value /= 10;
    } while( value != 0 );
    return( &buff[i] );

} /* dwordToStr */

/*
 * Init32BitTask - load and initialize the 32-bit application
 */
int Init32BitTask( HINSTANCE thishandle, HINSTANCE prevhandle, LPSTR cmdline,
                    int cmdshow )
{
    WORD                i,amount,bytes_read,j;
    WORD                sel;
    int                 handle;
    tiny_ret_t          rc;
    DWORD               size,currsize,curroff,minmem,maxmem;
    DWORD               relsize,exelen;
    struct wstart_vars  __far *dataptr;
    void                __far *aliasptr;
    DWORD               __far *relptr;
    struct fpu_area     __far *fpuptr;
    rex_exe             exe;
    exe_data            exedat;
    char                file[128];
    DWORD               flags;
    version_info        vi;
    DWORD               file_header_size;
    BOOL                tried_global_compact;
    DWORD               save_maxmem;
    dpmi_mem_block      adata;

    flags = GetWinFlags();
    /*
     * verify that we are running on a 32-bit DPMI
     */
    _fDPMIGetVersion( &vi );
    if( !(vi.flags & VERSION_80386) ) {
        MessageBox( NULL, "Not running on a 386 DPMI implementation",MsgTitle,
                        MB_OK | MB_ICONHAND | MB_TASKMODAL );
        return( FALSE );
    }

    /*
     * get exe to load
     */
    GetModuleFileName( thishandle, file, 128 );
    rc = _fTinyOpen( file, TIO_READ );
    if( TINY_ERROR( rc ) ) {
        return( Fini( 2, (char _FAR *)"Error opening file",
                    (char _FAR *)file) );
    }
    handle = TINY_INFO( rc );

    _TinySeek( handle, 0x38, TIO_SEEK_START );
    _fTinyRead( handle, &exelen, sizeof( DWORD ) );
    _TinySeek( handle, exelen, TIO_SEEK_START );

    /*
     * check if we are being run by the debugger.  When the debugger
     * sees the 'DEADBEEF' bytes at the start of the code segment
     * (see begin.asm), it swaps them to be BEEFDEAD.  The debugger
     * then tells us to go, and if we see that we have BEEFDEAD,
     * we execute a breakpoint just before we call our 32-bit code.
     * Then the debugger traces a single instruction and it looks to
     * the user like the start of his/her code is the start of
     * the application.
     */
    if( deadbeef == 0xBEEFDEAD ) {
        InDebugger = 0;
    } else {
        InDebugger = 1;
    }
    DPL = (CS() & 0x03) << 5;   /* our privilege level */

    /*
     * validate header signature
     */
    _fTinyRead( handle, &exe, sizeof( rex_exe ) );
//    BreakPoint();
    if( !(exe.sig[0] == 'M' && exe.sig[1] == 'Q') ) {
        return( Fini( 1,(char _FAR *)"Invalid EXE" ) );
    }
    file_header_size = (DWORD) exe.file_header * 16L;
    /*
     * exe.one is supposed to always contain a 1 for a .REX file.
     * However, to allow relocation tables bigger than 64K, the
     * we extended the linker to have the .one field contain the
     * number of full 64K chunks of relocations, minus 1.
     */
    file_header_size += (exe.one-1)*0x10000L*16L;

    /*
     * get exe data - data start and stack start
     */
    _TinySeek( handle, exelen + file_header_size + (long)exe.initial_eip, TIO_SEEK_START );
    _fTinyRead( handle, &exedat, sizeof( exe_data ) );
    /*
     * get file size
     */
    size = (long) exe.file_size2 * 512L;
    if( exe.file_size1 > 0 ) {
        size += (long) exe.file_size1 - 512L;
    }

    /*
     * get stack size
     */
    StackSize = Align4K( exe.initial_esp - ((exedat.stackstart+15) & ~15ul) );
    if( StackSize < 0x1000 ) {
        StackSize = 0x1000;
    }

    /*
     * get minimum/maximum amounts of heap, then add in exe size
     * to get total area
     */
//    BreakPoint();
    minmem = (DWORD) exe.min_data *(DWORD) 4096L;
    if( exe.max_data == (WORD)-1 ) {
        maxmem = 4096L;
    } else {
        maxmem = (DWORD) exe.max_data*4096L;
    }
    minmem = Align4K( minmem + size + 0x10000ul );
    maxmem = Align4K( maxmem + size + 0x10000ul );
    if( minmem > maxmem ) {
        maxmem = minmem;
    }

    /*
     * get memory to load file
     */
    tried_global_compact = FALSE;
    save_maxmem = maxmem;
    for(;;) {
        i = DPMIGet32( &adata, maxmem );
        if( i == 0 )  break;
        if( maxmem == minmem ) {
            if( tried_global_compact ) {
                return( Fini( 3,
                  (char _FAR *)"Not enough memory for application\n(minimum ",
                  dwordToStr( minmem ),(char _FAR *)" required)" ));
            }
            /*
             * GlobalCompact(-1) causes Windows to unfragment its
             * memory.  This might give us a chance to get a linear
             * chunk big enough
             */
            GlobalCompact( GlobalCompact( 0 ) );
            maxmem = save_maxmem;
            tried_global_compact = TRUE;
        } else if( maxmem < 64L * 1024L ) {
            maxmem = minmem;
        } else {
            maxmem -= 64L * 1024L;
            if( maxmem < minmem ) {                     /* 09-aug-93 */
                maxmem = minmem;
            }
        }
    }
    DataHandle = adata.handle;
    BaseAddr   = adata.linear + 0x10000ul;
#if FLAT
    i = InitFlatAddrSpace( BaseAddr, 0L );
#else
    i = InitFlatAddrSpace( BaseAddr, maxmem );
#endif
    BaseAddr = 0L;
    if( i ) {
        DPMIFreeMemoryBlock( DataHandle );
        return( Fini( 2,(char _FAR *)"Allocation error ", dwordToStr( i ) ) );
    }
    SaveSP = BaseAddr + StackSize;
    CodeLoadAddr = SaveSP;
    MyDataSelector = DataSelector;
    GetDataSelectorInfo();
    CodeEntry.off = exe.initial_eip + CodeLoadAddr + sizeof( exe_data );

    /*
     * this builds a collection of LDT selectors that are ready for
     * allocation
     */
    if( InitSelectorCache() != 0 ) {
        return( Fini( 1,(char _FAR *)outOfSelectors) );
    }

    /*
     * read the exe into memory
     */
    currsize = size - file_header_size;
    _TinySeek( handle, exelen + file_header_size, TIO_SEEK_START );
    i = DPMIGetAliases( CodeLoadAddr, (LPDWORD) &aliasptr, 0 );
    if( i ) {
        return( Fini( 3,(char _FAR *)"Error ",
                dwordToStr( i ),
                (char _FAR *)" getting alias for read" ) );
    }
    dataptr = aliasptr;
    sel = ((DWORD) dataptr) >> 16;
    curroff = CodeLoadAddr;
    while( currsize != 0 ) {

        if( currsize >= (DWORD) READSIZE ) {
            amount = READSIZE;
        } else {
            amount = (WORD) currsize;
        }
        rc = _fTinyRead( handle, dataptr, amount );
        bytes_read = TINY_INFO( rc );
        if( bytes_read != amount ) {
            return( Fini( 1,(char _FAR *)"Read error" ) );
        }
        currsize -= (DWORD) amount;
        curroff += (DWORD) amount;
        DPMISetSegmentBaseAddress( sel, DataSelectorBase + curroff );
    }
    EDataAddr = curroff;                        // 03-jan-95

    DPMISetSegmentBaseAddress( sel, DataSelectorBase );
    relptr = (DWORD __far *)aliasptr;             // point to 32-bit stack area
    /*
     * get and apply relocation table
     */
    relsize = sizeof( DWORD ) * (DWORD) exe.reloc_cnt;
    {
        DWORD   realsize;
        WORD    kcnt;

        realsize = file_header_size - (DWORD) exe.first_reloc;
        kcnt = realsize / (0x10000L*sizeof(DWORD));
        relsize += kcnt * (0x10000L*sizeof(DWORD));
    }
    if( relsize != 0 ) {
        _TinySeek( handle, exelen + (DWORD)exe.first_reloc, TIO_SEEK_START );
        if( StackSize >= (DWORD) READSIZE ) {
            amount = READSIZE;
        } else {
            amount = (WORD) StackSize;
        }
        while( relsize != 0L ) {
            if( relsize < (DWORD)amount ) {
                amount = (WORD) relsize;
            }
            rc = _fTinyRead( handle, relptr, amount );
            bytes_read = TINY_INFO( rc );
            if( bytes_read != amount ) {
                return( Fini( 1,(char _FAR *)"Relocation read error" ) );
            }
            CodeRelocate( relptr, amount/sizeof(DWORD) );
            relsize -= (DWORD) amount;
        }
    }

    _TinyClose( handle );

    /* initialize emulator 8087 save area 20-oct-94 */

    fpuptr = (struct fpu_area __far *)((char __far *)aliasptr + FPU_AREA);
    _fmemset( fpuptr, 0, sizeof(struct fpu_area) );
    fpuptr->control_word = 0x033F;
    fpuptr->tag_word = 0xFFFF;

    /*
     * set dataptr to special area in data segment of 32-bit app
     */
    curroff = exedat.datastart;
    if( exe.reloc_cnt != 0 )  curroff += CodeLoadAddr;
    DPMISetSegmentBaseAddress( sel, DataSelectorBase + curroff );

    /*
     * insert command line parms
     */
    dataptr->thishandle = (WORD)thishandle;
    dataptr->prevhandle = (WORD)prevhandle;
    dataptr->cmdline    = (DWORD) cmdline;
    dataptr->cmdshow    = cmdshow;
    dataptr->_no87      = _no87;

    /*
     * set hardware selectors for screen memory
     */
    dataptr->_A000H     = (WORD) &_A000H;
    dataptr->_B000H     = (WORD) &_B000H;
    dataptr->_B800H     = (WORD) &_B800H;
    dataptr->_C000H     = (WORD) &_C000H;
    dataptr->_D000H     = (WORD) &_D000H;
    dataptr->_E000H     = (WORD) &_E000H;
    dataptr->_F000H     = (WORD) &_F000H;

    /*
     * ptrs to some data areas
     */
    dataptr->CodeSelectorBase.seg =  (WORD) FP_SEG( &CodeSelectorBase );
    dataptr->CodeSelectorBase.off = (DWORD) FP_OFF( &CodeSelectorBase );
    dataptr->DataSelectorBase.seg =  (WORD) FP_SEG( &DataSelectorBase );
    dataptr->DataSelectorBase.off = (DWORD) FP_OFF( &DataSelectorBase );
    dataptr->_32BitCallBackAddr.seg =  (WORD) FP_SEG( &_32BitCallBackAddr );
    dataptr->_32BitCallBackAddr.off = (DWORD) FP_OFF( &_32BitCallBackAddr );
    dataptr->_DLLEntryAddr.seg =  (WORD) FP_SEG( &_DLLEntryAddr );
    dataptr->_DLLEntryAddr.off = (DWORD) FP_OFF( &_DLLEntryAddr );
    dataptr->_WEPAddr.seg =  (WORD) FP_SEG( &_WEPAddr );
    dataptr->_WEPAddr.off = (DWORD) FP_OFF( &_WEPAddr );
    dataptr->_16BitCallBackAddr = &__CallBack;

    /*
     * insert glue routines into data area of caller
     */
    for( j = 0; j < MaxGlueRoutines; j++ ) {
        dataptr->gluertns[j].seg =  (WORD) FP_SEG( Glue[j].rtn );
        dataptr->gluertns[j].off = (DWORD) FP_OFF( Glue[j].rtn );
    }
    DPMIFreeAlias( sel );

    /*
     * check for FPU and WGod
     */
    if( flags & WF_80x87 ) {
        Has87 = TRUE;
    } else {
        Has87 = FALSE;
    }
    if( CheckWin386Debug() == WGOD_VERSION ) {
//    BreakPoint();
        HasWGod = TRUE;
        if( !Has87 ) {
            EMUInit();
            EMURegister( CodeEntry.seg, SaveSP - StackSize + FPU_AREA );
        }
    } else {
        HasWGod = FALSE;
    }
    return( TRUE );

} /* Init32BitTask */

extern void RelocateDWORD( short, long, long );
extern void RelocateWORD( short, long, short );

#pragma aux RelocateDWORD = \
        "mov     es,si" \
        "shl     edx,16" \
        "mov     dx,ax" \
        "shl     ecx,16" \
        "mov     cx,bx" \
        "add     es:[edx],ecx" \
        parm [si] [ax dx] [bx cx] modify[es];

#pragma aux RelocateWORD = \
        "mov     es,si" \
        "shl     edx,16" \
        "mov     dx,ax" \
        "add     es:[edx],bx" \
        parm [si] [ax dx] [bx] modify[es];

/*
 * CodeRelocate - relocate a given chunk of code
 */
static void CodeRelocate( DWORD __far *reloc, WORD cnt )
{
    WORD        i;
    DWORD       tmp;

    for( i=0;i<cnt;i++) {
        tmp = reloc[i] & 0x7fffffff;
        tmp += CodeLoadAddr;
        if( reloc[i] & 0x80000000 ) {
            RelocateDWORD( DataSelector, tmp, CodeLoadAddr );
        } else {
            RelocateWORD( DataSelector, tmp, CodeLoadAddr );
        }
    }

} /* CodeRelocate */

/*
 * Cleanup - perform all cleanups
 */
void Cleanup( void )
{

    FiniSelectorCache();
    FiniSelList();
    FiniDLLs();
    FreeDPMIMemBlocks();

    if( DataSelector != 0 ) {
        DPMIFree32( DataHandle );
    }

    /*
     * clean up FPU stuff
     */
    if( HasWGod && !Has87 ) {
        EMUUnRegister( CodeEntry.seg );
        EMUShutdown();
    }

} /* Cleanup */

static char doneFini=FALSE;

/*
 * Fini - clean up after an error
 */
int Fini( int strcnt, ... )
{
#ifndef DLL32
    char        tmp[128];
    va_list     arg;
    char        _FAR *n;
#endif

    if( doneFini ) {
        return( 0 );
    }
    doneFini = TRUE;

#ifndef DLL32
    va_start( arg, strcnt );
    tmp[0] = 0;
    while( strcnt > 0 ) {
        n = va_arg( arg, char _FAR * );
        strcat( tmp, n );
        strcnt--;
    }
    va_end( arg );
    if( tmp[0] != 0 ) {
        MessageBox( NULL, tmp, MsgTitle, MB_OK | MB_ICONHAND | MB_TASKMODAL );
    }
#else
    strcnt = strcnt;
#endif
    Cleanup();
    return( FALSE );

} /* Fini */
