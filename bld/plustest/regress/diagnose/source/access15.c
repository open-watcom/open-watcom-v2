// ffacc3.cpp - test friend vs. member function access of data
//		having different storage classes

class B
{
	static int priSB;	// private static var
        int	   priB;	// private non-static var
protected:
	static int proSB;	// protected static var
	int 	   proB;	// protected non-static var
public:
	static int pubSB;	// public static var
        int	   pubB;	// public non-static var
};

class D1pri : private B { };  // B::priSB now private

class D2pri : public D1pri
{
        friend void fD2pri(B& b, D1pri& d1pri, D2pri& d2pri);
        D2pri(B& b, D1pri& d1pri, D2pri& d2pri)
        {
		B::priSB = 1;       // BC3.1 MW3    ATT3
                D1pri::priSB = 2;   // BC3.1 MW3 C7 ATT3
                D2pri::priSB = 3;   // BC3.1 MW3 C7 ATT3
                b.priSB = 1;        // BC3.1 MW3 C7 ATT3
                d1pri.priSB = 2;    // BC3.1 MW3 C7 ATT3
                d2pri.priSB = 3;    // BC3.1 MW3 C7 ATT3
                priSB = 4;          // BC3.1 MW3 C7 ATT3
                B::priB = 1;        // BC3.1 MW3 C7 ATT3
                D1pri::priB = 2;    // BC3.1 MW3 C7 ATT3
                D2pri::priB = 3;    // BC3.1 MW3 C7 ATT3
                b.priB = 1;         // BC3.1 MW3 C7 ATT3
                d1pri.priB = 2;     // BC3.1 MW3 C7 ATT3
                d2pri.priB = 3;     // BC3.1 MW3 C7 ATT3
                priB = 4;           // BC3.1 MW3 C7 ATT3
		B::proSB = 1;       // BC3.1
                D1pri::proSB = 2;   // BC3.1 MW3 C7
                D2pri::proSB = 3;   // BC3.1 MW3 C7
                b.proSB = 1;        // BC3.1     C7 ATT3
                d1pri.proSB = 2;    // BC3.1 MW3 C7 ATT3
                d2pri.proSB = 3;    // BC3.1 MW3 C7
                proSB = 4;          // BC3.1 MW3 C7
                B::proB = 1;        // BC3.1 MW3 C7 ATT3
                D1pri::proB = 2;    // BC3.1     C7 ATT3
                D2pri::proB = 3;    // BC3.1 MW3 C7 ATT3
                b.proB = 1;         // BC3.1 MW3 C7 ATT3
                d1pri.proB = 2;     // BC3.1 MW3 C7 ATT3
                d2pri.proB = 3;     // BC3.1 MW3 C7 ATT3
                proB = 4;           // BC3.1 MW3 C7 ATT3
		B::pubSB = 1;       // BC3.1
                D1pri::pubSB = 2;   // BC3.1 MW3 C7
                D2pri::pubSB = 3;   // BC3.1 MW3 C7
		b.pubSB = 1;        //
                d1pri.pubSB = 2;    // BC3.1 MW3 C7
                d2pri.pubSB = 3;    // BC3.1 MW3 C7
                pubSB = 4;          // BC3.1 MW3 C7
                B::pubB = 1;        // BC3.1 MW3 C7 ATT3
                D1pri::pubB = 2;    // BC3.1     C7 ATT3
                D2pri::pubB = 3;    // BC3.1 MW3 C7 ATT3
                b.pubB = 1;         //
                d1pri.pubB = 2;     // BC3.1 MW3 C7 ATT3
                d2pri.pubB = 3;     // BC3.1 MW3 C7 ATT3
                pubB = 4;           // BC3.1 MW3 C7 ATT3
	}
};

