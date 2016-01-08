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
#include <stdlib.h>
#include <string.h>
#include <dos.h>
#include <sys/stat.h>
#include "bool.h"
#include "wdebug.h"
#include "drwatcom.h"
#include "dump.h"
#include "tinyio.h"
#include "getcsip.h"
#include "jdlg.h"

static HTASK            currTask;

#define BLOCKSIZE 32000

#if(0)
static char             dumpFile[MAX_FNAME];
static int              dirCnt;
static int              modCnt;
static int              invokeDebugger;
static dir_ent          FAR *dirList;
static mod_ent          FAR *modList;

/*
 * DumpGE - dump a global entry
 */
static long DumpGE( HWND hwnd, char FAR *buff, int fh, GLOBALENTRY *ge )
{
    DWORD               bytes;
    DWORD               towrite;
    DWORD               offset;
    char                str[64];

    bytes = ge->dwBlockSize;
    offset = 0;

    sprintf( str,"%04x, size=%ld", (WORD)ge->hBlock, bytes );
    SetDlgItemText( hwnd, DUMP_CURR_SEG, str );

    while( bytes != 0 ) {
        towrite = bytes;
        if( towrite > BLOCKSIZE ) {
            towrite = BLOCKSIZE;
        }
        ReadMem( ge->hBlock, offset, buff, towrite );
        TinyWrite( fh, buff, towrite );
        offset += towrite;
        bytes -= towrite;
    }

    return( TINY_INFO_TELL( TinyTell( fh ) ) );

} /* DumpGE */

/*
 * AddDEData - add data to a directory entry
 */
static BOOL AddDEData( dir_ent *de, GLOBALENTRY *ge, DWORD fpos, WORD seg  )
{
    void        FAR *ptr;
    int         i;

    de->offset = fpos;
    de->seg_num = seg;
    if( ge->dwSize != 1 ) {
        ptr = GlobalLock( ge->hBlock );
        de->sel = FP_SEG( ptr );
    }  else {
        de->sel = ge->hBlock;
    }
    de->byte_count = ge->dwBlockSize;
    GetADescriptor( de->sel, &de->desc );
    if( ge->dwSize != 1 ) {
        GlobalUnlock( ge->hBlock );
    }
    if( de->sel == FP_SEG( AddDEData ) || de->sel == FP_SEG( dirList ) )
        return( TRUE );
    for( i=0;i<dirCnt;i++ ) {
        if( dirList[i].sel == de->sel ) {
            return( TRUE );
        }
    }
    return( FALSE );

} /* AddDEData */

/*
 * NewDirEntry - add a new segment directory entry
 */
static void NewDirEntry( dir_ent *de )
{
    if( dirCnt == 0 ) {
        dirList = MemAlloc( sizeof( dir_ent ) );
    } else {
        dirList = MemReAlloc( dirList, (dirCnt+1)*sizeof( dir_ent ) );
    }
    memcpy( &dirList[dirCnt], de, sizeof( dir_ent ) );
    dirCnt++;

} /* NewDirEntry */

/*
 * NewModuleEntry - add a new module entry
 */
static int NewModuleEntry( HANDLE hmod )
{
    WORD        i;
    mod_ent     mme;
    MODULEENTRY me;

    for( i=0;i<modCnt;i++ ) {
        if( modList[i].module_handle == hmod ) {
            return( i );
        }
    }
    MyModuleFindHandle( &me, hmod );
    mme.module_handle = hmod;
    _fstrcpy( mme.exepath, me.szExePath );

    if( modCnt == 0 ) {
        modList = MemAlloc( sizeof( mod_ent ) );
    } else {
        modList = MemReAlloc( modList, (modCnt+1)*sizeof( mod_ent ) );
    }
    memcpy( &modList[modCnt], &mme, sizeof( mod_ent ) );
    modCnt++;
    return( modCnt-1 );

} /* NewModuleEntry */

/*
 * DoDump - take dump of task
 */
