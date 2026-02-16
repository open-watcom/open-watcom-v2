/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Import library creation.
*
****************************************************************************/


#include "wlib.h"
#include "ar.h"
#include "coff.h"
#include "coffimpc.h"
#include "roundmac.h"
#include "exedos.h"
#include "i64.h"

#include "clibext.h"


static orl_sec_handle   found_sec_handle;
static orl_rva          export_table_rva;

static unsigned getLenName( libfile io, char *name )
{
    unsigned_8  namelen;

    *name = '\0';
    if( LibRead( io, &namelen, sizeof( namelen ) ) != sizeof( namelen ) ) {
        return( 0 );
    }
    if( namelen == 0 ) {
        return( 0 );
    }
    if( LibRead( io, name, namelen ) != namelen ) {
        FatalError( ERR_BAD_DLL, io->name );
    }
    name[namelen] = '\0';
    return( namelen );
}

static unsigned getU16( libfile io )
{
    unsigned_16 u16;

    if( LibRead( io, &u16, sizeof( u16 ) ) != sizeof( u16 ) ) {
        FatalError( ERR_BAD_DLL, io->name );
    }
    return( u16 );
}

static unsigned_32 getU32( libfile io )
{
    unsigned_32 u32;

    if( LibRead( io, &u32, sizeof( u32 ) ) != sizeof( u32 ) ) {
        FatalError( ERR_BAD_DLL, io->name );
    }
    return( u32 );
}

static orl_return FindHelper( orl_sec_handle sec )
{
    if( found_sec_handle == NULL ) {
        found_sec_handle = sec;
    }
    return( ORL_OKAY );
}

static orl_return FindExportTableHelper( orl_sec_handle sec )
{
    if( found_sec_handle == NULL ) {
        orl_sec_base    base;

        ORLSecGetBase( sec, &base );
        if( ( U64Low( base ) <= export_table_rva )
          && ( U64Low( base ) + ORLSecGetSize( sec ) > export_table_rva ) ) {
            found_sec_handle = sec;
        }
    }
    return( ORL_OKAY );
}

static orl_sec_handle FindSec( obj_file *ofile, const char *name )
{
    export_table_rva = 0;
    found_sec_handle = NULL;

    ORLFileScan( ofile->orl, name, FindHelper );
    if( found_sec_handle == NULL ) {
        if( stricmp( ".edata", name ) == 0 ) {
            export_table_rva = ORLExportTableRVA( ofile->orl );

            if( export_table_rva == 0 ) {
                FatalError( ERR_NO_EXPORTS, ofile->io->name );
            }

            ORLFileScan( ofile->orl, NULL, FindExportTableHelper );
        }

        if( found_sec_handle == NULL ) {
            FatalError( ERR_NO_EXPORTS, ofile->io->name );
        }
    }

    return( found_sec_handle );
}

static bool elfAddImport( libfile io, long header_offset, const arch_header *arch )
{
    obj_file        *ofile;
    orl_sec_handle  sym_sec;
    orl_sec_handle  export_sec;
    orl_sec_handle  string_sec;
    Elf32_Export    *export_table;
    Elf32_Sym       *sym_table;
    orl_sec_size    export_size;
//    orl_sec_size    sym_size;
    char            *strings;
    processor_type  processor = WL_PROC_NONE;
    name_len        dllName;
    Elf32_Word      ElfMagic;
    char            *dll_name;
    arch_header     tmp_arch;

    LibSeek( io, header_offset, SEEK_SET );
    if( LibRead( io, &ElfMagic, sizeof( ElfMagic ) ) != sizeof( ElfMagic ) ) {
        return( false );
    }
    if( memcmp( &ElfMagic, ELF_SIGNATURE, sizeof( SEEK_CUR ) ) != 0 ) {
        return( false );
    }
    LibSeek( io, 0x10 - sizeof( ElfMagic) , SEEK_CUR );
    if( LibRead( io, &ElfMagic, sizeof( ElfMagic ) ) != sizeof( ElfMagic ) ) {
        return( false );
    }
    if( (ElfMagic & 0xff) != ET_DYN ) {
        return( false );
    }
    LibSeek( io, 0, SEEK_SET );
    ofile = OpenORLObjFile( io->name );
    if( ofile->orl == NULL ) {
        FatalError( ERR_CANT_READ, io->name, "Unknown error" );
    }
    switch( ORLFileGetMachineType( ofile->orl ) ) {
    case ORL_MACHINE_TYPE_PPC601:
        processor = WL_PROC_PPC;
        break;
    default:
        FatalError( ERR_CANT_READ, io->name, "Not a PPC DLL" );
    }

    export_sec = FindSec( ofile, ".exports" );
    ORLSecGetContents( export_sec, (unsigned_8 **)&export_table );
    export_size = ORLSecGetSize( export_sec ) / sizeof( Elf32_Export );
    sym_sec = ORLSecGetSymbolTable( export_sec );
    ORLSecGetContents( sym_sec, (unsigned_8 **)&sym_table );
//    sym_size = ORLSecGetSize( sym_sec ) / sizeof( Elf32_Sym );
    string_sec = ORLSecGetStringTable( sym_sec );
    ORLSecGetContents( string_sec, (unsigned_8 **)&strings );

    tmp_arch.date = arch->date;
    tmp_arch.uid = arch->uid;
    tmp_arch.gid = arch->gid;
    tmp_arch.mode = arch->mode;
    tmp_arch.size = arch->size;
    tmp_arch.libtype = arch->libtype;

    dllName.name = dll_name = MemStrdup( TrimPath( arch->name ) );
    dllName.len = strlen( dllName.name );
    tmp_arch.name = dll_name;
    tmp_arch.ffname = NULL;

    ElfMKImport( &tmp_arch, ELF, export_size, &dllName, strings, export_table, sym_table, processor );

    MemFree( dll_name );

    CloseORLObjFile( ofile );
    MemFree( ofile );
    return( true );
}

