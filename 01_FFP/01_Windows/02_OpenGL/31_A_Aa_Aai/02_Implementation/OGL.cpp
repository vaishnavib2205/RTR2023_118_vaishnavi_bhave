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


// For STB Cutouts
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
unsigned char* pngImage = NULL;



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

BOOL firstScene = FALSE;
BOOL secondScene = FALSE;
BOOL thirdScene = FALSE;
BOOL fourthScene = FALSE;
BOOL fifthScene = FALSE;
BOOL sixthScene = FALSE;
BOOL seventhScene = FALSE;
BOOL eighthScene = FALSE;

GLuint schoolWallTexture = 0;
GLuint sparrowTexture = 0;
GLuint babySparrowTexture = 0;

GLuint a_aa_aai = 0;
GLuint ma_ma_maka = 0;
GLuint mi_tuza_mama = 0;
GLuint pa_pa_patang = 0;
GLuint chandomama = 0;
GLuint ghadyaal = 0;
GLuint thava = 0;
GLuint baal = 0;

GLfloat firstSceneTrans = -100.0f; //
GLfloat secondSceneTrans = -100.0f;
GLfloat thirdSceneTrans = -100.0f;
GLfloat fourthSceneTrans = -100.0f;
GLfloat fifthSceneTrans = -100.0f;
GLfloat sixthSceneTrans = -100.0f;
GLfloat seventhSceneTrans = -100.0f;
GLfloat eighthSceneTrans = -100.0f;

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
		case 'A':
		case 'a':
			firstScene = TRUE;
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
	BOOL loadPNGTexture(GLuint *, const char*);
	BOOL loadGlTexture(GLuint *, TCHAR[]);
	BOOL bResult = TRUE;


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


	bResult = loadGlTexture(&schoolWallTexture, MAKEINTRESOURCE(SCHOOLWALL_TEXTURE_BITMAP));

	if (bResult == FALSE) {
		fprintf(gpFile, "Loading of WALL texture failed \n");
		return -7;
	}


	//loading of PNG textures

	bResult = loadPNGTexture(&sparrowTexture, "sparrow1.png");
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loatPNGTexture of texture failed \n");
		return (-17);
	}

	bResult = loadPNGTexture(&babySparrowTexture, "babySparrow.png");
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loatPNGTexture of texture failed \n");
		return (-17);
	}

	bResult = loadPNGTexture(&a_aa_aai, "a_aa_aai.png");
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loatPNGTexture of texture failed \n");
		return (-17);
	}

	bResult = loadPNGTexture(&ma_ma_maka, "ma_ma_maka.png");
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loatPNGTexture of texture failed \n");
		return (-17);
	}

	bResult = loadPNGTexture(&mi_tuza_mama, "mi_tuza_mama.png");
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loatPNGTexture of texture failed \n");
		return (-17);
	}

	bResult = loadPNGTexture(&pa_pa_patang, "pa_pa_patang.png");
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loatPNGTexture of texture failed \n");
		return (-17);
	}

	bResult = loadPNGTexture(&chandomama, "chandomama.png");
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loatPNGTexture of texture failed \n");
		return (-17);
	}

	bResult = loadPNGTexture(&ghadyaal, "ghadyaal.png");
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loatPNGTexture of texture failed \n");
		return (-17);
	}

	bResult = loadPNGTexture(&thava, "thava.png");
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loatPNGTexture of texture failed \n");
		return (-17);
	}

	bResult = loadPNGTexture(&baal, "baal.png");
	if (bResult == FALSE)
	{
		fprintf(gpFile, "loatPNGTexture of texture failed \n");
		return (-17);
	}



	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Here OpenGL starts.....

	// Tell OpenGL To Enable The Texture
	glEnable(GL_TEXTURE_2D);

	//png blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);


	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

