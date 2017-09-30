#ifndef wradiobutton_class
#define wradionbutton_class

#include "wboolsw.hpp"

WCLASS WRadioButton : public WBoolSwitch
{
	public:
		WEXPORT WRadioButton( WWindow* parent, const WRect& r, char* text=NIL );
		WEXPORT ~WRadioButton();
};

#endif //wradiobutton_class
