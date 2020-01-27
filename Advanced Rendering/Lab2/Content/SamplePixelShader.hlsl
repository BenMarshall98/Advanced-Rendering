#define NOBJECTS 3

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

struct Sphere
{
    float3 centre;
    float rad2;
    float4 color;
    float Kd, ks, kr, shininess;
};

static float4 Eye = float4(0.0, 0.0, 15.0, 1.0);
static float nearPlane = 1.0f;
static float farPlane = 1000.0f;

static float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
static float3 lightPos = float3(0.0f, 0.0f, 0.0f);
static float4 backgroundColor = float4(0.1f, 0.2f, 0.3f, 1.0f);

static float4 sphereColor_1 = float4(1.0f, 0.0f, 0.0f, 1.0f);
static float4 sphereColor_2 = float4(0.0f, 1.0f, 0.0f, 1.0f);
static float4 sphereColor_3 = float4(1.0f, 0.0f, 1.0f, 1.0f);
static float shininess = 40.0f;

static Sphere objects[NOBJECTS] =
{
    { 0.0f, 0.0f, -3.0f, 1.0f, sphereColor_1, 0.3f, 0.5f, 0.7f, shininess },
    { -3.0f, 0.0f, -2.0f, 1.0f, sphereColor_2, 0.5f, 0.7f, 0.4f, shininess },
    { 3.0f, 0.0f, -2.0f, 1.0f, sphereColor_3, 0.5f, 0.3f, 0.3f, shininess }
};

float SphereIntersect(Sphere s, Ray ray, out bool hit);
float3 SphereNormal(Sphere s, float3 pos);
float3 NearestHit(Ray ray, out int hitobj, out bool anyhit);
float4 Phong(float3 n, float3 l, float3 v, float shininess, float4 diffuseColor, float4 specularColor);
float4 Shade(float3 hitPos, float3 normal, float3 viewDir, int hitobj, float lightIntensity);
float4 RayTracing(Ray eyeray);

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
    float zoom = 2.5f;
    float2 xy = zoom * input.canvasCoord;
    float distEye2Canvas = nearPlane;
    float3 PixelPos = float3(xy, -distEye2Canvas);
    
    Ray eyeray;
    eyeray.o = Eye.xyz;
    eyeray.d = normalize(PixelPos - Eye.xyz);
    
    return RayTracing(eyeray);
}

float SphereIntersect(Sphere s, Ray ray, out bool hit)
{
    float t;
    float3 v = s.centre - ray.o;
    float A = dot(v, ray.d);
    float B = dot(v, v) - A * A;
    
    float R = s.rad2;
    
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

float4 RayTracing(Ray ray)
{
    int hitobj;
    bool hit = false;
    float3 n;
    float4 c = (float4) 0;
    float lightIntensity = 1.0f;
    
    float3 i = NearestHit(ray, hitobj, hit);

    for (int depth = 1; depth < 5; depth++)
    {
        if (hit)
        {
            n = SphereNormal(objects[hitobj], i);
            c += Shade(i, n, ray.d, hitobj, lightIntensity);
            
            lightIntensity *= objects[hitobj].kr;
            ray.o = i;
            ray.d = reflect(ray.d, n);
            i = NearestHit(ray, hitobj, hit);
        }
        else
        {
            c += backgroundColor / depth / depth;
        }
    }
    
    return c;
}

float3 SphereNormal(Sphere s, float3 pos)
{
    return normalize(pos - s.centre);
}

float3 NearestHit(Ray ray, out int hitobj, out bool anyhit)
{
    float mint = farPlane;
    hitobj = -1;
    anyhit = false;
    for (int i = 0; i < NOBJECTS; i++)
    {
        bool hit = false;
        float t = SphereIntersect(objects[i], ray, hit);
        if (hit)
        {
            if (t < mint)
            {
                hitobj = i;
                mint = t;
                anyhit = true;
            }
        }
    }
    
    return ray.o + ray.d * mint;
}

float4 Phong(float3 n, float3 l, float3 v, float shininess, float4 diffuseColor, float4 specularColor)
{
    float NdotL = dot(n, l);
    float diff = saturate(NdotL);
    float3 r = reflect(l, n);
    float spec = pow(saturate(dot(v, r)), shininess) * (NdotL > 0.0);
    return diff * diffuseColor + spec * specularColor;
}

float4 Shade(float3 hitPos, float3 normal, float3 viewDir, int hitobj, float lightIntensity)
{
    float3 lightDir = normalize(lightPos - hitPos);
    
    float4 diff = objects[hitobj].color * objects[hitobj].Kd;
    float4 spec = objects[hitobj].color * objects[hitobj].ks;
    
    return lightColor * lightIntensity * Phong(normal, lightDir, viewDir, objects[hitobj].shininess, diff, spec);
}