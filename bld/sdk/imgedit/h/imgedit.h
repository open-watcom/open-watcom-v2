/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include "commonui.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "bool.h"
#include "watcom.h"
#include "macros.h"
#ifdef __OS2_PM__
    #include "img_os2.h"
#else
    #include "img_win.h"
    #include "palette.h"
#endif
#include "wrdll.h"
#include "wpi.h"
#include "img_wpi.h"
#include "imgtypes.h"
#include "fill.h"
#include "bits.h"
#include "dialogs.rh"
#include "icon.h"
#include "menuids.rh"
#include "statwnd.h"
#include "toolbr.h"
#include "errors.h"


#define MAX_DIM             2048
#define MIN_DIM             0
#define SB_SAVE_AS          1
#define SB_SAVE             0
#define LMOUSEBUTTON        0
#define RMOUSEBUTTON        1
#define INIT_STATUS_WIDTH   24
#define FUNCTIONBAR_WIDTH   26

#define BORDER_WIDTH    12      /* size of the border around the view area */

#ifndef FILE_OPENED             // this pre
    #define FILE_OPENED     1
#endif

#define FILE_SAVE       2
#define DIM_DEFAULT     32              // This is temporarily the default
#define NUMBER_OF_TOOLS 10
#define HINT_TEXT_LEN   40

#define IDM_FIRSTCHILD          100

#define UM_SHOWVIEWWINDOW       (WM_USER + 100)
#define UM_EXIT                 (WM_USER + 101)
#define UM_EXIT_NO_SAVE         (WM_USER + 102)
#define UM_SAVE_ALL             (WM_USER + 103)

#define RGBQ_SIZE( bc )         ((unsigned_32)sizeof( RGBQUAD ) * ((unsigned_32)1 << (bc)))
#define IMGED_DIM               WPI_RECTDIM

#define CMPFEXT(e,c)            (e[0] == '.' && stricmp(e + 1, c) == 0)

/*
 * typedefs
 */

#include "pushpck1.h"
typedef struct {
    WORD        file_id;
    DWORD       reserved[12];
    RGBQUAD     rgbqs[28];
} a_pal_file;
#include "poppck.h"

typedef struct {
    RECT        box;
    COLORREF    color;
    COLORREF    solid_color;
} palette_box;

typedef struct list_element {
    image_type                  imgtype;
    HWND                        hwnd;
    HWND                        viewhwnd;
    short                       width;
    short                       height;
    short                       bitcount;
    char                        fname[_MAX_PATH];
    bool                        issaved;
    WPI_POINT                   hotspot;
    WPI_HBITMAP                 xor_hbitmap;
    WPI_HBITMAP                 and_hbitmap;
    short                       num_of_images;
    struct list_element         *next;
    struct list_element         *nexticon;
#ifndef __OS2_PM__
    WRInfo                      *wrinfo;
    WResLangNode                *lnode;
#endif
} img_node;

typedef struct {
    short       x_pos;
    short       y_pos;
    short       last_xpos;
    short       last_ypos;
    short       width;
    short       height;
    short       show_state;
    short       tool_xpos;
    short       tool_ypos;
    short       pal_xpos;
    short       pal_ypos;
    short       view_xpos;
    short       view_ypos;
    short       shift;
    short       brush_size;
    boolbit     ismaximized     : 1;
    boolbit     square_grid     : 1;
    boolbit     grid_on         : 1;
} config_info;

typedef struct {
    int         settings;
    int         paste;
    int         rotate;
    int         viewwnd;
    bool        wrapshift;
    char        opendir[_MAX_PATH];
    char        savedir[_MAX_PATH];
    char        color[10];
} settings_info;

/*
 * globals
 */
extern char             const PaletteClass[];
extern char             const ViewWinClass[];
extern char             const DrawAreaClassB[];
extern char             const DrawAreaClassI[];
extern char             const DrawAreaClassC[];
extern char             const BitmapPickClass[];
extern HWND             HMainWindow;
extern HWND             ClientWindow;
extern HWND             HColorPalette;
extern WPI_INST         Instance;
extern WPI_FONT         SmallFont;
extern int              ColorPlanes;
extern int              BitsPerPixel;
extern config_info      ImgedConfigInfo;
extern bool             ImgedIsDDE;
extern char             *IEAppTitle;
extern char             *IEImageFilter;
extern char             *IEPaletteFilter;
extern char             *IEImageUntitled;
extern char             *IESaveImageTitle;
extern char             *IEOpenImageTitle;
extern char             *IESavePaletteTitle;
extern char             *IEOpenPaletteTitle;
extern int              StatusWidth;

