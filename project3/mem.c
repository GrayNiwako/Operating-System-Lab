#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include "mem.h"

typedef struct __node_t {
	int size;
	int free; // free = 1, not_free = 0
} node_t;

int m_error;
int called_time = 0;
void *head;  //initial初始化的头地址指针
void *end;   //申请地址空间的结束地址指针

int mem_init(int size_of_region)
{
	if (size_of_region <= 0 || called_time >= 1) {
		m_error = E_BAD_ARGS;
		return -1;
	}
	called_time ++;

	int page_size = getpagesize();
	if (size_of_region % page_size != 0)
		size_of_region = (size_of_region / page_size + 1) * page_size;  //申请大小为getpagesize()的整数倍
	int fd = open("/dev/zero", O_RDWR);
	head = mmap(NULL, size_of_region, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
	if (head == MAP_FAILED) {
		m_error = E_BAD_ARGS;
		return -1;
	}
	close(fd);

	node_t *current_block = (node_t *)head;  //现在这个头指向的块
	current_block -> size = size_of_region - sizeof(node_t);  //size为实际大小
	current_block -> free = 1;  //free为1，空
	end = head + size_of_region;  //结束地址指针的地址
	return 0;
}

void * mem_alloc(int size, int style)
{
	size = size + sizeof(node_t);
	if (size % 8 != 0)  //以8对齐
		size = (size / 8 + 1) * 8;
	void *address = head;  //address为当前的块的起始地址

	void *alloc_addr = NULL;  //指向用户申请的空间的结构体前
	if (style == M_BESTFIT) {
		int min = INT_MAX;  //min为在这之前最适合的
		while (address < end) {
			node_t *current_block = (node_t *)address;  //当前块的结构体前的起始地址
			if (size <= (current_block -> size + sizeof(node_t)) && current_block -> free == 1) {  //判断当前空块的大小是否满足
				if (min >= current_block -> size + sizeof(node_t)) {
					min = current_block -> size + sizeof(node_t);   //如果当前这一块满足且比之前找到的min还小，则替换min的值
					alloc_addr = address;  //用户申请的空间的起始地址
				}
			}
			address = address + current_block -> size + sizeof(node_t);  //如果当前块不满足则指向下一块
		}
	}
	else if (style == M_WORSTFIT)
	{
		int max = 0;  //max为在这之前最不合适的
		while (address < end) {
			node_t *current_block = (node_t *)address;
			if (size <= (current_block -> size + sizeof(node_t)) && current_block -> free == 1) {
				if (max <= current_block -> size + sizeof(node_t)) {
					max = current_block -> size + sizeof(node_t);  //如果当前这一块满足且比之前找到的max还大，则替换max的值
					alloc_addr = address;
				}
			}
			address = address + current_block -> size + sizeof(node_t);
		}
	}
	else if (style == M_FIRSTFIT)
	{
		while (address < end) {
			node_t *current_block = (node_t *)address;
			if (size <= (current_block -> size + sizeof(node_t)) && current_block -> free == 1) {
				alloc_addr = address;
				break;  //一旦找到满足的块就返回
			}
			address = address + current_block -> size + sizeof(node_t);
		}
	}

	if (alloc_addr == NULL) {
        m_error = E_NO_SPACE;  //如果用户申请空间的起始地址为空则出错
        return NULL;
    }

    node_t *alloc_block = (node_t *)alloc_addr;  //用户申请的块的结构体
    int current_size = alloc_block -> size + sizeof(node_t);
    alloc_block -> free = 0;  //free=0，把这一块置为满
    alloc_block -> size = size - sizeof(node_t);

    node_t *remain_block = (node_t *)(alloc_addr + size);  //满足用户申请后剩下块的结构体
    if (current_size - size - sizeof(node_t) != 0) {  //如果满足用户申请后剩下空间的大小大于0（即不为0）
    	remain_block -> size = current_size - size - sizeof(node_t);
    	remain_block -> free = 1;  //则分配其大小并置其为空块
    }

    return alloc_addr + sizeof(node_t);
}

int mem_free(void * ptr)
{
	if (ptr == NULL || ptr < head || ptr > end) { //出错情况判断
		m_error = E_BAD_POINTER;
		return -1;
	}

	void *address = head;  //从头开始遍历存储空间
	node_t *last_block, *current_block, *next_block;

	current_block = NULL;
	if (address == ptr - sizeof(node_t)) {
		last_block = NULL;  //如果用户申请的空间起始地址为initial初始化的头地址，则当前块为第一块
		current_block = (node_t *)address;
		current_block -> free = 1;
	}
	else {
		while (address < end) {
			if (address == ptr - sizeof(node_t)) {  //找到用户申请空间的地址
				current_block = (node_t *)address;
				current_block -> free = 1;  //当前块为用户申请的块，并置free为1，回收空间
				break;
			}
			last_block = (node_t *)address;
			address += last_block -> size + sizeof(node_t);  //address指向下一块循环的起始地址位置
		}
	}

	if (current_block == NULL) {
		m_error = E_BAD_POINTER;
		return -1;
	}

	if (address + current_block -> size + sizeof(node_t) < end) {
		address += current_block -> size + sizeof(node_t);  //next指向的下一块的大小
		next_block = (node_t *)address;
	}
	else
		next_block = NULL;

	if (next_block != NULL && next_block -> free == 1)  //如果用户申请的当前块的后一块为空，合并这两块空间
		current_block -> size += next_block -> size + sizeof(node_t);
	if (last_block != NULL && last_block -> free == 1)  //如果用户申请的当前块的前一块为空，合并这两块空间
		last_block -> size += current_block -> size + sizeof(node_t);

	return 0;
}

void mem_dump()
{
	void *address = head;
	node_t *current_block;
	while (address < end) {
		current_block = (node_t *)address;
		if (current_block -> free == 1)
			printf("free memory: %p ~ %p\n", address, address + current_block -> size + sizeof(node_t));
		address = address + current_block -> size + sizeof(node_t);
	}
}
