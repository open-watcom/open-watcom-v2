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
* Description:  Librarian symbol table processing.
*
****************************************************************************/


#include "wlib.h"
#include <errno.h>
#include "ar.h"
#include "convert.h"
#include "roundmac.h"

#include "clibext.h"


#define HASH_SIZE                   256
#define WLIB_LIST_MAX_MESSAGE_LEN   511
#define WLIB_LIST_LINE_WIDTH        79
#define WLIB_LIST_OFF_COLUMN        40

static char             listMsg[WLIB_LIST_MAX_MESSAGE_LEN + 1];
static size_t           msgLength = 0;

static sym_table        FileTable;
static sym_file         *CurrFile;
static sym_entry        **HashTable;
static sym_entry        **SortedSymbols;

static char             *padding_string;
static size_t           padding_string_len;

static unsigned long    NumFiles;
static size_t           NumSymbols;
static file_offset      TotalNameLength;
static file_offset      TotalFFNameLength;
static file_offset      TotalSymbolLength;

void InitFileTab( void )
/**********************/
{
    FileTable.first = NULL;
    FileTable.add_to = &FileTable.first;
    SortedSymbols = NULL;
    HashTable = MemAlloc( HASH_SIZE * sizeof( HashTable[0] ) );
    memset( HashTable, 0, HASH_SIZE * sizeof( HashTable[0] ) );
}

static void FiniSymFile( sym_file *sfile )
/****************************************/
{
    sym_entry           *sym;
    sym_entry           *next_sym;
    elf_import_sym      *elfimp;

    for( sym = sfile->first; sym != NULL; sym = next_sym ) {
        next_sym = sym->next;
        MemFree( sym );
    }
    MemFree( sfile->full_name );
    MemFree( sfile->arch.name );
    MemFree( sfile->arch.ffname );
    if( sfile->impsym != NULL ) {
        switch( sfile->impsym->type ) {
        case ELF:
        case ELFRENAMED:
            for( elfimp = sfile->impsym->u.elf.symlist; elfimp != NULL;
                         elfimp = sfile->impsym->u.elf.symlist ) {
                sfile->impsym->u.elf.symlist = elfimp->next;
                MemFree( (void *)elfimp->sym.name );
                MemFree( elfimp );
            }
            break;
        default:
            MemFree( sfile->impsym->u.omf_coff.symName );
            MemFree( sfile->impsym->u.omf_coff.exportedName );
            break;
        }
        MemFree( (void *)sfile->impsym->dllName.name );
        MemFree( sfile->impsym );
    }
    MemFree( sfile );
}


void CleanFileTab( void )
/***********************/
{
    sym_file    *last = NULL;
    sym_file    *curr;
    sym_file    *next;

    for( curr = FileTable.first; curr != NULL ; curr = next ) {
        next = curr->next;
        /*
         * If curr->first is NULL then either this file contains no
         * symbols or we are ignoring all of them.  Remove the file.
         */
        if( curr->first == NULL ) {
            if( last != NULL ) {
                last->next = curr->next;
            } else {
                FileTable.first = curr->next;
            }

            if( &(curr->next) == FileTable.add_to ) {
                if( last != NULL ) {
                    FileTable.add_to = &(last->next);
                } else {
                    FileTable.add_to = &(FileTable.first);
                }
            }

            FiniSymFile( curr );
        } else {
            last = curr;
        }
    }
}


void FiniFileTab( void )
/**********************/
{
    sym_file    *sfile;

    while( (sfile = FileTable.first) != NULL ) {
        FileTable.first = sfile->next;
        FiniSymFile( sfile );
    }
    FileTable.add_to = &FileTable.first;
    if( SortedSymbols != NULL ) {
        MemFree( SortedSymbols );
        SortedSymbols = NULL;
    }
    MemFree( HashTable );
    HashTable = NULL;
}


static int Hash( const char *string, unsigned *len )
/**************************************************/
{
    unsigned long       g;
    unsigned long       h;
    const char          *start;

    start = string;
    h = 0;
    while( *string != 0 ) {
        h = ( h << 4 ) + *string;
        if( (g = (h & 0xf0000000)) != 0 ) {
            h = h ^ ( g >> 24 );
            h = h ^ g;
        }
        ++string;
    }
    if( len != NULL ) {
        *len = (unsigned)( string - start );
    }
    return( h % HASH_SIZE );
}


static void RemoveFromHashTable( sym_entry *sym )
/***********************************************/
{
    sym_entry       *hash_sym;
    sym_entry       *hash_prev;
    int             hval;

    hash_prev = NULL;
    hval = Hash( sym->name, NULL );
    for( hash_sym = HashTable[hval]; hash_sym != NULL; hash_sym = hash_sym->hash_next ) {
        if( hash_sym == sym ) {
            if( hash_prev == NULL ) {
                HashTable[hval] = hash_sym->hash_next;
            } else {
                hash_prev->hash_next = hash_sym->hash_next;
            }
            break;
        } else {
            hash_prev = hash_sym;
        }
    }
}


