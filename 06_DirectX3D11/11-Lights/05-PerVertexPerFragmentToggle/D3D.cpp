#include<Windows.h> // Windows header files 
#include<stdlib.h> // For exit
#include<stdio.h> // For file IO (input output)

#include "D3D.h"
#include "Sphere.h"

// D3D related headerfiles
#include<d3d11.h>
#include<d3dcompiler.h>

#pragma warning(disable:4838)  // supress warning came from inl files of XNAMATH
#include"XNAMath_204/xnamath.h"

#pragma comment(lib , "d3d11.lib")
#pragma comment(lib , "d3dcompiler.lib")
#pragma comment(lib,"Sphere.lib")

// MACROS
#define WIN_WIDTH 800
#define WIN_HEIGHT 600



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

char gszLogFileName[] = "Log.txt";

// DirectX variables
IDXGISwapChain* gpIDXGISwapChain = NULL; // DirectX Graphics infrastructure
ID3D11Device* gpID3D11Device = NULL;
ID3D11DeviceContext* gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView* gpID3D11RenderTargetView = NULL;
float ClearColor[4];

ID3D11VertexShader* gpID3D11VertexShader_pv = NULL;
ID3D11PixelShader* gpID3D11PixelShader_pv = NULL;
ID3D11VertexShader* gpID3D11VertexShader_pf = NULL;
ID3D11PixelShader* gpID3D11PixelShader_pf = NULL;
ID3D11InputLayout* gpID3D11InputLayout = NULL;
ID3D11Buffer* gpID3D11Buffer_positionBuffer = NULL;
ID3D11Buffer* gpID3D11Buffer_normalBuffer = NULL;
ID3D11Buffer* gpID3D11Buffer_elementBuffer = NULL;
ID3D11Buffer* gpID3D11Buffer_constantBuffer_pv = NULL;
ID3D11Buffer* gpID3D11Buffer_constantBuffer_pf = NULL;

struct CBUFFER {
	XMMATRIX WorldMatrix;	
	XMMATRIX ViewMatrix;	
	XMMATRIX ProjectionMatrix;
	
	XMVECTOR lightAmbientUniform;
	XMVECTOR lightDiffuseUniform;
	XMVECTOR lightSpecularUniform;
	XMVECTOR lightPossitionUniform;

	XMVECTOR materialAmbientUniform;
	XMVECTOR materialDiffuseUniform;
	XMVECTOR materialSpecularUniform;
	FLOAT materialShininessUniform;

	unsigned int KeyPress;
};

BOOL bLightingEnable = FALSE;
BOOL bPerVertexEnable = TRUE;
BOOL bPerFragmentEnable = FALSE;

float lightAmbient[] = { 1.0f,1.0f,1.0f,1.0f };
float lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
float lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
float lightPossition[] = { 100.0f,100.0f,-100.0f,1.0f };

float  materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
float  materialDiffuse[] = { 0.5f,0.2f,0.7f,1.0f };
float  materialSpecular[] = { 0.7f,0.7f,0.7f,1.0f };
float  materialShininess = 128.0f;

XMMATRIX perspectiveProjectionMatrix;

