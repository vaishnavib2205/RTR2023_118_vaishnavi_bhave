#include<Windows.h> // Windows header files 
#include<stdlib.h> // For exit
#include<stdio.h> // For file IO (input output)

#include "D3D.h"

// D3D related headerfiles
#include<d3d11.h>
#include<d3dcompiler.h>

#pragma warning(disable:4838)  // supress warning came from inl files of XNAMATH
#include"XNAMath_204/xnamath.h"

#pragma comment(lib , "d3d11.lib")
#pragma comment(lib , "d3dcompiler.lib")

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

ID3D11VertexShader* gpID3D11VertexShader = NULL;
ID3D11PixelShader* gpID3D11PixelShader = NULL;
ID3D11InputLayout* gpID3D11InputLayout = NULL;
ID3D11Buffer* gpID3D11Buffer_positionBuffer = NULL;
ID3D11Buffer* gpID3D11Buffer_normalBuffer = NULL;
ID3D11Buffer* gpID3D11Buffer_elementBuffer = NULL;
ID3D11Buffer* gpID3D11Buffer_constantBuffer = NULL;

struct CBUFFER {
	XMMATRIX WorldMatrix;	
	XMMATRIX ViewMatrix;	
	XMMATRIX ProjectionMatrix;
	
	XMVECTOR lightAmbientUniform[2];
	XMVECTOR lightDiffuseUniform[2];
	XMVECTOR lightSpecularUniform[2];
	XMVECTOR lightPossitionUniform[2];

	XMVECTOR materialAmbientUniform;
	XMVECTOR materialDiffuseUniform;
	XMVECTOR materialSpecularUniform;
	FLOAT materialShininessUniform;

	unsigned int KeyPress;

};

BOOL bLightingEnable = FALSE;

float lightAmbient[][4] = { {0.0f,0.0f,0.0f,0.0f},
							{0.0f,0.0f,0.0f,0.0f} };
float lightDiffuse[][4] = { {1.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,1.0f,0.0f} };
float lightSpecular[][4] = { {1.0f,0.0f,0.0f,0.0f},{0.0f,0.0f,1.0f,0.0f} };

float lightPossition[][4] = { {-2.0f,0.0f,0.0f,1.0f},{2.0f,0.0f,0.0f,1.0f} };

float  materialAmbient[] = { 0.0f,0.0f,0.0f,1.0f };
float  materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
float  materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
float  materialShininess = 128.0f;

XMMATRIX perspectiveProjectionMatrix;

// Sphere variables
ID3D11RasterizerState* gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView* gpID3D10DepthStencilView = NULL;

