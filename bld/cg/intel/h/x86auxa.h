/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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


#define CALLER_POPS             ( 1LL << ( _TARG_AUX_SHIFT + 0 ) )
#define DLL_EXPORT              ( 1LL << ( _TARG_AUX_SHIFT + 1 ) )
#define SPECIAL_RETURN          ( 1LL << ( _TARG_AUX_SHIFT + 2 ) )
#define SPECIAL_STRUCT_RETURN   ( 1LL << ( _TARG_AUX_SHIFT + 3 ) )
#define FAR_CALL                ( 1LL << ( _TARG_AUX_SHIFT + 4 ) )
#define INTERRUPT               ( 1LL << ( _TARG_AUX_SHIFT + 5 ) )
#define LOAD_DS_ON_CALL         ( 1LL << ( _TARG_AUX_SHIFT + 6 ) )
#define LOAD_DS_ON_ENTRY        ( 1LL << ( _TARG_AUX_SHIFT + 7 ) )
#define MODIFY_EXACT            ( 1LL << ( _TARG_AUX_SHIFT + 8 ) )
#define NO_8087_RETURNS         ( 1LL << ( _TARG_AUX_SHIFT + 9 ) )
#define NO_FLOAT_REG_RETURNS    ( 1LL << ( _TARG_AUX_SHIFT + 10 ) )
#define NO_MEMORY_CHANGED       ( 1LL << ( _TARG_AUX_SHIFT + 11 ) )
#define NO_MEMORY_READ          ( 1LL << ( _TARG_AUX_SHIFT + 12 ) )
#define NO_STRUCT_REG_RETURNS   ( 1LL << ( _TARG_AUX_SHIFT + 13 ) )
#define ROUTINE_RETURN          ( 1LL << ( _TARG_AUX_SHIFT + 14 ) )
#define FAT_WINDOWS_PROLOG      ( 1LL << ( _TARG_AUX_SHIFT + 15 ) )
#define GENERATE_STACK_FRAME    ( 1LL << ( _TARG_AUX_SHIFT + 16 ) )
#define EMIT_FUNCTION_NAME      ( 1LL << ( _TARG_AUX_SHIFT + 17 ) )
#define GROW_STACK              ( 1LL << ( _TARG_AUX_SHIFT + 18 ) )
#define PROLOG_HOOKS            ( 1LL << ( _TARG_AUX_SHIFT + 19 ) )
#define THUNK_PROLOG            ( 1LL << ( _TARG_AUX_SHIFT + 20 ) )
#define EPILOG_HOOKS            ( 1LL << ( _TARG_AUX_SHIFT + 21 ) )
#define FAR16_CALL              ( 1LL << ( _TARG_AUX_SHIFT + 22 ) )
#define TOUCH_STACK             ( 1LL << ( _TARG_AUX_SHIFT + 23 ) )
#define LOAD_RDOSDEV_ON_ENTRY   ( 1LL << ( _TARG_AUX_SHIFT + 24 ) )
#define FARSS                   ( 1LL << ( _TARG_AUX_SHIFT + 25 ) )

#define USED_BITS               26

#if USED_BITS + _TARG_AUX_SHIFT > 64
    #error Overflowed a long long
#endif

typedef unsigned long long      call_class;

#define FLOATING_FIXUP_BYTE     0xFF

#define BYTE_SEQ_SYM    void *
#define BYTE_SEQ_OFF    unsigned

typedef enum {
    #define pick_fp(enum,name,alt_name,win,alt_win,others,alt_others) FIX_ ## enum,
    #include "fppatche.h"
    #undef pick_fp
    FIX_SYM_OFFSET,     /* followed by a long */
    FIX_SYM_SEGMENT,    /* .. */
    FIX_SYM_RELOFF      /* .. */
} cg_fixups;

typedef unsigned    byte_seq_len;

#define STRUCT_BYTE_SEQ( x ) \
{ \
    byte_seq_len    length; \
    bool            relocs; \
    byte            data[x]; \
}

typedef struct byte_seq STRUCT_BYTE_SEQ( 1 ) byte_seq;
