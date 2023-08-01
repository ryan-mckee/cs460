int search(INODE *ip, char *name)
{
  char buf[1024];
  int i;
  char c, *cp;
  DIR *dp;
  for (i=0; i<12; i++)
  {
    if (ip->i_block[i])
    {
      getblk(ip->i_block[i], buf);
      dp = (DIR *)buf;
      cp = buf;
      while (cp < &buf[1024])
      {
        c = dp->name[dp->name_len]; // save last byte
        dp->name[dp->name_len] = 0;
        uprintf("%s ", dp->name);
        if ( strcmp(dp->name, name) == 0 )
        {
          uprintf("found %s\n", name);
          return(dp->inode);
        }
        dp->name[dp->name_len] = c; // restore that last byte
        cp += dp->rec_len;
        dp = (DIR *)cp;
      }
    }
  }
  uprintf("serach failed\n");
  return 0;
}

int boot()
{
  char   *name[2];
  name[0]="boot";
  name[1]="eos";
  char   *address = 0x100000;  // EOS kernel loading address

  /*****************************
   sdimage is a disk image containing an EXT2 FS
   EOS kernel image is the file /boot/eos

   t.c:      booter();

             -------- YOUR task is to these -------
   booter(): FIND the file /boot/eos
             load its data blocks to 1MB = 0x100000
             return 1 to caller
             -------------------------------------
   caller:   call go() in ts.s:
             mov pc, 0x100000

  ****************************/
  //1. Get inodes start block# from GD in block#2:
  GD *gp;
  INODE *ip;
  DIR *dp;
  char buf[1024];
  u32 ubuf[256];

  getblk(2, buf);
  gp = (GD *)buf;
  int iblk = gp->bg_inode_table;  // print as %d to see its value

  uprintf("iblk=%d\n", iblk);

  //2. get the block iblk containing root inode into buf[]
  getblk(iblk, buf); // read first inode block block
  ip = (INODE *)buf + 1; // ip->root inode #2

  //3. search for "boot" in data block of / INODE
  //Once you find "boot", you have its ino in [inode, rec_len, naem_len, boot ]
  for (int i=0; i<2; i++)
  {
    int ino = search(ip, name[i]) - 1;
    if (ino < 0)
      return 0;
    /* 4. use Mailman's algorithm to get INODE of boot:
    blk = inodes_start_block + (ino-1)/8;
    offset = (ino-1) % 8
    get blk into a buf[], let IP -> INODE of /boot at offset*/
    getblk(iblk+(ino/8), buf); // read inode block of ino
    ip = (INODE *)buf + (ino % 8);
  }
  //5. Search for "eos" in data block of /boot's INODE to get its ino
  //6. load eos's INDOE block into a buf[] and
  //let  INODE *ip -> INODE of /boot/eos

  //7. ip->i_block[0]-[11] are direct blocks
  int count = 0;
  uprintf("loading direct blocks\n");
  for (int i = 0; i < 12; ++i)
  {
    uprintf("ip->i_block[%d]=%d\n", i, ip->i_block[i]);
    getblk(ip->i_block[i], address);
    address += 1024;
    ++count;
  }

  //8. ip->i_block[12] are indirect blocks; load them into
  //indirect blocks will be ubuf[0], ubud[1],..., until 0
  uprintf("loading indirect blocks\n");
  if (ip->i_block[12])
  {
    getblk(ip->i_block[12], ubuf);
    for (int i = 0; i < 256 && ubuf[i]; ++i)
    {
      uprintf("%d  ", ubuf[i]);
      getblk(ubuf[i], address);
      address += 1024;
      ++count;
    }
    uprintf("\n\n%d\n", count);
  }
}
