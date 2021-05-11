//------------------------------------------------------------
// Textures_5_1.cpp
// ¹ÝÅõ¸í ±¸ ±×¸®±â(Z¹öÆÛ ¹®Á¦°¡ ÀÖ´Â ¹öÀü)
//
//------------------------------------------------------------

#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>

#define VIEW_WIDTH 800	// È­¸é³Êºñ
#define VIEW_HEIGHT 600 // È­¸é³ôÀÌ

#define PI 3.1415927f							// ¿øÁÖÀ²
#define ROT_SPEED ( PI / 100.0f ) // È¸Àü¼Óµµ
#define R 2.0f										// µµÇüÅ©±â
#define CORNER_NUM 20							// °¢¼ö

// Á¤Á¡±¸Á¶Ã¼
struct CUSTOMVERTEX
{
	XMFLOAT4 v4Pos;
	XMFLOAT2 v2UV;
};

// ÅØ½ºÃ³ ÀÖ´Â ÀÎµ¦½ºµÈ 3D Æú¸®°ï ±×¸®±â
int DrawIndexed3DPolygonsTex( CUSTOMVERTEX *pVertices, int nVertexNum, WORD *pIndices, int nIndexNum );

// ¿ùµåÇà·Ä »ý¼º
XMMATRIX CreateWorldMatrix( void )
{
	float fAngleY;							 // y®‚ È¸Àü°¢
	static float fAngleX = 0.0f; // xÃà È¸Àü°¢
	XMMATRIX matRot_Y;					 // yÃà È¸ÀüÇà·Ä
	XMMATRIX matRot_X;					 // xÃà È¸ÀüÇà·Ä

	// °­Á¦È¸Àü
	fAngleY = 2.0f * PI * (float)( timeGetTime() % 3000 ) / 3000.0f;

	// Å°ÀÔ·ÂÀ¸·Î È¸Àü
	if( GetAsyncKeyState( VK_UP ) )
	{
		fAngleX += ROT_SPEED;
	}
	if( GetAsyncKeyState( VK_DOWN ) )
	{
		fAngleX -= ROT_SPEED;
	}

	// Çà·ÄÀÛ¼º
	matRot_Y = XMMatrixRotationY( fAngleY );
	matRot_X = XMMatrixRotationX( fAngleX );

	return matRot_Y * matRot_X; // È¸ÀüÀÇ ÇÕ¼º
}

// ÀÌ¹ÌÁö ±×±â¸®
int DrawChangingPictures( void )
{
	int i, j;
	CUSTOMVERTEX Vertices[( CORNER_NUM + 1 ) * ( CORNER_NUM / 4 + 1 )]; // Á¤Á¡ µ¥ÀÌÅÍ
	WORD wIndices[CORNER_NUM * CORNER_NUM / 4 * 2 * 3];									// ÀÎµ¦½º µ¥ÀÌÅÍ
	float fTheta;
	float fPhi;
	float fAngleDelta;
	int nIndex;	 // µ¥ÀÌÅÍÀÇ ÀÎµ¦½º
	int nIndexY; // y¹æÇâ ÀÎµ¦½º

	// Á¤Á¡ µ¥ÀÌÅÍ ÀÛ¼º
	fAngleDelta = 2.0f * PI / CORNER_NUM;
	nIndex = 0;
	fTheta = 0.0f;
	for( i = 0; i < CORNER_NUM / 4 + 1; i++ )
	{
		fPhi = 0.0f;
		for( j = 0; j < CORNER_NUM + 1; j++ )
		{
			Vertices[nIndex].v4Pos = XMFLOAT4( R * sinf( fTheta ) * cosf( fPhi ), R * cosf( fTheta ), R * sinf( fTheta ) * sinf( fPhi ), 1.0f );
			Vertices[nIndex].v2UV = XMFLOAT2( fPhi / ( 2.0f * PI ), fTheta / PI );
			nIndex++;
			fPhi += fAngleDelta;
		}
		fTheta += fAngleDelta;
	}

	// ÀÎµ¦½º µ¥ÀÌÅÍ ÀÛ¼º
	nIndex = 0;
	for( i = 0; i < CORNER_NUM; i++ )
	{
		for( j = 0; j < CORNER_NUM / 4; j++ )
		{
			nIndexY = j * ( CORNER_NUM + 1 );
			wIndices[nIndex] = nIndexY + i;
			wIndices[nIndex + 1] = nIndexY + ( CORNER_NUM + 1 ) + i;
			wIndices[nIndex + 2] = nIndexY + i + 1;
			nIndex += 3;
			wIndices[nIndex] = nIndexY + i + 1;
			wIndices[nIndex + 1] = nIndexY + ( CORNER_NUM + 1 ) + i;
			wIndices[nIndex + 2] = nIndexY + ( CORNER_NUM + 1 ) + i + 1;
			nIndex += 3;
		}
	}

	// Á¤Á¡ ÀÎµ¦½º·Î ±×¸®±â
	DrawIndexed3DPolygonsTex( Vertices, ( CORNER_NUM + 1 ) * ( CORNER_NUM / 4 + 1 ), wIndices, CORNER_NUM * CORNER_NUM / 4 * 2 * 3 );

	return 0;
}

