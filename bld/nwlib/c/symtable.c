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


#include <wlib.h>

static sym_table        FileTable;
sym_file                *CurrFile;
static sym_entry        **HashTable;
static sym_entry        **SortedSymbols;

#define HASH_SIZE       256

void InitFileTab()
/***************/
{
    FileTable.first = NULL;
    FileTable.add_to = &FileTable.first;
    SortedSymbols = NULL;
    HashTable = MemAllocGlobal( HASH_SIZE * sizeof( HashTable[0] ) );
    memset( HashTable, 0, HASH_SIZE * sizeof( HashTable[0] ) );
}

void FiniSymFile( sym_file *file )
{
    sym_entry           *sym, *next_sym;
    elf_import_sym      *temp;

    for( sym = file->first; sym != NULL; sym = next_sym ) {
        next_sym = sym->next;
        MemFreeGlobal( sym );
    }
    MemFreeGlobal( file->full_name );
    MemFreeGlobal( file->arch.name );
    MemFreeGlobal( file->arch.ffname );
    if( file->import != NULL ) {
        switch( file->import->type ) {
        case ELF:
        case ELFRENAMED:
            for( temp=file->import->symlist; temp!=NULL;
                         temp=file->import->symlist ) {
                file->import->symlist = temp->next;
                MemFreeGlobal( temp->name );
                MemFreeGlobal( temp );
            }
            MemFreeGlobal( file->import->DLLName );
            break;
        default:
            MemFreeGlobal( file->import->DLLName );
            MemFreeGlobal( file->import->symName );
            MemFreeGlobal( file->import->exportedName );
            break;
        }
        MemFreeGlobal( file->import );
    }
    MemFreeGlobal( file );
}


void CleanFileTab(void)
/*********************/
{
    sym_file *last = NULL;
    sym_file *curr;
    sym_file *next;

    for (curr = FileTable.first; curr; curr = next) {
        next = curr->next;

        /*
         * If curr->first is NULL then either this file contains no
         * symbols or we are ignoring all of them.  Remove the file.
         */
        if (!curr->first) {
            if (last) {
                last->next = curr->next;
            } else {
                FileTable.first = curr->next;
            }

            if (&(curr->next) == FileTable.add_to) {
                if (last) {
                    FileTable.add_to = &(last->next);
                } else {
                    FileTable.add_to = &(FileTable.first);
                }
            }

            FiniSymFile(curr);
        } else {
            last = curr;
        }
    } /* for */
} /* CleanFileTab() */


void ResetFileTab( void )
{
    sym_file            *file, *next_file;

    memset( HashTable, 0, HASH_SIZE * sizeof( HashTable[0] ) );
    for( file = FileTable.first; file != NULL; file = next_file ) {
        next_file = file->next;
        FiniSymFile( file );
    }
    FileTable.first = NULL;
    FileTable.add_to = &FileTable.first;
    MemFreeGlobal( SortedSymbols );
    SortedSymbols = NULL;
}

void FiniFileTab( void )
/***********************/
{
    MemFreeGlobal( HashTable );
}


void RemoveFromHashTable(sym_entry *sym)
/**************************************/
{
    sym_entry *hash;
    sym_entry *prev;
    int        hval;
    unsigned   len;

    hval = Hash(sym->name, &len);
    hash = HashTable[hval];

    if (hash == sym) {
        HashTable[hval] = sym->hash;
    } else if (hash) {
        prev = hash;

        for (hash = hash->hash; hash; hash = hash->hash) {
            if (hash == sym) {
                prev->hash = hash->hash;
                break;
            } else {
                prev = hash;
            }
        }
    }
} /* RemoveFromHashTable() */


