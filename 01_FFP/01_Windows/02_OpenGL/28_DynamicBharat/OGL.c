#include<Windows.h> // Windows header files 
#include "OGL.h"
#include<stdlib.h> // For exit
#include<stdio.h> // For file IO (input output)
#include<math.h>

// OpenGL header files
#include<gl/GL.h>
#include<gl/GLU.h>

// MACROS

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Link with OpenGL libraray

#pragma comment(lib,"OpenGL32.lib")
#pragma comment(lib, "glu32.lib")

//global function delaration



LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM); // Entry point function

// global variable declaration
FILE* gpFile = NULL;

HWND ghwnd = NULL;
BOOL gbActive = FALSE;

DWORD dwStyle = 0;
WINDOWPLACEMENT wpPrev = { sizeof(WINDOWPLACEMENT) };
BOOL gbFullscreen = FALSE;

//OpenGL related Global variables
HDC ghdc = NULL;   // DC
HGLRC ghrc = NULL; // rendating context

// FighterPlane Global variables

GLfloat x1_plane = -5.5f;
GLfloat y1_plane = 0.1f;

GLfloat x2_plane = -5.5f;
GLfloat y2_plane = 3.1f;

GLfloat x3_plane = -5.0f;
GLfloat y3_plane = -2.6f;

GLfloat rotatePlane = 90.0f;
//**************************************************************************************************************************
//***********************************///// BHARAT Global Variables /////****************************************************
//**************************************************************************************************************************

GLfloat x_letterB = -5.0f;
GLfloat y_letterB = 0.0f;

GLfloat x_letterH = -5.0f;
GLfloat y_letterH = 2.0f;

GLfloat x_letterA = -4.0f;
GLfloat y_letterA = -1.5f;

GLfloat x_letterR = 5.3f;
GLfloat y_letterR = 3.0f;

GLfloat x_letterA2 = 5.1f;
GLfloat y_letterA2 = 0.0f;

GLfloat x_letterT = 5.7f;
GLfloat y_letterT = 0.17f;



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCnmdLine, int iCmdShow) // function defition
{

	// Function Declarations

	int initialize(void);
	void uninitialized(void);
	void display(void);
	void update(void);

	// Local variable Declarations

	WNDCLASSEX wndclass;
	HWND hwnd;
	MSG msg;
	TCHAR szAppName[] = TEXT("VBWindow");
	int iResult = 0;
	BOOL bDone = FALSE;

	gpFile = fopen("Log.txt", "w");
	if (gpFile == NULL)
	{
		MessageBox(NULL, TEXT("Log file cannot be open!!!"), TEXT("File create error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "Program started successfully \n");

	//Code
	//WNDCLASSEX initialisation

	wndclass.cbSize = sizeof(WNDCLASSEX);
	wndclass.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wndclass.cbClsExtra = 0;
	wndclass.cbWndExtra = 0;
	wndclass.lpfnWndProc = WndProc;
	wndclass.hInstance = hInstance;
	wndclass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wndclass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));
	wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
	wndclass.lpszClassName = szAppName;
	wndclass.lpszMenuName = NULL;
	wndclass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(MYICON));

	// Register WNDCLASSEX
	RegisterClassEx(&wndclass);

	int WW = WIN_WIDTH;
	int WH = WIN_HEIGHT;

	float SW = GetSystemMetrics(SM_CXSCREEN);
	float SH = GetSystemMetrics(SM_CYSCREEN);
	int X = (SW - WW) / 2;
	int Y = (SH - WH) / 2;


	//Create Window 
	hwnd = CreateWindowEx(
		WS_EX_APPWINDOW,
		szAppName,
		TEXT("Vaishnavi Bhave"),
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
		X,
		Y,
		WW,
		WH,
		NULL,
		NULL,
		hInstance,
		NULL
	);

	ghwnd = hwnd;

	// initialization

	iResult = initialize();

	if (iResult != 0)
	{
		MessageBox(hwnd, TEXT("Initialize() FAILED !!!"), TEXT(" error"), MB_OK | MB_ICONERROR);
		DestroyWindow(hwnd);

	}

	// Show Window
	ShowWindow(hwnd, iCmdShow);

	SetForegroundWindow(hwnd);
	SetFocus(hwnd);


	// Game Loop
	while (bDone == FALSE)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
			{
				bDone = TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			if (gbActive == TRUE)
			{
				// Render
				display();

				// Update
				update();

			}
		}
	}

	//uninitialization
	uninitialized();

	return((int)msg.wParam);
}

