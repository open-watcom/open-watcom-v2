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


/***************************************\
*      Filebuff.cpp
*
*
\***************************************/
#include "thrdbuff.h"
#include <io.h>
#include <process.h>


namespace iothread {

#define STACK_SIZE 8192


static void BufferFactory::_writeBuffers(void *parm)
{
    BufferFactory *factory = (BufferFactory *)parm;
    Buffer *buf;
    int result=0;

    while(1) { // endless loop.
        // do we have more to kill?

        //printf("Writer waiting for buffer\n");
        factory->list_access.p();
        if(factory->full.size()<=0)
        {
            factory->output_state=ASLEEP;
            factory->list_access.v();
            factory->full_wait.decrease();
            factory->list_access.p();
            factory->output_state=AWAKE;
        }
        buf = factory->full.get();
        factory->list_access.v();

        //printf("Write recieved buffer\n");

        if(buf == NULL) {
            fprintf(stderr,"Error in asynchronous writer: NULL buffer. ");

        } else if(buf->_size < 0) {
            //printf("Writer thread flushing.\n");

            // check to see if the BufferFactory is being killed.
            if(buf->_size == SHUTDOWN_FLAG) {
                //printf("****** Writer thread shutdown.\n");
                factory->flush_flag.increase();
                buf->_size = 0;
                factory->list_access.p();
                factory->free.add(buf);
                factory->list_access.v();
                if((factory->input_state == ASLEEP)&&(factory->free.size()==1))
                {
                    factory->free_wait.increase();
                }
                _endthread();
            } else if(buf->_size == FILECLOSE_FLAG) {
                //printf("****** Closing file handle %i.\n",buf->_handle);
                buf->_size = 0;
                if(buf->_handle != -1)
                   close(buf->_handle);
                factory->list_access.p();
                factory->free.add(buf);
                if((factory->input_state == ASLEEP)&&(factory->free.size()==1))
                {
                    factory->free_wait.increase();
                }
                factory->list_access.v();
            } else {
                buf->_size = 0;
                factory->flush_flag.increase();
                factory->list_access.p();
                factory->free.add(buf);
                if((factory->input_state == ASLEEP)&&(factory->free.size()==1))
                {
                    factory->free_wait.increase();
                }
                factory->list_access.v();
            }
        } else if(buf->_size == 0) {
            //printf("Buffer of size 0. Not writing.\n");
            factory->list_access.p();
            factory->free.add(buf);
            if((factory->input_state == ASLEEP)&&(factory->free.size()==1))
            {
                factory->free_wait.increase();
            }
            factory->list_access.v();

        } else {
            if(buf->_handle >= 0)
            {
                result = write(buf->_handle, buf->_data, buf->_size);
                //printf("Wrote %i bytes\n", buf->_size);
            }
            buf->_size = 0;
            factory->list_access.p();
            factory->free.add(buf);
            if((factory->input_state == ASLEEP)&&(factory->free.size()==1))
            {
                factory->free_wait.increase();
            }
            factory->list_access.v();
        }
        //printf("* Writing process: returned buffer\n");

    }
//    printf("Exiting writer\n");
}

BufferFactory::BufferFactory(int buf_size)
        : flush_flag(0,1)
        , full_wait(0,NUM_BUFS)
        , free_wait(0,NUM_BUFS)
        , _buf_size(buf_size)
        , input_state(AWAKE)
        , output_state(AWAKE)
{
    // create two new buffers.
    Buffer *buf;
    for(int i = 0; i<NUM_BUFS; i++) {
        buf = new Buffer(_buf_size);
        free.add(buf);
    }
    io_threadid = _beginthread(_writeBuffers, STACK_SIZE, this);
}

BufferFactory::~BufferFactory()
{
    // kill the writer thread by sending a 'poison buffer'
    // with size set to SHUTDOWN_FLAG;
    list_access.p();
    if(free.size()<=0)
    {
        input_state=ASLEEP;
        list_access.v();
        free_wait.decrease();
        list_access.p();
        input_state=AWAKE;
    }
    Buffer *buf = free.get();
    list_access.v();
    buf->_size = SHUTDOWN_FLAG;

    list_access.p();
    full.add(buf);
    if((output_state == ASLEEP)&&(full.size()==1))
    {
        full_wait.increase();
    }
    list_access.v();
    //printf("Waiting for writer to flush\n");
    flush_flag.decrease(); // we can't decrease until the writer sees the SHUTDOWN_FLAG

    // clean out all 'freed' buffers. (We can't worry about buffers not yet returned.)
    //while( !free.isEmpty() ) {
    for(int i = 0; i<NUM_BUFS; i++) {
        list_access.p();
        if(free.size()<=0)
        {
            input_state=ASLEEP;
            list_access.v();
            free_wait.decrease();
            list_access.p();
            input_state=AWAKE;
        }
        buf=free.get();
        list_access.v();
        delete(buf);
        //printf("Deleted buffer\n");
    }

    if(!full.isEmpty()) { // then not all buffers were written .... oops.
        fprintf(stderr,"Error! not all buffers written!\n");
    }
}


void BufferFactory::bufferFilled(Buffer *buf) {
    list_access.p();
    full.add(buf);
    if((output_state == ASLEEP)&&(full.size()==1))
    {
        full_wait.increase();
    }
    list_access.v();
}

Buffer *BufferFactory::getBuffer()
{
    list_access.p();
    if(free.size()<=0)
    {
        input_state=ASLEEP;
        list_access.v();
        free_wait.decrease();
        list_access.p();
        input_state=AWAKE;
    }
    Buffer *temp = free.get();
    list_access.v();
    return(temp);
}

// accepts a full buffer and returns an empty one.
Buffer *BufferFactory::swapBuffer(Buffer *buf)
{
    list_access.p();

    full.add(buf);
    if(free.size()<=0)
    {
        input_state=ASLEEP;
        list_access.v();
        free_wait.decrease();
        list_access.p();
        input_state=AWAKE;
    }
    Buffer *temp = free.get();
    if((output_state == ASLEEP)&&(full.size()==1))
    {
        full_wait.increase();
    }

    list_access.v();

    return(temp);
}

void BufferFactory::closeFile(int handle)
{
    // send 'FILECLOSE_FLAG' buffer:
    list_access.p();

    if(free.size()<=0)
    {
        input_state=ASLEEP;
        list_access.v();
        free_wait.decrease();
        list_access.p();
        input_state=AWAKE;
    }
    Buffer *buf = free.get();

    buf->_size = FILECLOSE_FLAG;
    buf->_handle = handle;

    full.add(buf);
    list_access.v();
    if((output_state == ASLEEP)&&(full.size()==1))
    {
        full_wait.increase();
    }
}

// do not pass a full buffer to this function: it will NOT get written. For use with SwapBuffer.
void BufferFactory::closeFile(int handle, Buffer *buf)
{
    // send 'FILECLOSE_FLAG' buffer:

    buf->_size = FILECLOSE_FLAG;
    buf->_handle = handle;

    list_access.p();
    full.add(buf);
    list_access.v();
    if((output_state == ASLEEP)&&(full.size()==1))
    {
        full_wait.increase();
    }
}

} // end of namespace iothread;



