// NB the maximum allowable string length is 128 bytes

#include "about.msg"

// iefonts.c
pick(	WIE_STATUSFONT,
	"Helv.8",
	"Helv.8" )

// iestatus.c
pick(	WIE_STATUSPOSTEXT,
	"Pos:",
	"Pos:" )
pick(	WIE_STATUSSIZETEXT,
	"Size:",
	"Size:" )
// the leading %s MUST remain at the front of the string
pick(	WIE_STATUSPOSINFO,
	"%sPos: %d, %d",
	"%sPos: %d, %d" )
// the leading %s MUST remain at the front of the string
pick(	WIE_STATUSSIZEINFO,
	"%sSize: %dx%d",
	"%sSize: %dx%d" )
// the leading %s MUST remain at the front of the string
pick(	WIE_STATUSHOTSPOTINFO,
	"%sHotSpot: %d, %d",
	"%sHotSpot: %d, %d" )
// the leading %s MUST remain at the front of the string
pick(	WIE_BITMAPIMAGETEXT,
	"%sBitmap: %dx%d, %d Color",
	"%sBitmap: %dx%d, %d Color" )
// the leading %s MUST remain at the front of the string
pick(	WIE_CURSORIMAGETEXT,
	"%sCursor: %dx%d, %d Color",
	"%sCursor: %dx%d, %d Color" )
// the leading %s MUST remain at the front of the string
pick(	WIE_ICONIMAGETEXT,
	"%sIcon: %dx%d, %d Color",
	"%sIcon: %dx%d, %d Color" )

// errors.c
pick(	WIE_ERRORMSGTITLE,
	"WATCOM Image Editor",
	"WATCOM Image Editor" )
pick(	WIE_ERR_BITMAPNOTOPENED,
	"Bitmap: '%s' not opened.",
	"Bitmap: '%s' not opened." )
pick(	WIE_ERR_ICONNOTOPENED,
	"Icon: '%s' not opened.",
	"Icon: '%s' not opened." )
pick(	WIE_ERR_CURSORNOTOPENED,
	"Cursor: '%s' not opened.",
	"Cursor: '%s' not opened." )
pick(	WIE_ERR_INVALIDBITMAPFILE,
	"Invalid bitmap file: '%s'",
	"Invalid bitmap file: '%s'" )
pick(	WIE_ERR_INVALIDICONFILE,
	"Invalid icon file: '%s'",
	"Invalid icon file: '%s'" )
pick(	WIE_ERR_INVALIDCURSORFILE,
	"Invalid cursor file: '%s'",
	"Invalid cursor file: '%s'" )
pick(	WIE_ERR_INVALIDBITMAPDATA,
	"File '%s' contains invalid bitmap data.",
	"File '%s' contains invalid bitmap data." )
pick(	WIE_ERR_INVALIDICONDATA,
	"File '%s' contains invalid icon data.",
	"File '%s' contains invalid icon data." )
pick(	WIE_ERR_INVALIDCURSORDATA,
	"File '%s' contained invalid cursor data.",
	"File '%s' contained invalid cursor data." )
pick(	WIE_ERR_INVALIDFILENAME,
	"'%s' is an invalid file name!",
	"'%s' is an invalid file name!" )
pick(	WIE_ERR_FILE_NOT_OPENED,
	"Error opening '%s'",
	"Error opening '%s'" )
pick(	WIE_ERR_STARTUP_FNO,
	"Error opening '%s'",
	"Error opening '%s'" )
pick(	WIE_ERR_FILENOTOPENED,
	"File '%s' not opened.",
	"File '%s' not opened." )
pick(	WIE_ERR_FILENOTSAVED,
	"File '%s' not saved.",
	"File '%s' not saved." )
pick(	WIE_ERR_PALETTENOTSAVED,
	"Palette not saved.",
	"Palette not saved." )
