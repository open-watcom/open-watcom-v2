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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <dos.h>
#include <sys\stat.h>
#include "tinyio.h"
#include "wdebug.h"
#include "drwatcom.h"
#include "commdlg.h"

static void logPrint( char *str, ... );

static loginfo logInfo;

/*
 * getNewLogName
 */
static BOOL getNewLogName( HWND parent ) {
    OPENFILENAME        of;
    BOOL                ret;
    char                filter[100];
    char                *ptr;

    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = parent;
    of.hInstance = Instance;
    of.lpstrDefExt = NULL;
    of.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
    of.lpstrFile = logInfo.filename;
    of.nMaxFile = MAX_FNAME;
    CopyRCString( STR_ALL_FILE_FILTER, filter, sizeof( filter ) );
    ptr = filter + strlen( filter ) + 1;
    strcpy( ptr, "*.*" );
    ptr += strlen( ptr ) + 1;
    *ptr = '\0';
    of.lpstrFilter = filter;
    of.nFilterIndex = 1L;
    of.lpstrTitle = AllocRCString( STR_LOG_FILENAME );
    ret = GetSaveFileName( &of );
    FreeRCString( of.lpstrTitle );
    return( ret );
}

/*
 * LogDialog - show task status
 */
BOOL __export FAR PASCAL LogDialog( HWND hwnd, WORD msg, WORD wparam,
                                    DWORD lparam )
{
    int         i;

    lparam = lparam;

    switch( msg ) {
    case WM_INITDIALOG:
    {
        char tmp[128];

        logInfo = LogInfo;
        for( i=0;i<LOGFL_MAX;i++ ) {
            CheckDlgButton( hwnd, LOG_STACK_TRACE+i,
                                logInfo.flags[i] == '1' );
        }
        #if 0
            SetCourierFont( hwnd, LOG_FILE_NAME );
            SetCourierFont( hwnd, LOG_MAX_FILE_SIZE );
            SetCourierFont( hwnd, LOG_DISASM_BACKUP );
            SetCourierFont( hwnd, LOG_DISASM_LINES );
        #endif
        SetDlgItemText( hwnd, LOG_FILE_NAME, strlwr( logInfo.filename ) );
        ltoa( logInfo.maxlogsize, tmp, 10 );
        SetDlgItemText( hwnd, LOG_MAX_FILE_SIZE, tmp );
        itoa( logInfo.disasmbackup, tmp, 10 );
        SetDlgItemText( hwnd, LOG_DISASM_BACKUP, tmp );
        itoa( logInfo.disasmlines, tmp, 10 );
        SetDlgItemText( hwnd, LOG_DISASM_LINES, tmp );
        return( TRUE );
    }
    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, IDCANCEL, 0L );
        return( TRUE );
    case WM_COMMAND:
        if( wparam >= LOG_STACK_TRACE && wparam <= LOG_MAXFL ) {
            i = wparam - LOG_STACK_TRACE;
            if( logInfo.flags[i] == '1' ) {
                logInfo.flags[i] = '0';
            } else {
                logInfo.flags[i] = '1';
            }
            CheckDlgButton( hwnd, wparam, logInfo.flags[i] == '1' );
            if( i == LOGFL_MOD_SEGMENTS
               && logInfo.flags[ LOGFL_MOD_SEGMENTS ] == '1' ) {
                logInfo.flags[ LOGFL_MODULES ] = '1';
                CheckDlgButton( hwnd, LOG_STACK_TRACE + LOGFL_MODULES, TRUE );
            }
            if( i == LOGFL_MODULES && logInfo.flags[ LOGFL_MODULES ] == '0' )
            {
                logInfo.flags[ LOGFL_MOD_SEGMENTS ] = '0';
                CheckDlgButton( hwnd, LOG_STACK_TRACE + LOGFL_MOD_SEGMENTS,
                                FALSE );
            }
            return( TRUE );
        }
        switch( wparam ) {
        case IDOK:
        {
            char        tmp[128];

            GetDlgItemText( hwnd, LOG_FILE_NAME, (LPSTR) logInfo.filename,
                                MAX_FNAME );
            GetDlgItemText( hwnd, LOG_MAX_FILE_SIZE, (LPSTR) tmp, 128 );
            logInfo.maxlogsize = atol( tmp );
            GetDlgItemText( hwnd, LOG_DISASM_BACKUP, (LPSTR) tmp, 128 );
            logInfo.disasmbackup = atoi( tmp );
            GetDlgItemText( hwnd, LOG_DISASM_LINES, (LPSTR) tmp, 128 );
            logInfo.disasmlines = atoi( tmp );
            LogInfo = logInfo;
            EndDialog( hwnd, 0 );
            return( TRUE );
        }
        case IDCANCEL:
            EndDialog( hwnd, 0 );
            return( TRUE );
        case LOG_BROWSE:
            if( getNewLogName( hwnd ) ) {
                strlwr( logInfo.filename );
                SetDlgItemText( hwnd, LOG_FILE_NAME, logInfo.filename );
            }
            return( TRUE );
        }
    }
    return( FALSE );

} /* LogDialog */

