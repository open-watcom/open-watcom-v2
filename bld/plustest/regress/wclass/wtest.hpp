
#ifndef appmain_class
#define appmain_class

#include "wmdiwndw.hpp"
#include "wmdichld.hpp"
#include "wstring.hpp"

WCLASS WPopupMenu;
WCLASS WEditBox;
WCLASS WText;
WCLASS WListBox;
WCLASS WComboBox;
WCLASS WCheckBox;
WCLASS WServer;
WCLASS WClient;
WCLASS AppMain;
WCLASS WTimer;

WCLASS AppChild : public WMdiChild
{
	public:
		WEXPORT AppChild( AppMain* parent, char* title );
		WEXPORT ~AppChild();
		void WEXPORT timeTick( WTimer* timer, DWORD sysTime );
		bool WEXPORT paint();
		void	lbxChanged();
		void	cbxChanged();
		void	showText1();
		void	showText2();
		void	showText3();
		void	showText9();
	private:
		WEditBox*	e1;

		WText*		t1;
		WText*		t2;
		WText*		t3;
		WText*		t9;
		WText*		_tx;
		WListBox*	lbx;
		WComboBox*	cbx;
		WCheckBox*	c1;

		WText*		_tt;
		WTimer*		_timer;
};

WCLASS AppClient : public WMdiChild
{
	public:
		WEXPORT AppClient( AppMain* parent, char* title, HANDLE inst );
		WEXPORT ~AppClient();
		void WEXPORT timeTick( WTimer* timer, DWORD sysTime );
		void WEXPORT activate( bool active );
		bool paint() { return FALSE; }
	private:
		HANDLE		_inst;
		WPopupMenu*	_pop;
		WEditBox*	e1;
		WClient*	_client;
		void		createClient();
		void		deleteClient();
		void		connectClient();
		void		disconnectClient();
		void		sendmsgClient();
		void		clientNotify( char* msg );
		WText*		_tt;
		WTimer*		_timer;
};

WCLASS AppServer : public WMdiChild
{
	public:
		WEXPORT AppServer( AppMain* parent, char* title, HANDLE inst );
		WEXPORT ~AppServer();
		void WEXPORT timeTick( WTimer* timer, DWORD sysTime );
		void WEXPORT activate( bool active );
		bool paint() { return FALSE; }
	private:
		HANDLE		_inst;
		WPopupMenu*	_pop;
		WEditBox*	e1;
		WServer*	_server;
		void		createServer();
		void		deleteServer();
		WString*	serverNotify( char* msg );
		WText*		_tt;
		WTimer*		_timer;
};

WCLASS AppMain : public WMdiWindow
{
	public:
		WEXPORT AppMain( LPSTR cmdstr, HANDLE inst );
		WEXPORT ~AppMain();
		void WEXPORT exit();
		void openFile();
		bool paint() { return FALSE; }
	private:
		AppChild*	_child;
		AppClient*	_client;
		AppServer*	_server;
};

#endif //appmain_class

