// should the conversion from D& to V& be diagnosed when constructing 'D'?
class V {
protected:
    V() { }
};

class B : private virtual V { };

class D : public B { };

int main()
{
   B b;
   D d;
   return 0;
}
