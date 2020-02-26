#define SOBJECTS 3
#define TOBJECTS 4
#define COBJECTS 6
#define POBJECTS 1
#define EPSILON 0.005f

// A constant buffer that stores the three basic column-major matrices for composing geometry.
cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
    matrix model;
    matrix view;
    matrix projection;
    float4 eyePosition;
};

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

struct Plane
{
    float3 centre;
    float3 normal;
    float4 color;
    float Kd, ks, kr, shininess;
};

struct Triangle
{
    float3 pointA;
    float3 pointB;
    float3 pointC;
    float4 color;
    float Kd, ks, kr, shininess;
};

static float nearPlane = 1.0f;
static float farPlane = 1000.0f;

static float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
static float3 lightPos = float3(0.0f, 5.0f, 0.0f);
static float4 backgroundColor = float4(0.1f, 0.2f, 0.3f, 1.0f);

static float4 sphereColor_1 = float4(1.0f, 0.0f, 0.0f, 1.0f);
static float4 sphereColor_2 = float4(0.0f, 1.0f, 0.0f, 1.0f);
static float4 sphereColor_3 = float4(0.0f, 0.0f, 1.0f, 1.0f);
static float shininess = 40.0f;

static Sphere sphereObjects[SOBJECTS] =
{
    { 2.0f, -2.0f, 2.0f, 1.0f, sphereColor_1, 0.3f, 0.5f, 0.4f, shininess },
    { 2.0f, 0.0f, -2.0f, 0.5f, sphereColor_2, 0.5f, 0.7f, 0.3f, shininess },
    { -2.0f, 2.0f, 2.0f, 0.25f, sphereColor_3, 0.5f, 0.3f, 0.2f, shininess }
};

static Plane planeObjects[POBJECTS] =
{
    { 0.0f, -3.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.5f, 0.3f, 0.1f, shininess }
};

static Triangle triangleObjects[TOBJECTS] =
{
    {
       -1.0f, -1.0f, -1.0f,
       1.0f, -1.0f, -1.0f,
       0.0f, 1.0f, 0.0f,
       1.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.3f, 0.1f, shininess
    },
    {
        -1.0f, -1.0f, 1.0f,
       1.0f, -1.0f, 1.0f,
       0.0f, 1.0f, 0.0f,
       1.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.3f, 0.1f, shininess
    },
    {
       -1.0f, -1.0f, -1.0f,
       -1.0f, -1.0f, 1.0f,
       0.0f, 1.0f, 0.0f,
       1.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.3f, 0.1f, shininess
    },

    {
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.5f, 0.3f, 0.1f,
        shininess

    }
};

float SphereIntersect(Sphere s, Ray ray, out bool hit);
float PlaneIntersect(Plane p, Ray ray, out bool hit);
float TriangleIntersect(Triangle t, Ray ray, out bool hit);
float3 SphereNormal(Sphere s, float3 pos);
float3 TriangleNormal(Triangle t);
float3 NearestHit(Ray ray, out int hitobj, out bool anyhit);
float4 Phong(float3 n, float3 l, float3 v, float shininess, float4 diffuseColor, float4 specularColor);
float4 SphereShade(float3 hitPos, float3 normal, float3 viewDir, int hitobj, float lightIntensity);
float4 PlaneShade(float3 hitPos, float3 normal, float3 viewDir, int hitObj, float lightIntensity);
float4 TriangleShade(float3 hitPos, float3 normal, float3 viewDir, int hitObj, float lightIntensity);
float Shadow(float3 hitPos, float3 lightPos);
float4 RayTracing(Ray eyeray);

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
    float zoom = 1.0f;
    float2 xy = zoom * input.canvasCoord;
    float distEye2Canvas = nearPlane;
    float3 PixelPos = float3(xy, -distEye2Canvas);

    float3 xaxis = view._m00_m10_m20;
    float3 yaxis = view._m01_m11_m21;
    float3 zaxis = view._m02_m12_m22;
    
    Ray eyeray;
    eyeray.o = eyePosition.xyz;
    eyeray.d = normalize(PixelPos.x * xaxis + PixelPos.y * yaxis + PixelPos.z * zaxis);
    
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

