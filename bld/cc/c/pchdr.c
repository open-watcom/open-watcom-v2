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


#include "cvars.h"
#include "pragdefn.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <share.h>
#if _OS == _QNX
    #define PMODE       S_IRUSR+S_IWUSR+S_IRGRP+S_IWGRP+S_IROTH+S_IWOTH
#else
    #include <direct.h>
    #define PMODE       S_IRWXU
#endif


extern  TAGPTR  TagHash[TAG_HASH_SIZE + 1];

#define PH_BUF_SIZE     32768
#define PCH_VERSION     ((_HOST << 16) | 0x0196)

static  jmp_buf         PH_jmpbuf;
static  int             PH_handle;
static  char            *PH_Buffer;
static  char            *PH_BufPtr;
static  unsigned        PH_BufSize;
static  MEPTR           *PCHMacroHash;
static  MEPTR           PCHUndefMacroList;
static  TYPEPTR         TypeArray;
static  TAGPTR          *TagArray;
static  FNAMEPTR        FNameList;
static  struct textsegment **TextSegArray;
static  unsigned        PH_SymHashCount;
static  unsigned        PH_FileCount;
static  unsigned        PH_RDirCount;
static  unsigned        PH_IncFileCount;
static  unsigned        PH_LibraryCount;
static  unsigned        PH_SegCount;
static  unsigned        PH_MacroCount;
static  unsigned        PH_UndefMacroCount;
static  unsigned        PH_TypeCount;
static  unsigned        PH_TagCount;
static  unsigned        PH_PragmaCount;
static  unsigned        PH_size;
static  unsigned        PH_MacroSize;
static  unsigned        PH_cwd_len;
static  char            PH_computing_size;

static  struct  rdir_list *PCHRDirNames;  /* list of read only directorys */

struct  pheader {
    unsigned long       signature;      //  'WPCH'
    unsigned            version;
    unsigned            size_of_header;
    unsigned            size_of_int;
    unsigned            pack_amount;    // PackAmount
    unsigned long       gen_switches;   // GenSwitches
    unsigned long       target_switches;// TargetSwitches
    int                 toggles;        // Toggles
    unsigned            size;
    unsigned            macro_size;
    unsigned            file_count;
    unsigned            rdir_count;
    unsigned            incfile_count;
    unsigned            incline_count;  // IncLineCount
    unsigned            library_count;  // # of pragma library(s)
    unsigned            seg_count;
    unsigned            macro_count;
    unsigned            undef_macro_count;
    unsigned            type_count;
    unsigned            tag_count;
    unsigned            pragma_count;
    unsigned            symhash_count;
    unsigned            symbol_count;
    unsigned            specialsyms_count;
    unsigned            cwd_len;        // length of current working directory
    unsigned            msgflags_len;   // length of MsgFlags array
};

#if _MACHINE == _PC
static struct aux_info *BuiltinInfos[] = {
        &DefaultInfo,
        &CdeclInfo,
        &PascalInfo,
        &FortranInfo,
        &SyscallInfo,
        &StdcallInfo,
        &OptlinkInfo,
        &FastCallInfo,
        NULL
};
#endif
//========================================================================
//      This portion of the code creates the pre-compiled header.
//========================================================================

static void InitPHVars( void ){
//*****************************
//Set vars to 0
//*****************************
    PH_SymHashCount=0;
    PH_FileCount=0;
    PH_RDirCount=0;
    PH_IncFileCount=0;
    PH_LibraryCount=0;
    PH_SegCount=0;
    PH_MacroCount=0;
    PH_UndefMacroCount=0;
    PH_TypeCount=0;
    PH_TagCount=0;
    PH_PragmaCount=0;
    PH_size=0;
    PH_MacroSize=0;
    PH_cwd_len=0;
    PH_computing_size=0;
}

static void CreatePHeader( char *filename )
{
    PH_handle = sopen( filename, O_WRONLY|O_CREAT|O_TRUNC|O_BINARY, SH_DENYRW, PMODE );
    if( PH_handle == -1 ) {
        longjmp( PH_jmpbuf, 1 );
    }
}

static int WritePHeader( void *bufptr, unsigned len )
{
    unsigned    amt_written;
    char        *buf;

    buf = bufptr;
    if( PH_computing_size ) {
        PH_size += len;
    } else if( len != 0 ) {
        if( PH_Buffer != NULL ) {
            for(;;) {
                amt_written = len;
                if( amt_written > PH_BufSize )  amt_written = PH_BufSize;
                memcpy( PH_BufPtr, buf, amt_written );
                PH_BufSize -= amt_written;
                PH_BufPtr += amt_written;
                buf += amt_written;
                len -= amt_written;
                if( PH_BufSize == 0 ) {         // if buffer is full
                    PH_BufSize = PH_BUF_SIZE;
                    PH_BufPtr = PH_Buffer;
                    if( write( PH_handle, PH_Buffer, PH_BUF_SIZE ) !=
                                        PH_BUF_SIZE ) {
                        return( 1 );
                    }

                }
                if( len == 0 ) break;
            }
        } else {
            amt_written = write( PH_handle, buf, len );
            if( amt_written != len )  return( 1 );
        }
    }
    return( 0 );
}

static void FlushPHeader()
{
    unsigned    len;

    if( PH_BufSize != PH_BUF_SIZE ) {   // if buffer has some stuff in it
        len = PH_BUF_SIZE - PH_BufSize;
        if( write( PH_handle, PH_Buffer, len ) != len ) {
            longjmp( PH_jmpbuf, 1 );
        }
    }
}

static void ClosePHeader()
{
    close( PH_handle );
    PH_handle = -1;
}


