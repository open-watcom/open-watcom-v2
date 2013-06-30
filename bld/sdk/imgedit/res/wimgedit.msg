// NB the maximum allowable string length is 128 bytes

// iefonts.c
pick(   WIE_STATUSFONT,
        "Helv.8",
        "Helv.8" )

// iestatus.c
pick(   WIE_STATUSPOSTEXT,
        "Pos:",
        "Pos:" )
pick(   WIE_STATUSSIZETEXT,
        "Size:",
        "Size:" )
// the leading %s MUST remain at the front of the string
pick(   WIE_STATUSPOSINFO,
        "%sPos: %d, %d",
        "%sPos: %d, %d" )
// the leading %s MUST remain at the front of the string
pick(   WIE_STATUSSIZEINFO,
        "%sSize: %dx%d",
        "%sSize: %dx%d" )
// the leading %s MUST remain at the front of the string
pick(   WIE_STATUSHOTSPOTINFO,
        "%sHotSpot: %d, %d",
        "%sHotSpot: %d, %d" )
// the leading %s MUST remain at the front of the string
pick(   WIE_BITMAPIMAGETEXT,
        "%sBitmap: %dx%d, %d Color",
        "%sBitmap: %dx%d, %d Color" )
// the leading %s MUST remain at the front of the string
pick(   WIE_CURSORIMAGETEXT,
        "%sCursor: %dx%d, %d Color",
        "%sCursor: %dx%d, %d Color" )
// the leading %s MUST remain at the front of the string
pick(   WIE_ICONIMAGETEXT,
        "%sIcon: %dx%d, %d Color",
        "%sIcon: %dx%d, %d Color" )

// errors.c
pick(   WIE_ERRORMSGTITLE,
        "Open Watcom Image Editor",
        "Open Watcom Image Editor" )
pick(   WIE_ERR_BITMAPNOTOPENED,
        "Bitmap: '%s' not opened.",
        "Bitmap: '%s' not opened." )
pick(   WIE_ERR_ICONNOTOPENED,
        "Icon: '%s' not opened.",
        "Icon: '%s' not opened." )
pick(   WIE_ERR_CURSORNOTOPENED,
        "Cursor: '%s' not opened.",
        "Cursor: '%s' not opened." )
pick(   WIE_ERR_INVALIDBITMAPFILE,
        "Invalid bitmap file: '%s'",
        "Invalid bitmap file: '%s'" )
pick(   WIE_ERR_INVALIDICONFILE,
        "Invalid icon file: '%s'",
        "Invalid icon file: '%s'" )
pick(   WIE_ERR_INVALIDCURSORFILE,
        "Invalid cursor file: '%s'",
        "Invalid cursor file: '%s'" )
pick(   WIE_ERR_INVALIDBITMAPDATA,
        "File '%s' contains invalid bitmap data.",
        "File '%s' contains invalid bitmap data." )
pick(   WIE_ERR_INVALIDICONDATA,
        "File '%s' contains invalid icon data.",
        "File '%s' contains invalid icon data." )
pick(   WIE_ERR_INVALIDCURSORDATA,
        "File '%s' contained invalid cursor data.",
        "File '%s' contained invalid cursor data." )
pick(   WIE_ERR_INVALIDFILENAME,
        "'%s' is an invalid file name!",
        "'%s' is an invalid file name!" )
pick(   WIE_ERR_FILE_NOT_OPENED,
        "Error opening '%s'",
        "Error opening '%s'" )
pick(   WIE_ERR_STARTUP_FNO,
        "Error opening '%s'",
        "Error opening '%s'" )
pick(   WIE_ERR_FILENOTOPENED,
        "File '%s' not opened.",
        "File '%s' not opened." )
pick(   WIE_ERR_FILENOTSAVED,
        "File '%s' not saved.",
        "File '%s' not saved." )
pick(   WIE_ERR_PALETTENOTSAVED,
        "Palette not saved.",
        "Palette not saved." )
pick(   WIE_ERR_BAD_IMAGE_TYPE,
        "Active image is not an icon.",
        "Active image is not an icon." )
pick(   WIE_ERR_BAD_SELECTION,
        "Invalid icon selection.",
        "Invalid icon selection." )
pick(   WIE_ERR_NOFILLREGION,
        "Could not fill region.",
        "Could not fill region." )
