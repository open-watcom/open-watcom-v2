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
* Description:  Prototypes of internal editor functions.
*
****************************************************************************/


/* abandon.c */
void    AbandonHopeAllYeWhoEnterHere( vi_rc );

/* addstr.c */
void    AddString( char **, const char * );
void    AddString2( char **, const char * );
void    DeleteString( char ** );

/* alias.c */
vi_rc   SetAlias( char * );
vi_rc   CheckAlias( char *, char * );
bool    CheckAbbrev( char *, int * );
vi_rc   Abbrev( char * );
vi_rc   UnAbbrev( char * );
vi_rc   UnAlias( char * );

/* autosave.c */
void    DoAutoSave( void );
void    AutoSaveInit( void );
void    AutoSaveFini( void );
void    SetNextAutoSaveTime( void );
bool    LostFileCheck( void );
void    RemoveFromAutoSaveList( void );
void    GetCurrentFilePath( char *path );

/* bnddata.c */
void    BoundDataInit( void );
void    BoundDataFini( void );

/* change.c */
vi_rc   DoSubstitute( event **, event ** );
vi_rc   DoLineSubstitute( event **, event ** );
vi_rc   DoChangeLineEnd( event **, event ** );

/* cledit.c */
vi_rc   EditFile( char *, bool );
vi_rc   EditFileFromList( void );
vi_rc   OpenWindowOnFile( char *data );

/* clglob.c */
vi_rc   Global( linenum, linenum, char *, int );
void    ProcessingMessage( linenum );

/* clread.c */
vi_rc   ReadAFile( linenum, char * );

/* clset.c */
vi_rc   Set( char * );
char    *GetASetVal( char *token );
vi_rc   SettingSelected( char *item, char *value, int *winflag );

/* clsubs.c */
vi_rc   TwoPartSubstitute( char *, char *, int, int );
vi_rc   Substitute( linenum, linenum, char * );
linenum SplitUpLine( linenum );

/* cmdline.c */
void    InitCommandLine( void );
vi_rc   ProcessCommandLine( void );
vi_rc   FancyProcessCommandLine( void );
vi_rc   TryCompileableToken( int token, char *data, bool iscmdline );
vi_rc   RunCommandLine( char * );
vi_rc   ProcessWindow( int, char * );
void    FiniCommandLine( void );
void    EditRCSCurrentFile( void );

/* cstatus.c */
void    GetModeString( char * );
status_type UpdateCurrentStatus( status_type st );

/* cut.c */
vi_rc   Cut( linenum, int, linenum, int, bool );

/* dat.c */
#ifdef VICOMP
vi_rc   ReadDataFile( char *file, char **buffer, bool (*)( int ), bool (*)( int, char * ) );
#else
vi_rc   ReadDataFile( char *file, char **buffer, bool (*)( int ), bool (*)( int, char * ), bool );
#endif

/* delete.c */
vi_rc   DoDeleteRegion( event **, event ** );
vi_rc   DoDeleteLineEnd( event **, event ** );
vi_rc   DoDeleteCharAtCursor( event **, event ** );
vi_rc   DoDeleteCharBeforeCursor( event **, event ** );
vi_rc   DeleteSelectedRegion( void );
vi_rc   YankSelectedRegion( void );

/* op.c */
int     LineLength( linenum );
vi_rc   GetLineRange( range *, long, linenum );
vi_rc   Delete( range * );
vi_rc   DeleteLines( void );
vi_rc   Yank( range * );
vi_rc   YankLines( void );
vi_rc   Change( range * );
vi_rc   StartShove( range * );
vi_rc   StartSuck( range * );
vi_rc   ChangeCase( range * );
vi_rc   Filter( range * );

/* dir.c */
void    DirFini( void );
void    GetCWD1( char ** );
void    GetCWD2( char *, int );
vi_rc   ChangeDirectory( const char * );
vi_rc   ConditionalChangeDirectory( const char * );
void    FormatDirToFile( file *cfile, bool add_drives );

/* dirdisp.c */
vi_rc   StartFileComplete( char *, int, int, int );
vi_rc   ContinueFileComplete( char *data, int start, int max, int what );
void    PauseFileComplete( void );
void    FinishFileComplete( void );

