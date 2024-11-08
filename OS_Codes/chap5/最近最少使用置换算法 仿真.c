//
// Created by lisen on 24-11-7.
//
/*
 * 使用数组存储所有的页面
 * 使用头尾指针 将进入内存的页面链接起来
 *	每次访问到已在内存的页面 或者新插入的页面 将其移动到头指针后
 *	内存满时 将尾指针前的页移除 并将新页插入到头指针后
 */

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

/*
 * 使用双向链表来模拟
 * 初始化时LRU里的pages为全部页面结点的顺序存储
 * 访问页面后更改对应结点的前后指针
 * 通过head/tail指针组成一个长度为block的内存模拟
 * 每访问一个页面
 *	如果在内存中 将该结点移动到头结点后
 *	不在内存中
 *		内存未满 插入到头结点后
 *		内存已满 移出尾结点前的页面结点 并将新页面结点插入到头结点后
 */

typedef struct PageNode {
	int page_id;
	int in_memory;
	struct PageNode * prev;
	struct PageNode * next;
}PageNode;

typedef struct {
	PageNode * pages;
	PageNode * head;
	PageNode * tail;
	int blocks;
	int length;
	int s ;
	int f ;
}LRU;

// 返回待访问的页面序列中最大的页面号
int get_max_page_num(const int * pages, const int size) {
	int max = -INT_MAX;
	for(int i = 0; i < size; i++)
		if(pages[i] > max)
			max = pages[i];

	return max;
}

// 结点相关操作
// 创建结点
PageNode nodeCreate(int page_id) {
	PageNode page;

	page.page_id = page_id;
	page.in_memory = 0;
	page.next = page.prev = NULL;

	return page;
}

// 将结点插入到头指针后 用于访问的页面不在内存中时
void nodeInsertToHead(LRU * lru, PageNode * page) {
	page->next = lru->head->next;
	page->prev = lru->head;

	lru->head->next->prev = page;
	lru->head->next = page;
}

// 将结点移动到头指针后 用于访问的页面已在内存中时
void nodeMovedToHead(LRU * lru, PageNode * page) {
	/*
	 * page结点已在lru的双向链表中 因此将page结点的prev/next进行修改后 按照插入结点操作即可
	 */
	page->next->prev = page->prev;  // page后一个结点的prev 指向page前一个结点
	page->prev->next = page->next;  // page前一个结点的next 指向page后一个结点

	nodeInsertToHead(lru, page);
}

// 删除尾指针前的页面
PageNode * nodeDeleteFromTail(LRU * lru) {
	PageNode * del = lru->tail->prev;

	del->prev->next = del->next;
	del->next->prev = del->prev;

	del->next = del->prev = NULL;

	return del;
}

void display_memory(LRU lru) {
	PageNode * head = lru.head->next;
	PageNode * tail = lru.tail;

	while(head != tail) {
		if(head->next != tail)
			printf("%d ", head->page_id);
		else
			printf("%d\n", head->page_id);

		head = head->next;
	}
}

// LRU 相关操作
// 初始化LRU
LRU * lRUCreate(int blocks, int max_pages) {
	LRU * lru = calloc(1, sizeof(LRU));
	if(!lru)
		exit(EXIT_FAILURE);

	lru->blocks = blocks;
	lru->length = lru->s = lru->f = 0;
	lru->pages = calloc(max_pages+1, sizeof(PageNode));

	lru->head = calloc(1, sizeof(PageNode));
	lru->head->in_memory = lru->head->page_id = -1;

	lru->tail = calloc(1, sizeof(PageNode));
	lru->tail->in_memory = lru->tail->page_id = -1;

	lru->head->prev = NULL;
	lru->head->next = lru->tail;

	lru->tail->next = NULL;
	lru->tail->prev = lru->head;

	for(int i = 0; i <= max_pages; i++)
		lru->pages[i] = nodeCreate(i);

	return lru;
}

// // 页面置换
// void lRUExchange(LRU * lru, int page_id) {
//
// }

// 访问页面
void lRUAccess(LRU * lru, int page_id) {
	printf("Now accessing page num %d\n", page_id);
	if(lru->pages[page_id].in_memory == 1) {
		printf("Page num %d is already in memory\n", page_id);
		nodeMovedToHead(lru, &lru->pages[page_id]);
		display_memory(*lru);
	}
	else {
		// 判断是否需要替换
		if(lru->length < lru->blocks) {  // 内存未满
			printf("Memory is not full ");
			lru->length ++;
			nodeInsertToHead(lru, &lru->pages[page_id]);
			display_memory(*lru);
		}
		else { // 内存已满
			PageNode * del =  nodeDeleteFromTail(lru);
			del->in_memory = 0;
			printf("Memory is full and page num %d will be removed\n", del->page_id);
			nodeInsertToHead(lru, &lru->pages[page_id]);
			display_memory(*lru);
		}
		lru->pages[page_id].in_memory = 1;
	}
}


int main() {
	// int pages[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
	int pages[] = {4, 7, 0, 7, 1, 0, 1, 2, 1, 2, 6};
	int size = sizeof(pages) / sizeof(pages[0]);

	LRU * lru = lRUCreate(5, get_max_page_num(pages, size));
	for(int i = 0; i < size; i++)
		lRUAccess(lru, pages[i]);
}




