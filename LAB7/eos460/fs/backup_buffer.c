
#include "../type.h"

struct buf buffer[NBUF];
struct buf *freelist;
struct semaphore freebuf;

struct devtab devtab[NDEV];

int requests, hits;

struct buf readbuf, writebuf;  // 2 dedicated buffers

int binit()
{
  readbuf.buf = 0x700000;
  writebuf.buf = 0x700000 + 1024;
}


BUFFER *bread(dev,blk) // return a buffer containing valid data
{
  BUFFER *bp = getblk(dev,blk); // get a buffer for (dev,blk)
  if (bp->valid)
    return bp;
  bp->opcode = READ; // issue READ operation
  start_io(bp); // start I/O on device
  //wait for I/O completion;
  P(&bp->iodone);
  return bp;
}

int bwrite(BUFFER *bp)
{
  bp->opcode = WRITE;
  bp-valid  = 1;
  bp->dirty = 1;
  start_io(bp);
  //wait for I/O completion;
  brelse(bp); // release bp
}

int awrite(BUFFER *bp)
{
  bp->opcode = ASYNC; // for ASYNC write;
  start_io(bp);
}

int start_io(BUFFER *bp)
{
  int ps = int_off();
  //enter bp into device I/O queue;
  int dev = bp->dev;
  BUFFER *temp = devtab[dev].ioqueue;
  if (temp == NULL)
    devtab[dev].dev_list = bp;
  else
    while (temp)
    {
      if (temp->
    }
  //if (bp is first buffer in I/O queue)
  //issue I/O command for bp to device;
  if (bp == devtab[dev].io_queue)
  {
    if(bp->opcode == READ)
      getblock(bp->blk, bp->buf);
    else // WRITE
      putblock(bp->blk, bp->buf);
  }
  int_on(ps);
}

int InterruptHandler()
{
  //bp = dequeue(device I/O queue); // bp = remove head of I/O queue
  //(bp->opcode == ASYNC)? brelse(bp) : unblock process on bp;
  //if (!empty(device I/O queue))
  //issue I/O command for first bp in I/O queue;
}

/* getblk: return a buffer=(dev,blk) for exclusive use */
BUFFER *getblk(dev,blk)
{
  while(1)
  {
    //(1). search dev_list for a bp=(dev, blk);
    BUFFER *bp = devtab[dev].dev_list;
    while (bp)
    {
      if (bp->blk == blk)
        break;
      bp = bp->next_dev;
    }
    //(2). if (bp in dev_lst)
    if (bp)
    {
      //if (bp BUSY)
      if (bp->busy)
      {
        //set bp WANTED flag;
        bp->wanted = 1;
        sleep(bp); // wait for bp to be released
        continue; // retry the algorithm
      }
      /* bp not BUSY */
      //take bp out of freelist;
      if (freelist == bp);
        freelist = freelist->next_free;
      else
      {
        BUFFER *temp = freelist;
        while (temp)
        {
          if (temp->next_free == bp)
          {
            temp->next_free = bp->next_free;
            break;
          }
          temp = temp->next_free;
        }
      }
      //mark bp BUSY;
      bp->busy = 1;
      return bp;
    }
    //(3). /* bp not in cache; try to get a free buf from freelist */
    if (freelist == NULL)
    {
      //set freelist WANTED flag;
      freelist_wanted = 1;
      sleep(freelist); // wait for any free buffer
      continue; // retry the algorithm
    }
    //(4). /* freelist not empty */
    //bp = first bp taken out of freelist;
    bp = freelist;
    freelist = freelist->next_free;
    //mark bp BUSY;
    bp->busy = 1;
    if (bp->dirty)  // bp is for delayed write
    {
      awrite(bp); // write bp out ASYNC;
      continue; // from (1) but not retry
    }
    //(5). reassign bp to (dev,blk); // set bp data invalid, etc.
    bp->valid = 0;
    return bp;
  }
}

/** brelse: releases a buffer as FREE to freelist **/
brelse(BUFFER *bp)
{
  if (bp->wanted)
    wakeup(bp); // wakeup ALL proc's sleeping on bp;
  if (freelist_wanted)
    wakeup(freelist); // wakeup ALL proc's sleeping on freelist;
  clear bp and freelist WANTED flags;
  insert bp to (tail of) freelist;
}

/*int write_block(dev, blk, data) // write data from U space
{
  BUFFER *bp = bread(dev,blk); // read in the disk block first
  //write data to bp;
  bwrite(bp);
  //(synchronous write)? bwrite(bp) : dwrite(bp);
}*/

/*int dwrite(BUFFER *bp)
{
  //mark bp dirty for delay_write;
  bp->dirty = 1;
  brelse(bp); // release bp
}
*/





/*
struct buf *getblk(int dev, int blk)
{
  // just need a buffer, so let it be the writebuf
  struct buf *bp = &writebuf;
  bp->dev = dev;
  bp->blk = blk;
  bp->dirty = 0;
  return bp;
}

struct buf *bread(int dev, int blk)
{
  // use readbuf to read in a block; return readbuf pointer
  //printf("bread %d %d ", dev, blk);

  struct buf *bp = &readbuf;
  bp->dev = dev;
  bp->blk = blk;
  getblock(blk, bp->buf);
  //printf("end bread\n");
  return bp;
}

int bwrite(struct buf *bp)
{
  // write out bp->buf to bp->blk
  putblock(bp->blk, bp->buf);
}

int awrite(struct buf *bp)
{
  // no async write; all writes are immediate
  bwrite(bp);
}

int brelse(struct buf *bp)
{
  // no need to release any buffer
}

int khits(int y, int z) // syscall hits entry
{
  *(int *)(y) = requests;
  *(int *)(z) = hits;
  return NBUF;
}
*/
