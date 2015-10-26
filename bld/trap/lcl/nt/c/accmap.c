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
* Description:  Routines to keep track of loaded modules and address maps.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "stdnt.h"


typedef struct lli {
    HANDLE      file_handle;
    LPBYTE      base;
    addr_off    code_size;
    LPVOID      except_base;
    addr_off    except_size;
    char        is_16 : 1;
    char        has_real_filename : 1;
    char        newly_unloaded : 1;
    char        newly_loaded : 1;
    char        filename[MAX_PATH + 1];
    char        modname[40]; //
} lib_load_info;

typedef struct list {
    struct list *next;
    int         is_16;
    int         segcount;
    addr48_ptr  _WCUNALIGNED *segs;
    char        *filename;
    char        *modname;
} lib_list_info;

static lib_load_info    *moduleInfo;
static DWORD            lastLib = 0;
static const char       libPrefix[] = "NoName";

static lib_list_info    *listInfoHead;
static lib_list_info    *listInfoTail;

/*
 * freeListItem - free an individual lib list item
 */
static void freeListItem( lib_list_info *curr )
{
    LocalFree( curr->filename );
    LocalFree( curr->modname );
    LocalFree( curr->segs );
    LocalFree( curr );
}

/*
 * FreeLibList - free the lib list info
 */
void FreeLibList( void )
{
    lib_list_info   *curr;
    lib_list_info   *next;

    curr = listInfoHead;
    while( curr != NULL ) {
        next = curr->next;
        freeListItem( curr );
        curr = next;
    }
    listInfoHead = NULL;
    listInfoTail = NULL;
}

/*
 * addModuleToLibList - saves away the information about the current
 *                             module.  This is used to dump it out later
 *                             (TrapListLibs calls DoListLibs)
 */
static void addModuleToLibList( DWORD module )
{
    lib_list_info   *curr;
    lib_load_info   *lli;

    lli = &moduleInfo[module];
    for( curr = listInfoHead; curr != NULL; curr = curr->next ) {
        if( stricmp( lli->modname, curr->modname ) == 0 && stricmp( lli->filename, curr->filename ) == 0 ) {
            return;
        }
    }

    curr = LocalAlloc( LMEM_FIXED, sizeof( lib_list_info ) );
    if( curr == NULL ) {
        return;
    }
    curr->filename = LocalAlloc( LMEM_FIXED, strlen( lli->filename ) + 1 );
    if( curr->filename == NULL ) {
        LocalFree( curr );
        return;
    }
    strcpy( curr->filename, lli->filename );
    curr->modname = LocalAlloc( LMEM_FIXED, strlen( lli->modname ) + 1 );
    if( curr->modname == NULL ) {
        LocalFree( curr->filename );
        LocalFree( curr );
        return;
    }
    strcpy( curr->modname, lli->modname );
    curr->segcount = 0;
    curr->segs = NULL;
    curr->is_16 = lli->is_16;
    curr->next = NULL;
    if( listInfoHead == NULL ) {
        listInfoHead = listInfoTail = curr;
    } else {
        listInfoTail->next = curr;
        listInfoTail = curr;
    }
}

/*
 * RemoveModuleFromLibList - removes a module from our list once it is
 *                           unloaded or exits
 */
void RemoveModuleFromLibList( char *module, char *filename )
{
    lib_list_info   *curr;
    lib_list_info   *prev;

    curr = listInfoHead;
    prev = NULL;
    while( curr != NULL ) {
        if( !stricmp( module, curr->modname ) && !stricmp( filename, curr->filename ) ) {
            if( prev == NULL ) {
                listInfoHead = curr->next;
            } else {
                prev->next = curr->next;
            }
            if( curr == listInfoTail ) {
                listInfoTail = prev;
            }
            freeListItem( curr );
            return;
        }
        prev = curr;
        curr = curr->next;
    }
}

/*
 * addSegmentToLibList - add a new segment. We keep track of this so that
 *                       we can dump it later.
 */
