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
* Description:  CodeView debugging information types.
*
****************************************************************************/


#define CV_MAX_REC 256
#if _TARGET & _TARG_IAPX86
    #define CVSIZE 16  // 16:16 compiler
#elif _TARGET & (_TARG_80386 | _TARG_AXP | _TARG_PPC | _TARG_MIPS)
    #define CVSIZE 32  // 16:32 or 0:32 compiler
#endif
typedef struct cv_out {
    byte        *beg;  /* where to start writing from */
    byte        *ptr;  /* end of data */
    segment_id  seg;   /* segment to write to */
    byte        buff[CV_MAX_REC];  /* buffer to use */
}cv_out;

#include "cv4w.h"

/**** leaf type generators ****/
typedef enum lfg_index {
#define _LFMAC( n, N, c )    LFG_##N,
#include "cv4types.h"
#undef _LFMAC
    LFG_LAST
}lfg_index;

/********* Symbol generators ***********/
typedef enum sg_index {
    #define SLMAC( n, N, c )    SG_##N,
    #include "cv4syms.h"
    #undef SLMAC
    SG_LAST
}sg_index;

#if CVSIZE == 16
    #define S_BPREL     S_BPREL16
    #define S_LDATA     S_LDATA16
    #define S_GDATA     S_GDATA16
    #define S_PUB       S_PUB16
    #define S_LPROC     S_LPROC16
    #define S_GPROC     S_GPROC16
    #define S_THUNK     S_THUNK16
    #define S_BLOCK     S_BLOCK16
    #define S_WITH      S_WITH16
    #define S_LABEL     S_LABEL16
    #define S_CEXMODEL  S_CEXMODEL16
    #define S_VFTPATH   S_VFTPATH16
    #define S_REGREL    S_REGREL16
#elif CVSIZE == 32
    #define S_BPREL     S_BPREL32
    #define S_LDATA     S_LDATA32
    #define S_GDATA     S_GDATA32
    #define S_PUB       S_PUB32
    #define S_LPROC     S_LPROC32
    #define S_GPROC     S_GPROC32
    #define S_THUNK     S_THUNK32
    #define S_BLOCK     S_BLOCK32
    #define S_WITH      S_WITH32
    #define S_LABEL     S_LABEL32
    #define S_CEXMODEL  S_CEXMODEL32
    #define S_VFTPAT    S_VFTPATH3
    #define S_REGREL    S_REGREL32
#endif
#if CVSIZE == 16
    #define SG_BPREL     SG_BPREL16
    #define SG_LDATA     SG_LDATA16
    #define SG_GDATA     SG_GDATA16
    #define SG_PUB       SG_PUB16
    #define SG_LPROC     SG_LPROC16
    #define SG_GPROC     SG_GPROC16
    #define SG_THUNK     SG_THUNK16
    #define SG_BLOCK     SG_BLOCK16
    #define SG_WITH      SG_WITH16
    #define SG_LABEL     SG_LABEL16
    #define SG_CEXMODEL  SG_CEXMODEL16
    #define SG_VFTPATH   SG_VFTPATH16
    #define SG_REGREL    SG_REGREL16
#elif CVSIZE == 32
    #define SG_BPREL     SG_BPREL32
    #define SG_LDATA     SG_LDATA32
    #define SG_GDATA     SG_GDATA32
    #define SG_PUB       SG_PUB32
    #define SG_LPROC     SG_LPROC32
    #define SG_GPROC     SG_GPROC32
    #define SG_THUNK     SG_THUNK32
    #define SG_BLOCK     SG_BLOCK32
    #define SG_WITH      SG_WITH32
    #define SG_LABEL     SG_LABEL32
    #define SG_CEXMODEL  SG_CEXMODEL32
    #define SG_VFTPATH   SG_VFTPATH32
    #define SG_REGREL    SG_REGREL32
#endif

#if CVSIZE == 16
    #define s_bprel   s_bprel16
    #define s_ldata   s_ldata16
    #define s_gdata   s_gdata16
    #define s_pub     s_pub16
    #define s_lproc   s_lproc16
    #define s_gproc   s_gproc16
    #define s_thunk   s_thunk16
    #define s_block   s_block16
    #define s_with    s_with16
    #define s_label   s_label16
    #define s_cexmode s_cexmodel16
    #define s_vftpath s_vftpath16
    #define s_regrel  s_regrel16
    typedef u2        fsize;
#elif CVSIZE == 32
    #define s_bprel   s_bprel32
    #define s_ldata   s_ldata32
    #define s_gdata   s_gdata32
    #define s_pub     s_pub32
    #define s_lproc   s_lproc32
    #define s_gproc   s_gproc32
    #define s_thunk   s_thunk32
    #define s_block   s_block32
    #define s_with    s_with32
    #define s_label   s_label32
    #define s_cexmode s_cexmodel32
    #define s_vftpath s_vftpath32
    #define s_regrel  s_regrel32
    typedef u4        fsize;
#endif
#if CVSIZE == 16
    #define cs_bprel   cs_bprel16
    #define cs_ldata   cs_ldata16
    #define cs_gdata   cs_gdata16
    #define cs_pub     cs_pub16
    #define cs_lproc   cs_lproc16
    #define cs_gproc   cs_gproc16
    #define cs_thunk   cs_thunk16
    #define cs_block   cs_block16
    #define cs_with    cs_with16
    #define cs_label   cs_label16
    #define cs_cexmode cs_cexmodel16
    #define cs_vftpath cs_vftpath16
    #define cs_regrel  cs_regrel16
#elif CVSIZE == 32
    #define cs_bprel   cs_bprel32
    #define cs_ldata   cs_ldata32
    #define cs_gdata   cs_gdata32
    #define cs_pub     cs_pub32
    #define cs_lproc   cs_lproc32
    #define cs_gproc   cs_gproc32
    #define cs_thunk   cs_thunk32
    #define cs_block   cs_block32
    #define cs_with    cs_with32
    #define cs_label   cs_label32
    #define cs_cexmode cs_cexmodel32
    #define cs_vftpath cs_vftpath32
    #define cs_regrel  cs_regrel32
#endif
typedef union {    /* swiped from cvdump/h/cvin.h   */
   char           *a_data;
   ct_bclass      *a_bclass;
   ct_vbclass     *a_vbclass;
   ct_ivbclass    *a_ivbclass;
   ct_enumerate   *a_enumerate;
   ct_friendfcn   *a_friendfcn;
   ct_index       *a_index;
   ct_member      *a_member;
   ct_stmember    *a_stmember;
   ct_method      *a_method;
   ct_nestedtype  *a_nestedtype;
   ct_vfunctab    *a_vfunctab;
   ct_friendcls   *a_friendcls;
   ct_onemethod   *a_onemethod;
   ct_vfuncoff    *a_vfuncoff;
}ct_subfield_ptrs;

typedef enum {
    BASED_SELF,
    BASED_KIND,
    BASED_VOID,
    BASED_SEG,
    BASED_VALUE,
}cv_based_kind;

extern segment_id    CVSyms;
extern segment_id    CVTypes;
