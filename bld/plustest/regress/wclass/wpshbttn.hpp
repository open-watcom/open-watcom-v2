#ifndef wpushbutton_class
#define wpushbutton_class

#include "wbutton.hpp"

WCLASS WPushButton : public WButton
{
	public:
		WEXPORT WPushButton( WWindow* parent, const WRect& r, char* text=NIL );
		WEXPORT ~WPushButton();
};

#endif //wpushbutton_class
