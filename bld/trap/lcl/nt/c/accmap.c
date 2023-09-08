/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
#include "globals.h"


typedef struct lib_load_info {
    HANDLE      file_handle;
    LPBYTE      base;
    addr_off    code_size;
    LPVOID      except_base;
    addr_off    except_size;
    bool        is_16;
    bool        has_real_filename;
    bool        newly_unloaded;
    bool        newly_loaded;
    char        filename[MAX_PATH + 1];
    char        modname[40];
} lib_load_info;

typedef struct lib_list_info {
    struct lib_list_info *next;
    int         segcount;
    addr48_ptr  _WCUNALIGNED *segs;
    char        *filename;
    char        *modname;
    bool        is_16;
} lib_list_info;

static lib_load_info    *moduleInfo;
static DWORD            lastLib = 0;
static const char       libPrefix[] = "NoName";

static lib_list_info    *listInfoHead;
static lib_list_info    *listInfoTail;

static void freeListItem( lib_list_info *lli )
/*********************************************
 * free an individual lib list item
 */
{
    LocalFree( lli->filename );
    LocalFree( lli->modname );
    LocalFree( lli->segs );
    LocalFree( lli );
}

void FreeLibList( void )
/***********************
 * free the lib list info
 */
{
    lib_list_info   *lli;

    while( (lli = listInfoHead) != NULL ) {
        listInfoHead = lli->next;
        freeListItem( lli );
    }
    listInfoTail = NULL;
}

static void addModuleToLibList( DWORD module )
/*********************************************
 * saves away the information about the current
 * module.  This is used to dump it out later
 * (TrapListLibs calls DoListLibs)
 */
{
    lib_list_info   *lli;
    lib_load_info   *llo;

    llo = &moduleInfo[module];
    for( lli = listInfoHead; lli != NULL; lli = lli->next ) {
        if( stricmp( llo->modname, lli->modname ) == 0 && stricmp( llo->filename, lli->filename ) == 0 ) {
            return;
        }
    }

    lli = LocalAlloc( LMEM_FIXED, sizeof( lib_list_info ) );
    if( lli == NULL ) {
        return;
    }
    lli->filename = LocalAlloc( LMEM_FIXED, strlen( llo->filename ) + 1 );
    if( lli->filename == NULL ) {
        LocalFree( lli );
        return;
    }
    strcpy( lli->filename, llo->filename );
    lli->modname = LocalAlloc( LMEM_FIXED, strlen( llo->modname ) + 1 );
    if( lli->modname == NULL ) {
        LocalFree( lli->filename );
        LocalFree( lli );
        return;
    }
    strcpy( lli->modname, llo->modname );
    lli->segcount = 0;
    lli->segs = NULL;
    lli->is_16 = llo->is_16;
    lli->next = NULL;
    if( listInfoHead == NULL ) {
        listInfoHead = listInfoTail = lli;
    } else {
        listInfoTail->next = lli;
        listInfoTail = lli;
    }
}

void RemoveModuleFromLibList( const char *module, const char *filename )
/***********************************************************************
 * removes a module from our list once it is
 * unloaded or exits
 */
{
    lib_list_info   *curr;
    lib_list_info   *prev;

    prev = NULL;
    for( curr = listInfoHead; curr != NULL; curr = curr->next ) {
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
    }
}

static void addSegmentToLibList( DWORD module, WORD seg, DWORD off )
/*******************************************************************
 * add a new segment. We keep track of this so that
 * we can dump it later.
 */
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
        memcpy( new, listInfoTail->segs, sizeof( addr48_ptr ) * listInfoTail->segcount );
    }
    listInfoTail->segs = new;
    listInfoTail->segs[listInfoTail->segcount].segment = seg;
    listInfoTail->segs[listInfoTail->segcount].offset = off;
    listInfoTail->segcount++;
}

bool FindExceptInfo( LPVOID off, LPVOID *base, DWORD *size )
{
    unsigned        i;
    lib_load_info   *llo;

    for( i = 0; i < ModuleTop; ++i ) {
        llo = &moduleInfo[i];
        if( (LPBYTE)off >= llo->base && (LPBYTE)off < llo->base + llo->code_size ) {
            /*
             * this is the image
             */
            if( llo->except_size == 0 ) {
                return( false );
            }
            *base = llo->except_base;
            *size = llo->except_size;
            return( true );
        }
    }
    return( false );
}

