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


#include "pushpck1.h"

enum cv_ptype {
        CV_SPECIAL      =0x00,
        CV_SIGNED       =0x01,
        CV_UNSIGNED     =0x02,
        CV_BOOL         =0x03,
        CV_REAL         =0x04,
        CV_COMPLEX      =0x05,
        CV_SPECIAL2     =0x06,
        CV_REALLYINT    =0x07,     /* flag or'd in to make debugger say int */
};

enum cv_psize {
        CV_IB1          =0x00,
        CV_IB2          =0x01,
        CV_IB4          =0x02,
        CV_IB8          =0x03,
        CV_VOID         =0x03,
        CV_RC32         =0x00,
        CV_RC64         =0x01,
};
enum cv_pmode {
        CV_DIRECT       =0x00,
        CV_NEARP        =0x01,
        CV_FARP         =0x02,
        CV_HUGEP        =0x03,
        CV_NEAR32P      =0x04,
        CV_FAR32P       =0x05,
        CV_NEAR64P      =0x06,
};

typedef union cv_primitive{
    struct {
        u2  size:3;
        u2      :1;
        u2  type:4;
        u2  mode:3;
        u2      :5;
    }f;
    u2 s;
}cv_primitive;

enum cv_ptrtype {
    CV_NEAR             = 0,
    CV_FAR              = 1,
    CV_HUGE             = 2,
    CV_BASESEG          = 3,
    CV_BASEVAL          = 4,
    CV_BASESEGVAL       = 5,
    CV_BASESYM          = 6,
    CV_BASESEGSYM       = 7,
    CV_BASETYPE         = 8,
    CV_BASESELF         = 9,
    CV_NEAR32           = 10,
    CV_FAR32            = 11,
};


enum cv_ptrmode  {
    CV_PTR              = 0,
    CV_REF              = 1,
    CV_PTRTOMEMBER      = 2,
    CV_PTRTOMETHOD      = 3,
};

typedef enum { /* 16 - 16bit 32 - 32bit, VB - virtual base class       */
    CV_DM16             = 0,    /*DM - data member no virual functions */
    CV_DM16_VF          = 1,    /* has virtual functions               */
    CV_DMVB16           = 2,    /* has virtual bases                   */
    CV_DM32             = 3,    /* may have virtual functions          */
    CV_DMVB32           = 4,    /*                                     */
    CV_MF16             = 5,    /* MF - member function                */
    CV_MFMA16           = 6,    /* MA - multiple address points        */
    CV_MFVB16           = 7,    /*                                     */
    CV_FMF16            = 8,    /* FMF - far member function           */
    CV_FMFMA16          = 9,    /*                                     */
    CV_FMFVB16          = 10,
    CV_MF32             = 11,
    CV_MFMA32           = 12,
    CV_MFVB32           = 13
}cv_data_mem;

typedef struct { /* 16 virtual base class */
    u2 mdisp;
    u2 pdisp;
    u2 vdisp;
}cv_vb16;

typedef struct { /* 32 virtual base class */
    u4 mdisp;
    u4 pdisp;
    u4 vdisp;
}cv_vb32;

typedef struct { /* far 16 mem func addr */
    u2 off;
    u2 seg;
}cv_fmf16;

typedef struct {
    u2 off;
    u2 disp;
}cv_mfma16;

typedef struct {
    u2        mf;
    cv_vb16   vb;
}cv_mfvb16;

typedef struct {
    cv_fmf16  mf;
    u2        disp;
}cv_fmfma16;

typedef struct {
    cv_fmf16  mf;
    u2        disp;
}cv_fmfvb16;

typedef struct {
    u4        mf;
    u4        disp;
}cv_mfma32;

typedef struct {
    u4        mf;
    cv_vb32   vb;
}cv_mfvb32;

typedef struct{
    u2  class_idx;
    u2  format;
    union {
        u2          dm16;
        u2          dm16_vf;
        cv_vb16     dmvb16;
        u4          dm32;
        cv_vb32     dmvb32;
        u2          mf16;
        cv_mfma16   mfma16;
        cv_mfvb16   mfvb16;
        cv_fmf16    fmf16;
        cv_fmfma16  fmfma16;
        cv_fmfvb16  fmfvb16;
        u4          mf32;
        cv_mfma32   mfma32;
        cv_mfvb32   mfvb32;
    }p;
}cv_ptr_data_member;

typedef union cv_attrib{
    struct {
        u2 isconst :1;
        u2 isvol   :1;
        u2 unalign :1;
        u2 unused  :13;
    }f;
    u2 s;
}cv_attrib;

