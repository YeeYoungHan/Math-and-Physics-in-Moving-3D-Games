// ---------------------------------------------------------
// Basic_2D.fx
// Simple2D���̴�(�ؽ�ó����)
// ---------------------------------------------------------


// �ؽ�ó
Texture2D Tex2D : register( t0 );		// �ؽ�ó

// �ؽ�ó ���÷�
SamplerState MeshTextureSampler : register( s0 )
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

// ��� ����
cbuffer cbNeverChanges : register( b0 )
{
    matrix View;
};


// VertexShader�Է�����
struct VS_INPUT {
    float4 v4Position	: POSITION;		// ��ġ
    float4 v4Color		: COLOR;		// ��
//    float2 v2Tex		: TEXTURE;		// �ؽ�ó ��ǥ
};

// VertexShader�������
struct VS_OUTPUT {
    float4 v4Position	: SV_POSITION;	// ��ġ
    float4 v4Color		: COLOR;		// ��
//    float2 v2Tex		: TEXTURE;		// �ؽ�ó ��ǥ
};

// ���� ���̴�
VS_OUTPUT VS( VS_INPUT Input )
{
    VS_OUTPUT	Output;

    Output.v4Position = mul( Input.v4Position, View );
    Output.v4Color = Input.v4Color;
//    Output.v2Tex = Input.v2Tex;

    return Output;
}

// �ȼ� ���̴�
float4 PS( VS_OUTPUT Input ) : SV_TARGET {
    return Input.v4Color;
}
