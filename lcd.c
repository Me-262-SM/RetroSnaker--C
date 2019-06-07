#include "lcd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "bmplist.h"

LCD *lcd_init()
{
	LCD *lcd = malloc(sizeof(LCD));
	
	int ret ;
	struct fb_var_screeninfo fbi_var;//定义可变参数
	struct fb_fix_screeninfo fbi_fix;//定义固定参数
	struct fb_cmap cmap;
	//打开屏幕设备
	lcd->fd = open("/dev/fb0",O_RDWR);
	if(lcd->fd < 0){
		perror("open");
		close(lcd->fd);
	}
	//得到虚拟屏幕参数
	ret = ioctl(lcd->fd,FBIOGET_VSCREENINFO,&fbi_var);
	if(ret < 0){
		perror("ioctl");
		return NULL;
	}
//	printf("fbi.var.xres:%u ,fbi.var.yres:%u ,var.bpp:%d\n",fbi_var.xres ,fbi_var.yres,fbi_var.bits_per_pixel);
//	printf("fbi.var.xoffset:%u ,fbi.var.yoffset:%u\n",fbi_var.xoffset ,fbi_var.yoffset);
	lcd->bits = fbi_var.bits_per_pixel;
	lcd->virtualw = fbi_var.xres;
	lcd->virtualh = fbi_var.yres;
	
	//得到实际屏幕参数
	ret = ioctl(lcd->fd,FBIOGET_FSCREENINFO,&fbi_fix);
	if(ret < 0){
		perror("ioctl");
		return NULL;
	}
//	printf("fbi.fix.smem_start:%lu ,fbi.fix.smem_len:%u\n",fbi_fix.smem_start ,fbi_fix.smem_len);
//	printf("line_length: %u\n",fbi_fix.line_length);
	lcd->realw = fbi_fix.line_length/(fbi_var.bits_per_pixel/8);
	lcd->realh = fbi_fix.smem_len/fbi_fix.line_length;
	
	//获取显存起始地址
	lcd->addr = mmap(NULL,fbi_fix.smem_len,PROT_READ|PROT_WRITE,MAP_SHARED,lcd->fd,0);
	if(lcd->addr == NULL){
		perror("mmap");
		ret = munmap(lcd->addr,fbi_fix.smem_len);
		return NULL;
	}
	return lcd;
}

int lcd_release(LCD *lcd)
{
	munmap(lcd->addr,lcd->realw*lcd->realh*lcd->bits/8);
	close(lcd->fd);
}

void lcd_info(LCD *lcd)
{
	printf("realw: %d\n", lcd->realw);
	printf("realh: %d\n", lcd->realh);
	
	printf("virtualw: %d\n", lcd->virtualw);
	printf("virtualh: %d\n", lcd->virtualh);

	printf("bits: %d\n", lcd->bits);
}

//清屏
void lcd_clear(LCD *lcd, int color)
{
	unsigned int *addr = lcd->addr;
	
	int row, col;
	for(row=0; row<lcd->virtualh; row++)
	{
		for(col=0; col<lcd->virtualw; col++)
		{
			*addr++ = color;
		}
		addr += (lcd->realw - lcd->virtualw);
	}
}
/****************************************
画点
x, y点的位置， color点的颜色
******************************************/
void lcd_draw_point(LCD *lcd, int x, int y, int color)
{
	unsigned int *addr = lcd->addr;
	
	addr +=  y*lcd->realw+x;
	
	*addr = color;
}
/****************************************
画线
x, y起始点的位置，xx, yy目标点的位置， color线的颜色
******************************************/
void lcd_draw_line(LCD *lcd, int x, int y, int xx, int yy, int color)
{
	if(x == xx) //竖线
	{
		int i = y<yy?y:yy;
		for(;i<(y>yy?y:yy); i++)
		{
			lcd_draw_point(lcd, x, i, color);
		}
	}
	else
	if(y == yy) //横线
	{
		int i = x<xx?x:xx;
		for(;i<(x>xx?x:xx); i++)
		{
			lcd_draw_point(lcd, i, y, color);
		}
	}
	else
	{
		
	}
}
/****************************************
画三角形
（x, y），（xx, yy），（xxx, yyy）三个顶点的位置 color线的颜色
******************************************/
void lcd_draw_tria(LCD *lcd, int x, int y, int xx, int yy, int xxx, int yyy, int color)
{
	

}
/****************************************
画矩形
（x, y），左上角点的位置 w宽度， h高度 color线的颜色
******************************************/
void lcd_draw_rect(LCD *lcd, int x, int y, int w, int h, int color)
{
	lcd_draw_line(lcd, x, y, x+w, y, color);
	lcd_draw_line(lcd, x, y, x, y+h, color);
	lcd_draw_line(lcd, x+w, y+h, x+w, y, color);
	lcd_draw_line(lcd, x, y+h, x+w, y+h, color);
}
/****************************************
画圆形
（x, y）圆心  r半径  color线的颜色
******************************************/
void lcd_draw_circ(LCD *lcd, int x, int y, int r, int color)
{}
#pragma pack(1)

