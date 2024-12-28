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
//   float Gamma;                       // Offset:   64 Size:     4
//   float2 ViewSize;                   // Offset:   68 Size:     8 [unused]
//   int bDoSelection;                  // Offset:   76 Size:     4 [unused]
//   int bOneXLightmaps;                // Offset:   80 Size:     4 [unused]
//   int bCorrectFog;                   // Offset:   84 Size:     4 [unused]
//   uint FrameShaderFlags;             // Offset:   88 Size:     4
//   int GammaMode;                     // Offset:   92 Size:     4
//   float HDRExpansion;                // Offset:   96 Size:     4
//   float ResolutionScale;             // Offset:  100 Size:     4
//   float WhiteLevel;                  // Offset:  104 Size:     4
//   int RendMap;                       // Offset:  108 Size:     4 [unused]
//   float DepthDrawLimit;              // Offset:  112 Size:     4 [unused]
//   float GammaOffsetRed;              // Offset:  116 Size:     4
//   float GammaOffsetBlue;             // Offset:  120 Size:     4
//   float GammaOffsetGreen;            // Offset:  124 Size:     4
//
// }
//
//
// Resource Bindings:
//
// Name                                 Type  Format         Dim      HLSL Bind  Count
// ------------------------------ ---------- ------- ----------- -------------- ------
// ScreenState                       sampler      NA          NA             s0      1 
// Screen                            texture  float4          2d             t0      1 
// FrameVariables                    cbuffer      NA          NA            cb0      1 
//
//
//
// Input signature:
//
// Name                 Index   Mask Register SysValue  Format   Used
// -------------------- ----- ------ -------- -------- ------- ------
// SV_POSITION              0   xyzw        0      POS   float       
// TEXCOORD                 0   xy          1     NONE   float   xy  
// COLOR                    2     z         1     NONE   float       
// COLOR                    0   xyzw        2     NONE   float       
// COLOR                    1   xyzw        3     NONE   float       
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
dcl_sampler s0, mode_default
dcl_resource_texture2d (float,float,float,float) t0
dcl_input_ps linear v1.xy
dcl_output o0.xyzw
dcl_temps 6
eq r0.x, l(1.000000), cb0[6].y
if_nz r0.x
  sample_b r0.xyz, v1.xyxx, t0.xyzw, s0, l(0.000000)
else 
  resinfo_uint r0.w, l(0), t0.yzwx
  utof r0.w, r0.w
  mad r1.xy, v1.xyxx, r0.wwww, l(0.500000, 0.500000, 0.000000, 0.000000)
  round_ni r1.zw, r1.xxxy
  frc r1.xy, r1.xyxx
  mul r2.xy, r1.xyxx, r1.xyxx
  mad r1.xy, -r1.xyxx, l(2.000000, 2.000000, 0.000000, 0.000000), l(3.000000, 3.000000, 0.000000, 0.000000)
  mad r1.xy, r2.xyxx, r1.xyxx, r1.zwzz
  add r1.xy, r1.xyxx, l(-0.500000, -0.500000, 0.000000, 0.000000)
  div r1.xy, r1.xyxx, r0.wwww
  sample_b r0.xyz, r1.xyxx, t0.xyzw, s0, l(0.000000)
endif 
and r1.xy, l(1, 2, 0, 0), cb0[5].zzzz
ieq r0.w, r1.x, l(0)
ine r1.z, r1.y, l(0)
and r0.w, r0.w, r1.z
mul r1.z, l(0.500000), cb0[4].x
movc r0.w, r0.w, r1.z, cb0[4].x
ne r1.z, r0.w, l(1.000000)
div r0.w, l(1.000000, 1.000000, 1.000000, 1.000000), r0.w
log r2.xyz, |r0.xyzx|
mul r3.xyz, r0.wwww, r2.xyzx
exp r3.xyz, r3.xyzx
movc r3.xyz, r1.zzzz, r3.xyzx, r0.xyzx
ieq r0.w, l(1), cb0[5].w
mul r2.xyz, r2.xyzx, cb0[7].yzwy
exp r2.xyz, r2.xyzx
movc r0.xyz, r0.wwww, r2.xyzx, r0.xyzx
movc r0.xyz, cb0[5].wwww, r0.xyzx, r3.xyzx
log r2.xyz, |r0.xyzx|
mul r2.xyz, r2.xyzx, l(2.200000, 2.200000, 2.200000, 0.000000)
exp r2.xyz, r2.xyzx
mul r0.w, cb0[6].x, cb0[6].z
mul r2.xyz, r0.wwww, r2.xyzx
sqrt r3.xyz, r0.xyzx
sqrt r4.xyz, r3.xyzx
sqrt r5.xyz, r4.xyzx
mul r4.xyz, r4.xyzx, l(0.684122, 0.684122, 0.684122, 0.000000)
mad r3.xyz, r3.xyzx, l(0.662003, 0.662003, 0.662003, 0.000000), r4.xyzx
mad r3.xyz, -r5.xyzx, l(0.323584, 0.323584, 0.323584, 0.000000), r3.xyzx
mad r3.xyz, -r0.xyzx, l(0.022541, 0.022541, 0.022541, 0.000000), r3.xyzx
movc r1.xzw, r1.xxxx, r2.xxyz, r3.xxyz
movc o0.xyz, r1.yyyy, r1.xzwx, r0.xyzx
mov o0.w, l(1.000000)
ret 
// Approximately 49 instruction slots used
#endif

