//3D基本函数的文件 
#include "D3Dframe.h" 
#include "var.h"

const float spacesize = 2.0f;
ID3DXMesh* small_ball;
ID3DXMesh* big_ball;
ID3DXMesh* light_ball;
ID3DXMesh* line_cylinder;
D3DXMATRIX Worlds[25][25][25], temp;
D3DMATERIAL9 grey, green, red, redder, blue, bluer;

bool d3d::InitD3D(HWND hwnd, bool windowed, 
	D3DDEVTYPE deviceType, IDirect3DDevice9** device)
{//初始化Direct3D 
	HRESULT hr = 0;
	IDirect3D9* d3d9 = 0;
    d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
    if(!d3d9)
		REPORTERROR("Direct3DCreate9()");
    
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, deviceType, &caps);
	int vp = 0;
	if( caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT )
		 vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	else vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	
	D3DPRESENT_PARAMETERS d3dpp;
	d3dpp.BackBufferWidth            = Width;
	d3dpp.BackBufferHeight           = Height;
	d3dpp.BackBufferFormat           = D3DFMT_A8R8G8B8;
	d3dpp.BackBufferCount            = 1;
	d3dpp.MultiSampleType            = D3DMULTISAMPLE_NONE;
	d3dpp.MultiSampleQuality         = 0;
	d3dpp.SwapEffect                 = D3DSWAPEFFECT_DISCARD; 
	d3dpp.hDeviceWindow              = hwnd;
	d3dpp.Windowed                   = windowed;
	d3dpp.EnableAutoDepthStencil     = true; 
	d3dpp.AutoDepthStencilFormat     = D3DFMT_D24S8;
	d3dpp.Flags                      = 0;
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dpp.PresentationInterval       = D3DPRESENT_INTERVAL_IMMEDIATE;
	
	hr = d3d9->CreateDevice(
		D3DADAPTER_DEFAULT, deviceType, hwnd, vp, &d3dpp, device);
	if( FAILED(hr) )
	{
		d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
		hr = d3d9->CreateDevice(
			D3DADAPTER_DEFAULT, deviceType, hwnd, vp, &d3dpp, device);
		if( FAILED(hr) ){
			d3d9->Release();
			REPORTERROR("CreateDevice()");
		}
	}
	d3d9->Release();
	return true;
}

D3DLIGHT9 d3d::InitPointLight(D3DXVECTOR3* position, D3DXCOLOR* col)
{
	D3DLIGHT9 light;
	::ZeroMemory(&light, sizeof(light));
	light.Type      = D3DLIGHT_POINT;
	light.Ambient   = *col * 0.95f;
	light.Diffuse   = *col * 0.43f;
	light.Specular  = *col * 1.15f;
	light.Position  = *position;
	light.Range        = 500.0f;
	light.Falloff      = 1.0f;
	light.Attenuation0 = 1.0f;
	light.Attenuation1 = 0.04f;
	light.Attenuation2 = 0;
	return light;
}

D3DMATERIAL9 d3d::InitMtrl(D3DXCOLOR a, D3DXCOLOR d, 
	D3DXCOLOR s, D3DXCOLOR e, float pwr)
{
	D3DMATERIAL9 mtrl;
	mtrl.Ambient  = a;
	mtrl.Diffuse  = d;
	mtrl.Specular = s;
	mtrl.Emissive = e;
	mtrl.Power    = pwr;
	return mtrl;
}

