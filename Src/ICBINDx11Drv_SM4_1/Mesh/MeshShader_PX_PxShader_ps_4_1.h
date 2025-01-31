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
//   int bCorrectFog;                   // Offset:   84 Size:     4
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
//   float AltAlphaReject;              // Offset:    8 Size:     4 [unused]
//   float BWPercent;                   // Offset:   12 Size:     4
//   float3 SelectedColor;              // Offset:   16 Size:    12 [unused]
//   float PadSelect;                   // Offset:   28 Size:     4 [unused]
//   uint ShaderFlags;                  // Offset:   32 Size:     4
//   float3 PadFlags;                   // Offset:   36 Size:    12 [unused]
//
// }
//
// cbuffer TextureVariables
// {
//
//   uint bTexturesBound;               // Offset:    0 Size:     4
//   float3 TexturePad;                 // Offset:   16 Size:    12 [unused]
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
// DetailState                       sampler      NA          NA             s1      1 
// MacroState                        sampler      NA          NA             s2      1 
// Diffuse                           texture  float4          2d             t0      1 
// Detail                            texture  float4          2d             t1      1 
// Macro                             texture  float4          2d             t2      1 
// FrameVariables                    cbuffer      NA          NA            cb0      1 
// DFogVariables                     cbuffer      NA          NA            cb1      1 
// PolyflagVars                      cbuffer      NA          NA            cb2      1 
// TextureVariables                  cbuffer      NA          NA            cb3      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
// TEXCOORD                 0   xy          1     NONE   float   xy  
// TEXCOORD                 1     zw        1     NONE   float     zw
// TEXCOORD                 2   xy          2     NONE   float   xy  
// COLOR                    2     z         2     NONE   float     z 
// COLOR                    0   xyzw        3     NONE   float   xyzw
// COLOR                    1   xyzw        4     NONE   float   xyzw
//
//
// Output signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_TARGET                0   xyzw        0   TARGET   float   xyzw
//
ps_4_1
dcl_globalFlags refactoringAllowed
dcl_constantbuffer CB0[8], immediateIndexed
dcl_constantbuffer CB2[3], immediateIndexed
dcl_constantbuffer CB3[1], immediateIndexed
dcl_constantbuffer CB1[3], immediateIndexed
dcl_sampler s0, mode_default
dcl_sampler s1, mode_default
dcl_sampler s2, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_resource_texture2d (float,float,float,float) t1
dcl_resource_texture2d (float,float,float,float) t2
dcl_input_ps linear v1.xy
dcl_input_ps linear v1.zw
dcl_input_ps linear v2.xy
dcl_input_ps linear v2.z
dcl_input_ps linear v3.xyzw
dcl_input_ps linear v4.xyzw
dcl_output o0.xyzw
dcl_temps 5
add r0.x, -v4.w, l(1.000000)
mul r0.xyz, r0.xxxx, v3.xyzx
movc r0.xyz, cb0[5].yyyy, r0.xyzx, v3.xyzx
sample r1.xyzw, v1.xyxx, t0.xyzw, s0
ieq r2.x, l(20), cb0[6].w
if_nz r2.x
  mul r2.xyz, r0.xyzx, l(0.500000, 0.500000, 0.500000, 0.000000)
  and r3.x, l(1), cb2[2].x
  ine r3.y, r3.x, l(0)
  lt r3.z, r1.w, cb2[0].y
  and r3.y, r3.z, r3.y
  discard_nz r3.y
  movc r2.w, r3.x, r1.w, l(1.000000)