BOOL loadGlTexture(GLuint* texture, TCHAR imageResourceId[]) {

	//local variable declarations

	HBITMAP hBitmap = NULL;
	BITMAP bmp;
	hBitmap = (HBITMAP)LoadImage(GetModuleHandle(NULL), imageResourceId, IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION); //width aani height nahi denar .. load krto ti image icon aste tevhach image chi height aani width dyaychi aste 
	// mazi bitmap mi resource mhnun ghetleli aahe tithun width aani height ghe
	//LR_CREATEDIBSECTION mazi image resource mhnun load kar LR - Load Resource
	//mazya device independent bitmap cha section tayar kar aani load kar

	if (hBitmap == NULL) {

		fprintf(gpFile, "Load image failed \n");
		return FALSE;
	}

	GetObject(hBitmap, sizeof(BITMAP), &bmp); // bmp jatana rikama jato yetana bharun yeto as its adress given as param

	//create OpenGL texture obj  
	glGenTextures(1, texture);

	//Bind to the generated texture 
	glBindTexture(GL_TEXTURE_2D, *texture); //binding point opengl madhe state machine ne define kelela asto

	//decide image Pixel alignment and unpacking
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4); // load image ne that is os ne mem madhe tila smjnarya bhashet img la load kela opengl mhnty mi ti image baghto aani packed data asel tar unpack karto data karan opengl la data unpack lagto aani te 4 chya slot mdhe unpack karto 

	//set texture parameters 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // mala tuzya texture chya int param de
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);


	//create multiple mipmap images 
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, bmp.bmWidth, bmp.bmHeight, GL_BGR_EXT, GL_UNSIGNED_BYTE, (void*)bmp.bmBits);

	glBindTexture(GL_TEXTURE_2D, 0); // 0 sec param specifies unbind kelela aahe texture la

	DeleteObject(hBitmap);

	hBitmap = NULL;
}


