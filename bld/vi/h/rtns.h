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


/* abandon.c */
void AbandonHopeAllYeWhoEnterHere( int );

/* addstr.c */
void AddString( char **, char * );
void AddString2( char **, char * );
void DeleteString( char ** );

/* alias.c */
int SetAlias( char * );
int CheckAlias( char *, char * );
bool CheckAbbrev( char *, int * );
int Abbrev( char * );
int UnAbbrev( char * );
int UnAlias( char * );

/* autosave.c */
void DoAutoSave( void );
void AutoSaveInit( void );
void AutoSaveFini( void );
void SetNextAutoSaveTime( void );
bool LostFileCheck( void );
void RemoveFromAutoSaveList( void );
void GetCurrentFilePath( char *path );

/* bnddata.c */
void CheckForBoundData( void );
bool SpecialOpen( char *, GENERIC_FILE * );
void SpecialFclose( GENERIC_FILE * );
int SpecialFgets( char *, int, GENERIC_FILE * );

/* change.c */
int DoSubstitute( void **, void ** );
int DoLineSubstitute( void **, void ** );
int DoChangeLineEnd( void **, void ** );

/* cledit.c */
int EditFile( char *, int );
int EditFileFromList( void );
int OpenWindowOnFile( char *data );

/* clglob.c */
int Global( linenum, linenum, char *, int );
void ProcessingMessage( linenum );

/* clread.c */
int ReadAFile( linenum, char * );

/* clset.c */
int Set( char * );
char *GetASetVal( char *token );
int SettingSelected( char *item, char *value, int *winflag );

/* clsubs.c */
int TwoPartSubstitute( char *, char *, int, int );
int Substitute( linenum, linenum, char * );
linenum SplitUpLine( linenum );

/* cmdline.c */
void InitCommandLine( void );
int ProcessCommandLine( void );
int FancyProcessCommandLine( void );
int TryCompileableToken( int token, char *data, bool iscmdline, bool dmt );
int RunCommandLine( char * );
int ProcessWindow( int, char * );
int ReadCmdData( void );
void FreeCmdData( void );
void FiniCommandLine( void );

/* cstatus.c */
void GetModeString( char * );
int UpdateCurrentStatus( status_type st );

/* cut.c */
int Cut( linenum, int, linenum, int, int );

/* dat.c */
int ReadDataFile( char *, int *, char **, int **, bool );

/* delete.c */
int DoDeleteRegion( void **, void ** );
int DoDeleteLineEnd( void **, void ** );
int DoDeleteCharAtCursor( void **, void ** );
int DoDeleteCharBeforeCursor( void **, void ** );
int DeleteSelectedRegion( void );
int YankSelectedRegion( void );

/* op.c */
int LineLength( linenum );
int GetLineRange( range *, long, linenum );
int Delete( range * );
int DeleteLines( void );
int Yank( range * );
int YankLines( void );
int Change( range * );
int StartShove( range * );
int StartSuck( range * );
int ChangeCase( range * );
int Filter( range * );

/* dir.c */
void DirFini( void );
void GetCWD1( char ** );
void GetCWD2( char *, int );
int ChangeDirectory( char * );
int ConditionalChangeDirectory( char * );
void FormatDirToFile( file *cfile, bool add_drives );

/* dirdisp.c */
int StartFileComplete( char *, int, int, int );
int ContinueFileComplete( char *data, int start, int max, int what );
void PauseFileComplete( void );
void FinishFileComplete( void );

/* dosdir.c */
int MyGetFileSize( char *, long * );
void FormatFileEntry( direct_ent *file, char *res );
int IsDirectory( char *name );
void GetFileInfo( direct_ent *tmp, struct dirent *nd, char *path );

/* dotmode.c */
int DoDotMode( void );
int DoAltDotMode( void );
void SaveDotCmd( void );

/* editdc.c */
int DeleteBlockFromCurrentLine( int, int, int );
int DeleteRangeOnCurrentLine( int, int, int );