static void importOs2Table( libfile io, const arch_header *arch, name_len *dllName, bool coff_obj, importType type, unsigned length )
{
    unsigned_16 ordinal;
    char        sym_name[256];  /* maximum name len is 255 characters */
    unsigned    bytes_read;
    unsigned    total_read;

    total_read = 0;
    while( (bytes_read = getLenName( io, sym_name )) > 0 ) {
        ordinal = getU16( io );
        /*
         * Make sure we're not reading past the end of name table
         */
        total_read += bytes_read + 1 + sizeof( unsigned_16 );
        if( total_read > length ) {
            if( !Options.quiet ) {
                Warning( ERR_BAD_DLL, io->name );
            }
            return;
        }
        /*
         * The resident/non-resident name tables contain more than just exports
         * (module names, comments etc.). Everything that has ordinal of zero isn't
         * an export but more exports could follow
         */
        if( ordinal ) {
            if( coff_obj ) {
                CoffMKImport( arch, ORDINAL, ordinal, dllName, sym_name, NULL, WL_PROC_X86 );
            } else {
                OmfMKImport( arch, type, ordinal, dllName, sym_name, NULL, WL_PROC_X86 );
            }
        }
    }
}

static void os2AddImport( libfile io, long header_offset, const arch_header *arch )
{
    os2_exe_header  os2_header;
    char            dll_name[256];  /* maximum name len is 255 characters */
    char            junk[256];      /* maximum name len is 255 characters */
    importType      type;
    unsigned        bytes_read;
    name_len        dllName;

    LibSeek( io, header_offset, SEEK_SET );
    LibRead( io, &os2_header, sizeof( os2_header ) );
    LibSeek( io, os2_header.resident_off - sizeof( os2_header ), SEEK_CUR );
    dllName.len = getLenName( io, dll_name );
    dllName.name = dll_name;
    getU16( io );
    type = Options.r_ordinal ? ORDINAL : NAMED;
    importOs2Table( io, arch, &dllName, false, type, UINT_MAX );
    if( os2_header.nonres_off ) {
        type = Options.nr_ordinal ? ORDINAL : NAMED;
        LibSeek( io, os2_header.nonres_off, SEEK_SET );
        /*
         * The first entry is the module description and should be ignored
         */
        bytes_read = getLenName( io, junk );
        getU16( io );
        importOs2Table( io, arch, &dllName, false, type,
            os2_header.nonres_size - ( bytes_read + 1 + sizeof( unsigned_16 ) ) );
    }
}

static void AddSym2( name_len *n1, const char *n2, symbol_strength strength, unsigned char info )
/***********************************************************************************************/
{
    char    *sym_name;

    sym_name = MemAlloc( n1->len + strlen( n2 ) + 1 );
    strcpy( strcpy( sym_name, n1->name ) + n1->len, n2 );
    AddSym( sym_name, strength, info );
    MemFree( sym_name );
}

static void coffAddImportOverhead( const arch_header *arch, name_len *dllName, processor_type processor )
{
    char        *buffer;
    char        *mod_name;
    size_t      mod_len;

    buffer = MakeFName( dllName->name );
    mod_len = strlen( buffer );
    mod_name = strcpy( MemAlloc( mod_len + 1 ), buffer );
    buffer = MemAlloc( mod_len + 22 );
    strcpy( strcpy( buffer, str_coff_import_descriptor.name ) + str_coff_import_descriptor.len, mod_name );
    CoffMKImport( arch, IMPORT_DESCRIPTOR, 0, dllName, buffer, NULL, processor );
    CoffMKImport( arch, NULL_IMPORT_DESCRIPTOR, 0, dllName, str_coff_null_import_descriptor.name, NULL, processor );
    buffer[0] = 0x7f;
    strcpy( strcpy( buffer + 1, mod_name ) + mod_len, str_coff_null_thunk_data.name );
    CoffMKImport( arch, NULL_THUNK_DATA, 0, dllName, buffer, NULL, processor );
    MemFree( buffer );
    MemFree( mod_name );
}

