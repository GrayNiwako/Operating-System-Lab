#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "defrag.h"

FILE *fin, *fout;
int block_size; // 一个block块的大小
int data_region_start; // 数据区相对于整个磁盘映像文件开头的偏移量
int data_block_count = 0; // 已经写入fout的数据块个数
int size_not_read; // 对于inode索引指向的文件，还没有被读取到的大小

void new_block(int index, int *inode_iblock);

int main(int argc, char* argv[])
{
	char outfilename[20]; // 碎片整理后磁盘映像文件的名称
	int inode_block_number; // inode region占内存的块数
	int inode_number; // 整个磁盘映像文件中inode的个数
	int data_block_number = 0; // 数据区中的块数

	if ((fin = fopen(argv[1], "r")) == NULL) {
		fprintf(stderr, "error: open file <%s> fail\n", argv[1]);
		exit(1);
	}
	strcpy(outfilename, argv[1]);
	strcpy(outfilename + strlen(argv[1]) - 4, "-defrag.txt");
	if ((fout = fopen(outfilename, "w+")) == NULL) {
		fprintf(stderr, "error: open file <%s> fail\n", outfilename);
		exit(1);
	}

	// 复制写入引导块
	char *bootblock = malloc(512);
	fread(bootblock, 512, 1, fin);
	fwrite(bootblock, 512, 1, fout);
	free(bootblock);

	// 复制写入superblock
	SuperBlock *superblock = (SuperBlock*)malloc(512);
	fread(superblock, 512, 1, fin);
	fwrite(superblock, 512, 1, fout);
	block_size = superblock->size;
	data_region_start = 1024 + superblock->data_offset * block_size;
	inode_block_number = superblock->data_offset - superblock->inode_offset;
	inode_number = inode_block_number * block_size / sizeof(Inode);
	data_block_number = superblock->swap_offset - superblock->data_offset;

	// 为inode region预留内存空间
	char *inode_region = malloc(inode_block_number * block_size);
	fwrite(inode_region, inode_block_number * block_size, 1, fout);
	free(inode_region);

	// 遍历所有inode
	// 对于有效的inode，遍历它的直接块、一级块、二级块、三级块，进行数据块位置和索引的更新操作
	// 将无效的和更新后的inode写入fout
	int i, j;
	Inode *inode = (Inode*)malloc(sizeof(Inode));
	for (i = 0; i < inode_number; i ++) {
		fseek(fin, 1024 + i * sizeof(Inode), SEEK_SET);
		fread(inode, sizeof(Inode), 1, fin);
        if (inode->nlink != 0) {
        	size_not_read = inode->size;
            for (j = 0; j < N_DBLOCKS &&  size_not_read > 0; j ++)
            	new_block(0, &inode->dblocks[j]);
            for (j = 0; j < N_IBLOCKS &&  size_not_read > 0; j ++)
            	new_block(1, &inode->iblocks[j]);
            if (size_not_read > 0)
            	new_block(2, &inode->i2block);
            if (size_not_read > 0)
            	new_block(3, &inode->i3block);
        }
        int offset = ftell(fout);
        fseek(fout, 1024 + i * sizeof(Inode), SEEK_SET);
        fwrite(inode, sizeof(Inode), 1, fout);
        fseek(fout, offset, SEEK_SET);
	}

	// 建立新的空闲块列表并写入fout
	int *buffer;
	for (i = data_block_count; i < data_block_number; i ++) {
		buffer = (int *)malloc(block_size);
		if (i != data_block_number -1)
			*buffer = i + 1;
		else
			*buffer = -1;
		fwrite(buffer, block_size, 1, fout);
		free(buffer);
	}

	// 复制写入交换区
	fseek(fin, 0, SEEK_END);
	int swap_size = ftell(fin) - (1024 + superblock->swap_offset * block_size);
	if (swap_size > 0) {
		char *swap_region = malloc(swap_size);
		fseek(fin, 1024 + superblock->swap_offset * block_size, SEEK_SET);
		fread(swap_region, swap_size, 1, fin);
		fwrite(swap_region, swap_size, 1, fout);
		free(swap_region);
	}

	// 更新superblock的空闲块列表头
	fseek(fout, 512, SEEK_SET);
	fread(superblock, 512, 1, fout);
	superblock->free_iblock = data_block_count;
	fseek(fout, 512, SEEK_SET);
	fwrite(superblock, 512, 1, fout);

	fclose(fin);
	fclose(fout);
	free(superblock);
	free(inode);
	return 0;
}

// 递归地将数据块放在新的位置，同时更新索引
// index是目录级数，为0表示直接块，为1表示一级块，为2表示二级块，为3表示三级块
// *inode_iblock是inode中指向对应数据块的指针
void new_block(int index, int *inode_iblock)
{
	int *buffer = (int *)malloc(block_size);
	fseek(fin, data_region_start + (*inode_iblock) * block_size, SEEK_SET);
	fread(buffer, block_size, 1, fin);

	if (index == 0) {
		*inode_iblock = data_block_count;
		data_block_count ++;
		size_not_read -= block_size;
	}
	else {
		for (int i = 0; i < block_size / sizeof(int) && size_not_read > 0; i ++) {
			new_block(index - 1, (buffer + i));
		}
		*inode_iblock = data_block_count;
		data_block_count ++;
	}

	fwrite(buffer, block_size, 1, fout);
	free(buffer);
}