pick(   WIE_DDE_INIT_FAILED,
        "DDE Initialization failed.",
        "DDE Initialization failed." )
    
pick(   WIE_ERR_BITMAP_TOO_BIG,
        "A dimension of this bitmap exceeds 512 pixels.",
        "A dimension of this bitmap exceeds 512 pixels." )
pick(   WIE_ERR_256CLR_BITMAP,
        "Bitmaps with 256 colors are not yet supported.",
        "Bitmaps with 256 colors are not yet supported." )
pick(   WIE_ERR_BAD_BITMAP_FILE,
        "'%s' could not be opened.",
        "'%s' could not be opened." )
pick(   WIE_ERR_TOO_MANY_COLORS,
        "'Cannot edit images with greater than 256 colors.",
        "'Cannot edit images with greater than 256 colors." )
pick(   WIE_ERR_BAD_BITMAP_DATA,
        "File '%s' contains invalid bitmap data!",
        "File '%s' contains invalid bitmap data!" )
pick(   WIE_ERR_BAD_ICON_CLR,
        "Icons must have 2, 16 or 256 colors.",
        "Icons must have 2, 16 or 256 colors." )
pick(   WIE_ERR_BAD_ICON_FILE,
        "'%s' is not a valid icon file!",
        "'%s' is not a valid icon file!" )
pick(   WIE_ERR_BAD_ICON_DATA,
        "File '%s' contains bad icon data!",
        "File '%s' contains bad icon data!" )
pick(   WIE_ERR_BAD_CURSOR_FILE,
        "'%s' is not a valid cursor file!",
        "'%s' is not a valid cursor file!" )
pick(   WIE_ERR_BAD_CURSOR_DATA,
        "File '%s' contained invalid cursor data!",
        "File '%s' contained invalid cursor data!" )
pick(   WIE_ERR_BAD_FILENAME,
        "'%s' is an invalid file name!",
        "'%s' is an invalid file name!" )
pick(   WIE_ERR_BAD_FILE_EXT,
        "'%s' has an unrecognized file extension.",
        "'%s' has an unrecognized file extension." )
pick(   WIE_ERR_SAVE_FAIL,
        "Error writing data for file '%s'.  File not saved",
        "Error writing data for file '%s'.  File not saved" )
pick(   WIE_ERR_PALETTE_NOT16,
        "The color palette must in 16 color mode.",
        "The color palette must in 16 color mode." )
pick(   WIE_ERR_BAD_HWND,
        "%s\nInvalid Window Handle.",
        "%s\nInvalid Window Handle." )
pick(   WIE_ERR_BAD_FILLCASE,
        "%s\nCould not fill region.",
        "%s\nCould not fill region." )
pick(   WIE_ERR_BAD_ICONINDEX,
        "%s\nBad icon index.",
        "%s\nBad icon index." )
pick(   WIE_ERR_BAD_PALFILE,
        "%s is not a valid palette file.",
        "%s is not a valid palette file." )

// hinttext.c
pick(   WIE_SELECTSIMAGETOEDIT,
        "Select which image to edit.",
        "Select which image to edit." )
pick(   WIE_HINT_IMGED_NEW,
        "Create a new image.",
        "Create a new image." )
pick(   WIE_HINT_IMGED_OPEN,
        "Open an image.",
        "Open an image." )
pick(   WIE_HINT_IMGED_SAVE,
        "Save the current image.",
        "Save the current image." )
pick(   WIE_HINT_IMGED_SAVE_AS,
        "Save the current image with a new name.",
        "Save the current image with a new name." )
pick(   WIE_HINT_IMGED_EXIT,
        "Exit the Open Watcom Image Editor.",
        "Exit the Open Watcom Image Editor." )
pick(   WIE_HINT_IMGED_ABOUT,
        "Display program information.",
        "Display program information." )
pick(   WIE_HINT_IMGED_HELP,
        "List Open Watcom Image Editor help topics.",
        "List Open Watcom Image Editor help topics." )
pick(   WIE_HINT_IMGED_HELP_SEARCH,
        "Search for help on a specific topic.",
        "Search for help on a specific topic." )
pick(   WIE_HINT_IMGED_HELP_ON_HELP,
        "Display information on how to use help.",
        "Display information on how to use help." )
pick(   WIE_HINT_IMGED_REST,
        "Restore image to opened form.",
        "Restore image to opened form." )
