#ifndef wlist_class
#define wlist_class

#include "wvlist.hpp"

template <class S> class WList : public WVList
{
		public:
				WEXPORT WList() {}
				WEXPORT ~WList() {}
				S* WEXPORT find( S* member ) { return (S*)WVList::find( member ); }
				S* WEXPORT add( S* member ) { return (S*)WVList::add( member ); }
				S* WEXPORT removeSame( S* member ) { return (S*)WVList::removeSame( member ); }
};

#endif //wlist_class
