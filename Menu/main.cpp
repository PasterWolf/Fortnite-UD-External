#include <iostream>
#include <Windows.h>
#include "C:\Users\20110\Desktop\AERO GG\Win32\win_utils.h"
#include "C:\Users\20110\Desktop\AERO GG\Protection\xor.hpp"
#include <dwmapi.h>
#include "Main.h"
#include <vector>
#include "C:\Users\20110\Desktop\AERO GG\Misc\stdafx.h"
#include "C:\Users\20110\Desktop\AERO GG\Defines\define.h"
#include <iostream>
#include <fstream>

#include "C:\Users\20110\Desktop\AERO GG\Driver\driver.h"



#include <windows.h>
#include <windows.h>
#include <Lmcons.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <chrono>
#include <ctime>
#include <iostream>
#include "C:\Users\20110\Desktop\AERO GG\Imgui\imgui_internal.h"


#include <cstdlib>
#include <iostream>
#include <chrono>
#include <random>
#include <tchar.h>

#include "main header.h"


namespace offests {

	static ULONG64 offset_uworld;
	static ULONG64 offest_gname;
	static ULONG64 uworld;
	static ULONG64 persistent_level;
	static ULONG64 game_instance;
	static ULONG64 local_players_array;
	static ULONG64 player_controller;
	static ULONG64 camera_manager;
	static ULONG64 rootcomponent;
	static ULONG64 Pawn;
	static Vector3 relativelocation;

	static ULONG64 actor_array;
	static ULONG32 actor_count;
}

float MiccaX;
float MiccaY;
float MiccaZ;

DWORD_PTR UWorldSig;

FTransform GetBoneIndex(DWORD_PTR mesh, int index) {
	DWORD_PTR bonearray = read<DWORD_PTR>(mesh + 0x5B0);
	if (bonearray == NULL) {
		bonearray = read<DWORD_PTR>(mesh + 0x5B0 + 0x10);
	}
	return read<FTransform>(bonearray + (index * 0x30));
}
Vector3 GetBoneWithRotation(DWORD_PTR mesh, int id) {
	FTransform bone = GetBoneIndex(mesh, id);
	FTransform ComponentToWorld = read<FTransform>(mesh + 0x1C0);
	D3DMATRIX Matrix;
	Matrix = MatrixMultiplication(bone.ToMatrixWithScale(), ComponentToWorld.ToMatrixWithScale());
	return Vector3(Matrix._41, Matrix._42, Matrix._43);
}
D3DMATRIX Matrix(Vector3 rot, Vector3 origin = Vector3(0, 0, 0)) {
	float radPitch = (rot.x * float(M_PI) / 180.f);
	float radYaw = (rot.y * float(M_PI) / 180.f);
	float radRoll = (rot.z * float(M_PI) / 180.f);

	float SP = sinf(radPitch);
	float CP = cosf(radPitch);
	float SY = sinf(radYaw);
	float CY = cosf(radYaw);
	float SR = sinf(radRoll);
	float CR = cosf(radRoll);

	D3DMATRIX matrix;
	matrix.m[0][0] = CP * CY;
	matrix.m[0][1] = CP * SY;
	matrix.m[0][2] = SP;
	matrix.m[0][3] = 0.f;

	matrix.m[1][0] = SR * SP * CY - CR * SY;
	matrix.m[1][1] = SR * SP * SY + CR * CY;
	matrix.m[1][2] = -SR * CP;
	matrix.m[1][3] = 0.f;

	matrix.m[2][0] = -(CR * SP * CY + SR * SY);
	matrix.m[2][1] = CY * SR - CR * SP * SY;
	matrix.m[2][2] = CR * CP;
	matrix.m[2][3] = 0.f;

	matrix.m[3][0] = origin.x;
	matrix.m[3][1] = origin.y;
	matrix.m[3][2] = origin.z;
	matrix.m[3][3] = 1.f;

	return matrix;
}

extern Vector3 CameraEXT(0, 0, 0);
float FovAngle;

Vector3 Camera;

Vector3 ProjectWorldToScreen(Vector3 WorldLocation) {
	Vector3 Screenlocation = Vector3(0, 0, 0);
	auto chain69 = read<uintptr_t>(Localplayer + 0xa8);
	uint64_t chain699 = read<uintptr_t>(chain69 + 8);
	Camera.x = read<float>(chain699 + 0x7E8);
	Camera.y = read<float>(Rootcomp + 0x12C);
	float test = asin(Camera.x);
	float degrees = test * (180.0 / M_PI);
	Camera.x = degrees;
	if (Camera.y < 0)
		Camera.y = 360 + Camera.y;
	D3DMATRIX tempMatrix = Matrix(Camera);
	Vector3 vAxisX, vAxisY, vAxisZ;
	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	uint64_t chain = read<uint64_t>(Localplayer + 0x70);
	uint64_t chain1 = read<uint64_t>(chain + 0x98);
	uint64_t chain2 = read<uint64_t>(chain1 + 0x140);

	Vector3 vDelta = WorldLocation - read<Vector3>(chain2 + 0x10);
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));
	if (vTransformed.z < 1.f)
		vTransformed.z = 1.f;

	float zoom = read<float>(chain699 + 0x580);
	float FovAngle = 80.0f / (zoom / 1.19f);
	float ScreenCenterX = Width / 2;
	float ScreenCenterY = Height / 2;
	float ScreenCenterZ = Height / 2;
	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.z = ScreenCenterZ - vTransformed.z * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	return Screenlocation;
}






HRESULT DirectXInit(HWND hWnd)
{
	if (FAILED(Direct3DCreate9Ex(D3D_SDK_VERSION, &p_Object)))
		exit(3);

	ZeroMemory(&p_Params, sizeof(p_Params));
	p_Params.Windowed = TRUE;
	p_Params.SwapEffect = D3DSWAPEFFECT_DISCARD;
	p_Params.hDeviceWindow = hWnd;
	p_Params.MultiSampleQuality = D3DMULTISAMPLE_NONE;
	p_Params.BackBufferFormat = D3DFMT_A8R8G8B8;
	p_Params.BackBufferWidth = Width;
	p_Params.BackBufferHeight = Height;
	p_Params.EnableAutoDepthStencil = TRUE;
	p_Params.AutoDepthStencilFormat = D3DFMT_D16;
	p_Params.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	if (FAILED(p_Object->CreateDeviceEx(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_HARDWARE_VERTEXPROCESSING, &p_Params, 0, &p_Device)))
	{
		p_Object->Release();
		exit(4);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::GetIO().Fonts->AddFontDefault();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.Fonts->AddFontFromFileTTF(E("C:\\Windows\\Fonts\\Arial.ttf"), 16.f);


	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF,
		0x0400, 0x044F,
		0,
	};


	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(p_Device);

	return S_OK;
}bool IsVec3Valid(Vector3 vec3)
{
	return !(vec3.x == 0 && vec3.y == 0 && vec3.z == 0);
}
void SetupWindow()
{
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)SetWindowToTarget, 0, 0, 0);

	WNDCLASSEXA wcex = {
		sizeof(WNDCLASSEXA),
		0,
		DefWindowProcA,
		0,
		0,
		nullptr,
		LoadIcon(nullptr, IDI_APPLICATION),
		LoadCursor(nullptr, IDC_ARROW),
		nullptr,
		nullptr,
		("Discord"),
		LoadIcon(nullptr, IDI_APPLICATION)
	};

	RECT Rect;
	GetWindowRect(GetDesktopWindow(), &Rect);

	RegisterClassExA(&wcex);

	MyWnd = CreateWindowExA(NULL, E("Discord"), E("Discord"), WS_POPUP, Rect.left, Rect.top, Rect.right, Rect.bottom, NULL, NULL, wcex.hInstance, NULL);
	SetWindowLong(MyWnd, GWL_EXSTYLE, WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOOLWINDOW);
	SetLayeredWindowAttributes(MyWnd, RGB(0, 0, 0), 255, LWA_ALPHA);
	//SetWindowDisplayAffinity(MyWnd, 1);

	MARGINS margin = { -1 };
	DwmExtendFrameIntoClientArea(MyWnd, &margin);

	ShowWindow(MyWnd, SW_SHOW);
	UpdateWindow(MyWnd);
}
Vector3 AimbotCorrection(float bulletVelocity, float bulletGravity, float targetDistance, Vector3 targetPosition, Vector3 targetVelocity) {
	Vector3 recalculated = targetPosition;
	float gravity = fabs(bulletGravity);
	float time = targetDistance / fabs(bulletVelocity);
	float bulletDrop = (gravity / 250) * time * time;
	recalculated.z += bulletDrop * 120;
	recalculated.x += time * (targetVelocity.x);
	recalculated.y += time * (targetVelocity.y);
	recalculated.z += time * (targetVelocity.z);
	return recalculated;
}

void SetMouseAbsPosition(DWORD x, DWORD y)
{
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	input.mi.dx = x;
	input.mi.dy = y;
	SendInput(1, &input, sizeof(input));
}
static auto move_to(float x, float y) -> void {
	float center_x = (ImGui::GetIO().DisplaySize.x / 2);
	float center_y = (ImGui::GetIO().DisplaySize.y / 2);

	float target_x = 0;
	float target_y = 0;
	if (x != 0.f)
	{
		if (x > center_x)
		{
			target_x = -(center_x - x);
			target_x /= item.Smooth;
			if (target_x + center_x > center_x * 2.f) target_x = 0.f;
		}

		if (x < center_x)
		{
			target_x = x - center_x;
			target_x /= item.Smooth;
			if (target_x + center_x < 0.f) target_x = 0.f;
		}
	}

	if (y != 0.f)
	{
		if (y > center_y)
		{
			target_y = -(center_y - y);
			target_y /= item.Smooth;
			if (target_y + center_y > center_y * 2.f) target_y = 0.f;
		}

		if (y < center_y)
		{
			target_y = y - center_y;
			target_y /= item.Smooth;
			if (target_y + center_y < 0.f) target_y = 0.f;
		}
	}

	SetMouseAbsPosition(static_cast<DWORD>(target_x), static_cast<DWORD>(target_y));
}
double GetCrossDistance(double x1, double y1, double z1, double x2, double y2, double z2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2));
}

#define PI 3.14159265358979323846f

typedef struct
{
	DWORD R;
	DWORD G;
	DWORD B;
	DWORD A;
}RGBA;

class Color
{
public:
	RGBA red = { 255,0,0,255 };
	RGBA Magenta = { 255,0,255,255 };
	RGBA yellow = { 255,255,0,255 };
	RGBA grayblue = { 128,128,255,255 };
	RGBA green = { 128,224,0,255 };
	RGBA darkgreen = { 0,224,128,255 };
	RGBA brown = { 192,96,0,255 };
	RGBA pink = { 255,168,255,255 };
	RGBA DarkYellow = { 216,216,0,255 };
	RGBA SilverWhite = { 236,236,236,255 };
	RGBA purple = { 144,0,255,255 };
	RGBA Navy = { 88,48,224,255 };
	RGBA skyblue = { 0,136,255,255 };
	RGBA graygreen = { 128,160,128,255 };
	RGBA blue = { 0,96,192,255 };
	RGBA orange = { 255,128,0,255 };
	RGBA peachred = { 255,80,128,255 };
	RGBA reds = { 255,128,192,255 };
	RGBA darkgray = { 96,96,96,255 };
	RGBA Navys = { 0,0,128,255 };
	RGBA darkgreens = { 0,128,0,255 };
	RGBA darkblue = { 0,128,128,255 };
	RGBA redbrown = { 128,0,0,255 };
	RGBA purplered = { 128,0,128,255 };
	RGBA greens = { 25,255,25,140 };
	RGBA envy = { 0,255,255,255 };
	RGBA black = { 0,0,0,255 };
	RGBA neger = { 215, 240, 180, 255 };
	RGBA negernot = { 222, 180, 200, 255 };
	RGBA gray = { 128,128,128,255 };
	RGBA white = { 255,255,255,255 };
	RGBA blues = { 30,144,255,255 };
	RGBA lightblue = { 135,206,250,255 };
	RGBA Scarlet = { 220, 20, 60, 160 };
	RGBA white_ = { 255,255,255,200 };
	RGBA gray_ = { 128,128,128,200 };
	RGBA black_ = { 0,0,0,200 };
	RGBA red_ = { 255,0,0,200 };
	RGBA Magenta_ = { 255,0,255,200 };
	RGBA yellow_ = { 255,255,0,200 };
	RGBA grayblue_ = { 128,128,255,200 };
	RGBA green_ = { 128,224,0,200 };
	RGBA darkgreen_ = { 0,224,128,200 };
	RGBA brown_ = { 192,96,0,200 };
	RGBA pink_ = { 255,168,255,200 };
	RGBA darkyellow_ = { 216,216,0,200 };
	RGBA silverwhite_ = { 236,236,236,200 };
	RGBA purple_ = { 144,0,255,200 };
	RGBA Blue_ = { 88,48,224,200 };
	RGBA skyblue_ = { 0,136,255,200 };
	RGBA graygreen_ = { 128,160,128,200 };
	RGBA blue_ = { 0,96,192,200 };
	RGBA orange_ = { 255,128,0,200 };
	RGBA pinks_ = { 255,80,128,200 };
	RGBA Fuhong_ = { 255,128,192,200 };
	RGBA darkgray_ = { 96,96,96,200 };
	RGBA Navy_ = { 0,0,128,200 };
	RGBA darkgreens_ = { 0,128,0,200 };
	RGBA darkblue_ = { 0,128,128,200 };
	RGBA redbrown_ = { 128,0,0,200 };
	RGBA purplered_ = { 128,0,128,200 };
	RGBA greens_ = { 0,255,0,200 };
	RGBA envy_ = { 0,255,255,200 };

