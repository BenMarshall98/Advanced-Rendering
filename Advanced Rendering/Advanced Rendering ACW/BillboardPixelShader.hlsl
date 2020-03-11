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
    
    output.color = float4(input.uv, 0.0f, 1.0f);
    output.pos = input.pos;
    
    return output;
}