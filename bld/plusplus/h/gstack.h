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


#ifndef _GSTACK_H
#include "enum.h"
#include "class.h"
#include "datainit.h"
#include "template.h"
#include "fmttype.h"

typedef enum gstack_type {
    GS_DECL_SPEC        = 0x01,
    GS_ENUM_DATA        = 0x02,
    GS_DECL_INFO        = 0x03,
    GS_CLASS_DATA       = 0x04,
    GS_INIT_DATA        = 0x05,
    GS_TEMPLATE_DATA    = 0x06,
    GS_NULL             = 0x00
} gstack_type;

typedef struct global_stack GLOBAL_STACK;
struct global_stack {
    GLOBAL_STACK        *next;
    gstack_type         id;
    int                 : 0;
    union {
        ENUM_DATA       enumdata;
        CLASS_DATA      classdata;
        INITIALIZE_DATA initdata;
        TEMPLATE_DATA   templatedata;
        DECL_SPEC       *dspec;
        DECL_INFO       *dinfo;
    } u;
};

extern GLOBAL_STACK *GStackPush( GLOBAL_STACK **, gstack_type );
extern void GStackPop( GLOBAL_STACK ** );
extern gstack_type GStackType( GLOBAL_STACK * );

#define _GSTACK_H
#endif
