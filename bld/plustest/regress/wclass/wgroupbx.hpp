#ifndef wgroupbox_class
#define wgroupbox_class

#include "wbutton.hpp"

WCLASS WGroupBox : public WButton
{
	public:
		WEXPORT WGroupBox( WWindow* parent, const WRect& r, char* text=NIL );
		WEXPORT ~WGroupBox();
};

#endif //wgroupbox_class