//------------------------------------------------------------
// ¾Æ·¡´Â DirectX·Î Ç¥½ÃÇÏ´Â ÇÁ·Î±×·¥

#include <stdio.h>
#include <tchar.h> // Unicode, ¸ÖÆ¼¹ÙÀÌÆ® ¹®ÀÚ°ü°è
#include <windows.h>

#define MAX_BUFFER_VERTEX 10000 // ÃÖ´ë¹öÆÛ Á¤Á¡¼ö
#define MAX_BUFFER_INDEX 20000 // ÃÖ´ë¹öÆÛ ÀÎµ¦½º¼ö

// ¸µÅ©¶óÀÌºê·¯¸®
#pragma comment( lib, "d3d11.lib" ) // D3D11¶óÀÌºê·¯¸®
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "winmm.lib" )

// ¼¼ÀÌÇÁ ¸±¸®½º ¸ÅÅ©·Î
#ifndef SAFE_RELEASE
#define SAFE_RELEASE( p ) \
	{ \
		if( p ) \
		{ \
			( p )->Release(); \
			( p ) = NULL; \
		} \
	}
#endif

// ¼ÎÀÌ´õ »ó¼ö ±¸Á¶Ã¼
struct CBNeverChanges
{
	XMMATRIX mView;
};

// ÅØ½ºÃ³ ÀÌ¹ÌÁö ±¸Á¶Ã¼
struct TEX_PICTURE
{
	ID3D11ShaderResourceView *pSRViewTexture;
	D3D11_TEXTURE2D_DESC tdDesc;
	int nWidth, nHeight;
};

// ±Û·Î¹ú º¯¼ö
UINT g_nClientWidth;	// ±×¸± ¿µ¿ªÀÇ ³Êºñ
UINT g_nClientHeight; // ±×¸± ¿µ¿ªÀÇ ³ôÀÌ

HWND g_hWnd; // À©µµ¿ì ÇÚµé

ID3D11Device *g_pd3dDevice;													// µð¹ÙÀÌ½º
IDXGISwapChain *g_pSwapChain;												// DXGI½º¿ÒÃ¼ÀÎ
ID3D11DeviceContext *g_pImmediateContext;						// µð¹ÙÀÌ½º ÄÁÅØ½ºÆ®
ID3D11RasterizerState *g_pRS;												// ·¡½ºÅÍ¶óÀÌÀú
ID3D11RenderTargetView *g_pRTV;											// ·»´õ¸µ Å¸±ê
ID3D11Texture2D *g_pDepthStencil = NULL;						// Z¹öÆÛ
ID3D11DepthStencilView *g_pDepthStencilView = NULL; // Z¹öÆÛÀÇ ºä
ID3D11DepthStencilState *g_pDSStencilState = NULL;	// Z¹öÆÛÀÇ »óÅÂ
D3D_FEATURE_LEVEL g_FeatureLevel;										// ÇÇÃ³·¹º§