/* editins.c */
int InsertTextAtCursor( void );
int InsertTextAfterCursor( void );
int InsertTextAtLineEnd( void );
int InsertTextAtLineStart( void );
int InsertTextOnNextLine( void );
int DeleteAndInsertText( int, int );
int DeleteAndInsertTextString( int, int, char *);
int InsertTextOnPreviousLine( void );
int DoReplaceText( void );
int InsertLikeLast( void );
int IMChar( void );
int IMEsc( void );
int IMEnter( void );
int IMBackSpace( void );
int IMBackSpaceML( void );
int IMMouseEvent( void );
int IMCursorKey( void );
int IMMenuKey( void );
int IMSpace( void );
int IMTabs( void );
int IMEscapeNextChar( void );
int IMInsert( void );
int IMDelete( void );
int IMDeleteML( void );
int IMCloseBracket( void );
int IMCloseBrace( void );
void PushMode( void );
int PopMode( void );
void UpdateEditStatus( void );
void DoneCurrentInsert( bool );

/* editmain.c */
int DoMove( event *event );
int DoLastEvent( void );
void DoneLastEvent( int rc, bool is_dotmode );
void EditMain( void );
int AbsoluteNullResponse( void );
int NullResponse( void );
long GetRepeatCount( void );
void SetRepeatCount( long );
int DoDigit( void );
void DoneRepeat( void );
void KillRepeatWindow( void );
int InvalidKey( void );
void SetModifiedVar( bool val );
void Modified( bool );
void ResetDisplayLine( void );

/* editmv.c */
int MovePage( int, long, bool );
int MovePageUp( void );
int MovePageDown( void );
int MoveScreenML( linenum );
int MoveScreenDown( void );
int MoveScreenDownPageML( void );
int MoveScreenUp( void );
int MoveScreenUpPageML( void );
int MovePosition( void );
int MoveHalfPageUp( void );
int MoveHalfPageDown( void );
int MoveScreenLeftPageML( void );
int MoveScreenRightPageML( void );
int MoveScreenLeftRightML( int );

/* -------- new operators ------------- */
int MoveForwardWord( range *, long );
int MoveForwardBigWord( range *, long );
int MoveForwardWordEnd( range *, long );
int MoveForwardBigWordEnd( range *, long );
int MoveBackwardsWord( range *, long );
int MoveBackwardsBigWord( range *, long );
int MoveToLastCharFind( range *, long );
int MoveToLastCharFindRev( range *, long );
int MoveUpToBeforeChar( range *, long );
int MoveUpToChar( range *, long );
int MoveBackToAfterChar( range *, long );
int MoveBackToChar( range *, long );
int DoGo( range *, long );
int MoveStartOfFile( range *, long );
int MoveEndOfFile( range *, long );
int MoveStartNextLine( range *, long );
int MoveStartPrevLine( range *, long );
int MoveTab( range *, long );
int MoveShiftTab( range *, long );
int MoveLeft( range *, long );
int MoveRight( range *, long );
int MoveLineEnd( range *, long );
int MoveLineBegin( range *, long );
int MoveStartOfLine( range *, long );
int MoveToColumn( range *, long );
int MoveUp( range *, long );
int MoveDown( range *, long );
int MovePageTop( range *, long );
int MovePageMiddle( range *, long );
int MovePageBottom( range *, long );
int MoveTopOfPage( range *, long );
int MoveBottomOfPage( range *, long );

/* error.c */
void FatalError( int );
void Die( const char *, ... );
char *GetErrorMsg( int );
void Error( char *, ... );
void FreeErrorMsgData( void );
void ErrorFini( void );

/* expandfn.c */
int ExpandFileNames( char *, char *** );

/* fcb.c */
int ReadFcbData( file * );
void CreateFcbData( file *, int );
int FcbSize( fcb * );

/* fcb2.c */
int FindFcbWithLine( linenum, file *, fcb ** );