typedef union cv_ptrattr {
    struct {
        u2 type      :5;
        u2 mode      :3;
        u2 isflat32  :1;
        u2 isvol     :1;
        u2 isconst   :1;
        u2 unaligned :1;
        u2 unused    :4;
    }f;
    u2 s;
}cv_ptrattr;

enum cv_access {
    CV_NOPROTECT    = 0,
    CV_PRIVATE      = 1,
    CV_PROTECTED    = 2,
    CV_PUBLIC       = 3,
};

enum cv_mprop {
    CV_VANILLA          = 0,
    CV_VIRTUAL          = 1,
    CV_STATIC           = 2,
    CV_FRIEND           = 3,
    CV_INTROVIRT        = 4,
    CV_PUREVIRT         = 5,
    CV_PUREINTROVIRT    = 6,
};

typedef union cv_fldattr {
    struct {
        u2 access        :2;
        u2 mprop         :3;
        u2 pseudo        :1;
        u2 noinherit     :1;
        u2 noconstruct   :1;
        u2               :8;
    }f;
    u2  s;
}cv_fldattr;

typedef struct {
    cv_attrib   attr;
    u2          index;
}ct_modifier;

typedef struct {
    cv_ptrattr  attr;
    u2          type;
}ct_pointer;

typedef struct {
    u2          elemtype;
    u2          idxtype;
}ct_array;

typedef union cv_sprop{
    struct {
        u2 packed        :1;
        u2 ctor          :1;
        u2 overops       :1;
        u2 isnested      :1;
        u2 cnested       :1;
        u2 opassign      :1;
        u2 opcast        :1;
        u2 fwdref        :1;
        u2 scoped        :1;
        u2               :7;
    }f;
    u2  s;
}cv_sprop;

typedef struct {  /* struct or class */
    u2          count;
    u2          field;
    cv_sprop    property;
    u2          dList;
    u2          vshape;
}ct_class;

typedef ct_class ct_structure;

typedef struct {
    u2          count;
    u2          field;
    cv_sprop    property;
}ct_union;

typedef struct {
    u2          count;
    u2          type;
    u2          fList;
    cv_sprop    property;
}ct_enum;

typedef enum {
    CV_NEARC        =0,
    CV_FARC         =1,
    CV_NEARPASCAL   =2,
    CV_FARPASCAL    =3,
    CV_NEARFASTCALL =4,
    CV_FARFASTCALL  =5,
    CV_NEARSTDCALL  =7,
    CV_FARSTDCALL   =8,
    CV_NEARSYSCALL  =9,
    CV_FARSYSCALL   =10,
    CV_THISCALL     =11,
    CV_MIPS         =12,
    CV_GENERIC      =13,
    CV_AXP          =14,    // Guessed from what MS puts out

}cv_calls;

typedef struct {
    u2          rvtype;
    u1          call;
    u1          res;
    u2          parms;
    u2          arglist;
}ct_procedure;

typedef enum { /* vtshape bits */
    CV_VTNEAR    = 0,
    CV_VTFAR     = 1,
    CV_VTTHIN    = 2,
    CV_VTAPDISP  = 3,
    CV_VTFPMETA  = 4,
    CV_VTNEAR32  = 5,
    CV_VTFAR32   = 6,
}cv_vtshape;

typedef struct {
    u2          rvtype;
    u2          class_idx;
    u2          thisptr;
    u1          call;
    u1          res;
    u2          parms;
    u2          arglist;
    u4          thisadjust;
}ct_mfunction;

typedef struct {
    u2          count;
}ct_vtshape;

typedef struct {
    u2          parent;
}ct_cobol0;

typedef struct {
    u2          parent;
}ct_cobol1;

typedef struct {
    u2          type;
}ct_barray;

typedef struct {
    u2          mode;
}ct_label;

typedef struct {
    u2          utype;
    u2          diminfo;
}ct_dimarray;

typedef struct {
    u2          count;
}ct_vftpath;

typedef struct {
    u2          start;
    u2          count;
    u4          signature;
}ct_precomp;

typedef struct {
    u4          signature;
}ct_endprecomp;

typedef struct {
    u2          oem;
    u2          recoem;
    u2          count;
}ct_oem;

typedef struct {
    u2          index;
}ct_skip;

typedef struct {
    u2          argcount;
}ct_arglist;

typedef struct {
    u2          index;
}ct_defarg;

typedef struct {
    u2          count;
}ct_derived;

typedef struct {
    u1          length;
    u1          position;
    u2          type;
}ct_bitfield;

