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
* Description:  Command parsing tables.
*
****************************************************************************/


#include "linkstd.h"
#include "command.h"
#include "cmdall.h"
#include "cmdos2.h"
#include "cmdqnx.h"
#include "cmdnov.h"
#include "cmdelf.h"
#include "cmdphar.h"
#include "cmddos.h"
#include "cmdzdos.h"
#include "cmdrdv.h"
#include "cmdraw.h"
#include "cmd16m.h"
#include "cmdline.h"


parse_entry     PosDbgMods[] = {
    "LInes",        &ProcLine,          MK_ALL, 0,
    "Types",        &ProcType,          MK_ALL, 0,
    "LOcals",       &ProcLocal,         MK_ALL, 0,
//  "STatic",       &ProcDBIStatic,     MK_ALL, 0,
    "All",          &ProcAll,           MK_ALL, 0,
#ifdef _NOVELL
    "ONLyexports",  &ProcExportsDBI,    MK_NOVELL, 0,
#endif
    NULL
};

 parse_entry     DbgMods[] = {
    "Watcom",       &ProcWatcomDBI,     MK_ALL, 0,
    "Dwarf",        &ProcDwarfDBI,      MK_ALL, 0,
    "Codeview",     &ProcCodeviewDBI,   MK_ALL, 0,
#ifdef _NOVELL
    "Novell",       &ProcNovDBI,        MK_NOVELL, 0,
#endif
    NULL
};

parse_entry     SysBeginOptions[] = {
    "Begin",         &ProcSysBegin,     MK_ALL, 0,
    NULL
};

parse_entry     SysDeleteOptions[] = {
    "DELete",        &ProcSysDelete,    MK_ALL, 0,
    NULL
};

parse_entry     SysEndOptions[] = {
    "End",          &ProcSysEnd,        MK_ALL, 0,
    NULL
};

parse_entry     SortOptions[] = {
    "ALPhabetical", &ProcAlphabetical,  MK_ALL, 0,
    "GLobal",       &ProcGlobal,        MK_ALL, 0,
    NULL
};

parse_entry     Directives[] = {
    "File",         &ProcFiles,         MK_ALL, CF_HAVE_FILES,
    "MODFile",      &ProcModFiles,      MK_ALL, 0,
    "Library",      &ProcLibrary,       MK_ALL, 0,
    "Name",         &ProcName,          MK_ALL, 0,
    "OPtion",       &ProcOptions,       MK_ALL, 0,
    "Debug",        &ProcDebug,         MK_ALL, 0,
    "SYStem",       &ProcSystem,        MK_ALL, 0,
    "LIBPath",      &ProcLibPath,       MK_ALL, 0,
    "LIBFile",      &ProcLibFile,       MK_ALL, CF_HAVE_FILES,
    "Path",         &ProcPath,          MK_ALL, 0,
    "FORMat",       &ProcFormat,        MK_ALL, 0,
    "MODTrace",     &ProcModTrace,      MK_ALL, 0,
    "SYMTrace",     &ProcSymTrace,      MK_ALL, CF_AFTER_INC,
    "Alias",        &ProcAlias,         MK_ALL, CF_AFTER_INC,
    "REFerence",    &ProcReference,     MK_ALL, CF_AFTER_INC,
    "DISAble",      &ProcDisable,       MK_ALL, 0,
    "SOrt",         &ProcSort,          MK_ALL, 0,
    "LANGuage",     &ProcLanguage,      MK_ALL, 0,
    "STARTLink",    &ProcStartLink,     MK_ALL, 0,
    "OPTLIB",       &ProcOptLib,        MK_ALL, 0,
    "ORDer",        &ProcOrder,         MK_ALL, 0,
    "OUTput",       &ProcOutput,        MK_ALL, 0,
#ifdef _OS2
    "RESource",     &ProcResource,      MK_PE, 0,
    "COMmit",       &ProcCommit,        MK_PE, 0,
    "ANONymousexport",&ProcAnonExport,  MK_OS2, CF_AFTER_INC,
#endif
#if defined( _NOVELL ) || defined( _OS2 )
    "IMPort",       &ProcImport,        (MK_NOVELL | MK_ELF | MK_OS2 | MK_PE), CF_AFTER_INC,
    "EXPort",       &ProcExport,        (MK_NOVELL | MK_ELF | MK_OS2 | MK_PE | MK_WIN_VXD ), CF_AFTER_INC,
#endif
#if defined( _OS2 ) || defined( _QNXLOAD )
    "SEGment",      &ProcSegment,       (MK_QNX | MK_OS2 | MK_PE | MK_WIN_VXD ), 0,
#endif
#ifdef _EXE
    "OVerlay",      &ProcOverlay,       MK_OVERLAYS, 0,
    "Begin",        &ProcBegin,         MK_OVERLAYS, 0,
    "FIXedlib",     &ProcFixedLib,      MK_OVERLAYS, 0,
    "NOVector",     &ProcNoVector,      MK_OVERLAYS, CF_AFTER_INC,
    "VEctor",       &ProcVector,        MK_OVERLAYS, CF_AFTER_INC,
    "FORCEVEctor",  &ProcForceVector,   MK_OVERLAYS, CF_AFTER_INC,
#endif
#if defined( _PHARLAP ) || defined( _DOS16M ) || defined( _OS2 ) || defined( _ELF )
    "RUntime",      &ProcRuntime,       (MK_PHAR_LAP | MK_DOS16M | MK_PE | MK_ELF), 0,
#endif
#ifdef _NOVELL
    "MODUle",       &ProcModule,        MK_NOVELL | MK_ELF, 0,
#endif
#ifdef _DOS16M
    "MEMory",       &ProcMemory16M,     MK_DOS16M, 0,
    "TRansparent",  &ProcTransparent,   MK_DOS16M, 0,
#endif
#if defined( _OS2 ) || defined( _EXE ) || defined ( _QNXLOAD )
    "NEWsegment",   &ProcNewSegment,    (MK_OS2_16BIT | MK_DOS | MK_QNX), 0,
#endif
#ifdef _INT_DEBUG
    "Xdbg",         &ProcXDbg,          MK_ALL, 0,
    "INTDBG",       &ProcIntDbg,        MK_ALL, 0,
#endif
    NULL
};