static void NewSymFile( arch_header *arch )
/************************************/
{
    sym_file    *file;

    file = MemAllocGlobal( sizeof(sym_file) );
    file->first = NULL;
    file->next = NULL;
    file->arch = *arch;
    file->import = NULL;
    file->inlib_offset = 0;
    file->full_name = DupStrGlobal( file->arch.name );
    file->arch.name = DupStrGlobal( TrimPath( file->arch.name ) ); // make own copy
    file->name_length = strlen( file->arch.name );
    if( file->arch.ffname ) {
        file->arch.ffname = DupStrGlobal( file->arch.ffname );
        file->ffname_length = strlen( file->arch.ffname );
    } else {
        file->ffname_length = 0;
    }
    *(FileTable.add_to) = file;
    FileTable.add_to = &file->next;
    CurrFile = file;
}



static int CompSyms( const void *ap, const void *bp )
/***************************************************/
{
    const sym_entry *a = *(const sym_entry**)ap;
    const sym_entry *b = *(const sym_entry**)bp;
    return( strcmp( a->name, b->name ) );
}

static void WriteFileHeader( arch_header *arch )
/**********************************************/
{
    ar_header   ar;

    CreateARHeader( &ar, arch );
    WriteNew( &ar, AR_HEADER_SIZE );
}

static void WritePad( file_offset size )
{
    if( NeedsRounding( size ) ) {
        WriteNew( AR_FILE_PADDING_STRING, 1 );
    }
}

static long             NumFiles;
static long             NumSymbols;
static file_offset      TotalNameLength;
static file_offset      TotalFFNameLength;
static file_offset      TotalSymbolLength;

static void SortSymbols()
{
    sym_file *  file;
    sym_entry * sym;
    sym_entry   **sym_curr;
    int         i;
    int         name_length;
    int         name_extra;

    NumFiles = 0;
    NumSymbols = 0;
    TotalNameLength = 0;
    TotalFFNameLength = 0;
    TotalSymbolLength = 0;
    switch( Options.libtype ) {
        case WL_TYPE_AR:
            name_extra = 1;
            break;
        case WL_TYPE_MLIB:
            name_extra = 2;
            break;
    }
    for( file = FileTable.first; file != NULL; file = file->next ) {
        ++NumFiles;
        file->name_offset = TotalNameLength;
        switch( Options.libtype ) {
            case WL_TYPE_AR:
                // Always using "full" filename for AR
                if( file->arch.ffname ) {
                    name_length = file->ffname_length;
                } else {
                    file->ffname_length = 0;
                    name_length = file->name_length;
                }
                break;
            case WL_TYPE_MLIB:
                // If no full filename, assume name is full, and trim
                // it to get non-full filename.
                if( file->arch.ffname == NULL ) {
                    file->arch.ffname = file->arch.name;
                    file->ffname_length = strlen( file->arch.ffname );
                    file->arch.name = MemAllocGlobal( _MAX_FNAME + _MAX_EXT + 1 );
                    _splitpath( file->arch.ffname, NULL, NULL, file->arch.name, NULL );
                    _splitpath( file->arch.ffname, NULL, NULL, NULL, file->arch.name + strlen( file->arch.name ) );
                    file->name_length = strlen( file->arch.name );
                }
                name_length = file->name_length;
                break;
        }
        TotalNameLength += name_length + name_extra;
        TotalFFNameLength += file->ffname_length + 1;
        for( sym = file->first; sym != NULL; sym = sym->next ) {
            ++NumSymbols;
            TotalSymbolLength += sym->len + 1;
        }
    }

    if( NumSymbols == 0 ) {
        FatalError( ERR_NO_SYMBOLS );
    }

    SortedSymbols = MemAllocGlobal( NumSymbols * sizeof( SortedSymbols[0] ) );
    sym_curr = SortedSymbols;
    for( file = FileTable.first; file != NULL; file = file->next ) {
        for( sym = file->first; sym != NULL; sym = sym->next ) {
            *sym_curr = sym;
            ++sym_curr;
        }
    }

    qsort( SortedSymbols, NumSymbols, sizeof(sym_entry *), CompSyms );

    // re-hook symbols onto files in sorted order

    for( file = FileTable.first; file != NULL; file = file->next ) {
        file->first = NULL;
    }

    for( i = NumSymbols - 1; i >= 0; --i ) {
        sym = SortedSymbols[i];
        sym->next = sym->file->first;
        sym->file->first = sym;
    }
}

