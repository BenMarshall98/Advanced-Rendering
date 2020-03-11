//Based on example in Frank Luna's Introduction to 3D Game Programming with DirectX11

struct PatchTess
{
    float EdgeTess[3] : SV_TessFactor;
    float InsideTess : SV_InsideTessFactor;
};

struct VS_OUTPUT
{
    float3 Pos : POSITION0;
    float3 Normal : NORMAL0;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TANGENT0;
    float3 BiTangent : BITANGENT0;
};

PatchTess ConstantHS(InputPatch<VS_OUTPUT, 3> inPatch, uint patchID : SV_PrimitiveID)
{
    PatchTess outPatch;
    outPatch.EdgeTess[0] = 1;
    outPatch.EdgeTess[1] = 1;
    outPatch.EdgeTess[2] = 1;
    outPatch.InsideTess = 1;
    
    return outPatch;
}

struct HS_OUTPUT
{
    float3 Pos : POSITION0;
    float3 Normal : NORMAL0;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TANGENT0;
    float3 BiTangent : BITANGENT0;
};

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ConstantHS")]
[maxtessfactor(64.0f)]

HS_OUTPUT main(InputPatch<VS_OUTPUT, 3> patch, uint i : SV_OutputControlPointID, uint patchID : SV_PrimitiveID)
{
    HS_OUTPUT output = (HS_OUTPUT) 0;
    
    output.Pos = patch[i].Pos;
    output.Normal = patch[i].Normal;
    output.TexCoord = patch[i].TexCoord;
    output.Tangent = patch[i].Tangent;
    output.BiTangent = patch[i].BiTangent;
    
    return output;
}