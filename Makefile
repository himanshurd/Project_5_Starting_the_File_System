.PHONY: test 

simfs.a: free.o mkfs.o inode.o image.o block.o	pack.o
	ar rcs $@ $^

mkfs: mkfs.o simfs.a
	gcc -o $@ $^

block.o: block.c
	gcc -Wall -Wextra -c $<

image.o: image.c
	gcc -Wall -Wextra -c $<

inode.o: inode.c
	gcc -Wall -Wextra -c $<

free.o: free.c
	gcc -Wall -Wextra -c $<

mkfs.o: mkfs.c
	gcc -Wall -Wextra -c $<

ls.o: ls.c
	gcc -Wall -Wextra -c $<

test: simfs_test
	./simfs_test

pack.o: pack.c
	gcc -Wall -Wextra -c $<

simfs_test: simfs_test.c simfs.a
	gcc -Wall -Wextra -o $@ $^ -DCTEST_ENABLE 

clean: 
	rm -f *.a *.o *_test test_block test_image test_invalid_image test_block test_block_overwriting test_inode
