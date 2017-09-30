#ifndef wboolswitch_class
#define wboolswitch_class

#include "wbutton.hpp"

WCLASS WBoolSwitch : public WButton
{
	public:
		WEXPORT WBoolSwitch( WWindow* parent, const WRect& r, char* text, WStyle wstyle );
		WEXPORT ~WBoolSwitch();

		void WEXPORT setCheck( bool check );
		bool WEXPORT checked();
};

#endif //wboolswitch_class