BOOL loadPNGTexture(GLuint* texture, const char* filename)
{
	int width, height, channel;

	pngImage = stbi_load(filename, &width, &height, &channel, STBI_rgb_alpha);
	if (pngImage == NULL)
	{
		fprintf(gpFile, "stbi_load() failed");
		return FALSE;
	}

	fprintf(gpFile, "Image width : %d \t , height : %d \t , channel : %d \n", width, height, channel);

	glGenTextures(1, texture);

	glBindTexture(GL_TEXTURE_2D, *texture);

	// 9th step

	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

	// set texture paramerter

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)pngImage);

	glBindTexture(GL_TEXTURE_2D, 0);

	stbi_image_free(pngImage);

	pngImage = NULL;
	return TRUE;

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
	void commonCube(void);
	void blackboard(void);

	glClear(GL_COLOR_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -3.0f);
	//

	glPushMatrix();

	{
	glPushMatrix();
	glTranslatef(0.0f, -15.0f, 5.0f);
	glScalef(25.0f, 1.3f, 30.0f);
	glBindTexture(GL_TEXTURE_2D, schoolWallTexture);
	commonCube();
	glBindTexture(GL_TEXTURE_2D, 0);
	glPopMatrix();
	}
	glPopMatrix();

	//blackboard
	glPushMatrix();

	{
		glPushMatrix();
		glTranslatef(0.0f, -5.0f, 5.0f);
		glScalef(10.0f, 0.5f, 34.0f);
		//glBindTexture(GL_TEXTURE_2D, schoolWallTexture);
		glColor3f(0.0f, 0.0f, 0.0f);
		blackboard();
		glColor3f(1.0f, 1.0f, 1.0f);
		//glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();
	}
	glPopMatrix();

	// a aa aai
	if (firstScene == TRUE) {
	glPushMatrix();

	{
		glPushMatrix();
		glTranslatef(0.0f, -5.0f, firstSceneTrans);
		glScalef(10.0f, 0.5f, 34.0f);
		glBindTexture(GL_TEXTURE_2D, a_aa_aai);
		//glColor3f(0.0f, 0.0f, 0.0f);
		commonCube();
	    glBindTexture(GL_TEXTURE_2D, 0);
		//secondScene = TRUE;
		glPopMatrix();
	}
	glPopMatrix();

	}
	
	if(secondScene == TRUE)
	{
		glPushMatrix();

		{
			glPushMatrix();
			glTranslatef(0.0f, -5.0f, secondSceneTrans);
			glScalef(10.0f, 0.5f, 34.0f);
			glBindTexture(GL_TEXTURE_2D, ma_ma_maka);
			//glColor3f(0.0f, 0.0f, 0.0f);
			commonCube();
			glBindTexture(GL_TEXTURE_2D, 0);
			//secondScene = TRUE;
			glPopMatrix();
		}
		glPopMatrix();
	}

	if (thirdScene == TRUE)
	{
		glPushMatrix();

		{
			glPushMatrix();
			glTranslatef(0.0f, -5.0f, thirdSceneTrans);
			glScalef(10.0f, 0.5f, 34.0f);
			glBindTexture(GL_TEXTURE_2D, mi_tuza_mama);
			//glColor3f(0.0f, 0.0f, 0.0f);
			commonCube();
			glBindTexture(GL_TEXTURE_2D, 0);
			//secondScene = TRUE;
			glPopMatrix();
		}
		glPopMatrix();
	}

	if (fourthScene == TRUE)
	{
		glPushMatrix();

		{
			glPushMatrix();
			glTranslatef(0.0f, -5.0f, fourthSceneTrans);
			glScalef(10.0f, 0.5f, 34.0f);
			glBindTexture(GL_TEXTURE_2D, pa_pa_patang);
			//glColor3f(0.0f, 0.0f, 0.0f);
			commonCube();
			glBindTexture(GL_TEXTURE_2D, 0);
			//secondScene = TRUE;
			glPopMatrix();
		}
		glPopMatrix();
	}

	if (fifthScene == TRUE)
	{
		glPushMatrix();

		{
			glPushMatrix();
			glTranslatef(0.0f, -5.0f, fifthSceneTrans);
			glScalef(10.0f, 0.5f, 34.0f);
			glBindTexture(GL_TEXTURE_2D, chandomama);
			//glColor3f(0.0f, 0.0f, 0.0f);
			commonCube();
			glBindTexture(GL_TEXTURE_2D, 0);
			//secondScene = TRUE;
			glPopMatrix();
		}
		glPopMatrix();
	}

	if (sixthScene == TRUE)
	{
		glPushMatrix();

		{
			glPushMatrix();
			glTranslatef(0.0f, -5.0f, sixthSceneTrans);
			glScalef(10.0f, 0.5f, 34.0f);
			glBindTexture(GL_TEXTURE_2D, ghadyaal);
			//glColor3f(0.0f, 0.0f, 0.0f);
			commonCube();
			glBindTexture(GL_TEXTURE_2D, 0);
			//secondScene = TRUE;
			glPopMatrix();
		}
		glPopMatrix();
	}


	if (seventhScene == TRUE)
	{
		glPushMatrix();

		{
			glPushMatrix();
			glTranslatef(0.0f, -5.0f, seventhSceneTrans);
			glScalef(10.0f, 0.5f, 34.0f);
			glBindTexture(GL_TEXTURE_2D, thava);
			//glColor3f(0.0f, 0.0f, 0.0f);
			commonCube();
			glBindTexture(GL_TEXTURE_2D, 0);
			//secondScene = TRUE;
			glPopMatrix();
		}
		glPopMatrix();
	}

	if (eighthScene == TRUE)
	{
		glPushMatrix();

		{
			glPushMatrix();
			glTranslatef(0.0f, -5.0f, eighthSceneTrans);
			glScalef(10.0f, 0.5f, 34.0f);
			glBindTexture(GL_TEXTURE_2D, baal);
			//glColor3f(0.0f, 0.0f, 0.0f);
			commonCube();
			glBindTexture(GL_TEXTURE_2D, 0);
			//secondScene = TRUE;
			glPopMatrix();
		}
		glPopMatrix();
	}



	//sparrow 1
	glPushMatrix();

	{
		glPushMatrix();
		glTranslatef(-16.0f, -10.0f, 5.0f);
		glScalef(8.0f, 0.6f, 34.0f);
		glBindTexture(GL_TEXTURE_2D, sparrowTexture);
		commonCube();
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();
	}
	glPopMatrix();

	glPushMatrix();

	{
		glPushMatrix();
		glTranslatef(6.0f, -10.0f, 5.0f);
		glScalef(3.0f, 0.3f, 34.0f);
		glBindTexture(GL_TEXTURE_2D, babySparrowTexture);
		commonCube();
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();
	}
	glPopMatrix();

	glPushMatrix();

	{
		glPushMatrix();
		glTranslatef(10.0f, -10.0f, 5.0f);
		glScalef(2.4f, 0.25f, 34.0f);
		glBindTexture(GL_TEXTURE_2D, babySparrowTexture);
		commonCube();
		glBindTexture(GL_TEXTURE_2D, 0);
		glPopMatrix();
	}
	glPopMatrix();



