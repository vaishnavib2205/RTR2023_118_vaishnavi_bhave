#include<Windows.h> // Windows header files 
#include "OGL.h"
#include<stdlib.h> // For exit
#include<stdio.h> // For file IO (input output)

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

BOOL bLight = FALSE;

GLUquadric* quadric = NULL;


GLfloat lightAmbient[] = { 0.0f ,0.0f,0.0f,1.0f };
GLfloat lightDiffuse[] = { 1.0f , 1.0f , 1.0f , 1.0f };
GLfloat lightPosition[] = { 0.0f , 0.0f , 0.0f , 1.0f };

GLfloat angleForXRotation = 0.0f;
GLfloat angleForYRotation = 0.0f;
GLfloat angleForZRotation = 0.0f;

GLuint keyPressed = 0;




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

		case 'L':
		case 'l':

			if (bLight == FALSE)
			{
				glEnable(GL_LIGHTING);
				bLight = TRUE;
			}
			else
			{
				glDisable(GL_LIGHTING);
				bLight = FALSE;
			}
			break;

		case 'X':
		case 'x':

			keyPressed = 1;
			angleForXRotation = 0.0f; // resset the angle here
			break;

		case 'Y':
		case 'y':

			keyPressed = 2;
			angleForYRotation = 0.0f; // reset the angle for y here
			break;

		case 'Z':
		case 'z':

			keyPressed = 3;
			angleForZRotation = 0.0f; // reset the angle for z here
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
	pfd.cDepthBits = 32;

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

	// enabling depth 

	glShadeModel(GL_SMOOTH);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);


	// initialize quadric

	quadric = gluNewQuadric();
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// set the clear colour of window to blue

	glClearColor(0.5f, 0.5f, 0.5f, 1.0f); // dark greay backGround

	// Here OpenGL starts.....

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	glEnable(GL_LIGHT0);



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
	// variable declaration

	GLfloat materialAmbient[4];
	GLfloat materialDiffuse[4];
	GLfloat materialSpecular[4];
	GLfloat shinniness;


	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	glTranslatef(-10.0f, -8.0f, -18.0f);

	// animation



	glPushMatrix();
	{



		if (keyPressed == 1)
		{
			glRotatef(angleForXRotation, 1.0f, 0.0f, 0.0f);
			lightPosition[2] = angleForXRotation;
		}
		if (keyPressed == 2)
		{
			glRotatef(angleForYRotation, 0.0f, 1.0f, 0.0f);
			lightPosition[0] = angleForYRotation;
		}
		if (keyPressed == 3)
		{
			glRotatef(angleForZRotation, 0.0f, 0.0f, 1.0f);
			lightPosition[1] = angleForZRotation;
		}
		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

		glPushMatrix();
		{



			// first column of precious stones

			// 1st column, emerald *****
			glPushMatrix();
			{

				materialAmbient[0] = 0.0215;
				materialAmbient[1] = 0.1745;
				materialAmbient[2] = 0.0215;
				materialAmbient[3] = 1.0f;

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material
				materialDiffuse[0] = 0.07568; // r
				materialDiffuse[1] = 0.61424; // g
				materialDiffuse[2] = 0.07568; // b
				materialDiffuse[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material 
				materialSpecular[0] = 0.633;    // r
				materialSpecular[1] = 0.727811; // g
				materialSpecular[2] = 0.633;    // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.6 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(1.5f, 14.0f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}
			glPopMatrix();

			glPushMatrix();
			{
				// ***** 2nd sphere on 1st column, jade *****

				materialAmbient[0] = 0.135;  // r
				materialAmbient[1] = 0.2225; // g
				materialAmbient[2] = 0.1575; // b
				materialAmbient[3] = 1.0f;   // a

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material
			// diffuse material
				materialDiffuse[0] = 0.54; // r
				materialDiffuse[1] = 0.89; // g
				materialDiffuse[2] = 0.63; // b
				materialDiffuse[3] = 1.0f; // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material
				materialSpecular[0] = 0.316228; // r
				materialSpecular[1] = 0.316228; // g
				materialSpecular[2] = 0.316228; // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.1 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(1.5f, 11.5f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}
			glPopMatrix();

			// ***** 3rd sphere on 1st column, obsidian *****
			// ambient material

			glPushMatrix();
			{
				materialAmbient[0] = 0.05375; // r
				materialAmbient[1] = 0.05;    // g
				materialAmbient[2] = 0.06625; // b
				materialAmbient[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material
			// diffuse material
				materialDiffuse[0] = 0.18275; // r
				materialDiffuse[1] = 0.17;    // g
				materialDiffuse[2] = 0.22525; // b
				materialDiffuse[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material
				materialSpecular[0] = 0.332741; // r
				materialSpecular[1] = 0.328634; // g
				materialSpecular[2] = 0.346435; // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.3 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(1.5f, 9.0f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}

			glPopMatrix();

			// ***** 4th sphere on 1st column, pearl *****
			// ambient material

			glPushMatrix();
			{
				materialAmbient[0] = 0.25;    // r
				materialAmbient[1] = 0.20725; // g
				materialAmbient[2] = 0.20725; // b
				materialAmbient[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material
			// diffuse material
				materialDiffuse[0] = 1.0;   // r
				materialDiffuse[1] = 0.829; // g
				materialDiffuse[2] = 0.829; // b
				materialDiffuse[3] = 1.0f;  // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material
				materialSpecular[0] = 0.296648; // r
				materialSpecular[1] = 0.296648; // g
				materialSpecular[2] = 0.296648; // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.088 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(1.5f, 6.5f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}

			glPopMatrix();

			// ***** 5th sphere on 1st column, ruby *****
			// ambient material
			glPushMatrix();
			{
				materialAmbient[0] = 0.1745;  // r
				materialAmbient[1] = 0.01175; // g
				materialAmbient[2] = 0.01175; // b
				materialAmbient[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material
			// diffuse material
				materialDiffuse[0] = 0.61424; // r
				materialDiffuse[1] = 0.04136; // g
				materialDiffuse[2] = 0.04136; // b
				materialDiffuse[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material
				materialSpecular[0] = 0.727811; // r
				materialSpecular[1] = 0.626959; // g
				materialSpecular[2] = 0.626959; // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.6 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(1.5f, 4.0f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}

			glPopMatrix();

			// ***** 6th sphere on 1st column, turquoise *****
			// ambient material
			glPushMatrix();
			{
				materialAmbient[0] = 0.1;     // r
				materialAmbient[1] = 0.18725; // g
				materialAmbient[2] = 0.1745;  // b
				materialAmbient[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material
			// diffuse material
			// diffuse material
				materialDiffuse[0] = 0.396;   // r
				materialDiffuse[1] = 0.74151; // g
				materialDiffuse[2] = 0.69102; // b
				materialDiffuse[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material
				materialSpecular[0] = 0.297254; // r
				materialSpecular[1] = 0.30829;  // g
				materialSpecular[2] = 0.306678; // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.1 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(1.5f, 1.5f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}
			glPopMatrix();
		}
		glPopMatrix();


		// ***** 1st sphere on 2nd column, brass *****
		glPushMatrix();
		{
			// ambient material
			glPushMatrix();
			{
				materialAmbient[0] = 0.329412; // r
				materialAmbient[1] = 0.223529; // g
				materialAmbient[2] = 0.027451; // b
				materialAmbient[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material

				materialDiffuse[0] = 0.780392; // r
				materialDiffuse[1] = 0.568627; // g
				materialDiffuse[2] = 0.113725; // b
				materialDiffuse[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material
				materialSpecular[0] = 0.992157; // r
				materialSpecular[1] = 0.941176; // g
				materialSpecular[2] = 0.807843; // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.21794872 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(7.5f, 14.0f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}
			glPopMatrix();

			// ***** 2nd sphere on 2nd column, bronze *****
			// ambient material
			glPushMatrix();
			{
				materialAmbient[0] = 0.2125; // r
				materialAmbient[1] = 0.1275; // g
				materialAmbient[2] = 0.054;  // b
				materialAmbient[3] = 1.0f;   // a

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material

				materialDiffuse[0] = 0.714;   // r
				materialDiffuse[1] = 0.4284;  // g
				materialDiffuse[2] = 0.18144; // b
				materialDiffuse[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material
				materialSpecular[0] = 0.393548; // r
				materialSpecular[1] = 0.271906; // g
				materialSpecular[2] = 0.166721; // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.2 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(7.5f, 11.5f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}
			glPopMatrix();

			// ***** 3rd sphere on 2nd column, chrome *****
			// ambient material
			glPushMatrix();
			{
				materialAmbient[0] = 0.25; // r
				materialAmbient[1] = 0.25; // g
				materialAmbient[2] = 0.25; // b
				materialAmbient[3] = 1.0f; // a

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material

				materialDiffuse[0] = 0.4;  // r
				materialDiffuse[1] = 0.4;  // g
				materialDiffuse[2] = 0.4;  // b
				materialDiffuse[3] = 1.0f; // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material
				materialSpecular[0] = 0.774597; // r
				materialSpecular[1] = 0.774597; // g
				materialSpecular[2] = 0.774597; // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.6 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(7.5f, 9.0f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}
			glPopMatrix();

			// ***** 4th sphere on 2nd column, copper *****
			// ambient material
			glPushMatrix();
			{
				materialAmbient[0] = 0.19125; // r
				materialAmbient[1] = 0.0735;  // g
				materialAmbient[2] = 0.0225;  // b
				materialAmbient[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material

				materialDiffuse[0] = 0.7038;  // r
				materialDiffuse[1] = 0.27048; // g
				materialDiffuse[2] = 0.0828;  // b
				materialDiffuse[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material
				materialSpecular[0] = 0.256777; // r
				materialSpecular[1] = 0.137622; // g
				materialSpecular[2] = 0.086014; // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.1 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(7.5f, 6.5f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}
			glPopMatrix();

			// ***** 5th sphere on 2nd column, gold *****
			// ambient material
			glPushMatrix();
			{
				materialAmbient[0] = 0.24725; // r
				materialAmbient[1] = 0.1995;  // g
				materialAmbient[2] = 0.0745;  // b
				materialAmbient[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material

				materialDiffuse[0] = 0.75164; // r
				materialDiffuse[1] = 0.60648; // g
				materialDiffuse[2] = 0.22648; // b
				materialDiffuse[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material
				materialSpecular[0] = 0.628281; // r
				materialSpecular[1] = 0.555802; // g
				materialSpecular[2] = 0.366065; // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.4 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(7.5f, 4.0f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}
			glPopMatrix();

			// ***** 6th sphere on 2nd column, silver *****
			// ambient material
			glPushMatrix();
			{
				materialAmbient[0] = 0.19225; // r
				materialAmbient[1] = 0.19225; // g
				materialAmbient[2] = 0.19225; // b
				materialAmbient[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

				// diffuse material

				materialDiffuse[0] = 0.50754; // r
				materialDiffuse[1] = 0.50754; // g
				materialDiffuse[2] = 0.50754; // b
				materialDiffuse[3] = 1.0f;    // a

				glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

				// specular material
				materialSpecular[0] = 0.508273; // r
				materialSpecular[1] = 0.508273; // g
				materialSpecular[2] = 0.508273; // b
				materialSpecular[3] = 1.0f;     // a

				glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

				shinniness = 0.4 * 128;
				glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

				//glLoadIdentity();
				glTranslatef(7.5f, 1.5f, 0.0f);
				gluSphere(quadric, 1.0f, 30, 30);
			}
			glPopMatrix();
		}
		glPopMatrix();

		// *******************************************************

	// ***** 1st sphere on 3rd column, black *****
	// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.0;  // r
			materialAmbient[1] = 0.0;  // g
			materialAmbient[2] = 0.0;  // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.01; // r
			materialDiffuse[1] = 0.01; // g
			materialDiffuse[2] = 0.01; // b
			materialDiffuse[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.50; // r
			materialSpecular[1] = 0.50; // g
			materialSpecular[2] = 0.50; // b
			materialSpecular[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.25 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(13.5f, 14.0f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();

		// ***** 2nd sphere on 3rd column, cyan *****
		// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.0;  // r
			materialAmbient[1] = 0.1;  // g
			materialAmbient[2] = 0.06; // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.0;        // r
			materialDiffuse[1] = 0.50980392; // g
			materialDiffuse[2] = 0.50980392; // b
			materialDiffuse[3] = 1.0f;       // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.50196078; // r
			materialSpecular[1] = 0.50196078; // g
			materialSpecular[2] = 0.50196078; // b
			materialSpecular[3] = 1.0f;       // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.25 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(13.5f, 11.5f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();

		// ***** 3rd sphere on 2nd column, green *****
		// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.0;  // r
			materialAmbient[1] = 0.0;  // g
			materialAmbient[2] = 0.0;  // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.1;  // r
			materialDiffuse[1] = 0.35; // g
			materialDiffuse[2] = 0.1;  // b
			materialDiffuse[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.45; // r
			materialSpecular[1] = 0.55; // g
			materialSpecular[2] = 0.45; // b
			materialSpecular[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.25 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(13.5f, 9.0f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();

		// ***** 4th sphere on 3rd column, red *****
		// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.0;  // r
			materialAmbient[1] = 0.0;  // g
			materialAmbient[2] = 0.0;  // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.5;  // r
			materialDiffuse[1] = 0.0;  // g
			materialDiffuse[2] = 0.0;  // b
			materialDiffuse[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.7;  // r
			materialSpecular[1] = 0.6;  // g
			materialSpecular[2] = 0.6;  // b
			materialSpecular[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.25 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(13.5f, 6.5f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();

		// ***** 5th sphere on 3rd column, white *****
		// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.0;  // r
			materialAmbient[1] = 0.0;  // g
			materialAmbient[2] = 0.0;  // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.55; // r
			materialDiffuse[1] = 0.55; // g
			materialDiffuse[2] = 0.55; // b
			materialDiffuse[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.70; // r
			materialSpecular[1] = 0.70; // g
			materialSpecular[2] = 0.70; // b
			materialSpecular[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.25 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(13.5f, 4.0f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();


		// ***** 6th sphere on 3rd column, yellow plastic *****
		// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.0;  // r
			materialAmbient[1] = 0.0;  // g
			materialAmbient[2] = 0.0;  // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.5;  // r
			materialDiffuse[1] = 0.5;  // g
			materialDiffuse[2] = 0.0;  // b
			materialDiffuse[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.60; // r
			materialSpecular[1] = 0.60; // g
			materialSpecular[2] = 0.50; // b
			materialSpecular[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.25 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(13.5f, 1.5f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();


		// ***** 1st sphere on 4th column, black *****
		// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.02; // r
			materialAmbient[1] = 0.02; // g
			materialAmbient[2] = 0.02; // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.01; // r
			materialDiffuse[1] = 0.01; // g
			materialDiffuse[2] = 0.01; // b
			materialDiffuse[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.4;  // r
			materialSpecular[1] = 0.4;  // g
			materialSpecular[2] = 0.4;  // b
			materialSpecular[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.078125 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(19.5f, 14.0f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();


		// ***** 2nd sphere on 4th column, cyan *****
		// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.0;  // r
			materialAmbient[1] = 0.05; // g
			materialAmbient[2] = 0.05; // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.4;  // r
			materialDiffuse[1] = 0.5;  // g
			materialDiffuse[2] = 0.5;  // b
			materialDiffuse[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.04; // r
			materialSpecular[1] = 0.7;  // g
			materialSpecular[2] = 0.7;  // b
			materialSpecular[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.078125 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(19.5f, 11.5f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();


		// ***** 3rd sphere on 4th column, green *****
		// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.0;  // r
			materialAmbient[1] = 0.05; // g
			materialAmbient[2] = 0.0;  // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.4;  // r
			materialDiffuse[1] = 0.5;  // g
			materialDiffuse[2] = 0.4;  // b
			materialDiffuse[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.04; // r
			materialSpecular[1] = 0.7;  // g
			materialSpecular[2] = 0.04; // b
			materialSpecular[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.078125 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(19.5f, 9.0f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();

		// ***** 4th sphere on 4th column, red *****
	// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.05; // r
			materialAmbient[1] = 0.0;  // g
			materialAmbient[2] = 0.0;  // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.5;  // r
			materialDiffuse[1] = 0.4;  // g
			materialDiffuse[2] = 0.4;  // b
			materialDiffuse[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.7;  // r
			materialSpecular[1] = 0.04; // g
			materialSpecular[2] = 0.04; // b
			materialSpecular[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.078125 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(19.5f, 6.5f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();


		// ***** 5th sphere on 4th column, white *****
		// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.05; // r
			materialAmbient[1] = 0.05; // g
			materialAmbient[2] = 0.05; // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.5;  // r
			materialDiffuse[1] = 0.5;  // g
			materialDiffuse[2] = 0.5;  // b
			materialDiffuse[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.7;  // r
			materialSpecular[1] = 0.7;  // g
			materialSpecular[2] = 0.7;  // b
			materialSpecular[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.078125 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(19.5f, 4.0f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();


		// ***** 6th sphere on 4th column, yellow rubber *****
	// ambient material
		glPushMatrix();
		{
			materialAmbient[0] = 0.05; // r
			materialAmbient[1] = 0.05; // g
			materialAmbient[2] = 0.0;  // b
			materialAmbient[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_AMBIENT, materialAmbient);

			// diffuse material

			materialDiffuse[0] = 0.5;  // r
			materialDiffuse[1] = 0.5;  // g
			materialDiffuse[2] = 0.4;  // b
			materialDiffuse[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_DIFFUSE, materialDiffuse);

			// specular material
			materialSpecular[0] = 0.7;  // r
			materialSpecular[1] = 0.7;  // g
			materialSpecular[2] = 0.04; // b
			materialSpecular[3] = 1.0f; // a

			glMaterialfv(GL_FRONT, GL_SPECULAR, materialSpecular);

			shinniness = 0.078125 * 128;
			glMaterialf(GL_FRONT, GL_SHININESS, shinniness);

			//glLoadIdentity();
			glTranslatef(19.5f, 1.5f, 0.0f);
			gluSphere(quadric, 1.0f, 30, 30);
		}
		glPopMatrix();

	}
	glPopMatrix();



	SwapBuffers(ghdc);

}

void update(void)
{
	// Code

	if (keyPressed == 1)
	{
		angleForXRotation += 0.09f;
		if (angleForXRotation > 360.0f)
		{
			angleForXRotation -= 360.0f;
		}
	}
	if (keyPressed == 2)
	{
		angleForYRotation += 0.09f;
		if (angleForYRotation > 360.0f)
		{
			angleForYRotation -= 360.0f;
		}
	}
	if (keyPressed == 3)
	{
		angleForZRotation += 0.09f;
		if (angleForZRotation > 360.0f)
		{
			angleForZRotation -= 360.0f;
		}
	}
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

	if (quadric)
	{
		gluDeleteQuadric(quadric);
		quadric = NULL;
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