static void addSegmentToLibList( DWORD module, WORD seg, DWORD off )
{
    addr48_ptr  *new;

    if( listInfoTail == NULL ) {
        addModuleToLibList( module );
        if( listInfoTail == NULL ) {
            return;
        }
    }
    new = LocalAlloc( LMEM_FIXED, ( listInfoTail->segcount + 1 ) * sizeof( addr48_ptr ) );

    if( new == NULL ) {
        return;
    }
    if( listInfoTail->segs != NULL ) {
        memcpy( new, listInfoTail->segs, sizeof( addr48_ptr )* listInfoTail->segcount );
    }
    listInfoTail->segs = new;
    listInfoTail->segs[listInfoTail->segcount].segment = seg;
    listInfoTail->segs[listInfoTail->segcount].offset = off;
    listInfoTail->segcount++;
}

BOOL FindExceptInfo( LPVOID off, LPVOID *base, DWORD *size )
{
    unsigned        i;
    lib_load_info   *lli;

    for( i = 0; i < ModuleTop; ++i ) {
        lli = &moduleInfo[i];
        if( (LPBYTE)off >= lli->base && (LPBYTE)off < lli->base + lli->code_size ) {
            /* this is the image */
            if( lli->except_size == 0 ) {
                return( FALSE );
            }
            *base = lli->except_base;
            *size = lli->except_size;
            return( TRUE );
        }
    }
    return( FALSE );
}

static void FillInExceptInfo( lib_load_info *lli )
{
    DWORD           pe_off;
    SIZE_T          bytes;
    exe_pe_header   hdr;

    ReadProcessMemory( ProcessInfo.process_handle, lli->base + OS2_NE_OFFSET, &pe_off, sizeof( pe_off ), &bytes );
    ReadProcessMemory( ProcessInfo.process_handle, lli->base + pe_off, &hdr, sizeof( hdr ), &bytes );
    if( IS_PE64( hdr ) ) {
        lli->code_size = PE64( hdr ).code_base + PE64( hdr ).code_size;
        lli->except_base = lli->base + PE64( hdr ).table[PE_TBL_EXCEPTION].rva;
        lli->except_size = PE64( hdr ).table[PE_TBL_EXCEPTION].size;
    } else {
        lli->code_size = PE32( hdr ).code_base + PE32( hdr ).code_size;
        lli->except_base = lli->base + PE32( hdr ).table[PE_TBL_EXCEPTION].rva;
        lli->except_size = PE32( hdr ).table[PE_TBL_EXCEPTION].size;
    }
}

/*
 * AddProcess - a new process has been created
 */
void AddProcess( header_info *hi )
{
    lib_load_info   *lli;

    moduleInfo = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, sizeof( lib_load_info ) );
    ModuleTop = 1;

    lli = moduleInfo;

