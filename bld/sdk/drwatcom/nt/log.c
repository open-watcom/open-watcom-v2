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


#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <string.h>
#include <io.h>
#include <sys\stat.h>
#include "drwatcom.h"
#include "srchmsg.h"
#include "intdlg.h"
#include "log.h"
#include "memdmp.h"
#include "mem.h"

#define IsNT( x )       ( !( x & 0x80000000 ) )
#define GetMinVer( x )  ( ( x & 0x0000FF00 ) >> 8 )
#define GetMajVer( x )  ( x & 0x000000FF )
#define INDENT          5
#define BUF_SIZE        _MAX_PATH

typedef struct {
    HANDLE              prochdl;
    MemListData         list;
}SelMemDlgInfo;

typedef FILE    *LogFP;


static BOOL     ignoreLogChecks;
static BOOL     notesAdded;
static LogFP    logFileHdl;
static char     *logLine = { "==============================================="
                             "========================\n" };

static msglist ProcessorNames[] = {
    PROCESSOR_INTEL_386,                "80386",
    PROCESSOR_INTEL_486,                "80486",
//    PROCESSOR_INTEL_860,              "860",
//    PROCESSOR_MIPS_R2000,             "MIPS R2000",
//    PROCESSOR_MIPS_R3000,             "MIPS R3000",
    PROCESSOR_MIPS_R4000,               "MIPS R4000",
//    PROCESSOR_ALPHA_2164,             "ALPHA 2164",
    0,                                  NULL
};

msglist Actions[] = {
    INT_CHAIN_TO_NEXT,                  (char *)STR_FAULT_PASSED_ON,
    INT_TERMINATE,                      (char *)STR_PROCESS_TERMINATED,
    INT_RESTART,                        (char *)STR_INSTR_RESTARTED,
    0,                                  (char *)-1
};

/*
 * CheckLogSize
 */
void CheckLogSize( void ) {

    struct stat         st;
    char                buf[100];
    int                 ret;

    /* if the user has already said they don't want to delete a large log
     * file don't keep asking */
    if( ignoreLogChecks ) return;
    if( stat( LogData.logname, &st ) == -1 ) return;
    if( st.st_size > LogData.max_flen ) {
        RCsprintf( buf, STR_DEL_BIG_LOG_FILE, st.st_size );
        ret = MessageBox( MainHwnd, buf, AppName,
                          MB_YESNO | MB_SETFOREGROUND | MB_ICONQUESTION );
        if( ret == IDYES ) {
            remove( LogData.logname );
        } else {
            ignoreLogChecks = TRUE;
        }
    }
}

/*
 * startLogFile - start log for this session
 */
static BOOL startLogFile( void )
{
    logFileHdl = fopen( LogData.logname, "a" );
    if( logFileHdl != NULL ) {
        fseek( logFileHdl, 0, SEEK_END );
        return( FALSE );
    } else {
        return( TRUE );
    }
}

/*
 * finishLogFile - close up log file
 */
static finishLogFile( void )
{
    fclose( logFileHdl );
}

static logPrintf( DWORD id, ... ) {

    va_list     al;

    va_start( al, id );
    RCvfprintf( logFileHdl, id, al );
    va_end( al );
}

static logStrPrintf( char *str, ... ) {

    va_list     al;

    va_start( al, str );
    vfprintf( logFileHdl, str, al );
    va_end( al );
}


/***************************************************************************
 * Functions below this point should not know what a LogFP is.
 ***************************************************************************/

/*
 * NotePrint
 */
void NotePrint( char *str ) {
    if( !notesAdded ) {
        logPrintf( STR_USER_NOTES );
        notesAdded = TRUE;
    }
    logStrPrintf( "     %s\n", str );
}

/*
 * logSysInfo - record basic system info
 */
