template<class T>
    void f(T t_p, long l)
    {
        struct s { T t_f; s(s&){}; s(){}; } temp;
        temp.t_f = t_p;
        if (l > 0) {
            f(temp, l - 1);
        }
    };
extern long random();
void main() {
    f(3.14, random());
}