float angle = 0.0f;


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

		case 'l':
		case 'L':
			if (bLightingEnable == FALSE)
				bLightingEnable = TRUE;
			else
				bLightingEnable = FALSE;
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
	// code
	HRESULT resize(int , int);

	// Variable Declaration
	
	HRESULT hr = S_OK;
	ID3DBlob* pID3DBlob_VertexShaderSourceCode = NULL;
	ID3DBlob* pID3DBlob_PixelShaderSourceCode = NULL;
	ID3DBlob* pID3DBlob_Error = NULL;

	// initialize swapchain descriptor
	DXGI_SWAP_CHAIN_DESC  dxgiSwapChainDesc;

	ZeroMemory((void *)&dxgiSwapChainDesc , sizeof(DXGI_SWAP_CHAIN_DESC));

	dxgiSwapChainDesc.BufferDesc.Width = WIN_WIDTH;
	dxgiSwapChainDesc.BufferDesc.Height = WIN_HEIGHT;
	dxgiSwapChainDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	dxgiSwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	dxgiSwapChainDesc.SampleDesc.Count = 1;
	dxgiSwapChainDesc.SampleDesc.Quality = 0;
	dxgiSwapChainDesc.BufferCount= 1;
	dxgiSwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dxgiSwapChainDesc.OutputWindow = ghwnd;
	dxgiSwapChainDesc.Windowed = TRUE;

	// create d3d swapchain device , context , rendertargetview

	D3D_DRIVER_TYPE d3dDriverType;
	D3D_DRIVER_TYPE d3dDriverTypes[] =	
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_SOFTWARE,
		D3D_DRIVER_TYPE_REFERENCE,
		D3D_DRIVER_TYPE_NULL,
		D3D_DRIVER_TYPE_UNKNOWN
	};

	D3D_FEATURE_LEVEL d3dFeatureLevel_required = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL d3dFeatureLevel_acquired = D3D_FEATURE_LEVEL_10_0;

	UINT numDriverTypes;
	UINT numFeatureLevels = 1;

	numDriverTypes = sizeof(d3dDriverTypes) / sizeof(d3dDriverTypes[0]);

	for(UINT i=  0; i < numDriverTypes ; i++)
	{
		d3dDriverType = d3dDriverTypes[i];

		hr = D3D11CreateDeviceAndSwapChain(
			NULL ,
			d3dDriverType ,
			NULL ,
			0 ,
			&d3dFeatureLevel_required ,
			numFeatureLevels ,
			D3D11_SDK_VERSION , 
			&dxgiSwapChainDesc ,
			&gpIDXGISwapChain , 
			&gpID3D11Device ,
			&d3dFeatureLevel_acquired ,
			&gpID3D11DeviceContext
		);

		if(SUCCEEDED(hr))
			break;

	}

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "D3D11CreateDeviceAndSwapChain Failed !!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");

		// Print Which Driver We found

		if(d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf(gpFile , "HARDWARE Found !!!");
		}
		else if(d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf(gpFile , "WARP Driver Found !!!");
		}
		else if(d3dDriverType == D3D_DRIVER_TYPE_SOFTWARE)
		{
			fprintf(gpFile , "SOFTWARE Driver Found !!!");
		}
		else if(d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf(gpFile , "REFERENCE Driver Found !!!");
		}
		else if(d3dDriverType == D3D_DRIVER_TYPE_NULL)
		{
			fprintf(gpFile , "NULL Driver Found !!!");
		}
		else if(d3dDriverType == D3D_DRIVER_TYPE_UNKNOWN)
		{
			fprintf(gpFile , "UNKNOWn Driver Found !!!");
		}
		else
		{
			fprintf(gpFile , "UNDEFINED Driver Found !!!");

		}

		// Which Feature Level We Found

		if(d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf(gpFile , "11.0 Feature Level Found !!!");
		}
		else if(d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf(gpFile , "10.1 Feature Level Found !!!");
		}
		else if(d3dFeatureLevel_acquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf(gpFile , "10.0 Feature Level Found !!!");
		}
		else
		{
			fprintf(gpFile , "UNKONOWN Feature Level Found !!!");
		}

		fclose(gpFile);
	}

	// Vertex Shader
	const char* vertexShaderSourceCode =
		"cbuffer ConstantBuffer"\
		"{"\
		"float4x4 WorldMatrix;"\
		"float4x4 ViewMatrix;"\
		"float4x4 ProjectionMatrix;"\
		"float4 lightAmbieant[2];"\
		"float4 lightDiffuse[2];"\
		"float4 lightSpecular[2];"\
		"float4 lightPosition[2];"\
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
		"float3 lightDirection[2];"\
		"float3 reflectionVector[2];"\
		"float3 ambientLight[2];" \
		"float3 diffuseLight[2];" \
		"float3 specularLight[2];" \
		"output.PhongADSLight = float3(0.0f,0.0f,0.0f);"\
		"for(int i = 0;i < 2;i++)"\
		"{"\
		"lightDirection[i] = normalize((float3)(lightPosition[i] - eyeCoordinates));"\
		"reflectionVector[i] = reflect(-lightDirection[i], transformedMatrix);"\
		"float3 viewerVector = normalize(-eyeCoordinates.xyz);"\
		"ambientLight[i] = lightAmbieant[i] * materialAmbieant;"\
		"diffuseLight[i] = lightDiffuse[i] * materialDiffuse * max(dot(lightDirection[i],transformedMatrix), 0.0);"\
		"specularLight[i] = lightSpecular[i] * materialSpecular * pow(max(dot(reflectionVector[i],viewerVector), 0.0),materialShininess);"\
		"output.PhongADSLight = output.PhongADSLight + ambientLight[i] + diffuseLight[i] + specularLight[i];"\
		"}"\
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
		vertexShaderSourceCode,
		lstrlenA(vertexShaderSourceCode) + 1,
		"VS",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"vs_5_0",
		0,
		0,
		&pID3DBlob_VertexShaderSourceCode,
		&pID3DBlob_Error
	);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "vertex Shader compilation error %s\n...", (char*)pID3DBlob_Error->GetBufferPointer());
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "Vertex shader succeded...\n");
		fclose(gpFile);
	}

	// Create the vertex shader from above code
	hr = gpID3D11Device->CreateVertexShader(
		pID3DBlob_VertexShaderSourceCode->GetBufferPointer(),
		pID3DBlob_VertexShaderSourceCode->GetBufferSize(),
		NULL,
		&gpID3D11VertexShader);

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
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);


	// Pixel Shader
	const char* pixelShaderSourceCode =
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
		pixelShaderSourceCode,
		lstrlenA(pixelShaderSourceCode) + 1,
		"ps",
		NULL,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		"main",
		"ps_5_0",
		0,
		0,
		&pID3DBlob_PixelShaderSourceCode,
		&pID3DBlob_Error
	);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pixel Shader compilation error %s\n...", (char*)pID3DBlob_Error->GetBufferPointer());
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "pixel shader succeded...\n");
		fclose(gpFile);
	}

	// Create the vertex shader from above code
	hr = gpID3D11Device->CreatePixelShader(
		pID3DBlob_PixelShaderSourceCode->GetBufferPointer(),
		pID3DBlob_PixelShaderSourceCode->GetBufferSize(),
		NULL,
		&gpID3D11PixelShader);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreatePixelShader function failed...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreatePixelShader function succeded...\n");
		fclose(gpFile);

	}
	// set vertex shader into pipeline
	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);

	// release error blob
	if (pID3DBlob_Error)
	{
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
	}
	// release pixel shader blob
	if (pID3DBlob_PixelShaderSourceCode)
	{
		pID3DBlob_PixelShaderSourceCode->Release();
		pID3DBlob_PixelShaderSourceCode = NULL;
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
		pID3DBlob_VertexShaderSourceCode->GetBufferPointer(),
		pID3DBlob_VertexShaderSourceCode->GetBufferSize(),
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
	if (pID3DBlob_VertexShaderSourceCode)
	{
		pID3DBlob_VertexShaderSourceCode->Release();
		pID3DBlob_VertexShaderSourceCode = NULL;
	}

	// declared triangle geometry
	const float triangle_position[] = {
		// front
   +0.0f, +1.0f, +0.0f, // front-top
   +1.0f, -1.0f, -1.0f, // front-right
   -1.0f, -1.0f, -1.0f, // front-left

   // right
   +0.0f, +1.0f, +0.0f, // right-top
   +1.0f, -1.0f, +1.0f, // right-right
   +1.0f, -1.0f, -1.0f, // right-left

   // back
   +0.0f, +1.0f, +0.0f, // back-top
   -1.0f, -1.0f, +1.0f, // back-right
   +1.0f, -1.0f, +1.0f, // back-left

   // left
   +0.0f, +1.0f, +0.0f, // left-top
   -1.0f, -1.0f, -1.0f, // left-right
   -1.0f, -1.0f, +1.0f, // left-left
	};

	const float pyramid_normal[] = {
		// front
	+0.0f, +0.447214f, -0.894427f, // front-top
	+0.0f, +0.447214f, -0.894427f, // front-right
	+0.0f, +0.447214f, -0.894427f, // front-left

	// right			    
	+0.894427f, +0.447214f, +0.0f, // right-top
	+0.894427f, +0.447214f, +0.0f, // right-right
	+0.894427f, +0.447214f, +0.0f, // right-left

	// back
	+0.0f, +0.447214f, +0.894427f, // back-top
	+0.0f, +0.447214f, +0.894427f, // back-right
	+0.0f, +0.447214f, +0.894427f, // back-left

	// left
	-0.894427f, +0.447214f, +0.0f, // left-top
	-0.894427f, +0.447214f, +0.0f, // left-right
	-0.894427f, +0.447214f, +0.0f, // left-left
	};


	// create vertex bufffer for position
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory((void*)&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(triangle_position);
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// create vertex bufffer from above
	hr = gpID3D11Device->CreateBuffer(&d3dBufferDesc, NULL, &gpID3D11Buffer_positionBuffer);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer function failed...");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer function succeded...");
		fclose(gpFile);
	}
	// we will set this buffer in display because its dynamic

	// copy manually data cpu buffer to gpu buffer
	D3D11_MAPPED_SUBRESOURCE d3dMappedSubresource;
	ZeroMemory((void*)&d3dMappedSubresource, sizeof(d3dMappedSubresource));
	// mapped our position buffer with above resource
	hr = gpID3D11DeviceContext->Map(
		gpID3D11Buffer_positionBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&d3dMappedSubresource
	);
	// now copy actual data
	memcpy(d3dMappedSubresource.pData, triangle_position, sizeof(triangle_position));

	// UNMAPPED
	gpID3D11DeviceContext->Unmap(
		gpID3D11Buffer_positionBuffer,
		0
	);


	// ************************************** COLOR ***************************************
	// ************************************************************************************

		// create vertex bufffer for color
	ZeroMemory((void*)&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(pyramid_normal);
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// create vertex bufffer from above
	hr = gpID3D11Device->CreateBuffer(&d3dBufferDesc, NULL, &gpID3D11Buffer_normalBuffer);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer color function failed...");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer color function succeded...");
		fclose(gpFile);
	}
	// we will set this buffer in display because its dynamic

	// copy manually data cpu buffer to gpu buffer
	ZeroMemory((void*)&d3dMappedSubresource, sizeof(d3dMappedSubresource));
	// mapped our color buffer with above resource
	hr = gpID3D11DeviceContext->Map(
		gpID3D11Buffer_normalBuffer,
		0,
		D3D11_MAP_WRITE_DISCARD,
		0,
		&d3dMappedSubresource
	);
	// now copy actual data
	memcpy(d3dMappedSubresource.pData, pyramid_normal, sizeof(pyramid_normal));

	// UNMAPPED
	gpID3D11DeviceContext->Unmap(
		gpID3D11Buffer_normalBuffer,
		0
	);



	// Create constant buffer to send transformation 
	ZeroMemory((void*)&d3dBufferDesc, sizeof(d3dBufferDesc));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CBUFFER);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// create vertex bufffer from above
	hr = gpID3D11Device->CreateBuffer(&d3dBufferDesc, NULL, &gpID3D11Buffer_constantBuffer);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer function failed for gpID3D11Buffer_constantBuffer...");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer function succeded for gpID3D11Buffer_constantBuffer...");
		fclose(gpFile);
	}

	// set this into pipeline
	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer);

	// set rasterizer test to off back face culling
	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory((void*)&d3dRasterizerDesc, sizeof(d3dRasterizerDesc));
	d3dRasterizerDesc.CullMode = D3D11_CULL_NONE;
	d3dRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	d3dRasterizerDesc.MultisampleEnable = FALSE;
	d3dRasterizerDesc.DepthBias = 0;
	d3dRasterizerDesc.DepthBiasClamp = 0.0f;
	d3dRasterizerDesc.SlopeScaledDepthBias = 0;
	d3dRasterizerDesc.DepthClipEnable = TRUE;
	d3dRasterizerDesc.AntialiasedLineEnable = FALSE;
	d3dRasterizerDesc.FrontCounterClockwise = FALSE;
	d3dRasterizerDesc.ScissorEnable = FALSE;

	hr = gpID3D11Device->CreateRasterizerState(&d3dRasterizerDesc, &gpID3D11RasterizerState);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateRasterizerState function failed...");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateRasterizerState function succeded...");
		fclose(gpFile);

	}
	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

	// Define clear color
	ClearColor[0] = 0.0f;
	ClearColor[1] = 0.0f;
	ClearColor[2] = 0.0f;
	ClearColor[3] = 1.0f;

	perspectiveProjectionMatrix = XMMatrixIdentity();

	// warm up resize
	hr = resize(WIN_WIDTH, WIN_HEIGHT);

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

	
	return hr;

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
	ID3D11Texture2D* pID3D11Texture2D = NULL;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pID3D11Texture2D);

	// b
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D, NULL, &gpID3D11RenderTargetView);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateRenderTargetView function failed...");
		fclose(gpFile);
		pID3D11Texture2D->Release();
		pID3D11Texture2D = NULL;
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

	// Create an empty texture accoding to the changed size we will call it as depth buffer
	D3D11_TEXTURE2D_DESC d3dTexture2DDesc;
	ZeroMemory((void*)&d3dTexture2DDesc, sizeof(d3dTexture2DDesc));
	d3dTexture2DDesc.Width = (UINT)width;
	d3dTexture2DDesc.Height = (UINT)height;
	d3dTexture2DDesc.ArraySize = 1;
	d3dTexture2DDesc.MipLevels = 1;
	d3dTexture2DDesc.SampleDesc.Count = 1;
	d3dTexture2DDesc.SampleDesc.Quality = 0;
	d3dTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dTexture2DDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dTexture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dTexture2DDesc.CPUAccessFlags = 0;
	d3dTexture2DDesc.MiscFlags = 0;

	ID3D11Texture2D* pID3D11Texture2D_depthBuffer = NULL;

	hr = gpID3D11Device->CreateTexture2D(&d3dTexture2DDesc, NULL, &pID3D11Texture2D_depthBuffer);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateTexture2D function failed...");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateTexture2D function succeded...");
		fclose(gpFile);

	}

	// create dsv(deapth stencil view)
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory((void*)&d3dDepthStencilViewDesc, sizeof(d3dDepthStencilViewDesc));
	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS; //2D Multi sampling

	hr = gpID3D11Device->CreateDepthStencilView(pID3D11Texture2D_depthBuffer, &d3dDepthStencilViewDesc, &gpID3D10DepthStencilView);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateDepthStencilView function failed...\n");
		fclose(gpFile);
		pID3D11Texture2D_depthBuffer->Release();
		pID3D11Texture2D_depthBuffer = NULL;
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateDepthStencilView function succeded...\n");
		fclose(gpFile);
	}
	pID3D11Texture2D_depthBuffer->Release();
	pID3D11Texture2D_depthBuffer = NULL;

	// c
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D10DepthStencilView);

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
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D10DepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	// Transformation
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();
	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	
	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	rotationMatrix = XMMatrixRotationY(XMConvertToRadians(-angle));

	worldMatrix = rotationMatrix * translationMatrix;
	//struct CBUFFER cBuffer = gpID3D11Buffer_constantBuffer;
	struct CBUFFER cBuffer ;
	ZeroMemory(&cBuffer, sizeof(CBUFFER));
	cBuffer.WorldMatrix = worldMatrix;
	cBuffer.ViewMatrix = viewMatrix;
	cBuffer.ProjectionMatrix = perspectiveProjectionMatrix;

	if (bLightingEnable == TRUE)
	{
		cBuffer.KeyPress = 1;

		cBuffer.lightAmbientUniform[0] = XMVectorSet(lightAmbient[0][0], lightAmbient[0][1], lightAmbient[0][2], 0.0);
		cBuffer.lightDiffuseUniform[0] = XMVectorSet(lightDiffuse[0][0], lightDiffuse[0][1], lightDiffuse[0][2], 0.0);
		cBuffer.lightSpecularUniform[0] = XMVectorSet(lightSpecular[0][0], lightSpecular[0][1], lightSpecular[0][2], 0.0);
		cBuffer.lightPossitionUniform[0] = XMVectorSet(lightPossition[0][0], lightPossition[0][1], lightPossition[0][2], lightPossition[0][3]);
		
		cBuffer.lightAmbientUniform[1] = XMVectorSet(lightAmbient[1][0], lightAmbient[1][1], lightAmbient[1][2], 0.0);
		cBuffer.lightDiffuseUniform[1] = XMVectorSet(lightDiffuse[1][0], lightDiffuse[1][1], lightDiffuse[1][2], 0.0);
		cBuffer.lightSpecularUniform[1] = XMVectorSet(lightSpecular[1][0], lightSpecular[1][1], lightSpecular[1][2], 0.0);
		cBuffer.lightPossitionUniform[1] = XMVectorSet(lightPossition[1][0], lightPossition[1][1], lightPossition[1][2], lightPossition[1][3]);

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
		gpID3D11Buffer_constantBuffer,
		0,
		NULL,
		&cBuffer,
		0,
		0
	);

	// SET POSITION buffer here
	UINT Strid = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(
		0,
		1,
		&gpID3D11Buffer_positionBuffer,
		&Strid,
		&offset);

	// SET COLOR buffer here
	Strid = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(
		1,
		1,
		&gpID3D11Buffer_normalBuffer,
		&Strid,
		&offset);

	// Set primitive geometry
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw the Geometry
	gpID3D11DeviceContext->Draw(12, 0);

	gpIDXGISwapChain->Present(0, 0);

}
 
void update(void)
{
	// Code
	angle = angle + 0.1f;
	if (angle > 360.0f)
	{
		angle = angle - 360.0f;
	}

}

void uninitialized(void)
{
	// Function declaration
	void ToggleFullscreen(void);

	// Code
	if (gpID3D10DepthStencilView)
	{
		gpID3D10DepthStencilView->Release();
		gpID3D10DepthStencilView = NULL;
	}
	if (gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}
	
	if (gpID3D11Buffer_constantBuffer)
	{
		gpID3D11Buffer_constantBuffer->Release();
		gpID3D11Buffer_constantBuffer = NULL;
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
	if (gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}
	if (gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
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



