struct __stdcall WObject
{
    WObject();
    virtual ~WObject();

    int	_flag;
};

struct __stdcall Mrppt : public WObject
{
    private:

        int currentYear;

    public:

        Mrppt();
        ~Mrppt();
};


struct __stdcall Scanner 
{
    Scanner();
    ~Scanner();

    Mrppt	_queue[3];
};
