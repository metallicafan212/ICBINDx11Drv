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
//   int bOneXLightmaps;                // Offset:   80 Size:     4
//   int bCorrectFog;                   // Offset:   84 Size:     4 [unused]
//   int bHDR;                          // Offset:   88 Size:     4 [unused]
//   int GammaMode;                     // Offset:   92 Size:     4 [unused]
//   float HDRExpansion;                // Offset:   96 Size:     4 [unused]
//   float ResolutionScale;             // Offset:  100 Size:     4 [unused]
//   float WhiteLevel;                  // Offset:  104 Size:     4 [unused]
//   int bDepthDraw;                    // Offset:  108 Size:     4
//   float DepthDrawLimit;              // Offset:  112 Size:     4
//   float GammaOffsetRed;              // Offset:  116 Size:     4 [unused]
//   float GammaOffsetBlue;             // Offset:  120 Size:     4 [unused]
//   float GammaOffsetGreen;            // Offset:  124 Size:     4 [unused]
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
// cbuffer PolyflagVars
// {
//
//   int bColorMasked;                  // Offset:    0 Size:     4 [unused]
//   float AlphaReject;                 // Offset:    4 Size:     4
//   float BWPercent;                   // Offset:    8 Size:     4
//   int bAlphaEnabled;                 // Offset:   12 Size:     4
//   int bModulated;                    // Offset:   16 Size:     4 [unused]
//   float3 Pad;                        // Offset:   20 Size:    12 [unused]
//
// }
//
// cbuffer TextureVariables
// {
//
//   int4 bTexturesBound[4];            // Offset:    0 Size:    64
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// DiffState                         sampler      NA          NA             s0      1 
// LightState                        sampler      NA          NA             s1      1 
// MacroState                        sampler      NA          NA             s2      1 
// FogState                          sampler      NA          NA             s3      1 
// DetailState                       sampler      NA          NA             s4      1 
// Diffuse                           texture  float4          2d             t0      1 
// Light                             texture  float4          2d             t1      1 
// Macro                             texture  float4          2d             t2      1 
// Fogmap                            texture  float4          2d             t3      1 
// Detail                            texture  float4          2d             t4      1 
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
// TEXCOORD                 3     zw        2     NONE   float     zw
// TEXCOORD                 4   xyzw        3     NONE   float   xyz 
// COLOR                    0   xyzw        4     NONE   float   xyzw
// COLOR                    1   xyzw        5     NONE   float       
// COLOR                    2   x           6     NONE   float   x   
// COLOR                    3    yz         6     NONE   float       
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
dcl_constantbuffer CB1[3], immediateIndexed
dcl_constantbuffer CB2[1], immediateIndexed
dcl_constantbuffer CB3[2], immediateIndexed
dcl_sampler s0, mode_default
dcl_sampler s1, mode_default
dcl_sampler s2, mode_default
dcl_sampler s3, mode_default
dcl_sampler s4, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_resource_texture2d (float,float,float,float) t1
dcl_resource_texture2d (float,float,float,float) t2
dcl_resource_texture2d (float,float,float,float) t3
dcl_resource_texture2d (float,float,float,float) t4
dcl_input_ps linear v1.xy
dcl_input_ps linear v1.zw
dcl_input_ps linear v2.xy
dcl_input_ps linear v2.zw
dcl_input_ps linear v3.xyz
dcl_input_ps linear v4.xyzw
dcl_input_ps linear v6.x
dcl_output o0.xyzw
dcl_temps 4
if_nz cb3[0].x
  sample_b_indexable(texture2d)(float,float,float,float) r0.xyzw, v1.xyxx, t0.xyzw, s0, l(0.000000)
  mul r0.xyzw, r0.xyzw, v4.xyzw
else 
  mov r0.xyzw, v4.xyzw
endif 
ine r1.x, l(0), cb3[1].x
lt r1.y, v3.z, l(380.000000)
and r1.x, r1.y, r1.x
if_nz r1.x
  sample_b_indexable(texture2d)(float,float,float,float) r1.xyz, v3.xyxx, t4.xyzw, s4, l(0.000000)
  add r1.xyz, r1.xyzx, r1.xyzx
  mul r1.w, v3.z, l(0.002632)
  mad r2.x, -v3.z, l(0.002632), l(1.000000)
  mad r1.xyz, r1.xyzx, r2.xxxx, r1.wwww
  add r1.x, r1.y, r1.x
  add r1.x, r1.z, r1.x
  mul r1.x, r1.x, l(0.333333)
  mul r0.xyz, r0.xyzx, r1.xxxx