pick(   WIE_HINT_IMGED_LCOLOR,
        "Load a color palette.",
        "Load a color palette." )
pick(   WIE_HINT_IMGED_SCOLOR,
        "Save the current color palette.",
        "Save the current color palette." )
pick(   WIE_HINT_IMGED_RCOLOR,
        "Restore color palette to default.",
        "Restore color palette to default." )
pick(   WIE_HINT_IMGED_CLOSE,
        "Close the current image.",
        "Close the current image." )
pick(   WIE_HINT_IMGED_ARRANGE,
        "Arrange the icons.",
        "Arrange the icons." )
pick(   WIE_HINT_IMGED_CLOSEALL,
        "Close all images.",
        "Close all images." )
pick(   WIE_HINT_IMGED_COLOR,
        "Toggle display of the color palette.",
        "Toggle display of the color palette." )
pick(   WIE_HINT_IMGED_VIEW,
        "Toggle display of the view window.",
        "Toggle display of the view window." )
pick(   WIE_HINT_IMGED_TOOLBAR,
        "Toggle display of the tool window.",
        "Toggle display of the tool window." )
pick(   WIE_HINT_IMGED_FREEHAND,
        "Draw freehand with pencil.",
        "Draw freehand with pencil." )
pick(   WIE_HINT_IMGED_LINE,
        "Draw lines.",
        "Draw lines." )
pick(   WIE_HINT_IMGED_CIRCLEO,
        "Draw outlined ellipses.",
        "Draw outlined ellipses." )
pick(   WIE_HINT_IMGED_CIRCLEF,
        "Draw filled ellipses.",
        "Draw filled ellipses." )
pick(   WIE_HINT_IMGED_RECTO,
        "Draw outlined rectangles.",
        "Draw outlined rectangles." )
pick(   WIE_HINT_IMGED_RECTF,
        "Draw filled rectangles.",
        "Draw filled rectangles." )
pick(   WIE_HINT_IMGED_FILL,
        "Fill area under the cursor.",
        "Fill area under the cursor." )
pick(   WIE_HINT_IMGED_BRUSH,
        "Draw freehand with wide brush.",
        "Draw freehand with wide brush." )
pick(   WIE_HINT_IMGED_CLIP,
        "Select region to edit.",
        "Select region to edit." )
pick(   WIE_HINT_IMGED_HOTSPOT,
        "Set hotspot location.",
        "Set hotspot location." )
pick(   WIE_HINT_IMGED_2x2,
        "Set brush size to 2x2 pixels.",
        "Set brush size to 2x2 pixels." )
pick(   WIE_HINT_IMGED_3x3,
        "Set brush size to 3x3 pixels.",
        "Set brush size to 3x3 pixels." )
pick(   WIE_HINT_IMGED_4x4,
        "Set brush size to 4x4 pixels.",
        "Set brush size to 4x4 pixels." )
pick(   WIE_HINT_IMGED_5x5,
        "Set brush size to 5x5 pixels.",
        "Set brush size to 5x5 pixels." )
pick(   WIE_HINT_IMGED_TILE,
        "Tile image windows.",
        "Tile image windows." )
pick(   WIE_HINT_IMGED_NEWIMG,
        "Add an icon to current icon file.",
        "Add an icon to current icon file." )
pick(   WIE_HINT_IMGED_DELIMG,
        "Delete an icon from current icon file.",
        "Delete an icon from current icon file." )
pick(   WIE_HINT_IMGED_CASCADE,
        "Cascade all the image windows.",
        "Cascade all the image windows." )
pick(   WIE_HINT_IMGED_GRID,
        "Toggle display of the grid.",
        "Toggle display of the grid." )
pick(   WIE_HINT_IMGED_CLEAR,
        "Clear image or selected area.",
        "Clear image or selected area." )
pick(   WIE_HINT_IMGED_UNDO,
        "Undo the previous operation.",
        "Undo the previous operation." )
pick(   WIE_HINT_IMGED_REDO,
        "Reperform an undone operation.",
        "Reperform an undone operation." )
pick(   WIE_HINT_IMGED_CUT,
        "Cut selected area to clipboard.",
        "Cut selected area to clipboard." )
pick(   WIE_HINT_IMGED_COPY,
        "Copy selected area to clipboard.",
        "Copy selected area to clipboard." )
pick(   WIE_HINT_IMGED_PASTE,
        "Paste clipboard contents into current image.",
        "Paste clipboard contents into current image." )
