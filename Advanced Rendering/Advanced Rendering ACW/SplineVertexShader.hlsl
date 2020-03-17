struct VertexShaderInput
{
    float3 pos : POSITION;
    float3 bitangent : BITANGENT;
};

// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float3 bitangent : BITANGENT;
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output = (VertexShaderOutput) 0;
    
    output.pos = float4(input.pos, 1.0f);
    output.bitangent = input.bitangent;
    
    return output;
}