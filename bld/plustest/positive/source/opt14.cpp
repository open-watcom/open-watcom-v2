#include "fail.h"

class BRANCH;
class BRANCH_REF;
class TERM;

enum REF_TYPE { RT_BRANCH, RT_TERM };

struct BRANCH_REF {
    REF_TYPE tag;
    void*    what;     // BRANCH* or TERM*
};

struct BRANCH {
    BRANCH_REF left;
    BRANCH_REF right;
};

struct TERM {
    unsigned count;
    int doSomething() {
	return ++count;
    }
};


void trace( BRANCH_REF const &r ) {
    switch (r.tag) {
    case RT_BRANCH: {
	BRANCH *b = (BRANCH*) r.what;
	trace (b->left);
	trace (b->right);
	return;
    }
    case RT_TERM: {
        TERM *t = (TERM*) r.what;
	t->doSomething();
	return;
    }
    default:
	fail(__LINE__);
    }
}

TERM term;
BRANCH branch5 = { { RT_TERM, &term }, { RT_TERM, &term } };
BRANCH branch4 = { { RT_TERM, &term }, { RT_TERM, &term } };
BRANCH branch3 = { { RT_BRANCH, &branch5 }, { RT_TERM, &term } };
BRANCH branch2 = { { RT_TERM, &term }, { RT_BRANCH, &branch4 } };
BRANCH branch1 = { { RT_BRANCH, &branch2 }, { RT_BRANCH, &branch3 } };
BRANCH_REF ref1 = { RT_BRANCH, &branch1 };

int main() {
    trace( ref1 );
    if( term.count != 6 ) fail(__LINE__);
    _PASS;
}
