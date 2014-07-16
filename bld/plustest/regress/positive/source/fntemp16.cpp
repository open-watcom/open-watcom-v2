#include "fail.h"

/*
//  This regression test is linked to Bugzilla item numer 218
//  After a syncToRestart whilst in external linkage OR a namespace body
//  the parser could not reduce when the next token was a right brace.
//  a fix to the grammar to allow an empty token to satisfy the parser 
//  allowed the reduce.
*/
extern "C"{
   template < class t_Data > class t_Vector
   {
   public:  t_Data Do_Something(t_Data);
   };
   template < class t_Data >t_Data t_Vector< t_Data >::Do_Something(t_Data x){
      return(++x);
   }
}

namespace t_temp{
   template < class t_Data > class t_Vector2
   {
   public:  t_Data Do_Something(t_Data);
   };
   template < class t_Data >t_Data t_Vector2< t_Data >::Do_Something(t_Data x){
      return(++x);
   }
}

int main()
{
    _PASS;
}
