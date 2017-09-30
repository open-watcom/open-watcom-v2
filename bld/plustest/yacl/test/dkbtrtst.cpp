
// Testing the disk-based B-tree
//
// M. A. Sridhar 5/5/94

#include "io/dskbtree.h"
#include <iostream.h>
#include "base/memory.h"

class TestClass: public CL_Object {

public:
    TestClass () {};
    
    TestClass (const CL_String& name, long salary)
        :_name (name), _salary (salary) {};

    ~TestClass ();
    
    short Compare (const CL_Object&) const;

    // Override the CL_Object's methods:
    virtual bool ReadFrom (const CL_Stream&);

    virtual bool WriteTo  (CL_Stream&) const;

    CL_String AsString () const {return _name + ": " + CL_String (_salary);};

    CL_String _name;
    long      _salary;
};


TestClass::~TestClass ()
{
}

short TestClass::Compare (const CL_Object& o) const
{
    return _name.Compare (((const TestClass&) o)._name);
}

bool TestClass::ReadFrom (const CL_Stream& s)
{
    return _name.ReadFrom (s) && s.Read (_salary);
}


bool TestClass::WriteTo (CL_Stream& s) const
{
    return _name.WriteTo (s) && s.Write (_salary);
}


typedef CL_Builder<TestClass> TestBuilder;

#define BTREE_DATA_FILE "btree.dat"


struct {
    char* name;
    short value;
} Table [] = {
    "Michael Jordan", 300,
    "Lee Iacocca",    150,
    "Patty O'Furniture", 20,
    "Polly C. Maker", 40,
    "John Q. Public", 22,
    "Fly Swatter",    23,
    "Slim Pickens",     44,
    "Curly Moe",      202,
    "Meg O. Byte",    1001,
    0, 0
};

main ()
{
    TestBuilder bld;
    if (!CL_BinaryFile::Exists (BTREE_DATA_FILE)) {
        CL_ByteStringStore store (BTREE_DATA_FILE, TRUE);
        CL_DiskBTree diskTree (store, &bld, 3, TRUE);

        short i;
        for (i = 0; Table[i].name != 0; i++) {
            TestClass* p = new TestClass  (Table[i].name, Table[i].value);
            diskTree.Add (p);
        }
        diskTree.IntoStream (cout);
    }
    else {
        CL_ByteStringStore store (BTREE_DATA_FILE);
        CL_DiskBTree diskTree (store, &bld, 3, FALSE);
        diskTree.IntoStream (cout);
        TestClass q ("Fly Swatter", 0);
        TestClass* p = (TestClass*) diskTree.Find (&q);
        cout << "\n\n" << p->AsString() << endl;
    }
    return 0;
}