static sym_file *NewSymFile( const arch_header *arch, file_type obj_type )
/************************************************************************/
{
    sym_file    *sfile;

    sfile = MemAlloc( sizeof( sym_file ) );
    sfile->obj_type = obj_type;
    sfile->first = NULL;
    sfile->next = NULL;
    sfile->impsym = NULL;
    sfile->inlib_offset = 0;
    sfile->full_name = MemStrdup( arch->name );
    sfile->arch.date = arch->date;
    sfile->arch.uid = arch->uid;
    sfile->arch.gid = arch->gid;
    sfile->arch.mode = arch->mode;
    sfile->arch.size = arch->size;
    sfile->arch.libtype = arch->libtype;
    if( Options.trim_path ) {
        sfile->arch.name = MemStrdup( TrimPath( arch->name ) );
    } else {
        sfile->arch.name = MemStrdup( arch->name );
    }
    sfile->name_length = strlen( sfile->arch.name );
    if( arch->ffname != NULL ) {
        sfile->arch.ffname = MemStrdup( arch->ffname );
        sfile->ffname_length = strlen( sfile->arch.ffname );
    } else {
        sfile->arch.ffname = NULL;
        sfile->ffname_length = 0;
    }
    *(FileTable.add_to) = sfile;
    FileTable.add_to = &sfile->next;
    return( sfile );
}



static int CompSyms( const void *ap, const void *bp )
/***************************************************/
{
    const sym_entry *a = *(const sym_entry **)ap;
    const sym_entry *b = *(const sym_entry **)bp;
    return( strcmp( a->name, b->name ) );
}

static void WriteFileHeader( libfile io, const arch_header *arch )
/****************************************************************/
{
    ar_header   ar;

    CreateARHeader( &ar, arch );
    LibWrite( io, &ar, AR_HEADER_SIZE );
}

static void WritePadding( libfile io, file_offset size )
/******************************************************/
{
    if( size & 1 ) {
        LibWrite( io, padding_string, padding_string_len );
    }
}

static void SortSymbols( void )
/*****************************/
{
    sym_file    *sfile;
    sym_entry   *sym;
    sym_entry   **sym_curr;
    size_t      i;
    size_t      name_length = 0;

    NumFiles = 0;
    NumSymbols = 0;
    TotalNameLength = 0;
    TotalFFNameLength = 0;
    TotalSymbolLength = 0;
    for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
        ++NumFiles;
        switch( Options.libtype ) {
        case WL_LTYPE_AR:
            /*
             * Always using "full" filename for AR
             */
            if( sfile->arch.ffname != NULL ) {
                name_length = sfile->ffname_length;
            } else {
                sfile->ffname_length = 0;
                name_length = sfile->name_length;
            }
            if( Options.ar_libformat == AR_FMT_BSD ) {
                /*
                 * BSD doesn't use special file name table
                 */
                sfile->name_offset = -1;
                name_length = 0;
            } else if( Options.ar_libformat == AR_FMT_GNU ) {
                if( name_length < AR_NAME_LEN ) {
                    sfile->name_offset = -1;
                    name_length = 0;
                } else {
                    sfile->name_offset = TotalNameLength;
                    name_length += AR_LONG_NAME_END_STRING_LEN;   // + "/\n"
                }
            } else {
                sfile->name_offset = TotalNameLength;
                name_length += 1;     // + "\n"
            }
            break;
        case WL_LTYPE_MLIB:
            /*
             * If no full filename, assume name is full, and trim
             * it to get non-full filename.
             */
            if( sfile->arch.ffname == NULL ) {
                sfile->arch.ffname = sfile->arch.name;
                sfile->ffname_length = strlen( sfile->arch.ffname );
                sfile->arch.name = MemStrdup( TrimPath( sfile->arch.ffname ) );
                sfile->name_length = strlen( sfile->arch.name );
            }
            name_length = sfile->name_length;
            sfile->name_offset = TotalNameLength;
            name_length += LIB_LONG_NAME_END_STRING_LEN;  // + "/\n"
            break;
        }
        TotalNameLength += name_length;
        TotalFFNameLength += sfile->ffname_length + 1;
        for( sym = sfile->first; sym != NULL; sym = sym->next ) {
            ++NumSymbols;
            TotalSymbolLength += sym->len + 1;
        }
    }

    if( NumSymbols == 0 ) {
        SortedSymbols = NULL;
        if( !Options.quiet ) {
            Warning( ERR_NO_SYMBOLS );
        }
    } else {
        SortedSymbols = MemAlloc( NumSymbols * sizeof( SortedSymbols[0] ) );
    }

    sym_curr = SortedSymbols;
    for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
        for( sym = sfile->first; sym != NULL; sym = sym->next ) {
            *sym_curr = sym;
            ++sym_curr;
        }
    }

    qsort( SortedSymbols, NumSymbols, sizeof( sym_entry * ), CompSyms );
    /*
     * re-hook symbols onto files in sorted order
     */
    for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
        sfile->first = NULL;
    }

    for( i = NumSymbols; i-- > 0; ) {
        sym = SortedSymbols[i];
        sym->next = sym->file->first;
        sym->file->first = sym;
    }
}

void WriteFileBody( libfile dst, sym_file *sfile )
/************************************************/
{
    libfile     src;

    if( sfile->impsym == NULL ) {
        if( sfile->inlib_offset == 0 ) {
            src = LibOpen( sfile->full_name, LIBOPEN_READ );
        } else {
            src = InLibHandle( sfile->inlib );
            LibSeek( src, sfile->inlib_offset, SEEK_SET );
        }
        if( sfile->obj_type == WL_FTYPE_OMF ) {
            OmfCopy( src, dst, sfile );
        } else {
            Copy( src, dst, sfile->arch.size );
        }
        if( sfile->inlib_offset == 0 ) {
            LibClose( src );
        }
    } else {
        switch( sfile->obj_type ) {
        case WL_FTYPE_ELF:
            ElfWriteImport( dst, sfile );
            break;
        case WL_FTYPE_COFF:
            CoffWriteImport( dst, sfile, Options.coff_import_long );
            break;
        case WL_FTYPE_OMF:
            OmfWriteImport( dst, sfile );
            break;
        }
    }
}

