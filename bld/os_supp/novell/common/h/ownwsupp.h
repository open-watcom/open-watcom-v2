#ifndef _OWNWSUPP_H_F39997A6_88FC_434B_B339_554BE343B3E8
#define _OWNWSUPP_H_F39997A6_88FC_434B_B339_554BE343B3E8

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
* Description:  This is a special file that defines some of the scalar types
*               used by the OpenWatcom NetWare code. It was created to
*               minimize any usage of the SDK code.
*
****************************************************************************/

#ifdef __cplusplus
#define		CPP_START	extern "C"{
#define		CPP_END		}
#else
#define		CPP_START
#define		CPP_END
#endif

#ifndef LONG
#define LONG LONG
typedef unsigned long	LONG;
#endif

#ifndef BYTE
#define BYTE BYTE
typedef unsigned char	BYTE;
#endif

#ifndef WORD
#define WORD WORD
typedef unsigned short	WORD;
#endif

#ifndef	MEON
#define	MEON MEON
typedef	unsigned char	MEON;
#endif

#ifndef	MEON_STRING
#define	MEON_STRING MEON_STRING
typedef	unsigned char	MEON_STRING;
#endif

#ifndef	UINT8
#define	UINT8 UINT8
typedef	unsigned char	UINT8;
#endif

#ifndef	UINT16
#define	UINT16 UINT16
typedef	unsigned short	UINT16;
#endif


#ifndef	UINT32
#define UINT32 UINT32
typedef	unsigned int	UINT32;
#endif

#ifndef FALSE
#define FALSE	0
#endif

#ifndef TRUE
#define TRUE	1
#endif

#ifndef NULL
#ifdef __cplusplus
#define NULL ((void*)0)
#else
#define NULL 0
#endif
#endif

/*
//	Miscellaneous #defines that should be somewhere else
*/

#define PrimaryDataStream			0			/*	from nlm\nwfattr.h	*/

#define NOCHECK						0			/*	bindery stuff		*/
#define CHECK						0xFF		/*	bindery stuff		*/

#define _MAX_VOLUME					16			/*	max len of volume	*/

#include <nw_sigs.h>	/* NetWare resource signatures */


#endif /* _OWNWSUPP_H_F39997A6_88FC_434B_B339_554BE343B3E8 */
