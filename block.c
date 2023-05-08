#include "block.h"
#include "image.h"
#include "free.h"
#include <unistd.h>
#include <stdlib.h>

unsigned char *bread(int block_num, unsigned char *block)
{
    lseek(image_fd, block_num * 4096, SEEK_SET);
    read(image_fd, block, 4096); 
    return block;
}

void bwrite(int block_num, unsigned char *block)
{
    lseek(image_fd, block_num * 4096, SEEK_SET);
    write(image_fd, block, 4096);
}

int alloc(void)
{
    unsigned char data_buffer[4096] = { 0 };
    int free_block_num;
    
    bread(FREE_DATA_BLOCK_NUM, data_buffer);
    free_block_num = find_free(data_buffer);
    
    if (free_block_num != -1) {
        set_free(data_buffer, free_block_num, 1);
        bwrite(FREE_DATA_BLOCK_NUM, data_buffer);
    }
    
    return free_block_num;
}