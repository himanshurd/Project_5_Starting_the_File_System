#include "mkfs.h"
#include "block.h"
#include "image.h"

void mkfs(void){
    for(int i=0; i < NUMBER_OF_BLOCKS; i++){
        bwrite(0,0);
    }
    for(int i=0; i < 7; i++){
        alloc();
    }
    return;
}