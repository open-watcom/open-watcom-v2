// diagnose TRUE for and while loops

// from a customer
// no -ve test added yet
// warn that while expr is always non-zero
// all function pointers are non-zero
// I've looked into why we don't get a unref'd var msg
// for the block scope but there are a few reasons why
// we cannot change this behaviour

int Next(void); 

void test(void)
{ 
  do { 
    char* Next = "";
  } while (*Next);

  while (*Next) {
    char* Next = "";
  }

  for( ; (*Next); ) {
    char* Next = "";
  }
  Next += 1;
}
