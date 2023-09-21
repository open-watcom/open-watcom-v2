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
* Description:  DIP/MAD/TRP module loader for PE DLL file format.
*
****************************************************************************/


#include "enterdb.h"

/*
 * Module to implement a simple Portable Binary DLL library loader.
 *
 *       This code can be used to load PE DLL's under any Intel based OS,
 *       provided the DLL's do not have any imports in the import table.
 *
 *       NOTE: This loader module expects the DLL's to be built with
 *             Watcom C++ and may produce unexpected results with
 *             DLL's linked by another compiler.
 *
 *             Now used by Linux 32-bit code only for TRAP modules
 */

#if defined(__WATCOMC__) && defined(__386__) && !defined(NDEBUG)
#define WATCOM_DEBUG_SYMBOLS
#endif

typedef struct {
    char    *pbase;         /* Base of image in memory                  */
    char    *ptext;         /* Text section read from disk              */
    char    *pdata;         /* Data section read from disk              */
    char    *pbss;          /* BSS section read                         */
    char    *pimport;       /* Import section read from disk            */
    char    *pexport;       /* Export section read from disk            */
    u_long  textBase;       /* Base of text section in image            */
    u_long  dataBase;       /* Base of data section in image            */
    u_long  bssBase;        /* Base of BSS data section in image        */
    u_long  importBase;     /* Offset of import section in image        */
    u_long  exportBase;     /* Offset of export section in image        */
    u_long  exportDir;      /* Offset of export directory               */
    char    *modname;       /* Filename of the image                    */
} *module;

#ifdef WATCOM_DEBUG_SYMBOLS
unsigned short GetCS(void);
#pragma aux GetCS \
     __parm __caller [] = \
        "mov ax,cs" \
     __value [__ax]
#endif

static digld_error pe_read_header( FILE *fp, pe_exe_header *pe_hdr )
{
    dos_exe_header exehdr;
    u_long  ne_header_off;

    /*
     * Read the EXE header and check for valid header signature
     */
    if( DIGLoader( Seek )( fp, 0, SEEK_SET )
      || DIGLoader( Read )( fp, &exehdr, sizeof( exehdr ) ) )
        return( DIGS_ERR_CANT_LOAD_MODULE );
    if( ( exehdr.signature != EXESIGN_DOS )
      || !NE_HEADER_FOLLOWS( exehdr.reloc_offset ) )
        return( DIGS_ERR_BAD_MODULE_FILE );
    /*
     * Now seek to the start of the PE header defined at offset 0x3C
     * in the MS-DOS EXE header, and read the signature and check it.
     */
    if( DIGLoader( Seek )( fp, NE_HEADER_OFFSET, SEEK_SET )
      || DIGLoader( Read )( fp, &ne_header_off, sizeof( ne_header_off ) ) )
        return( DIGS_ERR_CANT_LOAD_MODULE );
    if( DIGLoader( Seek )( fp, ne_header_off, SEEK_SET )
      || DIGLoader( Read )( fp, &pe_hdr->signature, sizeof( pe_hdr->signature ) ) )
        return( DIGS_ERR_CANT_LOAD_MODULE );
    if( pe_hdr->signature != EXESIGN_PE )
        return( DIGS_ERR_BAD_MODULE_FILE );
    /*
     * Now read the PE file header and check that it is correct
     */
    if( DIGLoader( Read )( fp, &pe_hdr->fheader, sizeof( pe_hdr->fheader ) ) )
        return( DIGS_ERR_CANT_LOAD_MODULE );
    if( pe_hdr->fheader.cpu_type != PE_CPU_386 )
        return( DIGS_ERR_BAD_MODULE_FILE );
    if( (pe_hdr->fheader.flags & PE_FLG_32BIT_MACHINE) == 0 )
        return( DIGS_ERR_BAD_MODULE_FILE );
    if( (pe_hdr->fheader.flags & PE_FLG_LIBRARY) == 0 )
        return( DIGS_ERR_BAD_MODULE_FILE );
    if( DIGLoader( Read )( fp, &pe_hdr->opt.pe32, sizeof( pe_hdr->opt.pe32 ) ) )
        return( DIGS_ERR_CANT_LOAD_MODULE );
    if( pe_hdr->opt.pe32.magic != 0x10B )
        return( DIGS_ERR_BAD_MODULE_FILE );
    /*
     * Success, so next processing can continue!
     */
    return( DIGS_OK );
}

