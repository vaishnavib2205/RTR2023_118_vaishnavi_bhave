#include<Windows.h> // Windows header files 
#include "OGL.h"
#include<stdlib.h> // For exit
#include<stdio.h> // For file IO (input output)



// OpenGL header files
#include<gl/glew.h> // this must be before gl/GL.h
#include<gl/GL.h>


// header file for vmath
#include "vmath.h"

using namespace vmath;

//// CUDA OpenGL interop related header files

#include<cuda_gl_interop.h>
#include<cuda_runtime.h>
#include"SineWave.cu.h"


// MACROS

#define WIN_WIDTH 800
#define WIN_HEIGHT 600

// Link with OpenGL libraray

#pragma comment(lib,"glew32.lib")
#pragma comment(lib,"OpenGL32.lib")

// cuda related library

#pragma comment(lib , "cudart.lib")



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

//GLuint vertexShaderObject = 0;
//GLuint fragmentShaderObject = 0;
GLuint shaderProgramObject = 0;

enum
{
	AMC_ATTRIBUTE_POSITION = 0
};

GLuint vao = 0;
GLuint vbo_position_y_axis = 0;

GLuint mvpMatrixUniform = 0;

mat4 perspectiveProjectionMatrix; // mat4 datatype is in vmath.h header file, that means 4X4 matrix


// cuda opengl interop related variable

#define MESH_ARRAY_SIZE 1024*1024*4

int mesh_width = 1024;
int mesh_height = 1024;

float position[1024][1024][4];

GLuint vbo_GPU;

BOOL bOnGPU = FALSE;
float animationTime = 0.0f;

