#if 0
//
// Generated by Microsoft (R) HLSL Shader Compiler 10.1
//
//
// Buffer Definitions: 
//
// cbuffer FrameVariables
// {
//
//   float4x4 Proj;                     // Offset:    0 Size:    64 [unused]
//   float Gamma;                       // Offset:   64 Size:     4 [unused]
//   float2 ViewSize;                   // Offset:   68 Size:     8 [unused]
//   int bDoSelection;                  // Offset:   76 Size:     4
//   int bOneXLightmaps;                // Offset:   80 Size:     4 [unused]
//   int bCorrectFog;                   // Offset:   84 Size:     4 [unused]
//   uint FrameShaderFlags;             // Offset:   88 Size:     4
//   int GammaMode;                     // Offset:   92 Size:     4 [unused]
//   float HDRExpansion;                // Offset:   96 Size:     4 [unused]
//   float ResolutionScale;             // Offset:  100 Size:     4 [unused]
//   float WhiteLevel;                  // Offset:  104 Size:     4 [unused]
//   int RendMap;                       // Offset:  108 Size:     4
//   float DepthDrawLimit;              // Offset:  112 Size:     4
//   float GammaOffsetRed;              // Offset:  116 Size:     4 [unused]
//   float GammaOffsetBlue;             // Offset:  120 Size:     4 [unused]
//   float GammaOffsetGreen;            // Offset:  124 Size:     4 [unused]
//
// }
//
// cbuffer PolyflagVars
// {
//
//   int bSelected;                     // Offset:    0 Size:     4
//   float AlphaReject;                 // Offset:    4 Size:     4
//   float AltAlphaReject;              // Offset:    8 Size:     4
//   float BWPercent;                   // Offset:   12 Size:     4
//   float3 SelectedColor;              // Offset:   16 Size:    12 [unused]
//   float PadSelect;                   // Offset:   28 Size:     4 [unused]
//   uint ShaderFlags;                  // Offset:   32 Size:     4
//   float3 PadFlags;                   // Offset:   36 Size:    12 [unused]
//
// }
//
// cbuffer DFogVariables
// {
//
//   float4 DistanceFogColor;           // Offset:    0 Size:    16
//   float4 DistanceFogSettings;        // Offset:   16 Size:    16
//   int bDoDistanceFog;                // Offset:   32 Size:     4
//   float3 Paddy3;                     // Offset:   36 Size:    12 [unused]
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// DiffState                         sampler      NA          NA             s0      1 
// Diffuse                           texture  float4          2d             t0      1 
// FrameVariables                    cbuffer      NA          NA            cb0      1 
// DFogVariables                     cbuffer      NA          NA            cb1      1 
// PolyflagVars                      cbuffer      NA          NA            cb2      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
// TEXCOORD                 0   xy          1     NONE   float   xy  
// COLOR                    2     z         1     NONE   float     z 
// COLOR                    0   xyzw        2     NONE   float   xyzw
// COLOR                    1   xyzw        3     NONE   float       
// COLOR                    3   x           4     NONE     int   x   
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
ps_5_0
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[8], immediateIndexed
dcl_constantbuffer CB2[3], immediateIndexed
dcl_constantbuffer CB1[3], immediateIndexed
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_input_ps linear v1.xy
dcl_input_ps linear v1.z
dcl_input_ps linear v2.xyzw
dcl_input_ps constant v4.x
dcl_output o0.xyzw
dcl_temps 5
ge r0.x, l(1.000000), v1.z
and r0.y, l(1), cb2[2].x
ine r0.z, r0.y, l(0)
and r0.x, r0.z, r0.x
ne r0.w, cb2[0].z, cb2[0].y
and r0.x, r0.w, r0.x
movc r0.x, r0.x, cb2[0].z, cb2[0].y
sample_b_indexable(texture2d)(float,float,float,float) r1.xyzw, v1.xyxx, t0.xyzw, s0, l(0.000000)
add r0.w, r1.y, r1.x
add r0.w, r1.z, r0.w
mul r0.w, r0.w, l(0.333333)
movc r1.xyz, cb2[0].xxxx, r0.wwww, r1.xyzx
mul r1.xyzw, r1.xyzw, v2.xyzw
ieq r0.w, l(30), cb0[6].w
not r2.x, r0.w
and r0.z, r0.z, r2.x
lt r0.x, r1.w, r0.x
and r0.x, r0.x, r0.z
discard_nz r0.x
movc r1.w, r0.y, r1.w, l(1.000000)
movc r1.xyzw, r0.wwww, v2.xyzw, r1.xyzw
ieq r0.x, l(29), cb0[6].w
div r0.y, v1.z, cb0[7].x
movc r2.xyz, r0.xxxx, r0.yyyy, r1.xyzx
mad r0.y, v1.z, -cb1[1].x, cb1[1].y
mad_sat r0.y, -r0.y, cb1[1].z, cb1[1].w
ge r0.z, cb1[1].y, v1.z
movc r0.y, r0.z, l(0), r0.y
ge r0.z, l(0.000000), r0.y
add r3.xyz, -r2.xyzx, cb1[0].xyzx
mul r3.xyz, r3.xyzx, cb1[0].wwww
mad r3.xyz, r3.xyzx, r0.yyyy, r2.xyzx
movc r3.xyz, r0.zzzz, r2.xyzx, r3.xyzx
movc r1.xyz, cb1[2].xxxx, r3.xyzx, r2.xyzx
ine r0.y, v4.x, l(0)
lt r0.z, r1.w, cb2[0].y
mov r2.xyz, v2.xyzx
mov r2.w, l(1.000000)
movc r2.xyzw, r0.zzzz, l(0,0,0,0), r2.xyzw
or r0.x, r0.w, r0.x
and r0.z, l(1), cb0[5].z
mov_sat r3.xyz, r1.xyzx
movc r3.xyz, r0.zzzz, r1.xyzx, r3.xyzx
ge r0.z, l(0.000000), cb2[0].w
or r0.y, r0.y, r0.z
add r0.z, r3.y, r3.x
add r0.z, r3.z, r0.z
mad r4.xyz, r0.zzzz, l(0.333333, 0.333333, 0.333333, 0.000000), -r3.xyzx
mad r4.xyz, cb2[0].wwww, r4.xyzx, r3.xyzx
movc r3.xyz, r0.yyyy, r3.xyzx, r4.xyzx
mov r3.w, r1.w
movc r0.xyzw, r0.xxxx, r1.xyzw, r3.xyzw
movc o0.xyzw, cb0[4].wwww, r2.xyzw, r0.xyzw
ret 
// Approximately 54 instruction slots used
#endif