static void logSysInfo( ExceptDlgInfo *faultinfo )
{
    char        *str;
    time_t      tod;
    DWORD       ver;
    SYSTEM_INFO sysinfo;
    char        name[ MAX_COMPUTERNAME_LENGTH + 1 ];
    DWORD       bufsize;

    tod = time( NULL );
    str = ctime( &tod );
    if( faultinfo != NULL ) {
        logPrintf( STR_FAULT_FOUND_ON_X, AppName, str );
    } else {
        logPrintf( STR_LOG_TAKEN_ON_X, AppName, str );
    }

    bufsize = sizeof( name );
    GetComputerName( name, &bufsize );
    logPrintf( STR_COMPUTER_NAME, name );

    bufsize = sizeof( name );
    GetUserName( name, &bufsize );
    logPrintf( STR_USER_NAME, name );

    ver = GetVersion();
    logPrintf( STR_OPERATING_SYSTEM, IsNT( ver ) ? "Windows NT":"Win32s" );
    logPrintf( STR_OS_VERSION, (int)GetMajVer( ver ), (int)GetMinVer( ver ) );
    GetSystemInfo( &sysinfo );

    str = SrchMsg( sysinfo.dwProcessorType, ProcessorNames, NULL );
    if( str == NULL ) {
        str = AllocRCString( STR_UNKNOWN );
        logPrintf( STR_PROCESSOR_TYPE, str );
        FreeRCString( str );
    } else {
        logPrintf( STR_PROCESSOR_TYPE, str );
    }
    logPrintf( STR_NUM_PROCESSORS, sysinfo.dwNumberOfProcessors );
}

/*
 * logDisasm - log some disassembly
 */
static void logDisasm( ExceptDlgInfo *info ) {

    int         i;
    ADDRESS     addr;
    char        str[256];

    SetDisasmInfo( info->procinfo->prochdl, info->module );
    addr.seg = info->context.SegCs;
    addr.offset = info->context.Eip;
    InstructionBackup( LogData.asm_bkup, &addr );
    for( i=0;i<LogData.asm_cnt; i++ ) {
        if( addr.offset == info->context.Eip ) {
            logStrPrintf( "--->" );
        } else {
            logStrPrintf( "    " );
        }
        addr.offset += Disassemble( &addr, str, TRUE );
        logStrPrintf( "%s\n", str );
    }

} /* logDisasm */

/*
 * logStack
 */
static void logStack( ExceptDlgInfo *info ) {

    char        data[20];
    BOOL        rc;
    WORD        i;
    DWORD       offset;
    DWORD       linecnt;
    DWORD       bytesread;
    DWORD       dw;

    logPrintf( STR_STACK_DATA );
    offset = info->context.Esp;
    for( linecnt=0; linecnt < 20; linecnt ++ ) {
        rc = ReadProcessMemory( info->procinfo->prochdl, (void *)offset,
                                data, 16, &bytesread );
        if( !rc ) break;
        logStrPrintf( "%*s%08lX - ", INDENT, "", offset );
        for( i=0; i < bytesread; i +=4 ) {
            dw = *(DWORD *)(data + i );
            logStrPrintf( "%08lX ", dw );
        }
        logStrPrintf( "\n" );
        offset += 16;
    }
}

/*
 * logRegisters
 */
static void logRegisters( ExceptDlgInfo *info ) {

    logPrintf( STR_REGISTER_CONTENTS );
    logStrPrintf( "%*sEAX=%08lX EBX=%08lX ECX=%08lX EDX=%08lX\n",
                INDENT, "", info->context.Eax, info->context.Ebx,
                info->context.Ecx, info->context.Edx );
    logStrPrintf( "%*sEDI=%08lX ESI=%08lX EBP=%08lX ESP=%08lX\n",
                INDENT, "", info->context.Edi, info->context.Esi,
                info->context.Ebp, info->context.Esp );
    logStrPrintf( "%*sEIP=%08X FLAGS=%04X\n\n", INDENT, "",
                info->context.Eip, info->context.EFlags );
    logStrPrintf( "%*scs=%04X ds=%04X es=%04X\n", INDENT, "",
                info->context.SegCs, info->context.SegDs,
                info->context.SegEs );
    logStrPrintf( "%*sfs=%04X gs=%04X ss=%04X\n", INDENT, "",
                info->context.SegFs, info->context.SegGs,
                info->context.SegSs );
}

/*
 * logProcessList
 */

