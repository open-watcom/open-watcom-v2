/*
 *  afxres.rc   Application Framework standard resources
 *
:include crwatcnt.sp
 */

#ifndef __AFXRES_RC__
#define __AFXRES_RC__

#include <afxres.h>

AFX_IDD_NEWTYPEDLG DIALOG DISCARDABLE 20, 40, 145, 125
STYLE DS_MODALFRAME | DS_SETFONT | WS_POPUP | WS_CAPTION | WS_VISIBLE | WS_SYSMENU
CAPTION "New"
FONT 8, "MS Shell Dlg"
BEGIN
    LTEXT           "&File Type:", IDC_STATIC, 6, 6, 133, 8
    LISTBOX         AFX_IDC_LISTBOX, 6, 20, 133, 85, WS_BORDER | WS_VSCROLL
    DEFPUSHBUTTON   "OK", IDOK, 6, 105, 40, 14
    PUSHBUTTON      "Cancel", IDCANCEL, 99, 105, 40, 14
END

AFX_IDC_CONTEXTHELP CURSOR  DISCARDABLE "res\\help.cur"

STRINGTABLE DISCARDABLE
BEGIN
    AFX_IDS_OPENFILE                "Open"
    AFX_IDS_SAVEFILE                "Save As"
    AFX_IDS_ALLFILTER               "All Files (*.*)"
    AFX_IDS_UNTITLED                "Untitled"
    AFX_IDS_SAVEFILECOPY            "Save Copy As"
    AFX_IDP_NO_ERROR_AVAILABLE      "No error message is available."
    AFX_IDS_NOT_SUPPORTED_EXCEPTION "An unsupported action was attempted."
    AFX_IDS_RESOURCE_EXCEPTION      "A required resource is unavailable."
    AFX_IDS_MEMORY_EXCEPTION        "The system is out of memory."
    AFX_IDS_USER_EXCEPTION          "An unknown error has occurred."
    AFX_IDS_INVALID_ARG_EXCEPTION   "An invalid argument was passed to a function."
    AFX_IDP_INVALID_FILENAME        "Invalid filename."
    AFX_IDP_FAILED_TO_OPEN_DOC      "Failed to open document."
    AFX_IDP_FAILED_TO_SAVE_DOC      "Failed to save document."
    AFX_IDP_ASK_TO_SAVE             "The file '%s' has been modified.  Would you like to save it?"
    AFX_IDP_FAILED_TO_CREATE_DOC    "Failed to create document."
    AFX_IDP_FILE_TOO_LARGE          "The file is too large."
    AFX_IDP_FAILED_TO_START_PRINT   "Failed to start printing."
    AFX_IDP_FAILED_TO_LAUNCH_HELP   "Failed to launch Help."
    AFX_IDP_INTERNAL_FAILURE        "Internal failure."
    AFX_IDP_COMMAND_FAILURE         "Command failure."
    AFX_IDP_FAILED_MEMORY_ALLOC     "A memory allocation has failed."
    AFX_IDP_UNREG_DONE              "File types were successfully unregistered."
    AFX_IDP_UNREG_FAILURE           "Failed to unregister file types."
    AFX_IDP_DLL_LOAD_FAILED         "Failed to load the DLL '%s'."
    AFX_IDP_DLL_BAD_VERSION         "The version of the DLL '%s' is incorrect."
    AFX_IDP_PARSE_INT               "Please type an integer."
    AFX_IDP_PARSE_REAL              "Please type a number."
    AFX_IDP_PARSE_INT_RANGE         "Please type an integer from %1 to %2."
    AFX_IDP_PARSE_REAL_RANGE        "Please type a number from %1 to %2."
    AFX_IDP_PARSE_STRING_SIZE       "Please type at most %1 characters."
    AFX_IDP_PARSE_RADIO_BUTTON      "Please select a radio button."
    AFX_IDP_PARSE_BYTE              "Please type an integer from 0 to 255."
    AFX_IDP_PARSE_UINT              "Please type a positive integer."
    AFX_IDP_PARSE_DATETIME          "Please type a date or time."
    AFX_IDP_PARSE_CURRENCY          "Please type a currency value."
    AFX_IDP_PARSE_GUID              "Please type a globally unique identifier."
    AFX_IDP_PARSE_TIME              "Please type a time."
    AFX_IDP_PARSE_DATE              "Please type a date."
    AFX_IDP_FILE_NONE               "No error has occurred."
    AFX_IDP_FILE_GENERIC            "A file error has occurred."
    AFX_IDP_FILE_NOT_FOUND          "The file '%1' was not found."
    AFX_IDP_FILE_BAD_PATH           "The path '%1' is invalid."
    AFX_IDP_FILE_TOO_MANY_OPEN      "Too many files are open."
    AFX_IDP_FILE_ACCESS_DENIED      "Access denied."
    AFX_IDP_FILE_INVALID_FILE       "The file '%1' is invalid."
    AFX_IDP_FILE_REMOVE_CURRENT     "The directory '%1' could not be removed because it is the current directory."
    AFX_IDP_FILE_DIR_FULL           "The directory is full."
    AFX_IDP_FILE_BAD_SEEK           "An invalid seek operation was attempted."
    AFX_IDP_FILE_HARD_IO            "A hardware I/O error has occurred."
    AFX_IDP_FILE_SHARING            "A sharing violation has occurred."
    AFX_IDP_FILE_LOCKING            "A locking violation has occurred."
    AFX_IDP_FILE_DISKFULL           "The disk is full."
    AFX_IDP_FILE_EOF                "The end of file was reached."
    AFX_IDP_ARCH_NONE               "No error has occurred."
    AFX_IDP_ARCH_GENERIC            "An archive error has occurred."
    AFX_IDP_ARCH_READONLY           "An attempt was made to write the archive '%1' while in read-only mode."
    AFX_IDP_ARCH_ENDOFFILE          "The end of file was reached."
    AFX_IDP_ARCH_WRITEONLY          "An attempt was made to read the archive '%1' while in write-only mode."
    AFX_IDP_ARCH_BADINDEX           "An invalid index was specified."
    AFX_IDP_ARCH_BADCLASS           "An invalid class was specified."
    AFX_IDP_ARCH_BADSCHEMA          "An invalid schema was specified."
END

#endif /* __AFXRES_RC__ */
