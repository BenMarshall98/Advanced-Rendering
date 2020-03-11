// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 color : COLOR0;
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderInput
{
    float4 pos : SV_Position;
};

GeometryShaderInput main(VertexShaderInput input)
{
    GeometryShaderInput output;
    
    output.pos = float4(input.pos, 1.0f);
    
    return output;
}