bool Setup()
{
	D3DXFONT_DESC lf;

	ZeroMemory(&lf, sizeof(D3DXFONT_DESC));
	lf.Height  = 20;    
	lf.Width   = 10;     
	lf.Weight  = 500;      
	lf.CharSet = DEFAULT_CHARSET;         
	strcpy(lf.FaceName, "宋体"); // font style

	if(FAILED(D3DXCreateFontIndirect(Device, &lf, &Font)))
	{
		REPORTERROR("CreateFontIndirect()");
		::PostQuitMessage(0);
	}

	int i, j, k;
	timeBeginPeriod(1);
	small_ball = big_ball = light_ball = line_cylinder = 0;
	D3DXCreateSphere  (
		Device, spacesize / (20.0f * N), 20, 20, &small_ball, 0);
	D3DXCreateSphere  (
		Device, spacesize / (3.0f  * N), 80, 80, &big_ball,   0);
	D3DXCreateSphere  (
		Device, spacesize / (2.6f  * N), 40, 40, &light_ball, 0);
	D3DXCreateCylinder(
		Device, spacesize / (12.0f * N), 
		spacesize / (12.0f * N), 2.0f, 20, 1, &line_cylinder, 0);
	forijk 
		D3DXMatrixTranslation(&Worlds[i][j][k], F(i), F(j), F(k));
		
	grey  = d3d::GREY_MTRL;
	red   = d3d::RED_MTRL;
	blue  = d3d::BLUE_MTRL;
	green = d3d::GREEN_MTRL;
	redder= d3d::REDDER_MTRL;
	bluer = d3d::BLUER_MTRL;
	
	D3DXVECTOR3 pos(-10.0f, 6.0f, -6.0f);
	D3DXCOLOR c = d3d::WHITE;
	D3DLIGHT9 point = d3d::InitPointLight(&pos, &c);
	Device->SetLight(0, &point);
	Device->LightEnable(0, true);
	Device->SetRenderState(D3DRS_NORMALIZENORMALS, true);
	Device->SetRenderState(D3DRS_SPECULARENABLE, true);
	Device->SetRenderState(D3DRS_ALPHABLENDENABLE, true);
	Device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	Device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	D3DXMATRIX proj;
	D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI * 0.25f, 
	    (float) Width / (float) Height, 1.0f, 400.0f);
	Device->SetTransform(D3DTS_PROJECTION, &proj);
	return true;
}

void Cleanup()
{
	d3d::Release <ID3DXMesh*> (small_ball);
	d3d::Release <ID3DXMesh*> (big_ball);
	d3d::Release <ID3DXMesh*> (light_ball);
	d3d::Release <ID3DXMesh*> (line_cylinder);
	d3d::Release <ID3DXFont*> (Font);
	timeEndPeriod(1);
}

