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


#define DEFINE_RC_STRINGS

#ifdef DEFINE_RC_STRINGS

#include "guimsgs.h"

#define LITSTR( sym, val )
#define LIT( sym )      (_LIT_GUI_##sym)
#define LITREF( sym ) &LIT( sym )

#define pick( a, b, c ) extern char *LIT( a );
#include "gui.msg"
#undef pick

extern bool GUIInitInternalStringTable( void );
extern bool GUIFiniInternalStringTable( void );

#else

#ifdef DEFINE_STRINGS
#define LITSTR( sym, val ) const char _Literal_##sym[] = val;
#else
#ifdef __386__
    #define MAKEFAR
#else
    #ifdef __MEDIUM__
        #define MAKEFAR
    #else
        #define MAKEFAR far
    #endif
#endif
#define LITSTR( sym, val ) extern char MAKEFAR _Literal_##sym[sizeof( val )];
#endif
#define LIT( sym )      (_Literal_##sym)

#ifdef JAPANESE_MESSAGES
#define pick( a,b,c ) LITSTR( a,c )
#else
#define pick( a,b,c ) LITSTR( a,b )
#endif
#include "gui.msg"
#undef pick

#endif
