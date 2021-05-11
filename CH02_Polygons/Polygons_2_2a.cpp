//------------------------------------------------------------
// Polygons_2_2a.cpp
// ����ҰŸ� ������ ���� ������ �׸���
//
//------------------------------------------------------------

#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>

#define VIEW_WIDTH 800	// ȭ��ʺ�
#define VIEW_HEIGHT 600 // ȭ�����

int Draw3DPolygon( float x1, float y1, float z1, int nColor1, float x2, float y2, float z2, int nColor2, float x3, float y3, float z3, int nColor3 );
// 3D ������ �׸���

//���� ��� ����
XMMATRIX CreateWorldMatrix( void )
{
	float fAngle;

	fAngle = 2.0f * 3.1415f * (float)( timeGetTime() % 2000 ) / 2000.0f;
	return XMMatrixRotationY( fAngle );
}

// �׸���
int DrawChangingPictures( void )
{
	Draw3DPolygon( -1.0f, 1.0f, 0.0f, 0xffff0000, 1.0f, 1.0f, 0.0f, 0xff00ff00, 0.0f, -1.0f, 1.0f, 0xff0000ff );
	Draw3DPolygon( -1.0f, 1.0f, 0.0f, 0xffff0000, 1.0f, 1.0f, 0.0f, 0xff00ff00, 0.0f, -1.0f, -1.0f, 0xff0000ff );

	return 0;
}

//------------------------------------------------------------
// ���� DirectX�� ���� ǥ�� ���α׷�

#include <stdio.h>
#include <tchar.h> // Unicode ��Ƽ����Ʈ ���� ����
#include <windows.h>

#define MAX_BUFFER_VERTEX 1000 // �ִ� ���� ������

// ��ũ ���̺귯��
#pragma comment( lib, "d3d11.lib" ) // D3D11���̺귯��
#pragma comment( lib, "d3dx11.lib" )
#pragma comment( lib, "winmm.lib" )

// ������ ������ ��ũ��
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

// ��������ü
struct CUSTOMVERTEX
{
	XMFLOAT4 v4Pos;
	XMFLOAT4 v4Color;
	//	XMFLOAT2	v2UV;
};

// ���̴� ��� ����ü
struct CBNeverChanges
{
	XMMATRIX mView;
};

// �۷ι� ����
UINT g_nClientWidth;	// �׸� ������ �ʺ�
UINT g_nClientHeight; // �׸� ������ ����

HWND g_hWnd; // ������ �ڵ�

ID3D11Device *g_pd3dDevice;													// ����̽�
IDXGISwapChain *g_pSwapChain;												// DXGI���� ü��
ID3D11DeviceContext *g_pImmediateContext;						// ����̽� ���ؽ�Ʈ
ID3D11RasterizerState *g_pRS;												// �����Ͷ�����
ID3D11RenderTargetView *g_pRTV;											// ������ Ÿ��
ID3D11Texture2D *g_pDepthStencil = NULL;						// Z����
ID3D11DepthStencilView *g_pDepthStencilView = NULL; // Z������ ��
D3D_FEATURE_LEVEL g_FeatureLevel;										// ��ó����

ID3D11Buffer *g_pD3D11VertexBuffer;
ID3D11BlendState *g_pbsAlphaBlend;
ID3D11VertexShader *g_pVertexShader;
ID3D11PixelShader *g_pPixelShader;
ID3D11InputLayout *g_pInputLayout;
ID3D11SamplerState *g_pSamplerState;

ID3D11Buffer *g_pCBNeverChanges = NULL;

// �׸��� ���� ����
CUSTOMVERTEX g_cvVertices[MAX_BUFFER_VERTEX];
int g_nVertexNum = 0;
ID3D11ShaderResourceView *g_pNowTexture = NULL;

