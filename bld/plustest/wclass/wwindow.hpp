#ifndef wwindow_class
#define wwindow_class

extern "C" {
	#include <windows.h>
}

typedef unsigned long   DWORD;
//AFStypedef unsigned int	UINT;
typedef unsigned long 	WStyle;

#include "wobject.hpp"
#include "wobjmap.hpp"
#include "wvlist.hpp"
#include "wrect.hpp"

WCLASS WMenu;
WCLASS WWindow : public WObject
{
	public:
		WEXPORT WWindow();
		WEXPORT WWindow( char* text );
		WEXPORT WWindow( WWindow* parent, char* text, WStyle style=WS_OVERLAPPEDWINDOW );
		WEXPORT WWindow::WWindow( WWindow* parent, char* className, const WRect& r, char*text, WStyle wstyle );
		WEXPORT ~WWindow();
		virtual bool WEXPORT processMsg( UINT msg, UINT wparm, LONG lparm );
		virtual bool WEXPORT processCmd( WORD id, WORD code );
		HWND WEXPORT handle() { return _handle; }
		void WEXPORT addChild( WObject* child );
		void WEXPORT removeChild( WObject* child );
		DWORD WEXPORT sendMsg( WORD msg, WORD wparm, DWORD lparm );
		virtual void WEXPORT close();
		virtual WEXPORT bool paint() { return FALSE; }
		virtual bool WEXPORT confirmClose() { return TRUE; }
		WORD WEXPORT getText( char* textbuf, WORD length );
		WORD WEXPORT setText( char* text );
		virtual void WEXPORT setMenu( WMenu* menu );
		virtual void WEXPORT clearMenu();
		WMenu* WEXPORT menu() { return _menu; }
		void WEXPORT move( const WRect& r );
		WRect WEXPORT getRectangle();
		void WEXPORT show( int style=SW_SHOWNORMAL );
		void WEXPORT hide() { show( SW_HIDE ); }
		void WEXPORT update();

		static bool WEXPORT registerClass();
		static WObjectMap WEXPORT _objMap;
		static char* WEXPORT _appName;
	protected:
		HWND		_handle;
		WWindow*	_parent;
		WMenu*		_menu;
	private:
		HDC 		_dcHandle;
		WVList		_children;
		void	makeWindow( char* className, char* title, WStyle wstyle );
};

#endif //wwindow_class