void DoDump( HWND hwnd )
{
    GLOBALENTRY         ge;
    dump_header         dh;
    dir_ent             de;
    struct stat         st;
    tiny_ret_t          rc;
    int                 fh;
    int                 i;
    WORD                seg;
    long                fpos;
    char                FAR *buff;
    GLOBALHANDLE        gh;
    int                 modindex;

    gh = GlobalAlloc( GMEM_MOVEABLE, BLOCKSIZE );
    buff = GlobalLock( gh );

    if( gh == NULL ) {
        MessageBox( NULL, "No memory to take dump!", AppName,
                        MB_OK | MB_TASKMODAL );
        return;
    }

    SetDlgCourierFont( hwnd, DUMP_CURR_SEG );

    /*
     * build dump header
     */
    strcpy( dh.exe_name, DTModuleEntry.szExePath );
    stat( dh.exe_name, &st );
    dh.exe_time = st.st_atime;
    dh.cookie = MAGIC_COOKIE;
    dh.pid = DeadTask;
    dh.winflags = GetWinFlags();
    dh.num_dir_ents = 0;
    dh.num_mod_ents = 0;
    dh.dir_offset = 0;
    dh.mod_offset = 0;
    dh.regs.EAX = IntData.EAX;
    dh.regs.EBX = IntData.EBX;
    dh.regs.ECX = IntData.ECX;
    dh.regs.EDX = IntData.EDX;
    dh.regs.ESI = IntData.ESI;
    dh.regs.EDI = IntData.EDI;
    dh.regs.EBP = IntData.EBP;
    dh.regs.ESP = IntData.ESP;
    dh.regs.EFLAGS = IntData.EFlags;
    dh.regs.CS = IntData.CS;
    dh.regs.DS = IntData.DS;
    dh.regs.ES = IntData.ES;
    dh.regs.FS = IntData.FS;
    dh.regs.GS = IntData.GS;
    dh.regs.SS = IntData.SS;

    /*
     * init. dump file
     */
    remove( dumpFile );
    rc = TinyCreate( dumpFile, 0 );
    TinyClose( TINY_INFO( rc ) );
    rc = TinyOpen( dumpFile, TIO_WRITE );
    if( TINY_ERROR( rc ) ) {
        MessageBox( NULL,"Could not open dump file",AppName,
                        MB_OK | MB_TASKMODAL );
        return;
    }
    fh = TINY_INFO( rc );
    TinyWrite( fh, &dh, sizeof( dh ) );
    fpos = TINY_INFO_TELL( TinyTell( fh ) );
    dirCnt = 0;
    modCnt = 0;

    /*
     * dump task segments
     */
    modindex = NewModuleEntry( DTTaskEntry.hModule );
    seg = 1;
    while( seg < 32768 ) {
        if( DoGlobalEntryModule( &ge, DTTaskEntry.hModule, seg ) ) {
            de.module_index = modindex;
            AddDEData( &de, &ge, fpos, seg );
            fpos = DumpGE( hwnd, buff, fh, &ge );
            NewDirEntry( &de );
        }
        seg++;
    }

    /*
     * dump global heap, if required
     */
    if( DumpHow != DUMP_TASK ) {
        MyGlobalFirst( &ge, GLOBAL_ALL );
        do {
            if( ge.hOwner != Instance ) {
                if( ge.hOwner == DeadTask ||
                    ge.hOwner == DTTaskEntry.hModule ||
                    DumpHow == DUMP_ALL_MEMORY ) {

                    modindex = NewModuleEntry( ge.hOwner );
                    de.module_index = modindex;
                    if( ge.wType == GT_CODE ) {
                        seg = ge.wData;
                    } else if( ge.wType == GT_DGROUP ) {
                        /* !!!!!! need seg number of DGROUP !!!!!! */
                        seg = ge.wData;
                    } else {
                        seg = -1;
                    }
                    if( !AddDEData( &de, &ge, fpos, seg ) ) {
                        fpos = DumpGE( hwnd, buff, fh, &ge );
                        NewDirEntry( &de );
                    }
                }
            }
        } while( MyGlobalNext( &ge, GLOBAL_ALL ) );
    }

    GlobalUnlock( gh );
    GlobalFree( gh );

    /*
     * dump the directory
     */
    dh.num_dir_ents = dirCnt;
    dh.dir_offset = TINY_INFO_TELL( TinyTell( fh ) );
    for( i=0; i<dirCnt; i++ ) {
        TinyWrite( fh, &dirList[i], sizeof( dir_ent ) );
    }

    /*
     * dump the module table
     */
    dh.num_mod_ents = modCnt;
    dh.mod_offset = TINY_INFO_TELL( TinyTell( fh ) );
    for( i=0; i<modCnt; i++ ) {
        TinyWrite( fh, &modList[i], sizeof( mod_ent ) );
    }

    /*
     * re-write the header with the new info
     */
    TinySeek( fh, 0, TIO_SEEK_START );
    TinyWrite( fh, &dh, sizeof( dh ) );

    TinyClose( fh );
    MemFree( dirList );
    MemFree( modList );

} /* DoDump */

