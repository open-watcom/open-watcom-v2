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
* Description:  Import library creation.
*
****************************************************************************/


#include "wlib.h"

// must correspond to defines in implib.h
static char *procname[ 5 ] = { "", "AXP", "PPC", "X86","" };

static void coffAddImportOverhead( arch_header *arch, char *DLLName, processor_type processor );

static void fillInShort( unsigned_16 value, char *out )
{
    out[ 0 ] = value & 255;
    out[ 1 ] = ( value >> 8 ) & 255;
}

static void fillInLong( unsigned_32 value, char *out )
{
    out[ 0 ] = value & 255;
    out[ 1 ] = ( value >> 8 ) & 255;
    out[ 2 ] = ( value >> 16 ) & 255;
    out[ 3 ] = ( value >> 24 ) & 255;
}

static orl_sec_handle found_sec_handle;
static orl_return FindHelper( orl_sec_handle sec )
{
    if( found_sec_handle == 0 ) {
        found_sec_handle = sec;
    }
    return( ORL_OKAY );
}


static unsigned long  export_table_rva;
static orl_return FindExportTableHelper( orl_sec_handle sec )
{
    if( found_sec_handle == 0 ) {
        unsigned long   base = ORLSecGetBase( sec );

        if( ( base <= export_table_rva )
            && ( base + ORLSecGetSize( sec ) > export_table_rva ) ) {
            found_sec_handle = sec;
        }
    }

    return( ORL_OKAY );
}

static orl_sec_handle FindSec( obj_file *ofile, char *name )
{
    export_table_rva = 0;
    found_sec_handle = 0;

    ORLFileScan( ofile->orl, name, FindHelper );
    if( found_sec_handle == 0 ) {
        if( !stricmp( ".edata", name ) ) {
            export_table_rva = ORLExportTableRVA( ofile->orl );

            if( export_table_rva == 0L ) {
                FatalError( ERR_NO_EXPORTS, ofile->hdl->name );
            }

            ORLFileScan( ofile->orl, NULL, FindExportTableHelper );
        }

        if( found_sec_handle == 0 ) {
            FatalError( ERR_NO_EXPORTS, ofile->hdl->name );
        }
    }

    return( found_sec_handle );
}

static bool elfAddImport( arch_header *arch, libfile io )
{
    obj_file        *ofile;
    orl_sec_handle  sym_sec;
    orl_sec_handle  export_sec;
    orl_sec_handle  string_sec;
    Elf32_Export    *export_table;
    Elf32_Sym       *sym_table;
    orl_sec_size    export_size, sym_size;
    char            *strings;
    processor_type  processor = WL_PROC_NONE;
    char            *oldname;
    char            *DLLname;
    Elf32_Word      ElfMagic;

    LibSeek( io, 0x00, SEEK_SET );
    if( !LibRead( io, &ElfMagic, sizeof( ElfMagic ) ) == sizeof( ElfMagic ) ) {
        return( FALSE );
    }
    if( !memcmp( &ElfMagic, ELF_SIGNATURE, sizeof( SEEK_CUR ) ) == 0 ) {
        return( FALSE );
    }
    LibSeek( io, 0x10 - sizeof( ElfMagic) , SEEK_CUR );
    if( LibRead( io, &ElfMagic, sizeof( ElfMagic ) ) != sizeof( ElfMagic ) ) {
        return( FALSE );
    }
    if( (ElfMagic & 0xff) != ET_DYN ) {
        return( FALSE );
    }
    LibSeek( io, 0x00, SEEK_SET );
    ofile = OpenObjFile( io->name );
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

    oldname = arch->name;
    arch->ffname = arch->name;
    arch->name = MemAlloc( _MAX_FNAME + _MAX_EXT );
    _splitpath( oldname, NULL, NULL, arch->name, NULL );
    DLLname = DupStr( arch->name );
    _splitpath( oldname, NULL, NULL, NULL, arch->name + strlen( arch->name ) );

    export_sec = FindSec( ofile, ".exports" );
    ORLSecGetContents( export_sec, (unsigned_8 **)&export_table );
    export_size = (Elf32_Word) ORLSecGetSize( export_sec ) / sizeof( Elf32_Export );
    sym_sec = ORLSecGetSymbolTable( export_sec );
    ORLSecGetContents( sym_sec, (unsigned_8 **)&sym_table );
    sym_size = (Elf32_Word) ORLSecGetSize( sym_sec ) / sizeof( Elf32_Sym );
    string_sec = ORLSecGetStringTable( sym_sec );
    ORLSecGetContents( string_sec, (unsigned_8 **)&strings );

    ElfMKImport( arch, ELF, export_size, DLLname, strings, export_table, sym_table, processor );

    MemFree( arch->name );
    MemFree( DLLname );
    arch->name = oldname;
    CloseObjFile( ofile );
    return( TRUE );
}

