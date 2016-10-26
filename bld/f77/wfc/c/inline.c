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


//
// INLINE  : inline pragamas used to optimize certain operations
//

#include "ftnstd.h"
#include "global.h"
#include "cgdefs.h"
#include "cg.h"
#include "wf77auxd.h"
#include "wf77defs.h"
#include "fcgbls.h"
#include "cgflags.h"
#include "cpopt.h"
#include "inline.h"
#include "types.h"
#include "fctypes.h"
#include "rstutils.h"
#include "auxlook.h"
#include "wf77info.h"
#include "cgswitch.h"
#include "cgprotos.h"

#include <string.h>


extern  void            DoPragma(char *);


#if _CPU == 386 || _CPU == 8086

extern  cgflags_t       CGFlags;

#if _CPU == 386

// Space optimization pragmas (take an awful beating on a pentium)

// edi  - destination pointer
// esi  - source pointer
// ecx  - number of characters to move
static  char    __RTIStrBlastEqOS[] =  { "aux __RTIStrBlastEq           \
                                        parm    reverse                 \
                                                [edi] [esi] [ecx] =     \
                                        \"rep   movsb\"                 \
                                " };

// edi  - destination pointer
// eax  - number of spaces to append
// esi  - source pointer
// ecx  - number of characters to move
static  char    __RTIStrBlastNeOS[] =  { "aux __RTIStrBlastNe           \
                                        parm    reverse                 \
                                                [edi] [eax] [esi] [ecx]=\
                                        \"rep   movsb\"                 \
                                        \"mov   ecx, eax\"              \
                                        \"mov   eax, 0x20202020\"       \
                                        \"rep   stosb\"                 \
                                " };

// Time Optimization pragmas

// edi  - destination pointer
// esi  - source pointer
// ecx  - number of 4 character tuples to move (strlen >> 2)
// eax  - number of characters left over after initial 4-tuple move (strlen & 3)
static  char    __RTIStrBlastEq[] =  { "aux __RTIStrBlastEq             \
                                        parm    reverse                 \
                                                [edi] [esi] [ecx] [eax] =\
                                        \"rep   movsd\"                 \
                                        \"mov   ecx, eax\"              \
                                        \"rep   movsb\"                 \
                                " };

// edi  - destination pointer
// eax  - number of spaces to append
// esi  - source pointer
// ecx  - number of characters to move
static  char    __RTIStrBlastNe[] =  { "aux __RTIStrBlastNe             \
                                        parm    reverse                 \
                                                [edi] [edx] [esi] [eax] \
                                        modify  [ecx] =                 \
                                        \"mov   ecx, eax\"              \
                                        \"shr   ecx, 2\"                \
                                        \"rep   movsd\"                 \
                                        \"mov   ecx, eax\"              \
                                        \"and   ecx, 3\"                \
                                        \"rep   movsb\"                 \
                                        \"mov   ecx, edx\"              \
                                        \"shr   ecx, 2\"                \
                                        \"mov   eax, 0x20202020\"       \
                                        \"rep   stosd\"                 \
                                        \"mov   ecx, edx\"              \
                                        \"and   ecx, 3\"                \
                                        \"rep   stosb\"                 \
                                " };

#elif _CPU == 8086

// Space Optimization pragmas

// es di - destination pointer
// ds si - source pointer
// cx    - number of characters to move
static  char    __RTIStrBlastEqOS[] =  { "aux __RTIStrBlastEq           \
                                        parm    reverse                 \
                                                [es di] [ds si] [cx]    \
                                        modify  exact [di si cx] =      \
                                        \"rep   movsb\"                 \
                                " };

// es di - destination pointer
// ax    - number of spaces to append
// ds si - source pointer
// cx    - number of characters to move
static  char    __RTIStrBlastNeOS[] =  { "aux __RTIStrBlastNe           \
                                        parm    reverse                 \
                                                [es di] [ax] [ds si] [cx]\
                                        modify  exact [di ax si cx] =   \
                                        \"rep   movsb\"                 \
                                        \"mov   cx, ax\"                \
                                        \"mov   ax, 0x2020\"            \
                                        \"rep   stosb\"                 \
                                " };