parse_entry     MainOptions[] = {
    "Map",          &ProcMap,           MK_ALL, 0,
    "MAPLines",     &ProcMapLines,      MK_ALL, 0,
    "STack",        &ProcStack,         MK_ALL, 0,
    "NODefaultlibs",&ProcNoDefLibs,     MK_ALL, CF_NO_DEF_LIBS,
    "Quiet",        &ProcQuiet,         MK_ALL, 0,
    "Dosseg",       &ProcDosSeg,        MK_ALL, 0,
    "Caseexact",    &ProcCase,          MK_ALL, 0,
    "Verbose",      &ProcVerbose,       MK_ALL, 0,
    "Undefsok",     &ProcUndefsOK,      MK_ALL, 0,
    "NOUndefsok",   &ProcNoUndefsOK,    MK_ALL, 0,
    "NAMELen",      &ProcNameLen,       MK_ALL, 0,
    "SYMFile",      &ProcSymFile,       MK_ALL, CF_SEPARATE_SYM,
    "OSName",       &ProcOSName,        MK_ALL, 0,
    "ELiminate",    &ProcEliminate,     MK_ALL, 0,
    "FARcalls",     &ProcFarCalls,      MK_ALL, 0,
    "MAXErrors",    &ProcMaxErrors,     MK_ALL, 0,
    "NOCASEexact",  &ProcNoCaseExact,   MK_ALL, 0,
    "NOCAChe",      &ProcNoCache,       MK_ALL, 0,
    "NOEXTension",  &ProcNoExtension,   MK_ALL, 0,
    "NOFARcalls",   &ProcNoFarCalls,    MK_ALL, 0,
    "CAChe",        &ProcCache,         MK_ALL, 0,
    "MANGlednames", &ProcMangledNames,  MK_ALL, 0,
    "OBJAlign",     &ProcObjAlign,      MK_ALL, 0,
    "RESource",     &ProcOpResource,    MK_ALL, 0,
    "STATics",      &ProcStatics,       MK_ALL, 0,
    "START",        &ProcStart,         MK_ALL, 0,
    "ARTificial",   &ProcArtificial,    MK_ALL, 0,
    "SHOwdead",     &ProcShowDead,      MK_ALL, 0,
    "VFRemoval",    &ProcVFRemoval,     MK_ALL, 0,
    "REDefsok",     &ProcRedefsOK,      MK_ALL, 0,
    "NOREDefsok",   &ProcNoRedefs,      MK_ALL, 0,
    "CVPack",       &ProcCVPack,        MK_ALL, 0,
    "INCremental",  &ProcIncremental,   MK_ALL, 0,
    "SMall",        &ProcSmall,         MK_OVERLAYS, 0,
    "DIStribute",   &ProcDistribute,    MK_OVERLAYS, 0,
    "DYNamic",      &ProcDynamic,       MK_OVERLAYS, 0,
    "STANdard",     &ProcStandard,      MK_OVERLAYS, 0,
    "NOIndirect",   &ProcNoIndirect,    MK_OVERLAYS, 0,
    "ARea",         &ProcArea,          MK_OVERLAYS, 0,
    "PADSections",  &ProcPadSections,   MK_OVERLAYS, 0,
    "FULLHeader",   &ProcFullHeader,    MK_DOS, 0,
    "PACKCode",     &ProcPackcode,      (MK_OS2_16BIT | MK_DOS | MK_QNX | MK_DOS16M), 0,
    "PACKData",     &ProcPackdata,      (MK_OS2_16BIT | MK_DOS | MK_QNX | MK_DOS16M), 0,
    "Alignment",    &ProcAlignment,     MK_OS2_16BIT | MK_OS2_LX | MK_PE | MK_ELF, 0,
    "STUB",         &ProcStub,          MK_OS2 | MK_PE | MK_WIN_VXD | MK_PHAR_LAP | MK_DOS16M, 0,
    "NOSTUB",       &ProcNoStub,        MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    "ONEautodata",  &ProcSingle,        MK_OS2, CF_AUTO_SEG_FLAG,
    "MANYautodata", &ProcMultiple,      MK_OS2, CF_AUTO_SEG_FLAG,
    "NOAutodata",   &ProcNone,          MK_OS2_16BIT, CF_AUTO_SEG_FLAG,
    "OLDlibrary",   &ProcOldLibrary,    MK_OS2 | MK_PE, 0,
    "MODName",      &ProcModName,       MK_OS2 | MK_PE | MK_WIN_VXD , 0,
    "NEWFiles",     &ProcNewFiles,      MK_ONLY_OS2_16, 0,
    "PROTmode",     &ProcProtMode,      MK_OS2_16BIT, 0,
    "DEscription",  &ProcDescription,   MK_OS2 | MK_PE | MK_WIN_VXD , 0,
    "NOSTDCall",    &ProcNoStdCall,     MK_PE, 0,
    "RWReloccheck", &ProcRWRelocCheck,  MK_WINDOWS, 0,
    "SELFrelative", &ProcSelfRelative,  MK_OS2_LX, 0,
    "INTernalrelocs",&ProcInternalRelocs,MK_OS2_LX, 0,
    "TOGglerelocsflag",&ProcToggleRelocsFlag,MK_OS2_LX, 0,
    "Heapsize",     &ProcHeapSize,      (MK_OS2 | MK_QNX | MK_PE), 0,
//  "PACKExp",      &ProcPackExp,       MK_PHAR_FLAT, 0,
    "MINData",      &ProcMinData,       MK_PHAR_LAP, 0,
    "MAXData",      &ProcMaxData,       MK_PHAR_LAP, 0,
    "OFFset",       &ProcOffset,        MK_PHAR_FLAT|MK_OS2_FLAT|MK_PE|MK_QNX_FLAT|MK_ELF|MK_RAW, 0,
    "SCReenname",   &ProcScreenName,    MK_NOVELL, 0,
    "CHeck",        &ProcCheck,         MK_NOVELL, 0,
    "MULTILoad",    &ProcMultiLoad,     MK_NOVELL, 0,
    "AUTOUNload",   &ProcAutoUnload,    MK_NOVELL, 0,
    "REentrant",    &ProcReentrant,     MK_NOVELL, 0,
    "SYnchronize",  &ProcSynch,         MK_NOVELL, 0,
    "CUSTom",       &ProcCustom,        MK_NOVELL, 0,
    "EXit",         &ProcExit,          MK_NOVELL, 0,
    "THReadname",   &ProcThreadName,    MK_NOVELL, 0,
    "PSeudopreemption", &ProcPseudoPreemption,      MK_NOVELL, 0,
    "COPYRight",    &ProcCopyright,     MK_NOVELL, 0,
    "MESsages",     &ProcMessages,      MK_NOVELL, 0,
    "HElp",         &ProcHelp,          MK_NOVELL, 0,
    "XDCdata",      &ProcXDCData,       MK_NOVELL, 0,
    "SHArelib",     &ProcSharelib,      MK_NOVELL, 0,
    "OSDomain",     &ProcOSDomain,      MK_NOVELL, 0,
    "NLMFlags",     &ProcNLMFlags,      MK_NOVELL, 0,
    "VERSion",      &ProcVersion,       MK_NOVELL|MK_OS2_FLAT|MK_PE|MK_WINDOWS, 0,
    "IMPLib",       &ProcImplib,        MK_NOVELL|MK_OS2|MK_PE, 0,
    "IMPFile",      &ProcImpFile,       MK_NOVELL|MK_OS2|MK_PE, 0,
#ifdef _DOS16M
    "BUFfer",       &ProcBuffer,        MK_DOS16M, 0,
    "GDTsize",      &ProcGDTSize,       MK_DOS16M, 0,
    "RELocs",       &ProcRelocs,        MK_DOS16M, 0,
    "SELstart",     &ProcSelStart,      MK_DOS16M, 0,
    "DATASize",     &ProcDataSize,      MK_DOS16M, 0,
    "EXTended",     &ProcExtended,      MK_DOS16M, 0,
#endif
#ifdef _RDOS
    "CODESelector", &ProcRdosCodeSel,   MK_RDOS, 0,
    "DATASelector", &ProcRdosDataSel,   MK_RDOS, 0,
#endif
    "NORelocs",     &ProcNoRelocs,      (MK_QNX | MK_DOS16M  | MK_PE | MK_ELF), 0,
    "LOnglived",    &ProcLongLived,     MK_QNX, 0,
    "PRIVilege",    &ProcQNXPrivilege,  MK_QNX, 0,
    "LInearrelocs", &ProcLinearRelocs,  MK_QNX, 0,
    "EXTRASections",&ProcExtraSections, MK_ELF, 0,
    "EXPORTAll",    &ProcExportAll,     MK_ELF, 0,
    "LINKVersion",  &ProcLinkVersion,   MK_PE,  0,
    "OSVersion",    &ProcOsVersion,     MK_PE,  0,
    "CHECKSUM",     &ProcChecksum,      MK_PE,  0,
    "LARGEaddressaware",&ProcLargeAddressAware, MK_PE, 0,
    "NOLARGEaddressaware",&ProcNoLargeAddressAware, MK_PE, 0,
    "HSHIFT",       &ProcHshift,        (MK_DOS | MK_ALLOW_16),  0,
    "FILLchar",     &ProcFillchar,      MK_ALL,  0,
    "MIXed1632",    &ProcMixed1632,     MK_OS2_FLAT, 0,
    NULL
};

