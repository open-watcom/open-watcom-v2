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
* Description:  Macro processing related data types and constants.
*
****************************************************************************/


#ifndef MACRO_H
#define MACRO_H

typedef char *  MACADDR_T;       /* contains actual pointer to block of memory */

enum special_macros {
    #define pick( s, i, f )    i,
    #include "specmac.h"
    #undef pick
    MACRO_MAX
};

typedef enum {                          // kind of macro scanning
    MSCAN_MANY          = 0x01,         // - many tokens to be scanned
    MSCAN_EQUALS        = 0x02,         // - scan "=" after identifier
                                        //   (means define cmdline macro!)
                                        // derived from above
    MSCAN_CMDLN_NORMAL  = MSCAN_EQUALS, // - for /d, not extended
    MSCAN_CMDLN_PLUS    = ( MSCAN_EQUALS //- for /d+
                        | MSCAN_MANY ),
    MSCAN_DEFINE        = MSCAN_MANY,   // - for #define processing
    MSCAN_NULL          = 0x00
} macro_scanning;

typedef enum macro_flags {
    MFLAG_NONE,
    MFLAG_DEFINED_BEFORE_FIRST_INCLUDE  = 0x01,
    MFLAG_CAN_BE_REDEFINED              = 0x02,
    MFLAG_USER_DEFINED                  = 0x04,
    MFLAG_REFERENCED                    = 0x08,
    MFLAG_HAS_VAR_ARGS                  = 0x10,
    MFLAG_PCH_CHECKED                   = 0x20,
    MFLAG_PCH_OVERRIDE                  = 0x40,
// a special macro won't appear as a macro to the program (e.g. ifdef
// will return false)
    MFLAG_SPECIAL                       = 0x80,
// Following are used only in browsing, not in macro definitions
    MFLAG_BRINFO_UNDEF                  = 0x100,
} macro_flags;

#define MFLAG_PCH_TEMPORARY_FLAGS       ( MFLAG_PCH_CHECKED \
                                        | MFLAG_PCH_OVERRIDE )

// Following are used only in browsing, not in macro definitions

#define MFLAG_BRINFO_DEFN               ( MFLAG_USER_DEFINED \
                                        | MFLAG_BRINFO_UNDEF )

/* Actual macro definition is at (char *)mentry + mentry->macro_defn */

typedef struct macro_entry MEDEFN, *MEPTR;
struct macro_entry {
    union {
        MEPTR       next_macro; /* next macro in this hash chain */
        unsigned    pch_hash;
    } u;
    TOKEN_LOCN  defn;           /* where it was defined */
    uint_16     macro_defn;     /* offset to defn, 0 ==> special macro name*/
    uint_16     macro_len;      /* length of macro definition */
    macro_flags macro_flags;    /* flags */
    uint_8      parm_count;     /* special macro indicator if defn == 0 */
    unsigned    : 0;            /* align macro_name to a DWORD boundary */
    char        macro_name[1];  /* name,parms, and macro definition */
};

#include <pushpck1.h>
typedef struct macro_stack MSTACK, *MSTACKPTR;
struct macro_stack {
    MSTACKPTR   stacked_macro;
    MEPTR       prev_macro;
    char        **macro_parms;          /* pointer to array of actual parms */
    char        *prev_ptr;
    int         macro_class;            /* T_MACRO or T_MACRO_PARM */
    char        macro_definition[1];    /* copy of current macro definition */
};
#include <poppck.h>

#endif
