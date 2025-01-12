#ifndef DECLARE_SF_ENUM
#define DECLARE_SF_ENUM enum EShaderFlags
#endif

#ifndef DECLARE_SF
#define DECLARE_SF(name, val) name = val,
#endif

// Metallicafan212:	Gamma modes
DECLARE_SF_ENUM
{
	// Metallicafan212:	If alpha is currently enabled
	DECLARE_SF(SF_AlphaEnabled,		0x00000001)

	// Metallicafan212:	If modulation is enabled
	DECLARE_SF(SF_Modulated,		0x00000002)

	// Metallicafan212:	TILES ONLY! If the Z is below a specific range AND masking is enabled, use a separate alpha reject
	DECLARE_SF(SF_HudSmoothMasked,	0x00000004)

	// Metallicafan212:	MSDF font tile
	DECLARE_SF(SF_MSDFRendering,	0x00000008)
};

#undef DECLARE_SF_ENUM
#undef DECLARE_SF