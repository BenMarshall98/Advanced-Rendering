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
};

[domain("quad")]
VertexShaderOutput main(HS_Factors input, float2 UV : SV_DomainLocation)
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    float3 vPos1 = (1.0f - UV.y) * QuadPos[0].xyz + UV.y * QuadPos[1].xyz;
    float3 vPos2 = (1.0f - UV.y) * QuadPos[2].xyz + UV.y * QuadPos[3].xyz;
    
    float3 uvPos = (1.0f - UV.x) * vPos1 + UV.x * vPos2;
    
    output.pos = float4(0.6f * uvPos, 1.0f);
    
    return output;
}