pick(   WIE_HINT_IMGED_SNAP,
        "Capture a bitmap from the desktop.",
        "Capture a bitmap from the desktop." )
pick(   WIE_HINT_IMGED_RIGHT,
        "Shift the image right.",
        "Shift the image right." )
pick(   WIE_HINT_IMGED_LEFT,
        "Shift the image left.",
        "Shift the image left." )
pick(   WIE_HINT_IMGED_UP,
        "Shift the image up.",
        "Shift the image up." )
pick(   WIE_HINT_IMGED_DOWN,
        "Shift the image down.",
        "Shift the image down." )
pick(   WIE_HINT_IMGED_SELIMG,
        "Select icon from a multiple icon file.",
        "Select icon from a multiple icon file." )
pick(   WIE_HINT_IMGED_SQUARE,
        "Maintain a square grid.",
        "Maintain a square grid." )
pick(   WIE_HINT_IMGED_SIZE,
        "Change the size of the current image.",
        "Change the size of the current image." )
pick(   WIE_HINT_IMGED_SETTINGS,
        "Display the current settings.",
        "Display the current settings." )
pick(   WIE_HINT_IMGED_FLIPHORZ,
        "Flip image over horizontal axis.",
        "Flip image over horizontal axis." )
pick(   WIE_HINT_IMGED_FLIPVERT,
        "Flip image over vertical axis.",
        "Flip image over vertical axis." )
pick(   WIE_HINT_IMGED_ROTATECC,
        "Rotate image counterclockwise.",
        "Rotate image counterclockwise." )
pick(   WIE_HINT_IMGED_ROTATECL,
        "Rotate image clockwise.",
        "Rotate image clockwise." )
pick(   WIE_HINT_IMGED_CEDIT,
        "Edit the current left color.",
        "Edit the current left color." )
pick(   WIE_HINT_IMGED_CRESET,
        "Restore palette to unedited form.",
        "Restore palette to unedited form." )
pick(   WIE_HINT_IMGED_CSCREEN,
        "Set color to represent screen/inverse.",
        "Set color to represent screen/inverse." )
pick(   WIE_HINT_IMGED_MAXIMIZE,
        "Maximize the current edit window.",
        "Maximize the current edit window." )
pick(   WIE_HINT_IMGED_DDE_UPDATE_PRJ,
        "Update the file with this resource.",
        "Update the file with this resource." )
    
pick(   WIE_HINT_FILEMENU,
        "File operations.",
        "File operations." )
pick(   WIE_HINT_EDITMENU,
        "Edit operations for current image.",
        "Edit operations for current image." )
pick(   WIE_HINT_SHIFTMENU,
        "Shift the image.",
        "Shift the image." )
pick(   WIE_HINT_MIRRORMENU,
        "Create a mirror image.",
        "Create a mirror image." )
pick(   WIE_HINT_ROTATEMENU,
        "Rotate image 90 degrees.",
        "Rotate image 90 degrees." )
pick(   WIE_HINT_SETTINGSMENU,
        "Setting for image editor session.",
        "Setting for image editor session." )
pick(   WIE_HINT_BRUSHSIZEMENU,
        "Set the size for the paint brush.",
        "Set the size for the paint brush." )
pick(   WIE_HINT_PALETTEMENU,
        "Perform color palette operations.",
        "Perform color palette operations." )
pick(   WIE_HINT_DRAWTOOLSMENU,
        "Select tools with which to draw.",
        "Select tools with which to draw." )
pick(   WIE_HINT_ELLIPSEMENU,
        "Ellipse drawing tool.",
        "Ellipse drawing tool." )
pick(   WIE_HINT_RECTANGLEMENU,
        "Rectangle drawing tool.",
        "Rectangle drawing tool." )
pick(   WIE_HINT_HELPMENU,
        "Provides help for Open Watcom Image Editor.",
        "Provides help for Open Watcom Image Editor." )
    
// SNAP.C
pick(   WIE_BMPSNAPPEDFROMDSKTOP,
        "Bitmap snapped from desktop",
        "Bitmap snapped from desktop" )
pick(   WIE_BMPSNAPABORTED,
        "Snap aborted",
        "Snap aborted" )

// IEVIEWIN.C
pick(   WIE_USEOPTIONSTOHIDEALL,
        "Use options menu to hide all",
        "Use options menu to hide all" )