ID3D11Buffer *g_pVertexBuffer;			 // Á¤Á¡¹öÆÛ
ID3D11Buffer *g_pIndexBuffer;				 // ÀÎµ¦½º¹öÆÛ
ID3D11BlendState *g_pbsAddBlend;		 // °¡»êºí·»µå
ID3D11VertexShader *g_pVertexShader; // Á¤Á¡¼ÎÀÌ´õ
ID3D11PixelShader *g_pPixelShader;	 // ÇÈ¼¿¼ÎÀÌ´õ
ID3D11InputLayout *g_pInputLayout;	 // ¼ÎÀÌ´õÀÔ·Â ·¹ÀÌ¾Æ¿ô
ID3D11SamplerState *g_pSamplerState; // »ùÇÃ·¯ ½ºÅ×ÀÌÆ®

ID3D11Buffer *g_pCBNeverChanges = NULL;

// Á¤Á¡ ¹öÆÛ
CUSTOMVERTEX g_cvVertices[MAX_BUFFER_VERTEX];
int g_nVertexNum = 0;

WORD g_wIndices[MAX_BUFFER_INDEX];
int g_nIndexNum = 0;

TEX_PICTURE g_tTexture;

// ÇöÀç ÅØ½ºÃ³
ID3D11ShaderResourceView *g_pNowTexture = NULL;

// Direct3D ÃÊ±âÈ­
HRESULT InitD3D( void )
{
	HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL FeatureLevelsRequested[6] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
																									D3D_FEATURE_LEVEL_9_3,	D3D_FEATURE_LEVEL_9_2,	D3D_FEATURE_LEVEL_9_1 };
	UINT numLevelsRequested = 6;
	D3D_FEATURE_LEVEL FeatureLevelsSupported;

	// µð¹ÙÀÌ½º »ý¼º
	hr = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevelsRequested, numLevelsRequested, D3D11_SDK_VERSION, &g_pd3dDevice, &FeatureLevelsSupported,
													&g_pImmediateContext );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// ÆÑÅä¸® Ãëµæ
	IDXGIDevice *pDXGIDevice;
	hr = g_pd3dDevice->QueryInterface( __uuidof( IDXGIDevice ), (void **)&pDXGIDevice );
	IDXGIAdapter *pDXGIAdapter;
	hr = pDXGIDevice->GetParent( __uuidof( IDXGIAdapter ), (void **)&pDXGIAdapter );
	IDXGIFactory *pIDXGIFactory;
	pDXGIAdapter->GetParent( __uuidof( IDXGIFactory ), (void **)&pIDXGIFactory );

	// ½º¿ÒÃ¼ÀÎ »ý¼º
	DXGI_SWAP_CHAIN_DESC sd;
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

	if( FAILED( hr ) )
	{
		return hr;
	}

	// ·»´õ¸µ Å¸±ê »ý¼º
	ID3D11Texture2D *pBackBuffer = NULL;
	D3D11_TEXTURE2D_DESC BackBufferSurfaceDesc;
	hr = g_pSwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID *)&pBackBuffer );
	if( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "Can't get backbuffer." ), _T( "Error" ), MB_OK );
		return hr;
	}
	pBackBuffer->GetDesc( &BackBufferSurfaceDesc );
	hr = g_pd3dDevice->CreateRenderTargetView( pBackBuffer, NULL, &g_pRTV );
	SAFE_RELEASE( pBackBuffer );
	if( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "Can't create render target view." ), _T( "Error" ), MB_OK );
		return hr;
	}

	// *** Create depth stencil texture ***
	D3D11_TEXTURE2D_DESC descDepth;
	RECT rc;
	GetClientRect( g_hWnd, &rc );
	ZeroMemory( &descDepth, sizeof( descDepth ) );
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
	ZeroMemory( &descDSV, sizeof( descDSV ) );
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0;
	hr = g_pd3dDevice->CreateDepthStencilView( g_pDepthStencil, &descDSV, &g_pDepthStencilView );
	if( FAILED( hr ) )
		return hr;

	// *** ·»´õ¸µ Å¸±ê ¼³Á¤ ***
	g_pImmediateContext->OMSetRenderTargets( 1, &g_pRTV, g_pDepthStencilView );

	// ·¡½ºÅÍ¶óÀÌÀú ¼³Á¤
	D3D11_RASTERIZER_DESC drd;
	ZeroMemory( &drd, sizeof( drd ) );
	drd.FillMode = D3D11_FILL_SOLID;
	drd.CullMode = D3D11_CULL_NONE;
	drd.FrontCounterClockwise = FALSE;
	drd.DepthClipEnable = TRUE;
	hr = g_pd3dDevice->CreateRasterizerState( &drd, &g_pRS );
	if( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "Can't create rasterizer state." ), _T( "Error" ), MB_OK );
		return hr;
	}
	g_pImmediateContext->RSSetState( g_pRS );

	// ºäÆ÷Æ® ¼³Á¤
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)g_nClientWidth;
	vp.Height = (FLOAT)g_nClientHeight;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	g_pImmediateContext->RSSetViewports( 1, &vp );

	return S_OK;
}

