
#include <iostream.h>

#include "base/map.h"
#include "base/memory.h"

main ()
{
    CL_MemoryLeakChecker check (cout);
    CL_IntPtrMap aMap;
    CL_String* s = new CL_String ("Some entry");
    aMap.Add (56, s);
    CL_Object* pObj;
    cout << "56: " << *aMap[56] << endl;
    cout << "Nonexistent: " << aMap[10] << endl;
    delete s;

    CL_StringIntMap my_map;
    
    my_map.Add (CL_String ("Binary"), 750);
    my_map.Add (CL_String ("k-ary"), 1011);
    my_map.Add (CL_String ("Hash"), 1);
    my_map.Add (CL_String ("AVL"), 2);
    my_map.Add (CL_String ("balanced"), 3);
    my_map.Add (CL_String ("half-balanced"), 4);
    my_map.Add (CL_String ("B-tree"), 5);
    my_map.Add (CL_String ("Self-adjusting"), 6);
    long i = my_map ["balanced"];
    cout << "balanced: " << i << endl;

    cout << "-----------------------------------\n";
    CL_StringIntMapIterator iter (my_map);
    CL_StringIntAssoc assoc;
    for (iter.Reset(); iter.More(); ) {
        assoc = iter.Next();
        cout << assoc.key << " --> " << assoc.value << endl;
    }

    cout << "No such entry: " << my_map["No such entry"] << endl;
    
    my_map.Remove ("Binary");

    cout << "-----------------------------------\n";
    // for (iter.Reset(); iter.More(); ) {
    for (i = 0; i < my_map.Size(); i++) {
        assoc = my_map.ItemWithRank (i);
        cout << assoc.key << " --> " << assoc.value << endl;
    }

    cout << "-----------------------------------\n";
    CL_StringIntMap map2;
    map2.Add ("k-ary", 1011);      
    map2.Add ("Hash", 1);          
    map2.Add ("balanced", 3);      
    map2.Add ("half-balanced", 15);
    cout << map2;
    
//     for (iter.Reset(); iter.More(); ) {
//         assoc = iter.Next();
//         printf ("'%s': %ld\n", (const char*) assoc.key.AsPtr(),
//                 assoc.value);
//     }

    /*
    for (short k = 0; k < 10000; k++) {
        CL_String s ((long) k);
        my_map.Add (s, k);
        // if (k % 1000 == 0 && k > 0)
            printf ("k = %d\n", k);
    }
    */
        
return(0);
}


