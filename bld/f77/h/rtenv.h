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
* Description:  Special calling conventions for F77 runtime routines.
*
****************************************************************************/


#if defined( _M_IX86 )
    #pragma aux FMAIN "*";
    #if defined( __WINDOWS__ ) || defined( __NT__ )
      #pragma aux FWINMAIN "*";
    #endif

  #if defined( __386__ )
    #pragma aux rt_rtn "RT@*" parm routine [eax ebx ecx edx 8087];
    #pragma aux co_rtn "RT@*" parm routine [eax ebx ecx edx 8087];
    #pragma aux va_rtn "RT@*" parm caller [];
    #if defined( __SW_3S )
      #if defined( __FLAT__ )
        #pragma aux (rt_rtn) rt_rtn modify [8087 gs];
        #pragma aux (co_rtn) co_rtn modify [8087 gs];
        #pragma aux (va_rtn) va_rtn modify [8087 gs];
      #else
        #pragma aux (rt_rtn) rt_rtn modify [8087 es fs gs];
        #pragma aux (co_rtn) co_rtn modify [8087 es fs gs];
        #pragma aux (va_rtn) va_rtn modify [8087 es fs gs];
      #endif
      #if defined( __FPI__ )
        #pragma aux (rt_rtn) flt_rt_rtn value [8087];
      #else
        #pragma aux (rt_rtn) flt_rt_rtn;
      #endif
    #else
      #pragma aux (rt_rtn) flt_rt_rtn;
    #endif
  #else
    #pragma aux rt_rtn "RT@*" parm [ax bx cx dx 8087];
    #pragma aux co_rtn "RT@*" parm [ax bx cx dx 8087];
    #pragma aux va_rtn "RT@*" parm caller [];
    #pragma aux (rt_rtn) flt_rt_rtn;
  #endif

  #pragma aux (rt_rtn) PowII;
  #pragma aux (flt_rt_rtn) PowRR;
  #pragma aux (flt_rt_rtn) PowRI;
  #pragma aux (flt_rt_rtn) PowXI;
  #pragma aux (rt_rtn) C8Mul;
  #pragma aux (rt_rtn) C16Mul;
  #pragma aux (rt_rtn) C32Mul;
  #pragma aux (rt_rtn) C8Div;
  #pragma aux (rt_rtn) C16Div;
  #pragma aux (rt_rtn) C32Div;
  #pragma aux (rt_rtn) C8Pow;
  #pragma aux (rt_rtn) C16Pow;
  #pragma aux (rt_rtn) C32Pow;
  #pragma aux (rt_rtn) C8PowI;
  #pragma aux (rt_rtn) C16PowI;
  #pragma aux (rt_rtn) C32PowI;
  #pragma aux (rt_rtn) PrtArr;
  #pragma aux (rt_rtn) PrtChArr;
  #pragma aux (rt_rtn) InpArr;
  #pragma aux (rt_rtn) InpChArr;
  #pragma aux (co_rtn) OutLOG1;
  #pragma aux (co_rtn) OutLOG4;
  #pragma aux (co_rtn) OutINT1;
  #pragma aux (co_rtn) OutINT2;
  #pragma aux (co_rtn) OutINT4;
  #pragma aux (co_rtn) OutREAL;
  #pragma aux (co_rtn) OutDBLE;
  #pragma aux (co_rtn) OutXTND;
  #pragma aux (co_rtn) OutCPLX;
  #pragma aux (co_rtn) OutDBCX;
  #pragma aux (co_rtn) OutXTCX;
  #pragma aux (co_rtn) OutCHAR;
  #pragma aux (co_rtn) InpLOG1;
  #pragma aux (co_rtn) InpLOG4;
  #pragma aux (co_rtn) InpINT1;
  #pragma aux (co_rtn) InpINT2;
  #pragma aux (co_rtn) InpINT4;
  #pragma aux (co_rtn) InpREAL;
  #pragma aux (co_rtn) InpDBLE;
  #pragma aux (co_rtn) InpXTND;
  #pragma aux (co_rtn) InpCPLX;
  #pragma aux (co_rtn) InpDBCX;
  #pragma aux (co_rtn) InpXTCX;
  #pragma aux (co_rtn) InpCHAR;
  #pragma aux (va_rtn) Cat;
  #pragma aux (va_rtn) TCat;
  #pragma aux (rt_rtn) Move;
  #pragma aux (rt_rtn) Substring;
  #pragma aux (va_rtn) Subscript;
  #pragma aux (rt_rtn) FmtScan;
  #pragma aux (rt_rtn) FmtAScan;
  #pragma aux (rt_rtn) Stop;
  #pragma aux (rt_rtn) Pause;
  #pragma aux (rt_rtn) SetIntl;
  #pragma aux (rt_rtn) SetIOCB;
  #pragma aux (rt_rtn) SetUnit;
  #pragma aux (rt_rtn) SetFmt;
  #pragma aux (rt_rtn) SetNml;
  #pragma aux (rt_rtn) SetErr;
  #pragma aux (rt_rtn) SetEnd;
  #pragma aux (rt_rtn) SetRec;
  #pragma aux (rt_rtn) SetIOS;
  #pragma aux (rt_rtn) SetAcc;
  #pragma aux (rt_rtn) SetBlnk;
  #pragma aux (rt_rtn) SetFile;
  #pragma aux (rt_rtn) SetForm;
  #pragma aux (rt_rtn) SetLen;
  #pragma aux (rt_rtn) SetStat;
  #pragma aux (rt_rtn) SetDir;
  #pragma aux (rt_rtn) SetFmtd;
  #pragma aux (rt_rtn) SetName;
  #pragma aux (rt_rtn) SetSeq;
  #pragma aux (rt_rtn) SetUnFmtd;
  #pragma aux (rt_rtn) SetExst;
  #pragma aux (rt_rtn) SetNmd;
  #pragma aux (rt_rtn) SetNRec;
  #pragma aux (rt_rtn) SetNumb;
  #pragma aux (rt_rtn) SetOpen;
  #pragma aux (rt_rtn) SetRecl;
  #pragma aux (rt_rtn) SetNoFmt;
  #pragma aux (rt_rtn) SetAction;
  #pragma aux (rt_rtn) SetCCtrl;
  #pragma aux (rt_rtn) SetRecType;
  #pragma aux (rt_rtn) SetBlkSize;
  #pragma aux (rt_rtn) InqBlkSize;
  #pragma aux (rt_rtn) SetShare;
  #pragma aux (rt_rtn) IOOpen;
  #pragma aux (rt_rtn) IOClose;
  #pragma aux (rt_rtn) IORead;
  #pragma aux (rt_rtn) IOWrite;
  #pragma aux (rt_rtn) IORew;
  #pragma aux (rt_rtn) IOBack;
  #pragma aux (rt_rtn) IOEndf;
  #pragma aux (rt_rtn) IOInq;
  #pragma aux (rt_rtn) LexCmp;
  #pragma aux (rt_rtn) Alloc;
  #pragma aux (rt_rtn) DeAlloc;
  #pragma aux (rt_rtn) SetLine;
  #pragma aux (rt_rtn) SetModule;
  #pragma aux (rt_rtn) ADVFillHi;
  #pragma aux (rt_rtn) ADVFillHiLo1;

  #pragma aux (rt_rtn) lg_rtn "LG@*";
  #pragma aux (lg_rtn) STOP_HOOK;
  #pragma aux (lg_rtn) PAUSE_HOOK;
  #pragma aux (lg_rtn) ERR_HOOK;
