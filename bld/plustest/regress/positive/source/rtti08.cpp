#include "fail.h"

template <class T>
class Property
{
    virtual void f(){};
};

typedef enum {READY, RUNNING} State;
typedef enum {OK, ERROR} Error;
typedef enum {} Empty;
enum QW { A,B,C };
State s;
Error e;
Empty t;

Property<State> v1;
Property<Error> v2;
Property<QW> v3;
Property<Empty> v4;

ALWAYS_PASS;
