/*****************************************************************************
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
* Description:  Default section definitions.
*
*****************************************************************************/


#define SEC_ATTR_READONLY_DATA  (OWL_SEC_ATTR_DATA|OWL_SEC_ATTR_PERM_READ)

PICK( TEXT, ".text", OWL_SECTION_CODE, OBJ_DEF_ALIGNMENT )
PICK( DATA, ".data", OWL_SECTION_DATA, OBJ_DEF_ALIGNMENT )
PICK( BSS, ".bss", OWL_SECTION_BSS, OBJ_DEF_ALIGNMENT )
PICK( PDATA, ".pdata", OWL_SECTION_PDATA, OBJ_DEF_ALIGNMENT )
PICK( DEBUG_P, ".debug$P", OWL_SECTION_DEBUG, 0 )
PICK( DEBUG_S, ".debug$S", OWL_SECTION_DEBUG, 0 )
PICK( DEBUG_T, ".debug$T", OWL_SECTION_DEBUG, 0 )
PICK( RDATA, ".rdata", SEC_ATTR_READONLY_DATA, OBJ_DEF_ALIGNMENT )
PICK( XDATA, ".xdata", SEC_ATTR_READONLY_DATA, OBJ_DEF_ALIGNMENT )
PICK( YDATA, ".ydata", SEC_ATTR_READONLY_DATA, OBJ_DEF_ALIGNMENT )

#ifdef AS_PPC
PICK( RELDATA, ".reldata", OWL_SECTION_DATA, OBJ_DEF_ALIGNMENT )
PICK( TOCD, ".tocd", OWL_SECTION_DATA, OBJ_DEF_ALIGNMENT )
#endif