static void logProcessList( void ) {

    ProcList    info;
    ProcPlace   place;
    BOOL        rc;

    RefreshInfo();
    logPrintf( STR_CURRENT_PROCESSES );
    rc = GetNextProcess( &info, &place, TRUE );
    while( rc ) {
        logStrPrintf( "%*s%08lX %s\n", INDENT, "", info.pid, info.name );
        if( LogData.log_modules ) {
            logModules( info.pid, INDENT + 9 );
        }
        rc = GetNextProcess( &info, &place, FALSE );
    }
}

static void logMemManClass( MemByType *info ) {

    logPrintf( STR_NO_ACCESS,           2 * INDENT, "", info->noaccess / 1024 );
    logPrintf( STR_READ_ONLY,           2 * INDENT, "", info->read / 1024 );
    logPrintf( STR_READ_WRITE,          2 * INDENT, "", info->write / 1024 );
    logPrintf( STR_WRITE_COPY,          2 * INDENT, "", info->copy / 1024 );
    logPrintf( STR_EXECUTE,             2 * INDENT, "", info->exec / 1024 );
    logPrintf( STR_EXECUTE_READ,        2 * INDENT, "", info->execread / 1024 );
    logPrintf( STR_EXECUTE_READ_WRITE,  2 * INDENT, "", info->execwrite / 1024 );
    logPrintf( STR_EXECUTE_WRITE_COPY,  2 * INDENT, "", info->execcopy / 1024 );
    logStrPrintf( "%*s                    ==========\n", 2 * INDENT, "" );
    logPrintf( STR_TOTAL,               2 * INDENT, "", info->tot / 1024 );
}

/*
 * logMemManInfo
 */
static void logMemManInfo( DWORD procid ) {

    MemByType   imageinfo;
    MemInfo     meminfo;
    SYSTEM_INFO sysinfo;
    DWORD       i;

    logPrintf( STR_VIRTUAL_MEM_INFO );
    GetSystemInfo( &sysinfo );
    logPrintf( STR_VIRTUAL_PAGE_SIZE,
               INDENT, "", sysinfo.dwPageSize, sysinfo.dwPageSize / 1024 );
    if( GetMemInfo( procid, &meminfo ) ) {
        logPrintf( STR_RESERVED_MEM, INDENT, "" );
        logMemManClass( &meminfo.res );
        logPrintf( STR_COMMITTED_MEM, INDENT, "" );
        logMemManClass( &meminfo.mapped );
        logPrintf( STR_IMAGE_ADDR_SPACE_FOR, INDENT, "", TOTAL_MEM_STR );
        logMemManClass( &meminfo.image );
        for( i=0; i < meminfo.modcnt; i++ ) {
            logPrintf( STR_IMAGE_ADDR_SPACE_FOR, INDENT, "",
                        meminfo.modlist[i] );
            if( GetImageMemInfo( procid, meminfo.modlist[i], &imageinfo ) ) {
                logMemManClass( &imageinfo );
            } else {
                logPrintf( STR_NOT_AVAILABLE, 2 * INDENT, "" );
            }
        }
        FreeModuleList( meminfo.modlist, meminfo.modcnt );
    } else {
        logPrintf( STR_NOT_AVAILABLE, INDENT, "" );
    }
}

#define MEM_DMP_WIDTH   16

/*
 * logMemLine
 * NB data must be at least width+1 bytes long
 */
static void logMemLine( char *data, DWORD offset, DWORD width ) {

    DWORD       i;

    logStrPrintf( "%08lX  ", offset );
    for( i=0; i < width; i++ ) {
        logStrPrintf( "%02X ", (int)data[i] );
        if( !isalnum( data[i] ) && !ispunct( data[i] ) ) {
            data[i] = '.';
        }
    }
    data[width] = '\0';
    logStrPrintf( " %s\n", data );
}

/*
 * logDumpMemItem
 */