// es di - destination pointer
// ds si - source pointer
// cx    - number of 2 character tuples to move (strlen >> 21
// ax    - number of characters left over after initial 2-tuple move (strlen & 1)
static  char    __RTIStrBlastEq[] =  { "aux __RTIStrBlastEq             \
                                        parm    reverse                 \
                                                [es di] [ds si] [cx] [ax]\
                                        modify  exact [cx si di] =      \
                                        \"rep   movsw\"                 \
                                        \"mov   cx, ax\"                \
                                        \"rep   movsb\"                 \
                                " };

// es di - destination pointer
// ax    - number of spaces to append
// ds si - source pointer
// cx    - number of characters to move
static  char    __RTIStrBlastNe[] =  { "aux __RTIStrBlastNe             \
                                        parm    reverse                 \
                                                [es di] [dx] [ds si] [ax]\
                                        modify  exact [di dx si ax cx] =\
                                        \"mov   cx,ax\"                 \
                                        \"shr   cx,1\"                  \
                                        \"rep   movsw\"                 \
                                        \"adc   cx,0\"                  \
                                        \"rep   movsb\"                 \
                                        \"mov   cx,dx\"                 \
                                        \"mov   ax,0x2020\"             \
                                        \"shr   cx,1\"                  \
                                        \"rep   stosw\"                 \
                                        \"adc   cx,0\"                  \
                                        \"rep   stosb\"                 \
                                " };

// Windows pragmas (can't use DS as an argument since DS is pegged)

// es di - destination pointer
// si bx - source pointer
// cx    - number of characters to move
static  char    __RTIStrBlastEqWinOS[] =  { "aux __RTIStrBlastEq        \
                                        parm    reverse                 \
                                                [es di] [si bx] [cx]    \
                                        modify  exact [di si cx] =      \
                                        \"push  ds\"                    \
                                        \"mov   ds,si\"                 \
                                        \"mov   si,bx\"                 \
                                        \"rep   movsb\"                 \
                                        \"pop   ds\"                    \
                                " };

// es di - destination pointer
// ax    - number of spaces to append
// si bx - source pointer
// cx    - number of characters to move
static  char    __RTIStrBlastNeWinOS[] =  { "aux __RTIStrBlastNe        \
                                        parm    reverse                 \
                                                [es di] [ax] [si bx] [cx]\
                                        modify  exact [di ax si cx] =   \
                                        \"push  ds\"                    \
                                        \"mov   ds,si\"                 \
                                        \"mov   si,bx\"                 \
                                        \"rep   movsb\"                 \
                                        \"mov   cx, ax\"                \
                                        \"mov   ax, 0x2020\"            \
                                        \"rep   stosb\"                 \
                                        \"pop   ds\"                    \
                                " };

// es di - destination pointer
// si bx - source pointer
// cx    - number of 2 character tuples to move (strlen >> 21
// ax    - number of characters left over after initial 2-tuple move (strlen & 1)
static  char    __RTIStrBlastEqWin[] =  { "aux __RTIStrBlastEq          \
                                        parm    reverse                 \
                                                [es di] [si bx] [cx] [ax]\
                                        modify  exact [cx si di] =      \
                                        \"push  ds\"                    \
                                        \"mov   ds,si\"                 \
                                        \"mov   si,bx\"                 \
                                        \"rep   movsw\"                 \
                                        \"mov   cx,ax\"                 \
                                        \"rep   movsb\"                 \
                                        \"pop   ds\"                    \
                                " };

// es di - destination pointer
// ax    - number of spaces to append
// si bx - source pointer
// cx    - number of characters to move
static  char    __RTIStrBlastNeWin[] =  { "aux __RTIStrBlastNe          \
                                        parm    reverse                 \
                                                [es di] [dx] [si bx] [ax]\
                                        modify  exact [di dx si ax cx] =\
                                        \"push  ds\"                    \
                                        \"mov   ds,si\"                 \
                                        \"mov   si,bx\"                 \
                                        \"mov   cx,ax\"                 \
                                        \"shr   cx,1\"                  \
                                        \"rep   movsw\"                 \
                                        \"adc   cx,0\"                  \
                                        \"rep   movsb\"                 \
                                        \"mov   cx,dx\"                 \
                                        \"mov   ax,0x2020\"             \
                                        \"shr   cx,1\"                  \
                                        \"rep   stosw\"                 \
                                        \"adc   cx,0\"                  \
                                        \"rep   stosb\"                 \
                                        \"pop   ds\"                    \
                                " };