void fD2pri(B& b, D1pri& d1pri, D2pri& d2pri)
{
        B::priSB = 1;               //       MW3 C7 ATT3
        D1pri::priSB = 2;           //       MW3 C7 ATT3
        D2pri::priSB = 3;           //           C7 ATT3
        b.priSB = 1;                // BC3.1 MW3 C7 ATT3
        d1pri.priSB = 2;            // BC3.1 MW3 C7 ATT3
        d2pri.priSB = 3;            // BC3.1     C7 ATT3
        priSB = 4;                  // BC3.1 MW3 C7 ATT3
        B::priB = 1;                // BC3.1 MW3 C7 ATT3
        D1pri::priB = 2;            // BC3.1 MW3 C7 ATT3
        D2pri::priB = 3;            // BC3.1 MW3 C7 ATT3
        b.priB = 1;                 // BC3.1 MW3 C7 ATT3
        d1pri.priB = 2;             // BC3.1 MW3 C7 ATT3
        d2pri.priB = 3;             // BC3.1     C7 ATT3
        priB = 4;                   // BC3.1 MW3 C7 ATT3
        B::proSB = 1;               //       MW3 C7 ATT3
        D1pri::proSB = 2;           //       MW3 C7 ATT3
        D2pri::proSB = 3;           //           C7 ATT3
        b.proSB = 1;                // BC3.1 MW3 C7 ATT3
        d1pri.proSB = 2;            // BC3.1 MW3 C7 ATT3
        d2pri.proSB = 3;            // BC3.1     C7
        proSB = 4;                  // BC3.1 MW3 C7 ATT3
        B::proB = 1;                // BC3.1 MW3 C7 ATT3
        D1pri::proB = 2;            // BC3.1 MW3 C7 ATT3
        D2pri::proB = 3;            // BC3.1 MW3 C7 ATT3
        b.proB = 1;                 // BC3.1 MW3 C7 ATT3
        d1pri.proB = 2;             // BC3.1 MW3 C7 ATT3
        d2pri.proB = 3;             // BC3.1     C7 ATT3
        proB = 4;                   // BC3.1 MW3 C7 ATT3
        B::pubSB = 1;               //
        D1pri::pubSB = 2;           //       MW3 C7
        D2pri::pubSB = 3;           //           C7
        b.pubSB = 1;                //
        d1pri.pubSB = 2;            // BC3.1 MW3 C7
        d2pri.pubSB = 3;            // BC3.1     C7
        pubSB = 4;                  // BC3.1 MW3 C7 ATT3
        B::pubB = 1;                // BC3.1 MW3 C7 ATT3
        D1pri::pubB = 2;            // BC3.1 MW3 C7 ATT3
        D2pri::pubB = 3;            // BC3.1 MW3 C7 ATT3
        b.pubB = 1;                 //
        d1pri.pubB = 2;             // BC3.1 MW3 C7 ATT3
        d2pri.pubB = 3;             // BC3.1     C7 ATT3
        pubB = 4;                   // BC3.1 MW3 C7 ATT3
}                                               

class D1pro : protected B { };  // B::proSB now protected

class D2pro : public D1pro
{
        friend void fD2pro(B& b, D1pro& d1pro, D2pro& d2pro);
        D2pro(B& b, D1pro& d1pro, D2pro& d2pro)
        {
                B::priSB = 1;       // BC3.1 MW3    ATT3
                D1pro::priSB = 2;   // BC3.1 MW3 C7 ATT3
                D2pro::priSB = 3;   // BC3.1 MW3 C7 ATT3
                b.priSB = 1;        // BC3.1 MW3 C7 ATT3
                d1pro.priSB = 2;    // BC3.1 MW3 C7 ATT3
                d2pro.priSB = 3;    // BC3.1 MW3 C7 ATT3
                priSB = 4;          // BC3.1 MW3 C7 ATT3
                B::priB = 1;        // BC3.1 MW3 C7 ATT3
                D1pro::priB = 2;    // BC3.1 MW3 C7 ATT3
                D2pro::priB = 3;    // BC3.1 MW3 C7 ATT3
                b.priB = 1;         // BC3.1 MW3 C7 ATT3
                d1pro.priB = 2;     // BC3.1 MW3 C7 ATT3
                d2pro.priB = 3;     // BC3.1 MW3 C7 ATT3
                priB = 4;           // BC3.1 MW3 C7 ATT3
                B::proSB = 1;       //
                D1pro::proSB = 2;   //
                D2pro::proSB = 3;   //
                b.proSB = 1;        // BC3.1     C7 ATT3
                d1pro.proSB = 2;    // BC3.1     C7 ATT3
                d2pro.proSB = 3;    //
                proSB = 4;          //
                B::proB = 1;        // BC3.1
                D1pro::proB = 2;    //
                D2pro::proB = 3;    //
                b.proB = 1;         // BC3.1 MW3 C7 ATT3
                d1pro.proB = 2;     // BC3.1 MW3 C7 ATT3
                d2pro.proB = 3;     //
                proB = 4;           //
                B::pubSB = 1;       //
                D1pro::pubSB = 2;   //
                D2pro::pubSB = 3;   //
                b.pubSB = 1;        //
                d1pro.pubSB = 2;    // BC3.1     C7
                d2pro.pubSB = 3;    //
                pubSB = 4;          //
		B::pubB = 1;        // BC3.1
                D1pro::pubB = 2;    //
                D2pro::pubB = 3;    //
                b.pubB = 1;         //
                d1pro.pubB = 2;     // BC3.1 MW3 C7
                d2pro.pubB = 3;     //
                pubB = 4;           //
        }
};

