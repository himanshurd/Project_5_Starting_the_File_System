#ifndef MKFS_H
#define MKFS_H

#define NUMBER_OF_BLOCKS 1024

#define DIRECTORY_ENTRY_LENGTH 32
#define DIRECTORY_FLAG 2
#define DIRECTORY_START_SIZE DIRECTORY_ENTRY_LENGTH*2
#define FILE_NAME_OFFSET 2

void mkfs(void);

struct directory {
    struct inode *inode;
    unsigned int offset;
};

struct directory_entry {
    unsigned int inode_num;
    char name[16];
};

struct directory *directory_open(int inode_num);
int directory_get(struct directory *dir, struct directory_entry *ent);
void directory_close(struct directory *d);

#endif