#include "wtest.hpp"

#include "wfiledlg.hpp"
#include "wstring.hpp"
#include "wpshbttn.hpp"
#include "weditbox.hpp"
#include "wtext.hpp"
#include "wcheckbx.hpp"
#include "wgroupbx.hpp"
#include "wradbttn.hpp"
#include "wmsgdlg.hpp"

#include "wlistbox.hpp"
#include "wcombox.hpp"
#include "wmenu.hpp"
#include "wmenuitm.hpp"
#include "wpopmenu.hpp"
#include "wport.hpp"

#include "wserver.hpp"
#include "wclient.hpp"
#include "wtimer.hpp"

#include "wwinmain.hpp"

#include "stdio.h"
#include "stdlib.h"
#include "time.h"

UINT _A_wmain( LPSTR cmd, HINSTANCE inst )
{
	if( WMdiWindow::registerClass() ) {
		AppMain app( cmd, inst );
		return dispatch();
	}
	return 0;
}

WEXPORT AppMain::AppMain( LPSTR /*cmd*/, HINSTANCE inst )
	: WMdiWindow( "WClass Demo" )
{
	WMenu* menu = new WMenu();
	WPopupMenu* pop0 = new WPopupMenu( "&File" );
	menu->insertItem( pop0, 0 );
	pop0->insertItem( new WMenuItem( "item &0" ), 0 );
	pop0->insertItem( new WMenuItem( "item &1" ), 1 );
	pop0->insertItem( new WMenuItem( "&Open", this, (cb)&AppMain::openFile ), 2 );
	pop0->insertSeparator( 3 );
	pop0->insertItem( new WMenuItem( "&quit", this, (cb)&WWindow::close ), 4 );
	menu->insertItem( getMdiPopup(), 1 );
	setMenu( menu );
	_child = new AppChild( this, "Demo Child" );
	_client = new AppClient( this, "Demo Client", inst );
	_server = new AppServer( this, "Demo Server", inst );
//	_server->makeActive();
	show();
}

WEXPORT AppMain::~AppMain()
{
}

void AppMain::openFile()
{
	WFileDialog of( this );
	if( of.getOpenFileName() ) {
	}
}

WEXPORT AppChild::AppChild( AppMain* parent, char* title )
	: WMdiChild( parent, title )
{
	_tt = new WText( this, WRect(5,5,300,18), "timer stuff" );
	_tt->show();

	e1 = new WEditBox( this, WRect(5,55,300,0), "cont", ES_LEFT|WS_BORDER );
	e1->show();

	c1 = new WCheckBox( this, WRect(5,80), "this is a check box" );
	c1->onClick( this, (cb)&AppChild::showText9 );
	c1->setCheck( TRUE );
	c1->show();

	WGroupBox* g1 = new WGroupBox( this, WRect(5,110,100,100), "group box" );
	g1->show();

	WRadioButton* r1 = new WRadioButton( this, WRect(10,130,80,18), "button X" );;
	r1->onClick( this, (cb)&AppChild::showText1 );
	r1->setCheck( TRUE );
	r1->show();
	WRadioButton* r2 = new WRadioButton( this, WRect(10,155,80,18), "button 2" );
	r2->onClick( this, (cb)&AppChild::showText2 );
	r2->show();
	WRadioButton* r3 = new WRadioButton( this, WRect(10,180,80,18), "button 3" );
	r3->onClick( this, (cb)&AppChild::showText3 );
	r3->show();

	_tx = 0;
	t1 = new WText( this, WRect(150,110,200,18), "button 1 selected" );
	showText1();
	t2 = new WText( this, WRect(150,110,200,18), "button 2 selected" );
	t3 = new WText( this, WRect(150,110,200,18), "button 3 selected" );
	t9 = new WText( this, WRect(150,110,200,18), "" );

	lbx = new WListBox( this, WRect(325,5,200,200), "listbox" );
	lbx->insertString( "lbx line zero", 0 );
	lbx->insertString( "lbx line one", 1 );
	lbx->insertString( "lbx line two", 2 );
	lbx->insertString( "lbx line three", 3 );
	lbx->insertString( "lbx line four", 4 );
	lbx->insertString( "lbx line five", 5 );
	lbx->insertString( "lbx line six", 6 );
	lbx->insertString( "lbx line seven", 7 );
	lbx->insertString( "lbx line eight", 8 );
	lbx->insertString( "lbx line nine", 9 );
	lbx->insertString( "lbx line ten", 10 );
	lbx->insertString( "lbx line eleven", 11 );
	lbx->insertString( "lbx line twelve", 12 );
	lbx->onChanged( this, (cb)&AppChild::lbxChanged );
	lbx->select( 0 );
	lbx->show();

	cbx = new WComboBox( this, WRect(325,225,200,200), "combobox" );
	cbx->insertString( "cbx line zero", 0 );
	cbx->insertString( "cbx line one", 1 );
	cbx->insertString( "cbx line two", 2 );
	cbx->insertString( "cbx line three", 3 );
	cbx->insertString( "cbx line four", 4 );
	cbx->insertString( "cbx line five", 5 );
	cbx->insertString( "cbx line six", 6 );
	cbx->insertString( "cbx line seven", 7 );
	cbx->insertString( "cbx line eight", 8 );
	cbx->insertString( "cbx line nine", 9 );
	cbx->insertString( "cbx line ten", 10 );
	cbx->insertString( "cbx line eleven", 11 );
	cbx->insertString( "cbx line twelve", 12 );
	cbx->onChanged( this, (cb)&AppChild::cbxChanged );
	cbx->select( 0 );
	cbx->show();
	activate( TRUE );
	show();

	_timer = new WTimer( this, (cbt)&AppChild::timeTick );
	_timer->start( 1000 );
}

