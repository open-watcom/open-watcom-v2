/*
 *  Simple ring-buffer queue handler for reception of packets
 *  from network driver.
 *
 *  Created Jan-1998
 *  Gisle Vanem <giva@bgnett.no>
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "wattcp.h"
#include "pcqueue.h"

#define PKTQ_MARKER  0xDEADBEEF

#if defined(USE_DEBUG)
int pktq_check (struct pkt_ringbuf *q)
{
  int   i;
  char *buf;

  if (!q || !q->num_buf || !q->buf_start)
     return (0);

  buf = q->buf_start;

  for (i = 0; i < q->num_buf; i++)
  {
    buf += q->buf_size;
    if (*(DWORD*)(buf - sizeof(DWORD)) != PKTQ_MARKER)
       return (0);
  }
  return (1);
}
#endif  /* USE_DEBUG */

int pktq_init (struct pkt_ringbuf *q, int size, int num, char *buf)
{
  q->buf_size  = size;
  q->num_buf   = num;
  q->buf_start = buf;
  q->in_index  = 0;
  q->out_index = 0;
#if (DOSX & DOS4GW)
  q->dos_ofs = 0;   /* must be set manually */
#endif

#if defined(USE_DEBUG)
  assert (size);
  assert (num);
  assert (buf);
  {
    int i;
    for (i = 0; i < q->num_buf; i++)
    {
      buf += q->buf_size;
      *(DWORD*) (buf - sizeof(DWORD)) = PKTQ_MARKER;
    }
  }
#endif
  return (1);
}

/*
 * Increment the queue 'out_index' (tail).
 * Check for wraps. 
 */
int pktq_inc_out (struct pkt_ringbuf *q)
{
  q->out_index++;
  if (q->out_index >= q->num_buf)
      q->out_index = 0;
  return (q->out_index);
}

/*
 * Return the queue's next 'in_index' (head).
 * Check for wraps. Caller is responsible for using cli/sti
 * around this function when needed.
 */
int pktq_in_index (struct pkt_ringbuf *q)
{
  int index = q->in_index + 1;

  if (index >= q->num_buf)
      index = 0;
  return (index);
}

/*
 * Return the queue's head-buffer.
 * Should be interruptable because 'in_index' is 'volatile'.
 */
char *pktq_in_buf (struct pkt_ringbuf *q)
{
  return (q->buf_start + (q->buf_size * q->in_index));
}

/*
 * Return the queue's tail-buffer.
 */
char *pktq_out_buf (struct pkt_ringbuf *q)
{
  return (q->buf_start + (q->buf_size * q->out_index));
}

/*
 * Clear the queue ring-buffer by setting head=tail.
 */
void pktq_clear (struct pkt_ringbuf *q)
{
  DISABLE();
  q->in_index = q->out_index;
  ENABLE();
}

/*
 * Return number of buffers waiting in queue. Check for wraps.
 * Should be interruptable because 'in_index' is 'volatile'.
 */
int pktq_queued (struct pkt_ringbuf *q)
{
  register int index = q->out_index;
  register int num   = 0;

  /* DISABLE(); */

  while (index != q->in_index)
  {
    num++;
    index++;
    if (index >= q->num_buf)
        index = 0;
  }
  /* ENABLE(); */

  return (num);
}
