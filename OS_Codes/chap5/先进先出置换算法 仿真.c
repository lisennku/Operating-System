//
// Created by lisen on 24-11-7.
//

#include <stdio.h>
#include <stdlib.h>

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

int main() {
	int capacity = 3;
	int * container = calloc(capacity, sizeof(int));
	int container_idx = 0;
	int ptr = 0;

	int pages[] = {7, 0, 1, 2, 0, 3, 0, 4, 2, 3, 0, 3, 2, 1, 2, 0, 1, 7, 0, 1};
	int size = sizeof(pages) / sizeof(int);

	for(int i = 0; i < size; i++) {
		int page = pages[i];
		printf("now accessing no.%d pages with pages number %d\n", i, page);

		if(page_in_container(container, container_idx, page)) {
			printf("	Page num %d is in the container\n", page);
			display_container(container, container_idx);
		}
		else {
			if(container_idx == capacity) {
				printf("	container is full, page will be exchanged\n");
				printf("	page num %d will be removed. \n", container[ptr]);

				container[ptr] = page;
				ptr = (ptr + 1) % capacity;

				display_container(container, container_idx);
			}
			else {
				printf("	container is not full : ");
				container[container_idx ++] = page;
				display_container(container, container_idx);
			}
		}
	}

}