static bool getOs2Symbol( libfile io, char *symbol, unsigned_16 *ordinal, unsigned *len )
{
    unsigned_8  name_len;

    if( LibRead( io, &name_len, sizeof( name_len ) ) != sizeof( name_len ) ) {
        return( FALSE );
    }
    if( name_len == 0 ) {
        return( FALSE );
    }
    if( LibRead( io, symbol, name_len ) != name_len ) {
        FatalError( ERR_BAD_DLL, io->name );
    }
    symbol[ name_len ] = 0;
    if( LibRead( io, ordinal, sizeof( unsigned_16 ) ) != sizeof( unsigned_16 ) ) {
        FatalError( ERR_BAD_DLL, io->name );
    }
    *len = 1 + name_len + 2;
    return( TRUE );
}

static void importOs2Table( libfile io, arch_header *arch, char *dll_name, bool coff_obj, importType type, unsigned length )
{
    unsigned_16 ordinal;
    char        symbol[ 256 ];
    unsigned    bytes_read;
    unsigned    total_read = 0;

    while( getOs2Symbol( io, symbol, &ordinal, &bytes_read ) == TRUE ) {
        /* Make sure we're not reading past the end of name table */
        total_read += bytes_read;
        if( total_read > length ) {
            Warning( ERR_BAD_DLL, io->name );
            return;
        }
        /* The resident/non-resident name tables contain more than just exports
           (module names, comments etc.). Everything that has ordinal of zero isn't
           an export but more exports could follow */
        if( ordinal == 0 )
            continue;
        if( coff_obj == TRUE ) {
            CoffMKImport( arch, ORDINAL, ordinal, dll_name, symbol, NULL, WL_PROC_X86 );
        } else {
            OmfMKImport( arch, type, ordinal, dll_name, symbol, NULL, WL_PROC_X86 );
        }
    }
}

static void os2AddImport( arch_header *arch, libfile io )
{
    os2_exe_header  os2_header;
    unsigned_32     ne_offset;
    char            dll_name[ _MAX_FNAME + _MAX_EXT + 1 ];
    char            junk[ 256 ];
    unsigned_16     ordinal;
    importType      type;
    unsigned        bytes_read;

    LibSeek( io, OS2_NE_OFFSET, SEEK_SET );
    LibRead( io, &ne_offset, sizeof( ne_offset ) );
    LibSeek( io, ne_offset, SEEK_SET );
    LibRead( io, &os2_header, sizeof( os2_header ) );
    LibSeek( io, os2_header.resident_off - sizeof( os2_header ), SEEK_CUR );
    getOs2Symbol( io, dll_name, &ordinal, &bytes_read );
    type = Options.r_ordinal ? ORDINAL : NAMED;
    importOs2Table( io, arch, dll_name, FALSE, type, UINT_MAX );
    if( os2_header.nonres_off ) {
        type = Options.nr_ordinal ? ORDINAL : NAMED;
        LibSeek( io, os2_header.nonres_off, SEEK_SET );
        // The first entry is the module description and should be ignored
        getOs2Symbol( io, junk, &ordinal, &bytes_read );
        importOs2Table( io, arch, dll_name, FALSE, type, os2_header.nonres_size - bytes_read );
    }
}

static void os2FlatAddImport( arch_header *arch, libfile io )
{
    os2_flat_header os2_header;
    unsigned_32     ne_offset;
    char            dll_name[ 256 ];
    unsigned_16     ordinal;
    bool            coff_obj;
    importType      type;
    unsigned        bytes_read;

    if( Options.coff_found || (Options.libtype == WL_LTYPE_AR && !Options.omf_found) ) {
        coff_obj = TRUE;
    } else {
        coff_obj = FALSE;
    }
    LibSeek( io, OS2_NE_OFFSET, SEEK_SET );
    LibRead( io, &ne_offset, sizeof( ne_offset ) );
    LibSeek( io, ne_offset, SEEK_SET );
    LibRead( io, &os2_header, sizeof( os2_header ) );
    LibSeek( io, os2_header.resname_off - sizeof( os2_header ), SEEK_CUR );
    getOs2Symbol( io, dll_name, &ordinal, &bytes_read );
    if( coff_obj == TRUE ) {
        coffAddImportOverhead( arch, dll_name, WL_PROC_X86 );
    }
    type = Options.r_ordinal ? ORDINAL : NAMED;
    importOs2Table( io, arch, dll_name, coff_obj, type, os2_header.resname_off - os2_header.entry_off );
    if( os2_header.nonres_off ) {
        LibSeek( io, os2_header.nonres_off, SEEK_SET );
        type = Options.nr_ordinal ? ORDINAL : NAMED;
        importOs2Table( io, arch, dll_name, coff_obj, type, os2_header.nonres_size );
    }
}

