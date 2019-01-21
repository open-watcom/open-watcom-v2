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
* Description:  Module to implement a simple Portable Binary DLL loader
*               library. This library can be used to load PE DLL's under
*               any Intel based OS, provided the DLL's do not have any
*               imports in the import table.
*
*               NOTE: This loader module expects the DLL's to be built with
*                     Watcom C++ and may produce unexpected results with
*                     DLL's linked by another compiler.
*
****************************************************************************/

#include "peloader.h"
#include "pe.h"
#include <stdlib.h>
#include <sys/mman.h>
#include "bool.h"
#include "dbgmod.h"
#include "digld.h"

/*--------------------------- Global variables ----------------------------*/


static int          result = PE_ok;

/*------------------------- Implementation --------------------------------*/

/****************************************************************************
PARAMETERS:
f           - Handle to open file to read driver from
startOffset - Offset to the start of the driver within the file

RETURNS:
Handle to loaded PE DLL, or NULL on failure.

REMARKS:
This function loads a Portable Binary DLL library from disk, relocates
the code and returns a handle to the loaded library. This function is the
same as the regular PE_loadLibrary except that it take a handle to an
open file and an offset within that file for the DLL to load.
****************************************************************************/
static bool PE_readHeader(
    FILE *fp,
    long startOffset,
    FILE_HDR *filehdr,
    OPTIONAL_HDR *opthdr)
{
    EXE_HDR exehdr;
    u_long  offset, signature;

    /* Read the EXE header and check for valid header signature */
    result = PE_invalidDLLImage;
    DIGLoader( Seek )( fp, startOffset, SEEK_SET );
    if( DIGLoader( Read )( fp, &exehdr, sizeof( exehdr ) ) )
        return( false );
    if( exehdr.signature != 0x5A4D )
        return( false );

    /* Now seek to the start of the PE header defined at offset 0x3C
     * in the MS-DOS EXE header, and read the signature and check it.
     */
    DIGLoader( Seek )( fp, startOffset + 0x3C, SEEK_SET );
    if( DIGLoader( Read )( fp, &offset, sizeof( offset ) ) )
        return( false );
    DIGLoader( Seek )( fp, startOffset + offset, SEEK_SET );
    if( DIGLoader( Read )( fp, &signature, sizeof( signature ) ) )
        return( false );
    if( signature != 0x00004550 )
        return( false );

    /* Now read the PE file header and check that it is correct */
    if( DIGLoader( Read )( fp, filehdr, sizeof( *filehdr ) ) )
        return( false );
    if( filehdr->Machine != IMAGE_FILE_MACHINE_I386 )
        return( false );
    if( (filehdr->Characteristics & IMAGE_FILE_32BIT_MACHINE) == 0 )
        return( false );
    if( (filehdr->Characteristics & IMAGE_FILE_DLL) == 0 )
        return( false );
    if( DIGLoader( Read )( fp, opthdr, sizeof( *opthdr ) ) )
        return( false );
    if( opthdr->Magic != 0x10B )
        return( false );

    /* Success, so return true! */
    return( true );
}

/****************************************************************************
DESCRIPTION:
Find the actual size of a PE file image

HEADER:
peloader.h

PARAMETERS:
f           - Handle to open file to read driver from
startOffset - Offset to the start of the driver within the file

RETURNS:
Size of the DLL file on disk, or -1 on error

REMARKS:
This function scans the headers for a Portable Binary DLL to determine the
length of the DLL file on disk.
****************************************************************************/
u_long PE_getFileSize( FILE *fp, u_long startOffset )
{
    FILE_HDR        filehdr;
    OPTIONAL_HDR    opthdr;
    SECTION_HDR     secthdr;
    u_long          size;
    int             i;

    /* Read the PE file headers from disk */
    if( !PE_readHeader( fp, startOffset, &filehdr, &opthdr ) )
        return( 0xFFFFFFFF );

    /* Scan all the section headers summing up the total size */
    size = opthdr.SizeOfHeaders;
    for( i = 0; i < filehdr.NumberOfSections; i++ ) {
        if( DIGLoader( Read )( fp, &secthdr, sizeof( secthdr ) ) )
            return( 0xFFFFFFFF );
        if( (secthdr.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) == 0 ) {
            size += secthdr.SizeOfRawData;
        }
    }
    return( size );
}

