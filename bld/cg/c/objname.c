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


#include <string.h>
#include "standard.h"
#include "sysmacro.h"
#include "cg.h"
#include "bckdef.h"
#include "cgaux.h"
#include "objrep.h"
#include "system.h"
#include "model.h"
#include "hostsys.h"
#include "ocentry.h"
#include "import.h"
#include "feprotos.h"
#include "compcfg.h"

extern  uint            Length(pointer);
extern  char            *CopyStr(char*,char*);

#define SPEC_PREFIX             ".PREFIX_DATA."
#define DLLIMPORT_PREFIX        "__imp_"
#define PIC_RW_PREFIX           "__rw_"
#if COMP_CFG_COFF
#define MAX_OBJNAME_LEN         1024
#else
#define MAX_OBJNAME_LEN         256
#endif
#define MAX_OBJNAME_BUFF        (1024+16)       // kludge; allow some slack

extern  void    DoOutObjectName( sym_handle sym,
                                void (*outputter)( char *, void * ),
                                void *data,
                                import_type kind ) {
/*******************************************************************/

    char        *src;
    char        *dst;
    char        *end;
    char        *pattern;
    char        buffer[MAX_OBJNAME_BUFF];
    char        *fe_name;
    unsigned    fe_len;
    unsigned    pref_len;

    fe_name = FEExtName( sym, &pattern );
    fe_len = Length( fe_name );;
    assert( fe_len < sizeof( buffer ) );
    dst = buffer;
    switch( kind ) {
    case SPECIAL:
        pref_len = (sizeof( SPEC_PREFIX )-1);
        dst = CopyStr( SPEC_PREFIX, dst );
        break;
    case DLLIMPORT:
        pref_len = (sizeof( DLLIMPORT_PREFIX )-1);
        dst = CopyStr( DLLIMPORT_PREFIX, dst );
        break;
    case PIC_RW:
        pref_len = (sizeof( PIC_RW_PREFIX )-1);
        dst = CopyStr( PIC_RW_PREFIX, dst );
        break;
    default:
        pref_len = 0;
    }
    if(( pref_len + fe_len ) >= MAX_OBJNAME_LEN ) {
        // we need to keep the prefix so truncate the objname down
        fe_len -= pref_len;
        fe_name[fe_len] = '\0';
    }
    // NYI: check length before inserting into buff
    for( src = pattern; *src != NULLCHAR; ++src ) {
        switch( *src ) {
        case '\\':
            *dst++ = *++src;
            break;
        case '*':
            dst = CopyStr( fe_name, dst );
            break;
        case '^':
            end = CopyStr( fe_name, dst );
            while( dst != end ) {
                if( *dst >= 'a' && *dst <= 'z' ) *dst -= 'a' - 'A';
                ++dst;
            }
            break;
        case '!':
            end = CopyStr( fe_name, dst );
            while( dst != end ) {
                if( *dst >= 'A' && *dst <= 'Z' ) *dst += 'a' - 'A';
                ++dst;
            }
            break;
        default:
            *dst++ = *src;
            break;
        }
    }
    *dst = NULLCHAR;
    assert( Length( buffer ) < MAX_OBJNAME_LEN );
    outputter( buffer, data );
}

extern bool SymIsExported( sym_handle sym ) {
/*******************************************/

    bool        exported;

    exported = FALSE;
    if( sym != NULL ) {
        if( FEAttr( sym ) & FE_DLLEXPORT ) {
            exported = TRUE;
        } else if( *(call_class*)FEAuxInfo( FEAuxInfo( sym, AUX_LOOKUP ), CALL_CLASS ) & DLL_EXPORT ) {
            exported = TRUE;
        }
    }
    return( exported );
}
