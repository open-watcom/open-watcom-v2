#include "fail.h"

class TEXT_PARAMS {
};

class WIDGET {
public:
	WIDGET () { wdgt = -1; }
	int widget () const { return wdgt; }
	int wdgt;
};

class BLACKBOARD : public TEXT_PARAMS, virtual private WIDGET {
};

class FRAME : virtual public WIDGET, protected BLACKBOARD {
};

class DEFECT : public WIDGET {
};

class DEFECT_REPORT : public DEFECT, public FRAME {
public:
	int widget () const { return FRAME::wdgt; }
};

int main() {
    DEFECT_REPORT *p = new DEFECT_REPORT();
    if( p->widget() != -1 ) _fail;
    _PASS;
}