static void WriteOmfFileTable()
{
    sym_file *  file;
    unsigned    num_blocks;
    unsigned    dict_offset;

    if( Options.page_size == 0 ){
        Options.page_size = DEFAULT_PAGE_SIZE;
    }
    InitOmfUtil();
    PadOmf( TRUE );

    for( file = FileTable.first; file != NULL; file = file->next ) {
        WriteOmfFile(file);
    }
    WriteOmfLibTrailer();
    dict_offset = LibTell( NewLibrary );
    num_blocks = WriteOmfDict( FileTable.first );
    WriteOmfLibHeader( dict_offset, num_blocks );
    FiniOmfUtil();
}

static void WriteArMlibFileTable()
/******************/
{
    arch_header arch;
    sym_file *  file;
    sym_entry * sym;
    file_offset dict1_size;
    file_offset dict2_size;
    file_offset header_size;
    int         i;
    time_t      currenttime = time( NULL );
    file_offset obj_offset;
    int         index;
    libfile     io;
    char        buff[20];
    char *      stringpad;
    int         stringpadlen;


    SortSymbols();

    // figure out this dictionary sizes

    switch( Options.libtype ) {
        case WL_TYPE_AR:
            dict1_size = sizeof( unsigned_32 )
                       + NumSymbols * sizeof(unsigned_32)
                       + RoundWord( TotalSymbolLength );

            dict2_size = sizeof( unsigned_32 )
                       + NumFiles * sizeof( unsigned_32 )
                       + sizeof( unsigned_32 )
                       + NumSymbols * sizeof( unsigned_16 )
                       + RoundWord( TotalSymbolLength );

            header_size = AR_IDENT_LEN
                        + 3 * AR_HEADER_SIZE
                        + dict1_size
                        + dict2_size
                        + RoundWord( TotalNameLength );
            stringpad   = "\0";
            stringpadlen= 1;
            break;
        case WL_TYPE_MLIB:
            dict2_size = sizeof( unsigned_32 )
                        + NumSymbols * (1 + sizeof( unsigned_32 ) )
                        + TotalSymbolLength;

            header_size = LIBMAG_LEN + LIB_CLASS_LEN + LIB_DATA_LEN
                        + 3 * LIB_HEADER_SIZE
                        + RoundWord( dict2_size )
                        + RoundWord( TotalNameLength )
                        + RoundWord( TotalFFNameLength );
            stringpad   = LIB_LONG_NAME_END_STRING;
            stringpadlen= LIB_LONG_NAME_END_STRING_LEN;
            break;
    }

    // calculate the object files offsets

    index = 0;
    obj_offset = 0;
    for( file = FileTable.first; file != NULL; file = file->next ) {
        file->new_offset = obj_offset + header_size;
        file->index = ++index;
        obj_offset += RoundWord( file->arch.size ) + AR_HEADER_SIZE;
    }

    switch( Options.libtype ) {
        case WL_TYPE_AR:
            WriteNew( AR_IDENT, AR_IDENT_LEN );
            break;
        case WL_TYPE_MLIB:
            WriteNew( LIBMAG, LIBMAG_LEN );
            WriteNew( LIB_CLASS_DATA_SHOULDBE, LIB_CLASS_LEN + LIB_DATA_LEN );
            break;
    }

    // write the useless dictionary

    arch.date = currenttime;
    arch.uid = 0;
    arch.gid = 0;
    arch.mode = 0;
    if( Options.libtype == WL_TYPE_AR ) {
        arch.size = dict1_size;
        arch.name = "/";
        WriteFileHeader( &arch );

        WriteBigEndian32( NumSymbols );
        for( file = FileTable.first; file != NULL; file = file->next ) {
            for( sym = file->first; sym != NULL; sym = sym->next ) {
                WriteBigEndian32( sym->file->new_offset );
            }
        }
        for( file = FileTable.first; file != NULL; file = file->next ) {
            for( sym = file->first; sym != NULL; sym = sym->next ) {
                WriteNew( sym->name, sym->len+1 );
            }
        }
        WritePad( TotalSymbolLength );
    }

    // write the useful dictionary

    arch.size = dict2_size;
    arch.name = "/";
    WriteFileHeader( &arch );

    if( Options.libtype == WL_TYPE_AR ) {
        WriteLittleEndian32( NumFiles );
        for( file = FileTable.first; file != NULL; file = file->next ) {
            WriteLittleEndian32( file->new_offset );
        }
    }

    WriteLittleEndian32( NumSymbols );
    switch( Options.libtype ) {
        case WL_TYPE_AR:
            for( i = 0; i < NumSymbols; ++i ) {
                WriteLittleEndian16( SortedSymbols[i]->file->index );
            }
            break;
        case WL_TYPE_MLIB:
            for( i = 0; i < NumSymbols; ++i ) {
                WriteLittleEndian32( SortedSymbols[i]->file->index );
            }
            for( i=0; i < NumSymbols; ++i ) {
                WriteNew( &(SortedSymbols[i]->info), 1 );
            }
            break;
    }
    for( i = 0; i < NumSymbols; ++i ) {
        WriteNew( SortedSymbols[i]->name, SortedSymbols[i]->len + 1 );
    }
    switch( Options.libtype ) {
        case WL_TYPE_AR:
            WritePad( TotalSymbolLength );
            break;
        case WL_TYPE_MLIB:
            WritePad( dict2_size );
            break;
    }

    // write the string table

    arch.size = TotalNameLength;
    arch.name = "//";
    WriteFileHeader( &arch );
    for( file = FileTable.first; file != NULL; file = file->next ) {
        if( file->name_offset == -1 ) continue;
        // Always write the "full" filename for AR
        if( Options.libtype == WL_TYPE_AR && file->arch.ffname ) {
            WriteNew( file->arch.ffname, file->ffname_length );
        } else {
            WriteNew( file->arch.name, file->name_length );
        }
        WriteNew( stringpad, stringpadlen );
    }
    WritePad( TotalNameLength );

    // write the full filename table

    if( Options.libtype == WL_TYPE_MLIB ) {
        arch.size = TotalFFNameLength;
        arch.name = "///";
        WriteFileHeader( &arch );
        for( file=FileTable.first; file != NULL; file = file->next ) {
            WriteNew( file->arch.ffname, file->ffname_length + 1 );
        }
        WritePad( TotalFFNameLength );
    }

    for( file = FileTable.first; file != NULL; file = file->next ) {
        arch = file->arch;
        buff[0] = '/';
        itoa( file->name_offset, buff+1, 10 );
        arch.name = buff;
        WriteFileHeader( &arch );
        if( file->import == NULL ){
            if( file->inlib_offset != 0 ) {
                LibSeek( InLibHandle( file->inlib ), file->inlib_offset, SEEK_SET );
                Copy( InLibHandle( file->inlib ), NewLibrary, arch.size );
            } else {
                io = LibOpen( file->full_name, LIBOPEN_BINARY_READ );
                Copy( io, NewLibrary, arch.size );
                LibClose( io );
            }
        }else{
            switch( file->import->type ) {
                case ELF:
                case ELFRENAMED:
                    ElfWriteImport( NewLibrary, file );
                    break;
                default:
                    CoffWriteImport( NewLibrary, file );
                    break;
            }
        }
        WritePad( file->arch.size );
    }
}

