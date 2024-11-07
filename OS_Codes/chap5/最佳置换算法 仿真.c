//
// Created by lisen on 24-11-7.
//
/*
 * 给定一组数字，作为进程调用页面的序号 记为pages
 *  假定最小页号为0
 * 给定一个正整数n，作为分配给该进程的物理块数，对应一个长度为n的数组，作为当前调用物理块的数字，其中存储pages的页号
 * 程序从pages首位开始 每访问一次 给出当前物理块数组里的状态 如果发生换页 给出换出的页号 同时给出理由
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

// 返回待访问的页面序列中最大的页面号 用于后续生成二维数组
int get_max_page_num(int * pages, int size) {
	int max = -INT_MAX;
	for(int i = 0; i < size; i++)
		if(pages[i] > max)
			max = pages[i];

	return max;
}

// 展示当前块内包含的页号
void display_container(int * container, int len) {
	printf("	now the container is ");
	for(int i = 0; i < len; i++)
		printf("%d ", container[i]);
	putchar('\n');
}

// 判断某个页号是否在块内 因为如果在的话不涉及到置换等操作
int page_in_container(int * container, int len, int curr_page) {
	for(int i = 0; i < len; i++)
		if(container[i] == curr_page)
			return 1;
	return 0;
}

// 根据页号调用序列，记录每个页号被访问的位置
int ** record_page_access_position(int * pages, int size) {
	/*
	 * 以pages中最大页号+1 作为返回的二维数组的外部长度
	 * res[page_id][0] 记录该页被访问的总次数
	 * res[page_id][i] i >= 1, 依次记录被访问的位置
	 */

	// 找到最大页号，作为返回的二维数组的外层长度的基础
	int max = get_max_page_num(pages, size);

	// 创建返回的二维数组
	int ** res = calloc(max+1, sizeof(int *)); // 创建返回的二维数组 外层长度要max+1
	if(res == NULL)
		exit(EXIT_FAILURE);
	// 创建内层数组 其长度可以初始化为size+1，+1是因为0号位置存储该索引对应页号的总访问次数，真正存储访问位置的序号从1开始
	for(int i = 0; i <= max; i++) {
		res[i] = (int *) calloc(size + 1, sizeof(int));
		if(res[i] == NULL)
			exit(EXIT_FAILURE);
	}

	// 为二维数组赋值
	for(int i = 0; i < size; i++) {
		res[pages[i]][0]++;   // 总访问次数+1
		res[pages[i]][res[pages[i]][0]] = i;
	}

	return res;
}

// 用于获取应该置换出内存的页号和该页号在未来的第几次访问中才会被访问
void page_exchange(int ** rec, int max, int * container, int block, int curr_access_idx, int * future_vis_pos, int * out_idx) {
	/*
	 * @params
	 *	rec 记录每个页面总访问次数 和被访问的位置
	 *	max 最大页面号
	 *	container 物理块容器
	 *	block 物理块数
	 *	curr_access_idx 当前访问的位置
	 *	future_vis_pos 未来最近一次要被访问的位置
	 *	out_idx 换出页面在容器内的序号
	 * 计算容器中每个页面未来最近一次要被访问的位置
	 *	计算方法
	 *		从每个页面访问位置集合中遍历到第一个大于当前访问位置的位置 如果没有则为-1
	 * 如果某个位置为-1 表示以后都不会访问 因此直接将该页面换出
	 * 否则换出位置最大的
	 */
	int * future = calloc(block, sizeof(int));
	for(int i = 0; i < block; i++)
		future[i] = -1;
	if(future == NULL)
		exit(EXIT_FAILURE);

	for(int i = 0; i < block; i++) {
		int page = container[i];
		for(int j = 1; j <= rec[page][0]; j++)
			if(rec[page][j] > curr_access_idx) {
				if(future[i] < rec[page][j]) {
					future[i] = rec[page][j];
				}
				break;
			}
	}

	int max_pos = -1;
	for(int i = 0; i < block; i++) {
		if(future[i] == -1) {
			*future_vis_pos = -1;
			*out_idx = i;
			break;
		}
		else {
			if(max_pos < future[i]) {
				max_pos = future[i];
				*future_vis_pos = max_pos;
				*out_idx = i;
			}
		}
	}
}

void display_page_exchange(int * pages, int size, int blocks) {
	int * container = calloc(blocks, sizeof(int));
	if(container == NULL)
		exit(EXIT_FAILURE);

	int max = get_max_page_num(pages, size);
	int container_idx = 0;

	int ** rec = record_page_access_position(pages, size);

	for(int i = 0; i < size; i++) {
		int page = pages[i];
		printf("now accessing no.%d pages with pages number %d\n", i, page);
		if(page_in_container(container, container_idx, page)) {             // 无需换页
			printf("	Page num %d is in the container\n", page);
			display_container(container, container_idx);
		}
		else {
			if(container_idx == blocks) {  // 物理块满 需要置换
				int out_access_pos;
				int out_idx;
				printf("	container is full, page will be exchanged\n");
				page_exchange(rec, max, container, container_idx, i, &out_access_pos, &out_idx);
				if(out_access_pos == -1)
					printf("	page num %d will be removed due to it will not be visited ever. \n", container[out_idx]);
				else
					printf("	page num %d will be removed due to it will not be visited until the %d visit. \n", container[out_idx], out_access_pos);
				container[out_idx] = page;
				display_container(container, container_idx);
			}
			else {                         // 物理块未满 直接进入
				printf("	container is not full : ");
				container[container_idx ++] = page;
				display_container(container, container_idx);
			}
		}
	}
}

int main() {
	int pages[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
	int size = sizeof(pages) / sizeof(int);

	display_page_exchange(pages, size, 3);

	return 0;
}