/*
 * function prototypes
 */

/* ieglob.c */
void    IEFiniGlobalStrings( void );
bool    IEInitGlobalStrings( void );

/* ieproc.c */
WPI_EXPORT extern WPI_MRESULT CALLBACK ImgEdFrameProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
void CALLBACK       IEHelpCallBack( void );
void                IEEnableMenuInput( bool enable );
void                IEHelpRoutine( void );
void                IEHelpSearchRoutine( void );
void                IEHelpOnHelpRoutine( void );

/* ieclrpal.c */
WPI_EXPORT extern WPI_MRESULT CALLBACK ColorPalWinProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
void                CreateColorPal( void );
void                CheckPaletteItem( HMENU hmenu );
void                SetRGBValues( RGBQUAD *argbvals, int upperlimit );

/* clrcntls.c */
WPI_EXPORT extern WPI_MRESULT CALLBACK    ColorsWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
WPI_EXPORT extern WPI_MRESULT CALLBACK    ScreenWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
void                CreateColorControls( HWND hparent );
void                DisplayScreenClrs( bool fdisplay );
void                SetNumColors( int number_of_colors );
void                SetScreenClr( COLORREF screen_color );
void                ShowNewColor( int index, COLORREF newcolor, bool repaint );
void                SetInitScreenColor( COLORREF color );

/* curclr.c */
WPI_EXPORT extern WPI_MRESULT CALLBACK CurrentWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
void            CreateCurrentWnd( HWND hparent );
void            SetColor( int mousebutton, COLORREF color, COLORREF solid, wie_clrtype type );
COLORREF        GetSelectedColor( int mousebutton, COLORREF *solid, wie_clrtype *type );
void            VerifyCurrentClr( COLORREF screen_colour, COLORREF inverse_colour );
void            SetCurrentNumColors( int colour_count );
void            ChangeCurrentColor( void );

/* ieviewin.c */
WPI_EXPORT extern WPI_MRESULT CALLBACK ViewWindowProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
HWND            CreateViewWin( int width, int height );
void            CheckViewItem( HMENU hmenu );
COLORREF        GetViewBkColor( void );
void            SetViewBkColor( COLORREF color );
void            ResetViewWindow( HWND hwnd );
void            HideViewWindow( HWND hwnd );
void            RePositionViewWnd( img_node *node );
void            SetViewWindow( bool justone );
void            ShowViewWindows( HWND hwnd );
bool            IsOneViewWindow( void );

/* ieutil.c */
WPI_HBITMAP     CreateViewBitmap( img_node *node );
WPI_HBITMAP     DuplicateBitmap( WPI_HBITMAP hbitmap );
WPI_HBITMAP     EnlargeImage( HWND hwnd );
//void            ConvertToDIBitmap( WPI_HBITMAP hbitmap );
void            SetIsSaved( HWND hwnd, bool issaved );
void            OutlineRectangle( bool firsttime, WPI_PRES pres, WPI_RECT *prevrc, WPI_RECT *newrc );
void            GetFnameFromPath( const char *fullpath, char *fname );
void            GrayEditOptions( void );
void            FindOrigin( WPI_POINT *new_origin );
void            SetMenus( img_node *node );

/* ietoolbr.c */
bool    ToolBarProc(HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
void    InitTools( HWND hparent );
void    CheckToolbarItem( HMENU hmenu );
void    CloseToolBar( void );
void    AddHotSpotTool( bool faddhotspot );
void    PushToolButton( ctl_id cmdid );

/* ieopen.c */
void    SetupMenuAfterOpen( void );
bool    OpenImage( HANDLE hDrop );
bool    LoadColorPalette( void );
void    SetInitialOpenDir( char *new_dir );
char    *GetInitOpenDir( void );
void    OpenFileOnStart( const char *fname );
bool    ReadBitmapFromData( void *data, const char *fullname, WRInfo *info, WResLangNode *lnode );
bool    ReadIconFromData( void *data, const char *fname, WRInfo *info, WResLangNode *lnode  );
bool    ReadCursorFromData( void *data, const char *fname, WRInfo *info, WResLangNode *lnode );
void    WriteIconLoadedText( char *filename, int num );

/* iesave.c */
bool    SaveBitmapToData( img_node *node, BYTE **data, size_t *size );
bool    SaveImgToData( img_node *node, BYTE **data, size_t *size );
bool    SaveFile( int how );
bool    SaveFileFromNode( img_node *node, int how );
bool    SaveColorPalette( void );
void    SetInitialSaveDir( char *new_dir );
char    *GetInitSaveDir( void );

/* ienew.c */
WPI_EXPORT extern WPI_DLGRESULT CALLBACK  SelImgDlgProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
WPI_EXPORT extern WPI_DLGRESULT CALLBACK  SelBitmapDlgProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
#ifndef __OS2_PM__
WINEXPORT extern INT_PTR CALLBACK SelCursorDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );
#endif
bool        NewImage( image_type img_type, const char *filename );