// callBack Function
LRESULT CALLBACK WndProc(HWND hwnd, UINT iMsg, WPARAM wParam, LPARAM lParam)
{
	// function declaration

	void resize(int, int);

	void ToggleFullscreen(void);

	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Hello World !!!");

	// code
	switch (iMsg)
	{
	case WM_SETFOCUS:
		gbActive = TRUE;
		break;

	case WM_KILLFOCUS:
		gbActive = FALSE;
		break;

	case WM_SIZE:
		resize(LOWORD(lParam), HIWORD(lParam));
		break;

	case WM_ERASEBKGND:
		return 0;


	case WM_KEYDOWN:
		switch (LOWORD(wParam))
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);
			break;
		}
		break;

	case WM_CHAR:
		switch (LOWORD(wParam))
		{
		case 'F':
		case 'f':
			if (gbFullscreen == FALSE)
			{
				ToggleFullscreen();
				gbFullscreen = TRUE;
			}
			else
			{
				ToggleFullscreen();
				gbFullscreen = FALSE;
			}
			break;
		}
		break;

	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	default:
		break;
	}
	return(DefWindowProc(hwnd, iMsg, wParam, lParam));

}

void ToggleFullscreen(void)
{
	// local variable declarations

	MONITORINFO mi = { sizeof(MONITORINFO) };

	// code
	if (gbFullscreen == FALSE)
	{
		dwStyle = GetWindowLong(ghwnd, GWL_STYLE);
		if (dwStyle & WS_OVERLAPPEDWINDOW)
		{
			if (GetWindowPlacement(ghwnd, &wpPrev) && GetMonitorInfo(MonitorFromWindow(ghwnd, MONITORINFOF_PRIMARY), &mi))
			{
				SetWindowLong(ghwnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
				SetWindowPos(
					ghwnd,
					HWND_TOP, //WS_OVERLAPPED is brought back using this statement 
					mi.rcMonitor.left, // this is rect rc , that is rectasngle vala rc giving us left of monitor  
					mi.rcMonitor.top, // this is rect rc , that is rectasngle vala rc giving us top of monitor
					mi.rcMonitor.right - mi.rcMonitor.left, // right of monitor - left gives width
					mi.rcMonitor.bottom - mi.rcMonitor.top, // height of
					SWP_NOZORDER | SWP_FRAMECHANGED
				);
			}
		}
		ShowCursor(FALSE);
	}

	else
	{
		SetWindowPlacement(ghwnd, &wpPrev);
		SetWindowLong(ghwnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(
			ghwnd,
			HWND_TOP,
			0,
			0,
			0,
			0,
			SWP_NOMOVE | SWP_NOSIZE | SWP_NOOWNERZORDER | SWP_NOZORDER | SWP_FRAMECHANGED
		);
		ShowCursor(TRUE);
	}

}

int initialize(void)
{
	// Function Declarations

	void resize(int, int);

	// Code
	PIXELFORMATDESCRIPTOR pfd;
	int iPixelFormatIndex = 0;
	ZeroMemory(&pfd, sizeof(PIXELFORMATDESCRIPTOR)); 	// Remaining members are kept zero in pfd struct using this.

	pfd.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	pfd.nVersion = 1;
	pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	pfd.iPixelType = PFD_TYPE_RGBA;
	pfd.cColorBits = 32;
	pfd.cRedBits = 8;
	pfd.cGreenBits = 8;
	pfd.cBlueBits = 8;
	pfd.cAlphaBits = 8;

	// Get the DC (device context , painter)

	ghdc = GetDC(ghwnd);
	if (ghdc == NULL)
	{
		fprintf(gpFile, "GetDC() failed \n");
		return -1;
	}

	iPixelFormatIndex = ChoosePixelFormat(ghdc, &pfd); // iPixelFormatIndex must be always non-zero positive
	if (iPixelFormatIndex == 0)
	{
		fprintf(gpFile, "ChoosePixelFormat() Failed \n");
		return -2;
	}

	// set obtained pixel format step-4

	if (SetPixelFormat(ghdc, iPixelFormatIndex, &pfd) == FALSE)
	{
		fprintf(gpFile, "SetPixelFormat() Failed \n");
		return -3;
	}
	// Tell Windows graphing libraray to give OpenGL compatible context from this DC
	// Create OpenGl context from device context.

	ghrc = wglCreateContext(ghdc);
	if (ghrc == NULL)
	{
		fprintf(gpFile, "wglCreateContext() failed \n");
		return -4;
	}
	// now ghdc will end its role and will give control to ghrc for furthrt steps
	//Make rendering context current

	if (wglMakeCurrent(ghdc, ghrc) == FALSE)
	{
		fprintf(gpFile, "wglMakeCurrent() failed \n");
		return -5;
	}
	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Here OpenGL starts.....

	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void resize(int width, int height)
{
	// code
	if (height <= 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);

}

void display(void)
{
	// Code
	void BHARAT(void);
	void drawfighterplane(void);
	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	BHARAT();

	//********************************************************************************************************************************
	//**********************************************/// FIGHTERPLANE ///****************************************************************
	//********************************************************************************************************************************
	glLoadIdentity();

	glTranslatef(-0.7f, -0.75f, -3.0f);

	glPushMatrix();
	glTranslatef(x1_plane, y1_plane, -3.0f);
	glScalef(1.5f, 1.5f, 1.5f);
	drawfighterplane();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(x2_plane, y2_plane, -3.0f);
	glScalef(1.5f, 1.5f, 1.5f);
	glRotatef(-rotatePlane, 0.0f, 0.0f, 1.0f);
	drawfighterplane();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(x3_plane, y3_plane, -3.0f);
	glScalef(1.5f, 1.5f, 1.5f);
	glRotatef(rotatePlane, 0.0f, 0.0f, 1.0f);
	drawfighterplane();
	glPopMatrix();


	SwapBuffers(ghdc);

}

void update(void)
{
	// Code
	if (x_letterB <= -3.0)
	{
		x_letterB += 0.0004f;

	}
	else if (x_letterB >= -3.0f && x_letterH <= -2.0f)
	{
		x_letterH += 0.0004f;
		y_letterH -= 0.000294f;

	}
	else if (x_letterH >= -2.0f && x_letterA <= -0.75f)
	{
		x_letterA += 0.00034f;
		y_letterA += 0.00031f;
	}
	else if (x_letterA >= -0.75f && x_letterR >= 0.7f)
	{
		x_letterR -= 0.00055f;
		y_letterR -= 0.00018f;
	}
	else if (x_letterR <= 0.7f && x_letterA2 >= 1.7f)
	{
		x_letterA2 -= 0.00045f;
		y_letterA2 += 0.0002f;
	}
	else if (x_letterA2 <= 1.7f && x_letterT >= 3.5f)
	{
		x_letterT -= 0.0004f;
	}

	//************************** // fighterPlane Transformation // ***************************
	//****************************************************************************************

	else if (x_letterT <= 3.5f)
	{
		rotatePlane -= 0.004f;
		if (rotatePlane <= 0.0f)
		{
			rotatePlane = 0.0f;

		}
		//x1_plane += 0.00009;
		//x2_plane += 0.00009;
		//x3_plane += 0.000073;
		//y2_plane -= 0.000099;
		//y3_plane += 0.000044;
		//
		//if (y2_plane <= 0.0f) {
		//	y2_plane = 0.0f;
		//	x2_plane += 0.00001f;
		// 	
		//}
		//
		//if (y3_plane >= 0.0f) {
		//	y3_plane = 0.0f;
		//	y1_plane = 0.0f;
		//	x3_plane += 0.00001f;
		//	x1_plane += 0.00002f;
		x1_plane += 0.00009f;
		x2_plane += 0.00009f;
		x3_plane += 0.00008f;
		y2_plane -= 0.0001f;
		y3_plane += 0.000099f;
	}
	if (y2_plane <= 0.1f)
	{
		y2_plane = 0.1f;
		x2_plane = x1_plane;
	}
	if (y3_plane >= 0.1f)
	{
		y3_plane = 0.1f;
		x3_plane = x1_plane;
	}

	if (x1_plane >= 3.0f && rotatePlane <= 0.0f)
	{
		y2_plane += 0.00005f;
		y3_plane -= 0.00005f;
		rotatePlane -= 1.0f;
	}



}

void drawfighterplane(void) {

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glBegin(GL_QUADS);

	// body 
	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.3f, 0.4f, 0.0f);
		glVertex3f(0.3f, 0.2f, 0.0f);
		glVertex3f(0.1f, 0.2f, 0.0f);
		glVertex3f(0.1f, 0.4f, 0.0f);

	}

	// back side quad of body

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.1f, 0.38f, 0.0f);
		glVertex3f(0.1f, 0.22f, 0.0f);
		glVertex3f(-0.1f, 0.22f, 0.0f);
		glVertex3f(-0.1f, 0.38f, 0.0f);

	}

	//front trapezium part
	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.3f, 0.4f, 0.0f);
		glVertex3f(0.3f, 0.2f, 0.0f);
		glVertex3f(0.45f, 0.25f, 0.0f);
		glVertex3f(0.45f, 0.35f, 0.0f);

	}

	// front lid
	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.45f, 0.35f, 0.0f);
		glVertex3f(0.45f, 0.25f, 0.0f);
		glVertex3f(0.65f, 0.30f, 0.0f);
		glVertex3f(0.65f, 0.30f, 0.0f);

	}

	// plane both sided trapezium shaped sides

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.29f, 0.41, 0.0f);
		glVertex3f(0.10f, 0.41, 0.0f);
		glVertex3f(0.10f, 0.6, 0.0f);
		glVertex3f(0.15f, 0.6, 0.0f);
	}

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.29f, 0.19f, 0.0f);
		glVertex3f(0.10f, 0.19f, 0.0f);
		glVertex3f(0.10f, 0.01, 0.0f);
		glVertex3f(0.15f, 0.01, 0.0f);
	}

	//1 short fang back

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.05f, 0.385f, 0.0f);
		glVertex3f(-0.12f, 0.385f, 0.0f);
		glVertex3f(-0.12f, 0.5f, 0.0f);
		glVertex3f(-0.05f, 0.5f, 0.0f);

	}

	//2

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(0.05f, 0.215f, 0.0f);
		glVertex3f(-0.12f, 0.215f, 0.0f);
		glVertex3f(-0.12f, 0.1f, 0.0f);
		glVertex3f(-0.05f, 0.1f, 0.0f);

	}

	// black quad for smoke of plane

	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.05f, 0.35f, 0.0f);
		glVertex3f(-0.1f, 0.35f, 0.0f);
		glVertex3f(-0.1f, 0.25f, 0.0f);
		glVertex3f(-0.05f, 0.25f, 0.0f);

	}

	{
		glColor3f(0.137f, 0.388f, 0.50f);
		glVertex3f(-0.05f, 0.34f, 0.0f);
		glVertex3f(-0.08f, 0.34f, 0.0f);
		glVertex3f(-0.08f, 0.26f, 0.0f);
		glVertex3f(-0.05f, 0.26f, 0.0f);

	}

	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-0.08f, 0.335f, 0.0f);
		glVertex3f(-0.11f, 0.335f, 0.0f);
		glVertex3f(-0.11f, 0.265f, 0.0f);
		glVertex3f(-0.08f, 0.265f, 0.0f);

	}

	// flag tricolor smoke

	{
		glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(-0.11f, 0.333f, 0.0f);
		//glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(-0.31f, 0.333f, 0.0f);
		//glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(-0.31f, 0.311f, 0.0f);
		//glColor3f(1.0f, 0.403f, 0.11f);
		glVertex3f(-0.11f, 0.311f, 0.0f);

	}

	{
		glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.31f, 0.311f, 0.0f);
		//glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.11f, 0.311f, 0.0f);
		//glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.11f, 0.289f, 0.0f);
		//glColor3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-0.31f, 0.289f, 0.0f);

	}

	{
		glColor3f(0.015f, 0.415f, 0.219f);
		glVertex3f(-0.11f, 0.289f, 0.0f);
		//glColor3f(0.015f, 0.415f, 0.219f);
		glVertex3f(-0.31f, 0.289f, 0.0f);
		//glColor3f(0.015f, 0.415f, 0.219f);
		glVertex3f(-0.31f, 0.267f, 0.0f);
		//glColor3f(0.015f, 0.415f, 0.219f);
		glVertex3f(-0.11f, 0.267f, 0.0f);

	}

	glEnd();

	//glDisable(GL_BLEND);



	// ***************** IAF letters****************


	glTranslatef(0.2f, 0.3f, 0.0f);
	glScalef(0.4f, 0.4f, 0.4f);

	glBegin(GL_QUADS);

	{
		// letter I

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.3f, 0.1f, 0.0f);
		glVertex3f(-0.37f, 0.1f, 0.0f);
		glVertex3f(-0.37f, 0.075f, 0.0f);
		glVertex3f(-0.3f, 0.075f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.32f, 0.1f, 0.0f);
		glVertex3f(-0.35f, 0.1f, 0.0f);
		glVertex3f(-0.35f, -0.1f, 0.0f);
		glVertex3f(-0.32f, -0.1f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.3f, -0.1f, 0.0f);
		glVertex3f(-0.37f, -0.1f, 0.0f);
		glVertex3f(-0.37f, -0.075f, 0.0f);
		glVertex3f(-0.3f, -0.075f, 0.0f);

		// letter A

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.2f, 0.1f, 0.0f);
		glVertex3f(-0.23f, 0.1f, 0.0f);
		glVertex3f(-0.28f, -0.1f, 0.0f);
		glVertex3f(-0.25f, -0.1f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.17f, 0.0f, 0.0f);
		glVertex3f(-0.25f, 0.0f, 0.0f);
		glVertex3f(-0.26f, -0.03f, 0.0f);
		glVertex3f(-0.17f, -0.03f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.2f, 0.1f, 0.0f);
		glVertex3f(-0.23f, 0.1f, 0.0f);
		glVertex3f(-0.17f, -0.1f, 0.0f);
		glVertex3f(-0.14f, -0.1f, 0.0f);

		// letter F

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.09f, 0.1f, 0.0f);
		glVertex3f(-0.12f, 0.1f, 0.0f);
		glVertex3f(-0.12f, -0.1f, 0.0f);
		glVertex3f(-0.09f, -0.1f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.02f, 0.1f, 0.0f);
		glVertex3f(-0.09f, 0.1f, 0.0f);
		glVertex3f(-0.09f, 0.075f, 0.0f);
		glVertex3f(-0.02f, 0.075f, 0.0f);

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(-0.02f, 0.0f, 0.0f);
		glVertex3f(-0.09f, 0.0f, 0.0f);
		glVertex3f(-0.09f, 0.025f, 0.0f);
		glVertex3f(-0.02f, 0.025f, 0.0f);
	}
	glEnd();

}