// ÇÁ·Î±×·¡¸Óºí ¼ÎÀÌ´õ ÀÛ¼º
HRESULT MakeShaders( void )
{
	HRESULT hr;
	ID3DBlob *pVertexShaderBuffer = NULL;
	ID3DBlob *pPixelShaderBuffer = NULL;
	ID3DBlob *pError = NULL;

	DWORD dwShaderFlags = 0;
#ifdef _DEBUG
	dwShaderFlags |= D3DCOMPILE_DEBUG;
#endif
	// ÄÄÆÄÀÏ
	hr = D3DX11CompileFromFile( _T( "Basic_3D_Tex.fx" ), NULL, NULL, "VS", "vs_4_0_level_9_1", dwShaderFlags, 0, NULL, &pVertexShaderBuffer, &pError, NULL );
	if( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "Can't open Basic_3D_Tex.fx" ), _T( "Error" ), MB_OK );
		SAFE_RELEASE( pError );
		return hr;
	}
	hr = D3DX11CompileFromFile( _T( "Basic_3D_Tex.fx" ), NULL, NULL, "PS", "ps_4_0_level_9_1", dwShaderFlags, 0, NULL, &pPixelShaderBuffer, &pError, NULL );
	if( FAILED( hr ) )
	{
		SAFE_RELEASE( pVertexShaderBuffer );
		SAFE_RELEASE( pError );
		return hr;
	}
	SAFE_RELEASE( pError );

	// VertexShader »ý¼º
	hr = g_pd3dDevice->CreateVertexShader( pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), NULL, &g_pVertexShader );
	if( FAILED( hr ) )
	{
		SAFE_RELEASE( pVertexShaderBuffer );
		SAFE_RELEASE( pPixelShaderBuffer );
		return hr;
	}
	// PixelShader »ý¼º
	hr = g_pd3dDevice->CreatePixelShader( pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), NULL, &g_pPixelShader );
	if( FAILED( hr ) )
	{
		SAFE_RELEASE( pVertexShaderBuffer );
		SAFE_RELEASE( pPixelShaderBuffer );
		return hr;
	}

	// ÀÔ·Â¹öÆÛÀÇ ÀÔ·ÂÇü½Ä
	D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXTURE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );
	// ÀÔ·Â¹öÆÛÀÇ ÀÔ·ÂÇü½Ä »ý¼º
	hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), &g_pInputLayout );
	SAFE_RELEASE( pVertexShaderBuffer );
	SAFE_RELEASE( pPixelShaderBuffer );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// ¼ÎÀÌ´õ »ó¼ö ¹öÆÛ »ý¼º
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof( bd ) );
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( CBNeverChanges );
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBNeverChanges );
	if( FAILED( hr ) )
		return hr;

	// º¯È¯Çà·Ä
	CBNeverChanges cbNeverChanges;
	XMMATRIX mScreen;
	mScreen = XMMatrixIdentity();
	mScreen._11 = 2.0f / g_nClientWidth;
	mScreen._22 = -2.0f / g_nClientHeight;
	mScreen._41 = -1.0f;
	mScreen._42 = 1.0f;
	cbNeverChanges.mView = XMMatrixTranspose( mScreen );
	g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0 );

	return S_OK;
}

