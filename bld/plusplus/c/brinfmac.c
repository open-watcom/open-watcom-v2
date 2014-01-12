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


#include "plusplus.h"
#include "brinfoim.h"
#include "ring.h"
#include "memmgr.h"
#include "initdefs.h"
#include "vstk.h"
#include "name.h"

typedef struct _mac_undef MACUNDEF;

struct _mac_undef               // MACUNDEF -- source file for an #undef
{
    MEPTR macro;                // - macro that was undef'd
    SRCFILE srcfile;            // - source file from which #undef occurred
};

struct _mac_value               // MACVALUE -- macro value
{
    MACVALUE* next;             // - next in values chain
    union {
        char* undef;            // - MVT_UNDEFED: name
        char* defin;            // - MVT_DEFINED: name
        MEPTR value;            // - MVT_VALUE: defining value
    };
    MAC_VTYPE type;             // - type of entry
    unsigned :0;                // - force alignment
};

static carve_t carveMacVals;    // carving for macro values
static MACVALUE* values;        // ring of values
static VSTK_CTL undefs;         // undef's so far


static void brinfMacInit        // INITALIZATION
    ( INITFINI* defn )          // - definition
{
    defn = defn;
    values = NULL;
    carveMacVals = CarveCreate( sizeof( MACVALUE ), 1024 );
    VstkOpen( &undefs, sizeof( MACUNDEF ), 32 );
}


static void brinfMacFini        // COMPLETION
    ( INITFINI* defn )          // - definition
{
    defn = defn;
    if( 0 != carveMacVals ) {
        MACVALUE* curr;
        RingIterBeg( values, curr ) {
            switch( curr->type ) {
              case MVT_DEFINED :
              case MVT_UNDEFED :
              { void* temp = curr->defin;
                curr->defin = NULL;
                CMemFree( temp );   // same format for undef'ed
              } break;
            }
        } RingIterEnd( curr );
        CarveDestroy( carveMacVals );
        carveMacVals = 0;
        VstkClose( &undefs );
    }
}


void BrinfMacRestart            // RESTART DURING PCH READ
    ( void )
{
    brinfMacFini( NULL );
    brinfMacInit( NULL );
}


// This routine is invoked to release macro storage early:
//  (1) when no -fbi on command line
//  (2) when macro file has been written
//
void BrinfMacroRelease          // RELEASE BROWSE MACRO-ING WHEN NO BROWSING
    ( void )
{
    brinfMacFini( NULL );
}


INITDEFN (browse_macros, brinfMacInit, brinfMacFini );


static MACVALUE* findValue      // LOOKUP VALUE IN SAVED VALUES
    ( MEPTR src )               // - source value
{
    MACVALUE* retn;             // - found value
    MACVALUE* srch;             // - value during the search
    unsigned src_defn;          // - src->macro_defn
    unsigned src_len;           // - src->macro_len
    unsigned src_dsize;         // - size of source definition

    retn = NULL;
    src_defn = src->macro_defn;
    src_len = src->macro_len;
    src_dsize = src_len - src_defn;
    RingIterBeg( values, srch ) {
        if( srch->type == MVT_VALUE ) {
            MEPTR curr = srch->value;
            if( curr == src ) {
                retn = srch;
                break;
            } else {
                if( curr->macro_len == src_len
                 && curr->macro_defn == src_defn
                 && curr->parm_count == src->parm_count
                 && (curr->macro_flags & MFLAG_BRINFO_DEFN) == (src->macro_flags & MFLAG_BRINFO_DEFN)
                 && curr->macro_name[0] == src->macro_name[0]
                 && 0 == strcmp( curr->macro_name, src->macro_name )
                 && ( curr->macro_defn == 0
                   || 0 == memcmp( (char*)curr + src_defn
                                 , (char*)src  + src_defn
                                 , src_dsize ) )
                 ) {
                    retn = srch;
                    break;
                }
            }
        }
    } RingIterEnd( srch );
    return retn;
}


MACVALUE* BrinfMacAddValue      // ADD A VALUE
    ( MEPTR mac )               // - the macro
{
    MACVALUE* retn;             // - return location

    retn = findValue( mac );
    if( NULL == retn ) {
        retn = RingCarveAlloc( carveMacVals, &values );
        retn->type = MVT_VALUE;
        retn->value = mac;
    }
    return retn;
}


