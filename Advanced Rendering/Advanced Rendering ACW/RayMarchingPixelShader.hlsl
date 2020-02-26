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

//Scene
Object Scene(float3 position);
float4 RayMarching(Ray eyeray);
float4 Lighting(Ray ray, Object Object, float depth);
float4 Phong(float3 n, float3 l, float3 v, float shininess, float4 diffuseColor, float4 specularColor);

float3 Normal(float3 position);

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

	return RayMarching(eyeray);
}

float4 RayMarching(Ray ray)
{
	float depth = 0.0f;

	for (int i = 0; i < MAX_MARCHING_STEPS; i++)
	{
		Object obj = Scene(ray.o + depth * ray.d);

		if (obj.dist < EPSILON)
		{
			return Lighting(ray, obj, depth);
		}

		depth += obj.dist;

		if (depth >= farPlane)
		{
			return backgroundColor;
		}
	}
	
	return backgroundColor;
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
	obj.dist = sdSphere(position - float3(0.0f, 0.0f, 0.0f), 1.0f);
	obj.color = float4(1.0f, 0.0f, 0.0f, 1.0f);

	return obj;
}

float sdSphere(float3 position, float radius)
{
	return length(position) - radius;
}