/*
 *  afxprint.rc Application Framework printing resources
 *
:include crwatcnt.sp
 */

#ifndef __AFXPRINT_RC__
#define __AFXPRINT_RC__

#include <afxres.h>

AFX_IDD_PRINTDLG DIALOG DISCARDABLE 20, 40, 125, 100
STYLE DS_MODALFRAME | DS_SETFONT | WS_POPUP | WS_CAPTION | WS_VISIBLE | WS_SYSMENU
CAPTION "Printing"
FONT 8, "MS Shell Dlg"
BEGIN
    CTEXT           "Printing", IDC_STATIC, 6, 6, 113, 8
    LTEXT           "Document:", IDC_STATIC, 6, 20, 40, 8
    LTEXT           "", AFX_IDC_PRINT_DOCNAME, 50, 20, 69, 8
    LTEXT           "Printer:", IDC_STATIC, 6, 34, 40, 8
    LTEXT           "", AFX_IDC_PRINT_PRINTERNAME, 50, 34, 69, 8
    LTEXT           "Port:", IDC_STATIC, 6, 48, 40, 8
    LTEXT           "", AFX_IDC_PRINT_PORTNAME, 50, 48, 69, 8
    LTEXT           "Page:", IDC_STATIC, 6, 62, 40, 8
    LTEXT           "", AFX_IDC_PRINT_PAGENUM, 50, 62, 69, 8
    PUSHBUTTON      "Cancel", IDCANCEL, 42, 80, 40, 14
END

AFX_IDD_PREVIEW_TOOLBAR DIALOG DISCARDABLE 0, 0, 296, 20
STYLE WS_CHILD | DS_SETFONT
FONT 8, "MS Shell Dlg"
BEGIN
    PUSHBUTTON  "&Print...", AFX_ID_PREVIEW_PRINT, 2, 2, 40, 16
    PUSHBUTTON  "&Next Page", AFX_ID_PREVIEW_NEXT, 44, 2, 40, 16
    PUSHBUTTON  "Pre&v Page", AFX_ID_PREVIEW_PREV, 86, 2, 40, 16
    PUSHBUTTON  "&One Page", AFX_ID_PREVIEW_NUMPAGE, 128, 2, 40, 16
    PUSHBUTTON  "Zoom &In", AFX_ID_PREVIEW_ZOOMIN, 170, 2, 40, 16
    PUSHBUTTON  "Zoom &Out", AFX_ID_PREVIEW_ZOOMOUT, 212, 2, 40, 16
    PUSHBUTTON  "&Close", AFX_ID_PREVIEW_CLOSE, 254, 2, 40, 16
END

AFX_IDC_MAGNIFY CURSOR  DISCARDABLE "res\\magnify.cur"

STRINGTABLE DISCARDABLE
BEGIN
    AFX_IDS_ONEPAGE         "&One Page"
    AFX_IDS_TWOPAGE         "&Two Page"
    AFX_IDS_PREVIEWPAGEDESC "Page %u\nPages %u-%u"
END

#endif /* __AFXPRINT_RC__ */
