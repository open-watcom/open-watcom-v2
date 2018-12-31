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
* Description:  Routines for creating Novell NetWare load files.
*
****************************************************************************/


#include <string.h>
#include <time.h>
#include "linkstd.h"
#include "alloc.h"
#include "msg.h"
#include "exenov.h"
#include "dbginfo.h"
#include "reloc.h"
#include "wlnkmsg.h"
#include "virtmem.h"
#include "fileio.h"
#include "dbgall.h"
#include "impexp.h"
#include "loadfile.h"
#include "loadnov.h"
#include "machtype.h"
#include "wdbginfo.h"

#include "clibext.h"


#define DUMMY_THREAD_NAME " LONG"

static unsigned_32  DbgInfoCount;
static unsigned_32  DbgInfoLen;
static virt_mem     NovDbgInfo;
static virt_mem     CurrDbgLoc;

void ResetLoadNov( void )
/***********************/
{
    DbgInfoCount = 0;
    DbgInfoLen = 0;
}

static unsigned_32 WriteNovRelocs( fixed_header *header )
/*******************************************************/
// write the relocations.
{
    DumpRelocList( Root->reloclist );
    header->numberOfRelocationFixups = Root->relocs;
    return( (unsigned_32)Root->relocs * sizeof( nov_reloc_item ) );
}

static size_t create_sym_extname( symbol *sym, char *ext_name )
/*************************************************************/
{
    size_t  len;
    size_t  len1;

    /*
    //    netware prefix support
    */
    *ext_name = '\0';
    if( sym->prefix != NULL ) {
        strcpy( ext_name, sym->prefix );    // len < 255
        strcat( ext_name, "@" );
    }
    len = strlen( sym->name.u.ptr );
    len1 = strlen( ext_name );
    if( len + len1 > 255 )
        len = 255 - len1;
    memcpy( ext_name + len1, sym->name.u.ptr, len );
    len += len1;
    ext_name[len] = '\0';
    return( len );
}

static unsigned_32 WriteNovImports( fixed_header *header )
/********************************************************/
{
    nov_import      *import;
    unsigned_32     count;
    size_t          len;
    unsigned_32     wrote;
    unsigned_32     refs;
    virt_mem        *vmem_array;
    symbol          *sym;
    unsigned_32     size;

    wrote = count = 0;
    for( sym = HeadSym; sym != NULL; sym = sym->link ) {
        if( !IS_SYM_IMPORTED( sym ) )
            continue;
        /* so SymFini doesn't try to free it */
        if( sym->p.import == DUMMY_IMPORT_PTR )
            sym->p.import = NULL;
        import = sym->p.import;
        if( import != NULL ) {
            char    ext_name[255 + 1];

            len = create_sym_extname( sym, ext_name );
            wrote += WriteLoadU8Name( ext_name, len, false );

            if( import->contents <= MAX_IMP_INTERNAL ) {
                refs = import->contents;
            } else {        // imports are in virtual memory.
                refs = import->u.v.num_relocs;
            }
            WriteLoadU32( refs );
            size = refs * sizeof( refs );
            wrote += sizeof( unsigned_32 ) + size;
            if( import->contents <= MAX_IMP_INTERNAL ) {
                WriteLoad( import->u.r.relocs, size );
            } else {        // imports are in virtual memory.
                vmem_array = import->u.v.vm_ptr;
                for( ; size > IMP_VIRT_ALLOC_SIZE; size -= IMP_VIRT_ALLOC_SIZE ) {
                    WriteInfoLoad( *vmem_array++, IMP_VIRT_ALLOC_SIZE );
                }
                WriteInfoLoad( *vmem_array, size );
            }
            count++;
        }
    }
    header->numberOfExternalReferences = count;
    return( wrote );
}

