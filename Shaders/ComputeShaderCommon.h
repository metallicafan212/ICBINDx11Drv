typedef uint 	UBOOL;
typedef int		INT;

// Metallicafan212:	Ok, we're just going to fucking use defined types so we can do =
typedef float3 FVector;
typedef float4 FPlane;

class FRotator
{
	INT Pitch, Yaw, Roll;
};

FLOAT SizeSquared(FVector V)
{
	return V.x * V.x + V.y * V.y + V.z * V.z;
}

FLOAT Size(FVector V)
{
	return sqrt(V.x * V.x + V.y * V.y + V.z * V.z);
}

FLOAT SizeSquared(FPlane P)
{
	return P.x * P.x + P.y * P.y + P.z * P.z + P.w * P.w;
}

FLOAT Size(FPlane P)
{
	return sqrt(P.x * P.x + P.y * P.y + P.z * P.z + P.w * P.w);
}

FLOAT Dot(FVector L, FVector R)
{
	// Metallicafan212:	Return the dot product between us and the other vector
	return (L.x * R.x) + (L.y * R.y) + (L.z * R.z);
}

FVector MirrorByVector(FVector L, FVector R)
{
	/*
	// return *this - MirrorNormal * (2.f * (*this | MirrorNormal));
	FVector Temp = V.Mul(Dot(V) * 2.0f);
		
	// Metallicafan212:	Now subtract
	return Sub(Temp);
	*/
	
	return L - R * (2.f * (Dot(L, R)));
}

class FCoords
{
	FVector XAxis, YAxis, ZAxis;
};


struct FOutVector
{
	FVector Point;
	dword   Flags;
};

//enum EVectorFlags
//{
#define FVF_OutXMin	 	0x04,	// Outcode rejection, off left hand side of screen.
#define FVF_OutXMax	 	0x08,	// Outcode rejection, off right hand side of screen.
#define FVF_OutYMin	 	0x10,	// Outcode rejection, off top of screen.
#define FVF_OutYMax	 	0x20,	// Outcode rejection, off bottom of screen.
#define FVF_OutNear     0x40, 	// Near clipping plane.
#define FVF_OutFar      0x80, 	// Far clipping plane.
#define FVF_OutReject   (FVF_OutXMin | FVF_OutXMax | FVF_OutYMin | FVF_OutYMax), // Outcode rejectable.
#define FVF_OutSkip	 	(FVF_OutXMin | FVF_OutXMax | FVF_OutYMin | FVF_OutYMax), // Outcode clippable.
//};

struct FTransform : FOutVector
{
	float ScreenX;
	float ScreenY;
	int   IntY;
	float RZ;
};

struct FTransSample : FTransform
{
	FPlane Normal, Light, Fog;
	
	FTransSample Add(FTransSample T )
	{
		FTransSample Temp;
		Temp.Point = Point + T.Point;
		Temp.Light = Light + T.Light;
		Temp.Fog   = Fog + T.Fog;
		return Temp;
	}
    
	FTransSample Subtract(FTransSample T )
	{
		FTransSample Temp;
		Temp.Point = Point - T.Point;
		Temp.Light = Light - T.Light;
		Temp.Fog   = Fog - T.Fog;
		return Temp;
	}
    
	FTransSample Mul( FLOAT Scale )
	{
		FTransSample Temp;
		Temp.Point = Point * Scale;
		Temp.Light = Light * Scale;
		Temp.Fog   = Fog * Scale;
		return Temp;
	}
};

struct FTransTexture : FTransSample
{
	FLOAT U, V;
};

typedef dword ELightSource;

//enum ELightSource
//{
#define LD_Point 	0
#define LD_Plane 	1
#define LD_Ambient 	2
//};

typedef dword ELightKind;

//enum ELightKind
//{
#define ALO_StaticLight		0	// Actor is a non-moving, non-changing lightsource
#define ALO_DynamicLight	1	// Actor is a non-moving, changing lightsource
#define ALO_MovingLight		2	// Actor is a moving, changing lightsource
#define ALO_NotLight		3	// Not a surface light (probably volumetric only).
//};

struct FLightCommon
{
	// For all lights.
	ELightKind		Opt;					// Light type.
	FVector			Location;				// Transformed screenspace location of light.
	FVector			LightDir;				// Unit direction from actor to light (max diffuse reflection).
	FLOAT			Radius;					// Maximum effective radius.
	FLOAT			RRadiusMult;			// 1.0f / (Radius - RadiusInner);

	// Coloring.
	FPlane			FloatColor;				// Incident lighting color.
	FPlane			VolumetricColor;		// Volumetric lighting color.

	// For volumetric lights.
	FLOAT			VolBrightness;			// Volumetric lighting brightness.
	UBOOL       	VolInside;				// Viewpoint is inside the sphere.
	FLOAT			VolRadiusSquared;		// VolRadius*VolRadius.
	FLOAT			LocationSizeSquared;	// Location.SizeSqurated().
	FLOAT			RVolRadius;				// 1/Volumetric radius.
	
	// Metallicafan212:	Past this point, it's just hand-fed variables
	UBOOL			bVolumetric;
	
	// Metallicafan212:	Actor related data for this light
	ELightSource 	ActorLightSource;
	FRotator		ActorRotation;
	INT				ActorLightBrightness;
	UBOOL			ActorbDarkLight;
	dword			ActorLightEffect;
	
	// Metallicafan212:	Need to pad this struct to 16 bytes
	INT				Pad[2];
};