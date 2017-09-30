#ifndef wmdiwindow_class
#define wmdiwindow_class

#include "wwindow.hpp"

WCLASS WPopupMenu;
WCLASS WMdiWindow : public WWindow
{
	public:
		WEXPORT WMdiWindow( char* text );
		WEXPORT ~WMdiWindow();
		void WEXPORT createClient( HWND hwin );
		DWORD WEXPORT sendClientMsg( WORD msg, WORD wparm, DWORD lparm );
		HWND WEXPORT clientHandle() { return _clientHandle; }
		void WEXPORT setMenu( WMenu* menu );
		void WEXPORT clearMenu();
		void WEXPORT insertPopup( WPopupMenu* pop, int index );
		void WEXPORT removePopup( WPopupMenu* pop );
		WPopupMenu* WEXPORT getMdiPopup();
		void WEXPORT cascadeChildren();
		void WEXPORT tileChildren();
		void WEXPORT arrangeIcons();
		static bool WEXPORT registerClass();
	private:
		HWND		_clientHandle;
		WPopupMenu*	_winPopup;
};

#endif //wmdiwindow_class