float PlaneIntersect(Plane p, Ray ray, out bool hit)
{
    float c = dot(ray.d, p.normal);
    
    float t = farPlane;
    
    if (abs(c) < EPSILON)
    {
        hit = false;
    }
    else
    {
        t = dot(p.centre - ray.o, p.normal) / c;
        
        if (t < EPSILON)
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

float TriangleIntersect(Triangle tri, Ray ray, out bool hit)
{
    float3 normal = TriangleNormal(tri);
    
    float c = dot(ray.d, normal);
    
    float t = farPlane;
    
    if (abs(c) < EPSILON)
    {
        hit = false;
    }
    else
    {
        t = dot(tri.pointA - ray.o, normal) / c;
        
        if (t < EPSILON)
        {
            hit = false;
        }
        else
        {
            hit = true;
        }
        
        float3 p = ray.o + t * ray.d;
        
        float3 edge0 = tri.pointB - tri.pointA;
        float3 edge1 = tri.pointC - tri.pointB;
        float3 edge2 = tri.pointA - tri.pointC;
        
        float3 C0 = p - tri.pointA;
        float3 C1 = p - tri.pointB;
        float3 C2 = p - tri.pointC;
        
        if (dot(normal, cross(edge0, C0)) < 0)
        {
            hit = false;
        }
        
        if (dot(normal, cross(edge1, C1)) < 0)
        {
            hit = false;
        }
        
        if (dot(normal, cross(edge2, C2)) < 0)
        {
            hit = false;
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
        if (hit && hitobj < SOBJECTS)
        {
            n = SphereNormal(sphereObjects[hitobj], i);
            c += SphereShade(i, n, ray.d, hitobj, lightIntensity);
            
            lightIntensity *= sphereObjects[hitobj].kr;
            ray.o = i;
            ray.d = reflect(ray.d, n);
            i = NearestHit(ray, hitobj, hit);
        }
        else if (hit && hitobj < SOBJECTS + POBJECTS)
        {
            int object = hitobj - SOBJECTS;
            n = planeObjects[object].normal;
            c += PlaneShade(i, n, ray.d, object, lightIntensity);
            
            lightIntensity *= planeObjects[object].kr;
            ray.o = i;
            ray.d = reflect(ray.d, n);
            i = NearestHit(ray, hitobj, hit);
        }
        else if (hit && hitobj < SOBJECTS + POBJECTS + TOBJECTS)
        {
            int object = hitobj - SOBJECTS - POBJECTS;
            n = TriangleNormal(triangleObjects[object]);
            c += TriangleShade(i, n, ray.d, object, lightIntensity);
            
            lightIntensity *= triangleObjects[object].kr;
            ray.o = i;
            ray.d = reflect(ray.d, n);
            i = NearestHit(ray, hitobj, hit);
        }
        else if (!hit && depth == 1)
        {
            c = backgroundColor;
        }
    }
    
    return c;
}

float3 SphereNormal(Sphere s, float3 pos)
{
    return normalize(pos - s.centre);
}

float3 TriangleNormal(Triangle tri)
{
    float3 AB = tri.pointB - tri.pointA;
    float3 AC = tri.pointC - tri.pointA;
    
    return normalize(cross(AB, AC));
}

float3 NearestHit(Ray ray, out int hitobj, out bool anyhit)
{
    float mint = farPlane;
    hitobj = -1;
    anyhit = false;
    for (int i = 0; i < SOBJECTS; i++)
    {
        bool hit = false;
        float t = SphereIntersect(sphereObjects[i], ray, hit);
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
    
    int newHit = SOBJECTS;
    
    for (i = 0; i < POBJECTS; i++)
    {
        bool hit = false;
        float t = PlaneIntersect(planeObjects[i], ray, hit);
        if (hit)
        {
            if (t < mint)
            {
                hitobj = newHit + i;
                mint = t;
                anyhit = true;
            }
        }
    }
    
    newHit = SOBJECTS + POBJECTS;
    
    for (i = 0; i < TOBJECTS; i++)
    {
        bool hit = false;
        float t = TriangleIntersect(triangleObjects[i], ray, hit);
        if (hit)
        {
            if (t < mint)
            {
                hitobj = newHit + i;
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

float4 SphereShade(float3 hitPos, float3 normal, float3 viewDir, int hitobj, float lightIntensity)
{
    float3 lightDir = normalize(lightPos - hitPos);
    
    float4 diff = sphereObjects[hitobj].color * sphereObjects[hitobj].Kd;
    float4 spec = sphereObjects[hitobj].color * sphereObjects[hitobj].ks;
    float4 amb = sphereObjects[hitobj].color * 0.1f;
    
    float shadow = 1.0f - Shadow(hitPos, lightPos);
    
    return lightColor * lightIntensity * ((shadow * Phong(normal, lightDir, viewDir, sphereObjects[hitobj].shininess, diff, spec)) + amb);
}

float4 PlaneShade(float3 hitPos, float3 normal, float3 viewDir, int hitobj, float lightIntensity)
{
    float3 lightDir = normalize(lightPos - hitPos);
    
    float4 color = planeObjects[hitobj].color;
    
    if (frac((floor(hitPos.x) + floor(hitPos.z)) * 0.5f) * 2)
    {
        color = float4(0.5f, 0.5f, 0.5f, 1.0f);
    }
    
    float4 diff = color * planeObjects[hitobj].Kd;
    float4 spec = color * planeObjects[hitobj].ks;
    float4 amb = color * 0.1f;
    
    float shadow = 1.0f - Shadow(hitPos, lightPos);
    
    return lightColor * lightIntensity * ((shadow * Phong(normal, lightDir, viewDir, planeObjects[hitobj].shininess, diff, spec)) + amb);
}

float4 TriangleShade(float3 hitPos, float3 normal, float3 viewDir, int hitobj, float lightIntensity)
{
    float3 lightDir = normalize(lightPos - hitPos);
    
    float4 color = triangleObjects[hitobj].color;
    
    float4 diff = color * triangleObjects[hitobj].Kd;
    float4 spec = color * triangleObjects[hitobj].ks;
    float4 amb = color * 0.1f;
    
    float shadow = 1.0f - Shadow(hitPos, lightPos);
    
    return lightColor * lightIntensity * ((shadow * Phong(normal, lightDir, viewDir, triangleObjects[hitobj].shininess, diff, spec)) + amb);
}

float Shadow(float3 hitPos, float3 lightPos)
{
    Ray ray = (Ray) 0;
    ray.d = normalize(lightPos - hitPos);
    ray.o = hitPos + ray.d * EPSILON;
    
    float anyHit = 0.0f;
    
    for (int i = 0; i < SOBJECTS; i++)
    {
        bool hit;
        float t = SphereIntersect(sphereObjects[i], ray, hit);
        
        if (hit && t < length(hitPos - lightPos))
        {
            anyHit = 1.0f;
        }
    }
    
    for (i = 0; i < TOBJECTS; i++)
    {
        bool hit;
        float t = TriangleIntersect(triangleObjects[i], ray, hit);
        
        if (hit && t < length(hitPos - lightPos))
        {
            anyHit = 1.0f;
        }
    }
    
    return anyHit;
}