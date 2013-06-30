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
* Description:  Loader for OSI executables.
*
****************************************************************************/


#include <stdlib.h>
#include <stdarg.h>
#include <i86.h>
#include "tinyio.h"
#include <fcntl.h>
#include "wreslang.h"

#define COMPRESSION     0

#if defined(__OS2)
 #define INCL_DOSMISC
 #define INCL_DOSEXCEPTIONS
 #define INCL_DOSNLS
 #include <os2.h>
typedef unsigned short  WORD;
typedef unsigned long   DWORD;

#elif defined(__NT)
 #include <windows.h>
 #include <winnls.h>
 #include <excpt.h>

#else
typedef unsigned short  WORD;
typedef unsigned long   DWORD;
#endif

#include "loader.h"
#include "ostype.h"

#define READSIZE        (0x8000)
#define RELOC_NUM       1024

DWORD   BaseAddr;
DWORD   CodeLoadAddr;
DWORD   CodeEntryPoint;
char    BreakFlag;

int     __IsDBCS = 0;
int     MsgFileHandle   = 1;
#if defined(__OS2) || defined(__NT)
char    *ProgramName    = "";
char    *ProgramArgs    = "";
#endif
char    *ProgramEnv     = "\0";

#define Align4K( x ) (((x)+0xfffL) & ~0xfffL )

#pragma aux _end "*"
extern  char            _end;

extern  int     __fInt21( void );

struct  pgmparms {
        char    *pgmname;       // program name (argv[0])
        char    *cmdline;       // command line
        char    *envptr;        // pointer to environment strings
        char    *breakflagaddr; // pointer to breakflag
        char    *copyright;     // pointer to possible copyright string
        int     isDBCS;         // non-zero => DBCS
        DWORD   max_handle;     // OS2: max file handle count
};

extern  int _InvokePgm( char os,
                        DWORD baseaddr,
                        DWORD eip,
                        DWORD stacklow,
                        int (*int21)( void ),
                        struct pgmparms *parms );
#pragma aux _InvokePgm = "push  cs"     \
                         "mov   bx,cs"  \
                         "call  esi"    \
                         parm [ah] [ebx] [esi] [ecx] [edx] [edi] value [eax];

extern  int _LaunchPgm( DWORD baseaddr,
                        DWORD eip,
                        int (*int21)( void ),
                        struct pgmparms *parms );
#pragma aux _LaunchPgm parm [ebx] [esi] [edx] [edi] value [eax];

#define pick(name,msg)  name
#define E(msg) msg
#define J(msg)
enum {
 #include "ldrmsg.h"
};
#define pick(name,msg)  msg
#define E(msg) msg
#define J(msg)
char *EnglishMsgs[] = {
 #include "ldrmsg.h"
};
#define pick(name,msg)  msg
#define E(msg)
#define J(msg) msg
char *JapaneseMsgs[] = {
 #include "ldrmsg.h"
};

char *getenv( const char *var )
{
    char        *p;
    const char  *v;

    p = ProgramEnv;
    for( ;; ) {
        if( *p == '\0' ) break;
        v = var;
        for( ;; ) {
            if( *v == '\0' ) {
                if( *p == '=' )  return( p + 1 );
                break;
            }
            if( (*p | ' ') != (*v | ' ') ) break;
            ++p;
            ++v;
        }
        while( *p != '\0' ) ++p;
        ++p;
    }
    return( NULL );
}

char *PickMsg( int num )
{
    char        *msg;

    switch( _WResLanguage() ) {
    case RLE_JAPANESE:
        msg = JapaneseMsgs[num];
        break;
    default:
        msg = EnglishMsgs[num];
        break;
    }
    return( msg );
}

