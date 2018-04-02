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


#include "dip.h"
#include "madtypes.h"


#define cue_SIZE        DIPHandleSize( HK_CUE )
#define sym_SIZE        DIPHandleSize( HK_SYM )
#define type_SIZE       DIPHandleSize( HK_TYPE )

#define HDLAssign( type, dst, src )     memcpy( dst, src, type##_SIZE );

#define DIPHDL( t, v )  t##_handle *v = (t##_handle*)walloca( t##_SIZE )

#define SL2SH( l )      ((sym_handle *)((l)->h))
#define IIH2IH( i )     ((image_handle *)((char *)(i) - DIPHandleSizeWV( HK_IMAGE )))

#define SL2ISH( l )     ((imp_sym_handle *)((l)->h + DIPHandleSizeWV( HK_SYM )))
#define TH2ITH( t )     ((imp_type_handle *)((char *)(t) + DIPHandleSizeWV( HK_TYPE )))

typedef enum {
    SC_TYPE,
    SC_INTERNAL,
    SC_USER
} wv_sym_class;

typedef enum {
    #define pick(n,tk,tm,tt) INTERNAL_ ## n,
    #include "dipwvsym.h"
    #undef pick
} internal_idx;

typedef struct {
    dig_type_info       ti;
    wv_sym_class        sc;
    union {
        internal_idx    internal;   /* for SC_INTERNAL */
        unsigned long   uint;       /* remainder for SC_USER */
        long            sint;
        xreal           real;
        xcomplex        cmplx;
        void            *string;
        address         addr;
    }   v;
} fixed_wv_sym_entry;

typedef struct {
    fixed_wv_sym_entry  info;
    char                name[1];
} wv_sym_entry;

struct imp_sym_handle {
    const wv_sym_entry  *p;
    const mad_reg_info  *ri;
};

struct imp_type_handle {
    dig_type_info       ti;
    const mad_reg_info  *ri;
};

#define IMH_WV      IMH_BASE
