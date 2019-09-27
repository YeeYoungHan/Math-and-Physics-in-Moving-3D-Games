//------------------------------------------------------------
// 3DPhysics_5_4.cpp
// ���� ������(ZXŰ�� ������� ȸ��)
// 
//------------------------------------------------------------

#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>

#define VIEW_WIDTH					800						// ȭ��ʺ�
#define VIEW_HEIGHT					600						// ȭ�����

#define PI							3.1415927f				// ������
#define PLAYER_SPEED				0.2f					// �÷��̾� �̵��ӵ�
#define GROUND_SIZE					20.0f					// �ٴ� ũ��
#define GROUND_DIVIDE_NUM			30						// �ٴ� ���Ҽ�
#define BLOCK_NUM				( GROUND_DIVIDE_NUM )		// ������ϼ�
#define BLOCK_WIDTH				( GROUND_SIZE / BLOCK_NUM )	// ��� �ʺ�
#define ROUND_TIME					2000					// �ֱ�(�и��� ����)
#define ANGLE_SPEED					( 2.0f * PI / 200.0f )	// �ļ����� ȸ���ӵ�


// ���� ����
float		g_fHeights[GROUND_DIVIDE_NUM + 1][GROUND_DIVIDE_NUM + 1];


// ���� ����ü
struct CUSTOMVERTEX {
    XMFLOAT4	v4Pos;
	XMFLOAT2	v2UV;
};


struct MY_PLAYER {
	XMFLOAT3			v3Pos;					// ��ġ
};

MY_PLAYER	Player_1;							// �÷��̾� ������


// ǥ�� ������ �ۼ�
// �ļ� ���͸� �̿��� �ĵ��� ������� ����
int MakeHeights( void )
{
	int					i, j;
	float				x, z;
	float				kx, kz;					// �ļ� ����
	float				fTime;
	static float		fAngle = PI / 6.0f;

	if ( GetAsyncKeyState( 'Z' ) ) {
		fAngle += ANGLE_SPEED;
		if ( fAngle > 2.0f * PI ) fAngle -= 2.0f * PI;
	}
	if ( GetAsyncKeyState( 'X' ) ) {
		fAngle -= ANGLE_SPEED;
		if ( fAngle < 0.0f ) fAngle += 2.0f * PI;
	}

	// �ļ� ���� ����
	kx = cosf( fAngle ) / GROUND_SIZE;
	kz = sinf( fAngle ) / GROUND_SIZE;

	// �ð�
	fTime = ( float )( timeGetTime() % ROUND_TIME );

	z = -GROUND_SIZE / 2;
	for ( i = 0; i < GROUND_DIVIDE_NUM + 1; i++ ) {
		x = -GROUND_SIZE / 2;
		for ( j = 0; j < GROUND_DIVIDE_NUM + 1; j++ ) {
			g_fHeights[i][j] = 3.0f * sinf( 2.0f * PI * ( fTime / ROUND_TIME
														  -( kx * x + kz * z ) ) );
			x += BLOCK_WIDTH;
		}
		z += BLOCK_WIDTH;
	}

	return 0;
}


int InitPlayer( void )									// �÷��̾� �ʱ�ȭ
{
	// �÷��̾�1
	Player_1.v3Pos = XMFLOAT3( 0.0f, 0.0f, -4.0f );

	return 0;
}


int MovePlayer( void )									// ���� �̵�
{
	// ��
	if ( GetAsyncKeyState( VK_LEFT ) ) {
		Player_1.v3Pos.x -= PLAYER_SPEED;
	}
	// ��
	if ( GetAsyncKeyState( VK_RIGHT ) ) {
		Player_1.v3Pos.x += PLAYER_SPEED;
	}
	// ��
	if ( GetAsyncKeyState( VK_UP ) ) {
		Player_1.v3Pos.z += PLAYER_SPEED;
	}
	// ��
	if ( GetAsyncKeyState( VK_DOWN ) ) {
		Player_1.v3Pos.z -= PLAYER_SPEED;
	}

	return 0;
}


//------------------------------------------------------------
// �Ʒ��� DirectX�� ǥ���ϴ� ���α׷�

#include <stdio.h>
#include <windows.h>
#include <tchar.h>								// Unicode?��Ƽ����Ʈ ���ڰ���


#define MAX_BUFFER_VERTEX				10000	// �ִ����������
#define MAX_BUFFER_INDEX				20000	// �ִ�����ε�������
#define MAX_MODEL_NUM					100		// �ִ�𵨼�


// ��ũ ���̺귯��
#pragma comment( lib, "d3d11.lib" )   // D3D11 ���̺귯��
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "winmm.lib" )


