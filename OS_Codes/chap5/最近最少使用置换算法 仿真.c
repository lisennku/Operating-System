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
void nodeDeleteFromTail(LRU * lru);

// LRU 相关操作
// 初始化LRU
LRU * lRUCreate(int blocks, int max_pages) {
	LRU * lru = calloc(1, sizeof(LRU));
	if(!lru)
		exit(EXIT_FAILURE);

	lru->blocks = blocks;
	lru->length = lru->s = lru->f = 0;
	lru->pages = calloc(max_pages, sizeof(PageNode));

	lru->head = lru->tail = NULL;

	for(int i = 0; i < max_pages; i++)
		lru->pages[i] = nodeCreate(i);
}

// 页面置换
void lRUExchange(LRU * lru, int page_id);

// 访问页面
void lRUAccess(LRU * lru, int page_id);