/****************************************************************************
DESCRIPTION:
Loads a Portable Binary DLL into memory from an open file

HEADER:
peloader.h

PARAMETERS:
f           - Handle to open file to read driver from
startOffset - Offset to the start of the driver within the file
size        - Place to store the size of the driver loaded

RETURNS:
Handle to loaded PE DLL, or NULL on failure.

REMARKS:
This function loads a Portable Binary DLL library from disk, relocates
the code and returns a handle to the loaded library. This function is the
same as the regular PE_loadLibrary except that it take a handle to an
open file and an offset within that file for the DLL to load.

SEE ALSO:
PE_loadLibrary, PE_getProcAddress, PE_freeLibrary
****************************************************************************/
PE_MODULE * PE_loadLibraryExt(
    FILE *fp,
    u_long startOffset,
    u_long *size)
{
    FILE_HDR        filehdr;
    OPTIONAL_HDR    opthdr;
    SECTION_HDR     secthdr;
    u_long          offset, pageOffset;
    u_long          text_raw_off, text_base, text_size, text_end;
    u_long          data_raw_off, data_base, data_size, data_virt_size, data_end;
    u_long          bss_raw_off, bss_base, bss_size, bss_end;
    u_long          import_raw_off, import_base, import_size, import_end;
    u_long          export_raw_off, export_base, export_size, export_end;
    u_long          reloc_raw_off, reloc_base = 0, reloc_size;
    u_long          image_base = 0, image_size, image_end;
    u_char          *image_ptr;
    int             i, delta, numFixups;
    u_short         relocType, *fixup;
    PE_MODULE       *hMod = NULL;
    void            *reloc = NULL;
    BASE_RELOCATION *baseReloc;

    /* Read the PE file headers from disk */
    if( !PE_readHeader( fp, startOffset, &filehdr, &opthdr ) )
        return( NULL );

    /* Scan all the section headers and find the necessary sections */
    text_raw_off = text_base = text_size = text_end = 0;
    data_raw_off = data_base = data_size = data_virt_size = data_end = 0;
    bss_raw_off = bss_base = bss_size = bss_end = 0;
    import_raw_off = import_base = import_size = import_end = 0;
    export_raw_off = export_base = export_size = export_end = 0;
    reloc_raw_off = reloc_size = 0;
    for( i = 0; i < filehdr.NumberOfSections; i++ ) {
        if( DIGLoader( Read )( fp, &secthdr, sizeof( secthdr ) ) )
            goto Error;
        if( i == 0 )
            image_base = secthdr.VirtualAddress;
        if( strcmp( secthdr.Name, ".edata" ) == 0 || strcmp( secthdr.Name, ".rdata" ) == 0 ) {
            /* Exports section */
            export_raw_off = secthdr.PointerToRawData;
            export_base = secthdr.VirtualAddress;
            export_size = secthdr.SizeOfRawData;
            export_end = export_base + export_size;
        } else if( strcmp( secthdr.Name, ".idata" ) == 0 ) {
            /* Imports section */
            import_raw_off = secthdr.PointerToRawData;
            import_base = secthdr.VirtualAddress;
            import_size = secthdr.SizeOfRawData;
            import_end = import_base + import_size;
        } else if( strcmp( secthdr.Name, ".reloc" ) == 0 ) {
            /* Relocations section */
            reloc_raw_off = secthdr.PointerToRawData;
            reloc_base = secthdr.VirtualAddress;
            reloc_size = secthdr.SizeOfRawData;
        } else if( text_raw_off == 0 && (secthdr.Characteristics & IMAGE_SCN_CNT_CODE) ) {
            /* Code section */
            text_raw_off = secthdr.PointerToRawData;
            text_base = secthdr.VirtualAddress;
            text_size = secthdr.SizeOfRawData;
            text_end = text_base + text_size;
        } else if( data_raw_off == 0 && (secthdr.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) ) {
            /* Data section */
            data_raw_off = secthdr.PointerToRawData;
            data_base = secthdr.VirtualAddress;
            data_size = secthdr.SizeOfRawData;
            data_virt_size = secthdr.VirtualSize;
            data_end = data_base + data_size;
        } else if( bss_raw_off == 0 && (secthdr.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) ) {
            /* BSS data section */
            bss_raw_off = secthdr.PointerToRawData;
            bss_base = secthdr.VirtualAddress;
            bss_size = secthdr.SizeOfRawData;
            bss_end = bss_base + bss_size;
        }
    }

    /* Check to make sure that we have all the sections we need */
    if( text_raw_off == 0 || data_raw_off == 0 || export_raw_off == 0 || reloc_raw_off == 0 ) {
        result = PE_invalidDLLImage;
        goto Error;
    }

    /* Make sure the .reloc section is after everything else we load! */
    image_end = bss_end;
    if( image_end < data_end )
        image_end = data_end;
    if( image_end < text_end )
        image_end = text_end;
    if( image_end < import_end )
        image_end = import_end;
    if( image_end < export_end )
        image_end = export_end;
    if( reloc_base <= image_end ) {
        result = PE_unknownImageFormat;
        goto Error;
    }

    /* Find the size of the image sections to load and allocate memory for
     * them. We only load section data up to the .reloc section, and then
     * ignore everything else after that (eventually we might need to handle
     * the .rsrc section separately).
     */
    image_size = image_end - image_base;
    *size = sizeof( PE_MODULE ) + image_size + 4096;
    hMod = malloc( *size );
    reloc = malloc( reloc_size );
    if( hMod == NULL || reloc == NULL ) {
        result = PE_outOfMemory;
        goto Error;
    }

    /* Setup all the pointers into our loaded executeable image */
    image_ptr = (u_char *)ROUND_4K( (u_long)hMod + sizeof( PE_MODULE ) );
    hMod->pbase = image_ptr;
    hMod->ptext = image_ptr + ( text_base - image_base );
    hMod->pdata = image_ptr + ( data_base - image_base );
    if( bss_base ) {
        hMod->pbss = image_ptr + ( bss_base - image_base );
    } else {
        hMod->pbss = NULL;
    }
    if( import_base ) {
        hMod->pimport = image_ptr + ( import_base - image_base );
    } else {
        hMod->pimport = NULL;
    }
    hMod->pexport = image_ptr + ( export_base - image_base );
    hMod->textBase = text_base;
    hMod->dataBase = data_base;
    hMod->bssBase = bss_base;
    hMod->importBase = import_base;
    hMod->exportBase = export_base;
    hMod->exportDir = opthdr.DataDirectory[0].RelVirtualAddress - export_base;
    hMod->modname = NULL;

    /* Now read the section images from disk */
    result = PE_invalidDLLImage;
    DIGLoader( Seek )( fp, startOffset + text_raw_off, SEEK_SET );
    if( DIGLoader( Read )( fp, hMod->ptext, text_size ) )
        goto Error;
    DIGLoader( Seek )( fp, startOffset + data_raw_off, SEEK_SET );
    if( data_virt_size ) {
        /* Some linkers will put uninitalised data at the end
         * of the primary data section, so we first must clear
         * the data section to zeros for the entire length of
         * VirtualSize, which can be longer than the size on disk.
         * Note also that some linkers set this value to zero, so
         * we ignore this value in that case (those linkers also
         * have a seperate BSS section).
         */
        memset( hMod->pdata, 0, data_virt_size );
    }
    if( DIGLoader( Read )( fp, hMod->pdata, data_size ) )
        goto Error;
    if( import_base ) {
        DIGLoader( Seek )( fp, startOffset + import_raw_off, SEEK_SET );
        if( DIGLoader( Read )( fp, hMod->pimport, import_size ) ) {
            goto Error;
        }
    }
    DIGLoader( Seek )( fp, startOffset + export_raw_off, SEEK_SET );
    if( DIGLoader( Read )( fp, hMod->pexport, export_size ) )
        goto Error;
    DIGLoader( Seek )( fp, startOffset + reloc_raw_off, SEEK_SET );
    if( DIGLoader( Read )( fp, reloc, reloc_size ) )
        goto Error;

    /* Make sure the BSS section is cleared to zero if it exists */
    if( hMod->pbss )
        memset( hMod->pbss, 0, bss_size );

    /* Now perform relocations on all sections in the image */
    delta = (u_long)image_ptr - opthdr.ImageBase - image_base;
    baseReloc = (BASE_RELOCATION*)reloc;
    for( ;; ) {
        /* Check for termination condition */
        if( baseReloc->PageRVA == 0 || baseReloc->BlockSize == 0 )
            break;

        /* Do fixups */
        numFixups = ( baseReloc->BlockSize - sizeof( BASE_RELOCATION ) ) / sizeof( u_short );
        fixup = (u_short*)( baseReloc + 1 );
        pageOffset = baseReloc->PageRVA - image_base;
        for( i = 0; i < numFixups; i++ ) {
            relocType = *fixup >> 12;
            if( relocType ) {
                offset = pageOffset + ( *fixup & 0x0FFF );
                *(u_long*)( image_ptr + offset ) += delta;
            }
            fixup++;
        }

        /* Move to next relocation block */
        baseReloc = (BASE_RELOCATION*)( (u_long)baseReloc + baseReloc->BlockSize );
    }

    /* On some platforms (such as AMD64 or x86 with NX bit), it is required
     * to map the code pages loaded from the BPD as executable, otherwise
     * a segfault will occur when attempting to run any BPD code.
     */
    if( mprotect( (void*)image_ptr, image_size, PROT_READ | PROT_WRITE | PROT_EXEC ) != 0 )
        goto Error;

    /* Clean up, close the file and return the loaded module handle */
    free( reloc );
    result = PE_ok;
    return( hMod );

Error:
    free( hMod );
    free( reloc );
    return( NULL );
}