// ������ ������ ��ũ��
#ifndef SAFE_RELEASE
#define SAFE_RELEASE( p )      { if ( p ) { ( p )->Release(); ( p )=NULL; } }
#endif

// ���̴� ��� ����ü
struct CBNeverChanges
{
    XMMATRIX mView;
	XMFLOAT4 v4AddColor;
};

// �ؽ�ó �̹��� ����ü
struct TEX_PICTURE {
	ID3D11ShaderResourceView	*pSRViewTexture;
	D3D11_TEXTURE2D_DESC		tdDesc;
	int							nWidth, nHeight;
};

// �� ����ü
struct MY_MODEL {
	int					nVertexPos;						// ������ġ
	int					nVertexNum;						// ������
	int					nIndexPos;						// �ε�����ġ
	int					nIndexNum;						// �ε�����
	TEX_PICTURE			*ptpTexture;					// �ؽ�ó
	XMMATRIX			mMatrix;						// ��ȯ���
	XMFLOAT4			v4AddColor;						// �����
};


// �۷ι� ����
UINT  g_nClientWidth;							// �׸� ���� �ʺ�
UINT  g_nClientHeight;							// �׸� ���� ����

HWND        g_hWnd;         // ������ �ڵ�


ID3D11Device			*g_pd3dDevice;			// ����̽�
IDXGISwapChain			*g_pSwapChain;			// DXGI����ü��
ID3D11DeviceContext		*g_pImmediateContext;	// ����̽� ���ؽ�Ʈ
ID3D11RasterizerState	*g_pRS;					// �����Ͷ�����
ID3D11RasterizerState	*g_pRS_Cull_CW;			// �����Ͷ�����(�ð���� �ø�)
ID3D11RasterizerState	*g_pRS_Cull_CCW;		// �����Ͷ�����(�ݽð���� �ø�)
ID3D11RenderTargetView	*g_pRTV;				// ������ Ÿ��
ID3D11Texture2D*        g_pDepthStencil = NULL;	// Z����
ID3D11DepthStencilView* g_pDepthStencilView = NULL;	// Z���� ��
ID3D11DepthStencilState *g_pDSDepthState = NULL;	// Z���� ������Ʈ
ID3D11DepthStencilState *g_pDSDepthState_NoWrite = NULL;	// Z���۪ν�����Ʈ(Z���۾������)
D3D_FEATURE_LEVEL       g_FeatureLevel;			// ��ó����

ID3D11Buffer			*g_pVertexBuffer;		// ��������
ID3D11Buffer			*g_pIndexBuffer;		// �ε�������
ID3D11BlendState		*g_pbsAddBlend;			// �������
ID3D11VertexShader		*g_pVertexShader;		// �������̴�
ID3D11PixelShader		*g_pPixelShader;		// �ȼ����̴�
ID3D11InputLayout		*g_pInputLayout;		// ���̴� �Է� ���̾ƿ�
ID3D11SamplerState		*g_pSamplerState;		// ���÷� ������Ʈ

ID3D11Buffer			*g_pCBNeverChanges = NULL;

// �׸� ��������
CUSTOMVERTEX g_cvVertices[MAX_BUFFER_VERTEX];
int							g_nVertexNum = 0;

WORD		g_wIndices[MAX_BUFFER_INDEX];
int							g_nIndexNum = 0;

TEX_PICTURE				g_tGroundTexture;
MY_MODEL					g_mmGround;