/* iedraw.c */
void        CalculateDims( short img_width, short img_height, short *area_width, short *area_height );
void        BlowupImage( HWND hmdiwnd, WPI_PRES pres );
void        IEInvalidateNode( img_node *node );
void        CheckBounds( HWND hwnd, WPI_POINT *pt );
void        DrawLine( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, int mousebutton );
void        OutlineLine( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, WPI_POINT *prev_pt, bool firsttime );
void        DisplayRegion( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, int mousebutton );
void        OutlineClip( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, WPI_POINT *prev_pt, bool firsttime );
void        OutlineRegion( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, WPI_POINT *prev_pt, bool firsttime );
void        FillArea( WPI_POINT *pt, int mousebutton );
void        CreateNewDrawPad( img_node *node );
void        CreateDrawnImage( img_node *node );
void        CheckGridItem( HMENU hmenu );
void        ResetDrawArea( img_node *node );
void        SetDrawTool( int tool_type );
void        SetBrushSize( short new_size );
void        RepaintDrawArea( HWND hwnd );
WPI_POINT   GetPointSize( HWND hwnd );
void        ResizeChild( WPI_PARAM2 lparam, HWND hwnd, bool firsttime );
void        CheckSquareGrid( HMENU hmenu );
void        MaximizeCurrentChild( void );
void        DrawSinglePoint( HWND hwnd, WPI_POINT *pt, short mousebutton );
void        Paint( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, int mousebutton );

/* drawproc.c */
WPI_EXPORT extern WPI_MRESULT CALLBACK DrawAreaWinProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
int                 SetToolType( int toolid );
void                InitializeCursors( void );
void                CleanupCursors( void );

/* xorand.c */
void        MakeBitmap( img_node *node, bool isnew );
void        MakeIcon( img_node *node, bool isnew );
void        RemoveIcon( short index );
void        LineXorAnd( COLORREF xorcolor, COLORREF andcolor, WPI_POINT *startpt, WPI_POINT *endpt );
void        RegionXorAnd( COLORREF xorcolor, COLORREF andcolor, bool fFillRgn, WPI_RECT *r, bool is_rect );
void        FillXorAnd( COLORREF brushcolor, WPI_POINT *pt, wie_clrtype colortype );
void        SetNewHotSpot( WPI_POINT *pt );
void        FocusOnImage( HWND hwnd );
img_node    *GetCurrentNode( void );
void        SelectIcon( short index );
void        DeleteActiveImage( void );

/* freehand.c */
void    BeginFreeHand( HWND hwnd );
void    DrawThePoints( COLORREF color, COLORREF xorcolor, COLORREF andcolor, WPI_POINT *pt );
void    BrushThePoints( COLORREF color, COLORREF xorcolor, COLORREF andcolor, WPI_POINT *pt, int brushsize );
void    EndFreeHand( HWND hwnd );
bool    GetFreeHandPresentationSpaces( WPI_PRES *win, WPI_PRES *and, WPI_PRES *xor );

/* clip.c */
void    SetClipRect( HWND hwnd, WPI_POINT *startpt, WPI_POINT *endpt, WPI_POINT pointsize );
void    IECopyImage( void );
void    PlaceAndPaste( void );
void    PasteImage( WPI_POINT *pt, WPI_POINT pointsize, HWND hwnd );
void    CutImage( void );
void    DragClipBitmap( HWND hwnd, WPI_POINT *newpt, WPI_POINT pointsize );
void    CheckForClipboard( HMENU hmenu );
void    RedrawPrevClip( HWND hwnd );
void    SetRectExists( bool does_rect_exist );
void    CleanupClipboard( void );
void    DontPaste( HWND hwnd, WPI_POINT *topleft, WPI_POINT pointsize );
bool    DoesRectExist( WPI_RECT *rc );
void    SetDeviceClipRect( WPI_RECT *rect );

