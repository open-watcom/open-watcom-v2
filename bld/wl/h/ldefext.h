/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Default file extension definitions
*
****************************************************************************/

/*       enum        text   */

pick( E_COMMAND,  "lnk" )
pick( E_MAP,      "map" )
pick( E_LIBRARY,  "lib" )
#if defined( __UNIX__ )
pick( E_OBJECT,   "o"   )
#else
pick( E_OBJECT,   "obj" )
#endif
pick( E_LOAD,     "exe" )
pick( E_OVL,      "ovl" )
pick( E_DLL,      "dll" )
pick( E_PROTECT,  "exp" )
pick( E_NLM,      "nlm" )   /* netware loadable module  */
pick( E_LAN,      "lan" )   /* LAN driver               */
pick( E_DSK,      "dsk" )   /* disk driver              */
pick( E_NAM,      "nam" )   /* name space module        */
pick( E_NOV_MSL,  "msl" )   /* mirrored server link     */
pick( E_NOV_HAM,  "ham" )   /* host adapter module      */
pick( E_NOV_CDM,  "cdm" )   /* custom device module     */
pick( E_COM,      "com" )
pick( E_REX,      "rex" )
#if defined( __UNIX__ )
pick( E_QNX,      ""    )
#else
pick( E_QNX,      "qnx" )
#endif
pick( E_SYM,      "sym" )
pick( E_LBC,      "lbc" )
#if defined( __UNIX__ )
pick( E_ELF,      ""    )
#else
pick( E_ELF,      "elf" )
#endif
pick( E_ILK,      "ilk" )
pick( E_HEX,      "hex" )
pick( E_BIN,      "bin" )
pick( E_SYS,      "sys" )   /* ZDOS device driver */
pick( E_HWD,      "hwd" )   /* ZDOS hardware driver */
pick( E_FSD,      "fsd" )   /* ZDOS file system driver */
pick( E_RDV,      "rdv" )   /* RDOS device driver */
pick( E_EFI,      "efi" )   /* EFI */
