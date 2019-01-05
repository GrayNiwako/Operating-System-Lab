#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defrag.h"

//对于磁盘映像文件，打印superblock的相关参数和所有inode的部分参数
int main(int argc, char* argv[])
{
	FILE *fout;

	if ((fout = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "error: open file <%s> fail\n", argv[1]);
		exit(1);
	}
	fseek(fout, 512, SEEK_SET);
	SuperBlock *s = (SuperBlock*)malloc(512);
	fread(s, 512, 1, fout);

	printf("superblock:\n");
	printf("  size = %d\n", s->size);
	printf("  inode_offset = %d\n", s->inode_offset);
	printf("  data_offset = %d\n", s->data_offset);
	printf("  swap_offset = %d\n", s->swap_offset);
	printf("  free_inode = %d\n", s->free_inode);
	printf("  free_iblock = %d\n\n", s->free_iblock);

	int inode_block_number = s->data_offset - s->inode_offset;
	int inode_number = inode_block_number * s->size / sizeof(Inode);
	int i, j;

	Inode *ind = (Inode*)malloc(sizeof(Inode));
	for (i = 0; i < inode_number; i ++) {
		fread(ind, sizeof(Inode), 1, fout);
		printf("inode:\n");
		printf("  next_inode = %d\n", ind->next_inode);
		printf("  nlink = %d\n", ind->nlink);
		printf("  dblocks[N_DBLOCKS] = ");
		for (j = 0; j < N_DBLOCKS; j ++)
			printf("%d ", ind->dblocks[j]);
		printf("\n  iblocks[N_IBLOCKS] = ");
		for (j = 0; j < N_IBLOCKS; j ++)
			printf("%d ", ind->iblocks[j]);
		printf("\n  i2block = %d\n", ind->i2block);
		printf("  i3block = %d\n\n", ind->i3block);
	}

	fclose(fout);
	free(s);
	free(ind);
	return 0;
}