void WriteFileTable()
{
    if( Options.libtype == 0 && Options.omf_found ) {
        if( Options.coff_found ) {
            Options.libtype = WL_TYPE_AR;
        } else {
            Options.libtype = WL_TYPE_OMF;
            }
        }
    if( Options.coff_found && (Options.libtype == 0 || Options.libtype == WL_TYPE_OMF) ) {
        Options.libtype = WL_TYPE_AR;
    }
    if( Options.elf_found && Options.libtype == 0 ) {
        Options.libtype = WL_TYPE_MLIB;
    }
    if( Options.libtype == WL_TYPE_AR || Options.libtype == WL_TYPE_MLIB ) {
        WriteArMlibFileTable();
    } else {
        WriteOmfFileTable();
    }
}

static int Hash( char *string, unsigned *plen )
{
    unsigned long       g;
    unsigned long       h;

    h = 0;
    *plen = 0;
    while( *string != 0 ) {
        h = ( h << 4 ) + *string;
        if( g = h & 0xf0000000 ) {
            h = h ^ ( g >> 24 );
            h = h ^ g;
        }
        ++string;
        ++*plen;
    }
    return( h % HASH_SIZE );
}

void AddSym( char *name, symbol_strength strength, unsigned char info )
{
    sym_entry   *sym,**owner;
    int         hash;
    unsigned    name_len;

    hash = Hash( name, &name_len );
    for( sym = HashTable[ hash ]; sym != NULL; sym = sym->hash ) {
        if( sym->len != name_len ) continue;
        if( SymbolNameCmp( sym->name, name ) == 0 ) {
            if( strength > sym->strength ) {
                owner = &sym->file->first;
                while( *owner != sym ) {
                    owner = &(*owner)->next;
                }
                *owner = sym->next;
                owner = &HashTable[ hash ];
                while( *owner != sym ) {
                    owner = &(*owner)->hash;
                }
                *owner = sym->hash;
                MemFreeGlobal( sym );
                break; //db
            } else if( strength == sym->strength ) {
                if( strength == SYM_STRONG ) {
                    Warning( ERR_DUPLICATE_SYMBOL, FormSym( name ) );
                }
            }
            return;
        }
    }
    sym = MemAllocGlobal( sizeof(sym_entry) + name_len );
    sym->len = name_len;
    sym->strength = strength;
    sym->info = info;
    memcpy( sym->name, name, name_len + 1 );
    sym->next = CurrFile->first;
    CurrFile->first = sym;
    sym->file = CurrFile;
    sym->hash = HashTable[ hash ];
    HashTable[ hash ] = sym;
}


