#include "wpshbttn.hpp"

WEXPORT WPushButton::WPushButton( WWindow* parent, const WRect& r, char* text )
	: WButton( parent, r, text, BS_PUSHBUTTON )
{
}

WEXPORT WPushButton::~WPushButton()
{
}
