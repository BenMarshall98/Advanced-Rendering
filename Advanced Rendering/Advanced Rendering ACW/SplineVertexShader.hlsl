struct VertexShaderInput
{
    float3 pos : POSITION;
};

// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    output.pos = float4(input.pos, 1.0f);
    
    return output;
}