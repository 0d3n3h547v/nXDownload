#ifndef _MENUCUI_H_
#define _MENUCUI_H_

#include <stdio.h>
#include <switch.h>
#include <stdbool.h>// bool: 1 == true; 0 == false;
#define HALF_SCREEN 40

/* Structures */
struct V {
  short int half;
  short int half_length;
  short int final_length;
} var;

struct menu {
	char *exit;
	short int n;
	short int m;
} initial;

/* Prototypes */
void menu_options(void);
void title(char *str);
bool menu_main(void);
bool functionExit(void);

#endif