#ifndef _PUT_CHESS_H
#define _PUT_CHESS_H

#include "d3dframe.h"
void play_init();
bool incomp();
void entercomp();
bool put(int x, int y, int z);
bool pass();
void highlight(int i0, int j0, int k0);
bool cancel();
int calcscore(bool putchess);
int inform();
void confirm();

#endif 
