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


#include "owl.h"

typedef owl_reloc_type axp_reloc;

#define AXP_RELOC_BRANCH        OWL_RELOC_BRANCH_REL
#define AXP_RELOC_HALF_HIGH     OWL_RELOC_HALF_HI
#define AXP_RELOC_PAIR          OWL_RELOC_PAIR
#define AXP_RELOC_HALF_LOW      OWL_RELOC_HALF_LO
#define AXP_RELOC_WORD          OWL_RELOC_WORD
#define AXP_RELOC_SECT          OWL_RELOC_SECTION_INDEX
#define AXP_RELOC_OFF_SECT      OWL_RELOC_SECTION_OFFSET