struct bmp_file//BMP文件头结构
{
	char type[2];//位图文件的类型，必须为BM，我这里类型不对，所以显示有误。
	unsigned int size;//位图文件的大小，以字节为单位
	short rd1;// 位图文件保留字，必须为0
	short rd2;// 位图文件保留字，必须为0
	unsigned int offset;// 位图数据的起始位置，以相对于位图
};

struct bmp_info//图像信息区
{
	unsigned int bsize;//本结构体所占用字节数,即40个字节
	int width;// 位图的宽度，以像素为单位，像素数量是4字节对齐的
	int height;// 位图的高度，以像素为单位
	unsigned short planes;// 目标设备的级别，必须为1
	unsigned short count;// 每个像素所需的位数，必须是1(双色）// 4(16色)，8(256色)或24(真彩色)之一
	unsigned int compression;// 位图压缩类型，必须是 0(不压缩),// 1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一
	unsigned int sizeimage;// 位图的大小，以字节为单位
	unsigned int xmeter;// 位图水平分辨率，每米像素数
	unsigned int ymeter;// 位图垂直分辨率，每米像素数
	unsigned int cused;// 位图实际使用的颜色表中的颜色数
	unsigned int cimportant;// 位图显示过程中重要的颜色数

};

typedef struct bmp_head {
	struct bmp_file file;
	struct bmp_info info;

}BMPHEADER;

int getbmp_wh(const char *bmpfile, int *w, int *h)
{
	int fd = open(bmpfile, O_RDONLY);
	if(-1 == fd)
	{
		perror("open bmp");
		return -1;
	}

	BMPHEADER header = {0};
	read(fd, &header, sizeof(BMPHEADER));	
	

	*w = header.info.width;
	*h = header.info.height;

	close(fd);
}
/****************************************
画bmp图片
（x, y）左上角的位置 bmpname图片的名字
******************************************/
void lcd_draw_bmp(LCD *lcd, int x, int y, const char *bmpname)
{
	int w;
	int h;
	getbmp_wh(bmpname, &w, &h);
	
	
	int fd = open(bmpname, O_RDONLY);
	if(-1 == fd)
	{
		perror("open bmp");
		return ;
	}
	
	unsigned char buf[h*w*3];
	lseek(fd, 54, SEEK_SET);
	read(fd, buf, sizeof(buf));
	unsigned char *t = buf;
	
//	unsigned int bmp[265][344] = {0};
	
//	unsigned int *addr = lcd->addr+y*lcd->realw+x;
	unsigned int *addr = lcd->addr + (y+h)*lcd->realw + x;
	
	int row, col;
	for(row=0; row<h; row++)
	{
		for(col=0; col<w; col++)
		{
			*addr++ = *t | *(t+1)<<8 | *(t+2)<<16;//;bmp[row][col];
			t += 3;
		}
		addr -= (lcd->realw+w);
		
	}
}

void lcd_draw_bmplist(LCD *lcd, BMPLIST *bmplist)
{
	while(bmplist->next)
	{
		lcd_draw_bmp(lcd, bmplist->next->x, bmplist->next->y, bmplist->next->bmpname);
		bmplist = bmplist->next;
	}
}
