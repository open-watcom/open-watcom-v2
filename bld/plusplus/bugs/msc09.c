
/* remove 'virtual' or 'const' and it works */
class barf {
public:
    virtual operator const double& ();
};