/* fcb3.c */
int CMergeFcbs( fcb *, fcb * );
int CMergeAllFcbs( void );
int JoinFcbs( fcb *, fcb * );

/* fcbdmp.c */
int WalkUndo( void );
int HeapCheck( void );
int FcbDump( void );
int FcbThreadDump( void );
int LineInfo( void );
int SanityCheck( void );

/* fcbdup.c */
void CreateDuplicateFcbList( fcb *, fcb **, fcb ** );

/* fcbmem.c */
fcb *FcbAlloc( file * );
void FcbFree( fcb * );
void FreeEntireFcb( fcb * );
void FreeFcbList( fcb * );

/* fcbsplit.c */
int SplitFcbAtLine( linenum, file *, fcb * );
int CheckCurrentFcbCapacity( void );

/* fcbswap.c */
void FetchFcb( fcb * );
void SwapBlockFini( void );

/* fgrep.c */
int DoFGREP( char *, char *, bool );
int DoEGREP( char *, char * );

/* file.c */
void SaveInfo( info * );
void SaveCurrentInfo();
bool RestoreInfo( info * );
int DisplayFileStatus( void );
void CTurnOffFileDisplayBits( void );
bool CFileReadOnly( void );
void FileIOMessage( char *, linenum, long );
bool IsTextFile( char * );
int GimmeFileCount( void );

/* filemove.c */
int NextFile( void );
int NextFileDammit( void );
int RotateFileForward( void );
int RotateFileBackwards( void );
int GotoFile( window_id );
void BringUpFile( info *, bool );

/* filelast.c */
void UpdateLastFileList( char *fname );
char *GetFileInLastFileList( int num );

/* filenew.c */
int NewFile( char *, bool );
file *FileAlloc( char * );
void FileFree( file * );
void FreeEntireFile( file * );

/* filesave.c */
#ifdef __WIN__
int SaveFileAs( void );
#endif
int SaveFile( char *, linenum, linenum, int );
int StartSaveExit( void );
int SaveAndExit( char *fname );
bool FileExitOptionSaveChanges( file * );
bool FilePromptForSaveChanges( file *f );

/* filesel.c */
int SelectFileOpen( char *, char **, char *, bool );
int SelectFileSave( char * );
int SelectLineInFile( selflinedata *sfd );

/* filestk.c */
void InitFileStack( void );
int PushFileStack( void );
int PushFileStackAndMsg( void );
int PopFileStack( void );
void FiniFileStack( void );

/* filter.c */
int DoGenericFilter( linenum, linenum, char * );

/* findcmd.c */
void HilightSearchString( linenum, int, int );
void ResetLastFind( void );
int FindForwardWithString( char * );
int GetFind( char *, linenum *, int *, int *, int );
int FindBackwardsWithString( char * );
int GetFindString( range *, char *, int ( *)() );
void SaveFindRowColumn( void );
int ColorFind( char *, int );
void FindCmdFini( void );

int DoFindBackwards( range *, long );
int DoNextFindBackwards( range *, long );
int DoFindForward( range *, long );
int FancyDoFind( range *, long );
int FancyDoFindMisc( void );
int FancyDoReplace( void );
int DoNextFindForward( range *, long );
int DoNextFindForwardMisc( void );
int DoNextFindBackwardsMisc( void );
int ToggleToolbar( void );
int ToggleStatusbar( void );
int ToggleColorbar( void );
int ToggleFontbar( void );
int ToggleSSbar( void );

/* fini.c */
void Quit( const char **, const char *, ... );
void ExitEditor( int );
void QuitEditor( int );
void Usage( char * );

/* fmatch.c */
int FileMatch( char *name );
int FileMatchInit( char *wild );
void FileMatchFini( void );

/* gencfg.c */
int GenerateConfiguration( char *fname, bool is_cmdline );

/* getautoi.c */
int GetAutoIndentAmount( char *, int, bool );

/* getdir.c */
int GetSortDir( char *, bool );