endif 
if_nz cb3[0].z
  sample_b_indexable(texture2d)(float,float,float,float) r1.xyz, v2.xyxx, t2.xyzw, s2, l(0.000000)
  mul r1.xyz, r0.xyzx, r1.xyzx
  add r0.xyz, r1.xyzx, r1.xyzx
endif 
if_nz cb3[0].y
  sample_b_indexable(texture2d)(float,float,float,float) r1.xyz, v1.zwzz, t1.xyzw, s1, l(0.000000)
  mul r1.xyz, r0.xyzx, r1.xyzx
  add r2.xyz, r1.xyzx, r1.xyzx
  movc r0.xyz, cb0[5].xxxx, r1.xyzx, r2.xyzx
endif 
if_nz cb3[0].w
  sample_b_indexable(texture2d)(float,float,float,float) r1.xyzw, v2.zwzz, t3.xyzw, s3, l(0.000000)
  add r1.w, -r1.w, l(1.000000)
  mad r0.xyz, r0.xyzx, r1.wwww, r1.xyzx
endif 
movc r1.w, cb2[0].w, r0.w, l(1.000000)
add r0.w, r1.w, -cb2[0].y
lt r0.w, r0.w, l(0.000000)
discard_nz r0.w
div r0.w, v6.x, cb0[7].x
movc r0.xyz, cb0[6].wwww, r0.wwww, r0.xyzx
mad r0.w, v6.x, -cb1[1].x, cb1[1].y
mad_sat r0.w, -r0.w, cb1[1].z, cb1[1].w
ge r2.x, cb1[1].y, v6.x
movc r0.w, r2.x, l(0), r0.w
ge r2.x, l(0.000000), r0.w
add r2.yzw, -r0.xxyz, cb1[0].xxyz
mul r2.yzw, r2.yyzw, cb1[0].wwww
mad r2.yzw, r2.yyzw, r0.wwww, r0.xxyz
movc r2.xyz, r2.xxxx, r0.xyzx, r2.yzwy
movc r1.xyz, cb1[2].xxxx, r2.xyzx, r0.xyzx
lt r0.x, r1.w, cb2[0].y
mov r2.xyz, v4.xyzx
mov r2.w, l(1.000000)
movc r0.xyzw, r0.xxxx, l(0,0,0,0), r2.xyzw
mov_sat r2.xyz, r1.xyzx
ge r2.w, l(0.000000), cb2[0].z
add r3.x, r2.y, r2.x
add r3.x, r2.z, r3.x
mad r3.xyz, r3.xxxx, l(0.333333, 0.333333, 0.333333, 0.000000), -r2.xyzx
mad r3.xyz, cb2[0].zzzz, r3.xyzx, r2.xyzx
movc r2.xyz, r2.wwww, r2.xyzx, r3.xyzx
mov r2.w, r1.w
movc r1.xyzw, cb0[6].wwww, r1.xyzw, r2.xyzw
movc o0.xyzw, cb0[4].wwww, r0.xyzw, r1.xyzw
ret 
// Approximately 67 instruction slots used
#endif