static void WriteOmfLibTrailer( libfile io )
/*******************************************
 * output OMF library end page
 *
 * struct {
 *     // OMF record header
 *     unsigned_8  type;
 *     unsigned_16 len;
 *     // OMF Library trailer
 *     ...
 *     zeros up to dictionary page size alignment (512 bytes)
 *     ...
 * }
 *
 * it doesn't use omfRec because it is OMF record without check sum
 * and it is filled by zeros up to full size (aligned to 512 bytes)
 */
{
    unsigned_16 len;

    len = DIC_REC_SIZE - ( (unsigned long)LibTell( io ) % DIC_REC_SIZE ) - OMFHDRLEN;
    /*
     * output OMF record header
     */
    WriteOmfRecHeader( io, LIB_TRAILER_REC, len );
    /*
     * output OMF Library trailer data
     */
    LibWriteNulls( io, len );
}

static void WriteOmfLibHeader( libfile io, unsigned_32 dict_offset, unsigned_16 dict_size )
/******************************************************************************************
 * output OMF library first page
 *
 * struct {
 *     // OMF record header
 *     unsigned_8  type;
 *     unsigned_16 page_size;  //really page size - 3
 *     // OMF Library header
 *     unsigned_32 dict_offset;
 *     unsigned_16 dict_size;
 *     unsigned_8  flags;
 *     ...
 *     zeros up to OMF library page size
 *     ...
 * }
 *
 * it doesn't use omfRec because it is OMF record without check sum
 * and it is used as overlay for already zeroed OMF library full page size
 */
{
    /*
     * output OMF record header
     */
    WriteOmfRecHeader( io, LIB_HEADER_REC, Options.page_size - OMFHDRLEN );
    /*
     * output OMF library header data without trailing zeros
     */
    LibWriteU32LE( io, dict_offset );
    LibWriteU16LE( io, dict_size );
    LibWriteU8( io, ( Options.respect_case ) ? 1 : 0 );
}

static unsigned_16 OptimalPageSize( void )
/****************************************/
{
    unsigned    i;
    sym_file    *sfile;
    file_offset offset;
    unsigned_16 page_size;

    page_size = 0;
    for( i = 4; i < 16; i++ ) {
        page_size = 1 << i;
        offset = page_size;
        for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
            if( offset / page_size > (unsigned long)USHRT_MAX )
                break;
            offset += __ROUND_UP_SIZE( sfile->arch.size, page_size );
        }
        if( sfile == NULL ) {
            break;
        }
    }
    return( page_size );
}

static void WriteOmfLibPadding( libfile io, bool force )
/******************************************************/
{
    size_t      padding_size;

    /*
     * page size is always a power of 2
     * therefor x % Options.page_size == x & ( Options.page_size - 1 )
     */
    padding_size = Options.page_size - (LibTell( io ) & ( Options.page_size - 1 ));
    if( padding_size != Options.page_size || force ) {
        LibWriteNulls( io, padding_size );
    }
}

static void WriteOmfFileTable( libfile io )
/*****************************************/
{
    sym_file    *sfile;
    unsigned    num_blocks;
    long        dict_offset;

    if( Options.page_size == 0 ) {
        Options.page_size = DEFAULT_PAGE_SIZE;
    } else if( Options.page_size == (unsigned_16)-1 ) {
        Options.page_size = OptimalPageSize();
    }
    /*
     * allocate and clean space for OMF Library Header
     */
    WriteOmfLibPadding( io, true );
    /*
     * write all modules to OMF Library
     */
    for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
        WriteOmfFile( io, sfile );
        WriteOmfLibPadding( io, false );
    }
    /*
     * write OMF Library Trailer
     */
    WriteOmfLibTrailer( io );
    /*
     * write OMF Library Dictionaries
     */
    dict_offset = LibTell( io );
    num_blocks = WriteOmfDict( io, FileTable.first );
    /*
     * rewind file to beggining and write OMF Library Header
     */
    LibSeek( io, 0, SEEK_SET );
    WriteOmfLibHeader( io, dict_offset, num_blocks );
}