WEXPORT AppChild::~AppChild()
{
	_timer->stop();
}

void WEXPORT AppChild::timeTick( WTimer*, DWORD )
{
	time_t t = time( NULL );
	WString s( ctime( &t ) );
	s[24] = '\0';
	_tt->setText( s );
}

void AppChild::lbxChanged()
{
	e1->setText( (char*)lbx->getString( lbx->selected() ) );
}

void AppChild::cbxChanged()
{
	e1->setText( (char*)cbx->getString( cbx->selected() ) );
}

void AppChild::showText1()
{
	ifptr( _tx ) _tx->hide();
	t1->show();
	_tx = t1;
}

void AppChild::showText2()
{
	ifptr( _tx ) _tx->hide();
	t2->show();
	_tx = t2;
}

void AppChild::showText3()
{
	ifptr( _tx ) _tx->hide();
	t3->show();
	_tx = t3;
}

void AppChild::showText9()
{
	ifptr( _tx ) _tx->hide();
	t9->setText( c1->checked() ? "text box ON" : "text box OFF" );
	t9->show();
	_tx = t9;
}

bool WEXPORT AppChild::paint()
{
	WPort p( this );
	p.textOut( WRect(1,215), "Hello World!" );
	return TRUE;
}

WEXPORT AppClient::AppClient( AppMain* parent, char* title, HINSTANCE inst )
	: WMdiChild( parent, title )
	, _inst( inst )
	, _pop( NIL )
	, _client( NIL )
{
	_pop = new WPopupMenu( "&Client" );
	_pop->insertItem( new WMenuItem( "CreateClient", this, (cb)&AppClient::createClient ), 0 );
	_pop->insertItem( new WMenuItem( "DeleteClient", this, (cb)&AppClient::deleteClient ), 1 );
	_pop->insertSeparator( 2 );
	_pop->insertItem( new WMenuItem( "ConnectClient", this, (cb)&AppClient::connectClient ), 3 );
	_pop->insertItem( new WMenuItem( "DisconnectClient", this, (cb)&AppClient::disconnectClient ), 4 );
	_pop->insertItem( new WMenuItem( "sendMsgClient", this, (cb)&AppClient::sendmsgClient ), 5 );

	_tt = new WText( this, WRect(5,5,300,18), "timer stuff" );
	_tt->show();
	e1 = new WEditBox( this, WRect(5,55,300,0), "client", ES_LEFT|WS_BORDER );
	e1->show();
	activate( TRUE );
	show();
	_timer = new WTimer( this, (cbt)&AppClient::timeTick );
	_timer->start( 100 );
}