/* dosdir.c */
struct dirent;

vi_rc   MyGetFileSize( const char *, long * );
void    FormatFileEntry( direct_ent *file, char *res );
bool    IsDirectory( char *name );
void    GetFileInfo( direct_ent *tmp, struct dirent *nd, char *path );

/* dotmode.c */
vi_rc   DoDotMode( void );
vi_rc   DoAltDotMode( void );
void    SaveDotCmd( void );

/* editdc.c */
vi_rc   DeleteBlockFromCurrentLine( int, int, bool );
vi_rc   DeleteRangeOnCurrentLine( int, int, bool );

/* editins.c */
vi_rc   InsertTextAtCursor( void );
vi_rc   InsertTextAfterCursor( void );
vi_rc   InsertTextAtLineEnd( void );
vi_rc   InsertTextAtLineStart( void );
vi_rc   InsertTextOnNextLine( void );
vi_rc   DeleteAndInsertText( int, int );
vi_rc   DeleteAndInsertTextString( int, int, char * );
vi_rc   InsertTextOnPreviousLine( void );
vi_rc   DoReplaceText( void );
vi_rc   InsertLikeLast( void );
vi_rc   IMChar( void );
vi_rc   IMEsc( void );
vi_rc   IMEnter( void );
vi_rc   IMBackSpace( void );
vi_rc   IMBackSpaceML( void );
vi_rc   IMMouseEvent( void );
vi_rc   IMCursorKey( void );
vi_rc   IMMenuKey( void );
vi_rc   IMSpace( void );
vi_rc   IMTabs( void );
vi_rc   IMEscapeNextChar( void );
vi_rc   IMInsert( void );
vi_rc   IMDelete( void );
vi_rc   IMDeleteML( void );
vi_rc   IMCloseBracket( void );
vi_rc   IMCloseBrace( void );
void    PushMode( void );
vi_rc   PopMode( void );
void    UpdateEditStatus( void );
void    DoneCurrentInsert( bool );

/* editmain.c */
vi_rc   DoMove( event *event );
vi_rc   DoLastEvent( void );
void    DoneLastEvent( vi_rc rc, bool is_dotmode );
void    EditMain( void );
vi_rc   AbsoluteNullResponse( void );
vi_rc   NullResponse( void );
long    GetRepeatCount( void );
void    SetRepeatCount( long );
vi_rc   DoDigit( void );
void    DoneRepeat( void );
void    KillRepeatWindow( void );
vi_rc   InvalidKey( void );
void    SetModifiedVar( bool val );
void    Modified( bool );
void    ResetDisplayLine( void );

/* editmv.c */
vi_rc MovePage( int, long, bool );
vi_rc MovePageUp( void );
vi_rc MovePageDown( void );
vi_rc MoveScreenML( linenum );
vi_rc MoveScreenDown( void );
vi_rc MoveScreenDownPageML( void );
vi_rc MoveScreenUp( void );
vi_rc MoveScreenUpPageML( void );
vi_rc MovePosition( void );
vi_rc MoveHalfPageUp( void );
vi_rc MoveHalfPageDown( void );
vi_rc MoveScreenLeftPageML( void );
vi_rc MoveScreenRightPageML( void );
vi_rc MoveScreenLeftRightML( int );