/*
 * DoLogDialog - start log info dialog
 */
void DoLogDialog( HWND hwnd )
{
    FARPROC     fp;

    fp = MakeProcInstance( LogDialog, Instance );
    DialogBox( Instance, "LOG", hwnd, fp );
    FreeProcInstance( fp );

} /* DoLogDialog */

#define BUFF_SIZE       1024
static int              logFile;
static char             *workBuff;
static GLOBALHANDLE     workHandle;
static int              buffPos;

/*
 * startLogFile - start log for this session
 */
static BOOL startLogFile( void )
{
    tiny_ret_t  rc;

    rc = TinyOpen( LogInfo.filename, TIO_WRITE );
    if( TINY_ERROR( rc ) ) {
        rc = TinyCreate( LogInfo.filename, 0 );
        logFile = TINY_INFO( rc );
        TinyClose( logFile );
        rc = TinyOpen( LogInfo.filename, TIO_WRITE );
        if( TINY_ERROR( rc ) ) {
            return( FALSE );
        }
    }
    logFile = TINY_INFO( rc );

    workHandle = GlobalAlloc( GMEM_FIXED, BUFF_SIZE );
    workBuff = GlobalLock( workHandle );
    if( logFile == -1 || workBuff == NULL ) {
        if( workBuff != NULL ) {
            GlobalUnlock( workHandle );
            GlobalFree( workHandle );
        }
        return( FALSE );
    }
    TinySeek( logFile, 0L, TIO_SEEK_END );
    return( TRUE );

} /* startLogFile */

/*
 * finishLogFile - close up log file
 */
static void finishLogFile( void )
{

    logFlush();
    TinyClose( logFile );
    GlobalUnlock( workHandle );
    GlobalFree( workHandle );
    workBuff = NULL;

} /* finishLogFile */

/*
 * logFlush - flush the log file
 */
static void logFlush( void )
{
    TinyWrite( logFile, workBuff, buffPos );
    buffPos = 0;

} /* logFlush */

/*
 * dologPrint - print to the log file
 */
static void doLogPrint( char *str, va_list al )
{
    char        tmp[256];
    int         len,i;

    vsprintf( tmp, str, al );
    len = strlen( tmp );

    for( i=0;i<len;i++ ) {
        if( tmp[i] == '\n' ) {
            workBuff[ buffPos++ ] = '\r';
        }
        workBuff[ buffPos++ ]  = tmp[i];
        if( buffPos >= BUFF_SIZE-3 ) {
            logFlush();
        }
    }

} /* logPrint */

/*
 * logPrint
 */
static void logPrint( char *str, ... )
{
    va_list     al;

    va_start( al, str );
    doLogPrint( str, al );
    va_end( al );
}

/*
 * rcLogPrint
 */
static void rcLogPrint( DWORD msgid, ... )
{
    va_list     al;
    char        *str;

    str = AllocRCString( msgid );
    va_start( al, msgid );
    doLogPrint( str, al );
    va_end( al );
    FreeRCString( str );
}

/*
 * formatSel - format selector info
 */