static void WriteArMlibFileTable( libfile io )
/********************************************/
{
    arch_header     arch;
    sym_file        *sfile;
    sym_entry       *sym;
    file_offset     dict1_size = 0;
    file_offset     dict2_size = 0;
    file_offset     header_size = 0;
    size_t          i;
    time_t          currenttime = time( NULL );
    file_offset     obj_offset;
    int             index;
    bool            isBSD;


    SortSymbols();
    /*
     * figure out this dictionary sizes
     */
    switch( Options.libtype ) {
    case WL_LTYPE_AR:
        dict1_size = ( NumSymbols + 1 ) * sizeof( unsigned_32 ) + __ROUND_UP_SIZE_EVEN( TotalSymbolLength );

        header_size = AR_IDENT_LEN + AR_HEADER_SIZE + dict1_size;

        switch( Options.ar_libformat ) {
        case AR_FMT_BSD:
            dict2_size = 0;

            padding_string     = "\0";
            padding_string_len = 1;
            break;
        case AR_FMT_GNU:
            dict2_size = 0;

            if( TotalNameLength > 0 ) {
                header_size += AR_HEADER_SIZE + __ROUND_UP_SIZE_EVEN( TotalNameLength );
            }

            padding_string     = "\0";
            padding_string_len = 1;
            break;
        default:
            dict2_size = ( NumFiles + 1 ) * sizeof( unsigned_32 )
                        + sizeof( unsigned_32 ) + NumSymbols * sizeof( unsigned_16 )
                        + __ROUND_UP_SIZE_EVEN( TotalSymbolLength );

            header_size += AR_HEADER_SIZE + dict2_size;

            if( TotalNameLength > 0 ) {
                header_size += AR_HEADER_SIZE + __ROUND_UP_SIZE_EVEN( TotalNameLength );
            }

            padding_string     = AR_FILE_PADDING_STRING;
            padding_string_len = AR_FILE_PADDING_STRING_LEN;
            break;
        }
        break;
    case WL_LTYPE_MLIB:
        dict1_size = 0;

        dict2_size = ( NumSymbols + 1 ) * sizeof( unsigned_32 ) + NumSymbols
                    + TotalSymbolLength;

        header_size = LIBMAG_LEN + LIB_CLASS_LEN + LIB_DATA_LEN
                    + LIB_HEADER_SIZE + __ROUND_UP_SIZE_EVEN( dict2_size )
                    + LIB_HEADER_SIZE + __ROUND_UP_SIZE_EVEN( TotalNameLength )
                    + LIB_HEADER_SIZE + __ROUND_UP_SIZE_EVEN( TotalFFNameLength );

        padding_string     = LIB_FILE_PADDING_STRING;
        padding_string_len = LIB_FILE_PADDING_STRING_LEN;
        break;
    }
    /*
     * calculate the object files offsets
     */
    index = 0;
    obj_offset = 0;
    isBSD = ( ( Options.libtype == WL_LTYPE_AR ) && ( Options.ar_libformat == AR_FMT_BSD ) );
    for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
        sfile->u.new_offset = obj_offset + header_size;
        sfile->index = ++index;
        if( isBSD
          && ( sfile->name_length > AR_NAME_LEN
          || strchr( sfile->arch.name, ' ' ) != NULL ) ) {
            obj_offset += __ROUND_UP_SIZE_EVEN( sfile->arch.size + sfile->name_length ) + AR_HEADER_SIZE;
        } else {
            obj_offset += __ROUND_UP_SIZE_EVEN( sfile->arch.size ) + AR_HEADER_SIZE;
        }
    }

    switch( Options.libtype ) {
    case WL_LTYPE_AR:
        LibWrite( io, AR_IDENT, AR_IDENT_LEN );
        break;
    case WL_LTYPE_MLIB:
        LibWrite( io, LIBMAG, LIBMAG_LEN );
        LibWrite( io, LIB_CLASS_DATA_SHOULDBE, LIB_CLASS_LEN + LIB_DATA_LEN );
        break;
    }
    /*
     * write the useless dictionary
     */
    arch.date = currenttime;
    arch.uid = 0;
    arch.gid = 0;
    arch.mode = 0;
    if( dict1_size > 0 ) {
        arch.size = dict1_size;     // word round size
        arch.name = "/";
        WriteFileHeader( io, &arch );

        LibWriteU32BE( io, NumSymbols );
        for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
            for( sym = sfile->first; sym != NULL; sym = sym->next ) {
                LibWriteU32BE( io, sym->file->u.new_offset );
            }
        }
        for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
            for( sym = sfile->first; sym != NULL; sym = sym->next ) {
                LibWrite( io, sym->name, sym->len + 1 );
            }
        }
        WritePadding( io, TotalSymbolLength );
    }
    /*
     * write the useful dictionary
     */
    if( dict2_size > 0 ) {
        arch.size = dict2_size;     // word round size
        arch.name = "/";
        WriteFileHeader( io, &arch );

        if( Options.libtype == WL_LTYPE_AR ) {
            LibWriteU32LE( io, NumFiles );
            for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
                LibWriteU32LE( io, sfile->u.new_offset );
            }
        }

        LibWriteU32LE( io, NumSymbols );
        switch( Options.libtype ) {
        case WL_LTYPE_AR:
            for( i = 0; i < NumSymbols; ++i ) {
                LibWriteU16LE( io, SortedSymbols[i]->file->index );
            }
            break;
        case WL_LTYPE_MLIB:
            for( i = 0; i < NumSymbols; ++i ) {
                LibWriteU32LE( io, SortedSymbols[i]->file->index );
            }
            for( i = 0; i < NumSymbols; ++i ) {
                LibWriteU8( io, SortedSymbols[i]->info );
            }
            break;
        }
        for( i = 0; i < NumSymbols; ++i ) {
            LibWrite( io, SortedSymbols[i]->name, SortedSymbols[i]->len + 1 );
        }
        switch( Options.libtype ) {
        case WL_LTYPE_AR:
            WritePadding( io, TotalSymbolLength );
            break;
        case WL_LTYPE_MLIB:
            WritePadding( io, dict2_size );
            break;
        }
    }
    /*
     * write the string table
     */
    if( TotalNameLength > 0 ) {
        char    *stringpad;
        size_t  stringpadlen;

        if( Options.libtype == WL_LTYPE_MLIB ) {
            stringpad    = LIB_LONG_NAME_END_STRING;
            stringpadlen = LIB_LONG_NAME_END_STRING_LEN;
        } else if( Options.ar_libformat == AR_FMT_GNU ) {
            stringpad    = AR_LONG_NAME_END_STRING;
            stringpadlen = AR_LONG_NAME_END_STRING_LEN;
        } else if( Options.ar_libformat == AR_FMT_BSD ) {
            stringpad    = NULL;
            stringpadlen = 0;
        } else {
            stringpad    = "\0";
            stringpadlen = 1;
        }
        arch.size = TotalNameLength;        // real size
        arch.name = "//";
        WriteFileHeader( io, &arch );
        for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
            if( sfile->name_offset == -1 )
                continue;
            /*
             * Always write the "full" filename for AR
             */
            if( Options.libtype == WL_LTYPE_AR
              && sfile->arch.ffname != NULL ) {
                LibWrite( io, sfile->arch.ffname, sfile->ffname_length );
            } else {
                LibWrite( io, sfile->arch.name, sfile->name_length );
            }
            LibWrite( io, stringpad, stringpadlen );
        }
        WritePadding( io, TotalNameLength );
    }
    /*
     * write the full filename table
     */
    if( Options.libtype == WL_LTYPE_MLIB ) {
        arch.size = TotalFFNameLength;      // real size
        arch.name = "///";
        WriteFileHeader( io, &arch );
        for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
            LibWrite( io, sfile->arch.ffname, sfile->ffname_length + 1 );
        }
        WritePadding( io, TotalFFNameLength );
    }

    for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
        char        buff[AR_NAME_LEN + 1];
        bool        append_name;

        append_name = false;
        arch = sfile->arch;
        if( sfile->name_offset == -1 ) {
            if( Options.ar_libformat == AR_FMT_BSD ) {
                /*
                 * BSD append file name after header and before file image if it is longer then
                 *  max.length or it contains space
                 */
                if( sfile->name_length > AR_NAME_LEN
                  || strchr( sfile->arch.name, ' ' ) != NULL ) {
                    append_name = true;
                    arch.size += sfile->name_length;
                    sprintf( buff, AR_NAME_CONTINUED_AFTER "%lu", (unsigned long)sfile->name_length );
                    arch.name = buff;
                } else {
                    arch.name = sfile->arch.name;
                }
            } else {        // COFF, GNU
                strcpy( buff, sfile->arch.name );
                buff[sfile->name_length] = AR_NAME_END_CHAR;
                buff[sfile->name_length + 1] = '\0';
                arch.name = buff;
            }
        } else {
            sprintf( buff, "/%ld", sfile->name_offset );
            arch.name = buff;
        }
        WriteFileHeader( io, &arch );
        if( append_name ) {
            LibWrite( io, sfile->arch.name, sfile->name_length );
        }
        WriteFileBody( io, sfile );
        WritePadding( io, arch.size );
    }
}