/* getspcmd.c */
void GetSpawnCommandLine( char *path, char *cmdl, cmd_struct *cmds );

/* help.c */
int DoHelp( char *data );
int DoHelpOnContext( void );

/* hide.c */
int DoHideCmd( void );
int HideLineRange( linenum s, linenum e, bool unhide );
void GetHiddenRange( linenum l, linenum *s, linenum *e );
linenum GetHiddenLineCount( linenum s, linenum e );
linenum GetHiddenLineBreaks( linenum s, linenum e );

/* hist.c */
void LoadHistory( char *cmd );
void SaveHistory( void );
void CLHistInit( int );
void FilterHistInit( int );
void FindHistInit( int );
void LastFilesHistInit( int );
void HistFini( void );

#ifdef __IDE__
/* ide.c */
void IDEInit( void );
void IDEFini( void );
void IDEGetKeys( void );
#endif

/* init.c */
void InitializeEditor( void );
void SetConfigFileName( char *fn );
char *GetConfigFileName();
void FiniCFName( void );

/* io.c */
int FileExists( char * );
int FileOpen( char *, int, int, int, int * );
int FileSeek( int, long );
FILE *GetFromEnvAndOpen( char * );
void GetFromEnv( char *, char * );
int TmpFileOpen( char *, int * );
void TmpFileClose( int, char * );
void FileLower( char *str );
bool FileTemplateMatch( char *, char * );
char *StripPath( char * );
void VerifyTmpDir( void );
void MakeTmpPath( char *out, char *in );

/* key.c */
int GetVIKey( int ch, int scan, int shift );
int GetNextEvent( bool );
int GetKey( bool );
void ClearBreak( void );
int NonKeyboardEventsPending( void );
void KeyAdd( int ch );
void KeyAddString( char *str );
void AddCurrentMouseEvent( void );

/* linecfb.c */
bool CreateLinesFromBuffer( int, line **, line **, int *, int *, short * );

/* linedel.c */
void UpdateLineNumbers( linenum amt, fcb *cfcb  );
int DeleteLineRange( linenum, linenum, linedel_flags );
void LineDeleteMessage( linenum, linenum );

/* linedisp.c */
int ReDisplayScreen( void );
int ReDisplayBuffers( bool );
void DisplaySomeLines( int,int );
void DisplayAllLines( void  );

/* linefind.c */
int FindFirstCharInListForward( line *, char *, int  );
int FindFirstCharInListBackwards( line *, char *, int  );
int FindFirstCharNotInListForward( line *, char *, int  );
int FindFirstCharNotInListBackwards( line *, char *, int  );
int FindFirstCharInRangeForward( line *, char *, int );
int FindFirstCharInRangeBackwards( line *, char *, int );
int FindFirstCharNotInRangeForward( line *, char *, int );
int FindFirstCharNotInRangeBackward( line *, char *, int );
bool TestIfCharInRange( char, char * );
int FindCharOnCurrentLine( int, int, int *, int );
int FancyGotoLine( void );

/* lineins.c */
int InsertLines( linenum, fcb *, fcb *, undo_stack * );
int InsertLinesAtCursor( fcb *, fcb *, undo_stack * );

/* linemisc.c */
int FindStartOfCurrentLine( void );
int FindStartOfALine( line * );
int JoinCurrentLineToNext( void );
int GenericJoinCurrentLineToNext( bool remsp );
void SaveCurrentFilePos( void );
void RestoreCurrentFilePos( void );
int SaveAndResetFilePos( linenum );

/* lineptr.c */
int CGimmeLinePtr( linenum, fcb **, line ** );
int CGimmeNextLinePtr( fcb **, line ** );
int GimmeLinePtr( linenum, file *, fcb **, line ** );
int GimmeNextLinePtr( file *, fcb **, line ** );
int GimmePrevLinePtr( fcb **, line ** );
int GimmeLinePtrFromFcb( linenum, fcb *, line ** );
int CAdvanceToLine( linenum l );
int CFindLastLine( linenum * );
int IsPastLastLine( linenum l );
int ValidateCurrentLine( void );