static unsigned_32 WriteNovExports( fixed_header *header )
/********************************************************/
{
    obj_name_list   *export;
    symbol          *sym;
    unsigned_32     count;
    unsigned_32     wrote;
    unsigned_32     off;
    size_t          len;

    count = wrote = 0;
    for( export = FmtData.u.nov.exp.export; export != NULL; export = export->next ) {
        sym = SymOp( ST_FIND, export->name.u.ptr, export->len );
        if( ( sym == NULL ) || (sym->info & SYM_DEFINED) == 0 ) {
            LnkMsg( WRN+MSG_EXP_SYM_NOT_FOUND, "s", export->name.u.ptr );
        } else if( !IS_SYM_IMPORTED( sym ) ) {
            char    ext_name[255 + 1];

            len = create_sym_extname( sym, ext_name );
            wrote += WriteLoadU8Name( ext_name, len, false );

            off = sym->addr.off;
            if( sym->addr.seg == CODE_SEGMENT ) {
                off |= NOV_EXP_ISCODE;
            }
            WriteLoadU32( off );
            wrote += sizeof( unsigned_32 );
            count++;

            AddImpLibEntry( sym->name.u.ptr, ext_name, NOT_IMP_BY_ORDINAL );
        }
    }
    header->numberOfPublics = count;
    return( wrote );
}

static unsigned_32 WriteNovModules( fixed_header *header )
/********************************************************/
{
    obj_name_list   *module;
    unsigned_32     count;
    unsigned_32     wrote;

    count = wrote = 0;
    for( module = FmtData.u.nov.exp.module; module != NULL; module = module->next ) {
        wrote += WriteLoadU8Name( module->name.u.ptr, module->len, false );
        count++;
    }
    header->numberOfModuleDependencies = count;
    return( wrote );
}

void NovDBIAddGlobal( void *_sym )
/********************************/
{
    symbol *sym = _sym;

    if( !IS_SYM_A_REF( sym )
            && !IS_SYM_ALIAS( sym )
            && ( sym->p.seg != NULL )
            && (sym->info & SYM_DEAD) == 0
            && !sym->p.seg->isabs
            && (sym->info & SYM_STATIC) == 0
            && (FmtData.u.nov.flags & DO_NOV_EXPORTS) == 0 ) {
        DbgInfoLen += sizeof( nov_dbg_info ) + strlen( sym->name.u.ptr );
    }
}

void NovDBIAddrStart( void )
/**************************/
{
    if( DbgInfoLen != 0 ) {
        NovDbgInfo = AllocStg( DbgInfoLen );
        CurrDbgLoc = NovDbgInfo;
    }
}

void NovDBIGenGlobal( symbol *sym )
/*********************************/
{
    nov_dbg_info    info;
    size_t          len;

    if( ( DbgInfoLen != 0 ) && ( (FmtData.u.nov.flags & DO_NOV_REF_ONLY) == 0 || (sym->info & SYM_REFERENCED) ) ) {
        DbgInfoCount++;
        if( sym->addr.seg == DATA_SEGMENT ) {
            info.type = DBG_DATA;
        } else {
            info.type = DBG_CODE;
        }
        info.offset = sym->addr.off;
        len = strlen( sym->name.u.ptr );
        if( len > 255 )
            len = 255;
        info.namelen = (unsigned char)len;
        PutInfo( CurrDbgLoc, &info, sizeof( nov_dbg_info ) );
        CurrDbgLoc += sizeof( nov_dbg_info );
        PutInfo( CurrDbgLoc, sym->name.u.ptr, info.namelen );
        CurrDbgLoc += info.namelen;
    }
}