static void os2FlatAddImport( libfile io, long header_offset, const arch_header *arch )
{
    os2_flat_header os2_header;
    char            dll_name[256];  /* maximum name len is 255 characters */
    bool            coff_obj;
    importType      type;
    name_len        dllName;

    coff_obj = ( Options.coff_found || ( Options.libtype == WL_LTYPE_AR && !Options.omf_found ) );
    LibSeek( io, header_offset, SEEK_SET );
    LibRead( io, &os2_header, sizeof( os2_header ) );
    LibSeek( io, os2_header.resname_off - sizeof( os2_header ), SEEK_CUR );
    dllName.len = getLenName( io, dll_name );
    dllName.name = dll_name;
    getU16( io );
    if( coff_obj ) {
        coffAddImportOverhead( arch, &dllName, WL_PROC_X86 );
    }
    type = Options.r_ordinal ? ORDINAL : NAMED;
    importOs2Table( io, arch, &dllName, coff_obj, type, os2_header.resname_off - os2_header.entry_off );
    if( os2_header.nonres_off ) {
        LibSeek( io, os2_header.nonres_off, SEEK_SET );
        type = Options.nr_ordinal ? ORDINAL : NAMED;
        importOs2Table( io, arch, &dllName, coff_obj, type, os2_header.nonres_size );
    }
}

static bool nlmAddImport( libfile io, long header_offset, const arch_header *arch )
{
    nlm_header  nlm;
    char        dll_name[256];  /* maximum name len is 255 characters */
    char        sym_name[256];  /* maximum name len is 255 characters */
    name_len    dllName;

    LibSeek( io, header_offset, SEEK_SET );
    LibRead( io, &nlm, sizeof( nlm ) );
    if( memcmp( nlm.signature, NLM_SIGNATURE, NLM_SIGNATURE_LENGTH ) != 0 ) {
        return( false );
    }
    LibSeek( io, offsetof( nlm_header, moduleName ) , SEEK_SET );
    dllName.len = getLenName( io, dll_name );
    if( dllName.len == 0 ) {
        FatalError( ERR_BAD_DLL, io->name );
    }
    dllName.name = dll_name;
    LibSeek( io, nlm.publicsOffset, SEEK_SET  );
    while( nlm.numberOfPublics > 0 ) {
        nlm.numberOfPublics--;
        if( getLenName( io, sym_name ) == 0 ) {
            FatalError( ERR_BAD_DLL, io->name );
        }
        getU32( io );
        OmfMKImport( arch, NAMED, 0, &dllName, sym_name, NULL, WL_PROC_X86 );
    }
    return( true );
}

static void peAddImport( libfile io, long header_offset, const arch_header *arch )
{
    obj_file        *ofile;
    orl_sec_handle  export_sec;
    orl_sec_base    export_base;
    char            *edata;
    name_len        dllName;
    char            *sym_name;
    Coff32_Export   *export_header;
    Coff32_EName    *name_table;
    Coff32_EOrd     *ord_table;
    unsigned        i;
    long            ordinal_base;
    processor_type  processor = WL_PROC_NONE;
    importType      type;
    bool            coff_obj;
    long            adjust;
    arch_header     tmp_arch;

    LibSeek( io, header_offset, SEEK_SET );
    if( Options.libtype == WL_LTYPE_MLIB ) {
        FatalError( ERR_NOT_LIB, ctext_WL_FTYPE_COFF, ctext_WL_LTYPE_MLIB );
    }
    coff_obj = ( Options.coff_found || ( Options.libtype == WL_LTYPE_AR && !Options.omf_found ) );

    ofile = OpenORLObjFile( io->name );
    if( ofile->orl == NULL ) {
        FatalError( ERR_CANT_READ, io->name, "Unknown error" );
    }

    switch( ORLFileGetMachineType( ofile->orl ) ) {
    case ORL_MACHINE_TYPE_ALPHA:
        processor = WL_PROC_AXP;
        coff_obj = true;
        Options.coff_found = true;
        break;
    case ORL_MACHINE_TYPE_R4000:
        processor = WL_PROC_MIPS;
        coff_obj = true;
        Options.coff_found = true;
        break;
    case ORL_MACHINE_TYPE_PPC601:
        processor = WL_PROC_PPC;
        Options.coff_found = true;
        coff_obj = true;
        break;
    case ORL_MACHINE_TYPE_AMD64:
        processor = WL_PROC_X64;
        coff_obj = true;
        break;
    case ORL_MACHINE_TYPE_I386:
        processor = WL_PROC_X86;
        break;
    case ORL_MACHINE_TYPE_NONE:
        break;
    default:
        FatalError( ERR_CANT_READ, io->name, "Not an AXP or PPC DLL" );
    }
    export_sec = FindSec( ofile, ".edata" );
    ORLSecGetContents( export_sec, (unsigned_8 **)&edata );
    ORLSecGetBase( export_sec, &export_base );

    if( export_table_rva != 0 ) {
        adjust = U64Low( export_base ) - export_table_rva;
        edata += export_table_rva - U64Low( export_base );
    } else {
        adjust = 0;
    }

    export_header = (Coff32_Export *)edata;
    name_table = (Coff32_EName *)(edata + export_header->NamePointerTableRVA - U64Low( export_base ) + adjust);
    ord_table = (Coff32_EOrd *)(edata + export_header->OrdTableRVA - U64Low( export_base ) + adjust);
    ordinal_base = export_header->ordBase;

    tmp_arch.date = arch->date;
    tmp_arch.uid = arch->uid;
    tmp_arch.gid = arch->gid;
    tmp_arch.mode = arch->mode;
    tmp_arch.size = arch->size;
    tmp_arch.libtype = arch->libtype;

    dllName.name = tmp_arch.name = edata + export_header->nameRVA - U64Low( export_base ) + adjust;
    dllName.len = strlen( dllName.name );
    tmp_arch.ffname = NULL;

    if( coff_obj ) {
        coffAddImportOverhead( &tmp_arch, &dllName, processor );
    }
    for( i = 0; i < export_header->numNamePointer; i++ ) {
        sym_name = &(edata[name_table[i] - U64Low( export_base ) + adjust]);
        if( coff_obj ) {
            CoffMKImport( &tmp_arch, ORDINAL, ord_table[i] + ordinal_base, &dllName, sym_name, NULL, processor );
            AddSym2( &str_coff_imp_prefix, sym_name, SYM_WEAK, 0 );
        } else {
            type = Options.r_ordinal ? ORDINAL : NAMED;
            OmfMKImport( &tmp_arch, type, ord_table[i] + ordinal_base, &dllName, sym_name, NULL, WL_PROC_X86 );
//            AddSym2( &str_coff_imp_prefix, sym_name, SYM_WEAK, 0 );
        }
        if( processor == WL_PROC_PPC ) {
            AddSym2( &str_coff_ppc_prefix, sym_name, SYM_WEAK, 0 );
        }
    }

    CloseORLObjFile( ofile );
    MemFree( ofile );
}


