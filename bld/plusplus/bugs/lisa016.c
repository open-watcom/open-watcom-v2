// Test case: FIELD test stream
// Status: work complete
// (note: used to cause and infinite loop)

#pragma inline_depth(0);
typedef unsigned size_t;
//extern "C" double acos( double );

class field;
class ifield;

struct __field_node {
  double data[1];
  __field_node(const __field_node&);
};

struct field {
    field(void);
    field(const field&);
    field(const ifield&);

    friend field acos(const field&);
    struct __field_node *root;
};

struct ifield {
    ifield(void);
    ifield(const ifield&);
    ifield(const field&);
};

extern  field rtn;

field acos(const field& f){
  double d = acos(f.root->data[0]);
  return rtn;
}