static unsigned_32 WriteNovDBI( fixed_header *header )
/****************************************************/
{
    obj_name_list   *export;
    symbol          *sym;
    unsigned_32     count;
    unsigned_32     wrote;
    nov_dbg_info    info;

    if( DbgInfoLen > 0 ) {
        WriteInfoLoad( NovDbgInfo, CurrDbgLoc - NovDbgInfo );
        header->numberOfDebugRecords = DbgInfoCount;
        return( CurrDbgLoc - NovDbgInfo );
    } else if( FmtData.u.nov.flags & DO_NOV_EXPORTS ) {
        count = wrote = 0;
        for( export = FmtData.u.nov.exp.export; export != NULL; export = export->next ) {
            sym = SymOp( ST_FIND, export->name.u.ptr, export->len );
            if( ( sym != NULL ) && !IS_SYM_IMPORTED( sym ) ) {
                if( sym->addr.seg == DATA_SEGMENT ) {
                    info.type = DBG_DATA;
                } else {
                    info.type = DBG_CODE;
                }
                info.offset = sym->addr.off;
                WriteLoad( &info, offsetof( nov_dbg_info, namelen ) );
                wrote += offsetof( nov_dbg_info, namelen ) + WriteLoadU8Name( export->name.u.ptr, export->len, false );
                count++;
            }
        }
        header->numberOfDebugRecords = count;
        return( wrote );
    }
    header->numberOfDebugRecords = 0;
    return( 0 );
}

static unsigned_32 WriteMessages( extended_nlm_header *header )
/*************************************************************/
/* write out the messages file */
{
    f_handle    handle;
    unsigned_32 buf[2];

    header->messageFileLength = 0;
    if( FmtData.u.nov.messages != NULL ) {
        handle = QOpenR( FmtData.u.nov.messages );
        QRead( handle, TokBuff, MSG_FILE_SIGNATURE_LENGTH, FmtData.u.nov.messages );
        if( memcmp( TokBuff, MSG_FILE_SIGNATURE, MSG_FILE_SIGNATURE_LENGTH ) != 0 ) {
            LnkMsg( WRN+MSG_INV_MESSAGE_FILE, "s", FmtData.u.nov.messages );
        } else {
            QSeek( handle, LANGUAGE_ID_OFFSET, FmtData.u.nov.messages );
            QRead( handle, buf, 2 * sizeof( unsigned_32 ), FmtData.u.nov.messages );
            header->languageID = buf[0];
            header->messageCount = buf[1];
            QSeek( handle, 0, FmtData.u.nov.messages );
            header->messageFileLength = CopyToLoad( handle, FmtData.u.nov.messages );
        }
        QClose( handle, FmtData.u.nov.messages );
    }
    return( header->messageFileLength );
}

static unsigned_32 WriteSharedNLM( extended_nlm_header *header, unsigned_32 file_size )
/*************************************************************************************/
{
    f_handle            handle;
    fixed_header        *sharehdr;
    unsigned_32         size;

    size = 0;
    if( FmtData.u.nov.sharednlm != NULL ) {
        handle = QOpenR( FmtData.u.nov.sharednlm );
        QRead( handle, TokBuff, sizeof( fixed_header ), FmtData.u.nov.sharednlm );
        if( memcmp( TokBuff, NLM_SIGNATURE, sizeof( NLM_SIGNATURE ) - 1 ) != 0 ) {
            LnkMsg( WRN+MSG_INV_SHARED_NLM_FILE, "s", FmtData.u.nov.sharednlm );
        } else {
            sharehdr = (fixed_header *)TokBuff;
            header->sharedCodeOffset = sharehdr->codeImageOffset + file_size;
            header->sharedCodeLength = sharehdr->codeImageSize;
            header->sharedDataOffset = sharehdr->dataImageOffset + file_size;
            header->sharedDataLength = sharehdr->dataImageSize;
            header->sharedRelocationFixupOffset
                        = sharehdr->relocationFixupOffset + file_size;
            header->sharedRelocationFixupCount
                        = sharehdr->numberOfRelocationFixups;
            header->sharedExternalReferenceOffset
                        = sharehdr->externalReferencesOffset + file_size;
            header->sharedExternalReferenceCount
                        = sharehdr->numberOfExternalReferences;
            header->sharedPublicsOffset = sharehdr->publicsOffset + file_size;
            header->sharedPublicsCount = sharehdr->numberOfPublics;
            header->sharedDebugRecordOffset
                        = sharehdr->debugInfoOffset + file_size;
            header->sharedDebugRecordCount = sharehdr->numberOfDebugRecords;
            header->sharedInitializationOffset = sharehdr->codeStartOffset;
            header->sharedExitProcedureOffset = sharehdr->exitProcedureOffset;
            QSeek( handle, 0, FmtData.u.nov.sharednlm );
            size = CopyToLoad( handle, FmtData.u.nov.sharednlm );
        }
        QClose( handle, FmtData.u.nov.sharednlm );
    }
    return( size );
}