static void formatSel( char *which, WORD sel )
{
    descriptor  desc;
    DWORD       base;
    DWORD       limit;

    GetADescriptor( sel, &desc );
    if( which != NULL ) {
        logPrint( "      %s = %04x  ", which, sel );
        if( sel == 0L ) {
            logPrint( "********  ********  ****  ****  *   ***    *\n" );
            return;
        }
    } else {
        logPrint( " %04x  ", sel );
    }

    base = GET_DESC_BASE( desc );
    limit = GET_DESC_LIMIT( desc );
    logPrint( "%08lx  %08lx  ", base, limit );
    if( desc.granularity ) {
        logPrint( "page  " );
    } else {
        logPrint( "byte  " );
    }
    if( desc.type == 2 ) {
        logPrint( "data  " );
    } else {
        logPrint( "code  " );
    }
    logPrint( "%1d   ", (WORD) desc.dpl );
    if( desc.type == 2 )  {
        logPrint( "R" );
        if( desc.writeable_or_readable ) {
            logPrint( "/W" );
        } else {
            logPrint( "  " );
        }
        logPrint( "    " );
    } else {
        logPrint( "Ex" );
        if( desc.writeable_or_readable ) {
            logPrint( "/R" );
        } else {
            logPrint( "  " );
        }
        logPrint( "   " );
    }
    if( desc.big_or_default ) {
        logPrint( "Y\n" );
    } else {
        logPrint( " \n" );
    }

} /* formatSel */

/*
 * logDisasm - log some disassembly
 */
static void logDisasm( WORD CS, DWORD EIP )
{
    int         i;
    ADDRESS     addr;
    char        str[256];

    addr.seg = CS;
    addr.offset = EIP;
    InstructionBackup( LogInfo.disasmbackup, &addr );
    for( i=0;i<LogInfo.disasmlines; i++ ) {
        if( addr.offset == EIP ) {
            logPrint( "--->" );
        } else {
            logPrint( "    " );
        }
        addr.offset += Disassemble( &addr, str, TRUE );
        logPrint( "%s\n", str );
    }

} /* logDisasm */

/*
 * logSysInfo - record basic system info
 */
static void logSysInfo( BOOL wasfault )
{
    char        *s;
    time_t      tod;
    WORD        ver;

    tod = time( NULL );
    s = ctime( &tod );
    if( wasfault ) {
        rcLogPrint( STR_DETECTED_FAULT_ON, AppName, s );
    } else {
        rcLogPrint( STR_LOG_TAKEN_ON, AppName, s );
    }

    ver = GetVersion();
    rcLogPrint( STR_WIN_VER_IS, (WORD) ver&0xff, (WORD)ver>>8 );
    rcLogPrint( STR_WIN_FLAGS_ARE );
    if( WindowsFlags & WF_ENHANCED ) {
        rcLogPrint( STR_ENHANCED_MODE );
    }
    if( WindowsFlags & WF_STANDARD ) {
        rcLogPrint( STR_STANDARD_MODE );
    }
    if( WindowsFlags & WF_CPU286 ) {
        rcLogPrint( STR_PROCESSOR_286 );
    }
    if( WindowsFlags & WF_CPU386 ) {
        rcLogPrint( STR_PROCESSOR_386 );
    }
    if( WindowsFlags & WF_CPU486 ) {
        rcLogPrint( STR_PROCESSOR_486 );
    }
    if( WindowsFlags & WF_80x87 ) {
        rcLogPrint( STR_MATH_COPROC_PRESENT );
    }
    rcLogPrint( STR_N_TASKS_EXECUTING, GetNumTasks() );

} /* logSysInfo */

/*
 * logMyTask - record task info
 */