const BYTE g_PxShader[] =
{
     68,  88,  66,  67,  18,  36, 
    102,  46, 240,  21, 137, 249, 
      5, 102,  15, 224, 216, 193, 
    235, 151,   1,   0,   0,   0, 
    168,  17,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    184,   8,   0,   0, 116,   9, 
      0,   0, 168,   9,   0,   0, 
     12,  17,   0,   0,  82,  68, 
     69,  70, 124,   8,   0,   0, 
      3,   0,   0,   0,  24,   1, 
      0,   0,   5,   0,   0,   0, 
     60,   0,   0,   0,   0,   5, 
    255, 255,   0,   1,   0,   0, 
     82,   8,   0,   0,  82,  68, 
     49,  49,  60,   0,   0,   0, 
     24,   0,   0,   0,  32,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    220,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0, 230,   0,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0, 238,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    253,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  11,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  68, 105, 
    102, 102,  83, 116,  97, 116, 
    101,   0,  68, 105, 102, 102, 
    117, 115, 101,   0,  70, 114, 
     97, 109, 101,  86,  97, 114, 
    105,  97,  98, 108, 101, 115, 
      0,  68,  70, 111, 103,  86, 
     97, 114, 105,  97,  98, 108, 
    101, 115,   0,  80, 111, 108, 
    121, 102, 108,  97, 103,  86, 
     97, 114, 115,   0, 238,   0, 
      0,   0,  16,   0,   0,   0, 
     96,   1,   0,   0, 128,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  11,   1, 
      0,   0,   8,   0,   0,   0, 
    132,   5,   0,   0,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 253,   0, 
      0,   0,   4,   0,   0,   0, 
     76,   7,   0,   0,  48,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 224,   3, 
      0,   0,   0,   0,   0,   0, 
     64,   0,   0,   0,   0,   0, 
      0,   0, 240,   3,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  20,   4,   0,   0, 
     64,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
     32,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     68,   4,   0,   0,  68,   0, 
      0,   0,   8,   0,   0,   0, 
      0,   0,   0,   0,  84,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 120,   4, 
      0,   0,  76,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0, 140,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 176,   4,   0,   0, 
     80,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    140,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    191,   4,   0,   0,  84,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 140,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 203,   4, 
      0,   0,  88,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0, 228,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   8,   5,   0,   0, 
     92,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    140,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     18,   5,   0,   0,  96,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,  32,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  31,   5, 
      0,   0, 100,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  32,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  47,   5,   0,   0, 
    104,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
     32,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     58,   5,   0,   0, 108,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0, 140,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  66,   5, 
      0,   0, 112,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  32,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  81,   5,   0,   0, 
    116,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
     32,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     96,   5,   0,   0, 120,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,  32,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 112,   5, 
      0,   0, 124,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  32,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  80, 114, 111, 106, 
      0, 102, 108, 111,  97, 116, 
     52, 120,  52,   0, 171, 171, 
      3,   0,   3,   0,   4,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 229,   3,   0,   0, 
     71,  97, 109, 109,  97,   0, 
    102, 108, 111,  97, 116,   0, 
      0,   0,   3,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  26,   4,   0,   0, 
     86, 105, 101, 119,  83, 105, 
    122, 101,   0, 102, 108, 111, 
     97, 116,  50,   0,   1,   0, 
      3,   0,   1,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     77,   4,   0,   0,  98,  68, 
    111,  83, 101, 108, 101,  99, 
    116, 105, 111, 110,   0, 105, 
    110, 116,   0, 171, 171, 171, 
      0,   0,   2,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 133,   4,   0,   0, 
     98,  79, 110, 101,  88,  76, 
    105, 103, 104, 116, 109,  97, 
    112, 115,   0,  98,  67, 111, 
    114, 114, 101,  99, 116,  70, 
    111, 103,   0,  70, 114,  97, 
    109, 101,  83, 104,  97, 100, 
    101, 114,  70, 108,  97, 103, 
    115,   0, 100, 119, 111, 114, 
    100,   0, 171, 171,   0,   0, 
     19,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    220,   4,   0,   0,  71,  97, 
    109, 109,  97,  77, 111, 100, 
    101,   0,  72,  68,  82,  69, 
    120, 112,  97, 110, 115, 105, 
    111, 110,   0,  82, 101, 115, 
    111, 108, 117, 116, 105, 111, 
    110,  83,  99,  97, 108, 101, 
      0,  87, 104, 105, 116, 101, 
     76, 101, 118, 101, 108,   0, 
     82, 101, 110, 100,  77,  97, 
    112,   0,  68, 101, 112, 116, 
    104,  68, 114,  97, 119,  76, 
    105, 109, 105, 116,   0,  71, 
     97, 109, 109,  97,  79, 102, 
    102, 115, 101, 116,  82, 101, 
    100,   0,  71,  97, 109, 109, 
     97,  79, 102, 102, 115, 101, 
    116,  66, 108, 117, 101,   0, 
     71,  97, 109, 109,  97,  79, 
    102, 102, 115, 101, 116,  71, 
    114, 101, 101, 110,   0, 171, 
    171, 171, 196,   6,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    140,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    206,   6,   0,   0,   4,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0,  32,   4, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 218,   6, 
      0,   0,   8,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  32,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 233,   6,   0,   0, 
     12,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
     32,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    243,   6,   0,   0,  16,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0,   8,   7, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  44,   7, 
      0,   0,  28,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  32,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  54,   7,   0,   0, 
     32,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    228,   4,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     66,   7,   0,   0,  36,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0,   8,   7, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  98,  83, 
    101, 108, 101,  99, 116, 101, 
    100,   0,  65, 108, 112, 104, 
     97,  82, 101, 106, 101,  99, 
    116,   0,  65, 108, 116,  65, 
    108, 112, 104,  97,  82, 101, 
    106, 101,  99, 116,   0,  66, 
     87,  80, 101, 114,  99, 101, 
    110, 116,   0,  83, 101, 108, 
    101,  99, 116, 101, 100,  67, 
    111, 108, 111, 114,   0, 102, 
    108, 111,  97, 116,  51,   0, 
      1,   0,   3,   0,   1,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   7,   0,   0, 
     80,  97, 100,  83, 101, 108, 
    101,  99, 116,   0,  83, 104, 
     97, 100, 101, 114,  70, 108, 
     97, 103, 115,   0,  80,  97, 
    100,  70, 108,  97, 103, 115, 
      0, 171, 236,   7,   0,   0, 
      0,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
      4,   8,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     40,   8,   0,   0,  16,   0, 
      0,   0,  16,   0,   0,   0, 
      2,   0,   0,   0,   4,   8, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  60,   8, 
      0,   0,  32,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0, 140,   4,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  75,   8,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
      8,   7,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     68, 105, 115, 116,  97, 110, 
     99, 101,  70, 111, 103,  67, 
    111, 108, 111, 114,   0, 102, 
    108, 111,  97, 116,  52,   0, 
      1,   0,   3,   0,   1,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 253,   7,   0,   0, 
     68, 105, 115, 116,  97, 110, 
     99, 101,  70, 111, 103,  83, 
    101, 116, 116, 105, 110, 103, 
    115,   0,  98,  68, 111,  68, 
    105, 115, 116,  97, 110,  99, 
    101,  70, 111, 103,   0,  80, 
     97, 100, 100, 121,  51,   0, 
     77, 105,  99, 114, 111, 115, 
    111, 102, 116,  32,  40,  82, 
     41,  32,  72,  76,  83,  76, 
     32,  83, 104,  97, 100, 101, 
    114,  32,  67, 111, 109, 112, 
    105, 108, 101, 114,  32,  49, 
     48,  46,  49,   0, 171, 171, 
     73,  83,  71,  78, 180,   0, 
      0,   0,   6,   0,   0,   0, 
      8,   0,   0,   0, 152,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0, 164,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   3,   0,   0, 173,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      4,   4,   0,   0, 173,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,  15,   0,   0, 173,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,   0,   0,   0, 173,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   4,   0,   0,   0, 
      1,   1,   0,   0,  83,  86, 
     95,  80,  79,  83,  73,  84, 
     73,  79,  78,   0,  84,  69, 
     88,  67,  79,  79,  82,  68, 
      0,  67,  79,  76,  79,  82, 
      0, 171,  79,  83,  71,  78, 
     44,   0,   0,   0,   1,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
     83,  86,  95,  84,  65,  82, 
     71,  69,  84,   0, 171, 171, 
     83,  72,  69,  88,  92,   7, 
      0,   0,  80,   0,   0,   0, 
    215,   1,   0,   0, 106,   8, 
      0,   1,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   2,   0,   0,   0, 
      3,   0,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   0,   0, 
      0,   0,  85,  85,   0,   0, 
     98,  16,   0,   3,  50,  16, 
     16,   0,   1,   0,   0,   0, 
     98,  16,   0,   3,  66,  16, 
     16,   0,   1,   0,   0,   0, 
     98,  16,   0,   3, 242,  16, 
     16,   0,   2,   0,   0,   0, 
     98,   8,   0,   3,  18,  16, 
     16,   0,   4,   0,   0,   0, 
    101,   0,   0,   3, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    104,   0,   0,   2,   5,   0, 
      0,   0,  29,   0,   0,   7, 
     18,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  42,  16, 
     16,   0,   1,   0,   0,   0, 
      1,   0,   0,   8,  34,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   1,   0, 
      0,   0,  10, 128,  32,   0, 
      2,   0,   0,   0,   2,   0, 
      0,   0,  39,   0,   0,   7, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  57,   0, 
      0,   9, 130,   0,  16,   0, 
      0,   0,   0,   0,  42, 128, 
     32,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,  26, 128, 
     32,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  55,   0,   0,  11, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,  42, 128, 
     32,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,  26, 128, 
     32,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,  74,   0, 
      0, 141, 194,   0,   0, 128, 
     67,  85,  21,   0, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  16,  16,   0,   1,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   7, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   7, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 130,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0, 171, 170, 
    170,  62,  55,   0,   0,  10, 
    114,   0,  16,   0,   1,   0, 
      0,   0,   6, 128,  32,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0, 246,  15,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   7, 242,   0, 
     16,   0,   1,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  70,  30,  16,   0, 
      2,   0,   0,   0,  32,   0, 
      0,   8, 130,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,  30,   0,   0,   0, 
     58, 128,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
     59,   0,   0,   5,  18,   0, 
     16,   0,   2,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,   1,   0,   0,   7, 
     66,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   2,   0,   0,   0, 
     49,   0,   0,   7,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  13,   0,   4,   3, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  55,   0,   0,   9, 
    130,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  55,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0, 246,  15,  16,   0, 
      0,   0,   0,   0,  70,  30, 
     16,   0,   2,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  32,   0,   0,   8, 
     18,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
     29,   0,   0,   0,  58, 128, 
     32,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,  14,   0, 
      0,   8,  34,   0,  16,   0, 
      0,   0,   0,   0,  42,  16, 
     16,   0,   1,   0,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0,   7,   0,   0,   0, 
     55,   0,   0,   9, 114,   0, 
     16,   0,   2,   0,   0,   0, 
      6,   0,  16,   0,   0,   0, 
      0,   0,  86,   5,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     50,   0,   0,  12,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     42,  16,  16,   0,   1,   0, 
      0,   0,  10, 128,  32, 128, 
     65,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     26, 128,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     50,  32,   0,  12,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
     42, 128,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     58, 128,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     29,   0,   0,   8,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     26, 128,  32,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     42,  16,  16,   0,   1,   0, 
      0,   0,  55,   0,   0,   9, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  42,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   0,   0, 
      0,   0,  29,   0,   0,   7, 
     66,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  26,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   9, 114,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16, 128,  65,   0, 
      0,   0,   2,   0,   0,   0, 
     70, 130,  32,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
     56,   0,   0,   8, 114,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0, 246, 143,  32,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
    114,   0,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  55,   0,   0,   9, 
    114,   0,  16,   0,   3,   0, 
      0,   0, 166,  10,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  55,   0,   0,  10, 
    114,   0,  16,   0,   1,   0, 
      0,   0,   6, 128,  32,   0, 
      1,   0,   0,   0,   2,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     39,   0,   0,   7,  34,   0, 
     16,   0,   0,   0,   0,   0, 
     10,  16,  16,   0,   4,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  49,   0, 
      0,   8,  66,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     26, 128,  32,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
     54,   0,   0,   5, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     70,  18,  16,   0,   2,   0, 
      0,   0,  54,   0,   0,   5, 
    130,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  55,   0, 
      0,  12, 242,   0,  16,   0, 
      2,   0,   0,   0, 166,  10, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  60,   0, 
      0,   7,  18,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   0,   0, 
      0,   0,   1,   0,   0,   8, 
     66,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      1,   0,   0,   0,  42, 128, 
     32,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  54,  32, 
      0,   5, 114,   0,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     55,   0,   0,   9, 114,   0, 
     16,   0,   3,   0,   0,   0, 
    166,  10,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
     29,   0,   0,   8,  66,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  58, 128,  32,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,  60,   0,   0,   7, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  26,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   0,   0,   0,   0, 
      0,   0,   0,   7,  66,   0, 
     16,   0,   0,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   7,  66,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     42,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  13, 
    114,   0,  16,   0,   4,   0, 
      0,   0, 166,  10,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0, 171, 170, 170,  62, 
    171, 170, 170,  62, 171, 170, 
    170,  62,   0,   0,   0,   0, 
     70,   2,  16, 128,  65,   0, 
      0,   0,   3,   0,   0,   0, 
     50,   0,   0,  10, 114,   0, 
     16,   0,   4,   0,   0,   0, 
    246, 143,  32,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,  55,   0, 
      0,   9, 114,   0,  16,   0, 
      3,   0,   0,   0,  86,   5, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      4,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     55,   0,   0,   9, 242,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     55,   0,   0,  10, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    246, 143,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    148,   0,   0,   0,  54,   0, 
      0,   0,   5,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  21,   0,   0,   0, 
      4,   0,   0,   0,   9,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     13,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};
