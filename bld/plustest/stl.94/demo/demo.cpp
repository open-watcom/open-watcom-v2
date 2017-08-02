#include <iostream.h>
#include <algo.h>
#include <deque.h>
#include <list.h>
#include <vector.h>
#include <set.h>
#include <map.h>
#include <stack.h>
#include <bvector.h>
#include <multiset.h>
#include <multimap.h>

template<class T>
struct odd: unary_function<T, bool> {
    bool operator()(const T& x) const {
        return x & 1;
    }
};

template<class T>
struct bit_and: binary_function<T, T, T> {
    T operator()(const T& x, const T& y) const {
        return x & y;
    }
};


int main() {
    int i;
    
    cout << "Testing back_inserter(deque<char>):" << endl;


    deque<char> d;
    copy(istream_iterator<char, ptrdiff_t>(cin),
         istream_iterator<char, ptrdiff_t>(),
         back_inserter(d));
    copy(d.begin(), d.end(), ostream_iterator<char>(cout));
    cout << endl;

    cout << "Testing set<char>:" << endl;
    
    set<char, less<char> > sc;
    deque<char>::iterator di;
    for (di = d.begin(); di != d.end(); ++di)
        sc.insert(*di);
    copy(sc.begin(), sc.end(), ostream_iterator<char>(cout));
    cout << endl;

    cout << "Testing multiset<char>:" << endl;
    
    multiset<char, less<char> > msc;
    for (di = d.begin(); di != d.end(); di++)
        msc.insert(*di);
    copy(msc.begin(), msc.end(), ostream_iterator<char>(cout));
    cout << endl;

    cout << "Testing map<char, int>:" << endl;

    map<char, int, less<char> > mci;
    di = d.begin();
    while (di != d.end())
        mci[*di++]++;
    for (map<char, int, less<char> >::iterator mi = mci.begin(); mi != mci.end(); ++mi)
        cout << (*mi).first << "*" << (*mi).second << " ";
    cout << endl;

    cout << "Testing multimap<char, int>:" << endl;

    multimap<char, int, less<char> > mmci;
    for (di = d.begin(), i = 0; di != d.end(); ++di, ++i)
        mmci.insert(pair<const char,int>(*di, i));
    // can't use make_pair -- STL definition problem?
    for (multimap<char, int, less<char> >::iterator mmi = mmci.begin(); mmi != mmci.end(); ++mmi)
        cout << (*mmi).first << "*" << (*mmi).second << " ";
    cout << endl;

    cout << "Testing stack<vector<char> >:" << endl;

    stack<vector<char> > st;
    for (di = d.begin(); di != d.end(); di++)
        st.push(*di);
    while (!st.empty()) {
        cout << st.top();
        st.pop();
    }
    cout << endl;

    cout << "Testing queue<deque<char> >:" << endl;

    queue<deque<char> > qu;
    for (di = d.begin(); di != d.end(); di++)
        qu.push(*di);
    while (!qu.empty()) {
        cout << qu.front();
        qu.pop();
    }
    cout << endl;

    cout << "Testing vector<bool>:" << endl;

    vector<bool> bv(256);
    for (di = d.begin(); di != d.end(); di++)
        bv[(unsigned char)*di] = true;
    for (i = 0; i < 256; i++) {
        if (bv[i]) cout << char(i);
    }
    cout << endl;

    cout << "Testing sort(deque<char>):" << endl;
    
    sort(d.begin(), d.end());
    copy(d.begin(), d.end(), ostream_iterator<char>(cout));
    cout << endl;


    cout << "Testing stable_partition(list<char>):" << endl;
    
    list<char> v;
    copy(d.begin(), d.end(), back_inserter(v));
    list<char>::iterator p =
        stable_partition(v.begin(), v.end(), bind2nd(modulus<char>(),2));
    
    copy(v.begin(), p, ostream_iterator<char>(cout));
    cout << endl;
    copy(p, v.end(), ostream_iterator<char>(cout));
    cout << endl;

    cout << "Testing stable_partition(vector<char>):" << endl;

    vector<char> vec;
    copy(d.begin(), d.end(), back_inserter(vec));
    vector<char>::iterator pv =
        stable_partition(vec.begin(), vec.end(), bind2nd(modulus<char>(),2));
    
    copy(vec.begin(), pv, ostream_iterator<char>(cout));
    cout << endl;
    copy(pv, vec.end(), ostream_iterator<char>(cout));
    cout << endl;
    return 0;
}