/* these directives are the only ones that are harmless to run after the files
 * have been processed in pass 1 */

parse_entry     SysDirectives[] = {
    "Library",      &ProcLibrary,       MK_ALL, 0,
    "Name",         &ProcName,          MK_ALL, 0,
    "OPtion",       &ProcOptions,       MK_ALL, 0,
    "LIBPath",      &ProcLibPath,       MK_ALL, 0,
    "FORMat",       &ProcFormat,        MK_ALL, 0,
    "DISAble",      &ProcDisable,       MK_ALL, 0,
    "SOrt",         &ProcSort,          MK_ALL, 0,
    "ORDer",        &ProcOrder,         MK_ALL, 0,
    "OUTput",       &ProcOutput,        MK_ALL, 0,
#if defined( _PHARLAP ) || defined( _DOS16M ) || defined( _OS2 ) || defined( _ELF )
    "RUntime",      &ProcRuntime,       (MK_PHAR_LAP | MK_DOS16M | MK_PE | MK_ELF), 0,
#endif
#if defined( _OS2 ) || defined( _QNXLOAD )
    "SEGment",      &ProcSegment,       (MK_QNX | MK_OS2 | MK_PE | MK_WIN_VXD ), 0,
#endif
#ifdef _DOS16M
    "MEMory",       &ProcMemory16M,     MK_DOS16M, 0,
    "TRansparent",  &ProcTransparent,   MK_DOS16M, 0,
#endif
    NULL
};