static void *pe_get_proc_address( module modhdl, const char *proc_name )
{
    unsigned            i;
    pe_export_directory *exports;
    u_long              funcOffset;
    u_long              *AddressTable;
    u_long              *NameTable;
    u_short             *OrdinalTable;
    char                *name;

    /*
     * Find the address of the pexport tables from the pexport section
     */
    exports = (pe_export_directory *)( modhdl->pexport + modhdl->exportDir );
    AddressTable = (u_long *)( modhdl->pexport + exports->address_table_rva - modhdl->exportBase );
    NameTable = (u_long *)( modhdl->pexport + exports->name_ptr_table_rva - modhdl->exportBase );
    OrdinalTable = (u_short *)( modhdl->pexport + exports->ordinal_table_rva - modhdl->exportBase );
    /*
     * Search the pexport name table to find the function name
     */
    for( i = 0; i < exports->num_name_ptrs; i++ ) {
        name = (char *)( modhdl->pexport + NameTable[i] - modhdl->exportBase );
        if( strcmp( name, proc_name ) == 0 ) {
            break;
        }
    }
    if( i == exports->num_name_ptrs )
        return( NULL );
    funcOffset = AddressTable[OrdinalTable[i]];
    if( funcOffset == 0 )
        return( NULL );
    return( (void *)( modhdl->ptext + funcOffset - modhdl->textBase ) );
}

static void loader_unload_image( module modhdl )
{
#ifdef WATCOM_DEBUG_SYMBOLS
    if( modhdl->modname != NULL ) {
        /*
         * Notify the Watcom Debugger of module unload and let it remove symbolic info
         */
        DebuggerUnloadUserModule( modhdl->modname );
        free( modhdl->modname );
    }
#endif
    free( modhdl );
}

static digld_error loader_load_image( FILE *fp, char *filename, module *mod_hdl, void **init_func )
{
    pe_exe_header   pe_hdr;
    pe_object       secthdr;
    u_long          offset, pageOffset;
    u_long          text_raw_off, text_base, text_size, text_end;
    u_long          data_raw_off, data_base, data_size, data_virt_size, data_end;
    u_long          bss_raw_off, bss_base, bss_size, bss_end;
    u_long          import_raw_off, import_base, import_size, import_end;
    u_long          export_raw_off, export_base, export_size, export_end;
    u_long          reloc_raw_off, reloc_base = 0, reloc_size;
    u_long          image_base = 0, image_size, image_end;
    char            *image_ptr;
    size_t          i;
#ifdef WATCOM_DEBUG_SYMBOLS
    size_t          len;
#endif
    size_t          numFixups;
    int             delta;
    u_short         relocType, *fixup;
    module          modhdl;
    void            *reloc = NULL;
    pe_fixup_header *baseReloc;
    digld_error     err;

#ifndef WATCOM_DEBUG_SYMBOLS
    (void)filename;
#endif

    *mod_hdl = NULL;
    /*
     * Read the PE file headers from disk
     */
    err = pe_read_header( fp, &pe_hdr );
    if( err != DIGS_OK )
        return( err );
    /*
     * Scan all the section headers and find the necessary sections
     */
    text_raw_off = text_base = text_size = text_end = 0;
    data_raw_off = data_base = data_size = data_virt_size = data_end = 0;
    bss_raw_off = bss_base = bss_size = bss_end = 0;
    import_raw_off = import_base = import_size = import_end = 0;
    export_raw_off = export_base = export_size = export_end = 0;
    reloc_raw_off = reloc_size = 0;
    err = DIGS_ERR_CANT_LOAD_MODULE;
    for( i = 0; i < pe_hdr.fheader.num_objects; i++ ) {
        if( DIGLoader( Read )( fp, &secthdr, sizeof( secthdr ) ) )
            goto Error;
        if( i == 0 )
            image_base = secthdr.rva;
        if( strcmp( secthdr.name, ".edata" ) == 0
          || strcmp( secthdr.name, ".rdata" ) == 0 ) {
            /*
             * Exports section
             */
            export_raw_off = secthdr.physical_offset;
            export_base = secthdr.rva;
            export_size = secthdr.physical_size;
            export_end = export_base + export_size;
        } else if( strcmp( secthdr.name, ".idata" ) == 0 ) {
            /*
             * Imports section
             */
            import_raw_off = secthdr.physical_offset;
            import_base = secthdr.rva;
            import_size = secthdr.physical_size;
            import_end = import_base + import_size;
        } else if( strcmp( secthdr.name, ".reloc" ) == 0 ) {
            /*
             * Relocations section
             */
            reloc_raw_off = secthdr.physical_offset;
            reloc_base = secthdr.rva;
            reloc_size = secthdr.physical_size;
        } else if( text_raw_off == 0
          && (secthdr.flags & PE_OBJ_CODE) ) {
            /*
             * Code section
             */
            text_raw_off = secthdr.physical_offset;
            text_base = secthdr.rva;
            text_size = secthdr.physical_size;
            text_end = text_base + text_size;
        } else if( data_raw_off == 0
          && (secthdr.flags & PE_OBJ_INIT_DATA) ) {
            /*
             * Data section
             */
            data_raw_off = secthdr.physical_offset;
            data_base = secthdr.rva;
            data_size = secthdr.physical_size;
            data_virt_size = secthdr.virtual_size;
            data_end = data_base + data_size;
        } else if( bss_raw_off == 0
          && (secthdr.flags & PE_OBJ_UNINIT_DATA) ) {
            /*
             * BSS data section
             */
            bss_raw_off = secthdr.physical_offset;
            bss_base = secthdr.rva;
            bss_size = secthdr.physical_size;
            bss_end = bss_base + bss_size;
        }
    }
    /*
     * Check to make sure that we have all the sections we need
     */
    if( text_raw_off == 0
      || data_raw_off == 0
      || export_raw_off == 0
      || reloc_raw_off == 0 ) {
        err = DIGS_ERR_BAD_MODULE_FILE;
        goto Error;
    }
    /*
     * Make sure the .reloc section is after everything else we load!
     */
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
        err = DIGS_ERR_BAD_MODULE_FILE;
        goto Error;
    }
    /*
     * Find the size of the image sections to load and allocate memory for
     * them. We only load section data up to the .reloc section, and then
     * ignore everything else after that (eventually we might need to handle
     * the .rsrc section separately).
     */
    image_size = image_end - image_base;