static void OutPutHeader()
{
    int                 rc;
    struct pheader      pch;

    pch.signature       = 'WPCH';
    pch.version         = PCH_VERSION;
    pch.size_of_header  = sizeof(struct pheader);
    pch.size_of_int     = TARGET_INT;
    pch.pack_amount     = PackAmount;
    pch.gen_switches    = GenSwitches;
    pch.target_switches = TargetSwitches;
    pch.toggles         = Toggles;
    pch.size            = PH_size - PH_MacroSize;
    pch.macro_size      = PH_MacroSize;
    pch.file_count      = PH_FileCount;
    pch.rdir_count      = PH_RDirCount;
    pch.incfile_count   = PH_IncFileCount;
    pch.incline_count   = IncLineCount;
    pch.library_count   = PH_LibraryCount;
    pch.seg_count       = PH_SegCount;
    pch.macro_count     = PH_MacroCount;
    pch.undef_macro_count=PH_UndefMacroCount;
    pch.type_count      = PH_TypeCount;
    pch.tag_count       = PH_TagCount;
    pch.pragma_count    = PH_PragmaCount;
    pch.symhash_count   = PH_SymHashCount;
    pch.symbol_count    = NextSymHandle + 1;
    pch.specialsyms_count  = SpecialSyms;
    pch.cwd_len         = PH_cwd_len;
    if( MsgFlags != NULL ) {                            /* 06-jul-94 */
        pch.msgflags_len = ((HIGHEST_MESSAGE_NUMBER + 7) / 8)
                                + (sizeof(int) - 1) & - sizeof(int);
    } else {
        pch.msgflags_len = 0;
    }
    rc = WritePHeader( &pch, sizeof(struct pheader) );
    rc |= WritePHeader( PH_Buffer + sizeof(struct pheader), pch.cwd_len );
    if( rc != 0 )  longjmp( PH_jmpbuf, rc );
}

static void OutPutHFileList()           // output include paths
{
    int         rc;
    unsigned    len;

    len = strlen( HFileList ) + 1;
    len = (len + (sizeof(int) - 1)) & - sizeof(int);
    rc = WritePHeader( HFileList, len );
    if( rc != 0 )  longjmp( PH_jmpbuf, rc );
}

static void OutPutIncFileList()         // output primary include files
{
    int         rc;
    unsigned    len;
    INCFILE     *ifile;

    for( ifile = IncFileList; ifile; ifile = ifile->nextfile ) {
        len = sizeof(INCFILE) + ifile->len;
        len = (len + (sizeof(int) - 1)) & - sizeof(int);
        rc = WritePHeader( ifile, len );
        if( rc != 0 )  longjmp( PH_jmpbuf, rc );
        PH_IncFileCount++;
    }
}

static void OutPutLibrarys()
{
    int                 rc;
    unsigned            len;
    struct library_list *lib;

    for( lib = HeadLibs; lib; lib = lib->next ) {
        len = sizeof(struct library_list) + strlen( lib->name );
        len = (len + (sizeof(int) - 1)) & - sizeof(int);
        rc = WritePHeader( lib, len );
        if( rc != 0 )  longjmp( PH_jmpbuf, rc );
        PH_LibraryCount++;
    }
}