	RGBA glassblack = { 0, 0, 0, 160 };
	RGBA GlassBlue = { 65,105,225,80 };
	RGBA glassyellow = { 255,255,0,160 };
	RGBA glass = { 200,200,200,60 };

	RGBA filled = { 0, 0, 0, 150 };

	RGBA Plum = { 221,160,221,160 };

	RGBA rainbow() {

		static float x = 0, y = 0;
		static float r = 0, g = 0, b = 0;

		if (y >= 0.0f && y < 255.0f) {
			r = 255.0f;
			g = 0.0f;
			b = x;
		}
		else if (y >= 255.0f && y < 510.0f) {
			r = 255.0f - x;
			g = 0.0f;
			b = 255.0f;
		}
		else if (y >= 510.0f && y < 765.0f) {
			r = 0.0f;
			g = x;
			b = 255.0f;
		}
		else if (y >= 765.0f && y < 1020.0f) {
			r = 0.0f;
			g = 255.0f;
			b = 255.0f - x;
		}
		else if (y >= 1020.0f && y < 1275.0f) {
			r = x;
			g = 255.0f;
			b = 0.0f;
		}
		else if (y >= 1275.0f && y < 1530.0f) {
			r = 255.0f;
			g = 255.0f - x;
			b = 0.0f;
		}

		x += item.rainbow_speed; //increase this value to switch colors faster
		if (x >= 255.0f)
			x = 0.0f;

		y += item.rainbow_speed; //increase this value to switch colors faster
		if (y > 1530.0f)
			y = 0.0f;


		return RGBA{ (DWORD)r, (DWORD)g, (DWORD)b, 255 };
	}

};
Color Col;

std::string string_To_UTF8(const std::string& str)
{
	int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);

	wchar_t* pwBuf = new wchar_t[nwLen + 1];
	ZeroMemory(pwBuf, nwLen * 2 + 2);

	::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);

	int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);

	char* pBuf = new char[nLen + 1];
	ZeroMemory(pBuf, nLen + 1);

	::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);

	std::string retStr(pBuf);

	delete[]pwBuf;
	delete[]pBuf;

	pwBuf = NULL;
	pBuf = NULL;

	return retStr;
}
std::string WStringToUTF8(const wchar_t* lpwcszWString)
{
	char* pElementText;
	int iTextLen = ::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, NULL, 0, NULL, NULL);
	pElementText = new char[iTextLen + 1];
	memset((void*)pElementText, 0, (iTextLen + 1) * sizeof(char));
	::WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)lpwcszWString, -1, pElementText, iTextLen, NULL, NULL);
	std::string strReturn(pElementText);
	delete[] pElementText;
	return strReturn;
}
std::wstring MBytesToWString(const char* lpcszString)
{
	int len = strlen(lpcszString);
	int unicodeLen = ::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, NULL, 0);
	wchar_t* pUnicode = new wchar_t[unicodeLen + 1];
	memset(pUnicode, 0, (unicodeLen + 1) * sizeof(wchar_t));
	::MultiByteToWideChar(CP_ACP, 0, lpcszString, -1, (LPWSTR)pUnicode, unicodeLen);
	std::wstring wString = (wchar_t*)pUnicode;
	delete[] pUnicode;
	return wString;
}

void DrawStrokeText(int x, int y, RGBA* color, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}
void DrawStrokeText2(int x, int y, RGBA* color, const std::string str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x - 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}
void DrawNewText(int x, int y, RGBA* color, const char* str)
{
	ImFont a;
	std::string utf_8_1 = std::string(str);
	std::string utf_8_2 = string_To_UTF8(utf_8_1);
	ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
}
void DrawRect(int x, int y, int w, int h, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0, thickness);
}
void DrawFilledRect(int x, int y, int w, int h, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0);
}
void DrawLeftProgressBar(int x, int y, int w, int h, int thick, int m_health)
{
	int G = (255 * m_health / 100);
	int R = 255 - G;
	RGBA healthcol = { R, G, 0, 255 };

	DrawFilledRect(x - (w / 2) - 3, y, thick, (h)*m_health / 100, &healthcol);
}
void DrawRightProgressBar(int x, int y, int w, int h, int thick, int m_health)
{
	int G = (255 * m_health / 100);
	int R = 255 - G;
	RGBA healthcol = { R, G, 0, 255 };

	DrawFilledRect(x + (w / 2) - 25, y, thick, (h)*m_health / 100, &healthcol);
}
void DrawString(float fontSize, int x, int y, RGBA* color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 2;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), text.c_str());
}
void DrawCircleFilled(int x, int y, int radius, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddCircleFilled(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
}
void DrawCircle(int x, int y, int radius, RGBA* color, int segments)
{
	ImGui::GetOverlayDrawList()->AddCircle(ImVec2(x, y), radius, ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), segments);
}
void DrawTriangle(int x1, int y1, int x2, int y2, int x3, int y3, RGBA* color, float thickne)
{
	ImGui::GetOverlayDrawList()->AddTriangle(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickne);
}
void DrawTriangleFilled(int x1, int y1, int x2, int y2, int x3, int y3, RGBA* color)
{
	ImGui::GetOverlayDrawList()->AddTriangleFilled(ImVec2(x1, y1), ImVec2(x2, y2), ImVec2(x3, y3), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
}
void DrawLine(int x1, int y1, int x2, int y2, RGBA* color, int thickness)
{
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(x1, y1), ImVec2(x2, y2), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), thickness);
}
void DrawCornerBox(int x, int y, int w, int h, int borderPx, RGBA* color)
{
	DrawFilledRect(x + borderPx, y, w / 3, borderPx, color); //top 
	DrawFilledRect(x + w - w / 3 + borderPx, y, w / 3, borderPx, color); //top 
	DrawFilledRect(x, y, borderPx, h / 3, color); //left 
	DrawFilledRect(x, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color); //left 
	DrawFilledRect(x + borderPx, y + h + borderPx, w / 3, borderPx, color); //bottom 
	DrawFilledRect(x + w - w / 3 + borderPx, y + h + borderPx, w / 3, borderPx, color); //bottom 
	DrawFilledRect(x + w + borderPx, y, borderPx, h / 3, color);//right 
	DrawFilledRect(x + w + borderPx, y + h - h / 3 + borderPx * 2, borderPx, h / 3, color);//right 
}
void DrawNormalBox(int x, int y, int w, int h, int borderPx, RGBA* color)
{
	DrawFilledRect(x + borderPx, y, w, borderPx, color); //top 
	DrawFilledRect(x + w - w + borderPx, y, w, borderPx, color); //top 
	DrawFilledRect(x, y, borderPx, h, color); //left 
	DrawFilledRect(x, y + h - h + borderPx * 2, borderPx, h, color); //left 
	DrawFilledRect(x + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
	DrawFilledRect(x + w - w + borderPx, y + h + borderPx, w, borderPx, color); //bottom 
	DrawFilledRect(x + w + borderPx, y, borderPx, h, color);//right 
	DrawFilledRect(x + w + borderPx, y + h - h + borderPx * 2, borderPx, h, color);//right 
}
void DrawLine2(const ImVec2& from, const ImVec2& to, uint32_t color, float thickness)
{
	float a = (color >> 24) & 0xff;
	float r = (color >> 16) & 0xff;
	float g = (color >> 8) & 0xff;
	float b = (color) & 0xff;
	ImGui::GetOverlayDrawList()->AddLine(from, to, ImGui::GetColorU32(ImVec4(r / 255, g / 255, b / 255, a / 255)), thickness);
}
void DrawRectRainbow(int x, int y, int width, int height, float flSpeed, RGBA* color, float& flRainbow)
{
	ImDrawList* windowDrawList = ImGui::GetWindowDrawList();

	flRainbow += flSpeed;
	if (flRainbow > 1.f) flRainbow = 0.f;

	for (int i = 0; i < width; i++)
	{
		float hue = (1.f / (float)width) * i;
		hue -= flRainbow;
		if (hue < 0.f) hue += 1.f;

		windowDrawList->AddRectFilled(ImVec2(x + i, y), ImVec2(width, height), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)));
	}
}

typedef struct _FNlEntity {
	uint64_t Actor;
	int ID;
	uint64_t mesh;
}FNlEntity;

std::vector<FNlEntity> entityList;

std::string GetNameFromFName(int key)
{
	uint32_t ChunkOffset = (uint32_t)((int)(key) >> 16);
	uint16_t NameOffset = (uint16_t)key;

	uint64_t NamePoolChunk = read<uint64_t>((uintptr_t)sdk::module_base + 0xB908F40 + ((ChunkOffset + 2) * 8)); // ERROR_NAME_SIZE_EXCEEDED
	uint64_t entryOffset = NamePoolChunk + (DWORD)(2 * NameOffset);
	uint16_t nameEntry = read<uint16_t>(entryOffset);

	int nameLength = nameEntry >> 6;
	char buff[1028];

	char* v2 = buff; // rdi
	unsigned __int16* v3; // rbx
	signed int v4 = nameLength; // ebx
	__int16 result; // ax
	signed int v6; // edx
	signed int v7; // ecx
	signed int v8; // eax
	unsigned int v9; // ecx

	static uintptr_t decryptOffset = NULL;
	if (!decryptOffset)
		decryptOffset = read<uintptr_t>((uintptr_t)sdk::module_base + 0xB7AA3D8);

	result = decryptOffset >> 5;
	v6 = 0;
	v7 = 24;

	if ((uint32_t)nameLength && nameLength > 0)
	{
		driver->ReadProcessMemory(entryOffset + 2, buff, nameLength);

		do
		{
			v8 = v6++;
			v9 = (v8 | 0xB000) + v7;
			result = v9 ^ ~*v2;
			v7 = v9 >> 2;
			*v2 = result;
			++v2;
		} while (v6 < v4);


		buff[nameLength] = '\0';
		return std::string(buff);
	}
	else
	{
		return "";
	}
}

std::uintptr_t sigscan(const char* sig, const char* mask)
{
	auto buffer = std::make_unique<std::array<std::uint8_t, 0x100000>>();
	auto data = buffer.get()->data();

	for (std::uintptr_t i = 0u; i < (2u << 25u); ++i)
	{
		driver->ReadProcessMemory(sdk::module_base + i * 0x100000, data, 0x100000);

		if (!data)
			return 0;

		for (std::uintptr_t j = 0; j < 0x100000u; ++j)
		{
			if ([](std::uint8_t const* data, std::uint8_t const* sig, char const* mask)
				{
					for (; *mask; ++mask, ++data, ++sig)
					{
						if (*mask == 'x' && *data != *sig) return false;
					}
					return (*mask) == 0;
				}(data + j, (std::uint8_t*)sig, mask))
			{
				std::uintptr_t result = sdk::module_base + i * 0x100000 + j;
				std::uint32_t rel = 0;

				driver->ReadProcessMemory(result + 3, &rel, sizeof(std::uint32_t));

				if (!rel)
					return 0;

				return result;
			}
		}
	}

	return 0;
}


