#include "fail.h"

int active;

struct CD {
    static CD *ctored;
    CD	*next;
    int sig;
    CD(CD const &s) : sig(s.sig+1) {
	++active;
	next = ctored;
	ctored = this;
    };
    CD(int v) : sig(v) {
	++active;
	next = ctored;
	ctored = this;
    };
    ~CD(){
	CD **head = &ctored;
	--active;
	if( active < 0 ) fail(__LINE__);
        CD *p;
	for( p = *head; p != NULL; p = p->next ) {
	    if( p == this ) {
		*head = p->next;
		break;
	    }
	    head = &(p->next);
	}
	if( p == NULL ) fail(__LINE__);
    };
};
CD *CD::ctored;

struct useCD {
    CD _f1;
    useCD( const CD & f1 ) : _f1(f1) {}
};

void foo()
{
    useCD qq(CD(0));
}

int main()
{
    foo();
    if( active != 0 ) fail(__LINE__);
    _PASS;
}