// ÅØ½ºÃ³ ·Îµå
int LoadTexture( TCHAR *szFileName, TEX_PICTURE *pTexPic, int nWidth, int nHeight, int nTexWidth, int nTexHeight )
{
	HRESULT hr;
	D3DX11_IMAGE_LOAD_INFO liLoadInfo;
	ID3D11Texture2D *pTexture;

	ZeroMemory( &liLoadInfo, sizeof( D3DX11_IMAGE_LOAD_INFO ) );
	liLoadInfo.Width = nTexWidth;
	liLoadInfo.Height = nTexHeight;
	liLoadInfo.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	liLoadInfo.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	hr = D3DX11CreateShaderResourceViewFromFile( g_pd3dDevice, szFileName, &liLoadInfo, NULL, &( pTexPic->pSRViewTexture ), NULL );
	if( FAILED( hr ) )
	{
		return hr;
	}
	pTexPic->pSRViewTexture->GetResource( (ID3D11Resource **)&( pTexture ) );
	pTexture->GetDesc( &( pTexPic->tdDesc ) );
	pTexture->Release();

	pTexPic->nWidth = nWidth;
	pTexPic->nHeight = nHeight;

	return S_OK;
}

// µå·Î¿ì ¸ðµå ¿ÉÁ§Æ® ÃÊ±âÈ­
int InitDrawModes( void )
{
	HRESULT hr;

	// ºí·»µå ½ºÅ×ÀÌÆ®
	D3D11_BLEND_DESC BlendDesc;
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice->CreateBlendState( &BlendDesc, &g_pbsAddBlend );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// »ùÇÃ·¯
	D3D11_SAMPLER_DESC samDesc;
	ZeroMemory( &samDesc, sizeof( samDesc ) );
	samDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samDesc.MaxLOD = D3D11_FLOAT32_MAX;
	hr = g_pd3dDevice->CreateSamplerState( &samDesc, &g_pSamplerState );
	if( FAILED( hr ) )
	{
		return hr;
	}

	return S_OK;
}

// Áö¿À¸ÞÆ®¸® ÃÊ±âÈ­
HRESULT InitGeometry( void )
{
	HRESULT hr = S_OK;

	// Á¤Á¡¹öÆÛ »ý¼º
	D3D11_BUFFER_DESC BufferDesc;
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.ByteWidth = sizeof( CUSTOMVERTEX ) * MAX_BUFFER_VERTEX;
	BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA SubResourceData;
	SubResourceData.pSysMem = g_cvVertices;
	SubResourceData.SysMemPitch = 0;
	SubResourceData.SysMemSlicePitch = 0;
	hr = g_pd3dDevice->CreateBuffer( &BufferDesc, &SubResourceData, &g_pVertexBuffer );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// ÀÎµ¦½º ¹öÆÛ »ý¼º
	BufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	BufferDesc.ByteWidth = sizeof( WORD ) * MAX_BUFFER_INDEX;
	BufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	BufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	BufferDesc.MiscFlags = 0;

	SubResourceData.pSysMem = g_wIndices;
	hr = g_pd3dDevice->CreateBuffer( &BufferDesc, &SubResourceData, &g_pIndexBuffer );
	if( FAILED( hr ) )
		return hr;

	// ÅØ½ºÃ³ ÀÛ¼º
	g_tTexture.pSRViewTexture = NULL;
	hr = LoadTexture( _T( "6.bmp" ), &g_tTexture, 512, 512, 512, 512 );
	if( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "Can't open 6.bmp" ), _T( "Error" ), MB_OK );
		return hr;
	}

	return S_OK;
}

// Á¾·áÃ³¸®
int Cleanup( void )
{
	SAFE_RELEASE( g_tTexture.pSRViewTexture );
	SAFE_RELEASE( g_pVertexBuffer );
	SAFE_RELEASE( g_pIndexBuffer );

	SAFE_RELEASE( g_pSamplerState );
	SAFE_RELEASE( g_pbsAddBlend );
	SAFE_RELEASE( g_pInputLayout );
	SAFE_RELEASE( g_pPixelShader );
	SAFE_RELEASE( g_pVertexShader );
	SAFE_RELEASE( g_pCBNeverChanges );

	SAFE_RELEASE( g_pRS ); // ·¡½ºÅÍ¶óÀÌÀú

	// ½ºÅ×ÀÌÅÍ½º Å¬¸®¾î
	if( g_pImmediateContext )
	{
		g_pImmediateContext->ClearState();
		g_pImmediateContext->Flush();
	}

	SAFE_RELEASE( g_pRTV );							 // ·»´õ¸µ Å¸±ê
	SAFE_RELEASE( g_pDepthStencil );		 // Z¹öÆÛ
	SAFE_RELEASE( g_pDepthStencilView ); // Z¹öÆÛÀÇ ºä
	SAFE_RELEASE( g_pDSStencilState );	 // Z¹öÆÛÀÇ ½ºÅ×ÀÌÆ®

	// ½º¿ÒÃ¼ÀÎ
	if( g_pSwapChain != NULL )
	{
		g_pSwapChain->SetFullscreenState( FALSE, 0 );
	}
	SAFE_RELEASE( g_pSwapChain );

	SAFE_RELEASE( g_pImmediateContext ); // µð¹ÙÀÌ½º ÄÁÅØ½ºÆ®
	SAFE_RELEASE( g_pd3dDevice );				 // µð¹ÙÀÌ½º

	return 0;
}