/* undo.c */
void    RecordImage( HWND hwnd );
void    UndoOp( void );
void    CheckForUndo( img_node *node );
void    CreateUndoStack( img_node *node );
void    DeleteUndoStack( HWND hwnd );
void    RedoOp( void );
void    RestoreImage( void );
void    AllowRestoreOption( img_node *node );
void    ResetUndoStack( img_node *node );
void    SelIconUndoStack( HWND hwnd, short index );
void    AddIconUndoStack( img_node *node );
void    DelIconUndoStack( img_node *node, int index );
bool    RelieveUndos( void );

/* colors.c */
void        InitPalette( void );
void        SetBoxColors( palette_box *screen, palette_box *inverse, int num_colors, palette_box *avail_colors );
void        DisplayColorBox( WPI_PRES pres, palette_box *box );
COLORREF    GetInverseColor( COLORREF color );
void        ReplacePaletteEntry( COLORREF newcolor );
void        ResetColorPalette( void );
COLORREF    GetPaletteColor( int index );
int         GetColorIndex( COLORREF color );
void        SetCurrentColors( bool fshowscreenclrs );
void        InitFromColorPalette( palette_box *screen, palette_box *inverse, palette_box *avail_colors );
bool        GetPaletteFile( a_pal_file *pal_file );
void        SetNewPalette( a_pal_file *pal_file );
void        RestoreColorPalette( void );
void        InitPaletteBitmaps( HWND hwnd, WPI_HBITMAP *color_hbitmap, WPI_HBITMAP *mono_hbitmap );

/* iconinfo.c */
void    FiniIconInfo( void );
void    InitIconInfo( void );
bool    CreateNewIcon( short *width, short *height, short *bitcount,  bool is_icon );
void    AddNewIcon( void );
void    DeleteIconImg( void );
void    SelectIconImg( void );
void    SetIconInfo( img_node *node );

/* fill.c */
void    Fill( fill_info_struct *fillinfo, img_node *node );

/* funcbar.c */
void    InitFunctionBar( HWND hparent );
void    CloseFunctionBar( void );
void    ResizeFunctionBar( WPI_PARAM2 lparam );
void    PressGridButton( void );
int     GetFunctionBarHeight( void );

/* modclrs.c */
void    EditColors( void );
void    RestoreColors( void );
void    SetColorMenus( img_node *node );

/* bkcolor.c */
void    ChooseBkColor( void );

/* imgdata.c */
void        AddImageNode( img_node *node );
img_node    *SelectImage( HWND hwnd );
bool        DeleteNode( HWND hwnd );
void        DeleteList( void );
int         DoImagesExist( void );
img_node    *GetHeadNode( void );
img_node    *GetNthIcon( HWND hwnd, short index );
img_node    *GetImageNode( HWND hwnd );
void        AddIconToList( img_node *icon, img_node *current_node );
img_node    *RemoveIconFromList( img_node *node, int index );
img_node    *SelectFromViewHwnd( HWND viewhwnd );

/* imgutil.c */
const char  *GetImageFileExt( image_type img_type, bool res );
image_type  GetImageFileType( const char *ext, bool res );
void        CheckForExt( img_node *node );
void        CheckForPalExt( char *filename );

/* iestatus.c */
bool    InitStatusLine( HWND parent );
void    SetPosInStatus( WPI_POINT *pt, WPI_POINT *pointsize, HWND hwnd );
void    SetSizeInStatus( HWND hwnd, WPI_POINT *startpt, WPI_POINT *endpt, WPI_POINT *pointsize );
void    ResizeStatusBar( WPI_PARAM2 lparam );
void    FiniStatusLine( void );
void    SetHotSpot( img_node *node );
void    ClearImageText( void );
void    SetHintText( const char *msg );
void    PrintHintTextByID( msg_id id, const char *fname );
void    PrintHintText( const char *msg, const char *fname );
void    WriteSetSizeText( msg_id msg, int x, int y );
void    IEPrintAmtText( msg_id message, int amt );

/* iefonts.c */
void    CreateStatusFont( void );
void    DisplayImageText( img_node *node );

/* chgsize.c */
void    ChangeImageSize( void );

/* settings.c */
void    SelectOptions( void );
int     StretchPastedImage( void );
int     GetRotateType( void );
bool    DoKeepRect( void );
void    SetSettingsDlg( settings_info *info );
void    GetSettings( settings_info *info );
bool    IsShiftWrap( void );