static char *GetImportString( char **rawpntr, const char *original )
{
    bool    quote = false;
    char    *raw;
    char    *result;

    raw = *rawpntr;
    if( *raw == '\'' ) {
        quote = true;
        ++raw;
    }

    result = raw;

    while( *raw != '\0' && (quote || (*raw != '.')) ) {
        if( quote
          && (*raw == '\'') ) {
            quote  = false;
            *raw++ = '\0';
            break;
        }

        ++raw;
    }

    if( *raw == '.'
      || ( *raw == '\0'
      && !quote ) ) {
        if( *raw == '.' )
            *raw++ = '\0';

        *rawpntr = raw;
    } else {
        FatalError( ERR_BAD_CMDLINE, original );
    }

    return( result );
}


void ProcessImportWlib( const char *name )
{
    name_len        dllName;
    char            *symName;
    char            *exportedName;
    char            *ordString;
    long            ordinal;
    arch_header     arch;
    char            *buffer;
    Elf32_Export    export_table[2];
    Elf32_Sym       sym_table[3];
    char            *namecopy;
    char            *p;
    char            *dll_name;
    unsigned        sym_len;

    p = namecopy = MemStrdup( name );

    symName = GetImportString( &p, name );
    if( *p == '\0'
      || *symName == '\0' ) {
        FatalError( ERR_BAD_CMDLINE, name );
    }

    dll_name = GetImportString( &p, name );
    if( *dll_name == '\0' ) {
        FatalError( ERR_BAD_CMDLINE, name );
    }

    exportedName = symName;     // give it a default value

    ordinal = 0;
    if( *p != '\0' ) {
        ordString = GetImportString( &p, name );
        if( *ordString != '\0' ) {
            if( isdigit( *(unsigned char *)ordString ) ) {
                ordinal = strtoul( ordString, NULL, 0 );
            } else {
                symName = ordString;
            }
        }
        /*
         * Make sure there is nothing other than white space on the rest
         * of the line.
         */
        if( ordinal ) {
            while( isspace( *(unsigned char *)p ) )
                ++p;
            if( *p != '\0' ) {
                FatalError( ERR_BAD_CMDLINE, name );
            }
        } else if( *p != '\0' ) {
            exportedName = GetImportString( &p, name );
            if( exportedName == NULL
              || *exportedName == '\0' ) {
                exportedName = symName;
            } else if( isdigit( *(unsigned char *)exportedName ) ) {
                ordinal      = strtoul( exportedName, NULL, 0 );
                exportedName = symName;
            } else if( *p != '\0' ) {
                ordString = GetImportString( &p, name );
                if( *ordString != '\0' ) {
                    if( isdigit( *(unsigned char *)ordString ) ) {
                        ordinal = strtoul( ordString, NULL, 0 );
                    } else {
                        FatalError( ERR_BAD_CMDLINE, name );
                    }
                }
            }
        }
    }

    if( Options.filetype == WL_FTYPE_NONE ) {
        if( Options.omf_found ) {
            Options.filetype = WL_FTYPE_OMF;
        } else if( Options.coff_found ) {
            Options.filetype = WL_FTYPE_COFF;
        } else if( Options.elf_found ) {
            Options.filetype = WL_FTYPE_ELF;
        }
    }

    if( Options.processor == WL_PROC_NONE ) {
        switch( Options.filetype ) {
        case WL_FTYPE_OMF:
            Options.processor = WL_PROC_X86;
            break;
        case WL_FTYPE_ELF:
            Options.processor = WL_PROC_PPC;
            break;
        default:
            switch( Options.libtype ) {
            case WL_LTYPE_OMF:
                Options.processor = WL_PROC_X86;
                break;
            case WL_LTYPE_MLIB:
                Options.processor = WL_PROC_PPC;
                break;
            default:
#if defined( __AXP__ )
                Options.processor = WL_PROC_AXP;
#elif defined( __MIPS__ )
                Options.processor = WL_PROC_MIPS;
#elif defined( __PPC__ )
                Options.processor = WL_PROC_PPC;
#else
                Options.processor = WL_PROC_X86;
#endif
            }
        }
        if( !Options.quiet ) {
            const char      *ctext;

            switch( Options.processor ) {
            case WL_PROC_X86:
                ctext = ctext_WL_PROC_X86;
                break;
            case WL_PROC_AXP:
                ctext = ctext_WL_PROC_AXP;
                break;
            case WL_PROC_MIPS:
                ctext = ctext_WL_PROC_MIPS;
                break;
            case WL_PROC_PPC:
                ctext = ctext_WL_PROC_PPC;
                break;
            default:
                ctext = ctext_WL_PROC_NONE;
                break;
            }
            Warning( ERR_NO_PROCESSOR, ctext );
        }
    }
    if( Options.filetype == WL_FTYPE_NONE ) {
        switch( Options.libtype ) {
        case WL_LTYPE_MLIB:
            Options.filetype = WL_FTYPE_ELF;
            break;
        case WL_LTYPE_AR:
            Options.filetype = WL_FTYPE_COFF;
            break;
        case WL_LTYPE_OMF:
            Options.filetype = WL_FTYPE_OMF;
            break;
        default:
            switch( Options.processor ) {
            case WL_PROC_AXP:
                Options.libtype = WL_LTYPE_AR;
                Options.filetype = WL_FTYPE_COFF;
                break;
            case WL_PROC_MIPS:
#ifdef __NT__
                Options.libtype = WL_LTYPE_AR;
                Options.filetype = WL_FTYPE_COFF;
#else
                Options.libtype = WL_LTYPE_MLIB;
                Options.filetype = WL_FTYPE_ELF;
#endif
                break;
            case WL_PROC_PPC:
#ifdef __NT__
                Options.libtype = WL_LTYPE_AR;
                Options.filetype = WL_FTYPE_COFF;
#else
                Options.libtype = WL_LTYPE_MLIB;
                Options.filetype = WL_FTYPE_ELF;
#endif
                break;
            case WL_PROC_X64:
#ifdef __NT__
                Options.libtype = WL_LTYPE_AR;
                Options.filetype = WL_FTYPE_COFF;
#else
                Options.libtype = WL_LTYPE_MLIB;
                Options.filetype = WL_FTYPE_ELF;
#endif
                break;
            case WL_PROC_X86:
                Options.filetype = WL_FTYPE_OMF;
                break;
            }
        }
        if( !Options.quiet ) {
            const char      *ctext;

            switch( Options.filetype ) {
            case WL_FTYPE_OMF:
                ctext = ctext_WL_FTYPE_OMF;
                break;
            case WL_FTYPE_COFF:
                ctext = ctext_WL_FTYPE_COFF;
                break;
            case WL_FTYPE_ELF:
                ctext = ctext_WL_FTYPE_ELF;
                break;
            default:
                ctext = ctext_WL_FTYPE_NONE;
                break;
            }
            Warning( ERR_NO_TYPE, ctext );
        }
    }
    if( Options.libtype == WL_LTYPE_NONE ) {
        switch( Options.filetype ) {
        case WL_FTYPE_ELF:
            Options.libtype = WL_LTYPE_MLIB;
            break;
        case WL_FTYPE_COFF:
            Options.libtype = WL_LTYPE_AR;
            break;
        }
    }

    arch.name = dll_name;
    arch.ffname = NULL;
    arch.date = time( NULL );
    arch.uid = 0;
    arch.gid = 0;
    arch.mode = AR_S_IFREG | (AR_S_IRUSR | AR_S_IWUSR | AR_S_IRGRP | AR_S_IWGRP | AR_S_IROTH | AR_S_IWOTH);
    arch.size = 0;
    arch.libtype = WL_LTYPE_NONE;

    dllName.name = dll_name;
    dllName.len = strlen( dll_name );

    switch( Options.filetype ) {
    case WL_FTYPE_ELF:
        sym_len = strlen( symName ) + 1;
        if( ordinal ) {
            export_table[0].exp_ordinal = ordinal;
            export_table[1].exp_ordinal = ordinal;
        } else {
            export_table[0].exp_ordinal = -1;
            export_table[1].exp_ordinal = -1;
        }
        export_table[0].exp_symbol = 1;
        export_table[1].exp_symbol = 2;
        /*
         * create strings array "<symName>\0<exportedName>\0"
         */
        buffer = MemAlloc( sym_len + strlen( exportedName ) + 1 );
        strcpy( buffer, symName );
        strcpy( buffer + sym_len, exportedName );
        /*
         * set offsets to strings array
         */
        sym_table[1].st_name = 0;
        sym_table[2].st_name = sym_len;

        ElfMKImport( &arch, ELFRENAMED, 2, &dllName, buffer, export_table, sym_table, Options.processor );

        MemFree( buffer );

        if( ordinal ) {
            AddSym( symName, SYM_STRONG, ELF_IMPORT_SYM_INFO );
        } else {
            AddSym( symName, SYM_STRONG, ELF_IMPORT_NAMED_SYM_INFO );
        }
        break;
    case WL_FTYPE_COFF:
        if( Options.libtype == WL_LTYPE_OMF ) {
            FatalError( ERR_NOT_LIB, ctext_WL_FTYPE_COFF, ctext_WL_LTYPE_OMF );
        }
        if( Options.libtype == WL_LTYPE_MLIB ) {
            FatalError( ERR_NOT_LIB, ctext_WL_FTYPE_COFF, ctext_WL_LTYPE_MLIB );
        }
        coffAddImportOverhead( &arch, &dllName, Options.processor );
        if( ordinal ) {
            CoffMKImport( &arch, ORDINAL, ordinal, &dllName, symName, NULL, Options.processor );
        } else {
            CoffMKImport( &arch, NAMED, ordinal, &dllName, symName, exportedName, Options.processor );
        }
        AddSym2( &str_coff_imp_prefix, symName, SYM_WEAK, 0 );
        if( Options.processor == WL_PROC_PPC ) {
            AddSym2( &str_coff_ppc_prefix, symName, SYM_WEAK, 0 );
        }
        break;
    case WL_FTYPE_OMF:
        if( Options.libtype == WL_LTYPE_MLIB ) {
            FatalError( ERR_NOT_LIB, ctext_WL_FTYPE_OMF, ctext_WL_LTYPE_MLIB );
        }
        if( ordinal ) {
            OmfMKImport( &arch, ORDINAL, ordinal, &dllName, symName, NULL, WL_PROC_X86 );
        } else {
            OmfMKImport( &arch, NAMED, ordinal, &dllName, symName, exportedName, WL_PROC_X86 );
        }
        //AddSym2( &str_coff_imp_prefix, symName, SYM_WEAK, 0 );
        break;
    }
    MemFree( namecopy );
}