void PrintMsg( char *fmt, ... )
{
    char        c;
    int         i;
    char        *p;
    unsigned    value;
    va_list     args;
    int         len;
    char        buf[128];

    va_start( args, fmt );
    len = 0;
    for( ;; ) {
        c = *fmt++;
        if( c == '\0' ) break;
        if( c == '%' ) {
            c = *fmt++;
            if( c == 's' ) {
                p = va_arg( args, char * );
                for( ;; ) {
                    c = *p++;
                    if( c == '\0' ) break;
                    buf[len++] = c;
                }
            } else if( c == 'd' ) {
                i = va_arg( args, int );
                itoa( i, &buf[len], 10 );
                while( buf[len] != '\0' ) ++len;
            } else {
                value = va_arg( args, unsigned );
                i = 8;
                if( c == 'x' ) {
                    i = 4;
                    value <<= 16;
                } else if( c == 'h' ) {
                    i = 2;
                    value <<= 24;
                }
                for( ;; ) {
                    c = value >> 28;
                    value <<= 4;
                    if( c < 10 ) {
                        c += '0';
                    } else {
                        c += 'A' - 10;
                    }
                    buf[len++] = c;
                    --i;
                    if( i == 0 ) break;
                }
            }
        } else {
            buf[len++] = c;
        }
    }
    TinyWrite( MsgFileHandle, buf, len );
}


void DoRelocations( unsigned short *rel )
{
    unsigned short      count;
    unsigned long       addr;
    unsigned long       base_addr;

    base_addr = CodeLoadAddr;
    for( ;; ) {
        count = *rel++;
        if( count == 0 ) break;
        addr = *rel++ << 16;
        addr |= *rel++;
        addr += base_addr;
        for( ;; ) {
            *(unsigned long *)addr += base_addr;
            --count;
            if( count == 0 ) break;
            addr += *rel++;
        }
    }

} /* DoRelocations */

#if COMPRESSION
// extern void BPE_Expand( char *dst, char *src, char *srcend );
/*
 * BPE_Expand - Expand BPE file
 */
void BPE_Expand( char *dst, char *src, char *srcend )
{
    unsigned char       left[256];
    unsigned char       right[256];
    unsigned char       stack[30];
    int                 c;
    int                 count;
    int                 i;
    int                 size;

    /* Unpack each block until end of file */
    while( src != srcend ) {
        count = *src++;

        /* Set left to itself as literal flag */
        for( i = 0; i < 256; i++ ) {
            left[i] = i;
        }

        /* Read pair table */
        for( c = 0;; ) {
            /* Skip range of literal bytes */
            if( count > 127 ) {
                c += count - 127;
                count = 0;
            }
            if( c == 256 ) break;

            /* Read pairs, skip right if literal */
            for( i = 0; i <= count; i++, c++ ) {
                left[c] = *src++;
                if( c != left[c] ) {
                    right[c] = *src++;
                }
            }
            if( c == 256 )  break;
            count = *src++;
        }

        /* Calculate packed data block size */
        size = 256 * *src++;
        size += *src++;

        /* Unpack data block */
        i = 0;
        while( size != 0 ) {
            --size;
            c = *src++;
            for( ;; ) {
                while( c != left[c] ) {
                    stack[i++] = right[c];
                    c = left[c];
                }
                *dst++ = c;
                if( i == 0 ) break;
                c = stack[--i];
            }
        }
    }
}
#endif
/*
 * Init32BitTask - load and initialize the 32-bit application
 */