/*
 * DumpDialog - get dump info
 */
BOOL __export FAR PASCAL DumpDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char        drive[_MAX_DRIVE];
    char        dir[_MAX_DIR];
    char        fname[_MAX_FNAME];
    char        ext[_MAX_EXT];

    switch( msg ) {
    case WM_INITDIALOG:
        _splitpath( DTModuleEntry.szExePath, drive, dir, fname, ext );
        _makepath( dumpFile, drive, dir, fname, ".dmp" );
        strlwr( dumpFile );
#if 0
        SetCourierFont( hwnd, DUMP_FILE_NAME );
        SetCourierFont( hwnd, DUMP_MODULE_NAME );
        SetCourierFont( hwnd, DUMP_EXE_NAME );
#endif
        SetDlgItemText( hwnd, DUMP_FILE_NAME, dumpFile );
        SetDlgItemText( hwnd, DUMP_MODULE_NAME, DTModuleEntry.szModule );
        SetDlgItemText( hwnd, DUMP_EXE_NAME, DTModuleEntry.szExePath );
        invokeDebugger = FALSE;
        CheckDlgButton( hwnd, DUMP_INVOKE_DEBUGGER, BST_UNCHECKED );
        CheckRadioButton( hwnd, DUMP_ALL, DUMP_ALL_MEMORY, DumpHow );
        return( TRUE );
    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, IDCANCEL, 0L );
        return( TRUE );
    case WM_COMMAND:
        switch( wparam ) {
        case DUMP_INVOKE_DEBUGGER:
            if( HIWORD( lparam ) == BN_CLICKED ) {
                invokeDebugger = !invokeDebugger;
                if( invokeDebugger ) {
                    CheckDlgButton( hwnd, DUMP_INVOKE_DEBUGGER, BST_CHECKED );
                } else {
                    CheckDlgButton( hwnd, DUMP_INVOKE_DEBUGGER, BST_UNCHECKED );
                }
            }
            break;
        case DUMP_ALL_MEMORY:
        case DUMP_TASK:
        case DUMP_ALL:
            if( HIWORD( lparam ) == BN_CLICKED ) {
                DumpHow = wparam;
            }
            break;
        case IDOK:
            GetDlgItemText( hwnd, DUMP_FILE_NAME, dumpFile, MAX_FNAME );
            DoDump( hwnd );
            if( invokeDebugger ) {
                char str[256];
                sprintf( str, "wvideo /tr=pmd.dll %s", dumpFile );
                WinExec( str, SW_SHOWNORMAL );
            }
            EndDialog( hwnd, 0 );
            break;
        case IDCANCEL:
            EndDialog( hwnd, 0 );
            break;
        }
        return( TRUE );
    }
    return( FALSE );

} /* DumpDialog */

/*
 * DumpTask - run task dump dialog
 */
void DumpTask( HWND hwnd )
{
    FARPROC     fp;

    fp = MakeProcInstance( (FARPROC)DumpDialog, Instance );
    JDialogBox( Instance, "DMPTASK", hwnd, (DLGPROC)fp );
    FreeProcInstance( fp );

} /* DumpTask */

/*
 * SetForDump - set up for dumping a task
 */
void SetForDump( HWND hwnd )
{
    char        bp=0xcc;
    DWORD       csip;

    hwnd = hwnd;

    DumpAny.taskid = currTask;
    csip = GetRealCSIP( currTask, NULL );
    DumpAny.CS = HIWORD( csip );
    DumpAny.EIP = LOWORD( csip );
    DumpAny.dump_pending = TRUE;
    ReadMem( DumpAny.CS, DumpAny.EIP, &DumpAny.oldbyte, 1 );
    WriteMem( DumpAny.CS, DumpAny.EIP, &bp, 1 );
    ReadMem( DumpAny.CS, DumpAny.EIP, &bp, 1 );
    PostAppMessage( currTask, WM_NULL, 0, 0L );
    DirectedYield( currTask );

} /* SetForDump */
#endif
/*
 * FillTaskList - put all tasks into task list box
 */