// BKCOLOUR.C
pick(   WIE_NEWBKCOLORSELECTED,
        "New background color selected",
        "New background color selected" )

// COLOURS.C
pick(   WIE_PALETTERESTORED,
        "Color palette restored",
        "Color palette restored" )

// SETTINGS.C
pick(   WIE_SHIFTBETWEEN,
        "Shift amount must be between \n %d and %d",
        "Shift amount must be between \n %d and %d" )
pick(   WIE_NOTE,
        "NOTE",
        "NOTE" )

// IEDRAW.C
pick(   WIE_GRIDTURNEDOFF,
        "Grid turned off",
        "Grid turned off" )
pick(   WIE_GRIDTURNEDON,
        "Grid turned on",
        "Grid turned on" )
pick(   WIE_NEWBRUSHSIZE,
        "New brush size: %dx%d",
        "New brush size: %dx%d" )
pick(   WIE_NEWGRIDSIZE,
        "Grid size is %dx%d pixels.",
        "Grid size is %dx%d pixels." )
pick(   WIE_SQUAREGRIDOFF,
        "Square grid option turned off",
        "Square grid option turned off" )
pick(   WIE_SQUAREGRIDON,
        "Square grid option turned on",
        "Square grid option turned on" )

// IEDDE.C
pick(   WIE_DDEINITTITLE,
        "DDE Initialization",
        "DDE Initialization" )
pick(   WIE_DDETERMINATIONMSG,
        "The Image Editor is being terminated and \nwas unable to inform the Resource Editor.\nThis may cause the Resource Editor to reject\nany more attempts to edit any resources.\n",
        "The Image Editor is being terminated and \nwas unable to inform the Resource Editor.\nThis may cause the Resource Editor to reject\nany more attempts to edit any resources.\n" )

// CLIP.C
pick(   WIE_ENTIREIMAGECOPIED,
        "Entire image copied to clipboard",
        "Entire image copied to clipboard" )
pick(   WIE_AREACOPIED,
        "Area copied to clipboard",
        "Area copied to clipboard" )
pick(   WIE_NOBITMAPINCLIPBOARD,
        "No bitmap in the clipboard",
        "No bitmap in the clipboard" )
pick(   WIE_CLIPBOARDBITMAPSIZE,
        "Clipboard bitmap size is %dx%d",
        "Clipboard bitmap size is %dx%d" )
pick(   WIE_BITMAPPASTED,
        "Bitmap pasted from clipboard",
        "Bitmap pasted from clipboard" )
pick(   WIE_ENTIREIMAGECUT,
        "Entire image cut to clipboard",
        "Entire image cut to clipboard" )
pick(   WIE_AREACUT,
        "Area cut to clipboard",
        "Area cut to clipboard" )
pick(   WIE_PASTECANCELLED,
        "Paste cancelled",
        "Paste cancelled" )

// IEGLOB.C
pick(   WIE_APPNAME,
        "Open Watcom Image Editor",
        "Open Watcom Image Editor" )
pick(   WIE_IMAGEFILTER,
        "Bitmap (*.bmp)\t*.bmp\tIcon (*.ico)\t*.ico\t"
        "Cursor (*.cur)\t*.cur\tResource (*.res)\t*.res\t"
        "Executable (*.exe)\t*.exe\tDLL Libraries(*.dll)\t*.dll\t"
        "All Files (*.*)\t*.*\t",
        "Bitmap (*.bmp)\t*.bmp\tIcon (*.ico)\t*.ico\t"
        "Cursor (*.cur)\t*.cur\tResource (*.res)\t*.res\t"
        "Executable (*.exe)\t*.exe\tDLL Libraries(*.dll)\t*.dll\t"
        "All Files (*.*)\t*.*\t" )
pick(   WIE_PALETTEFILTER,
        "Palette (*.pal)\t*.pal\tAll Files (*.*)\t*.*\t",
        "Palette (*.pal)\t*.pal\tAll Files (*.*)\t*.*\t" )
pick(   WIE_IMAGEUNTITLED,
        "untitled",
        "untitled" )
pick(   WIE_SAVEIMAGETITLE,
        "Save Image File",
        "Save Image File" )
pick(   WIE_SAVEPALETTETITLE,
        "Save Color Palette File",
        "Save Color Palette File" )
pick(   WIE_OPENIMAGETITLE,
        "Open Image File",
        "Open Image File" )
