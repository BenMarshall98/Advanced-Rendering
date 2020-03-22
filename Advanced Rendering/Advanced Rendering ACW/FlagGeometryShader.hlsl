// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix inverseModel;
	matrix view;
	matrix projection;
	float4 eyePosition;
};

cbuffer TimeConstantBuffer : register(b5)
{
	float Time;
}

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderInput
{
	float4 pos : SV_Position;
};

struct PixelShaderInput
{
	float4 pos : SV_Position;
	float2 uv : TEXCOORD0;
};

static const float3 g_positions[4] =
{
	float3(-1, 2, 0),
	float3(-1, 0, 0),
	float3(1, 2, 0),
	float3(1, 0, 0)
};

static const float2 g_texCoords[4] =
{
	float2(0, 0),
	float2(0, 1),
	float2(1, 0),
	float2(1, 1)
};

[maxvertexcount(150)]
void main(point GeometryShaderInput input[1], inout TriangleStream<PixelShaderInput> OutputStream)
{
	float4 vPos = input[0].pos;

	float3 look = float3(1.0f, 0.0f, 0.0f);

	float3 up = float3(0.0f, 1.0f, 0.0f);

	float3 right = float3(0.0f, 0.0f, 1.0f);
	float3 quadSize = float3(10.0f, 5.0f, 10.0f);

	PixelShaderInput output = (PixelShaderInput)0;

	int tess = 25;

	for (int i = 0; i < 1; i++)
	{
		for (int j = 0; j < tess; j++)
		{
			float2 factor1 = float2(1.0f / tess * j, 1);
			float2 factor2 = float2(1.0f / tess * j, 0);
			float2 factor3 = float2(1.0f / tess * (j + 1), 1);
			float2 factor4 = float2(1.0f / tess * (j + 1), 0);

			output.pos = vPos + float4(quadSize * (factor1.x * right + factor1.y * up), 0.0f);
			output.pos.xyz += factor1.x * look * sin((factor1.x + Time) * 1.0f) * 0.5f + sin((factor1.x + Time) * 2.0f) * 0.25f + sin((factor1.x + Time) * 4.0f) * 0.125f + sin((factor1.x + Time) * 8.0f) * 0.06725f;
			output.pos = mul(output.pos, view);
			output.pos = mul(output.pos, projection);
			output.uv = factor1;
			output.uv.y *= -1;

			OutputStream.Append(output);

			output.pos = vPos + float4(quadSize * (factor2.x * right + factor2.y * up), 0.0f);
			output.pos.xyz += factor2.x *look * sin((factor2.x + Time) * 1.0f) * 0.5f + sin((factor2.x + Time) * 2.0f) * 0.25f + sin((factor2.x + Time) * 4.0f) * 0.125f + sin((factor2.x + Time) * 8.0f) * 0.06725f;
			output.pos = mul(output.pos, view);
			output.pos = mul(output.pos, projection);
			output.uv = factor2;
			output.uv.y *= -1;

			OutputStream.Append(output);

			output.pos = vPos + float4(quadSize * (factor3.x * right + factor3.y * up), 0.0f);
			output.pos.xyz += factor3.x *look * sin((factor3.x + Time) * 1.0f) * 0.5f + sin((factor3.x + Time) * 2.0f) * 0.25f + sin((factor3.x + Time) * 4.0f) * 0.125f + sin((factor3.x + Time) * 8.0f) * 0.06725f;
			output.pos = mul(output.pos, view);
			output.pos = mul(output.pos, projection);
			output.uv = factor3;
			output.uv.y *= -1;

			OutputStream.Append(output);

			OutputStream.RestartStrip();

			output.pos = vPos + float4(quadSize * (factor2.x * right + factor2.y * up), 0.0f);
			output.pos.xyz += factor2.x *look * sin((factor2.x + Time) * 1.0f) * 0.5f + sin((factor2.x + Time) * 2.0f) * 0.25f + sin((factor2.x + Time) * 4.0f) * 0.125f + sin((factor2.x + Time) * 8.0f) * 0.06725f;
			output.pos = mul(output.pos, view);
			output.pos = mul(output.pos, projection);
			output.uv = factor2;
			output.uv.y *= -1;

			OutputStream.Append(output);

			output.pos = vPos + float4(quadSize * (factor3.x * right + factor3.y * up), 0.0f);
			output.pos.xyz += factor3.x *look * sin((factor3.x + Time) * 1.0f) * 0.5f + sin((factor3.x + Time) * 2.0f) * 0.25f + sin((factor3.x + Time) * 4.0f) * 0.125f + sin((factor3.x + Time) * 8.0f) * 0.06725f;
			output.pos = mul(output.pos, view);
			output.pos = mul(output.pos, projection);
			output.uv = factor3;
			output.uv.y *= -1;

			OutputStream.Append(output);

			output.pos = vPos + float4(quadSize * (factor4.x * right + factor4.y * up), 0.0f);
			output.pos.xyz += factor4.x *look * sin((factor4.x + Time) * 1.0f) * 0.5f + sin((factor4.x + Time) * 2.0f) * 0.25f + sin((factor4.x + Time) * 4.0f) * 0.125f + sin((factor4.x + Time) * 8.0f) * 0.06725f;
			output.pos = mul(output.pos, view);
			output.pos = mul(output.pos, projection);
			output.uv = factor4;
			output.uv.y *= -1;

			OutputStream.Append(output);

			OutputStream.RestartStrip();
		}
	}
}