#ifndef DECLARE_ENUM
#define DECLARE_ENUM enum EGammaMode
#endif

#ifndef DECLARE_GM
#define DECLARE_GM(name, val) name = val,
#endif

// Metallicafan212:	Gamma modes
DECLARE_ENUM
{
	DECLARE_GM(GM_XOpenGL, 0)
	//DECLARE_GM(GM_PerObject, 1)
	DECLARE_GM(GM_DX9, 1)
	DECLARE_GM(GM_None, 2)
};

#undef DECLARE_ENUM
#undef DECLARE_GM