pick(   WIE_OPENPALETTETITLE,
        "Open Color Palette File",
        "Open Color Palette File" )
    
// DRAWPROC.C
pick(   WIE_QUERYIMAGESAVE,
        "%s is not saved!  Save Now?",
        "%s is not saved!  Save Now?" )
pick(   WIE_CLOSETITLE,
        "Close",
        "Close" )
pick(   WIE_FILENOTSAVED,
        "File not saved",
        "File not saved" )
pick(   WIE_BRUSHSIZEIS,
        "Brush size is %dx%d",
        "Brush size is %dx%d" )

// IESAVE.C
pick(   WIE_SAVEIMAGETITLE2,
        "Save Image",
        "Save Image" )
pick(   WIE_BITMAPSAVEDTO,
        "Bitmap saved to '%s'",
        "Bitmap saved to '%s'" )
pick(   WIE_ICONSAVEDTO,
        "Icon saved to '%s'",
        "Icon saved to '%s'" )
pick(   WIE_CURSORSAVEDTO,
        "Cursor saved to '%s'",
        "Cursor saved to '%s'" )
pick(   WIE_NOSAVE32TO16,
        "You may not save this 32 bit file as a 16 bit file.",
        "You may not save this 32 bit file as a 16 bit file." )
pick(   WIE_NOSAVE16TO32,
        "You may not save this 16 bit file as a 32 bit file.",
        "You may not save this 16 bit file as a 32 bit file." )
pick(   WIE_IMAGESAVEDTO,
        "Image saved to '%s'",
        "Image saved to '%s'" )
pick(   WIE_PALETTESAVEDTO,
        "Palette saved to '%s'",
        "Palette saved to '%s'" )
    
// CLRCNTLS.C
pick(   WIE_SCREENTEXT,
        "Screen:",
        "Screen:" )
pick(   WIE_INVERSETEXT,
        "Inverse:",
        "Inverse:" )
    
// IEPROC.C
pick(   WIE_SYSMENUOPERATIONS,
        "System menu operations",
        "System menu operations" )
pick(   WIE_NEIMAGENOTCREATED,
        "New image not created",
        "New image not created" )
pick(   WIE_ABOUTTEXT,
        "Open Watcom Image Editor for Windows" STR_BITNEES,
        "Open Watcom Image Editor for Windows" STR_BITNEES )
pick(   WIE_ABOUTVERSION,
        banner1p2( _RESEDIT_VERSION_ ),
        banner1p2( _RESEDIT_VERSION_ ) )
pick(   WIE_ABOUTFIRSTYEAR,
        "2002",
        "2002" )
pick(   WIE_ABOUTTITLE,
        "About Open Watcom Image Editor",
        "About Open Watcom Image Editor" )

// WINCREAT.C
pick(   WIE_COLORPALETTETITLE,
        "Color Palette",
        "Color Palette" )
pick(   WIE_DRAWTEXT,
        "Draw:",
        "Draw:" )
pick(   WIE_FILLTEXT,
        "Fill:",
        "Fill:" )

// ICONINFO.C
pick(   WIE_16COLOR32X32,
        "32x32, 16 Color",
        "32x32, 16 Color" )
pick(   WIE_2COLOR32X32,
        "32x32,  2 Color",
        "32x32,  2 Color" )
pick(   WIE_2COLOR32X16,
        "32x16,  2 Color",
        "32x16,  2 Color" )
pick(   WIE_16COLOR16X16,
        "16x16, 16 Color",
        "16x16, 16 Color" )
pick(   WIE_2COLOR16X16,
        "16x16,  2 Color",
        "16x16,  2 Color" )
pick(   WIE_256COLOR48X48,
        "48x48, 256 Color",
        "48x48, 256 Color" )
pick(   WIE_16COLOR48X48,
        "48x48,  16 Color",
        "48x48,  16 Color" )
pick(   WIE_16COLOR24X24,
        "24x24,  16 Color",
        "24x24,  16 Color" )

pick(   WIE_256COLOR32X32,
        "32x32, 256 Color",
        "32x32, 256 Color" )
pick(   WIE_256COLOR16X16,
        "16x16, 256 Color",
        "16x16, 256 Color" )

pick(   WIE_2COLOR64X64,
        "64x64,   2 Color",
        "64x64,   2 Color" )