static bool nlmAddImport( arch_header *arch, libfile io )
{
    nlm_header  nlm;
    unsigned_8  name_len;
    char        dll_name[ _MAX_FNAME + _MAX_EXT + 1 ];
    char        symbol[ 256 ];
    unsigned_32 offset;

    LibSeek( io, 0x00, SEEK_SET );
    LibRead( io, &nlm, sizeof( nlm ) );
    if( memcmp( nlm.signature, NLM_SIGNATURE, sizeof( nlm.signature ) ) != 0 ) {
        return( FALSE );
    }
    LibSeek( io, offsetof( nlm_header, moduleName ) , SEEK_SET );
    if( LibRead( io, &name_len, sizeof( name_len ) ) != sizeof( name_len ) ) {
        FatalError( ERR_BAD_DLL, io->name );
    }
    if( name_len == 0 ) {
        FatalError( ERR_BAD_DLL, io->name );
    }
    if( LibRead( io, dll_name, name_len ) != name_len ) {
        FatalError( ERR_BAD_DLL, io->name );
    }
    symbol[ name_len ] = 0;
    LibSeek( io, nlm.publicsOffset, SEEK_SET  );
    while( nlm.numberOfPublics > 0 ) {
        nlm.numberOfPublics--;
        if( LibRead( io, &name_len, sizeof( name_len ) ) != sizeof( name_len ) ) {
            FatalError( ERR_BAD_DLL, io->name );
        }
        if( LibRead( io, symbol, name_len ) != name_len ) {
            FatalError( ERR_BAD_DLL, io->name );
        }
        symbol[ name_len ] = 0;
        if( LibRead( io, &offset, sizeof( offset ) ) != sizeof( offset ) ) {
            FatalError( ERR_BAD_DLL, io->name );
        }
        OmfMKImport( arch, NAMED, 0, dll_name, symbol, NULL, WL_PROC_X86 );
    }
    return( TRUE );
}