pick(	WIE_ERR_BAD_IMAGE_TYPE,
	"Active image is not an icon.",
	"Active image is not an icon." )
pick(	WIE_ERR_BAD_SELECTION,
	"Invalid icon selection.",
	"Invalid icon selection." )
pick(	WIE_ERR_NOFILLREGION,
	"Could not fill region.",
	"Could not fill region." )
pick(	WIE_DDE_INIT_FAILED,
	"DDE Initialization failed.",
	"DDE Initialization failed." )
	
pick(	WIE_ERR_BITMAP_TOO_BIG,
	"A dimension of this bitmap exceeds 512 pixels.",
	"A dimension of this bitmap exceeds 512 pixels." )
pick(	WIE_ERR_256CLR_BITMAP,
	"Bitmaps with 256 colors are not yet supported.",
	"Bitmaps with 256 colors are not yet supported." )
pick(	WIE_ERR_BAD_BITMAP_FILE,
	"'%s' could not be opened.",
	"'%s' could not be opened." )
pick(	WIE_ERR_TOO_MANY_COLOURS,
	"'Cannot edit images with greater than 256 colors.",
	"'Cannot edit images with greater than 256 colors." )
pick(	WIE_ERR_BAD_BITMAP_DATA,
	"File '%s' contains invalid bitmap data!",
	"File '%s' contains invalid bitmap data!" )
pick(	WIE_ERR_BAD_ICON_CLR,
	"Icons must have 2 or 16 colors.",
	"Icons must have 2 or 16 colors." )
pick(	WIE_ERR_BAD_ICON_FILE,
	"'%s' is not a valid icon file!",
	"'%s' is not a valid icon file!" )
pick(	WIE_ERR_BAD_ICON_DATA,
	"File '%s' contains bad icon data!",
	"File '%s' contains bad icon data!" )
pick(	WIE_ERR_BAD_CURSOR_FILE,
	"'%s' is not a valid cursor file!",
	"'%s' is not a valid cursor file!" )
pick(	WIE_ERR_BAD_CURSOR_DATA,
	"File '%s' contained invalid cursor data!",
	"File '%s' contained invalid cursor data!" )
pick(	WIE_ERR_BAD_FILENAME,
	"'%s' is an invalid file name!",
	"'%s' is an invalid file name!" )
pick(	WIE_ERR_BAD_FILE_EXT,
	"'%s' has an unrecognized file extension.",
	"'%s' has an unrecognized file extension." )
pick(	WIE_ERR_SAVE_FAIL,
	"Error writing data for file '%s'.  File not saved",
	"Error writing data for file '%s'.  File not saved" )
pick(	WIE_ERR_PALETTE_NOT16,
	"The color palette must in 16 color mode.",
	"The color palette must in 16 color mode." )
pick(	WIE_ERR_BAD_HWND,
	"%s\nInvalid Window Handle.",
	"%s\nInvalid Window Handle." )
pick(	WIE_ERR_BAD_FILLCASE,
	"%s\nCould not fill region.",
	"%s\nCould not fill region." )
pick(	WIE_ERR_BAD_ICONINDEX,
	"%s\nBad icon index.",
	"%s\nBad icon index." )
pick(	WIE_ERR_BAD_PALFILE,
	"%s is not a valid palette file.",
	"%s is not a valid palette file." )

// hinttext.c
pick(	WIE_SELECTSIMAGETOEDIT,
	"Selects image to edit",
	"Selects image to edit" )
pick(	WIE_HINT_IMGED_NEW,
	"Creates a new image",
	"Creates a new image" )
pick(	WIE_HINT_IMGED_OPEN,
	"Opens an image",
	"Opens an image" )
pick(	WIE_HINT_IMGED_SAVE,
	"Saves current image",
	"Saves current image" )
pick(	WIE_HINT_IMGED_SAVE_AS,
	"Saves current image",
	"Saves current image" )
