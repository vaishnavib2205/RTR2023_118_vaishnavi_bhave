#include<Windows.h> // Windows header files 
#include<stdlib.h> // For exit
#include<stdio.h> // For file IO (input output)

#include "D3D.h"

#include"WICTextureLoader.h"
using namespace DirectX;

// D3D related headerfiles
#include<d3d11.h>
#include<d3dcompiler.h>

#pragma warning(disable:4838)  // supress warning came from inl files of XNAMATH
#include"XNAMath_204/xnamath.h"

#pragma comment(lib , "d3d11.lib")
#pragma comment(lib , "d3dcompiler.lib")
#pragma comment(lib,"DirectXTK.lib")


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
ID3D11Buffer* gpID3D11Buffer_pcntBuffer = NULL;
ID3D11Buffer* gpID3D11Buffer_constantBuffer = NULL;

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

float lightAmbient[] = { 0.2f,0.2f,0.2f,1.0f };
float lightDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
float lightSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
float lightPossition[] = { 10.0f,10.0f,-10.0f,1.0f };

float  materialAmbient[] = { 0.2f,0.2f,0.2f,1.0f };
float  materialDiffuse[] = { 1.0f,1.0f,1.0f,1.0f };
float  materialSpecular[] = { 1.0f,1.0f,1.0f,1.0f };
float  materialShininess = 128.0f;

XMMATRIX perspectiveProjectionMatrix;

