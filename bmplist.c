#include "bmplist.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void show_bmplist(BMPLIST *list)
{
	while(list->next)
	{
		printf("(%d, %d)=>", list->next->x, list->next->y);
		list = list->next;
	}
	printf("\n");
}

BMPLIST *create_bmplist(void)
{
	BMPLIST *head = malloc(sizeof(BMPLIST));
	head->next = NULL;

	return head;
}

int insert_bmplist(BMPLIST *list, int x, int y, const char *bmpname)
{
	BMPLIST *node = malloc(sizeof(BMPLIST));
	node->x = x;
	node->y = y;
	strcpy(node->bmpname, bmpname);
	node->next = NULL;

	node->next = list->next;
	list->next = node;
	return 0;
}

int delete_bmplist(BMPLIST *list)
{
	show_bmplist(list);
	
	//only one left
	if(list->next && (NULL == list->next->next))
		return 0;

	//find tail
	while(list->next)
	{
		printf("node %p: %d\n", list->next, list->next->x);
		if(list->next->next == NULL)
			break;
		list = list->next;
	}

	BMPLIST *t = list->next;
	list->next = NULL;
	free(t);

	return 0;
}