#ifdef __DEBUG__
void DumpFileTable(void)
{
    sym_file  *file;
    sym_entry *entry;
    sym_entry *hash;
    unsigned   len;
    int        hval;
    long       files    = 0L;
    long       symbols  = 0L;

    printf("----------------------------------------------------------\n");
    printf("File Table Dump\n");
    printf("----------------------------------------------------------\n");

    for (file = FileTable.first; file; file = file->next) {
        ++files;
        printf("File: \"%s\"\n", file->full_name);
        for (entry = file->first; entry; entry = entry->next) {
            ++symbols;

            hval = Hash(entry->name, &len);
            printf("\t\"%s\" (%d, %u, \"%s\")", entry->name, hval, len,
                (HashTable[hval] ? HashTable[hval]->name : "(NULL)"));

            for (hash = entry->hash; hash; hash = hash->hash) {
                printf(" -> \"%s\"", hash->name);
                fflush(stdout);
            }
            printf("\n");
        }
    }
    printf("----------------------------------------------------------\n");
    printf("Files         : %ld\n", files);
    printf("Symbols       : %ld\n", symbols);
    printf("----------------------------------------------------------\n");
} /* DumpFileTable() */


void DumpHashTable(void)
{
    sym_entry *hash;
    int        i;
    int        length;

    printf("----------------------------------------------------------\n");
    printf("Hash Table Dump\n");
    printf("----------------------------------------------------------\n");

    for (i = 0; i < HASH_SIZE; ++i) {
        length = 0;

        if (HashTable[i]) {
            for (hash = HashTable[i]; hash; hash = hash->next) {
                ++length;
            }
        }

        printf("Offset %6d: %d\n", i, length);
    }
    printf("----------------------------------------------------------\n");
} /* DumpHashTable() */
#endif // __DEBUG__