#if defined( MD_x86 )
    if( IsWOW || IsDOS ) {
        lli->is_16 = TRUE;
        lli->file_handle = 0;
        lli->base = NULL;
        lli->has_real_filename = TRUE;
        strcpy( lli->modname, hi->modname );
        strcpy( lli->filename, CurrEXEName );
    } else {
#else
    hi=hi;
    {
#endif
        lli->has_real_filename = FALSE;
        lli->is_16 = FALSE;
        lli->file_handle = DebugEvent.u.CreateProcessInfo.hFile;
        // kludge - NT doesn't give us a handle sometimes
        if( lli->file_handle == INVALID_HANDLE_VALUE ) {
            lli->file_handle = CreateFile( CurrEXEName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
        }
        lli->base = DebugEvent.u.CreateProcessInfo.lpBaseOfImage;
        FillInExceptInfo( lli );
        lli->modname[0] = 0;
        lli->filename[0] = 0;
    }
}

/*
 * NameFromProcess - get fully qualified filename for last DLL
 * that was loaded in process. Intended for Win9x.
 */
BOOL NameFromProcess( lib_load_info *lli, DWORD dwPID, char *name )
{
    HANDLE          hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32   me32;
    BOOL            bSuccess = FALSE;

    // Check if we have the KERNEL32 entrypoints.
    if( !pCreateToolhelp32Snapshot || !pModule32First || !pModule32Next )
        goto error_exit;

    // Take a snapshot of all modules in the specified process.
    hModuleSnap = pCreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
    if( hModuleSnap == INVALID_HANDLE_VALUE )
        goto error_exit;

    // Set the size of the structure before using it.
    me32.dwSize = sizeof( MODULEENTRY32 );

    // Attempt to retrieve information about the first module.
    if( !pModule32First( hModuleSnap, &me32 ) )
        goto error_exit;

    // Look for freshly loaded module. Not tested on Win9x.
    // Unfortunately in WinXP not all newly loaded modules are in the list.
    // This should not be relevant as all NT versions will use the PSAPI method anyway.
    // The PSAPI method works reliably but is not available on Win9x.
    do {
        if( me32.modBaseAddr == lli->base ) {
            strcpy( name, me32.szExePath );
            bSuccess = TRUE;
            break;
        }
    } while( pModule32Next( hModuleSnap, &me32 ) );

error_exit:
    if( hModuleSnap != INVALID_HANDLE_VALUE )
        CloseHandle( hModuleSnap );

    return( bSuccess );
}

/*
 * NameFromHandle - get fully qualified filename from file handle.
 * Intended for Windows NT.
 */
BOOL NameFromHandle( HANDLE hFile, char *name )
{
#define BUFSIZE 512
    BOOL        bSuccess = FALSE;
    char        pszFilename[MAX_PATH + 1];
    HANDLE      hFileMap = NULL;
    void        *pMem = NULL;
    char        szTemp[BUFSIZE];
    DWORD       dwFileSizeHi;
    DWORD       dwFileSizeLo;

    name[0] = 0;

    // Check if we have the required entrypoints (results depend on OS version).
    if( (hFile == INVALID_HANDLE_VALUE) || !pGetMappedFileName || !pQueryDosDevice )
        goto error_exit;

    // Get the file size.
    dwFileSizeLo = GetFileSize( hFile, &dwFileSizeHi );
    if( dwFileSizeLo == 0 && dwFileSizeHi == 0 )
        goto error_exit;

    // Create a file mapping object and map the file.
    hFileMap = CreateFileMapping( hFile, NULL, PAGE_READONLY, 0, 1, NULL );
    if( hFileMap == 0 ) {
        goto error_exit;
    }
    pMem = MapViewOfFile( hFileMap, FILE_MAP_READ, 0, 0, 1 );
    if( pMem == 0 ) {
        goto error_exit;
    }
    if( pGetMappedFileName( GetCurrentProcess(), pMem, pszFilename, MAX_PATH ) == 0 ) {
        goto error_exit;
    }

    // Translate path with device name to drive letters.
    szTemp[0] = '\0';

    if( GetLogicalDriveStrings( BUFSIZE - 1, szTemp ) ) {
        char    szName[MAX_PATH];
        char    szDrive[3] = " :";
        BOOL    bFound = FALSE;
        char    *p = szTemp;

        do {
            // Copy the drive letter to the template string
            *szDrive = *p;
            // Look up each device name
            if( pQueryDosDevice( szDrive, szName, BUFSIZE ) ) {
                UINT    uNameLen = (UINT)strlen( szName );
                if( uNameLen < MAX_PATH ) {
                    bFound = ( strnicmp( pszFilename, szName, uNameLen ) == 0
                               && pszFilename[ uNameLen ] == '\\' );
                    if( bFound ) {
                        // Reconstruct pszFilename using szTemp
                        // Replace device path with DOS path
                        strcpy( name, szDrive );
                        strcat( name, pszFilename + uNameLen );
                        bSuccess = TRUE;
                        break;
                    }
                }
            }

            // Go to the next NULL character.
            while( *p++ )
                ;
        } while( !bFound && *p ); // end of string
    }

error_exit:
    if( pMem )
        UnmapViewOfFile( pMem );
    if( hFileMap )
        CloseHandle( hFileMap );
    return( bSuccess );
#undef BUFSIZE
}



/*
 * AddLib - a new library has loaded
 */
void AddLib( BOOL is_16, IMAGE_NOTE *im )
{
    lib_load_info   *lli;

    ModuleTop++;
    lli = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, ModuleTop * sizeof( lib_load_info ) );
    memcpy( lli, moduleInfo, ( ModuleTop - 1 ) * sizeof( lib_load_info ) );
    LocalFree( moduleInfo );
    moduleInfo = lli;
    lli = &moduleInfo[ModuleTop - 1];

#if !defined( WOW ) || defined( MD_x64 )
    (void)im, (void)is_16; // Unused
#else
    if( is_16 ) {
        lli->is_16 = TRUE;
        lli->has_real_filename = TRUE;
        lli->file_handle = 0;
        lli->base = NULL;
        lli->newly_loaded = TRUE;
        lli->newly_unloaded = FALSE;
        strcpy( lli->filename, im->FileName );
        strcpy( lli->modname, im->Module );
    } else
#endif
 {
        lli->is_16 = FALSE;
        lli->has_real_filename = FALSE;
        /*
         * for a 32-bit DLL, we make up a fake name to tell the debugger
         * when the debugger asks to open this fake name, we return the
         * saved file handle
         */
        lli->file_handle = DebugEvent.u.LoadDll.hFile;
        lli->base = DebugEvent.u.LoadDll.lpBaseOfDll;
        lli->modname[0] = 0;
        if ( NameFromHandle( lli->file_handle, lli->filename) ) {
            lli->has_real_filename = TRUE;
        } else if( NameFromProcess( lli, DebugeePid, lli->filename ) ) {
            lli->has_real_filename = TRUE;
        } else if( !GetModuleName( lli->file_handle, lli->filename ) ) {
            lastLib++;
            strcpy( lli->filename, libPrefix );
            ultoa( lastLib, &lli->filename[sizeof( libPrefix ) - 1], 16 );
            strcat( lli->filename, ".dll" );
        }
        FillInExceptInfo( lli );
        lli->newly_loaded = TRUE;
        lli->newly_unloaded = FALSE;
    }
}

