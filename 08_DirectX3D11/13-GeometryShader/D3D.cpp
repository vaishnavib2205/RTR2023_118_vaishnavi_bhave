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

// D3D related GLobal Variables
IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;

float clearColor[4];

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;
ID3D11HullShader *gpID3D11HullShader = NULL;
ID3D11DomainShader *gpID3D11DomainShader = NULL;
ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_PositionBuffer = NULL;   // analogous to genbuffers
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_Hull_Shader = NULL;	  // analogous to Uniforms in OpenGL
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_Domain_Shader = NULL;	  // analogous to Uniforms in OpenGL
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer_Pixel_Shader = NULL;	  // analogous to Uniforms in OpenGL

struct CBUFFER_HULL_SHADER
{
	XMVECTOR Hull_Constant_Function_Params;
};

struct CBUFFER_DOMAIN_SHADER
{
	XMMATRIX WorldViewProjectionMatrix;  // model matrix in OpenGL
};

struct CBUFFER_PIXEL_SHADER
{
	XMVECTOR LineColor;
};

unsigned int uiNumberOfLineSegments = 0;

XMMATRIX perspectiveProjectionMatrix;

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
		case VK_UP:
			uiNumberOfLineSegments++;
			if(uiNumberOfLineSegments >= 30)
			{
				uiNumberOfLineSegments = 30;
			}
			break;

		case VK_DOWN:
			uiNumberOfLineSegments--;
			if(uiNumberOfLineSegments <= 1)
			{
				uiNumberOfLineSegments = 1;
			}
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

