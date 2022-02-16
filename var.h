#ifndef _VAR_H
#define _VAR_H 

#include "d3dframe.h"

extern int Width, Height; //窗口的宽和高 
extern short play_round, status, play_round_2; //下棋的轮数和状态 
//extern const float size;
extern IDirect3DDevice9* Device; 
extern ID3DXFont* Font;
extern char N, numtowin, player1, player2; //棋盘大小、连线棋子个数，先后手
extern HWND hwnd;
extern short state[25][25][25];
extern int   score[25][25][25];

extern struct __{int x1, y1, z1, x2, y2, z2;} pt; //连线的起点和终点 
extern float angle, theta, dist; //相机角度 

#endif