pick(	WIE_HINT_IMGED_EXIT,
	"Exits the image editor",
	"Exits the image editor" )
pick(	WIE_HINT_IMGED_ABOUT,
	"Describes the WATCOM Image Editor",
	"Describes the WATCOM Image Editor" )
pick(	WIE_HINT_IMGED_REST,
	"Restores image to opened form",
	"Restores image to opened form" )
pick(	WIE_HINT_IMGED_LCOLOUR,
	"Loads a color palette",
	"Loads a color palette" )
pick(	WIE_HINT_IMGED_SCOLOUR,
	"Saves the current color palette",
	"Saves the current color palette" )
pick(	WIE_HINT_IMGED_RCOLOUR,
	"Restores color palette to default",
	"Restores color palette to default" )
pick(	WIE_HINT_IMGED_CLOSE,
	"Closes the current image",
	"Closes the current image" )
pick(	WIE_HINT_IMGED_ARRANGE,
	"Arranges the icons",
	"Arranges the icons" )
pick(	WIE_HINT_IMGED_CLOSEALL,
	"Closes all images",
	"Closes all images" )
pick(	WIE_HINT_IMGED_COLOUR,
	"Toggles display of the color palette",
	"Toggles display of the color palette" )
pick(	WIE_HINT_IMGED_VIEW,
	"Toggles display of the view window",
	"Toggles display of the view window" )
pick(	WIE_HINT_IMGED_TOOLBAR,
	"Toggles display of the tool window",
	"Toggles display of the tool window" )
pick(	WIE_HINT_IMGED_FREEHAND,
	"Draw freehand with pencil",
	"Draw freehand with pencil" )
pick(	WIE_HINT_IMGED_LINE,
	"Draw lines",
	"Draw lines" )
pick(	WIE_HINT_IMGED_CIRCLEO,
	"Draw outlined ellipses",
	"Draw outlined ellipses" )
pick(	WIE_HINT_IMGED_CIRCLEF,
	"Draw filled ellipses",
	"Draw filled ellipses" )
pick(	WIE_HINT_IMGED_RECTO,
	"Draw outlined rectangles",
	"Draw outlined rectangles" )
pick(	WIE_HINT_IMGED_RECTF,
	"Draw filled rectangles",
	"Draw filled rectangles" )
pick(	WIE_HINT_IMGED_FILL,
	"Fill area under the cursor",
	"Fill area under the cursor" )
pick(	WIE_HINT_IMGED_BRUSH,
	"Draw freehand with wide brush",
	"Draw freehand with wide brush" )
pick(	WIE_HINT_IMGED_CLIP,
	"Select region to edit",
	"Select region to edit" )
pick(	WIE_HINT_IMGED_HOTSPOT,
	"Set hotspot location",
	"Set hotspot location" )
pick(	WIE_HINT_IMGED_2x2,
	"Set brush size to 2x2 pixels",
	"Set brush size to 2x2 pixels" )
pick(	WIE_HINT_IMGED_3x3,
	"Set brush size to 3x3 pixels",
	"Set brush size to 3x3 pixels" )
pick(	WIE_HINT_IMGED_4x4,
	"Set brush size to 4x4 pixels",
	"Set brush size to 4x4 pixels" )
pick(	WIE_HINT_IMGED_5x5,
	"Set brush size to 5x5 pixels",
	"Set brush size to 5x5 pixels" )
pick(	WIE_HINT_IMGED_TILE,
	"Tile image windows",
	"Tile image windows" )
pick(	WIE_HINT_IMGED_NEWIMG,
	"Add an icon to current icon file",
	"Add an icon to current icon file" )
pick(	WIE_HINT_IMGED_DELIMG,
	"Delete an icon from current icon file",
	"Delete an icon from current icon file" )
pick(	WIE_HINT_IMGED_CASCADE,
	"Cascade all the image windows",
	"Cascade all the image windows" )
