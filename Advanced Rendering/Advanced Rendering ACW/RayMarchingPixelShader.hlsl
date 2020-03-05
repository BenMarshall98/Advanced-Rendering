#define EPSILON 0.005f
#define MAX_MARCHING_STEPS 100

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

struct PixelShaderOutput
{
    float4 color : SV_Target0;
    float4 position : SV_Target1;
};

struct Ray
{
	float3 o;
	float3 d;
};

struct Object
{
	float dist;
	float4 color;
};

static float nearPlane = 1.0f;
static float farPlane = 1000.0f;

static float4 lightColor = float4(1.0f, 1.0f, 1.0f, 1.0f);
static float3 lightPos = float3(0.0f, 5.0f, 0.0f);
static float4 backgroundColor = float4(0.1f, 0.2f, 0.3f, 1.0f);

//Shape Distance Functions
float sdSphere(float3 position, float radius);
float sdBox(float3 position, float3 size);
float sdRoundBox(float3 position, float3 size, float radius);
float sdPlane(float3 position, float4 normal);
float sdHexPrism(float3 position, float3 size);
float sdTriPrism(float3 position, float2 size);
float sdCapsule(float3 position, float3 startPoint, float3 endPoint, float radius);
float sdVerticalCapsule(float3 position, float height, float radius);
float sdCylinder(float3 position, float3 size);
float sdCappedCylinder(float3 position, float height, float radius);
float sdCappedCylinder(float3 position, float3 startPoint, float3 endPoint, float radius);
float sdRoundedCylinder(float3 position, float radiusA, float radiusB, float height);
float sdCone(float3 position, float2 c);
float sdCappedCone(float3 position, float height, float radius1, float radius2);
float sdCappedCone(float3 position, float startPoint, float endPoint, float radius1, float radius2);
float sdSolidAngle(float3 position, float2 c, float ra);
float sdRoundCone(float3 position, float radius1, float radius2, float height);
float sdEllipsoid(float3 position, float3 radius);
float sdTorus(float3 position, float2 radius);
float sdCappedTorus(float3 position, float2 sc, float ra, float rb);
float sdJoint3DSphere(float3 position, float l, float a, float w);
float sdLink(float3 position, float le, float r1, float r2);
float sdOctahedron(float3 position, float s);
float sdPyramid(float3 position, float h);
float dot2(float3 position);
float dot2(float2 position);
float udTriangle(float3 position, float3 a, float3 b, float3 c);
float udQuad(float3 position, float3 a, float3 b, float3 c, float3 d);

//Scene
Object Scene(float3 position);
PixelShaderOutput RayMarching(Ray eyeray);
float4 Lighting(Ray ray, Object Object, float depth);
float4 Phong(float3 n, float3 l, float3 v, float shininess, float4 diffuseColor, float4 specularColor);

float3 Normal(float3 position);

// A pass-through function for the (interpolated) color data.
PixelShaderOutput main(PixelShaderInput input) : SV_TARGET
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

	return RayMarching(eyeray);
}

PixelShaderOutput RayMarching(Ray ray)
{
	float depth = 0.0f;
    
    PixelShaderOutput output = (PixelShaderOutput) 0;
    output.color = float4(0.0f, 0.0f, 0.0f, 0.0f);

	for (int i = 0; i < MAX_MARCHING_STEPS; i++)
	{
		Object obj = Scene(ray.o + depth * ray.d);

		if (obj.dist < EPSILON)
		{
            output.color = Lighting(ray, obj, depth);
            
            float4 pos = mul(float4(ray.o + depth * ray.d, 1.0f), view);
            pos = mul(pos, projection);
            
            output.position = pos;
            
            return output;
        }

		depth += obj.dist;

		if (depth >= farPlane)
		{
			return output;
		}
	}
	
	return output;
}

float4 Lighting(Ray ray, Object obj, float depth)
{
	float3 hitPos = ray.o + depth * ray.d;
	float3 normal = Normal(hitPos);
	
	float3 lightDir = normalize(lightPos - hitPos);
    
	float4 diff = obj.color;
	float4 spec = obj.color;
	float4 amb = obj.color * 0.1f;
	
	return lightColor * (Phong(normal, lightDir, ray.d, 40, diff, spec) + amb);
}