static void FillInExceptInfo( lib_load_info *llo )
{
    DWORD           ne_header_off;
    SIZE_T          bytes;
    pe_exe_header   pehdr;

    ReadProcessMemory( ProcessInfo.process_handle, llo->base + NE_HEADER_OFFSET, &ne_header_off, sizeof( ne_header_off ), &bytes );
    ReadProcessMemory( ProcessInfo.process_handle, llo->base + ne_header_off, &pehdr, PE_HDR_SIZE, &bytes );
    ReadProcessMemory( ProcessInfo.process_handle, llo->base + ne_header_off + PE_HDR_SIZE, (char *)&pehdr + PE_HDR_SIZE, PE_OPT_SIZE( pehdr ), &bytes );
    llo->code_size = PE( pehdr, code_base ) + PE( pehdr, code_size );
    llo->except_base = llo->base + PE_DIRECTORY( pehdr, PE_TBL_EXCEPTION ).rva;
    llo->except_size = PE_DIRECTORY( pehdr, PE_TBL_EXCEPTION ).size;
}

#ifdef WOW
#if MADARCH & MADARCH_X86
void AddProcess16( header_info *hi )
/***********************************
 * a new 16-bit process has been created
 */
{
    lib_load_info   *llo;

    moduleInfo = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, sizeof( lib_load_info ) );
    ModuleTop = 1;

    llo = moduleInfo;

    llo->is_16 = true;
    llo->file_handle = 0;
    llo->base = NULL;
    llo->has_real_filename = true;
    strcpy( llo->modname, hi->modname );
    strcpy( llo->filename, CurrEXEName );
}
#endif
#endif

void AddProcess( header_info *hi )
/*********************************
 * a new process has been created
 */
{
    lib_load_info   *llo;

    /* unused parameters */ (void)hi;

    moduleInfo = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, sizeof( lib_load_info ) );
    ModuleTop = 1;

    llo = moduleInfo;

    llo->has_real_filename = false;
    llo->is_16 = false;
    llo->file_handle = DebugEvent.u.CreateProcessInfo.hFile;
    /*
     * kludge - NT doesn't give us a handle sometimes
     */
    if( llo->file_handle == INVALID_HANDLE_VALUE ) {
        llo->file_handle = CreateFile( CurrEXEName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, 0 );
    }
    llo->base = DebugEvent.u.CreateProcessInfo.lpBaseOfImage;
    FillInExceptInfo( llo );
    llo->modname[0] = '\0';
    llo->filename[0] = '\0';
}

static bool NameFromProcess( lib_load_info *llo, DWORD dwPID, char *buff, size_t buff_maxlen )
/*********************************************************************************************
 * get fully qualified filename for last DLL
 * that was loaded in process. Intended for Win9x.
 */
{
    HANDLE          hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32   me32;
    bool            bSuccess = false;
    size_t          len;

    /*
     * Check if we have the KERNEL32 entrypoints.
     */
    if( !pCreateToolhelp32Snapshot || !pModule32First || !pModule32Next )
        goto error_exit;
    /*
     * Take a snapshot of all modules in the specified process.
     */
    hModuleSnap = pCreateToolhelp32Snapshot( TH32CS_SNAPMODULE, dwPID );
    if( hModuleSnap == INVALID_HANDLE_VALUE )
        goto error_exit;
    /*
     * Set the size of the structure before using it.
     */
    me32.dwSize = sizeof( MODULEENTRY32 );
    /*
     * Attempt to retrieve information about the first module.
     */
    if( !pModule32First( hModuleSnap, &me32 ) )
        goto error_exit;
    if( buff_maxlen > 0 ) {
        buff_maxlen--;
        /*
         * Look for freshly loaded module. Not tested on Win9x.
         * Unfortunately in WinXP not all newly loaded modules are in the list.
         * This should not be relevant as all NT versions will use the PSAPI method anyway.
         * The PSAPI method works reliably but is not available on Win9x.
         */
        do {
            if( me32.modBaseAddr == llo->base ) {
                len = strlen( me32.szExePath );
                if( buff_maxlen > len )
                    buff_maxlen = len;
                strncpy( buff, me32.szExePath, buff_maxlen );
                buff[buff_maxlen] = '\0';
                bSuccess = true;
                break;
            }
        } while( pModule32Next( hModuleSnap, &me32 ) );
    }

error_exit:
    if( hModuleSnap != INVALID_HANDLE_VALUE )
        CloseHandle( hModuleSnap );

    return( bSuccess );
}