pick(	WIE_HINT_IMGED_GRID,
	"Toggles the displaying of the grid",
	"Toggles the displaying of the grid" )
pick(	WIE_HINT_IMGED_CLEAR,
	"Clear image or selected area",
	"Clear image or selected area" )
pick(	WIE_HINT_IMGED_UNDO,
	"Undoes the previous operation",
	"Undoes the previous operation" )
pick(	WIE_HINT_IMGED_REDO,
	"Reperforms an undone operation",
	"Reperforms an undone operation" )
pick(	WIE_HINT_IMGED_CUT,
	"Cuts selected area to clipboard",
	"Cuts selected area to clipboard" )
pick(	WIE_HINT_IMGED_COPY,
	"Copies selected area to clipboard",
	"Copies selected area to clipboard" )
pick(	WIE_HINT_IMGED_PASTE,
	"Pastes into current image",
	"Pastes into current image" )
pick(	WIE_HINT_IMGED_SNAP,
	"Captures a bitmap from the desktop",
	"Captures a bitmap from the desktop" )
pick(	WIE_HINT_IMGED_RIGHT,
	"Shifts the image right",
	"Shifts the image right" )
pick(	WIE_HINT_IMGED_LEFT,
	"Shifts the image left",
	"Shifts the image left" )
pick(	WIE_HINT_IMGED_UP,
	"Shifts the image up",
	"Shifts the image up" )
pick(	WIE_HINT_IMGED_DOWN,
	"Shifts the image down",
	"Shifts the image down" )
pick(	WIE_HINT_IMGED_SELIMG,
	"Selects icon from a multiple icon file",
	"Selects icon from a multiple icon file" )
pick(	WIE_HINT_IMGED_SQUARE,
	"Maintains a square grid",
	"Maintains a square grid" )
pick(	WIE_HINT_IMGED_SIZE,
	"Changes size of current image",
	"Changes size of current image" )
pick(	WIE_HINT_IMGED_SETTINGS,
	"Displays the current settings",
	"Displays the current settings" )
pick(	WIE_HINT_IMGED_FLIPHORZ,
	"Flips image over horizontal axis",
	"Flips image over horizontal axis" )
pick(	WIE_HINT_IMGED_FLIPVERT,
	"Flips image over vertical axis",
	"Flips image over vertical axis" )
pick(	WIE_HINT_IMGED_ROTATECC,
	"Rotates image counterclockwise",
	"Rotates image counterclockwise" )
pick(	WIE_HINT_IMGED_ROTATECL,
	"Rotates image clockwise",
	"Rotates image clockwise" )
pick(	WIE_HINT_IMGED_CEDIT,
	"Edits the current left color",
	"Edits the current left color" )
pick(	WIE_HINT_IMGED_CRESET,
	"Restores palette to unedited form",
	"Restores palette to unedited form" )
pick(	WIE_HINT_IMGED_CSCREEN,
	"Sets color to represent screen/inverse",
	"Sets color to represent screen/inverse" )
pick(	WIE_HINT_IMGED_MAXIMIZE,
	"Maximizes the current edit window",
	"Maximizes the current edit window" )
pick(	WIE_HINT_IMGED_DDE_UPDATE_PRJ,
	"Update the file with this resource",
	"Update the file with this resource" )
	
pick(	WIE_HINT_FILEMENU,
	"File operations",
	"File operations" )
pick(	WIE_HINT_EDITMENU,
	"Edit operations for current image",
	"Edit operations for current image" )
pick(	WIE_HINT_SHIFTMENU,
	"Shifts the image",
	"Shifts the image" )
pick(	WIE_HINT_MIRRORMENU,
	"Creates a mirror image",
	"Creates a mirror image" )
pick(	WIE_HINT_ROTATEMENU,
	"Rotates image 90 degrees",
	"Rotates image 90 degrees" )
pick(	WIE_HINT_SETTINGSMENU,
	"Settings for image editor session",
	"Settings for image editor session" )
