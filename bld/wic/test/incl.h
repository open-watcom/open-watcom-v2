#include "incl1.h"  // Include 1
#include <incl2.h>  // Include 2
#include "incl3.h"  // Include 3

#define INT int  /* Comment 1 */
/* Comment 2 */

#define TYPE INT
#define DECLARE_C   TYPE /* comment 3 */     c // comment 4
int a; DECLARE_C; TYPE b;

int another_and_last_declaration;
#include <incl1.h>

#define VOID void
#define DECL_D			int unsigned  long   d1 ,d2;
int errorInclMain2 =;errorInclMain2;
struct s1_name {
    int unsigned i;
    float f;
    struct inside_s1_name {
	long char ch;
	VOID x1;
	VOID x2;
	DECL_D
    } inside_s1;
    struct s_name inside_s2;
//    float f2 = 1.12*23 >> 2 +   23 & (!!~23) ^ 9 - -1
//	    + 23;  float f3, f4;
    
}  s1;

struct s2;
struct {
    int i;
} s3;
