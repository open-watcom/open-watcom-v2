// The following code demonstrates a bug in the iterated intrusive
//	list functions. If one tries to add at the end of the list, the data
//	is added at the beginning.
//
//
#include <wclistit.h>
#include <iostream.h>

class int_ddata : public WCDLink {
public:
	inline int_ddata() {};
	inline int_ddata(int datum) : info (datum) {};

	int info;
};

#define NENTR	3000

int_ddata	 dd[NENTR];
int_ddata		*d1;
int_ddata		dapp, dapp2, dapp3;

int main(void) {

	WCIsvDList< int_ddata > list;
	WCIsvDListIter< int_ddata > iter1(list);

	int i=0, j=0;	

	for(i=0; i<NENTR; i++)
		dd[i].info = i;

        list.append(&dd[0]);
	++iter1;
	for(i=1; i<NENTR; i++) {
		iter1.append(&dd[i]);
		++iter1;
	}

	iter1.reset(list);
	++iter1;
	for(i=0; i<NENTR; i++) {
		d1 = iter1.current();
		iter1 += 1;
		if (i<20)
			cout << "dd[" << i << "].info = " << d1->info << endl;
	}

	list.clear();

	cout << endl << endl;
    return 0;
}