class FTextData {
public:
	char pad_0x0000[0x30];  //0x0000
	wchar_t* Name;          //0x0028 
	__int32 Length;         //0x0030 

};

struct FText {
	FTextData* Data;
	char UnknownData[0x10];

	wchar_t* Get() const {
		if (Data) {
			return Data->Name;
		}

		return nullptr;
	}
};

RGBA getColorFromTier(BYTE Tier) {
	RGBA color = { 255.f, 255.f, 255.f, 1.f };

	if (Tier == 1)
		color = { 255, 255, 255, 1 };
	else if (Tier == 2)
		color = { 255, 0, 0, 1 };
	else if (Tier == 3)
		color = { 9, 63, 150, 1 };
	else if (Tier == 4)
		color = { 154, 18, 179, 1 };
	else if (Tier == 5)
		color = { 95, 85, 45, 1 };

	return color;
}

uint64_t CurrentActor;
Vector3 relativelocationA;

uintptr_t curactoridA;

Vector3 LootPos;

wchar_t* LootName;

uintptr_t Tier;

RGBA LootColor;

namespace RenderA
{
	struct color_keys
	{
		ImVec4 value;
		color_keys() { value.x = value.y = value.z = value.y = 0.0f; }
		color_keys(int r, int g, int b, int a = 255) { float sc = 1.0f / 255.0f; value.x = (float)r * sc; value.y = (float)g * sc; value.z = (float)b * sc; value.w = (float)a * sc; }
		color_keys(float r, float g, float b, float a = 1.0f) { value.x = r; value.y = g; value.z = b; value.w = a; }
		color_keys(const ImVec4& col) { value = col; }
	};
	color_keys* col_keys;

	struct vec_2
	{
		int x, y;
	};

	struct Colors
	{
		ImColor red = { 255, 0, 0, 255 };
		ImColor green = { 255, 0, 0, 255 };
		ImColor blue = { 0, 136, 255, 255 };
		ImColor aqua_blue = { 0, 255, 255, 255 };
		ImColor cyan = { 0, 210, 210, 255 };
		ImColor royal_purple = { 102, 0, 255, 255 };
		ImColor dark_pink = { 255, 0, 174, 255 };
		ImColor black = { 0, 0, 0, 255 };
		ImColor white = { 255, 255, 255, 255 };
		ImColor purple = { 255, 0, 255, 255 };
		ImColor yellow = { 255, 255, 0, 255 };
		ImColor orange = { 255, 140, 0, 255 };
		ImColor gold = { 234, 255, 0, 255 };
		ImColor royal_blue = { 0, 30, 255, 255 };
		ImColor dark_red = { 150, 5, 5, 255 };
		ImColor dark_green = { 5, 150, 5, 255 };
		ImColor dark_blue = { 100, 100, 255, 255 };
		ImColor navy_blue = { 0, 73, 168, 255 };
		ImColor light_gray = { 200, 200, 200, 255 };
		ImColor dark_gray = { 150, 150, 150, 255 };
	};
	Colors color;

	void Text(int posx, int posy, ImColor clr, const char* pText, ...)
	{
		va_list va_alist;
		char buf[1024] = { 0 };
		va_start(va_alist, pText);
		_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
		va_end(va_alist);
		std::string textA = WStringToUTF8(MBytesToWString(buf).c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), 14.f, ImVec2(posx, posy), ImColor(clr), textA.c_str());
	}

	void OutlinedText(int posx, int posy, ImColor clr, const char* text)
	{
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx + 1, posy + 1), ImColor(color.black), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx - 1, posy - 1), ImColor(color.black), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx + 1, posy + 1), ImColor(color.black), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx - 1, posy - 1), ImColor(color.black), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx, posy), ImColor(clr), text);
	}

	void ShadowText(int posx, int posy, ImColor clr, const char* text)
	{
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx + 1, posy + 2), ImColor(0, 0, 0, 200), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx + 1, posy + 2), ImColor(0, 0, 0, 200), text);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(posx, posy), ImColor(clr), text);
	}

	void Rect(int x, int y, int w, int h, ImColor color, int thickness)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0, thickness);
	}

	void RectFilledGradient(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void RectFilled(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void Tab(const char* v, float size_x, float size_y, static int tab_name, int tab_next)
	{
		if (ImGui::Button(v, ImVec2{ size_x, size_y })) tab_name = tab_next;
	}
}

void DrawLString(float fontSize, int x, int y, ImU32 color, bool bCenter, bool stroke, const char* pText, ...)
{
	va_list va_alist;
	char buf[1024] = { 0 };
	va_start(va_alist, pText);
	_vsnprintf_s(buf, sizeof(buf), pText, va_alist);
	va_end(va_alist);
	std::string text = WStringToUTF8(MBytesToWString(buf).c_str());
	if (bCenter)
	{
		ImVec2 textSize = ImGui::CalcTextSize(text.c_str());
		x = x - textSize.x / 2;
		y = y - textSize.y;
	}
	if (stroke)
	{
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x + 1, y - 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x - 1, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(0, 0, 0, 1)), text.c_str());
	}
	ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), fontSize, ImVec2(x, y), color, text.c_str());
}


std::string Names2;
std::string Names4;

Vector3 WeakspotPorn;

//void cache()
//{
//	while (true) {
//		
//	}
//}

typedef struct _LootEntity {
	ImDrawList* Renderer;
	std::string name;
	uintptr_t CurrentActor;
	Vector3 LocalRelativeLocation;
}LootEntity;
static std::vector<LootEntity> LootentityList;



void drawlootloop() {
	while (true) {

		std::vector<FNlEntity> tmpList;
		std::vector<LootEntity> tmpListA;

		Uworld = read<DWORD_PTR>(sdk::module_base + 0xc446238);
		DWORD_PTR Gameinstance = read<DWORD_PTR>(Uworld + 0x190);
		DWORD_PTR LocalPlayers = read<DWORD_PTR>(Gameinstance + 0x38);

		Localplayer = read<DWORD_PTR>(LocalPlayers);
		PlayerController = read<DWORD_PTR>(Localplayer + 0x30);

		LocalPawn = read<DWORD_PTR>(PlayerController + 0x338);

		PlayerState = read<DWORD_PTR>(LocalPawn + 0x2B8);
		Rootcomp = read<DWORD_PTR>(LocalPawn + 0x138);

		offests::uworld = read<uint64_t>(sdk::module_base + 0xc446238);

		offests::game_instance = read<uint64_t>(offests::uworld + 0x190);

		offests::local_players_array = read<uint64_t>(read<uint64_t>(offests::game_instance + 0x38));

		offests::player_controller = read<uint64_t>(offests::local_players_array + 0x30);


		offests::rootcomponent = read<uint64_t>(offests::Pawn + 0x138);

		offests::relativelocation = read<Vector3>(offests::rootcomponent + 0x11C);

		relativelocation = read<DWORD_PTR>(Rootcomp + 0x11C);

		if (LocalPawn != 0) {
			localplayerID = read<int>(LocalPawn + 0x18);
		}

		Persistentlevel = read<DWORD_PTR>(Uworld + 0x30);
		DWORD ActorCount = read<DWORD>(Persistentlevel + 0xA0);
		DWORD_PTR AActors = read<DWORD_PTR>(Persistentlevel + 0x98);

		for (int i = 0; i < read<DWORD>(Uworld + (0x148 + sizeof(PVOID))); ++i) {
			uintptr_t ItemLevels = read<uintptr_t>(Uworld + 0x148);
			if (!ItemLevels) return;

			uintptr_t ItemLevel = read<uintptr_t>(ItemLevels + (i * sizeof(uintptr_t)));
			if (!ItemLevel) return;

			for (int i = 0; i < read<DWORD>(ItemLevel + (0x98 + sizeof(PVOID))); ++i) {

				uintptr_t ItemsPawns = read<uintptr_t>(ItemLevel + 0x98);
				uintptr_t CurrentItemPawn = read<uintptr_t>(ItemsPawns + (i * sizeof(uintptr_t)));
				int currentitemid = read<int>(CurrentItemPawn + 0x18);
				auto CurrentItemPawnName = GetNameFromFName(currentitemid);

				if (strstr(CurrentItemPawnName.c_str(), ("WestSausage")) || strstr(CurrentItemPawnName.c_str(), ("FortPickupAthena")) || strstr(CurrentItemPawnName.c_str(), ("Tiered_Chest")) || strstr(CurrentItemPawnName.c_str(), ("Assualt"))
					|| strstr(CurrentItemPawnName.c_str(), ("Athena_Shields")) || strstr(CurrentItemPawnName.c_str(), ("Vehicle")))
				{
					LootEntity fnlEntity{ };
					fnlEntity.CurrentActor = CurrentItemPawn;
					fnlEntity.name = CurrentItemPawnName;
					tmpListA.push_back(fnlEntity);
				}

				if (strstr(CurrentItemPawnName.c_str(), ("WeakSpot")))
				{
					auto Arootcomp = read<uint64_t>(CurrentItemPawn + 0x138);
					auto Arelativelocation = read<Vector3>(Arootcomp + 0x11C);
					WeakspotPorn = ProjectWorldToScreen(Arelativelocation);
					if (item.WeakspotAim && GetAsyncKeyState(item.aimkey)) {
						move_to(WeakspotPorn.x, WeakspotPorn.y);
					}
				}

				if (strstr(CurrentItemPawnName.c_str(), ("PlayerPawn"))) {
					FNlEntity fnlEntity{ };
					fnlEntity.Actor = CurrentItemPawn;
					fnlEntity.mesh = read<uint64_t>(CurrentItemPawn + 0x288);
					fnlEntity.ID = curactoridA;
					tmpList.push_back(fnlEntity);
				}
			}
		}

		entityList = tmpList;
		LootentityList = tmpListA;
		Sleep(1);
	}
}


void AimAt(DWORD_PTR entity) {
	uint64_t currentactormesh = read<uint64_t>(entity + 0x288);
	auto rootHead = GetBoneWithRotation(currentactormesh, 98);


	if (item.Aim_Prediction) {
		float distance = localactorpos.Distance(rootHead) / 250;
		uint64_t CurrentActorRootComponent = read<uint64_t>(entity + 0x138);
		Vector3 vellocity = read<Vector3>(CurrentActorRootComponent + 0x140);
		Vector3 Predicted = AimbotCorrection(30000, -1004, distance, rootHead, vellocity);
		Vector3 rootHeadOut = ProjectWorldToScreen(Predicted);
		if (rootHeadOut.x != 0 || rootHeadOut.y != 0 || rootHeadOut.z != 0) {
			if ((GetCrossDistance(rootHeadOut.x, rootHeadOut.y, rootHeadOut.z, Width / 2, Height / 2, Depth / 2) <= item.AimFOV * 1)) {
				move_to(rootHeadOut.x, rootHeadOut.y);

			}
		}
	}
	else {
		Vector3 rootHeadOut = ProjectWorldToScreen(rootHead);
		if (rootHeadOut.x != 0 || rootHeadOut.y != 0 || rootHeadOut.z != 0) {
			if ((GetCrossDistance(rootHeadOut.x, rootHeadOut.y, rootHeadOut.z, Width / 2, Height / 2, Depth / 2) <= item.AimFOV * 1)) {
				move_to(rootHeadOut.x, rootHeadOut.y);
			}
		}
	}
}
void AimAt2(DWORD_PTR entity) {
	uint64_t currentactormesh = read<uint64_t>(entity + 0x288);
	auto rootHead = GetBoneWithRotation(currentactormesh, 98);

	if (item.Aim_Prediction) {
		float distance = localactorpos.Distance(rootHead) / 250;
		uint64_t CurrentActorRootComponent = read<uint64_t>(entity + 0x138);
		Vector3 vellocity = read<Vector3>(CurrentActorRootComponent + 0x140);
		Vector3 Predicted = AimbotCorrection(30000, -1004, distance, rootHead, vellocity);
		Vector3 rootHeadOut = ProjectWorldToScreen(Predicted);
		if (rootHeadOut.x != 0 || rootHeadOut.y != 0 || rootHeadOut.z != 0) {
			if ((GetCrossDistance(rootHeadOut.x, rootHeadOut.y, rootHeadOut.z, Width / 2, Height / 2, Depth / 2) <= item.AimFOV * 1)) {
				if (item.Locl_line) {
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(rootHeadOut.x, rootHeadOut.y), ImGui::GetColorU32({ item.LockLine[0], item.LockLine[1], item.LockLine[2], 1.0f }), item.Thickness);

				}
			}
		}
	}
	else {
		Vector3 rootHeadOut = ProjectWorldToScreen(rootHead);
		if (rootHeadOut.x != 0 || rootHeadOut.y != 0 || rootHeadOut.z != 0) {
			if ((GetCrossDistance(rootHeadOut.x, rootHeadOut.y, rootHeadOut.z, Width / 2, Height / 2, Depth / 2) <= item.AimFOV * 1)) {
				if (item.Locl_line) {
					ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2), ImVec2(rootHeadOut.x, rootHeadOut.y), ImGui::GetColorU32({ item.LockLine[0], item.LockLine[1], item.LockLine[2], 1.0f }), item.Thickness);
				}
			}
		}
	}
}