// Direct3D �ʱ�ȭ
HRESULT InitD3D( void )
{
    HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL  FeatureLevelsRequested[6] = { D3D_FEATURE_LEVEL_11_0,
													 D3D_FEATURE_LEVEL_10_1,
													 D3D_FEATURE_LEVEL_10_0,
													 D3D_FEATURE_LEVEL_9_3,
													 D3D_FEATURE_LEVEL_9_2,
													 D3D_FEATURE_LEVEL_9_1 };
	UINT               numLevelsRequested = 6;
	D3D_FEATURE_LEVEL  FeatureLevelsSupported;

	// ����̽�����
	hr = D3D11CreateDevice( NULL,
					D3D_DRIVER_TYPE_HARDWARE, 
					NULL, 
					0,
					FeatureLevelsRequested, 
					numLevelsRequested,
					D3D11_SDK_VERSION, 
					&g_pd3dDevice,
					&FeatureLevelsSupported,
					&g_pImmediateContext );
	if( FAILED ( hr ) ) {
		return hr;
	}

	// ���丮 ���
	IDXGIDevice * pDXGIDevice;
	hr = g_pd3dDevice->QueryInterface( __uuidof( IDXGIDevice ), ( void ** )&pDXGIDevice );
	IDXGIAdapter * pDXGIAdapter;
	hr = pDXGIDevice->GetParent( __uuidof( IDXGIAdapter ), ( void ** )&pDXGIAdapter );
	IDXGIFactory * pIDXGIFactory;
	pDXGIAdapter->GetParent( __uuidof( IDXGIFactory ), ( void ** )&pIDXGIFactory);

	// ����ü�� ����
    DXGI_SWAP_CHAIN_DESC	sd;
	ZeroMemory( &sd, sizeof( sd ) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = g_nClientWidth;
	sd.BufferDesc.Height = g_nClientHeight;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;
	hr = pIDXGIFactory->CreateSwapChain( g_pd3dDevice, &sd, &g_pSwapChain );

	pDXGIDevice->Release();
	pDXGIAdapter->Release();
	pIDXGIFactory->Release();

	if( FAILED ( hr ) ) {
		return hr;
	}

    // ������ Ÿ�� ����
    ID3D11Texture2D			*pBackBuffer = NULL;
    D3D11_TEXTURE2D_DESC BackBufferSurfaceDesc;
    hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), ( LPVOID* )&pBackBuffer );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't get backbuffer." ), _T( "Error" ), MB_OK );
        return hr;
    }
    pBackBuffer->GetDesc( &BackBufferSurfaceDesc );
    hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRTV );
    SAFE_RELEASE( pBackBuffer );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't create render target view." ), _T( "Error" ), MB_OK );
        return hr;
    }

    // *** Create depth stencil texture ***
    D3D11_TEXTURE2D_DESC descDepth;
	RECT rc;
    GetClientRect( g_hWnd, &rc );
	ZeroMemory( &descDepth, sizeof(descDepth) );
    descDepth.Width = rc.right - rc.left;
    descDepth.Height = rc.bottom - rc.top;
    descDepth.MipLevels = 1;
    descDepth.ArraySize = 1;
    descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    descDepth.SampleDesc.Count = 1;
    descDepth.SampleDesc.Quality = 0;
    descDepth.Usage = D3D11_USAGE_DEFAULT;
    descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    descDepth.CPUAccessFlags = 0;
    descDepth.MiscFlags = 0;
    hr = g_pd3dDevice->CreateTexture2D( &descDepth, NULL, &g_pDepthStencil );
    if( FAILED( hr ) )
        return hr;

    // *** Create the depth stencil view ***
    D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
	ZeroMemory( &descDSV, sizeof(descDSV) );
    descDSV.Format = descDepth.Format;
    descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    descDSV.Texture2D.MipSlice = 0;
    hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
    if( FAILED( hr ) )
        return hr;

	// *** ������ Ÿ�꼳�� ***
    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRTV, g_pDepthStencilView );

	// ���ٽ� ������Ʈ �ۼ�
	D3D11_DEPTH_STENCIL_DESC dsDesc;

	// Depth test parameters
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

	// Stencil test parameters
	dsDesc.StencilEnable = true;
	dsDesc.StencilReadMask = 0xFF;
	dsDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing
	dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing
	dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create depth stencil state
	hr = g_pd3dDevice->CreateDepthStencilState( &dsDesc, &g_pDSDepthState );

	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	hr = g_pd3dDevice->CreateDepthStencilState( &dsDesc, &g_pDSDepthState_NoWrite );

//	g_pImmediateContext->OMSetDepthStencilState( g_pDSDepthState, 1 );

    // �����Ͷ����� ����
    D3D11_RASTERIZER_DESC drd;
	ZeroMemory( &drd, sizeof( drd ) );
	drd.FillMode				= D3D11_FILL_SOLID;
	drd.CullMode				= D3D11_CULL_NONE;
	drd.FrontCounterClockwise	= FALSE;
	drd.DepthClipEnable			= TRUE;
    hr = g_pd3dDevice->CreateRasterizerState( &drd, &g_pRS );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't create rasterizer state." ), _T( "Error" ), MB_OK );
        return hr;
    }
	g_pImmediateContext->RSSetState( g_pRS );

    // �����Ͷ����� ����(�ð���� �ø�)
	ZeroMemory( &drd, sizeof( drd ) );
	drd.FillMode				= D3D11_FILL_SOLID;
	drd.CullMode				= D3D11_CULL_BACK;
	drd.FrontCounterClockwise	= TRUE;
	drd.DepthClipEnable			= TRUE;
    hr = g_pd3dDevice->CreateRasterizerState( &drd, &g_pRS_Cull_CW );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't create rasterizer state." ), _T( "Error" ), MB_OK );
        return hr;
    }
