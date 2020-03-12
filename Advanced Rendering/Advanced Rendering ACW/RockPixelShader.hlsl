Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
SamplerState Sampler : register(s0);

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

struct PS_OUTPUT
{
    float4 color : SV_Target0;
    float4 pos : SV_Target1;
};

PS_OUTPUT main(DS_OUTPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;
    
    output.color = float4(normalTexture.Sample(Sampler, input.TexCoord).xyz, 1.0f);
    output.pos = input.Pos;
    
    return output;
}