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
#define P 5
#define R 3

typedef struct {
	int res_nums;
	int pro_nums;
	int * available;
	int ** max;
	int ** allocate;
	int ** need;
}Banker;

void init_banker(Banker * b, int p, int r) {

	b->pro_nums = p;
	b->res_nums = r;

	b->available = calloc(r, sizeof(int));

	b->max = calloc(p, sizeof(int *));
	for(int i = 0; i < p; i++)
		b->max[i] = calloc(r, sizeof(int));

	b->allocate = calloc(p, sizeof(int *));
	for(int i = 0; i < p; i++)
		b->allocate[i] = calloc(r, sizeof(int));

	b->need = calloc(p, sizeof(int *));
	for(int i = 0; i < p; i++)
		b->need[i] = calloc(r, sizeof(int));
}

void create_T0_env(Banker * b) {
	int t_available[R] = {3, 3, 2};

	int t_max[P][R] = {
		{7, 5, 3},
		{3, 2, 2},
		{9, 0, 2},
		{2, 2, 2},
		{4, 3, 3}
	};
	int t_allocate[P][R] = {
		{0, 1, 0},
		{2, 0, 0},
		{3, 0, 2},
		{2, 1, 1},
		{0, 0, 2}
	};

	// 初始化 available 数组
	for (int i = 0; i < b->res_nums; i++) {
		b->available[i] = t_available[i];
	}


	for(int i = 0; i < b->pro_nums; i++) {
		for(int j = 0; j < b->res_nums; j++) {
			b->max[i][j] = t_max[i][j];
			b->allocate[i][j] = t_allocate[i][j];
			b->need[i][j] = t_max[i][j] - t_allocate[i][j];
		}
	}
}

int banker_safety_check(Banker b, int * safety_list) {
	int * work = calloc(b.res_nums, sizeof(int));
	memcpy(work, b.available, sizeof(int) * b.res_nums);
	int * finish = calloc(b.pro_nums, sizeof(int));

	int all_finish = 0;
	int safety_list_pos = 0;
	while(!all_finish) {
		all_finish = 1;
		for(int i = 0; i < b.pro_nums; i++) {
			if(finish[i] == 0) {
				int allocate_flag = 1;
				for(int j = 0; j < b.res_nums; j++)
					if(b.need[i][j] > work[j]) {
						allocate_flag = 0;
						break;
					}

				if(allocate_flag) {
					for (int j = 0; j < b.res_nums; j++) {
						work[j] += b.allocate[i][j];
					}
					safety_list[safety_list_pos++] = i;
					finish[i] = 1;
					all_finish = 0;
				}
			}
		}
	}

	for(int i = 0; i < b.pro_nums; i++)
		if(finish[i] == 0)
			return 0;
	return 1;

}

int * banker_request(Banker * b, Banker tmp, int pid, int * request) {
	// 检查request和need
	for(int j = 0; j < b->res_nums; j ++)
		if(b->need[pid][j] < request[j])
			return NULL;
	// 检查request和available
	for(int j = 0; j < b->res_nums; j ++)
		if(b->available[j] < request[j])
			return NULL;
	// 尝试性分配
	for(int j = 0; j < tmp.res_nums; j++) {
		tmp.available[j] -= request[j];
		tmp.allocate[pid][j] += request[j];
		tmp.need[pid][j] -= request[j];
	}
	// 安全检查
	int * res = calloc(b->pro_nums, sizeof(int));
	int flag = banker_safety_check(tmp, res);
	if(flag) {
		*b = tmp;
		return res;
	}
	return NULL;

}

void display_matrix(Banker b, char mat_typ) {
	switch (mat_typ) {
		case 'a': {
			printf("%5s", "AVL");
			for(int i = 0; i < b.res_nums; i++)
				printf("%5c", i + 'A');
			printf("\n%5c", BLANK);
			for(int i = 0; i < b.res_nums; i++)
				printf("%5d", b.available[i]);
			putchar('\n');
			break;
		}
		case 'm': {
			printf("%5s", "MAX");
			for(int i = 0; i < b.res_nums; i++)
				printf("%5c", i + 'A');
			putchar('\n');
			for(int i = 0; i < b.pro_nums; i++, printf("\n")) {
				printf("%5d", i);
				for(int j = 0; j < b.res_nums; j++) {
					printf("%5d", b.max[i][j]);
				}
			}
			break;
		}
		case 'l': {
			printf("%5s", "ALT");
			for(int i = 0; i < b.res_nums; i++)
				printf("%5c", i + 'A');
			putchar('\n');
			for(int i = 0; i < b.pro_nums; i++, printf("\n")) {
				printf("%5d", i);
				for(int j = 0; j < b.res_nums; j++) {
					printf("%5d", b.allocate[i][j]);
				}
			}
			break;
		}
		case 'n': {
			printf("%5s", "NED");
			for(int i = 0; i < b.res_nums; i++)
				printf("%5c", i + 'A');
			putchar('\n');
			for(int i = 0; i < b.pro_nums; i++, printf("\n")) {
				printf("%5d", i);
				for(int j = 0; j < b.res_nums; j++) {
					printf("%5d", b.need[i][j]);
				}
			}
			break;
		}
		default:
			printf("error mat_typ para. value; a for available; m for max; l for allocate; n for need\n");
	}
}

int main() {
	Banker b;
	init_banker(&b, P, R);
	create_T0_env(&b);
	display_matrix(b, 'a');
	display_matrix(b, 'm');
	display_matrix(b, 'l');
	display_matrix(b, 'n');

	int safety_list[P];
	printf("T0 status is %s\n", banker_safety_check(b, safety_list) == 1 ? "true" : "false");
	for(int i = 0; i < b.pro_nums; i++)
		printf("%d ", safety_list[i]);
	putchar('\n');

	printf("now pid 1 request\n");
	int req1[] = {1, 0, 2};
	int * res1 = banker_request(&b, b, 1, req1);
	if(res1 != NULL) {
		printf("pid 1 requested successfully\n");
		for(int i = 0; i < b.pro_nums; i++)
			printf("%d ", res1[i]);
		putchar('\n');
		display_matrix(b, 'a');
		display_matrix(b, 'm');
		display_matrix(b, 'l');
		display_matrix(b, 'n');
	}
	else
		printf("pid 1 requested failed\n");
	printf("now pid 4 request\n");
	int req4[] = {1, 0, 2};
	int * res4 = banker_request(&b, b, 4, req1);
	if(res4 != NULL) {
		printf("pid 4 requested successfully\n");
		for(int i = 0; i < b.pro_nums; i++)
			printf("%d ", res4[i]);
		putchar('\n');
		display_matrix(b, 'a');
		display_matrix(b, 'm');
		display_matrix(b, 'l');
		display_matrix(b, 'n');
	}
	else
		printf("pid 4 requested failed\n");
}
