#include "ctest.h"
#include "image.h"
#include "block.h"
#include "free.h"
#include "inode.h"
#include "mkfs.h"
#include <string.h>
#include "ctest.h"

void test_block(void) {
    image_open("test_block", 0);

    unsigned char read_block[4096];
    unsigned char write_block[4096] = "Hello world!";
    
    bwrite(15, write_block);
    CTEST_ASSERT(memcmp(write_block, bread(15, read_block), 2) == 0, "test for block reading and writing");
    image_close();
}

void test_block_overwriting(void){
    image_open("test_block_overwriting", 0);

    unsigned char read_block[4096];
    unsigned char write_block[4096] = "Hello universe";
    bwrite(15, write_block);
    CTEST_ASSERT(memcmp(write_block, bread(15, read_block), 2) == 0, "testing block overwriting");
    image_close();
}


void test_image_valid_open_close()
{
    CTEST_ASSERT(image_open("test_image",0) != -1, "Test for open an image");
    CTEST_ASSERT(image_close() != -1, "test for closing an image");
}


void test_image_invalid_open_close()
{   
    CTEST_ASSERT(image_open("/test_invalid_image", 0) < 0, "Test for opening invalid image");
    CTEST_ASSERT(image_close() == -1, "Test for closing invalid image");
}


void test_mkfs(void) {
    image_open("mkfs_test", 0);
    mkfs();
    CTEST_ASSERT(alloc() == 7, "Testing mkfs function correctly assigns blocks during the initialization process");
    image_close();
}

void find_set_free_empty(void) {
    unsigned char block[4096] = {0};

    CTEST_ASSERT(find_free(block) == 0, "Testing find_free on empty block");

    set_free(block, 0, 1);
    CTEST_ASSERT(find_free(block) == 1, "Testing find_free and set_free with a value of 1");
}

void find_set_free_preexisting(void){
    unsigned char block[4096] = {0};
    set_free(block, 1, 1);
    CTEST_ASSERT(find_free(block) == 2, "Testing find_free and set_free when applied to a block that already contains preexisting 1's, with the objective of setting a value of 1");

    set_free(block, 0, 0);
    CTEST_ASSERT(find_free(block) == 0, "Testing set_free when setting a value of 0, along with find_free, when encountering 1's further within the array");
}

void test_inode(void) {
    image_open("inode_test", 0);

    struct inode *node = find_incore_free();
    node->size = 5;
    node->inode_num = 2;
    node->ref_count = 1;
    CTEST_ASSERT(find_incore(2)->size == 5, "Test to find_incore_ free and find_incore");

    write_inode(node);
    struct inode node2;
    read_inode(&node2, 2);
    CTEST_ASSERT(node2.size == 5, "Test for write inode and read inode");

    CTEST_ASSERT(iget(2)->size == 5, "Test iget for node in core");

    struct inode* node3 = iget(3);
    CTEST_ASSERT(find_incore(3)->ref_count == 1, "Test when iget for node is not in core");

    iget(3);
    iput(node3);
    CTEST_ASSERT(node3->ref_count == 1, "Test iput for ref_count less than 1");

    node3->size = 6;
    iput(node3);
    struct inode node4;
    read_inode(&node4, 3);
    CTEST_ASSERT(node4.size == 6, "Test iput for ref_count equal to 1");

    image_close();
}

int main() 
{
    CTEST_VERBOSE(1);
    
    test_image_valid_open_close();
    test_image_invalid_open_close();
    test_block();
    test_block_overwriting();
    test_inode();
    // test_inode_1();
    test_mkfs();
}