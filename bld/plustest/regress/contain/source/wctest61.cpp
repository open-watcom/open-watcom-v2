#include <wcstack.h>
#include <wcqueue.h>
#include <iostream.h>

int main() {
    int nums[ 5 ] = { 1, 2, 3, 4, 5 };
    WCStack<int *, WCPtrSList<int> > stack;

    for( int i = 0; i < 5; i++ ) {
	stack.push( &nums[ i ] );
    };
    for( i = 0; i < 5; i++ ) {
	cout << *stack.pop() << " ";
    }
    cout << "\n";

    WCQueue<int *, WCPtrSList<int> > queue;

    for( i = 0; i < 5; i++ ) {
	queue.insert( &nums[ i ] );
    };
    for( i = 0; i < 5; i++ ) {
	cout << *queue.get() << " ";
    }
    cout << "\n";
    return 0;
};