void BHARAT(void) {
	void drawLetterB(void);
	void drawLetterH(void);
	void drawLetterA(void);
	void drawLetterR(void);
	void drawLetterA2(void);
	void drawLetterT(void);

	glTranslatef(-0.7f, -1.0f, -6.0f);

	glPushMatrix();
	glTranslatef(x_letterB, y_letterB, 0.0f);
	drawLetterB();
	glPopMatrix();

	//H
	glPushMatrix();
	glTranslatef(x_letterH, y_letterH, 0.0f);  // -2.0f, -0.1999f, 0.0f
	drawLetterH();
	glPopMatrix();

	//A
	glPushMatrix();
	glTranslatef(x_letterA, y_letterA, 0.0f); // -0.75f, 1.47f, 0.0f
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	drawLetterA();
	glPopMatrix();

	//R
	glPushMatrix();
	glTranslatef(x_letterR, y_letterR, 0.0f); // 0.7f, 1.5f, 0.0f
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.0f, 1.05f, 1.0f);
	drawLetterR();
	glPopMatrix();

	//A2
	glPushMatrix();
	glTranslatef(x_letterA2, y_letterA2, 0.0f); // 1.7f, 1.55f, 0.0f
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glRotatef(90.0f, 1.0f, 0.0f, 0.0f);
	glScalef(1.0f, 1.05f, 1.0f);
	drawLetterA2();
	glPopMatrix();

	//T
	glPushMatrix();
	glTranslatef(x_letterT, y_letterT, 0.0f); // 3.5f, 0.15f, 0.0f
	glScalef(1.1f, 1.4f, 1.2f);
	drawLetterT();
	glPopMatrix();

}