//    g_pImmediateContext->RSSetState( g_pRS_Cull_CW );

    // �����Ͷ����� ����(�ݽð���� �ø�)
	ZeroMemory( &drd, sizeof( drd ) );
	drd.FillMode				= D3D11_FILL_SOLID;
	drd.CullMode				= D3D11_CULL_BACK;
	drd.FrontCounterClockwise	= FALSE;
	drd.DepthClipEnable			= TRUE;
    hr = g_pd3dDevice->CreateRasterizerState( &drd, &g_pRS_Cull_CCW );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't create rasterizer state." ), _T( "Error" ), MB_OK );
        return hr;
    }
//    g_pImmediateContext->RSSetState( g_pRS_Cull_CCW );

    // ����Ʈ ����
    D3D11_VIEWPORT vp;
    vp.Width    = ( FLOAT )g_nClientWidth;
    vp.Height   = ( FLOAT )g_nClientHeight;
    vp.MinDepth = 0.0f;
    vp.MaxDepth = 1.0f;
    vp.TopLeftX = 0.0f;
    vp.TopLeftY = 0.0f;
    g_pImmediateContext->RSSetViewports( 1, &vp );

    return S_OK;
}


// ���α׷��Ӻ� ���̴� �ۼ�
HRESULT MakeShaders( void )
{
    HRESULT hr;
    ID3DBlob* pVertexShaderBuffer = NULL;
    ID3DBlob* pPixelShaderBuffer = NULL;
    ID3DBlob* pError = NULL;

    DWORD dwShaderFlags = 0;
#ifdef _DEBUG
    dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
    // ������
    hr = D3DX11CompileFromFile( _T( "Basic_3D_TexMark.fx" ), NULL, NULL, "VS", "vs_4_0_level_9_1",
								dwShaderFlags, 0, NULL, &pVertexShaderBuffer, &pError, NULL );
    if ( FAILED( hr ) ) {
		MessageBox( NULL, _T( "Can't open Basic_3D_TexMark.fx" ), _T( "Error" ), MB_OK );
        SAFE_RELEASE( pError );
        return hr;
    }
    hr = D3DX11CompileFromFile( _T( "Basic_3D_TexMark.fx" ), NULL, NULL, "PS", "ps_4_0_level_9_1",
								dwShaderFlags, 0, NULL, &pPixelShaderBuffer, &pError, NULL );
    if ( FAILED( hr ) ) {
        SAFE_RELEASE( pVertexShaderBuffer );
        SAFE_RELEASE( pError );
        return hr;
    }
    SAFE_RELEASE( pError );
    
    // VertexShader ����
    hr = g_pd3dDevice->CreateVertexShader( pVertexShaderBuffer->GetBufferPointer(),
										   pVertexShaderBuffer->GetBufferSize(),
										   NULL, &g_pVertexShader );
    if ( FAILED( hr ) ) {
        SAFE_RELEASE( pVertexShaderBuffer );
        SAFE_RELEASE( pPixelShaderBuffer );
        return hr;
    }
    // PixelShader ����
    hr = g_pd3dDevice->CreatePixelShader( pPixelShaderBuffer->GetBufferPointer(),
										  pPixelShaderBuffer->GetBufferSize(),
										  NULL, &g_pPixelShader );
    if ( FAILED( hr ) ) {
        SAFE_RELEASE( pVertexShaderBuffer );
        SAFE_RELEASE( pPixelShaderBuffer );
        return hr;
    }

    // �Է¹��� �Է�����
    D3D11_INPUT_ELEMENT_DESC layout[] = {
        { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,  0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        { "TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
    };
	UINT numElements = ARRAYSIZE( layout );
	// �Է¹��� �Է����� ����
    hr = g_pd3dDevice->CreateInputLayout( layout, numElements,
										  pVertexShaderBuffer->GetBufferPointer(),
										  pVertexShaderBuffer->GetBufferSize(),
										  &g_pInputLayout );
    SAFE_RELEASE( pVertexShaderBuffer );
    SAFE_RELEASE( pPixelShaderBuffer );
    if ( FAILED( hr ) ) {
        return hr;
    }

    // ���̴� ��� ���� ����
    D3D11_BUFFER_DESC bd;
    ZeroMemory( &bd, sizeof( bd ) );
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.ByteWidth = sizeof( CBNeverChanges );
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = 0;
    hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBNeverChanges );
    if( FAILED( hr ) )
        return hr;

    return S_OK;
}


// �ؽ�ó �ε�
int LoadTexture( TCHAR *szFileName, TEX_PICTURE *pTexPic, int nWidth, int nHeight,
				 int nTexWidth, int nTexHeight )
{
    HRESULT						hr;
	D3DX11_IMAGE_LOAD_INFO		liLoadInfo;
	ID3D11Texture2D				*pTexture;

	ZeroMemory( &liLoadInfo, sizeof( D3DX11_IMAGE_LOAD_INFO ) );
	liLoadInfo.Width = nTexWidth;
	liLoadInfo.Height = nTexHeight;
	liLoadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	liLoadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	hr = D3DX11CreateShaderResourceViewFromFile( g_pd3dDevice, szFileName, &liLoadInfo,
												 NULL, &( pTexPic->pSRViewTexture ), NULL );
    if ( FAILED( hr ) ) {
        return hr;
    }
	pTexPic->pSRViewTexture->GetResource( ( ID3D11Resource ** )&( pTexture ) );
	pTexture->GetDesc( &( pTexPic->tdDesc ) );
	pTexture->Release();

	pTexPic->nWidth = nWidth;
	pTexPic->nHeight = nHeight;

	return S_OK;
}


// ��ο� ��� ������Ʈ  �ʱ�ȭ
int InitDrawModes( void )
{
    HRESULT				hr;

	// ���� ������Ʈ
    D3D11_BLEND_DESC BlendDesc;
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
    BlendDesc.RenderTarget[0].BlendEnable           = TRUE;
    BlendDesc.RenderTarget[0].SrcBlend              = D3D11_BLEND_ONE;
    BlendDesc.RenderTarget[0].DestBlend             = D3D11_BLEND_ONE;
    BlendDesc.RenderTarget[0].BlendOp               = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].SrcBlendAlpha         = D3D11_BLEND_ONE;
    BlendDesc.RenderTarget[0].DestBlendAlpha        = D3D11_BLEND_ZERO;
    BlendDesc.RenderTarget[0].BlendOpAlpha          = D3D11_BLEND_OP_ADD;
    BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    hr = g_pd3dDevice->CreateBlendState( &BlendDesc, &g_pbsAddBlend );
    if ( FAILED( hr ) ) {
        return hr;
    }

    // ���÷� 
    D3D11_SAMPLER_DESC samDesc;
    ZeroMemory( &samDesc, sizeof( samDesc ) );
    samDesc.Filter          = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
    samDesc.AddressU        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressV        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.AddressW        = D3D11_TEXTURE_ADDRESS_WRAP;
    samDesc.ComparisonFunc  = D3D11_COMPARISON_ALWAYS;
    samDesc.MaxLOD          = D3D11_FLOAT32_MAX;
    hr = g_pd3dDevice->CreateSamplerState( &samDesc, &g_pSamplerState );
    if ( FAILED( hr ) ) {
        return hr;
    }

    return S_OK;
}


