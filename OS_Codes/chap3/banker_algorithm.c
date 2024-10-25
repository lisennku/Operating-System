//
// Created by lisen on 24-10-25.
//
/*
 * 银行家算法实现
	 * 四个数据对象
	 *	- Available
	 *		一维数组 长度和系统资源类型的数目一致
	 *		初始存储系统中每种资源类型可用的全部数据 每个元素随着进程的释放请求动态变化
	 *	- Max
	 *		二维数组 每个元素表示某个进程需要某种资源类型的最大数目
	 *	- Allocate
	 *		二维数组 每个元素表示某个进程当前已经被分配某种资源类型的数目
	 *	- Need
	 *		二维数组 每个元素表示某个进程仍然需要某种资源类型的数目
	 *		初始情况下和Max相同
	 * 算法步骤
	 *	- 银行家算法
	 *		Request表示某个进程的请求向量 长度为资源类型数目 表示对每个资源类型当前的需求数目
	 *		1. 检查request和Need的关系 如果大于 报错 因为此时进程需要的资源数已超过他声明的最大值 如果小于等于 则继续
	 *			其实这里有两种过程
	 *				过程1 初始时Need和Max一致 所以此时Need里的就是最大值
	 *				过程2 非初始时 Need里的数据已经有Max-Allocate计算
	 *		2. 检查request和available的关系 如果大于 停止进程 如果小于等于 则继续
	 *		3. 尝试按照request进行资源分配
	 *			available = available - request
	 *			allocate = allocate + request
	 *			need = need - request
	 *		4. 执行安全性检查
	 *			安全则分配，否则恢复
	 *	- 安全性检查
	 *		1. 使用两个向量
	 *			- Work 每次安全性检查赋值为available 表示系统当前资源数
	 *			- Finish 表示每个进程是否可以获得需要的资源 初始为false
	 *		2. 从进程集合中找到符合要求的进程
	 *			Finish[i] = false && Need[i][j] <= Work[j]
	 *			找到 执行第三部 找不到 执行第四步
	 *		3. 模拟将其所需资源分配并完成，则
	 *			work = work + allocate[i]
	 *			finishi=true
	 *			重复执行第二步
	 *		4. 如果所有Finish都为True 则安全，否则不安全
 * 代码目的
	 * 构造T0时刻的状态 并判断当前时刻是否安全
	 * 在T0时刻的基础上进行进程资源请求 判断请求是否安全
	 * 检查/分配时要打印各个矩阵
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BLANK ' '

typedef struct {
	int res_nums;
	int pro_nums;
	int * available;
	int ** max;
	int ** allocate;
	int ** need;
}Banker;

void init_banker(Banker * b, int p, int r, int * init_res) {

	b->pro_nums = p;
	b->res_nums = r;

	b->available = calloc(r, sizeof(int));
	memcpy(b->available, init_res, r * sizeof(int));

	b->max = calloc(p, sizeof(int *));
	for(int i = 0; i < r; i++)
		b->max[i] = calloc(r, sizeof(int));

	b->allocate = calloc(p, sizeof(int *));
	for(int i = 0; i < r; i++)
		b->allocate[i] = calloc(r, sizeof(int));

	b->need = calloc(p, sizeof(int *));
	for(int i = 0; i < r; i++)
		b->need[i] = calloc(r, sizeof(int));
}

void create_T0_environment(Banker * b, int (*max_res)[b->res_nums], int (*cur_allocate)[b->res_nums]) {
	for(int i = 0; i < b->pro_nums; i++)
		for(int j = 0; j < b->res_nums; j++)
			b->need[i][j] = b->max[i][j] - b->allocate[i][j];
}

void display_matrix(Banker b, char mat_typ) {
	switch (mat_typ) {
		case 'a': {
			for(int j = 0; j < b.res_nums; j++)
				printf("resource %c: %d unit; ",j + 'A', b.available[j]);
			break;
		}
		case 'm': {
			for(int i = 0; i < b.res_nums; i++)
				printf("%5c %5c", BLANK, i + 'A');
			for(int i = 0; i < b.pro_nums; i++, printf("\n"))
				for(int j = 0; j < b.res_nums; j++) {
					printf("%5d", i);
					printf("%5d", b.max[i][j]);
				}
			break;
		}
		case 'l': {
			for(int i = 0; i < b.pro_nums; i++, printf("\n"))
				for(int j = 0; j < b.res_nums; j++) {
					printf("%5d", i);
					printf("%5d",  b.allocate[i][j]);
				}
			break;
		}
		case 'n': {
			for(int i = 0; i < b.pro_nums; i++, printf("\n"))
				for(int j = 0; j < b.res_nums; j++) {
					printf("%5d", i);
					printf("%5d", b.need[i][j]);
				}
			break;
		}
		default:
			printf("error mat_typ para. value; a for available; m for max; l for allocate; n for need\n");
	}
}

int main() {
	const int p = 5;
	const int r = 3;

	int init_res[] = {10, 5, 7};
	int max[p][r] = {
		{7, 5, 3},
		{3, 2, 2},
		{9, 0, 2},
		{2, 2, 2},
		{4, 3, 3}
	};
	int allocate[p][r] = {
		{0, 1, 0},
		{2, 0, 0},
		{3, 0, 2},
		{2, 1, 1},
		{0, 0, 2}
	};

	Banker b;
	init_banker(&b, p, r, init_res);
	create_T0_environment(&b, max, allocate);

}