bool RemoveObjectSymbols( char *name )
{
    sym_file    *file;
    sym_file    *prev;
    sym_entry   *sym;

    prev = NULL;
    for( file = FileTable.first; file != NULL; file = file->next ) {
        if( SymbolNameCmp( file->full_name, name ) == 0 ) {
            if (prev) { /* Not deleting from head of list */
                prev->next = file->next;

                if( FileTable.add_to == &file->next ){ /* Last node in list */
                    FileTable.add_to = &prev->next;
                }
            } else {
                if (FileTable.add_to == &file->next) { /* Only node in list */
                    FileTable.add_to = &FileTable.first;
                    FileTable.first = NULL;
                } else {  /* First node in the list */
                    FileTable.first = file->next;
                }
            }

            for( sym = file->first; sym != NULL; sym = sym->next ){
                RemoveFromHashTable(sym);
            }

            FiniSymFile(file);
            Options.modified = TRUE;
            return TRUE;
        }
        prev = file;
    }
    return FALSE;
}
void AddObjectSymbols( arch_header *arch, libfile io, long offset )
{
    obj_file            *file;

    file = OpenLibFile( arch->name, io );
    if ( file->orl ) {
        orl_file_handle orl;
        orl = file->orl;
        if( ORLFileGetFormat( file->orl ) == ORL_COFF ) {
            if( Options.libtype == WL_TYPE_MLIB ) {
                FatalError( ERR_NOT_LIB, "COFF" );
            }
            Options.coff_found = 1;
        } else {
            Options.elf_found = 1;
            if( Options.omf_found == 1 ) {
                FatalError( ERR_MIXED_OBJ, "ELF", "OMF" );
            }
        }
    } else {
        if( Options.libtype == WL_TYPE_MLIB ) {
            FatalError( ERR_NOT_LIB, "OMF" );
        }
        if( Options.elf_found == 1 ) {
            FatalError( ERR_MIXED_OBJ, "ELF", "OMF" );
        }
        Options.omf_found = 1;
    }
    NewSymFile( arch );
    CurrFile->inlib_offset = offset;
    CurrFile->inlib = FindInLib( io );
    ObjWalkSymList( file, CurrFile, AddSym );
    CloseLibFile( file );
}

void OmfMKImport( arch_header *arch, long ordinal, char *dll_name,
        char *sym_name, char *exportedName, importType type )
{
    Options.omf_found = 1;
    if( Options.elf_found == 1 ) {
        FatalError( ERR_MIXED_OBJ, "ELF", "OMF" );
    }
    NewSymFile( arch );
    CurrFile->import = MemAllocGlobal( sizeof( import_sym ) );
    CurrFile->import->DLLName = DupStrGlobal( dll_name );
    CurrFile->import->ordinal = ordinal;
    if( sym_name != NULL ) {
        CurrFile->import->symName = DupStrGlobal( sym_name );
    }else{
        CurrFile->import->symName = NULL;
    }
    if( exportedName != NULL ) {
        CurrFile->import->exportedName = DupStrGlobal( exportedName );
    }else{
        CurrFile->import->exportedName = NULL;
    }
    CurrFile->import->type = type;
    CurrFile->import->processor = WL_PROC_X86;  // set default JBS 99/12/21
}

void CoffMKImport( arch_header *arch, importType type,
        long ordinal, char *DLLname, char *symName, char *exportedName,
        long processor )
{

    NewSymFile( arch );
    Options.coff_found = TRUE;
    if( Options.elf_found == 1 ) {
        FatalError( ERR_MIXED_OBJ, "ELF", "COFF" );
    }

    CurrFile->import = MemAllocGlobal( sizeof( import_sym ) );
    CurrFile->import->type = type;
    CurrFile->import->ordinal = ordinal;
    CurrFile->import->DLLName = DupStrGlobal( DLLname );
    if( symName != NULL ) {
        CurrFile->import->symName = DupStrGlobal( symName );
    }else{
        CurrFile->import->symName = NULL;
    }
    if( exportedName != NULL ) {
        CurrFile->import->exportedName = DupStrGlobal( exportedName );
    }else{
        CurrFile->import->exportedName = NULL;
    }
    CurrFile->import->processor = processor;
    CurrFile->arch.size = CoffImportSize( type, DLLname, symName, exportedName, processor );
}

