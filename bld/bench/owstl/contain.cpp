
// This program was obtained from a May 29, 2003 USENET posting by Bjarne
// Stroustrup. See:
//
// http://groups.google.com/group/comp.lang.c++.moderated/browse_frm/thread/1f9b553a3934b963/731a399cb149e0df
// 
// It is reproduced here with permission from Dr. Stroustrup. He requests
// that if we make significant improvements to the program that we send him
// a copy.
//
// Certain sections are commented out below because Open Watcom (v1.5) does
// not yet support the necessary features in the language or the library.

#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <ctime>

#include <algorithm>
#include <deque>
#include <iterator>
#include <list>
#include <set>
#include <vector>

#include <iostream>
#include <iomanip>

typedef double element_t;

using namespace std;

// Results are pushed into this vector
vector<double> result_times;

void do_head()
{
  cout << "size" <<
                "\tarray"
                "\tvec_p"
                "\tvec_i"
    //          "\tdeque"
                "\tlist"
                "\tset"
                "\tmultiset" << '\n';
}

void do_row(int size)
{
  cout << size;
  cout << setprecision(3);
  for (size_t i = 0; i < result_times.size(); ++i)
    cout << '\t' << result_times[i];
  cout << '\n';
}

clock_t start_time;

inline void start_timer() { start_time = clock(); }

inline double timer()
{
  clock_t end_time = clock();
  return (end_time - start_time)/double(CLOCKS_PER_SEC);
}

typedef void(*Test)(element_t*, element_t*, int);

void run(Test f, element_t* first, element_t* last, int number_of_times)
{
  start_timer();
  while (number_of_times-- > 0) f(first, last, number_of_times);
  result_times.push_back(timer());
}

void array_test(element_t* first, element_t* last, int)
{
  element_t* array = new element_t[last - first];
  copy(first, last, array);
  sort(array, array + (last - first));
  unique(array, array + (last - first));
  delete [] array;  
}

void vector_pointer_test(element_t* first, element_t* last, int)
{
  // vector<element_t> container(first, last);
  vector<element_t> container;
  copy(first, last, back_inserter(container));
  element_t *p = &*container.begin();
  sort(p, p + (last - first));
  unique(p, p + (last-first));
}

void vector_iterator_test(element_t* first, element_t* last, int)
{
  // vector<element_t> container(first, last);
  vector<element_t> container;
  copy(first, last, back_inserter(container));
  sort(container.begin(), container.end());
  unique(container.begin(), container.end());
}

#ifdef NEVER
void deque_test(element_t* first, element_t* last, int)
{  
  // deque<element_t> container(first, last);
  deque<element_t> container;
  copy(first, last, back_inserter(container));
  copy(first, last, container.begin());
  sort(container.begin(), container.end());
  unique(container.begin(), container.end());
}
#endif

void list_test(element_t* first, element_t* last, int)
{
  // list<element_t> container(first, last);
  list<element_t> container;
  copy(first, last, back_inserter(container));
  // container.sort();
  // container.unique();
}

void set_test(element_t* first, element_t* last, int)
{
  // set<element_t> container(first, last);
  set<element_t> container;
  while (first != last) {
    container.insert(*first);
    ++first;
  }
}

void multiset_test(element_t* first, element_t* last, int)
{
  // multiset<element_t> container(first, last);
  multiset<element_t> container;
  while (first != last) {
    container.insert(*first);
    ++first;
  }

  typedef multiset<element_t>::iterator iterator;
  {
    iterator first = container.begin();
    iterator last = container.end();

    while (first != last) {
      iterator next = first;
      if (++next == last) break;
      if (*first == *next)
        container.erase(next);
      else
        ++first;
    }
  }
}

void initialize(element_t* first, element_t* last)
{
  element_t value = 0.0;
  while (first != last) {
    *first++ = value;
    value += 1.0;
  }
}

double logtwo(double x)
{
  return log(x)/log(2.0);
}

const int largest_size = 1000000;

int number_of_tests(int size)
{
  double n = size;
  double largest_n = largest_size;
  return int(floor((largest_n * logtwo(largest_n)) / (n * logtwo(n))));
}

void run_tests(int size)
{
  const int n = number_of_tests(size);
  const size_t length = 2*size;
  result_times.clear();

  // make a random test set of the chosen size:
  vector<element_t> buf(length);
  element_t* buffer = &*buf.begin();
  element_t* buffer_end = buffer + length;
  initialize(buffer, buffer + size);
  initialize(buffer + size, buffer_end);
  random_shuffle(buffer, buffer_end);

  // test the containers:
  run(array_test, buffer, buffer_end, n);
  run(vector_pointer_test, buffer, buffer_end, n);
  run(vector_iterator_test, buffer, buffer_end, n);
  // run(deque_test, buffer, buffer_end, n);
  run(list_test, buffer, buffer_end, n);
  run(set_test, buffer, buffer_end, n);
  run(multiset_test, buffer, buffer_end, n);
  do_row(size);
}

int main()
{
  do_head();
  const int sizes [] = {10, 100, 1000, 10000, 100000, 1000000};
  const int n = sizeof(sizes)/sizeof(int);
  for (int i = 0; i < n; ++i) run_tests(sizes[i]);
  return 0;
}