void drawLetterB(void) {
	//glTranslatef(0.5f, -2.0f, -6.0f);
	glBegin(GL_QUADS);

	//  | upper vertical line
//orange - 1.0f, 0.403f, 0.11f

	//green 0.015, 0.415, 0.219
	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(1.0f, 0.1f, 0.0f);
		glVertex3f(0.5f, 0.1f, 0.0f);
		glVertex3f(0.5f, 0.3f, 0.0f);
		glVertex3f(1.0f, 0.3f, 0.0f);

	}

	// - side vertical line

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.1f, 0.0f);
		glVertex3f(0.3f, 0.1f, 0.0f);
		glVertex3f(0.3f, 1.3f, 0.0f);
		glVertex3f(0.5f, 1.3f, 0.0f);

	}

	// - upper horizontal line

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(1.0f, 1.5f, 0.0f);
		glVertex3f(1.0f, 1.3f, 0.0f);
		glVertex3f(0.3f, 1.3f, 0.0f);
		glVertex3f(0.3f, 1.5f, 0.0f);

	}

	// middle vertical line 1

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(1.0f, 0.1f, 0.0f);
		glVertex3f(0.8f, 0.1f, 0.0f);
		glVertex3f(0.8f, 0.8f, 0.0f);
		glVertex3f(1.0f, 0.6f, 0.0f);

	}

	// middle vertical line 2

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.8f, 0.7f, 0.0f);
		glVertex3f(1.0f, 0.9f, 0.0f);
		glVertex3f(1.0f, 1.3f, 0.0f);
		glVertex3f(0.8f, 1.3f, 0.0f);

	}

	// middle horizontal line 

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.8f, 0.7f, 0.0f);
		glVertex3f(0.5f, 0.7f, 0.0f);
		glVertex3f(0.5f, 0.9f, 0.0f);
		glVertex3f(0.8f, 0.9f, 0.0f);

	}


	glEnd();
}

