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
* Description:  Default file extension definitions
*
****************************************************************************/

/*       enum        text   */

pick1(  E_COMMAND,  ".lnk"  )
pick1(  E_MAP,      ".map"  )
pick1(  E_LIBRARY,  ".lib"  )
#if defined( __UNIX__ )
pick1(  E_OBJECT,   ".o"    )
#else
pick1(  E_OBJECT,   ".obj"  )
#endif
pick1(  E_LOAD,     ".exe"  )
pick1(  E_OVL,      ".ovl"  )
pick1(  E_DLL,      ".dll"  )
pick1(  E_PROTECT,  ".exp"  )
pick1(  E_NLM,      ".nlm"  )   /* netware loadable module  */
pick1(  E_LAN,      ".lan"  )   /* LAN driver               */
pick1(  E_DSK,      ".dsk"  )   /* disk driver              */
pick1(  E_NAM,      ".nam"  )   /* name space module        */
pick1(  E_NOV_MSL,  ".msl"  )   /* mirrored server link     */
pick1(  E_NOV_HAM,  ".ham"  )   /* host adapter module      */
pick1(  E_NOV_CDM,  ".cdm"  )   /* custom device module     */
pick1(  E_COM,      ".com"  )
pick1(  E_REX,      ".rex"  )
#if defined( __UNIX__ )
pick1(  E_QNX,      ""      )
#else
pick1(  E_QNX,      ".qnx"  )
#endif
pick1(  E_SYM,      ".sym"  )
pick1(  E_LBC,      ".lbc"  )
#if defined( __UNIX__ )
pick1(  E_ELF,      ""      )
#else
pick1(  E_ELF,      ".elf"  )
#endif
pick1(  E_ILK,      ".ilk"  )
pick1(  E_HEX,      ".hex"  )
pick1(  E_BIN,      ".bin"  )
pick1(  E_SYS,      ".sys"  )   /* ZDOS device driver */
pick1(  E_HWD,      ".hwd"  )   /* ZDOS hardware driver */
pick1(  E_FSD,      ".fsd"  )   /* ZDOS file system driver */
pick1(  E_RDV,      ".rdv"  )   /* RDOS device driver */