HRESULT initialize(void)
{
	// code
	HRESULT resize(int , int);

	// Variable Declaration
	HRESULT hr = S_OK;

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

	// vertex shader

	const char* vertexShaderSourceCode = 
	"struct vertex_output" \
	"{" \
	"float4 position:POSITION;" \
	"};" \
	"vertex_output main(float2 pos:POSITION)" \
	"{" \
	"vertex_output output;" \
	"output.position = float4(pos, 0.0f , 1.0f);" \
	"return output;" \
	"}";

	ID3DBlob *piD3DBlob_VertexShaderSourceCode = NULL;
	ID3DBlob *pID3DBlob_Error = NULL;


	// compile above shader
	hr = D3DCompile(
		vertexShaderSourceCode ,                // glshadersource in OpenGL(1st paramater)
		lstrlenA(vertexShaderSourceCode) + 1 ,  // glshadersource in OpenGL(2nd Parameter)
		"VS" , 									// Vertex Shader
		NULL , 
		D3D_COMPILE_STANDARD_FILE_INCLUDE ,
		"main" ,								// give same function name as you gice in HLSL vertex shader
		"vs_5_0" ,
		0 ,
		0 ,
		&piD3DBlob_VertexShaderSourceCode ,
		&pID3DBlob_Error 
		);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Vertex Shader Compilation Error Log : %s \n" , (char *)pID3DBlob_Error->GetBufferPointer());
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Vertex Shader comipled Successfully !!!");
		fclose(gpFile);
	}

	// generallu all create calls are on device
	// and set related calls are on deviceContext

	// create the vertex shader from above code.
	hr = gpID3D11Device->CreateVertexShader(
		piD3DBlob_VertexShaderSourceCode->GetBufferPointer() ,
		piD3DBlob_VertexShaderSourceCode->GetBufferSize(),
		NULL ,
		&gpID3D11VertexShader		
		);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Create VertexShader Failed !!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Create VertexShader Successeded !!!");
		fclose(gpFile);
	}

	// set above created vertex shader into pipeline
	gpID3D11DeviceContext->VSSetShader(gpID3D11VertexShader , NULL , 0);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Hull shader

	const char *hullShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4 hull_constant_function_params;" \
		"}" \
		"struct vertex_output" \
		"{" \
		"float4 position:POSITION;" \
		"};" \
		"struct hull_constant_output" \
		"{" \
		"float edges[2] : SV_TESSFACTOR;" \
		"};" \
		"hull_constant_output hull_constant_function(void)" \
		"{" \
		"hull_constant_output output;" \
		"float numberOfStrips = hull_constant_function_params[0];" \
		"float numberOfSegments = hull_constant_function_params[1];" \
		"output.edges[0] = numberOfStrips;" \
		"output.edges[1] = numberOfSegments;" \
		"return output;" \
		"}" \
		"struct hull_output" \
		"{" \
		"float4 position:POSITION;" \
		"};" \
		"[domain(\"isoline\")]" \
		"[partitioning(\"integer\")]" \
		"[outputtopology(\"line\")]" \
		"[outputcontrolpoints(4)]" \
		"[patchconstantfunc(\"hull_constant_function\")]" \
		"hull_output main(InputPatch<vertex_output , 4> input_patch , uint i : SV_OUTPUTCONTROLPOINTID)" \
		"{" \
		"hull_output output;" \
		"output.position = input_patch[i].position;" \
		"return output;" \
		"}";

	
	ID3DBlob *piD3DBlob_HullShaderSourceCode = NULL;
	pID3DBlob_Error = NULL;


	// compile above shader
	hr = D3DCompile(
		hullShaderSourceCode ,                // glshadersource in OpenGL(1st paramater)
		lstrlenA(hullShaderSourceCode) + 1 ,  // glshadersource in OpenGL(2nd Parameter)
		"HS" , 									// Vertex Shader
		NULL , 
		D3D_COMPILE_STANDARD_FILE_INCLUDE ,
		"main" ,								// give same function name as you gice in HLSL vertex shader
		"hs_5_0" ,
		0 ,
		0 ,
		&piD3DBlob_HullShaderSourceCode ,
		&pID3DBlob_Error 
		);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Hull Shader Compilation Error Log : %s \n" , (char *)pID3DBlob_Error->GetBufferPointer());
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Hull Shader comipled Successfully !!!");
		fclose(gpFile);
	}

	// generallu all create calls are on device
	// and set related calls are on deviceContext

	// create the pixel shader from above code.
	hr = gpID3D11Device->CreateHullShader(
		piD3DBlob_HullShaderSourceCode->GetBufferPointer() ,
		piD3DBlob_HullShaderSourceCode->GetBufferSize(),
		NULL ,
		&gpID3D11HullShader		
		);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Create HullShader Failed !!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Create HullShader Successeded !!!");
		fclose(gpFile);
	}

	// set above created Pixel shader into pipeline
	gpID3D11DeviceContext->HSSetShader(gpID3D11HullShader , NULL , 0);

	// Release Hull Shader Blob
	if(piD3DBlob_HullShaderSourceCode)
	{
		piD3DBlob_HullShaderSourceCode->Release();
		piD3DBlob_HullShaderSourceCode = NULL;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Domain shader

	const char *domainShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4x4 worldViewProjectionMatrix;" \
		"}" \
		"struct hull_constant_output" \
		"{" \
		"float edges[2] : SV_TESSFACTOR;" \
		"};" \
		"struct hull_output" \
		"{" \
		"float4 position:POSITION;" \
		"};" \
		"struct domain_output" \
		"{" \
		"float4 position:SV_POSITION;" \
		"};" \
		"[domain(\"isoline\")]" \
		"domain_output main(hull_constant_output input , OutputPatch<hull_output , 4> output_patch , float2 tessCoord : SV_DOMAINLOCATION )" \
		"{" \
		"domain_output output;" \
		"float3 p0 = output_patch[0].position.xyz;" \
		"float3 p1 = output_patch[1].position.xyz;" \
		"float3 p2 = output_patch[2].position.xyz;" \
		"float3 p3 = output_patch[3].position.xyz;" \
		"float3 p = p0*(1.0-tessCoord.x)*(1.0-tessCoord.x)*(1.0-tessCoord.x)+p1*3.0*tessCoord.x*(1.0-tessCoord.x)*(1.0-tessCoord.x)+p2*3.0*tessCoord.x*tessCoord.x*(1.0-tessCoord.x)+p3*tessCoord.x*tessCoord.x*tessCoord.x;" \
		"output.position = mul(worldViewProjectionMatrix , float4(p , 1.0));" \
		"return output;" \
		"}";

	
	ID3DBlob *piD3DBlob_DomainShaderSourceCode = NULL;
	pID3DBlob_Error = NULL;


	// compile above shader
	hr = D3DCompile(
		domainShaderSourceCode ,                // glshadersource in OpenGL(1st paramater)
		lstrlenA(domainShaderSourceCode) + 1 ,  // glshadersource in OpenGL(2nd Parameter)
		"DS" , 									// Vertex Shader
		NULL , 
		D3D_COMPILE_STANDARD_FILE_INCLUDE ,
		"main" ,								// give same function name as you gice in HLSL vertex shader
		"ds_5_0" ,
		0 ,
		0 ,
		&piD3DBlob_DomainShaderSourceCode ,
		&pID3DBlob_Error 
		);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Domian Shader Compilation Error Log : %s \n" , (char *)pID3DBlob_Error->GetBufferPointer());
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Domian Shader comipled Successfully !!!");
		fclose(gpFile);
	}

	// generallu all create calls are on device
	// and set related calls are on deviceContext

	// create the pixel shader from above code.
	hr = gpID3D11Device->CreateDomainShader(
		piD3DBlob_DomainShaderSourceCode->GetBufferPointer() ,
		piD3DBlob_DomainShaderSourceCode->GetBufferSize(),
		NULL ,
		&gpID3D11DomainShader		
		);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Create DomainShader Failed !!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Create DomainShader Successeded !!!");
		fclose(gpFile);
	}

	// set above created Pixel shader into pipeline
	gpID3D11DeviceContext->DSSetShader(gpID3D11DomainShader , NULL , 0);

	// Release Hull Shader Blob
	if(piD3DBlob_DomainShaderSourceCode)
	{
		piD3DBlob_DomainShaderSourceCode->Release();
		piD3DBlob_DomainShaderSourceCode = NULL;
	}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// Pixel shader

	const char *pixelShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4 lineColor;" \
		"}" \
        "float4 main(void) : SV_TARGET" \
        "{" \
        "    float4 color = lineColor;" \
        "    return(color);" \
		"}";

	
	ID3DBlob *piD3DBlob_PixelShaderSourceCode = NULL;
	pID3DBlob_Error = NULL;


	// compile above shader
	hr = D3DCompile(
		pixelShaderSourceCode ,                // glshadersource in OpenGL(1st paramater)
		lstrlenA(pixelShaderSourceCode) + 1 ,  // glshadersource in OpenGL(2nd Parameter)
		"PS" , 									// Vertex Shader
		NULL , 
		D3D_COMPILE_STANDARD_FILE_INCLUDE ,
		"main" ,								// give same function name as you gice in HLSL vertex shader
		"ps_5_0" ,
		0 ,
		0 ,
		&piD3DBlob_PixelShaderSourceCode ,
		&pID3DBlob_Error 
		);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Pixel Shader Compilation Error Log : %s \n" , (char *)pID3DBlob_Error->GetBufferPointer());
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Pixel Shader comipled Successfully !!!");
		fclose(gpFile);
	}

	// generallu all create calls are on device
	// and set related calls are on deviceContext

	// create the pixel shader from above code.
	hr = gpID3D11Device->CreatePixelShader(
		piD3DBlob_PixelShaderSourceCode->GetBufferPointer() ,
		piD3DBlob_PixelShaderSourceCode->GetBufferSize(),
		NULL ,
		&gpID3D11PixelShader		
		);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Create PixelShader Failed !!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "Create PixelShader Successeded !!!");
		fclose(gpFile);
	}

	// set above created Pixel shader into pipeline
	gpID3D11DeviceContext->PSSetShader(gpID3D11PixelShader , NULL , 0);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// Release Error Blob
	if(pID3DBlob_Error)
	{
		pID3DBlob_Error->Release();
		pID3DBlob_Error = NULL;
	}

	// Release Pixel Shader Blob
	if(piD3DBlob_PixelShaderSourceCode)
	{
		piD3DBlob_PixelShaderSourceCode->Release();
		piD3DBlob_PixelShaderSourceCode = NULL;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// initialize input element structure

	// similar to glBindAtrrinLocation , in more details
	D3D11_INPUT_ELEMENT_DESC d3dInputElementDesc;
	ZeroMemory((void *)&d3dInputElementDesc , sizeof(D3D11_INPUT_ELEMENT_DESC));

	d3dInputElementDesc.SemanticName = "POSITION";
	d3dInputElementDesc.SemanticIndex = 0;
	d3dInputElementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
	d3dInputElementDesc.InputSlot= 0;
	d3dInputElementDesc.AlignedByteOffset = 0;
	d3dInputElementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3dInputElementDesc.InstanceDataStepRate = 0;

	// using above structure create input layout

	hr = gpID3D11Device->CreateInputLayout(&d3dInputElementDesc , 1 , piD3DBlob_VertexShaderSourceCode->GetBufferPointer() , piD3DBlob_VertexShaderSourceCode->GetBufferSize() , &gpID3D11InputLayout);
	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateInputLayout on gpID3D11Device Failed !!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateInputLayout on gpID3D11Device is Success !!!");
		fclose(gpFile);
	}

	// set above input layout in pipeline
	gpID3D11DeviceContext->IASetInputLayout(gpID3D11InputLayout);


	// now we can release vertexshader source code blob
	if(piD3DBlob_VertexShaderSourceCode)
	{
		piD3DBlob_VertexShaderSourceCode->Release();
		piD3DBlob_VertexShaderSourceCode = NULL;
	}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// declare triangle geometry
	const float point_position[] =
	{
		-1.0f , -1.0f , // third quadrant
		-0.5f , 1.0f , // 
		 0.5   , -1.0f , 
		 1.0f  ,  1.0f
	};

	// create vertex buffer for position
	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory((void *)&d3dBufferDesc , sizeof(D3D11_BUFFER_DESC));

	d3dBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	d3dBufferDesc.ByteWidth = sizeof(point_position);
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	// create vertex buffer using above strucuture
	hr = gpID3D11Device->CreateBuffer(
		&d3dBufferDesc , 
		NULL , 
		&gpID3D11Buffer_PositionBuffer
	);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateBuffer Failed for position buffer!!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateBuffer is Success for position buffer !!!");
		fclose(gpFile);
	}

	// we will set abouve buffer in pipeline through display, cause our buffer is dynamic

	// copy mannuly vertex data from CPU Buffer to GPU Buffer
	D3D11_MAPPED_SUBRESOURCE  d3dMappedSubresource;
	ZeroMemory((void *)&d3dMappedSubresource , sizeof(D3D11_MAPPED_SUBRESOURCE));

	// now map our position buffer with above resource
	gpID3D11DeviceContext->Map(gpID3D11Buffer_PositionBuffer , 0 , D3D11_MAP_WRITE_DISCARD ,  0 , &d3dMappedSubresource);

	// Now Copy actual data
	memcpy(d3dMappedSubresource.pData , point_position , sizeof(point_position));

	gpID3D11DeviceContext->Unmap(gpID3D11Buffer_PositionBuffer , 0);

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// create constant buffer for HULL Shader

	ZeroMemory((void *)&d3dBufferDesc , sizeof(D3D11_BUFFER_DESC));

	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CBUFFER_HULL_SHADER);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// create vertex buffer using above strucuture
	hr = gpID3D11Device->CreateBuffer(
		&d3dBufferDesc , 
		NULL , 
		&gpID3D11Buffer_ConstantBuffer_Hull_Shader
	);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateBuffer Failed for creating Hull constant buffer!!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateBuffer is Success for Hull constant buffer !!!");
		fclose(gpFile);
	}

		// set above buffer into pipeline
	gpID3D11DeviceContext->HSSetConstantBuffers(0 ,1 , &gpID3D11Buffer_ConstantBuffer_Hull_Shader);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// create constant buffer for domain shader

	ZeroMemory((void *)&d3dBufferDesc , sizeof(D3D11_BUFFER_DESC));

	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CBUFFER_DOMAIN_SHADER);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// create vertex buffer using above strucuture
	hr = gpID3D11Device->CreateBuffer(
		&d3dBufferDesc , 
		NULL , 
		&gpID3D11Buffer_ConstantBuffer_Domain_Shader

	);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateBuffer Failed for creating Domain constant buffer!!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateBuffer is Success for  Domainconstant buffer !!!");
		fclose(gpFile);
	}

			// set above buffer into pipeline
	gpID3D11DeviceContext->DSSetConstantBuffers(0 ,1 , &gpID3D11Buffer_ConstantBuffer_Domain_Shader);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// create constant buffer to send transformation like uniform data

	ZeroMemory((void *)&d3dBufferDesc , sizeof(D3D11_BUFFER_DESC));

	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(CBUFFER_PIXEL_SHADER);
	d3dBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// create vertex buffer using above strucuture
	hr = gpID3D11Device->CreateBuffer(
		&d3dBufferDesc , 
		NULL , 
		&gpID3D11Buffer_ConstantBuffer_Pixel_Shader

	);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateBuffer Failed for creating Pixel constant buffer!!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateBuffer is Success for Pixel constant buffer !!!");
		fclose(gpFile);
	}

			// set above buffer into pipeline
	gpID3D11DeviceContext->PSSetConstantBuffers(0 ,1 , &gpID3D11Buffer_ConstantBuffer_Pixel_Shader);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	// clear color
	clearColor[0] = 0.0f;
	clearColor[1] = 0.0f;
	clearColor[2] = 0.0f;
	clearColor[3] = 1.0f;

	uiNumberOfLineSegments = 1;    // for 1 line at the start

	perspectiveProjectionMatrix = XMMatrixIdentity();

	// WarmUp Resize
	hr = resize(WIN_WIDTH , WIN_HEIGHT);

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