static void logDumpMemItem( HANDLE prochdl, MEMORY_BASIC_INFORMATION *mbi ) {

    char        data[MEM_DMP_WIDTH + 1];
    DWORD       offset;
    DWORD       limit;
    DWORD       bytesleft;
    BOOL        ret;

    offset = (DWORD)mbi->BaseAddress;
    limit = offset + mbi->RegionSize;
    data[ MEM_DMP_WIDTH ] = '\0';
    ret = ReadProcessMemory( prochdl, (void *)offset, data,
                             MEM_DMP_WIDTH, NULL );
    if( !ret ) {
        logPrintf( STR_CANT_READ_MEM );
        return;
    }
    for( ;; ) {
        logMemLine( data, offset, MEM_DMP_WIDTH );
        offset += MEM_DMP_WIDTH;
        if( offset + MEM_DMP_WIDTH > limit ) break;
        ReadProcessMemory( prochdl, (void *)offset, data,
                           MEM_DMP_WIDTH, NULL );
    }
    if( offset < limit ) {
        bytesleft = limit - offset;
        ReadProcessMemory( prochdl, (void *)offset, data, bytesleft, NULL );
        logMemLine( data, offset, bytesleft );
    }
}

BOOL CALLBACK MemDmpDlgProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    char                        buf[150];
    SelMemDlgInfo               *info;
    DWORD                       selcnt;
    int                         *selitems;
    DWORD                       i;
    DWORD                       index;
    HWND                        lb;
    MEMORY_BASIC_INFORMATION    *mbi;

    info = (SelMemDlgInfo *)GetWindowLong( hwnd, DWL_USER );
    switch( msg ) {
    case WM_INITDIALOG:
        info = (SelMemDlgInfo *)lparam;
        SetWindowLong( hwnd, DWL_USER, lparam );
        lb = GetDlgItem( hwnd, DMP_BOX );
        SetDlgMonoFont( hwnd, DMP_BOX );
        SetDlgMonoFont( hwnd, DMP_LABEL );
        SetDlgItemText( hwnd, DMP_LABEL, MEM_WALKER_HEADER );
        for( i=0; i < info->list.used; i++ ) {
            if( info->list.data[i]->mbi.State == MEM_COMMIT ) {
                FormatMemListEntry( buf, info->list.data[i] );
                index = SendMessage( lb, LB_ADDSTRING, 0, (DWORD)buf );
                SendMessage( lb, LB_SETITEMDATA, index, i );
            }
        }
        break;
    case WM_COMMAND:
        switch( LOWORD( wparam ) ) {
        case IDOK:
            lb = GetDlgItem( hwnd, DMP_BOX );
            selcnt = SendMessage( lb, LB_GETSELCOUNT, 0, 0 );
            selitems = MemAlloc( selcnt * sizeof( int ) );
            SendMessage( lb, LB_GETSELITEMS, selcnt, (DWORD)selitems );
            if( selcnt > 0 ) logStrPrintf( "\n" );
            for( i=0; i < selcnt; i++ ) {
                index = SendMessage( lb, LB_GETITEMDATA, selitems[i], 0 );
                mbi = &info->list.data[ index ]->mbi;
                logPrintf( STR_MEM_DMP_X_TO_Y,
                           mbi->BaseAddress,
                           (DWORD)mbi->BaseAddress + mbi->RegionSize );
                logDumpMemItem( info->prochdl, mbi );
            }
            MemFree( selitems );
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case IDCANCEL:
            SendMessage( hwnd, WM_CLOSE, 0, 0L );
            break;
        case DMP_SEL_ALL:
            SendDlgItemMessage( hwnd, DMP_BOX, LB_SELITEMRANGE, TRUE,
                                MAKELPARAM( 0, info->list.used - 1 ) );
            break;
        case DMP_CLEAR_ALL:
            SendDlgItemMessage( hwnd, DMP_BOX, LB_SELITEMRANGE, FALSE,
                                MAKELPARAM( 0, info->list.used - 1 ) );
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    default:
        return( FALSE );
        break;
    }
    return( TRUE );
}

/*
 * logMemDump
 */