void DelLib( void )
{
    unsigned    i;

    for( i = 0; i < ModuleTop; ++i ) {
        if( moduleInfo[i].base == DebugEvent.u.UnloadDll.lpBaseOfDll ) {
            moduleInfo[i].newly_unloaded = TRUE;
            moduleInfo[i].base = NULL;
            moduleInfo[i].code_size = 0;
            if( moduleInfo[i].file_handle != INVALID_HANDLE_VALUE ) {
                CloseHandle( moduleInfo[i].file_handle );
                moduleInfo[i].file_handle = INVALID_HANDLE_VALUE;
            }
            break;
        }
    }
}

void DelProcess( BOOL closeHandles )
{
    unsigned    i;

    for( i = 0; i < ModuleTop; ++i ) {
        if( closeHandles ) {
            CloseHandle( moduleInfo[i].file_handle );
            moduleInfo[i].file_handle = INVALID_HANDLE_VALUE;
        }
        moduleInfo[i].base = NULL;
        moduleInfo[i].code_size = 0;
    }
}

/*
 * force16SegmentLoad - force a wow app to access its segment so that it
 *                      will be loaded into memory.
 */
#ifdef WOW
#if !defined( MD_x64 )

#define INS_BYTES 7

static void force16SegmentLoad( thread_info *ti, WORD sel )
{
    static unsigned char    getMemIns[INS_BYTES] = {
        0x8e, 0xc0, 0x26, 0xa1, 0x00, 0x00, 0xcc
    };
    static unsigned char    origBytes[INS_BYTES];
    static BOOL             gotOrig;
    MYCONTEXT               con;
    MYCONTEXT               oldcon;

    if( !UseVDMStuff ) {
        return;
    }

    if( !gotOrig ) {
        gotOrig = TRUE;
        ReadMem( WOWAppInfo.segment, WOWAppInfo.offset, origBytes, INS_BYTES );
    }
    WriteMem( WOWAppInfo.segment, WOWAppInfo.offset, getMemIns, INS_BYTES );
    MyGetThreadContext( ti, &con );
    oldcon = con;
    con.Eax = sel;
    con.Eip = WOWAppInfo.offset;
    con.SegCs = WOWAppInfo.segment;
    MySetThreadContext( ti, &con );
    DebugExecute( STATE_IGNORE_DEBUG_OUT | STATE_IGNORE_DEAD_THREAD | STATE_EXPECTING_FAULT, NULL, FALSE );
    MySetThreadContext( ti, &oldcon );
    WriteMem( WOWAppInfo.segment, WOWAppInfo.offset, origBytes, INS_BYTES );
}
#endif
#endif

