
#include <stdio.h>
#include <iostream.h>

#include "base/base.h"
#include "io/slotfile.h"
#include "base/memory.h"


#define SLOT_SIZE 10L
#define MAX_RECORDS 4120
#define FILENAME "slotfile.dat"


void WriteFile ()
{
    CL_ByteString buffer (SLOT_SIZE);
    CL_PrivateSlottedFile file1 (FILENAME, SLOT_SIZE, 200);
          // Make a new file
    buffer = 1L;
    CL_IntegerSequence handle (MAX_RECORDS);
    long i;
    for (i = 0; i < MAX_RECORDS; i++) {
        buffer = (long) i+3;
        handle[i] = file1.AddRecord (buffer);
        if (i % 1000 == 1) printf ("Added record %ld\n", i);
    }

    for (i = 0; i < 40; i++)
        if (!file1.DeleteRecord (handle[i])) 
            printf ("Delete failed: handle %lx\n", handle[i]);
    for (i = 50; i < 4090; i++)
        if (!file1.DeleteRecord (handle[i])) 
            printf ("Delete failed: handle %lx\n", handle[i]);
    for (i = 4099; i >= 4098; i--)
        if (!file1.DeleteRecord (handle[i]))
            printf ("Delete failed: handle %lx\n", handle[i]);
}

main ()
{
    CL_MemoryLeakChecker cheker (cout);
    if (!CL_BinaryFile::Exists (FILENAME))
        WriteFile ();
    CL_PrivateSlottedFile file1 (FILENAME);
    CL_ByteString buffer (SLOT_SIZE);
    printf ("Slot size: %ld User header size %d allocated %ld slots\n",
            file1.SlotSize(), file1.HeaderSize(), file1.SlotsAllocated());
    CL_SlottedFileHandle h;

    // Iterate one way
    printf ("Iteration test 1\n");
    h = file1.FirstRecord (buffer);
    while (h) {
        printf ("Handle %lx data %ld\n", h, buffer.LongValueAt (0));
        h = file1.NextRecord (h, buffer);
    }

    // Now iterate another way
    printf ("Iteration test 2\n");
    CL_SlottedFileIterator itr (file1);
    itr.Reset ();
    do {
        h = itr.Next (buffer);
        if (h == 0) break;
        printf ("Handle %lx data %ld\n", h, buffer.LongValueAt (0));
    } while (1);
return 0;
}

    
        
