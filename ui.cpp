#include "d3dframe.h"
#include "defs.h" 
#include "var.h"
#include "put_chess.h"

//界面模块 

HWND InitMainWindow(HINSTANCE hInstance)
{ 
	WNDCLASS wc;
	wc.style         = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc   = d3d::WndProc; 
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hInstance     = hInstance;
	wc.hIcon         = LoadIcon(hInstance, ICONNAME);
	wc.hCursor       = LoadCursor(0, IDC_ARROW);
	wc.lpszMenuName  = MENUNAME;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszClassName = APPNAME;
	if(!RegisterClass(&wc))
		REPORTERROR("RegisterClass()");
		
	hwnd = CreateWindow(APPNAME, APPNAME, WS_OVERLAPPEDWINDOW, 
	    CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,
	    0, 0, hInstance, 0);
	
	if(!hwnd)
		REPORTERROR("CreateWindow()");
	
	ShowWindow(hwnd, SW_MAXIMIZE);
	UpdateWindow(hwnd);
	
	RECT rect;
	GetClientRect(hwnd, &rect);
	Height = rect.bottom;
	Width  = rect.right;
	
	return hwnd;
}

int d3d::EnterMsgLoop()
{
	MSG msg;
	int currTime;
	ZeroMemory(&msg, sizeof(MSG));
	static int lastTime = timeGetTime(); 
	bool moved = false;
	while(msg.message != WM_QUIT)
	{
		if(::GetMessage(&msg, 0, 0, 0))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
			if      (msg.message == WM_MOUSEMOVE) continue;
			else if (msg.message == WM_KEYDOWN  )
			{
				lastTime = timeGetTime();
				while(msg.message != WM_QUIT)
				{
					currTime = timeGetTime();
					moved = Display(
						(currTime - lastTime) < 80 ?
						(currTime - lastTime) * 0.001f :
						0);
					lastTime = currTime;
					if( !moved ) break;
					if(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
					{
						::TranslateMessage(&msg);
						::DispatchMessage(&msg);
					}
			    }
			}
			//else ptr_display(0);
		}
    }
    return msg.wParam;
}

HMENU hmenu;
int delaytime = 1000;

#define READ(P)  fread (&P, sizeof(P), 1, fp)
#define WRITE(P) fwrite(&P, sizeof(P), 1, fp)
#define CHECK(P) CheckMenuItem(hmenu, P, MF_CHECKED)

void CorrectMenu()
{
	int i; 
	for(i = MENUBEGIN; i <= MENUEND; i++)
		CheckMenuItem(hmenu, i, MF_UNCHECKED);

	CHECK(ID_HUMAN_1 + player1);
	CHECK(ID_HUMAN_2 + player2);
	CHECK(MENUSIZE + 2 * N + 1);
	CHECK(MENUWINNUM + numtowin);
	CHECK(MENUTIME + delaytime / 100);
}

void SetOpenFile(OPENFILENAME* p, HWND hwnd, char* szFile)
{
	*szFile = 0;
	ZeroMemory(p, sizeof(OPENFILENAME));
	p->lStructSize  = sizeof(OPENFILENAME);
	p->hwndOwner    = hwnd;
	p->lpstrFile    = szFile;
	p->nMaxFile     = MAX_PATH;
	p->nFilterIndex = 1;
}

const char* InitName(const char* p1)//去除双引号 
{
	char* q, *r;
	if(strstr(p1, ".gomoku"))
	{
		if( (q = strchr(p1, '"')) != 0)// != 0
		{
			q = strdup(q + 1);
			r = strchr(q, '"');
			if(r != 0) *r = 0;
			else return 0;
		}
		else 
			q = strdup(p1);
		return q; 
	}
	else return 0;
}