// Sphere variables
ID3D11Buffer* gpID3D11Buffer_IndexBuffer = NULL;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumElements;
unsigned int gNumVertices;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCnmdLine, int iCmdShow) // function defition
{

	// Function Declarations
	
	HRESULT initialize(void);
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

	HRESULT hr = S_OK;

	if (fopen_s(&gpFile , gszLogFileName, "w") != 0)
	{
		MessageBox(NULL, TEXT("Log file cannot be open!!!"), TEXT("File create error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	else
	{
		fprintf(gpFile, "Program started successfully \n");
		fclose(gpFile);
	}


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

	hr = initialize();

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Initialize failed!!!");
		fclose(gpFile);
		DestroyWindow(hwnd);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Initialize is Success !!!");
		fclose(gpFile);
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

	HRESULT resize(int , int);
	void ToggleFullscreen(void);

	HDC hdc;
	PAINTSTRUCT ps;
	RECT rc;
	TCHAR str[] = TEXT("Hello World !!!");

	HRESULT hr = S_OK;

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
		if(gpID3D11DeviceContext)
		{
			hr = resize(LOWORD(lParam) , HIWORD(lParam));
		
			if(FAILED(hr))
			{
				gpFile = fopen(gszLogFileName, "a+");
				fprintf(gpFile , "Resize Failed !!!");
				fclose(gpFile);

				return hr;
			}
			else
			{
				gpFile = fopen(gszLogFileName, "a+");
				fprintf(gpFile , "Resize is Success !!!");
				fclose(gpFile);
			}

		}

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
		case 'Q':
		case 'q':
			DestroyWindow(hwnd);
			break;

		case 'l':
		case 'L':
			if (bLightingEnable == FALSE)
			{
				bLightingEnable = TRUE;
				bPerVertexEnable = TRUE;
			}
			else
			{
				bLightingEnable = FALSE;
			}
			break;
		case 'v':
		case 'V':
			if (bPerVertexEnable == FALSE)
			{
				bPerVertexEnable = TRUE;
				bPerFragmentEnable = FALSE;

			}
			else
			{
				bPerVertexEnable = FALSE;
			}
			break;
		case 'f':
		case 'F':
			if (bPerFragmentEnable == FALSE)
			{
				bPerFragmentEnable = TRUE;
				bPerVertexEnable = FALSE;

			}
			else
			{
				bPerFragmentEnable = FALSE;
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

HRESULT initialize(void)
{
	// Function declarations
	HRESULT resize(int width, int height);

	// variable declarations
	HRESULT hr = S_OK;
	ID3DBlob* pID3DBlob_vertexShaderSourceCode_pv = NULL;
	ID3DBlob* pID3DBlob_pixelShaderSourceCode_pv = NULL;
	ID3DBlob* pID3DBlob_vertexShaderSourceCode_pf = NULL;
	ID3DBlob* pID3DBlob_pixelShaderSourceCode_pf = NULL;
	ID3DBlob* pID3DBlob_Error = NULL;

	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	ZeroMemory((void*)&dxgiSwapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.BufferCount = 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.Windowed = TRUE;

	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] = {
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP, // WINDOWS Advanced Rasterizer Platform
			D3D_DRIVER_TYPE_SOFTWARE,
			D3D_DRIVER_TYPE_REFERENCE,
			D3D_DRIVER_TYPE_NULL,
			D3D_DRIVER_TYPE_UNKNOWN
	};
	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_accquired = D3D_FEATURE_LEVEL_10_0;
	UINT numDriverTypes;
	UINT numFeatureLevels = 1;
	
	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);
	for (UINT i = 0; i < numDriverTypes; i++)
	{
		d3dDriverType = d3dDriverTypes[i];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			d3dDriverType,
			NULL,
			0,
			&d3dFeatureLevel_required,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			&dxgiSwapChainDesc,
			&gpIDXGISwapChain,
			&gpID3D11Device,
			&d3dFeatureLevel_accquired,
			&gpID3D11DeviceContext
		);
		if (SUCCEEDED(hr))
		{
			break;
		}
		
	}
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "D3D11CreateDeviceAndSwapChain function failed...\n");
		fclose(gpFile);
		return(hr);
	}
	else {
		gpFile = fopen(gszLogFileName, "a+");
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf(gpFile, "Hardware driver found...\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf(gpFile, "WARP driver found...\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_SOFTWARE)
		{
			fprintf(gpFile, "Hardware driver found...\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf(gpFile, "Refference driver found...\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_NULL)
		{
			fprintf(gpFile, "NULL driver found...\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_UNKNOWN)
		{
			fprintf(gpFile, "Unknown driver found...\n");
		}
		else {
			fprintf(gpFile, "UNDEFINED Driver Found \n");

		}

		if (d3dFeatureLevel_accquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf(gpFile, "11.0 feature level found\n");
		}
		else if (d3dFeatureLevel_accquired == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf(gpFile, "10.1 feature level found\n");
		}
		else if (d3dFeatureLevel_accquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf(gpFile, "10.0 feature level found\n");
		}
		else
		{
			fprintf(gpFile, "Unknow fetuare level found\n");
			fclose(gpFile);
		}
	}

	// *************************************************************************
	// ************* PER PIXEL **********************************************
	// *************************************************************************
	// Vertex Shader

	const char* vertexShaderSourceCode_pf =
		"cbuffer ConstantBuffer"\
		"{"\
		"float4x4 WorldMatrix;"\
		"float4x4 ViewMatrix;"\
		"float4x4 ProjectionMatrix;"\
		"float4 lightAmbieant;"\
		"float4 lightDiffuse;"\
		"float4 lightSpecular;"\
		"float4 lightPosition;"\
		"float4 materialAmbieant;"\
		"float4 materialDiffuse;"\
		"float4 materialSpecular;"\
		"float materialShininess;"\
		"uint keyPress;"\
		"}"\
		"struct vertex_output"\
		"{"\
		"float4 position:SV_POSITION;"\
		"float3 transformedNormals:NORMAL0;"\
		"float3 lightDirection:NORMAL1;"\
		"float3 viewerVector:NORMAL2;"\
		"};"\
		"vertex_output main(float4 pos:POSITION, float3 norm:NORMAL)"\
		"{"\
		"vertex_output output;"\
		"if(keyPress == 1)"\
		"{"\
		"float4  eyeCoordinates = mul(ViewMatrix, mul(WorldMatrix, pos));"\
		"output.transformedNormals = mul((float3x3)WorldMatrix, norm);"\
		"output.lightDirection = (float3)(lightPosition - eyeCoordinates);"\
		"output.viewerVector = -eyeCoordinates.xyz;"\
		"}"\
		"else"\
		"{"\
		"output.transformedNormals = float3(0.0,0.0,0.0);"\
		"output.lightDirection = float3(0.0,0.0,0.0);"\
		"output.viewerVector = float3(0.0,0.0,0.0);"\
		"}"\
		"float4 position = mul(ProjectionMatrix, mul(ViewMatrix, mul(WorldMatrix, pos)));"
		"output.position = position;"\
		"return(output);"\
		"}";

	hr = D3DCompile(
		vertexShaderSourceCode_pf,
		lstrlenA(vertexShaderSourceCode_pf) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_vertexShaderSourceCode_pf,
		&pID3DBlob_Error
	);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pf vertex Shader compilation error %s\n...", (char*)pID3DBlob_Error->GetBufferPointer());
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pf Vertex shader succeded...\n");
		fclose(gpFile);
	}

	// Create the vertex shader from above code
	hr = gpID3D11Device->CreateVertexShader(
		pID3DBlob_vertexShaderSourceCode_pf->GetBufferPointer(),
		pID3DBlob_vertexShaderSourceCode_pf->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader_pf);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pf CreateVertexShader function failed...");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pf CreateVertexShader function succeded...\n");
		fclose(gpFile);

	}
	// set vertex shader into pipeline


	// Pixel Shader
	const char* pixelShaderSourceCode_pf =
		"cbuffer ConstantBuffer"\
		"{"\
		"float4x4 WorldMatrix;"\
		"float4x4 ViewMatrix;"\
		"float4x4 ProjectionMatrix;"\
		"float4 lightAmbieant;"\
		"float4 lightDiffuse;"\
		"float4 lightSpecular;"\
		"float4 lightPosition;"\
		"float4 materialAmbieant;"\
		"float4 materialDiffuse;"\
		"float4 materialSpecular;"\
		"float materialShininess;"\
		"uint keyPress;"\
		"}"\
		"struct vertex_output"\
		"{"\
		"float4 position:SV_POSITION;"\
		"float3 transformedNormals:NORMAL0;"\
		"float3 lightDirection:NORMAL1;"\
		"float3 viewerVector:NORMAL2;"\
		"};"\
		"float4 main(vertex_output input):SV_TARGET"\
		"{"\
		"float3 PhongADSLight;"\
		"if(keyPress == 1)"\
		"{"\
		"float3 normalizedTransformedNormals = normalize(input.transformedNormals);"\
		"float3 normalizedLightDirection = normalize(input.lightDirection);"\
		"float3 normalizedViewerVector = normalize(input.viewerVector);"\
		"float3 reflectionVector = reflect(-normalizedLightDirection, normalizedTransformedNormals);"\
		"float3 ambientLight = lightAmbieant * materialAmbieant;"\
		"float3 diffuseLight = lightDiffuse * materialDiffuse * max(dot(normalizedLightDirection,normalizedTransformedNormals), 0.0);"\
		"float3 specularLight = lightSpecular * materialSpecular * pow(max(dot(reflectionVector,normalizedViewerVector), 0.0),materialShininess);"\
		"PhongADSLight = ambientLight + diffuseLight + specularLight;"\
		"}"\
		"else"\
		"{"\
		"PhongADSLight = float3(1.0f,1.0f,1.0f);"\
		"}"\
		"float4 color = float4(PhongADSLight, 1.0);"\
		"return(color);"\
		"}";

	pID3DBlob_Error = NULL;

	hr = D3DCompile(
		pixelShaderSourceCode_pf,
		lstrlenA(pixelShaderSourceCode_pf) + 1,
		"ps",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_pixelShaderSourceCode_pf,
		&pID3DBlob_Error
	);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pf pixel Shader compilation error %s\n...", (char*)pID3DBlob_Error->GetBufferPointer());
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pf pixel shader succeded...\n");
		fclose(gpFile);
	}

	// Create the vertex shader from above code
	hr = gpID3D11Device->CreatePixelShader(
		pID3DBlob_pixelShaderSourceCode_pf->GetBufferPointer(),
		pID3DBlob_pixelShaderSourceCode_pf->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader_pf);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pf CreatePixelShader function failed...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pf CreatePixelShader function succeded...\n");
		fclose(gpFile);

	}
	// set vertex shader into pipeline

	// release error blob
	if (pID3DBlob_Error)
	{
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
	}
	// release pixel shader blob
	if (pID3DBlob_pixelShaderSourceCode_pf)
	{
		pID3DBlob_pixelShaderSourceCode_pf->Release();
		pID3DBlob_pixelShaderSourceCode_pf = NULL;
	}
	// *************************************************************************
	// ************* PER VERTEX ************************************************
	// *************************************************************************
	// Vertex Shader
	const char* vertexShaderSourceCode_pv =
		"cbuffer ConstantBuffer"\
		"{"\
		"float4x4 WorldMatrix;"\
		"float4x4 ViewMatrix;"\
		"float4x4 ProjectionMatrix;"\
		"float4 lightAmbieant;"\
		"float4 lightDiffuse;"\
		"float4 lightSpecular;"\
		"float4 lightPosition;"\
		"float4 materialAmbieant;"\
		"float4 materialDiffuse;"\
		"float4 materialSpecular;"\
		"float materialShininess;"\
		"uint keyPress;"\
		"}"\
		"struct vertex_output"\
		"{"\
		"float4 position:SV_POSITION;"\
		"float3 PhongADSLight:COLOR;"\
		"};"\
		"vertex_output main(float4 pos:POSITION, float3 norm:NORMAL)"\
		"{"\
		"vertex_output output;"\
		"if(keyPress == 1)"\
		"{"\
		"float4  eyeCoordinates = mul(ViewMatrix, mul(WorldMatrix, pos));"\
		"float3 transformedMatrix = normalize(mul((float3x3)WorldMatrix, norm));"\
		"float3 lightDirection = normalize((float3)(lightPosition - eyeCoordinates));"\
		"float3 reflectionVector = reflect(-lightDirection, transformedMatrix);"\
		"float3 viewerVector = normalize(-eyeCoordinates.xyz);"\
		"float3 ambientLight = lightAmbieant * materialAmbieant;"\
		"float3 diffuseLight = lightDiffuse * materialDiffuse * max(dot(lightDirection,transformedMatrix), 0.0);"\
		"float3 specularLight = lightSpecular * materialSpecular * pow(max(dot(reflectionVector,viewerVector), 0.0),materialShininess);"\
		"output.PhongADSLight = ambientLight + diffuseLight + specularLight;"\
		"}"\
		"else"\
		"{"\
		"output.PhongADSLight = float3(1.0f,1.0f,1.0f);"\
		"}"\
		"float4 position = mul(ProjectionMatrix, mul(ViewMatrix, mul(WorldMatrix, pos)));"
		"output.position = position;"\
		"return(output);"\
		"}";

	hr = D3DCompile(
		vertexShaderSourceCode_pv,
		lstrlenA(vertexShaderSourceCode_pv) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_vertexShaderSourceCode_pv,
		&pID3DBlob_Error
	);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pv vertex Shader compilation error %s\n...", (char*)pID3DBlob_Error->GetBufferPointer());
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pv Vertex shader succeded...\n");
		fclose(gpFile);
	}

	// Create the vertex shader from above code
	hr = gpID3D11Device->CreateVertexShader(
		pID3DBlob_vertexShaderSourceCode_pv->GetBufferPointer(),
		pID3DBlob_vertexShaderSourceCode_pv->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader_pv);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateVertexShader function failed...");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateVertexShader function succeded...\n");
		fclose(gpFile);

	}
	// set vertex shader into pipeline


	// Pixel Shader
	const char* pixelShaderSourceCode_pv =
		"struct vertex_output"\
		"{"\
		"float4 position:SV_POSITION;"\
		"float3 PhongADSLight:COLOR;"\
		"};"\
		"float4 main(vertex_output input):SV_TARGET"\
		"{"\
		"float4 color = float4(input.PhongADSLight, 1.0);"\
		"return(color);"\
		"}";

	pID3DBlob_Error = NULL;

	hr = D3DCompile(
		pixelShaderSourceCode_pv,
		lstrlenA(pixelShaderSourceCode_pv) + 1,
		"ps",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_pixelShaderSourceCode_pv,
		&pID3DBlob_Error
	);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pv pixel Shader compilation error %s\n...", (char*)pID3DBlob_Error->GetBufferPointer());
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pv pixel shader succeded...\n");
		fclose(gpFile);
	}

	// Create the vertex shader from above code
	hr = gpID3D11Device->CreatePixelShader(
		pID3DBlob_pixelShaderSourceCode_pv->GetBufferPointer(),
		pID3DBlob_pixelShaderSourceCode_pv->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader_pv);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pv CreatePixelShader function failed...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pv CreatePixelShader function succeded...\n");
		fclose(gpFile);

	}
	// set vertex shader into pipeline

	// release error blob
	if (pID3DBlob_Error)
	{
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
	}
	// release pixel shader blob
	if (pID3DBlob_pixelShaderSourceCode_pv)
	{
		pID3DBlob_pixelShaderSourceCode_pv->Release();
		pID3DBlob_pixelShaderSourceCode_pv = NULL;
	}


	// initialize input element structure
	D3D11_INPUT_ELEMENT_DESC d3dInputElementDesc[2];
	ZeroMemory((void*)&d3dInputElementDesc, sizeof(d3dInputElementDesc) * _ARRAYSIZE(d3dInputElementDesc));

	// position
	d3dInputElementDesc[0].SemanticName = "POSITION";
	d3dInputElementDesc[0].SemanticIndex = 0;
	d3dInputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	d3dInputElementDesc[0].InputSlot = 0;
	d3dInputElementDesc[0].AlignedByteOffset = 0;
	d3dInputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3dInputElementDesc[0].InstanceDataStepRate = 0;

	// normal
	d3dInputElementDesc[1].SemanticName = "NORMAL";
	d3dInputElementDesc[1].SemanticIndex = 0;
	d3dInputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	d3dInputElementDesc[1].InputSlot = 1;
	d3dInputElementDesc[1].AlignedByteOffset = 0;
	d3dInputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3dInputElementDesc[1].InstanceDataStepRate = 0;

	// create input layout using abbove
	
	hr = gpID3D11Device->CreateInputLayout(
		d3dInputElementDesc,
		_ARRAYSIZE(d3dInputElementDesc),
		pID3DBlob_vertexShaderSourceCode_pv->GetBufferPointer(),
		pID3DBlob_vertexShaderSourceCode_pv->GetBufferSize(),
		&gpID3D11InputLayout
	);
	
	hr = gpID3D11Device->CreateInputLayout(
		d3dInputElementDesc,
		_ARRAYSIZE(d3dInputElementDesc),
		pID3DBlob_vertexShaderSourceCode_pf->GetBufferPointer(),
		pID3DBlob_vertexShaderSourceCode_pf->GetBufferSize(),
		&gpID3D11InputLayout
	);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateInputLayout function failed...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateInputLayout function succeded...\n");
		fclose(gpFile);

	}

	// set above input layout in pipeline
	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);

	// now release vertexShaderSourcodeBolb
	if (pID3DBlob_vertexShaderSourceCode_pv)
	{
		pID3DBlob_vertexShaderSourceCode_pv->Release();
		pID3DBlob_vertexShaderSourceCode_pv = NULL;
	}
	// now release vertexShaderSourcodeBolb
	if (pID3DBlob_vertexShaderSourceCode_pf)
	{
		pID3DBlob_vertexShaderSourceCode_pf->Release();
		pID3DBlob_vertexShaderSourceCode_pf = NULL;
	}
	

	getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
	gNumVertices = getNumberOfSphereVertices();
	gNumElements = getNumberOfSphereElements();
	
	// create vertex bufffer for position
	// Position VB
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = gNumVertices * 3 * sizeof(float);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
	ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3d11SubresourceData.pSysMem = sphere_vertices;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, &d3d11SubresourceData, &gpID3D11Buffer_positionBuffer);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer position function failed...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer position function succeded...\n");
		fclose(gpFile);
	}
	

	// ************************************** COLOR ***************************************
	// ************************************************************************************
	 // Normals VB
	// Follow Same Above Method For Normals And Textures
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = gNumVertices * 3 * sizeof(float);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3d11SubresourceData.pSysMem = sphere_normals;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, &d3d11SubresourceData, &gpID3D11Buffer_positionBuffer);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer normal function failed...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer normal function succeded...\n");
		fclose(gpFile);
	}
	// we will set this buffer in display because its dynamic

	

	// ************************************** Element ***************************************
	// ************************************************************************************
	// create index buffer
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = gNumElements * sizeof(unsigned short);
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3d11SubresourceData.pSysMem = sphere_elements;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, &d3d11SubresourceData, &gpID3D11Buffer_IndexBuffer);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer element function failed...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer element function succeded...\n");
		fclose(gpFile);
	}
	
	// Constant buffer for per vertex ********************************************
	// Create constant buffer to send transformation 
	ZeroMemory((void*)&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// create vertex bufffer from above
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_constantBuffer_pv);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer function failed for gpID3D11Buffer_constantBuffer_pv...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer function succeded for gpID3D11Buffer_constantBuffer_pv...\n");
		fclose(gpFile);
	}

	// set this into pipeline
	//gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pv);
	//gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pv);


	// Constant buffer for per pixel ********************************************
	// Create constant buffer to send transformation 
	ZeroMemory((void*)&bufferDesc, sizeof(bufferDesc));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// create vertex bufffer from above
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_constantBuffer_pf);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer function failed for gpID3D11Buffer_constantBuffer_pf...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer function succeded for gpID3D11Buffer_constantBuffer_pf...\n");
		fclose(gpFile);
	}

	// set this into pipeline
	//gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pf);
	//gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pf);


	// Define clear color
	ClearColor[0] = 0.0f;
	ClearColor[1] = 0.0f;
	ClearColor[2] = 0.0f;
	ClearColor[3] = 1.0f;

	perspectiveProjectionMatrix = XMMatrixIdentity();

	// warm up resize
	hr = resize(WIN_WIDTH, WIN_HEIGHT);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "resize function failed...");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "resize function succeded...");
		fclose(gpFile);

	}

	return(hr);
}