void fD2pro(B& b, D1pro& d1pro, D2pro& d2pro)
{
        B::priSB = 1;               //       MW3 C7 ATT3
        D1pro::priSB = 2;           //       MW3 C7 ATT3
        D2pro::priSB = 3;           //           C7 ATT3
        b.priSB = 1;                // BC3.1 MW3 C7 ATT3
        d1pro.priSB = 2;            // BC3.1 MW3 C7 ATT3
        d2pro.priSB = 3;            // BC3.1     C7 ATT3
        priSB = 4;                  // BC3.1 MW3 C7 ATT3
        B::priB = 1;                // BC3.1 MW3 C7 ATT3
        D1pro::priB = 2;            // BC3.1 MW3 C7 ATT3
        D2pro::priB = 3;            // BC3.1 MW3 C7 ATT3
        b.priB = 1;                 // BC3.1 MW3 C7 ATT3
        d1pro.priB = 2;             // BC3.1 MW3 C7 ATT3
        d2pro.priB = 3;             // BC3.1     C7 ATT3
        priB = 4;                   // BC3.1 MW3 C7 ATT3
        B::proSB = 1;               //       MW3 C7 ATT3
        D1pro::proSB = 2;           //       MW3 C7 ATT3
        D2pro::proSB = 3;           //              ATT3
        b.proSB = 1;                // BC3.1 MW3 C7 ATT3
        d1pro.proSB = 2;            // BC3.1 MW3 C7 ATT3
        d2pro.proSB = 3;            //
        proSB = 4;                  // BC3.1 MW3 C7 ATT3
        B::proB = 1;                // BC3.1 MW3 C7 ATT3
        D1pro::proB = 2;            // BC3.1 MW3 C7 ATT3
        D2pro::proB = 3;            // BC3.1 MW3 C7 ATT3
        b.proB = 1;                 // BC3.1 MW3 C7 ATT3
        d1pro.proB = 2;             // BC3.1 MW3 C7 ATT3
        d2pro.proB = 3;             //              ATT3
        proB = 4;                   // BC3.1 MW3 C7 ATT3
        B::pubSB = 1;               //
        D1pro::pubSB = 2;           //       MW3 C7
        D2pro::pubSB = 3;           //
        b.pubSB = 1;                //
        d1pro.pubSB = 2;            // BC3.1 MW3 C7
        d2pro.pubSB = 3;            //
        pubSB = 4;                  // BC3.1 MW3    ATT3
        B::pubB = 1;                // BC3.1 MW3 C7 ATT3
        D1pro::pubB = 2;            // BC3.1 MW3 C7 ATT3
        D2pro::pubB = 3;            // BC3.1 MW3 C7 ATT3
        b.pubB = 1;                 //
        d1pro.pubB = 2;             // BC3.1 MW3 C7 ATT3
        d2pro.pubB = 3;             //              ATT3
        pubB = 4;                   // BC3.1 MW3    ATT3
}

class D1pub : public B { };  // B::pubSB now public

