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


#include "linkstd.h"
#include "cmdutils.h"
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
#include "cmdxxx.h"
#include "cmdtable.h"


#ifdef _DOS16M
/* parse tables used in cmd16m.c */

parse_entry     Strategies[] = {
    "TRYExtended",  ProcTryExtended,    MK_DOS16M, 0,
    "TRYLow",       ProcTryLow,         MK_DOS16M, 0,
    "FORCEExtended",ProcForceExtended,  MK_DOS16M, 0,
    "FORCELow",     ProcForceLow,       MK_DOS16M, 0,
    NULL
};

parse_entry     TransTypes[] = {
    "STack",        ProcTStack,         MK_DOS16M, 0,
    "DAta",         ProcTData,          MK_DOS16M, 0,
    NULL
};
#endif

#ifdef _QNX
/* parse tables used in CMDQNX.C */

parse_entry     QNXSegModel[] = {
    "EXECUTEOnly",  ProcQNXExecuteonly, MK_QNX, 0,
    "EXECUTERead",  ProcQNXExecuteread, MK_QNX, 0,
    "READOnly",     ProcQNXReadOnly,    MK_QNX, 0,
    "READWrite",    ProcQNXReadWrite,   MK_QNX, 0,
    NULL
};

parse_entry     QNXSegDesc[] = {
    "Class",        ProcQNXClass,       MK_QNX, 0,
    NULL
};

parse_entry     QNXFormats[] = {
    "FLat",         ProcQNXFlat,        MK_QNX_FLAT, 0,
    NULL
};
#endif

#ifdef _PHARLAP
/* parse tables used in CMDPHAR.C */

parse_entry     PharModels[] = {
    "EXTended",     ProcPharFlat,       MK_PHAR_FLAT,     0,
    "REX",          ProcRex,            MK_PHAR_REX,      0,
    "SEGmented",    ProcPharSegmented,  MK_PHAR_MULTISEG, 0,
    NULL
};
#endif

#ifdef _NOVELL
/* parse tables used in CMDNOV.C */

parse_entry     NovModels[] = {
    "NLM",          ProcNLM,            MK_NOVELL, 0,    /* 0 */
    "LAN",          ProcLAN,            MK_NOVELL, 0,    /* 1 */
    "DSK",          ProcDSK,            MK_NOVELL, 0,    /* 2 */
    "NAM",          ProcNAM,            MK_NOVELL, 0,    /* 3 */
    "0",            ProcNLM,            MK_NOVELL, 0,    /* 0 again */
    "1",            ProcLAN,            MK_NOVELL, 0,    /* etc */
    "2",            ProcDSK,            MK_NOVELL, 0,
    "3",            ProcNAM,            MK_NOVELL, 0,
    "4",            ProcModuleType4,    MK_NOVELL, 0,
    "5",            ProcModuleType5,    MK_NOVELL, 0,
    "6",            ProcModuleType6,    MK_NOVELL, 0,
    "7",            ProcModuleType7,    MK_NOVELL, 0,
    "8",            ProcModuleType8,    MK_NOVELL, 0,
    "9",            ProcModuleType9,    MK_NOVELL, 0,
#if 0
    /* NLM types 10 through 12 are currently reserved */
    "10",           ProcModuleType10,   MK_NOVELL, 0,
    "11",           ProcModuleType11,   MK_NOVELL, 0,
    "12",           ProcModuleType12,   MK_NOVELL, 0,
#endif
    NULL
};

parse_entry     NovDBIOptions[] = {
    "ONLyexports",  ProcNovDBIExports,  MK_NOVELL, 0,
    "REFerenced",   ProcNovDBIReferenced,MK_NOVELL, 0,
    NULL
};
#endif

#ifdef _EXE
/* parse tables used in CMDDOS.C */

parse_entry     Sections[] = {
    "Section",      ProcSection,        MK_OVERLAYS, 0,
    "AUTOSection",  ProcAutoSection,    MK_OVERLAYS, 0,
    "End",          ProcEnd,            MK_OVERLAYS, 0,
    NULL
};

parse_entry     SectOptions[] = {
    "INto",         ProcInto,           MK_OVERLAYS, 0,
    NULL
};

parse_entry     DosOptions[] = {
    "COM",          ProcCom,            MK_COM, 0,
    NULL
};
#endif

#ifdef _OS2
/* parse tables used in CMDOS2.C */

parse_entry     SubFormats[] = {
    "DLl",          ProcOS2DLL,         MK_OS2 | MK_PE, 0,
    "FLat",         ProcLX,             MK_OS2_LX, 0,
    "LE",           ProcLE,             MK_OS2_LE, 0,
    "LX",           ProcLX,             MK_OS2_LX, 0,
    "NT",           ProcPE,             MK_PE, 0,
    "PE",           ProcPE,             MK_PE, 0,
    "VXD",          ProcVXD,            MK_WIN_VXD, 0,
    NULL
};

parse_entry     OS2FormatKeywords[] = {
    "PM",           ProcPM,             MK_ONLY_OS2, 0,
    "PMCompatible", ProcPMCompatible,   MK_ONLY_OS2, 0,
    "FULLscreen",   ProcPMFullscreen,   MK_ONLY_OS2, 0,
    "PHYSdevice",   ProcPhysDevice,     MK_OS2_LE | MK_OS2_LX, 0,
    "VIRTdevice",   ProcVirtDevice,     MK_OS2_LE | MK_OS2_LX, 0,
    NULL
};

parse_entry     WindowsFormatKeywords[] = {
    "MEMory",       ProcMemory,         MK_WINDOWS, 0,
    "FOnt",         ProcFont,           MK_WINDOWS, 0,
    NULL
};

