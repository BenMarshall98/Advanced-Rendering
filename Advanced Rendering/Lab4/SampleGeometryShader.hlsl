// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
};

struct GeometryShaderInput
{
    float4 pos : SV_Position;
    float3 color : COLOR0;
};

struct PixelShaderInput
{
    float4 pos : SV_Position;
    float3 color : COLOR0;
    float2 uv : TEXCOORD0;
};

static const float3 g_positions[4] =
{
    float3(-1, 1, 0),
    float3(-1, -1, 0),
    float3(1, 1, 0),
    float3(1, -1, 0)
};

[maxvertexcount(6)]
void main(point GeometryShaderInput input[1], inout TriangleStream<PixelShaderInput> OutputStream)
{
    float4 vPos = input[0].pos;
    vPos = mul(vPos, model);
    vPos = mul(vPos, view);

    float quadSize = 0.1f;
    
    PixelShaderInput output = (PixelShaderInput) 0;
    
    output.pos = vPos + (float4(quadSize * g_positions[0], 0.0));
    output.pos = mul(output.pos, projection);
    
    output.color = input[0].color;
    
    OutputStream.Append(output);
    
    output.pos = vPos + (float4(quadSize * g_positions[1], 0.0));
    output.pos = mul(output.pos, projection);
    
    output.color = input[0].color;
    
    OutputStream.Append(output);
    
    output.pos = vPos + (float4(quadSize * g_positions[2], 0.0));
    output.pos = mul(output.pos, projection);
    
    output.color = input[0].color;
    
    OutputStream.Append(output);
    
    OutputStream.RestartStrip();
    
    output.pos = vPos + (float4(quadSize * g_positions[1], 0.0));
    output.pos = mul(output.pos, projection);
    
    output.color = input[0].color;
    
    OutputStream.Append(output);
    
    output.pos = vPos + (float4(quadSize * g_positions[2], 0.0));
    output.pos = mul(output.pos, projection);
    
    output.color = input[0].color;
    
    OutputStream.Append(output);
    
    output.pos = vPos + (float4(quadSize * g_positions[3], 0.0));
    output.pos = mul(output.pos, projection);
    
    output.color = input[0].color;
    
    OutputStream.Append(output);
    
    OutputStream.RestartStrip();

}