// À©µµ¿ì ÇÁ·Î½ÃÀú
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

// ±×¸®±â ´ë±âÇà·Ä ÇÃ·¯½Ã
int FlushDrawingPictures( void )
{
	HRESULT hr;

	if( ( g_nVertexNum > 0 ) && g_pNowTexture )
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hr = g_pImmediateContext->Map( g_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		if( SUCCEEDED( hr ) )
		{
			CopyMemory( mappedResource.pData, &( g_cvVertices[0] ), sizeof( CUSTOMVERTEX ) * g_nVertexNum );
			g_pImmediateContext->Unmap( g_pVertexBuffer, 0 );
			hr = g_pImmediateContext->Map( g_pIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
			if( SUCCEEDED( hr ) )
			{
				CopyMemory( mappedResource.pData, &( g_wIndices[0] ), sizeof( WORD ) * g_nIndexNum );
				g_pImmediateContext->Unmap( g_pIndexBuffer, 0 );
				g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pNowTexture );
				g_pImmediateContext->DrawIndexed( g_nIndexNum, 0, 0 );
			}
		}
	}
	g_nVertexNum = 0;
	g_nIndexNum = 0;

	return 0;
}

// ÅØ½ºÃ³ ÀÖ´Â ÀÎµ¦½ºµÈ 3D Æú¸®°ï ±×¸®±â
int DrawIndexed3DPolygonsTex( CUSTOMVERTEX *pVertices, int nVertexNum, WORD *pIndices, int nIndexNum )
{
	int i;

	CopyMemory( &g_cvVertices[0], pVertices, sizeof( CUSTOMVERTEX ) * nVertexNum );
	for( i = 0; i < nIndexNum; i++ )
	{
		g_wIndices[g_nIndexNum + i] = *( pIndices + i ) + g_nVertexNum;
	}

	g_nVertexNum += nVertexNum;
	g_nIndexNum += nIndexNum;

	g_pNowTexture = g_tTexture.pSRViewTexture;

	return 0;
}

// ·»´õ¸µ
HRESULT Render( void )
{
	// È­¸é Å¬¸®¾î
	XMFLOAT4 v4Color = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
	g_pImmediateContext->ClearRenderTargetView( g_pRTV, (float *)&v4Color );
	// *** Z¹öÆÛ Å¬¸®¾î ***
	g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

	// »ùÇÃ·¯ ·¡½ºÅÍ¶óÀÌÀú ¼³Á¤
	g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerState );
	g_pImmediateContext->RSSetState( g_pRS );

	// ±×¸®±â ¼³Á¤
	UINT nStrides = sizeof( CUSTOMVERTEX );
	UINT nOffsets = 0;
	g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pVertexBuffer, &nStrides, &nOffsets );
	g_pImmediateContext->IASetIndexBuffer( g_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0 );
	g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	g_pImmediateContext->IASetInputLayout( g_pInputLayout );

	// ¼ÎÀÌ´õ ¼³Á¤
	g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
	g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
	g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );

	// º¯È¯Çà·Ä
	CBNeverChanges cbNeverChanges;
	XMMATRIX mWorld;
	XMMATRIX mView;
	XMMATRIX mProjection;

	mWorld = CreateWorldMatrix();

	// Initialize the view matrix
	XMVECTOR Eye = XMVectorSet( 0.0f, 3.0f, -5.0f, 0.0f );
	XMVECTOR At = XMVectorSet( 0.0f, 0.0f, 0.0f, 0.0f );
	XMVECTOR Up = XMVectorSet( 0.0f, 1.0f, 0.0f, 0.0f );
	mView = XMMatrixLookAtLH( Eye, At, Up );

	// Initialize the projection matrix
	mProjection = XMMatrixPerspectiveFovLH( XM_PIDIV4, VIEW_WIDTH / (FLOAT)VIEW_HEIGHT, 0.01f, 100.0f );

	cbNeverChanges.mView = XMMatrixTranspose( mWorld * mView * mProjection );
	g_pImmediateContext->UpdateSubresource( g_pCBNeverChanges, 0, NULL, &cbNeverChanges, 0, 0 );

	// ±×¸®±â
	g_pImmediateContext->OMSetBlendState( g_pbsAddBlend, NULL, 0xFFFFFFFF );
	//    g_pImmediateContext->OMSetBlendState( NULL, NULL, 0xFFFFFFFF );
	DrawChangingPictures();

	// Ç¥½Ã
	FlushDrawingPictures();

	return S_OK;
}

