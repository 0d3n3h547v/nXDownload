#ifndef _MENUCUI_H_
#define _MENUCUI_H_

#include <stdio.h>
#include <switch.h>
#define HALF_SCREEN 40

struct V {
  short int half;
  short int half_length;
  short int final_length;
} var;

struct menu {
	char *exit;
	short int n;
	short int m;
};

void title(char *str);
void menu_options(void);

#endif