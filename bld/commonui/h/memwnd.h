/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2015-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Interface to memory display window.
*
****************************************************************************/


#ifndef _MEMWND_H_INCLUDED
#define _MEMWND_H_INCLUDED

#include <stdlib.h>
#include "selinfo.h"
#include "hdlinfo.h"

#define OFF_OFFSET                      101

#define MEMINFO_SAVE                    400
#define MEMINFO_SAVE_TO                 401
#define MEMINFO_SHOW                    403
#define MEMINFO_AUTO_POS                404
#define MEMINFO_EXIT                    405

#define MEMINFO_BYTE                    410
#define MEMINFO_WORD                    411
#define MEMINFO_DWORD                   412
#define MEMINFO_CODE_16                 413
#define MEMINFO_CODE_32                 414

#define MEMINFO_OFFSET                  420

#define MEM_DISPLAY_CLASS               "watmemdisplayclass"
#define MEM_DUMP_LIST                   100

#ifdef __NT__
    #define SCROLL_RANGE                0xFFFF
#else
    #define SCROLL_RANGE                100
#endif

#define MEMWND_MAX_FNAME        _MAX_PATH

typedef struct asminfo{
    DWORD       increment;
    WORD        usage_cnt;
    bool        big;
    bool        dummy;
    DWORD       data[1];        /* dynamic array */
} AsmInfo;

typedef struct memdata {
    WORD            sel;                    /* selector being displayed */
    uint_32         limit;                  /* limit of this selector */
    uint_32         base;                   /* base offset */
    unsigned        lastline;
    int             width;
    ctl_id          disp_type;              /* BYTE, WORD, DWORD or CODE (menu item value) */
    uint_32         ins_cnt;
    AsmInfo         *asm;
    uint_32         offset;
    HWND            scrlbar;                /* handle of scroll bar */
    HWND            parent;                 /* parent of this window */
    HWND            dialog;                 /* handle of mem info wnd */
    unsigned char   bytesdisp;
    bool            isdpmi :1;              /* is this a dpmi item */
    bool            maximized :1;           /* is window maximized */
    bool            autopos :1;             /* is memory info window auto positioned */
    bool            curwnd :1;              /* was the single window flag set
                                               when this window was created */
} MemWndInfo;

typedef enum {
    WND_MULTI,                          /* allow multiple windows */
    WND_SINGLE,                         /* don't create more than one window */
    WND_REPLACE                         /* destroy the previous window
                                           when a new one is created */
} MultWnd;

typedef struct memconfig {
    int         xpos;                   /* x coordinate of mem window */
    int         ypos;                   /* y coordinate of mem window */
    int         xsize;                  /* width of mem window */
    int         ysize;                  /* height of mem window */
    ctl_id      data_type;              /* display bytes, words, dwords (menu item value) */
    ctl_id      code_type;              /* display 16 or 32 bit code (menu item value) */
    char        *appname;               /* name of calling application */
    MultWnd     allowmult;              /* what to do when the user tries to open more than one window */
    char        fname[MEMWND_MAX_FNAME];/* file name of saves */
    bool        init :1;                /* for internal use only */
    bool        maximized :1;           /* is mem window maximized */
    bool        disp_info :1;           /* display memory info by default */
    bool        autopos_info :1;        /* keep the memory info window adjacent to the memory display window */
    bool        forget_pos :1;          /* don't update the position and size of the memory display window */
} MemWndConfig;


/*******************************************************************
 * IMPORTANT IMPORTANT   READ THIS OR ELSE...  IMPORTANT IMPORTANT
 *******************************************************************
 * RegMemWnd       - must be called by the first instance of a
 *                   user application
 * SetDefMemConfig - must be called in the initializeation code
 *                   of each instance of an application that will later
 *                   call SetMemWndConfig.
 * SetMemWndConfig - when calling SetMemWndConfig:
 *                      1. Call GetMemWndConfig to initialize the
 *                         MemWndConfig structure
 *                      2. modify the fields of the MemWndConfig structure
 *                         as desired
 *                      3. Call SetMemWndConfig
 */

extern bool    RegMemWndClass( HANDLE instance );
extern void    SetDefMemConfig( void );
extern void    SetMemWndConfig( MemWndConfig *cfg );
extern void    GetMemWndConfig( MemWndConfig *cfg );
extern void    GetMemWndDefault( MemWndConfig *cfg );
extern HWND    DispMem( HANDLE instance, HWND parent, WORD seg, bool isdpmi );
#ifdef __NT__
extern HWND    DispNTMem( HWND parent, HANDLE instance, HANDLE prochdl, DWORD offset, DWORD limit, char *title );
#endif

/************************************************************************
 * ALL FUNCTIONS PROTOTYPED BELOW THIS POINT ARE FOR INTERNAL USE ONLY
 ************************************************************************/

extern char    MkHexDigit( char ch );

/* memwndcd.c */
extern void    RedrawAsCode( HDC dc, MemWndInfo *info );
extern void    ScrollAsm( HWND hwnd, WORD wparam, WORD pos, MemWndInfo *info );
extern bool    NeedScrollBar( MemWndInfo *info );
extern void    SetFuzzyOffset( MemWndInfo *info, uint_32 offset );
extern void    DumpMemAsm( MemWndInfo *info, int hdl );
extern void    RegDisasmRtns( void );
extern uint_32 GetInsCnt( MemWndInfo *info, uint_32 offset );

#endif /* _MEMWND_H_INCLUDED */
