/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdlib.h>
#include <stdio.h>
#include "winlock.h"


namespace iothread {

#define DEFAULT_BUFFER_SIZE 512
#define NUM_BUFS 2
#define FLUSH_FLAG -1
#define SHUTDOWN_FLAG -2
#define FILECLOSE_FLAG -3

class Buffer {
    public:
    Buffer *_next;
    char *_data;
    int _size;
    int _handle;

    Buffer(int size)
    {
        _next = NULL;
        _size = size;
        _data = (char *) malloc(size);
        _handle = 0;
    }
    Buffer()
    {
        _next = NULL;
        _size = 0;
        _data = (char *) malloc(DEFAULT_BUFFER_SIZE);
        _handle = 0;
    }
    ~Buffer()
    {
        free(_data);
    }

    int getSize() { return _size; }
};

class BufferQueue {
    Buffer *_queue;
    int _size;

    public:
    BufferQueue() {
        _queue = NULL;
        _size = 0;
    }

    void add(Buffer *toAdd) {  // adds to end of list.
        //printf("Adding buffer size %i\n",toAdd->_size);
        Buffer *temp=_queue;
        toAdd->_next = NULL;
        if(temp == NULL) {
            _queue = toAdd;
        } else {
            while(temp->_next != NULL) { temp = temp->_next;  }
            temp->_next = toAdd;
        }
        _size++;
    }

    Buffer *get() {             // retrieves from beginning of list.
//      printf("Hello");
        Buffer *temp = _queue;
        if(temp != NULL) {
            _queue = _queue->_next;
        }
        temp->_next = NULL;
        _size--;
        if(temp == NULL) printf("Error! returning null buffer!");
        return(temp);
    }

    int size() { return(_size); }

    int isEmpty() {
        int result;
        if(_queue == NULL) result = 1;
        else result = 0;
        return(result);
    }

    ~BufferQueue() {
        Buffer *temp;
        if(_size != 0) printf("error: bufferQueue dying with buffers in it!");
        while(_queue != NULL) {
            temp = _queue->_next;
            free(_queue);
            _queue = temp;
        }
    }


};

class BufferFactory {
    BufferQueue free, full;

    int io_threadid; // thread id of the I/O thread.
    __semaphore flush_flag,full_wait,free_wait;  // 0 when waiting for a full or free buffer.
    enum ThreadState { AWAKE, ASLEEP };
    volatile int input_state, output_state; //state of the input and output threads.

    __lock list_access;
    int _buf_size;

    static void _writeBuffers(void *parm);

    public:
    BufferFactory(int buf_size);
    ~BufferFactory();
    Buffer *getBuffer();
    void bufferFilled(Buffer *buf);
    Buffer *swapBuffer(Buffer *buf); // accepts a full buffer and returns an empty one.
    void closeFile(int handle); // closes the given handle once all prior buffers are written.
    void closeFile(int handle, Buffer *buf); // same as above, but more efficient.
    // do not pass a full buffer to this function: it will NOT get written. For use with SwapBuffer.
};

} // end of namespace
