// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
    float3 pos : POSITION;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderInput
{
    float4 pos : SV_Position;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

GeometryShaderInput main(VertexShaderInput input)
{
    GeometryShaderInput output;
    
    output.pos = float4(input.pos, 1.0f);
    output.texCoord = input.texCoord;
    output.normal = input.normal;
    
    return output;
}