void drawLetterH(void) {

	//position p = 5.5 for x
	// 3.7 for y

	glBegin(GL_QUADS);

	// vertical line 1 left 
	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.3f, 0.0f);
		glVertex3f(0.3f, 0.3f, 0.0f);
		glVertex3f(0.3f, 1.7f, 0.0f);
		glVertex3f(0.5f, 1.7f, 0.0f);

	}

	// verticle line right 2

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(1.0f, 0.3f, 0.0f);
		glVertex3f(0.8f, 0.3f, 0.0f);
		glVertex3f(0.8f, 1.7f, 0.0f);
		glVertex3f(1.0f, 1.7f, 0.0f);

	}

	//horizontal middle line

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(1.0f, 0.9f, 0.0f);
		glVertex3f(0.5f, 0.9f, 0.0f);
		glVertex3f(0.5f, 1.2f, 0.0f);
		glVertex3f(1.0f, 1.2f, 0.0f);

	}



	glEnd();

}


void drawLetterA(void) {
	// 4.2 for  x 
	// 3.4 for y
	glBegin(GL_QUADS);
	// vertical line 1 left 
	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.7f, 0.1, 0.0f);
		glVertex3f(0.5f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.3f, 0.0f);
		glVertex3f(0.2f, 1.4f, 0.0f);
	}
	//right line
	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.7f, 0.1f, 0.0f);
		glVertex3f(0.5f, 0.2f, 0.0f);
		glVertex3f(1.0f, 1.4f, 0.0f);
		glVertex3f(1.2f, 1.3f, 0.0f);
	}

	//middle line of A

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.8f, 0.8f, 0.0f);
		glVertex3f(0.4f, 0.8f, 0.0f);
		glVertex3f(0.4f, 0.6f, 0.0f);
		glVertex3f(0.8f, 0.6f, 0.0f);

	}

	glEnd();
}