HRESULT resize(int width  , int height)
{
	// Variable Declaration
	HRESULT hr = S_OK;

	// release RTV if it is alredy created
	if(gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	// recreate swapchain buffer
	gpIDXGISwapChain->ResizeBuffers(1 , width , height , DXGI_FORMAT_B8G8R8A8_UNORM , 0);

	// a) get the buffer from RTV for swapchain into the texture
	ID3D11Texture2D *piD3D11Texture2D = NULL;

	gpIDXGISwapChain->GetBuffer(0 , __uuidof(ID3D11Texture2D) , (void **)&piD3D11Texture2D);

	// b) Create new RTV using above buffer
	hr = gpID3D11Device->CreateRenderTargetView(piD3D11Texture2D , NULL , &gpID3D11RenderTargetView);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateRenderTargetView Failed !!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateRenderTargetView is Success !!!");
		fclose(gpFile);
	}

	piD3D11Texture2D->Release();
	piD3D11Texture2D = NULL;

	// c) sewt this new render target view in pipeline
	gpID3D11DeviceContext->OMSetRenderTargets(1 , &gpID3D11RenderTargetView , NULL);


	// initialize and set the view port , just like view port in opengl
	D3D11_VIEWPORT d3dViewPort;

	ZeroMemory((void *)&d3dViewPort , sizeof(D3D11_VIEWPORT));

	d3dViewPort.TopLeftX = 0.0f;
	d3dViewPort.TopLeftY = 0.0f;
	d3dViewPort.Width = (float)width;
	d3dViewPort.Height = (float)height;
	d3dViewPort.MinDepth = 0.0f;
	d3dViewPort.MaxDepth = 1.0f;

	// set this viewPort in pipeline
	gpID3D11DeviceContext->RSSetViewports(1 , &d3dViewPort);

	// initialize perspectiveProjectionMatrix
	perspectiveProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f) , (float)width/(float)height , 0.1f , 100.0f);

	
	return hr;
	
}

