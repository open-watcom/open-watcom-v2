
class    RWDecimal64
{
public:
//RWDecimal64& operator+(const RWDecimal64& a);
  RWDecimal64& operator+=(const RWDecimal64& a)
    {
        RWDecimal64 sum = *this + a;
        return *this = sum;
    }
};