int Init32BitTask( char *file )
{
    int         handle;
    tiny_ret_t  rc;
    char        *small_chunks;
    DWORD       load_addr;
    DWORD       total_size;
    DWORD       read_size;
    struct w32_hdr *w32_hdr;
    union {
        dos_hdr dos_header;
        char    header[512];
    } u;

    rc = TinyOpen( file, TIO_READ );
    if( TINY_ERROR( rc ) ) {
        PrintMsg( PickMsg( LOADER_CANT_OPEN_EXE ), file, rc & 0x7fffffff );
        return( LOADER_CANT_OPEN_EXE );
    }
    handle = TINY_INFO( rc );

    TinyRead( handle, &u.header, 512 );
    w32_hdr = (struct w32_hdr *)
        &u.header[ u.dos_header.size_of_DOS_header_in_paras * 16 ];
    if( w32_hdr->sig != 'FC'
#if COMPRESSION
     && w32_hdr->sig != 'FCC'
#endif
     ) {
        PrintMsg( PickMsg( LOADER_INVALID_EXE ) );
        return( LOADER_INVALID_EXE );
    }
    TinySeek( handle, w32_hdr->start_of_W32_file, TIO_SEEK_START );

    /*
     * get memory to load file
     */
    BaseAddr = (DWORD)TinyMemAlloc( w32_hdr->memory_size );
    if( BaseAddr == 0 ) {
        PrintMsg( PickMsg( LOADER_NOT_ENOUGH_MEMORY ) );
        return( LOADER_NOT_ENOUGH_MEMORY );
    }
#ifdef __NT__
    {
    DWORD   old_flags;
    /* Adjust page protection to allow code execution. Required for DEP-enabled systems. */
    VirtualProtect( (LPVOID)BaseAddr, w32_hdr->memory_size, PAGE_EXECUTE_READWRITE, &old_flags );
    }
#endif
    CodeLoadAddr = BaseAddr;
    load_addr = CodeLoadAddr;
    CodeEntryPoint = w32_hdr->initial_EIP + CodeLoadAddr;
#if COMPRESSION
    if( w32_hdr->sig == 'FCC' ) {       /* if compressed file */
        /* read compressed file into top of memory range */
        load_addr += w32_hdr->memory_size - w32_hdr->size_of_W32_file;
    }
#endif

    /*
     * read the exe into memory
     */
    rc = TinyRead( handle, (void *)load_addr, w32_hdr->size_of_W32_file );
    if( TINY_LINFO(rc) != w32_hdr->size_of_W32_file ) {
        TinySeek( handle, w32_hdr->start_of_W32_file, TIO_SEEK_START );
        small_chunks = (void*) load_addr;
        total_size = w32_hdr->size_of_W32_file;
        while( total_size != 0 ) {
            read_size = total_size;
            if( read_size > READSIZE ) read_size = READSIZE;
            rc = TinyRead( handle, small_chunks, read_size );
            if( TINY_LINFO(rc) != read_size ) {
                TinyClose( handle );
                PrintMsg( PickMsg( LOADER_READ_ERROR ) );
                return( LOADER_READ_ERROR );
            }
            small_chunks += READSIZE;
            total_size -= read_size;
        }
    }
    TinyClose( handle );
#if COMPRESSION
    if( w32_hdr->sig == 'FCC' ) {       /* if compressed file */
        BPE_Expand( CodeLoadAddr, load_addr,
                        load_addr + w32_hdr->size_of_W32_file );
    }
#endif
    DoRelocations( (unsigned short *)(w32_hdr->offset_to_relocs + CodeLoadAddr) );
    return( LOADER_SUCCESS );
} /* Init32BitTask */

#if defined(__OS2) || defined(__NT)
void DumpEnvironment( void )
{
    char        *p;

    PrintMsg( "Environment Variables:\r\n" );
    p = ProgramEnv;
    for( ;; ) {
        if( *p == '\0' ) break;
        PrintMsg( "%s\r\n", p );
        while( *p != '\0' ) ++p;
        ++p;
    }
}
#endif
//////////////////////////////////////////////////////////////////////
#if defined(__OS2)

#pragma aux __OS2Main "*" parm caller []
char volatile NestedException = 0;

typedef struct SysERegRec {
    PEXCEPTIONREGISTRATIONRECORD pLink;
    ULONG (_cdecl *pSysEH)(PEXCEPTIONREPORTRECORD,
                           PEXCEPTIONREGISTRATIONRECORD,
                           PCONTEXTRECORD,
                           PVOID);
} SYSEREGREC;

void DumpContext( char *what, PCONTEXTRECORD p )
{
    int                 i;
    unsigned __far      *stk;
    char __far          *code;

    PrintMsg( "**** %s ****\r\n", what );
    PrintMsg( "OS=OS/2 BaseAddr=%X CS:EIP=%x:%X SS:ESP=%x:%X\r\n",
        BaseAddr, p->ctx_SegCs, p->ctx_RegEip, p->ctx_SegSs, p->ctx_RegEsp );
    PrintMsg( "EAX=%X EBX=%X ECX=%X EDX=%X\r\n",
        p->ctx_RegEax, p->ctx_RegEbx, p->ctx_RegEcx, p->ctx_RegEdx );
    PrintMsg( "ESI=%X EDI=%X EBP=%X FLG=%X\r\n",
        p->ctx_RegEsi, p->ctx_RegEdi, p->ctx_RegEbp, p->ctx_EFlags );
    PrintMsg( "DS=%x ES=%x FS=%x GS=%x\r\n",
        p->ctx_SegDs, p->ctx_SegEs, p->ctx_SegFs, p->ctx_SegGs );
    stk = MK_FP( p->ctx_SegSs, p->ctx_RegEsp );
    for( i = 0; i < 4 * 8; ) {
        PrintMsg( "%X ", *stk++ );
        i++;
        if( (i & 7) == 0 )  PrintMsg( "\r\n" );
    }
    PrintMsg( "CS:EIP -> " );
    code = MK_FP( p->ctx_SegCs, p->ctx_RegEip );
    for( i = 0; i < 16; ) {
        PrintMsg( "%h ", *code++ );
        i++;
    }
    PrintMsg( "\r\n" );
}

