#include "wclient.hpp"

WObjectMap WEXPORT WClient::_convMap;

HDDEDATA _export _far _pascal clientCallback( UINT type, UINT /*fmt*/, HCONV hconv,
		HSZ /*hsz1*/, HSZ /*hsz2*/, HDDEDATA /*hdata*/, DWORD /*dwdata1*/, DWORD /*dwdata2*/ )
{
	WClient* client = (WClient*)WClient::_convMap.findThis( (HANDLE)hconv );
	ifptr( client ) {
		switch( type ) {
		case XTYP_DISCONNECT:
			return (HDDEDATA)client->xtDisconnect();
		}
	}
	return (HDDEDATA)NIL;
}

#define INITFLAGS APPCMD_CLIENTONLY\
				| CBF_SKIP_REGISTRATIONS

WEXPORT WClient::WClient( HINSTANCE inst, WObject* owner, cbc notify )
	: _service( 0 )
	, _owner( owner )
	, _notify( notify )
	, _procid( 0 )
	, _hconv( 0 )
	, _ok( FALSE )
	, _connected( FALSE )
{
	if( !DdeInitialize( &_procid, (PFNCALLBACK)MakeProcInstance(
			(FARPROC)clientCallback, inst ), INITFLAGS, 0L ) ) {
			_ok = TRUE;
	}
}

WEXPORT WClient::~WClient()
{
	DdeUninitialize( _procid );
	_procid = NIL;
}

bool WEXPORT WClient::xtDisconnect()
{
	_connected = FALSE;
	if( isptr( _owner ) && isptr( _notify ) ) {
		(_owner->*_notify)( "disconnect" );
	}
	return FALSE;
}

bool WEXPORT WClient::connect( char* service )
{
	_service = DdeCreateStringHandle( _procid, service, CP_WINANSI );
	_hconv = DdeConnect( _procid, _service, _service, NIL );
	ifptr( _hconv ) {
		_convMap.setThis( this, (HANDLE)_hconv );
		_connected = TRUE;
		return TRUE;
	}
	DdeGetLastError( _procid );
	return FALSE;
}

void WEXPORT WClient::disconnect()
{
	WClient* client = (WClient*)WClient::_convMap.findThis( (HANDLE)_hconv );
	ifptr( client ) {
		DdeDisconnect( _hconv );
		_convMap.clearThis( this );
		_hconv = NIL;
	}
	ifptr( _service ) {
		DdeFreeStringHandle( _procid, _service );
		_service = NIL;
	}
	_connected = FALSE;
}

WString* WEXPORT WClient::sendMsg( char* msg, WClientFlags flags )
{
	HSZ hsz = DdeCreateStringHandle( _procid, msg, CP_WINANSI );
	HDDEDATA hdata = DdeClientTransaction( NIL, 0, _hconv, hsz, CF_TEXT, XTYP_REQUEST, 5000, NIL );
	DdeFreeStringHandle( _procid, hsz );
	ifptr( hdata ) {
		char* rsp = (char*)DdeAccessData( hdata, NIL );
		WString* reply = new WString( rsp );
		DdeFreeDataHandle( hdata );
		if( flags & CS_WANTREPLY ) {
			return reply;
		}
		delete reply;
	}
	return NIL;
}