struct cudaGraphicsResource* cuda_graphics_resource = NULL;
cudaError_t cudaResult;



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
	TCHAR szAppName[] = TEXT("PBWindow");
	int iResult = 0;
	BOOL bDone = FALSE;

	if (fopen_s(&gpFile , "Log.txt", "w") != 0)
	{
		MessageBox(NULL, TEXT("Log file cannot be open!!!"), TEXT("File create error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	fprintf(gpFile, "Program started successfully \n");
	fprintf(gpFile, "****************************************************************************************** \n");


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
		case 'C':
		case 'c':

			bOnGPU = FALSE;
			break;

		case 'G':
		case 'g':

			bOnGPU = TRUE;
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
	void printGLInfo(void);
	void uninitialized(void);

	// Code

	// check CUDA support and if supported select default CUDA device
    int dev_count;
	cudaResult = cudaGetDeviceCount(&dev_count);
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "cudaGetDeviceCount() failed\n");
		uninitialized();
		exit(0);
	}
	else if (dev_count == 0)
	{
		fprintf(gpFile, "No CUDA supported device found\n");
		uninitialized();
		exit(0);
	}

	else  // success
	{
		cudaSetDevice(0);   // 0th device is default
	}

	
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
	
	// initialize glew

	if (glewInit() != GLEW_OK)
	{
		fprintf(gpFile, "glewInit() failed to load GLEW \n");
		return -6;
	}

	// printGLInfo

	printGLInfo();

	// vertex shader

	// mat4 used in shader is inbuild in GLSL

	const GLchar* vertexShaderSourceCode = 
		"#version 460 core" \
		"\n" \
		"in vec4 aPosition;" \
		"uniform mat4 uMVPMatrix;" \
		"void main(void)" \
		"{" \
		"gl_Position = uMVPMatrix * aPosition;" \
		"}";

	

	GLuint vertexShaderObject = glCreateShader(GL_VERTEX_SHADER);

	glShaderSource(vertexShaderObject, 1, (const GLchar**)&vertexShaderSourceCode, NULL);

	glCompileShader(vertexShaderObject);

	// check for vertex shader compilation error if any

	GLint status = 0;
	GLint infoLogLength = 0;
	GLchar* szInfoLog = NULL;

	glGetShaderiv(vertexShaderObject, GL_COMPILE_STATUS, &status);
	if (status == FALSE)
	{
		glGetShaderiv(vertexShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(vertexShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Vertex Shader Compilation Error Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}	
		}
		uninitialized();
	}

	// fragment shader

	const GLchar* fragmentShaderSourceCode =
		"#version 460 core" \
		"\n" \
		"out vec4 FragColor;" \
		"void main(void)" \
		"{" \
		"FragColor = vec4(1.0 , 0.5 , 0.0 , 1.0);" \
		"}";

	GLuint fragmentShaderObject = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(fragmentShaderObject, 1, (const GLchar**)&fragmentShaderSourceCode, NULL);
	
	glCompileShader(fragmentShaderObject);

	// for fragment shader

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetShaderiv(fragmentShaderObject, GL_COMPILE_STATUS, &status);
	if (status == FALSE)
	{
		glGetShaderiv(fragmentShaderObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetShaderInfoLog(fragmentShaderObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Fragment Shader Compilation Error Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialized();
	}

	// create shader program

	shaderProgramObject = glCreateProgram();

	glAttachShader(shaderProgramObject, vertexShaderObject);
	glAttachShader(shaderProgramObject, fragmentShaderObject);

	glBindAttribLocation(shaderProgramObject, AMC_ATTRIBUTE_POSITION, "aPosition");


	glLinkProgram(shaderProgramObject);
	
	// for shader compilation

	status = 0;
	infoLogLength = 0;
	szInfoLog = NULL;

	glGetProgramiv(shaderProgramObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE)
	{
		glGetProgramiv(shaderProgramObject, GL_INFO_LOG_LENGTH, &infoLogLength);
		if (infoLogLength > 0)
		{
			szInfoLog = (GLchar*)malloc(infoLogLength);
			if (szInfoLog != NULL)
			{
				glGetProgramInfoLog(shaderProgramObject, infoLogLength, NULL, szInfoLog);
				fprintf(gpFile, "Shader Program linking Error Log : %s \n", szInfoLog);
				free(szInfoLog);
				szInfoLog = NULL;
			}
		}
		uninitialized();
	}

	// get shader uniform location

	mvpMatrixUniform = glGetUniformLocation(shaderProgramObject , "uMVPMatrix");


	// declare position and color arrays

	for(int i = 0; i < mesh_width ; i++)
	{
		for (int j = 0; j<mesh_height;j++)
		{
			for(int k=0; k<4 ;k++)
			{
				position[i][j][k] = 0.0f;
			}
		}
	}

	// VAO = vertex array object

	glGenVertexArrays(1, &vao);

	glBindVertexArray(vao);

	// VBO for cpu

	glGenBuffers(1, &vbo_position_y_axis);

	glBindBuffer(GL_ARRAY_BUFFER , vbo_position_y_axis);

	glBufferData(GL_ARRAY_BUFFER, MESH_ARRAY_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	// vbo for gpu

	glGenBuffers(1, &vbo_GPU);

	glBindBuffer(GL_ARRAY_BUFFER , vbo_GPU);

	glBufferData(GL_ARRAY_BUFFER, MESH_ARRAY_SIZE * sizeof(float), NULL, GL_DYNAMIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	// unbind with vao
	glBindVertexArray(0);


	// register opengl buffer to cuda for interop

	cudaResult = cudaGraphicsGLRegisterBuffer(&cuda_graphics_resource, vbo_GPU , cudaGraphicsMapFlagsWriteDiscard);  // writeonly
	if (cudaResult != cudaSuccess)
	{
		fprintf(gpFile, "cudaGraphicsGLRegisterBuffer() failed\n");
		uninitialized();
		exit(0);
	}







	// enabling depth 

	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);


	// set the clear colour of window to blue

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// Here OpenGL starts.....

	// initialise orthographicProjectionMatrix

	perspectiveProjectionMatrix = vmath::mat4::identity();


	resize(WIN_WIDTH, WIN_HEIGHT);

	return 0;
}

void printGLInfo(void)
{
	// variable declaration
	GLint numExtentions =0;
	GLint i;

	// code

	fprintf(gpFile, "OpenGL Vendor : %s \n", glGetString(GL_VENDOR));
	fprintf(gpFile, "OpenGL Renderer : %s \n", glGetString(GL_RENDERER));
	fprintf(gpFile, "OpenGL Version : %s \n", glGetString(GL_VERSION));
	fprintf(gpFile, "OpenGL GLSL Version : %s \n", glGetString(GL_SHADING_LANGUAGE_VERSION));

	fprintf(gpFile, "****************************************************************************************** \n");
	// listiong of supported extentions

	fprintf(gpFile, "Number of EXT. : %d \n", numExtentions);

	glGetIntegerv(GL_NUM_EXTENSIONS, &numExtentions);

	for (i = 0; i < numExtentions; i++)
	{
		fprintf(gpFile, "%s \n", glGetStringi(GL_EXTENSIONS , i));
	}

	fprintf(gpFile, "****************************************************************************************** \n");


}

void resize(int width, int height)
{
	// code
	if (height <= 0)
	{
		height = 1;
	}
	glViewport(0, 0, (GLsizei)width, (GLsizei)height);

	// set perspective projection matrix

	perspectiveProjectionMatrix = vmath::perspective(45.0f, (GLfloat)width / (GLfloat)height, 0.1f, 100.f);
	
}

void display(void)
{

	void launchCPUKernel(int , int , float);
	void uninitialized(void);

	// variable devlaration

	float4 *pPosition = NULL;

	
	// Code
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shaderProgramObject);


	// transformation

	mat4 modelViewMatrix = mat4::identity();

	mat4 modelViewProjectionMatrix = perspectiveProjectionMatrix * modelViewMatrix; // order of multiplication is very important

	// now push modelViewProjectionMatrix in the "uMVPMatrix" of shader , per frame.
	
	// push above mvp into vertex shader mvpMatrixUniform

	glUniformMatrix4fv(mvpMatrixUniform , 1 , GL_FALSE , modelViewProjectionMatrix);

	glBindVertexArray(vao);

	if(bOnGPU == TRUE)
	{
		// map cuda graphivs resource

		cudaResult = cudaGraphicsMapResources(1, &cuda_graphics_resource, 0);
		if (cudaResult != cudaSuccess)
		{
			fprintf(gpFile, "cudaGraphicsMapResources() failed\n");
			uninitialized();
			exit(0);
		}

		// get mapped resource cudaGraphicsMapResource Pointer usable in host code

		cudaResult = cudaGraphicsResourceGetMappedPointer((void**)&pPosition , NULL , cuda_graphics_resource);
		if(cudaResult != cudaSuccess)
		{
			fprintf(gpFile , "cuda cudaGraphicsResourceGetMappedPointer() filed \n");
			uninitialized();
			exit(0);
		}

		// launch the cuda kernel to write the data to the retrevied host mapped pointer 

		launchCUDAKernel(pPosition , mesh_width , mesh_height , animationTime);


		// unmap the host usable pointer to read on the host

		cudaResult = cudaGraphicsUnmapResources(1 , &cuda_graphics_resource , 0);
		if(cudaResult != cudaSuccess)
		{
			fprintf(gpFile , "cuda cudaGraphicsUnmapResources() filed \n");
			uninitialized();
			exit(0);
		}	

		//  re-bind with the opengl buffer object

		glBindBuffer(GL_ARRAY_BUFFER , vbo_GPU);

	}

	else
	{
		launchCPUKernel(mesh_width , mesh_height , animationTime);

		glBindBuffer(GL_ARRAY_BUFFER , vbo_position_y_axis);
		glBufferData(GL_ARRAY_BUFFER, MESH_ARRAY_SIZE * sizeof(float), position, GL_DYNAMIC_DRAW);
		
	}

	// to tell opengl where we mapped opengl buffer 
	glVertexAttribPointer(AMC_ATTRIBUTE_POSITION, 4, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(AMC_ATTRIBUTE_POSITION);

	glDrawArrays(GL_POINTS, 0, mesh_width * mesh_height);

	glBindBuffer(GL_ARRAY_BUFFER , 0);
	glBindVertexArray(0);

	glUseProgram(0);

	SwapBuffers(ghdc);

}

void launchCPUKernel(int width , int height , float time)
{
	// code

	for(int i = 0; i< width ; i++)
	{
		for(int j = 0; j<height; j++)
		{
			for(int k = 0; k<4; k++)
			{
				float u = (float) i / (float) width;
				float v = (float) j / (float) height;

				u = u * 2.0f - 1.0f; 
				v = v * 2.0f - 1.0f;

				float frequency = 4.0f;
				float w = sinf(u * frequency + time) * cosf(v * frequency + time) * 0.5f;

				if(k == 0)
				{
					position[i][j][k] = u; 
				}
				if(k == 1)
				{
					position[i][j][k] = w; // mapped with y
				}
				if(k == 2)
				{
					position[i][j][k] = v; 
				}
				if(k == 3)
				{
					position[i][j][k] = 1.0f;
				}
			}
		}
	}

}
 
void update(void)
{
	// Code
	animationTime += 0.01f;

}

void uninitialized(void)
{
	//  function declarations
	fprintf(gpFile, "****************************************************************************************** \n");
	// code

	if (shaderProgramObject)
	{
		glUseProgram(shaderProgramObject);

		GLint numShaders = 0;
		glGetProgramiv(shaderProgramObject, GL_ATTACHED_SHADERS, &numShaders);

		if (numShaders > 0)
		{
			GLuint* pShaders = (GLuint*)malloc(numShaders * sizeof(GLuint));
			if (pShaders != NULL)
			{
				glGetAttachedShaders(shaderProgramObject, numShaders, NULL, pShaders);

				for (GLint i = 0; i < numShaders; i++)
				{
					glDetachShader(shaderProgramObject, pShaders[i]);
					glDeleteShader(pShaders[i]);
					pShaders[i] = 0;
				}
				free(pShaders);
				pShaders = NULL;
			}
		}
		glUseProgram(0);
		glDeleteProgram(shaderProgramObject);
		shaderProgramObject = 0;
	}


	// delete gpu vbo 
	 
	if(vbo_GPU)
	{
		if(cuda_graphics_resource)
		{
			cudaGraphicsUnregisterResource(cuda_graphics_resource);
			cuda_graphics_resource = NULL;
		}

		glDeleteBuffers(1 , &vbo_GPU);
		vbo_GPU = 0;
	}

	// delete VBO of position

	if (vbo_position_y_axis)
	{
		glDeleteBuffers(1, &vbo_position_y_axis);
		vbo_position_y_axis = 0;
	}

	// delete VAO

	if (vao)
	{
		glDeleteVertexArrays(1, &vao);
		vao = 0;
	}

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



