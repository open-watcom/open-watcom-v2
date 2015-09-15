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

#if !defined(_F77_DATTYP_H_)
#define _F77_DATTYP_H_ 1

typedef struct adv_entry {
    signed_32   lo_bound;
    uint        num_elts;
} adv_entry;

#include "tddattyp.h"

typedef    signed_8     logstar1;       // 8-bit logical
typedef    signed_32    logstar4;       // 32-bit logical
typedef    signed_8     intstar1;       // 8-bit integer
typedef    signed_16    intstar2;       // 16-bit integer
typedef    signed_32    intstar4;       // 32-bit integer
typedef    real         single;         // single precision
typedef    long double  extended;       // extended precision

#if _CPU == 8086
    typedef    intstar2     inttarg;    // target integer
#else
    typedef    intstar4     inttarg;    // target integer
#endif

typedef    unsigned_32  ftnoption;

typedef struct scomplex {        // single precision complex
    single      realpart;
    single      imagpart;
} scomplex;

typedef struct dcomplex {        // double precision complex
    double      realpart;
    double      imagpart;
} dcomplex;

typedef struct xcomplex {        // extended precision complex
    extended    realpart;
    extended    imagpart;
} xcomplex;

typedef struct arr_desc {
    char                *data;
    unsigned long       num_elmts;
    unsigned int        elmt_size;
    PTYPE               typ;
} arr_desc;

#include "scb.h"

#define MAX_INT_SIZE    11       //  buffer for 32-bit integer strings

typedef struct symbol   *sym_id;

typedef union ftn_type {
    logstar1            logstar1;
    logstar4            logstar4;
    intstar1            intstar1;
    intstar2            intstar2;
    intstar4            intstar4;
    single              single;
    double              dble;
    extended            extended;
    struct scomplex     scomplex;
    struct dcomplex     dcomplex;
    struct xcomplex     xcomplex;
    struct cstring      cstring;
    struct string       string;
    struct arr_desc     arr_desc;
    void                PGM *pgm_ptr;
    struct {                            // we don't want the structure chain
        intstar4        field_offset;   // to destroy the field offset during
        void            *struct_chain;  // down-scan.
    } sc;
    struct {
        sym_id          field_id;       // structure name of fields
        sym_id          ss_id;          // substrung symbol
        uint            ss_size;        // length of substrung/subscripted
                                        // character string
    } st;
} ftn_type;

    typedef     unsigned int    label_id;
    typedef     obj_ptr         warp_label;
#endif
