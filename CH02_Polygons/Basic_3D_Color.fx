// ---------------------------------------------------------
// Basic_3D_Color.fx
// Simple3D���̴�(������ ��ĥ)
// ---------------------------------------------------------

// ��� ����
cbuffer cbNeverChanges : register( b0 ) { matrix View; };

// VertexShader�Է�����
struct VS_INPUT
{
	float4 v4Position : POSITION; // ��ġ
	float4 v4Color : COLOR;				// ��
};

// VertexShader�������
struct VS_OUTPUT
{
	float4 v4Position : SV_POSITION; // ��ġ
	float4 v4Color : COLOR;					 // ��
};

// ���� ���̴�
VS_OUTPUT VS( VS_INPUT Input )
{
	VS_OUTPUT Output;

	Output.v4Position = mul( Input.v4Position, View );
	Output.v4Color = Input.v4Color;

	return Output;
}

// �ȼ� ���̴�
float4 PS( VS_OUTPUT Input ) : SV_TARGET { return Input.v4Color; }