static void logMemDmp( ExceptDlgInfo *info ) {

    SelMemDlgInfo       selinfo;
    DWORD               i;
    char                buf[150];

    logPrintf( STR_PROCESS_MEM_DMP );
    selinfo.list.allocated = 0;
    selinfo.list.used = 0;
    selinfo.list.data = NULL;
    selinfo.prochdl = info->procinfo->prochdl;
    RefreshMemList( info->procinfo->procid, info->procinfo->prochdl,
                    &selinfo.list );
    logStrPrintf( "%s\n\n", MEM_WALKER_HEADER );
    for( i=0; i < selinfo.list.used; i++ ) {
        FormatMemListEntry( buf, selinfo.list.data[i] );
        logStrPrintf( "%s\n", buf );
    }
    DialogBoxParam( Instance, "SEL_MEM_TO_DMP", NULL, MemDmpDlgProc,
                    (DWORD)&selinfo );
    FreeMemList( &selinfo.list );
}

#define FNAME_BUFLEN    50
/*
 * logFaultInfo
 */
static void logFaultInfo( ExceptDlgInfo *info ) {

    char        *str;
    char        buf[150];
    char        fname[ FNAME_BUFLEN ];
    DWORD       type;
    DWORD       line;
    DWORD       gptype;
    ProcStats   stats;

    logStrPrintf( "\n" );
    logPrintf( STR_OFFENDING_PROC_ULINE );
    logPrintf( STR_OFFENDING_PROC_INFO );
    logPrintf( STR_OFFENDING_PROC_ULINE );

    while( !GetProcessInfo( info->procinfo->procid, &stats ) ) {
        Sleep( 100 );
        RefreshInfo();
    }
    logPrintf( STR_OFFENDING_PROC_X, stats.name, info->procinfo->procid );
    type = info->dbinfo->u.Exception.ExceptionRecord.ExceptionCode;
    FormatException( buf, type );
    logPrintf( STR_ERR_OCCURRED_AT_X_Y, buf, info->context.SegCs,
                info->dbinfo->u.Exception.ExceptionRecord.ExceptionAddress );
    if( type == EXCEPTION_ACCESS_VIOLATION ) {
        if( info->dbinfo->u.Exception.ExceptionRecord.ExceptionInformation[0] ) {
            gptype = STR_LOG_INV_WRITE_TO;
        } else {
            gptype = STR_LOG_INV_READ_FROM;
        }
        logPrintf( gptype,
          info->dbinfo->u.Exception.ExceptionRecord.ExceptionInformation[1] );
    }
    str = SrchMsg( info->action, Actions, NULL );
    if( str != NULL ) {
        logStrPrintf( "%s\n", str );
    }

    logPrintf( STR_MODULES_LOADED );
    logModules( info->procinfo->procid, INDENT );
    logRegisters( info );
    logPrintf( STR_SOURCE_INFORMATION );
    if( GetLineNum( info->module, info->context.Eip,
                    fname, FNAME_BUFLEN, &line ) ) {
        logPrintf( STR_LOG_LINE_X_OF_FILE, INDENT, "", line, fname );
    } else {
        logPrintf( STR_LOG_N_A, INDENT, "" );
    }
    logPrintf( STR_DISASSEMBLY );
    logDisasm( info );
    logStack( info );
    if( LogData.log_mem_manager ) {
        logMemManInfo( info->procinfo->procid );
    }
    if( LogData.log_mem_dmp ) {
        logMemDmp( info );
    }
}

static logModules( DWORD pid, WORD indent ) {

    char        **modules;
    char        end[10];
    DWORD       cnt;
    DWORD       i;
    ProcNode    *pnode;
    ModuleNode  *mnode;
    char        *name;

    pnode = FindProcess( pid );
    if( pnode != NULL ) {
        mnode = GetFirstModule( pnode );
        while( mnode != NULL ) {
            if( mnode->size == -1 ) {
                strcpy( end, "????????" );
            } else {
                sprintf( end, "%08lX", mnode->base + mnode->size );
            }
            if( mnode->name == NULL ) {
                name = "???";
            } else {
                name = mnode->name;
            }
            logPrintf( STR_MODULE_WITH_ADDR,
                        indent, "", mnode->base, end, name );
            mnode = GetNextModule( mnode );
        }
    } else {
        modules = GetModuleList( pid, &cnt );
        for( i=0; i < cnt; i++ ) {
            logPrintf( STR_MODULE, indent, "", modules[i] );
        }
        FreeModuleList( modules, cnt );
    }
}