// Direct3D�ʱ�ȭ
HRESULT InitD3D( void )
{
	HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL FeatureLevelsRequested[6] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
																									D3D_FEATURE_LEVEL_9_3,	D3D_FEATURE_LEVEL_9_2,	D3D_FEATURE_LEVEL_9_1 };
	UINT numLevelsRequested = 6;
	D3D_FEATURE_LEVEL FeatureLevelsSupported;

	// ����̽� �ۼ�
	hr = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevelsRequested, numLevelsRequested, D3D11_SDK_VERSION, &g_pd3dDevice, &FeatureLevelsSupported,
													&g_pImmediateContext );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// ���丮 ���
	IDXGIDevice *pDXGIDevice;
	hr = g_pd3dDevice->QueryInterface( __uuidof( IDXGIDevice ), (void **)&pDXGIDevice );
	IDXGIAdapter *pDXGIAdapter;
	hr = pDXGIDevice->GetParent( __uuidof( IDXGIAdapter ), (void **)&pDXGIAdapter );
	IDXGIFactory *pIDXGIFactory;
	pDXGIAdapter->GetParent( __uuidof( IDXGIFactory ), (void **)&pIDXGIFactory );

	// ����ü�� ����
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

	// ������ Ÿ�� ����
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

	// *** ������ Ÿ�� ���� ***
	g_pImmediateContext->OMSetRenderTargets( 1, &g_pRTV, g_pDepthStencilView );

	//    g_pImmediateContext->OMSetRenderTargets( 1, &g_pRTV, NULL );

	// �����Ͷ����� ����
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

	// ����Ʈ ����
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

