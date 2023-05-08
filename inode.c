#include "inode.h"
#include "block.h"
#include "free.h"

int ialloc(void)
{
    unsigned char inode_buffer[4096] = { 0 };
    int free_inode_num;

    bread(FREE_INODE_BLOCK_NUM, inode_buffer);
    free_inode_num = find_free(inode_buffer);

    if (free_inode_num != -1)
    {
        set_free(inode_buffer, free_inode_num, 1);
        bwrite(FREE_INODE_BLOCK_NUM, inode_buffer);
    }
    else
    {
        return -1; 
    }

    return free_inode_num;
}