HRESULT resize(int width, int height)
{
	// variable declarations
	HRESULT hr = S_OK;

	// Code
	if (height <= 0)
		height = 1;
	
	// Release RTV
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	// resize the swapchain buffer according to the new size
	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

	
	// a
	ID3D11Texture2D *pID3D11Texture2D = NULL;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pID3D11Texture2D);

	// b
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D, NULL, &gpID3D11RenderTargetView);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateRenderTargetView function failed...");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateRenderTargetView function succeded...");
		fclose(gpFile);

	}
	pID3D11Texture2D->Release();
	pID3D11Texture2D = NULL;

	// c
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, NULL);

	// set view port
	D3D11_VIEWPORT d3dViewPort;
	ZeroMemory((void*)&d3dViewPort, sizeof(D3D10_VIEWPORT));
	d3dViewPort.TopLeftX = 0.0f;
	d3dViewPort.TopLeftY = 0.0f;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;	
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;

	// set above viewport in pipeline
	gpID3D11DeviceContext->RSSetViewports(1, &d3dViewPort);

	// initialize perspective

	perspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	
	return(hr);
}

void display(void)
{
	// Code
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, ClearColor);

	if (bPerVertexEnable == TRUE) {
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_pv, NULL, 0);
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_pv, NULL, 0);
		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pv);
		gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pv);

		// Transformation
		XMMATRIX worldMatrix = XMMatrixIdentity();
		XMMATRIX viewMatrix = XMMatrixIdentity();
		worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 5.0f);

		//struct CBUFFER cBuffer = gpID3D11Buffer_constantBuffer_pv;
		struct CBUFFER cBuffer;
		ZeroMemory(&cBuffer, sizeof(CBUFFER));
		cBuffer.WorldMatrix = worldMatrix;
		cBuffer.ViewMatrix = viewMatrix;
		cBuffer.ProjectionMatrix = perspectiveProjectionMatrix;

		if (bLightingEnable == TRUE)
		{
			cBuffer.KeyPress = 1;

			cBuffer.lightAmbientUniform = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], 0.0);
			cBuffer.lightDiffuseUniform = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], 0.0);
			cBuffer.lightSpecularUniform = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], 0.0);
			cBuffer.lightPossitionUniform = XMVectorSet(lightPossition[0], lightPossition[1], lightPossition[2], lightPossition[3]);

			cBuffer.materialAmbientUniform = XMVectorSet(materialAmbient[0], materialAmbient[1], materialAmbient[2], 0.0);
			cBuffer.materialDiffuseUniform = XMVectorSet(materialDiffuse[0], materialDiffuse[1], materialDiffuse[2], 0.0);
			cBuffer.materialSpecularUniform = XMVectorSet(materialSpecular[0], materialSpecular[1], materialSpecular[2], 0.0);
			cBuffer.materialShininessUniform = materialShininess;
		}
		else
		{
			cBuffer.KeyPress = 0;
		}
		// Uniform4fv(..);
		gpID3D11DeviceContext->UpdateSubresource(
			gpID3D11Buffer_constantBuffer_pv,
			0,
			NULL,
			&cBuffer,
			0,
			0
		);
	}
	else {
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_pf, NULL, 0);
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_pf, NULL, 0);
		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pf);
		gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pf);

		// Transformation
		XMMATRIX worldMatrix = XMMatrixIdentity();
		XMMATRIX viewMatrix = XMMatrixIdentity();
		worldMatrix = XMMatrixTranslation(0.0f, 0.0f, 5.0f);

		//struct CBUFFER cBuffer = gpID3D11Buffer_constantBuffer_pv;
		struct CBUFFER cBuffer;
		ZeroMemory(&cBuffer, sizeof(CBUFFER));
		cBuffer.WorldMatrix = worldMatrix;
		cBuffer.ViewMatrix = viewMatrix;
		cBuffer.ProjectionMatrix = perspectiveProjectionMatrix;

		if (bLightingEnable == TRUE)
		{
			cBuffer.KeyPress = 1;

			cBuffer.lightAmbientUniform = XMVectorSet(lightAmbient[0], lightAmbient[1], lightAmbient[2], 0.0);
			cBuffer.lightDiffuseUniform = XMVectorSet(lightDiffuse[0], lightDiffuse[1], lightDiffuse[2], 0.0);
			cBuffer.lightSpecularUniform = XMVectorSet(lightSpecular[0], lightSpecular[1], lightSpecular[2], 0.0);
			cBuffer.lightPossitionUniform = XMVectorSet(lightPossition[0], lightPossition[1], lightPossition[2], lightPossition[3]);

			cBuffer.materialAmbientUniform = XMVectorSet(materialAmbient[0], materialAmbient[1], materialAmbient[2], 0.0);
			cBuffer.materialDiffuseUniform = XMVectorSet(materialDiffuse[0], materialDiffuse[1], materialDiffuse[2], 0.0);
			cBuffer.materialSpecularUniform = XMVectorSet(materialSpecular[0], materialSpecular[1], materialSpecular[2], 0.0);
			cBuffer.materialShininessUniform = materialShininess;
		}
		else
		{
			cBuffer.KeyPress = 0;
		}
		gpID3D11DeviceContext->UpdateSubresource(
			gpID3D11Buffer_constantBuffer_pf,
			0,
			NULL,
			&cBuffer,
			0,
			0
		);
	}

	
	// SET POSITION buffer here
	UINT Strid = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(
		0,
		1,
		&gpID3D11Buffer_positionBuffer,
		&Strid,
		&offset);

	// SET Normal buffer here
	Strid = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(
		1,
		1,
		&gpID3D11Buffer_positionBuffer,
		&Strid,
		&offset);

	// set index buffer
	gpID3D11DeviceContext->IASetIndexBuffer(
		gpID3D11Buffer_IndexBuffer,
		DXGI_FORMAT_R16_UINT,
		0); // R16 maps with 'short'

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// draw
	gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);


	gpIDXGISwapChain->Present(0, 0);
}