pick(	WIE_HINT_BRUSHSIZEMENU,
	"Set the size for the paint brush",
	"Set the size for the paint brush" )
pick(	WIE_HINT_PALETTEMENU,
	"Perform color palette operations",
	"Perform color palette operations" )
pick(	WIE_HINT_DRAWTOOLSMENU,
	"Selects tools with which to draw",
	"Selects tools with which to draw" )
pick(	WIE_HINT_ELLIPSEMENU,
	"Ellipse drawing tool",
	"Ellipse drawing tool" )
pick(	WIE_HINT_RECTANGLEMENU,
	"Rectangle drawing tool",
	"Rectangle drawing tool" )
pick(	WIE_HINT_HELPMENU,
	"Provides help for WATCOM image editor",
	"Provides help for WATCOM image editor" )
	
// SNAP.C
pick(	WIE_BMPSNAPPEDFROMDSKTOP,
	"Bitmap snapped from desktop",
	"Bitmap snapped from desktop" )
pick(	WIE_BMPSNAPABORTED,
	"Snap aborted",
	"Snap aborted" )

// IEVIEWIN.C
pick(	WIE_USEOPTIONSTOHIDEALL,
	"Use options menu to hide all",
	"Use options menu to hide all" )

// BKCOLOUR.C
pick(	WIE_NEWBKCOLORSELECTED,
	"New background color selected",
	"New background color selected" )

// COLOURS.C
pick(	WIE_PALETTERESTORED,
	"Color palette restored",
	"Color palette restored" )

// SETTINGS.C
pick(	WIE_SHIFTBETWEEN,
	"Shift amount must be between \n %d and %d",
	"Shift amount must be between \n %d and %d" )
pick(	WIE_NOTE,
	"NOTE",
	"NOTE" )

// IEDRAW.C
pick(	WIE_GRIDTURNEDOFF,
	"Grid turned off",
	"Grid turned off" )
pick(	WIE_GRIDTURNEDON,
	"Grid turned on",
	"Grid turned on" )
pick(	WIE_NEWBRUSHSIZE,
	"New brush size: %dx%d",
	"New brush size: %dx%d" )
pick(	WIE_NEWGRIDSIZE,
	"Grid size is %dx%d pixels.",
	"Grid size is %dx%d pixels." )
pick(	WIE_SQUAREGRIDOFF,
	"Square grid option turned off",
	"Square grid option turned off" )
pick(	WIE_SQUAREGRIDON,
	"Square grid option turned on",
	"Square grid option turned on" )

// IEDDE.C
pick(	WIE_DDEINITTITLE,
	"DDE Initialization",
	"DDE Initialization" )
pick(	WIE_DDETERMINATIONMSG,
	"The Image Editor is being terminated and \nwas unable to inform the Resource Editor.\nThis may cause the Resource Editor to reject\nany more attempts to edit any resources.\n",
	"The Image Editor is being terminated and \nwas unable to inform the Resource Editor.\nThis may cause the Resource Editor to reject\nany more attempts to edit any resources.\n" )

// CLIP.C
pick(	WIE_ENTIREIMAGECOPIED,
	"Entire image copied to clipboard",
	"Entire image copied to clipboard" )
pick(	WIE_AREACOPIED,
	"Area copied to clipboard",
	"Area copied to clipboard" )
pick(	WIE_NOBITMAPINCLIPBOARD,
	"No bitmap in the clipboard",
	"No bitmap in the clipboard" )
pick(	WIE_CLIPBOARDBITMAPSIZE,
	"Clipboard bitmap size is %dx%d",
	"Clipboard bitmap size is %dx%d" )
pick(	WIE_BITMAPPASTED,
	"Bitmap pasted from clipboard",
	"Bitmap pasted from clipboard" )
pick(	WIE_ENTIREIMAGECUT,
	"Entire image cut to clipboard",
	"Entire image cut to clipboard" )
