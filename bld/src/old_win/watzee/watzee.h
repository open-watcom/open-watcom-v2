#ifdef __WINDOWS_386__
#define _EXPORT
#else
#define _EXPORT __export
#endif

/*  Child window control messages  */

#define IDW_DICE1           91
#define IDW_DICE2           92
#define IDW_DICE3           93
#define IDW_DICE4           94
#define IDW_DICE5           95
#define IDW_ROLL            96
#define IDW_OK              97


/*  Menu messages  */

#define IDM_NEWGAME        101
#define IDM_OPTIONS        102
#define IDM_EXIT           103
#define IDM_SCORING        104
#define IDM_ABOUT          105


/*  Dialog messages  */

#define IDD_CLEAR           99
#define IDD_PC             100
#define IDD_OK             101
#define IDD_CANCEL         105
#define IDD_ROLL           106
#define IDD_KEEP           107
#define IDD_RESET          108


/*  Internal user messages  */

#define WMW_START_NEW_GAME  WM_USER+1
#define WMW_SCORE_CHECK     WM_USER+3
#define WMW_DIE_CHECK       WM_USER+4
#define WMW_ROLL            WM_USER+5
#define WMW_OK              WM_USER+6
#define WMW_GAME_OVER       WM_USER+7


#define ID_TIMER                 1
#define TIMER_INTERVAL        2500

#define MAX_PLAYERS              4
#define PLAYER_DATA_ELEMENTS    19
#define UNDEFINED            32767


/*  Player data elements  */

#define IS_COMPUTER              0
#define ACES                     1
#define TWOS                     2
#define THREES                   3
#define FOURS                    4
#define FIVES                    5
#define SIXES                    6
#define THREE_KIND               7
#define FOUR_KIND                8
#define FULL_HOUSE               9
#define SMALL_STRAIGHT          10
#define LARGE_STRAIGHT          11
#define WATZEE                  12
#define CHANCE                  13
#define WATZEE_BONUS            14
#define BONUS                   15
#define UPPER_TOTAL             16
#define LOWER_TOTAL             17
#define GRAND_TOTAL             18

#define SCORE_OPTIONS_WIDTH      8      /*  widths represent # of characters */
#define SCORE_CARD_WIDTH        20


typedef struct die_info {               /*  data structure for a single die  */
    short   value;
    BOOL    is_checked;
} die_info;


typedef struct dice_info {              /*  data structure for dice stats  */
    short   sum;
    short   count[7];
    BOOL    got_three_kind;
    BOOL    got_four_kind;
    BOOL    got_full_house;
    BOOL    got_small_straight;
    BOOL    got_large_straight;
    BOOL    got_watzee;
} dice_info;



/*  global variables  */

extern short        NumberOfPlayers;
extern short        Player[ MAX_PLAYERS ] [ PLAYER_DATA_ELEMENTS ];
extern char         PlayerName[ MAX_PLAYERS ][ 4 ];
extern BOOL         GotTimer;
extern BOOL         PCTurn;
extern BOOL         PlayingGameYet;
extern BOOL         DieCheckMeansRoll;
extern BOOL         ResetDieChecks;
extern die_info     Dice[5];
extern dice_info    DiceInfo;
extern short        CharWidth;
extern short        CharHeight;
extern short        SysHeight;
extern short        CurrentPlayer;
extern short        CurrentTurn;
extern short        CurrentRoll;
extern short        LastScoreSelection;
extern LOGFONT      SmallNormalFont;
extern LOGFONT      SmallBoldFont;


/*  function prototypes  */

extern void             CreateFonts( void );
extern void             DrawScoreCard( HDC );
extern void             DrawDice( HWND, HDC );
extern void             GetDiceInfo( void );
extern void             GetDiceBitmaps( HANDLE );
extern void             GetFontInfo( void );
extern BOOL _EXPORT FAR PASCAL AboutDialogProc( HWND, WORD, WORD, LONG );
extern BOOL _EXPORT FAR PASCAL GetNumPlayersDialogProc(HWND,WORD,WORD,LONG);
extern BOOL _EXPORT FAR PASCAL GetInitialsDialogProc(HWND,WORD,WORD,LONG );
extern BOOL _EXPORT FAR PASCAL HelpDialogProc(HWND,WORD,WORD,LONG );
extern BOOL _EXPORT FAR PASCAL OptionsDialogProc(HWND,WORD,WORD,LONG );
extern void             HighliteName( HDC, short );
extern BOOL             PCPlay( HWND );
extern void             RollDice( HWND, HDC );
extern void             SelectFont( HDC, short );
extern void             WriteInitials( HDC );
extern void             WriteScore( HDC, short, short );
extern void             WriteScoreOptions( HDC );
extern void             WriteScores( HDC );
