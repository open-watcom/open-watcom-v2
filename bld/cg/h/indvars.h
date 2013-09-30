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


typedef enum {
        IV_DEAD         = 0x0001, /* may no longer be used (from inner loop) */
        IV_SURVIVED     = 0x0002, /* survived one pass of loop */
        IV_ALIAS        = 0x0004, /* alias for another induction variable */
        IV_BASIC        = 0x0008, /* is a basic induction variable (i += const) */
        IV_TOP          = 0x0010, /* the end result of an induction expression */
        IV_INTRODUCED   = 0x0020, /* a new variable introduced due to reduction */
        IV_INDEXED      = 0x0040, /* a new variable introduced due to reduction */
        IV_USED         = 0x0080, /* a new variable introduced due to reduction */
        IV_NOREPLACE    = 0x0100, /* can't be replaced due to aliases */
} ind_var_state;

#define _IsV( var, bits )               (((var)->state & (bits) ) != 0)
#define _IsntV( var, bits )             (((var)->state & (bits) ) == 0)
#define _SetV( var, bits )              (var)->state |= (bits);
#define _ClrV( var, bits )              (var)->state &= ~(bits);

#define _ChkLoopUsage(x,b)              (((x)->v.block_usage & (var_usage)(b)) != 0)
#define _SetLoopUsage(x,b)              ((x)->v.block_usage = (var_usage)(b))

typedef signed_16       invar_id;
typedef struct invariant {              /*  an invariant list is */
        signed_32               times;  /*  + <v>*times */
        union  name             *name;  /*  this is <v> */
        struct invariant        *next;  /*  ... add the next one */
        invar_id                id;
} invariant;

typedef struct induction {              /* an induction variable is */
        signed_32               times;     /*  ( <i>*times ) */
        union name              *ivtimes;  /*     *ivtimes */
        struct invariant        *invar;    /*     +invariant_list[*ivtimes?] */
        signed_32               plus2;     /*     +plus2*ivtimes */
        signed_32               plus;      /*  .. +plus */
        union  name             *name;     /*  this is <j> */
        struct induction        *prev;     /*  link in list of induction vars */
        struct induction        *basic;    /*  this is <i> */
        struct induction        *next;     /*  link in list of induction vars */
        struct instruction      *ins;      /*  this is <j> = <j> + C */
        struct block            *header;   /*  this is the loop header */
        struct induction        *alias;    /*  points to an identical var */
        invar_id                lasttimes; /*  these first invars *ivtimes */
        ind_var_state           state;     /*  flags */
        type_class_def          type_class;/*  the type of <j> */
        int                     use_count; /*  how many times seen in loop */
        int                     index_use_count; /*  how many times as index? */
} induction;

typedef enum {
        IVU_UNUSED,
        IVU_USED_AS_INVARIANT,
        IVU_USED_AS_INDEX,
        IVU_USED_AS_OPERAND
} iv_usage;

typedef enum {
        VU_VARIANT              = 0x01,
        VU_INVARIANT            = 0x02,
        VU_INVARIANT_ADDRESS    = 0x04,
        VU_VARIED_ONCE          = 0x08
} variance;