void update(void)
{
	// code

}

void uninitialized(void)
{
	// Function declaration
	void ToggleFullscreen(void);

	// Code
	if (gpID3D11Buffer_IndexBuffer)
	{
		gpID3D11Buffer_IndexBuffer->Release();
		gpID3D11Buffer_IndexBuffer = NULL;
	}
	if (gpID3D11Buffer_constantBuffer_pv)
	{
		gpID3D11Buffer_constantBuffer_pv->Release();
		gpID3D11Buffer_constantBuffer_pv = NULL;
	}
	if (gpID3D11Buffer_normalBuffer)
	{
		gpID3D11Buffer_normalBuffer->Release();
		gpID3D11Buffer_normalBuffer = NULL;
	}
	if (gpID3D11Buffer_positionBuffer)
	{
		gpID3D11Buffer_positionBuffer->Release();
		gpID3D11Buffer_positionBuffer = NULL;
	}
	if (gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}
	if (gpID3D11PixelShader_pv)
	{
		gpID3D11PixelShader_pv->Release();
		gpID3D11PixelShader_pv = NULL;
	}
	if (gpID3D11VertexShader_pv)
	{
		gpID3D11VertexShader_pv->Release();
		gpID3D11VertexShader_pv = NULL;
	}
	// Release RTV
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}
	if (gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}
	if (gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}
	if (gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}
	// If application is exiting in fullscreen then this code is execute
	if (gbFullscreen == TRUE)
	{
		ToggleFullscreen();
		gbFullscreen = FALSE;
	}

	// Destroy window
	if (ghwnd)
	{
		DestroyWindow(ghwnd);
		ghwnd = NULL;
	}

	gpFile = fopen(gszLogFileName, "a+");
	fprintf(gpFile, "gpFile Closed ...");
	fclose(gpFile);

}



