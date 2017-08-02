extern int ack;
struct WObject
{
    WObject(){ack+=__LINE__;};
    virtual ~WObject(){ack+=__LINE__;};

    int	_flag;
};

struct Mrppt : public WObject
{
    private:

        int currentYear;

    public:

        Mrppt(){ack+=__LINE__;};
        ~Mrppt(){ack+=__LINE__;};
};


struct Scanner 
{
    Scanner(){ack+=__LINE__;};
    ~Scanner(){ack+=__LINE__;};

    Mrppt	_queue[3];
};