pick(   WIE_16COLOR64X64,
        "64x64,  16 Color",
        "64x64,  16 Color" )
pick(   WIE_256COLOR64X64,
        "64x64, 256 Color",
        "64x64, 256 Color" )
pick(   WIE_2COLOR96X96,
        "96x96,   2 Color",
        "96x96,   2 Color" )
pick(   WIE_16COLOR96X96,
        "96x96,  16 Color",
        "96x96,  16 Color" )
pick(   WIE_256COLOR96X96,
        "96x96, 256 Color",
        "96x96, 256 Color" )

pick(   WIE_TRUECOLOR32X32,
        "32x32, True Color",
        "32x32, True Color" )
pick(   WIE_TRUECOLOR16X16,
        "16x16, True Color",
        "16x16, True Color" )
pick(   WIE_TRUECOLOR48X48,
        "48x48, True Color",
        "48x48, True Color" )

pick(   WIE_ICONADDEDTEXT,
        "Icon added to current icon resource",
        "Icon added to current icon resource" )
pick(   WIE_ICONDELETEDTEXT,
        "Icon deleted from current icon resource",
        "Icon deleted from current icon resource" )
pick(   WIE_SELECTTARGETICON,
        "Select the target icon ...",
        "Select the target icon ..." )
pick(   WIE_SELECTICONTODELETE,
        "Select the icon to delete ...",
        "Select the icon to delete ..." )
pick(   WIE_SELECTTYPEOFNEWICON,
        "Select the type of new icon ...",
        "Select the type of new icon ..." )
pick(   WIE_SELECTICONTOEDIT,
        "Select the icon to edit ...",
        "Select the icon to edit ..." )

// TRNSFORM.C
pick(   WIE_FLIPPINGIMAGE,
        "Flipping image ...",
        "Flipping image ..." )
pick(   WIE_IMAGEREFLECTEDH,
        "Image reflected over horizontal axis",
        "Image reflected over horizontal axis" )
pick(   WIE_IMAGEREFLECTEDV,
        "Image reflected over vertical axis",
        "Image reflected over vertical axis" )
pick(   WIE_ROTATIONPERCENT,
        "Rotating image ... %d %% complete",
        "Rotating image ... %d %% complete" )
pick(   WIE_ROTATINGIMAGE,
        "Rotating image ...",
        "Rotating image ..." )
pick(   WIE_IMAGEROTATEDCCW,
        "Image rotated 90 deg. counter clockwise",
        "Image rotated 90 deg. counter clockwise" )
pick(   WIE_IMAGEROTATEDCW,
        "Image rotated 90 deg. clockwise",
        "Image rotated 90 deg. clockwise" )
pick(   WIE_AREACLEARED,
        "Area cleared",
        "Area cleared" )
pick(   WIE_IMAGESHIFTEDLEFT,
        "Image shifted %d pixel(s) left",
        "Image shifted %d pixel(s) left" )
pick(   WIE_IMAGESHIFTEDRIGHT,
        "Image shifted %d pixel(s) right",
        "Image shifted %d pixel(s) right" )
pick(   WIE_IMAGESHIFTEDUP,
        "Image shifted %d pixel(s) up",
        "Image shifted %d pixel(s) up" )
pick(   WIE_IMAGESHIFTEDDOWN,
        "Image shifted %d pixel(s) down",
        "Image shifted %d pixel(s) down" )

// IEOPEN.C
pick(   WIE_OPENEDTEXT,
        "Opened '%s'.",
        "Opened '%s'." )
pick(   WIE_ICONOPENEDTEXT,
        "Opened '%s' (%d icon(s))",
        "Opened '%s' (%d icon(s))" )
pick(   WIE_PALETTELOADEDFROM,
        "Palette loaded from '%s'",
        "Palette loaded from '%s'" )
    
// IENEW.C
pick(   WIE_DIMENSIONSBETWEEN,
        "Dimensions must be between \n %d and %d",
        "Dimensions must be between \n %d and %d" )
pick(   WIE_MONO32X32,
        "Monochrome, 32x32",
        "Monochrome, 32x32" )
pick(   WIE_NOTIMPLEMENTED,
        "Not implemented.",
        "Not implemented." )

// MODCLRS.C
pick(   WIE_COLORPALETTEMODIFIED,
        "Color Palette modified",
        "Color Palette modified" )
pick(   WIE_COLORPALETTERESTORED,
        "Color Palette restored",
        "Color Palette restored" )
    
