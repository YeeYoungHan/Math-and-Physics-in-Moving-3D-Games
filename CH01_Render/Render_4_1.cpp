//------------------------------------------------------------
// Render_4_1.cpp
// 이미지 확대
//
//------------------------------------------------------------

#include <windows.h>

#define VIEW_WIDTH 640	// 화면 너비
#define VIEW_HEIGHT 480 // 화면 높이

// 비트맵 구조체
struct MemoryBMP
{															// BMP 데이터 구조체
	int nDataSize;							// 데이터 사이즈
	BITMAPFILEHEADER *pbfFileH; // 파일헤더
	BITMAPINFOHEADER *pbiInfoH; // 인포헤더
	unsigned char *pcPixels;		// 픽셀데이터
	int nPixelOffset;						// 픽셀까지의 오프셋
	int nPaletteNum;						// 팔레트수
	int nPitch;									// 피치폭
};

MemoryBMP mbPicture; // 비트맵 그림

int GetBMPPixel( int x, int y, MemoryBMP *pmbSrc, int *pnRed, int *pnGreen, int *pnBlue );
// 픽셀 취득
int FlushDrawingPictures( void ); // 그림의 그리기 대기 행렬 플러시
int DrawPoints( int x, int y, int nRed, int nGreen, int nBlue );
// 한 점 그리기

// 스캔라인 알고리즘에 의한 렌더링
int RenderScanLine( void )
{
	int x, y;
	int nRed, nGreen, nBlue; // 픽셀색

	for( y = 0; y < VIEW_HEIGHT; y++ )
	{ // y방향 루프
		for( x = 0; x < VIEW_WIDTH; x++ )
		{																																	 // x방향 루프
			GetBMPPixel( x / 2, y / 2, &mbPicture, &nRed, &nGreen, &nBlue ); // 점 취득
			DrawPoints( x, y, nRed, nGreen, nBlue );												 // 점 그리기
		}
		FlushDrawingPictures();
	}

	return 0;
}

//------------------------------------------------------------
// 이하 DirectX에 의한 표시 프로그램

#include <stdio.h>
#include <tchar.h> // Unicode. 멀티바이트 문자 관련 헤더파일

#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <xnamath.h>

#define MAX_BUFFER_VERTEX 20000 // 최대 버퍼 정점 수

// 링크 라이브러리
#pragma comment( lib, "d3d11.lib" ) // D3D11라이브러리
#pragma comment( lib, "d3dx11.lib" )

// 세이프 릴리스 매크로
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

// 정점구조체
struct CUSTOMVERTEX
{
	XMFLOAT4 v4Pos;
	XMFLOAT4 v4Color;
};

// 셰이더 상수구조체
struct CBNeverChanges
{
	XMMATRIX mView;
};

// 텍스처 이미지 구조체
struct TEX_PICTURE
{
	ID3D11ShaderResourceView *pSRViewTexture;
	D3D11_TEXTURE2D_DESC tdDesc;
	int nWidth, nHeight;
};

// 글로벌 변수
UINT g_nClientWidth;	// 렌더링 영역의 가로폭
UINT g_nClientHeight; // 렌더링 영역의 높이

HWND g_hWnd; // 윈도우 핸들

ID3D11Device *g_pd3dDevice;								// 디바이스
IDXGISwapChain *g_pSwapChain;							// DXGI 스왑체인
ID3D11DeviceContext *g_pImmediateContext; // 디바이스 컨텍스트
ID3D11RasterizerState *g_pRS;							// 래스터라이저
ID3D11RenderTargetView *g_pRTV;						// 렌더링 타깃
D3D_FEATURE_LEVEL g_FeatureLevel;					// 피처 레벨

ID3D11Buffer *g_pD3D11VertexBuffer;	 // 정점 버퍼
ID3D11BlendState *g_pbsAlphaBlend;	 // 알파 블렌드
ID3D11VertexShader *g_pVertexShader; // 정점 셰이더
ID3D11PixelShader *g_pPixelShader;	 // 픽셀 셰이더
ID3D11InputLayout *g_pInputLayout;	 // 셰이더 입력 레이아웃
ID3D11SamplerState *g_pSamplerState; // 샘플러 스테이트