void DrawSkeleton(DWORD_PTR mesh, ImColor SkelColor)
{

	Vector3 vHeadBone = GetBoneWithRotation(mesh, 98);
	Vector3 vHip = GetBoneWithRotation(mesh, 2);
	Vector3 vHead = GetBoneWithRotation(mesh, 68);
	Vector3 vNeck = GetBoneWithRotation(mesh, 66);
	Vector3 vUpperArmLeft = GetBoneWithRotation(mesh, 93);
	Vector3 vUpperArmRight = GetBoneWithRotation(mesh, 9);
	Vector3 vLeftHand = GetBoneWithRotation(mesh, 62);
	Vector3 vRightHand = GetBoneWithRotation(mesh, 33);
	Vector3 vLeftHand1 = GetBoneWithRotation(mesh, 100);
	Vector3 vRightHand1 = GetBoneWithRotation(mesh, 99);
	Vector3 vRightThigh = GetBoneWithRotation(mesh, 69);
	Vector3 vLeftThigh = GetBoneWithRotation(mesh, 76);
	Vector3 vRightCalf = GetBoneWithRotation(mesh, 72);
	Vector3 vLeftCalf = GetBoneWithRotation(mesh, 79);
	Vector3 vLeftFoot = GetBoneWithRotation(mesh, 85);
	Vector3 vRightFoot = GetBoneWithRotation(mesh, 84);
	Vector3 vHeadBoneOut = ProjectWorldToScreen(vHeadBone);
	Vector3 vHipOut = ProjectWorldToScreen(vHip);
	Vector3 vNeckOut = ProjectWorldToScreen(vNeck);
	Vector3 vUpperArmLeftOut = ProjectWorldToScreen(vUpperArmLeft);
	Vector3 vUpperArmRightOut = ProjectWorldToScreen(vUpperArmRight);
	Vector3 vLeftHandOut = ProjectWorldToScreen(vLeftHand);
	Vector3 vRightHandOut = ProjectWorldToScreen(vRightHand);
	Vector3 vLeftHandOut1 = ProjectWorldToScreen(vLeftHand1);
	Vector3 vRightHandOut1 = ProjectWorldToScreen(vRightHand1);
	Vector3 vRightThighOut = ProjectWorldToScreen(vRightThigh);
	Vector3 vLeftThighOut = ProjectWorldToScreen(vLeftThigh);
	Vector3 vRightCalfOut = ProjectWorldToScreen(vRightCalf);
	Vector3 vLeftCalfOut = ProjectWorldToScreen(vLeftCalf);
	Vector3 vLeftFootOut = ProjectWorldToScreen(vLeftFoot);
	Vector3 vRightFootOut = ProjectWorldToScreen(vRightFoot);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vHeadBoneOut.x, vHeadBoneOut.y), ImVec2(vNeckOut.x, vNeckOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vHipOut.x, vHipOut.y), ImVec2(vNeckOut.x, vNeckOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), ImVec2(vNeckOut.x, vNeckOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), ImVec2(vNeckOut.x, vNeckOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vUpperArmLeftOut.x, vUpperArmLeftOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vUpperArmRightOut.x, vUpperArmRightOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftHandOut.x, vLeftHandOut.y), ImVec2(vLeftHandOut1.x, vLeftHandOut1.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightHandOut.x, vRightHandOut.y), ImVec2(vRightHandOut1.x, vRightHandOut1.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftThighOut.x, vLeftThighOut.y), ImVec2(vHipOut.x, vHipOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightThighOut.x, vRightThighOut.y), ImVec2(vHipOut.x, vHipOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), ImVec2(vLeftThighOut.x, vLeftThighOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightCalfOut.x, vRightCalfOut.y), ImVec2(vRightThighOut.x, vRightThighOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vLeftFootOut.x, vLeftFootOut.y), ImVec2(vLeftCalfOut.x, vLeftCalfOut.y), SkelColor, 1.0f);
	ImGui::GetOverlayDrawList()->AddLine(ImVec2(vRightFootOut.x, vRightFootOut.y), ImVec2(vRightCalfOut.x, vRightCalfOut.y), SkelColor, 1.0f);
}

bool Headd = true;
bool Neck = false;
bool Chest = false;
ImDrawList* Rendererrr = ImGui::GetOverlayDrawList();

bool isVisible(uint64_t mesh)
{
	float bing = read<float>(mesh + 0x280);
	float bong = read<float>(mesh + 0x284);
	const float tick = 0.06f;
	return bong + tick >= bing;
}

int CounterandPrevent;

void SetControlRotation(Vector3 NewRotation, bool bResetCamera = false)
{
	auto SetControlRotation_ = (*(void(__fastcall**)(uintptr_t Controller, Vector3 NewRotation, bool bResetCamera))(read<uint64_t>(PlayerController + 0x6F0)));
	SetControlRotation_(PlayerController, NewRotation, bResetCamera);
}