static void logMyTask( void )
{
    char        str[128];
    syminfo     si;
    ADDRESS     addr;

    logPrint( "\n" );
    rcLogPrint( STR_OFFENDING_TASK_LINE );
    rcLogPrint( STR_OFFENDING_TASK_INFO );
    rcLogPrint( STR_OFFENDING_TASK_LINE );
    rcLogPrint( STR_FAULT_OCCURRED_IN, IntData.InterruptNumber,
                DTTaskEntry.szModule, IntData.CS, (WORD) IntData.EIP );
    GetFaultString( IntData.InterruptNumber, str );
    rcLogPrint( STR_FAULT_TYPE_WAS, str );
    rcLogPrint( STR_EXECUTABLE_IS, DTModuleEntry.szExePath );
    rcLogPrint( STR_REGISTER_CONTENTS );
    if( WindowsFlags & WF_CPU386 || WindowsFlags & WF_CPU486 ) {
        logPrint( "    eax=%08lx  ebx=%08lx  ecx=%08lx  edx=%08lx\n",
                IntData.EAX, IntData.EBX, IntData.ECX,
                IntData.EDX );
        logPrint( "    edi=%08lx  esi=%08lx  ebp=%08lx  esp=%08lx\n",
                IntData.EDI,
                IntData.ESI, IntData.EBP, IntData.ESP );
        logPrint(
        "    AF=%d   CF=%d   DF=%d   IF=%d   OF=%d   PF=%d   SF=%d   ZF=%d\n\n",
                ( IntData.EFlags & FLAG_A_MASK ) ? 1 : 0,
                ( IntData.EFlags & FLAG_C_MASK ) ? 1 : 0,
                ( IntData.EFlags & FLAG_D_MASK ) ? 1 : 0,
                ( IntData.EFlags & FLAG_I_MASK ) ? 1 : 0,
                ( IntData.EFlags & FLAG_O_MASK ) ? 1 : 0,
                ( IntData.EFlags & FLAG_P_MASK ) ? 1 : 0,
                ( IntData.EFlags & FLAG_S_MASK ) ? 1 : 0,
                ( IntData.EFlags & FLAG_Z_MASK ) ? 1 : 0 );
        rcLogPrint( STR_SEGMENT_REGISTERS );
        rcLogPrint( STR_SELECTOR_HEADINGS );
        formatSel( "cs", IntData.CS );
        formatSel( "ds", IntData.DS );
        formatSel( "es", IntData.ES );
        formatSel( "fs", IntData.FS );
        formatSel( "gs", IntData.GS );
        formatSel( "ss", IntData.SS );
    } else {
    }
    addr.seg = IntData.CS;
    addr.offset = IntData.EIP;
    if( FindWatSymbol( &addr, &si, TRUE ) == FOUND ) {
        rcLogPrint( STR_SOURCE_INFO );
        rcLogPrint( STR_FILE_IS, si.filename );
        if( si.linenum > 0 ) {
            rcLogPrint( STR_FAULT_ON_LINE, si.linenum );
        }
    }

    rcLogPrint( STR_DISASSEMBLY );
    logDisasm( IntData.CS, IntData.EIP );

} /* logMyTask */


/*
 * LogSSBP - log memory at SS:BP
 */
#define BYTES   16
static void LogSSBP( WORD ss, WORD bp )
{
    WORD        start,end;
    WORD        i,num,j;
    char        buff[BYTES];

    start = (bp/(BYTES)*BYTES);
    if( start < BYTES ) {
        start = 0;
    } else {
        start -= BYTES;
    }
    end = start+BYTES*3;
    for( i=start;i<end; i+= BYTES ) {
        logPrint( "SS:%04x = ", i );
        num = ReadMem( ss, i, buff, BYTES );
        for( j=0;j<BYTES;j++ ) {
            if( j >= num ) {
                rcLogPrint( STR_QUEST_MARKS );
            } else {
                logPrint( "%02x ", (WORD) buff[j] );
            }
        }
        logPrint( "\n" );
    }

} /* LogSSBP */

/*
 * logStackTrace - record stack trace info
 */
static void logStackTrace( void )
{
    STACKTRACEENTRY     ste;
    MODULEENTRY         me;
    int                 frame=0;
    syminfo             si;
    ADDRESS             addr;

    if( LogInfo.flags[LOGFL_STACK_TRACE] == '0' ) {
        return;
    }
    ste.hTask = DeadTask;
    if( !MyStackTraceCSIPFirst( &ste, IntData.SS, IntData.CS,
                        (WORD) IntData.EIP, (WORD) IntData.EBP ) ) return;
    logPrint( "\n" );
    rcLogPrint( STR_STACK_TRACE_ULINE );
    rcLogPrint( STR_STACK_TRACE_HEADING );
    rcLogPrint( STR_STACK_TRACE_ULINE );
    while( 1 ) {
        MyModuleFindHandle( &me, ste.hModule );
        rcLogPrint( STR_STACK_FRAME, frame, me.szModule, ste.hModule );
        logPrint( "CS:IP=%04x:%04x, SS:BP=%04x:%04x\n", ste.wCS, ste.wIP,
                ste.wSS, ste.wBP );
        rcLogPrint( STR_CS_MAPS_TO_SEG_NUM, ste.wSegment );
        LogSSBP( ste.wSS, ste.wBP );

        addr.seg = ste.wCS;
        addr.offset = ste.wIP;
        if( FindWatSymbol( &addr, &si, TRUE ) == FOUND ) {
            rcLogPrint( STR_SOURCE_INFO );
            rcLogPrint( STR_FILE_IS, si.filename );
            if( si.linenum > 0 ) {
                rcLogPrint( STR_CSIP_REFERS_TO_LINE, si.linenum );
            }
        }
        rcLogPrint( STR_DISASSEMBLY );
        logDisasm( ste.wCS, ste.wIP );
        if( !MyStackTraceNext( &ste ) ) {
            break;
        }
        if( !IsValidSelector( ste.wCS )) {
            break;
        }
        logPrint( "\n" );
        frame++;
    }

} /* logStackTrace */