trap_retval ReqMap_addr( void )
{
    int             i;
    HANDLE          handle;
    DWORD           bytes;
    pe_object       obj;
    WORD            seg;
    map_addr_req    *acc;
    map_addr_ret    *ret;
    header_info     hi;
    lib_load_info   *lli;
    WORD            stack;
    int             num_objects;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );

    seg = acc->in_addr.segment;
    switch( seg ) {
    case MAP_FLAT_CODE_SELECTOR:
    case MAP_FLAT_DATA_SELECTOR:
        seg = 0;
        break;
    default:
        --seg;
        break;
    }

    lli = &moduleInfo[acc->handle];

#if !defined( MD_x64 )
#ifdef WOW
    if( lli->is_16 ) {
        LDT_ENTRY   ldt;
        WORD        sel;
        thread_info *ti;
        /*
         * much simpler for a WOW app.  We just ask for the selector that
         * maps to the given segment number.
         */
        ti = FindThread( DebugeeTid );
        pVDMGetModuleSelector( ProcessInfo.process_handle,
                        ti->thread_handle, seg, lli->modname, &sel );
        pVDMGetThreadSelectorEntry( ProcessInfo.process_handle,
                        ti->thread_handle, sel, &ldt );
        if( !ldt.HighWord.Bits.Pres ) {
            /*
             * if the segment is not present, then we make the app load it
             */
            force16SegmentLoad( ti, sel );
        }
        ret->out_addr.segment = sel;
        ret->out_addr.offset = 0;
    } else
#endif
#endif
    {
        DWORD   seek_offset;

        /*
         * for a 32-bit app, we get the PE header. We can look the up the
         * object in the header and determine if it is code or data, and
         * use that to assign the appropriate selector (either FlatCS
         * or FlatDS).
         */
        handle = lli->file_handle;

        if( !GetEXEHeader( handle, &hi, &stack ) ) {
            return( 0 );
        }
        if( hi.sig != EXE_PE ) {
            return( 0 );
        }

        seek_offset = SetFilePointer( handle, 0, NULL, FILE_CURRENT );
        if( seek_offset == INVALID_SET_FILE_POINTER ) {
            return( 0 );
        }
        if( IS_PE64( hi.u.peh ) ) {
            num_objects = PE64( hi.u.peh ).num_objects;
            seek_offset += PE64( hi.u.peh ).nt_hdr_size + offsetof( pe_header64, magic ) - sizeof( exe_pe_header );
        } else {
            num_objects = PE32( hi.u.peh ).num_objects;
            seek_offset += PE32( hi.u.peh ).nt_hdr_size + offsetof( pe_header, magic ) - sizeof( exe_pe_header );
        }
        if( num_objects == 0 ) {
            return( 0 );
        }
        /* position to begining of object table */
        if( SetFilePointer( handle, seek_offset, NULL, FILE_BEGIN ) == INVALID_SET_FILE_POINTER ) {
            return( 0 );
        }
        for( i = 0; i < num_objects; i++ ) {
            ReadFile( handle, &obj, sizeof( obj ), &bytes, NULL );
            if( i == seg ) {
                break;
            }
        }
        if( i == num_objects ) {
            return( 0 );
        }
        if( obj.flags & ( PE_OBJ_CODE | PE_OBJ_EXECUTABLE ) ) {
            ret->out_addr.segment = FlatCS;
        } else {
            ret->out_addr.segment = FlatDS;
        }
        ret->out_addr.offset = (ULONG_PTR)( lli->base + obj.rva );
    }
    addSegmentToLibList( acc->handle, ret->out_addr.segment, ret->out_addr.offset );
    ret->out_addr.offset += acc->in_addr.offset;
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    return( sizeof( *ret ) );
}

/*
 * AccGetLibName - get lib name of current module
 */