bool OpenByName(const char* const file)//写文件 
{
	int i, j, k; 
	FILE* fp = fopen(file, "rb");
	if(!fp)
	{
		REPORTERROR("文件名");
		return false;
	}
	ZeroMemory(&pt, sizeof(pt));
	READ(N);
	READ(numtowin);
	READ(player1);
	READ(player2);
	READ(play_round);
	play_round_2 = play_round;
	READ(angle);
	READ(theta);
	READ(dist);
	forijk 
		READ(state[i][j][k]);
	
	fclose(fp);
	return true;
}
LRESULT DlgProc(
	HWND hdlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static char tmp[16];
	switch (msg)
	{
		case WM_INITDIALOG:
			sprintf(tmp, "(0 ~ %d)", play_round_2);
			SetDlgItemText(hdlg, ID_REGION, tmp);
			return TRUE;
		case WM_COMMAND:
			if(LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
			{
				GetDlgItemText(hdlg, ID_INPUT, tmp, sizeof(tmp));
				play_round = atoi(tmp);
				if(play_round < 0)
					play_round = 0;
				if(play_round > play_round_2)
					play_round = play_round_2;
				EndDialog(hdlg, 0);
				return TRUE;
			}
			return FALSE;		
	}
	return FALSE;
}
bool begin = false;

void TimerProc1(HWND hwnd, UINT, WPARAM, DWORD)
{
	if(inform() > 0) //判断胜负并告知 
	{
		InvalidateRect(hwnd, 0, 0);
		KillTimer(hwnd, 1);
		return;
	}
	if ((player2 && play_round % 2 == 1)
		|| (player1 && play_round % 2 == 0))
	{
		if(calcscore(true)) //算分并下棋 
		{
			InvalidateRect(hwnd, 0, 0);
			return;
		}
	}
	KillTimer(hwnd, 1);
}

void TimerProc2(HWND hwnd, UINT, WPARAM, DWORD)
{
	if(incomp())
		KillTimer(hwnd, 2);
	if(play_round < play_round_2)
		++play_round;
	InvalidateRect(hwnd, 0, 0);
}

void TimerProc3(HWND hwnd, UINT, WPARAM, DWORD)
{
	InvalidateRect(hwnd, 0, 0);
	KillTimer(hwnd, 3);
}

LRESULT d3d::WndProc(
	HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)//窗口过程 
{
	int                 i, j, k;
	static int          lwparam; 
	static char         szFile[MAX_PATH + 2] = "";
    static OPENFILENAME ofn;
	static HINSTANCE    hInstance;
    RECT                rect;
    PAINTSTRUCT         ps;
    
	switch(msg)
	{
	case WM_CREATE:
		hmenu = GetMenu(hwnd);
		play_init();
		hInstance = ((LPCREATESTRUCT) lParam)->hInstance;
		CorrectMenu();
		//InvalidateRect(hwnd, 0, 0);
		SetTimer(hwnd, 3, 50, TimerProc3);
		break;
		
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
		
	case WM_KEYDOWN:
		switch (wParam)
		{
			case VK_ESCAPE:
				DestroyWindow(hwnd);
				return 0;
			case VK_SPACE:
				confirm();
				if(begin) 
					SetTimer(hwnd, 1, delaytime, TimerProc1); 
				break;
			case VK_DELETE:
				if(!cancel()) break;
				if(play_round >= 1) 
					cancel();
				if(begin) 
					SetTimer(hwnd, 1, delaytime, TimerProc1); 
				break;
			case 'p': case 'P':
				if(!pass()) break;
				if(begin) 
					SetTimer(hwnd, 1, delaytime, TimerProc1); 
				break;
			case VK_CONTROL:
				if(begin)
				{
					begin = false;
					KillTimer(hwnd, 1);
				}
				else
				{
					begin = true;
					SetTimer(hwnd, 1, delaytime, TimerProc1); 
				}
				break;
			case VK_PRIOR:
				if(play_round > 0)
					play_round--;
				KillTimer(hwnd, 1);
				break;
			case VK_NEXT:
				if(play_round < play_round_2) 
					play_round++;
				break;
			case VK_RETURN:
				KillTimer(hwnd, 1);
				KillTimer(hwnd, 2);
				entercomp();
				break;
		}
		InvalidateRect(hwnd, 0, 0);
		break;
	
	case WM_COMMAND:
		switch (lwparam = LOWORD(wParam))
		{//菜单处理 
			case ID_OPEN:
				SetOpenFile(&ofn, hwnd, szFile);
				ofn.lpstrFilter = "棋谱文件 (*.gomoku)\0*.gomoku\0";
				ofn.lpstrDefExt = "gomoku";
				ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
				if(GetOpenFileName(&ofn))
				{
					OpenByName(szFile);
					Cleanup();
					Setup();
					CorrectMenu();
				}
				else REPORTERROR("文件");
				break;
			case ID_SAVE:
				SetOpenFile(&ofn, hwnd, szFile);
				ofn.lpstrFilter = "棋谱文件 (*.gomoku)\0*.gomoku\0";
				ofn.lpstrDefExt = "gomoku";
				if(GetSaveFileName(&ofn))
				{
					FILE* fp;
					if(! (fp = fopen(szFile, "wb")) ) REPORTERROR("文件");
					WRITE(N);
					WRITE(numtowin);
					WRITE(player1);
					WRITE(player2);
					WRITE(play_round);
					WRITE(angle);
					WRITE(theta);
					WRITE(dist);
					forijk 
						WRITE(state[i][j][k]);
					fclose(fp);
				}
				else REPORTERROR("文件");
				break;
			case ID_SAVEIMAGE:
				SetOpenFile(&ofn, hwnd, szFile);
				ofn.lpstrFilter = "图片文件(*.jpg)\0*.jpg\0";
				ofn.lpstrDefExt = "jpg";
				if(GetSaveFileName(&ofn))
				{
					IDirect3DSurface9 *pBackBuffer;
					Device->GetBackBuffer(
						0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);
					D3DXSaveSurfaceToFile(
						szFile, D3DXIFF_JPG, pBackBuffer, NULL, NULL);
					pBackBuffer->Release();
				}
				else REPORTERROR("文件");
				break;
			case ID_BEGIN: 
				begin = true;
				SetTimer(hwnd, 1, delaytime, TimerProc1); 
				break;
			case ID_STOP: 
				KillTimer(hwnd, 1);
				KillTimer(hwnd, 2);
				begin = false;
				break;
			case ID_DELETE: 
				cancel();
				break;
			case ID_PASS: 
				pass();
				break;
			case ID_TURN:
				DialogBox(hInstance, "IDD_DIALOG1", hwnd, DlgProc);
				InvalidateRect(hwnd, 0, 0);
				break;
			case ID_PLAY: 
				if (incomp())
					play_round = 0;
				SetTimer(hwnd, 2, delaytime, TimerProc2); 
				break;
			case ID_REBEGIN: 
				KillTimer(hwnd, 1);
				begin = false;
				play_init();
				break;
			case ID_HINT: 
				calcscore(false);
				break;
			case ID_INFO: 
				MessageBox(0,
					"3D五子棋1.0版\n"
					"规则：在nxnxn网格中，某个与坐标轴垂直的平面上，\n"
					"m个相邻同色棋子连成一条横（竖、斜）线者胜。\n"
					"用鼠标点击格点，看到格点亮起时，\n"
					"再点击一次或按空格以放置棋子。\n"
					"使用上下左右箭头调整方向。按Ctrl以开始/暂停。\n"
					"使用+/-以放大缩小。按delete可以悔棋。",
					"规则", MB_ICONINFORMATION | MB_OK);
				break;
			case ID_EXIT:
				if(MessageBox(0, "确定退出吗？", 0, 
					MB_ICONINFORMATION | MB_OKCANCEL) == IDOK)
					PostQuitMessage(0);
				break;
			case ID_HUMAN_1:
				player1 = 0;
				break;
			case ID_MACHINE_1: 
				player1 = 1;
				break;
			case ID_HUMAN_2:
				player2 = 0;
				break;
			case ID_MACHINE_2:
				player2 = 1;
				break;
			default:
				if(lwparam >= ID_WINNUM_4 && lwparam <= ID_WINNUM_7)
				{
					numtowin = lwparam - ID_WINNUM_4 + 4;
					break;
				}
				if(lwparam > MENUTIME && lwparam <= ID_TIME_20)
				{
					delaytime = (lwparam - MENUTIME) * 100;
					if(begin) 
						SetTimer(hwnd, 1, delaytime, TimerProc1); 
					break;
				}
				if(lwparam > MENUSIZE && lwparam <= ID_SIZE_MAX
					&& MessageBox(0, "确定清空棋盘吗？",
					0, MB_ICONINFORMATION | MB_OKCANCEL) == IDOK)
				{
					N = (lwparam - MENUSIZE) / 2;
					KillTimer(hwnd, 1);
					begin = false;
					Cleanup();
					play_init();
					Setup();
					break;
				}
		}
		CorrectMenu();
		InvalidateRect(hwnd, 0, 0);
		break;

	case WM_SIZE:
		GetClientRect(hwnd, &rect);
		Width  = rect.right;
		Height = rect.bottom;
		break;
	case WM_PAINT:
		BeginPaint(hwnd, &ps);
		Display(0);
		EndPaint(hwnd, &ps);
		break;
	case WM_LBUTTONDOWN: {
		int posinfo = calcclickpos(LOWORD(lParam), HIWORD(lParam));
		int i0 = (posinfo >> 16) & 0xff;
		int j0 = (posinfo >>  8) & 0xff;
		int k0 =  posinfo        & 0xff;
		if(state[i0][j0][k0] == 0)
			highlight(i0, j0, k0);
		else
		{
			confirm();
			if(begin) 
				SetTimer(hwnd, 1, delaytime, TimerProc1); 
		}
		InvalidateRect(hwnd, 0, 0);
		}break;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//主函数 
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE, PSTR szCmd, int)
{
	if (!InitMainWindow(hInstance)) 
		REPORTERROR("窗口");
	if (!d3d::InitD3D(hwnd, true, D3DDEVTYPE_HAL, &Device))
		REPORTERROR("InitD3D()");
	
	if(*szCmd)
	{
		const char* p = InitName(szCmd);
		if(p && !OpenByName(p)) 
			REPORTERROR("文件");
		hmenu = GetMenu(hwnd);
		CorrectMenu();
	}
	
	Setup();
	d3d::EnterMsgLoop();
	Cleanup();
	Device->Release();
	return 0;
}