parse_entry    Models[] = {
    "Dos",          &ProcDos,           MK_DOS, 0,
#ifdef _OS2
    "OS2",          &ProcOS2,           MK_ONLY_OS2, 0,
    "WINdows",      &ProcWindows,       MK_WINDOWS|MK_PE|MK_WIN_VXD, 0,
#endif
#ifdef _PHARLAP
    "PHARlap",      &ProcPharLap,       MK_PHAR_LAP, 0,
#endif
#ifdef _NOVELL
    "NOVell",       &ProcNovell,        MK_NOVELL, 0,
#endif
#ifdef _QNXLOAD
    "QNX",          &ProcQNX,           MK_QNX, 0,
#endif
#ifdef _DOS16M
    "DOS16M",       &Proc16M,           MK_DOS16M, 0,
#endif
#ifdef _ELF
    "ELF",          &ProcELF,           MK_ELF, 0,
#endif
#ifdef _ZDOS
    "ZDos",         &ProcZdos,          MK_ZDOS, 0,
#endif
#ifdef _RDOS
    "RDos",         &ProcRdos,          MK_RDOS, 0,
#endif
#ifdef _RAW
    "Raw",          &ProcRaw,           MK_RAW, 0,
#endif
    NULL
};

parse_entry    Languages[] = {
    "JApanese",     &ProcJapanese,      MK_ALL, 0,
    "CHinese",      &ProcChinese,       MK_ALL, 0,
    "KOrean",       &ProcKorean,        MK_ALL, 0,
    NULL
};

