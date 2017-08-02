// The following code demonstrates a bug in the iterated intrusive
//	list functions. If one tries to add at the end of the list, the data
//	is added at the beginning.
//
//
#include <wclistit.h>
#include <iostream.h>


#define NENTR	1000

int		vect[ 1000 ];
int		d1, dapp, dapp2, dapp3;

int main(void) {

	WCPtrDList< int > list;
	WCPtrDListIter< int > iter1(list);

	int i=0, j=0;	

	list.append( &vect[0] );
	++iter1;
	for(i=1; i<NENTR; i++) {
	    	vect[i] = i;
		iter1.append( &vect[i] );
		++iter1;
	}

	iter1.reset(list);
	++iter1;
	for(i=0; i<NENTR; i++) {
		j = *iter1.current();
		iter1 += 1;
		if (i<10)
			cout << "vect[" << i << "] = " << j << endl;
	}

	list.clear();

	cout << endl << endl;
    return 0;
}
