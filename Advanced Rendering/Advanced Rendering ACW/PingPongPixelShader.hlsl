SamplerState Sampler : register(s0);

Texture2D colorTexture1 : register(t0);
Texture2D depthTexture1 : register(t1);

Texture2D colorTexture2 : register(t2);
Texture2D depthTexture2 : register(t3);

struct VS_OUTPUT
{
    float4 Pos : SV_Position;
    float2 TexCoord : TEXCOORD0;
};

struct PS_OUTPUT
{
    float4 Color : SV_Target;
    float Depth : SV_Depth;
};

PS_OUTPUT main(VS_OUTPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;
    
    float4 color1 = colorTexture1.Sample(Sampler, input.TexCoord);
    float4 color2 = colorTexture2.Sample(Sampler, input.TexCoord);
    
    float depth1 = depthTexture1.Sample(Sampler, input.TexCoord).r;
    float depth2 = depthTexture1.Sample(Sampler, input.TexCoord).r;
    
    if (color1.a == 0.0f && color2.a == 0.0f)
    {
        discard;
    }
    else if (color1.a != 0.0f && color2.a == 0.0f)
    {
        output.Color = color1;
        output.Depth = depth1;
    }
    else if (color1.a == 0.0f && color2.a != 0.0f)
    {
        output.Color = color2;
        output.Depth = depth2;
    }
    else
    {
        if (depth1 > depth2)
        {
            output.Color = color1;
            output.Depth = depth1;
        }
        else
        {
            output.Color = color2;
            output.Depth = depth2;
        }
    }
    
    return output;
}