static bool NameFromHandle( HANDLE hFile, char *buff, size_t buff_maxlen )
/*************************************************************************
 * get fully qualified filename from file handle.
 * Intended for Windows NT.
 */
{
#define BUFSIZE 512
    bool        bSuccess;
    char        pszFilename[MAX_PATH + 1];
    HANDLE      hFileMap;
    void        *pMem;
    char        szTemp[BUFSIZE];
    DWORD       dwFileSizeHi;
    DWORD       dwFileSizeLo;

    pMem = NULL;
    hFileMap = NULL;
    bSuccess = false;
    if( buff_maxlen == 0 )
        goto error_exit;
    buff[0] = '\0';
    /*
     * Check if we have the required entrypoints (results depend on OS version).
     */
    if( (hFile == INVALID_HANDLE_VALUE) || !pGetMappedFileName || !pQueryDosDevice )
        goto error_exit;
    /*
     * Get the file size.
     */
    dwFileSizeLo = GetFileSize( hFile, &dwFileSizeHi );
    if( dwFileSizeLo == 0 && dwFileSizeHi == 0 )
        goto error_exit;
    /*
     * Create a file mapping object and map the file.
     */
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
    /*
     * Translate path with device name to drive letters.
     */
    if( buff_maxlen > 3 ) {
        buff_maxlen -= 3;

        szTemp[0] = '\0';
        if( GetLogicalDriveStrings( BUFSIZE - 1, szTemp ) ) {
            char    szName[MAX_PATH];
            char    *p = szTemp;

            buff[1] = ':';
            buff[2] = '\0';
            do {
                /*
                 * Copy the drive letter to the template string
                 */
                buff[0] = *p;
                /*
                 * Look up each device name
                 */
                if( pQueryDosDevice( buff, szName, BUFSIZE ) ) {
                    size_t  len = strlen( szName );
                    if( len < MAX_PATH ) {
                        if( strnicmp( pszFilename, szName, len ) == 0 && pszFilename[len] == '\\' ) {
                            /*
                             * Reconstruct pszFilename using szTemp
                             * Replace device path with DOS path
                             */
                            p = pszFilename + len;
                            len = strlen( p );
                            if( buff_maxlen > len )
                                buff_maxlen = len;
                            strncpy( buff + 2, p, buff_maxlen );
                            buff[buff_maxlen + 2] = '\0';
                            bSuccess = true;
                            break;
                        }
                    }
                }
                /*
                 * Go to the next NULL character.
                 */
                while( *p++ != '\0' ) {
                    {}
                }
            } while( *p != '\0' ); // end of string
        }
    }

error_exit:
    if( pMem != NULL )
        UnmapViewOfFile( pMem );
    if( hFileMap != NULL )
        CloseHandle( hFileMap );
    return( bSuccess );
#undef BUFSIZE
}


#ifdef WOW
void AddLib16( IMAGE_NOTE *im )
/******************************
 * a new library has loaded
 */
{
    lib_load_info   *llo;

    ModuleTop++;
    llo = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, ModuleTop * sizeof( lib_load_info ) );
    memcpy( llo, moduleInfo, ( ModuleTop - 1 ) * sizeof( lib_load_info ) );
    LocalFree( moduleInfo );
    moduleInfo = llo;
    llo = &moduleInfo[ModuleTop - 1];

    llo->is_16 = true;
    llo->has_real_filename = true;
    llo->file_handle = 0;
    llo->base = NULL;
    llo->newly_loaded = true;
    llo->newly_unloaded = false;
    strcpy( llo->filename, im->FileName );
    strcpy( llo->modname, im->Module );
}
#endif

