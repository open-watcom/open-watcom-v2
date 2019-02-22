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


#define MENU_LIT( a )
#define MENU_ITEM( a, b )           a,
#define MENU_CASCADE( a, b, c )     a,
#define MENU_CASCADE_DUMMY( a, b )  a,
#define MENU_BAR

// level 0 main menu items must be multiples of MAIN_MENU_MULT
#define MAIN_MENU_MULT          50
#define MENU_START_ID( a )      (a * MAIN_MENU_MULT)
#define MAIN_MENU_ID( a )       (a - (a % MAIN_MENU_MULT))

enum {
    MENU_MAIN_FILE = MENU_START_ID( 1 ),
    #include "mmfile.h"

    MENU_MAIN_SEARCH = MENU_START_ID( 2 ),
    #include "mmsearch.h"

    MENU_MAIN_RUN = MENU_START_ID( 3 ),
    #include "mmrun.h"

    MENU_MAIN_BREAK = MENU_START_ID( 4 ),
    #include "mmbreak.h"

    MENU_MAIN_CODE = MENU_START_ID( 5 ),
    #include "mmcode.h"

    MENU_MAIN_HELP = MENU_START_ID( 6 ),
    #include "mmhelp.h"

    MENU_MAIN_DATA = MENU_START_ID( 7 ),
    #include "mmdat.h"

    MENU_MAIN_WINDOW = MENU_START_ID( 8 ),
    #include "mmwind.h"

    MENU_MEMORY = MENU_START_ID( 9 ),
    #include "menumem.h"
    MENU_MEMORY_FIRST_TYPE,

    MENU_MODULES = MENU_START_ID( 10 ),
    #include "menumod.h"

    MENU_FUNCTIONS = MENU_START_ID( 11 ),
    #include "menufunc.h"

    MENU_FILE = MENU_START_ID( 12 ),
    #include "menufile.h"
    #include "mfilshow.h"

    MENU_REGISTER = MENU_START_ID( 13 ),
    #include "menureg.h"
    MENU_REGISTER_TOGGLES,

    MENU_FPU = MENU_START_ID( 14 ),
    #include "menufpu.h"

    MENU_VAR = MENU_START_ID( 15 ),
    #include "menuvar.h"
    #include "mvartype.h"
    #include "mvaropt.h"
    #include "mvarshow.h"
    #include "mvarclas.h"

    MENU_BREAK = MENU_START_ID( 16 ),
    #include "menubrk.h"

    MENU_CALL = MENU_START_ID( 17 ),
    #include "menucall.h"

    MENU_ASM = MENU_START_ID( 18 ),
    #include "menuasm.h"
    #include "masmopt.h"
    #include "masmshow.h"
    MENU_ASM_TOGGLES,

    MENU_IO = MENU_START_ID( 19 ),
    #include "menuio.h"
    MENU_IO_FIRST_TYPE,

    MENU_GLOBALS = MENU_START_ID( 20 ),
    #include "menuglob.h"

    MENU_SEARCH = MENU_START_ID( 21 ),
    #include "menusrch.h"

    MENU_TOOL = MENU_START_ID( 22 ),
    #include "menutool.h"

    MENU_THREAD = MENU_START_ID( 23 ),
    #include "menutrd.h"

    MENU_MAIN_ACTION = MENU_START_ID( 24 ),

    MENU_IMAGE = MENU_START_ID( 25 ),
    #include "menuimg.h"

    MENU_MACRO = MENU_START_ID( 26 ),
    #include "menumac.h"

    MENU_MAIN_UNDO = MENU_START_ID( 27 ),
    #include "mmundo.h"

    MENU_REPLAY = MENU_START_ID( 28 ),
    #include "menurep.h"

    MENU_RUN_THREAD = MENU_START_ID( 29 ),
    #include "menurtrd.h"

    MENU_NONE   = 10000
};

#undef MENU_LIT
#undef MENU_ITEM
#undef MENU_CASCADE
#undef MENU_CASCADE_DUMMY
#undef MENU_BAR
