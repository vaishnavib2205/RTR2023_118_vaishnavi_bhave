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
FILE* gpFILE = NULL;

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
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;
float ClearColor[4];

ID3D11VertexShader* gpID3D11VertexShader_pv = NULL;
ID3D11PixelShader* gpID3D11PixelShader_pv = NULL;
ID3D11VertexShader* gpID3D11VertexShader_pf = NULL;
ID3D11PixelShader* gpID3D11PixelShader_pf = NULL;
ID3D11InputLayout* gpID3D11InputLayout = NULL;
ID3D11Buffer* gpID3D11Buffer_positionBuffer = NULL;
ID3D11Buffer* gpID3D11Buffer_normalBuffer = NULL;
ID3D11Buffer* gpID3D11Buffer_colorBuffer = NULL;
ID3D11Buffer* gpID3D11Buffer_constantBuffer_pv = NULL;
ID3D11Buffer* gpID3D11Buffer_constantBuffer_pf = NULL;

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;

struct CBUFFER 
{
	XMMATRIX WorldMatrix;
	XMMATRIX ViewMatrix;
	XMMATRIX ProjectionMatrix;

	XMVECTOR LightAmbient[3];
	XMVECTOR LightDiffuse[3];
	XMVECTOR LightSpecular[3];
	XMVECTOR LightPosition[3];

	XMVECTOR MaterialAmbient;
	XMVECTOR MaterialDiffuse;
	XMVECTOR MaterialSpecular;

	float MaterialShineness;

	int KeyPress;
};

BOOL bLightingEnable = FALSE;

// light animation global variable
float lightAngleZero = 0.0f;
float lightAngleOne = 0.0f;
float lightAngleTwo = 0.0f;

// for shader selection
char  choosenShader;

// light 0 properties global variable(per vertex)
float lightAmbient_0[] = { 0.0f, 0.0f, 0.0, 1.0f };
float lightDiffuse_0[] = { 1.0f, 0.0f, 0.0f, 1.0f }; 
float lightSpecular_0[] = { 1.0f, 0.0f, 0.0f, 1.0f };
float lightPosition_0[4];

// light 1 properties global variable(per vertex)
float lightAmbient_1[] = { 0.0f, 0.0f, 0.0, 1.0f };
float lightDiffuse_1[] = { 0.0f, 1.0f, 0.0f, 1.0f }; 
float lightSpecular_1[] = { 0.0f, 1.0f, 0.0f, 1.0f };
float lightPosition_1[4];

// light 2 properties global variable(per vertex)
float lightAmbient_2[] = { 0.0f, 0.0f, 0.0, 1.0f };
float lightDiffuse_2[] = { 0.0f, 0.0f, 1.0f, 1.0f }; 
float lightSpecular_2[] = { 0.0f, 0.0f, 1.0f, 1.0f };
float lightPosition_2[4];