static MACVALUE* findDefUndef   // LOOKUP (UN)DEFINED VALUE IN SAVED VALUES
    ( char const* name          // - macro name
    , unsigned nlen             // - name length
    , MAC_VTYPE type )          // - type of entry
{
    MACVALUE* retn;             // - found value
    MACVALUE* curr;             // - value during the search

    retn = NULL;
    RingIterBeg( values, curr ) {
        if( curr->type == type
         && curr->defin[0] == name[0]           // undef'ed has same format
         && 0 == memcmp( curr->defin, name, nlen )
         ) {
            retn = curr;
            break;
        }
    } RingIterEnd( curr );
    if( NULL == retn ) {
        char * save = CMemAlloc( nlen + 1 );
        memcpy( save, name, nlen );
        save[ nlen ] = '\0';
        retn = RingCarveAlloc( carveMacVals, &values );
        retn->type = type;
        retn->defin = save;   // note: undefed is same format
    }
    return retn;
}


MACVALUE* BrinfMacAddDefin      // ADD A defined(name) VALUE
    ( char const* name          // - macro name
    , unsigned nlen )           // - name length
{
    return findDefUndef( name, nlen, MVT_DEFINED );
}


MACVALUE* BrinfMacAddUndef      // ADD A !defined(name) VALUE
    ( char const* name          // - macro name
    , unsigned nlen )           // - name length
{
    return findDefUndef( name, nlen, MVT_UNDEFED );
}


// The following routines extract Browse data from MACVALUE pointers:
//
// (1) BrinfMacValueName can be called for any MACVALUE.
//     It returns the name of the macro
//
// The following can be called only for MACVALUE entries representing values
//
// (2) BrinfMacValueLocn -- get the definition location
//
// (3) BrinfMacValueDefn -- get the byte sequence for the definition (and its
//     length).  A return of NULL indicates that the macro is "special" (like
//     __FILE__).
//
// (4) BrinfMacValueParmCount -- get # parameters for macro.
//     0   ==> function-like declaration was not used
//     n>0 ==> function-like declaration with n-1 parameters
//
// The combination of the preceding two returns establishes a macro
// declaration.


char const *BrinfMacValueName   // GET NAME FROM MACVALUE
    ( MACVALUE const *mv )      // - the MACVALUE
{
    char const *name;           // - name for macro

    switch( mv->type ) {
      case MVT_VALUE :
        name = mv->value->macro_name;
        break;
      case MVT_DEFINED :
        name = mv->defin;
        break;
      case MVT_UNDEFED :
        name = mv->undef;
        break;
      DbgDefault( "Bad MACVALUE type" );
    }
    return name;
}


TOKEN_LOCN const* BrinfMacValueLocn // GET LOCATION FOR MACVALUE DECLARATION
    ( MACVALUE const *mv )      // - the MACVALUE
{
    DbgVerify( mv->type == MVT_VALUE, "Cannot get location for non-value" );
    return &mv->value->defn;
}


unsigned BrinfMacValueParmCount // GET # PARAMETERS FOR MACVALUE DECLARATION
    ( MACVALUE const *mv )      // - the MACVALUE
{
    // note: the parm count is 1 + # parameters
    DbgVerify( mv->type == MVT_VALUE, "Cannot get # parms for non-value" );
    return mv->value->parm_count;
}


uint_8 const * BrinfMacValueDefn // GET LOCATION FOR MACVALUE DECLARATION
    ( MACVALUE const *mv        // - the MACVALUE
    , unsigned* a_length )      // - addr[ length of definition ]
{
    MEPTR mdef;                 // - macro definition
    unsigned length;            // - length of definition
    uint_8 const *defn;         // - definition

    DbgVerify( mv->type == MVT_VALUE, "Cannot get definition for non-value" );
    mdef = mv->value;
    if( 0 == mdef->macro_defn ) {
        // special macro
        length = 0;
        defn = NULL;
    } else {
        length = mdef->macro_len - mdef->macro_defn;
        defn = (uint_8 const *)mdef + mdef->macro_defn;
    }
    *a_length = length;
    return defn;
}


void BrinfMacUndef              // RECORD UNDEFINE OF MACRO
    ( MEPTR macro               // - macro
    , SRCFILE src )             // - source file in which it occurred
{
    MACUNDEF* undef = VstkPush( &undefs );
    undef->macro = macro;
    undef->srcfile = src;
}


SRCFILE BrinfMacUndefSource     // GET SOURCE OF AN UNDEF
    ( char const *name )        // - macro name
{
    unsigned nlen;              // - length of name
    MACUNDEF* curr;             // - current undef entry
    SRCFILE retn;               // - return value

    nlen = strlen( name );
    retn = NULL;
    for( curr = VstkTop( &undefs )
       ; curr != NULL
       ; curr = VstkNext( &undefs, curr ) ) {
        if( NameMemCmp( curr->macro->macro_name, name, nlen ) == 0 ) {
            retn = curr->srcfile;
            break;
        }
    }
    return retn;
}