ID3D11Buffer *g_pCBNeverChanges = NULL;

// TEX_PICTURE				g_tBall, g_tBack;

// 렌더링 정점 버퍼
CUSTOMVERTEX g_cvVertices[MAX_BUFFER_VERTEX];
int g_nVertexNum = 0;
ID3D11ShaderResourceView *g_pNowTexture = NULL;

// Direct3D초기화
HRESULT InitD3D( void )
{
	HRESULT hr = S_OK;
	D3D_FEATURE_LEVEL FeatureLevelsRequested[6] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
																									D3D_FEATURE_LEVEL_9_3,	D3D_FEATURE_LEVEL_9_2,	D3D_FEATURE_LEVEL_9_1 };
	UINT numLevelsRequested = 6;
	D3D_FEATURE_LEVEL FeatureLevelsSupported;

	// 디바이스 생성
	hr = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, FeatureLevelsRequested, numLevelsRequested, D3D11_SDK_VERSION, &g_pd3dDevice, &FeatureLevelsSupported,
													&g_pImmediateContext );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// 팩토리 취득
	IDXGIDevice *pDXGIDevice;
	hr = g_pd3dDevice->QueryInterface( __uuidof( IDXGIDevice ), (void **)&pDXGIDevice );
	IDXGIAdapter *pDXGIAdapter;
	hr = pDXGIDevice->GetParent( __uuidof( IDXGIAdapter ), (void **)&pDXGIAdapter );
	IDXGIFactory *pIDXGIFactory;
	pDXGIAdapter->GetParent( __uuidof( IDXGIFactory ), (void **)&pIDXGIFactory );

	// 스왑체인 생성
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

	// 렌더링 타깃 생성
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

	// 래스터라이저 설정
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

	// 뷰포트 설정
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

// 프로그래머블 셰이더 작성
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
	// コンパイル
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

	// VertexShader생성
	hr = g_pd3dDevice->CreateVertexShader( pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), NULL, &g_pVertexShader );
	if( FAILED( hr ) )
	{
		SAFE_RELEASE( pVertexShaderBuffer );
		SAFE_RELEASE( pPixelShaderBuffer );
		return hr;
	}
	// PixelShader생성
	hr = g_pd3dDevice->CreatePixelShader( pPixelShaderBuffer->GetBufferPointer(), pPixelShaderBuffer->GetBufferSize(), NULL, &g_pPixelShader );
	if( FAILED( hr ) )
	{
		SAFE_RELEASE( pVertexShaderBuffer );
		SAFE_RELEASE( pPixelShaderBuffer );
		return hr;
	}

	// 입력 버퍼의 입력 형식
	D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			//        { "TEXTURE",  0, DXGI_FORMAT_R32G32_FLOAT,       0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	UINT numElements = ARRAYSIZE( layout );
	// 입력 버퍼의 입력형식 작성
	hr = g_pd3dDevice->CreateInputLayout( layout, numElements, pVertexShaderBuffer->GetBufferPointer(), pVertexShaderBuffer->GetBufferSize(), &g_pInputLayout );
	SAFE_RELEASE( pVertexShaderBuffer );
	SAFE_RELEASE( pPixelShaderBuffer );
	if( FAILED( hr ) )
	{
		return hr;
	}

	// 셰이더 상수 버퍼 생성
	D3D11_BUFFER_DESC bd;
	ZeroMemory( &bd, sizeof( bd ) );
	bd.Usage = D3D11_USAGE_DEFAULT;
	bd.ByteWidth = sizeof( CBNeverChanges );
	bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bd.CPUAccessFlags = 0;
	hr = g_pd3dDevice->CreateBuffer( &bd, NULL, &g_pCBNeverChanges );
	if( FAILED( hr ) )
		return hr;

	// 변환행렬
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