void Fatal( char *what, PCONTEXTRECORD p )
{
    tiny_ret_t  rc;

    DumpContext( what, p );
    rc = TinyCreate( "_watcom_.dmp", 0 );
    if( TINY_OK( rc ) ) {
        MsgFileHandle = TINY_INFO( rc );
        PrintMsg( "Program: %s\r\n", ProgramName );
        PrintMsg( "CmdLine: %s\r\n", ProgramArgs );
        DumpContext( what, p );
        DumpEnvironment();
        TinyClose( MsgFileHandle );
        MsgFileHandle = 1;
    }
    DosExit( EXIT_PROCESS, 8 );
}

ULONG _cdecl ExceptRoutine( PEXCEPTIONREPORTRECORD report,
                            PEXCEPTIONREGISTRATIONRECORD regrecord,
                            PCONTEXTRECORD context,
                            PVOID other )
{
    if( NestedException )  DosExit( EXIT_PROCESS, 8 );
    NestedException = 1;
    switch( report->ExceptionNum ) {
    case XCPT_ACCESS_VIOLATION:
        Fatal( "Access violation", context );
        break;
    case XCPT_ILLEGAL_INSTRUCTION:
        Fatal( "Illegal instruction", context );
        break;
    case XCPT_PRIVILEGED_INSTRUCTION:
        Fatal( "Privileged instruction", context );
        break;
    case XCPT_INTEGER_DIVIDE_BY_ZERO:
        Fatal( "Integer divide by 0", context );
        break;
    case XCPT_SIGNAL:
        DosAcknowledgeSignalException( report->ExceptionInfo[0] );
        /*
         * If the BreakFlag is already set then the app is probably not
         * paying attention to it.  So, just die.
         */
        if (BreakFlag == 1)
            DosExit(EXIT_PROCESS, 8);
        BreakFlag = 1;
        //DosAcknowledgeSignalException( report->ExceptionInfo[0] );
        break;
    default:
        NestedException = 0;
        return( XCPT_CONTINUE_SEARCH );
    }
    NestedException = 0;
    return( XCPT_CONTINUE_EXECUTION );
}

int __checkIsDBCS( void )
{
    COUNTRYCODE         countryInfo;
    CHAR                leadBytes[12];

    countryInfo.country = 0;                /* default country */
    countryInfo.codepage = 0;               /* default code page */
    if( DosQueryDBCSEnv( 12, &countryInfo, leadBytes ) == 0 ) {
        if( leadBytes[0] || leadBytes[1] )  return( 1 );
    }
    return( 0 );
}

int __OS2Main( unsigned hmod, unsigned reserved, char *env, char *cmd )
{
    char        *args;
    char        *pgm;
    int         rc;
    ULONG       nesting;
    LONG        req_count;
    struct pgmparms parms;
    auto SYSEREGREC RegRec;

    __IsDBCS = __checkIsDBCS();
    parms.isDBCS = __IsDBCS;
    hmod = hmod;
    reserved = reserved;
    for( args = cmd; *args != '\0'; ++args )
        ; /* skip over program name */
    ++args;
    for( pgm = cmd - 2; *pgm != '\0'; --pgm )
        ;
    ++pgm;
    ProgramArgs = args;
    ProgramName = pgm;
    ProgramEnv  = env;
    RegRec.pLink = 0;
    RegRec.pSysEH = ExceptRoutine;
    DosSetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD)&RegRec );
    DosSetSignalExceptionFocus( SIG_SETFOCUS, &nesting );
    rc = Init32BitTask( pgm );
    if( rc == LOADER_SUCCESS ) {
        parms.cmdline = args;
        parms.envptr = env;
        parms.pgmname = pgm;
        parms.breakflagaddr = &BreakFlag;
        parms.copyright = NULL;
        parms.max_handle = 0;
        req_count = 0;
        DosSetRelMaxFH( &req_count, &parms.max_handle );
        rc = _LaunchPgm( BaseAddr, CodeEntryPoint,
                        &__fInt21, &parms );
    }
    DosSetSignalExceptionFocus( SIG_UNSETFOCUS, &nesting );
    DosUnsetExceptionHandler( (PEXCEPTIONREGISTRATIONRECORD)&RegRec );
    return( rc );
}