// ¿£Æ®¸®Æ÷ÀÎÆ®
int WINAPI _tWinMain( HINSTANCE hInst, HINSTANCE, LPTSTR, int )
{
	LARGE_INTEGER nNowTime, nLastTime; // ÇöÀç¿Í Á÷Àü ½Ã°¢
	LARGE_INTEGER nTimeFreq;					 // ½Ã°£´ÜÀ§

	// È­¸éÅ©±â
	g_nClientWidth = VIEW_WIDTH;	 // ³Êºñ
	g_nClientHeight = VIEW_HEIGHT; // ³ôÀÌ

	// Register the window class
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle( NULL ), NULL, NULL, NULL, NULL, _T( "D3D Sample" ), NULL };
	RegisterClassEx( &wc );

	RECT rcRect;
	SetRect( &rcRect, 0, 0, g_nClientWidth, g_nClientHeight );
	AdjustWindowRect( &rcRect, WS_OVERLAPPEDWINDOW, FALSE );
	g_hWnd = CreateWindow( _T( "D3D Sample" ), _T( "Textures_5_1" ), WS_OVERLAPPEDWINDOW, 100, 20, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top, GetDesktopWindow(), NULL,
												 wc.hInstance, NULL );

	// Initialize Direct3D
	if( SUCCEEDED( InitD3D() ) && SUCCEEDED( MakeShaders() ) )
	{
		// Create the shaders
		if( SUCCEEDED( InitDrawModes() ) )
		{
			if( SUCCEEDED( InitGeometry() ) )
			{ // Áö¿À¸ÞÆ®¸® ÀÛ¼º

				// Show the window
				ShowWindow( g_hWnd, SW_SHOWDEFAULT );
				UpdateWindow( g_hWnd );

				QueryPerformanceFrequency( &nTimeFreq ); // ½Ã°£´ÜÀ§
				QueryPerformanceCounter( &nLastTime );	 // 1ÇÁ·¹ÀÓÀü ½Ã°¢ ÃÊ±âÈ­

				// Enter the message loop
				MSG msg;
				ZeroMemory( &msg, sizeof( msg ) );
				while( msg.message != WM_QUIT )
				{
					Render();
					do
					{
						if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
						{
							TranslateMessage( &msg );
							DispatchMessage( &msg );
						}
						QueryPerformanceCounter( &nNowTime );
					} while( ( ( nNowTime.QuadPart - nLastTime.QuadPart ) < ( nTimeFreq.QuadPart / 90 ) ) && ( msg.message != WM_QUIT ) );
					while( ( ( nNowTime.QuadPart - nLastTime.QuadPart ) < ( nTimeFreq.QuadPart / 60 ) ) && ( msg.message != WM_QUIT ) )
					{
						QueryPerformanceCounter( &nNowTime );
					}
					nLastTime = nNowTime;
					g_pSwapChain->Present( 0, 0 ); // Ç¥½Ã
				}
			}
		}
	}

	// Clean up everything and exit the app
	Cleanup();
	UnregisterClass( _T( "D3D Sample" ), wc.hInstance );
	return 0;
}