void WriteFileTable( libfile io )
/*******************************/
{
    if( Options.libtype == WL_LTYPE_NONE
      && Options.omf_found ) {
        if( Options.coff_found ) {
            Options.libtype = WL_LTYPE_AR;
        } else {
            Options.libtype = WL_LTYPE_OMF;
        }
    }
    if( Options.coff_found
      && (Options.libtype == WL_LTYPE_NONE
      || Options.libtype == WL_LTYPE_OMF) ) {
        Options.libtype = WL_LTYPE_AR;
    }
    if( Options.elf_found
      && (Options.libtype == WL_LTYPE_NONE
      || Options.libtype == WL_LTYPE_OMF) ) {
        Options.libtype = WL_LTYPE_AR;
    }
    if( Options.libtype == WL_LTYPE_AR
      || Options.libtype == WL_LTYPE_MLIB ) {
        WriteArMlibFileTable( io );
    } else {
        WriteOmfFileTable( io );
    }
}

void AddSym( const char *name, symbol_strength strength, unsigned char info )
/***************************************************************************/
{
    sym_entry   *hash_sym;
    sym_entry   **owner;
    int         hval;
    unsigned    namelen;

    hval = Hash( name, &namelen );
    for( hash_sym = HashTable[hval]; hash_sym != NULL; hash_sym = hash_sym->hash_next ) {
        if( hash_sym->len != namelen )
            continue;
        if( SymbolNameCmp( hash_sym->name, name ) == 0 ) {
            if( strength > hash_sym->strength ) {
                owner = &hash_sym->file->first;
                while( *owner != hash_sym ) {
                    owner = &(*owner)->next;
                }
                *owner = hash_sym->next;
                owner = HashTable + hval;
                while( *owner != hash_sym ) {
                    owner = &(*owner)->hash_next;
                }
                *owner = hash_sym->hash_next;
                MemFree( hash_sym );
                break; //db
            } else if( strength == hash_sym->strength ) {
                if( strength == SYM_STRONG ) {
                    if( !Options.quiet ) {
                        Warning( ERR_DUPLICATE_SYMBOL, FormSym( name ) );
                    }
                }
            }
            return;
        }
    }
    hash_sym = MemAlloc( sizeof( sym_entry ) + namelen );
    hash_sym->len = namelen;
    hash_sym->strength = strength;
    hash_sym->info = info;
    strcpy( hash_sym->name, name );
    hash_sym->next = CurrFile->first;
    CurrFile->first = hash_sym;
    hash_sym->file = CurrFile;
    hash_sym->hash_next = HashTable[hval];
    HashTable[hval] = hash_sym;
}