size_t ElfImportSize( import_sym *impsym )
{
    size_t          len;
    elf_import_sym  *elfimp;

    len = ELFBASEIMPORTSIZE + impsym->dllName.len + 1;
    switch( impsym->type ) {
    case ELF:
        len += impsym->u.elf.numsyms * 0x21;
        for( elfimp = impsym->u.elf.symlist; elfimp != NULL; elfimp = elfimp->next ) {
            len += elfimp->sym.len;
        }
        len = __ROUND_UP_SIZE_EVEN( len );
        break;
    case ELFRENAMED:
        len += 0x21 + 1 + impsym->u.elf.symlist->sym.len + impsym->u.elf.symlist->next->sym.len;
        len = __ROUND_UP_SIZE_EVEN( len );
        break;
    default:
        break;
    }
    return( len );
}

size_t CoffImportSize( import_sym *impsym )
{
    size_t  dll_len;
    size_t  mod_len;
    size_t  ret;
    size_t  sym_len;
    size_t  exp_len;
    size_t  opt_hdr_len;

    dll_len = impsym->dllName.len;
    mod_len = strlen( MakeFName( impsym->dllName.name ) );

    switch( impsym->type ) {
    case IMPORT_DESCRIPTOR:
        opt_hdr_len = 0;
        switch( impsym->processor ) {
        case WL_PROC_X64:
            if( Options.coff_import_long ) {
                opt_hdr_len = sizeof( coff_opt_hdr64 );
            }
            break;
        case WL_PROC_AXP:
        case WL_PROC_MIPS:
        case WL_PROC_PPC:
        case WL_PROC_X86:
        default:
            if( Options.coff_import_long ) {
                opt_hdr_len = sizeof( coff_opt_hdr32 );
            }
            break;
        }
        return( COFF_FILE_HEADER_SIZE                               // header
            + opt_hdr_len                                           // optional header
            + 2 * COFF_SECTION_HEADER_SIZE +                        // section table (headers)
            + 0x14 + 3 * COFF_RELOC_SIZE                            // section data
            + __ROUND_UP_SIZE_EVEN( dll_len + 1 )                   // section data
            + 7 * COFF_SYM_SIZE                                     // symbol table
            + 4                                                     // string table
            + str_coff_import_descriptor.len + mod_len + 1          // string table
            + str_coff_null_import_descriptor.len + 1               // string table
            + 1 + mod_len + str_coff_null_thunk_data.len + 1 );     // string table
    case NULL_IMPORT_DESCRIPTOR:
        return( COFF_FILE_HEADER_SIZE
            + COFF_SECTION_HEADER_SIZE
            + 0x14
            + COFF_SYM_SIZE
            + 4                                                     // string table
            + str_coff_null_import_descriptor.len + 1 ) ;           // string table
    case NULL_THUNK_DATA:
        return( COFF_FILE_HEADER_SIZE
            + 2 * COFF_SECTION_HEADER_SIZE
            + 0x4 + 0x4
            + COFF_SYM_SIZE
            + 4                                                     // string table
            + 1 + mod_len + str_coff_null_thunk_data.len + 1 );     // string table
    case ORDINAL:
    case NAMED:
        sym_len = strlen( impsym->u.omf_coff.symName );
        if( Options.coff_import_long ) {
            if( impsym->type == NAMED ) {
                if( impsym->u.omf_coff.exportedName == NULL ) {
                    exp_len = sym_len;
                } else {
                    exp_len = strlen( impsym->u.omf_coff.exportedName );
                }
                ret = COFF_FILE_HEADER_SIZE
                    + 4 * COFF_SECTION_HEADER_SIZE
                    + 4 + COFF_RELOC_SIZE                           // idata$5
                    + 4 + COFF_RELOC_SIZE                           // idata$4
                    + 2 + __ROUND_UP_SIZE_EVEN( exp_len + 1 )       // idata$6
                    + 11 * COFF_SYM_SIZE
                    + 4                                             // string table
                    + str_coff_import_descriptor.len + mod_len + 1; // string table
            } else {
                ret = COFF_FILE_HEADER_SIZE
                    + 3 * COFF_SECTION_HEADER_SIZE
                    + 4 + 4
                    + 9 * COFF_SYM_SIZE
                    + 4                                             // string table
                    + str_coff_import_descriptor.len + mod_len + 1; // string table
            }
            switch( impsym->processor ) {
            case WL_PROC_AXP:
                if( sym_len > 8 ) {
                    /*
                     * Everything goes to symbol table
                     */
                    ret += sym_len + 1                              // string table
                        + str_coff_imp_prefix.len + sym_len + 1;    // string table
                } else if( sym_len > 2 ) {
                    /*
                     * Undecorated symbol can be stored directly, but the
                     * version with "__imp_" prepended goes to symbol table
                     */
                    ret += str_coff_imp_prefix.len + sym_len + 1;   // string table
                }
                ret += 0xc + 3 * COFF_RELOC_SIZE;   // .text
                break;
            case WL_PROC_MIPS:
                /*
                 * not yet implemented
                 */
                break;
            case WL_PROC_PPC:
                if( sym_len > 8 ) {
                    ret += sym_len + 1                              // string table
                        + str_coff_ppc_prefix.len + sym_len + 1     // string table
                        + str_coff_imp_prefix.len + sym_len + 1;    // string table
                } else if( sym_len > 6 ) {
                    ret += str_coff_ppc_prefix.len + sym_len + 1    // string table
                        + str_coff_imp_prefix.len + sym_len + 1;    // string table
                } else if( sym_len > 2 ) {
                    ret += str_coff_imp_prefix.len + sym_len + 1;   // string table
                }
                ret += 6 * COFF_SYM_SIZE
                    + 2 * COFF_SECTION_HEADER_SIZE
                    + 0x18 + COFF_RELOC_SIZE
                    + 0x14 + 4 * COFF_RELOC_SIZE    // .pdata
                    + 0x8  + 2 * COFF_RELOC_SIZE;   // .reldata
                break;
            case WL_PROC_X64:
                /*
                 * See comment for AXP above
                 */
                if( sym_len > 8 ) {
                    ret += sym_len + 1                              // string table
                        + str_coff_imp_prefix.len + sym_len + 1;    // string table
                } else if( sym_len > 2 ) {
                    ret += str_coff_imp_prefix.len + sym_len + 1;   // string table
                }
                ret += 6 + COFF_RELOC_SIZE;     // .text
                break;
            case WL_PROC_X86:
                /*
                 * See comment for AXP above
                 */
                if( sym_len > 8 ) {
                    ret += sym_len + 1                              // string table
                        + str_coff_imp_prefix.len + sym_len + 1;    // string table
                } else if( sym_len > 2 ) {
                    ret += str_coff_imp_prefix.len + sym_len + 1;   // string table
                }
                ret += 6 + COFF_RELOC_SIZE;     // .text
                break;
            }
        } else {
            ret = sizeof( coff_import_object_header )
                + dll_len + 1
                + sym_len + 1;                                      // string table
        }
        return( ret );
    default:
        break;
    }
    return( 0 );
}