// ������Ʈ�� �ʱ�ȭ
HRESULT InitGeometry( void )
{
	int					i, j;
    HRESULT				hr = S_OK;

    // ���� ���� ����
    D3D11_BUFFER_DESC BufferDesc;
    BufferDesc.Usage                = D3D11_USAGE_DYNAMIC;
    BufferDesc.ByteWidth            = sizeof( CUSTOMVERTEX ) * MAX_BUFFER_VERTEX;
    BufferDesc.BindFlags            = D3D11_BIND_VERTEX_BUFFER;
    BufferDesc.CPUAccessFlags       = D3D11_CPU_ACCESS_WRITE;
    BufferDesc.MiscFlags            = 0;

    D3D11_SUBRESOURCE_DATA SubResourceData;
    SubResourceData.pSysMem             = g_cvVertices;
    SubResourceData.SysMemPitch         = 0;
    SubResourceData.SysMemSlicePitch    = 0;
    hr = g_pd3dDevice->CreateBuffer( &BufferDesc, &SubResourceData, &g_pVertexBuffer );
    if ( FAILED( hr ) ) {
        return hr;
    }

    // �ε��� ���� ����
    BufferDesc.Usage                = D3D11_USAGE_DYNAMIC;
    BufferDesc.ByteWidth            = sizeof( WORD ) * MAX_BUFFER_INDEX;
    BufferDesc.BindFlags            = D3D11_BIND_INDEX_BUFFER;
    BufferDesc.CPUAccessFlags       = D3D11_CPU_ACCESS_WRITE;
    BufferDesc.MiscFlags            = 0;

	SubResourceData.pSysMem         = g_wIndices;
    hr = g_pd3dDevice->CreateBuffer( &BufferDesc, &SubResourceData, &g_pIndexBuffer );
    if( FAILED( hr ) )
        return hr;

	// �ؽ�ó �ۼ�
	g_tGroundTexture.pSRViewTexture =  NULL;
	hr = LoadTexture( _T( "19_2.bmp" ), &g_tGroundTexture, 691, 691, 1024, 1024 );
    if ( FAILED( hr ) ) {
 		MessageBox( NULL, _T( "Can't open 19_2.bmp" ), _T( "Error" ), MB_OK );
       return hr;
    }

	// ����
	int				nIndex;
	int				nIndexZ1, nIndexZ2;
	float			x, z;
	float			u, v;
	nIndex = 0;
	z = -GROUND_SIZE / 2;
	for ( i = 0; i < GROUND_DIVIDE_NUM + 1; i++ ) {
		x = -GROUND_SIZE / 2;
		v = ( float )i / 10.0f;
		for ( j = 0; j < GROUND_DIVIDE_NUM + 1; j++ ) {
			u = ( float )j / 10.0f;
			g_cvVertices[g_nVertexNum + nIndex].v4Pos = XMFLOAT4( x, 0.0f, z, 1.0f );
			g_cvVertices[g_nVertexNum + nIndex].v2UV = XMFLOAT2( u, v );
			nIndex++;
			x += BLOCK_WIDTH;
		}
		z += BLOCK_WIDTH;
	}
	g_mmGround.nVertexPos = g_nVertexNum;
	g_mmGround.nVertexNum = nIndex;
	g_nVertexNum += nIndex;

	nIndex = 0;
	for ( i = 0; i < GROUND_DIVIDE_NUM; i++ ) {
		nIndexZ1 = i * ( GROUND_DIVIDE_NUM + 1 );
		nIndexZ2 = ( i + 1 ) * ( GROUND_DIVIDE_NUM + 1 );
		for ( j = 0; j < GROUND_DIVIDE_NUM; j++ ) {
			g_wIndices[g_nIndexNum + nIndex    ] = nIndexZ1 + j;
			g_wIndices[g_nIndexNum + nIndex + 1] = nIndexZ1 + j + 1;
			g_wIndices[g_nIndexNum + nIndex + 2] = nIndexZ2 + j;
			g_wIndices[g_nIndexNum + nIndex + 3] = nIndexZ1 + j + 1;
			g_wIndices[g_nIndexNum + nIndex + 4] = nIndexZ2 + j + 1;
			g_wIndices[g_nIndexNum + nIndex + 5] = nIndexZ2 + j;
			nIndex += 6;
		}
	}
	g_mmGround.nIndexPos = g_nIndexNum;
	g_mmGround.nIndexNum = nIndex;
	g_nIndexNum += nIndex;
	g_mmGround.ptpTexture = &g_tGroundTexture;
	g_mmGround.mMatrix = XMMatrixIdentity();
	g_mmGround.v4AddColor = XMFLOAT4( 0.0f, 0.0f, 0.0f, 0.0f );

	// ��������?�ε��� ���� �ۼ�
	D3D11_MAPPED_SUBRESOURCE mappedVertices, mappedIndices;
	hr = g_pImmediateContext->Map( g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertices );
    if( FAILED( hr ) )
        return hr;
	hr = g_pImmediateContext->Map( g_pIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedIndices );
    if( FAILED( hr ) ) {
		g_pImmediateContext->Unmap( g_pVertexBuffer, 0 );
        return hr;
	}
	CopyMemory( mappedVertices.pData,  g_cvVertices, sizeof( CUSTOMVERTEX ) * g_nVertexNum );
	CopyMemory( mappedIndices.pData,  g_wIndices, sizeof( WORD ) * g_nIndexNum );
	g_pImmediateContext->Unmap( g_pVertexBuffer, 0 );
	g_pImmediateContext->Unmap( g_pIndexBuffer, 0 );

	return S_OK;
}


