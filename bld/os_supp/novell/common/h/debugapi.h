#ifndef _DEBUGAPI_H_F39997A6_88FC_434B_B339_554BE343B3E8
#define _DEBUGAPI_H_F39997A6_88FC_434B_B339_554BE343B3E8

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
* Description:  Defines things required by NetWare's internal and alternate
*               debuggers.
*
****************************************************************************/

struct ProcessorStructure;
struct ResourceTagStructure;

#include <stkframe.h>

/* Position definitions */ 
#define ALWAYS_AT_FIRST					-1 
#define AT_FIRST						0 
#define AT_END							1 
#define ALWAYS_AT_END					2 

/* Breakpoint type definitions */ 
#define EXECUTION_BREAKPOINT			0 
#define WRITE_BREAKPOINT				1 
#define READ_WRITE_BREAKPOINT			3 

/* Breakpoint size definitions */ 
#define ONE_BYTE_BREAKPOINT				0 
#define TWO_BYTE_BREAKPOINT				1 
#define FOUR_BYTE_BREAKPOINT			3 

/* Exception flag definitions */ 
#define PROCESSOR_EXCEPTION_BIT			0x0001 
#define HAS_EXCEPTION_ERROR_CODE_BIT	0x0002 
#define DO_AES_DEBUG_BIT				0x0004 
#define HAS_OTHER_ERROR_CODE_BIT		0x0008 
#define ABEND_CALLED_BIT				0x0010 
#define HARD_ABEND_BIT					0x0020 
#define MSENGINE_BIT					0x0040 

/*
//	Exception number definitions 
//	Note: 
//
//	Numbers 0 - 31 are hardware processor exception numbers 
//	Numbers 32 - 999 are reserved for the internal OS 
//	Numbers 1000 - 4999 are available signal debug event 
//	Numbers 5000 and greater are reserved
//
*/
#define ENTER_DEBUGGER					256 
#define ENTER_DEBUGGER_EVENT			ENTER_DEBUGGER
#define KEYBOARD_BREAK					257 
#define KEYBOARD_BREAK_EVENT			KEYBOARD_BREAK
#define INVALID_INTERRUPT_ABEND			258 
#define INVALID_INTERRUPT_ABEND_EVENT	INVALID_INTERRUPT_ABEND
#define ASSEMBLY_ABEND					260 
#define ASSEMBLY_ABEND_EVENT			ASSEMBLY_ABEND
#define OTHER_PROCESSOR_BREAK			261 
#define OTHER_PROCESSOR_BREAK_EVENT		OTHER_PROCESSOR_BREAK
#define STATUS_DUMP						262 
#define STATUS_DUMP_EVENT				STATUS_DUMP 

#define START_NLM_EVENT					1000 
#define TERMINATE_NLM_EVENT				1001 
#define INIT_FAILED_NLM_EVENT			1002 

#define PROTECT_NLM_INIT				2000 
#define PROTECT_NLM_CHECK				2001 
#define PROTECT_NLM_EXIT				2002 

// from somewhere else
#define RETURN_TO_NEXT_DEBUGGER			0xFFFFFFFE		/* -2 */
#define RETURN_TO_NEXT_ALT_DEBUGGER		0xFFFFFFFF		/* -1 */
#define	RETURN_TO_PROGRAM				0x00000000		/*  0 */
#define RETURN_TO_INTERNAL_DEBUGGER		0x00000001		/*  1 */

#define START_NLM_EVENT					1000
#define TERMINATE_NLM_EVENT				1001
#define NLM_FAILED_INIT_EVENT			1002

#define START_THREAD_EVENT				1100
#define TERMINATE_THREAD_EVENT			1101
#define THREAD_BOOBY_TRAP_EVENT			1102
#define BREAKPOINT_FUNCTION_EVENT		1110

#define PROTECT_NLM_INIT				2000
#define PROTECT_NLM_CHECK				2001
#define PROTECT_NLM_EXIT				2002

#define PROCESSOR_EXCEPTION_BIT			0x0001
#define HAS_EXCEPTION_ERROR_CODE_SET	0x0002
#define DO_AES_DEBUG_BIT				0x0004
#define HAS_OTHER_ERROR_CODE_SET		0x0008
#define ABEND_CALLED_BIT				0x0010
#define HARD_ABEND_BIT					0x0020
#define MSENGINE_BIT					0x0040

//#define OBSOLETE_FRAME_BIT	?
#define EXTENDED_FRAME_BIT				0x010
#define TSS_FRAME_BIT					0x020

#define	MMU_PAGE_READ_WRITE_ENABLE		2
#define	SystemDomain					0

/* NW4 netware 4 specific??????????? */

#define THREAD_DEBUG					0x00000008
#define EXIT_LIMIT						0x20
#define	LO_DEBUG						0x00000004	/* contradicts LibC/NetWare.h - LD_MODULE_BEING_DEBUGGED == 0x40000000 */

/*
The following was from an informal e-mail. Use it to confirm data above

#define ALWAYS_AT_FIRST			-1
#define AT_FIRST				0
#define AT_END					1
#define ALWAYS_AT_END			2

#define RETURN_TO_NEXT_DEBUGGER	?
#define RETURN_TO_NEXT_ALT_DEBUGGER	?
#define RETURN_TO_PROGRAM	?
#define RETURN_TO_INTERNAL_DEBUGGER	?

START_NLM_EVENT	1000
TERMINATE_NLM_EVENT	1001
NLM_FAILED_INIT_EVENT	1002

START_THREAD_EVENT	1100
TERMINATE_THREAD_EVENT	1101
THREAD_BOOBY_TRAP_EVENT	1102
BREAKPOINT_FUNCTION_EVENT	1110

PROTECT_NLM_INIT	2000
PROTECT_NLM_CHECK	2001
PROTECT_NLM_EXIT	2002

PROCESSOR_EXCEPTION_BIT	0x0001
HAS_EXCEPTION_ERROR_CODE_SET	0x0002
DO_AES_DEBUG_BIT	0x0004
HAS_OTHER_ERROR_CODE_SET	0x0008
ABEND_CALLED_BIT	0x0010
HARD_ABEND_BIT	0x0020
MSENGINE_BIT	0x0040
OBSOLETE_FRAME_BIT	?
EXTENDED_FRAME_BIT	0x010
TSS_FRAME_BIT	0x020
*/


#endif /* _DEBUGAPI_H_F39997A6_88FC_434B_B339_554BE343B3E8 */
