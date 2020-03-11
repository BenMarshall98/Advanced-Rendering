// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float2 uv : COLOR0;
};

struct PixelShaderOutput
{
    float4 color : SV_Target0;
    float4 position : SV_Target1;
};

// A pass-through function for the (interpolated) color data.
PixelShaderOutput main(VertexShaderOutput input)
{
    PixelShaderOutput output = (PixelShaderOutput) 0;
    
    output.position = input.pos;
    output.color = float4(input.uv, 1.0f, 1.0f);
    
    return output;
}