/* linenew.c */
void AddNewLineAroundCurrent( char *, int, insert_dir );
void InsertNewLine( line *, line **, line **, char *, int, insert_dir );
void CreateNullLine( fcb * );
line *LineAlloc( char *, int );

/* linenum.c */
int LineNumbersSetup( void );

/* linework.c */
void GetCurrentLine( void );
int ReplaceCurrentLine( void );
void DisplayWorkLine( bool );

/* lineyank.c */
int YankLineRange( linenum, linenum );
int GetCopyOfLineRange( linenum, linenum, fcb **, fcb ** );
void LineYankMessage( linenum, linenum );

/* llrtns.c */
void AddLLItemAtEnd( void *, void *, void * );
void InsertLLItemAfter( void *, void *, void * );
void InsertLLItemBefore( void *, void *, void * );
void *DeleteLLItem( void *, void *, void * );
void ReplaceLLItem( void *, void *, void *, void * );
bool ValidateLL( void *, void * );

/* mapkey.c */
int MapKey( int flag, char *data );
int DoKeyMap( int );
void DoneInputKeyMap( void );
int StartInputKeyMap( int );
int RunKeyMap( key_map *, long );
int AddKeyMap( key_map *, char *, int );
void InitKeyMaps( void );
int ExecuteBuffer( void );
char *LookUpCharToken( char ch, bool want_single );
void FiniKeyMaps( void );

/* mark.c */
int SetMark( void );
int SetGenericMark( linenum, int, char );
int GoMark( range *, long );
int GoMarkLine( range *, long );
int GetMark( linenum *, int * );
int GetMarkLine( linenum * );
int VerifyMark( int, int );
void AllocateMarkList( void );
void FreeMarkList( void );
void MemorizeCurrentContext( void );
void SetMarkContext( void );

/* match.c */
int DoMatching( range *, long count );
int FindMatch( linenum *, int * );
int AddMatchString( char * );
void MatchFini( void );

/* mem.c */
void *MemAlloc( unsigned );
void *MemAllocUnsafe( unsigned );
void MemFree( void * );
void MemFreeList( int, void ** );
void MemFree2( void ** );
void *MemReAlloc( void *, unsigned );
void *MemReAllocUnsafe( void *ptr, unsigned size );
char *StaticAlloc( void );
void StaticFree( char * );
void StaticStart( void );
void StaticFini( void );
char *MemStrDup( char * );
int DumpMemory( void );
#ifdef TRMEM
    void InitTRMEM();
    void DumpTRMEM();
#endif

/* misc.c */
int ExecCmd( char *, char *, char * );
int GetResponse( char *, char * );
void ExitWithVerify( void );
bool ExitWithPrompt( bool );
bool PromptFilesForSave();
bool PromptThisFileForSave( const char * );
bool QueryFile( const char * );
int PrintHexValue( void );
int EnterHexKey( void );
int DoVersion( void );
char *StrMerge( int, char *, ... );
int ModificationTest( void );
void UpdateCurrentDirectory( void );
int DoAboutBox( void );
int CurFileExitOptionSaveChanges( void );
int NextBiggestPrime( int );
int FancySetFS( void );
int FancySetScr( void );
int FancySetGen( void );
int GenericQueryBool( char *str );

/* move.c */
int GoToLineRelCurs( linenum );
int GoToLineNoRelCurs( linenum );
int GoToColumn( int, int );
int NewColumn( int );
int GoToColumnOK( int );
int GoToColumnOnCurrentLine( int );
int SetCurrentLine( linenum );
void SetCurrentLineNumber( linenum );
int CheckLeftColumn( void );
void ValidateCurrentColumn( void );
int CheckCurrentColumn( void );
int ShiftTab( int, int );
int SetCurrentColumn( int );
int LocateCmd( char * );

