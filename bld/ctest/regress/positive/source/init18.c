#include "fail.h"
#include <stdlib.h>
#include <string.h>

/* more designated initializers */

#pragma pack(8);

struct {
    char    a[10];
    int     i;
} s1 = {
    .i = -1,
    .a = "Hello C99!",
};

struct {
    char    a[10];
    int     i;
} s2 = {
    "Hello C99!", -1,
};

struct { int a[3], b; } w1[] =
  { [0].a = {1}, [1].a[0] = 2 };

struct { int a[3], b; } w2[] =
  { {{1}}, {{2}} };

struct { int a[3], b; } x1[] =
  { [1].a = {1}, [0] = {1,2} };

struct { int a[3], b; } x2[] =
  { {{1,2,0},0}, {{1,0,0},0} };

enum { member_one, member_two };

const char *nm1[] = {
  [member_two] = "member two",
  [member_one] = "member one",
};

const char *nm2[] =
  { "member one", "member two" };

#define MAX 12

int aap1[MAX] = {
  1, 3, 5, 7, 9, [MAX-5] = 8, 6, 4, 2, 0
};
int aap2[MAX] = {1,3,5,7,9,0,0,8,6,4,2,0};

#undef MAX
#define MAX 8

int noot1[MAX] = {
  1, 3, 5, 7, 9, [MAX-5] = 8, 6, 4, 2, 0
};
int noot2[MAX] = {1,3,5,8,6,4,2,0};

int a1[10] = { [1] = 5, 6, [9]=7, [0]=4 };
int a2[10] = { 4,5,6,0,0,0,0,0,0,7 };

div_t answer1 = { .quot = 2, .rem = -1 };
div_t answer2 = { 2, -1 };

struct X1 {
    short       a[3];
    char        *b;
    char        *c;
};

struct Y {
  struct X1 c;
  struct X1 d;
  struct X1 e;
};

struct Y v1 = {
  1,2,3,"aap",
  .d.b = "noot", "mies",
  .e.c = "hek",
  .d.c = "schapen"
};

struct Y v2 = {
  {1,2,3,"aap"},
  {0,0,0,"noot", "schapen"},
  {0,0,0,0,"hek"}
};

/* see www.open-std.org/jtc1/sc22/wg14/www/docs/dr_253.htm */
struct fred
{
  char s [6];
  int n;
};
struct fred f1 [] = { { { "abc" }, 1 }, [0].s[0] = 'q'        };
struct fred f2 [] = { { { "qbc" }, 1 } };
struct fred f3 [] = { { { "abc" }, 1 }, [0] = { .s[0] = 'q' } };
struct fred f4 [] = { { { "q" }, 0 } };

int main() {
    if(memcmp(&s1, &s2, sizeof(s1))) fail(__LINE__);
    if(memcmp(&w1, &w2, sizeof(w1))) fail(__LINE__);
    if(memcmp(&x1, &x2, sizeof(x1))) fail(__LINE__);
    if(memcmp(&nm1, &nm2, sizeof(nm1))) fail(__LINE__);
    if(memcmp(&aap1, &aap2, sizeof(aap1))) fail(__LINE__);
    if(memcmp(&noot1, &noot2, sizeof(noot1))) fail(__LINE__);
    if(memcmp(&a1, &a2, sizeof(a1))) fail(__LINE__);
    if(memcmp(&answer1, &answer2, sizeof(answer1))) fail(__LINE__);
    if(memcmp(&v1, &v2, sizeof(v1))) fail(__LINE__);
    if(memcmp(&f1, &f2, sizeof(f1))) fail(__LINE__);
    if(memcmp(&f3, &f4, sizeof(f3))) fail(__LINE__);
    _PASS;
}
