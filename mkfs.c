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
        unsigned char block[4096] = {0};
        write(image_fd, block, sizeof(block));
    }
    for(int i=0; i < 7; i++){
        alloc();
    }
    struct inode *root_node = ialloc();
    int root_block = alloc();
    root_node->block_ptr[0] = root_block;
    root_node->flags = DIRECTORY_FLAG;
    root_node->size = DIRECTORY_START_SIZE;
    unsigned char block[BLOCK_SIZE];
    write_u16(block, root_node->inode_num);
    strcpy((char*)block+FILE_NAME_OFFSET, ".");
    write_u16(block+DIRECTORY_ENTRY_LENGTH, root_node->inode_num);
    strcpy((char*)block+FILE_NAME_OFFSET+DIRECTORY_ENTRY_LENGTH, "..");   
    bwrite(root_node->inode_num, block);
    iput(root_node);
}

struct directory *directory_open(int inode_num) {
    struct inode *directory_node = iget(inode_num);
    if (directory_node == NULL) {
        return NULL;
    }
    struct directory *dir = malloc(sizeof(struct directory));
    dir->inode = directory_node;
    dir->offset = 0;
    return dir;
}

int directory_get(struct directory *dir, struct directory_entry *ent)
{
    if (dir->offset >= dir->inode->size) {
        return -1;
    }

    int data_block_index = dir->offset / BLOCK_SIZE;
    int data_block_num = dir->inode->block_ptr[data_block_index];

    unsigned char block[BLOCK_SIZE];
    bread(data_block_num, block);

    int offset_in_block = dir->offset % BLOCK_SIZE;
    ent->inode_num = read_u16(block + offset_in_block);
    strcpy(ent->name, (char *)block + FILE_NAME_OFFSET);
    dir->offset += DIRECTORY_ENTRY_LENGTH;

    return 0;
}

void directory_close(struct directory *d) {
    iput(d->inode);
    free(d);
}