// Small memory pragmas.

// Space Optimizations pragmas

// di   - destination pointer
// si   - source pointer
// cx   - number of characters to move
static  char    __RTIStrBlastEqSOS[] =  { "aux __RTIStrBlastEq          \
                                        parm    reverse                 \
                                                [di] [si] [cx]          \
                                        modify  [es] =                  \
                                        \"push  ds\"                    \
                                        \"pop   es\"                    \
                                        \"rep   movsb\"                 \
                                " };

// di   - destination pointer
// ax   - number of spaces to append
// si   - source pointer
// cx   - number of characters to move
static  char    __RTIStrBlastNeSOS[] =  { "aux __RTIStrBlastNe          \
                                        parm    reverse                 \
                                                [di] [ax] [si] [cx]     \
                                        modify  [es] =                  \
                                        \"push  ds\"                    \
                                        \"pop   es\"                    \
                                        \"rep   movsb\"                 \
                                        \"mov   cx, ax\"                \
                                        \"mov   ax, 0x2020\"            \
                                        \"rep   stosb\"                 \
                                " };

// Time Optimization pragmas

// di   - destination pointer
// si   - source pointer
// cx   - number of 2 character tuples to move (strlen >> 21
// ax   - number of characters left over after initial 2-tuple move (strlen & 1)
static  char    __RTIStrBlastEqS[] =  { "aux __RTIStrBlastEq            \
                                        parm    reverse                 \
                                                [di] [si] [cx] [ax]     \
                                        modify  [es] =                  \
                                        \"push  ds\"                    \
                                        \"pop   es\"                    \
                                        \"rep   movsw\"                 \
                                        \"mov   cx, ax\"                \
                                        \"rep   movsb\"                 \
                                " };

// di   - destination pointer
// ax   - number of spaces to append
// si   - source pointer
// cx   - number of characters to move
static  char    __RTIStrBlastNeS[] =  { "aux __RTIStrBlastNe            \
                                        parm    reverse                 \
                                                [di] [dx] [si] [ax]     \
                                        modify  [cx es] =               \
                                        \"push  ds\"                    \
                                        \"pop   es\"                    \
                                        \"mov   cx, ax\"                \
                                        \"shr   cx, 1\"                 \
                                        \"rep   movsw\"                 \
                                        \"adc   cx, 0\"                 \
                                        \"rep   movsb\"                 \
                                        \"mov   cx, dx\"                \
                                        \"mov   ax, 0x2020\"            \
                                        \"shr   cx, 1\"                 \
                                        \"rep   stosw\"                 \
                                        \"adc   cx, 0\"                 \
                                        \"rep   stosb\"                 \
                                " };
#endif

typedef struct inline_rtn {
    char        __FAR *name;
    char        __FAR *pragma;
    cg_type     typ;
    sym_id      sym_ptr;
    aux_info    *aux;
} inline_rtn;

static inline_rtn  __FAR NormalInlineTab[] = {
        "__RTIStrBlastEq", __RTIStrBlastEq, TY_INTEGER, NULL, NULL,
        "__RTIStrBlastNe", __RTIStrBlastNe, TY_INTEGER, NULL, NULL
};

static inline_rtn  __FAR OptSpaceInlineTab[] = {
        "__RTIStrBlastEq", __RTIStrBlastEqOS, TY_INTEGER, NULL, NULL,
        "__RTIStrBlastNe", __RTIStrBlastNeOS, TY_INTEGER, NULL, NULL
};

#if _CPU == 8086
static inline_rtn  __FAR WinNormalInlineTab[] = {
        "__RTIStrBlastEq", __RTIStrBlastEqWin, TY_INTEGER, NULL, NULL,
        "__RTIStrBlastNe", __RTIStrBlastNeWin, TY_INTEGER, NULL, NULL
};

static inline_rtn  __FAR WinOptSpaceInlineTab[] = {
        "__RTIStrBlastEq", __RTIStrBlastEqWinOS, TY_INTEGER, NULL, NULL,
        "__RTIStrBlastNe", __RTIStrBlastNeWinOS, TY_INTEGER, NULL, NULL
};