// ���α׷��Ӻ� ���̴� �ۼ�
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
	// ������
	hr = D3DX11CompileFromFile( _T( "Basic_3D_Color.fx" ), NULL, NULL, "VS", "vs_4_0_level_9_1", dwShaderFlags, 0, NULL, &pVertexShaderBuffer, &pError, NULL );
	if( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "Can't open Basic_3D_Color.fx" ), _T( "Error" ), MB_OK );
		SAFE_RELEASE( pError );
		return hr;
	}
	hr = D3DX11CompileFromFile( _T( "Basic_3D_Color.fx" ), NULL, NULL, "PS", "ps_4_0_level_9_1", dwShaderFlags, 0, NULL, &pPixelShaderBuffer, &pError, NULL );
	if( FAILED( hr ) )
	{
		SAFE_RELEASE( pVertexShaderBuffer );
		SAFE_RELEASE( pError );
		return hr;
	}
	SAFE_RELEASE( pError );

	// VertexShader ����
	hr = g_pd3dDevice->CreateVertexShader( pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), NULL, &g_pVertexShader );
	if( FAILED( hr ) )
	{
		SAFE_RELEASE( pVertexShaderBuffer );
		SAFE_RELEASE( pPixelShaderBuffer );
		return hr;
	}
	// PixelShader ����
	hr = g_pd3dDevice->CreatePixelShader( pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), NULL, &g_pPixelShader );
	if( FAILED( hr ) )
	{
		SAFE_RELEASE( pVertexShaderBuffer );
		SAFE_RELEASE( pPixelShaderBuffer );
		return hr;
	}

	// �Է� ������ �Է� ����
	D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//        { "TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );
	// �Է� ������ �Է� ���� �ۼ�
	hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), &g_pInputLayout );
	SAFE_RELEASE( pVertexShaderBuffer );
	SAFE_RELEASE( pPixelShaderBuffer );
	if( FAILED( hr ) )
	{
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

	// ��ȯ���
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

// �׸��� ��� ������Ʈ �ʱ�ȭ
int InitDrawModes( void )
{
	HRESULT hr;

	// ���� ������Ʈ
	D3D11_BLEND_DESC BlendDesc;
	BlendDesc.AlphaToCoverageEnable = FALSE;
	BlendDesc.IndependentBlendEnable = FALSE;
	BlendDesc.RenderTarget[0].BlendEnable = TRUE;
	BlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	BlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	BlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	BlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	BlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	BlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	hr = g_pd3dDevice->CreateBlendState( &BlendDesc, &g_pbsAlphaBlend );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// ���÷�
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

// ������Ʈ�� �ʱ�ȭ
HRESULT InitGeometry( void )
{
	HRESULT hr = S_OK;

	// ���� ���� ����
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
	hr = g_pd3dDevice->CreateBuffer( &BufferDesc, &SubResourceData, &g_pD3D11VertexBuffer );
	if( FAILED( hr ) )
	{
		return hr;
	}

	return S_OK;
}

// ����ó��
int Cleanup( void )
{
	SAFE_RELEASE( g_pD3D11VertexBuffer );

	SAFE_RELEASE( g_pSamplerState );
	SAFE_RELEASE( g_pbsAlphaBlend );
	SAFE_RELEASE( g_pInputLayout );
	SAFE_RELEASE( g_pPixelShader );
	SAFE_RELEASE( g_pVertexShader );
	SAFE_RELEASE( g_pCBNeverChanges );

	SAFE_RELEASE( g_pRS ); // �����Ͷ�����

	// �������ͽ� Ŭ����
	if( g_pImmediateContext )
	{
		g_pImmediateContext->ClearState();
		g_pImmediateContext->Flush();
	}

	SAFE_RELEASE( g_pRTV );							 // ������ Ÿ��
	SAFE_RELEASE( g_pDepthStencil );		 // Z����
	SAFE_RELEASE( g_pDepthStencilView ); // Z������ ��

	// ����ü��
	if( g_pSwapChain != NULL )
	{
		g_pSwapChain->SetFullscreenState( FALSE, 0 );
	}
	SAFE_RELEASE( g_pSwapChain );

	SAFE_RELEASE( g_pImmediateContext ); // ����̽� ���ؽ�Ʈ
	SAFE_RELEASE( g_pd3dDevice );				 // ����̽�

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

// ?����?�����֪�ի�ë���
int FlushDrawingPictures( void )
{
	HRESULT hr;

	if( g_nVertexNum > 0 )
	{
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		hr = g_pImmediateContext->Map( g_pD3D11VertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource );
		if( SUCCEEDED( hr ) )
		{
			CopyMemory( mappedResource.pData, &( g_cvVertices[0] ), sizeof( CUSTOMVERTEX ) * g_nVertexNum );
			g_pImmediateContext->Unmap( g_pD3D11VertexBuffer, 0 );
		}
		// g_pImmediateContext->PSSetShaderResources( 0, 1, NULL );
		g_pImmediateContext->Draw( g_nVertexNum, 0 );
	}
	g_nVertexNum = 0;

	return 0;
}

// 3D ������ �׸���
int Draw3DPolygon( float x1, float y1, float z1, int nColor1, float x2, float y2, float z2, int nColor2, float x3, float y3, float z3, int nColor3 )
{
	if( g_nVertexNum > ( MAX_BUFFER_VERTEX - 3 ) )
		return -1; // ������ ���ۿ��� ��ĥ ���� �׸��� �ʰ�

	// �ؽ�ó�� ������ ��� ��� �÷���
	if( g_pNowTexture )
	{
		FlushDrawingPictures();
	}

	// ���� ��Ʈ
	g_cvVertices[g_nVertexNum + 0].v4Pos = XMFLOAT4( x1, y1, z1, 1.0f );
	g_cvVertices[g_nVertexNum + 0].v4Color = XMFLOAT4( (float)( ( nColor1 >> 16 ) & 0xff ) / 255.0f, (float)( ( nColor1 >> 8 ) & 0xff ) / 255.0f, (float)( (nColor1)&0xff ) / 255.0f,
																										 (float)( ( nColor1 >> 24 ) & 0xff ) / 255.0f );
	g_cvVertices[g_nVertexNum + 1].v4Pos = XMFLOAT4( x2, y2, z2, 1.0f );
	g_cvVertices[g_nVertexNum + 1].v4Color = XMFLOAT4( (float)( ( nColor2 >> 16 ) & 0xff ) / 255.0f, (float)( ( nColor2 >> 8 ) & 0xff ) / 255.0f, (float)( (nColor2)&0xff ) / 255.0f,
																										 (float)( ( nColor2 >> 24 ) & 0xff ) / 255.0f );
	g_cvVertices[g_nVertexNum + 2].v4Pos = XMFLOAT4( x3, y3, z3, 1.0f );
	g_cvVertices[g_nVertexNum + 2].v4Color = XMFLOAT4( (float)( ( nColor3 >> 16 ) & 0xff ) / 255.0f, (float)( ( nColor3 >> 8 ) & 0xff ) / 255.0f, (float)( (nColor3)&0xff ) / 255.0f,
																										 (float)( ( nColor3 >> 24 ) & 0xff ) / 255.0f );
	g_nVertexNum += 3;
	g_pNowTexture = NULL;

	return 0;
}

// ������
HRESULT Render( void )
{
	// ȭ�� Ŭ����
	XMFLOAT4 v4Color = XMFLOAT4( 0.0f, 0.0f, 0.0f, 1.0f );
	g_pImmediateContext->ClearRenderTargetView( g_pRTV, (float *)&v4Color );
	// *** Z���� Ŭ���� ***
	g_pImmediateContext->ClearDepthStencilView( g_pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0 );

	// ���÷� �����Ͷ����� ��Ʈ
	g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerState );
	g_pImmediateContext->RSSetState( g_pRS );

	// �׸��� ����
	UINT nStrides = sizeof( CUSTOMVERTEX );
	UINT nOffsets = 0;
	g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pD3D11VertexBuffer, &nStrides, &nOffsets );
	g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	g_pImmediateContext->IASetInputLayout( g_pInputLayout );

	// ���̴� ����
	g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
	g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
	g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );

	// ��ȯ���
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

	// �׸���
	g_pImmediateContext->OMSetBlendState( NULL, NULL, 0xFFFFFFFF );
	DrawChangingPictures();

	// ǥ��
	FlushDrawingPictures();

	return S_OK;
}