void AddLib( void )
/******************
 * a new library has loaded
 */
{
    lib_load_info   *llo;

    ModuleTop++;
    llo = LocalAlloc( LMEM_FIXED | LMEM_ZEROINIT, ModuleTop * sizeof( lib_load_info ) );
    memcpy( llo, moduleInfo, ( ModuleTop - 1 ) * sizeof( lib_load_info ) );
    LocalFree( moduleInfo );
    moduleInfo = llo;
    llo = &moduleInfo[ModuleTop - 1];

    llo->is_16 = false;
    llo->has_real_filename = false;
    /*
     * for a 32-bit DLL, we make up a fake name to tell the debugger
     * when the debugger asks to open this fake name, we return the
     * saved file handle
     */
    llo->file_handle = DebugEvent.u.LoadDll.hFile;
    llo->base = DebugEvent.u.LoadDll.lpBaseOfDll;
    llo->modname[0] = '\0';
    if ( NameFromHandle( llo->file_handle, llo->filename, sizeof( llo->filename ) ) ) {
        llo->has_real_filename = true;
    } else if( NameFromProcess( llo, DebugeePid, llo->filename, sizeof( llo->filename ) ) ) {
        llo->has_real_filename = true;
    } else if( !GetModuleName( llo->file_handle, llo->filename, sizeof( llo->filename ) ) ) {
        lastLib++;
        strcpy( llo->filename, libPrefix );
        ultoa( lastLib, &llo->filename[sizeof( libPrefix ) - 1], 16 );
        strcat( llo->filename, ".dll" );
    }
    FillInExceptInfo( llo );
    llo->newly_loaded = true;
    llo->newly_unloaded = false;
}

