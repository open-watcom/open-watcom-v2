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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "macros.h"

#ifdef __OS2_PM__
#include "img_os2.h"
#else
#include "img_win.h"
#include "..\..\misc\bitmap.h"
#include "..\..\misc\palette.h"
#include "..\..\misc\ctl3d.h"
#include "wrdll.h"
#endif

typedef enum {
    UNDEF_IMG   = 0,
    BITMAP_IMG  = 1,
    ICON_IMG    = 2,
    CURSOR_IMG  = 3
} wie_imgtype;

typedef enum {
    NORMAL_CLR  = 0,
    INVERSE_CLR = 1,
    SCREEN_CLR  = 2
} wie_clrtype;

#include "wpi.h"
#include "img_wpi.h"
#include "watcom.h"
#include "fill.h"
#include "bits.h"
#include "errors.h"
#include "dialogs.h"
#include "icon.h"
#include "menuids.h"
#include "..\..\misc\mem.h"
#include "..\..\misc\wstatus.h"
#include "..\..\misc\wptoolbr.h"

#define MAX_DIM         512
#define MIN_DIM         0
#define FALSE           0
#define TRUE            1
#define SB_SAVE_AS      1
#define SB_SAVE         0
#define LMOUSEBUTTON    0
#define RMOUSEBUTTON    1
#define STATUS_WIDTH    24
#define FUNCTIONBAR_WIDTH       26

#define BORDER_WIDTH    12      /* size of the border around the view area */

#define FILE_OPENED     1
#define FILE_SAVE       2
#define DIM_DEFAULT     32              // This is temporarily the default
#define UNDEF_IMG       0
#define BITMAP_IMG      1
#define ICON_IMG        2
#define CURSOR_IMG      3
#define RESOURCE_IMG    4
#define NUMBER_OF_TOOLS 10
#define MAX_IMAGES      5               // Check this
#define HINT_TEXT_LEN   40

#define IDM_FIRSTCHILD          100

#define UM_SHOWVIEWWINDOW       (WM_USER+100)
#define UM_EXIT                 (WM_USER+101)
#define UM_EXIT_NO_SAVE         (WM_USER+102)
#define UM_SAVE_ALL             (WM_USER+103)

#define RGBQ_SIZE( bc )         ( sizeof(RGBQUAD) * ((1<<(bc))))
#define IMGED_DIM               WPI_RECTDIM

/*
 * typedefs
 */
#pragma pack (1);
typedef struct {
    WORD        file_id;
    DWORD       reserved[12];
    RGBQUAD     rgbqs[28];
} a_pal_file;
#pragma pack();

typedef struct {
    RECT        box;
    COLORREF    colour;
    COLORREF    solid_colour;
} palette_box;

typedef struct list_element {
    wie_imgtype                 imgtype;
    HWND                        hwnd;
    HWND                        viewhwnd;
    short                       width;
    short                       height;
    short                       bitcount;
    char                        fname[ _MAX_PATH ];
    BOOL                        issaved;
    WPI_POINT                   hotspot;
    HBITMAP                     hxorbitmap;
    HBITMAP                     handbitmap;
    short                       num_of_images;
    struct list_element         *next;
    struct list_element         *nexticon;
#ifndef __OS2_PM__
    WRInfo                      *wrinfo;
    WResLangNode                *lnode;
#endif
} img_node;

typedef struct {
    BOOL        ismaximized;
    short       x_pos;
    short       y_pos;
    short       last_xpos;
    short       last_ypos;
    short       width;
    short       height;
    BOOL        show_state;
    short       tool_xpos;
    short       tool_ypos;
    short       pal_xpos;
    short       pal_ypos;
    short       view_xpos;
    short       view_ypos;
    short       shift;
    BOOL        square_grid;
    short       brush_size;
    BOOL        grid_on;
} config_info;

