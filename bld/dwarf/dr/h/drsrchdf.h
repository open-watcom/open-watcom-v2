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


#ifndef DRSRCHDF_H
#define DRSRCHDF_H

typedef enum {
    DR_DEPTH_FUNCTIONS  = 0x1,
    DR_DEPTH_CLASSES    = 0x2,
} dr_depth;

typedef enum {      // code in drsearch depends on the order */
    DR_SYM_FUNCTION,
    DR_SYM_CLASS,
    DR_SYM_ENUM,
    DR_SYM_TYPEDEF,
    DR_SYM_VARIABLE,
    DR_SYM_MACRO,
    DR_SYM_LABEL,
    DR_SYM_NOT_SYM      /* not a symbol - must be last */
} dr_sym_type;

typedef enum {
    DR_SEARCH_ALL,
    DR_SEARCH_FUNCTIONS,
    DR_SEARCH_CLASSES,
    DR_SEARCH_TYPES,
    DR_SEARCH_VARIABLES,
    DR_SEARCH_FRIENDS,
    DR_SEARCH_BASE,
    DR_SEARCH_MACROS,
    DR_SEARCH_NOT_SYM   /* should always be last */
} dr_search;

#endif // DRSRCHDF_H
