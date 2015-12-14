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


#ifndef DIP_JVMDIP
#define DIP_JVMDIP

#include <string.h>
#include "dip.h"
#include "dipimp.h"

#define _WINDOWS_       /* stop inclusion of NT headers */
typedef void *HANDLE;
#include "javadbg.h"

#define JAVA_OBJECT_NAME        "java/lang/Object"
#define JAVA_STRING_NAME        "java/lang/String"

#define MAX_NAME        1024

#define IMH_JAVA        IMH_BASE

typedef unsigned_32     ji_ptr; /* java interpreter pointer */

struct imp_sym_handle {
    union {
        ji_ptr          mb;
        ji_ptr          fb;
        ji_ptr          lv;
        ji_ptr          cn;
        ji_ptr          pk;
    }           u;
    enum {
        JS_METHOD,
        JS_FIELD,
        JS_LOCAL,
        JS_TYPE,
        JS_PACKAGE
    }           kind;
};

struct imp_type_handle {
    ji_ptr              sig;
    union {
        ji_ptr          object;
        imp_sym_handle  is;
    }   u;
    enum {
        JT_SIGNATURE,
        JT_RAWNAME,
        JT_INTEGER,
        JT_WANTOBJECT
    }                   kind;
};

struct imp_cue_handle {
    unsigned    mb_idx;
    unsigned    ln_idx;
    ji_ptr      cc;
};

struct imp_image_handle {
    ji_ptr              cc;
    ji_ptr              mb;
    ji_ptr              cp;
    ji_ptr              object_class;
    unsigned            num_methods;
    unsigned            last_method;
    struct methodblock  *methods;
};

#define IDX_TO_METHOD_BLOCK( ii, i )    (ii->mb + (i) * sizeof( struct methodblock ) )
#define METHOD_BLOCK_TO_IDX( ii, mb )   (((mb)-ii->mb) / sizeof( struct methodblock ) )

extern address  NilAddr;
extern address  DefCodeAddr;
extern address  DefDataAddr;
extern char     NameBuff[MAX_NAME];

void            LocationCreate( location_list *ll, location_type lt, void *d );
void            LocationAdd( location_list *ll, long sbits );
void            LocationTrunc( location_list *ll, unsigned bits );

dip_status      GetData( ji_ptr off, void *p, unsigned len );
unsigned        GetString( ji_ptr off, char *buff, unsigned buff_size );
ji_ptr          GetPointer( ji_ptr off );
ji_ptr          GetClass( ji_ptr off );
dip_status      GetLineCue( struct mad_jvm_findlinecue_acc *acc, struct mad_jvm_findline_ret *ret );
dip_status      GetAddrCue( struct mad_jvm_findaddrcue_acc *acc, struct mad_jvm_findline_ret *ret );
unsigned        GetU8( ji_ptr off );
unsigned        GetU16( ji_ptr off );
unsigned long   GetU32( ji_ptr off );

size_t          NameCopy( char *buff, const char *src, size_t buff_size, size_t len );
void            NormalizeClassName( char *p, unsigned len );


search_result   FindMBIndex( imp_image_handle *ii, addr_off off, unsigned *idx );

dip_status      FollowObject( ji_ptr sig, location_list *ll, ji_ptr *handle );

dip_status      ImpSymLocation( imp_image_handle *ii, imp_sym_handle *is, location_context *lc, location_list *ll, ji_ptr *obj_handle );
#endif
