  // technically we should diagnose this
  struct S {
   S() {}
  private:
   S();
  };

#if 0
S::S()  // make sure this still works! (perm will not be set)
{
}
#endif
