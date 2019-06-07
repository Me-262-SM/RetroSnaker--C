#ifndef _BMPLIST_
#define _BMPLIST_

typedef struct bmplist{
	int x;
	int y;
	char bmpname[50];
	struct bmplist *next;
}BMPLIST;

BMPLIST *create_bmplist(void);
int insert_bmplist(BMPLIST *list, int x, int y, const char *bmpname);
int delete_bmplist(BMPLIST *list);
void show_bmplist(BMPLIST *list);
#endif