void ElfMKImport( arch_header *arch, importType type, long export_size,
        char *DLLname, char *strings, Elf32_Export *export_table,
        Elf32_Sym *sym_table, long processor )
{
    int                 i;
    elf_import_sym      **temp;

    NewSymFile( arch );
    Options.elf_found = 1;
    if( Options.coff_found == 1 ) {
        FatalError( ERR_MIXED_OBJ, "ELF", "COFF" );
    }
    if( Options.omf_found == 1 ) {
        FatalError( ERR_MIXED_OBJ, "ELF", "OMF" );
    }

    CurrFile->import = MemAllocGlobal( sizeof( import_sym ) );
    CurrFile->import->type = type;
    CurrFile->import->DLLName = DupStrGlobal( DLLname );
    CurrFile->import->numsyms = 0;
    temp = &(CurrFile->import->symlist);

    for( i=0; i<export_size; i++ ) {
        if( export_table[i].exp_symbol ) {
            *temp = MemAllocGlobal( sizeof( elf_import_sym ) );
            (*temp)->name = DupStrGlobal(&(strings[sym_table[export_table[i].
                exp_symbol].st_name]));
            (*temp)->len = strlen( (*temp)->name );
            (*temp)->ordinal = export_table[i].exp_ordinal;
            if( type == ELF ) {
                AddSym( (*temp)->name, SYM_STRONG, ELF_IMPORT_SYM_INFO );
            }

            CurrFile->import->numsyms ++;

            temp = &((*temp)->next);
        }
    }
    *temp = NULL;
    CurrFile->import->processor = processor;
    CurrFile->arch.size = ElfImportSize( CurrFile->import );
}

#define MAX_MESSAGE_LEN 511
static char             listMsg[MAX_MESSAGE_LEN + 1];
static unsigned         msgLength = 0;
static void listPrint( FILE *fp, char *str, ... )
{
    va_list             arglist;
    va_start( arglist, str );
    msgLength += _vbprintf( listMsg + msgLength, MAX_MESSAGE_LEN - msgLength, str, arglist );
    va_end( arglist );
}

static void listNewLine( FILE *fp )
{
    if( fp ) {
        fprintf( fp, listMsg );
        fputc( '\n', fp );
    } else {
        Message( listMsg );
    }
    msgLength = 0;
    listMsg[0] = ' ';
    listMsg[1] = '\0';
}

#define LINE_WIDTH 79
#define OFF_COLUMN 40

static void fpadch( FILE *fp, char ch, int len )
{
    if ( len <= 0 ) {
        return;
    }
    len = min( MAX_MESSAGE_LEN - msgLength, len );
    memset( listMsg + msgLength, ch, len );
    msgLength += len;
    listMsg[ msgLength ] = '\0';
}

static void printVerboseTableEntry( arch_header *arch )
{
    char        member_mode[11];
    char        date[128];
    time_t      t;

    member_mode[10] = '\0';
    member_mode[9] = ' ';
    if( arch->mode & S_IRUSR ) {
        member_mode[0] = 'r';
    } else {
        member_mode[0] = '-';
    }
    if( arch->mode & S_IWUSR ) {
        member_mode[1] = 'w';
    } else {
        member_mode[1] = '-';
    }
    if( !( arch->mode & S_IXUSR ) && (arch->mode & S_ISUID ) ) {
        member_mode[2] = 'S';
    } else if( ( arch->mode & S_IXUSR ) && ( arch->mode & S_ISUID ) ) {
        member_mode[2] = 's';
    } else if( arch->mode & S_IXUSR ) {
        member_mode[2] = 'x';
    } else {
        member_mode[2] = '-';
    }
    if( arch->mode & S_IRGRP ) {
        member_mode[3] = 'r';
    } else {
        member_mode[3] = '-';
    }
    if( arch->mode & S_IWGRP ) {
        member_mode[4] = 'w';
    } else {
        member_mode[4] = '-';
    }
    if( !( arch->mode & S_IXGRP ) && (arch->mode & S_ISGID ) ) {
        member_mode[5] = 'S';
    } else if( ( arch->mode & S_IXGRP ) && ( arch->mode & S_ISGID ) ) {
        member_mode[5] = 's';
    } else if( arch->mode & S_IXGRP ) {
        member_mode[5] = 'x';
    } else {
        member_mode[5] = '-';
    }
    if( arch->mode & S_IROTH ) {
        member_mode[6] = 'r';
    } else {
        member_mode[6] = '-';
    }
    if( arch->mode & S_IWOTH ) {
        member_mode[7] = 'w';
    } else {
        member_mode[7] = '-';
    }
    if( arch->mode & S_IXOTH ) {
        member_mode[8] = 'x';
    } else {
        member_mode[8] = '-';
    }
    t = (time_t) arch->date;
    strftime( date, 127, "%b %d %H:%M %Y", localtime( &t ) );
    Message( "%s %u/%u %u %s %s", member_mode, arch->uid, arch->gid, arch->size,
        date, MakeFName( arch->name ) );
}


