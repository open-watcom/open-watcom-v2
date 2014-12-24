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
* Description:  Debugger input stack structure.
*
****************************************************************************/


typedef unsigned_8 inp_rtn_action; enum {
        INP_RTN_INIT,
        INP_RTN_EOL,
        INP_RTN_FINI
};

typedef unsigned_16 input_type; enum {
        INP_NIL         = 0x0000,
        INP_NORMAL      = 0x0000,
        INP_BREAK_POINT = 0x0001,
        INP_CAPTURED    = 0x0002,
        INP_DLG_CMD     = 0x0004,
        INP_NEW_LANG    = 0x0008,
        INP_STOP_PURGE  = 0x0010,
        INP_CMD_FILE    = 0x0020,
        INP_NO_CMD      = 0x0040,
        INP_HOLD        = 0x0080,
        INP_HOOK        = 0x0100,
        INP_REPLAYED    = 0x0200,
};

typedef struct input_stack {
        struct  input_stack     *link;
        const char              *scan;
        void                    *handle;
        bool                    (*rtn)( void *, inp_rtn_action );
        input_type              type;
        char                    *lang;
} input_stack;
