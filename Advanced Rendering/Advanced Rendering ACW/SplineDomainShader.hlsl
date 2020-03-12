// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix inverseModel;
    matrix view;
    matrix projection;
    float4 eyePosition;
};

struct HS_Factors
{
    float Edges[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

// Per-pixel color data passed through the pixel shader.
struct VertexShaderInput
{
    float4 pos : SV_POSITION;
};

// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float2 uv : COLOR0;
};

[domain("quad")]
VertexShaderOutput main(HS_Factors input, float2 UV : SV_DomainLocation, const OutputPatch<VertexShaderInput, 16> patch)
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    int x = floor(UV.x * 3);
    int y = floor(UV.y * 3);
    
    int index1 = x + 4 * y;
    int index2 = (x + 1) + 4 * y;
    int index3 = x + 4 * (y + 1);
    int index4 = (x + 1) + 4 * (y + 1);
    
    float2 weight = frac(UV * 3);
    
    float3 pos1 = lerp(patch[index1].pos.xyz, patch[index2].pos.xyz, weight.x);
    float3 pos2 = lerp(patch[index3].pos.xyz, patch[index4].pos.xyz, weight.x);
    
    float3 pos = lerp(pos1, pos2, weight.y);
    
    output.pos = float4(pos, 1.0f);
    
    output.pos = mul(output.pos, model);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    
    output.uv = UV;
    
    return output;
}