extern void ListContents( void )
/****************************/
{
    sym_file    *file;
    lib_cmd     *cmd;

    if( Options.ar ) {
        if( CmdList ) {
            for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
                if( cmd->ops & OP_FOUND ) {
                    if( Options.verbose ) {
                        for( file = FileTable.first; file != NULL; file = file->next ) {
                            if( SameName( file->arch.name, cmd->name ) ) {
                                if (Options.terse_listing) {
                                    Message(file->arch.name);
                                } else {
                                    printVerboseTableEntry( &( file->arch ) );
                                }
                                break;
                            }
                        }
                    } else {
                        Message( MakeFName( cmd->name ) );
                    }
                }
            }
        } else {
            if (Options.terse_listing) {
                for( file = FileTable.first; file != NULL; file = file->next ) {
                    Message("%s", file->arch.name);
                }
            } else {
                for( file = FileTable.first; file != NULL; file = file->next ) {
                    printVerboseTableEntry( & ( file->arch ) );
                }
            }
        }
    } else {
        sym_entry       *sym;
        int             i;
        FILE            *fp;
        char            *name;
        unsigned        name_len;

        if (Options.terse_listing) {
            SortSymbols();
            for( i = 0; i < NumSymbols; ++i ) {
                sym = SortedSymbols[i];
                name = FormSym( sym->name );
                name_len = strlen( name );
                Message(name);
            }
            return;
        }

        if( Options.list_file == NULL ) {
            Options.list_file = DupStr( MakeListName() );
        }
        if( Options.list_file[0] != 0 ) {
            fp = fopen( Options.list_file, "w" );
            if( fp == NULL ) {
                FatalError( ERR_CANT_OPEN, Options.list_file, strerror( errno ) );
            }
        } else {
            fp = NULL;
        }
        SortSymbols();

        for( i = 0; i < NumSymbols; ++i ) {
            sym = SortedSymbols[i];
            name = FormSym( sym->name );
            name_len = strlen( name );
            listPrint( fp, "%s..", name );
            fpadch( fp, '.', LINE_WIDTH - 2 - name_len - sym->file->name_length );
            listPrint( fp, "%s", sym->file->arch.name );
            listNewLine( fp );
        }

        listNewLine( fp );

        for( file = FileTable.first; file != NULL; file = file->next ) {
            if( file->arch.ffname ) {
                listPrint( fp, "%s ", file->arch.ffname );
            } else {
                listPrint( fp, "%s ", file->arch.name );
            }
            fpadch( fp, ' ', OFF_COLUMN - 1 - file->name_length - 16 );
            listPrint( fp, "Offset=%8.8xH", file->inlib_offset );
            listNewLine( fp );
            for( sym = file->first; sym != NULL; sym = sym->next ) {
                listPrint( fp, "    %s", FormSym( sym->name ) );
                listNewLine( fp );
            }
            listNewLine( fp );
        }

        if( fp ) {
            fclose( fp );
        }
    }
}