/* -------- new operators ------------- */
vi_rc MoveForwardWord( range *, long );
vi_rc MoveForwardBigWord( range *, long );
vi_rc MoveForwardWordEnd( range *, long );
vi_rc MoveForwardBigWordEnd( range *, long );
vi_rc MoveBackwardsWord( range *, long );
vi_rc MoveBackwardsBigWord( range *, long );
vi_rc MoveToLastCharFind( range *, long );
vi_rc MoveToLastCharFindRev( range *, long );
vi_rc MoveUpToBeforeChar( range *, long );
vi_rc MoveUpToChar( range *, long );
vi_rc MoveBackToAfterChar( range *, long );
vi_rc MoveBackToChar( range *, long );
vi_rc DoGo( range *, long );
vi_rc MoveStartOfFile( range *, long );
vi_rc MoveEndOfFile( range *, long );
vi_rc MoveStartNextLine( range *, long );
vi_rc MoveStartPrevLine( range *, long );
vi_rc MoveTab( range *, long );
vi_rc MoveShiftTab( range *, long );
vi_rc MoveLeft( range *, long );
vi_rc MoveRight( range *, long );
vi_rc MoveLineEnd( range *, long );
vi_rc MoveLineBegin( range *, long );
vi_rc MoveStartOfLine( range *, long );
vi_rc MoveToColumn( range *, long );
vi_rc MoveUp( range *, long );
vi_rc MoveDown( range *, long );
vi_rc MovePageTop( range *, long );
vi_rc MovePageMiddle( range *, long );
vi_rc MovePageBottom( range *, long );
vi_rc MoveTopOfPage( range *, long );
vi_rc MoveBottomOfPage( range *, long );

/* error.c */
void    StartupError( vi_rc );
void    FatalError( vi_rc );
void    Die( const char *, ... );
char    *GetErrorMsg( vi_rc );
void    Error( char *, ... );
void    ErrorBox( char *, ... );
void    FreeErrorMsgData( void );
void    ErrorFini( void );

/* expandfn.c */
int ExpandFileNames( char *, char *** );

/* fcb.c */
vi_rc   OpenFcbData( file * );
vi_rc   ReadFcbData( file *, bool * );
void    CreateFcbData( file *, int );
int     FcbSize( fcb * );

/* fcb2.c */
vi_rc FindFcbWithLine( linenum, file *, fcb ** );

/* fcb3.c */
vi_rc MergeFcbs( fcb_list *, fcb *, fcb * );
vi_rc MergeAllFcbs( fcb_list * );

/* fcbdmp.c */
vi_rc WalkUndo( void );
vi_rc HeapCheck( void );
vi_rc FcbDump( void );
vi_rc FcbThreadDump( void );
vi_rc LineInfo( void );
vi_rc SanityCheck( void );

/* fcbdup.c */
void    CreateDuplicateFcbList( fcb *, fcb_list * );

/* fcbmem.c */
fcb     *FcbAlloc( file * );
void    FcbFree( fcb * );
void    FreeEntireFcb( fcb * );
void    FreeFcbList( fcb * );

/* fcbsplit.c */
vi_rc SplitFcbAtLine( linenum, file *, fcb * );
vi_rc CheckCurrentFcbCapacity( void );

/* fcbswap.c */
void    FetchFcb( fcb * );
void    SwapBlockFini( void );

/* fgrep.c */
vi_rc DoFGREP( const char *, const char *, bool );
vi_rc DoEGREP( const char *, const char * );

/* file.c */
void    SaveInfo( info * );
void    SaveCurrentInfo( void );
bool    RestoreInfo( info * );
vi_rc   DisplayFileStatus( void );
void    CTurnOffFileDisplayBits( void );
bool    CFileReadOnly( void );
void    FileIOMessage( char *, linenum, long );
bool    IsTextFile( char * );
int     GimmeFileCount( void );

/* filemove.c */
vi_rc   NextFile( void );
vi_rc   NextFileDammit( void );
vi_rc   RotateFileForward( void );
vi_rc   RotateFileBackwards( void );
vi_rc   GotoFile( window_id );
void    BringUpFile( info *, bool );

/* filelast.c */
void    UpdateLastFileList( char *fname );
char    *GetFileInLastFileList( int num );

/* filenew.c */
vi_rc   NewFile( const char *, bool );
file    *FileAlloc( const char * );
void    FileFree( file * );
void    FreeEntireFile( file * );

/* filesave.c */
#ifdef __WIN__
vi_rc   SaveFileAs( void );
#endif
vi_rc   SaveFile( char *, linenum, linenum, bool );
vi_rc   StartSaveExit( void );
vi_rc   SaveAndExit( char *fname );
bool    FileExitOptionSaveChanges( file * );
bool    FilePromptForSaveChanges( file *f );
vi_rc   FancyFileSave( void );
vi_rc   DoKeyboardSave( void );