void DelLib( void )
{
    unsigned    i;

    for( i = 0; i < ModuleTop; ++i ) {
        if( moduleInfo[i].base == DebugEvent.u.UnloadDll.lpBaseOfDll ) {
            moduleInfo[i].newly_unloaded = true;
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

void DelProcess( bool closeHandles )
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

#if defined( WOW )

static void force16SegmentLoad( thread_info *ti, WORD sel )
/**********************************************************
 * force a wow app to access its segment so that it
 * will be loaded into memory.
 */
{
    #define INS_BYTES sizeof( getMemIns )

    static unsigned char    getMemIns[] = {
        0x8e, 0xc0,                 /* mov es,ax */
        0x26, 0xa1, 0x00, 0x00,     /* mov ax,es:[0] */
        0xcc                        /* int3 */
    };
    static unsigned char    origBytes[INS_BYTES] = { 0 };
    static bool             gotOrig;
    MYCONTEXT               con;
    MYCONTEXT               oldcon;

    if( !UseVDMStuff ) {
        return;
    }
    if( !gotOrig ) {
        gotOrig = true;
        ReadMemory( &WOWAppInfo.addr, origBytes, INS_BYTES );
    }
    WriteMemory( &WOWAppInfo.addr, getMemIns, INS_BYTES );
    MyGetThreadContext( ti, &con );
    oldcon = con;
    con.Eax = sel;
    con.Eip = WOWAppInfo.addr.offset;
    con.SegCs = WOWAppInfo.addr.segment;
    MySetThreadContext( ti, &con );
    DebugExecute( STATE_IGNORE_DEBUG_OUT | STATE_IGNORE_DEAD_THREAD | STATE_EXPECTING_FAULT, NULL, false );
    MySetThreadContext( ti, &oldcon );
    WriteMemory( &WOWAppInfo.addr, origBytes, INS_BYTES );

    #undef INS_BYTES
}

#endif  /* defined( WOW ) */

trap_retval TRAP_CORE( Map_addr )( void )
{
    int             i;
    HANDLE          handle;
    DWORD           bytes;
    pe_object       obj;
    WORD            seg;
    map_addr_req    *acc;
    map_addr_ret    *ret;
    header_info     hi;
    lib_load_info   *llo;
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

    llo = &moduleInfo[acc->mod_handle];

#ifdef WOW
    if( llo->is_16 ) {
        LDT_ENTRY   ldt;
        WORD        sel;
        thread_info *ti;
        /*
         * much simpler for a WOW app.  We just ask for the selector that
         * maps to the given segment number.
         */
        ti = FindThread( DebugeeTid );
        pVDMGetModuleSelector( ProcessInfo.process_handle,
                        ti->thread_handle, seg, llo->modname, &sel );
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
    } else {
#endif
        /*
         * for a 32-bit app, we get the PE header. We can look the up the
         * object in the header and determine if it is code or data, and
         * use that to assign the appropriate selector (either FlatCS
         * or FlatDS).
         */
        handle = llo->file_handle;

        if( !GetEXEHeader( handle, &hi, &stack ) ) {
            return( 0 );
        }
        if( hi.signature != EXESIGN_PE ) {
            return( 0 );
        }

        num_objects = hi.u.pehdr.fheader.num_objects;
        if( num_objects == 0 ) {
            return( 0 );
        }
        memset( &obj, 0, sizeof( obj ) );
        for( i = 0; i < num_objects; i++ ) {
            ReadFile( handle, &obj, sizeof( obj ), &bytes, NULL );
            if( i == seg ) {
                break;
            }
        }
        if( i == num_objects ) {
            return( 0 );
        }
        if( obj.flags & (PE_OBJ_CODE | PE_OBJ_EXECUTABLE) ) {
            ret->out_addr.segment = FlatCS;
        } else {
            ret->out_addr.segment = FlatDS;
        }
        ret->out_addr.offset = (ULONG_PTR)( llo->base + obj.rva );
#ifdef WOW
    }
#endif
    addSegmentToLibList( acc->mod_handle, ret->out_addr.segment, ret->out_addr.offset );
    ret->out_addr.offset += acc->in_addr.offset;
    ret->lo_bound = 0;
    ret->hi_bound = ~(addr48_off)0;
    return( sizeof( *ret ) );
}

trap_retval TRAP_CORE( Get_lib_name )( void )
/********************************************
 * get lib name of current module
 */
{
    get_lib_name_req    *acc;
    get_lib_name_ret    *ret;
    char                *name;
    unsigned            i;
    size_t              name_maxlen;

    acc = GetInPtr( 0 );
    ret = GetOutPtr( 0 );
    ret->mod_handle = 0;
    for( i = 0; i < ModuleTop; ++i ) {
        if( moduleInfo[i].newly_unloaded ) {
            ret->mod_handle = i;
            *(char *)GetOutPtr( sizeof( *ret ) ) = '\0';
            moduleInfo[i].newly_unloaded = false;
            return( sizeof( *ret ) + 1 );
        } else if( moduleInfo[i].newly_loaded ) {
            ret->mod_handle = i;
            name_maxlen = GetTotalSizeOut() - sizeof( *ret ) - 1;
            name = GetOutPtr( sizeof( *ret ) );
            strncpy( name, moduleInfo[i].filename, name_maxlen );
            name[name_maxlen] = '\0';
            moduleInfo[i].newly_loaded = false;
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

HANDLE GetMagicalFileHandle( const char *name )
/**********************************************
 * check if name is already opened by NT
 */
{
    DWORD i;

    for( i = 0; i < ModuleTop; i++ ) {
        if( stricmp( name, moduleInfo[i].filename ) == 0 ) {
            if( moduleInfo[i].has_real_filename ) {
                return( NULL );
            } else {
                return( moduleInfo[i].file_handle );
            }
        }
    }
    return( NULL );
}

bool IsMagicalFileHandle( HANDLE h )
/***********************************
 * test if a handle is one given by NT
 */
{
    DWORD i;

    for( i = 0; i < ModuleTop; i++ ) {
        if( moduleInfo[i].file_handle == h ) {
            return( true );
        }
    }
    return( false );
}

#if 0
static lib_list_info    *currInfo;
static int              currSeg;

static void formatSel( char *buff, int verbose )
/***********************************************
 * format a selector for display
 */
{
    LDT_ENTRY   ldt;
    DWORD       base;
    DWORD       limit;
    DWORD       off;
    WORD        sel;

    sel = currInfo->segs[currSeg].segment;
    off = currInfo->segs[currSeg].offset;

    if( currInfo->is_16 ) {
        wsprintf( buff, "%04x", sel );
    } else {
        wsprintf( buff, "%04x:%08lx", sel, off );
    }
    if( verbose && GetSelectorLDTEntry( sel, &ldt ) ) {
        base = off + GET_LDT_BASE( ldt );
        limit = GET_LDT_LIMIT( ldt );
        buff = &buff[strlen( buff )];
        if( currInfo->is_16 ) {
            wsprintf( buff, " - base:%08lx size:%04x", base, limit );
        } else {
            wsprintf( buff, " - base:%08lx size:%08lx", base, limit );
        }
    }
}

bool DoListLibs( char *buff, int is_first, int want_16, int want_32,
                                        int verbose, int sel )
/*******************************************************************
 * format up lib list info.  This is called repeatedly by
 * the debugger to dump all DLL's and their segments
 */
{
    bool    done;

    sel = sel;
    verbose = verbose;

    if( is_first ) {
        currInfo = listInfoHead;
        currSeg = -1;
    }
    done = false;
    while( !done ) {
        if( currInfo == NULL ) {
            return( false );
        }
        if( ( currInfo->is_16 && want_16 ) || ( !currInfo->is_16 && want_32 ) ) {
            done = true;
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
    return( true );
}
#endif
