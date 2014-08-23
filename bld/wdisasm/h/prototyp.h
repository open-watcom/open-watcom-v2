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


extern char *DBstring( void );
extern void ToUnixInsName( char *, instruction * );
extern void DoCode( instruction *, bool );
extern void DoControl( int );
extern void DoOTModRM( void );
extern void DoWtk( void );
extern bool HaveWtk( void );
extern void ModRMRegOp( int );
extern void ModRMOp( int, bool );
extern void FormatLine( fixup *, char *, bool, char );
extern void FormatIns( char *, instruction *, form_option );
#ifndef O2A
static void ZapUpper( char * );
#else
extern void ZapUpper( char * );
#endif
extern ins_name GetFppCode( void );
extern void GetFppOp( void );
extern char *GetWtkInsName( uint_16 );
extern void Obj2Asm( segment * );
extern void FlipToSeg( segment * );
extern void DisAssemble( void );
extern void EmitModule( void );
extern void EmitEndMod( void );
extern void EmitGroup( group * );
extern void EmitSegment( void );
extern void EmitEndSeg( void );
extern void EmitLabel( char *, uint_32 );
extern void DoEmit( int );
extern void Emit( char * );
extern int  EmitSym( char *, unsigned );
extern char *FormSym( char * );
extern void EmitNL( void );
extern void EmitHex( uint_32 );
extern void EmitLine( char * );
extern void EmitAddr( uint_32, int, char * );
extern void EmitDashes( int );
extern void EmitBlanks( int );
extern void DoEmitSpaced( int, int );
extern int  EmitSpaced(char *, int );
extern void DoEmitError( int );
extern void EmitError( char * );
extern void EmitLoc( void );
extern void EmitBytes( void );
extern void EmitSrc( void );
extern void InitAsm( void );
extern bool IsWtk( instruction * );
extern void FiniAsm( void );
extern void EmitDups( void );
extern char *SegName( void );
extern uint_8 GetSegByte( uint_32 );
extern uint_16 GetSegWord( uint_32 );
extern uint_32 GetSegDWord( uint_32 );
extern void PutSegByte( uint_32, uint_8 );
extern void PutSegWord( uint_32, uint_16 );
extern void PutSegDWord( uint_32, uint_32 );
extern void InitSegAccess( void );
extern uint_32 GetOffset( void );
extern int_16 GetDataByte( void );
extern int_16 PeekDataByte( void );
extern int_16 GetDataWord( void );
extern int_32 GetDataLong( void );
extern int_16 GetNextByte( void );
extern char EndOfSegment( void );
extern char *ToStr( uint_32, uint_16, uint_32 );
extern char *ToIndex( uint_32, uint_32 );
extern char *ToBrStr( uint_32, uint_32 );
extern char *ToSegStr( uint_32, uint_16, uint_32 );
extern char *JmpLabel( uint_32, uint_32 );
extern void IToHS( char *, uint_32, int );
extern void DoIToHS( char *, int_32, int );
extern void MyIToHS( char *, int_32 );
extern char *GetGroupName( fixup * );
extern int FGetObj( char *, int );
extern FILE *OpenBinRead( char * );
extern void CloseBin( FILE * );
extern FILE *OpenTxtRead( char * );
extern FILE *OpenTxtWrite( char * );
extern void CloseTxt( FILE * );
extern void FPutTxtRec( FILE *, char *, int );
extern int FGetTxtRec( FILE *, char *, int );
extern void InitObj( void );
extern void MarkBegData( void );
extern void SkipPcoRec( void );
extern void GetObjRec( void );
extern void ExchangeBuffers( void );
extern uint_32 Addr( uint_32 );
extern uint_32 Addr32( uint_32);
extern char GetByte( void );
extern void ParseObjectOMF( void );
extern void GrpDef( void );
extern void PubDef(bool);
extern void ExtDef(bool);
extern void ComDef(bool);
extern void ComDat( void );
extern void CExtDef( void );
extern void BackPatch( void );
extern void NBackPatch( void );
extern void LinNum( void );
extern void LinSym( void );
extern void FixUpp( void );
extern void THeadr( void );
extern void LName( void );
extern void SegDef( void );
extern void LEData( void );
extern void LIData( void );
extern void ModEnd( void );
extern void Coment( void );
extern uint_32 GetLong( void );
extern uint_16 GetWord( void );
extern uint_16 GetIndex( void );
extern uint_32 GetVarSize( void );
extern void Error( int, bool );
extern void DoError( char *, bool );
extern char *NameAlloc( char *, int );
extern char *GetName( int );
extern module *NewModule( void );
extern segment *NewSegment( void );
extern group *NewGroup( void );
extern handle *NewHandle( void * );
extern fixup *NewFixup( char );
extern void AddFix( segment *, fixup * );
extern void *GetTab( uint_16, void **, bool );
extern void PutTab( uint_16, void **, void * );
extern void InitTables( void );
extern char *FindExpName( uint_32, segment * );
extern char *FindLabel( uint_32, uint_32, segment * );
extern fixup *FindFixup( uint_32, segment * );
extern char *FindSymbol( uint_32 );
extern void FindSrcLine( uint_32 );
extern void InitSymList( void );
extern void DumpSymList( void );
extern char *GetFixName( fixup * );
extern void DumpImpList( void );
extern struct export_sym *AddLabel( uint_32, char *, segment *, bool, bool );
extern char *NewName( fixup * );
extern char *MakeFileName( char *, char * );
extern char *ParseName( char *, char * );
extern void PutString( char * );
extern void PutText( int );
extern void DirectPuts( int, FILE * );
extern void FPutEnd(FILE *);
extern void InitOutput( void );
extern void OpenSource( void );
extern void SysHelp( void );
extern int FuzzyStrCmp( char *, char * );
extern bool IsData( segment * );
extern void *AllocMem( size_t );
extern void *AllocNull( size_t );
extern void FreeMem( void * );
extern void SysError( int, bool );
extern bool LabelInInstr( void );
extern bool LabelOnInstr( void );
extern char *GetLabel( void );
extern uint_16 DoPcode( void );
extern void FormatPcode( char * );
extern char *stpcpy( char *, const char * );
extern void FreeSymTranslations( void );
extern bool InitORL( void );
extern void ParseObjectORL( void );
extern void FiniORL( void );
