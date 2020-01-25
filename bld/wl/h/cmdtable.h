/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Command parsing tables.
*
****************************************************************************/


extern parse_entry PosDbgMods[];
extern parse_entry DbgMods[];
extern parse_entry SysBeginOptions[];
extern parse_entry SysDeleteOptions[];
extern parse_entry SysEndOptions[];
extern parse_entry SortOptions[];
extern parse_entry Directives[];
extern parse_entry MainOptions[];
extern parse_entry SysDirectives[];
extern parse_entry Models[];
extern parse_entry Languages[];
extern parse_entry EndLinkOpt[];
#if defined( _PHARLAP ) || defined( _DOS16M ) || defined( _OS2 ) || defined( _ELF )
extern parse_entry RunOptions[];
#endif
extern parse_entry Strategies[];
extern parse_entry TransTypes[];
extern parse_entry QNXSegModel[];
extern parse_entry QNXSegDesc[];
extern parse_entry QNXFormats[];
extern parse_entry PharModels[];
extern parse_entry NovModels[];
extern parse_entry NovDBIOptions[];
extern parse_entry Sections[];
extern parse_entry SectOptions[];
extern parse_entry DosOptions[];
extern parse_entry SubFormats[];
extern parse_entry OS2FormatKeywords[];
extern parse_entry WindowsFormatKeywords[];
extern parse_entry NTFormatKeywords[];
extern parse_entry VXDFormatKeywords[];
extern parse_entry Init_Keywords[];
extern parse_entry Term_Keywords[];
extern parse_entry Exp_Keywords[];
extern parse_entry SegDesc[];
extern parse_entry SegTypeDesc[];
extern parse_entry SegModel[];
extern parse_entry CommitKeywords[];
extern parse_entry ELFFormatKeywords[];
extern parse_entry ZdosOptions[];
extern parse_entry RdosOptions[];
extern parse_entry RawOptions[];
extern parse_entry OrderOpts[];
extern parse_entry OrderClassOpts[];
extern parse_entry OrderSegOpts[];
extern parse_entry OutputOpts[];
