// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
};

struct HS_Quad_Tess_Factors
{
    float Edges[4] : SV_TessFactor;
    float Inside[2] : SV_InsideTessFactor;
};

HS_Quad_Tess_Factors ConstantHS(InputPatch<VertexShaderOutput, 4> ip)
{
    HS_Quad_Tess_Factors output;
    
    float tessAmount = 31.0f;
    output.Edges[0] = output.Edges[1] = output.Edges[2] = output.Edges[3] = tessAmount;
    output.Inside[0] = output.Inside[1] = tessAmount;
    
    return output;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHS")]

VertexShaderOutput main(InputPatch<VertexShaderOutput, 4> patch, uint i : SV_OutputControlPointID)
{
    VertexShaderOutput output;
    
    output.pos = patch[i].pos;
    
    return output;
}