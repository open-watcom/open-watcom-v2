// this test will give an exception as the clearAndDestroy in dblclear
// cannot delete items allocated from the stack


extern "C" {
        #include <stdio.h>
        #include <stdlib.h>
        #include <time.h>
}
#include <wclistit.h>
#include <iostream.h>

class test : public WCDLink
{
        public:

                int a;
                int b;
        test(int a1, int b1) { a = a1; b = b1; }
        print() { cout << this->a << " " << this->b << endl; }
};

template <class T> class linklist {

        WCIsvDList< T > dbl;

        int cur;
        int max;

public:

        linklist() { cur = max = 0; }
        linklist(T *t) {
                max = 1;
                dbl.append(t);
        }

//      ~linklist() { dbl.clearAndDestroy(); }

        void dblappend(T *t) { dbl.append(t); }
        void dblinsert(T *t) { dbl.insert(t); }

        T * dblread(int i) {
                T *tmp;
                try {
                        tmp = dbl.find(i);
                } catch ( ... ) {
                        return(NULL);
                }
                return(tmp);
        }

        int dbldelete(int i, int dofree = 0) {
                try {
                        T *tmp;
                        dbl.get(i);
                        if (dofree) free(tmp);
                } catch ( ... ) {
                        return(0);
                }
                return(1);
        }

        int dblentries() { return(dbl.entries()); }

        void dblclear() { dbl.clear(); }

        void dbliinit(WCIsvDListIter<T> &di) {
                cur = 0;
                max = dbl.entries();
                di.reset();
                if (max > 0) ++di;
        }

        void dblitop(WCIsvDListIter<T> &di) {
                this->dbliinit(di);
        }

        void dblibot(WCIsvDListIter<T> &di) {
T *tmp;
//              this->dbliinit(di);
cur = 0;
max = dbl.entries();
di.reset(dbl);
++di;
tmp = di.current();
tmp->print();

                cur = max - 1;
                di += cur - 1;
tmp = di.current();
tmp->print();
--di;
tmp = di.current();
tmp->print();
        }

        int dblinext(WCIsvDListIter<T> &di) { 
                try {
                        if (cur < max) {
                                cur++;
                                ++di;
                                return(cur);
                        } else {
                                return(-1);
                        }
                } catch ( ... ) {
                        return(-1);
                }
        }
        int dbliprev(WCIsvDListIter<T> &di) { 
                try {
                        if (cur > 0) {
                                cur--;
                                --di;
                                return(cur);
                        } else {
                                return(-1);
                        }
                } catch ( ... ) {
                        return(-1);
                }
        }
        int dblicur(WCIsvDListIter<T> &di) { di=di; return(cur); }

        T * dbliread(WCIsvDListIter<T> &di) { return(di.current()); }


        int dbliinsert(WCIsvDListIter<T> &di, T *t) {
                try {
                        if (!max) {
                                dbl.append(t);
                        } else if (max == 1) {
                                dbl.insert(t);
                        } else {
                                di.insert(t);
                        }
                        return(1);
                } catch (...) {
                        return(0);
                }
        }
        int dbliappend(WCIsvDListIter<T> &di, T *t) {
                try {
                        if (!max) {
                                dbl.append(t);
                        } else if (max == 1) {
                                dbl.append(t);
                        } else {
                                di.append(t);
                        }
                        return(1);
                } catch (...) {
                        return(0);
                }
        }

        int dblupdate(T *t, int dofree = 0) {
                try {
                        int i;
                        if ( (i = this->findprimary(t)) >= 0) {
                                if (this->dbldelete(i, dofree) != 1)
                                        return(0);
                                if (this->dbladd(t) != 1)
                                        return(0);
                                return(1);
                        } else {
                                return(0);
                        }
                } catch (...) {
                        return(0);
                }
        }

        int dblsupdadd(T *t, int dofree = 0) {
                try {
                        int i;
                        if ( (i = this->findsort(t)) >= 0) {
                                if (this->dbldelete(i, dofree) != 1)
                                        return(0);
                                if (this->dbladd(t) != 1)
                                        return(0);
                                return(1);
                        } else {
                                if (this->dbladd(t) != 1)
                                        return(0);
                                return(2);
                        }
                } catch (...) {
                        return(0);
                }
        }
        int dblpupdadd(T *t) {
                try {
                        int i;
                        if ( (i = this->findprimary(t)) >= 0) {
                                if (this->dbldelete(i) != 1)
                                        return(0);
                                if (this->dbladd(t) != 1)
                                        return(0);
                                return(1);
                        } else {
                                if (this->dbladd(t) != 1)
                                        return(0);
                                return(2);
                        }
                } catch (...) {
                        return(0);
                }
        }

        friend int cmppkey(T *, T *);
        friend int cmpskey(T *, T *);


        int findprimary(T *t) {
                int i;
                WCIsvDListIter< T > di(dbl);

                this->dbliinit(di);
                for(i=0; i<max; i++) {
                        try {
                                if (!(cmppkey(t, di.current())))
                                        return(i);
                                ++di;
                        } catch ( ... ) {
                                return(-1);
                        }
                }
                return(-1);
        }

        int findsort(T *t) {
                int i;
                WCIsvDListIter< T > di(dbl);

                this->dbliinit(di);
                for(i=0; i<max; i++) {
                        try {
                                if (!(cmpskey(t, di.current())))
                                        return(i);
                                ++di;
                        } catch ( ... ) {
                                return(-1);
                        }
                }
                return(-1);
        }

        int findless(WCIsvDListIter<T> &di, T *t) {
                this->dblibot(di);
                for(; cur>=0; cur--) {
                        try {
                                if ((cmpskey(di.current(), t)) <= 0) {
                                        return(cur);
                                }
                                di -= 1;
                        } catch ( ... ) {
                                return(-1);
                        }
                }
                return(-1);
        }

        int findgreater(WCIsvDListIter<T> &di, T *t) {
                this->dbliinit(di);
                for(; cur<max; cur++) {
                        try {
                                if ((cmpskey(di.current(), t)) >= 0) {
                                        return(cur);
                                }
                                ++di;
                        } catch ( ... ) {
                                return(-1);
                        }
                }
                return(max);
        }

        int findrange(T *t1, T *t2, int *mini, int *maxi)
        {
                WCIsvDListIter< T > di(dbl);

                *maxi = this->findless(di, t2);
                *mini = this->findgreater(di, t1);

                return ((*mini == -1 || *maxi == -1) ? 0 : 1);
        }

        int dbladd(T *t)
        {
                WCIsvDListIter< T > di(dbl);

                if (!this->findgreater(di, t))
                        return(0);
                if (!this->dbliinsert(di, t))
                        return(0);
                return(1);
        }

        void dbldebug() {
                int i;
                T *tmp;
                for (i=0; i<this->dblentries(); i++) {
                        tmp = this->dblread(i);
                        tmp->print();
                }
        }
};

