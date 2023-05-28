#include "mkfs.h"
#include "block.h"
#include "image.h"
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "inode.h"
#include "pack.h"

void mkfs(void){
    for(int i=0; i < NUMBER_OF_BLOCKS; i++){
        bwrite(0,0);
    }
    for(int i=0; i < 7; i++){
        alloc();
    }
    struct inode *root_node = ialloc();
    int root_block = alloc();
    root_node->block_ptr[0] = root_block;
    root_node->flags = 2;
    root_node->size = 32*2;
    unsigned char block[BLOCK_SIZE];
    write_u16(block, root_node->inode_num);
    strcpy((char*)block+2, ".");
    write_u16(block+32, root_node->inode_num);
    strcpy((char*)block+2+32, "..");   
    bwrite(root_node->inode_num, block);
    iput(root_node);
}