trap_retval ReqGet_lib_name( void )
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    unsigned            i;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    name = GetOutPtr( sizeof( *ret ) );

    ret->handle = 0;

    for( i = 0; i < ModuleTop; ++i ) {
        if( moduleInfo[i].newly_unloaded ) {
            ret->handle = i;
            name[0] = '\0';
            moduleInfo[i].newly_unloaded = FALSE;
            return( sizeof( *ret ) );
        } else if( moduleInfo[i].newly_loaded ) {
            ret->handle = i;
            strcpy( name, moduleInfo[i].filename );
            moduleInfo[i].newly_loaded = FALSE;
            /*
             * once the debugger asks for a lib name, we also add it to our lib
             * list.  This list is used to dump the list of all DLL's, and their
             * selectors
             */
            addModuleToLibList( i );
            return( sizeof( *ret ) + strlen( name ) + 1 );
        }
    }
    return( sizeof( *ret ) );
}

/*
 * GetMagicalFileHandle - check if name is already opened by NT
 */
HANDLE GetMagicalFileHandle( char *name )
{
    DWORD i;

    for( i = 0; i < ModuleTop; i++ ) {
        if( !stricmp( name, moduleInfo[i].filename ) ) {
            if( moduleInfo[i].has_real_filename ) {
                return( NULL );
            } else {
                return( moduleInfo[i].file_handle );
            }
        }
    }
    return( NULL );
}

/*
 * IsMagicalFileHandle - test if a handle is one given by NT
 */
BOOL IsMagicalFileHandle( HANDLE h )
{
    DWORD i;

    for( i = 0; i < ModuleTop; i++ ) {
        if( moduleInfo[i].file_handle == h ) {
            return( TRUE );
        }
    }
    return( FALSE );
}

#if 0
static lib_list_info    *currInfo;
static int              currSeg;

/*
 * formatSel - format a selector for display
 */
static void formatSel( char *buff, int verbose )
{
    thread_info *ti;
    LDT_ENTRY   ldt;
    DWORD       base;
    DWORD       limit;
    DWORD       off;
    WORD        seg;

    seg = currInfo->segs[currSeg].segment;
    off = currInfo->segs[currSeg].offset;

    if( currInfo->is_16 ) {
        wsprintf( buff, "%04x", seg );
    } else {
        wsprintf( buff, "%04x:%08lx", seg, off );
    }
    if( verbose ) {
        ti = FindThread( DebugeeTid );
        GetThreadSelectorEntry( ti->thread_handle, seg, &ldt );
        base = off + ( DWORD ) ldt.BaseLow +
            ( ( DWORD ) ldt.HighWord.Bytes.BaseMid << 16L ) +
            ( ( DWORD ) ldt.HighWord.Bytes.BaseHi << 24L );
        buff = &buff[strlen( buff )];
        limit = 1 + ( DWORD ) ldt.LimitLow +
            ( ( DWORD ) ldt.HighWord.Bits.LimitHi << 16L );
        if( ldt.HighWord.Bits.Granularity ) {
            limit *= 0x1000L;
        }
        if( currInfo->is_16 ) {
            wsprintf( buff, " - base:%08lx size:%04x", base, limit );
        } else {
            wsprintf( buff, " - base:%08lx size:%08lx", base, limit );
        }
    }
}

/*
 * DoListLibs - format up lib list info.  This is called repeatedly by
 *              the debugger to dump all DLL's and their segments
 */
int DoListLibs( char *buff, int is_first, int want_16, int want_32,
                                        int verbose, int sel )
{
    BOOL    done;

    sel = sel;
    verbose = verbose;

    if( is_first ) {
        currInfo = listInfoHead;
        currSeg = -1;
    }
    done = FALSE;
    while( !done ) {
        if( currInfo == NULL ) {
            return( FALSE );
        }
        if( ( currInfo->is_16 && want_16 ) || ( !currInfo->is_16 && want_32 ) ) {
            done = TRUE;
            if( currSeg == -1 ) {
                wsprintf( buff, "%s (%s):", currInfo->modname, currInfo->filename );
            } else {
                formatSel( buff, verbose );
            }
        } else {
            currSeg = currInfo->segcount - 1;
        }
        if( currSeg == currInfo->segcount - 1 ) {
            currInfo = currInfo->next;
            currSeg = -1;
        } else {
            currSeg++;
        }
    }
    return( TRUE );
}
#endif