static short    ElfProcessors[] = {
    #define WL_PROC(p,e,n)  e,
    WL_PROCS
    #undef WL_PROC
};

void ElfWriteImport( libfile io, sym_file *sfile )
{
    elf_import_sym  *elfimp;
    import_sym      *impsym;
    size_t          strtabsize;
    size_t          numsyms;
    bool            padding;
    size_t          offset;

    impsym = sfile->impsym;
    strtabsize = ELFBASESTRTABSIZE + impsym->dllName.len + 1;
    for( elfimp = impsym->u.elf.symlist; elfimp != NULL; elfimp = elfimp->next ) {
        strtabsize += elfimp->sym.len + 1;
    }
    padding = ( (strtabsize & 1) != 0 );
    strtabsize = __ROUND_UP_SIZE_EVEN( strtabsize );
    mset_U16LE( &(ElfBase[0x12]), ElfProcessors[impsym->processor] );
    mset_U32LE( &(ElfBase[0x74]), strtabsize );
    mset_U32LE( &(ElfBase[0x98]), strtabsize + 0x100 );
    mset_U32LE( &(ElfBase[0xc0]), strtabsize + 0x118 );
    switch( impsym->type ) {
    case ELF:
        numsyms = impsym->u.elf.numsyms;
        break;
    case ELFRENAMED:
        numsyms = 1;
        break;
    default:
        numsyms = 0;
        break;
    }
    mset_U32LE( &(ElfBase[0xc4]), 0x10 * ( numsyms + 1 ) );
    mset_U32LE( &(ElfBase[0xe8]), strtabsize + 0x128 + 0x10 * numsyms );
    mset_U32LE( &(ElfBase[0xec]), 0x10 * numsyms );
    LibWrite( io, ElfBase, ElfBase_SIZE );
    LibWrite( io, impsym->dllName.name, impsym->dllName.len + 1 );
    for( elfimp = impsym->u.elf.symlist; elfimp != NULL; elfimp = elfimp->next ) {
        LibWrite( io, elfimp->sym.name, elfimp->sym.len + 1 );
    }
    if( padding ) {
        LibWrite( io, AR_FILE_PADDING_STRING, AR_FILE_PADDING_STRING_LEN );
    }
    LibWrite( io, ElfOSInfo, ElfOSInfo_SIZE );

    offset = 0;
    strtabsize = ELFBASESTRTABSIZE + impsym->dllName.len + 1;
    for( elfimp = impsym->u.elf.symlist; elfimp != NULL; elfimp = elfimp->next ) {
        LibWriteU32LE( io, strtabsize );
        LibWriteU32LE( io, offset );
        LibWriteU32LE( io, 0x00000010 );
        LibWriteU32LE( io, ( elfimp->ordinal == -1 ) ? 0x00040015 : 0x00040010 );
        offset += 0x10;
        strtabsize += elfimp->sym.len + 1;
        if( offset >= (numsyms * 0x10) ) {
            break;
        }
    }
    offset = 0;
    strtabsize = ELFBASESTRTABSIZE + impsym->dllName.len + 1;
    switch( impsym->type ) {
    case ELF:
        for( elfimp = impsym->u.elf.symlist; elfimp != NULL; elfimp = elfimp->next ) {
            LibWriteU32LE( io, elfimp->ordinal );
            LibWriteU32LE( io, strtabsize );
            LibWriteU32LE( io, 0x01000022 );
            LibWriteU32LE( io, 0x00000000 );
            strtabsize += elfimp->sym.len + 1;
        }
        break;
    case ELFRENAMED:
        elfimp = impsym->u.elf.symlist;
        strtabsize += elfimp->sym.len + 1;
        elfimp = elfimp->next;
        LibWriteU32LE( io, elfimp->ordinal );
        LibWriteU32LE( io, strtabsize );
        LibWriteU32LE( io, 0x01000022 );
        LibWriteU32LE( io, 0x00000000 );
        break;
    default:
        break;
    }
}