// ����ó��
int Cleanup( void )
{
    SAFE_RELEASE( g_tGroundTexture.pSRViewTexture );
    SAFE_RELEASE( g_pVertexBuffer );
    SAFE_RELEASE( g_pIndexBuffer );

    SAFE_RELEASE( g_pSamplerState );
    SAFE_RELEASE( g_pbsAddBlend );
    SAFE_RELEASE( g_pInputLayout );
    SAFE_RELEASE( g_pPixelShader );
    SAFE_RELEASE( g_pVertexShader );
    SAFE_RELEASE( g_pCBNeverChanges );

    SAFE_RELEASE( g_pRS );									// �����Ͷ�����
    SAFE_RELEASE( g_pRS_Cull_CW );
    SAFE_RELEASE( g_pRS_Cull_CCW );

	// �������ͽ� Ŭ����
	if ( g_pImmediateContext ) {
		g_pImmediateContext->ClearState();
		g_pImmediateContext->Flush();
	}

    SAFE_RELEASE( g_pRTV );									// ������ Ÿ��
    SAFE_RELEASE( g_pDepthStencil );						// Z����
    SAFE_RELEASE( g_pDepthStencilView );					// Z���� ��
    SAFE_RELEASE( g_pDSDepthState );						// Z���� ������Ʈ
    SAFE_RELEASE( g_pDSDepthState_NoWrite );

    // ����ü��
    if ( g_pSwapChain != NULL ) {
        g_pSwapChain->SetFullscreenState( FALSE, 0 );
    }
    SAFE_RELEASE( g_pSwapChain );

    SAFE_RELEASE( g_pImmediateContext );					// ����̽����ؽ�Ʈ
    SAFE_RELEASE( g_pd3dDevice );							// ����̽�

	return 0;
}