void DrawESP() {

	float radarWidth = 230;
	float PosDx = 1280;
	float PosDy = 50;

	if (item.shield_esp)
	{
		ImVec2 midRadar = ImVec2(PosDx + (radarWidth / 2), PosDy + (radarWidth / 2));
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(PosDx, PosDy), ImVec2(PosDx + radarWidth, PosDy + radarWidth), IM_COL32(0, 0, 0, 45));
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(PosDx, PosDy), ImVec2(PosDx + radarWidth, PosDy + radarWidth), IM_COL32(0, 0, 0, 125));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(PosDx + (radarWidth / 2), PosDy + (radarWidth / 2)), ImVec2(PosDx, PosDy), IM_COL32(255, 255, 255, 90));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(PosDx + (radarWidth / 2), PosDy + (radarWidth / 2)), ImVec2(PosDx + radarWidth, PosDy), IM_COL32(255, 255, 255, 90));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(midRadar.x - (radarWidth / 2), midRadar.y), ImVec2(midRadar.x + (radarWidth / 2), midRadar.y), IM_COL32(255, 255, 255, 90));
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(midRadar.x, midRadar.y - (radarWidth / 2)), ImVec2(midRadar.x, midRadar.y + (radarWidth / 2)), IM_COL32(255, 255, 255, 90));
	}

	int curactorid = read<int>(CurrentActor + 0x18);
	//uintptr_t randomactormesh = read<uint64_t>(CurrentActor + 0x288);
	//Vector3 player_positionA = GetBoneWithRotation(randomactormesh, 68);
	//Vector3 player_screenA = ProjectWorldToScreen(player_positionA);

	//ImGui::GetOverlayDrawList()->AddText(ImGui::GetFont(), 14.f, ImVec2(player_screenA.x, player_screenA.y), ImColor(), std::to_string(curactoridAdsdd));

	auto entityListCopy = entityList;
	float closestDistance = FLT_MAX;
	DWORD_PTR closestPawn = NULL;

	/*if (CounterandPrevent == 0)
	{
		CounterandPrevent++;
	}
	else
	{
		drawlootloop();
	}*/

	DWORD_PTR AActors = read<DWORD_PTR>(Ulevel + 0x98);
	uint64_t persislevel = read<uint64_t>(Uworld + 0x30);
	uint64_t actors = read<uint64_t>(persislevel + 0x98);


	if (item.playerfly)
	{
		write<bool>(LocalPawn + 0x18D0 + 0x18, true);
		if (GetAsyncKeyState(VK_SHIFT))
		{
			write<float>(LocalPawn + 0x9C, 50);
		}

	}



	else
	{
		write<float>(LocalPawn + 0x9C, 0);
	}

	for (unsigned long i = 0; i < entityListCopy.size(); ++i) {
		FNlEntity entity = entityListCopy[i];
		uint64_t actor = read<uint64_t>(actors + (i * 0x8));
		uint64_t CurrentActor = read<uint64_t>(AActors + i * 0x8);

		uint64_t CurActorRootComponent = read<uint64_t>(entity.Actor + 0x138);
		if (CurActorRootComponent == (uint64_t)nullptr || CurActorRootComponent == -1 || CurActorRootComponent == NULL)
			continue;

		Vector3 actorpos = read<Vector3>(CurActorRootComponent + 0x11C);
		Vector3 actorposW2s = ProjectWorldToScreen(actorpos);

		DWORD64 otherPlayerState = read<uint64_t>(entity.Actor + 0x2B8);
		if (otherPlayerState == (uint64_t)nullptr || otherPlayerState == -1 || otherPlayerState == NULL)
			continue;

		if (item.hitboxexp) {
			if (LocalPawn)
			{
				uintptr_t LocalMesh = read<uintptr_t>(LocalPawn + 0x288);
				uintptr_t OtherMesh = read<uintptr_t>(entity.Actor + 0x288);
				write<Vector3>(OtherMesh + 0x134, Vector3(7, 7, 7));
				write<Vector3>(LocalMesh + 0x134, Vector3(1, 1, 1));
			}
	}

		localactorpos = read<Vector3>(Rootcomp + 0x11C);

		Vector3 bone66 = GetBoneWithRotation(entity.mesh, 98);
		Vector3 bone0 = GetBoneWithRotation(entity.mesh, 0);

		float distance = localactorpos.Distance(bone66) / 80.f;

		Vector3 top = ProjectWorldToScreen(bone66);
		Vector3 chest = ProjectWorldToScreen(bone66);
		Vector3 aimbotspot = ProjectWorldToScreen(bone66);
		Vector3 bottom = ProjectWorldToScreen(bone0);

		Vector3 Head = ProjectWorldToScreen(Vector3(bone66.x - 10, bone66.y, bone66.z + 15));

		Vector3 chestnone = GetBoneWithRotation(entity.mesh, 66);
		Vector3 chest1 = ProjectWorldToScreen(chestnone);
		Vector3 relativelocation = read<Vector3>(Rootcomp + 0x11C);

		Vector3 player_position = GetBoneWithRotation(entity.mesh, 0);
		Vector3 player_screen = ProjectWorldToScreen(player_position);

		Vector3 BoxHead = GetBoneWithRotation(CurrentActor, 98);
		Vector3 head_screen = ProjectWorldToScreen(Vector3(BoxHead.x, BoxHead.y - 0.6, BoxHead.z));
		//ImGui::GetOverlayDrawList()->AddText(ImVec2(head_screen.x, head_screen.z), ImGui::GetColorU32({ 1.f, 1.f, 1.f, 1.f }), std::to_string(curactorid).c_str());

		float BoxHeight = (float)(Head.y - bottom.y);
		float BoxWidth = BoxHeight / 1.8f;
		float CornerHeight = abs(Head.y - bottom.y);
		float CornerWidth = BoxHeight * 0.46;

		int MyTeamId = read<int>(PlayerState + 0xF50);
		int ActorTeamId = read<int>(otherPlayerState + 0xF50);
		int curactorid = read<int>(CurrentActor + 0x98);

		{
			auto SkelColor = ImColor(255, 0, 0, 255);
			auto BoxColor = ImColor(255, 0, 0, 255);
			auto NPC = Col.orange;

			if (isVisible(entity.mesh)) {
				SkelColor = ImColor(0, 255, 0, 255);
				BoxColor = ImColor(0, 255, 0, 255);
				NPC = Col.yellow;
			}
			else {
				SkelColor = ImColor(255, 0, 0, 255);
				BoxColor = ImColor(255, 0, 0, 255);
				NPC = Col.orange;
			}

			if (item.FovChanger)
			{
				uint64_t CamManager = read<uint64_t>(PlayerController + 0x2C8);
				write<float>(CamManager + 0x23C, item.FovValue); // + 0x4
			}
			
			if (item.WeakspotAim)
			{
				DrawString(16, WeakspotPorn.x, WeakspotPorn.y, &Col.peachred, false, true, "[ X ]");
			}

			if (MyTeamId != ActorTeamId)
			{
				if (item.Esp_box)
				{
					//DrawNormalBox(bottom.x - (CornerWidth / 2), Head.y, CornerWidth, CornerHeight, 1.f, &BoxColor);

					float Height = Head.y - bottom.y;

					if (Height < 0)
						Height = Height * (-1.f);

					float Width = Height * 0.75;

					Vector3 temp = Head;

					temp.x = temp.x - (Width / 2);

					Vector3 temp1 = temp;

					temp1.x = temp1.x + (Width);

					ImGui::GetOverlayDrawList()->AddRect(ImVec2(temp.x, temp.y), ImVec2(temp1.x, bottom.y), BoxColor, 0, 0, 0.5);
				}

				char dist[64];
				sprintf_s(dist, "%.f m", distance);

				if (item.Distance_Esp)
				{
					DrawString(16, Head.x - 8, Head.y - 45, &NPC, false, false, dist);
				}

				if (item.NameESP)
				{
					if (strstr(Names2.c_str(), ("BP_IOPlayerPawn_Base_C")))
					{
						DrawString(16, Head.x - 25, Head.y - 20, &NPC, false, false, "NPC / BOT");
					}
					else
					{
						DrawString(16, Head.x - 25, Head.y - 20, &NPC, false, false, "Real Player");
					}
				}

				if (item.Esp_line) {
					DrawLine((Width / 2), Height, player_screen.x, player_screen.y, &NPC, 1.f);
				}

				if (item.skeleton) {
					DrawSkeleton(entity.mesh, SkelColor);
				}

				if (item.WEAPON)
				{
					uint64_t weapon = read<uint64_t>(CurrentActor + 0x620);
					auto wdata = read<uint64_t>(weapon + 0x388);
					auto wtf = read<uint64_t>(wdata + 0x18);
					auto tier = read<BYTE>(wdata + 0x6C);
					auto names = GetNameFromFName(wtf);

					std::string Text;

					std::string null = ("");

					if (strstr(names.c_str(), ("WID_Pistol_CorePistol")))
						Text = null + ("Pistol");
					else if (strstr(names.c_str(), ("CoreSMG_Athena")))
						Text = null + ("Stinger SMG");
					else if (strstr(names.c_str(), ("CoreBurst_Athena")))
						Text = null + ("Striker Pump Shotgun");
					else if (strstr(names.c_str(), ("CoreDPS_Athena")))
						Text = null + ("Auto Shotgun");
					else if (strstr(names.c_str(), ("Athena_Medkit")))
						Text = null + ("Medkit");
					else if (strstr(names.c_str(), ("CoreAR_Athena")))
						Text = null + ("Assualt Rifle");
					else if (strstr(names.c_str(), ("AutoHigh_Athena")))
						Text = null + ("OG Assualt Rifle");
					else if (strstr(names.c_str(), ("Building")))
						Text = null + ("Building...");
					else if (strstr(names.c_str(), ("Sniper")))
						Text = null + ("Sniper Rifle");
					else if (strstr(names.c_str(), ("Flopper")))
						Text = null + ("Flopper");
					else if (strstr(names.c_str(), ("RedDot")))
						Text = null + ("Red Dot AR");
					else if (strstr(names.c_str(), ("Shotgun")))
						Text = null + ("Shotgun");
					else if (strstr(names.c_str(), ("Pistol_AutoHeavy")))
						Text = null + ("Auto SMG");
					else if (strstr(names.c_str(), ("Heavy_Athena")))
						Text = null + ("Heavy Shotgun");
					else if (strstr(names.c_str(), ("WestSausage")))
						Text = null + ("Spider Man");
					else if (strstr(names.c_str(), ("Charge_Athena")))
						Text = null + ("Charge Shotgun");

					ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());

					ImU32 Color;

					if (tier == 2)
					{
						Color = ImGui::GetColorU32({ 7.f / 255.f, 143.f / 255.f, 4.f / 255.f, 1 });
						DrawLString(14, bottom.x + 25, bottom.y + 25, Color, true, true, Text.c_str());
					}
					else if (tier == 3)
					{
						Color = ImGui::GetColorU32({ 0.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1 });
						DrawLString(14, bottom.x + 25, bottom.y + 25, Color, true, true, Text.c_str());
					}
					else if (tier == 4)
					{
						Color = ImGui::GetColorU32({ 112.f / 255.f, 7.f / 255.f, 135.f / 255.f, 1 });
						DrawLString(14, bottom.x + 25, bottom.y + 25, Color, true, true, Text.c_str());
					}
					else if (tier == 5)
					{
						Color = ImGui::GetColorU32({ 255.f / 255.f, 200.f / 255.f, 0.f / 255.f, 1 });
						DrawLString(14, bottom.x + 25, bottom.y + 25, Color, true, true, Text.c_str());
					}
					else if (tier == 6)
					{
						Color = ImGui::GetColorU32({ 255.f / 255.f, 150.f / 255.f, 11.f / 255.f, 1 });
						DrawLString(14, bottom.x + 25, bottom.y + 25, Color, true, true, Text.c_str());
					}
					else if (tier == 0 || tier == 1)
					{
						Color = ImGui::GetColorU32({ 168.f / 255.f, 168.f / 255.f, 168.f / 255.f, 1 });

						DrawLString(14, bottom.x + 25,bottom.y + 25, Color, true, true, Text.c_str());

					}
				}
			}
			if (MyTeamId != ActorTeamId)
			{
				if (isVisible(entity.mesh)) {
					if (item.Aimbot) {
						auto dx = aimbotspot.x - (Width / 2);
						auto dy = aimbotspot.y - (Height / 2);
						auto dist = sqrtf(dx * dx + dy * dy) / 50.0f;
						if (dist < item.AimFOV && dist < closestDistance) {
							closestDistance = dist;
							closestPawn = entity.Actor;
						}
					}

					if (item.StickySilent)
					{
						if (closestPawn && GetAsyncKeyState(item.aimkey)) {
							Vector3 VectorPos;
							uint64_t currentactormesh = read<uint64_t>(closestPawn + 0x288);
							auto rootHead = GetBoneWithRotation(currentactormesh, 98);
							VectorPos.x = rootHead.x - Camera.x;
							VectorPos.y = rootHead.y - Camera.y;
							VectorPos.z = rootHead.z - Camera.z;
							float distance = (double)(sqrtf(VectorPos.x * VectorPos.x + VectorPos.y * VectorPos.y + VectorPos.z * VectorPos.z));
							Vector3 rot;
							rot.x = -((acosf(VectorPos.z / distance) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510)) - 90.f);
							rot.y = atan2f(VectorPos.y, VectorPos.x) * (float)(180.0f / 3.14159265358979323846264338327950288419716939937510);
							rot.z = 0;
							SetControlRotation(rot, true);
						}
					}

					if (item.Aimbot) {
						if (closestPawn != 0) {
							if (item.Aimbot && closestPawn && GetAsyncKeyState(item.aimkey)) {
								AimAt(closestPawn);

								if (item.Dynamic_aim) {

									item.boneswitch += 1;
									if (item.boneswitch == 700) {
										item.boneswitch = 0;
									}

									if (item.boneswitch == 0) {
										item.hitboxpos = 0;
									}
									else if (item.boneswitch == 50) {
										item.hitboxpos = 1;
									}
									else if (item.boneswitch == 100) {
									}
									else if (item.boneswitch == 150) {
										item.hitboxpos = 3;
									}
									else if (item.boneswitch == 200) {
										item.hitboxpos = 4;
									}
									else if (item.boneswitch == 250) {
										item.hitboxpos = 5;
									}
									else if (item.boneswitch == 300) {
										item.hitboxpos = 6;
									}
									else if (item.boneswitch == 350) {
										item.hitboxpos = 7;
									}
									else if (item.boneswitch == 400) {
										item.hitboxpos = 6;
									}
									else if (item.boneswitch == 450) {
										item.hitboxpos = 5;
									}
									else if (item.boneswitch == 500) {
										item.hitboxpos = 4;
									}
									else if (item.boneswitch == 550) {
										item.hitboxpos = 3;
									}
									else if (item.boneswitch == 600) {
										item.hitboxpos = 2;
									}
									else if (item.boneswitch == 650) {
										item.hitboxpos = 1;



									}
								}
							}
							else {
								isaimbotting = false;
								AimAt2(closestPawn);
							}
						}
					}
				}
			}

			if (item.ospread)
			{
				uintptr_t CurrentWeapon = read<uintptr_t>(LocalPawn + 0x620); //CurrentWeapon Offset
				write<float>(CurrentWeapon + 0xA54, 0.f);

			}

			if (item.freezeplayer) {
				write<float>(LocalPawn + 0x9C, 0.0f);
			}
			else
			{
				write<float>(LocalPawn + 0x9C, 1.00f);
			}

			if (item.aiminair) {
				write<bool>(LocalPawn + 0x3F41, true);
			}

			if (item.gliderexploit && GetAsyncKeyState(VK_CONTROL)) {
				write<float>(LocalPawn + 0x14DE, 0.02f); //bIsParachuteOpen Offset
				write<float>(LocalPawn + 0x14DD, 0.02f); //bIsSkydiving  Offset
			}

			if (item.Rapidfire) {
				uintptr_t CurrentWeapon = read<uintptr_t>(LocalPawn + 0x620); //CurrentWeapon Offset
				write<bool>(CurrentWeapon + 0x17ac, true);
			}

			if (item.instant_res) {
				write<float>(LocalPawn + 0x37D8, 0.2f); //ReviveFromDBNOTime Offset
			}

			if (item.shield_esp)
			{
				Vector3 Coord;
				Vector3 rootPos, headPos;

				FLOAT fYaw = read<FLOAT>(PlayerController + 0x12C); //YAW in PLAYNIGGER

				FLOAT cY = cos(fYaw * (3.1415926 / 180.0f));
				FLOAT sY = sin(fYaw * (3.1415926 / 180.0f));

				FLOAT dX = rootPos.x - bone0.x;
				FLOAT dY = rootPos.y - bone0.y;

				Coord.x = (dY * cY - dX * sY) / 250.0f;
				Coord.y = (dX * cY + dY * sY) / 250.0f;

				Vector3 RetRadar;

				RetRadar.x = Coord.x + PosDx + (radarWidth / 2.0f);
				RetRadar.y = -Coord.y + PosDy + (radarWidth / 2.0f);

				if (RetRadar.x > (PosDx + radarWidth))
					RetRadar.x = (PosDx + radarWidth);
				else if (RetRadar.x < (PosDx))
					RetRadar.x = PosDx;

				if (RetRadar.y > (PosDy + radarWidth))
					RetRadar.y = (PosDy + radarWidth);
				else if (RetRadar.y < (PosDy))
					RetRadar.y = PosDy;

				DrawRect(RetRadar.x, RetRadar.y, 6, 6, &NPC, 1.f);
			}
		}
	}
	if (item.Loot || item.chest)
	{
		auto LootentityListCopy = LootentityList;

		for (unsigned long i = 0; i < LootentityListCopy.size(); ++i) {
			LootEntity entity = LootentityListCopy[i];
			if (LocalPawn)
			{
				uintptr_t ItemRootComponent = read<uintptr_t>(entity.CurrentActor + 0x138);
				Vector3 ItemPosition = read<Vector3>(ItemRootComponent + 0x11c);
				uintptr_t LocalRootComponent = read<uintptr_t>(LocalPawn + 0x138);
				Vector3 LocalRelativeLocation = read<Vector3>(LocalRootComponent + 0x11C);

				float ItemDist = LocalRelativeLocation.Distance(ItemPosition) / 100.f;
				//bool bAlreadySearched = rpm<bool>(entity.CurrentActor + 0xdf1);
				std::string null = ("");

				if (strstr(entity.name.c_str(), ("Bullet")) && item.NoSpread)
				{
					if (ItemDist < 250.f) {
						Vector3 ChestPosition;

						uintptr_t AItemRootComponent = read<uintptr_t>(entity.CurrentActor + 0x138);
						Vector3 AItemPosition = read<Vector3>(AItemRootComponent + 0x11c);

						ChestPosition = ProjectWorldToScreen(AItemPosition);
						// SDK::Classes::AController::WorldToScreen(ItemPosition, &ChestPosition);
						std::string Text;


						Text = null + ("Bullet [") + std::to_string((int)ItemDist) + ("m]");


						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());



						//ImGui::GetOverlayDrawList()->AddText(ImVec2(ChestPosition.x - TextSize.x / 2, ChestPosition.y - TextSize.y / 2), SettingsColor::ChestESP, Text.c_str());
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.red, true, true, Text.c_str());



					}
				}

				if (strstr(entity.name.c_str(), ("Vehicle")) && item.vehicless)
				{
					if (ItemDist < 250.f) {
						Vector3 ChestPosition;

						uintptr_t AItemRootComponent = read<uintptr_t>(entity.CurrentActor + 0x138);
						Vector3 AItemPosition = read<Vector3>(AItemRootComponent + 0x11c);

						ChestPosition = ProjectWorldToScreen(AItemPosition);
						// SDK::Classes::AController::WorldToScreen(ItemPosition, &ChestPosition);
						std::string Text;


						Text = null + ("Vehicle [") + std::to_string((int)ItemDist) + ("m]");


						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());



						//ImGui::GetOverlayDrawList()->AddText(ImVec2(ChestPosition.x - TextSize.x / 2, ChestPosition.y - TextSize.y / 2), SettingsColor::ChestESP, Text.c_str());
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.lightblue, true, true, Text.c_str());

					}
				}

				if (strstr(entity.name.c_str(), ("Tiered_Chest")) && item.chest)
				{
					//std::cout << ItemDist;
					//printf("\n");

					if (ItemDist < 250.f) {
						Vector3 ChestPosition;

						uintptr_t AItemRootComponent = read<uintptr_t>(entity.CurrentActor + 0x138);
						Vector3 AItemPosition = read<Vector3>(AItemRootComponent + 0x11c);

						ChestPosition = ProjectWorldToScreen(AItemPosition);
						// SDK::Classes::AController::WorldToScreen(ItemPosition, &ChestPosition);
						std::string Text;


						Text = null + ("Chest [") + std::to_string((int)ItemDist) + ("m]");


						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());



						//ImGui::GetOverlayDrawList()->AddText(ImVec2(ChestPosition.x - TextSize.x / 2, ChestPosition.y - TextSize.y / 2), SettingsColor::ChestESP, Text.c_str());
						DrawString(14, ChestPosition.x, ChestPosition.y, &Col.yellow, true, true, Text.c_str());

					}
				}
				else if (strstr(entity.name.c_str(), "FortPickupAthena") && item.Loot)
				{

					BYTE tier;
					if (ItemDist < 250.f) {


						auto definition = read<uint64_t>(entity.CurrentActor + 0x2A8 + 0x18);
						auto wtf = read<uint64_t>(definition + 0x18);

						auto names = GetNameFromFName(wtf);

						tier = read<BYTE>(definition + 0x6C);

						ImU32 Color;
						RGBA RGBAColor;

						uintptr_t AItemRootComponent = read<uintptr_t>(entity.CurrentActor + 0x138);
						Vector3 AItemPosition = read<Vector3>(AItemRootComponent + 0x11c);

						Vector3 ChestPosition;
						ChestPosition = ProjectWorldToScreen(AItemPosition);
						std::string Text;

						if (strstr(names.c_str(), ("WID_Pistol_CorePistol")))
							Text = null + ("Pistol");
						else if (strstr(names.c_str(), ("CoreSMG_Athena")))
							Text = null + ("Stinger SMG");
						else if (strstr(names.c_str(), ("CoreBurst_Athena")))
							Text = null + ("Striker Pump Shotgun");
						else if (strstr(names.c_str(), ("CoreDPS_Athena")))
							Text = null + ("Auto Shotgun");
						else if (strstr(names.c_str(), ("Athena_Medkit")))
							Text = null + ("Medkit");
						else if (strstr(names.c_str(), ("CoreAR_Athena")))
							Text = null + ("Assualt Rifle");
						else if (strstr(names.c_str(), ("AutoHigh_Athena")))
							Text = null + ("OG Assualt Rifle");
						else if (strstr(names.c_str(), ("Building")))
							Text = null + ("Building...");
						else if (strstr(names.c_str(), ("Sniper")))
							Text = null + ("Sniper Rifle");
						else if (strstr(names.c_str(), ("Flopper")))
							Text = null + ("Flopper");
						else if (strstr(names.c_str(), ("RedDot")))
							Text = null + ("Red Dot AR");
						else if (strstr(names.c_str(), ("Shotgun")))
							Text = null + ("Shotgun");
						else if (strstr(names.c_str(), ("Pistol_AutoHeavy")))
							Text = null + ("Auto SMG");
						else if (strstr(names.c_str(), ("Heavy_Athena")))
							Text = null + ("Heavy Shotgun");
						else if (strstr(names.c_str(), ("WestSausage")))
							Text = null + ("Spider Man");
						else if (strstr(names.c_str(), ("Charge_Athena")))
							Text = null + ("Charge Shotgun");
						else
							//Text = names.c_str();
							continue;



						std::string wtf2 = Text + " [" + std::to_string((int)ItemDist) + ("m]");

						ImVec2 TextSize = ImGui::CalcTextSize(Text.c_str());


						// ImGui::GetOverlayDrawList()->AddText(ImVec2(ChestPosition.x - TextSize.x / 2, ChestPosition.y - TextSize.y / 2), Color, Text.c_str());



						if (item.Loot)
						{
							if (tier == 2)
							{
								Color = ImGui::GetColorU32({ 7.f / 255.f, 143.f / 255.f, 4.f / 255.f, 1 });
								DrawLString(14, ChestPosition.x, ChestPosition.y, Color, true, true, wtf2.c_str());
							}
							else if (tier == 3)
							{
								Color = ImGui::GetColorU32({ 0.f / 255.f, 255.f / 255.f, 255.f / 255.f, 1 });
								DrawLString(14, ChestPosition.x, ChestPosition.y, Color, true, true, wtf2.c_str());
							}
							else if (tier == 4)
							{
								Color = ImGui::GetColorU32({ 112.f / 255.f, 7.f / 255.f, 135.f / 255.f, 1 });
								DrawLString(14, ChestPosition.x, ChestPosition.y, Color, true, true, wtf2.c_str());
							}
							else if (tier == 5)
							{
								Color = ImGui::GetColorU32({ 255.f / 255.f, 200.f / 255.f, 0.f / 255.f, 1 });
								DrawLString(14, ChestPosition.x, ChestPosition.y, Color, true, true, wtf2.c_str());
							}
							else if (tier == 6)
							{
								Color = ImGui::GetColorU32({ 255.f / 255.f, 150.f / 255.f, 11.f / 255.f, 1 });
								DrawLString(14, ChestPosition.x, ChestPosition.y, Color, true, true, wtf2.c_str());
							}
							else if (tier == 0 || tier == 1)
							{
								Color = ImGui::GetColorU32({ 168.f / 255.f, 168.f / 255.f, 168.f / 255.f, 1 });

								DrawLString(14, ChestPosition.x, ChestPosition.y, Color, true, true, wtf2.c_str());

							}
						}
					}
				}
			}

		}
	}
}

