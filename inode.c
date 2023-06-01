#include "inode.h"
#include "block.h"
#include "free.h"
#include "pack.h"
#include <stddef.h>
#include <string.h>

static struct inode incore[MAX_SYS_OPEN_FILES] = {0};

struct inode *ialloc(void)
{
    unsigned char inode_buffer[BLOCK_SIZE] = { 0 };
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
        return NULL;
    }

    struct inode *in = iget(free_inode_num);
    if(in == NULL) {
        return NULL;
    }

    in->size = 0;
    in->owner_id = 0;
    in->permissions = 0;
    in->flags = 0;

    for(int i=0; i<INODE_PTR_COUNT; i++){
        in->block_ptr[i]=0;
    }
    in->inode_num = free_inode_num;
    write_inode(in);
    return in;
}

struct inode *find_incore_free(void) {
    for(int i=0; i<MAX_SYS_OPEN_FILES; i++){
        if(incore[i].ref_count ==0){
            return &incore[i];
        }
    }
    return NULL;
}

struct inode *find_incore(unsigned int inode_num){
    for (int i=0; i<MAX_SYS_OPEN_FILES; i++){
        if(incore[i].ref_count !=0){
            if(incore[i].inode_num == inode_num){
                return &incore[i];
            }
        }
    }
    return NULL;
}

void read_inode(struct inode *in, int inode_num){
    int block_num = inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = inode_num % INODES_PER_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;

    unsigned char block[BLOCK_SIZE];
    bread(block_num, block);
    in->size = read_u32(block + block_offset_bytes + 0);
    in->owner_id = read_u16(block + block_offset_bytes + 4);
    in->permissions = read_u8(block+block_offset_bytes+6);
    in->flags = read_u8(block+block_offset_bytes+7); 
    in-> link_count = read_u8(block + block_offset_bytes+8);

    int offset_bytes = block_offset_bytes +9;
    for(int i=0; i<INODE_PTR_COUNT; i++){
        in->block_ptr[i] = read_u16(block+offset_bytes);
        offset_bytes +=2; 
    }
}

void write_inode(struct inode *in){
    int block_num = in->inode_num / INODES_PER_BLOCK + INODE_FIRST_BLOCK;
    int block_offset = in->inode_num % INODES_PER_BLOCK;
    int block_offset_bytes = block_offset * INODE_SIZE;

    unsigned char block[BLOCK_SIZE];
    bread(block_num, block);
    write_u32(block+block_offset_bytes+0, in->size);
    write_u16(block+block_offset_bytes+4, in->owner_id);
    write_u8(block+block_offset_bytes+6, in->permissions);
    write_u8(block+block_offset_bytes+7, in->flags);
    write_u8(block+block_offset_bytes+8, in->link_count);    

    int offset_bytes = block_offset_bytes +9;
    for(int i=0; i<INODE_PTR_COUNT; i++){
        write_u16(block+offset_bytes,in->block_ptr[i]);
        offset_bytes +=2;
    }
    bwrite(block_num, block);
}

struct inode *iget(int inode_num){
    struct inode *incore_node;

    incore_node = find_incore(inode_num);
    if(incore_node != NULL) {
        incore_node->ref_count++;
        return incore_node;
    }

    incore_node = find_incore_free();
    if(incore_node == NULL){
        return NULL;
    }

    read_inode(incore_node, inode_num);
    incore_node->ref_count = 1;
    incore_node->inode_num = inode_num;
    return incore_node;
}


void iput(struct inode *in){
    if(in->ref_count == 0){
        return;
    }
    in->ref_count--;
    if(in->ref_count == 0){
        write_inode(in);
    }
}

struct inode *namei(char *path){
    if(strcmp(path, "/") == 0) {
        struct inode* root = iget(ROOT_INODE_NUM);
        return root;
    }
    return NULL;
}

