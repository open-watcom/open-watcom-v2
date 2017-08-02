/*
 * Feeble attempt to duplicate perl associative arrays
 * So feeble I won't even call it PerlAssoc!
 * Anyway the key can only be a string, the value can be anything.
 */
#include <iostream.h>
#include "perlclass.h"
#include "perlassoc.h"

main()
{
    Assoc<int> a;
    a("one")= 1; a("two")= 2; a("three")= 3;
    cout << a << endl;
    
    cout << "Keys:" << endl << a.keys() << endl;
    cout << "Values:" << endl << a.values() << endl;
    
    cout << "a.isin(\"three\")= " << a.isin("three") << endl;
    cout << "a.isin(\"four\")= " << a.isin("four") << endl;
    
    int t= a.adelete("two");
    cout << "a.adelete(\"two\") returns: " << t << ", a= " << endl << a << endl;

    return( 0 );
}