void GetKey() {
	if (item.hitboxpos == 0) {
		item.hitbox = 98;
	}
	else if (item.hitboxpos == 1) {
		item.hitbox = 66;
	}
	else if (item.hitboxpos == 2) {
		item.hitbox = 5;
	}

	else if (item.hitboxpos == 3) {
		item.hitbox = 2;
	}


	DrawESP();
}

static int Tab = 0;

void shortcurts()
{
	if (Key.IsKeyPushing(VK_INSERT))
	{
		if (menu_key == false)
		{
			menu_key = true;
		}
		else if (menu_key == true)
		{
			menu_key = false;
		}
		Sleep(200);
	}
}


float color_red = 1.;
float color_green = 0;
float color_blue = 0;
float color_random = 0.0;
float color_speed = -10.0;

void ColorChange()
{
	static float Color[3];
	static DWORD Tickcount = 0;
	static DWORD Tickcheck = 0;
	ImGui::ColorConvertRGBtoHSV(color_red, color_green, color_blue, Color[0], Color[1], Color[2]);
	if (GetTickCount() - Tickcount >= 1)
	{
		if (Tickcheck != Tickcount)
		{
			Color[0] += 0.001f * color_speed;
			Tickcheck = Tickcount;
		}
		Tickcount = GetTickCount();
	}
	if (Color[0] < 0.0f) Color[0] += 1.0f;
	ImGui::ColorConvertHSVtoRGB(Color[0], Color[1], Color[2], color_red, color_green, color_blue);
}

ImGuiStyle* dst;
ImFont* icons = nullptr;
ImFont* big = nullptr;
ImFont* bigger = nullptr;
ImFont* g_font = nullptr;

bool TabA(const char* icon, const char* label, const ImVec2& size_arg, const bool selected)
{

	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	static float sizeplus = 0.f;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

	auto animcolor3 = ImColor(27, 27, 27, 255);
	auto animcolor2 = ImColor(3, 169, 252, 255);
	auto animcolor = ImColor(24, 24, 24, 255);

	window->DrawList->AddRectFilled({ bb.Min.x,bb.Max.y }, { bb.Max.x,bb.Min.y }, animcolor);

	if (selected)
		window->DrawList->AddRectFilled({ bb.Min.x,bb.Max.y }, { bb.Min.x + 2,bb.Min.y }, animcolor2);

	if (selected)
		window->DrawList->AddRectFilled({ bb.Min.x + 2,bb.Max.y }, { bb.Min.x + 148,bb.Min.y }, animcolor3);

	ImGui::PushFont(big);

	if (selected)
		window->DrawList->AddText({ bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2 }, ImColor(225 / 255.f, 225 / 255.f, 225 / 255.f, 255.f / 255.f), label);

	if (!selected)
		window->DrawList->AddText({ bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2 }, ImColor(75 / 255.f, 75 / 255.f, 75 / 255.f, 255.f / 255.f), label);


	ImGui::PopFont();

	return pressed;
}

bool SubTab(const char* label, const ImVec2& size_arg, const bool selected)
{
	ImGuiWindow* window = ImGui::GetCurrentWindow();
	if (window->SkipItems)
		return false;

	static float sizeplus = 0.f;

	ImGuiContext& g = *GImGui;
	const ImGuiStyle& style = g.Style;
	const ImGuiID id = window->GetID(label);
	const ImVec2 label_size = ImGui::CalcTextSize(label, NULL, true);

	ImVec2 pos = window->DC.CursorPos;

	ImVec2 size = ImGui::CalcItemSize(size_arg, label_size.x + style.FramePadding.x * 2.0f, label_size.y + style.FramePadding.y * 2.0f);

	const ImRect bb(pos, ImVec2(pos.x + size.x, pos.y + size.y));
	ImGui::ItemSize(size, style.FramePadding.y);
	if (!ImGui::ItemAdd(bb, id))
		return false;

	bool hovered, held;
	bool pressed = ImGui::ButtonBehavior(bb, id, &hovered, &held, 0);

	auto animcolor = ImColor(3, 169, 252, 255);

	window->DrawList->AddRectFilled({ bb.Max.x,bb.Max.y }, { bb.Min.x,bb.Max.y - 2 }, ImGui::GetColorU32(ImGuiCol_Border));

	if (selected)
		window->DrawList->AddRectFilled({ bb.Max.x,bb.Max.y }, { bb.Min.x,bb.Max.y - 2 }, animcolor);

	if (selected) {
		window->DrawList->AddText({ bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2 }, ImColor(255 / 255.f, 255 / 255.f, 255 / 255.f, 255.f / 255.f), label);
	}
	else {
		window->DrawList->AddText({ bb.Min.x + size_arg.x / 2 - ImGui::CalcTextSize(label).x / 2,bb.Min.y + size_arg.y / 2 - ImGui::CalcTextSize(label).y / 2 }, ImColor(200 / 255.f, 200 / 255.f, 200 / 255.f, 255.f / 255.f), label);
	}


	return pressed;
}

