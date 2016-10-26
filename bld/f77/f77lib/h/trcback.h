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
* Description:  Define traceback information
*
****************************************************************************/

// This is the basic traceback structure that must be common to optimizing
// and load 'n go compilers. If the line field is TB_LG it indicates that the
// traceback is a load 'n go traceback

typedef struct traceback {
    struct traceback PGM    *link;          // pointer to previous traceback
    uint                    line;           // current line
    char                    *name;          // current module
} traceback;

#define TB_LG           (~(uint)0)
#define TB_LG_DB        (~(uint)0-1)

#if defined( __MT__ )

    #define _EXCURR         (*(traceback **)&(__FTHREADDATAPTR->__ExCurr))

#else

    extern traceback PGM    *ExCurr;        // head of traceback list
    #define _EXCURR         (ExCurr)

#endif