// Material properties global variable
float materialAmbient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
float materialDiffuse[] = { 0.5f, 0.2f, 0.7f, 1.0f }; 
float materialSpecular[] = { 0.7f, 0.7f, 0.7f, 1.0f }; 
float materialShineness = 128.0f;

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

	if (fopen_s(&gpFILE , gszLogFileName, "w") != 0)
	{
		MessageBox(NULL, TEXT("Log file cannot be open!!!"), TEXT("File create error"), MB_OK | MB_ICONERROR);
		exit(0);
	}
	else
	{
		fprintf(gpFILE, "Program started successfully \n");
		fclose(gpFILE);
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
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE , "Initialize failed!!!");
		fclose(gpFILE);
		DestroyWindow(hwnd);
	}
	else
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE , "Initialize is Success !!!");
		fclose(gpFILE);
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
				gpFILE = fopen(gszLogFileName, "a+");
				fprintf(gpFILE , "Resize Failed !!!");
				fclose(gpFILE);

				return hr;
			}
			else
			{
				gpFILE = fopen(gszLogFileName, "a+");
				fprintf(gpFILE , "Resize is Success !!!");
				fclose(gpFILE);
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
					{
						bLightingEnable = TRUE;
						//bPV_Enable = TRUE;
					}
					else
					{
						bLightingEnable = FALSE;
					}
				break;

				// Per vertex Light ON
				case 'V':
				case 'v':
					choosenShader = 'V';					
				break;

				// Per fragment Light ON
				case 'G':
				case 'g':
					choosenShader = 'F';					
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
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "D3D11CreateDeviceAndSwapChain function failed...\n");
		fclose(gpFILE);
		return(hr);
	}
	else 
	{
		gpFILE = fopen(gszLogFileName, "a+");
		if (d3dDriverType == D3D_DRIVER_TYPE_HARDWARE)
		{
			fprintf(gpFILE, "Hardware driver found...\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_WARP)
		{
			fprintf(gpFILE, "WARP driver found...\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_SOFTWARE)
		{
			fprintf(gpFILE, "Hardware driver found...\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_REFERENCE)
		{
			fprintf(gpFILE, "Refference driver found...\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_NULL)
		{
			fprintf(gpFILE, "NULL driver found...\n");
		}
		else if (d3dDriverType == D3D_DRIVER_TYPE_UNKNOWN)
		{
			fprintf(gpFILE, "Unknown driver found...\n");
		}
		else {
			fprintf(gpFILE, "UNDEFINED Driver Found \n");
		}

		// Which Feature Level We Found
		if (d3dFeatureLevel_accquired == D3D_FEATURE_LEVEL_11_0)
		{
			fprintf(gpFILE, "11.0 feature level found\n");
		}
		else if (d3dFeatureLevel_accquired == D3D_FEATURE_LEVEL_10_1)
		{
			fprintf(gpFILE, "10.1 feature level found\n");
		}
		else if (d3dFeatureLevel_accquired == D3D_FEATURE_LEVEL_10_0)
		{
			fprintf(gpFILE, "10.0 feature level found\n");
		}
		else
		{
			fprintf(gpFILE, "Unknow fetuare level found\n");
			fclose(gpFILE);
		}
	}

	/*****************************************Per vertex */
	/********************************************************************************* */
	
		// Vertex Shader per vertex
		const char* vertexShaderSourceCode_pv =
		"cbuffer ConstantBuffer" \
		"{" \
			"float4x4 worldMatrix;" \
			"float4x4 viewMatrix;" \
			"float4x4 projectionMatrix;" \
			"float3 lightAmbient[3];" \
			"float3 lightDiffuse[3];" \
			"float3 lightSpecular[3];" \
			"float4 lightPosition[3];" \
			"float4 materialAmbient;" \
			"float4 materialDiffuse;" \
			"float4 materialSpecular;" \
			"float materialShineness;" \
			"int keyPress;" \
		"}" \
		"struct Vertex_Output" \
		"{" \
			"float4 position:SV_POSITION;" \
			"float3 phong_ADS_Light:COLOR;" \
		"};" \
		"Vertex_Output main(float4 pos:POSITION, float3 norm:NORMAL)" \
		"{" \
			"Vertex_Output output;" \
			"if(keyPress == 1)" \
			"{" \
				"float4 eyeCoordinates = mul(viewMatrix, mul(worldMatrix, pos));" \
				"float3 tranformedNormals = normalize(mul((float3x3)worldMatrix, norm));" \
				"float3 lightDirection[3];" \
				"float3 reflectionVector[3];" \
				"float3 ambientLight[3];" \
				"float3 diffuseLight[3];" \
				"float3 specularLight[3];" \
				"output.phong_ADS_Light = float3(0.0f, 0.0f, 0.0f);" \
				"for (int i = 0; i < 3; i++)" \
				"{" \
					"lightDirection[i] = normalize((float4)(lightPosition[i] - eyeCoordinates));"\
					"reflectionVector[i] = reflect(-lightDirection[i], tranformedNormals);" \
					"float3 viewerVector = normalize(-eyeCoordinates.xyz);" \
					"ambientLight[i] = lightAmbient[i] * materialAmbient;" \
					"diffuseLight[i] = lightDiffuse[i] * materialDiffuse * max(dot(lightDirection[i], tranformedNormals), 0.0);" \
					"specularLight[i] = lightSpecular[i] * materialSpecular * pow(max(dot(reflectionVector[i], viewerVector), 0.0), materialShineness);" \
					"output.phong_ADS_Light = output.phong_ADS_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];" \
				"}" \
			"}" \
			"else" \
			"{" \
				"output.phong_ADS_Light = float3(1.0f, 1.0f, 1.0f);" \
			"}" \
			"float4 position =  mul(projectionMatrix , mul(viewMatrix, mul(worldMatrix, pos)));" \
			"output.position = position;" \
			"return(output);" \
		"}";

		ID3DBlob* pID3DBlob_vertexShaderSourceCode_pv = NULL;
		ID3DBlob* pID3DBlob_Error = NULL;

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
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "vertex Shader per vertex compilation error %s\n...", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFILE);
			return(hr);
		}
		else
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "Vertex shader per vertex succeded...\n");
			fclose(gpFILE);
		}

		// Create the vertex shader from above code
		hr = gpID3D11Device->CreateVertexShader(
			pID3DBlob_vertexShaderSourceCode_pv->GetBufferPointer(),
			pID3DBlob_vertexShaderSourceCode_pv->GetBufferSize(),
			NULL,
			&gpID3D11VertexShader_pv
		);

		if (FAILED(hr))
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "CreateVertexShader function failed...\n");
			fclose(gpFILE);
			return(hr);
		}
		else
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "CreateVertexShader function succeded...\n");
			fclose(gpFILE);
		}
		// set vertex shader into pipeline
		//gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);

		/******************************************************************************************* */
		// Pixel Shader per Vertex
		const char* pixelShaderSourceCode_pv =
		"cbuffer ConstantBuffer" \
		"{" \
			"float4x4 worldMatrix;" \
			"float4x4 viewMatrix;" \
			"float4x4 projectionMatrix;" \
			"float3 lightAmbient[3];" \
			"float3 lightDiffuse[3];" \
			"float3 lightSpecular[3];" \
			"float4 lightPosition[3];" \
			"float4 materialAmbient;" \
			"float4 materialDiffuse;" \
			"float4 materialSpecular;" \
			"float materialShineness;" \
			"int keyPress;" \
		"}" \
		"struct Vertex_Output" \
		"{" \
			"float4 position:SV_POSITION;" \
			"float3 phong_ADS_Light:COLOR;" \
		"};" \
		"float4 main(Vertex_Output input):SV_TARGET" \
		"{" \
			"float4 color;" \
			"if (keyPress == 1)" \
			"{" \
				"color = float4(input.phong_ADS_Light, 1.0);" \
				"return(color);" \
			"}" \
			"else" \
			"{" \
				"color = float4(1.0, 1.0, 1.0, 1.0);" \
				"return(color);" \
			"}" \
		"}";

		ID3DBlob* pID3DBlob_pixelShaderSourceCode_pv = NULL;
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
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "pixel Shader per vertex compilation error %s\n...", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFILE);
			return(hr);
		}
		else
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "pixel shader per vertex succeded...\n");
			fclose(gpFILE);
		}

		// Create the vertex shader from above code
		hr = gpID3D11Device->CreatePixelShader(
			pID3DBlob_pixelShaderSourceCode_pv->GetBufferPointer(),
			pID3DBlob_pixelShaderSourceCode_pv->GetBufferSize(),
			NULL,
			&gpID3D11PixelShader_pv
		);

		if (FAILED(hr))
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "CreatePixelShader per vertex function failed...\n");
			fclose(gpFILE);
			return(hr);
		}
		else
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "CreatePixelShader per vertex function succeded...\n");
			fclose(gpFILE);
		}

		// set vertex shader into pipeline
		//gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);

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

	/************************************************************************************ */
		
	/************************************************************************************* */
	/**********************************Per Fragment*************************************************** */

	/********************************************************************************* */
	// Vertex Shader per fragment
		
		const char* vertexShaderSourceCode_pf =
		"cbuffer ConstantBuffer" \
		"{"\
			"float4x4 worldMatrix;" \
			"float4x4 viewMatrix;" \
			"float4x4 projectionMatrix;" \
			"float4 lightAmbient[3];" \
			"float4 lightDiffuse[3];" \
			"float4 lightSpecular[3];" \
			"float4 lightPosition[3];" \
			"float4 materialAmbient;" \
			"float4 materialDiffuse;" \
			"float4 materialSpecular;" \
			"float materialShineness;" \
			"int keyPress;" \
		"}" \
		"struct Vertex_Output" \
		"{" \
			"float4 position:SV_POSITION;" \
			"float3 tranformedNormals:NORMAL0;" \
			"float3 lightDirection:NORMAL1;" \
			"float3 lightDirection2:NORMAL2;" \
			"float3 lightDirection3:NORMAL3;" \
			"float3 viewerVector:NORMAL4;" \
		"};" \
		"Vertex_Output main(float4 pos:POSITION, float3 norm:NORMAL)" \
		"{" \
			"Vertex_Output output;" \
			"if (keyPress == 1)" \
			"{" \
				"float4 eyeCoordinates = mul(viewMatrix, mul(worldMatrix, pos));" \
				"output.tranformedNormals = mul((float3x3)worldMatrix, norm);" \
				"output.lightDirection = (float3)(lightPosition[0] - (float3)eyeCoordinates);" \
				"output.lightDirection2 = (float3)(lightPosition[1] - (float3)eyeCoordinates);" \
				"output.lightDirection3 = (float3)(lightPosition[2] - (float3)eyeCoordinates);" \
				"output.viewerVector = -eyeCoordinates.xyz;" \
			"}" \
			"else" \
			"{" \
				"output.tranformedNormals = float3(0.0f, 0.0f, 0.0f);" \
				"output.lightDirection[0] = float3(0.0f, 0.0f, 0.0f);" \
				"output.lightDirection[1] = float3(0.0f, 0.0f, 0.0f);" \
				"output.lightDirection[2] = float3(0.0f, 0.0f, 0.0f);" \
				"output.viewerVector = float3(0.0f, 0.0f, 0.0f);" \
			"}" \
			"float4 position = mul(projectionMatrix , mul(viewMatrix, mul(worldMatrix, pos)));" \
			"output.position = position;" \
			"return(output);" \
		"}";

		ID3DBlob* pID3DBlob_vertexShaderSourceCode_pf = NULL;
		pID3DBlob_Error = NULL;

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
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "vertex Shader per fragment compilation error %s\n...", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFILE);
			return(hr);
		}
		else
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "Vertex shader per fragment succeded...\n");
			fclose(gpFILE);
		}

		// Create the vertex shader from above code
		hr = gpID3D11Device->CreateVertexShader(
			pID3DBlob_vertexShaderSourceCode_pf->GetBufferPointer(),
			pID3DBlob_vertexShaderSourceCode_pf->GetBufferSize(),
			NULL,
			&gpID3D11VertexShader_pf
		);

		if (FAILED(hr))
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "CreateVertexShader per fragment function failed...\n");
			fclose(gpFILE);
			return(hr);
		}
		else
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "CreateVertexShader per fragment function succeded...\n");
			fclose(gpFILE);
		}
		// set vertex shader into pipeline
		//gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader, NULL, 0);

		/******************************************************************************************* */
		// Pixel Shader per pixel
		const char* pixelShaderSourceCode_pf =
		"cbuffer ConstantBuffer" \
		"{" \
			"float4x4 worldMatrix;" \
			"float4x4 viewMatrix;" \
			"float4x4 projectionMatrix;" \
			"float4 lightAmbient[3];" \
			"float4 lightDiffuse[3];" \
			"float4 lightSpecular[3];" \
			"float4 lightPosition[3];" \
			"float4 materialAmbient;" \
			"float4 materialDiffuse;" \
			"float4 materialSpecular;" \
			"float materialShineness;" \
			"int keyPress;" \
		"}" \
		"struct Vertex_Output" \
		"{" \
			"float4 position:SV_POSITION;" \
			"float3 tranformedNormals:NORMAL0;" \
			"float3 lightDirection:NORMAL1;" \
			"float3 lightDirection2:NORMAL2;" \
			"float3 lightDirection3:NORMAL3;" \
			"float3 viewerVector:NORMAL4;" \
		"};" \
		"float4 main(Vertex_Output input):SV_TARGET" \
		"{" \
			"float3 normalizedLightDirection[3];"\
			"float3 reflectionVector[3];"\
			"float3 ambientLight[3];" \
			"float3 diffuseLight[3];" \
			"float3 specularLight[3];" \
			"float3 phong_ADS_Light = float3(0.0f,0.0f,0.0f);" \
			"if(keyPress == 1)" \
			"{" \
				"float3 normalizedTransformedNormals = normalize(input.tranformedNormals);" \
				"float3 normalizedViewerVector = normalize(input.viewerVector);" \
				"normalizedLightDirection[0] = normalize(input.lightDirection);" \
				"normalizedLightDirection[1] = normalize(input.lightDirection2);" \
				"normalizedLightDirection[2] = normalize(input.lightDirection3);" \
				"for (int i = 0; i < 3; i++)" \
				"{" \
					"reflectionVector[i] = reflect(-normalizedLightDirection[i], normalizedTransformedNormals);" \
					"ambientLight[i] = lightAmbient[i] * materialAmbient;" \
					"diffuseLight[i] = lightDiffuse[i] * materialDiffuse * max(dot(normalizedLightDirection[i], normalizedTransformedNormals), 0.0f);" \
					"specularLight[i] = lightSpecular[i] * materialSpecular * pow(max(dot(reflectionVector[i], normalizedViewerVector), 0.0f), materialShineness);" \
					"phong_ADS_Light = phong_ADS_Light + ambientLight[i] + diffuseLight[i] + specularLight[i];" \
				"}" \
			"}" \
			"else" \
			"{" \
				"phong_ADS_Light = float3(1.0f, 1.0f, 1.0f);" \
			"}" \
			"float4 color = float4(phong_ADS_Light, 1.0f);" \
			"return(color);" \
		"}";

		ID3DBlob* pID3DBlob_PixelShaderSourceCode_pf = NULL;
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
			&pID3DBlob_PixelShaderSourceCode_pf,
			&pID3DBlob_Error
		);

		if (FAILED(hr))
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "pixel Shader per fragment compilation error %s\n...", (char*)pID3DBlob_Error->GetBufferPointer());
			fclose(gpFILE);
			return(hr);
		}
		else
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "pixel shader per fragment succeded...\n");
			fclose(gpFILE);
		}

		// Create the vertex shader from above code
		hr = gpID3D11Device->CreatePixelShader(
			pID3DBlob_PixelShaderSourceCode_pf->GetBufferPointer(),
			pID3DBlob_PixelShaderSourceCode_pf->GetBufferSize(),
			NULL,
			&gpID3D11PixelShader_pf
		);

		if (FAILED(hr))
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "CreatePixelShader function per fragment failed...\n");
			fclose(gpFILE);
			return(hr);
		}
		else
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "CreatePixelShader function per fragment succeded...\n");
			fclose(gpFILE);
		}

		// set vertex shader into pipeline
		//gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader, NULL, 0);

		// release error blob
		if (pID3DBlob_Error)
		{
			pID3DBlob_Error->Release();
			pID3DBlob_Error = NULL;
		}

		// release pixel shader blob
		if (pID3DBlob_PixelShaderSourceCode_pf)
		{
			pID3DBlob_PixelShaderSourceCode_pf->Release();
			pID3DBlob_PixelShaderSourceCode_pf = NULL;
		}

		/************************************************************************************ */
		
	
	/************************************************************************************ */
	// initialize input element structure
		D3D11_INPUT_ELEMENT_DESC d3dInputElementDesc[2];
		ZeroMemory((void*)&d3dInputElementDesc, sizeof(D3D11_INPUT_ELEMENT_DESC) * _ARRAYSIZE(d3dInputElementDesc));
		
		// position
		d3dInputElementDesc[0].SemanticName = "POSITION";
		d3dInputElementDesc[0].SemanticIndex = 0;
		d3dInputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		d3dInputElementDesc[0].InputSlot = 0;
		d3dInputElementDesc[0].AlignedByteOffset = 0;
		d3dInputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		d3dInputElementDesc[0].InstanceDataStepRate = 0;

		// Normal
		d3dInputElementDesc[1].SemanticName = "NORMAL";
		d3dInputElementDesc[1].SemanticIndex = 0;
		d3dInputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		d3dInputElementDesc[1].InputSlot = 1;
		d3dInputElementDesc[1].AlignedByteOffset = 0;
		d3dInputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		d3dInputElementDesc[1].InstanceDataStepRate = 0;

		// create input layout using pv above
		hr = gpID3D11Device->CreateInputLayout(
			d3dInputElementDesc,
			_ARRAYSIZE(d3dInputElementDesc),
			pID3DBlob_vertexShaderSourceCode_pv->GetBufferPointer(),
			pID3DBlob_vertexShaderSourceCode_pv->GetBufferSize(),
			&gpID3D11InputLayout
		);

		// create input layout using pf above
		hr = gpID3D11Device->CreateInputLayout(
			d3dInputElementDesc,
			_ARRAYSIZE(d3dInputElementDesc),
			pID3DBlob_vertexShaderSourceCode_pf->GetBufferPointer(),
			pID3DBlob_vertexShaderSourceCode_pf->GetBufferSize(),
			&gpID3D11InputLayout
		);

		if (FAILED(hr))
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "CreateInputLayout function failed...\n");
			fclose(gpFILE);
			return(hr);
		}
		else
		{
			gpFILE = fopen(gszLogFileName, "a+");
			fprintf(gpFILE, "CreateInputLayout function succeded...\n");
			fclose(gpFILE);
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

	///////////Sphere
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
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "gpID3D11Device->CreateBuffer position function failed...\n");
		fclose(gpFILE);
		return(hr);
	}
	else
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "gpID3D11Device->CreateBuffer position function succeded...\n");
		fclose(gpFILE);
	}	

	// ************************************** Normal ***************************************
	// ************************************************************************************
	 // Normals VB
	// Follow Same Above Method For Normals And Textures
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = gNumVertices * 3 * sizeof(float);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3d11SubresourceData.pSysMem = sphere_normals;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, &d3d11SubresourceData, &gpID3D11Buffer_normalBuffer);
	if (FAILED(hr))
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "gpID3D11Device->CreateBuffer normal function failed...\n");
		fclose(gpFILE);
		return(hr);
	}
	else
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "gpID3D11Device->CreateBuffer normal function succeded...\n");
		fclose(gpFILE);
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
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "gpID3D11Device->CreateBuffer element function failed...\n");
		fclose(gpFILE);
		return(hr);
	}
	else
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "gpID3D11Device->CreateBuffer element function succeded...\n");
		fclose(gpFILE);
	}	

	// Create constant buffer to send transformation 
	ZeroMemory((void*)&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// create vertex bufffer from above
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_constantBuffer_pv);
	if (FAILED(hr))
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "gpID3D11Device->CreateBuffer function failed for gpID3D11Buffer_constantBuffer_pv...\n");
		fclose(gpFILE);
		return(hr);
	}
	else
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "gpID3D11Device->CreateBuffer function succeded for gpID3D11Buffer_constantBuffer_pv...\n");
		fclose(gpFILE);
	}

	// Create constant buffer to send transformation 
	ZeroMemory((void*)&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// create vertex bufffer from above
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, NULL, &gpID3D11Buffer_constantBuffer_pf);
	if (FAILED(hr))
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "gpID3D11Device->CreateBuffer function failed for gpID3D11Buffer_constantBuffer_pf...\n");
		fclose(gpFILE);
		return(hr);
	}
	else
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "gpID3D11Device->CreateBuffer function succeded for gpID3D11Buffer_constantBuffer_pf...\n");
		fclose(gpFILE);
	}

	// set this into pipeline
	//gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer);
	//gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer);

	D3D11_RASTERIZER_DESC d3dRasterizerDesc;
	ZeroMemory((void *)&d3dRasterizerDesc , sizeof(D3D11_RASTERIZER_DESC));

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

	hr = gpID3D11Device->CreateRasterizerState(&d3dRasterizerDesc , &gpID3D11RasterizerState);
	if(FAILED(hr))
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE , "CreateRasterizerState Failed !!! \n");
		fclose(gpFILE);
		return(hr);
	}
	else
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE , "CreateRasterizerState is Success !!! \n");
		fclose(gpFILE);
	}

	// set this state in pipeline
	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

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
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "resize function failed...\n");
		fclose(gpFILE);
		return(hr);
	}
	else
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "resize function succeded...\n");
		fclose(gpFILE);
	}

	return(hr);
}