void main( void ) {}

//////////////////////////////////////////////////////////////////////
#elif defined(__NT)

#pragma aux __NTMain "*"
extern  void    __InitInt21( void );

extern DWORD GetFromFS( DWORD off );
extern void PutToFS( DWORD value, DWORD off );
#define UNWINDING       0x6

#pragma aux GetFromFS = \
        "mov    eax,fs:[eax]" \
        parm[eax] value[eax];

#pragma aux PutToFS = \
        "mov    fs:[edx], eax" \
        parm[eax] [edx];

typedef struct _REGISTRATION_RECORD {
    struct _REGISTRATION_RECORD *RegistrationRecordPrev;
    void                        *RegistrationRecordFilter;
} REGISTRATION_RECORD;

void DumpContext( char *what, PCONTEXT p )
{
    int                 i;
    unsigned __far      *stk;
    char __far          *code;

    PrintMsg( "**** %s ****\r\n", what );
    PrintMsg( "OS=NT BaseAddr=%X CS:EIP=%x:%X SS:ESP=%x:%X\r\n",
        BaseAddr, p->SegCs, p->Eip, p->SegSs, p->Esp );
    PrintMsg( "EAX=%X EBX=%X ECX=%X EDX=%X\r\n",
        p->Eax, p->Ebx, p->Ecx, p->Edx );
    PrintMsg( "ESI=%X EDI=%X EBP=%X FLG=%X\r\n",
        p->Esi, p->Edi, p->Ebp, p->EFlags );
    PrintMsg( "DS=%x ES=%x FS=%x GS=%x\r\n",
        p->SegDs, p->SegEs, p->SegFs, p->SegGs );
    stk = MK_FP( p->SegSs, p->Esp );
    for( i = 0; i < 4 * 8; ) {
        PrintMsg( "%X ", *stk++ );
        i++;
        if( (i & 7) == 0 )  PrintMsg( "\r\n" );
    }
    PrintMsg( "CS:EIP -> " );
    code = MK_FP( p->SegCs, p->Eip );
    for( i = 0; i < 16; ) {
        PrintMsg( "%h ", *code++ );
        i++;
    }
    PrintMsg( "\r\n" );
}

void Fatal( char *what, PCONTEXT p )
{
    tiny_ret_t  rc;

    DumpContext( what, p );
    rc = TinyCreate( "_watcom_.dmp", 0 );
    if( TINY_OK( rc ) ) {
        MsgFileHandle = TINY_INFO( rc );
        PrintMsg( "Program: %s\r\n", ProgramName );
        PrintMsg( "CmdLine: %s\r\n", ProgramArgs );
        DumpContext( what, p );
        DumpEnvironment();
        TinyClose( MsgFileHandle );
        MsgFileHandle = 1;
    }
    ExitProcess( 8 );
}

int __stdcall __ExceptionFilter( LPEXCEPTION_RECORD ex,
        LPVOID establisher_frame, LPCONTEXT context, LPVOID dispatch_context )
{
    if( ex->ExceptionFlags & EXCEPTION_NONCONTINUABLE ) {
        return( ExceptionContinueSearch );
    }
    if( ex->ExceptionFlags & UNWINDING ) {
        return( ExceptionContinueSearch );
    }

    dispatch_context = dispatch_context;
    establisher_frame = establisher_frame;

    switch( ex->ExceptionCode ) {
    case STATUS_CONTROL_C_EXIT:
        /*
         * If the BreakFlag is already set then the app is probably not
         * paying attention to it.  So, just die.
         */
        if (BreakFlag == 1)
            ExitProcess(1);
        BreakFlag = 1;
        return( ExceptionContinueExecution );
    case STATUS_ACCESS_VIOLATION:
        Fatal( "Access violation", context );
        break;
    case STATUS_PRIVILEGED_INSTRUCTION:
        Fatal( "Privileged instruction", context );
        break;
    case STATUS_ILLEGAL_INSTRUCTION:
        Fatal( "Illegal instruction", context );
        break;
    case STATUS_INTEGER_DIVIDE_BY_ZERO:
        Fatal( "Integer divide by 0", context );
        break;
    case STATUS_STACK_OVERFLOW:
        Fatal( "Stack overflow", context );
        break;
    }
    return( ExceptionContinueSearch );

} /* __ExceptionFilter */

