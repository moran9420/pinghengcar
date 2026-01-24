
#ifndef _MENU_H_
#define _MENU_H_


/* 菜单对外变量 */
extern int menu_mode;     // 1~5
extern int menu_page;     // 当前页面
extern int menu_cursor;   // 光标

void menu_init(void);
void menu_task(void);



#endif