#if defined( __LINUX__ )
    /*
     * the content in memory must be aligned to the _4K boundary,
     * so one _4K page is added
     */
    modhdl = malloc( sizeof( *modhdl ) + image_size + _4K );
#else
    modhdl = malloc( sizeof( *modhdl ) + image_size );
#endif
    reloc = malloc( reloc_size );
    if( modhdl == NULL
      || reloc == NULL ) {
        err = DIGS_ERR_OUT_OF_MEMORY;
        goto Error;
    }
#if defined( __LINUX__ )
    /*
     * align memory pointer to the _4K boundary
     */
    image_ptr = (char *)__ROUND_UP_SIZE_4K( (u_long)modhdl + sizeof( *modhdl ) );
#else
    image_ptr = (char *)( (u_long)modhdl + sizeof( *modhdl ) );
#endif
    /*
     * Setup all the pointers into our loaded executeable image
     */
    modhdl->pbase = image_ptr;
    modhdl->ptext = image_ptr + ( text_base - image_base );
    modhdl->pdata = image_ptr + ( data_base - image_base );
    if( bss_base ) {
        modhdl->pbss = image_ptr + ( bss_base - image_base );
    } else {
        modhdl->pbss = NULL;
    }
    if( import_base ) {
        modhdl->pimport = image_ptr + ( import_base - image_base );
    } else {
        modhdl->pimport = NULL;
    }
    modhdl->pexport = image_ptr + ( export_base - image_base );
    modhdl->textBase = text_base;
    modhdl->dataBase = data_base;
    modhdl->bssBase = bss_base;
    modhdl->importBase = import_base;
    modhdl->exportBase = export_base;
    modhdl->exportDir = pe_hdr.opt.pe32.table[PE_TBL_EXPORT].rva - export_base;
    modhdl->modname = NULL;
    /*
     * Now read the section images from disk
     */
    DIGLoader( Seek )( fp, text_raw_off, SEEK_SET );
    if( DIGLoader( Read )( fp, modhdl->ptext, text_size ) )
        goto Error;
    DIGLoader( Seek )( fp, data_raw_off, SEEK_SET );
    if( data_virt_size ) {
        /*
         * Some linkers will put uninitalised data at the end
         * of the primary data section, so we first must clear
         * the data section to zeros for the entire length of
         * virtual_size, which can be longer than the size on disk.
         * Note also that some linkers set this value to zero, so
         * we ignore this value in that case (those linkers also
         * have a seperate BSS section).
         */
        memset( modhdl->pdata, 0, data_virt_size );
    }
    if( DIGLoader( Read )( fp, modhdl->pdata, data_size ) )
        goto Error;
    if( import_base ) {
        DIGLoader( Seek )( fp, import_raw_off, SEEK_SET );
        if( DIGLoader( Read )( fp, modhdl->pimport, import_size ) ) {
            goto Error;
        }
    }
    DIGLoader( Seek )( fp, export_raw_off, SEEK_SET );
    if( DIGLoader( Read )( fp, modhdl->pexport, export_size ) )
        goto Error;
    DIGLoader( Seek )( fp, reloc_raw_off, SEEK_SET );
    if( DIGLoader( Read )( fp, reloc, reloc_size ) )
        goto Error;
    /*
     * Make sure the BSS section is cleared to zero if it exists
     */
    if( modhdl->pbss )
        memset( modhdl->pbss, 0, bss_size );
    /*
     * Now perform relocations on all sections in the image
     */
    delta = (u_long)image_ptr - pe_hdr.opt.pe32.image_base - image_base;
    baseReloc = (pe_fixup_header *)reloc;
    for( ;; ) {
        /*
         * Check for termination condition
         */
        if( baseReloc->page_rva == 0
          || baseReloc->block_size == 0 )
            break;
        /*
         * Do fixups
         */
        numFixups = ( baseReloc->block_size - sizeof( pe_fixup_header ) ) / sizeof( u_short );
        fixup = (u_short *)( baseReloc + 1 );
        pageOffset = baseReloc->page_rva - image_base;
        for( i = 0; i < numFixups; i++ ) {
            relocType = *fixup >> 12;
            if( relocType ) {
                offset = pageOffset + ( *fixup & 0x0FFF );
                *(u_long *)( image_ptr + offset ) += delta;
            }
            fixup++;
        }
        /*
         * Move to next relocation block
         */
        baseReloc = (pe_fixup_header *)( (u_long)baseReloc + baseReloc->block_size );
    }
