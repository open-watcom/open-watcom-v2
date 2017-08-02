#include "wboolsw.hpp"

WEXPORT WBoolSwitch::WBoolSwitch( WWindow* parent, const WRect& r, char* text, WStyle wstyle )
	: WButton( parent, r, text, wstyle )
{
}

WEXPORT WBoolSwitch::~WBoolSwitch()
{
}

void WEXPORT WBoolSwitch::setCheck( bool check )
{
	sendMsg( BM_SETCHECK, check, 0 );
}

bool WEXPORT WBoolSwitch::checked()
{
	return sendMsg( BM_GETCHECK, 0, 0 ) != 0;
}