typedef struct {
    int         settings;
    int         paste;
    int         rotate;
    int         viewwnd;
    BOOL        wrapshift;
    char        opendir[ _MAX_PATH ];
    char        savedir[ _MAX_PATH ];
    char        colour[ 10 ];
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
extern HWND             HColourPalette;
extern WPI_INST         Instance;
extern WPI_FONT         SmallFont;
extern int              ColourPlanes;
extern int              BitsPerPixel;
extern config_info      ImgedConfigInfo;
extern BOOL             ImgedIsDDE;
extern char             *IEAppTitle;
extern char             *IEImageFilter;
extern char             *IEPaletteFilter;
extern char             *IEImageUntitled;
extern char             *IESaveImageTitle;
extern char             *IEOpenImageTitle;
extern char             *IESavePaletteTitle;
extern char             *IEOpenPaletteTitle;

/*
 * function prototypes
 */
/* ieglob.c */
void IEFiniGlobalStrings( void );
BOOL IEInitGlobalStrings( void );

/* ieproc.c */
void IEEnableMenuInput( BOOL enable );
MRESULT CALLBACK ImgEdFrameProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                                                            WPI_PARAM2 lparam );
extern void IEHelpRoutine( void );
extern void CALLBACK IEHelpCallBack( void );

/* ieclrpal.c */
MRESULT CALLBACK ColourPalWinProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
void CreateColourPal( void );
void CheckPaletteItem( HMENU hmenu );
void SetRGBValues( RGBQUAD *argbvals, int upperlimit );

/* clrcntls.c */
MRESULT CALLBACK ColoursWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
MRESULT CALLBACK ScreenWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
void CreateColourControls( HWND hparent );
void DisplayScreenClrs( BOOL fdisplay );
void SetNumColours( int number_of_colours );
void SetScreenClr( COLORREF screen_colour );
void ShowNewColour( int index, COLORREF newcolour, BOOL repaint );
void SetInitScreenColour( COLORREF colour );

/* curclr.c */
MRESULT CALLBACK CurrentWndProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
void CreateCurrentWnd( HWND hparent );
void SetColour( short mousebutton, COLORREF colour, COLORREF solid, wie_clrtype type );
COLORREF GetSelectedColour( short mousebutton, COLORREF *solid, wie_clrtype *type );
void VerifyCurrentClr( COLORREF screen_colour, COLORREF inverse_colour );
void SetCurrentNumColours( int colour_count );
void ChangeCurrentColour( void );

/* ieviewin.c */
HWND CreateViewWin( int width, int height );
MRESULT CALLBACK ViewWindowProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
void CheckViewItem( HMENU hmenu );
COLORREF GetBkColour( void );
void SetBkColour( COLORREF colour );
void ResetViewWindow( HWND hwnd );
void HideViewWindow( HWND hwnd );
void RePositionViewWnd( img_node *node );
void SetViewWindow( BOOL justone );
void ShowViewWindows( HWND hwnd );
BOOL IsOneViewWindow( void );

/* ieutil.c */
HBITMAP CreateViewBitmap( img_node *node );
HBITMAP DuplicateBitmap( HBITMAP hbitmap);
HBITMAP EnlargeImage( HWND hwnd );
// void ConvertToDIBitmap( HBITMAP hbitmap );
void SetIsSaved( HWND hwnd, BOOL fissaved );
void OutlineRectangle( BOOL firsttime, WPI_PRES pres, WPI_RECT *prevrc,
                                                            WPI_RECT *newrc );
void GetFnameFromPath( char *fullpath, char *fname );
void GrayEditOptions( void );
void FindOrigin( WPI_POINT *new_origin );
void SetMenus( img_node *node );

/* ietoolbr.c */
BOOL ToolBarProc(HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam, WPI_PARAM2 lparam );
void InitTools( HWND hparent );
void CheckToolbarItem( HMENU hmenu );
void CloseToolBar( void );
void AddHotSpotTool( BOOL faddhotspot );
void PushToolButton( WORD cmdid );

/* ieopen.c */
void SetupMenuAfterOpen( void );
int OpenImage( void );
BOOL LoadColourPalette( void );
void SetInitialOpenDir( char *new_dir );
char *GetInitOpenDir( void );
void OpenFileOnStart( char *fname );
BOOL readBitmapFromData( void *data, char *fullname, WRInfo *info, WResLangNode *lnode );
BOOL readIconFromData( void *data, char *fname, WRInfo *info, WResLangNode *lnode  );
BOOL readCursorFromData( void *data, char *fname, WRInfo *info, WResLangNode *lnode );