HRESULT resize(int width, int height)
{
	// variable declarations
	HRESULT hr = S_OK;

	// Code	
	// release DSV
	if(gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

	// Release RTV
	if (gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	// resize the swapchain buffer according to the new size
	gpIDXGISwapChain->ResizeBuffers(1, width, height, DXGI_FORMAT_B8G8R8A8_UNORM, 0);

	
	// a get the buffer from RTV for swapchain in to texture
	ID3D11Texture2D *pID3D11Texture2D = NULL;
	gpIDXGISwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pID3D11Texture2D);

	// b create new RTV from above buffer
	hr = gpID3D11Device->CreateRenderTargetView(pID3D11Texture2D, NULL, &gpID3D11RenderTargetView);

	if (FAILED(hr))
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "CreateRenderTargetView function failed...\n");
		fclose(gpFILE);
		return(hr);
	}
	else
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE, "CreateRenderTargetView function succeded...\n");
		fclose(gpFILE);
	}

	pID3D11Texture2D->Release();
	pID3D11Texture2D = NULL;

	// create an empty texture according to the changed size, we will call it as depth texture
	D3D11_TEXTURE2D_DESC d3dTexture2DDesc;
	ZeroMemory((void *)&d3dTexture2DDesc , sizeof(D3D11_TEXTURE2D_DESC));

	d3dTexture2DDesc.Width = (UINT)width;
	d3dTexture2DDesc.Height = (UINT)height;
	d3dTexture2DDesc.MipLevels = 1;
	d3dTexture2DDesc.ArraySize = 1;
	d3dTexture2DDesc.SampleDesc.Count = 1;  // change this for changing quality
	d3dTexture2DDesc.SampleDesc.Quality = 0;
	d3dTexture2DDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dTexture2DDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dTexture2DDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	d3dTexture2DDesc.CPUAccessFlags = 0;
	d3dTexture2DDesc.MiscFlags = 0;

	ID3D11Texture2D *piD3D11Texture2D_DepthBuffer = NULL;

	hr = gpID3D11Device->CreateTexture2D(&d3dTexture2DDesc, NULL, &piD3D11Texture2D_DepthBuffer);
    if (FAILED(hr))
    {
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE , "CreateTexture2D Failed !!!\n");
		fclose(gpFILE);
		return(hr);
	}

	// create DSV according to above dept buffer texture
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory((void *)&d3dDepthStencilViewDesc , sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;  // multi sampling

	hr = gpID3D11Device->CreateDepthStencilView(piD3D11Texture2D_DepthBuffer , &d3dDepthStencilViewDesc , &gpID3D11DepthStencilView );
	if(FAILED(hr))
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE , "CreateDepthStencilView Failed !!!\n");
		fclose(gpFILE);

		piD3D11Texture2D_DepthBuffer->Release();
		piD3D11Texture2D_DepthBuffer = NULL;
		return hr;
	}
	else
	{
		gpFILE = fopen(gszLogFileName, "a+");
		fprintf(gpFILE , "CreateDepthStencilView is Success !!!\n");
		fclose(gpFILE);
	}

	piD3D11Texture2D_DepthBuffer->Release();
	piD3D11Texture2D_DepthBuffer = NULL;

	// c set new RTV in pipeline
	gpID3D11DeviceContext->OMSetRenderTargets(1, &gpID3D11RenderTargetView, gpID3D11DepthStencilView);

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

	// initialize perspectiveprojection matrix
	perspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
	
	return(hr);
}