typedef struct {
    cv_fldattr  attr;
    u2          type;
    u4          vtab;
}cv_mlist;

typedef struct {
    u2          rank;
    u2          index;
}ct_dimconu;

typedef struct {
    u2          rank;
    u2          index;
}ct_dimconlu;

typedef struct {
    u2          rank;
    u2          index;
}ct_dimvaru;

typedef struct {
    u2          rank;
    u2          index;
}ct_dimvarlu;

typedef struct {
    u2          type;
    cv_fldattr  attr;
}ct_bclass;

typedef struct { /*direct & indirect */
    u2          btype;
    u2          vtype;
    cv_fldattr  attr;
}ct_vbclass;

typedef ct_vbclass ct_ivbclass;

typedef struct {
    cv_fldattr  attr;
}ct_enumerate;

typedef struct {
    u2          type;
}ct_friendfcn;

typedef struct {
    u2          index;
}ct_index;

typedef struct {
    u2          type;
    cv_fldattr  attr;
}ct_member;

typedef struct {
    u2          type;
    cv_fldattr  attr;
}ct_stmember;

typedef struct {
    u2          count;
    u2          mList;
}ct_method;

typedef struct {
    u2          index;
}ct_nestedtype;

typedef struct {
    u2          type;
}ct_vfunctab;

typedef struct {
    u2          type;
}ct_friendcls;

typedef struct {
    cv_fldattr  attr;
    u2          type;
}ct_onemethod;

typedef struct {
    u2          type;
    u4          offset;
}ct_vfuncoff;


typedef enum {
    LANG_C           =0,
    LANG_CPP         =1,
    LANG_FORTRAN     =2,
    LANG_MASM        =3,
    LANG_PASCAL      =4,
    LANG_BASIC       =5,
    LANG_COBOL       =6,
}cv_lang;

typedef enum {
    AMBIENT_NEAR    =0,
    AMBIENT_FAR     =1,
    AMBIENT_HUGE    =2,
}cv_ambient;

typedef enum {
    FLOAT_PACKAGE_HARDWARE  =0,
    FLOAT_PACKAGE_EMULATOR  =1,
    FLOAT_PACKAGE_ALTMATH   =2,
}cv_float_package;

typedef enum {
    MACH_INTEL_8080     = 0x00,
    MACH_INTEL_8086     = 0x01,
    MACH_INTEL_80286    = 0x02,
    MACH_INTEL_80386    = 0x03,
    MACH_INTEL_80486    = 0x04,
    MACH_INTEL_PENTIUM  = 0x05,
    MACH_MIPS_R4000     = 0x10,
    MACH_MC68000        = 0x20,
    MACH_MC68010        = 0x21,
    MACH_MC68020        = 0x22,
    MACH_MC68030        = 0x23,
    MACH_MC68040        = 0x24,
    MACH_DECALPHA       = 0x30,
}cv_machine_class;

typedef union {
    struct {
        u2          PCodePresent:1;
        u2          FloatPrecision:2;
        u2          FloatPackage:2;
        u2          AmbientData:3;
        u2          AmbientCode:3;
        u2          Mode32:1;
        u2          Reserved:4;
    }f;
    u2 s;
}cv_compile;

typedef struct {
    u1          machine;
    u1          language;
    cv_compile  flags;
//  u1          version[1];
} cs_compile;

typedef struct {
    u2          type;
    u2          reg;
//  u1          name[1];
} cs_register;

typedef struct {
    u2          type;
//  u1          value[1];
//  u1          name[1];
} cs_constant;

typedef struct {
    u2          type;
//  u1          name[1];
} cs_udt;

typedef struct {
    u4          sym_off;
    u2          segment;
} cs_ssearch;

typedef struct {
    u4          signature;
//  u1          name[1];
} cs_objname;

typedef struct {
    u2          type;
//  u1          name[1];
} cs_coboludt;

typedef struct {
    u2          type;
    u1          count;
//  u1          reglist[count];
//  u1          name[1];
} cs_manyreg;

typedef union {
    struct {
        u1      cstyle  : 1;
        u1      rsclean : 1;
    } f;
    u2  s;
} cv_return_flags;

typedef enum {
    CVRET_VOID,
    CVRET_DIRECT,
    CVRET_CALLOC_NEAR,
    CVRET_CALLOC_FAR,
    CVRET_RALLOC_NEAR,
    CVRET_RALLOC_FAR
} cv_return_style;

typedef struct {
    cv_return_flags     flags;
    u1                  style;
//  u1                  data;
} cs_return;

typedef struct {
    i2          offset;
    u2          type;
//  u1          name[1];
} cs_bprel16;