pick(	WIE_AREACUT,
	"Area cut to clipboard",
	"Area cut to clipboard" )
pick(	WIE_PASTECANCELLED,
	"Paste cancelled",
	"Paste cancelled" )

// IEGLOB.C
pick(	WIE_APPNAME,
	"WATCOM Image Editor",
	"WATCOM Image Editor" )
pick(	WIE_IMAGEFILTER,
	"Bitmap (*.bmp)\t*.bmp\tIcon (*.ico)\t*.ico\t"
	"Cursor (*.cur)\t*.cur\tResource (*.res)\t*.res\t"
	"Executable (*.exe)\t*.exe\tDLL Libraries(*.dll)\t*.dll\t"
	"All Files (*.*)\t*.*\t",
	"Bitmap (*.bmp)\t*.bmp\tIcon (*.ico)\t*.ico\t"
	"Cursor (*.cur)\t*.cur\tResource (*.res)\t*.res\t"
	"Executable (*.exe)\t*.exe\tDLL Libraries(*.dll)\t*.dll\t"
	"All Files (*.*)\t*.*\t" )
pick(	WIE_PALETTEFILTER,
	"Palette (*.pal)\t*.pal\tAll Files (*.*)\t*.*\t",
	"Palette (*.pal)\t*.pal\tAll Files (*.*)\t*.*\t" )
pick(	WIE_IMAGEUNTITLED,
	"(Untitled)",
	"(Untitled)" )
pick(	WIE_SAVEIMAGETITLE,
	"Save Image File",
	"Save Image File" )
pick(	WIE_SAVEPALETTETITLE,
	"Save Color Palette File",
	"Save Color Palette File" )
pick(	WIE_OPENIMAGETITLE,
	"Open Image File",
	"Open Image File" )
pick(	WIE_OPENPALETTETITLE,
	"Open Color Palette File",
	"Open Color Palette File" )
	
// DRAWPROC.C
pick(	WIE_QUERYIMAGESAVE,
	"%s is not saved!  Save Now?",
	"%s is not saved!  Save Now?" )
pick(	WIE_CLOSETITLE,
	"Close",
	"Close" )
pick(	WIE_FILENOTSAVED,
	"File not saved",
	"File not saved" )
pick(	WIE_BRUSHSIZEIS,
	"Brush size is %dx%d",
	"Brush size is %dx%d" )

// IESAVE.C
pick(	WIE_SAVEIMAGETITLE2,
	"Save Image",
	"Save Image" )
pick(	WIE_BITMAPSAVEDTO,
	"Bitmap saved to '%s'",
	"Bitmap saved to '%s'" )
pick(	WIE_ICONSAVEDTO,
	"Icon saved to '%s'",
	"Icon saved to '%s'" )
pick(	WIE_CURSORSAVEDTO,
	"Cursor saved to '%s'",
	"Cursor saved to '%s'" )
pick(	WIE_NOSAVE32TO16,
	"You may not save this 32 bit file as a 16 bit file.",
	"You may not save this 32 bit file as a 16 bit file." )
pick(	WIE_NOSAVE16TO32,
	"You may not save this 16 bit file as a 32 bit file.",
	"You may not save this 16 bit file as a 32 bit file." )
pick(	WIE_IMAGESAVEDTO,
	"Image saved to '%s'",
	"Image saved to '%s'" )
pick(	WIE_PALETTESAVEDTO,
	"Palette saved to '%s'",
	"Palette saved to '%s'" )
	
// CLRCNTLS.C
pick(	WIE_SCREENTEXT,
	"Screen:",
	"Screen:" )
pick(	WIE_INVERSETEXT,
	"Inverse:",
	"Inverse:" )
	
// IEPROC.C
pick(	WIE_SYSMENUOPERATIONS,
	"System menu operations",
	"System menu operations" )