// 그리기 모드 오브젝트 초기화
int InitDrawModes( void )
{
	HRESULT hr;

	// 블렌드 스테이트
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

	// 샘플러
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

int LoadBMPData( char *szFileName, MemoryBMP *pmbDest ) // BMP 파일로드
{
	FILE *fp;								 // 파일포인터
	unsigned char *pcBMPBuf; // BMP 데이터 버퍼

	// 파일 열기
	if( fopen_s( &fp, szFileName, "rb" ) != 0 )
	{
		//	if ( ( fp = fopen( szFileName, "rb" ) ) == NULL ) {
		return -1;
	}

	// 파일 크기 구하기
	fseek( fp, 0, SEEK_END );					// 마지막까지 탐색
	pmbDest->nDataSize = ftell( fp ); // 위치를 조사한다
	fseek( fp, 0, SEEK_SET );					// 처음으로 돌아간다

	// 메모리 확보, 파일로드
	pcBMPBuf = (unsigned char *)malloc( pmbDest->nDataSize ); // 메모리 확보
	fread( pcBMPBuf, 1, pmbDest->nDataSize, fp );							// 파일 로드

	fclose( fp ); // 파일 닫기

	pmbDest->pbfFileH = (BITMAPFILEHEADER *)pcBMPBuf;																	 // 파일 헤더
	pmbDest->pbiInfoH = (BITMAPINFOHEADER *)( pcBMPBuf + sizeof( BITMAPFILEHEADER ) ); // 인포헤더

	if( pmbDest->pbiInfoH->biBitCount != 24 )
		return -1;
	// 팔레트 데이터 수 결정
	if( pmbDest->pbiInfoH->biBitCount <= 8 )
	{ // 256색 이하면 팔레트 데이터가 있다
		if( pmbDest->pbiInfoH->biClrUsed == 0 )
		{ // 디폴트일 경우
			pmbDest->nPaletteNum = 1 << pmbDest->pbiInfoH->biBitCount;
		}
		else
		{
			pmbDest->nPaletteNum = pmbDest->pbiInfoH->biClrUsed;
		}
	}

	// 픽셀 위치
	pmbDest->nPixelOffset = sizeof( BITMAPFILEHEADER ) + sizeof( BITMAPINFOHEADER ) + pmbDest->nPaletteNum * sizeof( RGBQUAD );
	pmbDest->pcPixels = pcBMPBuf + pmbDest->nPixelOffset;

	// 피치 계산
	pmbDest->nPitch = ( pmbDest->pbiInfoH->biBitCount * pmbDest->pbiInfoH->biWidth / 8 + 3 ) & 0xfffc;

	return 0;
}

int ReleaseBMPData( MemoryBMP *pmbRelease ) // BMP데이터 해제
{
	if( pmbRelease->pbfFileH )
	{																// 메모리가 남아 있는가
		free( pmbRelease->pbfFileH ); // 해제
		pmbRelease->pbfFileH = NULL;
	}

	return 0;
}

// 픽셀 가져오기
int GetBMPPixel( int x, int y, MemoryBMP *pmbSrc, int *pnRed, int *pnGreen, int *pnBlue )
{
	unsigned char *pLineHead, *pPixelLoc; // bmp 읽어내기용

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

// 지오메트리 초기화
HRESULT InitGeometry( void )
{
	HRESULT hr = S_OK;

	// 정점 버퍼 생성
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

	// 이미지 로드
	if( LoadBMPData( "1.bmp", &mbPicture ) < 0 )
	{
		MessageBox( NULL, _T( "Can't open 1.bmp" ), _T( "Error" ), MB_OK );
		return -1;
	}

	return S_OK;
}

// 종료처리
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

	SAFE_RELEASE( g_pRS ); // 래스터라이저

	// 스테이터스 클리어
	if( g_pImmediateContext )
	{
		g_pImmediateContext->ClearState();
		g_pImmediateContext->Flush();
	}

	SAFE_RELEASE( g_pRTV ); // 렌더링 타깃

	// 스왑체인
	if( g_pSwapChain != NULL )
	{
		g_pSwapChain->SetFullscreenState( FALSE, 0 );
	}
	SAFE_RELEASE( g_pSwapChain );

	SAFE_RELEASE( g_pImmediateContext ); // 디바이스 컨텍스트
	SAFE_RELEASE( g_pd3dDevice );				 // 디바이스

	return 0;
}

// 윈도우 프로시저
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

// 그리기 대기 행렬 플러시
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

// 1점 그리기
int DrawPoints( int x, int y, int nRed, int nGreen, int nBlue )
{
	g_cvVertices[g_nVertexNum].v4Pos = XMFLOAT4( (float)x, (float)y, 0.0f, 1.0f );
	g_cvVertices[g_nVertexNum].v4Color = XMFLOAT4( nRed / 255.0f, nGreen / 255.0f, nBlue / 255.0f, 1.0f );
	g_nVertexNum++;

	return 0;
}

// 렌더링
HRESULT Render( void )
{
	//	int				i, j;
	// 화면 클리어
	XMFLOAT4 v4Color = XMFLOAT4( 0.0f, 0.0f, 1.0f, 1.0f );
	g_pImmediateContext->ClearRenderTargetView( g_pRTV, (float *)&v4Color );

	// 샘플러 래스터라이저 세트
	g_pImmediateContext->PSSetSamplers( 0, 1, &g_pSamplerState );
	g_pImmediateContext->RSSetState( g_pRS );

	// 그리기 설정
	UINT nStrides = sizeof( CUSTOMVERTEX );
	UINT nOffsets = 0;
	g_pImmediateContext->IASetVertexBuffers( 0, 1, &g_pD3D11VertexBuffer, &nStrides, &nOffsets );
	//    g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST );
	g_pImmediateContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_POINTLIST );
	g_pImmediateContext->IASetInputLayout( g_pInputLayout );

	// 셰이더 설정
	g_pImmediateContext->VSSetShader( g_pVertexShader, NULL, 0 );
	g_pImmediateContext->VSSetConstantBuffers( 0, 1, &g_pCBNeverChanges );
	g_pImmediateContext->PSSetShader( g_pPixelShader, NULL, 0 );

	// 그리기
	g_pImmediateContext->OMSetBlendState( NULL, NULL, 0xFFFFFFFF );
	RenderScanLine();
	//	DrawPicture( 0.0f, 0.0f, &g_tBack );

	// 표시
	FlushDrawingPictures();

	return S_OK;
}

