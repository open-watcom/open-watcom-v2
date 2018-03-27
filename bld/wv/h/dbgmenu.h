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


#define MAIN_MENU_MULT  50 // level 0 main menu items must be multiples of 50

#define MENU_LIT( a )
#define MENU_ITEM( a, b ) a,
#define MENU_CASCADE( a, b, c ) a,
#define MENU_BAR

enum {
    MENU_MAIN_FILE = 100,
    #include "mmfile.h"

    MENU_MAIN_SEARCH = 200,
    #include "mmsearch.h"

    MENU_MAIN_RUN = 300,
    #include "mmrun.h"

    MENU_MAIN_BREAK = 400,
    #include "mmbreak.h"

    MENU_MAIN_CONFIG = 500,
    #include "mmconfig.h"

    MENU_MAIN_CODE = 600,
    #include "mmcode.h"

    MENU_MAIN_HELP = 700,
    #include "mmhelp.h"

    MENU_MAIN_DATA = 800,
    #include "mmdat.h"

    MENU_MAIN_WINDOW = 900,
    #include "mmwind.h"

    MENU_MEMORY = 1000,
    #include "menumem.h"
    MENU_MEMORY_FIRST_TYPE,

    MENU_MODULES = 1100,
    #include "menumod.h"

    MENU_FUNCTIONS = 1200,
    #include "menufunc.h"

    MENU_FILE = 1300,
    #include "menufile.h"
    #include "mfilshow.h"

    MENU_REGISTER = 1400,
    #include "menureg.h"
    MENU_REGISTER_TOGGLES,

    MENU_FPU = 1500,
    #include "menufpu.h"

    MENU_VAR = 1600,
    #include "menuvar.h"
    #include "mvartype.h"
    #include "mvaropt.h"
    #include "mvarshow.h"
    #include "mvarclas.h"

    MENU_BREAK = 1700,
    #include "menubrk.h"

    MENU_CALL = 1800,
    #include "menucall.h"

    MENU_ASM = 1900,
    #include "menuasm.h"
    #include "masmopt.h"
    #include "masmshow.h"
    MENU_ASM_TOGGLES,

    MENU_IO = 2000,
    #include "menuio.h"
    MENU_IO_FIRST_TYPE,

    MENU_GLOBALS = 2100,
    #include "menuglob.h"

    MENU_SEARCH = 2200,
    #include "menusrch.h"

    MENU_TOOL = 2300,
    #include "menutool.h"

    MENU_THREAD = 2400,
    #include "menutrd.h"

    MENU_MAIN_ACTION = 2500,

    MENU_IMAGE = 2600,
    #include "menuimg.h"

    MENU_MACRO = 2700,
    #include "menumac.h"

    MENU_MAIN_UNDO = 2800,
    #include "mmundo.h"

    MENU_REPLAY = 2900,
    #include "menurep.h"

    MENU_RUN_THREAD = 3000,
    #include "menurtrd.h"

    MENU_NONE   = 10000
};

#undef MENU_LIT
#undef MENU_ITEM
#undef MENU_CASCADE
#undef MENU_BAR