/* parse.c */
void RemoveLeadingSpaces( char *);
void TranslateTabs( char * );
int GetStringWithPossibleSlash( char *, char * );
int GetStringWithPossibleQuote( char *, char * );
int GetStringWithPossibleQuote2( char *, char *, bool );
int NextWord( char *, char *, char *);
int NextWordSlash( char *, char * );
int NextWord1( char *, char * );
void EliminateFirstN( char *, int );
int Tokenize( char *, char *, bool );
int GetLongestTokenLength( char * );
int GetNumberOfTokens( char * );
char **BuildTokenList( int, char * );
char *GetTokenString( char *, int );
int AddColorToken( char *);
int ReplaceSubString( char *, int, int, int, char *, int );
void GetSubString( char *, int, int, char * );
void GetEndString( char *data, char *res );

/* parsecfg.c */
void ParseConfigFile( char * );

/* parsecl.c */
int ParseCommandLine( char *, linenum *, int *, linenum *, int *, int *, char *, int * );
int GetAddress( char *, linenum * );

/* printf.c */
void MyVSprintf( char *, const char *, __va_list );
void MySprintf( char *, const char *, ... );
void MyPrintf( const char *, ... );
void MyVPrintf( const char *, __va_list );
void MyFprintf( FILE *,const char *, ... );

/* readstr.c */
bool ReadStringInWindow( window_id, int, char *, char *, int, history_data * );
int PromptForString( char *prompt, char *buff, int maxbuff, history_data *hist );
bool GetTextForSpecialKey( int str_max, int event, char *tmp );
void InsertTextForSpecialKey( int event, char *buff );

/* replace.c */
int ReplaceChar( void );

/* savebuf.c */
int InsertSavebufBefore( void );
int InsertSavebufAfter( void );
int InsertSavebufBefore2( void );
int InsertSavebufAfter2( void );
int InsertGenericSavebuf( int, int );
void InitSavebufs( void );
void AddLineToSavebuf( char *, int, int );
void AddSelRgnToSavebuf( void );
void AddSelRgnToSavebufAndDelete( void );
void AddFcbsToSavebuf( fcb *, fcb *, int );
int SwitchSavebuf( void );
int DoSavebufNumber( void );
int SetSavebufNumber( char * );
int GetSavebufString( char ** );
bool IsEmptySavebuf( char ch );
void FiniSavebufs( void );

/* select.c */
int SelectItem( selectitem *si );
int SelectItemAndValue( window_info *, char *, char **, int , int (*)(), int, char **, int );

/* selrgn.c */
void UpdateDrag( window_id, int, int );
void InitSelectedRegion( void );
void UnselectRegion( void );
void SetSelRegionCols( linenum sl, int sc, int ec );
void UpdateCursorDrag( void );
int ReselectRegion( void );
int SelectDown( void );
int SelectUp( void );
int SelectRight( range *r, long );
int SelectLeft( range *r, long );
int SelectHome( void );
int SelectEnd( void );
int SelectPageUp( void );
int SelectPageDown( void );
int SelectForwardWord( range *, long );
int SelectBackwardsWord( range *, long );
int SelectTopOfPage( range *, long );
int SelectBottomOfPage( range *, long );
int SelectStartOfFile( range *, long );
int SelectEndOfFile( range *, long );
int DoSelectSelection( bool );
int DoSelectSelectionPopMenu( void );
int GetSelectedRegion( range * );
int SetSelectedRegion( range * );
int SetSelectedRegionFromLine( range *, linenum );
void SelRgnInit( void );
void SelRgnFini( void );
void GetFixedSelectedRegion( select_rgn *);
void NormalizeRange( range * );

/* shove.c */
int Shift( linenum, linenum, char, bool );

/* sort.c */
int Sort( linenum, linenum, char * );

/* source.c */
int Source( char *, char *, int * );
void FileSPVAR( void );
void SourceError( char *msg );
void DeleteResidentScripts( void );

/* spawn.c */
int MySpawn( char * );
void ResetSpawnScreen( void );