/* iesave.c */
BOOL saveBitmapToData( img_node *node, BYTE **data, uint_32 *size );
BOOL saveImgToData( img_node *node, BYTE **data, uint_32 *size );
BOOL SaveFile( short how );
BOOL SaveFileFromNode( img_node *node, short how );
BOOL SaveColourPalette( void );
void SetInitialSaveDir( char *new_dir );
char *GetInitSaveDir( void );

/* ienew.c */
WPI_DLGRESULT CALLBACK SelImgProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                                                      WPI_PARAM2 lparam );
WPI_DLGRESULT CALLBACK SelBitmapProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                                       WPI_PARAM2 lparam );
#ifndef __OS2_PM__
BOOL CALLBACK SelCursorProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam );
#endif
int NewImage( int img_type, char *filename );

/* iedraw.c */
void CalculateDims( short img_width, short img_height, short *area_width, short *area_height );
void BlowupImage( HWND hmdiwnd, WPI_PRES pres );
void IEInvalidateNode( img_node *node );
void CheckBounds( HWND hwnd, WPI_POINT *pt );
void DrawLine( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, int mousebutton );
void OutlineLine( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, WPI_POINT *prev_pt, BOOL firsttime );
void DisplayRegion( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, int mousebutton );
void OutlineClip( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, WPI_POINT *prev_pt, BOOL firsttime );
void OutlineRegion( HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, WPI_POINT *prev_pt, BOOL firsttime );
void FillArea( WPI_POINT *pt, int mousebutton );
void CreateNewDrawPad( img_node *node );
void CreateDrawnImage( img_node *node );
void CheckGridItem( HMENU hmenu );
void ResetDrawArea( img_node *node );
void SetDrawTool( int tool_type );
void SetBrushSize( short new_size );
void RepaintDrawArea( HWND hwnd );
WPI_POINT GetPointSize( HWND hwnd );
void ResizeChild( WPI_PARAM2 lparam, HWND hwnd, BOOL firsttime );
void CheckSquareGrid( HMENU hmenu );
void MaximizeCurrentChild( void );
void DrawSinglePoint( HWND hwnd, WPI_POINT *pt, short mousebutton );
void Paint(HWND hwnd, WPI_POINT *start_pt, WPI_POINT *end_pt, int mousebutton );

/* drawproc.c */
MRESULT CALLBACK DrawAreaWinProc( HWND hwnd, WPI_MSG msg, WPI_PARAM1 wparam,
                                                            WPI_PARAM2 lparam );
int SetToolType( int toolid );
void InitializeCursors( void );
void CleanupCursors( void );

/* xorand.c */
void MakeBitmap( img_node *node, BOOL isnew );
void MakeIcon( img_node *node, BOOL isnew );
void RemoveIcon( short index );
void LineXorAnd( COLORREF xorcolour, COLORREF andcolour, WPI_POINT *startpt,
                                                        WPI_POINT *endpt );
void RegionXorAnd( COLORREF xorcolour, COLORREF andcolour, BOOL fFillRgn,
                                                WPI_RECT *r, BOOL is_rect );
void FillXorAnd( COLORREF brushcolour, WPI_POINT *pt, wie_clrtype colourtype );
void SetNewHotSpot( WPI_POINT *pt );
void FocusOnImage( HWND hwnd );
img_node *GetCurrentNode( void );
void SelectIcon( short index );
void DeleteActiveImage( void );

/* freehand.c */
void BeginFreeHand( HWND hwnd );
void DrawThePoints( COLORREF colour, COLORREF xorcolour, COLORREF andcolour,
                                                            WPI_POINT *pt );
void BrushThePoints( COLORREF colour, COLORREF xorcolour, COLORREF andcolour,
                                                WPI_POINT *pt, int brushsize );
void EndFreeHand( HWND hwnd );
BOOL GetFreeHandPresentationSpaces( WPI_PRES *win, WPI_PRES *and,
                                    WPI_PRES *xor );

/* clip.c */
void SetClipRect( HWND hwnd, WPI_POINT *startpt, WPI_POINT *endpt, WPI_POINT pointsize );
void IECopyImage( void );
void PlaceAndPaste( void );
void PasteImage( WPI_POINT *pt, WPI_POINT pointsize, HWND hwnd );
void CutImage( void );
void DragClipBitmap( HWND hwnd, WPI_POINT *newpt, WPI_POINT pointsize );
void CheckForClipboard( HMENU hmenu );
void RedrawPrevClip( HWND hwnd );
void SetRectExists( BOOL does_rect_exist );
void CleanupClipboard( void );
void DontPaste( HWND hwnd, WPI_POINT *topleft, WPI_POINT pointsize );
BOOL DoesRectExist( WPI_RECT *rc );
void SetDeviceClipRect( WPI_RECT *rect );