void display(void)
{
	// Code
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView, ClearColor);
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView , D3D11_CLEAR_DEPTH , 1.0f , 0);

	if (choosenShader == 'F')
	{		
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_pf, NULL, 0);
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_pf, NULL, 0);

		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pf);
		gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pf);

		// Transformation
		XMMATRIX worldMatrix = XMMatrixIdentity();
		XMMATRIX translationMatrix = XMMatrixIdentity();
		XMMATRIX viewMatrix = XMMatrixIdentity();

		translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);	
		worldMatrix= translationMatrix;
		
		CBUFFER cBuffer ;
		ZeroMemory(&cBuffer, sizeof(CBUFFER));
		cBuffer.WorldMatrix = worldMatrix;
		cBuffer.ViewMatrix = viewMatrix;
		cBuffer.ProjectionMatrix = perspectiveProjectionMatrix;

		if (bLightingEnable == TRUE)
		{
			cBuffer.KeyPress = 1;

			cBuffer.LightAmbient[0] = XMVectorSet(lightAmbient_0[0], lightAmbient_0[1], lightAmbient_0[2], 0.0f);
			cBuffer.LightDiffuse[0] = XMVectorSet(lightDiffuse_0[0], lightDiffuse_0[1], lightDiffuse_0[2], 0.0f);
			cBuffer.LightSpecular[0] = XMVectorSet(lightSpecular_0[0], lightSpecular_0[1], lightSpecular_0[2], 0.0f);
			cBuffer.LightPosition[0] = XMVectorSet(lightPosition_0[0], lightPosition_0[1], lightPosition_0[2], lightPosition_0[3]);

			cBuffer.LightAmbient[1] = XMVectorSet(lightAmbient_1[0], lightAmbient_1[1], lightAmbient_1[2], 0.0f);
			cBuffer.LightDiffuse[1] = XMVectorSet(lightDiffuse_1[0], lightDiffuse_1[1], lightDiffuse_1[2], 0.0f);
			cBuffer.LightSpecular[1] = XMVectorSet(lightSpecular_1[0], lightSpecular_1[1], lightSpecular_1[2], 0.0f);
			cBuffer.LightPosition[1] = XMVectorSet(lightPosition_1[0], lightPosition_1[1], lightPosition_1[2], lightPosition_1[3]);

			cBuffer.LightAmbient[2] = XMVectorSet(lightAmbient_2[0], lightAmbient_2[1], lightAmbient_2[2], 0.0f);
			cBuffer.LightDiffuse[2] = XMVectorSet(lightDiffuse_2[0], lightDiffuse_2[1], lightDiffuse_2[2], 0.0f);
			cBuffer.LightSpecular[2] = XMVectorSet(lightSpecular_2[0], lightSpecular_2[1], lightSpecular_2[2], 0.0f);
			cBuffer.LightPosition[2] = XMVectorSet(lightPosition_2[0], lightPosition_2[1], lightPosition_2[2], lightPosition_2[3]);

			cBuffer.MaterialAmbient = XMVectorSet(materialAmbient[0], materialAmbient[1], materialAmbient[2], 0.0f);
			cBuffer.MaterialDiffuse = XMVectorSet(materialDiffuse[0], materialDiffuse[1], materialDiffuse[2], 0.0f);
			cBuffer.MaterialSpecular = XMVectorSet(materialSpecular[0], materialSpecular[1], materialSpecular[2], 0.0f);
			cBuffer.MaterialShineness = materialShineness;		
		}
		else
		{
			cBuffer.KeyPress = 0;
		}

		// Uniform4fv(..);
		gpID3D11DeviceContext->UpdateSubresource(
			gpID3D11Buffer_constantBuffer_pf,
			0,
			NULL,
			&cBuffer,
			0,
			0
		);
	}
	else
	{
		gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader_pv, NULL, 0);
		gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader_pv, NULL, 0);

		gpID3D11DeviceContext->VSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pv);
		gpID3D11DeviceContext->PSSetConstantBuffers(0, 1, &gpID3D11Buffer_constantBuffer_pv);

		// Transformation
		XMMATRIX worldMatrix = XMMatrixIdentity();
		XMMATRIX translationMatrix = XMMatrixIdentity();
		XMMATRIX viewMatrix = XMMatrixIdentity();

		translationMatrix = XMMatrixTranslation(0.0f, 0.0f, 3.0f);	
		worldMatrix= translationMatrix;
		
		CBUFFER cBuffer ;
		ZeroMemory(&cBuffer, sizeof(CBUFFER));
		cBuffer.WorldMatrix = worldMatrix;
		cBuffer.ViewMatrix = viewMatrix;
		cBuffer.ProjectionMatrix = perspectiveProjectionMatrix;

		if (bLightingEnable == TRUE)
		{
			cBuffer.KeyPress = 1;

			
			cBuffer.LightAmbient[0] = XMVectorSet(lightAmbient_0[0], lightAmbient_0[1], lightAmbient_0[2], 0.0f);
			cBuffer.LightDiffuse[0] = XMVectorSet(lightDiffuse_0[0], lightDiffuse_0[1], lightDiffuse_0[2], 0.0f);
			cBuffer.LightSpecular[0] = XMVectorSet(lightSpecular_0[0], lightSpecular_0[1], lightSpecular_0[2], 0.0f);
			cBuffer.LightPosition[0] = XMVectorSet(lightPosition_0[0], lightPosition_0[1], lightPosition_0[2], lightPosition_0[3]);

			cBuffer.LightAmbient[1] = XMVectorSet(lightAmbient_1[0], lightAmbient_1[1], lightAmbient_1[2], 0.0f);
			cBuffer.LightDiffuse[1] = XMVectorSet(lightDiffuse_1[0], lightDiffuse_1[1], lightDiffuse_1[2], 0.0f);
			cBuffer.LightSpecular[1] = XMVectorSet(lightSpecular_1[0], lightSpecular_1[1], lightSpecular_1[2], 0.0f);
			cBuffer.LightPosition[1] = XMVectorSet(lightPosition_1[0], lightPosition_1[1], lightPosition_1[2], lightPosition_1[3]);

			cBuffer.LightAmbient[2] = XMVectorSet(lightAmbient_2[0], lightAmbient_2[1], lightAmbient_2[2], 0.0f);
			cBuffer.LightDiffuse[2] = XMVectorSet(lightDiffuse_2[0], lightDiffuse_2[1], lightDiffuse_2[2], 0.0f);
			cBuffer.LightSpecular[2] = XMVectorSet(lightSpecular_2[0], lightSpecular_2[1], lightSpecular_2[2], 0.0f);
			cBuffer.LightPosition[2] = XMVectorSet(lightPosition_2[0], lightPosition_2[1], lightPosition_2[2], lightPosition_2[3]);

			cBuffer.MaterialAmbient = XMVectorSet(materialAmbient[0], materialAmbient[1], materialAmbient[2], 0.0f);
			cBuffer.MaterialDiffuse = XMVectorSet(materialDiffuse[0], materialDiffuse[1], materialDiffuse[2], 0.0f);
			cBuffer.MaterialSpecular = XMVectorSet(materialSpecular[0], materialSpecular[1], materialSpecular[2], 0.0f);
			cBuffer.MaterialShineness = materialShineness;		
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

	// SET POSITION buffer here
	UINT Strid = sizeof(float) * 3;
	UINT offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(
		0,
		1,
		&gpID3D11Buffer_positionBuffer,
		&Strid,
		&offset
	);

	Strid = sizeof(float) * 3;
	offset = 0;
	gpID3D11DeviceContext->IASetVertexBuffers(
		1,
		1,
		&gpID3D11Buffer_normalBuffer,
		&Strid,
		&offset
	);

	// set index buffer
	gpID3D11DeviceContext->IASetIndexBuffer(
		gpID3D11Buffer_IndexBuffer,
		DXGI_FORMAT_R16_UINT,
		0
	); // R16 maps with 'short'

	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// draw
	gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

	gpIDXGISwapChain->Present(0, 0);
}

