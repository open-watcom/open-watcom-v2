#include "wcheckbx.hpp"

WEXPORT WCheckBox::WCheckBox( WWindow* parent, const WRect& r, char* text )
	: WBoolSwitch( parent, r, text, BS_AUTOCHECKBOX )
{
}

WEXPORT WCheckBox::~WCheckBox()
{
}
