#include "wradbttn.hpp"

WEXPORT WRadioButton::WRadioButton( WWindow* parent, const WRect& r, char* text )
	: WBoolSwitch( parent, r, text, BS_AUTORADIOBUTTON )
{
}

WEXPORT WRadioButton::~WRadioButton()
{
}