/****************************************************************************
DESCRIPTION:
Loads a Portable Binary DLL into memory

HEADER:
peloader.h

PARAMETERS:
fd          - File descriptor to load PE module from
szDLLName   - Name of the PE DLL library to load (for debug symbols)

RETURNS:
Handle to loaded PE DLL, or NULL on failure.

REMARKS:
Same as above but for an open file handle. szDLLName must be correct in
order to load debug symbols under the debugger.
****************************************************************************/
PE_MODULE * PE_loadLibraryFile( FILE *fp, const char *szDLLName )
{
    PE_MODULE   *hMod;
    u_long      size;

#ifndef WATCOM_DEBUG_SYMBOLS
    /* unused parameters */ (void)szDLLName;
#endif

    if( fp == NULL ) {
        result = PE_fileNotFound;
        return( NULL );
    }
    hMod = PE_loadLibraryExt( fp, 0, &size );

    /* Notify the Watcom Debugger of module load and let it load symbolic info */
#ifdef WATCOM_DEBUG_SYMBOLS
    if( hMod != NULL ) {
        u_long   size;
        char    *modname;

        /* Store the file name in the hMod structure; this must be the real
         * file name where the debugger will try to load symbolic info from
         */
        size = strlen( szDLLName ) + 1;
        modname = malloc( size );
        if( modname != NULL ) {
            if( szDLLName[1] == ':' ) {
                strcpy( modname, szDLLName + 2 );
            } else {
                strcpy( modname, szDLLName );
            }
            hMod->modname = modname;
            DebuggerLoadUserModule( hMod->modname, GetCS(), (u_long)hMod->pbase );
        }
    }
#endif
    return( hMod );
}