class D2pub : public D1pub
{
        friend void fD2pub(B& b, D1pub& d1pub, D2pub& d2pub);
        D2pub(B& b, D1pub& d1pub, D2pub& d2pub)
        {
                B::priSB = 1;       // BC3.1 MW3    ATT3
                D1pub::priSB = 2;   // BC3.1 MW3 C7 ATT3
                D2pub::priSB = 3;   // BC3.1 MW3 C7 ATT3
                b.priSB = 1;        // BC3.1 MW3 C7 ATT3
                d1pub.priSB = 2;    // BC3.1 MW3 C7 ATT3
                d2pub.priSB = 3;    // BC3.1 MW3 C7 ATT3
                priSB = 4;          // BC3.1 MW3 C7 ATT3
                B::priB = 1;        // BC3.1 MW3 C7 ATT3
                D1pub::priB = 2;    // BC3.1 MW3 C7 ATT3
                D2pub::priB = 3;    // BC3.1 MW3 C7 ATT3
                b.priB = 1;         // BC3.1 MW3 C7 ATT3
                d1pub.priB = 2;     // BC3.1 MW3 C7 ATT3
                d2pub.priB = 3;     // BC3.1 MW3 C7 ATT3
                priB = 4;           // BC3.1 MW3 C7 ATT3
                B::proSB = 1;       //
                D1pub::proSB = 2;   //
                D2pub::proSB = 3;   //
                b.proSB = 1;        // BC3.1     C7 ATT3
                d1pub.proSB = 2;    // BC3.1     C7 ATT3
                d2pub.proSB = 3;    //
                proSB = 4;          //
                B::proB = 1;        //
                D1pub::proB = 2;    //
                D2pub::proB = 3;    //
                b.proB = 1;         // BC3.1 MW3 C7 ATT3
                d1pub.proB = 2;     // BC3.1 MW3 C7 ATT3
                d2pub.proB = 3;     //
                proB = 4;           //
                B::pubSB = 1;       //
                D1pub::pubSB = 2;   //
                D2pub::pubSB = 3;   //
                b.pubSB = 1;        //
                d1pub.pubSB = 2;    //
                d2pub.pubSB = 3;    //
                pubSB = 4;          //
                B::pubB = 1;        //
                D1pub::pubB = 2;    //
                D2pub::pubB = 3;    //
                b.pubB = 1;         //
                d1pub.pubB = 2;     //
                d2pub.pubB = 3;     //
                pubB = 4;           //
        }
};

void fD2pub(B& b, D1pub& d1pub, D2pub& d2pub)
{
        B::priSB = 1;               //       MW3 C7 ATT3
        D1pub::priSB = 2;           //       MW3 C7 ATT3
        D2pub::priSB = 3;           //           C7 ATT3
        b.priSB = 1;                // BC3.1 MW3 C7 ATT3
        d1pub.priSB = 2;            // BC3.1 MW3 C7 ATT3
        d2pub.priSB = 3;            // BC3.1     C7 ATT3
        priSB = 4;                  // BC3.1 MW3    ATT3
        B::priB = 1;                // BC3.1 MW3 C7 ATT3
        D1pub::priB = 2;            // BC3.1 MW3 C7 ATT3
        D2pub::priB = 3;            // BC3.1 MW3 C7 ATT3
        b.priB = 1;                 // BC3.1 MW3 C7 ATT3
        d1pub.priB = 2;             // BC3.1 MW3 C7 ATT3
        d2pub.priB = 3;             // BC3.1     C7 ATT3
        priB = 4;                   // BC3.1 MW3    ATT3
        B::proSB = 1;               //       MW3 C7 ATT3
        D1pub::proSB = 2;           //       MW3 C7 ATT3
        D2pub::proSB = 3;           //              ATT3
        b.proSB = 1;                // BC3.1 MW3 C7 ATT3
        d1pub.proSB = 2;            // BC3.1 MW3 C7 ATT3
	d2pub.proSB = 3;            //
        proSB = 4;                  // BC3.1 MW3    ATT3
        B::proB = 1;                // BC3.1 MW3 C7 ATT3
        D1pub::proB = 2;            // BC3.1 MW3 C7 ATT3
        D2pub::proB = 3;            // BC3.1 MW3 C7 ATT3
        b.proB = 1;                 // BC3.1 MW3 C7 ATT3
        d1pub.proB = 2;             // BC3.1 MW3 C7 ATT3
	d2pub.proB = 3;             //
        proB = 4;                   // BC3.1 MW3    ATT3
        B::pubSB = 1;               //
        D1pub::pubSB = 2;           //
        D2pub::pubSB = 3;           //
        b.pubSB = 1;                //
        d1pub.pubSB = 2;            //
        d2pub.pubSB = 3;            //
        pubSB = 4;                  // BC3.1 MW3 C7 ATT3
        B::pubB = 1;                // BC3.1 MW3 C7 ATT3
        D1pub::pubB = 2;            // BC3.1 MW3 C7 ATT3
        D2pub::pubB = 3;            // BC3.1 MW3 C7 ATT3
        b.pubB = 1;                 //
        d1pub.pubB = 2;             //
        d2pub.pubB = 3;             //
        pubB = 4;                   // BC3.1 MW3 C7 ATT3
}

int main()
{
	return 0;
}