static void FillTaskList( HWND hwnd )
{
    DWORD       csip;
    TASKENTRY   te;
    char        str[128];

    SendDlgItemMessage( hwnd, TASKCTL_TASKLIST, LB_RESETCONTENT, 0, 0L );
    MyTaskFirst( &te );
    currTask = te.hTask;
    do {
        if( te.hTask != ProgramTask && !IsDip( te.hInst ) ) {
            csip = GetRealCSIP( te.hTask, NULL );
            RCsprintf( str, STR_TASK_LB_FMT_STR, te.szModule,
                       HIWORD( csip ), LOWORD( csip ), (WORD)te.hTask );
            SendDlgItemMessage( hwnd, TASKCTL_TASKLIST, LB_ADDSTRING, 0, (LPARAM)(LPSTR)str );
        }
    } while( MyTaskNext( &te ) );
    SendDlgItemMessage( hwnd, TASKCTL_TASKLIST, LB_SETCURSEL, 0, 0L );
    SendDlgItemMessage( hwnd, TASKCTL_TASKLIST, LB_GETTEXT, 0, (LPARAM)(LPSTR)str );

} /* FillTaskList */

/*
 * DebuggerOptDlg - manage the dialog to get debugger options
 */
BOOL __export FAR PASCAL DebuggerOptDlg( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char        str[128];

    lparam = lparam;
    switch( msg ) {
    case WM_INITDIALOG:
        SetDlgItemText( hwnd, DBG_DEBUGGER_OPT, DebuggerOpts );
        break;
    case WM_COMMAND:
        switch( wparam ) {
        case IDOK:
            GetDlgItemText( hwnd, DBG_DEBUGGER_OPT, DebuggerOpts,
                            sizeof( DebuggerOpts )  );
            sprintf( str,"wdw %s %d", DebuggerOpts, (WORD)currTask );
            WinExec( str, SW_SHOWNORMAL );
            EndDialog( hwnd, 0 );
            break;
        case IDCANCEL:
            EndDialog( hwnd, 0 );
            break;
        }
        break;
    default:
        return( FALSE );
    }
    return( TRUE );
}

/*
 * DumpAnyDialog - select a task to dump
 */
BOOL __export FAR PASCAL DumpAnyDialog( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    char        str[128];
    int         i,j;
    FARPROC     fp;

    lparam = lparam;

    switch( msg ) {
    case WM_INITDIALOG:
        DumpDialogHwnd = hwnd;
        SetDlgCourierFont( hwnd, TASKCTL_TASKLIST );
        FillTaskList( hwnd );
        SetDlgItemText( hwnd, TASKCTL_TASKNAME, str );
        return( TRUE );

    case WM_CLOSE:
        PostMessage( hwnd, WM_COMMAND, IDCANCEL, 0L );
        return( TRUE );

    case WM_COMMAND:
        switch( wparam ) {
        case TASKCTL_REDRAW:
            FillTaskList( hwnd );
            break;
        case TASKCTL_TASKLIST:
            i = SendDlgItemMessage( hwnd, TASKCTL_TASKLIST, LB_GETCURSEL, 0, 0L );
            SendDlgItemMessage( hwnd, TASKCTL_TASKLIST, LB_GETTEXT, i, (LPARAM)(LPSTR)str );
            SetDlgItemText( hwnd, TASKCTL_TASKNAME, str );
            j = strlen( str )-1;
            currTask = 0;
            i = 1;
            while( str[j] != '=' ) {
                if( str[j] >= 'a' && str[j] <='f' ) {
                    currTask += i * (str[j]-'a'+10);
                } else {
                    currTask += i * (str[j]-'0');
                }
                i *= 16;
                j--;
            }
            break;
        case TASKCTL_DEBUG:
            fp = MakeProcInstance( (FARPROC)DebuggerOptDlg, Instance );
            JDialogBox( Instance, "DEBUGGER_OPTS", hwnd, (DLGPROC)fp );
            FreeProcInstance( fp );
            break;
#if( 0 )
        case TASKCTL_DUMP:
            if( DumpAny.dump_pending ) {
                MessageBox( hwnd, "Previous dump is still pending", AppName, MB_OK );
                break;
            }
            SetForDump( hwnd );
            break;
#endif
        case TASKCTL_KILL:
            TerminateApp( currTask, NO_UAE_BOX );
            break;
        case IDCANCEL:
            EndDialog( hwnd, 0 );
            break;
        }
        return( TRUE );
    }
    return( FALSE );

} /* DumpAnyDialog */

/*
 * DumpATask - get a task from a list to dump
 */
void DumpATask( HWND hwnd )
{
    FARPROC     fp;

    fp = MakeProcInstance( (FARPROC)DumpAnyDialog, Instance );
    JDialogBox( Instance, "TASKCTL", hwnd, (DLGPROC)fp );
    DumpDialogHwnd = 0;
    FreeProcInstance( fp );

} /* DumpATask */
