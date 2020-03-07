// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
};

// A pass-through function for the (interpolated) color data.
float4 main(VertexShaderOutput input) : SV_TARGET
{
    return float4(1.0f, 1.0f, 1.0f, 1.0f);
}