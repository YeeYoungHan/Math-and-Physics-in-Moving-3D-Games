//------------------------------------------------------------
// Render_4_3.cpp
// �̹����� ���� ������ �����ϸ�
//
//------------------------------------------------------------

#include <windows.h>

#define VIEW_WIDTH 640	// ȭ�� �ʺ�
#define VIEW_HEIGHT 480 // ȭ�� ����
#define SCALE_MIN 0.1f	// �ּ�Ȯ����
#define SCALE_MAX 2.0f	// �ִ�Ȯ����

// ��Ʈ�� ����ü
struct MemoryBMP
{															// BMP ������ ����ü
	int nDataSize;							// ������ ������
	BITMAPFILEHEADER *pbfFileH; // �������
	BITMAPINFOHEADER *pbiInfoH; // �������
	unsigned char *pcPixels;		// �ȼ�������
	int nPixelOffset;						// �ȼ������� ������
	int nPaletteNum;						// �ȷ�Ʈ��
	int nPitch;									// ��ġ��
};

MemoryBMP mbPicture; // ��Ʈ�� �׸�

int GetBMPPixel( int x, int y, MemoryBMP *pmbSrc, int *pnRed, int *pnGreen, int *pnBlue );
// �ȼ� ���
int FlushDrawingPictures( void ); // �׸��� �׸��� ��� ��� �÷���
int DrawPoints( int x, int y, int nRed, int nGreen, int nBlue );
// �� �� �׸���

// ��ĵ���� �˰��� ���� ������
int RenderScanLine( void )
{
	int x, y;
	int nRed, nGreen, nBlue;		// �ȼ���
	static float fScale = 1.0f; // Ȯ����
	static float fScaleVel = 0.1f;

	for( y = 0; y < VIEW_HEIGHT; y++ )
	{ // y���� ����
		for( x = 0; x < VIEW_WIDTH; x++ )
		{																																						 // x���� ����
			GetBMPPixel( x / fScale, y / fScale, &mbPicture, &nRed, &nGreen, &nBlue ); // �� ���
			DrawPoints( x, y, nRed, nGreen, nBlue );																	 // �� �׸���
		}
		FlushDrawingPictures();
	}

	// Ȯ��������
	fScale += fScaleVel;
	if( fScale < SCALE_MIN )
	{
		fScale = SCALE_MIN;
		fScaleVel = -fScaleVel;
	}
	if( fScale > SCALE_MAX )
	{
		fScale = SCALE_MAX;
		fScaleVel = -fScaleVel;
	}

	return 0;
}

//------------------------------------------------------------
// ���� DirectX�� ���� ǥ�� ���α׷�

#include <stdio.h>
#include <tchar.h> // Unicode. ��Ƽ����Ʈ ���� ���� �������

#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>

#define MAX_BUFFER_VERTEX 20000 // �ִ� ���� ���� ��

// ��ũ ���̺귯��
#pragma comment( lib, "d3d11.lib" ) // D3D11���̺귯��
#pragma comment( lib, "d3dx11.lib" )

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
};

// ���̴� �������ü
struct CBNeverChanges
{
	XMMATRIX mView;
};

// �ؽ�ó �׸� ����ü
struct TEX_PICTURE
{
	ID3D11ShaderResourceView *pSRViewTexture;
	D3D11_TEXTURE2D_DESC tdDesc;
	int nWidth, nHeight;
};

// �۷ι� ����
UINT g_nClientWidth;	// ������ ������ ������
UINT g_nClientHeight; // ������ ������ ����

HWND g_hWnd; // ������ �ڵ�

ID3D11Device *g_pd3dDevice;								// ����̽�
IDXGISwapChain *g_pSwapChain;							// DXGI ����ü��
ID3D11DeviceContext *g_pImmediateContext; // ����̽� ���ؽ�Ʈ
ID3D11RasterizerState *g_pRS;							// �����Ͷ�����
ID3D11RenderTargetView *g_pRTV;						// ������ Ÿ��
D3D_FEATURE_LEVEL g_FeatureLevel;					// ��ó ����

ID3D11Buffer *g_pD3D11VertexBuffer;	 // ���� ����
ID3D11BlendState *g_pbsAlphaBlend;	 // ���� ����
ID3D11VertexShader *g_pVertexShader; // ���� ���̴�
ID3D11PixelShader *g_pPixelShader;	 // �ȼ� ���̴�
ID3D11InputLayout *g_pInputLayout;	 // ���̴� �Է� ���̾ƿ�
ID3D11SamplerState *g_pSamplerState; // ���÷� ������Ʈ

