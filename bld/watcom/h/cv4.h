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
* Description:  CodeView debugging information, CV4 format.
*
****************************************************************************/


#define CV_OMF_SIG          0x00000001
#define CV_ALIGN            4
#define CV_FIRST_USER_TYPE  0x1000

#include "watcom.h"
typedef unsigned_8      u1;     /* short forms for read/written types */
typedef unsigned_16     u2;
typedef unsigned_32     u4;
typedef unsigned_64     u8;
typedef signed_8        i1;
typedef signed_16       i2;
typedef signed_32       i4;
typedef signed_64       i8;

/*
 * here are structures used in the executables.
*/


#define CV_SIG_SIZE     4
#define CV4_NB05        "NB05"
#define CV4_NB07        "NB07"
#define CV4_NB08        "NB08"
#define CV4_NB09        "NB09"
#define CV4_NB10        "NB10"
#define CV4_NB11        "NB11"

/* leaf type codes as defined in CV4 doc 3.1 */

typedef enum  lf_values {   /* type enumerations */
/* predefined numeric types  add as needed */
    LF_TNOTYPE       =0x0000,
    LF_TSEGMENT      =0x0002,
    LF_TVOID         =0x0003,
    LF_TCHAR         =0x0010,
    LF_TSHORT        =0x0011,
    LF_TRCHAR        =0x0070,
    LF_TINT4         =0x0074,
#define _LFMAC( n, N, c )    LF_##N = c,
    #include "cv4types.h"
#undef _LFMAC
#define _LFNUM( n, N, c )    LF_##N = c,
    #include "cv4nums.h"
#undef _LFNUM
#undef _LFMAC
#define _LFPAD( n, N, c )    LF_##N = c,
    #include "cv4pads.h"
#undef _LFPAD
} lf_values;

/* leaf symbol codes as defined in CV4 doc 3.1 */

typedef enum s_values { /* symbol enumerations */
    #define SLMAC( n, N, c )    S_##N = c,
    #include "cv4syms.h"
    #undef SLMAC
} s_values;

#include "pushpck1.h"

typedef enum {
    sstModule=0x120,
    sstTypes,
    sstPublic,
    sstPublicSym,
    sstSymbols,
    sstAlignSym,
    sstSrcLnSeg,
    sstSrcModule,
    sstLibraries,
    sstGlobalSym,
    sstGlobalPub,
    sstGlobalTypes,
    sstMPC,
    sstSegMap,
    sstSegName,
    sstPreComp,
    sstPreCompMap,
    sstOffsetMap16,
    sstOffsetMap32,
    sstFileIndex,
    sstStaticSym
} sst;

typedef struct {
    char        sig[CV_SIG_SIZE];
    signed_32   offset;
} cv_trailer;

typedef struct {
    unsigned_16 cbDirHeader;
    unsigned_16 cbDirEntry;
    unsigned_32 cDir;
    unsigned_32 lfoNextDir;
    unsigned_32 flags;
} cv_subsection_directory;

typedef struct {
    unsigned_16 subsection;
    unsigned_16 iMod;
    unsigned_32 lfo;
    unsigned_32 cb;
} cv_directory_entry;

typedef struct {
    unsigned_16 Seg;
    unsigned_16 pad;
    unsigned_32 offset;
    unsigned_32 cbSeg;
} cv_seginfo;

#define CV_DEBUG_STYLE ('V' << 8 | 'C')

typedef struct {
    unsigned_16 ovlNumber;
    unsigned_16 iLib;
    unsigned_16 cSeg;
    unsigned_16 Style;
    cv_seginfo  SegInfo[1];
//  char        name[];
} cv_sst_module;

typedef struct {
    unsigned_16 offset;
    unsigned_16 seg;
    unsigned_16 type;
    char        name[1];
} cv_sst_public_16;

typedef struct {
    unsigned_32 offset;
    unsigned_16 seg;
    unsigned_16 type;
    char        name[1];
} cv_sst_public_32;

typedef struct {
//    char      name[1];
    unsigned_16 seg;
    unsigned_16 cPair;
//      line_offset_parms[1];
} cv_sst_src_lne_seg;

typedef struct {
    unsigned_16 cFile;
    unsigned_16 cSeg;
    unsigned_32 baseSrcFile[1];
//  unsigned_64 start_end[];
//  unsigned_16 seg[];
//  char        pad[];
} cv_sst_src_module_header;

typedef struct {
    unsigned_16 cSeg;
    unsigned_16 pad;
    unsigned_32 baseSrcLn[1];
//  unsigned_64 start_end[];
//  unsigned_16 cbName;
//  char        name[];
} cv_sst_src_module_file_table;

typedef struct {
    unsigned_16 Seg;
    unsigned_16 cPair;
    unsigned_32 offset[1];
//  unsigned_16 linenumber[];
//  char        pad[];
} cv_sst_src_module_line_number;

typedef struct {
    unsigned_16 symhash;
    unsigned_16 addrhash;
    unsigned_32 cbSymbol;
    unsigned_32 cbSymHash;
    unsigned_32 cbAddrHash;
} cv_sst_global_pub_header;

typedef struct {
    unsigned_32 flags;
    unsigned_32 cType;        /* number of types */
    unsigned_32 offType[1];   /*[cType] */
} cv_sst_global_types_header;

struct seg_desc_flags {
    unsigned_16 fRead:1;
    unsigned_16 fWrite:1;
    unsigned_16 fExecute:1;
    unsigned_16 f32Bit:1;
    unsigned_16 res3:4;
    unsigned_16 fSel:1;
    unsigned_16 fAbs:1;
    unsigned_16 res2:2;
    unsigned_16 fGroup:1;
    unsigned_16 res:3;
};

typedef struct {
    union {
        struct seg_desc_flags   b;
        unsigned_16             flags;
    } u;
    unsigned_16 ovl;
    unsigned_16 group;
    unsigned_16 frame;
    unsigned_16 iSegName;
    unsigned_16 iClassName;
    unsigned_32 offset;
    unsigned_32 cbseg;
} seg_desc;

typedef struct {
    unsigned_16 cSeg;
    unsigned_16 cSegLog;
    seg_desc    segdesc[1];
} cv_sst_seg_map;

#include "poppck.h"
