


// A program to illustrate the use of persistent data structures in YACL.
//
// Author: M. A. Sridhar
// Date:   May 22, 1994



#include <iostream.h>
#include <stdlib.h>

#include "base/base.h"
#include "io/io.h"




// ----------------------------------------------------
//                Define a small test class
//-----------------------------------------------------

class TestClass: public CL_Object {

public:
    TestClass (short i);

    TestClass () {};
    
    const CL_String& Data () const { return data; };

    CL_String AsString () const { return data; };
    // Overrides CL_Object's virtual method
    
    virtual long StorableFormWidth () const
        {return data.StorableFormWidth();};
    // Overrides CL_Object's virtual method

    virtual bool ReadFrom (const CL_Stream& s);
    // Overrides CL_Object's virtual method

    virtual bool WriteTo  (CL_Stream& s) const
        {return s.Write (ClassId()) && data.WriteTo (s);};
    // Overrides CL_Object's virtual method

    const char* ClassName () const {return "TestClass";};
    // Overrides CL_Object's virtual method

    CL_ClassId ClassId() const {return 2000;};
    
protected:
    CL_String data;
};


CL_DEFINE_CLASS(TestClass, 2000);
    
TestClass::TestClass (short i)
{
    data = "String: " + CL_String (i, 4, '0');
}

bool TestClass::ReadFrom (const CL_Stream& s)
{
    register CL_ClassId id;
    return ReadClassId (s) && data.ReadFrom (s);
}


//-----------------------------------------------------
// Main program
//-----------------------------------------------------

void ReadObjects  (const char* name);
void WriteObjects (const char* name);

main ()
{
    const char* name = "perstest.dat";
    if (!CL_BinaryFile::Exists (name))
        WriteObjects (name);
    else
        ReadObjects (name);
    return(0);
}


void WriteObjects (const char* name)
{
    CL_ObjectSequence obj_seq;
    CL_BinaryFile file (name, TRUE);
    long i;
    
    // First, create a sample sequence of objects
    for (i = 0; i < 20; i++) {
        obj_seq.Add (new TestClass (i*13 % 7));
    }

    // Save the sequence
    file << obj_seq;
    obj_seq.DestroyContents ();


    // Now create a set of strings
    CL_StringSet some_set;
    for (i = 0; i < 12; i++)
        some_set.Add ("String in set: " + CL_String (i, 4, '$'));
    file << some_set;
    
    // Next, create a few sample map objects
    CL_IntPtrMap map1;
    for (i = 0; i < 20; i++) {
        long n = i*13 % 7;
        TestClass* p = new TestClass (n);
        if (!map1.Add (n, p))
            delete p; // Guard against memory leaks
    }

    CL_StringStringMap map2;
    for (i = 0; i < 30; i++) {
        long n = i*13 % 7;
        map2.Add (CL_String (i), CL_String (n));
    }

    // Save the maps
    file << map1 << map2;
    map1.DestroyContents ();
    map2.DestroyContents ();

    {
        CL_String* p1, *p2, *p3;
        p1 = new CL_String ("String1");
        p2 = new CL_String ("String2");
        CL_ObjectSequence sq(5);
        sq[0] = p2;
        sq[1] = p1;
        sq[2] = p2;
        sq[3] = p2;
        sq[4] = 0;
        file.Remember ();  // Need to do this explicitly when saving
                           // multi-linked structures
        file << sq;
        delete p1;
        delete p2;
        file.Forget ();
    }
}

void ReadObjects (const char* name)
{
    CL_ObjectSequence obj_seq;
    CL_BinaryFile file (name);
    long i;
    
    // Read the sequence from the file
    file >> obj_seq; // Not necessarily equivalent to obj_seq.ReadFrom(file)
    cout << "Contents of restored sequence:\n";
    long n = obj_seq.Size();
    for (i = 0; i < n; i++) {
        TestClass* p = (TestClass*) obj_seq [i];
        cout << p->Data().AsPtr() << endl;
    }
    obj_seq.DestroyContents (); // Without this, there's a memory leak!

    // Read back the set
    CL_StringSet aSet;
    file >> aSet;
    n = aSet.Size();
    cout << "Restored set has " << n << " elements:\n";
    CL_StringSetIterator setIter (aSet);
    for (setIter.Reset (); setIter.More(); ) {
        cout << setIter.Next().AsPtr() << endl;
    }


    // And finally, read back the maps
    CL_IntPtrMap map1;
    CL_StringStringMap map2; // Note that there is no builder needed for
                             // non-pointer-based maps
    file >> map1 >> map2;

    cout << "Contents of restored map1:\n";
    CL_IntPtrMapIterator itr1 (map1);
    CL_IntPtrAssoc assoc1;
    for (itr1.Reset(); itr1.More(); ) {
        assoc1 = itr1.Next ();
        cout << assoc1.key << " --> " << 
                ((TestClass*) assoc1.value)->Data().AsPtr() << endl;
    }
    map1.DestroyContents ();



    cout << "Contents of restored map2:\n";
//     CL_StringStringMapIterator itr2 (map2);
//     CL_StringStringAssoc assoc2;
//     for (itr2.Reset(); itr2.More(); ) {
//         assoc2 = itr2.Next ();
//         printf ("'%s' --> '%s'\n", (const char*) assoc2.key,
//                 (const char *) assoc2.value);
//     }
    cout << map2 << endl;

    {
        CL_ObjectSequence sq;
        file >> sq;
        register long n = sq.Size();
        cout << "sq Size: " << n << endl;
        register long i;
        for (i = 0; i < n; i++) {
            CL_String* s = (CL_String*) sq[i];
            cout <<  (s ? s->AsPtr() : "(NULL)") << endl;
        }
        delete sq[0];
        delete sq[1];
    }
}

    

