namespace std {
  _WPRTLINK extern void terminate();
  #pragma aux terminate aborts
  _WPRTLINK extern void unexpected();
  #pragma aux unexpected aborts

  typedef void (*unexpected_handler)();
  typedef void (*terminate_handler)();

  _WPRTLINK extern terminate_handler set_terminate( terminate_handler );
  _WPRTLINK extern unexpected_handler set_unexpected( unexpected_handler );
}