pick(	WIE_NEIMAGENOTCREATED,
	"New image not created",
	"New image not created" )
#ifdef __NT__
pick(	WIE_ABOUTTEXT,
	"WATCOM Image Editor for Windows NT",
	"WATCOM Image Editor for Windows NT" )
#else
pick(	WIE_ABOUTTEXT,
	"WATCOM Image Editor for Windows",
	"WATCOM Image Editor for Windows" )
#endif
pick(	WIE_ABOUTFIRSTYEAR,
	"1994",
	"1994" )
pick(	WIE_ABOUTTITLE,
	"About the WATCOM Image Editor",
	"About the WATCOM Image Editor" )

// WINCREAT.C
pick(	WIE_COLORPALETTETITLE,
	"Color Palette",
	"Color Palette" )
pick(	WIE_DRAWTEXT,
	"Draw:",
	"Draw:" )
pick(	WIE_FILLTEXT,
	"Fill:",
	"Fill:" )

// ICONINFO.C
pick(	WIE_16COLOUR32X32,
	"32x32, 16 Color",
	"32x32, 16 Color" )
pick(	WIE_2COLOUR32X32,
	"32x32,  2 Color",
	"32x32,  2 Color" )
pick(	WIE_2COLOUR32X16,
	"32x16,  2 Color",
	"32x16,  2 Color" )
pick(	WIE_16COLOUR16X16,
	"16x16, 16 Color",
	"16x16, 16 Color" )
pick(	WIE_2COLOUR16X16,
	"16x16,  2 Color",
	"16x16,  2 Color" )
pick(	WIE_256COLOUR48x48,
	"48x48, 256 Color",
	"48x48, 256 Color" )
pick(	WIE_16COLOUR48x48,
	"48x48,  16 Color",
	"48x48,  16 Color" )
pick(	WIE_16COLOUR24x24,
	"24x24,  16 Color",
	"24x24,  16 Color" )
pick(	WIE_ICONADDEDTEXT,
	"Icon added to current icon resource",
	"Icon added to current icon resource" )
pick(	WIE_ICONDELETEDTEXT,
	"Icon deleted from current icon resource",
	"Icon deleted from current icon resource" )
pick(	WIE_SELECTTARGETICON,
	"Select the target icon ...",
	"Select the target icon ..." )
pick(	WIE_SELECTICONTODELETE,
	"Select the icon to delete ...",
	"Select the icon to delete ..." )
pick(	WIE_SELECTTYPEOFNEWICON,
	"Select the type of new icon ...",
	"Select the type of new icon ..." )
pick(	WIE_SELECTICONTOEDIT,
	"Select the icon to edit ...",
	"Select the icon to edit ..." )

// TRNSFORM.C
pick(	WIE_FLIPPINGIMAGE,
	"Flipping image ...",
	"Flipping image ..." )
pick(	WIE_IMAGEREFLECTEDH,
	"Image reflected over horizontal axis",
	"Image reflected over horizontal axis" )
pick(	WIE_IMAGEREFLECTEDV,
	"Image reflected over vertical axis",
	"Image reflected over vertical axis" )
pick(	WIE_ROTATIONPERCENT,
	"Rotating image ... %d %% complete",
	"Rotating image ... %d %% complete" )
pick(	WIE_ROTATINGIMAGE,
	"Rotating image ...",
	"Rotating image ..." )
pick(	WIE_IMAGEROTATEDCCW,
	"Image rotated 90 deg. counter clockwise",
	"Image rotated 90 deg. counter clockwise" )
pick(	WIE_IMAGEROTATEDCW,
	"Image rotated 90 deg. clockwise",
	"Image rotated 90 deg. clockwise" )
pick(	WIE_AREACLEARED,
	"Area cleared",
	"Area cleared" )
pick(	WIE_IMAGESHIFTEDLEFT,
	"Image shifted %d pixel(s) left",
	"Image shifted %d pixel(s) left" )