#ifdef __LINUX__
    /*
     * On some platforms (such as AMD64 or x86 with NX bit), it is required
     * to map the code pages loaded from the BPD as executable, otherwise
     * a segfault will occur when attempting to run any BPD code.
     */
    if( mprotect( (void *)image_ptr, image_size, PROT_READ | PROT_WRITE | PROT_EXEC ) != 0 )
        goto Error;
#endif
    /*
     * Clean up, close the file and return the loaded module handle
     */
    free( reloc );
    *init_func = pe_get_proc_address( modhdl, MODINIT );
    *mod_hdl = modhdl;
#ifdef WATCOM_DEBUG_SYMBOLS
    /*
     * Store the file name in the modhdl structure; this must be the real
     * file name where the debugger will try to load symbolic info from
     *
     * remove driver from begining of file name
     */
    if( filename[0] != '\0' && filename[1] == ':' ) {
        filename += 2;
    }
    len = strlen( filename );
    if( len > 0 ) {
        modhdl->modname = malloc( len + 4 + 1 );
        if( modhdl->modname != NULL ) {
            size_t  pos;

            pos = len;
            for( i = 0; i < len; i++ ) {
                switch( *filename ) {
                case '\\':
                case '/':
                    pos = len;
                    break;
                case '.':
                    pos = i;
                    break;
                }
                modhdl->modname[i] = *filename++;
            }
            strcpy( modhdl->modname + pos, ".sym" );
            /*
             * Notify the Watcom Debugger of module load and let it load symbolic info
             */
            DebuggerLoadUserModule( modhdl->modname, GetCS(), (unsigned long)image_ptr );
        }
    }
#endif
    return( DIGS_OK );

Error:
    loader_unload_image( modhdl );
    free( reloc );
    return( err );
}

