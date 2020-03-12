Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
SamplerState Sampler : register(s0);

cbuffer LightConstantBuffer : register(b2)
{
    float4 lightColor;
    float4 lightPos;
}

struct DS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float3 FragmentPos : POSITION0;
    float3 ViewPosition : POSITION1;
    float2 TexCoord : TEXCOORD0;
    float4 LightFragmentPos : POSITION2;
    float4 Position : POSITION3;
    float3x3 TBN : POSITION4;
};

struct PS_OUTPUT
{
    float4 color : SV_Target0;
    float4 pos : SV_Target1;
};

PS_OUTPUT main(DS_OUTPUT input)
{
    PS_OUTPUT output = (PS_OUTPUT) 0;
    
    float3 norm = normalTexture.Sample(Sampler, input.TexCoord).xyz;
    
    norm = 2.0f * norm - 1.0f;
    
    float3 normal = normalize(mul(norm, input.TBN));
    float3 viewDirection = normalize(input.ViewPosition - input.FragmentPos.xyz);
    
    float3 baseColor = colorTexture.Sample(Sampler, input.TexCoord).xyz;
    
    float spec = 32.0f;
    
    float3 lightDirection = normalize(lightPos.xyz - input.FragmentPos.xyz);

    float diffuse = max(dot(normal, lightDirection), 0.0f);

    float3 reflectDirection = reflect(-lightDirection, normal);

    float specular = pow(max(dot(viewDirection, reflectDirection), 0.0f), spec);
    
    output.color = float4(baseColor * 0.1f + baseColor * diffuse * lightColor.xyz + baseColor * specular * lightColor.xyz, 1.0f);
    output.pos = input.Pos;
    
    return output;
}