void display(void)
{
	// code
	// clear the RTV using clear color
	gpID3D11DeviceContext->ClearRenderTargetView(gpID3D11RenderTargetView , clearColor);

	// tranformations
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();

	worldMatrix = XMMatrixTranslation(0.0f , 0.0f , 3.0f);

	XMMATRIX wvpMatrix = worldMatrix * viewMatrix * perspectiveProjectionMatrix;

	// Update transformation matrix in constant buffer of domain shader
	CBUFFER_DOMAIN_SHADER constantBuffer_domain_shader;
	ZeroMemory((void *)&constantBuffer_domain_shader , sizeof(CBUFFER_DOMAIN_SHADER));

	constantBuffer_domain_shader.WorldViewProjectionMatrix = wvpMatrix;

	// glUniformMateix4fv in OpenGL
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_Domain_Shader , 0 , NULL , &constantBuffer_domain_shader , 0 , 0 );

	// Update lineStrips and lineSegments in constant buffer of hull shader
	CBUFFER_HULL_SHADER constantBuffer_hull_shader;
	ZeroMemory((void *)&constantBuffer_hull_shader , sizeof(CBUFFER_HULL_SHADER));

	constantBuffer_hull_shader.Hull_Constant_Function_Params = XMVectorSet(1.0f , uiNumberOfLineSegments , 0.0 , 0.0);

	// display changing number of line segments in windows caption bar
	TCHAR str[255];
	wsprintf(str , TEXT("Prasad Bhalkikar [Number Of Line Segments = %d]"), uiNumberOfLineSegments);
	SetWindowText(ghwnd , str);

	// glUniformMateix4fv in OpenGL
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_Hull_Shader , 0 , NULL , &constantBuffer_hull_shader , 0 , 0 );

	// Update LineColor in constant buffer of pixel shader

	CBUFFER_PIXEL_SHADER constantBuffer_pixel_shader;
	ZeroMemory((void *)&constantBuffer_pixel_shader , sizeof(CBUFFER_PIXEL_SHADER));

	constantBuffer_pixel_shader.LineColor = XMVectorSet(1.0f , 1.0f , 0.0f , 1.0f);

	// glUniformMateix4fv in OpenGL
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer_Pixel_Shader , 0 , NULL , &constantBuffer_pixel_shader , 0 , 0 );


	// set position buffer into pipeline here
	// parallel to vao binding in OpenGL
	UINT stride = sizeof(float) * 2;
	UINT offSet = 0  ;

	gpID3D11DeviceContext->IASetVertexBuffers(0 , 1 , &gpID3D11Buffer_PositionBuffer , &stride , &offSet);

	// set primitive geometry --- gldrawArrays's first parameter
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);  // GL_TRIANGLES

	// Draw The geometry
	gpID3D11DeviceContext->Draw(4 , 0);

	// Do double buffering by presenting the swapchain
	gpIDXGISwapChain->Present(0 , 0);

}
 
