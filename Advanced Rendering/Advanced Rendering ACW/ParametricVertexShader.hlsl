// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
};

VertexShaderOutput main()
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    output.pos = float4(0.0f, 0.0f, 0.0f, 1.0f);
    
    return output;
}