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

// D3D related GLobal Variables
IDXGISwapChain *gpIDXGISwapChain = NULL;
ID3D11Device *gpID3D11Device = NULL;
ID3D11DeviceContext *gpID3D11DeviceContext = NULL;
ID3D11RenderTargetView *gpID3D11RenderTargetView = NULL;
ID3D11DepthStencilView *gpID3D11DepthStencilView = NULL;

float clearColor[4];

ID3D11VertexShader *gpID3D11VertexShader = NULL;
ID3D11PixelShader *gpID3D11PixelShader = NULL;
ID3D11InputLayout *gpID3D11InputLayout = NULL;
ID3D11Buffer *gpID3D11Buffer_VertexBuffer_Position = NULL;   // analogous to genbuffers.
ID3D11Buffer *gpID3D11Buffer_NormalBuffer = NULL;   // analogous to genbuffers.
ID3D11Buffer *gpID3D11Buffer_ColorBuffer = NULL;   // analogous to vbo_color.
ID3D11Buffer *gpID3D11Buffer_ConstantBuffer = NULL;	  // analogous to Uniforms in OpenGL.

ID3D11RasterizerState *gpID3D11RasterizerState = NULL;

ID3D11Buffer *gpID3D11Buffer_IndexBuffer = NULL;

float sphere_vertices[1146];
float sphere_normals[1146];
float sphere_textures[764];
unsigned short sphere_elements[2280];
unsigned int gNumElements;
unsigned int gNumVertices;

struct CBUFFER
{
	XMMATRIX WorldMatrix;  // model matrix in OpenGL
	XMMATRIX ViewMatrix;  // model matrix in OpenGL
	XMMATRIX ProjectionMatrix;  // model matrix in OpenGL

	XMVECTOR LightAmbient;
	XMVECTOR LightDiffuse;
	XMVECTOR LightSpecular;
	XMVECTOR MaterialAmbient;
	XMVECTOR MaterialDiffuse;
	XMVECTOR MaterialSpecular;
	XMVECTOR LightPosition;

	float MaterialShininess;

	unsigned int KeyPressed;
};

BOOL bLightEnabled = FALSE;

float lightAmbient[] = { 0.1f , 0.1f , 0.1f , 1.0f };
float lightDiffuse[] = { 1.0f , 1.0f , 1.0f , 1.0f }; // white diffuse light
float lightSpecular[] = { 1.0f , 1.0f , 1.0f , 1.0f };
float lightPosition[] = { 100.0f , 100.0f , -100.0f , 1.0f };

