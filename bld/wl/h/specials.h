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


/*
    Magical names that the linker knows about
*/

#ifdef export
#define SPECIAL( sym, val ) char sym[] = val
#else
#define SPECIAL( sym, val ) extern char sym[sizeof( val )]
#endif

SPECIAL( AutoGrpName,           "AUTO" );
SPECIAL( DataGrpName,           "DGROUP" );
SPECIAL( IDataGrpName,          ".idata" );
SPECIAL( TLSGrpName,            "tls" );

SPECIAL( BSSClassName,          "BSS" );
SPECIAL( CodeClassName,         "CODE" );
SPECIAL( FarDataClassName,      "FAR_DATA" );
SPECIAL( DataClassName,         "DATA" );
SPECIAL( BegCodeClassName,      "BEGCODE" );
SPECIAL( BegDataClassName,      "BEGDATA" );
SPECIAL( StackClassName,        "STACK" );
SPECIAL( ConstClassName,        "CONST" );
SPECIAL( TLSClassName,          "TLS" );
SPECIAL( _MSTypeClass,          "DEBTYP" );
SPECIAL( _MSLocalClass,         "DEBSYM" );
SPECIAL( _DwarfClass,           "DWARF" );

SPECIAL( BSSStartSym,           "_edata" );
SPECIAL( BSSEndSym,             "_end" );
SPECIAL( BSS_StartSym,          "__edata" );
SPECIAL( BSS_EndSym,            "__end" );
SPECIAL( ImportSymPrefix,       "__imp_" );
SPECIAL( TLSSym,                "__tls_used" );
SPECIAL( TocSymName,            ".toc" );

SPECIAL( BegTextSegName,        "BEGTEXT" );
SPECIAL( CommunalSegName,       "c_common" );
SPECIAL( CoffTextSegName,       ".text" );
SPECIAL( CoffDebugSymName,      ".debug$S" );
SPECIAL( CoffDebugTypeName,     ".debug$T" );
SPECIAL( CoffDebugPrefix,       ".debug" );
SPECIAL( CoffIDataSegName,      ".idata" );
SPECIAL( CoffRelocSegName,      ".reloc" );
SPECIAL( CoffTocSegName,        ".idata$4toc" );
SPECIAL( CoffPDataSegName,      ".pdata" );
SPECIAL( CoffReldataSegName,    ".reldata" );
SPECIAL( CoffImportRefName,     "_IMPORT_DESCRIPTOR" );
SPECIAL( TLSSegPrefix,          ".tls" );

SPECIAL( LinkerModule,          "LINKER MODULE" );

SPECIAL( GotSymName,            "_GLOBAL_OFFSET_TABLE_" );
SPECIAL( DynSymName,            "_DYNAMIC" );
SPECIAL( GotSecName,            ".got" );
SPECIAL( ElfIDataSecName,       ".imports" );

SPECIAL( RelASecName,           ".rela" );