/*
 * MakeLog - make the log file
 */
void MakeLog( ExceptDlgInfo *faultinfo )
{
    RefreshCostlyInfo();
    if( startLogFile() ) {
        char    err_buf[ _MAX_PATH + 100 ];
        RCsprintf( err_buf, STR_CANT_OPEN_LOG, LogData.logname );
        MessageBox( NULL, err_buf, AppName, MB_OK | MB_ICONEXCLAMATION
                                            | MB_SETFOREGROUND );
        return;
    }
    logSysInfo( faultinfo );
    notesAdded = FALSE;
    if( LogData.query_notes ) {
        AnotateLog( MainHwnd, Instance, NotePrint );
    }
    if( faultinfo != NULL ) {
        logFaultInfo( faultinfo );
    }
    if( LogData.log_process ) {
        logProcessList();
    }
    logStrPrintf( logLine );
    finishLogFile();
}

/*
 * EraseLog
 */
void EraseLog( void ) {

    char        buf[500];
    int         ret;

    RCsprintf( buf, STR_ERASE_LOG_FILE, LogData.logname );
    ret = MessageBox( NULL, buf, AppName, MB_YESNO | MB_ICONQUESTION );
    if( ret == IDYES ) {
        remove( LogData.logname );
        ignoreLogChecks = FALSE;
    }
}

/*
 * ViewLog
 */
void ViewLog( void ) {

    char                        buf[ MAX_CMDLINE + _MAX_PATH ];
    BOOL                        ret;
    STARTUPINFO                 startinfo;
    PROCESS_INFORMATION         procinfo;

    if( access( LogData.logname, R_OK ) ) {
        RCsprintf( buf, STR_CANT_OPEN_LOG_FILE, LogData.logname );
        MessageBox( MainHwnd, buf, AppName,
                    MB_OK | MB_ICONEXCLAMATION | MB_SETFOREGROUND );
        return;
    }
    memset( &startinfo, 0, sizeof( STARTUPINFO ) );
    startinfo.cb = sizeof( STARTUPINFO );
    strcpy( buf, LogData.editor );
    strcat( buf, " " );
    strcat( buf, LogData.editor_cmdline );
    strcat( buf, " " );
    strcat( buf, LogData.logname );

    ret = CreateProcess( LogData.editor,        /* application path */
                   buf,                         /* command line */
                   NULL,                        /* process security
                                                   attributes */
                   NULL,                        /* main thread security
                                                   attributes */
                   FALSE,                       /* inherits parent handles */
                   NORMAL_PRIORITY_CLASS,       /* create parameters */
                   NULL,                        /* environment block */
                   NULL,                        /* current directory */
                   &startinfo,                  /* other startup info */
                   &procinfo );                 /* structure to get process
                                                   info */
    if( ret )  return;
    ret = CreateProcess( "notepad.exe",         /* application path */
                   LogData.logname,             /* command line */
                   NULL,                        /* process security
                                                   attributes */
                   NULL,                        /* main thread security
                                                   attributes */
                   FALSE,                       /* inherits parent handles */
                   NORMAL_PRIORITY_CLASS,       /* create parameters */
                   NULL,                        /* environment block */
                   NULL,                        /* current directory */
                   &startinfo,                  /* other startup info */
                   &procinfo );                 /* structure to get process
                                                   info */
    if( !ret ) {
        RCMessageBox( MainHwnd, STR_CANT_START_EDITOR,
                      AppName, MB_OK | MB_ICONEXCLAMATION );
    }
}

/*
 * fillLogOptions
 */
