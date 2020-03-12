// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix inverseModel;
    matrix view;
    matrix projection;
    float4 eyePosition;
};

// Per-pixel color data passed through the pixel shader.
struct GeometryShaderInput
{
    float4 pos : SV_Position;
};

struct PixelShaderInput
{
    float4 pos : SV_Position;
    float2 uv : TEXCOORD0;
};

static const float3 g_positions[4] =
{
    float3(-1, 1, 0),
    float3(-1, -1, 0),
    float3(1, 1, 0),
    float3(1, -1, 0)
};

static const float2 g_texCoords[4] =
{
    float2(0, 1),
    float2(0, 0),
    float2(1, 1),
    float2(1, 0)
};

[maxvertexcount(6)]
void main(point GeometryShaderInput input[1], inout TriangleStream<PixelShaderInput> OutputStream)
{
    float4 vPos = input[0].pos;
    
    float2 temp = normalize(eyePosition.xz - vPos.xz);
    float3 look = float3(temp.x, 0.0f, temp.y);
    
    float3 up = float3(0.0f, 1.0f, 0.0f);
    
    float3 right = cross(look, up);
    float quadSize = 1.0f;
    
    PixelShaderInput output = (PixelShaderInput) 0;
    
    output.pos = vPos + (float4(quadSize * g_positions[0].x * right + g_positions[0].y * float3(0.0f, 1.0f, 0.0f), 0.0));
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.uv = g_texCoords[0];
    
    OutputStream.Append(output);
    
    output.pos = vPos + (float4(quadSize * g_positions[1].x * right + g_positions[1].y * float3(0.0f, 1.0f, 0.0f), 0.0));
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.uv = g_texCoords[1];
    
    OutputStream.Append(output);
    
    output.pos = vPos + (float4(quadSize * g_positions[2].x * right + g_positions[2].y * float3(0.0f, 1.0f, 0.0f), 0.0));
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.uv = g_texCoords[2];
    
    OutputStream.Append(output);
    
    OutputStream.RestartStrip();
    
    output.pos = vPos + (float4(quadSize * g_positions[1].x * right + g_positions[1].y * float3(0.0f, 1.0f, 0.0f), 0.0));
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.uv = g_texCoords[1];
    
    OutputStream.Append(output);
    
    output.pos = vPos + (float4(quadSize * g_positions[2].x * right + g_positions[2].y * float3(0.0f, 1.0f, 0.0f), 0.0));
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.uv = g_texCoords[2];
    
    OutputStream.Append(output);
    
    output.pos = vPos + (float4(quadSize * g_positions[3].x * right + g_positions[3].y * float3(0.0f, 1.0f, 0.0f), 0.0));
    output.pos = mul(output.pos, view);
    output.pos = mul(output.pos, projection);
    output.uv = g_texCoords[3];
    
    OutputStream.Append(output);
    
    OutputStream.RestartStrip();

}