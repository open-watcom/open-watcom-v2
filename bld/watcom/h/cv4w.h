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


#include "cv4.h"            /* common defn for reading or writing codeview */

/* include fixed field defn */
#include "cv4f.h"

#pragma pack( 1 );         /* written out directly so we need packing */

typedef struct lf_common {
    u2          code;
}lf_common;

typedef struct lf_modifier {
    lf_common   common;
    ct_modifier f;
}lf_modifier;

typedef struct lf_pointer {
    lf_common   common;
    ct_pointer  f;
}lf_pointer;

typedef struct lf_array {
    lf_common   common;
    ct_array    f;
}lf_array;


typedef struct lf_class{  /* struct or class */
    lf_common   common;
    ct_class    f;
}lf_class;

typedef lf_class lf_structure;

typedef struct lf_union {
    lf_common   common;
    ct_union    f;
}lf_union;

typedef struct lf_enum {
    lf_common   common;
    ct_enum     f;
}lf_enum;


typedef struct lf_procedure {
    lf_common    common;
    ct_procedure f;
}lf_procedure;

typedef struct lf_mfunction{
    lf_common    common;
    ct_mfunction f;
}lf_mfunction;

typedef struct lf_vtshape  {
    lf_common   common;
    ct_vtshape   f;
}lf_vtshape;

typedef struct lf_cobol0{
    lf_common   common;
    ct_cobol0   f;
}lf_cobol0;

typedef struct lf_cobol1{
    lf_common   common;
    ct_cobol1   f;
}lf_cobol1;

typedef struct lf_barray  {
    lf_common   common;
    ct_barray   f;
}lf_barray;

typedef struct lf_label {
    lf_common   common;
    ct_label    f;
}lf_label;

typedef struct lf_null {
    lf_common   common;
}lf_null;

typedef struct lf_nottrans {
    lf_common   common;
}lf_nottrans;

typedef struct lf_dimarray {
    lf_common   common;
    ct_dimarray f;
}lf_dimarray;

typedef struct lf_vftpath {
    lf_common   common;
    ct_vftpath  f;
}lf_vftpath;

typedef struct lf_precomp{
    lf_common   common;
    ct_precomp  f;
}lf_precomp;

typedef struct lf_endprecomp{
    lf_common     common;
    ct_endprecomp f;
}lf_endprecomp;

typedef struct lf_oem{
    lf_common   common;
    ct_oem      f;
}lf_oem;

typedef struct lf_skip{
    lf_common   common;
    ct_skip     f;
}lf_skip;

typedef struct lf_arglist {
    lf_common   common;
    ct_arglist  f;
}lf_arglist;

typedef struct lf_defarg {
    lf_common   common;
    ct_defarg   f;
}lf_defarg;

typedef struct lf_list{
    lf_common   common;
}lf_list;

typedef struct lf_fieldlist{
    lf_common   common;
}lf_fieldlist;

typedef struct lf_derived {
    lf_common   common;
    ct_derived  f;
}lf_derived;

typedef struct lf_bitfield {
    lf_common   common;
    ct_bitfield f;
}lf_bitfield;

typedef struct lf_mlist {
    lf_common   common;
    u2          attr;  /* size dependent on attr */
    u2          type;
}lf_mlist;

typedef struct lf_dimconu {
    lf_common   common;
    ct_dimconu  f;
}lf_dimconu;

typedef struct lf_dimconlu {
    lf_common   common;
    ct_dimconlu f;
}lf_dimconlu;

typedef struct lf_dimvaru {
    lf_common   common;
    ct_dimvaru  f;
}lf_dimvaru;

typedef struct lf_dimvarlu {
    lf_common   common;
    ct_dimvarlu f;
}lf_dimvarlu;


typedef struct lf_refsym {
    lf_common   common;
}lf_refsym;

typedef struct lf_bclass {
    lf_common   common;
    ct_bclass   f;
}lf_bclass;

typedef struct lf_vbclass { /*direct & indirect */
    lf_common   common;
    ct_vbclass  f;
}lf_vbclass;