#ifdef DEVBUILD
void DumpFileTable( void )
{
    sym_file    *sfile;
    sym_entry   *entry;
    sym_entry   *hash_sym;
    int         hval;
    long        files    = 0L;
    long        symbols  = 0L;

    printf("----------------------------------------------------------\n");
    printf("File Table Dump\n");
    printf("----------------------------------------------------------\n");

    for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
        ++files;
        printf( "File: \"%s\"\n", sfile->full_name );
        for( entry = sfile->first; entry != NULL; entry = entry->next ) {
            ++symbols;

            hval = Hash( entry->name, NULL );
            printf( "\t\"%s\" (%d, %u, \"%s\")", entry->name, hval, (unsigned)( entry->len ),
                    (HashTable[hval] ? HashTable[hval]->name : "(NULL)") );

            for( hash_sym = entry->hash_next; hash_sym != NULL; hash_sym = hash_sym->hash_next ) {
                printf( " -> \"%s\"", hash_sym->name );
                fflush( stdout );
            }
            printf( "\n" );
        }
    }
    printf( "----------------------------------------------------------\n" );
    printf( "Files         : %ld\n", files );
    printf( "Symbols       : %ld\n", symbols );
    printf( "----------------------------------------------------------\n" );
}

void DumpHashTable( void )
{
    sym_entry   *hash_sym;
    int         hval;
    int         length;

    printf( "----------------------------------------------------------\n" );
    printf( "Hash Table Dump\n" );
    printf( "----------------------------------------------------------\n" );
    for( hval = 0; hval < HASH_SIZE; hval++ ) {
        length = 0;
        for( hash_sym = HashTable[hval]; hash_sym != NULL; hash_sym = hash_sym->next ) {
            ++length;
        }
        printf( "Offset %6d: %d\n", hval, length );
    }
    printf( "----------------------------------------------------------\n" );
}
#endif /* DEVBUILD */


bool RemoveObjectSymbols( const arch_header *arch )
/*************************************************/
{
    sym_file    *sfile;
    sym_file    *sfile_prev;
    sym_entry   *sym;

    sfile_prev = NULL;
    for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
        if( SymbolNameCmp( sfile->full_name, arch->name ) == 0 ) {
            if( sfile_prev != NULL ) {    /* Not deleting from head of list */
                sfile_prev->next = sfile->next;

                if( FileTable.add_to == &sfile->next ) { /* Last node in list */
                    FileTable.add_to = &sfile_prev->next;
                }
            } else {
                if( FileTable.add_to == &sfile->next ) { /* Only node in list */
                    FileTable.add_to = &FileTable.first;
                    FileTable.first = NULL;
                } else {  /* First node in the list */
                    FileTable.first = sfile->next;
                }
            }
            /*
             * remove all module symbols
             */
            for( sym = sfile->first; sym != NULL; sym = sym->next ) {
                RemoveFromHashTable( sym );
            }

            FiniSymFile( sfile );
            Options.modified = true;
            return( true );
        }
        sfile_prev = sfile;
    }
    return( false );
}

void AddObjectSymbols( libfile io, long offset, const arch_header *arch )
/***********************************************************************/
{
    obj_file    *ofile;
    file_type   obj_type;

    ofile = OpenORLLibFile( io, arch->name );
    if( ofile->orl != NULL ) {
        if( ORLFileGetFormat( ofile->orl ) == ORL_COFF ) {
            if( Options.libtype == WL_LTYPE_MLIB ) {
                FatalError( ERR_NOT_LIB, ctext_WL_FTYPE_COFF, ctext_WL_LTYPE_MLIB );
            }
            Options.coff_found = true;
            obj_type = WL_FTYPE_COFF;
        } else {
            if( Options.omf_found ) {
                FatalError( ERR_MIXED_OBJ, ctext_WL_FTYPE_ELF, ctext_WL_FTYPE_OMF );
            }
            Options.elf_found = true;
            obj_type = WL_FTYPE_ELF;
        }
    } else {
        if( Options.libtype == WL_LTYPE_MLIB ) {
            FatalError( ERR_NOT_LIB, ctext_WL_FTYPE_OMF, ctext_WL_LTYPE_MLIB );
        }
        if( Options.elf_found ) {
            FatalError( ERR_MIXED_OBJ, ctext_WL_FTYPE_ELF, ctext_WL_FTYPE_OMF );
        }
        Options.omf_found = true;
        obj_type = WL_FTYPE_OMF;
    }
    CurrFile = NewSymFile( arch, obj_type );
    CurrFile->inlib_offset = offset;
    CurrFile->inlib = FindInLib( io );
    ObjWalkSymList( ofile, CurrFile );
    CloseORLLibFile( ofile );
    MemFree( ofile );
}

void OmfMKImport( const arch_header *arch, importType type,
                  long ordinal, name_len *dllName, const char *symName,
                  const char *exportedName, processor_type processor )
{
    import_sym  *impsym;

    if( Options.elf_found ) {
        FatalError( ERR_MIXED_OBJ, ctext_WL_FTYPE_ELF, ctext_WL_FTYPE_OMF );
    }
    Options.omf_found = true;
    CurrFile = NewSymFile( arch, WL_FTYPE_OMF );
    impsym = MemAlloc( sizeof( import_sym ) );
    impsym->dllName.name = MemStrdup( dllName->name );
    impsym->dllName.len = dllName->len;
    impsym->u.omf_coff.ordinal = ordinal;
    impsym->u.omf_coff.symName = MemStrdup( symName );
    impsym->u.omf_coff.exportedName = MemStrdup( exportedName );
    impsym->type = type;
    impsym->processor = processor;
    CurrFile->impsym = impsym;
    CurrFile->arch.size = OmfImportSize( impsym );
    AddSym( symName, SYM_STRONG, 0 );
}