// �� �׸���
int DrawMyModel( MY_MODEL *pmmDrawModel, XMMATRIX *pmViewProjection )
{
    CBNeverChanges	cbNeverChanges;

	cbNeverChanges.mView = XMMatrixTranspose( pmmDrawModel->mMatrix * *pmViewProjection );
	cbNeverChanges.v4AddColor = pmmDrawModel->v4AddColor;
	g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0 );
	g_pImmediateContext->PSSetShaderResources( 0, 1, &( pmmDrawModel->ptpTexture->pSRViewTexture ) );
	g_pImmediateContext->DrawIndexed( pmmDrawModel->nIndexNum, pmmDrawModel->nIndexPos, pmmDrawModel->nVertexPos );

	return 0;
}


// �� �κ� �׸���
int DrawMyModelPartial( MY_MODEL *pmmDrawModel, XMMATRIX *pmViewProjection,
						int nTrianglePos, int nTriangleNum )
{
    CBNeverChanges	cbNeverChanges;
	int				nIndexPos, nIndexNum;

	cbNeverChanges.mView = XMMatrixTranspose( pmmDrawModel->mMatrix * *pmViewProjection );
	cbNeverChanges.v4AddColor = pmmDrawModel->v4AddColor;
	g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0 );
	g_pImmediateContext->PSSetShaderResources( 0, 1, &( pmmDrawModel->ptpTexture->pSRViewTexture ) );
	nIndexPos = nTrianglePos * 3;
	if ( nIndexPos > ( pmmDrawModel->nIndexNum - 3 ) ) nIndexPos = pmmDrawModel->nIndexNum - 3;
	nIndexNum = nTriangleNum * 3;
	if ( ( nIndexPos + nIndexNum ) > pmmDrawModel->nIndexNum ) nIndexNum = pmmDrawModel->nIndexNum - nIndexPos;
	g_pImmediateContext->DrawIndexed( nIndexNum, pmmDrawModel->nIndexPos + nIndexPos, pmmDrawModel->nVertexPos );

	return 0;
}


// ������ ���ν���
LRESULT WINAPI MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    switch( msg )
    {
        case WM_DESTROY:
            PostQuitMessage( 0 );
            return 0;
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}