void __NewExceptionHandler( REGISTRATION_RECORD *rr )
{
    rr->RegistrationRecordPrev = (LPVOID) GetFromFS( 0 );
    rr->RegistrationRecordFilter = __ExceptionFilter;
    PutToFS( (DWORD) rr, 0 );
}

void __DoneExceptionHandler( REGISTRATION_RECORD *rr )
{
    PutToFS( (DWORD) rr->RegistrationRecordPrev, 0 );
}

BOOL WINAPI CtrlCHandler( ULONG ctrl_type )
{
    DWORD       n;
    HANDLE      h;
    INPUT_RECORD r;
    extern HANDLE __FileHandleIDs[];

    if( ctrl_type == CTRL_C_EVENT  ||  ctrl_type == CTRL_BREAK_EVENT ) {
        /*
         * If the BreakFlag is already set then the app is probably not
         * paying attention to it.  So, just die.
         */
        if (BreakFlag == 1)
            ExitProcess(1);
        BreakFlag = 1;
        h = __FileHandleIDs[ 0 ];
        for( ;; ) {
            n = 0;
            if( PeekConsoleInput( h, &r, 1, &n ) == 0 ) break;
            if( n == 0 ) break;
            // flush out mouse, window, and key up events
            if( ReadConsoleInput( h, &r, 1, &n ) == 0 ) break;
        }
    }
    return( TRUE );
}

int __checkIsDBCS( void )
{
    CPINFO              cpInfo;

    if( GetCPInfo( CP_OEMCP, &cpInfo ) != FALSE ) {
        if( cpInfo.LeadByte[0] || cpInfo.LeadByte[1] )  return( 1 );
    }
    return( 0 );
}

#ifdef W32RUN
/*
 * This routine is used for BINNT\W32RUN.EXE
 */
void __NTMain( void )
{
    char        *cmd;
    char        *pgm;
    int         rc;
    struct pgmparms parms;
    REGISTRATION_RECORD rr;

    __IsDBCS = __checkIsDBCS();
    parms.isDBCS = __IsDBCS;
    __NewExceptionHandler( &rr );
    __InitInt21();
    cmd = GetEnvironmentStrings();
    parms.envptr = cmd;
    ProgramEnv = cmd;
    pgm = NULL;
    for( ;; ) {
        if( *cmd == '$'  &&  cmd[1] == '=' )  pgm = cmd + 2;
        while( *cmd )  ++cmd;
        ++cmd;
        if( *cmd == '\0' )  break;
    }
    parms.pgmname = pgm;
    if( pgm == NULL ) {
        /* This should only occur if someone tries running W32RUN by itself */
        /* The stub on the real executable will setup "$=program" in the */
        /* environment before spawning this program. */
        ExitProcess( LOADER_SUCCESS );
    }
    cmd = GetCommandLine();
    while( *cmd == ' '  ||  *cmd == '\t' ) ++cmd;
    for( ;; ) {                 // skip program name (\path\w32run.exe)
        if( *cmd == '\0' ) break;
        if( *cmd == ' '  ) break;
        if( *cmd == '\t' ) break;
        ++cmd;
    }
    while( *cmd == ' '  ||  *cmd == '\t' ) ++cmd;
    parms.cmdline = cmd;
    ProgramArgs = cmd;
    ProgramName = pgm;
    rc = Init32BitTask( pgm );
    if( rc == LOADER_SUCCESS ) {
        SetConsoleCtrlHandler( CtrlCHandler, TRUE );
        parms.breakflagaddr = &BreakFlag;
        parms.copyright = NULL;
        parms.max_handle = 0;
        rc = _LaunchPgm( BaseAddr, CodeEntryPoint, &__fInt21, &parms );
    }
    __DoneExceptionHandler( &rr );
    ExitProcess( rc );
}
#else
/*
 *  This section handles the case where we have a special loader in
 *  BINNT with the same name as the executable we want to run from BINW.
 */