/* filesel.c */
vi_rc SelectFileOpen( const char *, char **, const char *, bool );
vi_rc SelectFileSave( char * );
vi_rc SelectLineInFile( selflinedata *sfd );

/* filestk.c */
void    InitFileStack( void );
vi_rc   PushFileStack( void );
vi_rc   PushFileStackAndMsg( void );
vi_rc   PopFileStack( void );
void    FiniFileStack( void );

/* filter.c */
vi_rc DoGenericFilter( linenum, linenum, char * );

/* findcmd.c */
void    HilightSearchString( i_mark *, int );
void    ResetLastFind( info * );
vi_rc   FindForwardWithString( char * );
vi_rc   GetFind( char *, i_mark *, int *, find_type );
vi_rc   FindBackwardsWithString( char * );
void    SaveFindRowColumn( void );
vi_rc   ColorFind( const char *, find_type );
void    SetLastFind( const char * );
void    FindCmdFini( void );
void    JumpTo( i_mark * );

vi_rc DoFindBackwards( range *, long );
vi_rc DoNextFindBackwards( range *, long );
vi_rc DoFindForward( range *, long );
vi_rc FancyDoFind( range *, long );
vi_rc FancyDoFindMisc( void );
vi_rc FancyDoReplace( void );
vi_rc DoNextFindForward( range *, long );
vi_rc DoNextFindForwardMisc( void );
vi_rc DoNextFindBackwardsMisc( void );
vi_rc ToggleToolbar( void );
vi_rc ToggleStatusbar( void );
vi_rc ToggleColorbar( void );
vi_rc ToggleFontbar( void );
vi_rc ToggleSSbar( void );

/* fini.c */
#ifdef __WATCOMC__
#pragma aux Quit aborts;
#endif
void    Quit( const char **, const char *, ... );
void    ExitEditor( int );
void    QuitEditor( vi_rc );
void    Usage( char * );

/* fmatch.c */
bool    FileMatch( const char *name );
vi_rc   FileMatchInit( const char *wild );
void    FileMatchFini( void );

/* gencfg.c */
vi_rc   GenerateConfiguration( char *fname, bool is_cmdline );

/* getautoi.c */
int     GetAutoIndentAmount( char *, int, bool );

/* getdir.c */
vi_rc GetSortDir( const char *, bool );

/* getspcmd.c */
void    GetSpawnCommandLine( char *path, const char *cmdl, cmd_struct *cmds );

/* help.c */
vi_rc DoHelp( char *data );
vi_rc DoHelpOnContext( void );

/* hide.c */
vi_rc   DoHideCmd( void );
vi_rc   HideLineRange( linenum s, linenum e, bool unhide );
void    GetHiddenRange( linenum l, linenum *s, linenum *e );
linenum GetHiddenLineCount( linenum s, linenum e );
linenum GetHiddenLineBreaks( linenum s, linenum e );

/* hist.c */
void    LoadHistory( const char *cmd );
void    SaveHistory( void );
void    CLHistInit( int );
void    FilterHistInit( int );
void    FindHistInit( int );
void    LastFilesHistInit( int );
void    HistFini( void );

#ifdef __IDE__
/* ide.c */
void    IDEInit( void );
void    IDEFini( void );
void    IDEGetKeys( void );
#endif

/* init.c */
void    InitializeEditor( void );
void    SetConfigFileName( const char *fn );
char    *GetConfigFileName( void );
void    FiniConfigFileName( void );

/* io.c */
vi_rc   FileExists( const char * );
vi_rc   FileOpen( const char *, bool, int, int, int * );
vi_rc   FileSeek( int, long );
FILE    *GetFromEnvAndOpen( const char * );
void    GetFromEnv( const char *, char * );
vi_rc   TmpFileOpen( char *, int * );
void    TmpFileClose( int, const char * );
void    FileLower( char *str );
bool    FileTemplateMatch( const char *, const char * );
char    *StripPath( const char * );
void    VerifyTmpDir( void );
char    *MakeTmpPath( char *out, const char *in );