parse_entry     NTFormatKeywords[] = {
    "TNT",          ProcTNT,            MK_PE, 0,
    "RDOS",         ProcRDOS,           MK_PE, 0,
    "EFI",          ProcEFI,            MK_PE, 0,
    NULL
};

parse_entry     VXDFormatKeywords[] = {
    "DYNamic",      ProcDynamicDriver,  MK_WIN_VXD, 0,
    "STATic",       ProcStaticDriver,   MK_WIN_VXD, 0,
    NULL
};

parse_entry     Init_Keywords[] = {
    "INITGlobal",   ProcInitGlobal,     MK_OS2 | MK_PE, 0,
    "INITInstance", ProcInitInstance,   MK_OS2 | MK_PE, 0,
    "INITThread",   ProcInitThread,     MK_PE, 0,
    NULL
};

parse_entry     Term_Keywords[] = {
    "TERMGlobal",   ProcTermGlobal,     MK_OS2_LE | MK_OS2_LX | MK_PE, 0,
    "TERMInstance", ProcTermInstance,   MK_OS2_LE | MK_OS2_LX | MK_PE, 0,
    "TERMThread",   ProcTermThread,     MK_PE, 0,
    NULL
};

parse_entry     Exp_Keywords[] = {
    "RESident",     ProcExpResident,    MK_OS2, 0,
    "PRIVATE",      ProcPrivate,        MK_OS2 | MK_PE, 0,
    NULL
};

parse_entry     SegDesc[] = {
    "Class",        ProcOS2Class,       MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    "TYpe",         ProcSegType,        MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    NULL
};

parse_entry     SegTypeDesc[] = {
    "CODE",         ProcSegCode,        MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    "DATA",         ProcSegData,        MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    NULL
};

parse_entry     SegModel[] = {
    "PReload",      ProcPreload,        MK_OS2 | MK_WIN_VXD, 0,
    "LOadoncall",   ProcLoadoncall,     MK_OS2 | MK_WIN_VXD, 0,
    "Iopl",         ProcIopl,           MK_ONLY_OS2 | MK_WIN_VXD, 0,
    "NOIopl",       ProcNoIopl,         MK_ONLY_OS2 | MK_WIN_VXD, 0,
    "EXECUTEOnly",  ProcExecuteonly,    MK_OS2, 0,
    "EXECUTERead",  ProcExecuteread,    MK_OS2, 0,
    "SHared",       ProcShared,         MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    "NONShared",    ProcNonShared,      MK_OS2 | MK_PE | MK_WIN_VXD, 0,
    "READOnly",     ProcReadOnly,       MK_OS2, 0,
    "READWrite",    ProcReadWrite,      MK_OS2, 0,
    "CONforming",   ProcConforming,     MK_ONLY_OS2 | MK_WIN_VXD, 0,
    "NONConforming",ProcNonConforming,  MK_ONLY_OS2 | MK_WIN_VXD, 0,
    "MOVeable",     ProcMovable,        MK_OS2_16BIT, 0,
    "FIXed",        ProcFixed,          MK_WINDOWS, 0,
    "DIScardable",  ProcDiscardable,    MK_WINDOWS | MK_WIN_VXD, 0,
    "NONDiscardable",ProcNonDiscardable,MK_WIN_VXD, 0,
    "INValid",      ProcInvalid,        MK_OS2_LE | MK_OS2_LX, 0,
    "RESident",     ProcPermanent,      MK_OS2_LE | MK_OS2_LX | MK_WIN_VXD, 0,
    "CONTiguous",   ProcContiguous,     MK_OS2_LE | MK_OS2_LX, 0,
    "DYNamic",      ProcOS2Dynamic,     MK_OS2_LE | MK_OS2_LX, 0,
    "NONPERManent", ProcNonPermanent,   MK_OS2_LE | MK_OS2_LX, 0,
    "PERManent",    ProcPermanent,      MK_OS2_LE | MK_OS2_LX, 0,
    "PAGEable",     ProcPageable,       MK_PE, 0,
    "NONPageable",  ProcNonPageable,    MK_PE, 0,
    NULL
};

parse_entry     CommitKeywords[] = {
    "STack",        ProcCommitStack,    MK_PE, 0,
    "Heap",         ProcCommitHeap,     MK_PE, 0,
    NULL
};
#endif

#ifdef _ELF
parse_entry     ELFFormatKeywords[] = {
    "DLl",          ProcELFDLL,         MK_ELF, 0,
    NULL
};
#endif

#ifdef _ZDOS
parse_entry     ZdosOptions[] = {
    "SYS",          ProcZdosSYS,        MK_ZDOS, 0,
    "HWD",          ProcZdosHWD,        MK_ZDOS, 0,
    "FSD",          ProcZdosFSD,        MK_ZDOS, 0,
    NULL
};
#endif

#ifdef _RDOS
parse_entry     RdosOptions[] = {
    "DEV16",        ProcRdosDev16,      MK_RDOS, 0,
    "DEV32",        ProcRdosDev32,      MK_RDOS, 0,
    "BIN16",        ProcRdosBin16,      MK_RDOS, 0,
    "BIN32",        ProcRdosBin32,      MK_RDOS, 0,
    "MBOOT",        ProcRdosMboot,      MK_RDOS, 0,
    NULL
};
#endif

#ifdef _RAW
parse_entry     RawOptions[] = {
    "BIN",          ProcRawBIN,         MK_RAW, 0,
    "HEX",          ProcRawHEX,         MK_RAW, 0,
    NULL
};
#endif