/* undo.c */
void RecordImage( HWND hwnd );
void UndoOp( void );
void CheckForUndo( img_node *node );
void CreateUndoStack( img_node *node );
void DeleteUndoStack( HWND hwnd );
void RedoOp( void );
void RestoreImage( void );
void AllowRestoreOption( img_node *node );
void ResetUndoStack( img_node *node );
void SelIconUndoStack( HWND hwnd, short index );
void AddIconUndoStack( img_node *node );
void DelIconUndoStack( img_node *node, int index );
BOOL RelieveUndos( void );

/* colours.c */
void InitPalette( void );
void SetBoxColours( palette_box *screen, palette_box *inverse, int num_colours, palette_box *avail_colours );
void DisplayColourBox( WPI_PRES pres, palette_box *box );
COLORREF GetInverseColour( COLORREF colour );
void ReplacePaletteEntry( COLORREF newcolour );
void ResetColourPalette( void );
COLORREF GetPaletteColour( int index );
int GetColourIndex( COLORREF colour );
void SetCurrentColours( BOOL fshowscreenclrs );
void InitFromColourPalette( palette_box *screen, palette_box *inverse, palette_box *avail_colours );
BOOL GetPaletteFile( a_pal_file *pal_file );
void SetNewPalette( a_pal_file *pal_file );
void RestoreColourPalette( void );
void InitPaletteBitmaps( HWND hwnd, HBITMAP *colourbitmap, HBITMAP *monobitmap );

/* iconinfo.c */
void FiniIconInfo( void );
void InitIconInfo( void );
BOOL CreateNewIcon( short *width, short *height, short *bitcount, BOOL is_icon);
void AddNewIcon( void );
void DeleteIconImg( void );
void SelectIconImg( void );
void SetIconInfo( img_node *node );

/* fill.c */
void Fill( fill_info_struct *fillinfo, img_node *node );

/* funcbar.c */
void InitFunctionBar( HWND hparent );
void CloseFunctionBar( void );
void ResizeFunctionBar( WPI_PARAM2 lparam );
void PressGridButton( void );

/* modclrs.c */
void EditColours( void );
void RestoreColours( void );
void SetColourMenus( img_node *node );

/* bkcolour.c */
void ChooseBkColour( void );

/* imgdata.c */
void AddImageNode( img_node *node );
img_node *SelectImage( HWND hwnd );
BOOL DeleteNode( HWND hwnd );
void DeleteList( void );
int DoImagesExist( void );
img_node *GetHeadNode( void );
img_node *GetNthIcon( HWND hwnd, short index );
img_node *GetImageNode( HWND hwnd );
void AddIconToList( img_node *icon, img_node *current_node);
img_node *RemoveIconFromList( img_node *node, int index );
img_node *SelectFromViewHwnd( HWND viewhwnd );

/* iestatus.c */
BOOL InitStatusLine( HWND parent );
void SetPosInStatus( WPI_POINT *pt, WPI_POINT *pointsize, HWND hwnd );
void SetSizeInStatus( HWND hwnd, WPI_POINT *startpt, WPI_POINT *endpt, WPI_POINT *pointsize );
void ResizeStatusBar( WPI_PARAM2 lparam );
void FiniStatusLine( void );
void SetHotSpot( img_node *node );
void ClearImageText( void );
void SetHintText( char *msg );
void PrintHintTextByID( DWORD id, char *fname );
void PrintHintText( char *msg, char *fname );
void WriteSetSizeText( DWORD msg, int x, int y );
void IEPrintAmtText( DWORD message, int amt );

/* iefonts.c */
void CreateStatusFont( void );
void DisplayImageText( img_node *node );

/* chgsize.c */
void ChangeImageSize( void );

/* settings.c */
void SelectOptions( void );
BOOL StretchPastedImage( void );
int GetRotateType( void );
BOOL DoKeepRect( void );
void SetSettingsDlg( settings_info *info );
void GetSettings( settings_info *info );
BOOL IsShiftWrap( void );

