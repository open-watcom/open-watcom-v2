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
* Description:  Assembler routines called by C.
*
****************************************************************************/


/* To make the 386 graphics library, the two libraries, graph3r and
   graph3s, are combined. Only one copy of the assembler modules is
   placed in the combined library. For this to work properly, all
   references to symbol names must be done the same way. */

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

#if defined( _SUPERVGA )

#pragma aux _MoveUpVESA "*_";
#pragma aux _MoveDownVESA "*_";
#pragma aux _MoveLeftVESA "*_";
#pragma aux _MoveRightVESA "*_";

#pragma aux _RepWord "*_";
#pragma aux _AndWord "*_";
#pragma aux _XorWord "*_";
#pragma aux _OrWord "*_";
#pragma aux _GetDotWord "*_";
#pragma aux _ZapWord "*_";
#pragma aux _RepDWord "*_";
#pragma aux _AndDWord "*_";
#pragma aux _XorDWord "*_";
#pragma aux _OrDWord "*_";
#pragma aux _GetDotDWord "*_";
#pragma aux _ZapDWord "*_";
#pragma aux _RepTByte "*_";
#pragma aux _AndTByte "*_";
#pragma aux _XorTByte "*_";
#pragma aux _OrTByte "*_";
#pragma aux _GetDotTByte "*_";
#pragma aux _ZapTByte "*_";
#pragma aux _FillWord "*_";
#pragma aux _FillDWord "*_";
#pragma aux _FillTByte "*_";
#pragma aux _PixCopyWord "*_";
#pragma aux _PixReadWord "*_";
#pragma aux _PixCopyDWord "*_";
#pragma aux _PixReadDWord "*_";
#pragma aux _PixCopyTByte "*_";
#pragma aux _PixReadTByte "*_";
#pragma aux _ScanLeftWord "*_";
#pragma aux _ScanRightWord "*_";
#pragma aux _ScanLeftDWord "*_";
#pragma aux _ScanRightDWord "*_";
#pragma aux _ScanLeftTByte "*_";
#pragma aux _ScanRightTByte "*_";

#pragma aux _MoveLeftEGAX "*_";
#pragma aux _MoveRightEGAX "*_";
#pragma aux _ZapEGAX "*_";
#pragma aux _FillEGAX "*_";
#pragma aux _PixCopyEGAX "*_";
#pragma aux _PixReadRowEGAX "*_";
#pragma aux _ScanLeftEGAX "*_";
#pragma aux _ScanRightEGAX "*_";

#if 0
// This mode is untested
#pragma aux _MoveUp1280 "*_";
#pragma aux _MoveDown1280 "*_";
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
#pragma aux _MonoSeg "_*";
#pragma aux _CgaSeg "_*";
#pragma aux _EgaSeg "_*";
#pragma aux _RomSeg "_*";
#pragma aux _MonoOff "_*";
#pragma aux _CgaOff "_*";
#pragma aux _EgaOff "_*";
#pragma aux _RomOff "_*";

#if defined( __QNX__ )
#pragma aux _CompileSeg "_*";
//#pragma aux _ShadowSeg "_*";
#endif

#if defined( VERSION2 )
#pragma aux _coltbl "_*";
#endif

#if defined( _SUPERVGA ) && defined( VERSION2 )
#pragma aux _VGABytesPerPixel "_*";
#endif

#if defined( _SUPERVGA )
#pragma aux _VGAPage "_*";
#pragma aux _VGAGran "_*";
#pragma aux _SVGAType "_*";
#pragma aux _SetVGAPage "_*";
#pragma aux _VGAStride "_*";
#endif

// generic, CGA or Hecules
extern void __pascal    __near _MoveUp( void );
extern void __pascal    __near _Move1Left( void );
extern void __pascal    __near _Move2Left( void );
extern void __pascal    __near _MoveDown( void );
extern void __pascal    __near _Move1Right( void );
extern void __pascal    __near _Move2Right( void );
extern void __pascal    __near _HercMoveUp( void );
extern void __pascal    __near _HercMoveDown( void );

extern void             __near _CoRep( char __far *, grcolor, int );
extern void             __near _CoXor( char __far *, grcolor, int );
extern void             __near _CoAnd( char __far *, grcolor, int );
extern void             __near _CoOr( char __far *, grcolor, int );
extern grcolor          __near _Get1Dot( char __far *, int );
extern grcolor          __near _Get2Dot( char __far *, int );
extern void             __near _Pix1Zap( char __far *, grcolor, int, int, int );
extern void             __near _Pix2Zap( char __far *, grcolor, int, int, int );
extern void             __near _Pix1Fill( char __far *, grcolor, int, int, int );
extern void             __near _Pix2Fill( char __far *, grcolor, int, int, int );
extern void             __near _Pix1Copy( char __far *, char __far *, int, int, int );
extern void             __near _Pix2Copy( char __far *, char __far *, int, int, int );
extern void             __near _Pix1Read( char __far *, char __far *, int, int, int );
extern void             __near _Pix2Read( char __far *, char __far *, int, int, int );
extern short            __near _CGAScanLeft( char __far *, grcolor, int, int, int, int );
extern short            __near _CGAScan1Right( char __far *, grcolor, int, int, int, int );
extern short            __near _CGAScan2Right( char __far *, grcolor, int, int, int, int );

