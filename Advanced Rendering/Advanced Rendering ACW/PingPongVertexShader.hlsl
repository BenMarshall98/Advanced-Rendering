struct VS_INPUT
{
    float3 Pos : POSITION;
    float3 TexCoord : COLOR0;
};

struct VS_OUTPUT
{
    float4 Pos : SV_Position;
    float2 TexCoord : TEXCOORD0;
};

VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT) 0;
    output.Pos = float4(sign(input.Pos.xz), 0.0f, 1.0f);
    output.TexCoord = input.TexCoord.yz;
    
    return output;
}