const BYTE g_PxShader[] =
{
     68,  88,  66,  67,  83,  76, 
    205, 249,  33,  81,  62, 230, 
    215, 178, 234, 240, 136, 137, 
     66, 194,   1,   0,   0,   0, 
     84,  11,   0,   0,   5,   0, 
      0,   0,  52,   0,   0,   0, 
    192,   3,   0,   0, 100,   4, 
      0,   0, 152,   4,   0,   0, 
    216,  10,   0,   0,  82,  68, 
     69,  70, 132,   3,   0,   0, 
      1,   0,   0,   0, 160,   0, 
      0,   0,   3,   0,   0,   0, 
     28,   0,   0,   0,   1,   4, 
    255, 255,   0,   1,   0,   0, 
     89,   3,   0,   0, 124,   0, 
      0,   0,   3,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   1,   0,   0,   0, 
    136,   0,   0,   0,   2,   0, 
      0,   0,   5,   0,   0,   0, 
      4,   0,   0,   0, 255, 255, 
    255, 255,   0,   0,   0,   0, 
      1,   0,   0,   0,  13,   0, 
      0,   0, 143,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   0, 
      1,   0,   0,   0,  83,  99, 
    114, 101, 101, 110,  83, 116, 
     97, 116, 101,   0,  83,  99, 
    114, 101, 101, 110,   0,  70, 
    114,  97, 109, 101,  86,  97, 
    114, 105,  97,  98, 108, 101, 
    115,   0, 171, 171, 143,   0, 
      0,   0,  16,   0,   0,   0, 
    184,   0,   0,   0, 128,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  56,   2, 
      0,   0,   0,   0,   0,   0, 
     64,   0,   0,   0,   0,   0, 
      0,   0,  64,   2,   0,   0, 
      0,   0,   0,   0,  80,   2, 
      0,   0,  64,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  88,   2,   0,   0, 
      0,   0,   0,   0, 104,   2, 
      0,   0,  68,   0,   0,   0, 
      8,   0,   0,   0,   0,   0, 
      0,   0, 116,   2,   0,   0, 
      0,   0,   0,   0, 132,   2, 
      0,   0,  76,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 148,   2,   0,   0, 
      0,   0,   0,   0, 164,   2, 
      0,   0,  80,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 148,   2,   0,   0, 
      0,   0,   0,   0, 179,   2, 
      0,   0,  84,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 148,   2,   0,   0, 
      0,   0,   0,   0, 191,   2, 
      0,   0,  88,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0, 208,   2,   0,   0, 
      0,   0,   0,   0, 224,   2, 
      0,   0,  92,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0, 148,   2,   0,   0, 
      0,   0,   0,   0, 234,   2, 
      0,   0,  96,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  88,   2,   0,   0, 
      0,   0,   0,   0, 247,   2, 
      0,   0, 100,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  88,   2,   0,   0, 
      0,   0,   0,   0,   7,   3, 
      0,   0, 104,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  88,   2,   0,   0, 
      0,   0,   0,   0,  18,   3, 
      0,   0, 108,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0, 148,   2,   0,   0, 
      0,   0,   0,   0,  26,   3, 
      0,   0, 112,   0,   0,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,  88,   2,   0,   0, 
      0,   0,   0,   0,  41,   3, 
      0,   0, 116,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  88,   2,   0,   0, 
      0,   0,   0,   0,  56,   3, 
      0,   0, 120,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  88,   2,   0,   0, 
      0,   0,   0,   0,  72,   3, 
      0,   0, 124,   0,   0,   0, 
      4,   0,   0,   0,   2,   0, 
      0,   0,  88,   2,   0,   0, 
      0,   0,   0,   0,  80, 114, 
    111, 106,   0, 171, 171, 171, 
      3,   0,   3,   0,   4,   0, 
      4,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  71,  97, 
    109, 109,  97,   0, 171, 171, 
      0,   0,   3,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  86, 105, 
    101, 119,  83, 105, 122, 101, 
      0, 171, 171, 171,   1,   0, 
      3,   0,   1,   0,   2,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  98,  68, 111,  83, 
    101, 108, 101,  99, 116, 105, 
    111, 110,   0, 171, 171, 171, 
      0,   0,   2,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  98,  79, 
    110, 101,  88,  76, 105, 103, 
    104, 116, 109,  97, 112, 115, 
      0,  98,  67, 111, 114, 114, 
    101,  99, 116,  70, 111, 103, 
      0,  70, 114,  97, 109, 101, 
     83, 104,  97, 100, 101, 114, 
     70, 108,  97, 103, 115,   0, 
      0,   0,  19,   0,   1,   0, 
      1,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,  71,  97, 
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
    114, 101, 101, 110,   0,  77, 
    105,  99, 114, 111, 115, 111, 
    102, 116,  32,  40,  82,  41, 
     32,  72,  76,  83,  76,  32, 
     83, 104,  97, 100, 101, 114, 
     32,  67, 111, 109, 112, 105, 
    108, 101, 114,  32,  49,  48, 
     46,  49,   0, 171, 171, 171, 
     73,  83,  71,  78, 156,   0, 
      0,   0,   5,   0,   0,   0, 
      8,   0,   0,   0, 128,   0, 
      0,   0,   0,   0,   0,   0, 
      1,   0,   0,   0,   3,   0, 
      0,   0,   0,   0,   0,   0, 
     15,   0,   0,   0, 140,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      3,   3,   0,   0, 149,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   1,   0,   0,   0, 
      4,   0,   0,   0, 149,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   2,   0,   0,   0, 
     15,   0,   0,   0, 149,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   3,   0, 
      0,   0,   3,   0,   0,   0, 
     15,   0,   0,   0,  83,  86, 
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
     83,  72,  68,  82,  56,   6, 
      0,   0,  65,   0,   0,   0, 
    142,   1,   0,   0, 106,   8, 
      0,   1,  89,   0,   0,   4, 
     70, 142,  32,   0,   0,   0, 
      0,   0,   8,   0,   0,   0, 
     90,   0,   0,   3,   0,  96, 
     16,   0,   0,   0,   0,   0, 
     88,  24,   0,   4,   0, 112, 
     16,   0,   0,   0,   0,   0, 
     85,  85,   0,   0,  98,  16, 
      0,   3,  50,  16,  16,   0, 
      1,   0,   0,   0, 101,   0, 
      0,   3, 242,  32,  16,   0, 
      0,   0,   0,   0, 104,   0, 
      0,   2,   6,   0,   0,   0, 
     24,   0,   0,   8,  18,   0, 
     16,   0,   0,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
    128,  63,  26, 128,  32,   0, 
      0,   0,   0,   0,   6,   0, 
      0,   0,  31,   0,   4,   3, 
     10,   0,  16,   0,   0,   0, 
      0,   0,  74,   0,   0,  11, 
    114,   0,  16,   0,   0,   0, 
      0,   0,  70,  16,  16,   0, 
      1,   0,   0,   0,  70, 126, 
     16,   0,   0,   0,   0,   0, 
      0,  96,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  18,   0, 
      0,   1,  61,  16,   0,   7, 
    130,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0, 150, 115, 
     16,   0,   0,   0,   0,   0, 
     86,   0,   0,   5, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  50,   0,   0,  12, 
     50,   0,  16,   0,   1,   0, 
      0,   0,  70,  16,  16,   0, 
      1,   0,   0,   0, 246,  15, 
     16,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
      0,  63,   0,   0,   0,  63, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  65,   0,   0,   5, 
    194,   0,  16,   0,   1,   0, 
      0,   0,   6,   4,  16,   0, 
      1,   0,   0,   0,  26,   0, 
      0,   5,  50,   0,  16,   0, 
      1,   0,   0,   0,  70,   0, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   7,  50,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0,  70,   0,  16,   0, 
      1,   0,   0,   0,  50,   0, 
      0,  16,  50,   0,  16,   0, 
      1,   0,   0,   0,  70,   0, 
     16, 128,  65,   0,   0,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0,  64, 
      0,   0,   0,  64,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,   0,   0, 
     64,  64,   0,   0,  64,  64, 
      0,   0,   0,   0,   0,   0, 
      0,   0,  50,   0,   0,   9, 
     50,   0,  16,   0,   1,   0, 
      0,   0,  70,   0,  16,   0, 
      2,   0,   0,   0,  70,   0, 
     16,   0,   1,   0,   0,   0, 
    230,  10,  16,   0,   1,   0, 
      0,   0,   0,   0,   0,  10, 
     50,   0,  16,   0,   1,   0, 
      0,   0,  70,   0,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   0,   0,   0, 191, 
      0,   0,   0, 191,   0,   0, 
      0,   0,   0,   0,   0,   0, 
     14,   0,   0,   7,  50,   0, 
     16,   0,   1,   0,   0,   0, 
     70,   0,  16,   0,   1,   0, 
      0,   0, 246,  15,  16,   0, 
      0,   0,   0,   0,  74,   0, 
      0,  11, 114,   0,  16,   0, 
      0,   0,   0,   0,  70,   0, 
     16,   0,   1,   0,   0,   0, 
     70, 126,  16,   0,   0,   0, 
      0,   0,   0,  96,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0,   0,   0, 
     21,   0,   0,   1,   1,   0, 
      0,  11,  50,   0,  16,   0, 
      1,   0,   0,   0,   2,  64, 
      0,   0,   1,   0,   0,   0, 
      2,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
    166, 138,  32,   0,   0,   0, 
      0,   0,   5,   0,   0,   0, 
     32,   0,   0,   7, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     10,   0,  16,   0,   1,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0,   0,   0,  39,   0, 
      0,   7,  66,   0,  16,   0, 
      1,   0,   0,   0,  26,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,   0,   1,   0,   0,   7, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
     56,   0,   0,   8,  66,   0, 
     16,   0,   1,   0,   0,   0, 
      1,  64,   0,   0,   0,   0, 
      0,  63,  10, 128,  32,   0, 
      0,   0,   0,   0,   4,   0, 
      0,   0,  55,   0,   0,  10, 
    130,   0,  16,   0,   0,   0, 
      0,   0,  58,   0,  16,   0, 
      0,   0,   0,   0,  42,   0, 
     16,   0,   1,   0,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0,   4,   0,   0,   0, 
     57,   0,   0,   7,  66,   0, 
     16,   0,   1,   0,   0,   0, 
     58,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      0,   0, 128,  63,  14,   0, 
      0,  10, 130,   0,  16,   0, 
      0,   0,   0,   0,   2,  64, 
      0,   0,   0,   0, 128,  63, 
      0,   0, 128,  63,   0,   0, 
    128,  63,   0,   0, 128,  63, 
     58,   0,  16,   0,   0,   0, 
      0,   0,  47,   0,   0,   6, 
    114,   0,  16,   0,   2,   0, 
      0,   0,  70,   2,  16, 128, 
    129,   0,   0,   0,   0,   0, 
      0,   0,  56,   0,   0,   7, 
    114,   0,  16,   0,   3,   0, 
      0,   0, 246,  15,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     25,   0,   0,   5, 114,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  55,   0,   0,   9, 
    114,   0,  16,   0,   3,   0, 
      0,   0, 166,  10,  16,   0, 
      1,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  32,   0,   0,   8, 
    130,   0,  16,   0,   0,   0, 
      0,   0,   1,  64,   0,   0, 
      1,   0,   0,   0,  58, 128, 
     32,   0,   0,   0,   0,   0, 
      5,   0,   0,   0,  56,   0, 
      0,   8, 114,   0,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
    150, 135,  32,   0,   0,   0, 
      0,   0,   7,   0,   0,   0, 
     25,   0,   0,   5, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,  55,   0,   0,   9, 
    114,   0,  16,   0,   0,   0, 
      0,   0, 246,  15,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   0,   0, 
      0,   0,  55,   0,   0,  10, 
    114,   0,  16,   0,   0,   0, 
      0,   0, 246, 143,  32,   0, 
      0,   0,   0,   0,   5,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
     47,   0,   0,   6, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16, 128, 129,   0, 
      0,   0,   0,   0,   0,   0, 
     56,   0,   0,  10, 114,   0, 
     16,   0,   2,   0,   0,   0, 
     70,   2,  16,   0,   2,   0, 
      0,   0,   2,  64,   0,   0, 
    205, 204,  12,  64, 205, 204, 
     12,  64, 205, 204,  12,  64, 
      0,   0,   0,   0,  25,   0, 
      0,   5, 114,   0,  16,   0, 
      2,   0,   0,   0,  70,   2, 
     16,   0,   2,   0,   0,   0, 
     56,   0,   0,   9, 130,   0, 
     16,   0,   0,   0,   0,   0, 
     10, 128,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
     42, 128,  32,   0,   0,   0, 
      0,   0,   6,   0,   0,   0, 
     56,   0,   0,   7, 114,   0, 
     16,   0,   2,   0,   0,   0, 
    246,  15,  16,   0,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      2,   0,   0,   0,  75,   0, 
      0,   5, 114,   0,  16,   0, 
      3,   0,   0,   0,  70,   2, 
     16,   0,   0,   0,   0,   0, 
     75,   0,   0,   5, 114,   0, 
     16,   0,   4,   0,   0,   0, 
     70,   2,  16,   0,   3,   0, 
      0,   0,  75,   0,   0,   5, 
    114,   0,  16,   0,   5,   0, 
      0,   0,  70,   2,  16,   0, 
      4,   0,   0,   0,  56,   0, 
      0,  10, 114,   0,  16,   0, 
      4,   0,   0,   0,  70,   2, 
     16,   0,   4,   0,   0,   0, 
      2,  64,   0,   0, 160,  34, 
     47,  63, 160,  34,  47,  63, 
    160,  34,  47,  63,   0,   0, 
      0,   0,  50,   0,   0,  12, 
    114,   0,  16,   0,   3,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,   2,  64, 
      0,   0,   2, 121,  41,  63, 
      2, 121,  41,  63,   2, 121, 
     41,  63,   0,   0,   0,   0, 
     70,   2,  16,   0,   4,   0, 
      0,   0,  50,   0,   0,  13, 
    114,   0,  16,   0,   3,   0, 
      0,   0,  70,   2,  16, 128, 
     65,   0,   0,   0,   5,   0, 
      0,   0,   2,  64,   0,   0, 
    192, 172, 165,  62, 192, 172, 
    165,  62, 192, 172, 165,  62, 
      0,   0,   0,   0,  70,   2, 
     16,   0,   3,   0,   0,   0, 
     50,   0,   0,  13, 114,   0, 
     16,   0,   3,   0,   0,   0, 
     70,   2,  16, 128,  65,   0, 
      0,   0,   0,   0,   0,   0, 
      2,  64,   0,   0,  54, 168, 
    184,  60,  54, 168, 184,  60, 
     54, 168, 184,  60,   0,   0, 
      0,   0,  70,   2,  16,   0, 
      3,   0,   0,   0,  55,   0, 
      0,   9, 210,   0,  16,   0, 
      1,   0,   0,   0,   6,   0, 
     16,   0,   1,   0,   0,   0, 
      6,   9,  16,   0,   2,   0, 
      0,   0,   6,   9,  16,   0, 
      3,   0,   0,   0,  55,   0, 
      0,   9, 114,  32,  16,   0, 
      0,   0,   0,   0,  86,   5, 
     16,   0,   1,   0,   0,   0, 
    134,   3,  16,   0,   1,   0, 
      0,   0,  70,   2,  16,   0, 
      0,   0,   0,   0,  54,   0, 
      0,   5, 130,  32,  16,   0, 
      0,   0,   0,   0,   1,  64, 
      0,   0,   0,   0, 128,  63, 
     62,   0,   0,   1,  83,  84, 
     65,  84, 116,   0,   0,   0, 
     49,   0,   0,   0,   6,   0, 
      0,   0,   0,   0,   0,   0, 
      2,   0,   0,   0,  29,   0, 
      0,   0,   3,   0,   0,   0, 
      2,   0,   0,   0,   2,   0, 
      0,   0,   1,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   2,   0,   0,   0, 
      0,   0,   0,   0,   1,   0, 
      0,   0,   6,   0,   0,   0, 
      3,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0,   0,   0,   0,   0, 
      0,   0
};
