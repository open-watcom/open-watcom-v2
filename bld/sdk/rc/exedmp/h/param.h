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


#ifndef PARAM_H
#define PARAM_H

#include "common.h"
#include "strings.h"

typedef struct Parameters {
    bool        dumpOffsets;
    bool        dumpHexHeaders;
    bool        dumpHexData;
    bool        dumpInterpretation;
    bool        dumpExeHeaderInfo;
    bool        dumpResObjectInfo;
    bool        printRuler;
    bool        specificType;
    unsigned_16 specificTypeID;
    int         indentSpaces;
    int         hexIndentSpaces;
    char       *filename;
} Parameters;

/* default parameters */

#define DEF_DUMPOFFSETS         true
#define DEF_DUMPHEXHEADERS      true
#define DEF_DUMPHEXDATA         true
#define DEF_DUMPINTERPRETATION  true
#define DEF_DUMPEXEHEADERINFO   false
#define DEF_DUMPRESOBJECTINFO   true
#define DEF_PRINTRULER          true
#define DEF_SPECIFICTYPE        false
#define DEF_SPECIFICTYPEID      RT_NONE
#define DEF_INDENTSPACES        4
#define DEF_HEXINDENTSPACES     0

/* fix MSG_HELP in strings.h when you fix these */

#define OPT_DUMPOFFSETS         'o'
#define OPT_DUMPHEXHEADERS      'h'
#define OPT_DUMPHEXDATA         'd'
#define OPT_DUMPINTERPRETATION  'i'
#define OPT_DUMPEXEHEADERINFO   'x'
#define OPT_DUMPRESOBJECTINFO   'r'
#define OPT_PRINTRULER          'l'
#define OPT_INDENTSPACES        's'
#define OPT_HEXINDENTSPACES     'n'
#define OPT_SPECIFICTYPE        't'


void defaultParam( Parameters *param );
bool loadParam( Parameters *param, int count, char *params[] );

#endif