void update(void)
{
	// Code	
	if (bLightingEnable == TRUE)
	{
		// Animating light 0
		lightPosition_0[0] = 0.0f;// x rotation by rule
		lightPosition_0[1] = 5.0f * sin(lightAngleZero); // precision
		lightPosition_0[2] = 5.0f * cos(lightAngleZero); // light animate value
		lightPosition_0[3] = 1.0f; // positional light

		lightAngleZero = lightAngleZero + 0.0008f;
		if (lightAngleZero > 360.0f)
		{
			lightAngleZero = lightAngleZero - 360.0f;
		}

		// Animating light 1
		lightPosition_1[0] = 5.0f * cos(lightAngleOne); // light animate value
		lightPosition_1[1] = 0.0; // y rotation by rule
		lightPosition_1[2] = 5.0 * sin(lightAngleOne); // precision
		lightPosition_1[3] = 1.0f; // positional light 

		lightAngleOne = lightAngleOne + 0.0008f;
		if (lightAngleOne > 360.0f)
		{
			lightAngleOne = lightAngleOne - 360.0f;
		}

		// Animating light 2
		lightPosition_2[0] = 5.0f * cos(lightAngleTwo);
		lightPosition_2[1] = 5.0f * sin(lightAngleTwo); // light animate value
		lightPosition_2[2] = 0.0; // z rotation by rule
		lightPosition_2[3] = 1.0f; // positional light 

		lightAngleTwo = lightAngleTwo + 0.0008f;
		if (lightAngleTwo > 360.0f)
		{
			lightAngleTwo = lightAngleTwo - 360.0f;
		}
	}	
}

