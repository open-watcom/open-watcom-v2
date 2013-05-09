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


#ifndef PILTYPES_H

#include "digtypes.h"
#include "digpck.h"

/*
 * Link Routine Types
 */

struct link_handle;
struct link_instance;
struct link_trigger;
struct link_buffer;
struct link_imp_instance;
struct link_imp_handle;
struct pill_imp_routines;

typedef struct link_handle              link_handle;
typedef struct link_instance            link_instance;
typedef struct link_trigger             link_trigger;
typedef struct link_buffer              link_buffer;
typedef struct link_imp_instance        link_imp_instance;
typedef struct link_imp_handle          link_imp_handle;
typedef struct pill_imp_routines        pill_imp_routines;
typedef void (DIGENTRY pill_private_func)();

#if defined(__X86__)
    typedef     unsigned_8      link_atomic_t;
#elif defined(__AXP__)
    typedef     unsigned        link_atomic_t;
#else
    #error link_atomic_t not configured for processor
#endif

#include "pushpck8.h"

typedef enum {
    PL_ENGLISH,
    PL_JAPANESE,
    PL_NUM
} pil_language;

typedef struct {
    unsigned long       id;
    union {
        unsigned long   code;
        void            *ptr;
    }                   data;
    link_handle         *source;
} link_message;

/* the data.code field holds a system error number */
#define LM_SYSTEM_ERROR 0

typedef enum {
    LS_INITIAL,
    LS_CONNECT_ATTEMPT,
    LS_CONNECTED,
    LS_DISCONNECTED,
    LS_MESSAGE,
    LS_FAILURE          = 0x80,
    LS_FAILURE_INIT,
    LS_FAILURE_CONNECT,
    LS_FAILURE_TRANS,
    LS_FAILURE_RECV
} link_status;

typedef struct {
    void        *data;
    unsigned    len;
} lmx_entry;

struct link_buffer {
    link_buffer         *next;
    unsigned short      len;
    unsigned short      flags;
    union {
        unsigned_64     dummy;          /* to get maximal alignment */
        unsigned_8      data[1];        /* variable sized */
    };
};

typedef struct {
    link_buffer         volatile *head;
    link_buffer         * volatile *tail_owner;
} link_queue;

#include "poppck.h"

/* maximum packet sizes */
#define PILL_MAX_PACKET         0x7fff  /* max allowed */
#define PILL_MIN_PACKET         0x0200  /* min allowed */

/* special return values from LinkKick */
#define PILL_KICK_POLL          0
#define PILL_KICK_WAIT          (-1U)

/*
 * PIL Protocol Types
 */

typedef struct {
    unsigned_8  r_service;
    unsigned_8  r_num;
    unsigned_8  flags;
    unsigned_8  mpx_handle;
    unsigned_16 req_handle;
    unsigned_16 proc_handle;
} pil_header;

enum pil_header_flags {
    PHF_MORE_TO_COME    = 0x01,
    PHF_REMOTE          = 0x02,
    PHF_LAST_PACKET     = 0x04
};

typedef unsigned_16     pil_string_header;

enum pil_character_encoding {
    PCE_SYSTEM,
    PCE_ASCII,
    PCE_UTF8,
    PCE_UNICODE
};

enum pil_preferred_language { PPL_PREFERRED, PPL_OTHER };

#define MKPILSTR( e, p, l )     ((e)<<13|(p)<<12|(l))

#define MKPREFERREDSTR( e, l )  ( e, PPL_PREFERRED, l )
#define MKOTHERSTR( e, l )      ( e, PPL_OTHER, l )

#define PS_ENCODEGET( h )       (((h)>>13) & 0x7)
#define PS_LANGGET( h )         (((h)>>12) & 0x1)
#define PS_SIZEGET( h )         ((h) & 0xfff)

enum pil_alignment {
    PIL_ALIGN_1         = 0x0000,
    PIL_ALIGN_2         = 0x1000,
    PIL_ALIGN_4         = 0x2000,
    PIL_ALIGN_8         = 0x3000,
    PIL_ALIGN_16        = 0x4000
};

#define MKALIGNSIZE( a, s )     (PIL_ALIGN_##a + (s))
#define AS_ALIGNGET( as )       (1 << ((as) >> 12))
#define AS_SIZEGET( as )        ((as) & 0xfff)

#define ALIGNSIZE_0             MKALIGNSIZE( 1, 0 )
#define ALIGNSIZE_1             MKALIGNSIZE( 1, 1 )
#define ALIGNSIZE_2             MKALIGNSIZE( 2, 2 )
#define ALIGNSIZE_4             MKALIGNSIZE( 4, 4 )

#define PILTYPES_H

#include "digunpck.h"
#endif