void drawLetterR(void) {

	// 2.8 for x coordinates 
	// 3.4 for y coordinates

	glBegin(GL_QUADS);
	// vertical line 1 left 
	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.2f, 0.0f, 0.0f);
		glVertex3f(0.0f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.3f, 0.0f);
		glVertex3f(0.2f, 1.3f, 0.0f);
	}

	// vertical side line 

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.7f, 0.2f, 0.0f);
		glVertex3f(0.5f, 0.2f, 0.0f);
		glVertex3f(0.5f, 0.6f, 0.0f);
		glVertex3f(0.7f, 0.6f, 0.0f);
	}

	// horizontal upper line 

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.2f, 0.0f, 0.0f);
		glVertex3f(0.7f, 0.0f, 0.0f);
		glVertex3f(0.7f, 0.2f, 0.0f);
		glVertex3f(0.2f, 0.2f, 0.0f);
	}

	// horizontal lower line

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.2f, 0.6f, 0.0f);
		glVertex3f(0.7f, 0.6f, 0.0f);
		glVertex3f(0.7f, 0.8f, 0.0f);
		glVertex3f(0.2f, 0.8f, 0.0f);
	}

	//slant line

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.45f, 0.8f, 0.0f);
		glVertex3f(0.2f, 0.8f, 0.0f);
		glVertex3f(0.55f, 1.3f, 0.0f);
		glVertex3f(0.8f, 1.3f, 0.0f);

	}


	glEnd();
}


