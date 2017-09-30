#include <wclist.h>
#include <iostream.h>
#include <string.hpp>


int main() {
    WCPtrSList<String>  list;
    String              d1("str#1");
    String              d2("str#2");
    String              d3("str#3");

    list.insert( &d1 );
    list.insert( &d2 );
    list.insert( &d3 );
    cout << "[" << *list.find(0) << "] - find(0)\n";
    cout << "[" << *list.find(2) << "] - find(2)\n";
    cout << "[" << *list.find() << "] - find()\n";
    cout << "[" << *list.find(1) << "] - find(1)\n";
    cout << "[" << *list.findLast() << "] - findLast()\n";
    cout.flush();
    return 0;
}