parse_entry      EndLinkOpt[] = {
    "ENDLink",      &ProcEndLink,       MK_ALL, 0,
};

parse_entry  RunOptions[] = {
#ifdef _DOS16M
    "KEYboard",     &ProcKeyboard,      MK_DOS16M, 0,
    "OVERload",     &ProcOverload,      MK_DOS16M, 0,
    "INIT00",       &ProcInit00,        MK_DOS16M, 0,
    "INITFF",       &ProcInitFF,        MK_DOS16M, 0,
    "ROTate",       &ProcRotate,        MK_DOS16M, 0,
    "AUTO",         &ProcAuto,          MK_DOS16M, 0,
    "SELectors",    &ProcSelectors,     MK_DOS16M, 0,
    "INT10",        &ProcInt10,         MK_DOS16M, 0,
#endif
#ifdef _PHARLAP
    "MINReal",      &ProcMinReal,       MK_PHAR_FLAT, 0,
    "MAXReal",      &ProcMaxReal,       MK_PHAR_FLAT, 0,
    "REALBreak",    &ProcRealBreak,     MK_PHAR_FLAT, CF_HAVE_REALBREAK,
    "CALLBufs",     &ProcCallBufs,      MK_PHAR_FLAT, 0,
    "MINIBuf",      &ProcMiniBuf,       MK_PHAR_FLAT, 0,
    "MAXIBuf",      &ProcMaxiBuf,       MK_PHAR_FLAT, 0,
    "NISTack",      &ProcNIStack,       MK_PHAR_FLAT, 0,
    "ISTKsize",     &ProcIStkSize,      MK_PHAR_FLAT, 0,
    "UNPRIVileged", &ProcUnpriv,        MK_PHAR_FLAT, 0,
    "PRIVileged",   &ProcPriv,          MK_PHAR_FLAT, 0,
/* WARNING: do not document the following directive -- for internal use only */
    "FLAGs",        &ProcFlags,         MK_PHAR_FLAT, 0,
#endif
#ifdef _OS2
    "NATive",       &ProcRunNative,     MK_PE, 0,
    "WINdows",      &ProcRunWindows,    MK_PE, 0,
    "CONsole",      &ProcRunConsole,    MK_PE, 0,
    "POSix",        &ProcRunPosix,      MK_PE, 0,
    "OS2",          &ProcRunOS2,        MK_PE, 0,
    "DOSstyle",     &ProcRunDosstyle,   MK_PE, 0,
#endif
#ifdef _ELF
    "ABIver",       &ProcELFRNumber,    MK_ELF, 0,
    "SVR4",         &ProcELFRSVR4,      MK_ELF, 0,
    "NETbsd",       &ProcELFRNetBSD,    MK_ELF, 0,
    "LINux",        &ProcELFRLinux,     MK_ELF, 0,
    "FREebsd",      &ProcELFRFBSD,      MK_ELF, 0,
    "SOLaris",      &ProcELFRSolrs,     MK_ELF, 0,
#endif
    NULL
};

#ifdef _DOS16M
/* parse tables used in cmd16m.c */