namespace L
{
	typedef struct
	{
		DWORD R;
		DWORD G;
		DWORD B;
		DWORD A;
	}RGBA;

	class RBGAColr
	{
	public:
		RGBA red = { 255,0,0,255 };
		RGBA Magenta = { 255,0,255,255 };
		RGBA yellow = { 255,255,0,255 };
		RGBA grayblue = { 128,128,255,255 };
		RGBA green = { 128,224,0,255 };
		RGBA darkgreen = { 0,224,128,255 };
		RGBA brown = { 192,96,0,255 };
		RGBA pink = { 255,168,255,255 };
		RGBA DarkYellow = { 216,216,0,255 };
		RGBA SilverWhite = { 236,236,236,255 };
		RGBA purple = { 128,0,0,255 };
		RGBA Navy = { 88,48,224,255 };
		RGBA skyblue = { 0,136,255,255 };
		RGBA graygreen = { 128,160,128,255 };
		RGBA blue = { 0,96,192,255 };
		RGBA orange = { 255,128,0,255 };
		RGBA peachred = { 255,80,128,255 };
		RGBA reds = { 255,128,192,255 };
		RGBA darkgray = { 96,96,96,255 };
		RGBA Navys = { 0,0,128,255 };
		RGBA darkgreens = { 0,128,0,255 };
		RGBA darkblue = { 0,128,128,255 };
		RGBA redbrown = { 128,0,0,255 };
		RGBA purplered = { 128,0,0,255 };
		RGBA greens = { 0,255,0,255 };
		RGBA envy = { 0,255,255,255 };
		RGBA black = { 0,0,0,255 };
		RGBA gray = { 128,128,128,255 };
		RGBA white = { 255,255,255,255 };
		RGBA blues = { 30,144,255,255 };
		RGBA lightblue = { 135,206,250,160 };
		RGBA Scarlet = { 220, 20, 60, 160 };
		RGBA white_ = { 255,255,255,200 };
		RGBA gray_ = { 128,128,128,200 };
		RGBA black_ = { 0,0,0,200 };
		RGBA red_ = { 255,0,0,200 };
		RGBA Magenta_ = { 255,0,255,200 };
		RGBA yellow_ = { 255,255,0,200 };
		RGBA grayblue_ = { 128,128,255,200 };
		RGBA green_ = { 128,224,0,200 };
		RGBA darkgreen_ = { 0,224,128,200 };
		RGBA brown_ = { 192,96,0,200 };
		RGBA pink_ = { 255,168,255,200 };
		RGBA darkyellow_ = { 216,216,0,200 };
		RGBA silverwhite_ = { 236,236,236,200 };
		RGBA purple_ = { 128,0,0,255 };
		RGBA Blue_ = { 88,48,224,200 };
		RGBA skyblue_ = { 0,136,255,200 };
		RGBA graygreen_ = { 128,160,128,200 };
		RGBA blue_ = { 0,96,192,200 };
		RGBA orange_ = { 255,128,0,200 };
		RGBA pinks_ = { 255,80,128,200 };
		RGBA Fuhong_ = { 255,128,192,200 };
		RGBA darkgray_ = { 96,96,96,200 };
		RGBA Navy_ = { 0,0,128,200 };
		RGBA darkgreens_ = { 0,128,0,200 };
		RGBA darkblue_ = { 0,128,128,200 };
		RGBA redbrown_ = { 128,0,0,200 };
		RGBA purplered_ = { 128,0,0,255 };
		RGBA greens_ = { 0,255,0,200 };
		RGBA envy_ = { 0,255,255,200 };
		RGBA glassblack = { 0, 0, 0, 160 };
		RGBA GlassBlue = { 65,105,225,80 };
		RGBA glassyellow = { 255,255,0,160 };
		RGBA glass = { 200,200,200,60 };
		RGBA Plum = { 221,160,221,160 };
		RGBA neongreen = { 0, 255, 229,0 };

	};
	RBGAColr Color;

	std::string string_To_UTF8(const std::string& str)
	{
		int nwLen = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, NULL, 0);
		wchar_t* pwBuf = new wchar_t[nwLen + 1];
		ZeroMemory(pwBuf, nwLen * 2 + 2);
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), str.length(), pwBuf, nwLen);
		int nLen = ::WideCharToMultiByte(CP_UTF8, 0, pwBuf, -1, NULL, NULL, NULL, NULL);
		char* pBuf = new char[nLen + 1];
		ZeroMemory(pBuf, nLen + 1);
		::WideCharToMultiByte(CP_UTF8, 0, pwBuf, nwLen, pBuf, nLen, NULL, NULL);
		std::string retStr(pBuf);
		delete[]pwBuf;
		delete[]pBuf;
		pwBuf = NULL;
		pBuf = NULL;
		return retStr;
	}
	// Drawings for custom menus + Toggle Button's / Checkboxes ETC
	void K2_text(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
	}
	void K2_customtext(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 240)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImColor(0, 0, 0, 240)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
	}
	void OutlinedRBGText(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 30 / 30.0)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 2), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 30 / 30.0)), utf_8_2.c_str());
		//ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), utf_8_2.c_str());
	}
	void RegularText(int x, int y, RGBA* color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), utf_8_2.c_str());
	}
	void NeonRBGText(int x, int y, ImColor color, const char* str)
	{
		ImFont a;
		std::string utf_8_1 = std::string(str);
		std::string utf_8_2 = string_To_UTF8(utf_8_1);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y - 1), ImColor(64, 255, 0), utf_8_2.c_str());
		ImGui::GetOverlayDrawList()->AddText(ImVec2(x + 1, y + 1), ImColor(64, 255, 0), utf_8_2.c_str());
		//ImGui::GetOverlayDrawList()->AddText(ImVec2(x, y + 1), ImGui::ColorConvertFloat4ToU32(ImVec4(1 / 255.0, 1 / 255.0, 1 / 255.0, 255 / 255.0)), utf_8_2.c_str());
	}
	void BackgroundRBGOutline(int x, int y, int w, int h, ImColor color, int thickness)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0, thickness);
	}
	void BackgroundOutline(int x, int y, int w, int h, RGBA* color, int thickness)
	{
		ImGui::GetOverlayDrawList()->AddRect(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0, thickness);
	}
	void TitleBar(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), 0, 0);
	}
	void TitleBarGraident(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), (color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0), 0, 0);
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), (color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0), 0, 0);
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), (color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0), 0, 0);
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0)), (color->R / 255.0, color->G / 255.0, color->B / 255.0, color->A / 255.0), 0, 0);
	}
	void BackgroundLowOpacity(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 0.20f)), 0);
	}

	void BackgroundLowRounded(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 0.20f)), 6);
	}

	void BackgroundRBGGradient(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void K2_drawfilled(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color)), 0, 0);
	}

	void SelectedFilled(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 0.40f)), 0, 0);
	}

	void BackgroundFilled(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 1.00f)), 0);
	}

	void BackgroundFilledRound(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 1.00f)), 6);
	}

	void BackgroundGradient(int x, int y, int w, int h, RGBA* color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilledMultiColor(ImVec2(x, y), ImVec2(x + w, y + h), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 1.00f)), ImGui::ColorConvertFloat4ToU32(ImVec4(color->R / 255.0, color->G / 255.0, color->B / 255.0, 1.00f)), 0, 0);
	}
	void RoundedRect(int x, int y, int w, int h, ImColor color)
	{
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(x, y), ImVec2(x + w, y + h), ImColor(color), 2);
	}
}