typedef lf_vbclass lf_ivbclass;

typedef struct lf_enumerate {
    lf_common     common;
    ct_enumerate  f;
}lf_enumerate;

typedef struct lf_friendfcn {
    lf_common    common;
    ct_friendfcn f;
}lf_friendfcn;

typedef struct lf_index {
    lf_common   common;
    ct_index    f;
}lf_index;

typedef struct lf_member {
    lf_common   common;
    ct_member   f;
}lf_member;

typedef struct lf_stmember {
    lf_common   common;
    ct_stmember f;
}lf_stmember;

typedef struct lf_method {
    lf_common   common;
    ct_method   f;
}lf_method;

typedef struct lf_nestedtype{
    lf_common     common;
    ct_nestedtype f;
}lf_nestedtype;

typedef struct lf_vfunctab{
    lf_common   common;
    ct_vfunctab f;
}lf_vfunctab;

typedef struct lf_friendcls{
    lf_common    common;
    ct_friendcls f;
}lf_friendcls;

typedef struct lf_onemethod{
    lf_common    common;
    ct_onemethod f;
}lf_onemethod;

typedef struct lf_vfuncoff{
    lf_common   common;
    ct_vfuncoff f;
}lf_vfuncoff;

typedef union {
    lf_common           common;
    lf_modifier         modifier;
    lf_pointer          pointer;
    lf_array            array;
    lf_class            class_;
    lf_structure        structure;
    lf_union            union_;
    lf_enum             enum_;
    lf_procedure        procedure;
    lf_mfunction        mfunction;
    lf_vtshape          vtshape;
    lf_cobol0           cobol0;
    lf_cobol1           cobol1;
    lf_barray           barray;
    lf_label            label;
    lf_null             null;
    lf_nottrans         nottrans;
    lf_dimarray         dimarray;
    lf_vftpath          vftpath;
    lf_precomp          precomp;
    lf_endprecomp       endprecomp;
    lf_oem              oem;
    lf_skip             skip;
    lf_arglist          arglist;
    lf_defarg           defarg;
    lf_list             list;
    lf_fieldlist        fieldlist;
    lf_derived          derived;
    lf_bitfield         bitfield;
    lf_mlist            mlist;
    lf_dimconu          dimconu;
    lf_dimconlu         dimconlu;
    lf_dimvaru          dimvaru;
    lf_dimvarlu         dimvarlu;
    lf_refsym           refsym;
    lf_bclass           bclass;
    lf_vbclass          vbclass;
    lf_enumerate        enumerate;
    lf_friendfcn        friendfcn;
    lf_index            index;
    lf_member           member;
    lf_stmember         stmember;
    lf_method           method;
    lf_nestedtype       nestedtype;
    lf_vfunctab         vfunctab;
    lf_friendcls        friendcls;
    lf_onemethod        onemethod;
    lf_vfuncoff         vfuncoff;
} lf_all;

/********* Symbol defn ******************/
/* Symbol Indices */



/* common to all symbol defs */
typedef struct s_common{
    u2          length;
    u2          code;
}s_common;



typedef struct {
    s_common    common;
    cs_compile  f;
} s_compile;

typedef struct {
    s_common    common;
    cs_register f;
} s_register;

typedef struct {
    s_common    common;
    cs_constant f;
} s_constant;

typedef struct {
    s_common    common;
    cs_udt      f;
} s_udt;

typedef struct {
    s_common    common;
    cs_ssearch  f;
} s_ssearch;

typedef struct {
    s_common    common;
} s_end;

typedef struct {
    s_common    common;
} s_skip;

typedef struct {
    s_common    common;
} s_cvreserve;

typedef struct {
    s_common    common;
    cs_objname  f;
} s_objname;

typedef struct {
    s_common    common;
} s_endarg;

typedef struct {
    s_common    common;
    cs_coboludt f;
} s_coboludt;

typedef struct {
    s_common    common;
    cs_manyreg  f;
} s_manyreg;