// EGA
extern void __pascal    __near _EGAMoveLeft( void );
extern void __pascal    __near _EGAMoveLeftX( void );
extern void __pascal    __near _EGAMoveRight( void );
extern void __pascal    __near _EGAMoveRightX( void );
extern void __pascal    __near _EGAMoveUpLo( void );
extern void __pascal    __near _EGAMoveUpHi( void );
extern void __pascal    __near _EGAMoveDownLo( void );
extern void __pascal    __near _EGAMoveDownHi( void );
extern void             __near _EGARep( char __far *, grcolor, int );
extern grcolor          __near _EGAGetDot( char __far *, int );
extern grcolor          __near _EGAGetDotEO( char __far *, int );
extern grcolor          __near _EGAGetDotMono( char __far *, int );
extern void             __near _EGAZap( char __far *, grcolor, int, int, int );
extern void             __near _EGAZapX( char __far *, grcolor, int, int, int );
extern void             __near _EGAZapEO( char __far *, grcolor, int, int, int );
extern void             __near _EGAZapMono( char __far *, grcolor, int, int, int );
extern void             __near _EGAFill( char __far *, grcolor, int, int, int );
extern void             __near _EGAFillX( char __far *, grcolor, int, int, int );
extern void             __near _EGAFillEO( char __far *, grcolor, int, int, int );
extern void             __near _EGAFillMono( char __far *, grcolor, int, int, int );
extern void             __near _EGAPixCopy( char __far *, char __far *, int, int, int );
extern void             __near _EGAPixCopyX( char __far *, char __far *, int, int, int );
extern void             __near _EGAPixCopyEO( char __far *, char __far *, int, int, int );
extern void             __near _EGAPixCopyMono( char __far *, char __far *, int, int, int );
extern void             __near _EGAReadRow( char __far *, char __far *, int, int, int );
extern void             __near _EGAReadRowX( char __far *, char __far *, int, int, int );
extern void             __near _EGAReadRowEO( char __far *, char __far *, int, int, int );
extern void             __near _EGAReadRowMono( char __far *, char __far *, int, int, int );
extern short            __near _EGAScanLeft( char __far *, grcolor, int, int, int, int );
extern short            __near _EGAScanLeftX( char __far *, grcolor, int, int, int, int );
extern short            __near _EGAScanLeftEO( char __far *, grcolor, int, int, int, int );
extern short            __near _EGAScanLeftMono( char __far *, grcolor, int, int, int, int );
extern short            __near _EGAScanRight( char __far *, grcolor, int, int, int, int );
extern short            __near _EGAScanRightX( char __far *, grcolor, int, int, int, int );
extern short            __near _EGAScanRightEO( char __far *, grcolor, int, int, int, int );
extern short            __near _EGAScanRightMono( char __far *, grcolor, int, int, int, int );

// VGA
extern void __pascal    __near _MoveUp19( void );
extern void __pascal    __near _MoveLeft19( void );
extern void __pascal    __near _MoveDown19( void );
extern void __pascal    __near _MoveRight19( void );
extern void             __near _And19( char __far *, grcolor, int );
extern void             __near _Rep19( char __far *, grcolor, int );
extern grcolor          __near _GetDot19( char __far *, int );
extern void             __near _Fill19( char __far *, grcolor, int, int, int );
extern void             __near _Zap19( char __far *, grcolor, int, int, int );
extern void             __near _PixCopy19( char __far *, char __far *, int, int, int );
extern void             __near _PixRead19( char __far *, char __far *, int, int, int );
extern short            __near _ScanLeft19( char __far *, grcolor, int, int, int, int );
extern short            __near _ScanRight19( char __far *, grcolor, int, int, int, int );

#if defined( _SUPERVGA )

