#ifndef wclient_class
#define wclient_class

#include <windows.h>
#include <ddeml.h>

#include "wobject.hpp"
#include "wstring.hpp"
#include "wobjmap.hpp"

typedef int WClientFlags;
#define CS_NOFLAGS   0
#define CS_WANTREPLY 1

WCLASS WClient : public WObject
{
	public:
		WEXPORT WClient( HINSTANCE inst, WObject* owner, cbc notify );
		WEXPORT ~WClient();
		bool WEXPORT connected() { return _connected; }
		bool WEXPORT connect( char* service );
		void WEXPORT disconnect();
		WString* WEXPORT sendMsg( char* msg, WClientFlags flags=CS_NOFLAGS );

		bool WEXPORT xtDisconnect();
		static WObjectMap WEXPORT _convMap;
	private:
		HSZ		_service;
		DWORD	_procid;
		HCONV	_hconv;
		WObject* _owner;
		cbc		_notify;
		bool	_ok;
		bool	_connected;
};

#endif