void update(void)
{
	// code

}

void uninitialized(void)
{
	// code

	if(gpID3D11Buffer_ConstantBuffer_Pixel_Shader)
	{
		gpID3D11Buffer_ConstantBuffer_Pixel_Shader->Release();
		gpID3D11Buffer_ConstantBuffer_Pixel_Shader = NULL;
	}

	if(gpID3D11Buffer_ConstantBuffer_Domain_Shader)
	{
		gpID3D11Buffer_ConstantBuffer_Domain_Shader->Release();
		gpID3D11Buffer_ConstantBuffer_Domain_Shader = NULL;
	}

	if(gpID3D11Buffer_ConstantBuffer_Hull_Shader)
	{
		gpID3D11Buffer_ConstantBuffer_Hull_Shader->Release();
		gpID3D11Buffer_ConstantBuffer_Hull_Shader = NULL;
	}

	if(gpID3D11Buffer_PositionBuffer)
	{
		gpID3D11Buffer_PositionBuffer->Release();
		gpID3D11Buffer_PositionBuffer = NULL;
	}

	if(gpID3D11InputLayout)
	{
		gpID3D11InputLayout->Release();
		gpID3D11InputLayout = NULL;
	}

	if(gpID3D11PixelShader)
	{
		gpID3D11PixelShader->Release();
		gpID3D11PixelShader = NULL;
	}

	if(gpID3D11DomainShader)
	{
		gpID3D11DomainShader->Release();
		gpID3D11DomainShader = NULL;
	}

	if(gpID3D11HullShader)
	{
		gpID3D11HullShader->Release();
		gpID3D11HullShader = NULL;
	}

	if(gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}

	if(gpID3D11RenderTargetView)
	{
		gpID3D11RenderTargetView->Release();
		gpID3D11RenderTargetView = NULL;
	}

	if(gpID3D11DeviceContext)
	{
		gpID3D11DeviceContext->Release();
		gpID3D11DeviceContext = NULL;
	}

	if(gpIDXGISwapChain)
	{
		gpIDXGISwapChain->Release();
		gpIDXGISwapChain = NULL;
	}

	if(gpID3D11Device)
	{
		gpID3D11Device->Release();
		gpID3D11Device = NULL;
	}

	gpFile = fopen(gszLogFileName, "a+");
	fprintf(gpFile , "Log File Is Successfully Closed !!!");
	fclose(gpFile);

}



