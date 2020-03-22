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
    float3 fragPos : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
};

[domain("quad")]
VertexShaderOutput main(HS_Factors input, float2 UV : SV_DomainLocation)
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    float pi = 3.14159265359f;
    
    float radius1 = 1.0f;
    float radius2 = 2.0f;
    float radius3 = 3.0f;
    
    float x = radius1 * cos(UV.x * pi * 2.0f) * sin(UV.y * pi * 2.0f);
    float y = radius2 * sin(UV.x * pi * 2.0f) * sin(UV.y * pi * 2.0f);
    float z = radius3 * cos(UV.y * pi * 2.0f);
    
    output.pos = float4(x, y, z, 1.0f);
    
    output.normal = normalize(output.pos.xyz);
    
    output.normal = normalize(mul(float4(output.normal, 1.0f), matrix(1 / (radius1 * radius1), 0, 0, 0, 0, 1 / (radius2 * radius2), 0, 0, 0, 0, 1 / (radius3 * radius3), 0, 0, 0, 0, 1)).xyz);
    
    output.pos = mul(output.pos, model);
    
    output.fragPos = output.pos.xyz;
    
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.uv = UV;
    
    return output;
}