SwapBuffers(ghdc);

}
 
void update(void)
{
// Code
	if (firstScene == TRUE)
	{
	firstSceneTrans += 0.017f;
	if (firstSceneTrans >= -5.0f)
	{
		firstSceneTrans = -5.0f;
		secondScene = TRUE;
	}


	}
	if (secondScene == TRUE) {

		secondSceneTrans += 0.02f;
		if (secondSceneTrans > -5.0f)
		{
			secondScene = -5.0f;
			thirdScene = TRUE;
		}
		if (secondSceneTrans >= -70.0f)
		{
			firstScene = FALSE;
		}

	}
	if (thirdScene == TRUE) 
	{

		thirdSceneTrans += 0.02f;
		if (thirdSceneTrans > 2.0f)
		{
			thirdScene = 2.0f;
			fourthScene = TRUE;
		}
		if (thirdSceneTrans >= -20.0f)
		{
			secondScene = FALSE;
		}

	

	}
	if (fourthScene == TRUE)
	{

		fourthSceneTrans += 0.02f;
		if (fourthSceneTrans > 0.0f)
		{
			fourthScene = 0.0f;
			fifthScene = TRUE;
		}
		if (fourthSceneTrans >= -40.0f)
		{
			thirdScene = FALSE;
		}

	}

	if (fifthScene == TRUE)
	{

		fifthSceneTrans += 0.02f;
		if (fifthSceneTrans > -3.0f)
		{
			fifthScene = -3.0f;
			sixthScene = TRUE;
		}
		if (fifthSceneTrans >= -40.0f)
		{
			fourthScene = FALSE;
		}

	}

	if (sixthScene == TRUE)
	{

		sixthSceneTrans += 0.02f;
		if (sixthSceneTrans > -3.0f)
		{
			sixthScene = -3.0f;
			seventhScene = TRUE;
		}
		if (sixthSceneTrans >= -40.0f)
		{
		    fifthScene = FALSE;
		}

	}

	if (seventhScene == TRUE)
	{

		seventhSceneTrans += 0.02f;
		if (seventhSceneTrans > -3.0f)
		{
			seventhScene = -3.0f;
			eighthScene = TRUE;
		}
		if (seventhSceneTrans >= -40.0f)
		{
			sixthScene = FALSE;
		}

	}

	if (eighthScene == TRUE)
	{

		eighthSceneTrans += 0.02f;
		if (eighthSceneTrans > -3.0f)
		{
			eighthScene = -3.0f;
			//firstScene = TRUE;
		}
		if (eighthSceneTrans >= -40.0f)
		{
			seventhScene = FALSE;
		}

	}



}

void commonCube(void)
{
	//code

	//{
	//	glTexCoord2f(0.0f, 0.0f);
	//	glVertex3f(1.0f, 24.0f, 1.0f);
	//	glTexCoord2f(1.0f, 0.0f);
	//	glVertex3f(-1.0f, 24.0f, 1.0f);
	//	glTexCoord2f(1.0f, 1.0f);
	//	glVertex3f(-1.0f, -.0f, 1.0f);
	//	glTexCoord2f(0.0f, 1.0f);
	//	glVertex3f(1.0f, -1.0f, 1.0f);
	//}
	//// right face
	/*{
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(1.0f, 24.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, 24.0f, 1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, 1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
	}*/
	// Back face
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, 24.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, 24.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
	}
	glEnd();
	//for black board

}

void blackboard(void)
{
	glBegin(GL_QUADS);
	{
		glTexCoord2f(0.0f, 0.0f);
		glVertex3f(-1.0f, 24.0f, -1.0f);
		glTexCoord2f(1.0f, 0.0f);
		glVertex3f(1.0f, 24.0f, -1.0f);
		glTexCoord2f(1.0f, 1.0f);
		glVertex3f(1.0f, -1.0f, -1.0f);
		glTexCoord2f(0.0f, 1.0f);
		glVertex3f(-1.0f, -1.0f, -1.0f);
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