// 엔트리 포인트
int WINAPI _tWinMain( HINSTANCE hInst, HINSTANCE, LPTSTR, int )
{
	LARGE_INTEGER nNowTime, nLastTime; // 현재와 직전 시각
	LARGE_INTEGER nTimeFreq;					 // 시간단위

	// 화면 크기
	g_nClientWidth = VIEW_WIDTH;	 // 너비
	g_nClientHeight = VIEW_HEIGHT; // 높이

	// Register the window class
	WNDCLASSEX wc = { sizeof( WNDCLASSEX ), CS_CLASSDC, MsgProc, 0L, 0L, GetModuleHandle( NULL ), NULL, NULL, NULL, NULL, _T( "D3D Sample" ), NULL };
	RegisterClassEx( &wc );

	RECT rcRect;
	SetRect( &rcRect, 0, 0, g_nClientWidth, g_nClientHeight );
	AdjustWindowRect( &rcRect, WS_OVERLAPPEDWINDOW, FALSE );
	g_hWnd = CreateWindow( _T( "D3D Sample" ), _T( "Render_4_1" ), WS_OVERLAPPEDWINDOW, 100, 20, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top, GetDesktopWindow(), NULL,
												 wc.hInstance, NULL );

	// Initialize Direct3D
	if( SUCCEEDED( InitD3D() ) && SUCCEEDED( MakeShaders() ) )
	{
		// Create the shaders
		if( SUCCEEDED( InitDrawModes() ) )
		{
			if( SUCCEEDED( InitGeometry() ) )
			{ // 지오메트리  생성

				// Show the window
				ShowWindow( g_hWnd, SW_SHOWDEFAULT );
				UpdateWindow( g_hWnd );

				//				InitCharacter();									// 캐릭터 초기화

				QueryPerformanceFrequency( &nTimeFreq ); // 시간단위
				QueryPerformanceCounter( &nLastTime );	 // 1프레임전 시각 초기화

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
					g_pSwapChain->Present( 0, 0 ); // 표시
				}
			}
		}
	}

	// Clean up everything and exit the app
	Cleanup();
	UnregisterClass( _T( "D3D Sample" ), wc.hInstance );
	return 0;
}
