
/* Functions declared as OVL_EXTERN are used only within the module
 * they are declared in however pointers to them are used.  In order
 * for the overlay manager to be able to keep track of these pointers
 * the functions must be extern when an overlayed debugger is made
 */

#if defined(_OVERLAYED_)
#define OVL_EXTERN      extern
#else
#define OVL_EXTERN      static
#endif
