#ifndef _LOADER_H_F39997A6_88FC_434B_B339_554BE343B3E8
#define _LOADER_H_F39997A6_88FC_434B_B339_554BE343B3E8
/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*    Portions Copyright (c) 1989-2002 Novell, Inc.  All Rights Reserved.                      
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
*   This header file was generated for the OpenWatcom project by Carl Young
*       carl.young@keycomm.co.uk
*   Any problems or updates required, please either contact the author or
*   the OpenWatcom contributors forums. 
*       http://www.openwatcom.com/
*
* Description:  Provides a dumbed down version of the internal loader
*               definition record. All unused fields have been removed to
*               minimize any possible use of undocumented fields.
*
****************************************************************************/

#include <clib.h>

#pragma pack (push, 1)

/*
//  As you will be able to clearly see this structure is not well defined
//  as we are trying to protect ourselves from ourselves. Intimate knowledge
//  of this structure could lead to myopia and stunted growth.
*/
/* LoadDefinitionStructure */
typedef struct LoadDefinitionStructure
{
	struct LoadDefinitionStructure *	LDLink;
	LONG                                undef00[4];
	UINT32 /* UINT8 * */				LDCodeImageOffset;
	LONG                                undef01;
	UINT32 /* UINT8 * */				LDDataImageOffset;
	LONG                                undef02[6];
    LONG                                LDInitializationProcedure;
    LONG                                undef03[3];
	MEON								LDFileName[36];
}LoadDefinitionStructure;

#if !defined(MODULE_HANDLE)
#define MODULE_HANDLE MODULE_HANDLE
typedef LoadDefinitionStructure	MODULE_HANDLE;
#endif

#if !defined(LoadDefinitionStruct)
#define LoadDefinitionStruct LoadDefinitionStructure
#endif

#pragma pack (pop)

#endif /* _LOADER_H_F39997A6_88FC_434B_B339_554BE343B3E8 */
