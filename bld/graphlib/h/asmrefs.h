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


/* To make the 386 graphics library, the two libraries, graph3r and
   graph3s, are combined. Only one copy of the assembler modules is
   placed in the combined library. For this to work properly, all
   references to symbol names must be done the same way. */

/* Assembler routines called by C. */

#if defined( _NEC_PC )
#pragma aux _NECSet "*_";
#pragma aux _NECReset "*_";
#pragma aux _NECByteRep "*_";
#pragma aux _NECByteXor "*_";
#pragma aux _NECByteAnd "*_";
#pragma aux _NECByteOr "*_";
#pragma aux _NECRep "*_";
#pragma aux _NECXor "*_";
#pragma aux _NECAnd "*_";
#pragma aux _NECOr "*_";
#pragma aux _NECGetDot "*_";
#pragma aux _NECPixZap "*_";
#pragma aux _NECPixFill "*_";
#pragma aux _NECPixCopy "*_";
#pragma aux _NECPixRead "*_";
#pragma aux _NEC16Xor "*_";
#pragma aux _NEC16And "*_";
#pragma aux _NEC16Or "*_";
#pragma aux _NEC16GetDot "*_";
#pragma aux _NEC16PixZap "*_";
#pragma aux _NEC16PixFill "*_";
#pragma aux _NEC16PixCopy "*_";
#pragma aux _NEC16PixRead "*_";
#pragma aux _NEC16ScanLeft "*_";
#pragma aux _NEC16ScanRight "*_";
#pragma aux _NECMoveUp "*_";
#pragma aux _NECMoveDown "*_";
#pragma aux _NECMoveLeft "*_";
#pragma aux _NECMoveRight "*_";
#pragma aux _NECScanLeft "*_";
#pragma aux _NECScanRight "*_";
#pragma aux _NEC1120MoveUp "*_";
#pragma aux _NEC1120MoveDown "*_";
#pragma aux _NEC1120MoveLeft "*_";
#pragma aux _NEC1120MoveRight "*_";
#pragma aux _NEC1120Xor "*_";
#pragma aux _NEC1120Or "*_";
#pragma aux _NEC1120And "*_";
#pragma aux _NEC1120Zap "*_";
#pragma aux _NEC1120Fill "*_";
#pragma aux _NEC1120GetDot "*_";
#pragma aux _NEC1120Copy "*_";
#pragma aux _NEC1120Read "*_";
#else
#pragma aux _CoRep "*_";
#pragma aux _CoXor "*_";
#pragma aux _CoAnd "*_";
#pragma aux _CoOr "*_";
#pragma aux _MoveUp "*_";
#pragma aux _MoveDown "*_";
#pragma aux _Move1Left "*_";
#pragma aux _Move2Left "*_";
#pragma aux _Move1Right "*_";
#pragma aux _Move2Right "*_";
#pragma aux _Get1Dot "*_";
#pragma aux _Get2Dot "*_";
#pragma aux _Pix1Zap "*_";
#pragma aux _Pix2Zap "*_";
#pragma aux _Pix1Fill "*_";
#pragma aux _Pix2Fill "*_";
#pragma aux _Pix1Copy "*_";
#pragma aux _Pix2Copy "*_";
#pragma aux _Pix1Read "*_";
#pragma aux _Pix2Read "*_";
#pragma aux _CGAScanLeft "*_";
#pragma aux _CGAScan1Right "*_";
#pragma aux _CGAScan2Right "*_";
#pragma aux _EGAMoveUpHi "*_";
#pragma aux _EGAMoveUpLo "*_";
#pragma aux _EGAMoveDownHi "*_";
#pragma aux _EGAMoveDownLo "*_";
#pragma aux _EGAMoveLeft "*_";
#pragma aux _EGAMoveRight "*_";
#pragma aux _EGARep "*_";
#pragma aux _EGAGetDot "*_";
#pragma aux _EGAGetDotEO "*_";
#pragma aux _EGAGetDotMono "*_";
#pragma aux _EGAZap "*_";
#pragma aux _EGAZapEO "*_";
#pragma aux _EGAZapMono "*_";
#pragma aux _EGAFill "*_";
#pragma aux _EGAFillEO "*_";
#pragma aux _EGAFillMono "*_";
#pragma aux _EGAPixCopy "*_";
#pragma aux _EGAPixCopyEO "*_";
#pragma aux _EGAPixCopyMono "*_";
#pragma aux _EGAReadRow "*_";
#pragma aux _EGAReadRowEO "*_";
#pragma aux _EGAReadRowMono "*_";
#pragma aux _EGAScanLeft "*_";
#pragma aux _EGAScanLeftEO "*_";
#pragma aux _EGAScanLeftMono "*_";
#pragma aux _EGAScanRight "*_";
#pragma aux _EGAScanRightEO "*_";
#pragma aux _EGAScanRightMono "*_";
#pragma aux _HercMoveUp "*_";
#pragma aux _HercMoveDown "*_";
#pragma aux _MoveUp19 "*_";
#pragma aux _MoveDown19 "*_";
#pragma aux _MoveLeft19 "*_";
#pragma aux _MoveRight19 "*_";
#pragma aux _Rep19 "*_";
#pragma aux _And19 "*_";
#pragma aux _Zap19 "*_";
#pragma aux _GetDot19 "*_";
#pragma aux _Fill19 "*_";
#pragma aux _PixCopy19 "*_";
#pragma aux _PixRead19 "*_";
#pragma aux _ScanLeft19 "*_";
#pragma aux _ScanRight19 "*_";
#pragma aux _MoveUp100 "*_";
#pragma aux _MoveDown100 "*_";
#pragma aux _MoveLeft256 "*_";
#pragma aux _MoveRight256 "*_";
#pragma aux _MoveUp640 "*_";
#pragma aux _MoveDown640 "*_";
#pragma aux _MoveUp800 "*_";
#pragma aux _MoveDown800 "*_";
#pragma aux _MoveDown1024 "*_";
#pragma aux _MoveDown128 "*_";
#pragma aux _MoveUp1024 "*_";
#pragma aux _MoveUp128 "*_";
#pragma aux _Zap256 "*_";
#pragma aux _Fill256 "*_";
#pragma aux _PixCopy256 "*_";
#pragma aux _PixRead256 "*_";
#pragma aux _ScanLeft256 "*_";
#pragma aux _ScanRight256 "*_";
#if 0
// This mode is untested
#pragma aux _MoveUp1280 "*_";
#pragma aux _MoveDown1280 "*_";
#pragma aux _MoveRightWord "*_";
#pragma aux _MoveLeftWord "*_";
#pragma aux _RepWord "*_";
#pragma aux _AndWord "*_";
#pragma aux _XorWord "*_";
#pragma aux _OrWord "*_";
#pragma aux _GetDotWord "*_";
#pragma aux _ZapWord "*_";
#pragma aux _FillWord "*_";
#pragma aux _PixCopyWord "*_";
#pragma aux _PixReadWord "*_";
#pragma aux _ScanLeftWord "*_";
#pragma aux _ScanRightWord "*_";
#endif
#pragma aux _PageVESA "*_";
#pragma aux _PageVideo7 "*_";
#pragma aux _PageParadise "*_";
#pragma aux _PageATI "*_";
#pragma aux _PageTseng3 "*_";
#pragma aux _PageTseng4 "*_";
#pragma aux _PageOak "*_";
#pragma aux _PageTrident "*_";
#pragma aux _PageChips "*_";
#pragma aux _PageGenoa "*_";
#pragma aux _PageS3 "*_";
#pragma aux _PageCirrus "*_";
#pragma aux _PageViper "*_";
#endif