void uninitialized(void)
{
	// Function declaration
	void ToggleFullscreen(void);

	// Code
	if (gpID3D11Buffer_constantBuffer_pv)
	{
		gpID3D11Buffer_constantBuffer_pv->Release();
		gpID3D11Buffer_constantBuffer_pv = NULL;
	}

	if (gpID3D11Buffer_constantBuffer_pf)
	{
		gpID3D11Buffer_constantBuffer_pf->Release();
		gpID3D11Buffer_constantBuffer_pf = NULL;
	}

	if(gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

	if (gpID3D11Buffer_positionBuffer)
	{
		gpID3D11Buffer_positionBuffer->Release();
		gpID3D11Buffer_positionBuffer = NULL;
	}

	if (gpID3D11Buffer_IndexBuffer)
	{
		gpID3D11Buffer_IndexBuffer->Release();
		gpID3D11Buffer_IndexBuffer = NULL;
	}
	
	if (gpID3D11Buffer_normalBuffer)
	{
		gpID3D11Buffer_normalBuffer->Release();
		gpID3D11Buffer_normalBuffer = NULL;
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

	if (gpID3D11PixelShader_pf)
	{
		gpID3D11PixelShader_pf->Release();
		gpID3D11PixelShader_pf = NULL;
	}

	if (gpID3D11VertexShader_pv)
	{
		gpID3D11VertexShader_pv->Release();
		gpID3D11VertexShader_pv = NULL;
	}

	if (gpID3D11VertexShader_pf)
	{
		gpID3D11VertexShader_pf->Release();
		gpID3D11VertexShader_pf = NULL;
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

	gpFILE = fopen(gszLogFileName, "a+");
	fprintf(gpFILE, "gpFILE Closed ...\n");
	fclose(gpFILE);
}