bool AddImport( libfile io, const arch_header *arch )
{
    unsigned_32     ne_header_off;
    long            header_offset;
    unsigned_16     signature;
    bool            ok;


    LibSeek( io, 0, SEEK_SET );
    ok = ( LibRead( io, &signature, sizeof( signature ) ) == sizeof( signature ) );
    if( ok ) {
        header_offset = 0;
        if( signature == EXESIGN_DOS ) {
            header_offset = NE_HEADER_OFFSET;
            LibSeek( io, header_offset, SEEK_SET );
            ok = ( LibRead( io, &ne_header_off, sizeof( ne_header_off ) ) == sizeof( ne_header_off ) );
            if( ok ) {
                header_offset = ne_header_off;
                LibSeek( io, header_offset, SEEK_SET );
                ok = ( LibRead( io, &signature, sizeof( signature ) ) == sizeof( signature ) );
            }
        }
        if( ok ) {
            if( signature == EXESIGN_LE ) {
                os2FlatAddImport( io, header_offset, arch );
            } else if( signature == EXESIGN_LX ) {
                os2FlatAddImport( io, header_offset, arch );
            } else if( signature == EXESIGN_NE ) {
                os2AddImport( io, header_offset, arch );
            } else if( signature == EXESIGN_PE ) {
                peAddImport( io, header_offset, arch );
            } else if( header_offset ) {
                ok = false;
            } else {
                ok = elfAddImport( io, header_offset, arch );
                if( !ok ) {
                    ok = nlmAddImport( io, header_offset, arch );
                }
            }
        }
    }
    LibSeek( io, 0, SEEK_SET );
    return( ok );
}
