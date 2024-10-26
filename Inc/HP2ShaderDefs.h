// Metallicafan212:	Generalized macro definitions for HP2 shaders
#ifndef HP2_GLOBAL_VARS
#define HP2_GLOBAL_VARS 1

#define WINE 0
#define USE_GEO_SHADER !WINE

// Metallicafan212:	Generate the constant key
#ifndef FIRST_USER_CONSTBUFF

#define CAT(x, y) x##y

#define FIRST_USER_CONSTBUFF_NUM 4
#define MACRO_STR(mac) #mac
#define MACRO_TO_STR(mac) MACRO_STR(mac)
#define FIRST_USER_CONSTBUFF_STR CAT(b, FIRST_USER_CONSTBUFF_NUM)
#define FIRST_USER_CONSTBUFF FIRST_USER_CONSTBUFF_STR

#endif

#define EXTRA_VERT_INFO			1
#define COMPLEX_SURF_MANUAL_UVs 1

// Metallicafan212:	Add the gamma modes
/*
#define DECLARE_ENUM
#define DECLARE_GM(name, val #define name val
#include "GammaModes.h"
*/
#define GM_XOpenGL 0
#define GM_DX9 1

/*
// Metallicafan212:	Shader flags
#define DECLARE_SF_ENUM
#define DECLARE_SF(name, val #define name val
#include "ShaderFlags.h"
*/

#define SF_AlphaEnabled		0x00000001
#define SF_Modulated		0x00000002
#define SF_HudSmoothMasked	0x00000004

#define RENMAPS				1
#define NO_CUSTOM_RMODES	0

/*
#undef BEGIN_REND_ENUM
#undef END_REND_ENUM
#undef DECLARE_RENDMODE
#define BEGIN_REND_ENUM
#define END_REND_ENUM
#define DECLARE_RENDMODE(name, val#define name val
#include "UnRenderTypes.h"
#undef DECLARE_RENDMODE
#undef BEGIN_REND_ENUM
#undef END_REND_ENUM
*/

// Metallicafan212:	This sucks...
#define REN_None			0	// Hide completely.
#define REN_Wire			1	// Wireframe of EdPolys.
#define REN_Zones			2	// Show zones and zone portals.
#define REN_Polys			3	// Flat-shaded Bsp.
#define REN_PolyCuts		4	// Flat-shaded Bsp with normals displayed.
#define REN_DynLight		5	// Illuminated texture mapping.
#define REN_PlainTex		6	// Plain texture mapping.
#define REN_OrthXY			13	// Orthogonal overhead (XY view.
#define REN_OrthXZ			14	// Orthogonal XZ view.
#define REN_OrthYZ			15	// Orthogonal YZ view.
#define REN_TexView			16	// Viewing a texture (no actor.
#define REN_TexBrowser		17	// Viewing a texture browser (no actor.
#define REN_MeshView		18	// Viewing a mesh.
#define REN_MatineeIP		19	// Matinee - interpolation point editing.
#define REN_LightingOnly	20	// Forces the BSP surfaces to use the same solid color to show lighting
#define REN_Prefab			21	// Metallicafan212:	Normal no BSP prefabs
#define REN_PrefabCompiled	22	// Metallicafan212:	Compiled prefab with BSP enabled
#define REN_AnimView		23	// Metallicafan212:	New mesh browser that is a level
#define REN_AnimViewWire	24	// Metallicafan212:	Wireframe for the mesh browser
#define REN_Leafs			25	// Metallicafan212: Visibility (lighting leafs
#define REN_SpecialPoly		26	// Metallicafan212:	Display special poly (climbable surfaces
#define REN_RenderPasses	27	// Metallicafan212:	Color pass 1 objects blue pass 2 objects green
#define REN_ParticleView	28	// Metallicafan212:	New particleFX browser
#define REN_Depth			29	// Metallicafan212:	Depth RMode (exclusive to DX11
#define REN_Normals			30 // Metallicafan212:	Render vertex normals
#define REN_MAX				31

#endif