static inline_rtn  __FAR SmallModelInlineTab[] = {
        "__RTIStrBlastEq", __RTIStrBlastEqS, TY_INTEGER, NULL, NULL,
        "__RTIStrBlastNe", __RTIStrBlastNeS, TY_INTEGER, NULL, NULL
};

static inline_rtn  __FAR OptSpaceSmallModelInlineTab[] = {
        "__RTIStrBlastEq", __RTIStrBlastEqSOS, TY_INTEGER, NULL, NULL,
        "__RTIStrBlastNe", __RTIStrBlastNeSOS, TY_INTEGER, NULL, NULL
};
#endif

static inline_rtn  __FAR *InlineTab = NormalInlineTab;

#define MAX_IN_INDEX    (sizeof( NormalInlineTab ) / sizeof( inline_rtn ))

static bool     CreatedPragmas = false;

#endif

void    InitInlinePragmas( void ) {
//===========================

#if _CPU == 386 || _CPU == 8086
    int index;

    if( !CreatedPragmas ) {
        if( OZOpts & OZOPT_O_SPACE ) {
            InlineTab = OptSpaceInlineTab;
#if _CPU == 8086
            if( CGOpts & CGOPT_M_MEDIUM ) {
                InlineTab = OptSpaceSmallModelInlineTab;
            } else {
                // using large/huge memory model
                if( CGOpts & CGOPT_WINDOWS ) {
                    InlineTab = WinOptSpaceInlineTab;
                }
            }
        } else {
            if( CGOpts & CGOPT_M_MEDIUM ) {
                InlineTab = SmallModelInlineTab;
            } else {
                // using large/huge memory model
                if( CGOpts & CGOPT_WINDOWS ) {
                    InlineTab = WinNormalInlineTab;
                }
            }
#endif
        }
        for( index = 0; index < MAX_IN_INDEX; index++ ) {
            DoPragma( InlineTab[ index ].pragma );
        }
        CreatedPragmas = true;
    }
    for( index = 0; index < MAX_IN_INDEX; index++ ) {
        InlineTab[ index ].sym_ptr = NULL;
    }
#endif
}


call_handle     InitInlineCall( int rtn_id ) {
//============================================

// Initialize a call to a runtime routine.

#if _CPU == 386 || _CPU == 8086
    sym_id              sym;
    inline_rtn __FAR    *in_entry;
    int                 name_len;

    if( !CreatedPragmas ) {
        InitInlinePragmas();
    }
    in_entry = &InlineTab[ rtn_id ];
    sym = in_entry->sym_ptr;
    if( sym == NULL ) {
        name_len = strlen( in_entry->name );
        strcpy( SymBuff, in_entry->name );
        sym = STAdd( SymBuff, name_len );
        sym->u.ns.flags = SY_USAGE | SY_TYPE | SY_SUBPROGRAM | SY_FUNCTION;
        sym->u.ns.u1.s.typ = FT_INTEGER_TARG;
        sym->u.ns.xt.size = TypeSize( sym->u.ns.u1.s.typ );
        sym->u.ns.u3.address = NULL;
        in_entry->sym_ptr = sym;
        in_entry->aux = AuxLookupName( in_entry->name, name_len );
    }
    return( CGInitCall( CGFEName( sym, in_entry->typ ), in_entry->typ, in_entry->sym_ptr ) );
#else
    rtn_id = rtn_id;
    return( 0 );
#endif
}


void    FreeInlinePragmas( void ) {
//===========================

// Free symbol table entries for run-time routines.

#if _CPU == 386 || _CPU == 8086
    int         index;
    sym_id      sym;

    if( !CreatedPragmas ) return;
    for( index = 0; index < MAX_IN_INDEX; index++ ) {
        sym = InlineTab[ index ].sym_ptr;
        if( sym != NULL ) {
            if( ( CGFlags & CG_FATAL ) == 0 ) {
                if( sym->u.ns.u3.address != NULL ) {
                    BEFreeBack( sym->u.ns.u3.address );
                }
            }
            STFree( sym );
            InlineTab[ index ].sym_ptr = NULL;
        }
    }
#endif
}
