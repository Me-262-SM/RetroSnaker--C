#include "./lcd.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <signal.h>
#include <stdio.h>
#include <unistd.h>


//(fn+)ctrl+alt+F1切黑屏    alt+F7 切图形界面
BMPLIST *bmplist = NULL;
LCD *lcd = NULL;

void addbody(int sig)
{
    static int x = 0;
    insert_bmplist(bmplist, x, 0, "/root/CLionProjects/myfirst/50.bmp");
    delete_bmplist(bmplist);
    printf("x = %d\n", x);
    x += 50;
    lcd_draw_bmp(lcd, 0, 0, "/root/CLionProjects/myfirst/back.bmp");
    lcd_draw_bmplist(lcd, bmplist);
    alarm(1);
}

int main(int argc, const char *argv[])
{



    lcd = lcd_init();
/*
	lcd_info(lcd);

	lcd_clear(lcd, RED);

	lcd_draw_point(lcd, 0, 0, WHITE);
	lcd_draw_point(lcd, 200, 200, WHITE);

	lcd_draw_line(lcd, 100, 100, 100, 200, BLUE);
	lcd_draw_line(lcd, 100, 200, 200, 200, BLUE);

	lcd_draw_rect(lcd, 200, 200, 50, 50, BLACK);
*/

    //读取按键事件
    struct input_event event;
    int keyfd = open("/dev/input/event1", O_RDONLY);
    if(-1 == keyfd)
    {
        perror("open keyboard");
        return -1;
    }


    bmplist = create_bmplist();

    alarm(1);
    signal(SIGALRM, addbody);

    int x = 0;
    int y = 0;
    while(1)
    {
        lcd_clear(lcd, RED);


/*
		lcd_draw_bmp(lcd, 100, 100, "snake.bmp");

		lcd_draw_bmp(lcd, x, y, "dog.bmp");
		//char c = getchar();
*/
        read(keyfd, &event, sizeof(event));
        if(event.type == EV_KEY)//如果是按键事件
        {
            if(event.value) //按下事件
            {
                switch(event.code)
                {
                    case 17: y-= 50; break;
                    case 31: y+= 50; break;
                    case 30: x-= 50; break;
                    case 32: x+= 50; break;

                }
            }
        }
    }

    lcd_release(lcd);

    return 0;
}
