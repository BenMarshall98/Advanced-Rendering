// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix inverseModel;
    matrix view;
    matrix projection;
    float4 eyePosition;
};

static float3 QuadPos[4] =
{
    float3(-1, 1, 0),
    float3(-1, -1, 0),
    float3(1, 1, 0),
    float3(1, -1, 0)
};

struct HS_Factors
{
    float Edges[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float2 uv : COLOR0;
};

[domain("quad")]
VertexShaderOutput main(HS_Factors input, float2 UV : SV_DomainLocation)
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    float pi = 3.14159265359f;
    
    float radius1 = 3.0f;
    float radius2 = 1.0f;
    
    float x = (radius1 + radius2 * cos(UV.x * pi * 2.0f)) * cos(UV.y * pi * 2.0f);
    float y = (radius1 + radius2 * cos(UV.x * pi * 2.0f)) * sin(UV.y * pi * 2.0f);
    float z = radius2 * sin(UV.x * pi * 2.0f);
    
    output.pos = float4(x, y, z, 1.0f);
    
    output.pos = mul(output.pos, model);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    
    output.uv = UV;
    
    return output;
}