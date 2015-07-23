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


//
// PCREGDEF.H -- define precompiled header registration
//
// PC_REGISTER -- defines symbolic index and global function name
//
// This file is never directly #included except by PCHEADER.C.
// Use the following:
//
//  * PCHEADER.H        for constants and R/W prototypes
//
// Date         By              Reason
// ====         ==              ======
// 93/12/23     A.F.Scian       defined
//
// ********************************************************************
// any changes here should result in a version change in the PCH header
// ********************************************************************
//
#ifndef PCH_EXEC
#define PCH_EXEC( p1, p2 )
#endif
#ifndef PCH_RELOC
#define PCH_RELOC( p1, p2 )
#endif
//
// PCH_EXEC -- PCH Read/Write Functions
//
// constraints:
//      - NAME
//          - must be before modules that use names
//      - MODULE
//          - must be before CGIO because ->open_ins in BLK_INITs in LABEL
//            must be zapped when CGIO processes ICs (we don't want the CGIO
//            zaps to be ruined by reading in BLK_INITs)
//
PCH_EXEC( CMACADD,              Macros )
PCH_EXEC( NAME,                 Names )
PCH_EXEC( STRINGL,              StringPool )
PCH_EXEC( CAUX,                 Pragmas )
PCH_EXEC( SRCFILE,              SrcFiles )
PCH_EXEC( SYMLOCN,              TokenLocns )
PCH_EXEC( SEGMENT,              Segments )
PCH_EXEC( TYPESIG,              TypeSigs )
PCH_EXEC( RTTI,                 RttiDescriptors )
PCH_EXEC( HASHTAB,              HashTables )
PCH_EXEC( REWRITE,              Rewrites )
PCH_EXEC( TEMPLATE,             Templates )
PCH_EXEC( PTREE,                PTrees )
PCH_EXEC( PTREEDEC,             Ptds )
PCH_EXEC( CONPOOL,              ConstantPool )
PCH_EXEC( SCOPE,                Scopes )
PCH_EXEC( TYPE,                 Types )
PCH_EXEC( CLASS,                Bases )
PCH_EXEC( MODULE,               ModuleData )
PCH_EXEC( CGIO,                 CGFiles )
PCH_EXEC( CGFRONT,              FrontData )
PCH_EXEC( CDOPT,                CdOptData )
PCH_EXEC( YTAB,                 ParserData )
PCH_EXEC( OPOVLOAD,             OperatorOverloadData )
PCH_EXEC( FNNAME,               OpNames )
PCH_EXEC( RTFUN,                RTFNames )
PCH_EXEC( RTNGEN,               Generators )
PCH_EXEC( CPRAGMA,              PragmaData )
PCH_EXEC( MESSAGE,              ErrWarnData )
PCH_EXEC( INCALIAS,             IncAlias )
PCH_EXEC( STATS,                Stats )
//PCH_EXEC( CARVE,                Test )
PCH_EXEC( PCHEADER,             Verify )

//
// PCH_RELOC -- PCH Relocation Functions
//
PCH_RELOC( TYPE,                Types )

#undef PCH_EXEC
#undef PCH_RELOC