/****************************************************************************
DESCRIPTION:
Gets a function address from a Portable Binary DLL

HEADER:
peloader.h

PARAMETERS:
hModule     - Handle to a loaded PE DLL library
szProcName  - Name of the function to get the address of

RETURNS:
Pointer to the function, or NULL on failure.

REMARKS:
This function searches for the named, exported function in a loaded PE
DLL library, and returns the address of the function. If the function is
not found in the library, this function return NULL.

SEE ALSO:
PE_loadLibrary, PE_freeLibrary
****************************************************************************/
void * PE_getProcAddress(
    PE_MODULE *hModule,
    const char *szProcName )
{
    unsigned            i;
    EXPORT_DIRECTORY    *exports;
    u_long              funcOffset;
    u_long              *AddressTable;
    u_long              *NameTable;
    u_short             *OrdinalTable;
    char                *name;

    /* Find the address of the pexport tables from the pexport section */
    if( hModule == NULL )
        return( NULL );
    exports = (EXPORT_DIRECTORY *)( hModule->pexport + hModule->exportDir );
    AddressTable = (u_long *)( hModule->pexport + exports->AddressTableRVA - hModule->exportBase );
    NameTable = (u_long*)( hModule->pexport + exports->NameTableRVA - hModule->exportBase );
    OrdinalTable = (u_short*)( hModule->pexport + exports->OrdinalTableRVA - hModule->exportBase );

    /* Search the pexport name table to find the function name */
    for( i = 0; i < exports->NumberOfNamePointers; i++ ) {
        name = (char*)( hModule->pexport + NameTable[i] - hModule->exportBase );
        if( strcmp( name, szProcName ) == 0 ) {
            break;
        }
    }
    if( i == exports->NumberOfNamePointers )
        return( NULL );
    funcOffset = AddressTable[OrdinalTable[i]];
    if( funcOffset == 0 )
        return( NULL );
    return( (void*)( hModule->ptext + funcOffset - hModule->textBase ) );
}

/****************************************************************************
DESCRIPTION:
Frees a loaded Portable Binary DLL

HEADER:
peloader.h

PARAMETERS:
hModule     - Handle to a loaded PE DLL library to free

REMARKS:
This function frees a loaded PE DLL library from memory.

SEE ALSO:
PE_getProcAddress, PE_loadLibrary
****************************************************************************/
void PE_freeLibrary( PE_MODULE *hModule )
{
    if( hModule != NULL ) {
        /* Notify the Watcom Debugger of module load and let it remove symbolic info */
#ifdef WATCOM_DEBUG_SYMBOLS
        if( hModule->modname ) {
            DebuggerUnloadUserModule( hModule->modname );
            free( hModule->modname );
        }
#endif
        free( hModule );
    }
}

/****************************************************************************
DESCRIPTION:
Returns the error code for the last operation

HEADER:
peloader.h

RETURNS:
Error code for the last operation.

SEE ALSO:
PE_getProcAddress, PE_loadLibrary
****************************************************************************/
int PE_getError( void )
{
    return( result );
}
