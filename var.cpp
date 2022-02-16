#include <d3dx9.h>
 
int Width = 1080, Height = 720;//窗口的宽和高 
short play_round, status = 0, play_round_2 = 0;//下棋的轮数和状态 

char N = 4, numtowin = 5, player1 = 0, player2 = 1;//棋盘大小、连线棋子个数，先后手
IDirect3DDevice9* Device = 0; 
ID3DXFont* Font = 0;
HWND hwnd;
short state[25][25][25];
int   score[25][25][25];

struct __{int x1, y1, z1, x2, y2, z2;} pt;//连线的起点和终点 
float angle = (1.5f * D3DX_PI), theta = 0, dist = 7.0f;//相机角度 