static void OutPutMsgFlags()
{
    int         rc;

    if( MsgFlags != NULL ) {                            /* 06-jul-94 */
        rc = WritePHeader( MsgFlags, (HIGHEST_MESSAGE_NUMBER + 7) / 8 );
        if( rc != 0 )  longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutIncludes()
{
    FNAMEPTR    flist;
    FNAMEPTR    next_flist;
    unsigned    len;
    int         rc;

    flist = FNames;
    // don't want to include the primary source file
    while( flist != NULL ){
        if( strcmp( flist->name, SrcFile->src_name ) == 0 ){
            flist = flist->next;
            break;
        }
        flist = flist->next;
    }
    while( flist != NULL ) {
        next_flist = flist->next;
        len = strlen( flist->name ) + sizeof(struct fname_list);
        len = (len + (sizeof(int) - 1)) & - sizeof(int);
        flist->fname_len = len;
        rc = WritePHeader( flist, len );
        flist->next = next_flist;
        if( rc != 0 )  longjmp( PH_jmpbuf, rc );
        flist = next_flist;
        PH_FileCount++;
    }
}

static void OutPutRoDirList()
{
    RDIRPTR   dirlist;
    RDIRPTR   next_dirlist;
    unsigned    len;
    int         rc;

    dirlist = PCHRDirNames;
    while( dirlist != NULL ) {
        next_dirlist = dirlist->next;
        len = strlen( dirlist->name ) + sizeof(struct rdir_list);
        len = (len + (sizeof(int) - 1)) & - sizeof(int);
        dirlist->name_len = len;
        rc = WritePHeader( dirlist, len );
        dirlist->next = next_dirlist;
        if( rc != 0 )  longjmp( PH_jmpbuf, rc );
        dirlist = next_dirlist;
        PH_RDirCount++;
    }
}

static void OutPutSegInfo()
{
    struct textsegment  *seg;
    struct textsegment  *next;
    int                 rc;
    unsigned            len;

    for( seg = TextSegList; seg; seg = seg->next ) {
        ++PH_SegCount;
        seg->index = PH_SegCount;
        len = strlen( seg->segname );           // segment name
        len += strlen( &seg->segname[len+1] );  // class name
        len += sizeof(struct textsegment) + 1;
        len = (len + (sizeof(int) - 1)) & - sizeof(int);
        next = seg->next;                       // save next pointer
        seg->textsegment_len = len;             // replace with len
        rc = WritePHeader( seg, len );
        seg->next = next;                       // restore next pointer
        if( rc != 0 )  longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutEnums( ENUMPTR ep, TAGPTR parent )
{
    int         rc;
    unsigned    len;

    for( ; ep; ep = ep->thread ) {
        len = strlen( ep->name ) + sizeof(ENUMDEFN);
        len = (len + (sizeof(int) - 1)) & - sizeof(int);
        ep->enum_len = len;
        rc = WritePHeader( ep, len );
        ep->parent = parent;            // enum_len is union'ed with parent
        if( rc != 0 )  longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutFields( FIELDPTR field )
{
    int         rc;
    int         len;
    FIELDPTR    next_field;
    TYPEPTR     typ;

    while( field != NULL ) {
        len = strlen( field->name ) + sizeof(FIELD_ENTRY);
        len = (len + (sizeof(int) - 1)) & - sizeof(int);
        next_field = field->next_field;         // save pointer
        field->field_len = len;                 // replace with length
        if( next_field == NULL ) {
            field->field_len = - len;           // marker for end of list
        }
        typ = field->field_type;                // save type pointer
        field->field_type_index = typ->type_index;// replace with type index
        rc = WritePHeader( field, len );
        field->next_field = next_field;         // restore pointer
        field->field_type = typ;                // restore type pointer
        if( rc != 0 )  longjmp( PH_jmpbuf, rc );
        field = next_field;
    }
}

static void OutPutATag( TAGPTR tag )
{
    int         rc;
    TYPEPTR     typ;
    unsigned    len;

    typ = tag->sym_type;
    tag->sym_type_index = typ->type_index;
    len = strlen( tag->name ) + sizeof(TAGDEFN);
    len = (len + (sizeof(int) - 1)) & - sizeof(int);
    rc = WritePHeader( tag, len );
    tag->sym_type = typ;
    if( rc != 0 )  longjmp( PH_jmpbuf, rc );
    if( typ->decl_type == TYPE_ENUM ) {
        OutPutEnums( tag->u.enum_list, tag );
    } else {
        OutPutFields( tag->u.field_list );
    }
}

static void OutPutTags()
{
    WalkTagList( OutPutATag );
}

void SetTypeIndex( TYPEPTR typ )
{
    ++PH_TypeCount;
    typ->type_index = PH_TypeCount;
}

void SetFuncTypeIndex( TYPEPTR typ, int index )
{
    index;
    SetTypeIndex( typ );
}

static void NumberTypes()
{
    PH_TypeCount = 0;
    WalkTypeList( SetTypeIndex );
    WalkFuncTypeList( SetFuncTypeIndex );
}

static void SetTagIndex( TAGPTR tag )
{
    tag->tag_index = PH_TagCount;
    ++PH_TagCount;
}

static void SetDebugTag( TAGPTR tag )
{
    tag->refno = 0;
}

static void NumberTags()
{
    PH_TagCount = 0;
    WalkTagList( SetTagIndex );
}

static void InitDebugTags()
{
    WalkTagList( SetDebugTag );
}

static int WriteType( TYPEPTR typ )
{
    int         rc;
    TYPEPTR     object;

    object = typ->object;
    if( object != NULL ) {
        typ->object_index = object->type_index;
        rc = WritePHeader( typ, sizeof(TYPEDEFN) );
        typ->object = object;
    } else {
        rc = WritePHeader( typ, sizeof(TYPEDEFN) );
    }
    return( rc );
}

struct type_indices {
        int     basetype_index[TYPE_PLAIN_CHAR+1];
        int     stringtype_index;
        int     constchartype_index;
};

static void OutPutTypeIndexes()                         /* 02-jan-95 */
{
    TYPEPTR     typ;
    int         rc;
    int         i;
    struct type_indices typ_index;

    for( i = TYPE_CHAR; i <= TYPE_DOT_DOT_DOT; i++ ) {
        typ = BaseTypes[i];
        if( typ == NULL ) {
            typ_index.basetype_index[i] = 0;
        } else {
            typ_index.basetype_index[i] = typ->type_index;
        }
    }
    typ_index.stringtype_index = StringType->type_index;
    typ_index.constchartype_index = ConstCharType->type_index;
    rc = WritePHeader( &typ_index, sizeof(struct type_indices) );
    if( rc != 0 )  longjmp( PH_jmpbuf, rc );
}

static void OutPutAType( TYPEPTR typ )
{
    TAGPTR      tag;
    int         rc;
    struct array_info *array;

    rc = 0;
    switch( typ->decl_type ) {
    case TYPE_ARRAY:
        array = typ->u.array;                   // save pointer
        typ->u.array_dimension = array->dimension;      // replace with dim
        rc = WriteType( typ );
        typ->u.array = array;                   // restore pointer
        break;
    case TYPE_STRUCT:
    case TYPE_UNION:
    case TYPE_ENUM:
        tag = typ->u.tag;                               // save tag
        typ->u.tag_index = tag->tag_index;              // replace with index
        rc = WriteType( typ );
        typ->u.tag = tag;                               // restore tag pointer
        break;
    default:
        if( typ->decl_type != TYPE_FUNCTION ) {
            rc = WriteType( typ );
        }
        break;
    }
    if( rc != 0 )  longjmp( PH_jmpbuf, rc );
}

static void OutPutAFuncType( TYPEPTR typ, int index )
{
    TYPEPTR     *parm_list;
    int         rc;

    parm_list = typ->u.parms;                   // save pointer
    typ->u.parm_index = index;                  // replace with index
    rc = WriteType( typ );
    typ->u.parms = parm_list;                   // restore pointer
    if( rc != 0 )  longjmp( PH_jmpbuf, rc );
}

static void OutPutFuncParmList( TYPEPTR typ, int index )
{
    TYPEPTR     *parm_list;
    int         rc;
    union parmtype {
        TYPEPTR parm_typ;
        int     type_index;
    } parm;

    index;
    parm_list = typ->u.parms;
    if( parm_list != NULL ) {
        for( ; *parm_list; ++parm_list ) {
            parm.type_index = (*parm_list)->type_index;
            rc = WritePHeader( &parm, sizeof(union parmtype) );
            if( rc != 0 )  longjmp( PH_jmpbuf, rc );
        }
    }
    parm.type_index = -1;
    rc = WritePHeader( &parm, sizeof(union parmtype) );
    if( rc != 0 )  longjmp( PH_jmpbuf, rc );
}

static void OutPutTypes()
{
    NumberTypes();
    NumberTags();
    WalkTypeList( OutPutAType );
    WalkFuncTypeList( OutPutAFuncType );
    WalkFuncTypeList( OutPutFuncParmList );
    OutPutTypeIndexes();
}

#if _MACHINE == _PC
static void OutPutAuxInfo( struct aux_info *info )
{
    hw_reg_set          *regs;
    hw_reg_set          *save_parms;
    char                *save_objname;
    byte_seq            *save_code;
    int                 rc;
    unsigned            len;
    unsigned            padding;

    save_parms = info->parms;
    save_objname = info->objname;
    save_code = info->code;
    len = sizeof( struct aux_info );
    if( save_parms != NULL ) {
        info->parms_size = 0;
        regs = save_parms;
        for(;;) {
            info->parms_size += sizeof( hw_reg_set );
            if( HW_CEqual( *regs, HW_EMPTY ) ) break;
            ++regs;
        }
    }
    if( save_objname != NULL ) {
        info->objname_size = strlen( save_objname ) + 1;
        len += info->objname_size;
    }
    if( save_code != NULL ) {
        info->code_size = (save_code->length & MAX_BYTE_SEQ_LEN)
                                + sizeof(byte_seq);
        len += info->code_size;
    }
    padding = ((len + (sizeof(int) - 1)) & - sizeof(int)) - len;
    rc = WritePHeader( info, sizeof( struct aux_info ) );
    if( save_parms != NULL ) {
        regs = save_parms;
        for(;;) {
            rc |= WritePHeader( regs, sizeof( hw_reg_set ) );
            if( HW_CEqual( *regs, HW_EMPTY ) ) break;
            ++regs;
        }
    }
    if( save_objname != NULL ) {
        len = strlen( save_objname ) + 1;
        rc |= WritePHeader( save_objname, len );
    }
    if( save_code != NULL ) {
        len = (save_code->length & MAX_BYTE_SEQ_LEN)
                        + sizeof(byte_seq);
        rc |= WritePHeader( save_code, len );
    }
    rc |= WritePHeader( "    ", padding );
    info->parms = save_parms;
    info->objname = save_objname;
    info->code = save_code;
    if( rc != 0 )  longjmp( PH_jmpbuf, rc );
}

static void OutPutPragmaInfo()
{
    struct aux_entry    *ent;
    struct aux_info     *info;
    int                 index;
    int                 rc;
    unsigned            len;

    for( index = 0; info = BuiltinInfos[index]; ++index ) {
        OutPutAuxInfo( info );          // write out the aux_info struct
    }
    PH_PragmaCount = 0;
    for( ent = AuxList; ent; ent = ent->next ) {
        info = ent->info;
        info->aux_info_index = -1;
    }
    index = 0;
    for( ent = AuxList; ent; ent = ent->next ) {
        info = ent->info;
        if( info->aux_info_index == -1 ) {
            info->aux_info_index = index;
            index++;
            OutPutAuxInfo( info );      // write out the aux_info struct
            ++PH_PragmaCount;
        }
    }
    for( ent = AuxList; ent; ent = ent->next ) {
        info = ent->info;
        ent->aux_info_index = info->aux_info_index;
        // write out aux_entry
        len = sizeof( struct aux_entry ) + strlen( ent->name );
        len = (len + (sizeof(int) - 1)) & - sizeof(int);
        rc = WritePHeader( ent, len );
        ent->info = info;                       // restore pointer
        if( rc != 0 )  longjmp( PH_jmpbuf, rc );
    }
}
#endif

static void OutPutMacros()
{
    int         i;
    int         rc;
    MEPTR       mentry;
    MEPTR       next_macro;
    unsigned    mentry_len;

    PH_MacroCount = 0;
    PH_MacroSize = PH_size;
    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        for( mentry = MacHash[i]; mentry; mentry = mentry->next_macro ) {
            mentry_len = mentry->macro_len;
            next_macro = mentry->next_macro;        // save pointer
            mentry->macro_index = i;        // replace with hash index
            rc = WritePHeader( mentry, mentry_len );
            mentry->next_macro = next_macro;        // restore pointer
            mentry_len = ((mentry_len + (sizeof(int) - 1)) & -sizeof(int))
                                - mentry_len;
            rc |= WritePHeader( "    ", mentry_len );
            if( rc != 0 )  longjmp( PH_jmpbuf, rc );
            ++PH_MacroCount;
        }
    }
    /* write out undefined macro list.  26-may-94 */
    PH_UndefMacroCount = 0;
    for( mentry = UndefMacroList; mentry; mentry = mentry->next_macro ) {
        mentry_len = mentry->macro_len;
        rc = WritePHeader( mentry, mentry_len );
        mentry_len = ((mentry_len + (sizeof(int) - 1)) & - sizeof(int))
                            - mentry_len;
        rc |= WritePHeader( "    ", mentry_len );
        if( rc != 0 )  longjmp( PH_jmpbuf, rc );
        ++PH_UndefMacroCount;
    }
    PH_MacroSize = PH_size - PH_MacroSize;
}

static void OutPutSymHashTable()
{
    SYM_HASHPTR hsym;
    SYM_HASHPTR next_hsymptr;
    SYM_HASHPTR sym_list;
    TYPEPTR     typ;
    int         i;
    int         rc;
    unsigned    len;

    for( i=0; i < SYM_HASH_SIZE; i++ ) {
        // reverse the list
        sym_list = NULL;
        for( hsym = HashTab[i]; hsym; hsym = next_hsymptr ) {
            next_hsymptr = hsym->next_sym;
            hsym->next_sym = sym_list;
            sym_list = hsym;
            ++PH_SymHashCount;
        }
        HashTab[i] = NULL;
        rc = 0;
        for( hsym = sym_list; hsym; hsym = next_hsymptr ) {
            next_hsymptr = hsym->next_sym;
            hsym->hash_index = i;
            typ = hsym->sym_type;               // save type pointer
            if( typ != NULL ) {
                hsym->sym_type_index = typ->type_index; // replace with index
            }
            len = strlen( hsym->name ) + sizeof(struct sym_hash_entry);
            len = (len + (sizeof(int) - 1)) & - sizeof(int);
            rc |= WritePHeader( hsym, len );
            hsym->sym_type = typ;               // restore type pointer
            hsym->next_sym = HashTab[i];
            HashTab[i] = hsym;
        }
        if( rc != 0 )  longjmp( PH_jmpbuf, rc );
    }
}

static void OutPutSymbols()
{
    SYM_ENTRY   sym;
    SYM_HANDLE  sym_handle;
    int         rc;

    if( PH_computing_size ) {
        PH_size += (NextSymHandle+1) * sizeof(SYM_ENTRY);
    } else {
        for( sym_handle = 0; sym_handle <= NextSymHandle; sym_handle++ ) {
            SymGet( &sym, sym_handle );
            if( sym.sym_type != NULL ) {
                sym.sym_type_index = sym.sym_type->type_index;
            }
            if( sym.seginfo != NULL ) {
                sym.seginfo_index = sym.seginfo->index;
            }
            sym.name = NULL; // can't carry a name across
            rc = WritePHeader( &sym, sizeof(SYM_ENTRY) );
            if( rc != 0 )  longjmp( PH_jmpbuf, rc );
        }
    }
}

void OutPutEverything()
{
    PH_SymHashCount = 0;
    PH_FileCount = 0;
    PH_RDirCount=0;
    PH_SegCount = 0;
    OutPutIncludes();
    OutPutRoDirList();
    OutPutHFileList();
    OutPutIncFileList();
    OutPutLibrarys();
    OutPutSegInfo();
    OutPutTypes();
    OutPutTags();
#if _MACHINE == _PC
    OutPutPragmaInfo();
#endif
    OutPutSymHashTable();
    OutPutSymbols();
    OutPutMsgFlags();
    OutPutMacros();
}

void InitBuildPreCompiledHeader( void )
//Save any before info when building pre compiled headers
{
    struct  rdir_list *start;

    start = RDirNames;
    if( start != NULL ){
        start = start->next;
    }
    PCHRDirNames = start;
}

void BuildPreCompiledHeader( char *filename )
{
    int         rc;
    char        *cwd;

    if(  QuadIndex != 0 || DataQuadSegIndex != -1 ) {

        PCHNote( PCHDR_NO_OBJECT );
        return;
    }
    InitPHVars();
    PH_Buffer = FEmalloc( PH_BUF_SIZE );
    PH_BufPtr = PH_Buffer;
    PH_BufSize = PH_BUF_SIZE;
    cwd = getcwd( PH_Buffer + sizeof(struct pheader),
                  PH_BUF_SIZE - sizeof(struct pheader) );
    rc = setjmp( PH_jmpbuf );
    if( rc == 0 ) {
        CreatePHeader( filename );
        PH_cwd_len = ((strlen(cwd) + 1) + sizeof(int) - 1)
                                 & - sizeof(int);
        PH_size = PH_cwd_len;
        PH_computing_size = 1;
        OutPutEverything();
        PH_computing_size = 0;
        OutPutHeader();
        OutPutEverything();
        FlushPHeader();
        ClosePHeader();
    } else {                            // error creating pre-compiled header
        if( PH_handle != -1 ) {
            ClosePHeader();             // close the file
            remove( filename );         // delete the pre-compiled header
        }
    }
    InitDebugTypes();
    InitDebugTags();
    FEfree( PH_Buffer );
    PCH_FileName = NULL;
    CompFlags.make_precompiled_header = 0;
}
//========================================================================
//      This portion of the code loads the pre-compiled header and
//      rebuilds the data structures by replacing all the indices with
//      pointers, reconstructing hash tables, etc.
//========================================================================

static char *FixupIncFileList( char *p, unsigned incfile_count )
{
    INCFILE     *ifile;
    unsigned    len;

    IncFileList = NULL;
    if( incfile_count != 0 ) {
        IncFileList = (INCFILE *)p;
        for(;;) {
            ifile = (INCFILE *)p;
            len = sizeof(INCFILE) + ifile->len;
            len = (len + (sizeof(int) - 1)) & - sizeof(int);
            p += len;
            --incfile_count;
            if( incfile_count == 0 ) break;
            ifile->nextfile = (INCFILE *)p;
        }
    }
    return( p );
}

static char *FixupIncludes( char *p, unsigned file_count )
{
    FNAMEPTR    flist;
    unsigned    len;

    FNameList = NULL;
    if( file_count != 0 ) {
        FNameList = (FNAMEPTR)p;
        for(;;) {
            flist = (FNAMEPTR)p;
            len = flist->fname_len;
            flist->fullpath = NULL;
            p += len;
            flist->next = (FNAMEPTR)p;
            --file_count;
            if( file_count == 0 ) break;
        }
        flist->next = NULL;
    }
    return( p );
}

static char *FixupRoDirList( char *p, unsigned list_count )
{
    RDIRPTR     dirlist;
    RDIRPTR    *lnk;
    unsigned    len;

    lnk = &RDirNames;
    while( (dirlist = *lnk) != NULL ) {
        lnk = &dirlist->next;
    }
    while( list_count != 0 ) {
        dirlist = (RDIRPTR)p;
        len = dirlist->name_len;
        p += len;
        *lnk = dirlist;
        lnk = &dirlist->next;
        --list_count;
    }
    *lnk = NULL;
    return( p );
}

static int VerifyIncludes()
{
    FNAMEPTR    flist;
    time_t      mtime;
    bool        failed;

    failed = FALSE;
    for( flist = FNameList; flist; flist = flist->next ) {

        if( flist->rwflag ){
            if( SrcFileInRDir( flist ) ){
                flist->rwflag = FALSE;
            }
        }
        if( flist->rwflag ){
            if( (SrcFileTime( flist->name, &mtime ) != 0 ) || flist->mtime != mtime ){
                PCHNote( PCHDR_INCFILE_CHANGED, flist->name  );
          #if 0
                printf( "%s old %d new %d\n",flist->name, flist->mtime, mtime );
          #endif
                failed = TRUE;
            }
        }
    }
    return( failed );
}

static char *FixupLibrarys( char *p, unsigned library_count )
{
    struct library_list *lib;
    unsigned            len;

    HeadLibs = NULL;
    if( library_count != 0 ) {
        lib = (struct library_list *)p;
        HeadLibs = lib;
        for(;;) {
            len = sizeof(struct library_list) + strlen( lib->name );
            len = (len + (sizeof(int) - 1)) & - sizeof(int);
            p += len;
            lib->next = (struct library_list *)p;
            --library_count;
            if( library_count == 0 ) break;
            lib = (struct library_list *)p;
        }
        lib->next = NULL;
    }
    return( p );
}

static char *FixupSegInfo( char *p, unsigned seg_count )
{
    struct textsegment  *seg;
    unsigned            len;

    TextSegArray = (struct textsegment **)CMemAlloc( (seg_count+1) *
                                    sizeof(struct textsegment *) );
    TextSegArray[0] = NULL;
    if( seg_count != 0 ) {
        while( seg_count != 0 ) {
            seg = (struct textsegment *)p;
            TextSegArray[ seg->index + 1 ] = seg;
            len = seg->textsegment_len;
            p += len;
            seg->next = (struct textsegment *)p;
            --seg_count;
        }
        seg->next = NULL;
    }
    return( p );
}

static char *FixupMacros( char *p, unsigned macro_count )
{
    int         i;
    MEPTR       mentry;
    unsigned    mentry_len;

    while( macro_count != 0 ) {
        mentry = (MEPTR)p;
        i = mentry->macro_index;                // get hash index
        mentry->next_macro = PCHMacroHash[i];
        PCHMacroHash[i] = mentry;
        mentry_len = (mentry->macro_len + (sizeof(int) - 1)) & - sizeof(int);
        p += mentry_len;
        --macro_count;
    }
    return( p );
}

static char *FixupUndefMacros( char *p, unsigned undef_macro_count )
{   // Read in as written out
    MEPTR       mentry, *lnk;
    unsigned    mentry_len;

    PCHUndefMacroList = NULL;
    lnk = &PCHUndefMacroList;
    while( undef_macro_count != 0 ) {
        mentry = (MEPTR)p;
        *lnk = mentry;
        lnk = &mentry->next_macro;
        mentry_len = (mentry->macro_len + (sizeof(int) - 1)) & - sizeof(int);
        p += mentry_len;
        --undef_macro_count;
    }
    *lnk = NULL;
    return( p );
}

static int VerifyMacros( char *p, unsigned macro_count, unsigned undef_count )
{
    int         i;
    MEPTR       mpch;
    MEPTR       mcur;
    int         macro_compare;

    PCHMacroHash = (MEPTR *)CMemAlloc( MACRO_HASH_SIZE * sizeof(MEPTR) );
    p = FixupMacros( p, macro_count );
    p = FixupUndefMacros( p, undef_count );
    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        MEPTR       prev_mpch;

        prev_mpch = NULL;
        for( mpch = PCHMacroHash[i]; mpch; mpch = mpch->next_macro ) {
            if( mpch->macro_flags & MACRO_DEFINED_BEFORE_FIRST_INCLUDE ) {
                mcur = MacHash[ i ];
                while( mcur != NULL ) {
                    if( strcmp( mcur->macro_name, mpch->macro_name ) == 0 ) {
                        macro_compare = MacroCompare( mpch, mcur );
                        if( mpch->macro_flags & MACRO_REFERENCED ) {
                            if( macro_compare == 0 )  break;
                            return( -1 );       // abort: macros different
                        }
                        if( macro_compare != 0 ) { /* if different */
                            /* delete macro from pch, add new one */
                            if( prev_mpch == NULL ) {
                                PCHMacroHash[i] = mpch->next_macro;
                            } else {
                                prev_mpch->next_macro = mpch->next_macro;
                            }
                        }
                        break;
                    }
                    mcur = mcur->next_macro;
                }
                if( mcur == NULL ) {  /* macro not found in current compile */
                    if( mpch->macro_flags & MACRO_REFERENCED ) {
                        return( -1 );   // abort: macro definition required
                    }
                    // delete macro from PCH list
                    if( prev_mpch == NULL ) {
                        PCHMacroHash[i] = mpch->next_macro;
                    } else {
                        prev_mpch->next_macro = mpch->next_macro;
                    }
                }
            }
            prev_mpch = mpch;
        }
    }
    // check if any user-defined macros have been defined that were not
    // defined when the pre-compiled header file was created
    //
    // for all macros in current compilation unit
    // - if it is present in the PCH header file set of macros
    // -- if not defined before first include
    // --- abort:
    // -- endif
    // - else macro NOT in PCH
    // -- if the macro is a user-defined macro
    // --- abort:
    // -- endif
    // - endif
    // endloop
    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        for( mcur = MacHash[i]; mcur; mcur = mcur->next_macro ) {
            for( mpch = PCHMacroHash[i]; mpch; mpch = mpch->next_macro ) {
                if( strcmp( mpch->macro_name, mcur->macro_name ) == 0 ) break;
            }
            if( mpch == NULL || !(mpch->macro_flags & MACRO_DEFINED_BEFORE_FIRST_INCLUDE) ){
            // macro may either have been undef'd (mpch == NULL ) or undef'd and defined
                if( mcur->macro_flags & MACRO_USER_DEFINED ){ //compiler defined macros not saved on undefs
                    for( mpch = PCHUndefMacroList; mpch; mpch = mpch->next_macro ) {
                        if( strcmp(mpch->macro_name,mcur->macro_name) == 0 ){
                            if( MacroCompare( mpch, mcur ) != 0 ){
                                return( -1 );
                            }else{
                                break;
                            }
                        }
                    }
                    if( mpch == NULL ) {
                        // abort: macro wasn't defined before include
                       return( -1 );
                    }
                }
            }
        }
    }

    for( i = 0; i < MACRO_HASH_SIZE; ++i ) {
        MEPTR       next_mcur;

        for( mcur = MacHash[i]; mcur; mcur = next_mcur ) {
            for( mpch = PCHMacroHash[i]; mpch; mpch = mpch->next_macro ) {
                if( strcmp( mpch->macro_name, mcur->macro_name ) == 0 ) break;
            }
            next_mcur = mcur->next_macro;
            if( mpch == NULL ) {          // if this macro not found in PCH
                mcur->next_macro = PCHMacroHash[i];   // add it to PCH
                PCHMacroHash[i] = mcur;
            }
        }
    }
    memcpy( MacHash, PCHMacroHash, MACRO_HASH_SIZE * sizeof(MEPTR) );
    CMemFree( PCHMacroHash );
    PCHMacroHash = NULL;
    UndefMacroList = PCHUndefMacroList;
    return( 0 );
}

static char *FixupSymHashTable( char *p, unsigned symhash_count )
{
    SYM_HASHPTR hsym;
    int         i;
    unsigned    len;

    while( symhash_count != 0 ) {
        hsym = (SYM_HASHPTR)p;
        i = hsym->hash_index;
        hsym->next_sym = HashTab[i];
        HashTab[i] = hsym;
        if( hsym->sym_type_index != 0 ) {
            hsym->sym_type = TypeArray + hsym->sym_type_index;
        }
        len = strlen( hsym->name ) + sizeof(struct sym_hash_entry);
        len = (len + (sizeof(int) - 1)) & - sizeof(int);
        p += len;
        --symhash_count;
    }
    return( p );
}

static char *FixupSymbols( char *p, unsigned symbol_count )
{
    SYMPTR      symptr;
    SYM_ENTRY   sym;
    SYM_HANDLE  sym_handle;

    sym_handle = 0;
    while( symbol_count != 0 ) {
        symptr = (SYMPTR)p;
        if( symptr->sym_type_index != 0 ) {
            symptr->sym_type = TypeArray + symptr->sym_type_index;
        }
        symptr->seginfo = TextSegArray[ symptr->seginfo_index ];
        PCH_SymArray[ sym_handle ] = symptr;
        p += sizeof(SYM_ENTRY);
        ++sym_handle;
        --symbol_count;
    }
    for( sym_handle = 0; sym_handle < SpecialSyms; sym_handle++ ) {
        SymGet( &sym, sym_handle );  // Redo special syms
        symptr =  PCH_SymArray[ sym_handle ];
        *symptr = sym;
    }
    return( p );
}

static void FixupTypeIndexes( struct type_indices *typ_index ) /* 02-jan-95 */
{
    int         i;
    int         index;

    for( i = TYPE_CHAR; i <= TYPE_DOT_DOT_DOT; i++ ) {
        index = typ_index->basetype_index[i];
        if( index != 0 ) {
            BaseTypes[i] = TypeArray + index;
        }
    }
    VoidParmList[0] = BaseTypes[TYPE_VOID];
    StringType    = TypeArray + typ_index->stringtype_index;
    ConstCharType = TypeArray + typ_index->constchartype_index;
}

static char *FixupTypes( char *p, unsigned type_count )
{
    TYPEPTR     typ;
    int         index;
    struct array_info *array;
    union parmtype {
        TYPEPTR parm_typ;
        int     type_index;
    } *parm_list;

    InitTypeHashTables();
    typ = (TYPEPTR)p;
    TypeArray = typ - 1;
    while( type_count != 0 ) {
        if( typ->decl_type == TYPE_FUNCTION )  break;
        if( typ->object_index != 0 ) {
            typ->object = &TypeArray[ typ->object_index ];
        }
        if( typ->decl_type == TYPE_ARRAY ) {
            array = (struct array_info *)CMemAlloc(sizeof(struct array_info));
            array->dimension = typ->u.array_dimension;
            typ->u.array = array;
        }
        AddTypeHash( typ );
        ++typ;
        --type_count;
    }
    parm_list = (union parmtype *)(typ + type_count);
    while( type_count != 0 ) {
        index = typ->u.parm_index;
        typ->next_type = FuncTypeHead[ index ];
        FuncTypeHead[ index ] = typ;
        if( typ->object_index != 0 ) {
            typ->object = &TypeArray[ typ->object_index ];
        }
        if( parm_list->type_index == -1 ) {
            typ->u.parms = NULL;
        } else {
            typ->u.parms = (TYPEPTR *)parm_list;
            for(;;) {
                index = parm_list->type_index;
                if( index == -1 ) break;
                parm_list->parm_typ = TypeArray + index;
                parm_list++;
            }
            parm_list->parm_typ = NULL;
        }
        parm_list++;
        ++typ;
        --type_count;
    }
    FixupTypeIndexes( (struct type_indices *)parm_list );
    return( (char *)parm_list + sizeof(struct type_indices) );
}

static char *FixupEnums( char *p, TAGPTR parent )
{
    ENUMPTR     ep;

    for(;;) {
        ep = (ENUMPTR)p;
        p += ep->enum_len;
        ep->parent = parent;            // parent is union'ed with enum_len
        ep->next_enum = EnumTable[ ep->hash ];
        EnumTable[ ep->hash ] = ep;
        if( ep->thread == NULL ) break;
        ep->thread = (ENUMPTR)p;
    }
    return( p );
}

static char *FixupFields( char *p )
{
    FIELDPTR    field;
    int         len;

    for(;;) {
        field = (FIELDPTR)p;
        field->field_type = TypeArray + field->field_type_index;
        len = field->field_len;
        p += len;
        field->next_field = (FIELDPTR)p;
        if( len < 0 ) break;
    }
    field->next_field = NULL;
    p -= len + len;
    return( p );
}

static void FixupTag( TYPEPTR typ )
{
    switch( typ->decl_type ) {
    case TYPE_STRUCT:
    case TYPE_UNION:
    case TYPE_ENUM:
        typ->u.tag = TagArray[ typ->u.tag_index ];
        break;
    }
}

static void FixupTagPointers()
{
    WalkTypeList( FixupTag );
}

static char *FixupTags( char *p, unsigned tag_count )
{
    TAGPTR      tag;
    TAGPTR      prevtag;
    TAGPTR      nexttag;
    TYPEPTR     typ;
    unsigned    len;

    if( tag_count != 0 ) {
        TagArray = (TAGPTR *)CMemAlloc( tag_count * sizeof(TAGPTR) );
    }
    tag = NULL;
    prevtag = NULL;
    while( tag_count != 0 ) {
        tag = (TAGPTR)p;
        TagArray[ tag->tag_index ] = tag;
        typ = TypeArray + tag->sym_type_index;
        tag->sym_type = typ;
        len = strlen( tag->name ) + sizeof(TAGDEFN);
        len = (len + (sizeof(int) - 1)) & - sizeof(int);
        p += len;
        if( typ->decl_type == TYPE_ENUM ) {
            if( tag->u.enum_list != NULL ) {
                tag->u.enum_list = (ENUMPTR)p;
                p = FixupEnums( p, tag );
            }
        } else {
            if( tag->u.field_list != NULL ) {
                tag->u.field_list = (FIELDPTR)p;
                p = FixupFields( p );
            }
        }
        tag->next_tag = prevtag;
        prevtag = tag;
        --tag_count;
    }
    for( tag = prevtag; tag; tag = nexttag ) {
        nexttag = tag->next_tag;
        tag->next_tag = TagHash[ tag->hash ];
        TagHash[ tag->hash ] = tag;
    }
    FixupTagPointers();
    return( p );
}

#if _MACHINE == _PC
static char *FixupAuxInfo( char *p, struct aux_info *info )
{
    unsigned            len;
    unsigned            codelen;

    p += sizeof( struct aux_info );
    len = info->parms_size;
    if( len != 0 ) {
        info->parms = (hw_reg_set *)p;
        p += len;
    }
    len = info->objname_size;
    if( len != 0 ) {
        info->objname = (char *)p;
        p += len;
    }
    codelen = info->code_size;
    if( codelen != 0 ) {
        info->code = (byte_seq *)p;
        p += codelen;
    }
    len += codelen;
    p += ((len + (sizeof(int) - 1)) & - sizeof(int)) - len;
    return( p );
}

static char *FixupPragmaInfo( char *p, unsigned pragma_count )
{
    struct aux_entry    *ent;
    struct aux_info     *info;
    struct aux_info     **info_array;
    int                 index;
    unsigned            len;

    for( index = 0; info = BuiltinInfos[index]; ++index ) {
        memcpy( info, p, sizeof(struct aux_info) );
        p = FixupAuxInfo( p, info );
    }
    if( pragma_count == 0 )  return( p );
    info_array = (struct aux_info **)
                        CMemAlloc( pragma_count * sizeof(struct aux_info *) );
    index = 0;
    while( pragma_count != 0 ) {
        info = (struct aux_info *)p;
        info_array[index++] = info;
        p = FixupAuxInfo( p, info );
        --pragma_count;
    }
    AuxList = (struct aux_entry *)p;
    for(;;) {
        ent = (struct aux_entry *)p;
        len = sizeof( struct aux_entry ) + strlen( ent->name );
        len = (len + (sizeof(int) - 1)) & - sizeof(int);
        p += len;
        ent->info = info_array[ ent->aux_info_index ];
        if( ent->next == NULL ) break;
        ent->next = (struct aux_entry *)p;
    }
    CMemFree( info_array );
    return( p );
}
#endif

void FixupFNames( void ){
    FNAMEPTR    *lnk;
    FNAMEPTR    flist;

    lnk = &FNames;
    while( (flist = *lnk) != NULL ){
        lnk  = &flist->next;
    }
    *lnk = FNameList;
}

int ValidHeader( struct pheader *pch )
{
    if( pch->signature      == 'WPCH'           &&
        pch->version        == PCH_VERSION      &&
        pch->size_of_header == sizeof(struct pheader)  &&
        pch->size_of_int    == TARGET_INT       &&
        pch->specialsyms_count    == SpecialSyms      &&
        pch->pack_amount    == PackAmount ) {
        return( 1 );
    }
    return( 0 );                // indicate unusable pre-compiled header
}

int LoadPreCompiledHeader( char *p, struct pheader *pch )
{
    int                 rc;

    rc = FixupDataStructures( p, pch );
    CMemFree( TagArray );
    CMemFree( TextSegArray );
    CMemFree( PCHMacroHash );
    return( rc );
}

static int FixupDataStructures( char *p, struct pheader *pch )
{
    p = FixupLibrarys( p, pch->library_count );
    p = FixupSegInfo( p, pch->seg_count );
    p = FixupTypes( p, pch->type_count );
    p = FixupTags( p, pch->tag_count );
#if _MACHINE == _PC
    p = FixupPragmaInfo( p, pch->pragma_count );
#endif
    p = FixupSymHashTable( p, pch->symhash_count );
    p = FixupSymbols( p, pch->symbol_count );
    if( pch->msgflags_len != 0 ) {                      /* 06-jul-94 */
        MsgFlags = p;
        p += pch->msgflags_len;
    }
    PCH_MaxSymHandle = pch->symbol_count;
    NextSymHandle = pch->symbol_count - 1;
    IncLineCount = pch->incline_count;
    Toggles = pch->toggles;
    FixupFNames();
    InitDebugTypes();
    InitDebugTags();
    return( 0 );
}

int SameCWD( char *p )
{
    char        *cwd;
    int         same;

    cwd = getcwd( NULL, 0 );
    same = strcmp( cwd, p );
    free( cwd );
    return( same == 0 );
}

void FreePreCompiledHeader( void ){
    FEfree( PCH_Start );
    FEfree( PCH_Macros );
    FEfree( PCH_SymArray );
}

void AbortPreCompiledHeader( void )
{
    FreePreCompiledHeader();
    CMemFree( TagArray );
    CMemFree( TextSegArray );
    CMemFree( PCHMacroHash );
    PCH_Start = NULL;
    PCH_End = NULL;
    PCH_Macros = NULL;
    PCH_SymArray = NULL;
    PCH_MaxSymHandle = 0;
    TagArray = NULL;
    TextSegArray = NULL;
    FNameList = NULL;
    IncFileList = NULL;
    PCHMacroHash = NULL;
    CompFlags.make_precompiled_header = 1;      // force new PCH to be created
}

//========================================================================
//      This portion of the code checks to see if we can use the
//      existing pre-compiled header.
//      - all the predefined macros have the same definition
//      - all the include files are the same and have not been modified
//========================================================================

int UsePreCompiledHeader( char *filename )
{
    int                 handle;
    unsigned            len;
    char                *p;
    struct pheader      pch;

    handle = sopen( PCH_FileName, O_RDONLY|O_BINARY, SH_DENYWR );
    if( handle == -1 ) {
        CompFlags.make_precompiled_header = 1;
        return( -1 );
    }
    PCH_Start = NULL;
    TextSegArray = NULL;
    TagArray = NULL;
    PCHMacroHash = NULL;
    len = read( handle, &pch, sizeof(struct pheader) );
    if( len != sizeof(struct pheader) ) {
        close( handle );
        PCHNote( PCHDR_READ_ERROR );
        AbortPreCompiledHeader();
        return( -1 );
    }
    if( !ValidHeader( &pch ) ) {
        close( handle );
        PCHNote( PCHDR_INVALID_HEADER );
        AbortPreCompiledHeader();
        return( -1 );
    }
    if( pch.gen_switches != GenSwitches  ||
        pch.target_switches != TargetSwitches ) {
        close( handle );
        PCHNote( PCHDR_DIFFERENT_OPTIONS );
        AbortPreCompiledHeader();
        return( -1 );
    }
    p = FEmalloc( pch.size );                   // allocate big memory block
    PCH_Start = p;
    PCH_End = p + pch.size;
    PH_size = read( handle, p, pch.size );      // read rest of the file
    PCH_Macros = FEmalloc( pch.macro_size );
    len = read( handle, PCH_Macros, pch.macro_size );
    close( handle );
    PCH_SymArray = (SYMPTR *)FEmalloc( pch.symbol_count * sizeof(SYMPTR) );
    if( PH_size != pch.size  ||  len != pch.macro_size ) {
        PCHNote( PCHDR_READ_ERROR );
        AbortPreCompiledHeader();
        return( -1 );
    }
    if( ! SameCWD( p ) ) {
        PCHNote( PCHDR_DIFFERENT_CWD );
        AbortPreCompiledHeader();
        return( -1 );
    }
    p = FixupIncludes( p + pch.cwd_len, pch.file_count );
    p = FixupRoDirList( p, pch.rdir_count );
    if( VerifyIncludes() ){
        AbortPreCompiledHeader();
        return( -1 );
    }
    len = strlen( p ) + 1;              // get length of saved HFileList
    len = (len + sizeof(int) - 1) & - sizeof(int);
    if( strcmp( p, HFileList ) != 0 ) {
        PCHNote( PCHDR_INCFILE_DIFFERENT );
        AbortPreCompiledHeader();
        return( -1 );
    }
    p = FixupIncFileList( p + len, pch.incfile_count );
    if( strcmp( filename, IncFileList->filename ) != 0 ) {
        PCHNote( PCHDR_INCPATH_CHANGED );
        AbortPreCompiledHeader();
        return( -1 );                           // can't use PCH
    }
    if( VerifyMacros(PCH_Macros,pch.macro_count,pch.undef_macro_count) != 0) {
        PCHNote( PCHDR_MACRO_CHANGED );
        AbortPreCompiledHeader();
        return( -1 );
    }
    LoadPreCompiledHeader( p, &pch );
    PCH_FileName = NULL;
    return( 0 );
}

void SetDebugType( TYPEPTR typ )
{
    typ->debug_type = DBG_NIL_TYPE;
}

void SetFuncDebugType( TYPEPTR typ, int index )
{
    index;
    typ->debug_type = DBG_NIL_TYPE;
}

void InitDebugTypes()
{
    WalkTypeList( SetDebugType );
    WalkFuncTypeList( SetFuncDebugType );
}
