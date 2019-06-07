#ifndef _LCD_
#define _LCD_

#include "./bmplist.h"

#define WHITE  	0xffffffff
#define BLACK  	0x00000000
#define RED  	0xffff0000
#define GREEN  	0xff00ff00
#define BLUE	0xff0000ff

typedef struct{
	int fd;
	int virtualw;				//虚拟屏宽
	int virtualh;				//虚拟屏高
	int realw;					//实际屏宽
	int realh;					//实际屏高
	int bits;					//位深
	unsigned int *addr;
}LCD;

void lcd_info(LCD *lcd);
LCD *lcd_init(void);
int lcd_release(LCD *lcd);
//清屏
void lcd_clear(LCD *lcd, int color);
/****************************************
画点
x, y点的位置， color点的颜色
******************************************/
void lcd_draw_point(LCD *lcd, int x, int y, int color);
/****************************************
画线
x, y起始点的位置，xx, yy目标点的位置， color线的颜色
******************************************/
void lcd_draw_line(LCD *lcd, int x, int y, int xx, int yy, int color);
/****************************************
画三角形
（x, y），（xx, yy），（xxx, yyy）三个顶点的位置 color线的颜色
******************************************/
void lcd_draw_tria(LCD *lcd, int x, int y, int xx, int yy, int xxx, int yyy, int color);
/****************************************
画矩形
（x, y），左上角点的位置 w宽度， h高度 color线的颜色
******************************************/
void lcd_draw_rect(LCD *lcd, int x, int y, int w, int h, int color);
/****************************************
画圆形
（x, y）圆心  r半径  color线的颜色
******************************************/
void lcd_draw_circ(LCD *lcd, int x, int y, int r, int color);
/****************************************
画bmp图片
（x, y）左上角的位置 bmpname图片的名字
******************************************/
void lcd_draw_bmp(LCD *lcd, int x, int y, const char *bmpname);

void lcd_draw_bmplist(LCD *lcd, BMPLIST *bmplist);

#endif
