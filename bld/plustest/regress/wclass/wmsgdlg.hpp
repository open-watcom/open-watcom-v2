#ifndef wmessagedialog_class
#define wmessagedialog_class

extern "C" {
	#include <windows.h>
	#include "commdlg.h"
	#include "cderr.h"
}

#include "wdialog.hpp"

typedef enum msgType {
	  MsgOk			= MB_OK
	, MsgError		= MB_OK | MB_ICONSTOP
	, MsgWarning	= MB_OK | MB_ICONEXCLAMATION
	, MsgInfo		= MB_OK | MB_ICONINFORMATION
} MsgType;

WCLASS WWindow;
WCLASS WMessageDialog  : public WDialog
{
	public:
		WMessageDialog( WWindow* parent, char* text, char* caption, MsgType type );
		~WMessageDialog() {}
		static void WEXPORT message( WWindow* parent, MsgType type, char* text... );
		static void WEXPORT info( WWindow* parent, char* text... );
};

#endif