void CoffMKImport( const arch_header *arch, importType type,
                   long ordinal, name_len *dllName, const char *symName,
                   const char *exportedName, processor_type processor )
{
    import_sym  *impsym;

    if( Options.elf_found ) {
        FatalError( ERR_MIXED_OBJ, ctext_WL_FTYPE_ELF, ctext_WL_FTYPE_COFF );
    }
    Options.coff_found = true;
    CurrFile = NewSymFile( arch, WL_FTYPE_COFF );
    impsym = MemAlloc( sizeof( import_sym ) );
    impsym->type = type;
    impsym->u.omf_coff.ordinal = ordinal;
    impsym->dllName.name = MemStrdup( dllName->name );
    impsym->dllName.len = dllName->len;
    impsym->u.omf_coff.symName = MemStrdup( symName );
    impsym->u.omf_coff.exportedName = MemStrdup( exportedName );
    impsym->processor = processor;
    CurrFile->impsym = impsym;
    CurrFile->arch.size = CoffImportSize( impsym );
    switch( type ) {
    case IMPORT_DESCRIPTOR:
    case NULL_IMPORT_DESCRIPTOR:
    case NULL_THUNK_DATA:
        AddSym( symName, SYM_WEAK, 0 );
        break;
    default:
        AddSym( symName, SYM_STRONG, 0 );
        break;
    }
}

void ElfMKImport( const arch_header *arch, importType type, long export_size,
                  name_len *dllName, const char *strings, Elf32_Export *export_table,
                  Elf32_Sym *sym_table, processor_type processor )
{
    int             i;
    elf_import_sym  **pelfimp;
    elf_import_sym  *elfimp;
    import_sym      *impsym;

    if( Options.coff_found ) {
        FatalError( ERR_MIXED_OBJ, ctext_WL_FTYPE_ELF, ctext_WL_FTYPE_COFF );
    }
    if( Options.omf_found ) {
        FatalError( ERR_MIXED_OBJ, ctext_WL_FTYPE_ELF, ctext_WL_FTYPE_OMF );
    }
    Options.elf_found = true;
    CurrFile = NewSymFile( arch, WL_FTYPE_ELF );
    impsym = MemAlloc( sizeof( import_sym ) );
    impsym->type = type;
    impsym->dllName.name = MemStrdup( dllName->name );
    impsym->dllName.len = dllName->len;
    impsym->u.elf.numsyms = 0;
    impsym->processor = processor;
    CurrFile->impsym = impsym;

    pelfimp = &(impsym->u.elf.symlist);
    for( i = 0; i < export_size; i++ ) {
        if( export_table[i].exp_symbol ) {
            elfimp = MemAlloc( sizeof( elf_import_sym ) );
            elfimp->sym.name = MemStrdup( strings + sym_table[export_table[i].exp_symbol].st_name );
            elfimp->sym.len = strlen( elfimp->sym.name );
            elfimp->ordinal = export_table[i].exp_ordinal;
            if( type == ELF ) {
                AddSym( elfimp->sym.name, SYM_STRONG, ELF_IMPORT_SYM_INFO );
            }

            impsym->u.elf.numsyms++;

            *pelfimp = elfimp;
            pelfimp = &(elfimp->next);
        }
    }
    *pelfimp = NULL;

    CurrFile->arch.size = ElfImportSize( impsym );
}

static void printVerboseTableEntryAr( sym_file *sfile )
{
    char        member_mode[11];
    char        date[128];
    time_t      t;

    member_mode[10] = '\0';
    member_mode[9] = ' ';
    if( sfile->arch.mode & AR_S_IRUSR ) {
        member_mode[0] = 'r';
    } else {
        member_mode[0] = '-';
    }
    if( sfile->arch.mode & AR_S_IWUSR ) {
        member_mode[1] = 'w';
    } else {
        member_mode[1] = '-';
    }
    if( (sfile->arch.mode & AR_S_IXUSR) == 0
      && (sfile->arch.mode & AR_S_ISUID) ) {
        member_mode[2] = 'S';
    } else if( (sfile->arch.mode & AR_S_IXUSR)
      && (sfile->arch.mode & AR_S_ISUID) ) {
        member_mode[2] = 's';
    } else if( sfile->arch.mode & AR_S_IXUSR ) {
        member_mode[2] = 'x';
    } else {
        member_mode[2] = '-';
    }
    if( sfile->arch.mode & AR_S_IRGRP ) {
        member_mode[3] = 'r';
    } else {
        member_mode[3] = '-';
    }
    if( sfile->arch.mode & AR_S_IWGRP ) {
        member_mode[4] = 'w';
    } else {
        member_mode[4] = '-';
    }
    if( (sfile->arch.mode & AR_S_IXGRP) == 0
      && (sfile->arch.mode & AR_S_ISGID) ) {
        member_mode[5] = 'S';
    } else if( (sfile->arch.mode & AR_S_IXGRP)
      && (sfile->arch.mode & AR_S_ISGID) ) {
        member_mode[5] = 's';
    } else if( sfile->arch.mode & AR_S_IXGRP ) {
        member_mode[5] = 'x';
    } else {
        member_mode[5] = '-';
    }
    if( sfile->arch.mode & AR_S_IROTH ) {
        member_mode[6] = 'r';
    } else {
        member_mode[6] = '-';
    }
    if( sfile->arch.mode & AR_S_IWOTH ) {
        member_mode[7] = 'w';
    } else {
        member_mode[7] = '-';
    }
    if( sfile->arch.mode & AR_S_IXOTH ) {
        member_mode[8] = 'x';
    } else {
        member_mode[8] = '-';
    }
    t = (time_t) sfile->arch.date;
    strftime( date, 127, "%b %d %H:%M %Y", localtime( &t ) );
    Message( "%s %u/%u %u %s %s", member_mode, sfile->arch.uid, sfile->arch.gid, sfile->arch.size,
        date, MakeFName( sfile->arch.name ) );
}


