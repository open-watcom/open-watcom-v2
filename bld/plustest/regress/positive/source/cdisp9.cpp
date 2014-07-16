// MetaWare and IBM CSet++ can't compile this program
// Borland can't execute it properly
#include "fail.h"

struct V {
    int v;
    virtual void check( int v_value, unsigned line )
    {
	if( v != v_value ) {
	    fail(line);
	}
    }
};
template <class VBase>
    struct Trouble : virtual VBase {
	Trouble()
	{
	    V *pv = this;
    
	    v = __LINE__;
	    pv->check( v, __LINE__ );
	}
	virtual void check( int v_value, unsigned line )
	{
	    if( v != v_value ) {
		fail(line);
	    }
	}
    };
template <class VBase>
    struct Disrupt : virtual VBase {
	Disrupt()
	{
	    V *pv = this;
    
	    v = __LINE__;
	    pv->check( v, __LINE__ );
	}
    };
int main()
{
    Disrupt<
     Trouble<
      Disrupt<
       Trouble<
        Disrupt<
         Trouble<
          Disrupt<
           Trouble<
    V
           >
          >
         >
        >
       >
      >
     >
    > dummy;
    _PASS;
}
