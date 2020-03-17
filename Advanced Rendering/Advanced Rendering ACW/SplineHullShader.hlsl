// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float3 bitangent : BITANGENT;
};

struct HS_Factors
{
    float Edges[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

HS_Factors ConstantHS(InputPatch<VertexShaderOutput, 4> ip)
{
    HS_Factors output = (HS_Factors) 0;
    
    output.Edges[0] = 32.0f;
    output.Edges[1] = 32.0f;
    output.Edges[2] = 32.0f;
    output.Edges[3] = 32.0f;
    
    output.Inside[0] = 32.0f;
    output.Inside[1] = 32.0f;
    
    return output;
}

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]

VertexShaderOutput main(InputPatch<VertexShaderOutput, 4> patch, uint i : SV_OutputControlPointID)
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    output.pos = patch[i].pos;
    output.bitangent = patch[i].bitangent;
    
    return output;
}