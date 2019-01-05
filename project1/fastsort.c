#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include "sort.h"
#include <sys/stat.h>
#include <time.h>

#define MaxFileLength 1000

void usage();
void pOpenError(char*);
void overflow();
void pReadError(char*);
void pWriteError(char*);
int cmp(const void*, const void*);

int main(int argc, char *argv[])
{
    // 变量
    char *inFile = argv[1];

    // 检查参数数量是否正确
	if (argc != 3)
	    usage();

	// 打开文件
	int fp = open(inFile, O_RDONLY);
    if (fp < 0) {
    	pOpenError(inFile);
        exit(1);
    }

    // 计算文件大小和rec_t的个数
    struct stat buf;  
    stat(inFile, &buf);    
    long int number = buf.st_size / sizeof(rec_t);

    rec_t *K_R = (rec_t*)malloc(number*sizeof(rec_t));
    int count = 0;  // 记录读入文件长度
    while (1) {	

    	// 判断已经读入的文件长度是否达到规定的最大文件长度
    	if (count == MaxFileLength) {
    		overflow();
    		(void) close(fp);
            free(K_R);
	    	exit(1);
    	}

    	// 读入文件内容
		int rc;
		rc = read(fp, &K_R[count], sizeof(rec_t));  // 这里包括下一个if判断还没改
		count++;

		// 0表示EOF
		if (rc == 0) {
			count--;
	    	break;
		}

		// 读入出错
		if (rc < 0) {
	    	pReadError(inFile);
            (void) close(fp);
            free(K_R);
            exit(1);
        }

    }

    (void) close(fp);

    // 排序
    qsort(K_R, count, sizeof(rec_t), cmp);
    
    // 输出
    char *outFile = argv[2];
	int fd = open(outFile, O_WRONLY|O_CREAT|O_TRUNC, S_IRWXU);
    if (fd < 0) {
    	pOpenError(outFile);
        free(K_R);
        exit(1);
    }

    int i,rt;
    for (i = 0; i < count; i++) {

		rt = write(fd, &K_R[i], sizeof(rec_t));

		if (rt != sizeof(rec_t)) {
			pWriteError(outFile);
			(void) close(fd);
            free(K_R);
			exit(1);
		}
	}

	(void) close(fd);
    free(K_R);

    return 0;
}

// 若输入的参数数量不正确，则打印并退出
void usage() 
{
    fprintf(stderr, "Usage: fastsort inputfile outputfile\n");
    exit(1);
}

// 若文件打开失败，则打印
void pOpenError(char *prog) 
{
    fprintf(stderr, "Error: Cannot open file %s\n", prog);
}

// 若文件长度超出规定的最大长度，则打印
void overflow() 
{
    fprintf(stderr, "Error: File length too long\n");
}

// 若文件读入失败，则打印
void pReadError(char *prog) 
{
    fprintf(stderr, "Error: Read file %s failed\n", prog);
}

// 若文件写入失败，则打印
void pWriteError(char *prog) 
{
    fprintf(stderr, "Error: Write to file %s failed\n", prog);
}

// 按key的大小进行升序排序
int cmp(const void *a, const void *b)
{
	rec_t *p1 = (rec_t*)a;
	rec_t *p2 = (rec_t*)b;
	if (p1->key > p2->key)
		return 1;
	else if (p1->key == p2->key)
		return 0;
	else
		return -1;
}