WEXPORT AppClient::~AppClient()
{
	_timer->stop();
	delete _pop;
}

void WEXPORT AppClient::timeTick( WTimer*, DWORD )
{
	WString s( 20 );
	static int t=0;
	ultoa( t++, s, 10 );
	_tt->setText( s );
}

void WEXPORT AppClient::activate( bool active )
{
	if( active ) {
		insertPopup( _pop, 2 );
		setText( "Client - GOT IT!" );
	} else {
		removePopup( _pop );
		setText( "Client" );
	}
}

void AppClient::createClient()
{
	ifnil( _client ) {
		_client = new WClient( _inst, this, (cbc)&AppClient::clientNotify );
		e1->setText( "client created" );
		return;
	}
	e1->setText( "client already exists" );
}

void AppClient::deleteClient()
{
	ifptr( _client ) {
		delete _client;
		_client = NIL;
		e1->setText( "client deleted" );
		return;
	}
	e1->setText( "no client exists" );
}

void AppClient::connectClient()
{
	ifptr( _client ) {
		if( _client->connect( "Server99" ) ) {
			e1->setText( "client connected" );
			return;
		}
		e1->setText( "client connected failure" );
		return;
	}
	e1->setText( "no client exists" );
}

void AppClient::disconnectClient()
{
	ifptr( _client ) {
		_client->disconnect();
		e1->setText( "client disconnected" );
		return;
	}
	e1->setText( "no client exists" );
}

void AppClient::sendmsgClient()
{
	ifptr( _client ) {
		WString* reply = _client->sendMsg( "hello there", CS_WANTREPLY );
		ifptr( reply ) {
			e1->setText( (char*)*reply );
			delete reply;
		}
	}
}

void AppClient::clientNotify( char* msg )
{
	e1->setText( msg );
}

WEXPORT AppServer::AppServer( AppMain* parent, char* title, HINSTANCE inst )
	: WMdiChild( parent, title )
	, _inst( inst )
	, _pop( NIL )
	, _server( NIL )
{
	_pop = new WPopupMenu( "&Server" );
	_pop->insertItem( new WMenuItem( "CreateServer", this, (cb)&AppServer::createServer ), 0 );
	_pop->insertItem( new WMenuItem( "DeleteServer", this, (cb)&AppServer::deleteServer ), 1 );

	_tt = new WText( this, WRect(5,5,300,18), "timer stuff" );
	_tt->show();
	e1 = new WEditBox( this, WRect(5,55,300,0), "server", ES_LEFT|WS_BORDER );
	e1->show();
	activate( TRUE );
	show();
	_timer = new WTimer( this, (cbt)&AppServer::timeTick );
	_timer->start( 500 );
}

WEXPORT AppServer::~AppServer()
{
	_timer->stop();
	delete _pop;
}

void WEXPORT AppServer::timeTick( WTimer*, DWORD )
{
	static int t=0;		// AFS set to t=0 and AppServer:vfo is wrong
	WString s( 20 );
	ultoa( t++, s, 10 );
	_tt->setText( s );
}

void WEXPORT AppServer::activate( bool active )
{
	if( active ) {
		insertPopup( _pop, 2 );
		setText( "Server - GOT IT!" );
	} else {
		removePopup( _pop );
		setText( "Server" );
	}
}

void AppServer::createServer()
{
	ifnil( _server ) {
		_server = new WServer( "Server99", _inst, this, (sbc)&AppServer::serverNotify );
		e1->setText( "server created" );
		return;
	}
	e1->setText( "server already exists" );
}

void AppServer::deleteServer()
{
	ifptr( _server ) {
		delete _server;
		_server = NIL;
		e1->setText( "server deleted" );
		return;
	}
	e1->setText( "no server exists" );
}

WString* AppServer::serverNotify( char* msg )
{
	if( strieq( msg, "hello there" ) ) {
		e1->setText( msg );
		WMessageDialog::info( this, "message: %s received", msg );
		WMessageDialog::message( this, MsgError, "Error: %s received", msg );
		WMessageDialog::message( this, MsgWarning, "Warning: %s received", msg );
		WMessageDialog::message( this, MsgInfo, "Info: %s received", msg );
		return new WString( "this is a reply" );
	}
	return NIL;
}