// ��Ʈ������Ʈ
int WINAPI _tWinMain( HINSTANCE hInst, HINSTANCE, LPTSTR, int )
{
	LARGE_INTEGER nNowTime, nLastTime; // ����� ���� �ð�
	LARGE_INTEGER nTimeFreq;					 // �ð� ����

	// ȭ�� ũ��
	g_nClientWidth = VIEW_WIDTH;	 // �ʺ�
	g_nClientHeight = VIEW_HEIGHT; // ����

	// Register the window class
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle( NULL ), NULL, NULL, NULL, NULL, _T( "D3D Sample" ), NULL };
	RegisterClassEx( &wc );

	RECT rcRect;
	SetRect( &rcRect, 0, 0, g_nClientWidth, g_nClientHeight );
	AdjustWindowRect( &rcRect, WS_OVERLAPPEDWINDOW, FALSE );
	g_hWnd = CreateWindow( _T( "D3D Sample" ), _T( "Polygons_2_2a" ), WS_OVERLAPPEDWINDOW, 100, 20, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top, GetDesktopWindow(), NULL,
												 wc.hInstance, NULL );

	// Initialize Direct3D
	if( SUCCEEDED( InitD3D() ) && SUCCEEDED( MakeShaders() ) )
	{
		// Create the shaders
		if( SUCCEEDED( InitDrawModes() ) )
		{
			if( SUCCEEDED( InitGeometry() ) )
			{ // ������Ʈ�� �ۼ�

				// Show the window
				ShowWindow( g_hWnd, SW_SHOWDEFAULT );
				UpdateWindow( g_hWnd );

				QueryPerformanceFrequency( &nTimeFreq ); // �ð� ����
				QueryPerformanceCounter( &nLastTime );	 // 1�������� �ð� �ʱ�ȭ

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
					g_pSwapChain->Present( 0, 0 ); // ǥ��
				}
			}
		}
	}

	// Clean up everything and exit the app
	Cleanup();
	UnregisterClass( _T( "D3D Sample" ), wc.hInstance );
	return 0;
}
