Texture2D colorTexture : register(t0);
SamplerState Sampler : register(s0);

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix inverseModel;
    matrix view;
    matrix projection;
    float4 eyePosition;
};

cbuffer LightConstantBuffer : register(b2)
{
    float4 lightColor;
    float4 lightPos;
}

// Per-pixel color data passed through the pixel shader.
struct VertexShaderOutput
{
    float4 pos : SV_POSITION;
    float3 fragPos : POSITION0;
    float3 normal : NORMAL0;
    float2 uv : TEXCOORD0;
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

    float3 pos = input.fragPos;
    float3 normal = input.normal;

    float3 lightDir = normalize(lightPos.xyz - pos);

    float4 color = colorTexture.Sample(Sampler, input.uv);

    float4 diffuseColor = color;
    float4 specularColor = color;
    float4 amb = color * 0.1f;
    
    float shininess = 32.0f;
    
    float3 viewDir = normalize(eyePosition.xyz - pos);
    
    float NdotL = dot(normal, lightDir);
    float diff = saturate(NdotL);
    float3 r = reflect(lightDir, normal);
    float spec = pow(saturate(dot(viewDir, r)), shininess) * (NdotL > 0.0);
    float4
    phong = diff * diffuseColor + spec * specularColor;

    output.color = float4((lightColor * (phong + amb)).xyz, 1.0f);
    
    return output;
}