void __ChgBINNT( char *fn )
{
    char        *p;

    p = fn;
    for( ;; ) {
        for( ;; ) {
            if( *p == '\0' ) break;
            if( *p == '\\' ) break;
            ++p;
        }
        if( *p == '\0' ) break;
        ++p;
        if( (p[0] | ' ') == 'b'  &&
            (p[1] | ' ') == 'i'  &&
            (p[2] | ' ') == 'n'  &&
            (p[3] | ' ') == 'n'  &&
            (p[4] | ' ') == 't'  &&
            p[5] == '\\' ) {
            p[3] = p[3] + ('w' - 'n');  // change path from binnt to binw
            p += 4;
            while( p[0] = p[1] ) p++;
            break;
        }
    }
}

void __NTMain( void )
{
    char        *cmd;
    int         rc;
    struct pgmparms parms;
    REGISTRATION_RECORD rr;
    char        fn[_MAX_PATH];

    __IsDBCS = __checkIsDBCS();
    parms.isDBCS = __IsDBCS;
    __NewExceptionHandler( &rr );
    __InitInt21();
    cmd = GetCommandLine();
    // cmd should look like:
    // PROGRAM command line arguments
    while( *cmd == ' '  ||  *cmd == '\t' )  ++cmd;
    for( ;; ) {                         // skip over program name
        if( *cmd == '\0' ) break;
        if( *cmd == ' '  ) break;
        if( *cmd == '\t' ) break;
        ++cmd;
    }
    while( *cmd == ' '  ||  *cmd == '\t' )  ++cmd;
    parms.cmdline = cmd;
    ProgramArgs = cmd;
    parms.envptr = GetEnvironmentStrings();
    ProgramEnv = parms.envptr;
    GetModuleFileName( NULL, fn, sizeof(fn) );
    parms.pgmname = fn;
    __ChgBINNT( fn );
    ProgramName = fn;
    rc = Init32BitTask( fn );
    if( rc == LOADER_SUCCESS ) {
        SetConsoleCtrlHandler( CtrlCHandler, TRUE );
        parms.breakflagaddr = &BreakFlag;
        parms.copyright = NULL;
        parms.max_handle = 0;
        rc = _LaunchPgm( BaseAddr, CodeEntryPoint, &__fInt21, &parms );
    }
    __DoneExceptionHandler( &rr );
    ExitProcess( rc );
}
#endif

void main( void ) {}

//////////////////////////////////////////////////////////////////////
#else   // __DOS

#include <string.h>

#if defined(__DOS4G) || defined(__CAUSEWAY)
 #include "dpmi.h"
#endif

#ifdef __DOS4G
 #include "dginfo.gh"
char DOS4GOPTIONS[] =
        "[dos4g-global]\n"
        "Include=OSIOPTS.INI\n"
        "[dos4g-kernel]\n"
        "StartupBanner=FALSE\n"
;
#endif

extern  char    *_LpCmdLine;
extern  char    *_Envptr;

#ifdef __TNT
typedef struct {
    unsigned short  int_num;
    unsigned short  real_ds;
    unsigned short  real_es;
    unsigned short  real_fs;
    unsigned short  real_gs;
    long            real_eax;
    long            real_edx;
} PHARLAP_block;
#endif

extern unsigned short __get_ds( void );
#pragma aux __get_ds = "mov ax,ds" value [ax];