#else
  #define PowII         __RT_PowII
  #define PowRR         __RT_PowRR
  #define PowRI         __RT_PowRI
  #define PowXI         __RT_PowXI
  #define C8Mul         __RT_C8Mul
  #define C16Mul        __RT_C16Mul
  #define C32Mul        __RT_C32Mul
  #define C8Div         __RT_C8Div
  #define C16Div        __RT_C16Div
  #define C32Div        __RT_C32Div
  #define C8Pow         __RT_C8Pow
  #define C16Pow        __RT_C16Pow
  #define C32Pow        __RT_C32Pow
  #define C8PowI        __RT_C8PowI
  #define C16PowI       __RT_C16PowI
  #define C32PowI       __RT_C32PowI
  #define PrtArr        __RT_PrtArr
  #define PrtChArr      __RT_PrtChArr
  #define InpArr        __RT_InpArr
  #define InpChArr      __RT_InpChArr
  #define OutLOG1       __RT_OutLOG1
  #define OutLOG4       __RT_OutLOG4
  #define OutINT1       __RT_OutINT1
  #define OutINT2       __RT_OutINT2
  #define OutINT4       __RT_OutINT4
  #define OutREAL       __RT_OutREAL
  #define OutDBLE       __RT_OutDBLE
  #define OutXTND       __RT_OutXTND
  #define OutCPLX       __RT_OutCPLX
  #define OutDBCX       __RT_OutDBCX
  #define OutXTCX       __RT_OutXTCX
  #define OutCHAR       __RT_OutCHAR
  #define InpLOG1       __RT_InpLOG1
  #define InpLOG4       __RT_InpLOG4
  #define InpINT1       __RT_InpINT1
  #define InpINT2       __RT_InpINT2
  #define InpINT4       __RT_InpINT4
  #define InpREAL       __RT_InpREAL
  #define InpDBLE       __RT_InpDBLE
  #define InpXTND       __RT_InpXTND
  #define InpCPLX       __RT_InpCPLX
  #define InpDBCX       __RT_InpDBCX
  #define InpXTCX       __RT_InpXTCX
  #define InpCHAR       __RT_InpCHAR
  #define Cat           __RT_Cat
  #define TCat          __RT_TCat
  #define Move          __RT_Move
  #define Substring     __RT_Substring
  #define Subscript     __RT_Subscript
  #define FmtScan       __RT_FmtScan
  #define FmtAScan      __RT_FmtAScan
  #define Stop          __RT_Stop
  #define Pause         __RT_Pause
  #define SetIntl       __RT_SetIntl
  #define SetIOCB       __RT_SetIOCB
  #define SetUnit       __RT_SetUnit
  #define SetFmt        __RT_SetFmt
  #define SetNml        __RT_SetNml
  #define SetErr        __RT_SetErr
  #define SetEnd        __RT_SetEnd
  #define SetRec        __RT_SetRec
  #define SetIOS        __RT_SetIOS
  #define SetAcc        __RT_SetAcc
  #define SetBlnk       __RT_SetBlnk
  #define SetFile       __RT_SetFile
  #define SetForm       __RT_SetForm
  #define SetLen        __RT_SetLen
  #define SetStat       __RT_SetStat
  #define SetDir        __RT_SetDir
  #define SetFmtd       __RT_SetFmtd
  #define SetName       __RT_SetName
  #define SetSeq        __RT_SetSeq
  #define SetUnFmtd     __RT_SetUnFmtd
  #define SetExst       __RT_SetExst
  #define SetNmd        __RT_SetNmd
  #define SetNRec       __RT_SetNRec
  #define SetNumb       __RT_SetNumb
  #define SetOpen       __RT_SetOpen
  #define SetRecl       __RT_SetRecl
  #define SetNoFmt      __RT_SetNoFmt
  #define SetAction     __RT_SetAction
  #define SetCCtrl      __RT_SetCCtrl
  #define SetRecType    __RT_SetRecType
  #define SetBlkSize    __RT_SetBlkSize
  #define InqBlkSize    __RT_InqBlkSize
  #define SetShare      __RT_SetShare
  #define IOOpen        __RT_IOOpen
  #define IOClose       __RT_IOClose
  #define IORead        __RT_IORead
  #define IOWrite       __RT_IOWrite
  #define IORew         __RT_IORew
  #define IOBack        __RT_IOBack
  #define IOEndf        __RT_IOEndf
  #define IOInq         __RT_IOInq
  #define LexCmp        __RT_LexCmp
  #define Alloc         __RT_Alloc
  #define DeAlloc       __RT_DeAlloc
  #define SetLine       __RT_SetLine
  #define SetModule     __RT_SetModule
  #define ADVFillHi     __RT_ADVFillHi
  #define ADVFillHiLo1  __RT_ADVFillHiLo1
