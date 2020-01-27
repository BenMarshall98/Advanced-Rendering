// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
    float4 pos : SV_POSITION;
    float2 canvasCoord : TEXCOORD0;
};

struct Ray
{
    float3 o;
    float3 d;
};

static float4 Eye = float4(0.0, 0.0, 15.0, 1.0);
static float nearPlane = 1.0f;
static float farPlane = 1000.0f;

static float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
static float3 lightPos = float3(0.0f, 100.0f, 0.0f);
static float4 backgroundColor = float4(0.1f, 0.2f, 0.3f, 1.0f);

float SphereIntersect(Ray ray, out bool hit);
float4 RayCasting(Ray eyeray);

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
    float zoom = 5.0f;
    float2 xy = zoom * input.canvasCoord;
    float distEye2Canvas = nearPlane;
    float3 PixelPos = float3(xy, -distEye2Canvas);
    
    Ray eyeray;
    eyeray.o = Eye.xyz;
    eyeray.d = normalize(PixelPos - Eye.xyz);
    
    return RayCasting(eyeray);
}

float SphereIntersect(Ray ray, out bool hit)
{
    float t;
    float3 v = -ray.o;
    float A = dot(v, ray.d);
    float B = dot(v, v) - A * A;
    
    float R = 2;
    
    if (B > R * R)
    {
        hit = false;
        t = farPlane;
    }
    else
    {
        float disc = sqrt(R * R - B);
        t = A - disc;
        if (t < 0.0)
        {
            hit = false;
        }
        else
        {
            hit = true;
        }
    }
    return t;
}

float4 RayCasting(Ray eyeray)
{
    bool hit = false;
    float t = SphereIntersect(eyeray, hit);
    float3 interP = eyeray.o + t * normalize(eyeray.d);
    
    float4 RTColor = (float4) 0;
    
    if (!hit)
    {
        RTColor = backgroundColor;
    }
    else
    {
        float3 C = lightColor.rgb;
        float3 N = normalize(interP);
        float3 L = normalize(lightPos - interP);
        float3 V = normalize(Eye.xyz - interP);
        float3 R = reflect(-L, N);
        float r = max(0.5 * dot(N, L), 0.2);
        r += pow(max(0.1, dot(R, V)), 50.0);
        RTColor = float4(1.5f * r * C, 1.0f);
    }
    
    return RTColor;
}