/* srcvar.c */
void VarAddGlobal( char *, char * );
void VarFini( void );

/* status.c */
void UpdateStatusWindow( void );
int NewStatusWindow( void );

/* tab_hell.c */
int ExpandWhiteSpace( void );
int CompressWhiteSpace( void );
bool ExpandTabsInABufferUpToColumn( int, char *, int, char *, int );
bool ExpandTabsInABuffer( char *, int, char *, int );
int InsertTabSpace( int, char *, bool * );
int GetVirtualCursorPosition( char *, int );
int VirtualCursorPosition( void );
int VirtualCursorPosition2( int );
int RealCursorPosition( int );
int RealCursorPositionInString( char *, int );
int RealCursorPositionOnLine( linenum, int );
int WinRealCursorPosition( char *, int );
int WinVirtualCursorPosition( char *, int );
int RealLineLen( char * );
int AddLeadingTabSpace( short *, char *, int );
bool ConvertSpacesToTabsUpToColumn( int, char *, int, char *, int );

/* tags.c */
int GetCurrentTag( void );
int TagHunt( char * );
int FindTag( char * );
int LocateTag( char *, char *, char * );

/* time.c */
void GetTimeString( char *st );
void GetDateString( char *st );
void GetDateTimeString( char *st );

/* undo.c */
void StartUndoGroup( undo_stack * );
void StartUndoGroupWithPosition( undo_stack *stack, linenum lne, linenum top, int col );
int UndoReplaceLines( linenum, linenum );
void UndoDeleteFcbs( linenum, fcb *, fcb *, undo_stack * );
void UndoInsert( linenum, linenum, undo_stack * );
void PatchDeleteUndo( undo_stack * );
void EndUndoGroup( undo_stack * );
void TryEndUndoGroup( undo_stack *cstack );

/* undoclne.c */
void CurrentLineReplaceUndoStart( void );
void CurrentLineReplaceUndoCancel( void );
void ConditionalCurrentLineReplaceUndoEnd( void );
void CurrentLineReplaceUndoEnd( int );

/* undo_do.c */
int DoUndo( void );
int DoUndoUndo( void );

/* undostks.c */
undo *UndoAlloc( undo_stack *stack, int type );
void UndoFree( undo *cundo, int freefcbs );
void AddUndoToCurrent( undo *item, undo_stack *stack );
void PurgeUndoStack( undo_stack *stack );
bool TossUndos( void );
void AllocateUndoStacks( void );
void FreeUndoStacks( void );
void FreeAllUndos( void );
undo *PopUndoStack( undo_stack *stack );
void PushUndoStack( undo *item, undo_stack *stack );

/* wingen.c */
int DisplayExtraInfo( window_info *, window_id *, char  _NEAR * _NEAR *, int );
int NewMessageWindow( void );
int NewWindow2( window_id *, window_info * );
void Message1( char *, ... );
void Message2( char *, ... );
int WPrintfLine( window_id, int, char *, ... );
bool ColumnInWindow( int, int * );
void SetWindowSizes( void );
void SetWindowCursor( void );
void SetWindowCursorForReal( void );
int CurrentWindowResize( int x1, int y1, int x2, int y2 );
void SetFileWindowTitle( window_id cw, info *cinfo, bool hilite );
void ResetAllWindows( void );

/* word.c */
void InitWordSearch( char *regword );
int FindColumnOfNextWordForward( line *, int *, bool, bool );
int FindColumnOfNextWordBackwards( line *, int *, bool, bool );
int GimmeCurrentWord( char *, int, bool );
int GetWordBound( line *, int, bool, int *, int * );
int GimmeCurrentWord( char *, int, bool );
int GimmeCurrentEntireWordDim( int *sc, int *ec, bool big );
int MarkStartOfNextWordForward( i_mark *, i_mark *, bool );
int MarkEndOfNextWordForward( i_mark *, i_mark *, bool );
int MarkStartOfNextWordBackward( i_mark *, i_mark *, bool );