const BYTE g_PxShader[] =
{
     68,  88,  66,  67, 142, 181, 
    117,  88,  11,  83, 234,  75, 
    212, 121,  60, 208, 110, 160, 
      4, 239,   1,   0,   0,   0, 
    244,  20,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
      4,  10,   0,   0,  32,  11, 
      0,   0,  84,  11,   0,   0, 
     88,  20,   0,   0,  82,  68, 
     69,  70, 200,   9,   0,   0, 
      4,   0,   0,   0, 144,   2, 
      0,   0,  14,   0,   0,   0, 
     60,   0,   0,   0,   0,   5, 
    255, 255,   0,   1,   0,   0, 
    160,   9,   0,   0,  82,  68, 
     49,  49,  60,   0,   0,   0, 
     24,   0,   0,   0,  32,   0, 
      0,   0,  40,   0,   0,   0, 
     36,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    252,   1,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   6,   2,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  17,   2, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
     28,   2,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   3,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  37,   2,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  49,   2, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
     57,   2,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   1,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0,  63,   2,   0,   0, 
      2,   0,   0,   0,   5,   0, 
      0,   0,   4,   0,   0,   0, 
    255, 255, 255, 255,   2,   0, 
      0,   0,   1,   0,   0,   0, 
     13,   0,   0,   0,  69,   2, 
      0,   0,   2,   0,   0,   0, 
      5,   0,   0,   0,   4,   0, 
      0,   0, 255, 255, 255, 255, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  13,   0,   0,   0, 
     76,   2,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   4,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0,  83,   2,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  98,   2, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    112,   2,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0, 125,   2,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  68, 105, 
    102, 102,  83, 116,  97, 116, 
    101,   0,  76, 105, 103, 104, 
    116,  83, 116,  97, 116, 101, 
      0,  77,  97,  99, 114, 111, 
     83, 116,  97, 116, 101,   0, 
     70, 111, 103,  83, 116,  97, 
    116, 101,   0,  68, 101, 116, 
     97, 105, 108,  83, 116,  97, 
    116, 101,   0,  68, 105, 102, 
    102, 117, 115, 101,   0,  76, 
    105, 103, 104, 116,   0,  77, 
     97,  99, 114, 111,   0,  70, 
    111, 103, 109,  97, 112,   0, 
     68, 101, 116,  97, 105, 108, 
      0,  70, 114,  97, 109, 101, 
     86,  97, 114, 105,  97,  98, 
    108, 101, 115,   0,  68,  70, 
    111, 103,  86,  97, 114, 105, 
     97,  98, 108, 101, 115,   0, 
     80, 111, 108, 121, 102, 108, 
     97, 103,  86,  97, 114, 115, 
      0,  84, 101, 120, 116, 117, 
    114, 101,  86,  97, 114, 105, 
     97,  98, 108, 101, 115,   0, 
    171, 171,  83,   2,   0,   0, 
     16,   0,   0,   0, 240,   2, 
      0,   0, 128,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  98,   2,   0,   0, 
      4,   0,   0,   0, 220,   6, 
      0,   0,  48,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 112,   2,   0,   0, 
      6,   0,   0,   0,  16,   8, 
      0,   0,  32,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 125,   2,   0,   0, 
      1,   0,   0,   0,  64,   9, 
      0,   0,  64,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 112,   5,   0,   0, 
      0,   0,   0,   0,  64,   0, 
      0,   0,   0,   0,   0,   0, 
    128,   5,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    164,   5,   0,   0,  64,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 176,   5, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 212,   5, 
      0,   0,  68,   0,   0,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0, 228,   5,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,   8,   6,   0,   0, 
     76,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
     28,   6,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     64,   6,   0,   0,  80,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0,  28,   6, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  79,   6, 
      0,   0,  84,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  28,   6,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  91,   6,   0,   0, 
     88,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
     28,   6,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     96,   6,   0,   0,  92,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,  28,   6, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 106,   6, 
      0,   0,  96,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 176,   5,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 119,   6,   0,   0, 
    100,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    176,   5,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    135,   6,   0,   0, 104,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 176,   5, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 146,   6, 
      0,   0, 108,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  28,   6,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 157,   6,   0,   0, 
    112,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    176,   5,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    172,   6,   0,   0, 116,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0, 176,   5, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 187,   6, 
      0,   0, 120,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 176,   5,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 203,   6,   0,   0, 
    124,   0,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
    176,   5,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     80, 114, 111, 106,   0, 102, 
    108, 111,  97, 116,  52, 120, 
     52,   0, 171, 171,   3,   0, 
      3,   0,   4,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    117,   5,   0,   0,  71,  97, 
    109, 109,  97,   0, 102, 108, 
    111,  97, 116,   0,   0,   0, 
      3,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    170,   5,   0,   0,  86, 105, 
    101, 119,  83, 105, 122, 101, 
      0, 102, 108, 111,  97, 116, 
     50,   0,   1,   0,   3,   0, 
      1,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 221,   5, 
      0,   0,  98,  68, 111,  83, 
    101, 108, 101,  99, 116, 105, 
    111, 110,   0, 105, 110, 116, 
      0, 171, 171, 171,   0,   0, 
      2,   0,   1,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     21,   6,   0,   0,  98,  79, 
    110, 101,  88,  76, 105, 103, 
    104, 116, 109,  97, 112, 115, 
      0,  98,  67, 111, 114, 114, 
    101,  99, 116,  70, 111, 103, 
      0,  98,  72,  68,  82,   0, 
     71,  97, 109, 109,  97,  77, 
    111, 100, 101,   0,  72,  68, 
     82,  69, 120, 112,  97, 110, 
    115, 105, 111, 110,   0,  82, 
    101, 115, 111, 108, 117, 116, 
    105, 111, 110,  83,  99,  97, 
    108, 101,   0,  87, 104, 105, 
    116, 101,  76, 101, 118, 101, 
    108,   0,  98,  68, 101, 112, 
    116, 104,  68, 114,  97, 119, 
      0,  68, 101, 112, 116, 104, 
     68, 114,  97, 119,  76, 105, 
    109, 105, 116,   0,  71,  97, 
    109, 109,  97,  79, 102, 102, 
    115, 101, 116,  82, 101, 100, 
      0,  71,  97, 109, 109,  97, 
     79, 102, 102, 115, 101, 116, 
     66, 108, 117, 101,   0,  71, 
     97, 109, 109,  97,  79, 102, 
    102, 115, 101, 116,  71, 114, 
    101, 101, 110,   0, 124,   7, 
      0,   0,   0,   0,   0,   0, 
     16,   0,   0,   0,   2,   0, 
      0,   0, 148,   7,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 184,   7,   0,   0, 
     16,   0,   0,   0,  16,   0, 
      0,   0,   2,   0,   0,   0, 
    148,   7,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    204,   7,   0,   0,  32,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0,  28,   6, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 219,   7, 
      0,   0,  36,   0,   0,   0, 
     12,   0,   0,   0,   0,   0, 
      0,   0, 236,   7,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  68, 105, 115, 116, 
     97, 110,  99, 101,  70, 111, 
    103,  67, 111, 108, 111, 114, 
      0, 102, 108, 111,  97, 116, 
     52,   0,   1,   0,   3,   0, 
      1,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0, 141,   7, 
      0,   0,  68, 105, 115, 116, 
     97, 110,  99, 101,  70, 111, 
    103,  83, 101, 116, 116, 105, 
    110, 103, 115,   0,  98,  68, 
    111,  68, 105, 115, 116,  97, 
    110,  99, 101,  70, 111, 103, 
      0,  80,  97, 100, 100, 121, 
     51,   0, 102, 108, 111,  97, 
    116,  51,   0, 171, 171, 171, 
      1,   0,   3,   0,   1,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0, 226,   7,   0,   0, 
      0,   9,   0,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
      0,   0,   0,   0,  28,   6, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  13,   9, 
      0,   0,   4,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0, 176,   5,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  25,   9,   0,   0, 
      8,   0,   0,   0,   4,   0, 
      0,   0,   2,   0,   0,   0, 
    176,   5,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     35,   9,   0,   0,  12,   0, 
      0,   0,   4,   0,   0,   0, 
      2,   0,   0,   0,  28,   6, 
      0,   0,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0,  49,   9, 
      0,   0,  16,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  28,   6,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  60,   9,   0,   0, 
     20,   0,   0,   0,  12,   0, 
      0,   0,   0,   0,   0,   0, 
    236,   7,   0,   0,   0,   0, 
      0,   0, 255, 255, 255, 255, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
     98,  67, 111, 108, 111, 114, 
     77,  97, 115, 107, 101, 100, 
      0,  65, 108, 112, 104,  97, 
     82, 101, 106, 101,  99, 116, 
      0,  66,  87,  80, 101, 114, 
     99, 101, 110, 116,   0,  98, 
     65, 108, 112, 104,  97,  69, 
    110,  97,  98, 108, 101, 100, 
      0,  98,  77, 111, 100, 117, 
    108,  97, 116, 101, 100,   0, 
     80,  97, 100,   0, 104,   9, 
      0,   0,   0,   0,   0,   0, 
     64,   0,   0,   0,   2,   0, 
      0,   0, 124,   9,   0,   0, 
      0,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
    255, 255, 255, 255,   0,   0, 
      0,   0,  98,  84, 101, 120, 
    116, 117, 114, 101, 115,  66, 
    111, 117, 110, 100,   0, 105, 
    110, 116,  52,   0,   1,   0, 
      2,   0,   1,   0,   4,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    119,   9,   0,   0,  77, 105, 
     99, 114, 111, 115, 111, 102, 
    116,  32,  40,  82,  41,  32, 
     72,  76,  83,  76,  32,  83, 
    104,  97, 100, 101, 114,  32, 
     67, 111, 109, 112, 105, 108, 
    101, 114,  32,  49,  48,  46, 
     49,   0,  73,  83,  71,  78, 
     20,   1,   0,   0,  10,   0, 
      0,   0,   8,   0,   0,   0, 
    248,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  15,   0,   0,   0, 
      4,   1,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,   3,   3,   0,   0, 
      4,   1,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   1,   0, 
      0,   0,  12,  12,   0,   0, 
      4,   1,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,   3,   3,   0,   0, 
      4,   1,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,  12,  12,   0,   0, 
      4,   1,   0,   0,   4,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   3,   0, 
      0,   0,  15,   7,   0,   0, 
     13,   1,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   4,   0, 
      0,   0,  15,  15,   0,   0, 
     13,   1,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   5,   0, 
      0,   0,  15,   0,   0,   0, 
     13,   1,   0,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   6,   0, 
      0,   0,   1,   1,   0,   0, 
     13,   1,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
      3,   0,   0,   0,   6,   0, 
      0,   0,   6,   0,   0,   0, 
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
    171, 171,  83,  72,  69,  88, 
    252,   8,   0,   0,  80,   0, 
      0,   0,  63,   2,   0,   0, 
    106,   8,   0,   1,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      0,   0,   0,   0,   8,   0, 
      0,   0,  89,   0,   0,   4, 
     70, 142,  32,   0,   1,   0, 
      0,   0,   3,   0,   0,   0, 
     89,   0,   0,   4,  70, 142, 
     32,   0,   2,   0,   0,   0, 
      1,   0,   0,   0,  89,   0, 
      0,   4,  70, 142,  32,   0, 
      3,   0,   0,   0,   2,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   0,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   1,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   2,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   3,   0, 
      0,   0,  90,   0,   0,   3, 
      0,  96,  16,   0,   4,   0, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   0,   0, 
      0,   0,  85,  85,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   1,   0,   0,   0, 
     85,  85,   0,   0,  88,  24, 
      0,   4,   0, 112,  16,   0, 
      2,   0,   0,   0,  85,  85, 
      0,   0,  88,  24,   0,   4, 
      0, 112,  16,   0,   3,   0, 
      0,   0,  85,  85,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   4,   0,   0,   0, 
     85,  85,   0,   0,  98,  16, 
      0,   3,  50,  16,  16,   0, 
      1,   0,   0,   0,  98,  16, 
      0,   3, 194,  16,  16,   0, 
      1,   0,   0,   0,  98,  16, 
      0,   3,  50,  16,  16,   0, 
      2,   0,   0,   0,  98,  16, 
      0,   3, 194,  16,  16,   0, 
      2,   0,   0,   0,  98,  16, 
      0,   3, 114,  16,  16,   0, 
      3,   0,   0,   0,  98,  16, 
      0,   3, 242,  16,  16,   0, 
      4,   0,   0,   0,  98,  16, 
      0,   3,  18,  16,  16,   0, 
      6,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      0,   0,   0,   0, 104,   0, 
      0,   2,   4,   0,   0,   0, 
     31,   0,   4,   4,  10, 128, 
     32,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,  74,   0, 
      0, 141, 194,   0,   0, 128, 
     67,  85,  21,   0, 242,   0, 
     16,   0,   0,   0,   0,   0, 
     70,  16,  16,   0,   1,   0, 
      0,   0,  70, 126,  16,   0, 
      0,   0,   0,   0,   0,  96, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
    242,   0,  16,   0,   0,   0, 
      0,   0,  70,  14,  16,   0, 
      0,   0,   0,   0,  70,  30, 
     16,   0,   4,   0,   0,   0, 
     18,   0,   0,   1,  54,   0, 
      0,   5, 242,   0,  16,   0, 
      0,   0,   0,   0,  70,  30, 
     16,   0,   4,   0,   0,   0, 
     21,   0,   0,   1,  39,   0, 
      0,   8,  18,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     10, 128,  32,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
     49,   0,   0,   7,  34,   0, 
     16,   0,   1,   0,   0,   0, 
     42,  16,  16,   0,   3,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 190,  67,   1,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  31,   0,   4,   3, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  74,   0,   0, 141, 
    194,   0,   0, 128,  67,  85, 
     21,   0, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,  16, 
     16,   0,   3,   0,   0,   0, 
     70, 126,  16,   0,   4,   0, 
      0,   0,   0,  96,  16,   0, 
      4,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   7, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  56,   0, 
      0,   7, 130,   0,  16,   0, 
      1,   0,   0,   0,  42,  16, 
     16,   0,   3,   0,   0,   0, 
      1,  64,   0,   0, 146, 118, 
     44,  59,  50,   0,   0,  10, 
     18,   0,  16,   0,   2,   0, 
      0,   0,  42,  16,  16, 128, 
     65,   0,   0,   0,   3,   0, 
      0,   0,   1,  64,   0,   0, 
    146, 118,  44,  59,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     50,   0,   0,   9, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,   6,   0,  16,   0, 
      2,   0,   0,   0, 246,  15, 
     16,   0,   1,   0,   0,   0, 
      0,   0,   0,   7,  18,   0, 
     16,   0,   1,   0,   0,   0, 
     26,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   7,  18,   0,  16,   0, 
      1,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,  56,   0,   0,   7, 
     18,   0,  16,   0,   1,   0, 
      0,   0,  10,   0,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0, 171, 170, 170,  62, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,   6,   0,  16,   0, 
      1,   0,   0,   0,  21,   0, 
      0,   1,  31,   0,   4,   4, 
     42, 128,  32,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     74,   0,   0, 141, 194,   0, 
      0, 128,  67,  85,  21,   0, 
    114,   0,  16,   0,   1,   0, 
      0,   0,  70,  16,  16,   0, 
      2,   0,   0,   0,  70, 126, 
     16,   0,   2,   0,   0,   0, 
      0,  96,  16,   0,   2,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  56,   0, 
      0,   7, 114,   0,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,   0,   0,   0,   7, 
    114,   0,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     21,   0,   0,   1,  31,   0, 
      4,   4,  26, 128,  32,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  74,   0,   0, 141, 
    194,   0,   0, 128,  67,  85, 
     21,   0, 114,   0,  16,   0, 
      1,   0,   0,   0, 230,  26, 
     16,   0,   1,   0,   0,   0, 
     70, 126,  16,   0,   1,   0, 
      0,   0,   0,  96,  16,   0, 
      1,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   7, 114,   0,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  55,   0,   0,  10, 
    114,   0,  16,   0,   0,   0, 
      0,   0,   6, 128,  32,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     21,   0,   0,   1,  31,   0, 
      4,   4,  58, 128,  32,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,  74,   0,   0, 141, 
    194,   0,   0, 128,  67,  85, 
     21,   0, 242,   0,  16,   0, 
      1,   0,   0,   0, 230,  26, 
     16,   0,   2,   0,   0,   0, 
     70, 126,  16,   0,   3,   0, 
      0,   0,   0,  96,  16,   0, 
      3,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   8, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16, 128,  65,   0, 
      0,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  50,   0,   0,   9, 
    114,   0,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0, 246,  15, 
     16,   0,   1,   0,   0,   0, 
     70,   2,  16,   0,   1,   0, 
      0,   0,  21,   0,   0,   1, 
     55,   0,   0,  10, 130,   0, 
     16,   0,   1,   0,   0,   0, 
     58, 128,  32,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,   0,   0, 
      0,   9, 130,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     26, 128,  32, 128,  65,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,  49,   0, 
      0,   7, 130,   0,  16,   0, 
      0,   0,   0,   0,  58,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  13,   0,   4,   3, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  14,   0,   0,   8, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  10,  16,  16,   0, 
      6,   0,   0,   0,  10, 128, 
     32,   0,   0,   0,   0,   0, 
      7,   0,   0,   0,  55,   0, 
      0,  10, 114,   0,  16,   0, 
      0,   0,   0,   0, 246, 143, 
     32,   0,   0,   0,   0,   0, 
      6,   0,   0,   0, 246,  15, 
     16,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  12, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  10,  16,  16,   0, 
      6,   0,   0,   0,  10, 128, 
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
     18,   0,  16,   0,   2,   0, 
      0,   0,  26, 128,  32,   0, 
      1,   0,   0,   0,   1,   0, 
      0,   0,  10,  16,  16,   0, 
      6,   0,   0,   0,  55,   0, 
      0,   9, 130,   0,  16,   0, 
      0,   0,   0,   0,  10,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  29,   0, 
      0,   7,  18,   0,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,   0,   0,   0,   9, 
    226,   0,  16,   0,   2,   0, 
      0,   0,   6,   9,  16, 128, 
     65,   0,   0,   0,   0,   0, 
      0,   0,   6, 137,  32,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,  56,   0,   0,   8, 
    226,   0,  16,   0,   2,   0, 
      0,   0,  86,  14,  16,   0, 
      2,   0,   0,   0, 246, 143, 
     32,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,  50,   0, 
      0,   9, 226,   0,  16,   0, 
      2,   0,   0,   0,  86,  14, 
     16,   0,   2,   0,   0,   0, 
    246,  15,  16,   0,   0,   0, 
      0,   0,   6,   9,  16,   0, 
      0,   0,   0,   0,  55,   0, 
      0,   9, 114,   0,  16,   0, 
      2,   0,   0,   0,   6,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0, 150,   7,  16,   0, 
      2,   0,   0,   0,  55,   0, 
      0,  10, 114,   0,  16,   0, 
      1,   0,   0,   0,   6, 128, 
     32,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  49,   0,   0,   8, 
     18,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      1,   0,   0,   0,  26, 128, 
     32,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 114,   0,  16,   0, 
      2,   0,   0,   0,  70,  18, 
     16,   0,   4,   0,   0,   0, 
     54,   0,   0,   5, 130,   0, 
     16,   0,   2,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  55,   0,   0,  12, 
    242,   0,  16,   0,   0,   0, 
      0,   0,   6,   0,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   2,   0, 
      0,   0,  54,  32,   0,   5, 
    114,   0,  16,   0,   2,   0, 
      0,   0,  70,   2,  16,   0, 
      1,   0,   0,   0,  29,   0, 
      0,   8, 130,   0,  16,   0, 
      2,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     42, 128,  32,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   7,  18,   0, 
     16,   0,   3,   0,   0,   0, 
     26,   0,  16,   0,   2,   0, 
      0,   0,  10,   0,  16,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   7,  18,   0,  16,   0, 
      3,   0,   0,   0,  42,   0, 
     16,   0,   2,   0,   0,   0, 
     10,   0,  16,   0,   3,   0, 
      0,   0,  50,   0,   0,  13, 
    114,   0,  16,   0,   3,   0, 
      0,   0,   6,   0,  16,   0, 
      3,   0,   0,   0,   2,  64, 
      0,   0, 171, 170, 170,  62, 
    171, 170, 170,  62, 171, 170, 
    170,  62,   0,   0,   0,   0, 
     70,   2,  16, 128,  65,   0, 
      0,   0,   2,   0,   0,   0, 
     50,   0,   0,  10, 114,   0, 
     16,   0,   3,   0,   0,   0, 
    166, 138,  32,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  55,   0, 
      0,   9, 114,   0,  16,   0, 
      2,   0,   0,   0, 246,  15, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,  54,   0, 
      0,   5, 130,   0,  16,   0, 
      2,   0,   0,   0,  58,   0, 
     16,   0,   1,   0,   0,   0, 
     55,   0,   0,  10, 242,   0, 
     16,   0,   1,   0,   0,   0, 
    246, 143,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  70,  14,  16,   0, 
      2,   0,   0,   0,  55,   0, 
      0,  10, 242,  32,  16,   0, 
      0,   0,   0,   0, 246, 143, 
     32,   0,   0,   0,   0,   0, 
      4,   0,   0,   0,  70,  14, 
     16,   0,   0,   0,   0,   0, 
     70,  14,  16,   0,   1,   0, 
      0,   0,  62,   0,   0,   1, 
     83,  84,  65,  84, 148,   0, 
      0,   0,  67,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     32,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
      6,   0,   0,   0,   1,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  10,   0, 
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
      0,   0,   0,   0,   0,   0
};