/* key.c */
vi_key  GetVIKey( vi_key ch, int scan, bool shift );
vi_key  GetNextEvent( bool );
vi_key  GetKey( bool );
void    ClearBreak( void );
bool    NonKeyboardEventsPending( void );
void    KeyAdd( vi_key ch );
void    KeyAddString( char *str );
void    AddCurrentMouseEvent( void );

/* linecfb.c */
bool    CreateLinesFromBuffer( int, line_list *, int *, int *, short * );
bool    CreateLinesFromFileBuffer( int, line_list *, int *, int *, short *, bool * );

/* linedel.c */
void    UpdateLineNumbers( linenum amt, fcb *cfcb  );
vi_rc   DeleteLineRange( linenum, linenum, linedel_flags );
void    LineDeleteMessage( linenum, linenum );

/* linedisp.c */
vi_rc   ReDisplayScreen( void );
vi_rc   ReDisplayBuffers( bool );
void    DisplaySomeLines( int,int );
void    DisplayAllLines( void  );

/* linefind.c */
int     FindFirstCharInListForward( line *, char *, int  );
int     FindFirstCharInListBackwards( line *, char *, int  );
int     FindFirstCharNotInListForward( line *, char *, int  );
int     FindFirstCharNotInListBackwards( line *, char *, int  );
int     FindFirstCharInRangeForward( line *, char *, int );
int     FindFirstCharInRangeBackwards( line *, char *, int );
int     FindFirstCharNotInRangeForward( line *, char *, int );
int     FindFirstCharNotInRangeBackward( line *, char *, int );
bool    TestIfCharInRange( char, char * );
vi_rc   FindCharOnCurrentLine( bool, int, int *, int );
vi_rc   FancyGotoLine( void );

/* lineins.c */
vi_rc InsertLines( linenum, fcb_list *, undo_stack * );
vi_rc InsertLinesAtCursor( fcb_list *, undo_stack * );

/* linemisc.c */
int     FindStartOfCurrentLine( void );
int     FindStartOfALine( line * );
vi_rc   JoinCurrentLineToNext( void );
vi_rc   GenericJoinCurrentLineToNext( bool remsp );
void    SaveCurrentFilePos( void );
void    RestoreCurrentFilePos( void );
vi_rc   SaveAndResetFilePos( linenum );

/* lineptr.c */
vi_rc CGimmeLinePtr( linenum, fcb **, line ** );
vi_rc CGimmeNextLinePtr( fcb **, line ** );
vi_rc GimmeLinePtr( linenum, file *, fcb **, line ** );
vi_rc GimmeNextLinePtr( file *, fcb **, line ** );
vi_rc GimmePrevLinePtr( fcb **, line ** );
vi_rc GimmeLinePtrFromFcb( linenum, fcb *, line ** );
vi_rc CAdvanceToLine( linenum l );
vi_rc CFindLastLine( linenum * );
bool  IsPastLastLine( linenum l );
vi_rc ValidateCurrentLine( void );

/* linenew.c */
void    AddNewLineAroundCurrent( char *, int, insert_dir );
void    InsertNewLine( line *, line_list *, char *, int, insert_dir );
void    CreateNullLine( fcb * );
line    *LineAlloc( char *, int );

/* linenum.c */
vi_rc LineNumbersSetup( void );

/* linework.c */
void    GetCurrentLine( void );
vi_rc   ReplaceCurrentLine( void );
void    DisplayWorkLine( bool );

/* lineyank.c */
vi_rc   YankLineRange( linenum, linenum );
vi_rc   GetCopyOfLineRange( linenum, linenum, fcb_list * );
void    LineYankMessage( linenum, linenum );

/* llrtns.c */
void    AddLLItemAtEnd( ss **, ss **, ss * );
void    InsertLLItemAfter( ss **, ss *, ss * );
void    InsertLLItemBefore( ss **, ss *, ss * );
void    *DeleteLLItem( ss **, ss **, ss * );
void    ReplaceLLItem( ss **, ss **, ss *, ss * );
bool    ValidateLL( ss *, ss * );