float materialAmbient[] = { 0.0f , 0.0f , 0.0f , 1.0f };
float materialDiffuse[] = { 1.0f , 1.0f , 1.0f , 1.0f };
float materialSpecular[] = { 1.0f , 1.0f , 1.0f , 1.0f };
float materialShineness =  50.0f ;

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

			if (bLightEnabled == FALSE)
			{
				bLightEnabled = TRUE;
			}
			else
			{
				bLightEnabled = FALSE;
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
	"cbuffer ConstantBuffer" \
	"{" \
	"float4x4 worldMatrix; \n" \
	"float4x4 viewMatrix; \n" \
	"float4x4 projectionMatrix; \n" \
	"float4 lightAmbient; \n" \
	"float4 lightDiffuse; \n" \
	"float4 lightSpecular; \n" \
	"float4 materialAmbient; \n" \
	"float4 materialDiffuse; \n" \
	"float4 materialSpecular; \n" \
	"float4 lightPosition; \n" \
	"float materialShininess; \n" \
	"uint keyPressed; \n" \
	"}" \
	"struct vertex_output" \
	"{" \
	"float4 position:SV_POSITION; \n" \
	"float3 transformedNormals:NORMAL0; \n" \
	"float3 lightDirection:NORMAL1; \n" \
	"float3 viewerVector:NORMAL2; \n" \
	"};" \
	"vertex_output main(float4 pos:POSITION , float3 norm:NORMAL)\n" \
	"{" \
	"vertex_output output;\n" \
	"if(keyPressed == 1) \n" \
	"{ \n" \
	"float4 eyeCoordinates = mul(viewMatrix , mul(worldMatrix , pos)); \n" \
	"output.transformedNormals = mul((float3x3)worldMatrix , norm); \n " \
	"output.lightDirection = (float3)(lightPosition) - (float3)eyeCoordinates; \n " \
	"output.viewerVector = -eyeCoordinates.xyz; \n " \
	"}  \n" \
	"else  \n" \
	"{  \n" \
	"output.transformedNormals = float3(0.0f , 0.0f , 0.0f); \n" \
	"output.lightDirection = float3(0.0f , 0.0f , 0.0f); \n" \
	"output.viewerVector = float3(0.0f , 0.0f , 0.0f); \n" \
	"}  \n" \
	"float4 position = mul(projectionMatrix , mul(viewMatrix , mul(worldMatrix , pos)));" \
	"output.position = position;" \
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

	// Pixel shader

	const char *pixelShaderSourceCode =
		"cbuffer ConstantBuffer" \
		"{" \
		"float4x4 worldMatrix; \n" \
		"float4x4 viewMatrix; \n" \
		"float4x4 projectionMatrix; \n" \
		"float4 lightAmbient; \n" \
		"float4 lightDiffuse; \n" \
		"float4 lightSpecular; \n" \
		"float4 materialAmbient; \n" \
		"float4 materialDiffuse; \n" \
		"float4 materialSpecular; \n" \
		"float4 lightPosition; \n" \
		"float materialShininess; \n" \
		"uint keyPressed; \n" \
		"}" \
		"struct vertex_output" \
		"{" \
		"float4 position:SV_POSITION; \n" \
		"float3 transformedNormals:NORMAL0; \n" \
		"float3 lightDirection:NORMAL1; \n" \
		"float3 viewerVector:NORMAL2; \n" \
		"};" \
        "float4 main(vertex_output input) : SV_TARGET" \
        "{" \
        "float3 phong_ads_light; \n" \
        "if(keyPressed == 1)" \
        "{" \
        "float3 normalizedTranformedNormals = normalize(input.transformedNormals); \n" \
        "float3 normalizedLightDirection = normalize(input.lightDirection); \n" \
        "float3 normalizedViewerVector = normalize(input.viewerVector); \n" \
		"float3 reflectionVector = reflect(-normalizedLightDirection , normalizedTranformedNormals); \n " \
		"float3 ambientLight = lightAmbient * materialAmbient; \n " \
		"float3 diffuseLight = lightDiffuse * materialDiffuse * max(dot(normalizedLightDirection , normalizedTranformedNormals) , 0.0f); \n " \
		"float3 specularLight = lightSpecular * materialSpecular * pow(max(dot(reflectionVector , normalizedViewerVector) , 0.0f) , materialShininess); \n " \
		"phong_ads_light = ambientLight + diffuseLight + specularLight;  \n" \
		"}  \n" \
		"else \n" \
		"{ \n" \
		"phong_ads_light = float3(1.0f , 1.0f , 1.0f); \n" \
		"} \n" \
        "    float4 color = float4(phong_ads_light , 1.0f); \n" \
        "    return(color); \n" \
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
	D3D11_INPUT_ELEMENT_DESC d3dInputElementDesc[2];
	ZeroMemory((void *)&d3dInputElementDesc , sizeof(D3D11_INPUT_ELEMENT_DESC)*_ARRAYSIZE(d3dInputElementDesc));

	// for  vertives position (VBO_POSITION in OpenGL)
	d3dInputElementDesc[0].SemanticName = "POSITION";
	d3dInputElementDesc[0].SemanticIndex = 0;
	d3dInputElementDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	d3dInputElementDesc[0].InputSlot= 0;	// position will be second input slot
	d3dInputElementDesc[0].AlignedByteOffset = 0;
	d3dInputElementDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3dInputElementDesc[0].InstanceDataStepRate = 0;

	// now do for color
	d3dInputElementDesc[1].SemanticName = "NORMAL";
	d3dInputElementDesc[1].SemanticIndex = 0;
	d3dInputElementDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	d3dInputElementDesc[1].InputSlot= 1;  // color will be second input slot
	d3dInputElementDesc[1].AlignedByteOffset = 0;
	d3dInputElementDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	d3dInputElementDesc[1].InstanceDataStepRate = 0;

	// using above structure create input layout

	hr = gpID3D11Device->CreateInputLayout(d3dInputElementDesc , _ARRAYSIZE(d3dInputElementDesc) , piD3DBlob_VertexShaderSourceCode->GetBufferPointer() , piD3DBlob_VertexShaderSourceCode->GetBufferSize() , &gpID3D11InputLayout);
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

    getSphereVertexData(sphere_vertices, sphere_normals, sphere_textures, sphere_elements);
    gNumVertices = getNumberOfSphereVertices();
    gNumElements = getNumberOfSphereElements();

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// For position

	// create vertex buffer for position
	D3D11_BUFFER_DESC bufferDesc;
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = gNumVertices * 3 * sizeof(float);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA d3d11SubresourceData;
	ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3d11SubresourceData.pSysMem = sphere_vertices;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, &d3d11SubresourceData, &gpID3D11Buffer_VertexBuffer_Position);

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


	// Normals VB
	// Follow Same Above Method For Normals And Textures
	ZeroMemory(&bufferDesc, sizeof(D3D11_BUFFER_DESC));
	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = gNumVertices * 3 * sizeof(float);
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	ZeroMemory((void*)&d3d11SubresourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3d11SubresourceData.pSysMem = sphere_normals;
	hr = gpID3D11Device->CreateBuffer(&bufferDesc, &d3d11SubresourceData, &gpID3D11Buffer_NormalBuffer);
	if (FAILED(hr))
	{
		fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateBuffer failed sphere normals failed\n");
		fclose(gpFile);

		return hr;
	}
	else
	{
		fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateBuffer failed sphere normals success\n");
		fclose(gpFile);

		
	}

	

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


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
		fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateBuffer failed sphere element failed\n");
		fclose(gpFile);

		return hr;
	}
	else
	{
		fopen(gszLogFileName, "a+");
		fprintf(gpFile, "CreateBuffer failed sphere element success\n");
		fclose(gpFile);
	}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// create constant buffer to send transformation like uniform data

	ZeroMemory((void *)&bufferDesc , sizeof(D3D11_BUFFER_DESC));

	bufferDesc.Usage = D3D11_USAGE_DEFAULT;
	bufferDesc.ByteWidth = sizeof(CBUFFER);
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	// create vertex buffer using above strucuture
	hr = gpID3D11Device->CreateBuffer(
		&bufferDesc , 
		NULL , 
		&gpID3D11Buffer_ConstantBuffer

	);

	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateBuffer Failed for creating constant buffer!!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateBuffer is Success for constant buffer !!!");
		fclose(gpFile);
	}

	// set above buffer into pipeline
	gpID3D11DeviceContext->VSSetConstantBuffers(0 ,1 , &gpID3D11Buffer_ConstantBuffer);
	gpID3D11DeviceContext->PSSetConstantBuffers(0 ,1 , &gpID3D11Buffer_ConstantBuffer);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	// create and set the rasterizer state to off backface culling

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
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateRasterizerState Failed !!!");
		fclose(gpFile);

		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateRasterizerState is Success !!!");
		fclose(gpFile);
	}

	// set this state in pipeline
	gpID3D11DeviceContext->RSSetState(gpID3D11RasterizerState);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




	// clear color
	clearColor[0] = 0.0f;
	clearColor[1] = 0.0f;
	clearColor[2] = 0.0f;
	clearColor[3] = 1.0f;

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

	// release DSV if it is alredy created
	if(gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
	}

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
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateTexture2D Failed !!!");
		fclose(gpFile);

		return hr;
	}

	// create DSV according to above dept buffer texture
	D3D11_DEPTH_STENCIL_VIEW_DESC d3dDepthStencilViewDesc;
	ZeroMemory((void *)&d3dDepthStencilViewDesc , sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));

	d3dDepthStencilViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	d3dDepthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;  // multi sampling


	hr = gpID3D11Device->CreateDepthStencilView(piD3D11Texture2D_DepthBuffer , &d3dDepthStencilViewDesc , &gpID3D11DepthStencilView );
	if(FAILED(hr))
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateDepthStencilView Failed !!!");
		fclose(gpFile);

		piD3D11Texture2D_DepthBuffer->Release();
		piD3D11Texture2D_DepthBuffer = NULL;
		return hr;
	}
	else
	{
		gpFile = fopen(gszLogFileName, "a+");
		fprintf(gpFile , "CreateDepthStencilView is Success !!!");
		fclose(gpFile);
	}

	piD3D11Texture2D_DepthBuffer->Release();
	piD3D11Texture2D_DepthBuffer = NULL;

	


	// c) sewt this new render target view in pipeline
	gpID3D11DeviceContext->OMSetRenderTargets(1 , &gpID3D11RenderTargetView , gpID3D11DepthStencilView);




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
	gpID3D11DeviceContext->ClearDepthStencilView(gpID3D11DepthStencilView , D3D11_CLEAR_DEPTH , 1.0f , 0);

	// tranformations
	XMMATRIX worldMatrix = XMMatrixIdentity();
	XMMATRIX translationMatrix = XMMatrixIdentity();
	XMMATRIX viewMatrix = XMMatrixIdentity();

	translationMatrix = XMMatrixTranslation(0.0f , 0.0f , 2.0f);

	worldMatrix= translationMatrix;

	CBUFFER constantBuffer;
	ZeroMemory((void *)&constantBuffer , sizeof(CBUFFER));

	constantBuffer.WorldMatrix = worldMatrix;
	constantBuffer.ViewMatrix = viewMatrix;
	constantBuffer.ProjectionMatrix = perspectiveProjectionMatrix;

	if(bLightEnabled == TRUE)
	{
		constantBuffer.LightAmbient = XMVectorSet(lightAmbient[0] , lightAmbient[1] , lightAmbient[2] , 0.0f);
		constantBuffer.LightDiffuse = XMVectorSet(lightDiffuse[0] , lightDiffuse[1] , lightDiffuse[2] , 0.0f);
		constantBuffer.LightSpecular = XMVectorSet(lightSpecular[0] , lightSpecular[1] , lightSpecular[2] , 0.0f);

		constantBuffer.MaterialAmbient = XMVectorSet(materialAmbient[0] , materialAmbient[1] , materialAmbient[2] , 0.0f);
		constantBuffer.MaterialDiffuse = XMVectorSet(materialDiffuse[0] , materialDiffuse[1] , materialDiffuse[2] , 0.0f);
		constantBuffer.MaterialSpecular = XMVectorSet(materialSpecular[0] , materialSpecular[1] , materialSpecular[2] , 0.0f);

		constantBuffer.LightPosition = XMVectorSet(lightPosition[0] , lightPosition[1] , lightPosition[2] , lightPosition[3]);

		constantBuffer.MaterialShininess = materialShineness;

		constantBuffer.KeyPressed = 1;

	}
	else
	{
		constantBuffer.KeyPressed = 0;
	}


	// glUniformMateix4fv in OpenGL
	gpID3D11DeviceContext->UpdateSubresource(gpID3D11Buffer_ConstantBuffer , 0 , NULL , &constantBuffer , 0 , 0 );

	// set position buffer into pipeline here
	// parallel to vao binding in OpenGL
	UINT stride = sizeof(float) * 3;
	UINT offSet = 0  ;

	gpID3D11DeviceContext->IASetVertexBuffers(0 , 1 , &gpID3D11Buffer_VertexBuffer_Position , &stride , &offSet);

	// set color buffer into pipeline here
	// parallel to vao_color binding in OpenGL

	stride = sizeof(float) * 3;
	offSet = 0  ;

	gpID3D11DeviceContext->IASetVertexBuffers(1 , 1 , &gpID3D11Buffer_NormalBuffer , &stride , &offSet);


	// set index buffer
	gpID3D11DeviceContext->IASetIndexBuffer(gpID3D11Buffer_IndexBuffer, DXGI_FORMAT_R16_UINT, 0); // R16 maps with 'short'

	// set primitive geometry --- gldrawArrays's first parameter
	gpID3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);  // GL_TRIANGLES

    // draw
	gpID3D11DeviceContext->DrawIndexed(gNumElements, 0, 0);

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

	if(gpID3D11Buffer_ConstantBuffer)
	{
		gpID3D11Buffer_ConstantBuffer->Release();
		gpID3D11Buffer_ConstantBuffer = NULL;
	}

	if(gpID3D11RasterizerState)
	{
		gpID3D11RasterizerState->Release();
		gpID3D11RasterizerState = NULL;
	}

	if(gpID3D11Buffer_ColorBuffer)
	{
		gpID3D11Buffer_ColorBuffer->Release();
		gpID3D11Buffer_ColorBuffer = NULL;
	}

	if(gpID3D11Buffer_VertexBuffer_Position)
	{
		gpID3D11Buffer_VertexBuffer_Position->Release();
		gpID3D11Buffer_VertexBuffer_Position = NULL;
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

	if(gpID3D11VertexShader)
	{
		gpID3D11VertexShader->Release();
		gpID3D11VertexShader = NULL;
	}

	if(gpID3D11DepthStencilView)
	{
		gpID3D11DepthStencilView->Release();
		gpID3D11DepthStencilView = NULL;
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

