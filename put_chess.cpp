#include "d3dframe.h"
#include "var.h"

//积分 
const int self[] =  {1, 4, 20,  2000, 90000, 1700000, 70000000};
const int other[] = {0, 2,  8,   200, 16000,  400000,  7000000};

void play_init()
{
	play_round = play_round_2 = status = 0;
	srand(timeGetTime());
	ZeroMemory(&pt,   sizeof(pt));
	ZeroMemory(state, sizeof(state));
	ZeroMemory(score, sizeof(score));
}
//随机因子 
float ran()
{
	return 1.0f + rand() / 400000.0f;
}

bool incoord(int i, int j, int k)
{
	return (0<=i && i<=2*N   
	   &&   0<=j && j<=2*N   
	   &&   0<=k && k<=2*N);
}

bool incomp()
{
	return play_round == play_round_2;
}

void entercomp()
{
	int i, j, k;
	forijk
		if(state[i][j][k] > play_round)
			state[i][j][k] = 0;
	play_round_2 = play_round;
	status = 0;
}



bool put(int x, int y, int z)
{
	if(!incomp() || state[x][y][z] > 0)
		return false;
	state[x][y][z] = ++play_round;
	++play_round_2;
	return true;
}

bool pass()
{
	if(!incomp()) 
		return false;
	++play_round;
	++play_round_2;
	return true;
}

void highlight(int i0, int j0, int k0)
{
	int i, j, k;
	forijk 
		if(state[i][j][k] < 0) 
			state[i][j][k] = 0;

	if(state[i0][j0][k0] == 0)
		state[i0][j0][k0] = -1;
}

bool cancel()
{
	int i, j, k;
	if(!incomp()) 
		return false;
	if(play_round <= 0)
	{
		MessageBox(0, "不得撤消！", 0, 0); 
		return false;
	}
	--play_round;
	--play_round_2;
	forijk 
		if(state[i][j][k] > play_round) 
			state[i][j][k] = 0;
	
	status = 0;
	return true;
}
//计算一个五子的得分 
void calcsingleline(int dx, int dy, int dz, 
	const int x0, const int y0, const int z0)
{
	int x1 = x0, y1 = y0, z1 = z0, p = play_round % 2;
	if((!incoord(x0, y0, z0)) 
	|| (!incoord(
		x0 + (numtowin-1) * dx, 
		y0 + (numtowin-1) * dy, 
		z0 + (numtowin-1) * dz)) 
	|| (dx==0 && dy==0 && dz==0))
		return;//那么不可能，返回 
	int n1 = 0, n2 = 0, n0 = 0;
	//n1：对方棋子个数，n2：我方棋子个数，n0：空位个数 
	for(int i = 1; i <= numtowin; i++)
	{
		if(state[x1][y1][z1] > 0)
		{
			if(state[x1][y1][z1] % 2 == p) 
			     n1++; 
			else n2++;
		}
		else     n0++;
		x1 += dx;
		y1 += dy;
		z1 += dz;
	}
	
	x1 = x0;
	y1 = y0;
	z1 = z0;
	//凡是空位都加分 
	if(n1 == 0)
		for(int i = 1; i <= numtowin; i++)
		{
			if(state[x1][y1][z1] <= 0)
				score[x1][y1][z1] += self[n2];
			x1 += dx;
			y1 += dy;
			z1 += dz;
		}
	else if(n2 == 0)
		for(int i = 1; i <= numtowin; i++)
		{
			if(state[x1][y1][z1] <= 0)
				score[x1][y1][z1] += other[n1];
			x1 += dx;
			y1 += dy;
			z1 += dz;
		}
	//n1，n2都不为0，则不积分 
}

int calcscore(bool putchess)
//计算所有积分之和并落子 
{
	ZeroMemory(score, sizeof(score));
	int i, j, k, di, dj, dk, max = -1;
	
	for(di = -1; di <= 1; di++)
	for(dj = -1; dj <= 1; dj++)
	for(dk = -1; dk <= 1; dk++)
		if (abs(di) + abs(dj) + abs(dk) == 1  
		||  abs(di) + abs(dj) + abs(dk) == 2) 
			forijk  
				calcsingleline(di, dj, dk, i, j, k);
	
	forijk
	{
		if(state[i][j][k] <= 0)
		{	
			score[i][j][k] += 2;
			score[i][j][k] *= ran();
		}
	 	if(score[i][j][k] > max)
		{
			max = score[i][j][k];
			di = i;
			dj = j;
			dk = k;
		}
	}
	
	if(putchess)
		return put(di, dj, dk);
	else 
		highlight(di, dj, dk);
	
	return 1;
}

bool same(int i, int j, int k, int di, int dj, int dk)
{
	int c;
	for(c = 1; c <= numtowin - 1; c++)
		if((state[i][j][k] % 2) != (state[i+c*di][j+c*dj][k+c*dk] % 2)
		 || state[i][j][k] <= 0 ||  state[i+c*di][j+c*dj][k+c*dk] <= 0)
		 	return false;
	return true;
}

int judge()//判断胜负 
{
	int i, j, k, di, dj, dk, c = 3;
	forijk 
		if(state[i][j][k] <= 0)
			c = 0;
			//还不确定 
	for(di = -1; di <= 1; di++)
	for(dj = -1; dj <= 1; dj++)
	for(dk = -1; dk <= 1; dk++)
		if(abs(di) + abs(dj) + abs(dk) == 1 
		|| abs(di) + abs(dj) + abs(dk) == 2) 
			forijk 
				if(incoord(i, j, k)
				&& incoord(
					i + (numtowin - 1) * di, 
					j + (numtowin - 1) * dj, 
					k + (numtowin - 1) * dk)
				//这几个点都在坐标内 
				&& same(i, j, k, di, dj, dk))
				//且相等 
				{
					pt.x1 = i;
					pt.y1 = j;
					pt.z1 = k;
					pt.x2 = i + (numtowin - 1) * di;
					pt.y2 = j + (numtowin - 1) * dj;
					pt.z2 = k + (numtowin - 1) * dk;
					return ( ( state[i][j][k] % 2 == 1 )? 1: 2);
					//1:红方胜 2:蓝方胜
				}
	return c;		//3:平局 
}

int inform()//告知用户 
{
	if(status == 0)
	switch(status = judge())
	{
		case 1:
			if(MessageBox (hwnd, "红方胜！悔棋吗？", 0, 
			MB_ICONINFORMATION | MB_YESNO) == IDYES){
				cancel(); cancel();
			}
			break;
		case 2:
			if(MessageBox (hwnd, "蓝方胜！悔棋吗？", 0, 
			MB_ICONINFORMATION | MB_YESNO) == IDYES){
				cancel(); cancel();
			}
			break;
		case 3:
			if(MessageBox (hwnd, "平局了！再来吗？", 0, 
			MB_ICONINFORMATION | MB_YESNO) == IDYES) play_init();
			break;
	}
	return status;
}

void confirm()
{
	int i, j, k;
	if(!incomp()) 
		return;
	if(!inform()) 
		forijk 
			if(state[i][j][k] == -1)
			{
				put(i, j, k);
				return;
			}
}
