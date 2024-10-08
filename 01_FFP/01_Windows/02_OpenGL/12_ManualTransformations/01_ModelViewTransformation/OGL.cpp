#include<Windows.h> // Windows header files 
#include "OGL.h"
#include<stdlib.h> // For exit
#include<stdio.h> // For file IO (input output)

#define _USE_MATH_DEFINES 1
#include<math.h> // For converting to radian operations , another mathematical operations

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

GLfloat cAngle = 0.0f;

GLfloat identityMatrix[16];
GLfloat translationMatrix[16];
GLfloat scaleMatrix[16];
GLfloat rotationMatrix_X[16];
GLfloat rotationMatrix_Y[16];
GLfloat rotationMatrix_Z[16];



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
	glHint(GL_PERSPECTIVE_CORRECTION_HINT , GL_NICEST);

	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Here OpenGL starts.....

	// initialization of mitrix
	{ 
		// Identity matrix
		// column 1
		identityMatrix[0] = 1.0f;
		identityMatrix[1] = 0.0f;
		identityMatrix[2] = 0.0f;
		identityMatrix[3] = 0.0f;
		// column 2
		identityMatrix[4] = 0.0f;
		identityMatrix[5] = 1.0f;
		identityMatrix[6] = 0.0f;
		identityMatrix[7] = 0.0f;
		// column 3
		identityMatrix[8] = 0.0f;
		identityMatrix[9] = 0.0f;
		identityMatrix[10] = 1.0f;
		identityMatrix[11] = 0.0f;
		// column 4
		identityMatrix[12] = 0.0f;
		identityMatrix[13] = 0.0f;
		identityMatrix[14] = 0.0f;
		identityMatrix[15] = 1.0f;
	}
	{
		// translation matrix
		//column 1
		translationMatrix[0] = 1.0f;
		translationMatrix[1] = 0.0f;
		translationMatrix[2] = 0.0f;
		translationMatrix[3] = 0.0f;
		//column 2
		translationMatrix[4] = 0.0f;
		translationMatrix[5] = 1.0f;
		translationMatrix[6] = 0.0f;
		translationMatrix[7] = 0.0f;
		// column 3
		translationMatrix[8] = 0.0f;
		translationMatrix[9] = 0.0f;
		translationMatrix[10] = 1.0f;
		translationMatrix[11] = 0.0f;
		//column 4
		translationMatrix[12] = 0.0f;
		translationMatrix[13] = 0.0f;
		translationMatrix[14] = -6.0f;
		translationMatrix[15] = 1.0f;

	}
	{
		// scale matrix
		// column 1
		scaleMatrix[0] = 0.75f;
		scaleMatrix[1] = 0.0f;
		scaleMatrix[2] = 0.0f;
		scaleMatrix[3] = 0.0f;
		//column2
		scaleMatrix[4] = 0.0f;
		scaleMatrix[5] = 0.75f;
		scaleMatrix[6] = 0.0f;
		scaleMatrix[7] = 0.0f;
		// column 3
		scaleMatrix[8] = 0.0f;
		scaleMatrix[9] = 0.0f;
		scaleMatrix[10] = 0.75f;
		scaleMatrix[11] = 0.0f;
		// column 4
		scaleMatrix[12] = 0.0f;
		scaleMatrix[13] = 0.0f;
		scaleMatrix[14] = 0.0f;
		scaleMatrix[15] = 1.0f;
		
	}

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
	static GLfloat angle;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);

	// glLoadIdentity();
	glLoadMatrixf(identityMatrix);

	// glTranslatef(0.0f, 0.0f, -6.0f);
	glMultMatrixf(translationMatrix);

	// glScalef(0.75f, 0.75f, 0.75f);
	glMultMatrixf(scaleMatrix);

	//glRotatef(cAngle, 1.0f, 0.0f, 0.0f);
	//glRotatef(cAngle, 0.0f, 1.0f, 0.0f);
	//glRotatef(cAngle, 0.0f, 0.0f, 1.0f);

	angle = cAngle * (M_PI / 180.0f);

	{
		// rotation through X
		// column 1
		rotationMatrix_X[0] = 1.0f;
		rotationMatrix_X[1] = 0.0f;
		rotationMatrix_X[2] = 0.0f;
		rotationMatrix_X[3] = 0.0f;
		//column 2
		rotationMatrix_X[4] = 0.0f;
		rotationMatrix_X[5] = cos(angle);
		rotationMatrix_X[6] = sin(angle);
		rotationMatrix_X[7] = 0.0f;
		// column 3
		rotationMatrix_X[8] = 0.0f;
		rotationMatrix_X[9] = -sin(angle);
		rotationMatrix_X[10] = cos(angle);
		rotationMatrix_X[11] = 0.0f;
		// column 4
		rotationMatrix_X[12] = 0.0f;
		rotationMatrix_X[13] = 0.0f;
		rotationMatrix_X[14] = 0.0f;
		rotationMatrix_X[15] = 1.0f;
	}
	{
		// rotation through Y
		// column 1
		rotationMatrix_Y[0] = cos(angle);
		rotationMatrix_Y[1] = 0.0f;
		rotationMatrix_Y[2] = -sin(angle);
		rotationMatrix_Y[3] = 0.0f;
		// column 2
		rotationMatrix_Y[4] = 0.0f;
		rotationMatrix_Y[5] = 1.0f;
		rotationMatrix_Y[6] = 0.0f;
		rotationMatrix_Y[7] = 0.0f;
		// column 3
		rotationMatrix_Y[8] =sin(angle);
		rotationMatrix_Y[9] = 0.0f;
		rotationMatrix_Y[10] = cos(angle);
		rotationMatrix_Y[11] = 0.0f;
		// column 4
		rotationMatrix_Y[12] = 0.0f;
		rotationMatrix_Y[13] = 0.0f;
		rotationMatrix_Y[14] = 0.0f;
		rotationMatrix_Y[15] = 1.0f;

	}
	{
		// rotation through Z
		//column 1
		rotationMatrix_Z[0] = cos(angle);
		rotationMatrix_Z[1] = sin(angle);
		rotationMatrix_Z[2] = 0.0f;
		rotationMatrix_Z[3] = 0.0f;
		// column 3
		rotationMatrix_Z[4] = -sin(angle);
		rotationMatrix_Z[5] = cos(angle);
		rotationMatrix_Z[6] = 0.0f;
		rotationMatrix_Z[7] = 0.0f;
		// column 3
		rotationMatrix_Z[8] = 0.0f;
		rotationMatrix_Z[9] = 0.0f;
		rotationMatrix_Z[10] = 1.0f;
		rotationMatrix_Z[11] = 0.0f;
		// column 4
		rotationMatrix_Z[12] = 0.0f;
		rotationMatrix_Z[13] = 0.0f;
		rotationMatrix_Z[14] = 0.0f;
		rotationMatrix_Z[15] = 1.0f;

	}

	glMultMatrixf(rotationMatrix_X);
	glMultMatrixf(rotationMatrix_Y);
	glMultMatrixf(rotationMatrix_Z);
	

	glBegin(GL_QUADS);
	// front face
	{
		glColor3f(1.0f, 0.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
	}
	// right face
	{
		glColor3f(0.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
	}
	// Back face
	{
		glColor3f(0.0f, 0.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
	}
	// Left face
	{
		glColor3f(0.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
	}
	// Top face
	{
		glColor3f(1.0f, 0.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, -1.0f);
		glVertex3f(-1.0f, 1.0f, 1.0f);
		glVertex3f(1.0f, 1.0f, 1.0f);
	}
	// Bottom face
	{
		glColor3f(1.0f, 1.0f, 0.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
		glVertex3f(-1.0f, -1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);

	}

	glEnd();

	SwapBuffers(ghdc);

}
 
void update(void)
{
	// Code
	cAngle = cAngle - 0.5f;
	if (cAngle <= 0.0f)
	{
		cAngle = cAngle + 360.0f;
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



