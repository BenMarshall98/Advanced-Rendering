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
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

struct PixelShaderInput
{
    float4 pos : SV_Position;
    float3 fragPos : POSITION0;
    float2 texCoord : TEXCOORD0;
    float3 normal : NORMAL0;
};

float4x4 inverse(float4x4 m);

[maxvertexcount(60)]
void main(triangle GeometryShaderInput input[3], inout TriangleStream<PixelShaderInput> OutputStream)
{
    float4 vPos = input[0].pos;
    
    float2 temp = normalize(eyePosition.xz - vPos.xz);
    float3 look = float3(temp.x, 0.0f, temp.y);
    
    float3 up = float3(0.0f, 1.0f, 0.0f);
    
    float3 right = cross(look, up);
    float quadSize = 1.0f;
    
    const int numSculptures = 20;
    
    const float3 positions[numSculptures] =
    {
        float3(7.5, 10, 5),
        float3(7.5, 10, 15),
        float3(7.5, 10, 25),
        float3(7.5, 10, 35),
        float3(7.5, 10, 45),
        float3(7.5, 10, 55),
        float3(7.5, 10, 65),
        float3(7.5, 10, 75),
        float3(7.5, 10, 85),
        float3(7.5, 10, 95),
        float3(-7.5, 10, 5),
        float3(-7.5, 10, 15),
        float3(-7.5, 10, 25),
        float3(-7.5, 10, 35),
        float3(-7.5, 10, 45),
        float3(-7.5, 10, 55),
        float3(-7.5, 10, 65),
        float3(-7.5, 10, 75),
        float3(-7.5, 10, 85),
        float3(-7.5, 10, 95)
    };
    
    PixelShaderInput output = (PixelShaderInput) 0;
    
    for (int i = 0; i < numSculptures; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            float4 vPos = input[j].pos;
            vPos.xyz *= 0.2f;
            vPos.xyz += positions[i];
    
            float2 temp = normalize(eyePosition.xz - vPos.xz);
            float3 look = normalize(float3(temp.x, 0.0f, temp.y));
    
            float3 up = float3(0.0f, 1.0f, 0.0f);
    
            float3 right = normalize(cross(look, up));
            
            vPos.xyz -= positions[i];
            
            matrix tempMat = matrix(float4(right, 0.0f), float4(up, 0.0f), float4(look, 0.0f), float4(0.0f, 0.0f, 0.0f, 1.0f));
            
            output.pos = mul(vPos, transpose(tempMat)) + float4(positions[i], 0.0f);
            output.fragPos = output.pos.xyz;
            output.pos = mul(output.pos, view);
            output.pos = mul(output.pos, projection);
            output.texCoord = input[j].texCoord;
            output.normal = normalize(mul(float4(input[j].normal, 1.0f), inverse(tempMat)).xyz);
            
            OutputStream.Append(output);
        }
        
        OutputStream.RestartStrip();
    }
    
    //output.pos = vPos + (float4(quadSize * g_positions[1].x * right + g_positions[1].y * float3(0.0f, 1.0f, 0.0f), 0.0));
}

//Code from https://gist.github.com/mattatz/86fff4b32d198d0928d0fa4ff32cf6fa

float4x4 inverse(float4x4 m)
{
    float n11 = m[0][0], n12 = m[1][0], n13 = m[2][0], n14 = m[3][0];
    float n21 = m[0][1], n22 = m[1][1], n23 = m[2][1], n24 = m[3][1];
    float n31 = m[0][2], n32 = m[1][2], n33 = m[2][2], n34 = m[3][2];
    float n41 = m[0][3], n42 = m[1][3], n43 = m[2][3], n44 = m[3][3];

    float t11 = n23 * n34 * n42 - n24 * n33 * n42 + n24 * n32 * n43 - n22 * n34 * n43 - n23 * n32 * n44 + n22 * n33 * n44;
    float t12 = n14 * n33 * n42 - n13 * n34 * n42 - n14 * n32 * n43 + n12 * n34 * n43 + n13 * n32 * n44 - n12 * n33 * n44;
    float t13 = n13 * n24 * n42 - n14 * n23 * n42 + n14 * n22 * n43 - n12 * n24 * n43 - n13 * n22 * n44 + n12 * n23 * n44;
    float t14 = n14 * n23 * n32 - n13 * n24 * n32 - n14 * n22 * n33 + n12 * n24 * n33 + n13 * n22 * n34 - n12 * n23 * n34;

    float det = n11 * t11 + n21 * t12 + n31 * t13 + n41 * t14;
    float idet = 1.0f / det;

    float4x4 ret;

    ret[0][0] = t11 * idet;
    ret[0][1] = (n24 * n33 * n41 - n23 * n34 * n41 - n24 * n31 * n43 + n21 * n34 * n43 + n23 * n31 * n44 - n21 * n33 * n44) * idet;
    ret[0][2] = (n22 * n34 * n41 - n24 * n32 * n41 + n24 * n31 * n42 - n21 * n34 * n42 - n22 * n31 * n44 + n21 * n32 * n44) * idet;
    ret[0][3] = (n23 * n32 * n41 - n22 * n33 * n41 - n23 * n31 * n42 + n21 * n33 * n42 + n22 * n31 * n43 - n21 * n32 * n43) * idet;

    ret[1][0] = t12 * idet;
    ret[1][1] = (n13 * n34 * n41 - n14 * n33 * n41 + n14 * n31 * n43 - n11 * n34 * n43 - n13 * n31 * n44 + n11 * n33 * n44) * idet;
    ret[1][2] = (n14 * n32 * n41 - n12 * n34 * n41 - n14 * n31 * n42 + n11 * n34 * n42 + n12 * n31 * n44 - n11 * n32 * n44) * idet;
    ret[1][3] = (n12 * n33 * n41 - n13 * n32 * n41 + n13 * n31 * n42 - n11 * n33 * n42 - n12 * n31 * n43 + n11 * n32 * n43) * idet;

    ret[2][0] = t13 * idet;
    ret[2][1] = (n14 * n23 * n41 - n13 * n24 * n41 - n14 * n21 * n43 + n11 * n24 * n43 + n13 * n21 * n44 - n11 * n23 * n44) * idet;
    ret[2][2] = (n12 * n24 * n41 - n14 * n22 * n41 + n14 * n21 * n42 - n11 * n24 * n42 - n12 * n21 * n44 + n11 * n22 * n44) * idet;
    ret[2][3] = (n13 * n22 * n41 - n12 * n23 * n41 - n13 * n21 * n42 + n11 * n23 * n42 + n12 * n21 * n43 - n11 * n22 * n43) * idet;

    ret[3][0] = t14 * idet;
    ret[3][1] = (n13 * n24 * n31 - n14 * n23 * n31 + n14 * n21 * n33 - n11 * n24 * n33 - n13 * n21 * n34 + n11 * n23 * n34) * idet;
    ret[3][2] = (n14 * n22 * n31 - n12 * n24 * n31 - n14 * n21 * n32 + n11 * n24 * n32 + n12 * n21 * n34 - n11 * n22 * n34) * idet;
    ret[3][3] = (n12 * n23 * n31 - n13 * n22 * n31 + n13 * n21 * n32 - n11 * n23 * n32 - n12 * n21 * n33 + n11 * n22 * n33) * idet;

    return ret;
}