// CHGSIZE.C
pick(   WIE_IMAGESIZEUNCHANGED,
        "Image size unchanged.",
        "Image size unchanged." )
pick(   WIE_RESETUNDOSTACKWARNING,
        "Note that this will reset the undo stack.\nContinue?",
        "Note that this will reset the undo stack.\nContinue?" )
pick(   WIE_INFORMATIONTEXT,
        "Information",
        "Information" )
pick(   WIE_NEWIMAGESIZE,
        "New image size:  %dx%d",
        "New image size:  %dx%d" )
    
// PICKBMP.C
pick(   WIE_INVALIDREGIONSELECTED,
        "An invalid region was selected.",
        "An invalid region was selected." )

// UNDO.C
pick(   WIE_UNDOSTACKEMPTY,
        "Undo stack is empty.",
        "Undo stack is empty." )
pick(   WIE_UNDOITEMSLEFT,
        "%d undo items left.",
        "%d undo items left." )
pick(   WIE_CLOSINGIMAGEPERCENT,
        "Closing image ... %d %% complete",
        "Closing image ... %d %% complete" )
pick(   WIE_REDOSTACKEMPTY,
        "Redo stack is empty.",
        "Redo stack is empty." )
pick(   WIE_REDOITEMSLEFT,
        "%d redo items left.",
        "%d redo items left." )
pick(   WIE_IMAGERESTORED,
        "Image restored",
        "Image restored" )
pick(   WIE_DISCARDINGUNDOS,
        "Discarding undo operations ...",
        "Discarding undo operations ..." )
pick(   WIE_NOUNDOSRECORDED,
        "Memory is low!  No undos recorded.",
        "Memory is low!  No undos recorded." )
    
// CLOSEALL.C
pick(   WIE_EXITTITLE,
        "Exit",
        "Exit" )
pick(   WIE_QUERYIMAGEUPDATE,
        "%s is not saved!  Update?",
        "%s is not saved!  Update?" )
pick(   WIE_NOIMAGESTOCLOSE,
        "No Images to Close.",
        "No Images to Close." )
pick(   WIE_ALLIMAGESCLOSED,
        "All images have been closed",
        "All images have been closed" )
pick(   WIE_FILEHASBEENCLOSED,
        "%s has been closed",
        "%s has been closed" )

// FUNCBAR.C
pick(   WIE_TIP_NEW,
        "New",
        "New" )
pick(   WIE_TIP_OPEN,
        "Open",
        "Open" )
pick(   WIE_TIP_SAVE,
        "Save",
        "Save" )
pick(   WIE_TIP_GRID,
        "Grid",
        "Grid" )
pick(   WIE_TIP_MAXIMIZE,
        "Maximize Current Image",
        "Maximize Current Image" )
pick(   WIE_TIP_CUT,
        "Cut",
        "Cut" )
pick(   WIE_TIP_COPY,
        "Copy",
        "Copy" )
pick(   WIE_TIP_PASTE,
        "Paste",
        "Paste" )
pick(   WIE_TIP_UNDO,
        "Undo",
        "Undo" )
pick(   WIE_TIP_REDO,
        "Redo",
        "Redo" )
pick(   WIE_TIP_CLEAR,
        "Clear",
        "Clear" )
pick(   WIE_TIP_SNAP,
        "Snap Bitmap",
        "Snap Bitmap" )
pick(   WIE_TIP_RIGHT,
        "Shift Right",
        "Shift Right" )
pick(   WIE_TIP_LEFT,
        "Shift Left",
        "Shift Left" )
pick(   WIE_TIP_UP,
        "Shift Up",
        "Shift Up" )
pick(   WIE_TIP_DOWN,
        "Shift Down",
        "Shift Down" )
pick(   WIE_TIP_FLIPHORZ,
        "Flip Over Horizontal Axis",
        "Flip Over Horizontal Axis" )
pick(   WIE_TIP_FLIPVERT,
        "Flip Over Vertical Axis",
        "Flip Over Vertical Axis" )
pick(   WIE_TIP_ROTATECL,
        "Rotate Clockwise",
        "Rotate Clockwise" )
pick(   WIE_TIP_ROTATECC,
        "Rotate Counterclockwise",
        "Rotate Counterclockwise" )
pick(   WIE_TIP_UPDATE,
        "Update",
        "Update" )
