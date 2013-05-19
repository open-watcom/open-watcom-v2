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
* Description:  Miscellaneous structures and constants.
*
****************************************************************************/


#include "watcom.h"
#include "exedos.h"
#include "exeos2.h"
#include "exeflat.h"
#include "exepe.h"
#include "exenov.h"
#include "exephar.h"
#include "proc386.h"
#include "exeqnx.h"
#include "exeelf.h"
#include "exe16m.h"
#include "dwarf.h"
#include "coff.h"
#include "macho.h"
#include "ar.h"
#include "lib.h"

#define PERLINE                 16
#define NUMLINE                 24
#define LINE_LEN                78
#define SLEN                    30
#define TRUE                    1
#define FALSE                   0
#define STDOUT                  1
#define BSIZE                   512
#define MAX_SECTION_DATA        4096
#define SELECTOR_FIX            0x02
#define table( x )              { x, #x }
#define MAX_BUFF                0xff00
#define bool                    char

#define FIX_DMP         0x0001  /* dump the fixups         */
#define RESRC_DMP       0x0002  /* dump the resources      */
#define DOS_SEG_DMP     0x0004
#define OS2_SEG_DMP     0x0008
#define DEBUG_INFO      0x0010
#define EXE_INFO        0x0020
#define HAS_NEW_EXE     0x0040
#define QUIET           0x0080
#define IMPORT_LIB      0x0100
#define BINARY_DMP      0x0200
#define PAGE_DMP        0x0400  /* dump the page maps       */
#define IMPORT_DEF      0x0800

#define MODULE_INFO     0x01
#define GLOBAL_INFO     0x02
#define ADDR_INFO       0x04
#define LINE_NUMS       0x08
#define LOCALS          0x10
#define TYPES           0x20

extern int                      Handle;         /* the file handle         */
extern int                      Lhandle;        /* the listfile handle     */
extern unsigned_16              Sizeleft;       /* size left from BSIZE    */
extern unsigned_16              WSize;          /* size left from BSIZE    */
extern unsigned_16              Num_buff;       /* number of BSIZE buffer  */
extern char                     Read_buff[BSIZE];       /* BSIZE buffer    */
extern char                     Write_buff[BSIZE];      /* BSIZE buffer    */
extern char                     *Wbuff;         /* buffer          */
extern char                     *Lang_lst;      /* buffer          */
extern int                      Num_read;       /* number of bytes read    */
extern unsigned_32              Data_count;     /* used in OS2, QNX and ELF*/
extern unsigned_32              Curr_sectoff;
extern unsigned_32              Coff_off;       // offset of coff file */
extern unsigned_32              New_exe_off;    /* offset of new exe head  */
extern unsigned_32              Exp_off;        /* offset   */
extern unsigned_32              Imp_off;        /* offset   */
extern unsigned_32              Fix_off;        /* offset   */
extern unsigned_32              Res_off;        /* offset   */
extern unsigned_32              Data_off;       /* offset   */
extern struct segment_record *  Int_seg_tab;    /* internal segment table  */
extern char **                  Int_mod_ref_tab;/* internal mod_ref table  */
extern unsigned_16              Segspec;        /* spec seg # for dmp_seg  */
extern unsigned long            Hexoff;         /* offset for binary dump  */
extern Elf32_Ehdr               Elf_head;       /* the elf_header          */
extern lmf_header               Qnx_head;       /* the qnx_header          */
extern extended_header          Phar_ext_head;  /* the phar_extended_header*/
extern simple_header            Phar_head;      /* the phar_header         */
extern extended_nlm_header      Nlm_ext_head;   /* the nlm_extended_header */
extern nlm_header               Nlm_head;       /* the nlm_header          */
extern pe_header                Pe_head;        /* the pe_header           */
extern struct os2_flat_header   Os2_386_head;   /* the new_header (V2.0)   */
extern struct os2_exe_header    Os2_head;       /* the new_header          */
extern struct dos_exe_header    Dos_head;       /* the old_header          */
extern unsigned_16              Resrc_shift_cnt;/* the restab shift        */
extern unsigned_16              Options_dmp;
extern unsigned_8               Debug_options;
extern char                     *Name;          /* file name               */
extern unsigned_32              Load_len;       /* start of debugging info */
extern unsigned_32              Resrc_end;      /* end of resrc table      */
extern jmp_buf                  Se_env;         /* for the setjmp          */
extern int                      Form;
extern bool                     Byte_swap;      /* endianness flag         */

enum {
    FORM_NE,
    FORM_LE,
    FORM_LX
};

typedef struct {
    unsigned_16 len;
    char        *rname;
} res_name;

struct int_entry_pnt {
        struct int_entry_pnt *  next;
        unsigned_16             ordinal;
        unsigned_8              ent_flag;
        unsigned_16             seg_num;
        unsigned_16             offset;
};

struct relocation_item {
        unsigned_8      addr_type;          /* see below                     */
        unsigned_8      reloc_type;         /* see below                     */
        unsigned_16     reloc_offset;       /* where to put addr in curr seg */
        union externalneeded {              /* where,how to get xtrnl addr   */
            struct internalreference {      /*   in this module              */
                unsigned_8      seg_num;    /*      segment number           */
                unsigned_8      rsrvd;      /*      == 0                     */
                unsigned_16     ord_off;    /*      xtrnl offset in that seg */
            }               internal;
            struct importedordinal {        /*   by ordinal                  */
                unsigned_16     modref_idx; /*      in 'module_name'.dll     */
                unsigned_16     ord_num;    /*      entry point index        */
            }               ordinal;
            struct importedname {           /*   by name                     */
                unsigned_16     modref_idx; /*      in 'module_name'.dll     */
                unsigned_16     impnam_off; /*      with res_name table      */
            }               name;
            unsigned_32 data;
        }               xtrnl;
};

struct section_data {
    uint        cur_offset;
    uint        max_offset;
    unsigned_8  *data;
};

typedef struct {
    uint_32     value;
    char        *name;
} readable_name;

/* the debugging sections */
enum {
    DW_DEBUG_INFO,
    DW_DEBUG_PUBNAMES,
    DW_DEBUG_ARANGES,
    DW_DEBUG_LINE,
    DW_DEBUG_LOC,
    DW_DEBUG_ABBREV,
    DW_DEBUG_MACINFO,
    DW_DEBUG_STR,
    DW_DEBUG_REF,
    DW_DEBUG_MAX
};

typedef enum {
    DR_DEBUG_INFO,
    DR_DEBUG_PUBNAMES,
    DR_DEBUG_ARANGES,
    DR_DEBUG_LINE,
    DR_DEBUG_LOC,
    DR_DEBUG_ABBREV,
    DR_DEBUG_MACINFO,
    DR_DEBUG_STR,
    DR_DEBUG_REF,
    DR_DEBUG_NUM_SECTS          // the number of debug info sections.
} dr_section;

extern struct                   section_data Sections[DR_DEBUG_NUM_SECTS];
