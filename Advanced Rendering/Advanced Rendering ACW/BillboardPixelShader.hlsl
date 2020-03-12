Texture2D colorTexture : register(t0);
SamplerState Sampler : register(s0);

struct PixelShaderInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

struct PixelShaderOutput
{
    float4 color : SV_Target0;
    float4 pos : SV_Target1;
};

PixelShaderOutput main(PixelShaderInput input)
{
    PixelShaderOutput output = (PixelShaderOutput) 0;
    
    output.color = colorTexture.Sample(Sampler, input.uv);
    
    if (output.color.a < 0.1f)
    {
        discard;
    }
    output.pos = input.pos;
    
    return output;
}