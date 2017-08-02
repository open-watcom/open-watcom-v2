#include "wserver.hpp"

static WServer* _server = NIL;

HDDEDATA _export _far _pascal serverCallback( UINT type, UINT fmt, HCONV /*hconv*/,
		HSZ hsz1, HSZ hsz2, HDDEDATA /*hdata*/, DWORD /*dwdata1*/, DWORD /*dwdata2*/ )
{
	ifptr( _server ) {
		switch( type ) {
		case XTYP_REGISTER:
			return (HDDEDATA)TRUE;
		case XTYP_CONNECT:
			return (HDDEDATA)_server->xtConnect( hsz1, hsz2 );
		case XTYP_REQUEST:
			return (HDDEDATA)_server->xtRequest( fmt, hsz1, hsz2 );
		}
	}
	return (HDDEDATA)NIL;
}

#define INITFLAGS APPCMD_FILTERINITS\
				| CBF_FAIL_ADVISES\
				| CBF_FAIL_EXECUTES\
				| CBF_FAIL_POKES\
				| CBF_SKIP_CONNECT_CONFIRMS\
				| CBF_SKIP_REGISTRATIONS\
				| CBF_SKIP_UNREGISTRATIONS

WEXPORT WServer::WServer( char* service, HINSTANCE inst, WObject* owner, sbc notify )
	: _procid( 0 )
	, _service( 0 )
	, _ok( FALSE )
{
	ifnil( _server ) {
		_server = this;
		_owner = owner;
		_notify = notify;
		if( !DdeInitialize( &_procid, (PFNCALLBACK)MakeProcInstance(
				(FARPROC)serverCallback, inst ), INITFLAGS, 0L ) ) {
			_service = DdeCreateStringHandle( _procid, service, CP_WINANSI );
			if( DdeNameService( _procid, _service, (HSZ)NIL, DNS_REGISTER ) ) {
				_ok = TRUE;
			}
		}
	}
}

WEXPORT WServer::~WServer()
{
	_server = NIL;
	if( DdeNameService( _procid, _service, (HSZ)NIL, DNS_UNREGISTER ) ) {
	}
	DdeFreeStringHandle( _procid, _service );
	DdeUninitialize( _procid );
	_procid = NIL;
	_service = NIL;
}

bool WEXPORT WServer::xtConnect( HSZ hservice, HSZ /*topic*/ )
{
	if( hservice == _service ) {
		return TRUE;
	}
	return FALSE;
}

HDDEDATA WEXPORT WServer::xtRequest( UINT fmt, HSZ /*htopic*/, HSZ hitem )
{
	if( isptr( _owner ) && isptr( _notify ) ) {
		int len = DdeQueryString( _procid, hitem, (LPSTR)NIL, 0L, CP_WINANSI );
		WString* request = new WString( len );
		DdeQueryString( _procid, hitem, (char*)*request, len+1, CP_WINANSI );
		WString* reply = (_owner->*_notify)( (char*)*request );
		HDDEDATA hdata = NIL;
		ifptr( reply ) {
			hdata = DdeCreateDataHandle( _procid, (unsigned char*)((char*)*reply), reply->size()+1, 0, hitem, fmt, FALSE );
			delete reply;
		}
		delete request;
		return hdata;
	}
	return NIL;
}