cmppkey(test *t1, test *t2) {
        if (t1->a < t2->a) return(-1);
        if (t1->a > t2->a) return(1);
        return(0);
}       

cmpskey(test *t1, test *t2) {
        if (t1->b < t2->b) return(-1);
        if (t1->b > t2->b) return(1);
        return(0);
}       


int main()
{
        int i,j;
        test t1(1,2);
        test t2(5,6);
        test t3(10,11);
        test t4(20,21);
        test t0(0,0);

        test ti(14, 15);

        test tfind(14, 16);


        linklist<test> Tlink;

        Tlink.dblappend(&t1);
        Tlink.dblappend(&t2);
        Tlink.dblappend(&t3);
        Tlink.dblappend(&t4);
        Tlink.dblinsert(&t0);

        Tlink.dbldebug();
        cout << endl;

        if (!Tlink.dbladd(&ti))
                exit(-1);

        Tlink.dbldebug();

        Tlink.dbldelete(3, 0);
        cout << endl;

        Tlink.dbldebug();

        cout << "primary 14 = " << Tlink.findprimary(&tfind) << endl;
        cout << "sort 16 = " << Tlink.findsort(&tfind) << endl;
        tfind.b = ti.b;
        cout << "sort 15 = " << Tlink.findsort(&tfind) << endl;

        Tlink.findrange(&t1, &t2, &i, &j);
        cout << "range for " << t1.b << "-" << t2.b << " is " << i << " to " << j << endl; 


//      Tlink.dblclear();
    return 0;
}