// Sphere variables
ID3D11RasterizerState* gpID3D11RasterizerState = NULL;
ID3D11DepthStencilView* gpID3D10DepthStencilView = NULL;
ID3D11ShaderResourceView* gpID3D11ShaderResourceView = NULL;
ID3D11SamplerState* gpID3D11SamplerState = NULL;

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
	// Function declarations
	HRESULT resize(int width, int height);
	HRESULT loadD3DTexture(const wchar_t* textureFileName, ID3D11ShaderResourceView * *ppID3D11ShaderResourceView);

	// variable declarations
	HRESULT hr = S_OK;
	ID3DBlob* pID3DBlob_VertexShaderSourceCode = NULL;
	ID3DBlob* pID3DBlob_PixelShaderSourceCode = NULL;
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
	
	// Vertex Shader
	const char* vertexShaderSourceCode =
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
		"float2 texcoord:TEXCOORD;"\
		"float4 color:COLOR;"\
		"float3 transformedNormals:NORMAL0;"\
		"float3 lightDirection:NORMAL1;"\
		"float3 viewerVector:NORMAL2;"\
		"};"\
		"vertex_output main(float4 pos:POSITION, float4 col:COLOR, float4 norm:NORMAL, float2 tex:TEXCOORD)"\
		"{"\
		"vertex_output output;"\
		"if(keyPress == 1)"\
		"{"\
		"float4  eyeCoordinates = mul(ViewMatrix, mul(WorldMatrix, pos));"\
		"output.transformedNormals = mul((float3x3)WorldMatrix, (float3)norm);"\
		"output.lightDirection = (float3)(lightPosition - eyeCoordinates);"\
		"output.viewerVector = -eyeCoordinates.xyz;"\
		"}"\
		"else"\
		"{"\
		"output.transformedNormals = float3(0.0,0.0,0.0);"\
		"output.lightDirection = float3(0.0,0.0,0.0);"\
		"output.viewerVector = float3(0.0,0.0,0.0);"\
		"}"\
		"float4 position = mul(ProjectionMatrix, mul(ViewMatrix, mul(WorldMatrix, pos)));"\
		"output.position = position;"\
		"output.texcoord = tex;"\
		"output.color = col;"\
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
		"float2 texcoord:TEXCOORD;"\
		"float4 color:COLOR;"\
		"float3 transformedNormals:NORMAL0;"\
		"float3 lightDirection:NORMAL1;"\
		"float3 viewerVector:NORMAL2;"\
		"};"\
		"Texture2D myTexture2D;"\
		"SamplerState mySamplerState;"\
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
		"float3 tex = (float3)myTexture2D.Sample(mySamplerState,input.texcoord);"\
		"float4 color =  input.color * float4(tex * PhongADSLight, 1.0);"\
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
	D3D11_INPUT_ELEMENT_DESC d3dInputElementDesc[4];
	ZeroMemory((void*)&d3dInputElementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC) * _ARRAYSIZE(d3dInputElementDesc));

	// position
	d3dInputElementDesc[0].SemanticName = "POSITION";
	d3dInputElementDesc[0].SemanticIndex = 0;
	d3dInputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	d3dInputElementDesc[0].InputSlot = 0;
	d3dInputElementDesc[0].AlignedByteOffset = 0;
	d3dInputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3dInputElementDesc[0].InstanceDataStepRate = 0;

	// color
	d3dInputElementDesc[1].SemanticName = "COLOR";
	d3dInputElementDesc[1].SemanticIndex = 0;
	d3dInputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	d3dInputElementDesc[1].InputSlot = 1;
	d3dInputElementDesc[1].AlignedByteOffset = 0;
	d3dInputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3dInputElementDesc[1].InstanceDataStepRate = 0;

	// normal
	d3dInputElementDesc[2].SemanticName = "NORMAL";
	d3dInputElementDesc[2].SemanticIndex = 0;
	d3dInputElementDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	d3dInputElementDesc[2].InputSlot = 2;
	d3dInputElementDesc[2].AlignedByteOffset = 0;
	d3dInputElementDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3dInputElementDesc[2].InstanceDataStepRate = 0;

	// texcoord
	d3dInputElementDesc[3].SemanticName = "TEXCOORD";
	d3dInputElementDesc[3].SemanticIndex = 0;
	d3dInputElementDesc[3].Format = DXGI_FORMAT_R32G32_FLOAT;
	d3dInputElementDesc[3].InputSlot = 3;
	d3dInputElementDesc[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	d3dInputElementDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3dInputElementDesc[3].InstanceDataStepRate = 0;

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

	

	const float cube_PCNT[] =
	{
		// position : x,y,z    color : r,g,b          normal : x,y,z         Tex : u,v
		// SIDE 1 ( FRONT )
		-1.0f, +1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   +0.0f, +0.0f, -1.0f,   0.0f, 1.0f, // top-left of front
		+1.0f, +1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   +0.0f, +0.0f, -1.0f,   1.0f, 1.0f, // top-right of front
		-1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   +0.0f, +0.0f, -1.0f,   0.0f, 0.0f, // bottom-left of front

		-1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   +0.0f, +0.0f, -1.0f,   0.0f, 0.0f, // bottom-left of front
		+1.0f, +1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   +0.0f, +0.0f, -1.0f,   1.0f, 1.0f, // top-right of front
		+1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 0.0f,   +0.0f, +0.0f, -1.0f,   1.0f, 0.0f, // bottom-right of front

		// SIDE 2 ( RIGHT )
		+1.0f, +1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   +1.0f, +0.0f, +0.0f,   0.0f, 1.0f, // top-left of right
		+1.0f, +1.0f, +1.0f,   0.0f, 0.0f, 1.0f,   +1.0f, +0.0f, +0.0f,   1.0f, 1.0f, // top-right of right
		+1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   +1.0f, +0.0f, +0.0f,   0.0f, 0.0f, // bottom-left of right

		+1.0f, -1.0f, -1.0f,   0.0f, 0.0f, 1.0f,   +1.0f, +0.0f, +0.0f,   0.0f, 0.0f, // bottom-left of right
		+1.0f, +1.0f, +1.0f,   0.0f, 0.0f, 1.0f,   +1.0f, +0.0f, +0.0f,   1.0f, 1.0f, // top-right of right
		+1.0f, -1.0f, +1.0f,   0.0f, 0.0f, 1.0f,   +1.0f, +0.0f, +0.0f,   1.0f, 0.0f, // bottom-right of right

		// SIDE 3 ( BACK )
		+1.0f, +1.0f, +1.0f,   1.0f, 1.0f, 0.0f,   +0.0f, +0.0f, +1.0f,   0.0f, 1.0f, // top-left of back
		-1.0f, +1.0f, +1.0f,   1.0f, 1.0f, 0.0f,   +0.0f, +0.0f, +1.0f,   1.0f, 1.0f, // top-right of back
		+1.0f, -1.0f, +1.0f,   1.0f, 1.0f, 0.0f,   +0.0f, +0.0f, +1.0f,   0.0f, 0.0f, // bottom-left of back

		+1.0f, -1.0f, +1.0f,   1.0f, 1.0f, 0.0f,   +0.0f, +0.0f, +1.0f,   0.0f, 0.0f, // bottom-left of back
		-1.0f, +1.0f, +1.0f,   1.0f, 1.0f, 0.0f,   +0.0f, +0.0f, +1.0f,   1.0f, 1.0f, // top-right of back
		-1.0f, -1.0f, +1.0f,   1.0f, 1.0f, 0.0f,   +0.0f, +0.0f, +1.0f,   1.0f, 0.0f, // bottom-right of back

		// SIDE 4 ( LEFT )
		-1.0f, +1.0f, +1.0f,   1.0f, 0.0f, 1.0f,   -1.0f, +0.0f, +0.0f,   0.0f, 1.0f, // top-left of left
		-1.0f, +1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   -1.0f, +0.0f, +0.0f,   1.0f, 1.0f, // top-right of left
		-1.0f, -1.0f, +1.0f,   1.0f, 0.0f, 1.0f,   -1.0f, +0.0f, +0.0f,   0.0f, 0.0f, // bottom-left of left

		-1.0f, -1.0f, +1.0f,   1.0f, 0.0f, 1.0f,   -1.0f, +0.0f, +0.0f,   0.0f, 0.0f, // bottom-left of left
		-1.0f, +1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   -1.0f, +0.0f, +0.0f,   1.0f, 1.0f, // top-right of left
		-1.0f, -1.0f, -1.0f,   1.0f, 0.0f, 1.0f,   -1.0f, +0.0f, +0.0f,   1.0f, 0.0f, // bottom-right of left

		// SIDE 5 ( TOP )
		-1.0f, +1.0f, +1.0f,   0.0f, 1.0f, 0.0f,   +0.0f, +1.0f, +0.0f,   0.0f, 1.0f, // top-left of top
		+1.0f, +1.0f, +1.0f,   0.0f, 1.0f, 0.0f,   +0.0f, +1.0f, +0.0f,   1.0f, 1.0f, // top-right of top
		-1.0f, +1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   +0.0f, +1.0f, +0.0f,   0.0f, 0.0f, // bottom-left of top

		-1.0f, +1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   +0.0f, +1.0f, +0.0f,   0.0f, 0.0f, // bottom-left of top
		+1.0f, +1.0f, +1.0f,   0.0f, 1.0f, 0.0f,   +0.0f, +1.0f, +0.0f,   1.0f, 1.0f, // top-right of top
		+1.0f, +1.0f, -1.0f,   0.0f, 1.0f, 0.0f,   +0.0f, +1.0f, +0.0f,   1.0f, 0.0f, // bottom-right of top

		// SIDE 6 ( BOTTOM )
		-1.0f, -1.0f, -1.0f,   1.0f, 0.5f, 0.0f,   +0.0f, -1.0f, +0.0f,   0.0f, 1.0f, // top-left of bottom
		+1.0f, -1.0f, -1.0f,   1.0f, 0.5f, 0.0f,   +0.0f, -1.0f, +0.0f,   1.0f, 1.0f, // top-right of bottom
		-1.0f, -1.0f, +1.0f,   1.0f, 0.5f, 0.0f,   +0.0f, -1.0f, +0.0f,   0.0f, 0.0f, // bottom-left of bottom

		-1.0f, -1.0f, +1.0f,   1.0f, 0.5f, 0.0f,   +0.0f, -1.0f, +0.0f,   0.0f, 0.0f, // bottom-left of bottom
		+1.0f, -1.0f, -1.0f,   1.0f, 0.5f, 0.0f,   +0.0f, -1.0f, +0.0f,   1.0f, 1.0f, // top-right of bottom
		+1.0f, -1.0f, +1.0f,   1.0f, 0.5f, 0.0f,   +0.0f, -1.0f, +0.0f,   1.0f, 0.0f, // bottom-right of bottom
	};

	// create vertex bufffer for position
	// Position VB
	D3D11_BUFFER_DESC d3dBufferDesc;
	//ZeroMemory((void*)&d3dBufferDesc, sizeof(d3dBufferDesc));
	//d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//d3dBufferDesc.ByteWidth = sizeof(cube_PCNT) * sizeof(float);
	//d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//// create vertex bufffer from above
	//hr = gpID3D11Device->CreateBuffer(&d3dBufferDesc, NULL, &gpID3D11Buffer_pcntBuffer);
	//if (FAILED(hr))
	//{
	//	gpFile = fopen(gszLogFileName, "a+");
	//	fprintf(gpFile, "gpID3D11Device->CreateBuffer function failed...");
	//	fclose(gpFile);
	//	return(hr);
	//}
	//else
	//{
	//	gpFile = fopen(gszLogFileName, "a+");
	//	fprintf(gpFile, "gpID3D11Device->CreateBuffer function succeded...");
	//	fclose(gpFile);
	//}
	//// we will set this buffer in display because its dynamic

	//// copy manually data cpu buffer to gpu buffer
	D3D11_MAPPED_SUBRESOURCE d3dMappedSubresource;

	//ZeroMemory((void*)&d3dMappedSubresource, sizeof(d3dMappedSubresource));
	//// mapped our position buffer with above resource
	//hr = gpID3D11DeviceContext->Map(
	//	gpID3D11Buffer_pcntBuffer,
	//	0,
	//	D3D11_MAP_WRITE_DISCARD,
	//	0,
	//	&d3dMappedSubresource
	//);
	//// now copy actual data
	//memcpy(d3dMappedSubresource.pData, cube_PCNT, sizeof(cube_PCNT));

	//// UNMAPPED
	//gpID3D11DeviceContext->Unmap(
	//	gpID3D11Buffer_pcntBuffer,
	//	0
	//);


	// ************************************** COLOR ***************************************
	// ************************************************************************************
	 // Normals VB
	D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
	// Follow Same Above Method For Normals And Textures
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = _ARRAYSIZE(cube_PCNT) * sizeof(float);
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3d11SubresourceData.pSysMem = cube_PCNT; // Same like map and unmap

	hr = gpID3D11Device->CreateBuffer(&d3dBufferDesc, &d3d11SubresourceData, &gpID3D11Buffer_pcntBuffer);
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

	

	//// ************************************** Element ***************************************
	//// ************************************************************************************
	//// create index buffer
	//ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	//bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	//bufferDesc.ByteWidth = gNumElements * sizeof(unsigned short);
	//bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	//d3d11SubresourceData.pSysMem = sphere_elements;
	//hr = gpID3D11Device->CreateBuffer(&bufferDesc, &d3d11SubresourceData, &gpID3D11Buffer_IndexBuffer);
	//if (FAILED(hr))
	//{
	//	gpFile = fopen(gszLogFileName, "a+");
	//	fprintf(gpFile, "gpID3D11Device->CreateBuffer element function failed...\n");
	//	fclose(gpFile);
	//	return(hr);
	//}
	//else
	//{
	//	gpFile = fopen(gszLogFileName, "a+");
	//	fprintf(gpFile, "gpID3D11Device->CreateBuffer element function succeded...\n");
	//	fclose(gpFile);
	//}
	//
	 
	// **********************************************************************************************************8888
	// **********************************************************************************************************8888
	// **********************************************************************************************************8888
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
		fprintf(gpFile, "gpID3D11Device->CreateBuffer function failed for gpID3D11Buffer_constantBuffer...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "gpID3D11Device->CreateBuffer function succeded for gpID3D11Buffer_constantBuffer...\n");
		fclose(gpFile);
	}

	// set this into pipeline
	gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer);
	gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer);

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

	hr = loadD3DTexture(L"marble.bmp", &gpID3D11ShaderResourceView);

	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "loadD3DTexture function failed...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "loadD3DTexture function succeded...\n");
		fclose(gpFile);

	}
	D3D11_SAMPLER_DESC d3dSamplerDesc;
	ZeroMemory((void*)&d3dSamplerDesc, sizeof(D3D11_SAMPLER_DESC));
	d3dSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	d3dSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	d3dSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = gpID3D11Device->CreateSamplerState(&d3dSamplerDesc, &gpID3D11SamplerState);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateSamplerState function failed...\n");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateSamplerState function succeded...\n");
		fclose(gpFile);

	}

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
HRESULT loadD3DTexture(const wchar_t* textureFileName, ID3D11ShaderResourceView** ppID3D11ShaderResourceView)
{
	// Code
	HRESULT hr = S_OK;

	hr = CreateWICTextureFromFile(gpID3D11Device, gpID3D11DeviceContext, textureFileName, NULL, ppID3D11ShaderResourceView);
	if (FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateWICTextureFromFile function failed...");
		fclose(gpFile);
		return(hr);
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateWICTextureFromFile function succeded...");
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
	ZeroMemory((void*)&d3dDepthStencilViewDesc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
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
	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX rotationX = XMMatrixIdentity();
	XMMATRIX rotationY = XMMatrixIdentity();
	XMMATRIX rotationZ = XMMatrixIdentity();
	XMMATRIX rotationMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 6.0f);
	rotationX = XMMatrixRotationX(XMConvertToRadians(-angle));
	rotationY = XMMatrixRotationY(XMConvertToRadians(-angle));
	rotationZ = XMMatrixRotationZ(XMConvertToRadians(-angle));
	rotationMatrix = rotationX * rotationY * rotationZ;

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
		gpID3D11Buffer_constantBuffer,
		0,
		NULL,
		&cBuffer,
		0,
		0
	);

	// SET POSITION buffer here
	UINT Strid = sizeof(float) * 11;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(
		0,
		1,
		&gpID3D11Buffer_pcntBuffer,
		&Strid,
		&offset);

	// SET COLOR buffer here
	Strid = sizeof(float) * 11;
	offset = sizeof(float) * 3;
	gpID3D11DeviceContext->IASetVertexBuffers(
		1,
		1,
		&gpID3D11Buffer_pcntBuffer,
		&Strid,
		&offset);

	// SET Normal buffer here
	Strid = sizeof(float) * 11;
	offset = sizeof(float) * 6;
	gpID3D11DeviceContext->IASetVertexBuffers(
		2,
		1,
		&gpID3D11Buffer_pcntBuffer,
		&Strid,
		&offset);	


	// SET Normal buffer here
	Strid = sizeof(float) * 11;
	offset = sizeof(float) * 9;
	gpID3D11DeviceContext->IASetVertexBuffers(
		3,
		1,
		&gpID3D11Buffer_pcntBuffer,
		&Strid,
		&offset);
	
	 //set the texture shader view in pixel shader 
	gpID3D11DeviceContext->PSSetShaderResources(
		0,
		1,
		&gpID3D11ShaderResourceView
	);
	
	// Set texture sampler state in pixel shader
	gpID3D11DeviceContext->PSSetSamplers(
		0,
		1,
		&gpID3D11SamplerState
	);
	// Set primitive geometry
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Draw the Geometry
	//gpID3D11DeviceContext->Draw(36, 0);
	gpID3D11DeviceContext->Draw(6, 0);
	gpID3D11DeviceContext->Draw(6, 6);
	gpID3D11DeviceContext->Draw(6, 12);
	gpID3D11DeviceContext->Draw(6, 18);
	gpID3D11DeviceContext->Draw(6, 24);
	gpID3D11DeviceContext->Draw(6, 30);

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
	
	if (gpID3D11Buffer_constantBuffer)
	{
		gpID3D11Buffer_constantBuffer->Release();
		gpID3D11Buffer_constantBuffer = NULL;
	}

	if (gpID3D11Buffer_pcntBuffer)
	{
		gpID3D11Buffer_pcntBuffer->Release();
		gpID3D11Buffer_pcntBuffer = NULL;
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


