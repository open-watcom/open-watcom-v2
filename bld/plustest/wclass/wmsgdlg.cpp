#include "wmsgdlg.hpp"
#include "wwindow.hpp"

#include "wstring.hpp"

extern "C" {
	#include <stdio.h>
	#include <stdarg.h>
}

#define MAX_MESSAGE 500

WEXPORT WMessageDialog::WMessageDialog( WWindow* parent, char* text, char* caption, MsgType type )
{
	MessageBox( parent->handle(), text, caption, type );
}

void WEXPORT WMessageDialog::message( WWindow* parent, MsgType type, char* text... )
{
	WString txt( MAX_MESSAGE );
	va_list args;
	va_start( args, text );
	vsprintf( txt, text, args );
	(void)WMessageDialog( parent, txt, NIL, type );
}

void WEXPORT WMessageDialog::info( WWindow* parent, char* text... )
{
	WString txt( MAX_MESSAGE );
	va_list args;
	va_start( args, text );
	vsprintf( txt, text, args );
	(void)WMessageDialog( parent, txt, NIL, MsgOk );
}