/* Variables */

#pragma aux _StartUp "_*";
#pragma aux _GrMode "_*";
#pragma aux _ErrorStatus "_*";
#pragma aux _CurrPos "_*";
#pragma aux _LogOrg "_*";
#pragma aux _Clipping "_*";
#pragma aux _Tx_Row_Min "_*";
#pragma aux _Tx_Col_Min "_*";
#pragma aux _Tx_Row_Max "_*";
#pragma aux _Tx_Col_Max "_*";
#pragma aux _TextPos "_*";
#pragma aux _CharAttr "_*";
#pragma aux _Wrap "_*";
#pragma aux _CursState "_*";
#pragma aux _CursorShape "_*";
#pragma aux _GrCursor "_*";
#pragma aux _CurrActivePage "_*";
#pragma aux _CurrVisualPage "_*";
#pragma aux _DefMode "_*";
#pragma aux _DefTextRows "_*";
#pragma aux _CurrBkColor "_*";
#pragma aux _CurrColor "_*";
#pragma aux _Palette "_*";
#pragma aux _FillMask "_*";
#pragma aux _DefMask "_*";
#pragma aux _HaveMask "_*";
#pragma aux _Transparent "_*";
#pragma aux _PaRf_x "_*";
#pragma aux _PaRf_y "_*";
#pragma aux _LineStyle "_*";
#pragma aux _StyleWrap "_*";
#pragma aux _PlotAct "_*";
#pragma aux _TextSettings "_*";
#pragma aux _StdFont "_*";
#pragma aux _8x8Font "_*";
#pragma aux _8x8BitMap "_*";
#pragma aux _HercFont "_*";
#pragma aux _CurrFont "_*";
#pragma aux _ArcInfo "_*";
#pragma aux _CurrPos_w "_*";
#pragma aux _Window "_*";
#pragma aux _Screen "_*";
#pragma aux _IsDBCS "_*";
#pragma aux _DBCSPairs "_*";
#pragma aux _CurrState "_*";
#pragma aux _ConfigBuffer "_*";
#pragma aux _StackSeg "_*";
#pragma aux _BiosSeg "_*";
#pragma aux _BiosOff "_*";

#if defined( _NEC_PC )
#pragma aux _NecSeg "_*";
#pragma aux _TextSeg "_*";
#pragma aux _AttrSeg "_*";
#pragma aux _NecOff "_*";
#pragma aux _TextOff "_*";
#pragma aux _AttrOff "_*";
#pragma aux _KanjiBuf "_*";
#pragma aux _NECPalette "_*";
#pragma aux _NECDefPalette "_*";
#pragma aux _GRCGPort "_*";
#else
#pragma aux _MonoSeg "_*";
#pragma aux _CgaSeg "_*";
#pragma aux _EgaSeg "_*";
#pragma aux _RomSeg "_*";
#pragma aux _MonoOff "_*";
#pragma aux _CgaOff "_*";
#pragma aux _EgaOff "_*";
#pragma aux _RomOff "_*";
#endif

#if defined( __QNX__ )
#pragma aux _CompileSeg "_*";
#pragma aux _ShadowSeg "_*";
#endif

#if defined( _SUPERVGA )
#pragma aux _VGAPage "_*";
#pragma aux _VGAGran "_*";
#pragma aux _SVGAType "_*";
#pragma aux _SetVGAPage "_*";
#endif
