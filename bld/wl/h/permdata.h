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



#define INC_FILE_SIG_SIZE        36
#ifdef __QNX__
#define INC_FILE_SIG  "WLINK Incremental Link File V1.02\n\x0c\x04"
#else
#define INC_FILE_SIG  "WLINK Incremental Link File V1.02\r\n\x1a"
#endif

typedef struct {
    unsigned_32 num;
//    unsigned_32       fileoff;         not currently used
} carve_info;

typedef struct {
     char               signature[INC_FILE_SIG_SIZE];
     unsigned_32        hdrsize;        // size of hdr + small structs
     unsigned_32        strtabsize;     // size of string table
     unsigned_32        relocsize;      // size of all relocs
     unsigned_32        numgroups;      // number of group_entry structures
     carve_info         mods;           // mod_entry info
     carve_info         segdatas;       // segdata info
     carve_info         symbols;        // symbol info
     unsigned_32        altdefsize;     // size of altdef data
     unsigned_32        rootmodidx;     // Root->mods swizzled pointer
     unsigned_32        headsymidx;     // HeadSym swizzled pointer
     unsigned_32        libmodidx;      // LibModules swizzled pointer
     unsigned_32        linkstate;      // value of LinkState
     unsigned_32        exename;        // name of the .exe
     unsigned_32        symname;        // name of the sym file
     unsigned_32        numdllsyms;     // OS/2&PE only: number of dll symbols
     unsigned_32        numexports;     // OS/2&PE only: number of exports
     unsigned_32        startmodidx;    // start module swizzled pointer
     unsigned_32        startidx;       // swizzled pointer to start object
     unsigned_32        startoff;       // delta from object of start addr.
     unsigned_32        flags;          // miscellaneous flags
     unsigned_32        numuserlibs;    // number of user specified libs
     unsigned_32        numdeflibs;     // number of default libs
     time_t             exemodtime;     // time exe was written.
     time_t             symmodtime;     // sym file time
} inc_file_header;

#define INC_FLAG_START_SEG      0x0001  // startidx is a segname

typedef struct groupdef {
    struct groupdef *   next;
    unsigned            numsegs;
    char *              names[1];
} incgroupdef;

typedef struct liblist {
    struct liblist *    next;
    unsigned            namelen;
    char                name[1];
} libnamelist;

/* data and functions used for permanent data structure storage */

extern stringtable      PermStrings;
extern carve_t          CarveLeader;
extern carve_t          CarveModEntry;
extern carve_t          CarveDLLInfo;
extern carve_t          CarveExportInfo;
extern carve_t          CarveSymbol;
extern carve_t          CarveSegData;
extern carve_t          CarveClass;
extern carve_t          CarveGroup;
extern char *           IncFileName;
extern incgroupdef *    IncGroupDefs;
extern group_entry **   IncGroups;
extern libnamelist *    SavedUserLibs;
extern libnamelist *    SavedDefLibs;

extern void     ResetPermData( void );
extern void     CleanPermData( void );
extern void     ReadPermData( void );
extern void     WritePermData( void );

extern void     IncP2Start( void );
extern void     PermSaveFixup( void *, unsigned );
extern void     PermStartMod( mod_entry * );
extern void     PermEndMod( mod_entry * );
extern void     WritePermFile( void *, void *, unsigned );
extern void     ReadPermFile( void *, void *, unsigned );
extern void     IterateModRelocs( unsigned,unsigned,unsigned (*)(void *));
extern void *   GetSegContents( segdata *, virt_mem );
extern void *   GetAltdefContents( segdata * );
extern void     FreeSavedRelocs( void );
