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


#include "diptypes.h"
#include "madtypes.h"

typedef struct {
    type_kind           k;
    type_modifier       m;
    unsigned short      s;
} wv_type_entry;

enum wv_sym_class {
    SC_TYPE,
    SC_INTERNAL,
    SC_USER
};

typedef struct {
    wv_type_entry       t;
    enum wv_sym_class   sc;
    union {
        /* for SC_INTERNAL */
        unsigned        internal;
        /* remainder for SC_USER */
        unsigned long   uint;
        long            sint;
        xreal           real;
        xcomplex        cmplx;
        void            *string;
        address         addr;
    }   v;
} fixed_wv_sym_entry;

/* Note: The following strcture previously was:

typedef struct {
    fixed_wv_sym_entry;
    char                name[1];
} wv_sym_entry;

This was fine with Watcom and gcc 3.2, but not gcc 3.3. If you
change the fixed_wv_sym_entry struct, wv_sym_entry needs to
reflect those changes and vice versa.

*/

typedef struct {
    struct {
        wv_type_entry       t;
        enum wv_sym_class   sc;
        union {
            /* for SC_INTERNAL */
            unsigned        internal;
            /* remainder for SC_USER */
            unsigned long   uint;
            long            sint;
            xreal           real;
            xcomplex        cmplx;
            void            *string;
            address         addr;
        }   v;
    } info;
    char                name[1];        /* variable sized, first is length */
} wv_sym_entry;

struct imp_sym_handle {
    wv_sym_entry        *p;
    const mad_reg_info  *ri;
};

struct imp_type_handle {
    wv_type_entry       t;
    const mad_reg_info  *ri;
};

#define WV_INT_MH       1