ID3D11Buffer *g_pCBNeverChanges = NULL;

// TEX_PICTURE				g_tBall, g_tBack;

// ������ ���� ����
CUSTOMVERTEX g_cvVertices[MAX_BUFFER_VERTEX];
int g_nVertexNum = 0;
ID3D11ShaderResourceView *g_pNowTexture = NULL;

// Direct3D �ʱ�ȭ
HRESULT InitD3D( void )
{
	HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL FeatureLevelsRequested[6] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
																									D3D_FEATURE_LEVEL_9_3,	D3D_FEATURE_LEVEL_9_2,	D3D_FEATURE_LEVEL_9_1 };
	UINT numLevelsRequested = 6;
	D3D_FEATURE_LEVEL FeatureLevelsSupported;

	// ����̽� ����
	hr = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevelsRequested, numLevelsRequested, D3D11_SDK_VERSION, &g_pd3dDevice, &FeatureLevelsSupported,
													&g_pImmediateContext );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// ���丮 ȹ��
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

	g_pImmediateContext->OMSetRenderTargets( 1, &g_pRTV, NULL );

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
	hr = D3DX11CompileFromFile( _T( "Basic_2D_Geom.fx" ), NULL, NULL, "VS", "vs_4_0_level_9_1", dwShaderFlags, 0, NULL, &pVertexShaderBuffer, &pError, NULL );
	if( FAILED( hr ) )
	{
		MessageBox( NULL, _T( "Can't open Basic_2D_Geom.fx" ), _T( "Error" ), MB_OK );
		SAFE_RELEASE( pError );
		return hr;
	}
	hr = D3DX11CompileFromFile( _T( "Basic_2D_Geom.fx" ), NULL, NULL, "PS", "ps_4_0_level_9_1", dwShaderFlags, 0, NULL, &pPixelShaderBuffer, &pError, NULL );
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
	// �Է� ���� �Է� ���� ����
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

// ��ο� ��� ������Ʈ �ʱ�ȭ
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

int LoadBMPData( char *szFileName, MemoryBMP *pmbDest ) // BMP  ���Ϸε�
{
	FILE *fp;								 // ���� ������
	unsigned char *pcBMPBuf; // BMP �����͹���

	// ���� ����
	if( fopen_s( &fp, szFileName, "rb" ) != 0 )
	{
		//	if ( ( fp = fopen( szFileName, "rb" ) ) == NULL ) {
		return -1;
	}

	// ���� ũ�� ���ϱ�
	fseek( fp, 0, SEEK_END );					// ������ Ž��
	pmbDest->nDataSize = ftell( fp ); // ��ġ�� �����Ѵ�
	fseek( fp, 0, SEEK_SET );					// ó������ ���ư���

	// �޸� Ȯ��, ���� �ε�
	pcBMPBuf = (unsigned char *)malloc( pmbDest->nDataSize ); // �޸� Ȯ��
	fread( pcBMPBuf, 1, pmbDest->nDataSize, fp );							// ���� �ε�

	fclose( fp ); // ���� �ݱ�

	pmbDest->pbfFileH = (BITMAPFILEHEADER *)pcBMPBuf;																	 // ���� ���
	pmbDest->pbiInfoH = (BITMAPINFOHEADER *)( pcBMPBuf + sizeof( BITMAPFILEHEADER ) ); // �������

	if( pmbDest->pbiInfoH->biBitCount != 24 )
		return -1;
	// �ȷ�Ʈ �����ͼ� ����
	if( pmbDest->pbiInfoH->biBitCount <= 8 )
	{ // 256�� ���϶�� �ȷ�Ʈ �����Ͱ� �ִ�
		if( pmbDest->pbiInfoH->biClrUsed == 0 )
		{ // ����Ʈ�� ���
			pmbDest->nPaletteNum = 1 << pmbDest->pbiInfoH->biBitCount;
		}
		else
		{
			pmbDest->nPaletteNum = pmbDest->pbiInfoH->biClrUsed;
		}
	}

	// �ȼ� ��ġ
	pmbDest->nPixelOffset = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + pmbDest->nPaletteNum * sizeof( RGBQUAD );
	pmbDest->pcPixels = pcBMPBuf + pmbDest->nPixelOffset;

	// ��ġ ���
	pmbDest->nPitch = ( pmbDest->pbiInfoH->biBitCount * pmbDest->pbiInfoH->biWidth / 8 + 3 ) & 0xfffc;

	return 0;
}

