
/*
    foo<int> causes bar<int> to be expanded which requires foo<int> to be
    fully defined

    fix by deferring full expansion until TypeDefined is called
*/
template <class Type> class bar;

template <class Type>
class foo {
public:
    typedef bar<Type> *barPtr;
    foo() { doh=0; }

private:
    barPtr doh;       // no problem, barPtr visible w/o scope op
};

template <class Type>
class bar {
public:
    bar(foo<Type>::barPtr b) { test=b; }  // SHOULDN'T BE A PROBLEM?!

private:
    foo<Type>::barPtr test;    // ALSO SHOULDN'T BE A PROBLEM?!
};

int main() {
    foo<int> aFoo;
    return 0;
}
