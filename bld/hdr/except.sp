namespace std {
  _WPRTLINK _WCNORETURN extern void terminate();
  _WPRTLINK _WCNORETURN extern void unexpected();

  typedef void (*unexpected_handler)();
  typedef void (*terminate_handler)();

  _WPRTLINK extern terminate_handler set_terminate( terminate_handler );
  _WPRTLINK extern unexpected_handler set_unexpected( unexpected_handler );
}