extern bool     ProcTryExtended( void );
extern bool     ProcTryLow( void );
extern bool     ProcForceExtended( void );
extern bool     ProcForceLow( void );

parse_entry  Strategies[] = {
    "TRYExtended",  &ProcTryExtended,   MK_DOS16M, 0,
    "TRYLow",       &ProcTryLow,        MK_DOS16M, 0,
    "FORCEExtended",&ProcForceExtended, MK_DOS16M, 0,
    "FORCELow",     &ProcForceLow,      MK_DOS16M, 0,
    NULL
};

extern bool     ProcTStack( void );
extern bool     ProcTData( void );

parse_entry  TransTypes[] = {
    "STack",        &ProcTStack,        MK_DOS16M, 0,
    "DAta",         &ProcTData,         MK_DOS16M, 0,
    NULL
};
#endif

#ifdef _QNXLOAD
/* parse tables used in CMDQNX.C */

parse_entry QNXSegModel[] = {
    "EXECUTEOnly",  &ProcQNXExecuteonly,MK_QNX, 0,
    "EXECUTERead",  &ProcQNXExecuteread,MK_QNX, 0,
    "READOnly",     &ProcQNXReadOnly,   MK_QNX, 0,
    "READWrite",    &ProcQNXReadWrite,  MK_QNX, 0,
    NULL
};

parse_entry QNXSegDesc[] = {
    "Class",        &ProcQNXClass,      MK_QNX, 0,
    NULL
};

parse_entry QNXFormats[] = {
    "FLat",         &ProcQNXFlat,       MK_QNX_FLAT, 0,
    NULL
};
#endif

#ifdef _PHARLAP
/* parse tables used in CMDPHAR.C */

parse_entry  PharModels[] = {
    "EXTended",     &ProcPharFlat,      MK_PHAR_FLAT, 0,
    "REX",          &ProcRex,           MK_PHAR_REX, 0,
    "SEGmented",    &ProcPharSegmented, MK_PHAR_MULTISEG, 0,
    NULL
};
#endif

#ifdef _NOVELL
/* parse tables used in CMDNOV.C */

parse_entry     NovModels[] = {
    "NLM",          &ProcNLM,           MK_NOVELL, 0,    /* 0 */
    "LAN",          &ProcLAN,           MK_NOVELL, 0,    /* 1 */
    "DSK",          &ProcDSK,           MK_NOVELL, 0,    /* 2 */
    "NAM",          &ProcNAM,           MK_NOVELL, 0,    /* 3 */
    "0",            &ProcNLM,           MK_NOVELL, 0,    /* 0 again */
    "1",            &ProcLAN,           MK_NOVELL, 0,    /* etc */
    "2",            &ProcDSK,           MK_NOVELL, 0,
    "3",            &ProcNAM,           MK_NOVELL, 0,
    "4",            &ProcModuleType4,   MK_NOVELL, 0,
    "5",            &ProcModuleType5,   MK_NOVELL, 0,
    "6",            &ProcModuleType6,   MK_NOVELL, 0,
    "7",            &ProcModuleType7,   MK_NOVELL, 0,
    "8",            &ProcModuleType8,   MK_NOVELL, 0,
    "9",            &ProcModuleType9,   MK_NOVELL, 0,
#if 0
    /* NLM types 10 through 12 are currently reserved */
    "10",           &ProcModuleType10,  MK_NOVELL, 0,
    "11",           &ProcModuleType11,  MK_NOVELL, 0,
    "12",           &ProcModuleType12,  MK_NOVELL, 0,
#endif
    NULL
};

parse_entry      NovDBIOptions[] = {
    "ONLyexports",  &ProcNovDBIExports, MK_NOVELL, 0,
    "REFerenced",   &ProcNovDBIReferenced,MK_NOVELL, 0,
    NULL
};
#endif

/* parse tables used in CMDDOS.C */

parse_entry     Sections[] = {
    "Section",      &ProcSection,       MK_OVERLAYS, 0,
    "AUTOSection",  &ProcAutoSection,   MK_OVERLAYS, 0,
    "End",          &ProcEnd,           MK_OVERLAYS, 0,
    NULL
};

parse_entry     SectOptions[] = {
    "INto",         &ProcInto,          MK_OVERLAYS, 0,
    NULL
};

parse_entry     DosOptions[] = {
    "COM",          &ProcCom,           MK_COM, 0,
    NULL
};