static void GetProcOffsets( fixed_header *header )
/************************************************/
{
    symbol      *sym;
    const char  *name;

    header->checkUnloadProcedureOffset = 0;
    if( FmtData.u.nov.checkfn != NULL ) {
        sym = FindISymbol( FmtData.u.nov.checkfn );
        if( sym == NULL ) {
            LnkMsg( WRN+MSG_CHECK_NOT_FOUND, NULL );
        } else {
            header->checkUnloadProcedureOffset = sym->addr.off;
        }
    }
    if( StartInfo.type == START_IS_SYM ) {
        name = StartInfo.targ.sym->name.u.ptr;
    } else {
        name = DEFAULT_PRELUDE_FN_CLIB;
    }
    sym = FindISymbol( name );
    if( ( sym == NULL ) || (sym->info & SYM_DEFINED) == 0 ) {
        LnkMsg( ERR + MSG_START_PROC_NOT_FOUND, NULL );
    } else {
        header->codeStartOffset = sym->addr.off;
    }
    if( FmtData.u.nov.exitfn != NULL ) {
        name = FmtData.u.nov.exitfn;
    } else {
        name = DEFAULT_EXIT_FN_CLIB;
    }
    sym = FindISymbol( name );
    if( sym == NULL ) {
        LnkMsg( ERR + MSG_EXIT_PROC_NOT_FOUND, NULL );
    } else {
        header->exitProcedureOffset = sym->addr.off;
    }
}

static unsigned_32 WriteNovImage( unsigned_32 file_pos, bool docode )
/*******************************************************************/
// Write a Novell image
{
    group_entry         *group;
    outfilelist         *fnode;
    bool                repos;
    bool                iscode;

    /* write groups.*/
    fnode = Root->outfile;
    for( group = Groups; group != NULL; group = group->next_group ) {
        if( group->grp_addr.seg == CODE_SEGMENT ) {
            iscode = true;
        } else {
            iscode = false;
        }
        if( iscode == docode ) {  // logical XNOR would be better, but...
            repos = WriteDOSGroup( group );
            if( repos ) {
                SeekLoad( fnode->file_loc );
            }
        }
    }
    return( fnode->file_loc - file_pos );
}

static unsigned_32 WriteNovData( unsigned_32 file_pos, fixed_header *header )
/***************************************************************************/
// write both the code image and the data image.
{
    unsigned_32     codesize;

    DEBUG(( DBG_BASE, "Writing data" ));
    OrderGroups( CompareProtSegments );
    CurrSect = Root;        // needed for WriteInfo.
    Root->outfile->file_loc = file_pos;
    Root->u.file_loc = file_pos;
    Root->sect_addr.off = Groups->grp_addr.off;
    Root->sect_addr.seg = CODE_SEGMENT;
    codesize = WriteNovImage( file_pos, true );         // true = do code.
    header->codeImageSize = codesize;
    file_pos += codesize;
    header->dataImageOffset = file_pos;
    Root->outfile->file_loc = file_pos;
    Root->u.file_loc = file_pos;
    Root->sect_addr.off = Groups->grp_addr.off;
    Root->sect_addr.seg = DATA_SEGMENT;
    header->dataImageSize = WriteNovImage( file_pos, false );   // do data.
    return( codesize + header->dataImageSize );
}