int __checkIsDBCS( void )
{
#if defined(__TNT)
    unsigned short far *leadBytes;
    PHARLAP_block       pblock;
    union REGPACK       regs;

    memset( &pblock, 0, sizeof( pblock ) );
    memset( &regs, 0, sizeof( regs ) );
    pblock.real_eax = 0x6300;           /* get DBCS vector table */
    pblock.int_num = 0x21;              /* DOS call */
    regs.x.eax = 0x2511;                /* issue real-mode interrupt */
    regs.x.edx = FP_OFF( &pblock );     /* DS:EDX -> parameter block */
    regs.w.ds = FP_SEG( &pblock );
    intr( 0x21, &regs );
    if( pblock.real_ds != 0xFFFF ) {    /* weird OS/2 value */
        if( (pblock.real_ds | regs.w.si) != 0 ) {
            leadBytes = MK_FP( 0x34 | (__get_ds() & 0x03),
                           (((unsigned)pblock.real_ds)<<4) + regs.w.si );
            if( leadBytes[0] || leadBytes[1] )  return( 1 );
        }
    }
    return( 0 );
#elif defined(__DOS4G) || defined(__CAUSEWAY)
    unsigned short  *leadBytes;
    rm_call_struct  dblock;

    memset( &dblock, 0, sizeof( dblock ) );
    dblock.eax = 0x6300;                /* get DBCS vector table */
    DPMISimulateRealModeInterrupt( 0x21, 0, 0, &dblock );
    if( (dblock.flags & 1) == 0 ) {
        if( (dblock.ds | dblock.esi) != 0 ) {
            leadBytes = (unsigned short *)
                          ((((unsigned)dblock.ds)<<4) + (dblock.esi&0xFFFF));
            if( leadBytes[0] || leadBytes[1] )  return( 1 );
        }
    }
    return( 0 );
#elif defined(__X32)
    unsigned            esi;
    unsigned short far *leadBytes;
    struct parms {
        unsigned short  interrupt_num;
        unsigned short  selector_ds;
        unsigned short  selector_es;
        unsigned short  selector_fs;
        unsigned short  selector_gs;
        unsigned long   register_eax;
        unsigned long   register_edx;
    } parm_struct;
    extern  short       __x386_zero_base_selector;
    #pragma aux         __x386_zero_base_selector "*";
    extern unsigned getLeadBytes( struct parms * );
    #pragma aux getLeadBytes = \
                "push ebp" \
                "push edi" \
                "push ecx" \
                "push ebx" \
                "mov ax,2511h" \
                "int 21h" \
                "pop ebx" \
                "pop ecx" \
                "pop edi" \
                "pop ebp" \
                parm [edx] value [esi];

    parm_struct.interrupt_num = 0x21;
    parm_struct.selector_ds = 0;
    parm_struct.selector_es = 0;
    parm_struct.selector_fs = 0;
    parm_struct.selector_gs = 0;
    parm_struct.register_eax = 0x6300;
    parm_struct.register_edx = 0;
    esi = getLeadBytes( &parm_struct );
    if( (parm_struct.selector_ds | esi) != 0 ) {
        leadBytes = MK_FP( __x386_zero_base_selector,
                        (parm_struct.selector_ds << 4) + (esi & 0xFFFF) );
        if( leadBytes[0] || leadBytes[1] )  return( 1 );
    }
    return( 0 );
#endif
}

int main( void )
{
    char        *cmd;
    char        *pgm;
    int         rc;
    struct pgmparms parms;

    __IsDBCS = __checkIsDBCS();
    parms.isDBCS = __IsDBCS;
    cmd = _LpCmdLine;
    while( *cmd == ' '  ||  *cmd == '\t' ) ++cmd;
    parms.cmdline = cmd;
    parms.envptr = _Envptr;
    ProgramEnv = _Envptr;
    cmd = _Envptr;
    pgm = NULL;
    for( ;; ) {
        if( *cmd == '$'  &&  cmd[1] == '=' )  pgm = cmd + 2;
        while( *cmd )  ++cmd;
        ++cmd;
        if( *cmd == '\0' )  break;
    }
    parms.pgmname = pgm;
    if( pgm == NULL ) {
        /* This should only occur if someone tries running W32RUN by itself */
        /* The stub on the real executable will setup "$=program" in the */
        /* environment before spawning this program. */
        return( LOADER_SUCCESS );
    }
    rc = Init32BitTask( pgm );
    if( rc == LOADER_SUCCESS ) {
        parms.breakflagaddr = &BreakFlag;
        #ifdef __DOS4G
            parms.copyright = D32_SHORT_NAME " Version " D32_VERSION " "
                              D32_COPYRIGHT;
        #else
            parms.copyright = NULL;
        #endif
        parms.max_handle = 0;
        rc = _InvokePgm( OS_DOS, BaseAddr, CodeEntryPoint, (unsigned)&_end,
                                (int (*)( void ))&__fInt21, &parms );
    }
    return( rc );
}
#endif