pick(	WIE_IMAGESHIFTEDRIGHT,
	"Image shifted %d pixel(s) right",
	"Image shifted %d pixel(s) right" )
pick(	WIE_IMAGESHIFTEDUP,
	"Image shifted %d pixel(s) up",
	"Image shifted %d pixel(s) up" )
pick(	WIE_IMAGESHIFTEDDOWN,
	"Image shifted %d pixel(s) down",
	"Image shifted %d pixel(s) down" )

// IEOPEN.C
pick(	WIE_OPENEDTEXT,
	"Opened '%s'.",
	"Opened '%s'." )
pick(	WIE_ICONOPENEDTEXT,
	"Opened '%s' (%d icon(s))",
	"Opened '%s' (%d icon(s))" )
pick(	WIE_PALETTELOADEDFROM,
	"Palette loaded from '%s'",
	"Palette loaded from '%s'" )
	
// IENEW.C
pick(	WIE_DIMENSIONSBETWEEN,
	"Dimensions must be between \n %d and %d",
	"Dimensions must be between \n %d and %d" )
pick(	WIE_MONO32X32,
	"Monochrome, 32x32",
	"Monochrome, 32x32" )
pick(	WIE_NOTIMPLEMENTED,
	"Not implemented.",
	"Not implemented." )

// MODCLRS.C
pick(	WIE_COLOURPALETTEMODIFIED,
	"Color Palette modified",
	"Color Palette modified" )
pick(	WIE_COLOURPALETTERESTORED,
	"Color Palette restored",
	"Color Palette restored" )
	
// CHGSIZE.C
pick(	WIE_IMAGESIZEUNCHANGED,
	"Image size unchanged.",
	"Image size unchanged." )
pick(	WIE_RESETUNDOSTACKWARNING,
	"Note that this will reset the undo stack.\nContinue?",
	"Note that this will reset the undo stack.\nContinue?" )
pick(	WIE_INFORMATIONTEXT,
	"Information",
	"Information" )
pick(	WIE_NEWIMAGESIZE,
	"New image size:  %dx%d",
	"New image size:  %dx%d" )
	
// PICKBMP.C
pick(	WIE_INVALIDREGIONSELECTED,
	"An invalid region was selected.",
	"An invalid region was selected." )

// UNDO.C
pick(	WIE_UNDOSTACKEMPTY,
	"Undo stack is empty.",
	"Undo stack is empty." )
pick(	WIE_UNDOITEMSLEFT,
	"%d undo items left.",
	"%d undo items left." )
pick(	WIE_CLOSINGIMAGEPERCENT,
	"Closing image ... %d %% complete",
	"Closing image ... %d %% complete" )
pick(	WIE_REDOSTACKEMPTY,
	"Redo stack is empty.",
	"Redo stack is empty." )
pick(	WIE_REDOITEMSLEFT,
	"%d redo items left.",
	"%d redo items left." )
pick(	WIE_IMAGERESTORED,
	"Image restored",
	"Image restored" )
pick(	WIE_DISCARDINGUNDOS,
	"Discarding undo operations ...",
	"Discarding undo operations ..." )
pick(	WIE_NOUNDOSRECORDED,
	"Memory is low!  No undos recorded.",
	"Memory is low!  No undos recorded." )
	
// CLOSEALL.C
pick(	WIE_EXITTITLE,
	"Exit",
	"Exit" )
pick(	WIE_QUERYIMAGEUPDATE,
	"%s is not saved!  Update?",
	"%s is not saved!  Update?" )
pick(	WIE_NOIMAGESTOCLOSE,
	"No Images to Close.",
	"No Images to Close." )
pick(	WIE_ALLIMAGESCLOSED,
	"All images have been closed",
	"All images have been closed" )
pick(	WIE_FILEHASBEENCLOSED,
	"%s has been closed",
	"%s has been closed" )