static void fillLogOptions( HWND hwnd ) {

    char        buf[50];

    if( LogData.log_process ) {
        CheckDlgButton( hwnd, LOG_TASKS, TRUE );
    }
    if( LogData.log_stacktrace ) {
        CheckDlgButton( hwnd, LOG_STACK_TRACE, TRUE );
    }
    if( LogData.log_mem_manager ) {
        CheckDlgButton( hwnd, LOG_MEM, TRUE );
    }
    if( LogData.log_mem_dmp ) {
        CheckDlgButton( hwnd, LOG_MEM_DMP, TRUE );
    }
    if( LogData.query_notes ) {
        CheckDlgButton( hwnd, LOG_QUERY_NOTES, TRUE );
    }
    if( LogData.autolog ) {
        CheckDlgButton( hwnd, LOG_AUTOLOG, TRUE );
    }
    if( LogData.log_restarts ) {
        CheckDlgButton( hwnd, LOG_NO_RESTART, TRUE );
    }
    if( LogData.log_chains ) {
        CheckDlgButton( hwnd, LOG_NO_CHAIN, TRUE );
    }
    if( LogData.log_modules ) {
        CheckDlgButton( hwnd, LOG_LOADED_MODULES, TRUE );
    }
    sprintf( buf, "%ld", LogData.max_flen );
    SetDlgItemText( hwnd, LOG_MAXFL, buf );
    sprintf( buf, "%ld", LogData.asm_cnt );
    SetDlgItemText( hwnd, LOG_DISASM_LINES, buf );
    sprintf( buf, "%ld", LogData.asm_bkup );
    SetDlgItemText( hwnd, LOG_DISASM_BACKUP, buf );
    SetDlgItemText( hwnd, LOG_VIEWER, strlwr( LogData.editor ) );
    SetDlgItemText( hwnd, LOG_FILE_NAME, strlwr( LogData.logname ) );

    SendDlgItemMessage( hwnd, LOG_MAXFL, EM_LIMITTEXT, BUF_SIZE - 1, 0 );
    SendDlgItemMessage( hwnd, LOG_DISASM_BACKUP, EM_LIMITTEXT,
                        BUF_SIZE - 1, 0 );
    SendDlgItemMessage( hwnd, LOG_DISASM_LINES, EM_LIMITTEXT,
                        BUF_SIZE - 1, 0 );
    SendDlgItemMessage( hwnd, LOG_VIEWER, EM_LIMITTEXT, BUF_SIZE - 1, 0 );
    SendDlgItemMessage( hwnd, LOG_FILE_NAME, EM_LIMITTEXT, BUF_SIZE - 1, 0 );
}

#define NUM_HELP_SIZE   150
static BOOL readLogOptions( HWND hwnd, char *buf ) {

    DWORD       asm_lines;
    DWORD       asm_bkup;
    DWORD       max_fsize;
    char        numhelp[NUM_HELP_SIZE];

    CopyRCString( STR_FIELD_MUST_BE_NUMERIC, numhelp, NUM_HELP_SIZE );
    GetDlgItemText( hwnd, LOG_MAXFL, buf, BUF_SIZE );
    if( !ParseNumeric( buf, FALSE, &max_fsize ) ) {
        RCsprintf( buf, STR_MAX_LOG_FILE_SIZE_INV, numhelp );
        MessageBox( hwnd, buf, AppName, MB_OK | MB_ICONEXCLAMATION );
        return( FALSE );
    }
    GetDlgItemText( hwnd, LOG_DISASM_LINES, buf, BUF_SIZE );
    if( !ParseNumeric( buf, FALSE, &asm_lines ) ) {
        RCsprintf( buf, STR_DISASM_LINES_INVALID, numhelp );
        MessageBox( hwnd, buf, AppName, MB_OK | MB_ICONEXCLAMATION );
        return( FALSE );
    }
    GetDlgItemText( hwnd, LOG_DISASM_BACKUP, buf, BUF_SIZE );
    if( !ParseNumeric( buf, FALSE, &asm_bkup ) ) {
        RCsprintf( buf, STR_DISASM_BKUP_INVALID, numhelp );
        MessageBox( hwnd, buf, AppName, MB_OK | MB_ICONEXCLAMATION );
        return( FALSE );
    }
    GetDlgItemText( hwnd, LOG_FILE_NAME, LogData.logname, BUF_SIZE );
    GetDlgItemText( hwnd, LOG_VIEWER, LogData.editor, BUF_SIZE );
    LogData.asm_cnt = asm_lines;
    LogData.asm_bkup = asm_bkup;
    LogData.max_flen = max_fsize;
    LogData.log_process = IsDlgButtonChecked( hwnd, LOG_TASKS );
    LogData.log_stacktrace = IsDlgButtonChecked( hwnd, LOG_STACK_TRACE );
    LogData.log_mem_manager = IsDlgButtonChecked( hwnd, LOG_MEM );
    LogData.log_mem_dmp = IsDlgButtonChecked( hwnd, LOG_MEM_DMP );
    LogData.query_notes = IsDlgButtonChecked( hwnd, LOG_QUERY_NOTES );
    LogData.autolog = IsDlgButtonChecked( hwnd, LOG_AUTOLOG );
    LogData.log_restarts = IsDlgButtonChecked( hwnd, LOG_NO_RESTART );
    LogData.log_chains = IsDlgButtonChecked( hwnd, LOG_NO_CHAIN );
    LogData.log_modules = IsDlgButtonChecked( hwnd, LOG_LOADED_MODULES );
    return( TRUE );
}