void render() {

	ImGuiIO& io = ImGui::GetIO();

	io.IniFilename = nullptr;

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	DrawESP();

	if (item.drawfov_circle) {
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(Width / 2, Height / 2), float(item.AimFOV), ImGui::GetColorU32({ 255, 0, 0, 255.f }), 65);
	}

	if (item.cross_hair) {
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 - 11, Height / 2), ImVec2(Width / 2 + 1, Height / 2), ImGui::GetColorU32({ 255, 0, 0, 255.f }), 1.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2 + 12, Height / 2), ImVec2(Width / 2 + 1, Height / 2), ImGui::GetColorU32({ 255, 0, 0, 255.f }), 1.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2 - 11), ImVec2(Width / 2, Height / 2), ImGui::GetColorU32({ 255, 0, 0, 255.f }), 1.0f);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(Width / 2, Height / 2 + 12), ImVec2(Width / 2, Height / 2), ImGui::GetColorU32({ 255, 0, 0, 255.f }), 1.0f);
	}

	shortcurts();

	if (menu_key)
	{
		ImGui::StyleColorsDark();

		ImVec4* colors = ImGui::GetStyle().Colors;
		colors[ImGuiCol_WindowBg] = ImColor(25, 25, 25, 255);
		colors[ImGuiCol_ButtonHovered] = ImColor(128, 0, 0, 255);
		colors[ImGuiCol_ButtonActive] = ImColor(128, 0, 0, 255);
		colors[ImGuiCol_Button] = ImColor(128, 0, 0, 255);
		colors[ImGuiCol_FrameBgActive] = ImColor(25, 25, 25, 255);
		colors[ImGuiCol_FrameBg] = ImColor(25, 25, 25, 255);
		colors[ImGuiCol_CheckMark] = ImColor(128, 0, 0, 255);
		colors[ImGuiCol_Text] = ImColor(255, 255, 255);
		colors[ImGuiCol_Border] = ImColor(25, 25, 25, 255);
		colors[ImGuiCol_SliderGrab] = ImColor(128, 0, 0, 255);
		colors[ImGuiCol_SliderGrabActive] = ImColor(128, 0, 0, 255);

		ImGuiStyle* style = &ImGui::GetStyle();
		style->WindowPadding = ImVec2(15, 15);
		style->WindowRounding = 0.0f;
		style->FramePadding = ImVec2(2, 2);
		style->ItemSpacing = ImVec2(8, 8);
		style->ItemInnerSpacing = ImVec2(8, 6);
		style->IndentSpacing = 25.0f;
		style->ScrollbarSize = 15.0f;
		style->ScrollbarRounding = 0.0f;
		style->GrabMinSize = 10.0f;
		style->WindowBorderSize = 20.0f;
		style->WindowTitleAlign.x = 0.50f;
		style->FrameRounding = 0.0f;

		ImGui::SetNextWindowSize({ 530, 500 });
		ImGui::Begin("#Fortnite", NULL, ImGuiWindowFlags_::ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_::ImGuiWindowFlags_NoResize | ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);

		int x = 30;
		int y = 20;
		ImVec2 p = ImGui::GetWindowPos();

		L::K2_drawfilled(p.x, p.y, 195, 500, ImColor(37, 37, 37, 255));
		L::K2_text(p.x + 67, p.y + 10, ImColor(255, 255, 255), ("Aero GG"));


		L::K2_customtext(p.x + 10, p.y + 50, ImColor(255, 255, 255, 255), ("AIMBOT"));
		L::K2_customtext(p.x + 10, p.y + 89, ImColor(255, 255, 255, 255), ("VISUALS"));
		L::K2_customtext(p.x + 10, p.y + 128, ImColor(255, 255, 255, 255), ("MODS"));
		L::K2_customtext(p.x + 10, p.y + 167, ImColor(255, 255, 255, 255), ("MISC"));

		static int fortnitetab;
		ImGuiStyle* Style = &ImGui::GetStyle();

		ImGui::SetCursorPos({ 10, 44 });
		if (ImGui::Button(("##Aimbot_TAB"), ImVec2(110, 28))) fortnitetab = 1;

		if (ImGui::Button(("##Visuals_TAB"), ImVec2(110, 28))) fortnitetab = 2;

		ImGui::SetCursorPos({ 1, 120 });
			if (ImGui::Button(("##Mods_TAB"), ImVec2(110, 28))) fortnitetab = 3;

		ImGui::SetCursorPos({ 1, 160 });
		if (ImGui::Button(("##Misc_TAB"), ImVec2(110, 28))) fortnitetab = 4;

		if (fortnitetab == 0)
		{
			L::K2_customtext(p.x + 213, p.y + 2, ImColor(255, 255, 255, 255), ("Enable options only ingame or you will crash"));
		}
		if (fortnitetab == 1)
		{
			ImGui::SetCursorPos({ 213, 68 });
			L::K2_drawfilled(p.x + 0, p.y + 39, 195, 40, ImColor(27, 27, 27, 255));

			L::K2_customtext(p.x + 10, p.y + 50, ImColor(128, 0, 0, 255), ("AIMBOT"));

			ImGui::SetCursorPos({ 213, 35 });
			L::K2_customtext(p.x + 457, p.y + 35, ImColor(255, 255, 255, 255), ("[F1]"));

			ImGui::Checkbox(("Memory Aim"), &item.Aimbot);

			ImGui::SetCursorPos({ 213, 68 });
			L::K2_customtext(p.x + 457, p.y + 68, ImColor(255, 255, 255, 255), ("[F2]"));
			ImGui::Checkbox(("Weakspot Aim    "), &item.WeakspotAim);

			ImGui::TextColored(ImColor(0, 0, 0), "Aim Key: ");
			HotkeyButton(item.aimkey, ChangeKey, keystatus);

			ImGui::SetCursorPos({ 213, 101 });

			ImGui::Checkbox(("Draw fovcircle"), &item.drawfov_circle);

			ImGui::SetCursorPos({ 213, 167 });
			//L::K2_customtext(p.x + 447, p.y + 150, ImColor(255, 255, 255), FOVAMT.c_str());
			L::K2_customtext(p.x + 213, p.y + 150, ImColor(255, 255, 255, 255), ("RADIUS"));
			ImGui::SliderFloat(("##FovCircle_Radius"), &item.AimFOV, 50.0f, 500.0f, "%.3f", 1.f);

			ImGui::SetCursorPos({ 213, 210 });
			//L::K2_customtext(p.x + 447, p.y + 190, ImColor(255, 255, 255), SMTH.c_str());
			L::K2_customtext(p.x + 213, p.y + 190, ImColor(255, 255, 255, 255), ("SMOOTH"));
			ImGui::SliderFloat(("##Smooth"), &item.Smooth, 1.0f, 15.0f, "%.3f", 1.f);

			L::K2_customtext(p.x + 213, p.y + 2, ImColor(255, 255, 255, 255), ("Aimbot"));



		}
		if (fortnitetab == 2)
		{
			L::K2_drawfilled(p.x + 0, p.y + 78, 195, 40, ImColor(27, 27, 27, 255));
			L::K2_customtext(p.x + 10, p.y + 89, ImColor(128, 0, 0, 255), ("PLAYER VISUALS"));
			L::K2_customtext(p.x + 213, p.y + 2, ImColor(255, 255, 255, 255), ("Visuals"));

			ImGui::SetCursorPos({ 213, 35 });
			ImGui::Checkbox(("Draw boxes"), &item.Esp_box);

			ImGui::SetCursorPos({ 213, 68 });
			ImGui::Checkbox(("Draw lines"), &item.Esp_line);

			ImGui::SetCursorPos({ 213, 101 });
			ImGui::Checkbox(("Draw skeletons"), &item.skeleton);

			ImGui::SetCursorPos({ 213, 134 });
			ImGui::Checkbox(("Draw crosshair"), &item.cross_hair);

			ImGui::SetCursorPos({ 213, 167 });
			ImGui::Checkbox(("Draw distance"), &item.Distance_Esp);

			ImGui::SetCursorPos({ 213, 200 });
			ImGui::Checkbox(("Draw radar"), &item.shield_esp);

			ImGui::SetCursorPos({ 213, 233 });
			ImGui::Checkbox(("Draw Loot"), &item.Loot);

			ImGui::SetCursorPos({ 213, 266 });
			ImGui::Checkbox(("Draw Chest"), &item.chest);

			ImGui::SetCursorPos({ 213, 299 });
			ImGui::Checkbox(("Draw Vehicle"), &item.vehicless);

			ImGui::SetCursorPos({ 213, 333 });
			ImGui::Checkbox(("Draw Bullets"), &item.NoSpread);
		}

		if (fortnitetab == 3)
		{
			L::K2_drawfilled(p.x + 0, p.y + 117, 195, 40, ImColor(27, 27, 27, 255));
			L::K2_customtext(p.x + 10, p.y + 128, ImColor(128, 0, 0, 255), ("MODS"));
			L::K2_customtext(p.x + 213, p.y + 2, ImColor(255, 255, 255, 255), ("Mods"));

			ImGui::SetCursorPos({ 213, 35 });
			ImGui::Checkbox(("No weapon spread (F5) (might be unsafe unless its nobloom only)"), &item.ospread);

			ImGui::SetCursorPos({ 213, 68 });
			ImGui::Checkbox(("Instant revive (E key) (might be unsafe)"), &item.instant_res);


			ImGui::SetCursorPos({ 213, 101 });
			ImGui::Checkbox(("Aim While Jumping (might be unsafe)"), &item.aiminair);

			ImGui::SetCursorPos({ 213, 134 });
			ImGui::Checkbox(("HitBox Extender (might be unsafe)"), &item.hitboxexp);

			ImGui::SetCursorPos({ 213, 167 });
			ImGui::Checkbox(("Player Fly (risk!!)"), &item.playerfly);

			ImGui::SetCursorPos({ 213, 210 });
			ImGui::Checkbox(("Camera Fov Changer (risk!!)"), &item.FovChanger);

			ImGui::SetCursorPos({ 213, 250 });
			L::K2_customtext(p.x + 213, p.y + 190, ImColor(255, 255, 255, 255), ("Cam Fov Slider"));
			ImGui::SliderFloat(("##FOVCHANGERRR"), &item.FovValue, 0.0f, 160.0f, "%.3f", 1.f);
		}
		if (fortnitetab == 4)
		{
			L::K2_drawfilled(p.x + 0, p.y + 156, 195, 40, ImColor(27, 27, 27, 255));
			L::K2_customtext(p.x + 10, p.y + 167, ImColor(128, 0, 0, 255), ("MISC"));
			L::K2_customtext(p.x + 213, p.y + 2, ImColor(255, 255, 255, 255), ("Made with love by Klixx and Mambaz"));
			L::K2_customtext(p.x + 213, p.y + 485, ImColor(255, 255, 255, 255), ("anticheat screenshot cleaned: 0"));
		}


		ImGui::End();
	}

	//Menu();

	ImGui::EndFrame();
	p_Device->SetRenderState(D3DRS_ZENABLE, false);
	p_Device->SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	p_Device->SetRenderState(D3DRS_SCISSORTESTENABLE, false);
	p_Device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
	if (p_Device->BeginScene() >= 0)
	{
		ImGui::Render();
		ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
		p_Device->EndScene();
	}
	HRESULT result = p_Device->Present(NULL, NULL, NULL, NULL);

	if (result == D3DERR_DEVICELOST && p_Device->TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
	{
		ImGui_ImplDX9_InvalidateDeviceObjects();
		p_Device->Reset(&p_Params);
		ImGui_ImplDX9_CreateDeviceObjects();
	}
}

WPARAM MainLoop()
{
	static RECT old_rc;
	ZeroMemory(&Message, sizeof(MSG));

	while (Message.message != WM_QUIT)
	{
		if (PeekMessage(&Message, MyWnd, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessage(&Message);
		}

		HWND hwnd_active = GetForegroundWindow();
		if (GetAsyncKeyState(0x23) & 1)
			exit(8);

		if (hwnd_active == GameWnd) {
			HWND hwndtest = GetWindow(hwnd_active, GW_HWNDPREV);
			SetWindowPos(MyWnd, hwndtest, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
		RECT rc;
		POINT xy;

		ZeroMemory(&rc, sizeof(RECT));
		ZeroMemory(&xy, sizeof(POINT));
		GetClientRect(GameWnd, &rc);
		ClientToScreen(GameWnd, &xy);
		rc.left = xy.x;
		rc.top = xy.y;

		ImGuiIO& io = ImGui::GetIO();
		io.ImeWindowHandle = GameWnd;
		io.DeltaTime = 1.0f / 60.0f;

		POINT p;
		GetCursorPos(&p);
		io.MousePos.x = p.x - xy.x;
		io.MousePos.y = p.y - xy.y;

		if (GetAsyncKeyState(0x1)) {
			io.MouseDown[0] = true;
			io.MouseClicked[0] = true;
			io.MouseClickedPos[0].x = io.MousePos.x;
			io.MouseClickedPos[0].x = io.MousePos.y;
		}
		else
			io.MouseDown[0] = false;
		if (rc.left != old_rc.left || rc.right != old_rc.right || rc.top != old_rc.top || rc.bottom != old_rc.bottom)
		{

			old_rc = rc;

			Width = rc.right;
			Height = rc.bottom;

			p_Params.BackBufferWidth = Width;
			p_Params.BackBufferHeight = Height;
			SetWindowPos(MyWnd, (HWND)0, xy.x, xy.y, Width, Height, SWP_NOREDRAW);
			p_Device->Reset(&p_Params);
		}
		render();
	}
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();



	CleanuoD3D();
	DestroyWindow(MyWnd);

	return Message.wParam;
}
LRESULT CALLBACK WinProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, Message, wParam, lParam))
		return true;

	switch (Message)
	{
	case WM_DESTROY:
		CleanuoD3D();
		PostQuitMessage(0);
		exit(4);
		break;
	case WM_SIZE:
		if (p_Device != NULL && wParam != SIZE_MINIMIZED)
		{
			ImGui_ImplDX9_InvalidateDeviceObjects();
			p_Params.BackBufferWidth = LOWORD(lParam);
			p_Params.BackBufferHeight = HIWORD(lParam);
			HRESULT hr = p_Device->Reset(&p_Params);
			if (hr == D3DERR_INVALIDCALL)
				IM_ASSERT(0);
			ImGui_ImplDX9_CreateDeviceObjects();
		}
		break;
	default:
		return DefWindowProc(hWnd, Message, wParam, lParam);
		break;
	}
	return 0;
}

void CleanuoD3D()
{
	if (p_Device != NULL)
	{
		p_Device->EndScene();
		p_Device->Release();
	}
	if (p_Object != NULL)
	{
		p_Object->Release();
	}
}
void SetWindowToTarget()
{
	while (true)
	{
		GameWnd = get_process_wnd(sdk::process_id);
		if (GameWnd)
		{
			ZeroMemory(&GameRect, sizeof(GameRect));
			GetWindowRect(GameWnd, &GameRect);
			Width = GameRect.right - GameRect.left;
			Height = GameRect.bottom - GameRect.top;
			DWORD dwStyle = GetWindowLong(GameWnd, GWL_STYLE);
			if (dwStyle & WS_BORDER)
			{
				GameRect.top += 32;
				Height -= 39;
			}
			ScreenCenterX = Width / 2;
			ScreenCenterY = Height / 2;
			MoveWindow(MyWnd, GameRect.left, GameRect.top, Width, Height, true);
		}
	}
}


int main()
{
	manyo();
	
	if (driver->Init(FALSE)) {
		printf(("Success!\n"));
		Sleep(1000);
		driver->Attach((L"FortniteClient-Win64-Shipping.exe"));

		SetupWindow();

		DirectXInit(MyWnd);

		sdk::process_id = driver->GetProcessId((L"FortniteClient-Win64-Shipping.exe"));
		sdk::module_base = driver->GetModuleBase((L"FortniteClient-Win64-Shipping.exe"));

		printf(("FortniteClient-Win64-Shipping.exe :0x%llX\n"), sdk::module_base);

		HANDLE handleA = CreateThread(nullptr, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(drawlootloop), nullptr, NULL, nullptr);

		MainLoop();
	}
	return 0;
	printf(("Failed!\n"));
	system(("pause"));
	return 1;
}