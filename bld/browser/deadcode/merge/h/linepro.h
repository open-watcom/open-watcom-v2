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


#ifndef _LINE_PROLOGUE_H
#define _LINE_PROLOGUE_H

// System includes --------------------------------------------------------

#include <wstd.h>
#include <string.h>

// Project includes -------------------------------------------------------

#include "dbgobj.h"
#include "fileinfo.h"
#include "dirinfo.h"

/*
 * LinePrologue -- used internally within LineSection
 */
class LinePrologue {
friend class MBRFile;
friend class DwarfFileMerger;
                                LinePrologue();
                                ~LinePrologue();

    bool                        operator==( const LinePrologue & pro );
    bool                        operator!=( const LinePrologue & pro );

    uint_32                     unit_length;            //
    uint_16                     version;                //
    uint_32                     prologue_length;        //

    uint_8                      min_instr;
    uint_8                      default_is_stmt;
    int_8                       line_base;
    uint_8                      line_range;
    uint_8                      opcode_base;
    uint_8 *                    opcode_lengths;

    DebugObjectVector           directories;
    DebugObjectVector           filenames;
};

#endif
