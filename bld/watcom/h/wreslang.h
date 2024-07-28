/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2024 The Open Watcom Contributors. All Rights Reserved.
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


#ifndef _WRESLANG_H
#define _WRESLANG_H

#include "wreslang.rh"

typedef enum {
    LANG_RLE_NONE = -1,
    #define LANG_RLE_DEF( id, val, dbcs )   LANG_##val = val,
    LANG_RLE_DEFS
    #undef LANG_RLE_DEF
    LANG_RLE_MAX,
} wres_lang_id;

#define LANG_RLE_FIRST_INTERNATIONAL    LANG_RLE_JAPANESE

#ifdef __WATCOMC__
#ifndef _COMDEF_H_INCLUDED
 #include <_comdef.h>
#endif
_WCRTLINK extern wres_lang_id _WResLanguage( void );
#else
extern wres_lang_id _WResLanguage( void );
#endif

#endif
