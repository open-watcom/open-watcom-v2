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
#include "dbgmod.h"

/*--------------------------- Global variables ----------------------------*/

#define true    1
#define false   0

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
static int PE_readHeader(
    FILE *f,
    long startOffset,
    FILE_HDR *filehdr,
    OPTIONAL_HDR *opthdr)
{
    EXE_HDR exehdr;
    u_long  offset,signature;

    /* Read the EXE header and check for valid header signature */
    result = PE_invalidDLLImage;
    fseek(f, startOffset, SEEK_SET);
    if (fread(&exehdr, 1, sizeof(exehdr), f) != sizeof(exehdr))
        return false;
    if (exehdr.signature != 0x5A4D)
        return false;

    /* Now seek to the start of the PE header defined at offset 0x3C
     * in the MS-DOS EXE header, and read the signature and check it.
     */
    fseek(f, startOffset+0x3C, SEEK_SET);
    if (fread(&offset, 1, sizeof(offset), f) != sizeof(offset))
        return false;
    fseek(f, startOffset+offset, SEEK_SET);
    if (fread(&signature, 1, sizeof(signature), f) != sizeof(signature))
        return false;
    if (signature != 0x00004550)
        return false;

    /* Now read the PE file header and check that it is correct */
    if (fread(filehdr, 1, sizeof(*filehdr), f) != sizeof(*filehdr))
        return false;
    if (filehdr->Machine != IMAGE_FILE_MACHINE_I386)
        return false;
    if (!(filehdr->Characteristics & IMAGE_FILE_32BIT_MACHINE))
        return false;
    if (!(filehdr->Characteristics & IMAGE_FILE_DLL))
        return false;
    if (fread(opthdr, 1, sizeof(*opthdr), f) != sizeof(*opthdr))
        return false;
    if (opthdr->Magic != 0x10B)
        return false;

    /* Success, so return true! */
    return true;
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
u_long PE_getFileSize(
    FILE *f,
    u_long startOffset)
{
    FILE_HDR        filehdr;
    OPTIONAL_HDR    opthdr;
    SECTION_HDR     secthdr;
    u_long          size;
    int             i;

    /* Read the PE file headers from disk */
    if (!PE_readHeader(f,startOffset,&filehdr,&opthdr))
        return 0xFFFFFFFF;

    /* Scan all the section headers summing up the total size */
    size = opthdr.SizeOfHeaders;
    for (i = 0; i < filehdr.NumberOfSections; i++) {
        if (fread(&secthdr, 1, sizeof(secthdr), f) != sizeof(secthdr))
            return 0xFFFFFFFF;
        if (!(secthdr.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA))
            size += secthdr.SizeOfRawData;
        }
    return size;
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
    FILE *f,
    u_long startOffset,
    u_long *size)
{
    FILE_HDR        filehdr;
    OPTIONAL_HDR    opthdr;
    SECTION_HDR     secthdr;
    u_long          offset,pageOffset;
    u_long          text_raw_off,text_base,text_size,text_end;
    u_long          data_raw_off,data_base,data_size,data_virt_size,data_end;
    u_long          bss_raw_off,bss_base,bss_size,bss_end;
    u_long          import_raw_off,import_base,import_size,import_end;
    u_long          export_raw_off,export_base,export_size,export_end;
    u_long          reloc_raw_off,reloc_base= 0,reloc_size;
    u_long          image_base = 0,image_size,image_end;
    u_char          *image_ptr;
    int             i,delta,numFixups;
    u_short         relocType,*fixup;
    PE_MODULE       *hMod = NULL;
    void            *reloc = NULL;
    BASE_RELOCATION *baseReloc;

    /* Read the PE file headers from disk */
    if (!PE_readHeader(f,startOffset,&filehdr,&opthdr))
        return NULL;

    /* Scan all the section headers and find the necessary sections */
    text_raw_off = text_base = text_size = text_end = 0;
    data_raw_off = data_base = data_size = data_virt_size = data_end = 0;
    bss_raw_off = bss_base = bss_size = bss_end = 0;
    import_raw_off = import_base = import_size = import_end = 0;
    export_raw_off = export_base = export_size = export_end = 0;
    reloc_raw_off = reloc_size = 0;
    for (i = 0; i < filehdr.NumberOfSections; i++) {
        if (fread(&secthdr, 1, sizeof(secthdr), f) != sizeof(secthdr))
            goto Error;
        if (i == 0)
            image_base = secthdr.VirtualAddress;
        if (strcmp(secthdr.Name, ".edata") == 0 || strcmp(secthdr.Name, ".rdata") == 0) {
            /* Exports section */
            export_raw_off = secthdr.PointerToRawData;
            export_base = secthdr.VirtualAddress;
            export_size = secthdr.SizeOfRawData;
            export_end = export_base + export_size;
            }
        else if (strcmp(secthdr.Name, ".idata") == 0) {
            /* Imports section */
            import_raw_off = secthdr.PointerToRawData;
            import_base = secthdr.VirtualAddress;
            import_size = secthdr.SizeOfRawData;
            import_end = import_base + import_size;
            }
        else if (strcmp(secthdr.Name, ".reloc") == 0) {
            /* Relocations section */
            reloc_raw_off = secthdr.PointerToRawData;
            reloc_base = secthdr.VirtualAddress;
            reloc_size = secthdr.SizeOfRawData;
            }
        else if (!text_raw_off && secthdr.Characteristics & IMAGE_SCN_CNT_CODE) {
            /* Code section */
            text_raw_off = secthdr.PointerToRawData;
            text_base = secthdr.VirtualAddress;
            text_size = secthdr.SizeOfRawData;
            text_end = text_base + text_size;
            }
        else if (!data_raw_off && secthdr.Characteristics & IMAGE_SCN_CNT_INITIALIZED_DATA) {
            /* Data section */
            data_raw_off = secthdr.PointerToRawData;
            data_base = secthdr.VirtualAddress;
            data_size = secthdr.SizeOfRawData;
            data_virt_size = secthdr.VirtualSize;
            data_end = data_base + data_size;
            }
        else if (!bss_raw_off && secthdr.Characteristics & IMAGE_SCN_CNT_UNINITIALIZED_DATA) {
            /* BSS data section */
            bss_raw_off = secthdr.PointerToRawData;
            bss_base = secthdr.VirtualAddress;
            bss_size = secthdr.SizeOfRawData;
            bss_end = bss_base + bss_size;
            }
        }

    /* Check to make sure that we have all the sections we need */
    if (!text_raw_off || !data_raw_off || !export_raw_off || !reloc_raw_off) {
        result = PE_invalidDLLImage;
        goto Error;
        }

    /* Make sure the .reloc section is after everything else we load! */
    image_end = max(max(max(max(bss_end,data_end),text_end),import_end),export_end);
    if (reloc_base <= image_end) {
        result = PE_unknownImageFormat;
        goto Error;
        }

    /* Find the size of the image sections to load and allocate memory for
     * them. We only load section data up to the .reloc section, and then
     * ignore everything else after that (eventually we might need to handle
     * the .rsrc section separately).
     */
    image_size = image_end - image_base;
    *size = sizeof(PE_MODULE) + image_size + 4096;
    hMod = malloc(*size);
    reloc = malloc(reloc_size);
    if (!hMod || !reloc) {
        result = PE_outOfMemory;
        goto Error;
        }

    /* Setup all the pointers into our loaded executeable image */
    image_ptr = (u_char*)ROUND_4K((u_long)hMod + sizeof(PE_MODULE));
    hMod->pbase = image_ptr;
    hMod->ptext = image_ptr + (text_base - image_base);
    hMod->pdata = image_ptr + (data_base - image_base);
    if (bss_base)
        hMod->pbss = image_ptr + (bss_base - image_base);
    else
        hMod->pbss = NULL;
    if (import_base)
        hMod->pimport = image_ptr + (import_base - image_base);
    else
        hMod->pimport = NULL;
    hMod->pexport = image_ptr + (export_base - image_base);
    hMod->textBase = text_base;
    hMod->dataBase = data_base;
    hMod->bssBase = bss_base;
    hMod->importBase = import_base;
    hMod->exportBase = export_base;
    hMod->exportDir = opthdr.DataDirectory[0].RelVirtualAddress - export_base;
    hMod->modname = NULL;

    /* Now read the section images from disk */
    result = PE_invalidDLLImage;
    fseek(f, startOffset+text_raw_off, SEEK_SET);
    if (fread(hMod->ptext, 1, text_size, f) != text_size)
        goto Error;
    fseek(f, startOffset+data_raw_off, SEEK_SET);
    if (data_virt_size) {
        /* Some linkers will put uninitalised data at the end
         * of the primary data section, so we first must clear
         * the data section to zeros for the entire length of
         * VirtualSize, which can be longer than the size on disk.
         * Note also that some linkers set this value to zero, so
         * we ignore this value in that case (those linkers also
         * have a seperate BSS section).
         */
        memset(hMod->pdata, 0, data_virt_size);
        }
    if (fread(hMod->pdata, 1, data_size, f) != data_size)
        goto Error;
    if (import_base) {
        fseek(f, startOffset+import_raw_off, SEEK_SET);
        if (fread(hMod->pimport, 1, import_size, f) != import_size)
            goto Error;
        }
    fseek(f, startOffset+export_raw_off, SEEK_SET);
    if (fread(hMod->pexport, 1, export_size, f) != export_size)
        goto Error;
    fseek(f, startOffset+reloc_raw_off, SEEK_SET);
    if (fread(reloc, 1, reloc_size, f) != reloc_size)
        goto Error;

    /* Make sure the BSS section is cleared to zero if it exists */
    if (hMod->pbss)
        memset(hMod->pbss, 0, bss_size);

    /* Now perform relocations on all sections in the image */
    delta = (u_long)image_ptr - opthdr.ImageBase - image_base;
    baseReloc = (BASE_RELOCATION*)reloc;
    for (;;) {
        /* Check for termination condition */
        if (!baseReloc->PageRVA || !baseReloc->BlockSize)
            break;

        /* Do fixups */
        numFixups = (baseReloc->BlockSize - sizeof(BASE_RELOCATION)) / sizeof(u_short);
        fixup = (u_short*)(baseReloc + 1);
        pageOffset = baseReloc->PageRVA - image_base;
        for (i = 0; i < numFixups; i++) {
            relocType = *fixup >> 12;
            if (relocType) {
                offset = pageOffset + (*fixup & 0x0FFF);
                *(u_long*)(image_ptr + offset) += delta;
                }
            fixup++;
            }

        /* Move to next relocation block */
        baseReloc = (BASE_RELOCATION*)((u_long)baseReloc + baseReloc->BlockSize);
        }

    /* On some platforms (such as AMD64 or x86 with NX bit), it is required
     * to map the code pages loaded from the BPD as executable, otherwise
     * a segfault will occur when attempting to run any BPD code.
     */
    if (mprotect((void*)image_ptr, image_size, PROT_READ | PROT_WRITE | PROT_EXEC) != 0)
        goto Error;

    /* Clean up, close the file and return the loaded module handle */
    free(reloc);
    result = PE_ok;
    return hMod;

Error:
    free(hMod);
    free(reloc);
    return NULL;
}

/****************************************************************************
DESCRIPTION:
Loads a Portable Binary DLL into memory

HEADER:
peloader.h

PARAMETERS:
szDLLName   - Name of the PE DLL library to load

RETURNS:
Handle to loaded PE DLL, or NULL on failure.

REMARKS:
This function loads a Portable Binary DLL library from disk, relocates
the code and returns a handle to the loaded library. This function
will only work on DLL's that do not have any imports, since we don't
resolve pimport dependencies in this function.

SEE ALSO:
PE_getProcAddress, PE_freeLibrary
****************************************************************************/
PE_MODULE * PE_loadLibrary(
    const char *szDLLName)
{
    PE_MODULE   *hMod;
    FILE        *f;
    u_long      size;

    /* Attempt to open the file on disk */
    if ((f = fopen(szDLLName,"rb")) == NULL) {
        result = PE_fileNotFound;
        return NULL;
        }
    hMod = PE_loadLibraryExt(f,0,&size);
    fclose(f);

    /* Notify the Watcom Debugger of module load and let it load symbolic info */
#ifdef WATCOM_DEBUG_SYMBOLS
    if (hMod) {
        u_long   size;
        char    *modname;

        /* Store the file name in the hMod structure; this must be the real
         * file name where the debugger will try to load symbolic info from
         */
        size = strlen(szDLLName) + 1;
        modname = malloc(size);
        if (modname) {
            if (szDLLName[1] == ':')
                strcpy(modname, szDLLName+2);
            else
                strcpy(modname, szDLLName);
            hMod->modname = modname;
            NotifyWDLoad(hMod->modname, (u_long)hMod->pbase);
            }
        }
#endif
    return hMod;
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
PE_MODULE * PE_loadLibraryHandle(
    int fd,
    const char *szDLLName)
{
    PE_MODULE   *hMod;
    FILE        *f;
    u_long      size;

    /* Attempt to open the file on disk */
    if ((f = fdopen(fd,"rb")) == NULL) {
        result = PE_fileNotFound;
        return NULL;
        }
    hMod = PE_loadLibraryExt(f,0,&size);
    fclose(f);

    /* Notify the Watcom Debugger of module load and let it load symbolic info */
#ifdef WATCOM_DEBUG_SYMBOLS
    if (hMod) {
        u_long   size;
        char    *modname;

        /* Store the file name in the hMod structure; this must be the real
         * file name where the debugger will try to load symbolic info from
         */
        size = strlen(szDLLName) + 1;
        modname = malloc(size);
        if (modname) {
            if (szDLLName[1] == ':')
                strcpy(modname, szDLLName+2);
            else
                strcpy(modname, szDLLName);
            hMod->modname = modname;
            NotifyWDLoad(hMod->modname, (u_long)hMod->pbase);
            }
        }
#endif
    return hMod;
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
    const char *szProcName)
{
    unsigned            i;
    EXPORT_DIRECTORY    *exports;
    u_long              funcOffset;
    u_long              *AddressTable;
    u_long              *NameTable;
    u_short             *OrdinalTable;
    char                *name;

    /* Find the address of the pexport tables from the pexport section */
    if (!hModule)
        return NULL;
    exports = (EXPORT_DIRECTORY*)(hModule->pexport + hModule->exportDir);
    AddressTable = (u_long*)(hModule->pexport + exports->AddressTableRVA - hModule->exportBase);
    NameTable = (u_long*)(hModule->pexport + exports->NameTableRVA - hModule->exportBase);
    OrdinalTable = (u_short*)(hModule->pexport + exports->OrdinalTableRVA - hModule->exportBase);

    /* Search the pexport name table to find the function name */
    for (i = 0; i < exports->NumberOfNamePointers; i++) {
        name = (char*)(hModule->pexport + NameTable[i] - hModule->exportBase);
        if (strcmp(name,szProcName) == 0)
            break;
        }
    if (i == exports->NumberOfNamePointers)
        return NULL;
    funcOffset = AddressTable[OrdinalTable[i]];
    if (!funcOffset)
        return NULL;
    return (void*)(hModule->ptext + funcOffset - hModule->textBase);
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
void PE_freeLibrary(
    PE_MODULE *hModule)
{
    if (hModule) {
        /* Notify the Watcom Debugger of module load and let it remove symbolic info */
#ifdef WATCOM_DEBUG_SYMBOLS
        if (hModule->modname) {
            NotifyWDUnload(hModule->modname);
            free(hModule->modname);
            }
#endif
        free(hModule);
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
int PE_getError(void)
{
    return result;
}