/* trnsform.c */
void    FlipImage( WORD whichway );
void    RotateImage( WORD whichway );
void    ClearImage( void );
void    ShiftImage( WORD shiftdirection );

/* ieprofil.c */
void    LoadImgedConfig( void );
void    SaveImgedConfig( void );

/* closeall.c */
void    CloseAllImages( void );
void    CloseCurrentImage( HWND hwnd );
void    SaveAllImages( void );

/* bits.c */
bitmap_bits *GetTheBits( WPI_HBITMAP hbitmap );
COLORREF    MyGetPixel( bitmap_bits *bits, int x, int y );
void        MySetPixel( bitmap_bits *bits, int x, int y, COLORREF color );
void        FreeTheBits( bitmap_bits *bits, WPI_HBITMAP hbitmap, bool setbits );

/* iectl3d.c */
bool    IECtl3dInit( HINSTANCE );
void    IECtl3dFini( HINSTANCE );
void    IECtl3dColorChange( void );
void    IECtl3dSubclassDlg( HWND, WORD );
void    IECtl3dSubclassDlgAll( HWND );

/* errors.c */
void    WImgEditError( msg_id error, const char *fname );

#ifndef __OS2_PM__

/* wincreat.c */
void    Win_CreateColorPal( void );
HWND    Win_CreateCurrentDisp( HWND hparent );
void    Win_CreateColorCtrls( HWND hpar, HWND *colors, HWND *screenclrs, HWND *screentxt, HWND *inversetxt );
HWND    WinCreateViewWin( HWND hviewwnd, bool foneview, int *showstate, int width, int height );
HWND    WinNewDrawPad( img_node *node );

/* winutils.c */
void    InitXorAndBitmaps( img_node *node );

/* icon.c */
void        ImageFini( an_img * );
WPI_HBITMAP ImgToXorBitmap( HDC, an_img * );
WPI_HBITMAP ImgToAndBitmap( HDC, an_img * );
void        ImageClose( an_img_file * );
an_img_file *ImageOpen( FILE * );
an_img_file *ImageOpenData( BYTE *, unsigned * );
an_img      *ImgResourceToImg( FILE *, an_img_file *, UINT );
an_img      *ImgResourceToImgData( BYTE *, unsigned *, an_img_file *, UINT );

/* snap.c */
void    OutlineSnap( void );
void    SnapPicture( void );
void    TransferImage( HWND hwnd );

/* getdata.c */
void        GetBitmapInfoHeader( BITMAPINFOHEADER *bmih, img_node *node );
void        FillImageResource( an_img_resource *img_res, img_node *node );
void        GetImageData( an_img *img, img_node *node );
BITMAPINFO  *GetDIBitmapInfo( img_node *node );
void        FreeDIBitmapInfo( BITMAPINFO *bmi );

/* pickbmp.c */
extern bool SelectDynamicBitmap( img_node *node, int imgcount, const char *filename );
WINEXPORT extern LRESULT CALLBACK BitmapPickProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam );

/* hinttext.c */
void    ShowHintText( ctl_id id );
void    InitMenus( HMENU hmenu );

#else

/* pmcreate.c */
void    PM_CreateColorPal( void );
HWND    PM_CreateCurrentDisp( HWND hparent );
void    PM_CreateColorCtrls( HWND hpar, HWND *colours, HWND *screenclrs, HWND *screentxt, HWND *inversetxt );
HWND    PMNewDrawPad( img_node *node );
HWND    PMCreateViewWin( HWND hviewwnd, bool foneview, int *showstate, int width, int height );

/* pmutils.c */
void        InitXorAndBitmaps( img_node *node );
WPI_HBITMAP CreateInverseBitmap( WPI_HBITMAP and_hbitmap, WPI_HBITMAP xor_hbitmap );
WPI_HBITMAP CreateColorBitmap( WPI_HBITMAP and_hbitmap, WPI_HBITMAP xor_hbitmap );

/* getdata.c */
void        GetBitmapInfoHeader( WPI_BITMAPINFOHEADER *bmih, img_node *node );
BITMAPINFO2 *GetXorBitmapInfo( img_node *node );
BITMAPINFO2 *GetAndBitmapInfo( img_node *node );
void        FreeDIBitmapInfo( BITMAPINFO2 *bmi );

/* hinttext.c */
void    ShowHintText( ctl_id id );

#endif