/* mapkey.c */
vi_rc   MapKey( int flag, char *data );
vi_rc   DoKeyMap( vi_key );
void    DoneInputKeyMap( void );
vi_rc   StartInputKeyMap( vi_key );
vi_rc   RunKeyMap( key_map *, long );
vi_rc   AddKeyMap( key_map *, char * );
void    InitKeyMaps( void );
vi_rc   ExecuteBuffer( void );
char    *LookUpCharToken( vi_key ch, bool want_single );
void    FiniKeyMaps( void );

/* mark.c */
vi_rc   SetMark( void );
vi_rc   SetGenericMark( linenum, int, char );
vi_rc   GoMark( range *, long );
vi_rc   GoMarkLine( range *, long );
vi_rc   GetMark( linenum *, int * );
vi_rc   GetMarkLine( linenum * );
vi_rc   VerifyMark( unsigned, bool );
void    AllocateMarkList( void );
void    FreeMarkList( void );
void    MemorizeCurrentContext( void );
void    SetMarkContext( void );

/* match.c */
vi_rc   DoMatching( range *, long count );
vi_rc   FindMatch( i_mark * );
vi_rc   AddMatchString( char * );
void    MatchInit( void );
void    MatchFini( void );

/* mem.c */
void    *MemAlloc( unsigned );
void    *MemAllocUnsafe( unsigned );
void    MemFree( void * );
void    MemFreePtr( void ** );
void    MemFreeList( int, char ** );
void    *MemReAlloc( void *, unsigned );
void    *MemReAllocUnsafe( void *ptr, unsigned size );
void    *StaticAlloc( void );
void    StaticFree( char * );
void    StaticStart( void );
void    StaticFini( void );
char    *MemStrDup( char * );
vi_rc   DumpMemory( void );
void    InitMem( void );
void    FiniMem( void );

/* misc.c */
long    ExecCmd( char *, char *, const char * );
vi_rc   GetResponse( char *, char * );
void    ExitWithVerify( void );
bool    ExitWithPrompt( bool, bool );
bool    PromptFilesForSave( void );
bool    PromptThisFileForSave( const char * );
bool    QueryFile( const char * );
vi_rc   PrintHexValue( void );
vi_rc   EnterHexKey( void );
vi_rc   DoVersion( void );
char    *StrMerge( int, char *, ... );
vi_rc   ModificationTest( void );
void    UpdateCurrentDirectory( void );
vi_rc   DoAboutBox( void );
vi_rc   CurFileExitOptionSaveChanges( void );
int     NextBiggestPrime( int );
vi_rc   FancySetFS( void );
vi_rc   FancySetScr( void );
vi_rc   FancySetGen( void );
bool    GenericQueryBool( char *str );

/* move.c */
vi_rc   GoToLineRelCurs( linenum );
vi_rc   GoToLineNoRelCurs( linenum );
vi_rc   GoToColumn( int, int );
int     NewColumn( int );
vi_rc   GoToColumnOK( int );
vi_rc   GoToColumnOnCurrentLine( int );
vi_rc   SetCurrentLine( linenum );
void    SetCurrentLineNumber( linenum );
bool    CheckLeftColumn( void );
void    ValidateCurrentColumn( void );
bool    CheckCurrentColumn( void );
int     ShiftTab( int, int );
vi_rc   SetCurrentColumn( int );
vi_rc   LocateCmd( char * );

/* parse.c */
void    RemoveLeadingSpaces( char * );
char    *SkipLeadingSpaces( const char * );
void    TranslateTabs( char * );
vi_rc   GetStringWithPossibleSlash( char *, char * );
vi_rc   GetStringWithPossibleQuote( char *, char * );
vi_rc   GetStringWithPossibleQuote2( char *, char *, bool );
int     NextWord( char *, char *, const char *);
char    *GetNextWord( const char *, char *, const char *);
int     NextWordSlash( char *, char * );
int     NextWord1( char *, char * );
char    *GetNextWord1( const char *, char * );
void    EliminateFirstN( char *, int );
int     Tokenize( const char *, const char *, bool );
int     GetLongestTokenLength( const char * );
int     GetNumberOfTokens( const char * );
char    **BuildTokenList( int, char * );
char    *GetTokenString( const char *, int );
char    *GetTokenStringCVT( const char *, int, char *, bool );
char    *ExpandTokenSet( char *token_no, char *buff );
int     AddColorToken( char *);
int     ReplaceSubString( char *, int, int, int, char *, int );
void    GetSubString( char *, int, int, char * );
void    GetEndString( char *data, char *res );