/*
 * logCurrentTasks - record current tasks in system
 */
static void logCurrentTasks( void )
{
    MODULEENTRY         me;
    TASKENTRY           te;
    struct stat         st;

    if( LogInfo.flags[LOGFL_TASKS] == '0' ) {
        return;
    }

    if( !MyTaskFirst( &te ) ) {
        return;
    }

    logPrint( "\n" );
    rcLogPrint( STR_SYS_TASKS_ULINE );
    rcLogPrint( STR_SYSTEM_TASKS );
    rcLogPrint( STR_SYS_TASKS_ULINE );

    do {
        MyModuleFindHandle( &me, te.hModule );
        rcLogPrint( STR_TASK, te.szModule );
        rcLogPrint( STR_ID_PARENT_ID, te.hTask, te.hTaskParent );
        rcLogPrint( STR_MOD_HDL_USAGE_CNT, te.hModule, me.wcUsage );
        rcLogPrint( STR_SSSP_STACKTOP_BOTTOM,
                    te.wSS, te.wSP, te.wStackTop, te.wStackBottom );
        stat( me.szExePath, &st );
        rcLogPrint( STR_EXECUTABLE_EQ, me.szExePath );
        rcLogPrint( STR_SIZE_TIME, st.st_size, ctime( &st.st_atime ) );
    } while( MyTaskNext( &te ) );

} /* logCurrentTasks */

static BOOL     hasTitle;

/*
 * formatModuleSeg - format a segment for a module entry
 */
void formatModuleSeg( int index, WORD sel )
{
    if( !hasTitle ) {
        hasTitle = TRUE;
        rcLogPrint( STR_SEGMENTS_TITLE );
    }
    logPrint( "              %3d  ", index );
    formatSel( NULL, sel );

} /* formatModuleSeg */


/*
 * logModuleSegments - record all segments allocated to a module
 */
static void logModuleSegments( MODULEENTRY *me )
{
    int         i;
    GLOBALENTRY ge;

    hasTitle = FALSE;
    if( CheckIsModuleWin32App( me->hModule, &Win32DS, &Win32CS, &Win32InitialEIP ) ) {
        formatModuleSeg( 1, Win32CS );
        formatModuleSeg( 2, Win32DS );
    } else {
        for( i=0;i<512;i++ ) {
            if( MyGlobalEntryModule( &ge, me->hModule, i ) ) {
                formatModuleSeg( i, ge.hBlock );
            }
        }
    }

} /* logModuleSegments */

/*
 * logModules - record current modules in system
 */
static void logModules( void )
{
    MODULEENTRY         me;
    struct stat         st;

    if( LogInfo.flags[LOGFL_MODULES] == '0' ) {
        return;
    }
    logPrint( "\n" );
    rcLogPrint( STR_SYS_MODULES_ULINE );
    rcLogPrint( STR_SYSTEM_MODULES );
    rcLogPrint( STR_SYS_MODULES_ULINE );

    me.dwSize = sizeof( MODULEENTRY );
    if( !ModuleFirst( &me ) ) {
        return;
    }
    do {
        rcLogPrint( STR_MODULE, me.szModule );
        rcLogPrint( STR_MOD_HDL_USAGE_CNT, me.hModule, me.wcUsage );
        stat( me.szExePath, &st );
        rcLogPrint( STR_EXECUTABLE_EQ, me.szExePath );
        rcLogPrint( STR_SIZE_TIME, st.st_size, ctime( &st.st_atime ) );
        me.dwSize = sizeof( MODULEENTRY );
        if( LogInfo.flags[LOGFL_MOD_SEGMENTS] == '1' ) {
            logModuleSegments( &me );
        }
    } while( ModuleNext( &me ) );

} /* logModules */

/*
 * logMemory - record current memory state
 */
