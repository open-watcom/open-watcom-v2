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


#ifndef _DEBUGAPI_H_INCLUDED
#ifdef __cplusplus
extern "C" {
#define BOOL_DEFINED
#endif

#include "dbgdefn.h"
#include "dbgmem.h"
#include "dbginfo.h"
#include "bool.h"
#include "ambigsym.h"
#include "dbgtoggl.h"
#include "dbglit.h"
#include "dbgstk.h"
#include "dbgbreak.h"
#include "mad.h"
#include "dui.h"
#include "dbgvar.h"
#include "dbgchain.h"
#include "dbgreg.h"
//struct        gui_menu_struct;
#include "memtypes.h"
#include "namelist.h"

#include <string.h>
#include <stdio.h>
#include <process.h>
#include <conio.h>

// from rest of debugger

extern unsigned         ChangeMemUndoable( address addr, void *data, unsigned len );
extern void             ToItemMAD( stack_entry *entry, item_mach *tmp, mad_type_info *mti );
extern unsigned         GetMADMaxFormatWidth( mad_type_handle th );
extern void             PopEntry(void);

extern name_list        *SymCompInit( bool code, bool data, bool d2_only, bool dup_ok, mod_handle );
extern void             SymCompMatches( name_list *list, char *match, unsigned *pfirst, unsigned *plast );
extern char             *SymPickText( ambig_info *ambig, int i );

extern unsigned         ProgPeekWrap(address addr,char * buff,unsigned length );
extern void             MemFiniTypes( mem_type_walk_data *data );
extern void             MemInitTypes( mad_type_kind mas, mem_type_walk_data *data, int );
extern mad_type_handle  GetMADTypeHandleDefaultAt( address a, mad_type_kind mtk );


extern bool             DUIGetSourceLine( cue_handle *ch, char *buff, unsigned len );
extern void             ReStart( void );
extern address          GetCodeDot();
extern bool             GetSourceLine( cue_handle *ch, char *buff, unsigned len );
extern void             UnAsm( address addr, unsigned max, char *buff );
extern void             DoCmd( char *);
extern char             *DupStr(char*);
extern void             DoInput();
extern void             *WndAsmInspect( address );
extern void             StepIntoFunction( char * );
extern void             PosMachState( int rel_pos );
extern void             MoveStackPos( int by );
extern void             GoHome();
extern void             LastStackPos();
extern void             LastMachState();
extern void             BrkEnableAll(void);
extern void             BrkDisableAll(void);
extern void             BrkClearAll(void);
extern address          GetCodeDot();
extern bool             InsMemRef( mad_disasm_data *dd );
extern void             DebugMain();
extern void             FlushEOC(void);
extern char             *DupStr(char*);
extern void             DoCmd(char*);
extern void             DoInput(void);
extern void             DebugFini();
extern unsigned         Go( bool );
extern void             ExecTrace( trace_cmd_type type, debug_level level );
extern void             GoToReturn();
extern void             StepIntoFunction( char * );
extern void             ExprValue( stack_entry *entry );
extern void             *OpenSrcFile(cue_handle * );
extern void             *FOpenSource(char *,mod_handle,cue_file_id);
extern int              FReadLine(void   *,int ,int ,char *,int );
extern void             FDoneSource(void         *);
extern char             *FGetName( void *hndl );
extern void             AddSourceSpec( char *str );
extern void             FiniSource();
extern bool             DlgScanCodeAddr( char *str, address *value );
extern bool             DlgScanDataAddr( char *str, address *value );
extern bool             DlgScanGivenAddr( char *str, address *value );
extern bool             DlgScanSyntax( char *str );
extern brk              *AddBreak( address addr );
extern brk              *FindBreak( address );
extern void             RemovePoint( brk *bp );

extern bool             RemoveBreak( address addr );
extern void             LoadNewProg( char *prog, char *cmdline );
extern void             ReleaseProgOvlay( bool free_syms );
extern char             *StrCopy( char *, char * );
extern char             *GetCmdName( int index );
extern unsigned         NewCurrRadix(unsigned);
extern void             FindRadixSpec(unsigned char ,char **,unsigned int *);

extern void             PrintValue();
extern void             FreezeStack();
extern void             UnFreezeStack( bool nuke_top );
extern bool             DlgScanAny( char *str, void *value );
extern bool             DlgScanLong( char *str, long *value );
extern void             DoAssign();
extern int              Spawn( register void (*func)(void) );
extern int              SpawnP( register void (*func)(void*), void*);
extern int              SpawnPP( register void (*func)(void*,void*), void*, void* );

extern void             RecordEvent( char *p );

extern void             InitTraceBack( cached_traceback* );
extern void             FiniTraceBack( cached_traceback *tb );
extern void             UpdateTraceBack( cached_traceback *tb );
extern call_chain       *GetCallChain( cached_traceback *tb, int row );
extern void             UnWindToFrame( call_chain *chain, int, int );
extern address          FindNextIns( address );
extern void             ToggleBreak( address addr );
extern void             GoToAddr( address addr );
extern void             SkipToAddr( address addr );
extern void             BreakOnImageLoad( char *name, unsigned len, bool clear );

extern int              GetStackPos();

extern void             SetBPCountDown( brk *bp, long countdown );
extern void             SetBPResume( brk *bp, int resume );
extern void             SetBPCondition( brk *bp, char const *condition );
extern void             SetBPPatch( brk *bp, char const *patch );
extern void             SetBPAutoDestruct( brk *bp, int auto );
extern long             GetBPCountDown( brk *bp );
extern char             *GetBPCondition( brk *bp );
extern char             *GetBPPatch( brk *bp );
extern int              GetBPResume( brk *bp );
extern int              GetBPAutoDestruct( brk *bp );
extern void             GetBPText( brk *bp, char *buff );
extern void             GetBPAddr( brk *bp, char *buff );


extern address          GetRegIP();
extern char             *AddrToString( address *a, mad_address_format af, char *p, unsigned );

extern bool             SemAllowClosestLine( bool );
extern void             RegFindData( mad_type_kind kind, mad_reg_set_data **pdata );
extern void             RegValue( item_mach *value, const mad_reg_info *reginfo, machine_state *mach );

extern void             PrevError( char * );

extern void             RegNewValue( const mad_reg_info *reginfo, item_mach *new_val, mad_type_handle type );
extern void             DbgUpdate(update_list );
extern char             *CnvULongHex( unsigned long value, char *p );
extern bool             IsThdCurr( thread_state *thd );
extern void             MakeThdCurr( thread_state * );

extern inspect_type     WndGetExprSPInspectType( address *paddr );
extern bool             ScanSelectedExpr( char *expr );
extern void             BreakOnSelected( char *item );
extern bool             BreakOnRawMemory( address addr, char *comment, int size );
extern bool             WndEvalInspectExpr( char *item, bool pop );

extern void             CheckBPErrors( void );
extern int              AddrComp( address a, address b );
extern address          AddrAddWrap(address,long);

extern char             *GetUndoString();
extern char             *GetRedoString();

extern void             FiniTrap();
extern bool             InitTrap( char * );

extern bool             ReInitTrap( char * );
extern void             SetSymName( char * );
extern void             SetSymForDLL( char *sym, char *dll );
extern bool             CopyToRemote( char *local, char *remote, bool strip, void *cookie );
extern void             FindLocalDebugInfo( char * );
extern bool             RemoteSetEnvironmentVariable( char *name, char *value );

extern char             *StrAddr(address *,char *,unsigned);
extern char             *CnvNearestAddr( address addr, char *buff, unsigned max );
extern char             *ModImageName( mod_handle handle );
extern char             *SkipPathInfo( char *, char );

extern unsigned         DbgCueFile( cue_handle *ch, char *file, unsigned max );
extern unsigned long    DbgCueLine( cue_handle *ch );
extern cue_file_id      DbgCueFileId( cue_handle *ch );
extern search_result    DbgLineCue( mod_handle mod, cue_file_id id,
                          unsigned long line, unsigned column, cue_handle *ch );

extern void             FiniLocalInfo( void );
extern void             TrapSetAccessCallBack( register void (*func)(void) );
extern void             TrapFailAllRequests();
extern void             GetSysConfig();

extern int              EnvLkup( char *src, char *dst, int max_len );

extern name_list        SortedNames;
extern machine_state    *DbgRegs;
extern char             *TrpFile;
extern debug_level      DbgLevel;
extern char             *TxtBuff;
extern unsigned char    CurrRadix;
extern char             *TxtBuff;
extern update_list      WndFlags;
extern stack_entry      *ExprSP;
extern mod_handle       ContextMod;
extern address          NilAddr;
extern thread_state     *HeadThd;
extern brk              *BrkList;
extern unsigned         TaskId;
extern image_entry      *DbgImageList;



#ifdef __cplusplus
}
#endif
#define _DEBUGAPI_H_INCLUDED
#endif