int ReleaseBMPData( MemoryBMP *pmbRelease ) // BMP ������ ����
{
	if( pmbRelease->pbfFileH )
	{																// �޸𸮰� ���� �ִ°�
		free( pmbRelease->pbfFileH ); // ����
		pmbRelease->pbfFileH = NULL;
	}

	return 0;
}

// �ȼ� ���
int GetBMPPixel( int x, int y, MemoryBMP *pmbSrc, int *pnRed, int *pnGreen, int *pnBlue )
{
	unsigned char *pLineHead, *pPixelLoc; // bmp �о���

	if( ( x >= 0 ) && ( x < pmbSrc->pbiInfoH->biWidth ) && ( y >= 0 ) && ( y < pmbSrc->pbiInfoH->biHeight ) )
	{
		pLineHead = pmbSrc->pcPixels + pmbSrc->nPitch * ( pmbSrc->pbiInfoH->biHeight - 1 );
		pPixelLoc = pLineHead - pmbSrc->nPitch * y + x * 3;
		*pnRed = (int)*( pPixelLoc + 2 );
		*pnGreen = (int)*( pPixelLoc + 1 );
		*pnBlue = (int)*pPixelLoc;
	}
	else
	{
		*pnRed = 0;
		*pnGreen = 0;
		*pnBlue = 255;
	}

	return 0;
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

	// �̹��� �ε�
	if( LoadBMPData( "2.bmp", &mbPicture ) < 0 )
	{
		MessageBox( NULL, _T( "Can't open 2.BMP" ), _T( "Error" ), MB_OK );
		return -1;
	}

	return S_OK;
}

// ����ó��
int Cleanup( void )
{
	ReleaseBMPData( &mbPicture );
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

	SAFE_RELEASE( g_pRTV ); // ������ Ÿ��

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

// �׸��� ��� ��� �÷���
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
		g_pImmediateContext->PSSetShaderResources( 0, 1, &g_pNowTexture );
		g_pImmediateContext->Draw( g_nVertexNum, 0 );
	}
	g_nVertexNum = 0;
	g_pNowTexture = NULL;

	return 0;
}

// 1�� �׸���
int DrawPoints( int x, int y, int nRed, int nGreen, int nBlue )
{
	g_cvVertices[g_nVertexNum].v4Pos = XMFLOAT4( (float)x, (float)y, 0.0f, 1.0f );
	g_cvVertices[g_nVertexNum].v4Color = XMFLOAT4( nRed / 255.0f, nGreen / 255.0f, nBlue / 255.0f, 1.0f );
	g_nVertexNum++;

	return 0;
}

// ������
HRESULT Render( void )
{
	//	int				i, j;
	// ȭ�� Ŭ����
	XMFLOAT4 v4Color = XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f );
	g_pImmediateContext->ClearRenderTargetView( g_pRTV, (float *)&v4Color );

	// ���÷� �����Ͷ����� ��Ʈ
	g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerState );
	g_pImmediateContext->RSSetState( g_pRS );

	// �׸��� ����
	UINT nStrides = sizeof( CUSTOMVERTEX );
	UINT nOffsets = 0;
	g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pD3D11VertexBuffer, &nStrides, &nOffsets );
	//    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	g_pImmediateContext->IASetInputLayout( g_pInputLayout );

	// ���̴� ����
	g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
	g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
	g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );

	// �׸���
	g_pImmediateContext->OMSetBlendState( NULL, NULL, 0xFFFFFFFF );
	RenderScanLine();
	//	DrawPicture( 0.0f, 0.0f, &g_tBack );

	// ǥ��
	FlushDrawingPictures();

	return S_OK;
}

// ��Ʈ�� ����Ʈ
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
	g_hWnd = CreateWindow( _T( "D3D Sample" ), _T( "Render_4_3" ), WS_OVERLAPPEDWINDOW, 100, 20, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top, GetDesktopWindow(), NULL,
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

				//				InitCharacter();									// ĳ���� �ʱ�ȭ

				QueryPerformanceFrequency( &nTimeFreq ); // �ð�����
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