#ifdef _OS2
/* parse tables used in CMDOS2.C */

parse_entry  SubFormats[] = {
    "DLl",          &ProcOS2DLL,        MK_OS2 | MK_PE, 0,
    "FLat",         &ProcLX,            MK_OS2_LX, 0,
    "LE",           &ProcLE,            MK_OS2_LE, 0,
    "LX",           &ProcLX,            MK_OS2_LX, 0,
    "NT",           &ProcPE,            MK_PE, 0,
    "PE",           &ProcPE,            MK_PE, 0,
    "VXD",          &ProcVXD,           MK_WIN_VXD, 0,
    NULL
};

parse_entry OS2FormatKeywords[] = {
    "PM",           &ProcPM,            MK_ONLY_OS2, 0,
    "PMCompatible", &ProcPMCompatible,  MK_ONLY_OS2, 0,
    "FULLscreen",   &ProcPMFullscreen,  MK_ONLY_OS2, 0,
    "PHYSdevice",   &ProcPhysDevice,    MK_OS2_LE | MK_OS2_LX, 0,
    "VIRTdevice",   &ProcVirtDevice,    MK_OS2_LE | MK_OS2_LX, 0,
    NULL
};

parse_entry WindowsFormatKeywords[] = {
    "MEMory",       &ProcMemory,        MK_WINDOWS, 0,
    "FOnt",         &ProcFont,          MK_WINDOWS, 0,
    NULL
};

parse_entry NTFormatKeywords[] = {
    "TNT",          &ProcTNT,           MK_PE, 0,
    "RDOS",         &ProcRDOS,          MK_PE, 0,
    "EFI",          &ProcEFI,           MK_PE, 0,
    NULL
};

parse_entry VXDFormatKeywords[] = {
    "DYNamic",      &ProcDynamicDriver, MK_WIN_VXD, 0,
    "STATic",       &ProcStaticDriver,  MK_WIN_VXD, 0,
    NULL
};

parse_entry Init_Keywords[] = {
    "INITGlobal",   &ProcInitGlobal,    MK_OS2|MK_PE, 0,
    "INITInstance", &ProcInitInstance,  MK_OS2|MK_PE, 0,
    "INITThread",   &ProcInitThread,    MK_PE, 0,
    NULL
};

parse_entry Term_Keywords[] = {
    "TERMGlobal",   &ProcTermGlobal,    MK_OS2_LE | MK_OS2_LX | MK_PE, 0,
    "TERMInstance", &ProcTermInstance,  MK_OS2_LE | MK_OS2_LX | MK_PE, 0,
    "TERMThread",   &ProcTermThread,    MK_PE, 0,
    NULL
};

parse_entry Exp_Keywords[] = {
    "RESident",     &ProcExpResident,   MK_OS2, 0,
    "PRIVATE",      &ProcPrivate,       MK_OS2|MK_PE, 0,
    NULL
};

parse_entry SegDesc[] = {
    "Class",        &ProcOS2Class,      MK_OS2|MK_PE|MK_WIN_VXD, 0,
    "TYpe",         &ProcSegType,       MK_OS2|MK_PE|MK_WIN_VXD, 0,
    NULL
};

parse_entry SegTypeDesc[] = {
    "CODE",         &ProcSegCode,       MK_OS2|MK_PE|MK_WIN_VXD, 0,
    "DATA",         &ProcSegData,       MK_OS2|MK_PE|MK_WIN_VXD, 0,
    NULL
};