// ������ 
HRESULT Render( void )
{
    // ȭ�� Ŭ����
	XMFLOAT4	v4Color = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
    g_pImmediateContext->ClearRenderTargetView( g_pRTV, ( float * )&v4Color );
	// *** Z���� Ŭ���� ***
    g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

    // ���÷� ����
    g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerState );
    
    // �׸��� ����
    UINT nStrides = sizeof( CUSTOMVERTEX );
    UINT nOffsets = 0;
    g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &nStrides, &nOffsets );
    g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
    g_pImmediateContext->IASetInputLayout( g_pInputLayout );

    // ���̴� ����
    g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
    g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
    g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );
    g_pImmediateContext->PSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
		
	// ��ȯ���
    CBNeverChanges	cbNeverChanges;
	XMMATRIX		mWorld;
	XMMATRIX		mView;
	XMMATRIX		mProjection;
	XMMATRIX		mViewProjection;

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet( Player_1.v3Pos.x, Player_1.v3Pos.y + 10.0f, Player_1.v3Pos.z - 15.0f, 0.0f );
	XMVECTOR At = XMVectorSet( Player_1.v3Pos.x, Player_1.v3Pos.y, Player_1.v3Pos.z, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	mView = XMMatrixLookAtLH( Eye, At, Up );

    // Initialize the projection matrix
	mProjection = XMMatrixPerspectiveFovLH( XM_PIDIV4, VIEW_WIDTH / ( FLOAT )VIEW_HEIGHT, 0.01f, 100.0f );

	mViewProjection = mView * mProjection;

    // �׸���
	g_pImmediateContext->OMSetDepthStencilState( g_pDSDepthState, 1 );
    g_pImmediateContext->RSSetState( g_pRS_Cull_CW );				// �ø�����

	// ����
    HRESULT				hr = S_OK;
	int					i, j;
	int					nIndex = 0;
	for ( i = 0; i < GROUND_DIVIDE_NUM + 1; i++ ) {
		for ( j = 0; j < GROUND_DIVIDE_NUM + 1; j++ ) {
			g_cvVertices[g_mmGround.nVertexPos + nIndex].v4Pos.y = g_fHeights[i][j];
			nIndex++;
		}
	}
	D3D11_MAPPED_SUBRESOURCE mappedVertices;
	hr = g_pImmediateContext->Map( g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedVertices );
    if( FAILED( hr ) )
        return hr;
	CopyMemory( mappedVertices.pData,  g_cvVertices, sizeof( CUSTOMVERTEX ) * g_nVertexNum );
	g_pImmediateContext->Unmap( g_pVertexBuffer, 0 );

    g_pImmediateContext->OMSetBlendState( NULL, NULL, 0xFFFFFFFF );
	g_mmGround.v4AddColor = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
	DrawMyModel( &g_mmGround, &mViewProjection );

    return S_OK;
}


// ��Ʈ������Ʈ
int WINAPI _tWinMain( HINSTANCE hInst, HINSTANCE, LPTSTR, int )
{
	LARGE_INTEGER			nNowTime, nLastTime;		// ����� �����ð�
	LARGE_INTEGER			nTimeFreq;					// �ð�����

    // ȭ��ũ��
    g_nClientWidth  = VIEW_WIDTH;						// �ʺ�
    g_nClientHeight = VIEW_HEIGHT;						// ����

	// Register the window class
    WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L,
                      GetModuleHandle( NULL ), NULL, NULL, NULL, NULL,
                      _T( "D3D Sample" ), NULL };
    RegisterClassEx( &wc );

	RECT rcRect;
	SetRect( &rcRect, 0, 0, g_nClientWidth, g_nClientHeight );
	AdjustWindowRect( &rcRect, WS_OVERLAPPEDWINDOW, FALSE );
    g_hWnd = CreateWindow( _T( "D3D Sample" ), _T( "3DPhysics_5_4" ),
						   WS_OVERLAPPEDWINDOW, 100, 20, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top,
						   GetDesktopWindow(), NULL, wc.hInstance, NULL );

    // Initialize Direct3D
    if( SUCCEEDED( InitD3D() ) && SUCCEEDED( MakeShaders() ) )
    {
        // Create the shaders
        if( SUCCEEDED( InitDrawModes() ) )
        {
			if ( SUCCEEDED( InitGeometry() ) ) {					// ������Ʈ�� �ۼ�
				InitPlayer();										// �÷��̾�� �ʱ�ȭ
				// Show the window
				ShowWindow( g_hWnd, SW_SHOWDEFAULT );
				UpdateWindow( g_hWnd );
				
				QueryPerformanceFrequency( &nTimeFreq );			// �ð�����
				QueryPerformanceCounter( &nLastTime );				// 1�������� �ð� �ʱ�ȭ

				// Enter the message loop
				MSG msg;
				ZeroMemory( &msg, sizeof( msg ) );
				while( msg.message != WM_QUIT )
				{
					MakeHeights();
					MovePlayer();
					Render();
					do {
						if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
						{
							TranslateMessage( &msg );
							DispatchMessage( &msg );
						}
						QueryPerformanceCounter( &nNowTime );
					} while( ( ( nNowTime.QuadPart - nLastTime.QuadPart ) < ( nTimeFreq.QuadPart / 90 ) ) &&
							 ( msg.message != WM_QUIT ) );
					while( ( ( nNowTime.QuadPart - nLastTime.QuadPart ) < ( nTimeFreq.QuadPart / 60 ) ) &&
						   ( msg.message != WM_QUIT ) )
					{
						QueryPerformanceCounter( &nNowTime );
					}
					nLastTime = nNowTime;
					g_pSwapChain->Present( 0, 0 );					// ǥ��
				}
			}
        }
    }

    // Clean up everything and exit the app
    Cleanup();
    UnregisterClass( _T( "D3D Sample" ), wc.hInstance );
    return 0;
}

