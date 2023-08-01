
#include "../type.h"

typedef struct buf BUFFER;

struct buf buffer[NBUF];
struct buf *freelist;
struct semaphore freebuf;

struct devtab devtab[NDEV];

int requests, hits;

struct buf readbuf, writebuf;  // 2 dedicated buffers

int binit()
{
  //readbuf.buf = 0x700000;
  //writebuf.buf = 0x700000 + 1024;
  printf("entering buffers into freelist\n");
  for (int i = 0; i < NBUF; ++i)
  {
    freebuf.value = NBUF;
    buffer[i].lock.value = 1;
    buffer[i].dirty = 0;
    enter_freelist(buffer[i]);
  }

  printf("*********CHECKPOINT1**************");
}

BUFFER *search_dev(dev,blk)
{
  BUFFER *bp = devtab[dev].dev_list;
  while (bp)
  {
    if (bp->dev == dev && bp->blk == blk)
      break;
    bp = bp->next_dev;
  }
}

int out_freelist(BUFFER *bp)
{
  BUFFER *temp = freelist;
  if (bp == temp)
  {
    freelist = freelist->next_free;
    return 1;
  }
  while (temp->next_free)
  {
    if (temp->next_free == bp)
    {
      temp->next_free = bp->next_free;
      return 1;
    }
    temp = temp->next_free;
  }
}

int out_devlist(BUFFER *bp)
{
  BUFFER *temp = devtab[bp->dev].dev_list;
  if (bp == temp)
  {
    devtab[bp->dev].dev_list = temp->next_dev;
    return 1;
  }
  while (temp->next_dev)
  {
    if (temp->next_dev == bp)
    {
      temp->next_dev = bp->next_dev;
      return 1;
    }
    temp = temp->next_dev;
  }
  return 0;
}

int enter_freelist(BUFFER *bp)
{
  BUFFER *temp = freelist;
  if (temp == 0)
  {
    freelist = bp;
    return 0;
  }
  while (temp->next_free)
    temp = temp->next_free;
  temp->next_free = bp;
  return 0;
}

int enter_devlist(BUFFER *bp)
{
  BUFFER *temp = devtab[bp->dev].dev_list;
  if (temp == 0)
  {
    devtab[bp->dev].dev_list = bp;
    return 0;
  }
  while (temp->next_dev)
    temp = temp->next_dev;
  temp->next_dev = bp;
  return 0;
}

struct buf *getblk(int dev, int blk)
{
  printf("*********CHECKPOINT2**************");
  while(1)
  {
    //(1). P(free); // get a free buffer first
    P(&freebuf);
    //(2). if (bp in dev_list)
    BUFFER *bp = search_dev(dev,blk);
    if (bp)
    {
      //(3). if (bp not BUSY)
      if (bp->busy == 0)
      {
        bp->busy = 1;
        //remove bp from freelist;
        out_freelist(bp);
        P(&bp->lock); // lock bp but does not wait
        return bp;
      }
      // bp in cache but BUSY
      V(&freebuf); // give up the free buffer
      //(4). P(bp); // wait in bp queue
      P(&bp->lock);
      return bp;
    }
    // bp not in cache, try to create a bp=(dev, blk)
    lock();
    bp = freelist;
    freelist = freelist->next_free;
    unlock();

    //(5). bp = frist buffer taken out of freelist;
    P(&bp->lock); // lock bp, no wait
    //(6). if (bp dirty)
    if (bp->dirty)
    {
    printf("*********CHECKPOINT3**************");
      awrite(bp); // write bp out ASYNC, no wait
    printf("*********CHECKPOINT4**************");
      continue; // continue from (1)
    }
    //(7). reassign bp to (dev,blk); // mark bp data invalid, not dirty
    if (bp->dev != dev)
    {
      if (bp->dev >= 0)
        out_devlist(bp);
      bp->dev = dev;
      enter_devlist(bp);
    }
    bp->dev = dev;
    bp->blk = blk;
    bp->valid = 0;
    bp->async = 0;
    bp->dirty = 0;
    return bp;
  } // end of while(1)
}

struct buf *bread(int dev, int blk)
{
  BUFFER *bp = getblk(dev, blk);
  if (bp->valid)
    return bp;
  bp->opcode = READ;
  start_io(bp);
  P(&bp->iodone);
  bp->valid = 1;
  return bp;
}

/*int bwrite(struct buf *bp)
{
  // write out bp->buf to bp->blk
  putblock(bp->blk, bp->buf);
  brelse(bp);

  struct buffer *bp;
  if (write new block or a complete block)
    bp = getblk(dev, blk); // get a buffer for (dev,blk)
  else // write to existing block
    bp = bread(dev,blk); // get a buffer with valid data
  //write data to bp;
  //mark bp data valid and dirty (for delayed write-back)
  bp-valid  = 1;
  bp->dirty = 1;
  brelse(bp); // release bp
}*/

/*****************************************************/
int bwrite(BUFFER *bp)
{
  bp->opcode = WRITE;
  start_io(bp);
  //wait for I/O completion;
  P(&bp->iodone);
  bp->valid = 1;
  bp->dirty = 1;
  brelse(bp); // release bp
}

int start_io(BUFFER *bp)
{
  int ps = int_off();
  //enter bp into device I/O queue;
  int dev = bp->dev;
  if (devtab[dev].io_queue == 0)
    devtab[dev].io_queue = bp;
  else
  {
    BUFFER *temp = devtab[dev].io_queue;
    while (temp->next_free)
      temp = temp->next_free;
    temp->next_free = bp;
  }
  //if (bp is first buffer in I/O queue)
  if (devtab[dev].io_queue == bp)
  {
    //issue I/O command for bp to device;
    if (bp->opcode == READ)
      getblock(bp->blk, bp->buf);
    if (bp->opcode == WRITE)
      putblock(bp->blk, bp->buf);
  }
  int_on(ps);
}

int awrite(struct buf *bp)
{
  bp->opcode = WRITE; // for ASYNC write;
  putblock(bp->blk, bp->buf);
  bp->dirty = 0;
  brelse(bp);
}

int brelse(BUFFER *bp)
{
  //(8). if (bp queue has waiter){ V(bp); return; }
  if (bp->lock.value < 0)  // bp has waiter
  {
    V(&bp->lock);
    return;
  }
  //(9). if (bp dirty && free queue has waiter){ awrite(bp); return; }
  if (freebuf.value < 0 && bp->dirty)
  {
    awrite(bp);
    return;
  }
  //(10). enter bp into (tail of) freelist; V(bp); V(free);
  enter_freelist(bp); // enter b pint bfreeList
  bp->busy = 0; // bp non longer busy
  V(&bp->lock); // unlock bp
  V(&freebuf); // V(freebuf)
}

int khits(int y, int z) // syscall hits entry
{
  *(int *)(y) = requests;
  *(int *)(z) = hits;
  return NBUF;
}