void drawLetterA2(void) {


	glBegin(GL_QUADS);
	// vertical line 1 left 
	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.7f, 0.1, 0.0f);
		glVertex3f(0.5f, 0.0f, 0.0f);
		glVertex3f(0.0f, 1.3f, 0.0f);
		glVertex3f(0.2f, 1.4f, 0.0f);
	}
	//right line
	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.7f, 0.1f, 0.0f);
		glVertex3f(0.5f, 0.2f, 0.0f);
		glVertex3f(1.0f, 1.4f, 0.0f);
		glVertex3f(1.2f, 1.3f, 0.0f);
	}

	//middle line of A

	{
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.8f, 0.8f, 0.0f);
		glVertex3f(0.4f, 0.8f, 0.0f);
		glVertex3f(0.4f, 0.6f, 0.0f);
		glVertex3f(0.8f, 0.6f, 0.0f);

	}

	glEnd();
}


void drawLetterT(void) {
	glBegin(GL_QUADS);

	{

		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.5f, 0.8f, 0.0f);
		glVertex3f(0.5f, 1.0f, 0.0f);
		glVertex3f(-0.5f, 1.0f, 0.0f);
		glVertex3f(-0.5f, 0.8f, 0.0f);

	}

	{

		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(0.1f, 0.8f, 0.0f);
		glVertex3f(-0.1f, 0.8f, 0.0f);
		glVertex3f(-0.1f, 0.0f, 0.0f);
		glVertex3f(0.1f, 0.0f, 0.0f);

	}

	glEnd();
}



void uninitialized(void)
{
	//  function declarations

	void ToggleFullscreen(void);
	if (gbFullscreen == TRUE)
	{
		ToggleFullscreen();
		gbFullscreen = FALSE;
	}

	// make the hdc as current context

	if (wglGetCurrentContext() == ghrc)
	{
		wglMakeCurrent(NULL, NULL);
	}

	if (ghrc)
	{
		wglDeleteContext(ghrc);
		ghrc = NULL;

	}

	// release the hdc

	if (ghdc)
	{
		ReleaseDC(ghwnd, ghdc);
	}

	// code
	// if application is exiting in fullscreen
	// destroy window
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	// close the log file

	if (gpFile)
	{
		fprintf(gpFile, "Program ended successfully \n");
		fclose(gpFile);
		gpFile = NULL;
	}
}


