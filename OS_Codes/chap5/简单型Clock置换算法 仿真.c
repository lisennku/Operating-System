//
// Created by lisen on 24-11-8.
//

/*
 * 将内存中的物理块以循环链表形式体现 每次访问/置换等操作不需要移动链表(数据也可以 较为简单)
 * 每次访问页面时 将访问位设置为1
 * 发生置换时
 *	从头检查访问位是否为0
 *		如果是1 先置为0
 *		如果是0 则更改页号
 */
#include <complex.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct PageNode {
	int page_id;
	int used;
	struct PageNode * next;
} PageNode;

typedef struct Clock {
	int blocks;
	int length;
	PageNode * ptr;
	PageNode * head;
}Clock;

void display_memory(Clock clock) {
	PageNode * head = clock.head->next;

	while(head != clock.head) {
		if(head->page_id != -1)
			printf("%d ", head->page_id);
		head = head->next;
	}
	putchar('\n');
}

Clock * clockCreate(int blocks) {
	Clock * clock = calloc(1, sizeof(Clock));
	if(!clock)
		exit(EXIT_FAILURE);

	clock->blocks = blocks;
	clock->length = 0;

	clock->head = calloc(1, sizeof(PageNode));
	clock->head->next = clock->head;
	clock->head->page_id = clock->head->used = -1;

	PageNode * h = clock->head;

	// 初始化循环链表其他结点
	for(int i = 0; i < blocks; i++) {
		PageNode * p = calloc(1, sizeof(PageNode));
		if(!p)
			exit(EXIT_FAILURE);

		p->page_id = -1;
		p->used = 0;
		p->next = clock->head;

		h->next = p;
		h = h->next;
	}

	clock->ptr = clock->head->next;

	return clock;
}

void clockAccess(Clock * clock, int page_id) {
	PageNode * tmp ;
	printf("Now accessing page num %d\n", page_id);
	// 检查页面是否在内存中
	PageNode * cur = clock->head->next;
	do {
		if(cur->page_id == page_id) {
			printf("Page num %d is already in memory\n", page_id);
			cur->used = 1;
			display_memory(*clock);
			return ;
		}
		cur = cur->next;
	}while(cur != clock->head);

	if(clock->length < clock->blocks) {
		printf("Memory is not full ");
		clock->length ++;
		tmp = clock->head->next;
		while(tmp->page_id != -1)
			tmp = tmp->next;

		tmp->page_id = page_id;
		tmp->used = 1;
		display_memory(*clock);
	}
	else {
		while(1) {
			if(clock->ptr == clock->head) {
				clock->ptr = clock->head->next;
				continue;
			}
			if(clock->ptr->used == 0) {
				printf("Memory is full and page num %d will be removed\n", clock->ptr->page_id);
				clock->ptr->page_id = page_id;
				clock->ptr->used = 1;
				display_memory(*clock);
				return ;
			}
			clock->ptr->used = 0;
			clock->ptr = clock->ptr->next;
		}
	}
}

int main() {
	int pages[] = {4, 7, 0, 7, 1, 0, 1, 2, 1, 2, 6, 5, 3, 4, 1};
	int size = sizeof(pages) / sizeof(pages[0]);
	int blocks = 5;

	Clock * c = clockCreate(blocks);
	for(int i = 0; i < size; i++)
		clockAccess(c, pages[i]);

	return 0;
}