typedef struct {
    i2          offset;
    u2          reg;
    u2          type;
//  u1          name[1];
} cs_regrel16;

typedef struct {
    u2          offset;
    u2          segment;
    u2          type;
//  u1          name[1];
} cs_ldata16,cs_gdata16;

typedef struct {
    u2          offset;
    u2          segment;
    u2          type;
//  u1          name[1];
} cs_pub16;

typedef union cv_proc{
    struct {
        u1 fpo          :1;
        u1 interr       :1;
        u1 far_ret      :1;
        u1 never        :1;
        u1 unused       :4;
    }f;
    u1 s;
}cv_proc;

typedef struct {
    u4          pParent;
    u4          pEnd;
    u4          pNext;
    u2          proc_length;
    u2          debug_start;
    u2          debug_end;
    u2          offset;
    u2          segment;
    u2          proctype;
    cv_proc     flags;
//  u1          name[1];
} cs_lproc16,cs_gproc16;

typedef enum {
    NOTYPE     = 0,
    ADJUSTOR   = 1,
    VCALL      = 2,
    PCODE      = 3,
}cv_ordinal;

typedef union {
    struct {
            u2   delta;
            char thisnm[1];
    }adjustor;
    u2 vcall;
    struct {
        u2 offset;
        u2 segment;
    }pcode;
}cv_ord_variant;

typedef struct {
    u4          pParent;
    u4          pEnd;
    u4          pNext;
    u2          offset;
    u2          segment;
    u2          length;
    unsigned_8  ordinal;
    u1          name[1];
//    u1        varient[1];
} cs_thunk16;

typedef struct {
    u4          pParent;
    u4          pEnd;
    u2          length;
    u2          offset;
    u2          segment;
//  u1          name[1];
} cs_block16;

typedef struct {
    u4          pParent;
    u4          pEnd;
    u2          length;
    u2          offset;
    u2          segment;
//  u1          value[1];
} cs_with16;

typedef struct {
    u2          offset;
    u2          segment;
    unsigned_8  near_far;
//  u1          name[1];
} cs_label16;

typedef struct {
    u2          offset;
    u2          segment;
    u2          model;
//  u1          varient[1];
} cs_cexmodel16;

typedef struct {
    u2          offset;
    u2          segment;
    u2          root;
    u2          path;
} cs_vftpath16;

typedef struct {
    i4          offset;
    u2          type;
//  u1          name[1];
} cs_bprel32;

typedef struct {
    i4          offset;
    u2          reg;
    u2          type;
//  u1          name[1];
} cs_regrel32;

typedef struct {
    u4          offset;
    u2          segment;
    u2          type;
//  u1          name[1];
} cs_lthread32, cs_gthread32;

typedef struct {
    u4          offset;
    u2          segment;
    u2          type;
//  u1          name[1];
} cs_ldata32,cs_gdata32;

typedef struct {
    u4          offset;
    u2          segment;
    u2          type;
//  u1          name[1];
} cs_pub32;

typedef struct {
    u4          pParent;
    u4          pEnd;
    u4          pNext;
    u4          proc_length;
    u4          debug_start;
    u4          debug_end;
    u4          offset;
    u2          segment;
    u2          proctype;
    cv_proc     flags;
//  u1          name[1];
} cs_lproc32,cs_gproc32;

typedef struct {
    u4          pParent;
    u4          pEnd;
    u4          pNext;
    u4          offset;
    u2          segment;
    u2          length;
    u2          ordinal;
 //  u1          name[1];
//    u1        varient[1];
} cs_thunk32;

typedef struct {
    u4          pParent;
    u4          pEnd;
    u4          length;
    u4          offset;
    u2          segment;
//  u1          name[1];
} cs_block32;

typedef struct {
    u4          pParent;
    u4          pEnd;
    u4          length;
    u4          offset;
    u2          segment;
//  u1          value[1];
} cs_with32;

typedef struct {
    u4          offset;
    u2          segment;
    unsigned_8  near_far;
//  u1          name[1];
} cs_label32;

typedef struct {
    u4          offset;
    u2          segment;
    u2          model;
//  u1          varient[1];
} cs_cexmodel32;

typedef struct {
    u4          offset;
    u2          segment;
    u2          root;
    u2          path;
} cs_vftpath32;

typedef struct {
    u4          checksum;
    u4          offset;
    u2          module;
} cs_procref, cs_dataref;

typedef struct {
    u4          type;
    u4          offset;
    u2          segment;
//  u1          name[1];
} cs_pub32_new;

#include "poppck.h"