float4 Phong(float3 n, float3 l, float3 v, float shininess, float4 diffuseColor, float4 specularColor)
{
	float NdotL = dot(n, l);
	float diff = saturate(NdotL);
	float3 r = reflect(l, n);
	float spec = pow(saturate(dot(v, r)), shininess) * (NdotL > 0.0);
	return diff * diffuseColor + spec * specularColor;
}

float3 Normal(float3 position)
{
	Object minX = Scene(float3(position.x - EPSILON, position.y, position.z));
	Object maxX = Scene(float3(position.x + EPSILON, position.y, position.z));
	Object minY = Scene(float3(position.x, position.y - EPSILON, position.z));
	Object maxY = Scene(float3(position.x, position.y + EPSILON, position.z));
	Object minZ = Scene(float3(position.x, position.y, position.z - EPSILON));
	Object maxZ = Scene(float3(position.x, position.y, position.z + EPSILON));

	return normalize(float3(maxX.dist - minX.dist, maxY.dist - minY.dist, maxZ.dist - minZ.dist));
}

Object Scene(float3 position)
{
	Object obj = (Object) 0;
	obj.dist = sdSphere(position - float3(5.0f, 0.0f, 0.0f), 1.0f);
	obj.color = float4(1.0f, 0.0f, 0.0f, 1.0f);
    
    const int numberOfColumns = 10;
    const float3 columnPositions[numberOfColumns] =
    {
        float3(-5.0f, 0.0f, 0.0f),
        float3(5.0f, 0.0f, 10.0f),
        float3(-5.0f, 0.0f, 20.0f),
        float3(5.0f, 0.0f, 30.0f),
        float3(-5.0f, 0.0f, 40.0f),
        float3(5.0f, 0.0f, 50.0f),
        float3(-5.0f, 0.0f, 60.0f),
        float3(5.0f, 0.0f, 70.0f),
        float3(-5.0f, 0.0f, 80.0f),
        float3(5.0f, 0.0f, 90.0f)
    };
    
    for (int i = 0; i < numberOfColumns; i++)
    {
        float tempDist = sdBox((position - columnPositions[i]) + float3(0.0f, -5.0f, 0.0f), float3(1.5f, 5.5f, 1.5f));
        
        if (tempDist < obj.dist)
        {
            tempDist = sdCappedCylinder(position, columnPositions[i], columnPositions[i] + float3(0.0f, 10.0f, 0.0f), 0.5f);
        }
        
        if (tempDist < obj.dist)
        {
            obj.dist = tempDist;
            obj.color = float4(0.84f, 0.77f, 0.67f, 1.0f);
        }
    }

	return obj;
}

//Distance Functions From https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm

float sdSphere(float3 position, float radius)
{
	return length(position) - radius;
}

float sdBox(float3 p, float3 b)
{
    float3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0);
}

float sdRoundBox(float3 p, float3 b, float r)
{
    float3 q = abs(p) - b;
    return length(max(q, 0.0)) + min(max(q.x, max(q.y, q.z)), 0.0) - r;
}

float sdPlane(float3 p, float4 n)
{
  // n must be normalized
    return dot(p, n.xyz) + n.w;
}

float sdHexPrism(float3 p, float2 h)
{
    const float3 k = float3(-0.8660254, 0.5, 0.57735);
    p = abs(p);
    p.xy -= 2.0 * min(dot(k.xy, p.xy), 0.0) * k.xy;
    float2 d = float2(
       length(p.xy - float2(clamp(p.x, -k.z * h.x, k.z * h.x), h.x)) * sign(p.y - h.x),
       p.z - h.y);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float sdTriPrism(float3 p, float2 h)
{
    const float k = sqrt(3.0);
    h.x *= 0.5 * k;
    p.xy /= h.x;
    p.x = abs(p.x) - 1.0;
    p.y = p.y + 1.0 / k;
    if (p.x + k * p.y > 0.0)
        p.xy = float2(p.x - k * p.y, -k * p.x - p.y) / 2.0;
    p.x -= clamp(p.x, -2.0, 0.0);
    float d1 = length(p.xy) * sign(-p.y) * h.x;
    float d2 = abs(p.z) - h.y;
    return length(max(float2(d1, d2), 0.0)) + min(max(d1, d2), 0.);
}

float sdCapsule(float3 p, float3 a, float3 b, float r)
{
    float3 pa = p - a, ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h) - r;
}

