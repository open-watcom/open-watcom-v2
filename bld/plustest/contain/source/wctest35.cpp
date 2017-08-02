#include <wcstack.h>
#include <iostream.h>


int main() {
    WCStack<int,WCValDList<int> >       stack;

    stack.push( 7 );
    stack.push( 8 );
    stack.push( 9 );
    stack.push( 10 );

    cout << "\nNumber of stack 2 entries: " << stack.entries() << "\n";
    cout << "Top entry = [" << stack.top() << "]\n";
    while( !stack.isEmpty() ) {
        cout << stack.pop() << "\n";
    };
    cout.flush();
    return 0;
}