#endif

typedef struct traceback traceback;

extern void     Alloc( unsigned_16 alloc_type, uint num, ... );
extern void     DeAlloc( intstar4 PGM *stat, uint num, ... );
extern void     SetRecType( string PGM *recfmptr );
extern void     SetFmt( void PGM *ptr );
extern void     SetNml( void PGM *nml, ... );
extern void     SetUnit( intstar4 unitid );
extern void     SetIntl( string PGM *internal, unsigned_32 elmts );
extern void     SetErr( void );
extern void     SetEnd( void );
extern void     SetRec( intstar4 rec );
extern void     SetIOS( intstar4 PGM *iosptr );
extern void     SetAcc( string PGM *accptr );
extern void     SetBlnk( string PGM *blnkptr );
extern void     SetFile( string PGM *filename );
extern void     SetForm( string PGM *formptr );
extern void     SetLen( intstar4 PGM *lenptr );
extern void     SetStat( string PGM *statptr );
extern void     SetDir( string PGM *dirptr );
extern void     SetFmtd( string PGM *fmtdptr );
extern void     SetName( string PGM *nameptr );
extern void     SetSeq( string PGM *seqptr );
extern void     SetUnFmtd( string PGM *ufmtdptr );
extern void     SetExst( logstar4 PGM *exstptr );
extern void     SetNmd( logstar4 PGM *nmdptr );
extern void     SetNRec( intstar4 PGM *nrecptr );
extern void     SetNumb( intstar4 PGM *numbptr );
extern void     SetOpen( logstar4 PGM *openptr );
extern void     SetRecl( intstar4 recl );
extern void     SetNoFmt( void );
extern void     SetCCtrl( string PGM *cc );
extern void     SetAction( string PGM *action );
extern void     SetBlkSize( intstar4 bsize );
extern void     SetLine( uint src_line );
extern void     SetModule( traceback *tb );
extern void     InqBlkSize( intstar4 PGM *bsizeptr );
extern void     SetShare( string PGM *share );
extern void     __ReleaseIOSys( void );
extern int      IOBack( void );
extern int      IOClose( void );
extern int      IOEndf( void );
extern int      IOInq( void );
extern int      IOOpen( void );
extern int      IORew( void );
extern void     FmtScan( string *fmt, uint extend_format );
extern void     FmtAScan( char PGM *array, long int num_elts, int elt_size, uint extend_format );
extern void     Stop( string PGM *ptr );
extern void     Pause( string PGM *ptr );
extern void     Cat( int num_args, string *dest, ... );
extern void     TCat( int num_args, string *dest, ... );
extern void     Move( string *dst, string *src );
extern void     Substring( string *src, intstar4 start, intstar4 end, string *dst );
extern intstar4 Subscript( int dims, adv_entry *adv, ... );
extern void     ADVFillHi( adv_entry *adv, unsigned ss, intstar4 hi );
extern void     ADVFillHiLo1( adv_entry *adv, unsigned ss, intstar4 hi );
extern int      LexCmp( string PGM *str1, string PGM *str2 );