float sdVerticalCapsule(float3 p, float h, float r)
{
    p.y -= clamp(p.y, 0.0, h);
    return length(p) - r;
}

float sdCylinder(float3 p, float3 c)
{
    return length(p.xz - c.xy) - c.z;
}

float sdCappedCylinder(float3 p, float h, float r)
{
    float2 d = abs(float2(length(p.xz), p.y)) - float2(h, r);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0));
}

float sdCappedCylinder(float3 p, float3 a, float3 b, float r)
{
    float3 ba = b - a;
    float3 pa = p - a;
    float baba = dot(ba, ba);
    float paba = dot(pa, ba);
    float x = length(pa * baba - ba * paba) - r * baba;
    float y = abs(paba - baba * 0.5) - baba * 0.5;
    float x2 = x * x;
    float y2 = y * y * baba;
    float d = (max(x, y) < 0.0) ? -min(x2, y2) : (((x > 0.0) ? x2 : 0.0) + ((y > 0.0) ? y2 : 0.0));
    return sign(d) * sqrt(abs(d)) / baba;
}

float sdRoundedCylinder(float3 p, float ra, float rb, float h)
{
    float2 d = float2(length(p.xz) - 2.0 * ra + rb, abs(p.y) - h);
    return min(max(d.x, d.y), 0.0) + length(max(d, 0.0)) - rb;
}

float sdCone(float3 p, float2 c)
{
  // c is the sin/cos of the angle
    float q = length(p.xy);
    return dot(c, float2(q, p.z));
}

float sdCappedCone(float3 p, float h, float r1, float r2)
{
    float2 q = float2(length(p.xz), p.y);
    float2 k1 = float2(r2, h);
    float2 k2 = float2(r2 - r1, 2.0 * h);
    float2 ca = float2(q.x - min(q.x, (q.y < 0.0) ? r1 : r2), abs(q.y) - h);
    float2 cb = q - k1 + k2 * clamp(dot(k1 - q, k2) / dot2(k2), 0.0, 1.0);
    float s = (cb.x < 0.0 && ca.y < 0.0) ? -1.0 : 1.0;
    return s * sqrt(min(dot2(ca), dot2(cb)));
}

float sdCappedCone(float3 p, float3 a, float3 b, float ra, float rb)
{
    float rba = rb - ra;
    float baba = dot(b - a, b - a);
    float papa = dot(p - a, p - a);
    float paba = dot(p - a, b - a) / baba;
    float x = sqrt(papa - paba * paba * baba);
    float cax = max(0.0, x - ((paba < 0.5) ? ra : rb));
    float cay = abs(paba - 0.5) - 0.5;
    float k = rba * rba + baba;
    float f = clamp((rba * (x - ra) + paba * baba) / k, 0.0, 1.0);
    float cbx = x - ra - f * rba;
    float cby = paba - f;
    float s = (cbx < 0.0 && cay < 0.0) ? -1.0 : 1.0;
    return s * sqrt(min(cax * cax + cay * cay * baba, cbx * cbx + cby * cby * baba));
}

float sdSolidAngle(float3 p, float2 c, float ra)
{
  // c is the sin/cos of the angle
    float2 q = float2(length(p.xz), p.y);
    float l = length(q) - ra;
    float m = length(q - c * clamp(dot(q, c), 0.0, ra));
    return max(l, m * sign(c.y * q.x - c.x * q.y));
}

float sdRoundCone(float3 p, float r1, float r2, float h)
{
    float2 q = float2(length(p.xz), p.y);
    
    float b = (r1 - r2) / h;
    float a = sqrt(1.0 - b * b);
    float k = dot(q, float2(-b, a));
    
    if (k < 0.0)
        return length(q) - r1;
    if (k > a * h)
        return length(q - float2(0.0, h)) - r2;
        
    return dot(q, float2(a, b)) - r1;
}

float sdEllipsoid(float3 p, float3 r)
{
    float k0 = length(p / r);
    float k1 = length(p / (r * r));
    return k0 * (k0 - 1.0) / k1;
}

float sdTorus(float3 p, float2 t)
{
    float2 q = float2(length(p.xz) - t.x, p.y);
    return length(q) - t.y;
}