/* parsecfg.c */
void    ParseConfigFile( char * );

/* parsecl.c */
vi_rc ParseCommandLine( char *, linenum *, bool *, linenum *, bool *, int *, char * );
vi_rc GetAddress( char *, linenum * );

/* printf.c */
void    MySprintf( char *, const char *, ... );
void    MyPrintf( const char *, ... );
void    MyFprintf( FILE *,const char *, ... );

/* readstr.c */
bool    ReadStringInWindow( window_id, int, char *, char *, int, history_data * );
vi_rc   PromptForString( char *prompt, char *buff, int maxbuff, history_data *hist );
bool    GetTextForSpecialKey( int str_max, vi_key event, char *tmp );
void    InsertTextForSpecialKey( vi_key event, char *buff );

/* replace.c */
vi_rc ReplaceChar( void );

/* savebuf.c */
vi_rc   InsertSavebufBefore( void );
vi_rc   InsertSavebufAfter( void );
vi_rc   InsertSavebufBefore2( void );
vi_rc   InsertSavebufAfter2( void );
vi_rc   InsertGenericSavebuf( int, int );
void    InitSavebufs( void );
void    AddLineToSavebuf( char *, int, int );
vi_rc   AddSelRgnToSavebuf( void );
vi_rc   AddSelRgnToSavebufAndDelete( void );
void    AddFcbsToSavebuf( fcb_list *, bool );
vi_rc   SwitchSavebuf( void );
vi_rc   DoSavebufNumber( void );
vi_rc   SetSavebufNumber( char * );
vi_rc   GetSavebufString( char ** );
bool    IsEmptySavebuf( char ch );
void    FiniSavebufs( void );

/* select.c */
vi_rc SelectItem( selectitem *si );
vi_rc SelectItemAndValue( window_info *, char *, char **, int , vi_rc (*)(char *, char *, int * ), int, char **, int );

/* selrgn.c */
void    UpdateDrag( window_id, int, int );
void    InitSelectedRegion( void );
void    UnselectRegion( void );
void    SetSelRegionCols( linenum sl, int sc, int ec );
void    UpdateCursorDrag( void );
vi_rc   ReselectRegion( void );
vi_rc   SelectDown( void );
vi_rc   SelectUp( void );
vi_rc   SelectRight( range *r, long );
vi_rc   SelectLeft( range *r, long );
vi_rc   SelectHome( void );
vi_rc   SelectEnd( void );
vi_rc   SelectPageUp( void );
vi_rc   SelectPageDown( void );
vi_rc   SelectForwardWord( range *, long );
vi_rc   SelectBackwardsWord( range *, long );
vi_rc   SelectTopOfPage( range *, long );
vi_rc   SelectBottomOfPage( range *, long );
vi_rc   SelectStartOfFile( range *, long );
vi_rc   SelectEndOfFile( range *, long );
vi_rc   DoSelectSelection( bool );
vi_rc   DoSelectSelectionPopMenu( void );
vi_rc   GetSelectedRegion( range * );
vi_rc   SetSelectedRegion( range * );
vi_rc   SetSelectedRegionFromLine( range *, linenum );
void    SelRgnInit( void );
void    SelRgnFini( void );
void    GetFixedSelectedRegion( select_rgn *);
void    NormalizeRange( range * );
vi_rc   SelectAll( void );

/* shove.c */
vi_rc Shift( linenum, linenum, char, bool );

/* source.c */
vi_rc   Source( const char *, char *, unsigned * );
void    FileSPVAR( void );
void    SourceError( char *msg );
void    DeleteResidentScripts( void );

/* spawn.c */
long    MySpawn( const char * );
void    ResetSpawnScreen( void );

/* srcvar.c */
void    VarAddGlobalStr( const char *, const char * );
void    VarFini( void );

/* status.c */
void    UpdateStatusWindow( void );
vi_rc   NewStatusWindow( void );