typedef struct {
    s_common    common;
    cs_return   f;
} s_return;

typedef struct {
    s_common    common;
//  another symbol
} s_entrythis;

typedef struct {
    s_common    common;
    cs_bprel16  f;
} s_bprel16;

typedef struct {
    s_common    common;
    cs_regrel16 f;
} s_regrel16;

typedef struct {
    s_common    common;
    cs_ldata16  f;
} s_ldata16,s_gdata16;

typedef struct {
    s_common    common;
    cs_pub16    f;
} s_pub16;


typedef struct {
    s_common    common;
    cs_lproc16  f;
} s_lproc16,s_gproc16;

typedef struct {
    s_common    common;
    cs_thunk16  f;
} s_thunk16;

typedef struct {
    s_common    common;
    cs_block16  f;
} s_block16;

typedef struct {
    s_common    common;
    cs_with16   f;
} s_with16;

typedef struct {
    s_common    common;
    cs_label16  f;
} s_label16;

typedef struct {
    s_common      common;
    cs_cexmodel16 f;
} s_cexmodel16;

typedef struct {
    s_common     common;
    cs_vftpath16 f;
} s_vftpath16;

typedef struct {
    s_common    common;
    cs_bprel32  f;
} s_bprel32;

typedef struct {
    s_common    common;
    cs_regrel32 f;
} s_regrel32;

typedef struct {
    s_common     common;
    cs_lthread32 f;
} s_lthread32, s_gthread32;

typedef struct {
    s_common    common;
    cs_ldata32  f;
} s_ldata32,s_gdata32;

typedef struct {
    s_common    common;
    cs_pub32    f;
} s_pub32;

typedef struct {
    s_common    common;
    cs_lproc32  f;
} s_lproc32,s_gproc32;

typedef struct {
    s_common    common;
    cs_thunk32  f;
} s_thunk32;

typedef struct {
    s_common    common;
    cs_block32  f;
} s_block32;

typedef struct {
    s_common    common;
    cs_with32   f;
} s_with32;

typedef struct {
    s_common    common;
    cs_label32  f;
} s_label32;

typedef struct {
    s_common      common;
    cs_cexmodel32 f;
} s_cexmodel32;

typedef struct {
    s_common      common;
    cs_vftpath32  f;
} s_vftpath32;

typedef struct {
    s_common    common;
    cs_procref  f;
} s_procref, s_dataref;

typedef struct {
    s_common    common;
} s_align;

typedef union {
    s_common            common;
    s_compile           compile;
    s_register          register_;
    s_constant          constant;
    s_udt               udt;
    s_ssearch           ssearch;
    s_end               end;
    s_skip              skip;
    s_cvreserve         cvreserve;
    s_objname           objname;
    s_endarg            endarg;
    s_coboludt          coboludt;
    s_manyreg           manyreg;
    s_bprel16           bprel16;
    s_regrel16          regrel16;
    s_ldata16           ldata16;
    s_gdata16           gdata16;
    s_pub16             pub16;
    s_lproc16           lproc16;
    s_gproc16           gproc16;
    s_thunk16           thunk16;
    s_block16           block16;
    s_with16            with16;
    s_label16           label16;
    s_cexmodel16        cexmodel16;
    s_vftpath16         vftpath16;
    s_bprel32           bprel32;
    s_regrel32          regrel32;
    s_ldata32           ldata32;
    s_gdata32           gdata32;
    s_pub32             pub32;
    s_lproc32           lproc32;
    s_gproc32           gproc32;
    s_thunk32           thunk32;
    s_block32           block32;
    s_with32            with32;
    s_label32           label32;
    s_cexmodel32        cexmodel32;
    s_vftpath32         vftpath32;
    s_procref           procref;
    s_dataref           dataref;
    s_align             align;
    s_lthread32         lthread32;
    s_gthread32         gthread32;
    s_return            return_;
    s_entrythis         entrythis;
} s_all;

#pragma pack();
