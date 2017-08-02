class WRampEntry;
class WObject {
};

class WRampFile :  public WObject
{
    public:
        WRampFile();
        ~WRampFile();
        operator WRampEntry *  [] (long int entry);
};

WRampFile::WRampFile()
{
    
}
WRampFile::~WRampFile()
{
    
}
 WRampEntry * WRampFile::operator [] (long int entry)
{
    
}
