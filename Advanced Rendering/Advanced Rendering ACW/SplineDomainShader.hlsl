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
    float3 bitangent : BITANGENT;
};

// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float2 uv : COLOR0;
};

float3 HermiteCurve(float3 p1, float3 t1, float3 p2, float3 t2, float w);
float3 RotateY(float3 pos, float angle);

[domain("quad")]
VertexShaderOutput main(HS_Factors input, float2 UV : SV_DomainLocation, const OutputPatch<VertexShaderInput, 4> patch)
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    const float PI = 3.14159265359;
    
    int x = floor(UV.x * 3);
    
    int index1 = x;
    int index2 = (x + 1);
    
    float2 weight = float2(frac(UV.x * 3), UV.y);
    
    float3 pos = HermiteCurve(patch[index1].pos.xyz, patch[index1].bitangent, patch[index2].pos.xyz, patch[index2].bitangent, weight.x);
    //float3 pos = lerp(patch[index1].pos.xyz, patch[index2].pos.xyz, weight.x);
    
    pos = RotateY(pos, weight.y * PI * 2.0f);
    
    output.pos = float4(pos, 1.0f);
    
    output.pos = mul(output.pos, model);
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    
    output.uv = UV;
    
    return output;
}

float3 HermiteCurve(float3 p1, float3 t1, float3 p2, float3 t2, float w)
{
    float b0 = 2 * pow(w, 3) - 3 * pow(w, 2) + 1;
    float b1 = pow(w, 3) - 2 * pow(w, 2) + w;
    float b2 = -2 * pow(w, 3) + 3 * pow(w, 2);
    float b3 = pow(w, 3) - pow(w, 2);
    
    return p1 * b0 + t1 * b1 + b2 * p2 + b3 * t2;
}

float3 RotateY(float3 pos, float angle)
{
    return mul(pos, float3x3(cos(angle), 0, sin(angle), 0, 1, 0, -sin(angle), 0, cos(angle)));
}