float sdCappedTorus(in float3 p, in float2 sc, in float ra, in float rb)
{
    p.x = abs(p.x);
    float k = (sc.y * p.x > sc.x * p.y) ? dot(p.xy, sc) : length(p.xy);
    return sqrt(dot(p, p) + ra * ra - 2.0 * ra * k) - rb;
}

float sdLink(float3 p, float le, float r1, float r2)
{
    float3 q = float3(p.x, max(abs(p.y) - le, 0.0), p.z);
    return length(float2(length(q.xy) - r1, q.z)) - r2;
}

float sdOctahedron(float3 p, float s)
{
    p = abs(p);
    float m = p.x + p.y + p.z - s;
    float3 q;
    if (3.0 * p.x < m)
        q = p.xyz;
    else if (3.0 * p.y < m)
        q = p.yzx;
    else if (3.0 * p.z < m)
        q = p.zxy;
    else
        return m * 0.57735027;
    
    float k = clamp(0.5 * (q.z - q.y + s), 0.0, s);
    return length(float3(q.x, q.y - s + k, q.z - k));
}

float sdPyramid(float3 p, float h)
{
    float m2 = h * h + 0.25;
    
    p.xz = abs(p.xz);
    p.xz = (p.z > p.x) ? p.zx : p.xz;
    p.xz -= 0.5;

    float3 q = float3(p.z, h * p.y - 0.5 * p.x, h * p.x + 0.5 * p.y);
   
    float s = max(-q.x, 0.0);
    float t = clamp((q.y - 0.5 * p.z) / (m2 + 0.25), 0.0, 1.0);
    
    float a = m2 * (q.x + s) * (q.x + s) + q.y * q.y;
    float b = m2 * (q.x + 0.5 * t) * (q.x + 0.5 * t) + (q.y - m2 * t) * (q.y - m2 * t);
    
    float d2 = min(q.y, -q.x * m2 - q.y * 0.5) > 0.0 ? 0.0 : min(a, b);
    
    return sqrt((d2 + q.z * q.z) / m2) * sign(max(q.z, -p.y));
}

float dot2(float3 v)
{
    return dot(v, v);
}

float dot2(float2 v)
{
    return dot(v, v);
}

float udTriangle(float3 p, float3 a, float3 b, float3 c)
{
    float3 ba = b - a;
    float3 pa = p - a;
    float3 cb = c - b;
    float3 pb = p - b;
    float3 ac = a - c;
    float3 pc = p - c;
    float3 nor = cross(ba, ac);

    return sqrt(
    (sign(dot(cross(ba, nor), pa)) +
     sign(dot(cross(cb, nor), pb)) +
     sign(dot(cross(ac, nor), pc)) < 2.0)
     ?
     min(min(
     dot2(ba * clamp(dot(ba, pa) / dot2(ba), 0.0, 1.0) - pa),
     dot2(cb * clamp(dot(cb, pb) / dot2(cb), 0.0, 1.0) - pb)),
     dot2(ac * clamp(dot(ac, pc) / dot2(ac), 0.0, 1.0) - pc))
     :
     dot(nor, pa) * dot(nor, pa) / dot2(nor));
}

float udQuad(float3 p, float3 a, float3 b, float3 c, float3 d)
{
    float3 ba = b - a;
    float3 pa = p - a;
    float3 cb = c - b;
    float3 pb = p - b;
    float3 dc = d - c;
    float3 pc = p - c;
    float3 ad = a - d;
    float3 pd = p - d;
    float3 nor = cross(ba, ad);

    return sqrt(
    (sign(dot(cross(ba, nor), pa)) +
     sign(dot(cross(cb, nor), pb)) +
     sign(dot(cross(dc, nor), pc)) +
     sign(dot(cross(ad, nor), pd)) < 3.0)
     ?
     min(min(min(
     dot2(ba * clamp(dot(ba, pa) / dot2(ba), 0.0, 1.0) - pa),
     dot2(cb * clamp(dot(cb, pb) / dot2(cb), 0.0, 1.0) - pb)),
     dot2(dc * clamp(dot(dc, pc) / dot2(dc), 0.0, 1.0) - pc)),
     dot2(ad * clamp(dot(ad, pd) / dot2(ad), 0.0, 1.0) - pd))
     :
     dot(nor, pa) * dot(nor, pa) / dot2(nor));
}