static void NovNameWrite( const char *name )
/******************************************/
// write a name to the loadfile in the typical novell fashion
{
    if( name != NULL ) {
        WriteLoadU8Name( name, strlen( name ), false );
    } else {
        WriteLoadU8( 0 );
    }
    WriteLoadU8( '\0' );    // terminating null character
}

void FiniNovellLoadFile( void )
/*****************************/
{
    unsigned_32         file_size;
    fixed_header        nov_header;
    fixed_hdr_2         second_header;
    fixed_hdr_3         third_header;
    extended_nlm_header ext_header;
    unsigned_32         temp;
    unsigned_32         image_size;
    const char          *startname;
    const char          *p;
    char                ch;
    size_t              len;
    size_t              len1;
    struct tm           *currtime;
    time_t              thetime;
    const char          *pPeriod = NULL;
    char                module_name[NOV_MAX_MODNAME_LEN + 1];
    bool                name_trunc;
    const char          *desc;

/* find module name (output file name without the path.) */

    startname = Root->outfile->fname;
    for( p = startname; (ch = *p) != '\0'; p++ ) {
        if( '.' == ch ) {
            pPeriod = p;
            continue;
        }
        if( IS_PATH_SEP( ch ) ) {
            startname = p + 1;
            pPeriod = NULL;
        }
    }

    /*
    // cull the module name to 8.3 (NOV_MAX_MODNAME_LEN) if necessary
    */
    name_trunc = false;
    if( pPeriod != NULL ) {
        len1 = strlen( pPeriod );
        if( len1 > NOV_MAX_EXT_LEN + 1 ) {   /* +1 include period */
            len1 = NOV_MAX_EXT_LEN + 1;
            name_trunc = true;
        }
        len = pPeriod - startname;
        if( len > NOV_MAX_NAME_LEN ) {
            len = NOV_MAX_NAME_LEN;
            name_trunc = true;
        }
        memcpy( module_name, startname, len );
        memcpy( module_name + len, pPeriod, len1 );   /* must include period */
        len += len1;
    } else {
        /* still only copy 8 chars else the module name will be too long */
        len = strlen( startname );
        if( len > NOV_MAX_NAME_LEN ) {
            len = NOV_MAX_NAME_LEN;
            name_trunc = true;
        }
        memcpy( module_name, startname, len );
    }
    module_name[len] = '\0';
    strupr( module_name );

    if( name_trunc ) {
        LnkMsg( WRN+MSG_INTERNAL_MOD_NAME_DIFF_FROM_FILE, "s", module_name );
    }
    /* setup description */
    if( FmtData.description == NULL ) {
        desc = module_name;
        nov_header.descriptionLength = len;
    } else {
        nov_header.descriptionLength = strlen( FmtData.description );
        if( nov_header.descriptionLength > MAX_DESCRIPTION_LENGTH ) {
            nov_header.descriptionLength = MAX_DESCRIPTION_LENGTH;
            FmtData.description[MAX_DESCRIPTION_LENGTH] = '\0';
            LnkMsg( WRN+MSG_VALUE_TOO_LARGE, "s", "description" );
        }
        desc = FmtData.description;
    }

    file_size = nov_header.descriptionLength + sizeof( fixed_header )
                + sizeof( extended_nlm_header ) + 2 * sizeof( unsigned_32 )
                + NOV_MAX_MODNAME_LEN;
    if( FmtData.u.nov.screenname != NULL ) {
        file_size += strlen( FmtData.u.nov.screenname );
    }
    if( FmtData.u.nov.threadname != NULL ) {
        file_size += strlen( FmtData.u.nov.threadname );
    } else {
        file_size += len;
    }
    if( ( FmtData.major != 0 ) || ( FmtData.minor != 0 ) ) {
        file_size += sizeof( fixed_hdr_2 );
    }
    if( FmtData.u.nov.copyright != NULL ) {
        file_size += sizeof( fixed_hdr_3 ) + strlen( FmtData.u.nov.copyright );
    }
    SeekLoad( file_size );
    nov_header.codeImageOffset = file_size;
    image_size = WriteNovData( file_size, &nov_header );
    len1 = MemorySize() - image_size;
    if( len1 > 0 ) {       // write out BSS.
        PadLoad( len1 );
        nov_header.dataImageSize += len1;
        image_size += len1;
    }
    file_size += image_size;
    nov_header.relocationFixupOffset = file_size;
    file_size += WriteNovRelocs( &nov_header );
    nov_header.externalReferencesOffset = file_size;
    file_size += WriteNovImports( &nov_header );
    nov_header.publicsOffset = file_size;
    file_size += WriteNovExports( &nov_header );
    nov_header.moduleDependencyOffset = file_size;
    file_size += WriteNovModules( &nov_header );
    nov_header.customDataOffset = file_size;
    temp = AppendToLoadFile( FmtData.u.nov.customdata );
    nov_header.customDataSize = temp;
    file_size += temp;
    memset( &ext_header, 0, sizeof( ext_header ) );
    memcpy( ext_header.stamp, EXTENDED_NLM_SIGNATURE, EXTENDED_NLM_SIGNATURE_LENGTH );
    ext_header.messageFileOffset = file_size;
    file_size += WriteMessages( &ext_header );
    ext_header.helpFileOffset = file_size;
    temp = AppendToLoadFile( FmtData.u.nov.help );
    ext_header.helpFileLength = temp;
    file_size += temp;
    ext_header.RPCDataOffset = file_size;
    temp = AppendToLoadFile( FmtData.u.nov.rpcdata );
    ext_header.RPCDataLength = temp;
    file_size += temp;
    file_size += WriteSharedNLM( &ext_header, file_size );
    nov_header.debugInfoOffset = file_size;
    file_size += WriteNovDBI( &nov_header );
    DBIWrite();
    memcpy( nov_header.signature, NLM_SIGNATURE, sizeof( NLM_SIGNATURE ) );
    nov_header.version = NLM_VERSION;
    nov_header.moduleName[0] = (char)len;
    memcpy( nov_header.moduleName + 1, module_name, len );
    memset( nov_header.moduleName + 1 + len, 0, NOV_MAX_MODNAME_LEN - len ); // zero rest.
    nov_header.uninitializedDataSize = 0; // MemorySize() - image_size;
    GetProcOffsets( &nov_header );
    nov_header.moduleType = FmtData.u.nov.moduletype;
    nov_header.flags = FmtData.u.nov.exeflags;
    SeekLoad( 0L );
    WriteLoad( &nov_header, sizeof( nov_header ) );
    WriteLoad( desc, nov_header.descriptionLength + 1 );
    WriteLoadU32( StackSize );
    WriteLoadU32( 0 );          // reserved.
    WriteLoad( DUMMY_THREAD_NAME, OLD_THREAD_NAME_LENGTH );
    NovNameWrite( FmtData.u.nov.screenname );
    if( FmtData.u.nov.threadname != NULL ) {
        NovNameWrite( FmtData.u.nov.threadname );
    } else {
        NovNameWrite( module_name );      // use module name as a default
    }
    if( ( FmtData.major != 0 ) || ( FmtData.minor != 0 ) ) {
        memcpy( second_header.versionSignature, VERSION_SIGNATURE, VERSION_SIGNATURE_LENGTH );
        second_header.majorVersion = FmtData.major;
        second_header.minorVersion = FmtData.minor;
        second_header.revision = FmtData.revision;
        thetime = time( NULL );
        currtime = localtime( &thetime );
        second_header.year = currtime->tm_year + 1900;
        second_header.month = currtime->tm_mon + 1;
        second_header.day = currtime->tm_mday;
        WriteLoad( &second_header, sizeof( second_header ) );
    }

    if( FmtData.u.nov.copyright != NULL ) {
        memcpy( third_header.copyrightSignature, COPYRIGHT_SIGNATURE, COPYRIGHT_SIGNATURE_LENGTH);
        WriteLoad( &third_header, sizeof( third_header ) );
        NovNameWrite( FmtData.u.nov.copyright );
    }
    WriteLoad( &ext_header, sizeof( ext_header ) );
}