/*
 * getNewLogName
 */
static BOOL getNewLogName( HWND parent, char *buf, char *title, BOOL outfile ) {
    OPENFILENAME        of;
    BOOL                ret;
    char                filter[100];
    char                *ptr;

    memset( &of, 0, sizeof( OPENFILENAME ) );
    of.lStructSize = sizeof( OPENFILENAME );
    of.hwndOwner = parent;
    of.hInstance = Instance;
    of.lpstrDefExt = NULL;
    of.lpstrFile = buf;
    of.nMaxFile = BUF_SIZE;
    CopyRCString( STR_ALL_FILE_FILTER, filter, sizeof( filter ) );
    ptr = filter + strlen( filter ) + 1;
    strcpy( ptr, "*.*" );
    ptr += strlen( ptr ) + 1;
    *ptr = '\0';
    of.lpstrFilter = filter;
    of.nFilterIndex = 1L;
    of.lpstrTitle = title;
    if( outfile ) {
        of.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST;
        ret = GetSaveFileName( &of );
    } else {
        of.Flags = OFN_HIDEREADONLY | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
        ret = GetOpenFileName( &of );
    }
    return( ret );
}

/*
 * LogOptsDlgProc
 */
BOOL CALLBACK LogOptsDlgProc( HWND hwnd, UINT msg, UINT wparam, DWORD lparam )
{
    WORD                cmd;
    char                buf[BUF_SIZE];
    char                title[BUF_SIZE];

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        fillLogOptions( hwnd );
        break;
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
        case IDOK:
            if( readLogOptions( hwnd, buf ) ) {
                SendMessage( hwnd, WM_CLOSE, 0, 0 );
            }
            break;
        case IDCANCEL:
            SendMessage( hwnd, WM_CLOSE, 0, 0 );
            break;
        case LOG_BROWSE_FILE:
            CopyRCString( STR_PICK_LOG_FILE, title, BUF_SIZE );
            strcpy( buf, LogData.logname );
            if( getNewLogName( hwnd, buf, title, TRUE ) ) {
                strlwr( buf );
                SetDlgItemText( hwnd, LOG_FILE_NAME, buf );
            }
            break;
        case LOG_BROWSE_VIEWER:
            CopyRCString( STR_PICK_LOG_VIEWER, title, BUF_SIZE );
            strcpy( buf, LogData.editor );
            if( getNewLogName( hwnd, buf, title, FALSE ) ) {
                strlwr( buf );
                SetDlgItemText( hwnd, LOG_VIEWER, buf );
            }
            break;
        }
        break;
    case WM_CLOSE:
        EndDialog( hwnd, 0 );
        break;
    default:
        return( FALSE );
        break;
    }
    return( TRUE );
}

/*
 * SetLogOptions
 */
void SetLogOptions( HWND hwnd ) {
    DialogBox( Instance, "LOG", hwnd, LogOptsDlgProc );
}