else 
  add r3.x, r1.y, r1.x
  add r3.x, r1.z, r3.x
  mul r3.x, r3.x, l(0.333333)
  movc r1.xyz, cb2[0].xxxx, r3.xxxx, r1.xyzx
  and r3.xy, l(2, 4, 0, 0), cb3[0].xxxx
  if_nz r3.x
    lt r3.x, v2.z, l(1024.000000)
    if_nz r3.x
      sample r3.xzw, v1.zwzz, t1.xwyz, s1
      add r3.xzw, r3.xxzw, r3.xxzw
      mul r4.x, v2.z, l(0.000977)
      mad r4.y, -v2.z, l(0.000977), l(1.000000)
      mad r3.xzw, r3.xxzw, r4.yyyy, r4.xxxx
      add r3.x, r3.z, r3.x
      add r3.x, r3.w, r3.x
      mul r3.x, r3.x, l(0.333333)
      mul r1.xyz, r1.xyzx, r3.xxxx
    endif 
  endif 
  if_nz r3.y
    sample r3.xyz, v2.xyxx, t2.xyzw, s2
    mul r3.xyz, r1.xyzx, r3.xyzx
    add r1.xyz, r3.xyzx, r3.xyzx
  endif 
  mov r0.w, v3.w
  mul r0.xyzw, r0.xyzw, r1.xyzw
  ieq r1.x, l(30), cb0[6].w
  not r1.y, r1.x
  and r1.z, l(1), cb2[2].x
  ine r1.w, r1.z, l(0)
  and r1.y, r1.w, r1.y
  lt r1.w, r0.w, cb2[0].y
  and r1.y, r1.w, r1.y
  discard_nz r1.y
  movc r0.w, r1.z, r0.w, l(1.000000)
  movc r2.xyzw, r1.xxxx, v3.xyzw, r0.xyzw
  ieq r0.x, l(29), cb0[6].w
  div r0.y, v2.z, cb0[7].x
  movc r0.xyz, r0.xxxx, r0.yyyy, r2.xyzx
  add r0.xyz, r0.xyzx, v4.xyzx
  mad r0.w, v2.z, -cb1[1].x, cb1[1].y
  mad_sat r0.w, -r0.w, cb1[1].z, cb1[1].w
  ge r1.x, cb1[1].y, v2.z
  movc r0.w, r1.x, l(0), r0.w
  ge r1.x, l(0.000000), r0.w
  add r1.yzw, -r0.xxyz, cb1[0].xxyz
  mul r1.yzw, r1.yyzw, cb1[0].wwww
  mad r1.yzw, r1.yyzw, r0.wwww, r0.xxyz
  movc r1.xyz, r1.xxxx, r0.xyzx, r1.yzwy
  movc r2.xyz, cb1[2].xxxx, r1.xyzx, r0.xyzx
endif 
lt r0.x, r2.w, cb2[0].y
mov r1.xyz, v3.xyzx
mov r1.w, l(1.000000)
movc r0.xyzw, r0.xxxx, l(0,0,0,0), r1.xyzw
ieq r1.xy, l(29, 30, 0, 0), cb0[6].wwww
or r1.x, r1.y, r1.x
and r1.y, l(1), cb0[5].z
mov_sat r3.xyz, r2.xyzx
movc r1.yzw, r1.yyyy, r2.xxyz, r3.xxyz
ge r3.x, l(0.000000), cb2[0].w
add r3.y, r1.z, r1.y
add r3.y, r1.w, r3.y
mad r3.yzw, r3.yyyy, l(0.000000, 0.333333, 0.333333, 0.333333), -r1.yyzw
mad r3.yzw, cb2[0].wwww, r3.yyzw, r1.yyzw
movc r3.xyz, r3.xxxx, r1.yzwy, r3.yzwy
mov r3.w, r2.w
movc r1.xyzw, r1.xxxx, r2.xyzw, r3.xyzw
movc o0.xyzw, cb0[4].wwww, r0.xyzw, r1.xyzw
ret 
// Approximately 84 instruction slots used
#endif