static void ListContentsAr( void )
/********************************/
{
    sym_file    *sfile;
    lib_cmd     *cmd;

    if( CmdList != NULL ) {
        for( cmd = CmdList; cmd != NULL; cmd = cmd->next ) {
            if( cmd->ops & OP_FOUND ) {
                if( Options.verbose ) {
                    for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
                        if( IsSameModuleCase( sfile->arch.name, cmd->name, ( sfile->obj_type == WL_FTYPE_OMF ) ) ) {
                            if( Options.terse_listing ) {
                                Message( sfile->arch.name );
                            } else {
                                printVerboseTableEntryAr( sfile );
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
        if( Options.verbose ) {
            for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
                printVerboseTableEntryAr( sfile );
            }
        } else {
            for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
                Message( "%s", MakeFName( sfile->arch.name ) );
            }
        }
    }
}


static void listPrintWlib( FILE *fp, const char *str, ... )
{
    va_list             arglist;

    /* unused parameters */ (void)fp;

    va_start( arglist, str );
    msgLength += vsnprintf( listMsg + msgLength, WLIB_LIST_MAX_MESSAGE_LEN - msgLength, str, arglist );
    va_end( arglist );
}


static void listNewLineWlib( FILE *fp )
{
    if( fp != NULL ) {
        fprintf( fp, "%s\n", listMsg );
    } else {
        if( !Options.quiet ) {
            Message( listMsg );
        }
    }
    msgLength = 0;
    listMsg[0] = ' ';
    listMsg[1] = '\0';
}


static void fpadchWlib( FILE *fp, char ch, size_t len )
{
    /* unused parameters */ (void)fp;

    if( len > 0 ) {
        if( len > WLIB_LIST_MAX_MESSAGE_LEN - msgLength )
            len = WLIB_LIST_MAX_MESSAGE_LEN - msgLength;
        memset( listMsg + msgLength, ch, len );
        msgLength += len;
        listMsg[msgLength] = '\0';
    }
}

static void ListContentsWlib( void )
/**********************************/
{
    sym_file    *sfile;
    sym_entry   *sym;
    int         i;
    FILE        *fp;
    char        *name;
    size_t      namelen;

    if( Options.terse_listing ) {
        SortSymbols();
        for( i = 0; i < NumSymbols; ++i ) {
            sym = SortedSymbols[i];
            name = FormSym( sym->name );
            namelen = strlen( name );
            if( !Options.quiet ) {
                Message( name );
            }
        }
        return;
    }

    if( Options.list_file == NULL ) {
        Options.list_file = MemStrdup( MakeListName() );
    }
    if( Options.list_file[0] != '\0' ) {
        if( Options.list_file[0] == '.' && Options.list_file[1] == '\0' ) {
            fp = stdout;
        } else {
            fp = fopen( Options.list_file, "w" );
            if( fp == NULL ) {
                FatalError( ERR_CANT_OPEN, Options.list_file, strerror( errno ) );
            }
        }
    } else {
        fp = NULL;
    }
    SortSymbols();

    for( i = 0; i < NumSymbols; ++i ) {
        sym = SortedSymbols[i];
        name = FormSym( sym->name );
        namelen = strlen( name );
        listPrintWlib( fp, "%s..", name );
        if( WLIB_LIST_LINE_WIDTH > 2 + namelen + sym->file->name_length ) {
            fpadchWlib( fp, '.', WLIB_LIST_LINE_WIDTH - 2 - namelen - sym->file->name_length );
        }
        listPrintWlib( fp, "%s", sym->file->arch.name );
        listNewLineWlib( fp );
    }

    listNewLineWlib( fp );

    for( sfile = FileTable.first; sfile != NULL; sfile = sfile->next ) {
        if( sfile->arch.ffname != NULL ) {
            listPrintWlib( fp, "%s ", sfile->arch.ffname );
        } else {
            listPrintWlib( fp, "%s ", sfile->arch.name );
        }
        if( WLIB_LIST_OFF_COLUMN > 1 - sfile->name_length - 16 ) {
            fpadchWlib( fp, ' ', WLIB_LIST_OFF_COLUMN - 1 - sfile->name_length - 16 );
        }
        listPrintWlib( fp, "Offset=%8.8xH", sfile->inlib_offset );
        listNewLineWlib( fp );
        for( sym = sfile->first; sym != NULL; sym = sym->next ) {
            listPrintWlib( fp, "    %s", FormSym( sym->name ) );
            listNewLineWlib( fp );
        }
        listNewLineWlib( fp );
    }

    if( fp != NULL ) {
        if( fp == stdout ) {
            fflush( fp );
        } else {
            fclose( fp );
        }
    }
}


void ListContents( void )
/***********************/
{
    if( Options.ar ) {
        ListContentsAr();
    } else {
        ListContentsWlib();
    }
}
