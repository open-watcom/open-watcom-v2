//
//
// A short and hopefully easy to understand demonstration of CauseWay DLL
// usage with Watcom.
//
//


// Need function definitions
#include "dllfunc.h"


// Name the module. DLLS for stack version and DLLR for register
char ModuleName[]={"DLLR"};


//
//The actual do something code.
//
int main()
{
unsigned char *DLL;
void _cdecl (*DLLFunction)(char *);

	// Try and load the module.
	DLL=LoadModule(ModuleName);
	if (DLL) {

		printf("Module ");
		printf(ModuleName);
		printf(" loaded sucessfully\n");

		// Fetch the test function address
		DLLFunction=GetProcAddress(DLL,"_SAYHELLO");

		if (DLLFunction) {

			// Give the test function a shout
			DLLFunction("Hello World!\n");

		} else {

			printf("Failed to GetProcAddress\n");

		}

			// Lose the module again
			FreeModule(DLL);

			printf("Module ");
			printf(ModuleName);
			printf(" discarded\n");

	} else {

		printf("Failed to load ");
		printf(ModuleName);
		printf(" module...\n");

	}

return(0);
}