static void logMemory( void )
{
    MEMMANINFO  mmi;

    if( LogInfo.flags[LOGFL_MEM] == '0' ) {
        return;
    }
    logPrint( "\n" );
    rcLogPrint( STR_MEM_MAN_ULINE );
    rcLogPrint( STR_MEM_MAN_INFO );
    rcLogPrint( STR_MEM_MAN_ULINE );

    mmi.dwSize = sizeof( MEMMANINFO );
    MemManInfo( &mmi );
    rcLogPrint( STR_TOT_LIN_SPACE, mmi.dwTotalLinearSpace*mmi.wPageSize,
                mmi.dwTotalLinearSpace*mmi.wPageSize/1024L );
    rcLogPrint( STR_TOTAL_PAGES, mmi.dwTotalPages );
    rcLogPrint( STR_SWAP_FILE_PAGES, mmi.dwSwapFilePages );
    rcLogPrint( STR_PAGE_SIZE, mmi.wPageSize, mmi.wPageSize/1024 );
    rcLogPrint( STR_FREE_LIN_SPACE, mmi.dwFreeLinearSpace*mmi.wPageSize,
        mmi.dwFreeLinearSpace*mmi.wPageSize/1024L );
    rcLogPrint( STR_LARGEST_FREE_BLOCK, mmi.dwLargestFreeBlock,
        mmi.dwLargestFreeBlock/1024L );
    rcLogPrint( STR_FREE_PAGES, mmi.dwFreePages );
    rcLogPrint( STR_MAX_PAGES_AVAILABLE, mmi.dwMaxPagesAvailable );
    rcLogPrint( STR_MAX_PAGES_LOCKABLE, mmi.dwMaxPagesLockable );
    rcLogPrint( STR_TOT_UNLOCKED_PAGES, mmi.dwTotalUnlockedPages );

} /* logMemory */

/*
 * logGDI - record current GDI state
 */
static void logGDI( void )
{
    SYSHEAPINFO hi;
    GLOBALENTRY ge;

    if( LogInfo.flags[LOGFL_GDI] == '0' ) {
        return;
    }
    hi.dwSize = sizeof( SYSHEAPINFO );
    logPrint( "\n" );
    rcLogPrint( STR_GDI_HEAP_ULINE );
    rcLogPrint( STR_GDI_HEAP_INFO );
    rcLogPrint( STR_GDI_HEAP_ULINE );
    if( SystemHeapInfo( &hi ) ) {
        MyGlobalEntryHandle( &ge, hi.hGDISegment );
        rcLogPrint( STR_HEAP_SEGMENT, hi.hGDISegment );
        rcLogPrint( STR_HEAP_SIZE, ge.dwBlockSize );
        rcLogPrint( STR_PERCENT_FREE, hi.wGDIFreePercent );
    }

} /* logGDI */

/*
 * logUSER - record current USER state
 */
static void logUSER( void )
{
    SYSHEAPINFO hi;
    GLOBALENTRY ge;

    if( LogInfo.flags[LOGFL_USER] == '0' ) {
        return;
    }
    hi.dwSize = sizeof( SYSHEAPINFO );
    logPrint( "\n" );
    rcLogPrint( STR_USER_HEAP_ULINE );
    rcLogPrint( STR_USER_HEAP_INFO );
    rcLogPrint( STR_USER_HEAP_ULINE );
    if( SystemHeapInfo( &hi ) ) {
        MyGlobalEntryHandle( &ge, hi.hUserSegment );
        rcLogPrint( STR_HEAP_SEGMENT, hi.hUserSegment );
        rcLogPrint( STR_HEAP_SIZE, ge.dwBlockSize );
        rcLogPrint( STR_PERCENT_FREE, hi.wUserFreePercent );
    }

} /* logUSER */

char underLine[] = "===========================================================================";

/*
 * MakeLog - make the log file
 */
void MakeLog( BOOL wasfault )
{
    char        *str;

    if( !startLogFile() ) {
        str = GetRCString( STR_LOG_OPEN_FAILED );
        MessageBox( NULL, LogInfo.filename, str, MB_OK | MB_SYSTEMMODAL );
        return;
    }
    MinAddrSpaces = 20;
    logPrint( "%s\n", underLine );
    logSysInfo( wasfault );
    if( wasfault ) {
        logMyTask();
        logStackTrace();
    }
    logCurrentTasks();
    logModules();
    logGDI();
    logUSER();
    logMemory();
    logPrint( "%s\n", underLine );
    finishLogFile();

} /* MakeLog */