void AddNovImpReloc( symbol *sym, unsigned_32 offset, bool isrelative, bool isdata )
/**********************************************************************************/
// add a relocation to the import record.
{
    nov_import      *imp;
    nov_import      *new;
    virt_mem        vmem_ptr;
    unsigned        vblock;     // which virt_mem block
    unsigned        voff;       // offset into a virt_mem block

    if( !isrelative ) {
         offset |= NOV_IMP_NONRELATIVE;
    }
    if( !isdata ) {
        offset |= NOV_IMP_ISCODE;
    }
    imp = sym->p.import;
    if( imp == DUMMY_IMPORT_PTR ) {
        _ChkAlloc( imp, sizeof( nov_import ) );
        sym->p.import = imp;
        imp->contents = 0;
        imp->u.r.relocs[imp->contents++] = offset;
    } else if( imp->contents < MAX_IMP_INTERNAL ) {
        if( imp->contents == 2 ) {
            _ChkAlloc( new, ( MAX_IMP_INTERNAL - 2 ) * sizeof( unsigned_32 ) + sizeof( nov_import ) );
            memcpy( new, imp, sizeof( nov_import ) );
            _LnkFree( imp );
            imp = new;
            sym->p.import = imp;
        }
        imp->u.r.relocs[imp->contents++] = offset;
    } else if( imp->contents == MAX_IMP_INTERNAL ) { // set up virt.mem
        vmem_ptr = AllocStg( IMP_VIRT_ALLOC_SIZE );
        PutInfo( vmem_ptr, imp->u.r.relocs, MAX_IMP_INTERNAL * sizeof( unsigned_32 ) );
        PutInfo( vmem_ptr + MAX_IMP_INTERNAL * sizeof( unsigned_32 ), &offset, sizeof( unsigned_32 ) );
        imp->contents++;
        imp->u.v.num_relocs = imp->contents;
        imp->u.v.vm_ptr[0] = vmem_ptr;
    } else {    // imp->contents > MAX_IMP_INTERNAL
        vblock = imp->u.v.num_relocs / IMP_NUM_VIRT;
        voff = imp->u.v.num_relocs % IMP_NUM_VIRT;
        if( voff == 0 ) {
            if( vblock >= (unsigned)( imp->contents - MAX_IMP_INTERNAL ) * MAX_IMP_VIRT ) {
                _ChkAlloc( new, sizeof( nov_import ) - sizeof( unsigned_32 ) + vblock * sizeof( unsigned_32 ) * 2 );
                memcpy( new, imp, sizeof( nov_import ) - sizeof( unsigned_32 ) + vblock * sizeof( unsigned_32 ) );
                _LnkFree( imp );
                imp = new;
                imp->contents++;
                sym->p.import = imp;
            }
            imp->u.v.vm_ptr[vblock] = AllocStg( IMP_VIRT_ALLOC_SIZE );
        }
        PutInfo( imp->u.v.vm_ptr[vblock] + voff * sizeof( unsigned_32 ), &offset, sizeof( unsigned_32 ) );
        imp->u.v.num_relocs++;
    }
}

void FindExportedSyms( void )
/***************************/
{
    obj_name_list   *export;
    symbol          *sym;
    debug_info      *dinfo;

    dinfo = CurrSect->dbg_info;
    if( (FmtData.u.nov.flags & DO_WATCOM_EXPORTS) && ( dinfo != NULL ) ) {
        for( export = FmtData.u.nov.exp.export; export != NULL; export = export->next ) {
            sym = SymOp( ST_FIND, export->name.u.ptr, export->len );
            if( ( sym != NULL ) && !IS_SYM_IMPORTED( sym ) ) {
                dinfo->global.curr.u.vm_offs += sizeof( v3_gbl_info ) + strlen( sym->name.u.ptr );
            }
        }
    }
}