/* trnsform.c */
void FlipImage( WORD whichway );
void RotateImage( WORD whichway );
void ClearImage( void );
void ShiftImage( WORD shiftdirection );

/* ieprofil.c */
void LoadImgedConfig( void );
void SaveImgedConfig( void );

/* closeall.c */
void CloseAllImages( void );
void CloseCurrentImage( HWND hwnd );
void SaveAllImages( void );

/* bits.c */
bitmap_bits *GetTheBits( HBITMAP bitmap );
COLORREF MyGetPixel( bitmap_bits *bits, int x, int y );
void MySetPixel( bitmap_bits *bits, int x, int y, COLORREF colour );
void FreeTheBits( bitmap_bits *bits, HBITMAP bitmap, BOOL setbits );

/* iectl3d.c */
BOOL IECtl3DInit( HINSTANCE );
void IECtl3DFini( HINSTANCE );
void IECtl3dColorChange( void );
void IECtl3dSubclassDlg( HWND, WORD );

/* errors.c */
void WImgEditError( DWORD error, char *fname );

#ifndef __OS2_PM__
/* wincreat.c */
void Win_CreateColourPal( void );
HWND Win_CreateCurrentDisp( HWND hparent );
void Win_CreateColourCtrls( HWND hpar, HWND *colours, HWND *screenclrs, HWND *screentxt, HWND *inversetxt );
HWND WinCreateViewWin( HWND hviewwnd, BOOL foneview, int *showstate, int width, int height );
HWND WinNewDrawPad( img_node *node );

/* winutils.c */
void InitXorAndBitmaps( img_node *node );

/* icon.c */
void ImageFini(an_img *);
HBITMAP ImgToXorBitmap(HDC ,an_img *);
HBITMAP ImgToAndBitmap(HDC ,an_img *);
void ImageClose(an_img_file *);
an_img_file *ImageOpen(FILE *);
an_img_file *ImageOpenData(BYTE *, unsigned *);
an_img *ImgResourceToImg(FILE *, an_img_file *, UINT );
an_img *ImgResourceToImgData(BYTE *, unsigned *, an_img_file *, UINT );

/* snap.c */
void OutlineSnap( void );
void SnapPicture( void );
void TransferImage( HWND hwnd );

/* getdata.c */
void GetBitmapInfoHeader( BITMAPINFOHEADER *bmih, img_node *node );
void FillImageResource( an_img_resource *img_res, img_node *node );
void GetImageData( an_img *img, img_node *node );
BITMAPINFO *GetDIBitmapInfo( img_node *node );
void FreeDIBitmapInfo( BITMAPINFO *bmi );

/* pickbmp.c */
BOOL SelectDynamicBitmap( img_node *node, int imgcount, char *filename );
LONG CALLBACK BitmapPickProc( HWND hwnd, UINT msg, UINT wparam, LONG lparam );

/* hinttext.c */
void ShowHintText( UINT wparam );
void InitMenus( HMENU hmenu );

#else
/* pmcreate.c */
void PM_CreateColourPal( void );
HWND PM_CreateCurrentDisp( HWND hparent );
void PM_CreateColourCtrls( HWND hpar, HWND *colours, HWND *screenclrs,
                                        HWND *screentxt, HWND *inversetxt );
HWND PMNewDrawPad( img_node *node );
HWND PMCreateViewWin( HWND hviewwnd, BOOL foneview, int *showstate,
                                                int width, int height );

/* pmutils.c */
void InitXorAndBitmaps( img_node *node );
HBITMAP CreateInverseBitmap( HBITMAP andbitmap, HBITMAP xorbitmap );
HBITMAP CreateColourBitmap( HBITMAP andbitmap, HBITMAP xorbitmap );
WPI_HANDLE MakeWPIBitmap( HBITMAP hbitmap );

/* getdata.c */
void GetBitmapInfoHeader( WPI_BITMAPINFOHEADER *bmih, img_node *node );
BITMAPINFO2 *GetXorBitmapInfo( img_node *node );
BITMAPINFO2 *GetAndBitmapInfo( img_node *node );
void FreeDIBitmapInfo( BITMAPINFO2 *bmi );

/* hinttext.c */
void ShowHintText( MPARAM wparam );

#endif