#define PRESS(P) if(GetAsyncKeyState(VK_##P) & 0x8000f)
bool Display(float deltaT)
{
	int i, j, k;
	bool moved = false;
	if(Device)
	{
		PRESS(LEFT)     angle -= 0.5f * deltaT, moved = true; 
		PRESS(RIGHT)    angle += 0.5f * deltaT, moved = true; 
		PRESS(UP)       theta += 0.5f * deltaT, moved = true; 
		PRESS(DOWN)     theta -= 0.5f * deltaT, moved = true; 
		PRESS(OEM_PLUS) dist  -=        deltaT, moved = true; 
		PRESS(OEM_MINUS)dist  +=        deltaT, moved = true; 

		if (theta >  D3DX_PI / 2.0f - d3d::EPSILON) 
			theta =  D3DX_PI / 2.0f - d3d::EPSILON;
		if (theta < -D3DX_PI / 2.0f + d3d::EPSILON) 
			theta = -D3DX_PI / 2.0f + d3d::EPSILON;
		if (dist < 5.0f)
		{
			float l = 1.0f + sqrtf(3.0f) * spacesize 
				+ spacesize / (2.6f * N); 
			if(dist < l) 
				dist = l;
		}
		if (dist > 20.0f)
			dist = 20.0f;
			
		D3DXVECTOR3 
			position(
				cosf(angle) * cosf(theta) * dist, 
				sinf(theta) * dist, 
				sinf(angle) * cosf(theta) * dist),
			target(0, 0, 0), 
			up    (0, 1, 0);
		D3DXMATRIX V;
		D3DXMatrixLookAtLH(&V, &position, &target, &up);
		
		Device->SetTransform(D3DTS_VIEW, &V);
		Device->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 
			0x000000, 1, 0);
		Device->BeginScene();
		
		//画棋子
		forijk
		{
			if      (state[i][j][k] ==  0 || state[i][j][k] > play_round) 
					 Device->SetMaterial(&grey);
			else if (state[i][j][k] == -1) 
					 Device->SetMaterial(&green);
			else if (state[i][j][k] % 2 == 1)
			{
				if  (state[i][j][k] == play_round)
				     Device->SetMaterial(&redder);
				else Device->SetMaterial(&red);
			}
			else if (state[i][j][k] % 2 == 0)
			{
				if  (state[i][j][k] == play_round)
				     Device->SetMaterial(&bluer);
				else Device->SetMaterial(&blue);
			}
			Device->SetTransform(D3DTS_WORLD, &Worlds[i][j][k]);
			//画球 
			if      (state[i][j][k] <= 0  || state[i][j][k] > play_round)
				small_ball->DrawSubset(0);
			else if (state[i][j][k] != play_round)
				big_ball  ->DrawSubset(0);
			else 
				light_ball->DrawSubset(0);
		}
		
		//画文字
		RECT rect = {0, 0, Width, Height};
		char infostr[100] = {};
		if(play_round != play_round_2)
			sprintf(infostr, "第%d/%d步\n浏览模式\n"
				"按Enter从当前局面下棋", 
				play_round,
				play_round_2);
		else
			sprintf(infostr, "第%d步\n轮到%s方", 
				play_round,
				play_round % 2? "蓝": "红");
		
		Font->DrawText(NULL, infostr, -1, &rect,
			DT_TOP | DT_LEFT, 0xffffff00);

		//画圆柱 （胜利时）
		if((status == 1 || status == 2) 
			&& ((pt.x1 - pt.x2) || (pt.y1 - pt.y2) || (pt.z1 - pt.z2))
			&& play_round == play_round_2)
		{
			D3DXVECTOR3 
				a(F(pt.x1), F(pt.y1), F(pt.z1)),
				b(F(pt.x2), F(pt.y2), F(pt.z2)),
				c, d, d0;
			
			c = 0.5f * (a + b),
			d = 0.5f * (a - b);
			D3DXVec3Normalize (&d0, &d);
			D3DXVECTOR3 e0(d.y - d.z, d.z - d.x, d.x - d.y), f0;
			if (d.y == d.z && d.z == d.x)
				e0.x = 1.0f, e0.y = 0.0f, e0.z = -1.0f;
			D3DXVec3Normalize (&e0, &e0);
			D3DXVec3Cross(&f0, &d0, &e0);
			D3DXMATRIX m(
			 e0.x, e0.y, e0.z, 0,
			 f0.x, f0.y, f0.z, 0,
 			  d.x,  d.y,  d.z, 0,
			  c.x,  c.y,  c.z, 1
			  );
			Device->SetMaterial(&green);
			Device->SetTransform(D3DTS_WORLD, &m);
			line_cylinder->DrawSubset(0);
		}
		Device->EndScene();
		Device->Present(0, 0, 0, 0);
	}
	return moved; 
}

//计算距离 
float calcdist2(D3DXVECTOR3* p, d3d::Ray* r)
{
	D3DXVECTOR3 *s = &(r->orn), *t = &(r->dir);
	D3DXVECTOR3 d = *s - *p;
	float a = D3DXVec3Dot(&d, t);
	return - (a * a) / ( D3DXVec3Dot(t, t) * D3DXVec3Dot(&d, &d) );
}
//计算点击位置
int calcclickpos(int x, int y)
{
	int i, j, k, i0 = 0, j0 = 0, k0 = 0;
	D3DXMATRIX proj;
	Device->GetTransform(D3DTS_PROJECTION, &proj);
	d3d::Ray ray;
	ray.orn = D3DXVECTOR3(0, 0, 0);
	ray.dir = D3DXVECTOR3( 
		((( 2.0f * x) /  Width) - 1) / proj(0, 0),
	    (((-2.0f * y) / Height) + 1) / proj(1, 1), 1);
	D3DXMATRIX view;
	Device->GetTransform(D3DTS_VIEW, &view);
	D3DXMATRIX viewinv;
	D3DXMatrixInverse(&viewinv,	0, &view);

	//矩阵变换 
	D3DXVec3TransformCoord (&ray.orn, &ray.orn, &viewinv);
	D3DXVec3TransformNormal(&ray.dir, &ray.dir, &viewinv);
	D3DXVec3Normalize      (&ray.dir, &ray.dir);

	float min = d3d::INFINITY_, tmp;
	forijk
	{//取夹角最小值 
		D3DXVECTOR3 p(F(i), F(j), F(k));
		if(state[i][j][k] <= 0 && (tmp = calcdist2(&p, &ray)) < min)
		{
			min = tmp;
			i0 = i;
			j0 = j;
			k0 = k;
		}
	}
	return (i0 << 16) | (j0 << 8) | k0;
}