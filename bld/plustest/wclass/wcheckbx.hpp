#ifndef wcheckbox_class
#define wcheckbox_class

#include "wboolsw.hpp"

WCLASS WCheckBox : public WBoolSwitch
{
	public:
		WEXPORT WCheckBox( WWindow* parent, const WRect& r, char* text=NIL );
		WEXPORT ~WCheckBox();
};

#endif //wcheckbox_class