// SVGA
extern void __pascal    __near _MoveUp100( void );
extern void __pascal    __near _MoveUp128( void );
extern void __pascal    __near _MoveUp640( void );
extern void __pascal    __near _MoveUp800( void );
extern void __pascal    __near _MoveUp1024( void );
extern void __pascal    __near _MoveLeft256( void );
extern void __pascal    __near _MoveDown100( void );
extern void __pascal    __near _MoveDown128( void );
extern void __pascal    __near _MoveDown640( void );
extern void __pascal    __near _MoveDown800( void );
extern void __pascal    __near _MoveDown1024( void );
extern void __pascal    __near _MoveRight256( void );
extern void             __near _Zap256( char __far *, grcolor, int, int, int );
extern void             __near _Fill256( char __far *, grcolor, int, int, int );
extern void             __near _PixCopy256( char __far *, char __far *, int, int, int );
extern void             __near _PixRead256( char __far *, char __far *, int, int, int );
extern short            __near _ScanLeft256( char __far *, grcolor, int, int, int, int );
extern short            __near _ScanRight256( char __far *, grcolor, int, int, int, int );
extern void __pascal    __near _MoveUpVESA( void );
extern void __pascal    __near _MoveDownVESA( void );
extern void __pascal    __near _MoveLeftVESA( void );
extern void __pascal    __near _MoveRightVESA( void );
extern void             __near _RepWord( char __far *, grcolor, int );
extern void             __near _XorWord( char __far *, grcolor, int );
extern void             __near _AndWord( char __far *, grcolor, int );
extern void             __near _OrWord( char __far *, grcolor, int );
extern grcolor          __near _GetDotWord( char __far *, int );
extern grcolor          __near _GetDotDWord( char __far *, int );
extern grcolor          __near _GetDotTByte( char __far *, int );
extern void             __near _ZapWord( char __far *, grcolor, int, int, int );
extern void             __near _ZapDWord( char __far *, grcolor, int, int, int );
extern void             __near _ZapTByte( char __far *, grcolor, int, int, int );
extern void             __near _RepDWord( char __far *, grcolor, int );
extern void             __near _XorDWord( char __far *, grcolor, int );
extern void             __near _AndDWord( char __far *, grcolor, int );
extern void             __near _OrDWord( char __far *, grcolor, int );
extern void             __near _RepTByte( char __far *, grcolor, int );
extern void             __near _XorTByte( char __far *, grcolor, int );
extern void             __near _AndTByte( char __far *, grcolor, int );
extern void             __near _OrTByte( char __far *, grcolor, int );
extern void             __near _FillWord( char __far *, grcolor, int, int, int );
extern void             __near _FillDWord( char __far *, grcolor, int, int, int );
extern void             __near _FillTByte( char __far *, grcolor, int, int, int );
extern void             __near _PixCopyWord( char __far *, char __far *, int, int, int );
extern void             __near _PixReadWord( char __far *, char __far *, int, int, int );
extern void             __near _PixCopyDWord( char __far *, char __far *, int, int, int );
extern void             __near _PixReadDWord( char __far *, char __far *, int, int, int );
extern void             __near _PixCopyTByte( char __far *, char __far *, int, int, int );
extern void             __near _PixReadTByte( char __far *, char __far *, int, int, int );
extern short            __near _ScanLeftWord( char __far *, long, int, int, int, int );
extern short            __near _ScanRightWord( char __far *, long, int, int, int, int );
extern short            __near _ScanLeftDWord( char __far *, long, int, int, int, int );
extern short            __near _ScanRightDWord( char __far *, long, int, int, int, int );
extern short            __near _ScanLeftTByte( char __far *, long, int, int, int, int );
extern short            __near _ScanRightTByte( char __far *, long, int, int, int, int );

extern void __pascal    __near _MoveLeftEGAX( void );
extern void __pascal    __near _MoveRightEGAX( void );
extern void             __near _ZapEGAX( char __far *, grcolor, int, int, int );
extern void             __near _FillEGAX( char __far *, grcolor, int, int, int );
extern void             __near _PixCopyEGAX( char __far *, char __far *, int, int, int );
extern void             __near _PixReadRowEGAX( char __far *, char __far *, int, int, int );
extern short            __near _ScanLeftEGAX( char __far *, long, int, int, int, int );
extern short            __near _ScanRightEGAX( char __far *, long, int, int, int, int );

#if 0 // This mode is untested
extern void             __near _MoveUp1280( void );
extern void             __near _MoveDown1280( void );
extern void             __near _MoveLeftWord( void );
extern void             __near _MoveRightWord( void );
#endif

extern void __pascal    _FARC _PageVESA( short );
extern void __pascal    _FARC _PageVideo7( short );
extern void __pascal    _FARC _PageParadise( short );
extern void __pascal    _FARC _PageATI( short );
extern void __pascal    _FARC _PageTseng3( short );
extern void __pascal    _FARC _PageTseng4( short );
extern void __pascal    _FARC _PageOak( short );
extern void __pascal    _FARC _PageTrident( short );
extern void __pascal    _FARC _PageChips( short );
extern void __pascal    _FARC _PageGenoa( short );
extern void __pascal    _FARC _PageS3( short );
extern void __pascal    _FARC _PageCirrus( short );
extern void __pascal    _FARC _PageViper( short );

#endif