static void peAddImport( arch_header *arch, libfile io )
{
    obj_file        *ofile;
    orl_sec_handle  export_sec;
    orl_sec_offset  export_base;
    char            *edata;
    char            *DLLName;
    char            *oldname;
    char            *currname;
    char            *buffer;
    Coff32_Export   *export_header;
    Coff32_EName    *name_table;
    Coff32_EOrd     *ord_table;
    int             i;
    long            ordinal_base;
    processor_type  processor = WL_PROC_NONE;
    importType      type;
    bool            coff_obj;
    long            adjust;
    int             sym_len;

    LibSeek( io, 0x00, SEEK_SET );
    if( Options.libtype == WL_LTYPE_MLIB ) {
        FatalError( ERR_NOT_LIB, "COFF", LibFormat() );
    }
    if( Options.coff_found || (Options.libtype == WL_LTYPE_AR && !Options.omf_found) ) {
        coff_obj = TRUE;
    } else {
        coff_obj = FALSE;
    }

    ofile = OpenObjFile( io->name );
    if( ofile->orl == NULL ) {
        FatalError( ERR_CANT_READ, io->name, "Unknown error" );
    }

    switch( ORLFileGetMachineType( ofile->orl ) ) {
    case ORL_MACHINE_TYPE_ALPHA:
        processor = WL_PROC_AXP;
        coff_obj = TRUE;
        Options.coff_found = TRUE;
        break;
    case ORL_MACHINE_TYPE_PPC601:
        processor = WL_PROC_PPC;
        Options.coff_found = TRUE;
        coff_obj = TRUE;
        break;
    case ORL_MACHINE_TYPE_AMD64:
        processor = WL_PROC_X64;
        coff_obj = TRUE;
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
    export_base = ORLSecGetBase( export_sec );

    if( export_table_rva != 0 ) {
        adjust = export_base - export_table_rva;
        edata += export_table_rva - export_base;
    } else {
        adjust = 0;
    }

    export_header = (Coff32_Export *)edata;
    name_table = (Coff32_EName *)(edata + export_header->NamePointerTableRVA - export_base + adjust);
    ord_table = (Coff32_EOrd *)(edata + export_header->OrdTableRVA - export_base + adjust);
    ordinal_base = export_header->ordBase;

    DLLName = edata + export_header->nameRVA - export_base + adjust;
    oldname = arch->name;
    arch->name = DLLName;
    arch->ffname = NULL;
    DLLName = DupStr( DLLName );
    if( coff_obj == TRUE ) {
        coffAddImportOverhead( arch, DLLName, processor );
    }
    for( i = 0; i < export_header->numNamePointer; i++ ) {
        currname = &(edata[ name_table[ i ] - export_base + adjust ]);
        // allocate the space for the current symbol name and
        // add enough room for the following strcpy/strcat pairs.
        sym_len = strlen( currname ) + 1;
        buffer = MemAlloc( sym_len + 6 );
        if( coff_obj == TRUE ) {
            CoffMKImport( arch, ORDINAL, ord_table[ i ] + ordinal_base, DLLName, currname, NULL, processor );
            memcpy( buffer, "__imp_", 6 );
            memcpy( buffer + 6, currname, sym_len );
            AddSym( buffer, SYM_WEAK, 0 );
        } else {
            type = Options.r_ordinal ? ORDINAL : NAMED;
            OmfMKImport( arch, type, ord_table[ i ] + ordinal_base, DLLName, currname, NULL, WL_PROC_X86 );
//            memcpy( buffer, "__imp_", 6 );
//            memcpy( buffer + 6, currname, sym_len );
//            AddSym( buffer, SYM_WEAK, 0 );
        }
        if( processor == WL_PROC_PPC ) {
            memcpy( buffer, "..", 2 );
            memcpy( buffer + 2, currname, sym_len );
            AddSym( buffer, SYM_WEAK, 0 );
        }
        MemFree( buffer ); // dispose symbol name.
    }
    MemFree( DLLName );

    arch->name = oldname;
    CloseObjFile( ofile );
}


static char *GetImportString( char **rawpntr, char *original )
{
    int     quote = FALSE;
    char    *raw  = *rawpntr;
    char    *result;

    if( *raw == '\'' ) {
        quote = TRUE;
        ++raw;
    }

    result = raw;

    while( *raw && (quote || (*raw != '.')) ) {
        if( quote && (*raw == '\'') ) {
            quote  = FALSE;
            *raw++ = 0x00;
            break;
        }

        ++raw;
    }

    if( *raw == '.' || (!*raw && !quote) ) {
        if( *raw == '.' )
            *raw++ = 0x00;

        *rawpntr = raw;
    } else {
        FatalError( ERR_BAD_CMDLINE, original );
    }

    return( result );
}


void ProcessImport( char *name )
{
    char            *DLLName, *symName, *exportedName, *ordString;
    long            ordinal = 0;
    arch_header     *arch;
    char            *buffer;
    Elf32_Export    export_table[ 2 ];
    Elf32_Sym       sym_table[ 3 ];
    char            *namecopy;
    int             sym_len;

    namecopy = DupStr( name );

    symName = GetImportString( &name, namecopy );
    if( !*name || !*symName ) {
        FatalError( ERR_BAD_CMDLINE, namecopy );
    }

    DLLName = GetImportString( &name, namecopy );
    if( !*DLLName ) {
        FatalError( ERR_BAD_CMDLINE, namecopy );
    }

    exportedName = symName;     // JBS 99/07/01 give it a default value

    if( *name ) {
        ordString = GetImportString( &name, namecopy );
        if( *ordString ) {
            if( isdigit( *ordString ) ) {
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
            while( *name && isspace( *name ) )
                ++name;

            if( *name != 0x00 ) {
                FatalError( ERR_BAD_CMDLINE, namecopy );
            }
        } else if( *name ) {
            exportedName = GetImportString( &name, namecopy );
            if( !exportedName || !*exportedName ) {
                exportedName = symName;
            } else if( isdigit( *exportedName ) ) {
                ordinal      = strtoul( exportedName, NULL, 0 );
                exportedName = symName;
            } else if( *name ) {
                ordString = GetImportString( &name, namecopy );
                if( *ordString ) {
                    if( isdigit( *ordString ) ) {
                        ordinal = strtoul( ordString, NULL, 0 );
                    } else {
                        FatalError( ERR_BAD_CMDLINE, namecopy );
                    }
                }
            }
        }
    }

    MemFree( namecopy );

    arch = MemAlloc( sizeof( arch_header ) );

    arch->name = DupStr( DLLName );
    arch->ffname = NULL;
    arch->date = time( NULL );
    arch->uid = 0;
    arch->gid = 0;
    arch->mode = AR_S_IFREG | (AR_S_IRUSR | AR_S_IWUSR | AR_S_IRGRP | AR_S_IWGRP | AR_S_IROTH | AR_S_IWOTH);
    arch->size = 0;
    arch->fnametab = NULL;
    arch->ffnametab = NULL;

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
#if defined( __PPC__ )
                Options.processor = WL_PROC_PPC;
#elif defined( __AXP__ )
                Options.processor = WL_PROC_AXP;
#else
                Options.processor = WL_PROC_X86;
#endif
            }
        }
        Warning( ERR_NO_PROCESSOR, procname[ Options.processor ] );
    }
    if( Options.filetype == WL_FTYPE_NONE ) {
        switch( Options.libtype ) {
        case WL_LTYPE_MLIB:
            Options.filetype = WL_FTYPE_ELF;
            Warning( ERR_NO_TYPE, "ELF" );
            break;
        case WL_LTYPE_AR:
            Options.filetype = WL_FTYPE_COFF;
            Warning( ERR_NO_TYPE, "COFF" );
            break;
        case WL_LTYPE_OMF:
            Options.filetype = WL_FTYPE_OMF;
            Warning( ERR_NO_TYPE, "OMF" );
            break;
        default:
            switch( Options.processor ) {
            case WL_PROC_X86:
                Options.filetype = WL_FTYPE_OMF;
                Warning( ERR_NO_TYPE, "OMF" );
                break;
            case WL_PROC_X64:
#ifdef __NT__
                Options.libtype = WL_LTYPE_AR;
                Options.filetype = WL_FTYPE_COFF;
                Warning( ERR_NO_TYPE, "COFF" );
#else
                Options.libtype = WL_LTYPE_MLIB;
                Options.filetype = WL_FTYPE_ELF;
                Warning( ERR_NO_TYPE, "ELF" );
#endif
                break;
            case WL_PROC_AXP:
                Options.libtype = WL_LTYPE_AR;
                Options.filetype = WL_FTYPE_COFF;
                Warning( ERR_NO_TYPE, "COFF" );
                break;
            case WL_PROC_PPC:
#ifdef __NT__
                Options.libtype = WL_LTYPE_AR;
                Options.filetype = WL_FTYPE_COFF;
                Warning( ERR_NO_TYPE, "COFF" );
#else
                Options.libtype = WL_LTYPE_MLIB;
                Options.filetype = WL_FTYPE_ELF;
                Warning( ERR_NO_TYPE, "ELF" );
#endif
            }
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

    switch( Options.filetype ) {
    case WL_FTYPE_ELF:
        sym_len = strlen( symName ) + 1;
        if( ordinal == 0 ) {
            export_table[ 0 ].exp_ordinal = -1;
            export_table[ 1 ].exp_ordinal = -1;
        } else {
            export_table[ 0 ].exp_ordinal = ordinal;
            export_table[ 1 ].exp_ordinal = ordinal;
        }
        export_table[ 0 ].exp_symbol = 1;
        export_table[ 1 ].exp_symbol = 2;

        sym_table[ 1 ].st_name = 0;
        sym_table[ 2 ].st_name = sym_len;

        buffer = MemAlloc( sym_len + strlen( exportedName ) + 1 );
        memcpy( buffer, symName, sym_len );
        strcpy( buffer + sym_len, exportedName );

        ElfMKImport( arch, ELFRENAMED, 2, DLLName, buffer, export_table, sym_table, Options.processor );

        if( ordinal == 0 ) {
            AddSym( symName, SYM_STRONG, ELF_IMPORT_NAMED_SYM_INFO );
        } else {
            AddSym( symName, SYM_STRONG, ELF_IMPORT_SYM_INFO );
        }
        MemFree( buffer );
        break;
    case WL_FTYPE_COFF:
        if( Options.libtype != WL_LTYPE_AR ) {
            FatalError( ERR_NOT_LIB, "COFF", LibFormat() );
        }
        coffAddImportOverhead( arch, DLLName, Options.processor );

        if( ordinal == 0 ) {
            CoffMKImport( arch, NAMED, ordinal, DLLName, symName, exportedName, Options.processor );
        } else {
            CoffMKImport( arch, ORDINAL, ordinal, DLLName, symName, NULL, Options.processor );
        }
        sym_len = strlen( symName ) + 1;
        buffer = MemAlloc( sym_len + 6 );
        memcpy( buffer, "__imp_", 6 );
        memcpy( buffer + 6, symName, sym_len );
        AddSym( buffer, SYM_WEAK, 0 );
        if( Options.processor == WL_PROC_PPC ) {
            memcpy( buffer, "..", 2 );
            memcpy( buffer + 2, symName, sym_len );
            AddSym( buffer, SYM_WEAK, 0 );
        }
        MemFree( buffer );
        break;
    case WL_FTYPE_OMF:
        if( Options.libtype == WL_LTYPE_MLIB ) {
            FatalError( ERR_NOT_LIB, "OMF", LibFormat() );
        }
        if( ordinal == 0 ) {
            OmfMKImport( arch, NAMED, ordinal, DLLName, symName, exportedName, WL_PROC_X86 );
        } else {
            OmfMKImport( arch, ORDINAL, ordinal, DLLName, symName, NULL, WL_PROC_X86 );
        }
        //sym_len = strlen( symName ) + 1;
        //buffer = MemAlloc( sym_len + 6 );
        //memcpy( buffer, "__imp_", 6 );
        //memcpy( buffer + 6, symName, sym_len );
        //AddSym( buffer, SYM_WEAK, 0 );
        //MemFree( buffer );
        break;
    }
    MemFree( arch->name );
    MemFree( arch );
}

static void coffAddImportOverhead( arch_header *arch, char *DLLName, processor_type processor )
{
    char *buffer;

    buffer = MemAlloc( 100 );

    memcpy( buffer, "__IMPORT_DESCRIPTOR_", 20 );
    _splitpath( DLLName, NULL, NULL, buffer + 20, NULL );
    CoffMKImport( arch, IMPORT_DESCRIPTOR, 0, DLLName, buffer, NULL, processor );

    CoffMKImport( arch, NULL_IMPORT_DESCRIPTOR, 0, DLLName, "__NULL_IMPORT_DESCRIPTOR", NULL, processor );

    buffer[ 0 ] = 0x7f;
    _splitpath( DLLName, NULL, NULL, buffer + 1, NULL );
    strcat( buffer, "_NULL_THUNK_DATA" );
    CoffMKImport( arch, NULL_THUNK_DATA, 0, DLLName, buffer, NULL, processor );

    MemFree( buffer );
}

int ElfImportSize( import_sym *import )
{
    int             len;
    elf_import_sym  *temp;

    len = ELFBASEIMPORTSIZE + strlen( import->DLLName ) + 1;
    switch( import->type ) {
    case ELF:
        len += import->u.elf.numsyms * 0x21;
        for( temp = import->u.elf.symlist; temp != NULL; temp = temp->next ) {
            len += temp->len;
        }
        if( len & 1 ) {
            len++;
        }
        break;
    case ELFRENAMED:
        len += 0x22 + import->u.elf.symlist->len + import->u.elf.symlist->next->len;
        if( len & 1 ) {
            len++;
        }
        break;
    default:
        break;
    }
    return( len );
}

int CoffImportSize( import_sym *import )
{
    int dll_len;
    int mod_len;
    int ret;
    int sym_len;
    int exp_len;
    int opt_hdr_len;

    dll_len = strlen( import->DLLName );
    mod_len = strlen( MakeFName( import->DLLName ) );

    switch( import->type ) {
    case IMPORT_DESCRIPTOR:
        opt_hdr_len = 0;
        switch( import->processor ) {
        case WL_PROC_X64:
            if( Options.coff_import_long ) {
                opt_hdr_len = sizeof( coff_opt_hdr64 );
            }
            break;
        case WL_PROC_AXP:
        case WL_PROC_PPC:
        case WL_PROC_X86:
        default:
            if( Options.coff_import_long ) {
                opt_hdr_len = sizeof( coff_opt_hdr );
            }
            break;
        }
        return( COFF_FILE_HEADER_SIZE                   // header
            + opt_hdr_len                               // optional header
            + 2 * COFF_SECTION_HEADER_SIZE +            // section table (headers)
            + 0x14 + 3 * COFF_RELOC_SIZE                // section data
            + (dll_len | 1) + 1                         // section data
            + 7 * COFF_SYM_SIZE                         // symbol table
            + 4 + mod_len + 21 + 25 + mod_len + 18 );   // string table
    case NULL_IMPORT_DESCRIPTOR:
        return( COFF_FILE_HEADER_SIZE
            + COFF_SECTION_HEADER_SIZE
            + 0x14
            + COFF_SYM_SIZE
            + 4 + 25 ) ;
    case NULL_THUNK_DATA:
        return( COFF_FILE_HEADER_SIZE
            + 2 * COFF_SECTION_HEADER_SIZE
            + 0x4 + 0x4
            + COFF_SYM_SIZE
            + 4 + mod_len + 18 ) ;
    case ORDINAL:
    case NAMED:
        sym_len = strlen( import->u.sym.symName );
        if( Options.coff_import_long ) {
            if( import->type == NAMED ) {
                if( import->u.sym.exportedName == NULL ) {
                    exp_len = sym_len;
                } else {
                    exp_len = strlen( import->u.sym.exportedName );
                }
                ret = COFF_FILE_HEADER_SIZE
                    + 4 * COFF_SECTION_HEADER_SIZE
                    + 4 + COFF_RELOC_SIZE       // idata$5
                    + 4 + COFF_RELOC_SIZE       // idata$4
                    + ( exp_len | 1 ) + 1 + 2   // idata$6
                    + 11 * COFF_SYM_SIZE
                    + 4 + mod_len + 21;         // 21 = strlen("__IMPORT_DESCRIPTOR_") + 1
            } else {
                ret = COFF_FILE_HEADER_SIZE
                    + 3 * COFF_SECTION_HEADER_SIZE
                    + 4 + 4
                    + 9 * COFF_SYM_SIZE
                    + 4 + mod_len + 21;
            }
            switch( import->processor ) {
            case WL_PROC_AXP:
                if( sym_len > 8 ) {
                    // Everything goes to symbol table
                    ret += sym_len + 1 + sym_len + 7;
                } else if( sym_len > 2 ) {
                    // Undecorated symbol can be stored directly, but the
                    // version with "__imp_" prepended goes to symbol table
                    ret += 7 + sym_len;
                }
                ret += 0xc + 3 * COFF_RELOC_SIZE;   // .text
                break;
            case WL_PROC_PPC:
                if( sym_len > 8 ) {
                    ret += sym_len + 1 + sym_len + 3 + sym_len + 7;
                } else if( sym_len > 6 ) {
                    ret += sym_len + 3 + sym_len + 7;
                } else if( sym_len > 2 ) {
                    ret += sym_len + 7;
                }
                ret += 6 * COFF_SYM_SIZE
                    + 2 * COFF_SECTION_HEADER_SIZE
                    + 0x18 + COFF_RELOC_SIZE
                    + 0x14 + 4 * COFF_RELOC_SIZE    // .pdata
                    + 0x8  + 2 * COFF_RELOC_SIZE;   // .reldata
                break;
            case WL_PROC_X64:
                // See comment for AXP above
                if( sym_len > 8 ) {
                    ret += sym_len + 1 + sym_len + 7;
                } else if( sym_len > 2 ) {
                    ret += 7 + sym_len;
                }
                ret += 6 + COFF_RELOC_SIZE;     // .text
                break;
            case WL_PROC_X86:
                // See comment for AXP above
                if( sym_len > 8 ) {
                    ret += sym_len + 1 + sym_len + 7;
                } else if( sym_len > 2 ) {
                    ret += 7 + sym_len;
                }
                ret += 6 + COFF_RELOC_SIZE;     // .text
                break;
            }
        } else {
            ret = sizeof( coff_import_object_header ) + dll_len + 1 + sym_len + 1;
        }
        return( ret );
    default:
        break;
    }
    return( 0 );
}

static short    ElfProcessors[ 4 ] = { 0, EM_ALPHA, EM_PPC, 0 };

void ElfWriteImport( libfile io, sym_file *sfile )
{
    elf_import_sym  *temp;
    import_sym      *import;
    long            strtabsize;
    long            numsyms = 0;
    long            parity;
    long            offset;
    long            more;

    import = sfile->import;
    strtabsize = ELFBASESTRTABSIZE + strlen( import->DLLName ) + 1;
    for( temp=import->u.elf.symlist; temp != NULL; temp = temp->next ) {
        strtabsize += temp->len + 1;
    }
    parity = strtabsize & 1;
    if( parity ) {
        strtabsize++;
    }
    fillInShort( ElfProcessors[ import->processor ], &(ElfBase[ 0x12 ]) );
    fillInLong( strtabsize, &(ElfBase[ 0x74 ]) );
    fillInLong( strtabsize + 0x100, &(ElfBase[ 0x98 ]) );
    fillInLong( strtabsize + 0x118, &(ElfBase[ 0xc0 ]) );
    switch( import->type ) {
    case ELF:
        numsyms = import->u.elf.numsyms;
        break;
    case ELFRENAMED:
        numsyms = 1;
        break;
    default:
        break;
    }
    fillInLong( 0x10 * (numsyms + 1), &(ElfBase[ 0xc4 ]) );
    fillInLong( strtabsize + 0x128 + 0x10*numsyms, &(ElfBase[ 0xe8 ]) );
    fillInLong( 0x10 * numsyms, &(ElfBase[ 0xec ]) );
    LibWrite( io, &ElfBase, ElfBase_SIZE );
    LibWrite( io, import->DLLName, strlen( import->DLLName ) + 1);
    for( temp = import->u.elf.symlist; temp != NULL; temp = temp->next ) {
        LibWrite( io, temp->name, temp->len + 1 );
    }
    if( parity ) {
        LibWrite( io, AR_FILE_PADDING_STRING, AR_FILE_PADDING_STRING_LEN );
    }
    LibWrite( io, &ElfOSInfo, ElfOSInfo_SIZE );

    offset = 0;
    strtabsize = ELFBASESTRTABSIZE + strlen( import->DLLName ) + 1;
    for( temp = import->u.elf.symlist; temp != NULL; temp = temp->next ) {
        LibWrite( io, &strtabsize, 4 );
        LibWrite( io, &offset, 4 );
        more = 0x10;
        LibWrite( io, &more, 4 );
        more = 0x00040010;
        if( temp->ordinal == -1 ) {
            more |= 0x5;
        }
        LibWrite( io, &more, 4 );

        offset += 0x10;
        strtabsize += temp->len + 1;
        if( offset >= (numsyms * 0x10) ) {
            break;
        }
    }
    offset = 0;
    strtabsize = ELFBASESTRTABSIZE + strlen( import->DLLName ) + 1;
    switch( import->type ) {
    case ELF:
        for( temp = import->u.elf.symlist; temp != NULL; temp = temp->next ) {
            LibWrite( io, &temp->ordinal, 4 );
            LibWrite( io, &strtabsize, 4 );
            more = 0x01000022;
            LibWrite( io, &more, 4 );
            more = 0;
            LibWrite( io, &more, 4 );
            strtabsize += temp->len + 1;
        }
        break;
    case ELFRENAMED:
        temp = import->u.elf.symlist;
        strtabsize += temp->len + 1;
        temp = temp->next;
        LibWrite( io, &(temp->ordinal), 4 );
        LibWrite( io, &strtabsize, 4 ) ;
        more = 0x01000022;
        LibWrite( io, &more, 4 );
        more = 0;
        LibWrite( io, &more, 4 );
        break;
    default:
        break;
    }
}

bool AddImport( arch_header *arch, libfile io )
{
    unsigned_16     dos_sig;
    unsigned_32     offset;
    unsigned_16     signature;

    LibSeek( io, 0x00, SEEK_SET );
    if( LibRead( io, &dos_sig, sizeof( dos_sig ) ) == sizeof( dos_sig ) ) {
        if( dos_sig == DOS_SIGNATURE ) {
            LibSeek( io, OS2_NE_OFFSET, SEEK_SET );
            if( LibRead( io, &offset, sizeof( offset ) ) == sizeof( offset ) ) {
                LibSeek( io, offset, SEEK_SET );
                if( LibRead( io, &signature, sizeof( signature ) ) == sizeof( signature ) ) {
                    switch( signature ) {
                    case OSF_FLAT_SIGNATURE:
                    case OSF_FLAT_LX_SIGNATURE:
                        os2FlatAddImport( arch, io );
                        return( TRUE );
                    case OS2_SIGNATURE_WORD:
                        os2AddImport( arch, io );
                        return( TRUE );
                    case PE_SIGNATURE:
                        peAddImport( arch, io );
                        return( TRUE );
                    default:
                        return( FALSE );
                    }
                }
            }
        } else if( elfAddImport( arch, io ) ) {
            return( TRUE );
        } else {
            return( nlmAddImport( arch, io ) );
        }
    }
    LibSeek( io, 0x00, SEEK_SET );
    return( FALSE );
}