parse_entry SegModel[] = {
    "PReload",      &ProcPreload,       MK_OS2|MK_WIN_VXD, 0,
    "LOadoncall",   &ProcLoadoncall,    MK_OS2|MK_WIN_VXD, 0,
    "Iopl",         &ProcIopl,          MK_ONLY_OS2|MK_WIN_VXD, 0,
    "NOIopl",       &ProcNoIopl,        MK_ONLY_OS2|MK_WIN_VXD, 0,
    "EXECUTEOnly",  &ProcExecuteonly,   MK_OS2, 0,
    "EXECUTERead",  &ProcExecuteread,   MK_OS2, 0,
    "SHared",       &ProcShared,        MK_OS2|MK_PE|MK_WIN_VXD, 0,
    "NONShared",    &ProcNonShared,     MK_OS2|MK_PE|MK_WIN_VXD, 0,
    "READOnly",     &ProcReadOnly,      MK_OS2, 0,
    "READWrite",    &ProcReadWrite,     MK_OS2, 0,
    "CONforming",   &ProcConforming,    MK_ONLY_OS2|MK_WIN_VXD, 0,
    "NONConforming",&ProcNonConforming, MK_ONLY_OS2|MK_WIN_VXD, 0,
    "MOVeable",     &ProcMovable,       MK_OS2_16BIT, 0,
    "FIXed",        &ProcFixed,         MK_WINDOWS, 0,
    "DIScardable",  &ProcDiscardable,   MK_WINDOWS|MK_WIN_VXD, 0,
    "NONDiscardable",&ProcNonDiscardable,MK_WIN_VXD, 0,
    "INValid",      &ProcInvalid,       MK_OS2_LE|MK_OS2_LX, 0,
    "RESident",     &ProcPermanent,     MK_OS2_LE|MK_OS2_LX|MK_WIN_VXD, 0,
    "CONTiguous",   &ProcContiguous,    MK_OS2_LE|MK_OS2_LX, 0,
    "DYNamic",      &ProcOS2Dynamic,    MK_OS2_LE|MK_OS2_LX, 0,
    "NONPERManent", &ProcNonPermanent,  MK_OS2_LE|MK_OS2_LX, 0,
    "PERManent",    &ProcPermanent,     MK_OS2_LE|MK_OS2_LX, 0,
    "PAGEable",     &ProcPageable,      MK_PE, 0,
    "NONPageable",  &ProcNonPageable,   MK_PE, 0,
    NULL
};

parse_entry CommitKeywords[] = {
    "STack",        &ProcCommitStack,   MK_PE, 0,
    "Heap",         &ProcCommitHeap,    MK_PE, 0,
    NULL
};
#endif

#ifdef _ELF
parse_entry ELFFormatKeywords[] = {
    "DLl",          &ProcELFDLL,        MK_ELF, 0,
    NULL
};
#endif

#ifdef _ZDOS
parse_entry ZdosOptions[] = {
    "SYS",          &ProcZdosSYS,           MK_ZDOS, 0,
    "HWD",          &ProcZdosHWD,           MK_ZDOS, 0,
    "FSD",          &ProcZdosFSD,           MK_ZDOS, 0,
    NULL };
#endif

#ifdef _RDOS
parse_entry RdosOptions[] = {
    "DEV16",          &ProcRdosDev16,       MK_RDOS, 0,
    "DEV32",          &ProcRdosDev32,       MK_RDOS, 0,
    "BIN16",          &ProcRdosBin16,       MK_RDOS, 0,
    "BIN32",          &ProcRdosBin32,       MK_RDOS, 0,
    "MBOOT",          &ProcRdosMboot,       MK_RDOS, 0,
    NULL };
#endif

#ifdef _RAW
parse_entry RawOptions[] = {
    "BIN",          &ProcRawBIN,            MK_RAW, 0,
    "HEX",          &ProcRawHEX,            MK_RAW, 0,
    NULL };
#endif

parse_entry OrderOpts[] = {
    "CLName",       &ProcOrdClass,      MK_ALL, 0,
    NULL
};

parse_entry OrderClassOpts[] = {
    "SEGAddr",      &ProcOrdSegAdr,     MK_ALL, 0,
    "OFFset",       &ProcOrdOfsAdr,     MK_ALL, 0,
    "COpy",         &ProcOrdCopy,       MK_ALL, 0,
    "NOEmit",       &ProcOrdNoEmit,     MK_ALL, 0,
    "SEGMent",      &ProcOrdSeg,        MK_ALL, 0,
    NULL
};

parse_entry OrderSegOpts[] = {
    "SEGAddr",      &ProcOrdSegSegAdr,  MK_ALL, 0,
    "OFFset",       &ProcOrdSegOfsAdr,  MK_ALL, 0,
    "NOEmit",       &ProcOrdSegNoEmit,  MK_ALL, 0,
    NULL
};

parse_entry OutputOpts[] = {
    "RAW",          &ProcOutputRaw,     MK_ALL, 0,
    "HEX",          &ProcOutputHex,     MK_ALL, 0,
    "OFFset",       &ProcOutputOfs,     MK_ALL, 0,
    "HSHIFT",       &ProcOutputHshift,  MK_ALL, 0,
    "STartrec",     &ProcOutputStart,   MK_ALL, 0,
    NULL
};
