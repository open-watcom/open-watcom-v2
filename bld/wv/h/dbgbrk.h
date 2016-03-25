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
* Description:  Breakpoint and watchpoint management.
*
****************************************************************************/


extern void     InitBPs( void );
extern bool     InsertBPs( bool force );
extern void     RemoveBPs( void );
extern void     GetBPAddr( brkp *bp, char *buff );
extern void     GetBPText( brkp *bp, char *buff );
extern brkp     *FindBreakByLine( mod_handle mod, cue_fileid id, unsigned line );
extern brkp     *FindBreak( address addr );
extern bool     DispBPMsg( bool stack_cmds );
extern void     ActPoint( brkp *bp, bool act );
extern void     BrkEnableAll( void );
extern void     BrkDisableAll( void );
extern void     RemovePoint( brkp *bp );
extern bool     RemoveBreak( address addr );
extern void     BrkClearAll( void );
extern void     BPsDeac( void );
extern void     BPsUnHit( void );
extern void     RecordNewPoint( brkp *bp );
extern void     RecordPointStart( void );
extern void     RecordClearPoint( brkp *bp );
extern void     GetBreakOnImageCmd( const char *name, char *buff, bool clear );
extern void     ShowBPs( void );
extern void     ProcBreak( void );
extern char     *CopySourceLine( cue_handle *ch );
extern bool     GetBPSymAddr( brkp *bp, address *addr );
extern void     SetPointAddr( brkp *bp, address addr );
extern bool     BrkCheckWatchLimit( address loc, mad_type_handle th );
extern brkp     *AddBreak( address addr );
extern void     ToggleBreak( address addr );
extern void     BrkAddrRefresh( void );
extern void     SetBPCountDown( brkp *bp, long countdown );
extern bool     GetBPAutoDestruct( brkp *bp );
extern void     SetBPAutoDestruct( brkp *bp, int autodes );
extern void     SetBPCondition( brkp *bp, const char *condition );
extern void     SetBPPatch( brkp *bp, char *patch );
extern void     SetBPResume( brkp *bp, int resume );
extern long     GetBPCountDown( brkp *bp );
extern char     *GetBPCondition( brkp *bp );
extern char     *GetBPPatch( brkp *bp );
extern int      GetBPResume( brkp *bp );
extern bool     BreakWrite( address addr, mad_type_handle th, const char *comment );
extern bool     BreakOnRawMemory( address addr, const char *comment, int size );
extern void     BreakOnExprSP( const char *comment );
extern void     PointFini( void );
extern void     CheckBPErrors( void );
extern void     BrkCmdError( void );
extern unsigned CheckBPs( unsigned conditions, unsigned run_conditions );
extern bool     UpdateWPs( void );
extern void     InsertWPs( void );
extern void     SaveBreaksToFile( const char *name );
extern void     RestoreBreaksFromFile( const char *name );
extern bool     FindFirstCue( mod_handle mod, cue_handle *ch );
extern void     BreakAllModEntries( mod_handle handle );
extern void     ClearAllModBreaks( mod_handle handle );
extern address  GetRowAddrDirectly( mod_handle mod, cue_fileid file_id, int row, bool exact );
extern brkp     *GetBPAtIndex( int index );