const BYTE g_PxShader[] =
{
     68,  88,  66,  67, 153, 105, 
    122,  78,  64, 169, 178, 227, 
     26, 232,  83, 255, 248,  88, 
    134, 189,   1,   0,   0,   0, 
    116,  19,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    100,   7,   0,   0,  56,   8, 
      0,   0, 108,   8,   0,   0, 
    248,  18,   0,   0,  82,  68, 
     69,  70,  40,   7,   0,   0, 
      4,   0,   0,   0, 208,   1, 
      0,   0,  10,   0,   0,   0, 
     28,   0,   0,   0,   1,   4, 
    255, 255,   0,   1,   0,   0, 
    254,   6,   0,   0,  92,   1, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    102,   1,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 114,   1,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 125,   1, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
    133,   1,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   1,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0, 140,   1,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   2,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0, 146,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    161,   1,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 175,   1,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   2,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0, 188,   1, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     68, 105, 102, 102,  83, 116, 
     97, 116, 101,   0,  68, 101, 
    116,  97, 105, 108,  83, 116, 
     97, 116, 101,   0,  77,  97, 
     99, 114, 111,  83, 116,  97, 
    116, 101,   0,  68, 105, 102, 
    102, 117, 115, 101,   0,  68, 
    101, 116,  97, 105, 108,   0, 
     77,  97,  99, 114, 111,   0, 
     70, 114,  97, 109, 101,  86, 
     97, 114, 105,  97,  98, 108, 
    101, 115,   0,  68,  70, 111, 
    103,  86,  97, 114, 105,  97, 
     98, 108, 101, 115,   0,  80, 
    111, 108, 121, 102, 108,  97, 
    103,  86,  97, 114, 115,   0, 
     84, 101, 120, 116, 117, 114, 
    101,  86,  97, 114, 105,  97, 
     98, 108, 101, 115,   0, 171, 
    171, 171, 146,   1,   0,   0, 
     16,   0,   0,   0,  48,   2, 
      0,   0, 128,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 175,   1,   0,   0, 
      8,   0,   0,   0, 212,   4, 
      0,   0,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 188,   1,   0,   0, 
      2,   0,   0,   0,   4,   6, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 161,   1,   0,   0, 
      4,   0,   0,   0,  80,   6, 
      0,   0,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 176,   3,   0,   0, 
      0,   0,   0,   0,  64,   0, 
      0,   0,   0,   0,   0,   0, 
    184,   3,   0,   0,   0,   0, 
      0,   0, 200,   3,   0,   0, 
     64,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    208,   3,   0,   0,   0,   0, 
      0,   0, 224,   3,   0,   0, 
     68,   0,   0,   0,   8,   0, 
      0,   0,   0,   0,   0,   0, 
    236,   3,   0,   0,   0,   0, 
      0,   0, 252,   3,   0,   0, 
     76,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
     12,   4,   0,   0,   0,   0, 
      0,   0,  28,   4,   0,   0, 
     80,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
     12,   4,   0,   0,   0,   0, 
      0,   0,  43,   4,   0,   0, 
     84,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
     12,   4,   0,   0,   0,   0, 
      0,   0,  55,   4,   0,   0, 
     88,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
     72,   4,   0,   0,   0,   0, 
      0,   0,  88,   4,   0,   0, 
     92,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
     12,   4,   0,   0,   0,   0, 
      0,   0,  98,   4,   0,   0, 
     96,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    208,   3,   0,   0,   0,   0, 
      0,   0, 111,   4,   0,   0, 
    100,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    208,   3,   0,   0,   0,   0, 
      0,   0, 127,   4,   0,   0, 
    104,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    208,   3,   0,   0,   0,   0, 
      0,   0, 138,   4,   0,   0, 
    108,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
     12,   4,   0,   0,   0,   0, 
      0,   0, 146,   4,   0,   0, 
    112,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    208,   3,   0,   0,   0,   0, 
      0,   0, 161,   4,   0,   0, 
    116,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    208,   3,   0,   0,   0,   0, 
      0,   0, 176,   4,   0,   0, 
    120,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    208,   3,   0,   0,   0,   0, 
      0,   0, 192,   4,   0,   0, 
    124,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    208,   3,   0,   0,   0,   0, 
      0,   0,  80, 114, 111, 106, 
      0, 171, 171, 171,   3,   0, 
      3,   0,   4,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  71,  97, 109, 109, 
     97,   0, 171, 171,   0,   0, 
      3,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  86, 105, 101, 119, 
     83, 105, 122, 101,   0, 171, 
    171, 171,   1,   0,   3,   0, 
      1,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     98,  68, 111,  83, 101, 108, 
    101,  99, 116, 105, 111, 110, 
      0, 171, 171, 171,   0,   0, 
      2,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  98,  79, 110, 101, 
     88,  76, 105, 103, 104, 116, 
    109,  97, 112, 115,   0,  98, 
     67, 111, 114, 114, 101,  99, 
    116,  70, 111, 103,   0,  70, 
    114,  97, 109, 101,  83, 104, 
     97, 100, 101, 114,  70, 108, 
     97, 103, 115,   0,   0,   0, 
     19,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  71,  97, 109, 109, 
     97,  77, 111, 100, 101,   0, 
     72,  68,  82,  69, 120, 112, 
     97, 110, 115, 105, 111, 110, 
      0,  82, 101, 115, 111, 108, 
    117, 116, 105, 111, 110,  83, 
     99,  97, 108, 101,   0,  87, 
    104, 105, 116, 101,  76, 101, 
    118, 101, 108,   0,  82, 101, 
    110, 100,  77,  97, 112,   0, 
     68, 101, 112, 116, 104,  68, 
    114,  97, 119,  76, 105, 109, 
    105, 116,   0,  71,  97, 109, 
    109,  97,  79, 102, 102, 115, 
    101, 116,  82, 101, 100,   0, 
     71,  97, 109, 109,  97,  79, 
    102, 102, 115, 101, 116,  66, 
    108, 117, 101,   0,  71,  97, 
    109, 109,  97,  79, 102, 102, 
    115, 101, 116,  71, 114, 101, 
    101, 110,   0, 171, 171, 171, 
    148,   5,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0,  12,   4, 
      0,   0,   0,   0,   0,   0, 
    158,   5,   0,   0,   4,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0, 208,   3, 
      0,   0,   0,   0,   0,   0, 
    170,   5,   0,   0,   8,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 208,   3, 
      0,   0,   0,   0,   0,   0, 
    185,   5,   0,   0,  12,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0, 208,   3, 
      0,   0,   0,   0,   0,   0, 
    195,   5,   0,   0,  16,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0, 212,   5, 
      0,   0,   0,   0,   0,   0, 
    228,   5,   0,   0,  28,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 208,   3, 
      0,   0,   0,   0,   0,   0, 
    238,   5,   0,   0,  32,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0,  72,   4, 
      0,   0,   0,   0,   0,   0, 
    250,   5,   0,   0,  36,   0, 
      0,   0,  12,   0,   0,   0, 
      0,   0,   0,   0, 212,   5, 
      0,   0,   0,   0,   0,   0, 
     98,  83, 101, 108, 101,  99, 
    116, 101, 100,   0,  65, 108, 
    112, 104,  97,  82, 101, 106, 
    101,  99, 116,   0,  65, 108, 
    116,  65, 108, 112, 104,  97, 
     82, 101, 106, 101,  99, 116, 
      0,  66,  87,  80, 101, 114, 
     99, 101, 110, 116,   0,  83, 
    101, 108, 101,  99, 116, 101, 
    100,  67, 111, 108, 111, 114, 
      0, 171, 171, 171,   1,   0, 
      3,   0,   1,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  80,  97, 100,  83, 
    101, 108, 101,  99, 116,   0, 
     83, 104,  97, 100, 101, 114, 
     70, 108,  97, 103, 115,   0, 
     80,  97, 100,  70, 108,  97, 
    103, 115,   0, 171,  52,   6, 
      0,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  72,   4,   0,   0, 
      0,   0,   0,   0,  67,   6, 
      0,   0,  16,   0,   0,   0, 
     12,   0,   0,   0,   0,   0, 
      0,   0, 212,   5,   0,   0, 
      0,   0,   0,   0,  98,  84, 
    101, 120, 116, 117, 114, 101, 
    115,  66, 111, 117, 110, 100, 
      0,  84, 101, 120, 116, 117, 
    114, 101,  80,  97, 100,   0, 
    171, 171, 176,   6,   0,   0, 
      0,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    196,   6,   0,   0,   0,   0, 
      0,   0, 212,   6,   0,   0, 
     16,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    196,   6,   0,   0,   0,   0, 
      0,   0, 232,   6,   0,   0, 
     32,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
     12,   4,   0,   0,   0,   0, 
      0,   0, 247,   6,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    212,   5,   0,   0,   0,   0, 
      0,   0,  68, 105, 115, 116, 
     97, 110,  99, 101,  70, 111, 
    103,  67, 111, 108, 111, 114, 
      0, 171, 171, 171,   1,   0, 
      3,   0,   1,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  68, 105, 115, 116, 
     97, 110,  99, 101,  70, 111, 
    103,  83, 101, 116, 116, 105, 
    110, 103, 115,   0,  98,  68, 
    111,  68, 105, 115, 116,  97, 
    110,  99, 101,  70, 111, 103, 
      0,  80,  97, 100, 100, 121, 
     51,   0,  77, 105,  99, 114, 
    111, 115, 111, 102, 116,  32, 
     40,  82,  41,  32,  72,  76, 
     83,  76,  32,  83, 104,  97, 
    100, 101, 114,  32,  67, 111, 
    109, 112, 105, 108, 101, 114, 
     32,  49,  48,  46,  49,   0, 
    171, 171,  73,  83,  71,  78, 
    204,   0,   0,   0,   7,   0, 
      0,   0,   8,   0,   0,   0, 
    176,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
    188,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   3,   3,   0,   0, 
    188,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  12,  12,   0,   0, 
    188,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   3,   3,   0,   0, 
    197,   0,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   4,   4,   0,   0, 
    197,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,  15,  15,   0,   0, 
    197,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   4,   0, 
      0,   0,  15,  15,   0,   0, 
     83,  86,  95,  80,  79,  83, 
     73,  84,  73,  79,  78,   0, 
     84,  69,  88,  67,  79,  79, 
     82,  68,   0,  67,  79,  76, 
     79,  82,   0, 171,  79,  83, 
     71,  78,  44,   0,   0,   0, 
      1,   0,   0,   0,   8,   0, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  15,   0, 
      0,   0,  83,  86,  95,  84, 
     65,  82,  71,  69,  84,   0, 
    171, 171,  83,  72,  68,  82, 
    132,  10,   0,   0,  65,   0, 
      0,   0, 161,   2,   0,   0, 
    106,   8,   0,   1,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  89,   0,   0,   4, 
     70, 142,  32,   0,   2,   0, 
      0,   0,   3,   0,   0,   0, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   1,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   2,   0, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   0,   0, 
      0,   0,  85,  85,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   1,   0,   0,   0, 
     85,  85,   0,   0,  88,  24, 
      0,   4,   0, 112,  16,   0, 
      2,   0,   0,   0,  85,  85, 
      0,   0,  98,  16,   0,   3, 
     50,  16,  16,   0,   1,   0, 
      0,   0,  98,  16,   0,   3, 
    194,  16,  16,   0,   1,   0, 
      0,   0,  98,  16,   0,   3, 
     50,  16,  16,   0,   2,   0, 
      0,   0,  98,  16,   0,   3, 
     66,  16,  16,   0,   2,   0, 
      0,   0,  98,  16,   0,   3, 
    242,  16,  16,   0,   3,   0, 
      0,   0,  98,  16,   0,   3, 
    242,  16,  16,   0,   4,   0, 
      0,   0, 101,   0,   0,   3, 
    242,  32,  16,   0,   0,   0, 
      0,   0, 104,   0,   0,   2, 
      5,   0,   0,   0,   0,   0, 
      0,   8,  18,   0,  16,   0, 
      0,   0,   0,   0,  58,  16, 
     16, 128,  65,   0,   0,   0, 
      4,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   0,  16,   0,   0,   0, 
      0,   0,  70,  18,  16,   0, 
      3,   0,   0,   0,  55,   0, 
      0,  10, 114,   0,  16,   0, 
      0,   0,   0,   0,  86, 133, 
     32,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     70,  18,  16,   0,   3,   0, 
      0,   0,  69,   0,   0,   9, 
    242,   0,  16,   0,   1,   0, 
      0,   0,  70,  16,  16,   0, 
      1,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  32,   0,   0,   8, 
     18,   0,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
     20,   0,   0,   0,  58, 128, 
     32,   0,   0,   0,   0,   0, 
      6,   0,   0,   0,  31,   0, 
      4,   3,  10,   0,  16,   0, 
      2,   0,   0,   0,  56,   0, 
      0,  10, 114,   0,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,  63,   0,   0,   0,  63, 
      0,   0,   0,  63,   0,   0, 
      0,   0,   1,   0,   0,   8, 
     18,   0,  16,   0,   3,   0, 
      0,   0,   1,  64,   0,   0, 
      1,   0,   0,   0,  10, 128, 
     32,   0,   2,   0,   0,   0, 
      2,   0,   0,   0,  39,   0, 
      0,   7,  34,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   3,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  49,   0,   0,   8, 
     66,   0,  16,   0,   3,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  26, 128, 
     32,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   7,  34,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,  13,   0,   4,   3, 
     26,   0,  16,   0,   3,   0, 
      0,   0,  55,   0,   0,   9, 
    130,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  18,   0,   0,   1, 
      0,   0,   0,   7,  18,   0, 
     16,   0,   3,   0,   0,   0, 
     26,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   7,  18,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   3,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   3,   0, 
      0,   0,  10,   0,  16,   0, 
      3,   0,   0,   0,   1,  64, 
      0,   0, 171, 170, 170,  62, 
     55,   0,   0,  10, 114,   0, 
     16,   0,   1,   0,   0,   0, 
      6, 128,  32,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      6,   0,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,   1,   0, 
      0,  11,  50,   0,  16,   0, 
      3,   0,   0,   0,   2,  64, 
      0,   0,   2,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      6, 128,  32,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     31,   0,   4,   3,  10,   0, 
     16,   0,   3,   0,   0,   0, 
     49,   0,   0,   7,  18,   0, 
     16,   0,   3,   0,   0,   0, 
     42,  16,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  68,  31,   0, 
      4,   3,  10,   0,  16,   0, 
      3,   0,   0,   0,  69,   0, 
      0,   9, 210,   0,  16,   0, 
      3,   0,   0,   0, 230,  26, 
     16,   0,   1,   0,   0,   0, 
    198, 121,  16,   0,   1,   0, 
      0,   0,   0,  96,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   7, 210,   0,  16,   0, 
      3,   0,   0,   0,   6,  14, 
     16,   0,   3,   0,   0,   0, 
      6,  14,  16,   0,   3,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   4,   0, 
      0,   0,  42,  16,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  58, 
     50,   0,   0,  10,  34,   0, 
     16,   0,   4,   0,   0,   0, 
     42,  16,  16, 128,  65,   0, 
      0,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  58,   1,  64,   0,   0, 
      0,   0, 128,  63,  50,   0, 
      0,   9, 210,   0,  16,   0, 
      3,   0,   0,   0,   6,  14, 
     16,   0,   3,   0,   0,   0, 
     86,   5,  16,   0,   4,   0, 
      0,   0,   6,   0,  16,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   7,  18,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   3,   0, 
      0,   0,   0,   0,   0,   7, 
     18,   0,  16,   0,   3,   0, 
      0,   0,  58,   0,  16,   0, 
      3,   0,   0,   0,  10,   0, 
     16,   0,   3,   0,   0,   0, 
     56,   0,   0,   7,  18,   0, 
     16,   0,   3,   0,   0,   0, 
     10,   0,  16,   0,   3,   0, 
      0,   0,   1,  64,   0,   0, 
    171, 170, 170,  62,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
      6,   0,  16,   0,   3,   0, 
      0,   0,  21,   0,   0,   1, 
     21,   0,   0,   1,  31,   0, 
      4,   3,  26,   0,  16,   0, 
      3,   0,   0,   0,  69,   0, 
      0,   9, 114,   0,  16,   0, 
      3,   0,   0,   0,  70,  16, 
     16,   0,   2,   0,   0,   0, 
     70, 126,  16,   0,   2,   0, 
      0,   0,   0,  96,  16,   0, 
      2,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,   0,   0,   0,   7, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
     21,   0,   0,   1,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      0,   0,   0,   0,  58,  16, 
     16,   0,   3,   0,   0,   0, 
     56,   0,   0,   7, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  32,   0, 
      0,   8,  18,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,  30,   0,   0,   0, 
     58, 128,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
     59,   0,   0,   5,  34,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,   1,   0,   0,   8, 
     66,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
      1,   0,   0,   0,  10, 128, 
     32,   0,   2,   0,   0,   0, 
      2,   0,   0,   0,  39,   0, 
      0,   7, 130,   0,  16,   0, 
      1,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   7, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
     49,   0,   0,   8, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  26, 128,  32,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   7, 
     34,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
     13,   0,   4,   3,  26,   0, 
     16,   0,   1,   0,   0,   0, 
     55,   0,   0,   9, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     42,   0,  16,   0,   1,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     55,   0,   0,   9, 242,   0, 
     16,   0,   2,   0,   0,   0, 
      6,   0,  16,   0,   1,   0, 
      0,   0,  70,  30,  16,   0, 
      3,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     32,   0,   0,   8,  18,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,  29,   0, 
      0,   0,  58, 128,  32,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,  14,   0,   0,   8, 
     34,   0,  16,   0,   0,   0, 
      0,   0,  42,  16,  16,   0, 
      2,   0,   0,   0,  10, 128, 
     32,   0,   0,   0,   0,   0, 
      7,   0,   0,   0,  55,   0, 
      0,   9, 114,   0,  16,   0, 
      0,   0,   0,   0,   6,   0, 
     16,   0,   0,   0,   0,   0, 
     86,   5,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   7, 114,   0,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     70,  18,  16,   0,   4,   0, 
      0,   0,  50,   0,   0,  12, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  42,  16,  16,   0, 
      2,   0,   0,   0,  10, 128, 
     32, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  26, 128,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  50,  32,   0,  12, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,  42, 128,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  58, 128,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  29,   0,   0,   8, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  26, 128,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  42,  16,  16,   0, 
      2,   0,   0,   0,  55,   0, 
      0,   9, 130,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  29,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   9, 
    226,   0,  16,   0,   1,   0, 
      0,   0,   6,   9,  16, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   6, 137,  32,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  56,   0,   0,   8, 
    226,   0,  16,   0,   1,   0, 
      0,   0,  86,  14,  16,   0, 
      1,   0,   0,   0, 246, 143, 
     32,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,   9, 226,   0,  16,   0, 
      1,   0,   0,   0,  86,  14, 
     16,   0,   1,   0,   0,   0, 
    246,  15,  16,   0,   0,   0, 
      0,   0,   6,   9,  16,   0, 
      0,   0,   0,   0,  55,   0, 
      0,   9, 114,   0,  16,   0, 
      1,   0,   0,   0,   6,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0, 150,   7,  16,   0, 
      1,   0,   0,   0,  55,   0, 
      0,  10, 114,   0,  16,   0, 
      2,   0,   0,   0,   6, 128, 
     32,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  21,   0,   0,   1, 
     49,   0,   0,   8,  18,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   2,   0, 
      0,   0,  26, 128,  32,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,  54,   0,   0,   5, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,  18,  16,   0, 
      3,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     55,   0,   0,  12, 242,   0, 
     16,   0,   0,   0,   0,   0, 
      6,   0,  16,   0,   0,   0, 
      0,   0,   2,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  70,  14, 
     16,   0,   1,   0,   0,   0, 
     32,   0,   0,  11,  50,   0, 
     16,   0,   1,   0,   0,   0, 
      2,  64,   0,   0,  29,   0, 
      0,   0,  30,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 246, 143,  32,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,  60,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      1,   0,   0,   0,  10,   0, 
     16,   0,   1,   0,   0,   0, 
      1,   0,   0,   8,  34,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   1,   0, 
      0,   0,  42, 128,  32,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  54,  32,   0,   5, 
    114,   0,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  55,   0, 
      0,   9, 226,   0,  16,   0, 
      1,   0,   0,   0,  86,   5, 
     16,   0,   1,   0,   0,   0, 
      6,   9,  16,   0,   2,   0, 
      0,   0,   6,   9,  16,   0, 
      3,   0,   0,   0,  29,   0, 
      0,   8,  18,   0,  16,   0, 
      3,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     58, 128,  32,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   7,  34,   0, 
     16,   0,   3,   0,   0,   0, 
     42,   0,  16,   0,   1,   0, 
      0,   0,  26,   0,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   7,  34,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     26,   0,  16,   0,   3,   0, 
      0,   0,  50,   0,   0,  13, 
    226,   0,  16,   0,   3,   0, 
      0,   0,  86,   5,  16,   0, 
      3,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
    171, 170, 170,  62, 171, 170, 
    170,  62, 171, 170, 170,  62, 
     86,  14,  16, 128,  65,   0, 
      0,   0,   1,   0,   0,   0, 
     50,   0,   0,  10, 226,   0, 
     16,   0,   3,   0,   0,   0, 
    246, 143,  32,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
     86,  14,  16,   0,   3,   0, 
      0,   0,  86,  14,  16,   0, 
      1,   0,   0,   0,  55,   0, 
      0,   9, 114,   0,  16,   0, 
      3,   0,   0,   0,   6,   0, 
     16,   0,   3,   0,   0,   0, 
    150,   7,  16,   0,   1,   0, 
      0,   0, 150,   7,  16,   0, 
      3,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      3,   0,   0,   0,  58,   0, 
     16,   0,   2,   0,   0,   0, 
     55,   0,   0,   9, 242,   0, 
     16,   0,   1,   0,   0,   0, 
      6,   0,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  70,  14, 
     16,   0,   3,   0,   0,   0, 
     55,   0,   0,  10, 242,  32, 
     16,   0,   0,   0,   0,   0, 
    246, 143,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     70,  14,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      1,   0,   0,   0,  62,   0, 
      0,   1,  83,  84,  65,  84, 
    116,   0,   0,   0,  84,   0, 
      0,   0,   5,   0,   0,   0, 
      0,   0,   0,   0,   7,   0, 
      0,   0,  35,   0,   0,   0, 
      6,   0,   0,   0,   9,   0, 
      0,   0,   2,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
     14,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0
};
