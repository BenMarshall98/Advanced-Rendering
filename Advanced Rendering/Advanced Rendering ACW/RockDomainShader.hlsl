//Based on example in Frank Luna's Introduction to 3D Game Programming with DirectX11

Texture2D heightTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix inverseModel;
    matrix view;
    matrix projection;
    float4 eyePosition;
};

cbuffer TessConstantBuffer : register(b1)
{
    float tess;
    float height;
    float2 padding;
};

struct PatchTess
{
    float EdgeTess[3] : SV_TessFactor;
    float InsideTess : SV_InsideTessFactor;
};

struct HS_OUTPUT
{
    float3 Pos : POSITION0;
    float3 Normal : NORMAL0;
    float2 TexCoord : TEXCOORD0;
    float3 Tangent : TANGENT0;
    float3 BiTangent : BITANGENT0;
};

struct DS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 FragmentPos : POSITION0;
    float3 ViewPosition : POSITION1;
    float2 TexCoord : TEXCOORD0;
    float4 LightFragmentPos : POSITION2;
    float4 Position : POSITION3;
    float3x3 TBN : POSITION4;
};

[domain("tri")]
DS_OUTPUT main(PatchTess patch, float3 uvw : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 3> tri)
{
    DS_OUTPUT output = (DS_OUTPUT) 0;
    
    output.TexCoord = uvw[0] * tri[0].TexCoord;
    output.TexCoord += uvw[1] * tri[1].TexCoord;
    output.TexCoord += uvw[2] * tri[2].TexCoord;
    
    float tempHeight = height * heightTexture.SampleLevel(Sampler, output.TexCoord, 0).x;
    
    //Position
    float3 pos = uvw[0] * tri[0].Pos;
    pos += uvw[1] * tri[1].Pos;
    pos += uvw[2] * tri[2].Pos;
    
    //Normal
    float3 normal = uvw[0] * tri[0].Normal;
    normal += uvw[1] * tri[1].Normal;
    normal += uvw[2] * tri[2].Normal;    
    //FragmentPosition
    output.Pos = mul(float4(pos, 1.0f), model);
    
    output.Pos.xyz += normalize(mul(float4(normalize(normal), 1.0f), inverseModel).xyz) * tempHeight;
    
    output.FragmentPos = output.Pos.xyz;
    
    output.Pos = mul(output.Pos, view);
    output.Pos = mul(output.Pos, projection);
    
    //Tangent
    float3 tangent = uvw[0] * tri[0].Tangent;
    tangent += uvw[1] * tri[1].Tangent;
    tangent += uvw[2] * tri[2].Tangent;
    
    //BiTangent
    float3 biTangent = uvw[0] * tri[0].BiTangent;
    biTangent += uvw[1] * tri[1].BiTangent;
    biTangent += uvw[2] * tri[2].BiTangent;
    
    output.TBN = float3x3(normalize(mul(float4(normalize(tangent), 1.0f), inverseModel).xyz),
                        normalize(mul(float4(normalize(biTangent), 1.0f), inverseModel).xyz),
                        normalize(mul(float4(normalize(normal), 1.0f), inverseModel).xyz));
    
    //ViewPosition
    output.ViewPosition = eyePosition;
    
    output.Position = output.Pos;
    
    return output;
}