/* tab_hell.c */
vi_rc   ExpandWhiteSpace( void );
vi_rc   CompressWhiteSpace( void );
bool    ExpandTabsInABufferUpToColumn( int, char *, int, char *, int );
bool    ExpandTabsInABuffer( char *, int, char *, int );
int     InsertTabSpace( int, char *, bool * );
int     GetVirtualCursorPosition( char *, int );
int     VirtualColumnOnCurrentLine( int );
int     RealColumnOnCurrentLine( int );
int     RealCursorPositionInString( char *, int );
int     RealCursorPositionOnLine( linenum, int );
int     WinRealCursorPosition( char *, int );
int     WinVirtualCursorPosition( char *, int );
int     VirtualLineLen( char * );
bool    AddLeadingTabSpace( short *, char *, int );
bool    ConvertSpacesToTabsUpToColumn( int, char *, int, char *, int );

/* tags.c */
vi_rc GetCurrentTag( void );
vi_rc TagHunt( const char * );
vi_rc FindTag( const char * );
vi_rc LocateTag( const char *, char *, char * );

/* time.c */
void    GetTimeString( char *st );
void    GetDateString( char *st );
void    GetDateTimeString( char *st );

/* undo.c */
void    StartUndoGroup( undo_stack * );
void    StartUndoGroupWithPosition( undo_stack *stack, linenum lne, linenum top, int col );
vi_rc   UndoReplaceLines( linenum, linenum );
void    UndoDeleteFcbs( linenum, fcb_list *, undo_stack * );
void    UndoInsert( linenum, linenum, undo_stack * );
void    PatchDeleteUndo( undo_stack * );
void    EndUndoGroup( undo_stack * );
void    TryEndUndoGroup( undo_stack *cstack );

/* undoclne.c */
void    CurrentLineReplaceUndoStart( void );
void    CurrentLineReplaceUndoCancel( void );
void    ConditionalCurrentLineReplaceUndoEnd( void );
void    CurrentLineReplaceUndoEnd( bool );

/* undo_do.c */
vi_rc DoUndo( void );
vi_rc DoUndoUndo( void );

/* undostks.c */
undo    *UndoAlloc( undo_stack *stack, int type );
void    UndoFree( undo *cundo, bool freefcbs );
void    AddUndoToCurrent( undo *item, undo_stack *stack );
void    PurgeUndoStack( undo_stack *stack );
bool    TossUndos( void );
void    AllocateUndoStacks( void );
void    FreeUndoStacks( void );
void    FreeAllUndos( void );
undo    *PopUndoStack( undo_stack *stack );
void    PushUndoStack( undo *item, undo_stack *stack );

/* wingen.c */
vi_rc   DisplayExtraInfo( window_info *, window_id *, char  _NEAR * _NEAR *, int );
vi_rc   NewMessageWindow( void );
vi_rc   NewWindow2( window_id *, window_info * );
void    Message1( char *, ... );
void    Message1Box( char *, ... );
void    Message2( char *, ... );
vi_rc   WPrintfLine( window_id, int, char *, ... );
bool    ColumnInWindow( int, int * );
void    SetWindowSizes( void );
void    SetWindowCursor( void );
void    SetWindowCursorForReal( void );
vi_rc   CurrentWindowResize( int x1, int y1, int x2, int y2 );
void    SetFileWindowTitle( window_id cw, info *cinfo, bool hilite );
void    ResetAllWindows( void );

/* word.c */
void    InitWordSearch( char *regword );
vi_rc   FindColumnOfNextWordForward( line *, int *, bool, bool );
vi_rc   FindColumnOfNextWordBackwards( line *, int *, bool, bool );
vi_rc   GimmeCurrentWord( char *, int, bool );
vi_rc   GetWordBound( line *, int, bool, int *, int * );
vi_rc   GimmeCurrentEntireWordDim( int *sc, int *ec, bool big );
vi_rc   MarkStartOfNextWordForward( i_mark *, i_mark *, bool );
vi_rc   MarkEndOfNextWordForward( i_mark *, i_mark *, bool );
vi_rc   MarkStartOfNextWordBackward( i_mark *, i_mark *, bool );
